# -*- coding: utf-8 -*-
# cython: profile=True
"""Read SR Research EDF files and parse them into a list of dicts."""

cimport numpy as np
import numpy as np
import string

from libc.stdint cimport int16_t, uint16_t, uint32_t, int64_t
from libc.stdlib cimport malloc, free

from edfread.edf_data import *
from pyedfread.data cimport ALLF_DATA

import struct


type2label = {
    STARTFIX: 'fixation',
    STARTSACC: 'saccade',
    STARTBLINK: 'blink',
    ENDFIX: 'fixation',
    ENDSACC: 'saccade',
    ENDBLINK: 'blink',
    MESSAGEEVENT: 'message',
}

sample_columns = [
    'time',
    'px_left',
    'px_right',
    'py_left',
    'py_right',
    'hx_left',
    'hx_right',
    'hy_left',
    'hy_right',
    'pa_left',
    'pa_right',
    'gx_left',
    'gx_right',
    'gy_left',
    'gy_right',
    'rx',
    'ry',
    'gxvel_left',
    'gxvel_right',
    'gyvel_left',
    'gyvel_right',
    'hxvel_left',
    'hxvel_right',
    'hyvel_left',
    'hyvel_right',
    'rxvel_left',
    'rxvel_right',
    'ryvel_left',
    'ryvel_right',
    'fgxvel',
    'fgyvel',
    'fhxyvel',
    'fhyvel',
    'frxyvel',
    'fryvel',
    'flags',
    'input',
    'buttons',
    'htype',
    'errors',
]


cdef extern from "edf.h":
    ctypedef int EDFFILE
    int * edf_open_file(
        const char * fname,
        int consistency,
        int load_events,
        int load_samples,
        int * errval,
    )
    int edf_get_preamble_text_length(EDFFILE * edf)
    int edf_get_preamble_text(EDFFILE * ef, char * buffer, int length)
    int edf_get_next_data(EDFFILE * ef)
    ALLF_DATA * edf_get_float_data(EDFFILE * ef)
    int edf_get_element_count(EDFFILE * ef)
    int edf_close_file(EDFFILE * ef)


def read_preamble(filename, consistency=0):
    """Read preamble of EDF file."""
    cdef int errval = 1
    cdef int * ef
    ef = edf_open_file(filename.encode('utf-8'), consistency, 1, 1, & errval)
    if errval < 0:
        raise IOError(f'Could not open file: {filename}')
    cdef int psize = edf_get_preamble_text_length(ef)
    cdef char * buf = <char * > malloc(psize * sizeof(char))
    e = edf_get_preamble_text(ef, buf, psize)
    edf_close_file(ef)
    return buf.decode('utf-8')


def read_messages(filename, startswith=None, consistency=0):
    """Read messages from an edf file."""
    cdef int errval = 1
    cdef int * ef
    cdef char * msg
    ef = edf_open_file(filename.encode('utf-8'), consistency, 1, 1, & errval)
    if errval < 0:
        raise IOError(f'Could not open file: {filename}')

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
                message = message.decode('utf-8').replace('\x00', '').strip()
                if (
                    startswith is None
                    or any([message.startswith(s) for s in startswith])
                ):
                    messages.append(message)
    return messages


def read_calibration(filename, consistency=0):
    """Read calibration/validation messages from an EDF file."""
    start = ['!VAL', '!CAL']
    messages = read_messages(filename, startswith=start, consistency=consistency)
    return messages


cdef data2dict(sample_type, int * ef):
    """Convert EDF event to a dictionary."""
    fd = edf_get_float_data(ef)
    cdef char * msg
    d = None
    if (
        (sample_type == STARTFIX)
        or (sample_type == STARTSACC)
        or (sample_type == STARTBLINK)
        or (sample_type == ENDFIX)
        or (sample_type == ENDSACC)
        or (sample_type == ENDBLINK)
        or (sample_type == MESSAGEEVENT)
    ):
        message = ''
        if < int > fd.fe.message != 0:
            msg = & fd.fe.message.c
            message = msg[:fd.fe.message.len]
        d = {
            'time': fd.fe.time,
            'type': type2label[sample_type],
            'start': fd.fe.sttime,
            'end': fd.fe.entime,
            'hstx': fd.fe.hstx,
            'hsty': fd.fe.hsty,
            'gstx': fd.fe.gstx,
            'gsty': fd.fe.gsty,
            'sta': fd.fe.sta,
            'henx': fd.fe.henx,
            'heny': fd.fe.heny,
            'genx': fd.fe.genx,
            'geny': fd.fe.geny,
            'ena': fd.fe.ena,
            'havx': fd.fe.havx,
            'havy': fd.fe.havy,
            'gavx': fd.fe.gavx,
            'gavy': fd.fe.gavy,
            'ava': fd.fe.ava,
            'avel': fd.fe.avel,
            'pvel': fd.fe.pvel,
            'svel': fd.fe.svel,
            'evel': fd.fe.evel,
            'supd_x': fd.fe.supd_x,
            'eupd_x': fd.fe.eupd_x,
            'eye': fd.fe.eye,
            'buttons': fd.fe.buttons,
            'message': message,
        }
    return d


def parse_datum(
    data,
    sample_type,
    trial,
    current_event,
    event_accumulator,
):
    """Parse a datum into data structures."""
    if data is None:
        return current_event

    if (sample_type == STARTFIX) or (sample_type == STARTSACC):
        current_event = data
        current_event['blink'] = False
        current_event['trial'] = trial

    if (sample_type == ENDFIX) or (sample_type == ENDSACC):
        current_event.update(data)
        event_accumulator.append(current_event)

    if (sample_type == STARTBLINK) or (sample_type == ENDBLINK):
        current_event['blink'] = True
    return current_event


def parse_message(data, trial, message_accumulator, message_filter, trial_marker):
    """Parse message information based on message type."""
    message = data['message'].decode('utf-8').replace('\x00', '').strip()
    if message.startswith(trial_marker):
        if trial <= 0:
            trial = 1
        else:
            trial += 1

    if message_filter is None or any([message.startwith(s) for s in message_filter]):
        info = {'time': data['start'], 'trial': trial, 'message': message}
        message_accumulator.append(info)
    return trial


def parse_edf(
    filename, ignore_samples=False, message_filter=None, trial_marker='TRIALID'
):
    """Read samples, events, and messages from an EDF file."""
    cdef int errval = 1
    cdef char * buf = < char * > malloc(1024 * sizeof(char))
    cdef int * ef
    cdef int sample_type, cnt, trial

    # open the file
    ef = edf_open_file(filename.encode('utf-8'), 0, 1, 1, & errval)
    if errval < 0:
        raise IOError(f'Could not open: {filename}')
    e = edf_get_preamble_text(ef, buf, 1024)

    # initialize sample array
    num_elements = edf_get_element_count(ef)
    if ignore_samples:
        num_elements = 0
    cdef np.ndarray npsamples = np.ndarray((num_elements, 40), dtype=np.float64)
    cdef np.float64_t[:, :] samples = npsamples

    # parse samples and events
    trial = -1
    cnt = 0
    current_messages = {}
    current_event = {}
    message_accumulator = []
    event_accumulator = []
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
            trial = parse_message(
                data, trial, message_accumulator, message_filter, trial_marker
            )

        else:
            data = data2dict(sample_type, ef)
            current_event = parse_datum(
                data, sample_type, trial, current_event, event_accumulator
            )
    free(buf)
    return samples, event_accumulator, message_accumulator
