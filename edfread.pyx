# -*- coding: utf-8 -*-
'''
Reads SR Research EDF files and parses them into ocupy datamats.
'''
from libc.stdint cimport int16_t, uint16_t, uint32_t, int64_t
from libc.stdlib cimport malloc, free

from edf_data import *
from data cimport *

cimport numpy as np
import numpy as np
import string 

from ocupy import datamat

try:
    import progressbar
except ImportError:
    progressbar = None

progressbar = None

type2label = {STARTFIX:'fixation', STARTSACC:'saccade',STARTBLINK:'blink',
        ENDFIX:'fixation', ENDSACC: 'saccade', ENDBLINK: 'blink',
        MESSAGEEVENT:'message'}

type2info = {STARTFIX:'start fixation', STARTSACC:'start saccade',
        STARTBLINK:'start blink', ENDFIX:'end fixation', ENDSACC: 'end saccade', 
        ENDBLINK: 'end blink', MESSAGEEVENT:'message', SAMPLE_TYPE:'sample'}


cdef extern from "edf.h":
    ctypedef int EDFFILE
    int* edf_open_file(char *theta, int consistency, int load_events, 
            int load_samples, int *errval)
    int  edf_get_preamble_text(EDFFILE *ef,
            char * buffer, int length)
    int edf_get_next_data(EDFFILE *ef)
    ALLF_DATA* edf_get_float_data(EDFFILE *ef)
    int edf_get_element_count(EDFFILE *ef)
    int edf_close_file(EDFFILE *ef)

def remove_time_fields(events):
    for key in events.fieldnames():
        if 'message_send_time' in key:
            events.rm_field(key)

def trials2events(events, messages):
    for key in messages.fieldnames():
        if key == 'trial':
            continue
        events.add_field(key, 
                np.empty(events.field(events.fieldnames()[0]).shape,
                dtype=messages.field(key).dtype))
    for trial in np.unique(messages.field('trial')):
        idt = events.trial ==  trial
        for key in messages.fieldnames():
            if key == 'trial':
                continue
            try:
                events.field(key)[idt] = messages.field(key)[trial]
            except ValueError:
                raise RuntimeError(
'''
Can not assign trial metadata to fixations. The trial metadata probably
contains an array, e.g. more than one value per trial. In this case I 
don't know how to assign these values to fixations.
''')
    

def fread(filename, ignore_samples = False, filter = []):
    cdef int errval = 1
    cdef char* buf = <char*> malloc(1024*sizeof(char)) 
    cdef int* ef
    cdef int sample_type
    current_event = {'samples':[]}
    current_messages = {}
    current_buttons = {}
    
    event_accumulator = datamat.AccumulatorFactory()
    message_accumulator = datamat.AccumulatorFactory()
    sample_accumulator = SampleAccumulator()

    ef = edf_open_file(filename, 0, 1, 1, &errval)
    if errval<0:
        print filename, ' could not be openend.'
        import sys
        sys.exit(-1)
    e = edf_get_preamble_text(ef, buf, 1024)
    num_elements = edf_get_element_count(ef)
    if progressbar is not None:
        bar = progressbar.ProgressBar(maxval=num_elements).start()
        cnt = 0
    trial = 0
    while True:
        sample_type = edf_get_next_data(ef)
        data = data2dict(sample_type, ef)
        if (sample_type == STARTFIX) or (sample_type == STARTSACC): 
            current_event = data
            current_event['trial']=trial
            current_event['blink']=False
            if not ignore_samples:
                sample_accumulator = SampleAccumulator()

        if (sample_type == ENDFIX) or (sample_type == ENDSACC): 
            if not ignore_samples:
                current_event['samples'] = sample_accumulator
            current_event.update(data)
            event_accumulator.update(current_event)

        if (sample_type == STARTBLINK) or (sample_type == ENDBLINK):
            current_event['blink'] = True

        if (sample_type == SAMPLE_TYPE) and not ignore_samples:
            sample_accumulator.update(data)

        if sample_type == MESSAGEEVENT:
            if data['message'].startswith('TRIALID'):
                if (trial > 0) and (len(current_messages.keys()) > 0):
                    current_messages['trial'] = trial
                    message_accumulator.update(current_messages)
                trial +=1
                current_messages = {}
            elif data['message'].startswith('SYNCTIME'):
                current_messages['SYNCTIME'] = data['time']
            elif data['message'].startswith('DRIFTCORRECT'):
                current_messages['DRIFTCORRECT'] = data['message']
            elif data['message'].startswith('METATR'):
                parts = data['message'].split(' ')
                msg, key = parts[0], parts[1] 
                if len(parts) == 3:
                    value = parts[2].strip().replace('\x00','')
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
                msg = data['message'].strip().replace('\x00','').split(' ')
                if (len(msg) == 1) or not (msg[0] in filter):
                    continue
                try:
                    value = [string.atof(v) for v in msg[1:]]
                except ValueError:
                    value = msg[1:]

                if len(msg) > 2:
                    key, value = msg[0], value 
                elif len(msg) == 2:
                    key, value = msg[0], value[0]
                if key not in current_messages.keys():
                    current_messages[key] = []
                    current_messages[key+'_time'] = [] 
                current_messages[key].append(value)
                current_messages[key+'_time'].append(data['start'])

        if sample_type == NO_PENDING_ITEMS:
            if len(current_messages.keys())>0:
                message_accumulator.update(current_messages)
            edf_close_file(ef)
            break
        if progressbar is not None:
            bar.update(cnt)
            cnt += 1
    free(buf)
    if not ignore_samples:
        for i in range(len(event_accumulator.d['samples'])):
            event_accumulator.d['samples'][i] = event_accumulator.d['samples'][i].get_dict() 
    return event_accumulator.get_dm(), message_accumulator.get_dm()


cdef data2dict(sample_type, int* ef, filter=['type','time','sttime',
                'entime','gx','gy','gstx','gsty','genx','geny',
                'gxvel','gyvel','start','end', 'gavx', 'gavy']):
    fd = edf_get_float_data(ef)
    cdef char *msg
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
        d = {'time': fd.fe.time, 'type':type2label[sample_type],
            'start':fd.fe.sttime,'end':fd.fe.entime,
            'hstx':fd.fe.hstx, 'hsty':fd.fe.hsty, 
            'gstx':fd.fe.gstx,'gsty':fd.fe.gsty,
            'sta':fd.fe.sta, 'henx':fd.fe.henx,
            'heny':fd.fe.heny, 'genx':fd.fe.genx,
            'geny':fd.fe.geny, 'ena':fd.fe.ena, 
            'havx':fd.fe.havx, 'havy':fd.fe.havy,
            'gavx':fd.fe.gavx, 'gavy':fd.fe.gavy, 
            'ava':fd.fe.ava, 'avel':fd.fe.avel,
            'pvel':fd.fe.pvel, 'svel':fd.fe.svel, 
            'evel':fd.fe.evel, 'supd_x':fd.fe.supd_x, 'eupd_x':fd.fe.eupd_x, 
            'eye':fd.fe.eye, 'buttons':fd.fe.buttons, 'message':message, 
            }
    
    if sample_type == SAMPLE_TYPE:
        d = {'time':fd.fs.time,
            'px':(fd.fs.px[0], fd.fs.px[1]),
            'py':(fd.fs.py[0], fd.fs.py[1]),
            'hx':(fd.fs.hx[0], fd.fs.hx[1]),
            'hy':(fd.fs.hy[0], fd.fs.hy[1]),
            'gx':(fd.fs.gx[0], fd.fs.gx[1]),
            'gy':(fd.fs.gy[0], fd.fs.gy[1]),
            'pa':(fd.fs.pa[0], fd.fs.pa[1]),
            'rx':fd.fs.rx,
            'ry':fd.fs.ry,
            'gxvel':(fd.fs.gxvel[0], fd.fs.gxvel[1]),
            'gyvel':(fd.fs.gyvel[0], fd.fs.gyvel[1]),
            'hxvel':(fd.fs.hxvel[0], fd.fs.hxvel[1]),
            'hyvel':(fd.fs.hyvel[0], fd.fs.hyvel[1]),
            'rxvel':(fd.fs.rxvel[0], fd.fs.rxvel[1]),
            'ryvel':(fd.fs.ryvel[0], fd.fs.ryvel[1]),
            'fgxvel':(fd.fs.fgxvel[0], fd.fs.fgxvel[1]),
            'fgyvel':(fd.fs.fgyvel[0], fd.fs.fgyvel[1]),
            'fhxvel':(fd.fs.fhxvel[0], fd.fs.fhxvel[1]),
            'fhyvel':(fd.fs.fhyvel[0], fd.fs.fhyvel[1]),
            'frxvel':(fd.fs.frxvel[0], fd.fs.frxvel[1]),
            'fryvel':(fd.fs.fryvel[0], fd.fs.fryvel[1])}
    if d is None:
        return d
    rd = {}
    for key, val in d.iteritems():
        if key in filter + ['message']:
            rd[key] = val
    return rd

class SampleAccumulator(object):
    
    def __init__(self):
        self.d = {}

    def update(self, a):
        if len(self.d.keys()) == 0:
            self.d = dict((k,[v]) for k,v in a.iteritems())
        else:
            # For all fields in a that are also in dict
            all_keys = set(a.keys() + self.d.keys())
            for key in all_keys:
                if key in a.keys():
                    value = a[key]
                if not key in self.d.keys():
                    # key is not yet in d. add it
                    self.d[key] = [np.nan]*len(self.d[self.d.keys()[0]])
                if not key in a.keys():
                    # key is not in new data. value should be nan
                    value = np.nan
                self.d[key].extend([value])                
    
    def get_dict(self, params = None):
        for key, value in self.d.iteritems():
            self.d[key] = np.array(value)
        return self.d


