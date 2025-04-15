# -*- coding: utf-8 -*-
# cython: profile=True
"""Read SR Research EDF files and parse them into a list of dicts."""

cimport numpy as np
import numpy as np
import string

from libc.stdint cimport int16_t, uint16_t, uint32_t, int64_t
from libc.stdlib cimport malloc, free

from libc.stdio cimport printf

from pyedfread.edf_data import *
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
    'fhxvel',
    'fhyvel',
    'frxvel',
    'fryvel',
    'flags',
    'input',
    'buttons',
    'htype',
    'errors',
]


cdef extern from "edf.h":
    ctypedef int EDFFILE
    EDFFILE * edf_open_file(
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
    cdef EDFFILE * ef
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
    cdef EDFFILE * ef
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


cdef data2dict(sample_type, EDFFILE * ef):
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
        current_event['contains_blink'] = False
        current_event['trial'] = trial


    if (sample_type == ENDFIX) or (sample_type == ENDSACC):
        current_event.update(data)
        event_accumulator.append(current_event)

    if (sample_type == STARTBLINK) or (sample_type == ENDBLINK):
        current_event['contains_blink'] = True
        if (sample_type == ENDBLINK):
            event_accumulator.append(data)

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

def create_sample_array_memory(num_elements):
    #First, just initialize all to float to save time.
    # Note, num_elements is longer than the maximum that we will
    # actually hold, so we must be sure to truncate at very end
    # of parse.
    mydict = {  colname : np.ndarray(num_elements, dtype=np.float64)  for colname in sample_columns };
    #Then, reallocate only those which are not float64s...
    mydict['time'] = np.ndarray(num_elements, dtype=np.uint32);
    mydict['flags'] = np.ndarray(num_elements, dtype=np.uint16);
    mydict['input'] = np.ndarray(num_elements, dtype=np.uint16);
    mydict['buttons'] = np.ndarray(num_elements, dtype=np.uint16);
    mydict['htype'] = np.ndarray(num_elements, dtype=np.int16);
    mydict['errors'] = np.ndarray(num_elements, dtype=np.uint16);
    # mydict['hdata'] is not included but may be added in future
    # (it is reserved for future use).
    # REV: We should check that htype is always 0. If not, this must
    # be fixed to address new API/capabilities.
    return mydict;

def parse_edf(
    filename, ignore_samples=False, message_filter=None, trial_marker='TRIALID'
):
    """Read samples, events, and messages from an EDF file."""
    cdef int errval = 1
    cdef char * buf = < char * > malloc(1024 * sizeof(char))
    cdef EDFFILE * ef
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

    
    samples = create_sample_array_memory(num_elements);
    
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
            #samples = parse_sample(fd, cnt, samples);
	    
            # We may have to re-order sorted dict order into
            # sample_columns before returning DF if we care.
            
            samples['time'][cnt] = fd.fs.time; #0
            samples['px_left'][cnt] = float(fd.fs.px[0]); #1
            samples['px_right'][cnt] = float(fd.fs.px[1]); #2
            samples['py_left'][cnt] = float(fd.fs.py[0]); #3
            samples['py_right'][cnt] = float(fd.fs.py[1]); #4
            samples['hx_left'][cnt] = float(fd.fs.hx[0]); #5
            samples['hx_right'][cnt] = float(fd.fs.hx[1]); #6
            samples['hy_left'][cnt] = float(fd.fs.hy[0]); #7
            samples['hy_right'][cnt] = float(fd.fs.hy[1]); #8
            samples['pa_left'][cnt] = float(fd.fs.pa[0]); #9
            samples['pa_right'][cnt] = float(fd.fs.pa[1]); #10
            samples['gx_left'][cnt] = float(fd.fs.gx[0]); #11
            samples['gx_right'][cnt] = float(fd.fs.gx[1]); #12
            samples['gy_left'][cnt] = float(fd.fs.gy[0]); #13
            samples['gy_right'][cnt] = float(fd.fs.gy[1]); #14
            samples['rx'][cnt] = float(fd.fs.rx); #15
            samples['ry'][cnt] = float(fd.fs.ry); #16
            samples['gxvel_left'][cnt] = float(fd.fs.gxvel[0]); #17
            samples['gxvel_right'][cnt] = float(fd.fs.gxvel[1]); #18
            samples['gyvel_left'][cnt] = float(fd.fs.gyvel[0]); #19
            samples['gyvel_right'][cnt] = float(fd.fs.gyvel[1]); #20
            samples['hxvel_left'][cnt] = float(fd.fs.hxvel[0]); #21
            samples['hxvel_right'][cnt] = float(fd.fs.hxvel[1]); #22
            samples['hyvel_left'][cnt] = float(fd.fs.hyvel[0]); #23
            samples['hyvel_right'][cnt] = float(fd.fs.hyvel[1]); #24
            samples['rxvel_left'][cnt] = float(fd.fs.rxvel[0]); #25
            samples['rxvel_right'][cnt] = float(fd.fs.rxvel[1]); #26
            samples['ryvel_left'][cnt] = float(fd.fs.ryvel[0]); #27
            samples['ryvel_right'][cnt] = float(fd.fs.ryvel[1]); #28
            
            ## REV: These are all accessing 0th element for some reason
            samples['fgxvel'][cnt] = float(fd.fs.fgxvel[0]); #29
            samples['fgyvel'][cnt] = float(fd.fs.fgyvel[0]); #30
            samples['fhxvel'][cnt] = float(fd.fs.fhxvel[0]); #31
            samples['fhyvel'][cnt] = float(fd.fs.fhyvel[0]); #32
            samples['frxvel'][cnt] = float(fd.fs.frxvel[0]); #33
            samples['fryvel'][cnt] = float(fd.fs.fryvel[0]); #34
            
            # samples[cnt, 39:48] =  <float>fd.fs.hdata # head-tracker data
            # (not prescaled)
            
            samples['flags'][cnt] = fd.fs.flags; #35
            samples['input'][cnt] = fd.fs.input; #36 # extra (input word)
            samples['buttons'][cnt] = fd.fs.buttons  #37 # button state & changes
            samples['htype'][cnt] = fd.fs.htype; #38  # head-tracker data type
            samples['errors'][cnt] = fd.fs.errors; #39
            
            cnt += 1;
            pass;
        
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
    # num_elements contained number of combined samples, messages, and events. This truncates
    # to only number of samples read (cnt).
    for key in samples:
        samples[key] = samples[key][:cnt];
        pass;
    
    return samples, event_accumulator, message_accumulator



#REV: converts samples 'time' column from default uint32 to float64, with 0.5 msec offsets.
def samples_to_ftime(samples):
    """
    Converts time column from uint32 to float64, and adds 0.5 for each with flags column
    SAMPLE_ADD_OFFSET bit set true.

    Input: pandas dataframe samples.

    Output: Returns modified dataframe samples with time converted to float64, and with appropriate OFFSET
    added.
    """
    bitmask = SAMPLE_ADD_OFFSET; #bitmask = 0x0002;
    HALFMS=0.5; #unit is already MSEC for edf's "time".
    if( samples['flags'].dtype != np.uint16 ):
        raise Exception("ERROR: flags should be of unsigned integer type for bitwise and to work properly")
    samples['_halfms'] = (0 != (bitmask & samples['flags']));
    samples['time'] = samples['time'].astype(np.float64);
    s.loc[ (True==s['_halfms']), 'time' ] += HALFMS;
    
    # Drop temporary _halfms column
    samples = samples[ [a for a in samples.columns if a is not '_halfms' ] ];
    
    return samples;
