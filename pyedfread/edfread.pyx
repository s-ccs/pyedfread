# -*- coding: utf-8 -*-
# cython: profile=True
'''
Reads SR Research EDF files and parses them into a list of dicts.
'''

cimport numpy as np
import numpy as np
import string

from libc.stdint cimport int16_t, uint16_t, uint32_t, int64_t
from libc.stdlib cimport malloc, free

from pyedfread.edf_data import *
from pyedfread.data cimport ALLF_DATA

import struct


type2label = {STARTFIX: 'fixation', STARTSACC: 'saccade', STARTBLINK: 'blink',
              ENDFIX: 'fixation', ENDSACC: 'saccade', ENDBLINK: 'blink',
              MESSAGEEVENT: 'message'}


sample_columns = ['time', 'px_left', 'px_right', 'py_left', 'py_right',
                  'hx_left', 'hx_right', 'hy_left', 'hy_right', 'pa_left',
                  'pa_right', 'gx_left', 'gx_right', 'gy_left', 'gy_right',
                  'rx', 'ry', 'gxvel_left', 'gxvel_right', 'gyvel_left',
                  'gyvel_right', 'hxvel_left', 'hxvel_right', 'hyvel_left',
                  'hyvel_right', 'rxvel_left', 'rxvel_right', 'ryvel_left',
                  'ryvel_right', 'fgxvel', 'fgyvel', 'fhxyvel', 'fhyvel',
                  'frxyvel', 'fryvel', 'flags', 'input', 'buttons', 'htype',
                  'errors']


cdef extern from "edf.h":
    ctypedef int EDFFILE
    int * edf_open_file(const char * fname, int consistency, int load_events,
                        int load_samples, int * errval)
    int edf_get_preamble_text_length(EDFFILE * edf)
    int edf_get_preamble_text(EDFFILE * ef,
                              char * buffer, int length)
    int edf_get_next_data(EDFFILE * ef)
    ALLF_DATA * edf_get_float_data(EDFFILE * ef)
    int edf_get_element_count(EDFFILE * ef)
    int edf_close_file(EDFFILE * ef)


def read_preamble(filename, consistency=0):
    cdef int errval = 1
    cdef int * ef
    ef = edf_open_file(filename.encode('utf-8'), consistency, 1, 1, & errval)
    if errval < 0:
        raise IOError('Could not open: %s' % filename)
    cdef int psize = edf_get_preamble_text_length(ef)
    cdef char * buf = <char * > malloc(psize * sizeof(char))
    e = edf_get_preamble_text(ef, buf, psize)
    edf_close_file(ef)
    return buf


def read_calibration(filename, consistency=0):
    '''
    Read calibration/validation messages from EDF file.
    '''
    return read_messages(filename,
                         startswith=['!VAL', '!CAL'],
                         consistency=consistency)


def read_messages(filename, startswith=None, consistency=0):
    '''
    Read messages from edf file

    '''
    if startswith is not None:
        startswith = [s.encode('utf-8') for s in startswith]
    cdef int errval = 1
    cdef int * ef
    cdef char * msg
    ef = edf_open_file(filename.encode('utf-8'), consistency, 1, 1, & errval)
    if errval < 0:
        raise IOError('Could not open: %s' % filename)

    messages = []
    samples = []
    while True:
        sample_type = edf_get_next_data(ef)
        if sample_type == NO_PENDING_ITEMS:
            edf_close_file(ef)
            break
        samples.append(sample_type)
        if sample_type == MESSAGEEVENT or sample_type == RECORDING_INFO:
            fd = edf_get_float_data(ef)
            message = ''
            if < int > fd.fe.message != 0:
                msg = &fd.fe.message.c
                message = msg[:fd.fe.message.len]
                if (startswith is None or
                        any([message.startswith(s) for s in startswith])):
                    messages.append(message)

    return messages


def fread(filename,
          ignore_samples=False,
          filter=[],
          split_char=' ',
          trial_marker=b'TRIALID'):
    '''
    Read an EDF file into a list of dicts.

    For documentation see edf.pread()
    '''
    return parse_edf(filename, ignore_samples, filter, split_char, trial_marker)


cdef parse_edf(filename, ignore_samples, filter, split_char, trial_marker):
    cdef int errval = 1
    cdef char * buf = < char * > malloc(1024 * sizeof(char))
    cdef int * ef
    cdef int sample_type, cnt, trial

    left_acc, right_acc = [], []
    current_messages = {}
    current_event = {}
    message_accumulator = []
    event_accumulator = []
    ef = edf_open_file(filename.encode('utf-8'), 0, 1, 1, & errval)
    if errval < 0:
        raise IOError('Could not open: %s' % filename)
    e = edf_get_preamble_text(ef, buf, 1024)
    num_elements = edf_get_element_count(ef)
    if ignore_samples:
        num_elements = 0

    cdef np.ndarray npsamples = np.ndarray((num_elements,  40), dtype=np.float64)
    cdef np.float64_t[:, :] samples = npsamples
    trial, cnt = -1, 0

    while True:
        sample_type = edf_get_next_data(ef)
        if sample_type == NO_PENDING_ITEMS:
            edf_close_file(ef)
            break

        if not ignore_samples and (sample_type == SAMPLE_TYPE):
            fd = edf_get_float_data(ef)
            # Map fields explicitly into memory view
            samples[cnt, 0] = float(fd.fs.time)
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

            # samples[cnt, 39:48] =  <float>fd.fs.hdata # head-tracker data
            # (not prescaled)
            samples[cnt, 35] = fd.fs.flags  # flags to indicate contents
            samples[cnt, 36] = fd.fs.input  # extra (input word)
            samples[cnt, 37] = fd.fs.buttons  # button state & changes
            samples[cnt, 38] = fd.fs.htype  # head-tracker data type
            samples[cnt, 39] = fd.fs.errors
            cnt += 1

        elif sample_type == MESSAGEEVENT:
            data = data2dict(sample_type, ef)
            trial, current_messages, message_accumulator = parse_message(
                data, trial, current_messages, message_accumulator,
                split_char, filter, trial_marker)

        else:
            data = data2dict(sample_type, ef)
            current_event, event_accumulator = parse_datum(data,
                                                           sample_type,
                                                           trial,
                                                           split_char,
                                                           filter,
                                                           ignore_samples,
                                                           current_event,
                                                           event_accumulator)
    free(buf)
    return samples, event_accumulator, message_accumulator


def parse_datum(data, sample_type, trial, split_char, filter, ignore_samples,
                current_event, event_accumulator):
    '''
    Parse a datum into data structures.
    '''
    if data is None:
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


def parse_message(data, trial, current_messages, message_accumulator,
                  split_char, filter, trial_marker):
    if data['message'].startswith(trial_marker):
        if (trial <= 0) and (len(current_messages.keys()) > 0):
            current_messages['py_trial_marker'] = trial
            message_accumulator.append(current_messages)
            trial = 1
        else:
            trial += 1
        current_messages = {'py_trial_marker': trial}
        message_accumulator.append(current_messages)
        current_messages['trialid '] = data['message'].decode(
            'utf-8').strip().replace('\x00', '')
        current_messages['trialid_time'] = data['start']

    elif data['message'].startswith(b'SYNCTIME'):
        current_messages['SYNCTIME'] = data['start']
        current_messages['SYNCTIME_start'] = data['start']

    elif data['message'].startswith(b'DRIFTCORRECT'):
        current_messages['DRIFTCORRECT'] = data[
            'message'].decode('utf-8').strip().replace('\x00', '')

    elif data['message'].startswith(b'METATR'):
        parts = data['message'].decode(
            'utf-8').strip().replace('\x00', '').split(' ')
        msg, key = parts[0], parts[1]
        if len(parts) == 3:
            value = parts[2]
        else:
            value = str(parts[2:])
        current_messages[key + '_time'] = data['start']
        try:
            current_messages[key] = float(value)
        except (TypeError, ValueError):
            current_messages[key] = value
    else:
        # These are messageevents that accumulate during a fixation.
        # I treat them as key value pairs
        msg = data['message'].decode(
            'utf-8').strip().replace('\x00', '').split(split_char)

        if filter == 'all' or msg[0] in filter:
            try:
                value = [float(v) for v in msg[1:]]
            except ValueError:
                value = msg[1:]

            if len(msg) == 1:
                key, value = msg[0], np.nan
            elif len(msg) == 2:
                key, value = msg[0], value[0]
            elif len(msg) > 2:
                key, value = msg[0], value

            if key not in current_messages.keys():
                current_messages[key] = value
                current_messages[key + '_time'] = data['start']
            else:
                try:
                    current_messages[key].extend(value)
                    current_messages[key + '_time'].extend(data['start'])
                except (AttributeError, TypeError) as e:
                    current_messages[key] = [current_messages[key], value]
                    current_messages[
                        key + '_time'] = [current_messages[key + '_time'],
                                          data['start']]

    return trial, current_messages, message_accumulator


cdef data2dict(sample_type, int * ef):
    '''
    Converts EDF event to dictionary.
    '''
    fd = edf_get_float_data(ef)
    cdef char * msg
    d = None
    if ((sample_type == STARTFIX) or
            (sample_type == STARTSACC) or
            (sample_type == STARTBLINK) or
            (sample_type == ENDFIX) or
            (sample_type == ENDSACC) or
            (sample_type == ENDBLINK) or
            (sample_type == MESSAGEEVENT)):
        message = ''
        if < int > fd.fe.message != 0:
            msg = & fd.fe.message.c
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
    return d
