# -*- coding: utf-8 -*-
# cython: profile=True
'''
Reads SR Research EDF files and parses them into ocupy datamats.
'''
cimport numpy as np
import numpy as np
import string

from libc.stdint cimport int16_t, uint16_t, uint32_t, int64_t
from libc.stdlib cimport malloc, free

from pyedfread.edf_data import *
from pyedfread.data cimport FSAMPLE, FEVENT, ALLF_DATA, FS
from pyedfread.data cimport fsample_fs

from sampledict import SampleAccumulator

import pandas as pd
import struct

try:
    import progressbar
except ImportError:
    progressbar = None

try:
    import pandas
except ImportError:
    pandas = None


type2label = {STARTFIX: 'fixation', STARTSACC: 'saccade', STARTBLINK: 'blink',
              ENDFIX: 'fixation', ENDSACC: 'saccade', ENDBLINK: 'blink',
              MESSAGEEVENT: 'message'}

sample_columns = ['time', 'px_left', 'px_right', 'py_left', 'py_right',
    'hx_left', 'hx_right', 'hy_left', 'hy_right', 'pa_left', 'pa_right',
    'gx_left', 'gx_right', 'gy_left', 'gy_right', 'rx', 'ry',
    'gxvel_left', 'gxvel_right', 'gyvel_left', 'gyvel_right', 'hxvel_left', 'hxvel_right',
    'hyvel_left', 'hyvel_right', 'rxvel_left', 'rxvel_right', 'ryvel_left', 'ryvel_right',
    'fgxvel', 'fgyvel', 'fhxyvel', 'fhyvel',  'frxyvel', 'fryvel',
    'flags', 'input', 'buttons', 'htype', 'errors']

cdef extern from "edf.h":
    ctypedef int EDFFILE
    int * edf_open_file(const char * fname, int consistency, int load_events,
                        int load_samples, int * errval)
    int edf_get_preamble_text_length (EDFFILE * edf);
    int edf_get_preamble_text(EDFFILE * ef,
                               char * buffer, int length)
    int edf_get_next_data(EDFFILE * ef)
    ALLF_DATA * edf_get_float_data(EDFFILE * ef)
    int edf_get_element_count(EDFFILE * ef)
    int edf_close_file(EDFFILE * ef)


def unbox_messages(current):
    for key in current.keys():
        try:
            if len(current[key])==1:
                current[key] = current[key][0]
        except TypeError:
            pass
    return current

def read_preamble(filename, consistency=0):
    cdef int errval = 1

    cdef int* ef
    ef = edf_open_file(filename, consistency, 1, 1, &errval)
    if errval < 0:
        print filename, ' could not be openend.'
        raise IOError('Could not open: %s'%filename)
    cdef int psize = edf_get_preamble_text_length(ef)
    cdef char* buf = <char*> malloc(psize * sizeof(char))
    e = edf_get_preamble_text(ef, buf, psize)
    edf_close_file(ef)
    return buf


def fread(filename,
          ignore_samples=False,
          filter=[],
          split_char=' ',
          properties_filter=['type', 'time', 'sttime',
                             'entime', 'gx', 'gy', 'gstx', 'gsty', 'genx',
                             'geny', 'gxvel', 'gyvel', 'start', 'end', 'gavx',
                             'gavy', 'eye']):
    '''
    Read an EDF file into a list of dicts.

    ingnore_samples : If true individual samples will not be saved, but only
        event averages.
    filter : List of strings.
        The SR system allows to send trial meta data messages into the data
        stream. This function decides which messages to keep by checking if the
        message string is in this filter list. Messages are split by
        'split_char' and the first part of the message is checked against the
        filter list. Example:
            Message is "beep_150" and split_char = '_' -> (beep, 150)
            Message is "beep 150" and split_char = ' ' -> (beep, 150)
    split_char : Character used to split metadata messages.
    properties_filter : Determines which event properties to read from the EDF.
        Corresponds to fieldnames of the underlying c structs. For a list see
        data2dict in this file and the EDF acces API.
    '''

    if 'eye' not in properties_filter:
        properties_filter.append('eye')
    return st(filename, ignore_samples, filter, split_char.encode('utf-8'), properties_filter)


cdef st(filename, ignore_samples, filter, split_char, properties_filter):
    cdef int errval = 1
    cdef char* buf = <char*> malloc(1024 * sizeof(char))
    cdef int* ef
    cdef int sample_type, cnt

    left_acc, right_acc = [], []
    current_messages = {}
    current_event = {}
    message_accumulator = []
    event_accumulator = []
    ef = edf_open_file(filename.encode('utf-8'), 0, 1, 1, &errval)
    if errval < 0:
        print filename, ' could not be openend.'
        raise IOError('Could not open: %s'%filename)
    e = edf_get_preamble_text(ef, buf, 1024)
    num_elements = edf_get_element_count(ef)
    cdef np.ndarray npsamples = np.ndarray((num_elements,  40), dtype=np.float64)
    cdef np.float64_t[:, :] samples = npsamples

    trial = 0
    cnt=0
    cdef int i=0
    cdef int pt

    while True:
        sample_type = edf_get_next_data(ef)
        if sample_type == NO_PENDING_ITEMS:
            edf_close_file(ef)
            break

        if (sample_type == SAMPLE_TYPE):
            fd = edf_get_float_data(ef)
            samples[cnt, 0] = float(fd.fs.time)  # time of sample
            samples[cnt, 1] = float(fd.fs.px[0])
            samples[cnt, 2] = float(fd.fs.px[1])
            samples[cnt, 3] = float(fd.fs.py[0])
            samples[cnt, 4] = float(fd.fs.py[1])
            samples[cnt, 5] = float(fd.fs.hx[0])
            samples[cnt, 6] = float(fd.fs.hx[1])
            samples[cnt, 7] = float(fd.fs.hy[0])
            samples[cnt, 8] = float(fd.fs.hy[1])
            samples[cnt, 9] = float(fd.fs.pa[0])
            samples[cnt, 10] = fd.fs.pa[1]
            samples[cnt, 11] = fd.fs.gx[0]
            samples[cnt, 12] = fd.fs.gx[1]
            samples[cnt, 13] = fd.fs.gy[0]
            samples[cnt, 14] = fd.fs.gy[1]
            samples[cnt, 15] = fd.fs.rx
            samples[cnt, 16] = fd.fs.ry

            samples[cnt, 17] = fd.fs.gxvel[0]
            samples[cnt, 18] = fd.fs.gxvel[1]
            samples[cnt, 19] = fd.fs.gyvel[0]
            samples[cnt, 20] = fd.fs.gyvel[1]
            samples[cnt, 21] = fd.fs.hxvel[0]
            samples[cnt, 22] = fd.fs.hxvel[1]
            samples[cnt, 23] = fd.fs.hyvel[0]
            samples[cnt, 24] = fd.fs.hyvel[1]
            samples[cnt, 25] = fd.fs.rxvel[0]
            samples[cnt, 26] = fd.fs.rxvel[1]
            samples[cnt, 27] = fd.fs.ryvel[0]
            samples[cnt, 28] = fd.fs.ryvel[1]

            samples[cnt, 29] = fd.fs.fgxvel[0]
            samples[cnt, 30] = fd.fs.fgyvel[0]
            samples[cnt, 31] = fd.fs.fhxvel[0]
            samples[cnt, 32] = fd.fs.fhyvel[0]
            samples[cnt, 33] = fd.fs.frxvel[0]
            samples[cnt, 34] = fd.fs.fryvel[0]

            #samples[cnt, 39:48] =  <float>fd.fs.hdata # head-tracker data (not prescaled)
            samples[cnt, 35] = fd.fs.flags #flags to indicate contents
            samples[cnt, 36] = fd.fs.input #extra (input word)
            samples[cnt, 37] = fd.fs.buttons # button state & changes
            samples[cnt, 38] = fd.fs.htype #head-tracker data type
            samples[cnt, 39] = fd.fs.errors
            cnt+=1

        elif sample_type == MESSAGEEVENT:
            data = data2dict(sample_type, ef, filter=properties_filter)
            trial, current_messages, message_accumulator = parse_message(
                data, trial, current_messages, message_accumulator, split_char, filter)

        else:
            data = data2dict(sample_type, ef, filter=properties_filter)
            current_event, event_accumulator = parse_datum(data, sample_type,
                trial, split_char, filter, ignore_samples, current_event, event_accumulator)

        i = i+1
    free(buf)
    return samples, event_accumulator, message_accumulator




def parse_datum(data, sample_type, trial, split_char, filter, ignore_samples,
    current_event, event_accumulator):
    '''
    Parse a datum into data structures.
    '''
    if len(data) == 0:
        return current_event, event_accumulator

    if (sample_type == STARTFIX) or (sample_type == STARTSACC):
        current_event = data
        current_event['blink'] = False
        current_event['trial'] = trial

    if (sample_type == ENDFIX) or (sample_type == ENDSACC):
        current_event.update(data)
        event_accumulator.append(current_event)

    if (sample_type == STARTBLINK) or (sample_type == ENDBLINK):
        current_event['blink'] = True


    return current_event, event_accumulator


def parse_message(data, trial, current_messages, message_accumulator, split_char, filter):
    if data['message'].startswith(b'TRIALID'):
        if (trial > 0) and (len(current_messages.keys()) > 0):
            current_messages['trial'] = trial
            message_accumulator.append(unbox_messages(current_messages))
        trial += 1
        current_messages = {}
        current_messages['trialid '] = data['message']
        current_messages['trialid_time'] = data['start']

    elif data['message'].startswith(b'SYNCTIME'):
        current_messages['SYNCTIME'] = data['start']
        current_messages['SYNCTIME_start'] = data['start']

    elif data['message'].startswith(b'DRIFTCORRECT'):
        current_messages['DRIFTCORRECT'] = data['message']

    elif data['message'].startswith(b'METATR'):
        parts = data['message'].split(b' ')
        msg, key = parts[0], parts[1]
        if len(parts) == 3:
            value = parts[2].strip().replace(b'\x00', b'')
        else:
            value = str(parts[2:])
        current_messages[key + b'_message_send_time'] = data['start']
        try:
            current_messages[key] = float(value)
        except (TypeError, ValueError):
            current_messages[key] = value
    else:
        # These are messageevents that accumulate during a fixation.
        # I treat them as key value pairs

        msg = data['message'].strip().replace(b'\x00', b'').split(split_char)
        if filter == 'all' or msg[0] in filter:
            try:
                value = [string.atof(v) for v in msg[1:]]
            except ValueError:
                value = msg[1:]

            if len(msg) == 1:
                key, value = msg[0], np.nan
            elif len(msg) == 2:
                key, value = msg[0], value[0]
            elif len(msg) > 2:
                key, value = msg[0], value

            if key not in current_messages.keys():
                current_messages[key] = []
                current_messages[key+b'_time'] = []
            current_messages[key].append(value)
            current_messages[key+'_time'].append(data['start'])
    return trial, current_messages, message_accumulator


cdef data2dict(sample_type, int* ef, filter=['type', 'time', 'sttime',
                                             'entime', 'gx', 'gy', 'gstx',
                                             'gsty', 'genx', 'geny', 'gxvel',
                                             'gyvel', 'start', 'end', 'gavx',
                                             'gavy', 'eye']):
    '''
    Converts EDF sample to a dictionary.
    '''
    fd = edf_get_float_data(ef)
    cdef char* msg
    d = None
    if ((sample_type == STARTFIX) or
       (sample_type == STARTSACC) or
       (sample_type == STARTBLINK) or
       (sample_type == ENDFIX) or
       (sample_type == ENDSACC) or
       (sample_type == ENDBLINK) or
       (sample_type == MESSAGEEVENT)):
        message = ''
        if <int>fd.fe.message != 0:
            msg = &fd.fe.message.c
            message = msg[:fd.fe.message.len]
        print(fd.fe.time, fd.fe.type, fd.fe.eye)
        print(fd.fe.sttime)
        d = {'time': fd.fe.time, 'type': type2label[sample_type],
             'start': fd.fe.sttime, 'end': fd.fe.entime,
             'hstx': fd.fe.hstx, 'hsty': fd.fe.hsty,
             'gstx': fd.fe.gstx, 'gsty': fd.fe.gsty,
             'sta': fd.fe.sta, 'henx': fd.fe.henx,
             'heny': fd.fe.heny, 'genx': fd.fe.genx,
             'geny': fd.fe.geny, 'ena': fd.fe.ena,
             'havx': fd.fe.havx, 'havy': fd.fe.havy,
             'gavx': fd.fe.gavx, 'gavy': fd.fe.gavy,
             'ava': fd.fe.ava, 'avel': fd.fe.avel,
             'pvel': fd.fe.pvel, 'svel': fd.fe.svel,
             'evel': fd.fe.evel, 'supd_x': fd.fe.supd_x, 'eupd_x': fd.fe.eupd_x,
             'eye': fd.fe.eye, 'buttons': fd.fe.buttons, 'message': message,
             }
    if sample_type == SAMPLE_TYPE:
        d = {'time': fd.fs.time,
         'px': (fd.fs.px[0], fd.fs.px[1]),
         'py': (fd.fs.py[0], fd.fs.py[1]),
         'hx': (fd.fs.hx[0], fd.fs.hx[1]),
         'hy': (fd.fs.hy[0], fd.fs.hy[1]),
         'gx': (fd.fs.gx[0], fd.fs.gx[1]),
         'gy': (fd.fs.gy[0], fd.fs.gy[1]),
         'pa': (fd.fs.pa[0], fd.fs.pa[1]),
         'rx': fd.fs.rx,
         'ry': fd.fs.ry,
         'gxvel': (fd.fs.gxvel[0], fd.fs.gxvel[1]),
         'gyvel': (fd.fs.gyvel[0], fd.fs.gyvel[1]),
         'hxvel': (fd.fs.hxvel[0], fd.fs.hxvel[1]),
         'hyvel': (fd.fs.hyvel[0], fd.fs.hyvel[1]),
         'rxvel': (fd.fs.rxvel[0], fd.fs.rxvel[1]),
         'ryvel': (fd.fs.ryvel[0], fd.fs.ryvel[1]),
         'fgxvel': (fd.fs.fgxvel[0], fd.fs.fgxvel[1]),
         'fgyvel': (fd.fs.fgyvel[0], fd.fs.fgyvel[1]),
         'fhxvel': (fd.fs.fhxvel[0], fd.fs.fhxvel[1]),
         'fhyvel': (fd.fs.fhyvel[0], fd.fs.fhyvel[1]),
         'frxvel': (fd.fs.frxvel[0], fd.fs.frxvel[1]),
         'fryvel': (fd.fs.fryvel[0], fd.fs.fryvel[1])
}
    if d is None:
        return {}
    rd = {}
    for key, val in d.iteritems():
        if key in filter + ['message']:
            rd[key] = val
    return rd


def to_eye(data):
    if 'eye' in data.keys():
        if  data['eye'] == 0:
            return data, {}
        else:
            return {}, data
    else:
        left, right = {}, {}
        for k, v in data.iteritems():
            try:
                left[k] = v[0]
                right[k] = v[1]
            except TypeError:
                if k == 'gx':
                    raise RuntimeError('///')
                left[k] = v
                right[k] = v
    return left, right
