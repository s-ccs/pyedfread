/**********************************************************************************
 * EYELINK PORTABLE EXPT SUPPORT      (c) 1996, 2003 by SR Research               *
 *     8 June '97 by Dave Stampe       For non-commercial use only                *
 * Header file for standard functions                                             *
 * This module is for user applications   Use is granted for non-commercial       *
 * applications by Eyelink licencees only                                         *
 *                                                                                *
 *                                                                                *
 ******************************************* WARNING ******************************
 *                                                                                *
 * UNDER NO CIRCUMSTANCES SHOULD PARTS OF THESE FILES BE COPIED OR COMBINED.      *
 * This will make your code impossible to upgrade to new releases in the future,  *
 * and SR Research will not give tech support for reorganized code.               *
 *                                                                                *
 * This file should not be modified. If you must modify it, copy the entire file  *
 * with a new name, and change the the new file.                                  *
 *                                                                                *
 **********************************************************************************/

/*!
	\file eye_data.h
        \brief Declaration of complex EyeLink data types and link data structures
 */

#ifndef __SRRESEARCH__EYE_DATA_H__
#define __SRRESEARCH__EYE_DATA_H__

#ifndef BYTEDEF
  #include "eyetypes.h"
#endif

#ifndef SIMTYPESINCL
#define SIMTYPESINCL

#ifdef __cplusplus     /* For C++ compilation */
extern "C" {
#endif

/*********** EYE DATA FORMATS **********/
/**********************************************************************************
 * ALL fields use MISSING_DATA when value was not read, EXCEPT for <buttons>,     *
 * <time>, <sttime> and <entime>, which use 0. This is true for both floating     *
 * point or integer variables. Both samples and events may have several fields    *
 * that have not been updated. These fields may be detected from the content      *
 * flags, or by testing the field value against these constants:                  *
 **********************************************************************************/
/*! 
@defgroup gendataflags General Data Constants 
@ingroup messaging
@{
*/
#define MISSING_DATA -32768     /*!< data is missing (integer) */
#define MISSING -32768			/*!< data is missing (integer) */
#define INaN -32768				/*!< data is missing (integer) */

/**********************************************************************************
 * binocular data needs to ID the eye for events
 * samples need to index the data
 * These constants are used as eye identifiers
 **********************************************************************************/

#define LEFT_EYE  0		/*!< Index and ID of left eye */
#define RIGHT_EYE 1		/*!< Index and ID of right eye */
#define LEFTEYEI  0		/*!< Index and ID of left eye */
#define RIGHTEYEI 1		/*!< Index and ID of right eye */
#define LEFT      0		/*!< Index and ID of left eye */
#define RIGHT     1		/*!< Index and ID of right eye */
#define BINOCULAR 2		/*!< Data for both eyes available */


/*!
Convenient macro to get the float time. Only used in Eyelink1000 2K
*/ 
#define FLOAT_TIME(x) (((double)((x)->time)) + (((x)->type==SAMPLE_TYPE && (x)->flags & SAMPLE_ADD_OFFSET)?0.5:0.0))
/*! 
@}
*/



/********* EYE SAMPLE DATA FORMATS *******/
/*! 
@defgroup sampledataflags Sample Data Flags
@ingroup messaging

The SAMPLE struct contains data from one 4-msec eye-tracker sample. The <flags>
field has a bit for each type of data in the sample. Fields not read have 0
flag bits, and are set to MISSING_DATA flags to define what data is included
in each sample.  There is one bit for each type.  Total data for samples
in a block is indicated by these bits in the <sam_data> field of ILINKDATA or
EDF_FILE, and is updated by the STARTSAMPLES control event.

@{
*/




#define SAMPLE_LEFT      0x8000  /*!< Data for left eye */
#define SAMPLE_RIGHT     0x4000  /*!< Data for right eye */

#define SAMPLE_TIMESTAMP 0x2000  /*!< Time stamp for sample. bit always set for link sample, used to compress files */

#define SAMPLE_PUPILXY   0x1000  /*!< Pupil x,y pair */
#define SAMPLE_HREFXY    0x0800  /*!< Head-referenced x,y pair */
#define SAMPLE_GAZEXY    0x0400  /*!< Gaze x,y pair */
#define SAMPLE_GAZERES   0x0200  /*!< Gaze res (x,y pixels per degree) pair */
#define SAMPLE_PUPILSIZE 0x0100  /*!< Pupil size */
#define SAMPLE_STATUS    0x0080  /*!< Error flags */
#define SAMPLE_INPUTS    0x0040  /*!< Input data port */
#define SAMPLE_BUTTONS   0x0020  /*!< Button state: LSBy state, MSBy changes */

#define SAMPLE_HEADPOS   0x0010  /*!< Head-position: byte tells # words */
#define SAMPLE_TAGGED    0x0008  /*!< Reserved variable-length tagged */
#define SAMPLE_UTAGGED   0x0004  /*!< User-defineabe variable-length tagged */
#define SAMPLE_ADD_OFFSET 0x0002 /*!< If this flag is set for the sample add .5ms to the sample time */
/*!
@}
*/
#ifndef ISAMPLEDEF
#define ISAMPLEDEF
/*! @ingroup messaging
	\brief Integer sample data */
typedef struct {
    UINT32 	time;         	/*!< time of sample */
    INT16  	type;           /*!< always SAMPLE_TYPE */
    UINT16 	flags;         	/*!< flags to indicate contents */
    INT16  	px[2];			/*!< pupil x */
    INT16  	py[2];    		/*!< pupil y */
    INT16  	hx[2];			/*!< headref x */
    INT16  	hy[2];    		/*!< headref y */
    UINT16 	pa[2];          /*!< pupil size or area */
    INT16  	gx[2];			/*!< screen gaze x */
    INT16  	gy[2];     		/*!< screen gaze y */
    INT16  	rx;				/*!< screen pixels per degree */
    INT16  	ry;           	/*!< screen pixels per degree */
    UINT16 	status;         /*!< tracker status flags    */
    UINT16 	input;          /*!< extra (input word)      */
    UINT16 	buttons;        /*!< button state & changes  */
    INT16  	htype;          /*!< head-tracker data type (0=none) */
    INT16  hdata[8];        /*!< head-tracker data */
} ISAMPLE;
#endif



#ifndef FSAMPLEDEF
#define FSAMPLEDEF 1    	/* gaze, resolution prescaling removed */

/*! @ingroup messaging
\brief Floating-point sample

The EyeLink tracker measures eye position 250, 500, 1000 or 2000 times per second depending on
the tracking mode you are working with, and computes true gaze position on the display
using the head camera data. This data is stored in the EDF file, and made available
through the link in as little as 3 milliseconds after a physical eye movement.
Samples can be read from the link by eyelink_get_float_data() or eyelink_newest_float_sample().

If sample rate is 2000hz, two samples with same time stamp possible. If SAMPLE_ADD_OFFSET is set on the
flags, add .5 ms to get the real time. Convenient FLOAT_TIME can also be used.
*/
typedef struct {
    UINT32 	time;          	/*!< time of sample */
    INT16  	type;           /*!< always SAMPLE_TYPE */
    UINT16 	flags;         	/*!< flags to indicate contents */
    float  	px[2];			/*!< pupil x */
    float	py[2];  		/*!< pupil y */
    float  	hx[2]; 			/*!< headref x */
    float 	hy[2];    		/*!< headref y */
    float  	pa[2];         	/*!< pupil size or area */
    float  	gx[2];			/*!< screen gaze x */
    float	gy[2];        	/*!< screen gaze y */
    float 	rx;				/*!< screen pixels per degree */
    float	ry;            	/*!< screen pixels per degree */
    UINT16 	status;        	/*!< tracker status flags    */
    UINT16 	input;        	/*!< extra (input word)      */
    UINT16 	buttons;       	/*!< button state & changes  */
    INT16  	htype;         	/*!< head-tracker data type (0=none)   */
    INT16  	hdata[8];      	/*!< head-tracker data (not prescaled) */
} FSAMPLE;
#endif

#ifndef DSAMPLEDEF
#define DSAMPLEDEF 1    	/* gaze, resolution prescaling removed */

/*! @ingroup messaging
\brief Floating-point sample with floating point time

The EyeLink tracker measures eye position 250, 500, 1000 or 2000 times per second depending on
the tracking mode you are working with, and computes true gaze position on the display
using the head camera data. This data is stored in the EDF file, and made available
through the link in as little as 3 milliseconds after a physical eye movement.
Samples can be read from the link by eyelink_get_double_data() or eyelink_newest_double_sample().

*/
typedef struct {
    double 	time;          	/*!< time of sample */
    INT16  	type;           /*!< always SAMPLE_TYPE */
    UINT16 	flags;         	/*!< flags to indicate contents */
    float  	px[2];			/*!< pupil x */
    float	py[2];  		/*!< pupil y */
    float  	hx[2]; 			/*!< headref x */
    float 	hy[2];    		/*!< headref y */
    float  	pa[2];         	/*!< pupil size or area */
    float  	gx[2];			/*!< screen gaze x */
    float	gy[2];        	/*!< screen gaze y */
    float 	rx;				/*!< screen pixels per degree */
    float	ry;            	/*!< screen pixels per degree */
    UINT16 	status;        	/*!< tracker status flags    */
    UINT16 	input;        	/*!< extra (input word)      */
    UINT16 	buttons;       	/*!< button state & changes  */
    INT16  	htype;         	/*!< head-tracker data type (0=none)   */
    INT16  	hdata[8];      	/*!< head-tracker data (not prescaled) */
} DSAMPLE;
#endif 







#ifndef FSAMPLERAWDEF
#define FSAMPLERAWDEF 1    	/* gaze, resolution prescaling removed */
/*! 
	@internal
	Used to access raw online data.
 */
typedef struct {
	UINT32 struct_size; // size of the structure. This is not used a the moment. 
						// for future use to support backwards compatibility

	float raw_pupil[2];
	float raw_cr[2];
	UINT32 pupil_area;
	UINT32 cr_area;
	UINT32 pupil_dimension[2];
	UINT32 cr_dimension[2];
	UINT32 window_position[2];
	float pupil_cr[2];

	UINT32 cr_area2;
	float raw_cr2[2];
} FSAMPLE_RAW;
#endif




/******** EVENT DATA FORMATS *******/

/**********************************************************************************
 * ALL fields use MISSING_DATA when value was not read,
 * EXCEPT for <buttons>, <time>, <sttime> and <entime>, which use 0.
 * This is true for both floating point or integer variables.
 **********************************************************************************/


#ifndef IEVENTDEF
#define IEVENTDEF
/*! @ingroup messaging
\brief Integer eye-movement events */
typedef struct  {
    UINT32 	time;          	/*!< effective time of event */
    INT16  	type;          	/*!< event type */
    UINT16 	read;          	/*!< flags which items were included */
    INT16  	eye;           	/*!< eye: 0=left,1=right */
    UINT32 	sttime;			/*!< start times */
    UINT32	entime;    		/*!< end times */
    INT16  	hstx;			/*!< starting point x */
    INT16	hsty;        	/*!< starting point y */
    INT16  	gstx; 			/*!< starting point x */
    INT16	gsty;        	/*!< starting point y */
    UINT16 	sta;
    INT16  	henx;			/*!< ending point x */
    INT16	heny;        	/*!< ending point y */
    INT16  	genx;			/*!< ending point x */
    INT16 	geny;        	/*!< ending point y */
    UINT16 	ena;
    INT16  	havx;			/*!< average x */
    INT16 	havy;        	/*!< average y */
    INT16  	gavx;			/*!< average x */
    INT16	gavy;        	/*!< average y */
    UINT16 	ava;           	/*!< also used as accumulator */
    INT16 	avel;          	/*!< avg velocity accum */
    INT16 	pvel;          	/*!< peak velocity accum */
    INT16 	svel;			/*!< start velocity */
    INT16 	evel;       	/*!< end velocity */
    INT16 	supd_x;			/*!< start units-per-degree x */
    INT16	eupd_x;   		/*!< end units-per-degree y */
    INT16 	supd_y;			/*!< start units-per-degree y */
    INT16 	eupd_y;   		/*!< end units-per-degree y */
    UINT16	status;       	/*!< error, warning flags */
} IEVENT;
#endif




#ifndef FEVENTDEF       /* gaze, resolution, velocity prescaling removed */
#define FEVENTDEF 1
/*!@ingroup messaging
 \brief Floating-point eye event

The EyeLink tracker analyzes the eye-position samples during recording to
detect saccades, and accumulates data on saccades and fixations. Events
are produced to mark the start and end of saccades, fixations and blinks.
When both eyes are being tracked, left and right eye events are produced,
as indicated in the eye field of the FEVENT structure.

Start events contain only the start time, and optionally the start eye
or gaze position. End events contain the start and end time, plus summary
data on saccades and fixations. This includes start and end and average
measures of position and pupil size, plus peak and average velocity in
degrees per second.


*/
typedef struct  {
    UINT32 time;            /*!< effective time of event */
    INT16  type;            /*!< event type */
    UINT16 read;            /*!< flags which items were included */
    INT16  eye;             /*!< eye: 0=left,1=right */
    UINT32 sttime;			/*!< start times */
    UINT32 entime;  		/*!< end times */
    float  hstx;			/*!< starting point x */
    float  hsty;      		/*!< starting point y */
    float  gstx;			/*!< starting point x */
    float  gsty;      		/*!< starting point y */
    float  sta;             /*!< starting area */
    float  henx;			/*!< ending point x */
    float  heny;      		/*!< ending point y */
    float  genx;			/*!< ending point x */
    float  geny;      		/*!< ending point y */
    float  ena;             /*!< ending area */
    float  havx;			/*!< average x */
    float  havy;      		/*!< average y */
    float  gavx;			/*!< average x */
    float  gavy;      		/*!< average y */
    float  ava;             /*!< average area */
    float  avel;            /*!< avg velocity accum */
    float  pvel;            /*!< peak velocity accum */
    float  svel;			/*!< start velocity */
    float  evel;      		/*!< end velocity */
    float  supd_x;			/*!< start units-per-degree x */
    float  eupd_x;  		/*!< end units-per-degree x */
    float  supd_y;			/*!< start units-per-degree y */
    float  eupd_y;  		/*!< end units-per-degree y */
    UINT16 status;          /*!< error, warning flags */
} FEVENT;
#endif

#ifndef DEVENTDEF       /* gaze, resolution, velocity prescaling removed */
#define DEVENTDEF 1
/*!@ingroup messaging
 \brief Floating-point eye event with floating point time

The EyeLink tracker analyzes the eye-position samples during recording to
detect saccades, and accumulates data on saccades and fixations. Events
are produced to mark the start and end of saccades, fixations and blinks.
When both eyes are being tracked, left and right eye events are produced,
as indicated in the eye field of the FEVENT structure.

Start events contain only the start time, and optionally the start eye
or gaze position. End events contain the start and end time, plus summary
data on saccades and fixations. This includes start and end and average
measures of position and pupil size, plus peak and average velocity in
degrees per second.


*/
typedef struct  {
    double time;            /*!< effective time of event */
    INT16  type;            /*!< event type */
    UINT16 read;            /*!< flags which items were included */
    INT16  eye;             /*!< eye: 0=left,1=right */
    double sttime;			/*!< start times */
    double entime;  		/*!< end times */
    float  hstx;			/*!< starting point x */
    float  hsty;      		/*!< starting point y */
    float  gstx;			/*!< starting point x */
    float  gsty;      		/*!< starting point y */
    float  sta;             /*!< starting area */
    float  henx;			/*!< ending point x */
    float  heny;      		/*!< ending point y */
    float  genx;			/*!< ending point x */
    float  geny;      		/*!< ending point y */
    float  ena;             /*!< ending area */
    float  havx;			/*!< average x */
    float  havy;      		/*!< average y */
    float  gavx;			/*!< average x */
    float  gavy;      		/*!< average y */
    float  ava;             /*!< average area */
    float  avel;            /*!< avg velocity accum */
    float  pvel;            /*!< peak velocity accum */
    float  svel;			/*!< start velocity */
    float  evel;      		/*!< end velocity */
    float  supd_x;			/*!< start units-per-degree x */
    float  eupd_x;  		/*!< end units-per-degree x */
    float  supd_y;			/*!< start units-per-degree y */
    float  eupd_y;  		/*!< end units-per-degree y */
    UINT16 status;          /*!< error, warning flags */
} DEVENT;
#endif

#ifndef IMESSAGEDEF
#define IMESSAGEDEF
/*!@ingroup messaging
\brief Message events: usually text but may contain binary data

A message event is created by your experiment program, and placed in the EDF file.
It is possible to enable the sending of these messages back through the link,
although there is rarely a reason to do this. Although this method might be
used to determine the tracker time (the time field of a message event will indicate
when the message was received by the tracker), the use of eyelink_request_time()
and eyelink_read_time() is more efficient for retrieving the current time from the
eye tracker's timestamp clock. The eye tracker time is rarely needed in any case,
and would only be useful to compute link transport delays.
*/
typedef struct  {
    UINT32 time;       	/*!< time message logged */
    INT16  type;       	/*!< event type: usually MESSAGEEVENT */
    UINT16 length;     	/*!< length of message */
    byte   text[260];  	/*!< message contents (max length 255) */
} IMESSAGE;
#endif


#ifndef DMESSAGEDEF
#define DMESSAGEDEF
/*!@ingroup messaging
\brief Message events: usually text but may contain binary data with floating point time.

A message event is created by your experiment program, and placed in the EDF file.
It is possible to enable the sending of these messages back through the link,
although there is rarely a reason to do this. Although this method might be
used to determine the tracker time (the time field of a message event will indicate
when the message was received by the tracker), the use of eyelink_request_time()
and eyelink_read_time() is more efficient for retrieving the current time from the
eye tracker's timestamp clock. The eye tracker time is rarely needed in any case,
and would only be useful to compute link transport delays.
*/
typedef struct  {
    double time;       	/*!< time message logged */
    INT16  type;       	/*!< event type: usually MESSAGEEVENT */
    UINT16 length;     	/*!< length of message */
    byte   text[260];  	/*!< message contents (max length 255) */
} DMESSAGE;
#endif


#ifndef IOEVENTDEF
#define IOEVENTDEF
/*! @ingroup messaging
\brief Button, input, other simple events

BUTTONEVENT and INPUTEVENT types are the simplest events, reporting
changes in button status or in the input port data. The time field
records the timestamp of the eye-data sample where the change occurred,
although the event itself is usually sent before that sample. The data
field contains the data after the change, in the same format as in the
FSAMPLE structure.

Button events from the link are rarely used; monitoring buttons with one
of eyelink_read_keybutton(), eyelink_last_button_press(), or
eyelink_button_states() is preferable, since these can report button
states at any time, not just during recording.
*/
typedef struct  {
    UINT32 time;       	/*!< time logged */
    INT16  type;       	/*!< event type: */
    UINT16 data;       	/*!< coded event data */
} IOEVENT;
#endif


#ifndef DIOEVENTDEF
#define DIOEVENTDEF
/*! @ingroup messaging
\brief Button, input, other simple events with floating point time.

BUTTONEVENT and INPUTEVENT types are the simplest events, reporting
changes in button status or in the input port data. The time field
records the timestamp of the eye-data sample where the change occurred,
although the event itself is usually sent before that sample. The data
field contains the data after the change, in the same format as in the
FSAMPLE structure.

Button events from the link are rarely used; monitoring buttons with one
of eyelink_read_keybutton(), eyelink_last_button_press(), or
eyelink_button_states() is preferable, since these can report button
states at any time, not just during recording.
*/
typedef struct  {
    double time;       	/*!< time logged */
    INT16  type;       	/*!< event type: */
    UINT16 data;       	/*!< coded event data */
} DIOEVENT;
#endif


/************ COMPOSITE DATA BUFFERS ***********/
/*! @ingroup messaging
\brief Union of message, io event and integer sample and integer event. 
*/
#ifndef ALLDATADEF
#define ALLDATADEF
typedef union {
    IEVENT    ie;
    IMESSAGE  im;
    IOEVENT   io;
    ISAMPLE   is;
} ALL_DATA ;
#endif

            /* FLOATING POINT TYPES for eye event, sample */
#ifndef EDFDATADEF
#define EDFDATADEF

/*! @ingroup messaging
\brief Union of message, io event and float sample and float event. 
*/
typedef union {
    FEVENT    fe;
    IMESSAGE  im;
    IOEVENT   io;
    FSAMPLE   fs;
} ALLF_DATA ;
#endif


            /* FLOATING POINT TYPES for eye event, sample */
#ifndef EDFDDATADEF
#define EDFDDATADEF
/*! @ingroup messaging
\brief Union of message, io event and double sample and double event. 
*/
typedef union {
    DEVENT    fe;
    DMESSAGE  im;
    DIOEVENT  io;
    DSAMPLE   fs;
} ALLD_DATA ;
#endif

/********** SAMPLE, EVENT BUFFER TYPE CODES ***********/




/*! 
@defgroup sample_event_type Eyelink Sample and Event Type Identifiers
@ingroup messaging
These codes can be used to identify the type of event or sample.
@{
*/
#define SAMPLE_TYPE 200 /*!< The type code for samples */

/************* EVENT TYPE CODES ************/
            /* buffer = IEVENT, FEVENT, btype = IEVENT_BUFFER */
#define STARTPARSE 1     /* these only have time and eye data */
#define ENDPARSE   2
#define BREAKPARSE 10


/* EYE DATA: contents determined by evt_data */
/* and by "read" data item */
/* all use IEVENT format */
#define STARTBLINK 3    /*!< Event is a start blink event */
#define ENDBLINK   4    /*!< Event is an end blink event */
#define STARTSACC  5	/*!< Event is a start saccade event */
#define ENDSACC    6	/*!< Event is an end saccade event */
#define STARTFIX   7	/*!< Event is a start fixation event */
#define ENDFIX     8	/*!< Event is an end fixation event */
#define FIXUPDATE  9	/*!< Event is a fixation update event */

  /* buffer = (none, directly affects state), btype = CONTROL_BUFFER */
             /* control events: all put data into */
             /* the EDF_FILE or ILINKDATA status  */
#define STARTSAMPLES 15  /*!< Start of events in block */
#define ENDSAMPLES   16  /*!< End of samples in block */
#define STARTEVENTS  17  /*!< Start of events in block */
#define ENDEVENTS    18  /*!< End of events in block */


    /* buffer = IMESSAGE, btype = IMESSAGE_BUFFER */
#define MESSAGEEVENT 24  /*!< User-definable text or data */


    /* buffer = IOEVENT, btype = IOEVENT_BUFFER */
#define BUTTONEVENT  25  /*!< Button state change */
#define INPUTEVENT   28  /*!< Change of input port */

#define LOST_DATA_EVENT 0x3F   /*!< NEW: Event flags gap in data stream */
/*! 
@}
*/


/**********************************************************************************
 * This type can help in detecting event type or in allocating storage, but does
 * not differentiate between integer and floating-point versions of data.
 * BUFFER TYPE id's: in "last_data_buffer_type"
**********************************************************************************/
#define ISAMPLE_BUFFER   SAMPLE_TYPE      /* old alias */
#define IEVENT_BUFFER     66
#define IOEVENT_BUFFER     8
#define IMESSAGE_BUFFER  250
#define CONTROL_BUFFER    36
#define ILINKDATA_BUFFER  CONTROL_BUFFER  /* old alias */


/************* CONSTANTS FOR EVENTS ************/



/*! 
@defgroup readdataflags Read Data Flags
@ingroup messaging
The "read" flag contents in IEVENT 
@{
*/

    
    /* time data */
#define READ_ENDTIME 0x0040     /*!< end time (start time always read) */

                /* non-position eye data: */
#define READ_GRES    0x0200     /*!< gaze resolution xy */
#define READ_SIZE    0x0080     /*!< pupil size */
#define READ_VEL     0x0100     /*!< velocity (avg, peak) */
#define READ_STATUS  0x2000     /*!< status (error word) */

#define READ_BEG     0x0001     /*!< event has start data for vel,size,gres */
#define READ_END     0x0002     /*!< event has end data for vel,size,gres */
#define READ_AVG     0x0004     /*!< event has avg pupil size, velocity */

                /* position eye data */
#define READ_PUPILXY 0x0400    /*!< pupilxy REPLACES gaze, href data if read */
#define READ_HREFXY  0x0800
#define READ_GAZEXY  0x1000

#define READ_BEGPOS  0x0008    /*!< position data for these parts of event */
#define READ_ENDPOS  0x0010
#define READ_AVGPOS  0x0020


               /* RAW FILE/LINK CODES: REVERSE IN R/W */
#define FRIGHTEYE_EVENTS  0x8000 /*!< has right eye events */
#define FLEFTEYE_EVENTS   0x4000 /*!< has left eye events */


/*! 
@}
*/

/*! 
@defgroup eventtypeflags Data Type Flags
@ingroup messaging
The "event_types" flag in ILINKDATA or EDF_FILE 
tells what types of events were written by tracker 

@{
*/
    

#define LEFTEYE_EVENTS   0x8000 /*!< has left eye events */
#define RIGHTEYE_EVENTS  0x4000 /*!< has right eye events */
#define BLINK_EVENTS     0x2000 /*!< has blink events */
#define FIXATION_EVENTS  0x1000 /*!< has fixation events */
#define FIXUPDATE_EVENTS 0x0800 /*!< has fixation updates */
#define SACCADE_EVENTS   0x0400 /*!< has saccade events */
#define MESSAGE_EVENTS   0x0200 /*!< has message events */
#define BUTTON_EVENTS    0x0040 /*!< has button events */
#define INPUT_EVENTS     0x0020 /*!< has input port events */

/*! 
@}
*/

/*! 
@defgroup eventdataflags Event Data Flags
@ingroup messaging
The "event_data" flags in ILINKDATA or EDF_FILE tells what 
types of data were included in events by tracker. 

@{
*/
  
#define EVENT_VELOCITY  0x8000  /*!< has velocity data */
#define EVENT_PUPILSIZE 0x4000  /*!< has pupil size data */
#define EVENT_GAZERES   0x2000  /*!< has gaze resolution */
#define EVENT_STATUS    0x1000  /*!< has status flags */

#define EVENT_GAZEXY    0x0400  /*!< has gaze xy position */
#define EVENT_HREFXY    0x0200  /*!< has head-ref xy position */
#define EVENT_PUPILXY   0x0100  /*!< has pupil xy position */

#define FIX_AVG_ONLY    0x0008  /*!< only avg. data to fixation evts */
#define START_TIME_ONLY 0x0004  /*!< only start-time in start events */

#define PARSEDBY_GAZE   0x00C0  /*!< events were generated using GAZE*/
#define PARSEDBY_HREF   0x0080  /*!< events were generated using HREF*/
#define PARSEDBY_PUPIL  0x0040  /*!< events were generated using PUPIL*/

/*!
@}
*/ 

/************ LINK STATE DATA ************/

    /* the data on current state of link read and SIMTSR   */
    /* For EDF file state, see EDFFILE.H for EDF_FILE type */

#ifndef ILINKDATADEF
#define ILINKDATADEF
#define ILINKDATAVERSION 2

#define ELNAMESIZE    40    /*!< max. tracker or remote name size   */
#define ELREMBUFSIZE  420   /*!< max. remote-to-remote message size */
#define ELINKADDRSIZE 16    /*!< Node address (format varies) */

typedef byte ELINKADDR[ELINKADDRSIZE];  /* all-purpose address for link */

/*! \brief Name and address for connection.

Name and address for connection or ping */
typedef struct  {
    ELINKADDR addr; /*!< address of the remote  or local tracker */
    char name[ELNAMESIZE]; /*!< name of the remote  or local tracker */
} ELINKNODE;

/*! \brief Class to represent tracker status.

Class to represent tracker status information such as time stamps, flags, tracker addresses and so on.
*/
typedef struct  {

    UINT32 time;        	/*!< time of last control event */
    UINT32 version;     	/*!< structure version */

    UINT16 samrate;     	/*!< 10*sample rate (0 if no samples, 1 if nonconstant) */
    UINT16 samdiv;      	/*!< sample "divisor" (min msec between samples) */

    UINT16 prescaler;   	/*!< amount to divide gaze x,y,res by */
    UINT16 vprescaler;  	/*!< amount to divide velocity by */
    UINT16 pprescaler;  	/*!< pupil prescale (1 if area, greater if diameter) */
    UINT16 hprescaler;  	/*!< head-distance prescale (to mm) */

    UINT16 sample_data;     /*!< 0 if off, else all flags */
    UINT16 event_data;    	/*!< 0 if off, else all flags */
    UINT16 event_types;     /*!< 0 if off, else event-type flags */

    byte in_sample_block;   /*!< set if in block with samples */
    byte in_event_block;    /*!< set if in block with events */
    byte have_left_eye;     /*!< set if any left-eye data expected */
    byte have_right_eye;    /*!< set if any right-eye data expected */

    UINT16 last_data_gap_types;    	/*!< flags what we lost before last item */
    UINT16 last_data_buffer_type;   /*!< buffer-type code */
    UINT16 last_data_buffer_size;   /*!< buffer size of last item */

    UINT16 control_read;   	/*!< set if control event read with last data */
    UINT16 first_in_block; 	/*!< set if control event started new block */

    UINT32 last_data_item_time;    	/*!< time field of item */
    UINT16 last_data_item_type;     /*!< type: 100=sample, 0=none, else event type */
    UINT16 last_data_item_contents; /*!< content: &lt;read&gt; (IEVENT), &lt;flags&gt; (ISAMPLE) */

    ALL_DATA last_data_item;    	/*!< buffer containing last item */

    UINT32 block_number; 	/*!< block in file */
    UINT32 block_sample;    /*!< samples read in block so far */
    UINT32 block_event;    	/*!< events (excl. control read in block */

               /* RESTORES DROPPED SAMPLE DATA */
    UINT16 last_resx;       /*!< updated by samples only */
    UINT16 last_resy;       /*!< updated by samples only */
    UINT16 last_pupil[2];  	/*!< updated by samples only */
    UINT16 last_status;     /*!< updated by samples, events */

            /* LINK-SPECIFIC DATA */

    UINT16 queued_samples;  /*!< number of items in queue */
    UINT16 queued_events;   /*!< includes control events */

    UINT16 queue_size;      /*!< total queue buffer size */
    UINT16 queue_free;      /*!< unused bytes in queue */

    UINT32 last_rcve_time;  /*!< time tracker last sent packet */

    byte   samples_on;      /*!< data type rcve enable (switch) */
    byte   events_on;

    UINT16 packet_flags;    /*!< status flags from data packet */

    UINT16 link_flags;      /*!< status flags from link packet header */
    UINT16 state_flags;     /*!< tracker error state flags */
    byte   link_dstatus;    /*!< tracker data output state */
    byte   link_pendcmd;    /*!< tracker commands pending  */
    UINT16 reserved;        /*!< 0 for EyeLink I or original EyeLink API DLL.
                            	 EYELINK II ONLY: MSB set if read
                            	 crmode<<8 + file_filter<<4 + link_filter
                            	 crmode = 0 if pupil, else pupil-CR
                            	 file_filter, link_filter: 0, 1, or 2
                            	 for level of heuristic filter applied      */

            /* zero-term. strings: names for connection */
            /* if blank, connects to any tracker    */
    char our_name[40];      /*!< a name for our machine       */
    ELINKADDR our_address;
    char eye_name[40];      /*!< name of tracker connected to */
    ELINKADDR eye_address;

    ELINKADDR ebroadcast_address; /*!< Broadcast address for eye trackers */
    ELINKADDR rbroadcast_address; /*!< Broadcast address for remotes */

    UINT16 polling_remotes; /*!< 1 if polling remotes, else polling trackers */
    UINT16 poll_responses;  /*!< total nodes responding to polling */
    ELINKNODE nodes[4];     /*!< data on nodes */

} ILINKDATA;
#endif

    /* packet_flags: */
#define PUPIL_DIA_FLAG     0x0001  /*!< set if pupil is diameter (else area) */
#define HAVE_SAMPLES_FLAG  0x0002  /*!< set if we have samples */
#define HAVE_EVENTS_FLAG   0x0004  /*!< set if we have events */

#define HAVE_LEFT_FLAG     0x8000  /*!< set if we have left-eye data */
#define HAVE_RIGHT_FLAG    0x4000  /*!< set if we have right-eye data */

    /* dropped events or samples preceding a read item */
    /* are reported using these flag bits in "last_data_gap_types" */
    /* Dropped control events are used to update */
    /* the link state prior to discarding. */
#define DROPPED_SAMPLE  0x8000
#define DROPPED_EVENT   0x4000
#define DROPPED_CONTROL 0x2000

        /* <link_dstatus> FLAGS */
#define DFILE_IS_OPEN    0x80      /*!< disk file active */
#define DFILE_EVENTS_ON  0x40      /*!< disk file writing events */
#define DFILE_SAMPLES_ON 0x20      /*!< disk file writing samples */
#define DLINK_EVENTS_ON  0x08      /*!< link sending events */
#define DLINK_SAMPLES_ON 0x04      /*!< link sending samples */
#define DRECORD_ACTIVE   0x01      /*!< in active recording mode */

        /* <link_flags> flags */
#define COMMAND_FULL_WARN 0x01     /*!< too many commands: pause */
#define MESSAGE_FULL_WARN 0x02     /*!< too many messages: pause */
#define LINK_FULL_WARN    0x04     /*!< link, command, or message load */
#define FULL_WARN         0x0F     /*!< test mask for any warning */

#define LINK_CONNECTED    0x10     /*!< link is connected */
#define LINK_BROADCAST    0x20     /*!< link is broadcasting */
#define LINK_IS_TCPIP     0x40     /*!< link is TCP/IP (else packet) */


/************ STATUS FLAGS (samples and events) ****************/

/*! 
@}
*/

/*! 
@defgroup elmarker Eyelink II/Eyelink I Marker flags
@ingroup messaging
@{
*/
#define LED_TOP_WARNING       0x0080    /*!< marker is in border of image*/
#define LED_BOT_WARNING       0x0040    /*!< marker is in border of image*/
#define LED_LEFT_WARNING      0x0020    /*!< marker is in border of image*/
#define LED_RIGHT_WARNING     0x0010    /*!< marker is in border of image*/
#define HEAD_POSITION_WARNING 0x00F0    /*!< head too far from calibr??? */

#define LED_EXTRA_WARNING     0x0008    /*!< glitch or extra markers */
#define LED_MISSING_WARNING   0x0004    /*!< <2 good data points in last 100 msec)*/
#define HEAD_VELOCITY_WARNING 0x0001    /*!< head moving too fast*/

#define CALIBRATION_AREA_WARNING 0x0002  /*!< pupil out of good mapping area*/

#define MATH_ERROR_WARNING   0x2000  /*!< math error in proc. sample */
/*! 
@}
*/







/*! 
@defgroup elii Eyelink II specific flags
@ingroup messaging
@{
*/

/*!
    This sample interpolated to preserve sample rate
    usually because speed dropped due to missing pupil
 */
#define INTERP_SAMPLE_WARNING 0x1000
/*!
    Pupil interpolated this sample
    usually means pupil loss or
    500 Hz sample with CR but no pupil
*/
#define INTERP_PUPIL_WARNING  0x8000

            /* all CR-related errors */
#define CR_WARNING       0x0F00 /*!< CR related warning for one or both eyes*/ 
#define CR_LEFT_WARNING  0x0500 /*!< CR related warning for left eye*/ 
#define CR_RIGHT_WARNING 0x0A00 /*!< CR related warning for right eye*/ 

            /* CR is actually lost */
#define CR_LOST_WARNING        0x0300 /*!< CR is actually lost for one or both eyes*/ 
#define CR_LOST_LEFT_WARNING   0x0100 /*!< CR is actually lost for left eye*/ 
#define CR_LOST_RIGHT_WARNING  0x0200 /*!< CR is actually lost for right eye*/ 

            /* this sample has interpolated/held CR */
#define CR_RECOV_WARNING       0x0C00 /*!<  this sample has interpolated/held CR for one or both eyes*/ 
#define CR_RECOV_LEFT_WARNING  0x0400 /*!<  this sample has interpolated/held CR for left eye*/ 
#define CR_RECOV_RIGHT_WARNING 0x0800 /*!<  this sample has interpolated/held CR for right eye*/ 


/*! 
@}
*/

/*! 
@defgroup el1000remote Eyelink 1000 Remote specific target status flags
@ingroup messaging
These are available through the HTARGET data type
( htype==0xB4, hdata = tx, ty, distance, flags
where tx, ty range from 0 to 10000
distance = 10*mm

Here is an example of getting target flags over the link.
@remark This example only applicable to EyeLink 1000 Remote



\code
#include <stdio.h>
#include <core_expt.h>

void printHTarget(INT16 hdata[8])
{
	INT16 v = hdata[3];	
	printf("\rTarget x=%d y=%d distance=%d flags=%x", hdata[0],hdata[1],hdata[2],hdata[3]);
	if(v & TFLAG_MISSING)  printf(" TFLAG_MISSING");
	if(v & TFLAG_ANGLE)    printf(" TFLAG_ANGLE");  
	if(v & TFLAG_NEAREYE)  printf(" TFLAG_NEAREYE");
	if(v & TFLAG_CLOSE)    printf(" TFLAG_CLOSE");  
	if(v & TFLAG_FAR)      printf(" TFLAG_FAR");    
	if(v & TFLAG_T_TSIDE)  printf(" TFLAG_T_TSIDE");
	if(v & TFLAG_T_BSIDE)  printf(" TFLAG_T_BSIDE");
	if(v & TFLAG_T_LSIDE)  printf(" TFLAG_T_LSIDE");
	if(v & TFLAG_T_RSIDE)  printf(" TFLAG_T_RSIDE");
	if(v & TFLAG_E_TSIDE)  printf(" TFLAG_E_TSIDE");
	if(v & TFLAG_E_BSIDE)  printf(" TFLAG_E_BSIDE");
	if(v & TFLAG_E_LSIDE)  printf(" TFLAG_E_LSIDE");
	if(v & TFLAG_E_RSIDE)  printf(" TFLAG_E_RSIDE");
}

int main(int argc, char ** argv)
{
    if(open_eyelink_connection(0) !=0) // connect to the tracker
		return 0;
	
	// tell tracker to send HTARGET data
	eyecmd_printf("link_sample_data  = LEFT,RIGHT,GAZE,HREF,HTARGET,GAZERES,AREA,PUPIL,STATUS");

	// start recording
	if(start_recording(0,0,1,1) != 0)
	{
		INT16 eye_used = eyelink_eye_available();  // find which eye is available
		UINT32 st = current_time();
		while(st+20000>current_time()) // record for 20 seconds
		{

			ALLF_DATA f1;
			if(eyelink_newest_float_sample(&f1)>0) // get the newest float sample
			{
				printHTarget(f1.fs.hdata); // print the flags
			}
		}
		stop_recording(); // stop recording
	}
	else
	{
		printf("failed to start recording \n");
	}
	close_eyelink_system(); // close the connection
	return 1;
   
}
\endcode


Example code to access sample status flags.
\code
#include <stdio.h>
#include <core_expt.h>


void printStatus(UINT32 time,UINT16 v)
{
	printf("Status :%d %x\t",time,v);
	if(v & HPOS_TOP_WARNING)      printf("HPOS_TOP_WARNING ");
	if(v & HPOS_BOT_WARNING )     printf("HPOS_BOT_WARNING ");
	if(v & HPOS_LEFT_WARNING)     printf("HPOS_LEFT_WARNING ");
	if(v & HPOS_RIGHT_WARNING)    printf("HPOS_RIGHT_WARNING ");
	if(v & HPOS_WARNING)		  printf("HPOS_WARNING ");
	if(v & HPOS_ANGLE_WARNING  )  printf("HPOS_ANGLE_WARNING ");
	if(v & HPOS_MISSING_WARNING ) printf("HPOS_MISSING_WARNING ");
	if(v & HPOS_DISTANCE_WARNING )printf("HPOS_DISTANCE_WARNING ");

	printf("\n");
}



int main(int argc, char ** argv)
{
    if(open_eyelink_connection(0) !=0) // connect to the tracker
		return 0;
	
	// tell tracker to send HTARGET data
	eyecmd_printf("link_sample_data  = LEFT,RIGHT,GAZE,HREF,HTARGET,GAZERES,AREA,PUPIL,STATUS");

	// start recording
	if(start_recording(0,0,1,1) != 0)
	{
		INT16 eye_used = eyelink_eye_available();  // find which eye is available
		UINT32 st = current_time();
		while(st+20000>current_time()) // record for 20 seconds
		{

			ALLF_DATA f1;
			if(eyelink_newest_float_sample(&f1)>0) // get the newest float sample
			{
				printStatus(f1.fs.time,f1.fs.status); // print the flags
			}
		}
		stop_recording(); // stop recording
	}
	else
	{
		printf("failed to start recording \n");
	}
	close_eyelink_system(); // close the connection
	return 1;
   
}
\endcode


@{
*/

// SAMPLE STATUS BITS:
// These are for eye or target at edges
//
#define HPOS_TOP_WARNING       0x0080    /*!<  Marker is in top border of image*/ 
#define HPOS_BOT_WARNING       0x0040    /*!<  Marker is in bottom border of image*/ 
#define HPOS_LEFT_WARNING      0x0020    /*!<  Marker is in left border of image*/ 
#define HPOS_RIGHT_WARNING     0x0010    /*!<  Marker is in right border of image*/ 
#define HPOS_WARNING  0x00F0             /*!<  Head too far from calibration */
//
// These flag target conditions:
//                                //
#define HPOS_ANGLE_WARNING     0x0008   /*!< Target at too great an angle for accuracy*/
#define HPOS_MISSING_WARNING   0x0004   /*!< Target is missing*/
#define HPOS_DISTANCE_WARNING  0x0001   /*!< Target too close or too far*/




//
//  TARGET WARNINGS
#define TFLAG_MISSING   0x4000    /*!< Target missing*/
#define TFLAG_ANGLE     0x2000    /*!< Extreme target angle*/
#define TFLAG_NEAREYE   0x1000    /*!< Target near eye so windows overlapping*/
//  DISTANCE WARNINGS (limits set by remote_distance_warn_range command)
#define TFLAG_CLOSE     0x0800    /*!< distance vs. limits*/
#define TFLAG_FAR       0x0400
// TARGET TO CAMERA EDGE  (margin set by remote_edge_warn_pixels command)
#define TFLAG_T_TSIDE   0x0080    /*!< Target near top edge of image */
#define TFLAG_T_BSIDE   0x0040    /*!< Target near bottom edge of image */
#define TFLAG_T_LSIDE   0x0020    /*!< Target near left edge of image */
#define TFLAG_T_RSIDE   0x0010    /*!< Target near right edge of image */
// EYE TO CAMERA EDGE  (margin set by remote_edge_warn_pixels command)
#define TFLAG_E_TSIDE   0x0008    /*!< Eye near top edge of image */
#define TFLAG_E_BSIDE   0x0004    /*!< Eye near bottom edge of image */
#define TFLAG_E_LSIDE   0x0002    /*!< Eye near left edge of image */
#define TFLAG_E_RSIDE   0x0001    /*!< Eye near right edge of image */
/*!
@}
*/




#ifdef __cplusplus    /* For C++ compilation */
}
#endif

#endif /* SIMTYPESINCL */
#endif /*__SRRESEARCH__EYE_DATA_H__ */
