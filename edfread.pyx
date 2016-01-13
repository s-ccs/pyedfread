# -*- coding: utf-8 -*-
'''
Reads SR Research EDF files and parses them into ocupy datamats.
'''

cimport numpy as np
import numpy as np
import string

from libc.stdint cimport int16_t, uint16_t, uint32_t, int64_t
from libc.stdlib cimport malloc, free

from edf_data import *
from data cimport FSAMPLE, FEVENT, ALLF_DATA

from sampledict import SampleAccumulator

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


cdef extern from "edf.h":
    ctypedef int EDFFILE
    int * edf_open_file(const char * fname, int consistency, int load_events,
                        int load_samples, int * errval)
    int  edf_get_preamble_text(EDFFILE * ef,
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

def fread(filename,
          ignore_samples=False,
          filter=[],
          split_char=' ',
          properties_filter=['type', 'time', 'sttime',
                             'entime', 'gx', 'gy', 'gstx', 'gsty', 'genx',
                             'geny', 'gxvel', 'gyvel', 'start', 'end', 'gavx',
                             'gavy']):
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
    cdef int errval = 1
    cdef char* buf = <char*> malloc(1024 * sizeof(char))
    cdef int* ef
    cdef int sample_type
    current_event = {'samples': []}
    current_messages = {}
    current_buttons = {}
    event_accumulator = []
    message_accumulator = []
    sample_accumulator = SampleAccumulator()
    ef = edf_open_file(filename, 0, 1, 1, &errval)
    if errval < 0:
        print filename, ' could not be openend.'
        import sys
        sys.exit(-1)
    e = edf_get_preamble_text(ef, buf, 1024)
    num_elements = edf_get_element_count(ef)
    if progressbar is not None:
        bar = progressbar.ProgressBar(num_elements).start()
        cnt = 0
    trial = 0
    while True:
        sample_type = edf_get_next_data(ef)
        data = data2dict(sample_type, ef, filter=properties_filter)
        if (sample_type == STARTFIX) or (sample_type == STARTSACC):
            current_event = data
            current_event['trial'] = trial
            current_event['blink'] = False
            if not ignore_samples:
                sample_accumulator = SampleAccumulator()

        if (sample_type == ENDFIX) or (sample_type == ENDSACC):
            if not ignore_samples:
                current_event['samples'] = sample_accumulator

            current_event.update(data)
            if current_event['message'] == '':
                del current_event['message']
            event_accumulator.append(current_event)

        if (sample_type == STARTBLINK) or (sample_type == ENDBLINK):
            current_event['blink'] = True

        if (sample_type == SAMPLE_TYPE) and not ignore_samples:
            sample_accumulator.update(data)

        if sample_type == MESSAGEEVENT:
            if data['message'].startswith('TRIALID'):
                if (trial > 0) and (len(current_messages.keys()) > 0):
                    current_messages['trial'] = trial
                    message_accumulator.append(unbox_messages(current_messages))
                trial += 1
                current_messages = {}

            elif data['message'].startswith('SYNCTIME'):
                current_messages['SYNCTIME'] = data['start']
                current_messages['SYNCTIME_start'] = data['start']

            elif data['message'].startswith('DRIFTCORRECT'):
                current_messages['DRIFTCORRECT'] = data['message']

            elif data['message'].startswith('METATR'):
                parts = data['message'].split(' ')
                msg, key = parts[0], parts[1]
                if len(parts) == 3:
                    value = parts[2].strip().replace('\x00', '')
                else:
                    value = str(parts[2:])
                current_messages[key + '_message_send_time'] = data['start']
                try:
                    current_messages[key] = string.atof(value)
                except (TypeError, ValueError):
                    current_messages[key] = value
            else:
                # These are messageevents that accumulate during a fixation.
                # I treat them as key value pairs
                msg = data['message'].strip().replace('\x00', '').split(split_char)
                if not filter == 'all':
                    if msg[0] not in filter:
                        continue
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
                    current_messages[key+'_time'] = []
                current_messages[key].append(value)
                print key, value
                current_messages[key+'_time'].append(data['start'])

        if sample_type == NO_PENDING_ITEMS:
            if len(current_messages.keys()) > 0:
                current_messages['trial'] = trial
                message_accumulator.append(unbox_messages(current_messages))
            edf_close_file(ef)
            break
        if progressbar is not None:
            bar.update(cnt)
            cnt += 1
    free(buf)
    if not ignore_samples:
        for i in range(len(event_accumulator)):
            event_accumulator[i]['samples'] = event_accumulator[i]['samples'].get_dict()
    return event_accumulator, message_accumulator


cdef data2dict(sample_type, int* ef, filter=['type', 'time', 'sttime',
                                             'entime', 'gx', 'gy', 'gstx',
                                             'gsty', 'genx', 'geny', 'gxvel',
                                             'gyvel', 'start', 'end', 'gavx',
                                             'gavy']):
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
        return d
    rd = {}
    for key, val in d.iteritems():

        if key in filter + ['message']:
            rd[key] = val
    return rd
