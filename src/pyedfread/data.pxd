# -*- coding: utf-8 -*-
'''
Define SR Research structures used in the EDF.
''' 

from libc.stdint cimport int16_t, uint16_t, uint32_t, int64_t

cdef extern from 'edf_data.h':
    ctypedef struct FSAMPLE:
        uint32_t time  # time of sample
        float px[2]
        float py[2] # pupil xy
        float hx[2]
        float hy[2] # headref xy
        float pa[2] # pupil size or area
        float gx[2]
        float gy[2] # screen gaze xy
        float rx
        float ry # screen pixels per degree

        float gxvel[2]
        float gyvel[2]
        float hxvel[2]
        float hyvel[2]
        float rxvel[2]
        float ryvel[2]

        float fgxvel[2]
        float fgyvel[2]
        float fhxvel[2]
        float fhyvel[2]
        float frxvel[2]
        float fryvel[2]

        int16_t  hdata[8] # head-tracker data (not prescaled)
        uint16_t flags #flags to indicate contents
        uint16_t input #extra (input word)
        uint16_t buttons # button state & changes
        int16_t  htype #head-tracker data type
        int16_t errors #process error flags

    ctypedef struct LSTRING:
        int16_t len
        char c

    ctypedef struct FEVENT:
        uint32_t time #effective time of event
        int16_t type # event type
        int16_t read #flags which items were included
        int16_t eye # eye: 0=left,1=right

        uint32_t sttime, entime # start, end times
        float  hstx, hsty # starting points
        float  gstx, gsty # starting points
        float  sta
        float  henx, heny # ending points
        float  genx, geny # ending points
        float  ena
        float  havx, havy # averages
        float  gavx, gavy # averages
        float  ava
        float  avel # avg velocity accum
        float  pvel # peak velocity accum
        float  svel, evel # start, end velocity
        float  supd_x, eupd_x #start, end units-per-degree
        float  supd_y, eupd_y # start, end units-per-degree

        uint16_t status # error, warning flags
        uint16_t flags # error, warning flag
        uint16_t input
        uint16_t buttons
        uint16_t parsedby # 7 bits of flags: PARSEDBY code
        LSTRING *message # any message strin

    ctypedef struct  IMESSAGE:
        uint32_t time # time message logged
        int16_t  type # event type: usually MESSAGEEVENT
        uint16_t length # length of message
        char text[260] # message contents (max length 255)

    ctypedef struct IOEVENT:
        uint32_t time # time logged
        int16_t  type # event type:
        uint16_t data # coded event data

    ctypedef struct RECORDINGS:
        uint32_t time # start time or end tim
        float sample_rate # 250 or 500 or 100
        uint16_t eflags # to hold extra information about events
        uint16_t sflags # to hold extra information about samples
        char state # 0 = END, 1=START
        char record_type # 1 = SAMPLES, 2= EVENTS, 3= SAMPLES and EVENT
        char pupil_type	# 0 = AREA, 1 = DIAMETE
        char recording_mode # 0 = PUPIL, 1 = CR
        char filter_type # 1,2,3
        char  pos_type #PARSEDBY_GAZE  PARSEDBY_HREF PARSEDBY_PUPI
        char eye # 1=LEFT, 2=RIGHT, 3=LEFT and RIGHT

    ctypedef union ALLF_DATA:
        FEVENT    fe
        IMESSAGE  im
        IOEVENT   io
        FSAMPLE   fs
        RECORDINGS rec
