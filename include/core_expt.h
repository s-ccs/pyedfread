/**********************************************************************************
 * EYELINK PORTABLE EXPT SUPPORT      (c) 1996, 2003 by SR Research               *
 *     8 June '97 by Dave Stampe       For non-commercial use only                *
 * Greately modified by Suganthan Subramaniam, 11 March 2003                      *
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
	\file core_expt.h
        \brief Declarations of eyelink_core functions and types.

        This file will also reference the other EyeLink header files.
 */

#ifndef __SRRESEARCH__EXPT_SPPT_H__
#define __SRRESEARCH__EXPT_SPPT_H__
#ifdef __cplusplus     /* For C++ compilers */
extern "C" {
#endif
#ifndef BYTEDEF
  #include "eyetypes.h"
  #include "eyelink.h"
#endif




/***************************************KEY SCANNING*******************************
 * some useful keys returned by getkey()                                          *
 * These keys allow remote control of tracker during setup.                       *
 * on non-DOS platforms, you should produce these codes and                       *
 * all printable (0x20..0x7F) keys codes as well.                                 *
 * Return JUNK_KEY (not 0) if untranslatable key pressed.                         *
 * TERMINATE_KEY can be to break out of EXPTSPPT loops.                           *
 **********************************************************************************/
/*! 
@addtogroup keyinput
@{
*/
#define CURS_UP    0x4800    /*!< Cursor up key.*/
#define CURS_DOWN  0x5000    /*!< Cursor down key.*/
#define CURS_LEFT  0x4B00    /*!< Cursor left key.*/
#define CURS_RIGHT 0x4D00    /*!< Cursor right key.*/

#define ESC_KEY   0x001B     /*!< Escape key.*/
#define ENTER_KEY 0x000D     /*!< Return key.*/

#define PAGE_UP   0x4900     /*!< Page up key.*/
#define PAGE_DOWN 0x5100     /*!< Page down key.*/
#define JUNK_KEY      1      /*!< Junk key to indicate untranslatable key.*/ 
#define TERMINATE_KEY 0x7FFF /*!< Returned by getkey if program should exit.*/ 
/*! 
@}
*/

/*!
	\brief This structure holds information on the display.

	This structure holds information on the display
  	Call \c get_display_infomation() to fill this with data
  	Check mode before running experiment!
 */
#ifndef DISPLAYINFODEF
  #define DISPLAYINFODEF
  typedef struct
  {
   INT32 left;   /*!< left of display */
   INT32 top;    /*!< top of display */
   INT32 right;  /*!< right of display*/
   INT32 bottom; /*!< bottom of display */
   INT32 width;  /*!< width of display */
   INT32 height; /*!< height of display */
   INT32 bits;   /*!< bits per pixel */
   INT32 palsize;/*!< total entries in palette (\c 0 if not indexed display mode) */
   INT32 palrsvd;/*!< number of static entries in palette
                    ( \c 0 if not indexed display mode)*/
   INT32 pages;  /*!< pages supported */
   float refresh;/*!< refresh rate in Hz */
   INT32 winnt;  /*!< \c 0 if Windows 95, \c 1 if Windows NT */
  } DISPLAYINFO;
#endif


/************ SYSTEM SETUP *************/

/*! @ingroup init_eyelink
	Initializes the EyeLink library, and opens a connection to the EyeLink tracker.

 	@remarks By setting \c &lt;mode&gt; to be \c 1, the connection can be simulated for debugging
 				purposes.  Only timing operations and simple tests should be done in simulation
 				mode, and the Windows TCP/IP system must be installed.  This function is
 				intended for networks where a single tracker is connected to the network.
 	@param mode Mode of connection:
 					\arg \c 0, opens a connection with the eye tracker;
 					\arg \c 1, will create a dummy connection for simulation;
 					\arg \c -1, initializes the DLL but does not open a connection.
	@return \c 0 if success, else error code

	\b Example:
	\code
	// This program illustrates the use of open_eyelink_connection()

	#include <eyelink.h>

	// Sets the EyeLink host address, if tracker IP address is different
	// from the default "100.1.1.1"
	if (set_eyelink_address("100.1.1.7"))
	    return -1;

	//Initializes EyeLink library, and opens connection to the tracker
	if(open_eyelink_connection(0))
	    return -1;

	...
	// Code for the setup, recording, and cleanups
	close_eyelink_connection();      // disconnect from tracker
	\endcode

	\sa \c close_eyelink_connection(), \c close_eyelink_system(), \c eyelink_broadcast_open(),
		\c eyelink_dummy_open(), \c eyelink_open() and \c eyelink_open_node()
 */
INT16 ELCALLTYPE open_eyelink_connection(INT16 mode);

/*! @ingroup init_eyelink
	Closes any connection to the eye tracker, and closes the link.

 	@remarks NEW (v2.1):  Broadcast connections can be closed, but not to affect the eye tracker. If
 				a non-broadcast (primary) connection is closed, all broadcast connections to the
 				tracker are also closed.

	\b Example: See \c open_eyelink_connection()

	\sa \c open_eyelink_connection() and \c eyelink_close()
 */
void ELCALLTYPE close_eyelink_connection(void);

/*! @ingroup init_eyelink
	Sets the IP address used for connection to the EyeLink tracker.  This is set to "100.1.1.1" in the DLL,
	but may need to be changed for some network configurations.  This must be set before attempting to open
	a connection to the tracker.

	@remarks A "broadcast" address ("255.255.255.255") may be used if the tracker address is not known -
				this will work only if a single Ethernet card is installed, or if DLL version 2.1 or higher,
				and the latest tracker software versions (EyeLink I v2.1 or higher, and EyeLink II v1.1 or
				higher) are installed.
	@param addr Pointer to a string containing a "dotted" 4-digit IP address.
	@return \c 0 if success, \c -1 if could not parse address string.

	\b Example:
	\code
	// This program illustrates the use of set_eyelink_address()

	#include <eyelink.h>

	// Initialize the EyeLink DLL
	if(open_eyelink_connection(-1))
	    return -1;

	// Set the address of the tracker
	set_eyelink_address("100.1.1.1");

	// connect to single Eyelink tracker
	if (eyelink_open())
	    return -1;
	\endcode

	\sa \c eyelink_open(), \c eyelink_open_node(), \c text_to_elinkaddr()
 */
INT16 ELCALLTYPE set_eyelink_address(char *addr);

/*! @ingroup priority
	Changes the multitasking proirity of current application
	Using THREAD_PRIORITY_ABOVE_NORMAL may reduce latency
 	Reset priority with THREAD_PRIORITY_NORMAL
 	Too high priority will stop the link from functioning!
	@deprecated  This function is depricated. It is left for compatibility reasons.
	The implementation does nothing.
 */
INT32 ELCALLTYPE set_application_priority(INT32 priority);


/************* SET CALIBRATION WINDOW ***********/





/************* MESSAGE PUMP ************/

/*******************************************************************************
 * allows messages to operate in loops
 * returns nonzero if app terminated
 * eats key events, places on key queue for getkey()
 * getkey() and echo_key() also call this function
 *
 * calling this in loops allows Windows to process messages
 * returns nonzero if application terminated (ALT-F4 sent to window)
 * set <dialog_hook> to handle of modeless dialog
 * in order to properly handle its messages as well
 *******************************************************************************/
/*! @ingroup pump
	Almost all experiments must run in a deterministic fashion, executing sequentially and in
	loops instead of the traditional Windows event-processing model.  However, Windows messages
	must still be dispatched for keyboard input and other events.  Calling \c getkey() will dispatch
	messages and return keys.  The \c message_pump() function also dispatches messages, but does
	not read the keys.  It can also handle messages for a modeless dialog box.

	@return \c 0 normally, \c 1 if ALT-F4 or CTRL-C pressed, or if \c terminal_break() called.  Any loops
			should exit in this case.

	\b Example:
	The following two programs works together to show the use of \c message_pump() function. In this
	case, writes a message to EDF file when the left mouse button is pressed.

	\code
	// Program 1.  The typical trial loop plus the message_pump()
	#include <eyelink.h>

	// Trial loop: till timeout or response
	while(1)
	{
	    ...
	    // Other code for display update, trial terminating, etc.

	    // Allows messages to operate in loops
	    message_pump(NULL);
	}
	\endcode

	\code
	// Program 2.  Revised code in the full_screen_window_proc() function of the w32_demo_window.c
	// module.  In this case, a WM_LBUTTONDOWN message is recorded in the EDF file.

	#include <eyelink.h>
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_CHAR:
	    // Processes key messages: these can be accessed by getkey()
	    process_key_messages(hwnd, message, wparam, lparam);
	    break;

	case WM_LBUTTONDOWN:
	    eyemsg_printf("Left button is down");
	    break;
	    ...
	    // Other windows messages and events
	}
	\endcode

	\b Output:
	\code
	MSG	11661891 SYNCTIME 1
	MSG	11662745 left button is down
	MSG	11663048 left button is down
	BUTTON	11665520	4	1
	MSG	11665521 ENDBUTTON 4
	\endcode

	\sa \c pump_delay()
 */
INT16 ELCALLTYPE message_pump();

/*! @ingroup pump
	Similar to \c message_pump(), but only processes keypresses.
 	This may help reduce latency.
 */
INT16 ELCALLTYPE key_message_pump(void);


/*******************************************************************************
 * Similar to msec_delay(), but allows Widows message process
 * only allows message processing if delay > 20 msec
 * does not process dialog box messages
 *******************************************************************************/
/*! @ingroup pump
	During calls to \c msec_delay(), Windows is not able to handle messages.  One result of this is
	that windows may not appear.  This is the preferred delay function when accurate timing is
	not needed.  It calls \c message_pump() until the last 20 milliseconds of the delay, allowing
	Windows to function properly.  In rare cases, the delay may be longer than expected.  It does
	not process modeless dialog box messages.

	@param delay Number of milliseconds to delay.

	\b Example:
	\code
	// This program illustrates the use of pump_delay() at the end of trial
	#include <eyelink.h>

	// End recording: adds 100 msec of data to catch final events
	static void end_trial(void)
	{
	    ...
	    // Add code here to clean the display

	    // Ensure we release realtime lock
	    end_realtime_mode();
	    // Delay for 100 msec, allow Windows to clean up
	    pump_delay(100);

	    // halt recording, return when tracker finished mode switch
	    stop_recording();
	}
	\endcode

	\sa \c msec_delay() and \c message_pump()
 */
void ELCALLTYPE pump_delay(UINT32 delay);



/*********** KEYBOARD SUPPORT **********/

/*******************************************************************************
 * Initializes and empties local key queue
 *******************************************************************************/
/*! @ingroup keyinput
	Initializes the key queue used by \c getkey().  This should be called at the start of your
	program.  It may be called at any time to get rid any of old keys from the queue.

	\b Example:
	\code
	// This program illustrates the use of flush_getkey_queue()

	#include <eyelink.h>

	// Sets the EyeLink host address, if tracker IP address is different
	// from the default "100.1.1.1"
	if (set_eyelink_address("100.1.1.7"))
	    return -1;

	// Initializes EyeLink library, and opens connection to the tracker
	if(open_eyelink_connection(0))
	    return -1;

	flush_getkey_queue();   // initialize getkey() system

	...
	// Code for the setup, recording, and cleanups
	close_eyelink_connection();      // disconnect from tracker
	\endcode

	\sa \c read_getkey_queue()
 */
void ELCALLTYPE flush_getkey_queue(void);

/*******************************************************************************
 * Similar to getkey(), but doesnt call message pump
 * Use to build message pump for your own window
 *******************************************************************************/
/*! @ingroup keyinput
	Reads keys from the key queue.  It is similar to \c getkey(), but does not process Windows
	messages.  This can be used to build key-message handlers in languages other than C.

	@remarks These functions are intended to support languages other than C or C++.
	@return \c 0 if no key pressed.\n
			<tt> JUNK_KEY (1)</tt> if untranslatable key.\n
			<tt>TERMINATE_KEY (0x7FFF)</tt> if CTRL-C is pressed, \c terminal_break() was called,  or
			the program has been terminated with ALT-F4.\n
			else, code of key if any key pressed.

	\sa \c flush_getkey_queue()
 */
UINT16 ELCALLTYPE read_getkey_queue(void);





/*******************************************************************************
 * Calls getkey(), also sends keys to tracker for remote control               *
 * User implementation allows filtering of keys before sending                 *
 * returns same codes as getkey()                                              *
 *******************************************************************************/
/*! @ingroup keyinput
	Checks for Windows keystroke events and dispatches messages; similar to \c getkey(), but
	also sends keystroke to tracker.

	@remarks Warning:  Under Windows XP, this call will not work in realtime mode at all,
				and will take several seconds to respond if graphics are being drawn
				continuously.  This function works well in realtime mode under Windows 2000.
	@return \c 0 if no key pressed, else key code.\n
			\c TERMINATE_KEY if CTRL-C held down or program has been terminated.

	\b Example:
	\code
	// This program illustrates the use of echo_key(), if you want to write your own code for do_tracker_setup routine
	#include <eyelink.h>

	// Resets keys and buttons from tracker
	eyelink_flush_keybutton();
	// dump any accumulated key presses
	while(getkey());

	// If we make sure that we are in set-up mode
	while(eyelink_current_mode() & IN_SETUP_MODE)
	{
	    int i = eyelink_current_mode();

	    // calibrate, validate, etc: show targets
	    if(i & IN_TARGET_MODE)
	    {
	        ...
	        // Code here for displaying and updating calibration target
	        // If using Windows library, call target_mode_display();
	    }
	    else if(i & IN_IMAGE_MODE)
	    {
	        ...
	        // Code here for showing the camera image
	        // If using Windows library, call target_mode_display();
	    }

	    // If we allow local tracker control, echo to tracker for remote control
	    echo_key();
	}
	\endcode

	\sa \c eyelink_read_keybutton(), \c eyelink_send_keybutton() and \c getkey()
 */
UINT16 ELCALLTYPE echo_key(void);

/*******************************************************************************
 * EYELINK tracker (MS-DOS) key scan equivalent
 * Processes Windows messages, records key events
 * Returns 0 if no key pressed
 * returns 1-255 for non-extended keys
 * returns 0x##00 for extended keys (##=hex code)
 *******************************************************************************/
/*! @ingroup keyinput
	A routine for checking for Windows keystroke events, and dispatching Windows messages.
	If no key is pressed or waiting, it returns \c 0.  For a standard ASCII key, a value from
	\c 31 to \c 127 is returned.  For extended keys, a special key value is returned.  If the
	program has been terminated by ALT-F4 or a call to \c terminal_break(), or the "Ctrl" and
	"C" keys are held down, the value \c TERMINATE_KEY is returned.  The value <tt>JUNK_KEY (1)</tt>
	is returned if a non-translatable key is pressed.
 	@remarks Warning: This function processes and dispatches any waiting messages.  This
 				will allow Windows to perform disk access and negates the purpose of
 				realtime mode.  Usually these delays will be only a few milliseconds, but
 				delays over 20 milliseconds have been observed.  You may wish to call
 				\c escape_pressed() or \c break_pressed() in recording loops instead of \c getkey()
 				if timing is critical, for example in a gaze-contingent display.  Under
 				Windows XP, these calls will not work in realtime mode at all (although
 				these do work under Windows 2000).  Under Windows 95/98/Me, realtime
 				performance is impossible even with this strategy.\n
				Some useful keys are defined in core_expt.h, as:\n
				\arg \c CURS_UP    		\c 0x4800
				\arg \c CURS_DOWN  		\c 0x5000
				\arg \c CURS_LEFT  		\c 0x4B00
				\arg \c CURS_RIGHT 		\c 0x4D00
				\arg \c ESC_KEY   		\c 0x001B
				\arg \c ENTER_KEY 		\c 0x000D
				\arg \c TERMINATE_KEY 	\c 0x7FFF
				\arg \c JUNK_KEY 		\c 0x0001
	@return \c 0 if no key pressed, else key code.\n
 			\c TERMINATE_KEY if CTRL-C held down or program has been terminated.

	\b Example:
	\code
	// This program illustrates the use of getkey()

	#include <eyelink.h>
	UINT32  delay_time = 5000L;  // Set the maximum wait time

	// flushes any waiting keys or buttons
	eyelink_flush_keybuttons(0);

	delay_time += current_msec();
	while(1)
	{
	    // Waitkey times out
	    if(current_time() > delay_time)
	    {
	        eyemsg_printf("WAITKEY TIMEOUT");
	        break;
	    }
	    key = getkey();
	    if(key) // If key press occurs
	    {
	        // Is this printable key?
	        if(key < 256 && isprint(key))
	            eyemsg_printf("WAITKEY '%c'", key);
	        else
	            eyemsg_printf("WAITKEY 0x%04X", key);
	        break;
	    }
	}
	\endcode

	\sa \c break_pressed(), \c echo_key(), \c escape_pressed(), \c eyelink_flush_keybuttons()
		and \c eyelink_send_keybutton()
 */
UINT16 ELCALLTYPE getkey(void);


/*!@ingroup keyinput
Same as getkey except it returns the modifier and the key pressed.
It returns a 32 bit unsigned integer. The first 16 bits are reserved for the modifier and the
last 16 bits are reserved for the key values. If there are no modifiers present,
the return value of this is the same as getkey().
If non null pointer passed in for unicode, the translated key value will be set if a key is preent.
*/
UINT32 ELCALLTYPE getkey_with_mod(UINT16 *unicode) ;



/**************************** LINK FORMATTED COMMANDS **************************/

/*******************************************************************************
 * link command formatting                                                     *
 * use just like printf()                                                      *
 * returns command result                                                      *
 * allows 500 msec. for command to finish                                      *
 *******************************************************************************/
/*! @ingroup messaging
	The EyeLink tracker accepts text commands through the link.  These commands may be used
	to configure the system, open data files, and so on.

	@remarks The function waits up to 500 msec. for a success or failure code to be returned
				from the tracker, then returns the error code \c NO_REPLY.  If you need more
				time, use \c eyelink_timed_command() instead.
	@param fmt Similar to printf(), format string plus arguments.
	@return \c 0 if successfully executed, else error code.

	\b Example:
	\code
	// This program illustrates the use of eyecmd_printf()

	#include <eyelink.h>

	UINT32 t = current_msec();
	int i, j;

	// Draws a box on the tracker screen
	eyecmd_printf("draw_box %d %d %d %d  7", 100, 100, 300, 400);

	// Waits for a maximum of 1000 msec
	while(current_msec()-t < 1000)
	{
	    // Checks for result from command execution
	    i = eyelink_command_result();
	    // Used to get more information on tracker result
	    j = eyelink_last_message(buf);

	    if (i == OK_RESULT)
	    {
	        eyemsg_printf("Command executed successfully: %s", j?buf:"");
	        break;
	    }
	    else if (i!=NO_REPLY)
	    {
	        eyemsg_printf("Error in executing command: %s", j?buf:"");
	        break;
	    }
	}
	\endcode

	\b Output:
	\code
	MSG	5312110 Command executed successfully: OK
	\endcode

	\sa \c eyemsg_printf(), \c eyelink_send_command() and \c eyelink_timed_command()

 */
INT16 ELCALLTYPE eyecmd_printf(const char *fmt, ...);

/*******************************************************************************
 * link message formatting                                                     *
 * use just like printf()                                                      *
 * returns any send error                                                      *
 *******************************************************************************/
/*! @ingroup messaging
	This sends a text message to the EyeLink tracker, which timestamps it and writes it to
	the EDF data file.  Messages are useful for recording trial conditions, subject responses,
	or the time of important events.  This function is used with the same formatting methods
	as printf(), allowing numbers to be included.  Avoid end-of-line characters ("\n") at
	end of messages.

	@param fmt Similar to printf(), format string plus arguments.
	@return \c 0 if successfully sent to tracker, else error code.

	\b Example:
	\code
	// This program illustrates the use of eyemsg_printf()

	#include <eyelink.h>
	char program_name[100] = "Windows Sample Experiment 2.0";

	// Add a message to the EDF file
	eyemsg_printf("RECORDED BY %s", program_name);
	\endcode

	\b Output:
	\code
	MSG	2248248 RECORDED BY Windows Sample Experiment 2.0
	\endcode

	\sa \c eyecmd_printf()
 */
INT16 ELCALLTYPE eyemsg_printf(const char *fmt, ...);

/*! @ingroup messaging
	This allows us to send messages to the Eyelink tracker to be logged into
	the data file with a time offset.  Use it just like printf() to format the message text.
	@param exectime time offset that reflects in the message's time stamp
	@param fmt printf format string
	@return \c 0 if OK, else error code.

	example:
	@code

	if(open_eyelink_connection(0) !=0)
		return 0;
	open_data_file("msgtest.edf");

	eyemsg_printf("hello");
	msec_delay(100);
	eyemsg_printf_ex(-100,"hello1");
	msec_delay(100);

	eyemsg_printf_ex(100,"hello2");
	msec_delay(100);
	eyemsg_printf("hello3");
	msec_delay(100);
	close_data_file();
	receive_data_file("",".",1);

	@endcode

    As you can see in the edf file data generated by the above code, both
	Message1 and Message2 has the same time stamp and message3 and message4 has the
	same time stamp.
	@code
	MSG     8004932 Message1
	MSG     8004932 Message2
	MSG     8005232 Message3
	MSG     8005232 Message4
	@endcode
 */
INT16 ELCALLTYPE eyemsg_printf_ex(UINT32 exectime, const char *fmt, ...);




/***************************** RECORDING SUPPORT FUNCTIONS *********************
 * RETURN THESE CODES FROM YOUR RECORDING FUNCTIONS                            *
 * These codes are returned by all these functions                             *
 *******************************************************************************/
/*! 
@addtogroup recording
@{
*/
#define DONE_TRIAL       0  /*!< return codes for trial result */
#define TRIAL_OK         0  /*!< return codes for trial result */
#define REPEAT_TRIAL     1  /*!< return codes for trial result */
#define SKIP_TRIAL       2  /*!< return codes for trial result */
#define ABORT_EXPT       3  /*!< return codes for trial result */
#define TRIAL_ERROR     -1  /*!< Bad trial: no data, etc. */
/*! 
@}
*/


/*******************************************************************************
 * Start recording with data types requested                                   *
 * Check that all requested link data is arriving                              *
 * return 0 if OK, else trial exit code                                        *
 *******************************************************************************/
/*! @ingroup recording
	Starts the EyeLink tracker recording, sets up link for data reception if enabled.

 	@remarks Recording may take 10 to 30 milliseconds to begin from this command.  The function
 				also waits until at least one of all requested link data types have been received.
 				If the return value is not zero, return the result as the trial result code.
 	@param file_samples If \c 1, writes samples to EDF file.  If \c 0, disables sample recording.
	@param file_events If \c 1, writes events to EDF file.  If \c 0, disables event recording.
	@param link_samples If \c 1, sends samples through link.  If \c 0, disables link sample access.
	@param link_events If \c 1, sends events through link.  If \c 0, disables link event access.
	@return \c 0 if successful, else trial return code.

	\b Example:
	\code
	// This program illustrates the use of start_recording(), stop_recording(), checking_recording(),
	// and check_record_exit()

	#include <eyelink.h>

	// Starts data recording to EDF file
	// Records samples and events to EDF file only in this example
	// Returns error code if failed
	error = start_recording(1,1,0,0);
	if(error != 0)  return error;

	// Sets up for realtime execution
	begin_realtime_mode(100);

	...
	// Display drawing code here

	// Trial loop
	while(1)
	{
	    // Checks if recording aborted
	    if((error=check_recording())!=0) return error;
	    ...
	    // Other code for display updates, timing, key or button
	    // response handling
	}

	// Ensures we release realtime lock
	end_realtime_mode();
	// Records additional 100 msec of data
	pump_delay(100);
	// halt recording, return when tracker finished mode switch
	stop_recording();

	while(getkey());          // dump any accumulated key presses

	// Call this at the end of the trial, to handle special conditions
	return check_record_exit();
	\endcode

	\sa \c check_record_exit(), \c check_recording(), \c eyelink_data_start() and \c stop_recording()
 */
INT16 ELCALLTYPE start_recording(INT16 file_samples, INT16 file_events,
                         INT16 link_samples, INT16 link_events);

/*******************************************************************************
 * Check if we are recording: if not, report an error                          *
 * Also calls record_abort_hide() if recording aborted                         *
 * Returns 0 if recording in progress                                          *
 * Returns ABORT_EXPT if link disconnected                                     *
 * Handles recors abort menu if trial interrupted                              *
 * Returns TRIAL_ERROR if other non-recording state                            *
 * Typical use is                                                              *
 *   if((error=check_recording())!=0) return error;                            *
 *******************************************************************************/
/*! @ingroup recording
	Check if we are recording: if not, report an error.  Call this function while recording.
	It will return \c 0 if recording is still in progress, or an error code if not.  It will
	also handle the EyeLink Abort menu by calling \c record_abort_handler().  Any errors returned
	by this function should be returned by the trial function.  On error, this will disable
	realtime mode and restore the heuristic.

 	@return <tt>TRIAL_OK (0)</tt> if no error.\n
			<tt>REPEAT_TRIAL, SKIP_TRIAL, ABORT_EXPT, TRIAL_ERROR</tt> if recording aborted.

	\b Example: See \c start_recording()

	\sa \c check_record_exit(), \c eyelink_abort(), \c start_recording() and \c stop_recording()
 */
INT16 ELCALLTYPE check_recording(void);

/*******************************************************************************
 * halt recording, return when tracker finished mode switch
 *******************************************************************************/
/*! @ingroup recording
	Stops recording, resets EyeLink data mode.

 	@remarks Call 50 to 100 msec after an event occurs that ends the trial.  This function
 				waits for mode switch before returning.

	\b Example: See \c start_recording()

	\sa \c eyelink_data_stop(), \c set_offline_mode() and \c start_recording()
 */
void ELCALLTYPE stop_recording(void);

/*******************************************************************************
 * enter tracker idle mode, wait  till finished mode switch                    *
 *******************************************************************************/
/*! @ingroup eyelink_mode
	Places EyeLink tracker in off-line (idle) mode.  Wait till the tracker has finished the mode transition.
.

	\b Example:
	\code
	// This program illustrates the use of set_offline_mode() function when doing cleaning up
	// at the end of data recoridng

	// Checks whether we still have the connection to the tracker
	if(eyelink_is_connected())
	{
	    // Places EyeLink tracker in off-line (idle) mode
	    set_offline_mode();
	    eyecmd_printf("close_data_file");    // close data file
	    eyelink_close(1);         // disconnect from tracker
	}

	// shut down system (MUST do before exiting)
	close_eyelink_system();
	\endcode

	\sa \c eyelink_abort()
 */
void ELCALLTYPE set_offline_mode(void);

/*******************************************************************************
 * call at end of trial, return result                                         *
 * check if we are in Abort menu after recording stopped                       *
 * returns trial exit code                                                     *
 *******************************************************************************/
/*! @ingroup recording
	Checks if we are in Abort menu after recording stopped and returns trial exit code. Call this
	function on leaving a trial.  It checks if the EyeLink tracker is displaying the Abort menu, and
	handles it if required.  The return value from this function should be returned as the trial
	result code.

	@return \c TRIAL_OK if no error.\n
			<tt>REPEAT_TRIAL, SKIP_TRIAL, ABORT_EXPT</tt> if Abort menu activated.

	\b Example: See \c start_recording()

	\sa \c check_recording(), \c eyelink_abort(), \c start_recording() and \c stop_recording()
 */
INT16 ELCALLTYPE check_record_exit(void);


/*******************************************************************************
 *CALIBRATION, DRIFT CORRECTION CONTROL
 *******************************************************************************/
/*******************************************************************************
 * Call this to stop calibration/drift correction in progress
 * This could be called from a Windows message handler
 *******************************************************************************/
/*! @ingroup setup_eyelink
	This function should be called from an message or event handler if an ongoing call to
	\c do_drift_correct() or \c do_tracker_setup() should return immediately.

 	\b Example:
	\code
	// The following code illustrates the use of exit_calibration().  This would usually be
	// called from a message or event handler (see the w32_demo_window.c module) for an example.
	#include <eyelink.h>
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_CHAR:
	    {
	        // Process messages, translates key messages and queue
	        UINT16 key = process_key_messages(hwnd, message, wparam, lparam);
	        eyemsg_printf("key %d", key);

	        // Checks the current tracker state.  If it is in setup mode, pressing the PAGE_DOWN
	        // key would abort the setup process (i.e., drift-calibration, validation, coreection)
	        if (key == 0x5100 && (eyelink_current_mode() & IN_SETUP_MODE))
	            exit_calibration();
	        break;
	    }
	    ...
	    // Other windows messages and events
	}
	\endcode

	\sa \c do_tracker_setup(), \c do_drift_correct() and \c eyelink_current_mode()
 */
void ELCALLTYPE exit_calibration(void);


/*******************************************************************************
 * Starts tracker into Setup Menu.                                             *
 * From this the operator can do camera setup, calibrations, etc.              *
 * Pressing ESC on the tracker exits.                                          *
 * Leaving the setup menu on the tracker (ESC) key) also exits.                *
 * RETURNS: 0 if OK, 27 if aborted, TERMINATE_KEY if pressed                   *
 *******************************************************************************/
/*! @ingroup setup_eyelink
	Switches the EyeLink tracker to the Setup menu, from which camera setup, calibration,
	validation, drift correction, and configuration may be performed. Pressing the 'ESC' key on
	the tracker keyboard will exit the Setup menu and return from this function.  Calling
	\c exit_calibration() from an event handler will cause any call to \c do_tracker_setup() in
	progress to return immediately.

	@return Always \c 0.

	\b Example:
	\code
	// This program shows an example of using the do_tracker_setup()
	#include <eyelink.h>

	// Colors used for drawing calibration target and background
	COLORREF target_foreground_color = RGB(0,0,0);
	COLORREF target_background_color = RGB(255,255,255);
	int i = SCRWIDTH/60;     // Selects best size for calibration target
	int j = SCRWIDTH/300;    // Selects size for focal spot in target
	if(j < 2) j = 2;

	// Sets diameter of target and of hole in middle of target
	set_target_size(i, j);

	// Sets target color and display background color
	set_calibration_colors(target_foreground_color, target_background_color);

	// Sets sounds for Setup menu (calibration, validation)
	set_cal_sounds("", "", "");
	// Sets sounds for drift correction
	set_dcorr_sounds("", "off", "off");

	// Performs camera setup, calibration, validation, etc.
	do_tracker_setup();

	...
	// Code for running the trials
	\endcode

	\sa \c do_drift_correct(), \c set_cal_sounds(), \c set_calibration_colors() and \c set_target_size()
 */
INT16 ELCALLTYPE do_tracker_setup(void);


/*******************************************************************************
 *PERFORM DRIFT CORRECTION ON TRACKER
 *******************************************************************************/
/*******************************************************************************
 * Performs a drift correction, with target at (x,y).                          *
 * If operator aborts with ESC, we assume there's a setup                      *
 * problem and go to the setup menu (which may clear the                       *
 * display).  Redraw display if needed and repeat the                          *
 * call to  do_drift_correct() in this case.                                   *
 * ARGS: x, y: position of target                                              *
 *       draw: draws target if 1, 0 if you draw target first                   *
 *       allow_setup: 0 disables ESC key setup mode entry                      *
 * RETURNS: 0 if OK, 27 if Setup was called, TERMINATE_KEY if pressed          *
 *******************************************************************************/
/*! @ingroup setup_eyelink
	Performs a drift correction before a trial.

	@remarks When the 'ESC' key is pressed during drift correction, \c &lt;allow_setup&gt; determines
				the result.  If \c 1, the EyeLink Setup menu is accessed. This will always clear
				the display, so redrawing of hidden stimuli may be required.  Otherwise, the drift
				correction is aborted. Calling \c exit_calibration() from an event handler will cause
				any call to \c do_drift_correct() in progress to return immediately.  In all cases,
				the return code will be \c 27 (\c ESC_KEY).
	@param x X Position of drift correction target.
	@param y Y Position of drift correction target.
	@param draw If \c 1, the drift correction will clear the screen to the target background color,
				draw the target, and clear the screen again when the drift correction is done.
				If \c 0, the fixation target must be drawn by the user.
	@param allow_setup If \c 1, accesses Setup menu before returning, else aborts drift correction.
	@return \c 0 if successful, \c 27 if 'ESC' key was pressed to enter Setup menu or abort.

	\b Example:
	\code
	// This program illustrates the use of do_drift_correction with drift correction target drawn by the user

	#include <eyelink.h>

	int target_shown = 0;
	int draw_own_target = 1;

	while(1)
	{
	    // Checks link often so we can exit if tracker stopped
	    if(!eyelink_is_connected())
	        return ABORT_EXPT;

	    // If drift correct target is not drawn, we have to draw it here
	    if (draw_own_target && !target_shown)
	    {
	        // Code for drawing own drift correction target
	        target_shown = 1;
	    }

	    // Performs drift correction with target drawn in the center
	    error = do_drift_correct(SCRWIDTH/2, SCRHEIGHT/2,
		draw_own_target, 1);

	    // repeat if ESC was pressed to access Setup menu
	    // Redawing the target may be necessary
	    if(error!=27)
	        break;
	    else
	        target_shown = 0;
	}
	\endcode

	\sa \c do_tracker_setup() and \c set_dcorr_sounds()
 */
INT16 ELCALLTYPE do_drift_correct(INT16 x, INT16 y, INT16 draw, INT16 allow_setup);

/*! @ingroup setup_eyelink
  	Performs a drift correction before a trial. Same as do_drift_correct() except, this takes the x,y values as float.

	@remarks When the 'ESC' key is pressed during drift correction, \c &lt;allow_setup&gt; determines
				the result.  If \c 1, the EyeLink Setup menu is accessed. This will always clear
				the display, so redrawing of hidden stimuli may be required.  Otherwise, the drift
				correction is aborted. Calling \c exit_calibration() from an event handler will cause
				any call to \c do_drift_correct() in progress to return immediately.  In all cases,
				the return code will be \c 27 (\c ESC_KEY).
	@param x X Position of drift correction target.
	@param y Y Position of drift correction target.
	@param draw If \c 1, the drift correction will clear the screen to the target background color,
				draw the target, and clear the screen again when the drift correction is done.
				If \c 0, the fixation target must be drawn by the user.
	@param allow_setup If \c 1, accesses Setup menu before returning, else aborts drift correction.
	@return \c 0 if successful, \c 27 if 'ESC' key was pressed to enter Setup menu or abort.

	\b Example:
	\code
	// This program illustrates the use of do_drift_correction with drift correction target drawn by the user

	#include <eyelink.h>

	int target_shown = 0;
	int draw_own_target = 1;

	while(1)
	{
	    // Checks link often so we can exit if tracker stopped
	    if(!eyelink_is_connected())
	        return ABORT_EXPT;

	    // If drift correct target is not drawn, we have to draw it here
	    if (draw_own_target && !target_shown)
	    {
	        // Code for drawing own drift correction target
	        target_shown = 1;
	    }

	    // Performs drift correction with target drawn in the center
	    error = do_drift_correctf(SCRWIDTH/2, SCRHEIGHT/2,
		draw_own_target, 1);

	    // repeat if ESC was pressed to access Setup menu
	    // Redawing the target may be necessary
	    if(error!=27)
	        break;
	    else
	        target_shown = 0;
	}
	\endcode

	\sa \c do_tracker_setup() and \c set_dcorr_sounds()

*/
INT16 ELCALLTYPE do_drift_correctf(float x, float y, INT16 draw, INT16 allow_setup);

/*******************************************************************************
 * (USED BY do_tracker_setup(), YOU DO NOT NEED TO CALL normally               *
 * Called for you by do_tracker_setup() and do_drift_correct()                 *                 *
 *******************************************************************************/
/*! @ingroup display
	This function needs some "helper" graphics to clear the scren and draw the
	fixation targets.  Since C graphics are compiler-dependent, these are found
	in other C source files.

	While tracker is in any mode with fixation targets.
	Reproduce targets tracker needs.
	(if local_trigger) Local Spacebar acts as trigger.
	(if local_control)  Local keys echoes to tracker.
	@return \c 0 if OK, \c 27 if aborted, \c TERMINATE_KEY if pressed.
 */
INT16 ELCALLTYPE target_mode_display(void);

/*******************************************************************************
 * (USED BY do_tracker_setup(), YOU DO NOT NEED TO CALL   normally             *
 *******************************************************************************/
/*! @ingroup display
	This handles display of the EyeLink camera images.  While in imaging mode,
	it contiuously requests and displays the current camera image.  It also
	displays the camera name and threshold setting.  Keys on the subject PC
	keyboard are sent to the tracker, so the experimenter can use it during
	setup.  It will exit when the tracker leaves imaging mode or disconnects.

    @return \c 0 if OK, \c TERMINATE_KEY if pressed, \c -1 if disconnect.

	@remark This function not normally used externally.  If you need camera setup
	use do_tracker_setup() or if you need drift correction use do_drift_correct()
 */
INT16 ELCALLTYPE image_mode_display(void);


/********** ALERT BOX ************/

/*******************************************************************************
 * displays general STOP-icon alert box
 * text is formatted via printf-like arguments
 *******************************************************************************/
/*! @ingroup error_reporting
	When an error occurs, a notification must be given to the user.  If no alert_printf_hook
	is set, this function uses the Windows MessageBox() function in windows. On other
	platforms printf is called.

 	@param fmt A printf() formatting string<...>:  any arguments required.

	@remark The error message may no showup in certain display environment eg.
	SDL with SDL_FULLSCREEN|SDL_HWSURFACE |  SDL_DOUBLEBUF
 */
void ELCALLTYPE alert_printf(char *fmt, ...);




/*******************************************************************************
 *FILE TRANSFER
 *******************************************************************************/

/*******************************************************************************
 * THIS ROUTINE MAY NEED TO BE CREATED FOR EACH PLATFORM                       *
 * This call should be implemented for a standard programming interface        *
 * Copies tracker file <src> to local file <dest>.                             *
 * If specifying full file name, be sure to add ".edf"                         *
 * extensions for data files.                                                  *
 * If <src> = "", tracker will send last opened data file.                     *
 * If <dest> is NULL or "", creates local file with source file name.          *
 * Else, creates file using <dest> as name.  If <dest_is_path> != 0            *
 * uses source file name but adds <dest> as directory path.                    *
 * returns: file size if OK, negative =  error code                            *
 *******************************************************************************/
/*! @ingroup datafile
	This receives a data file from the EyeLink tracker PC.

	@param[in] src Name of eye tracker file (including extension).  If "" (empty string), asks
				tracker for name of last opened data file.
	@param[in] dest Name of local file to write to (including extension).  This must be a
			   valid file name or directory name.
	@param[in] dest_is_path If nonzero, appends file name to &lt;dest&gt; as a directory path.
	@return \c 0 if file transfer was cancelled.\n
			Size of file if successful.\n
			\c FILE_CANT_OPEN if no such file.\n
			\c FILE_XFER_ABORTED if data error.

	\b Example: See \c open_data_file()

	\sa \c close_data_file() and \c open_data_file()

    @remark If linked against eyelink_exptkit20.lib or w32_dialogs.h is included,
	the behaviour of this function is receive_data_file_dialog().
 */
INT32 ELCALLTYPE receive_data_file(char *src, char *dest, INT16 dest_is_path);

/*******************************************************************
 * exact same as receive_data_file. except the feedback parameters can be used
 * for showing what is the full size of the edf file and howmuch is received so far.
 * this function is currently used internally only.
 *******************************************************************/
/*! @ingroup datafile
	This receives a data file from the EyeLink tracker PC.
	Exact same as \c receive_data_file(). except the feedback parameters can be used
	for showing what is the full size of the edf file and how much is received so far.
	This function is currently used internally only.

	@param[in] src Name of eye tracker file (including extension).  If "" (empty string), asks
				tracker for name of last opened data file.
	@param[in] dest Name of local file to write to (including extension).  This must be a
			   valid file name or directory name.
	@param[in] dest_is_path If nonzero, appends file name to &lt;dest&gt; as a directory path.
	@param[in] progress A function pointer, that takes size and received size integers.
			   This allows, one to display progress bar on edf file transfer.
	@return \c 0 if file transfer was cancelled.\n
			Size of file if successful.\n
			\c FILE_CANT_OPEN if no such file.\n
			\c FILE_XFER_ABORTED if data error.


	\sa \c receive_data_file, \c close_data_file() and \c open_data_file()
 */
INT32 ELCALLTYPE receive_data_file_feedback(char *src, char *dest, INT16 dest_is_path,
                  void(ELCALLTYPE *progress)(unsigned int size,unsigned int received));

/*******************************************************************************
 * EDF OPEN, CLOSE
 *******************************************************************************/

/*******************************************************************************
 * These functions were added as future revisions of EyeLink
 * might require significant time to open and close EDF files
 * Opens EDF file on tracker hard disk
 *******************************************************************************/
/*! @ingroup datafile
	Opens an EDF file, closes any existing file.

 	@param name Name of data file.
 	@return Returns \c 0 if success, else error code.
 */
INT16 ELCALLTYPE open_data_file(char *name);

/*******************************************************************************
 * Closes EDF file on tracker hard disk
 *******************************************************************************/
/*! @ingroup datafile
	Closes any open EDF file.

	@return Returns \c 0 if success, else error code.
 */
INT16 ELCALLTYPE close_data_file(void);








/************* BREAK TESTS *********/

/*! @ingroup keyinput
	This function tests if the 'ESC' key is held down, and is usually used to break out of
	nested loops.  This does not allow processing of Windows messages, unlike \c getkey().

 	@remarks Warning:  Under Windows XP, this call will not work in realtime mode at all,
 				and will take several seconds to respond if graphics are being drawn
 				continuously.  This function works well in realtime mode under Windows 2000.
	@return \c 1 if 'ESC' key held down; \c 0 if not.

	\b Example: See \c break_pressed()

	\sa \c break_pressed(), \c getkey() and \c echo_key()
 */
INT16 ELCALLTYPE escape_pressed(void);

/*! @ingroup keyinput
	Tests if the program is being interrupted.  You should break out of loops immediately if
	this function does not return \c 0, if \c getkey() return \c TERMINATE_KEY, or if
	\c eyelink_is_connected() returns \c 0.

 	@remarks Warning:  Under Windows XP, this call will not work in realtime mode at all, and
 				will take several seconds to respond if graphics are being drawn continuously.
 				This function works well in realtime mode under Windows 2000.
 	@return \c 1 if CTRL-C is pressed, \c terminal_break() was called,  or the program has been
 			terminated with ALT-F4; \c 0 otherwise.

	\b Example:
	\code
	// This program illustrates the use of break_pressed() and escape_pressed()

	#include <eyelink.h>

	// reset keys and buttons from tracker
	eyelink_flush_keybuttons(0);

	// Trial loop: till timeout or response
	while(1)
	{
	    // Checks if recording aborted
	    if((error=check_recording())!=0) return error;

	    // check for program termination or ALT-F4 or CTRL-C keys
	    if(break_pressed())
		   return ABORT_EXPT;

	    // check for local ESC key to abort trial (useful in debugging)
	    if(escape_pressed())
		   return SKIP_TRIAL;

	    ...
	    // Other code for display update, trial terminating, etc.
	}
	\endcode

	\sa \c echo_key(), \c escape_pressed() and \c getkey()
 */
INT16 ELCALLTYPE break_pressed(void);


/********* ASYNCHRONOUS BREAKOUTS *********/
/*******************************************************************************
 * Because Windows is multi-tasking, some other event (i.e. a timer event or
 * ALT-TAB) may affect your application during loops or calibration.
 * Your event handlers can call these functions to stop ongoing operations
 *
 * call from Windows event handlers when application must exit
 * forces calibration or drift correction to exit with result=27
 * when <assert> is nonzero,  will caused break_pressed() to test true
 * continuously, also causes getkey() to return TERMINATE_KEY
 * If <assert> is 0, will restore break_pressed() and getkey() to normal
 *******************************************************************************/
/*! @ingroup keyinput
	This function can be called in an event handler to signal that the program is terminating.
	Calling this function with an argument of \c 1 will cause \c break_pressed() to return \c 1, and
	\c getkey() to return \c TERMINATE_KEY.  These functions can be re-enabled by calling
	\c terminal_break() with an argument of \c 0.

	@param assert \c 1 to signal a program break, \c 0 to reset break.

	\b Example:
	\code
	// The following code illustrates the use of terminal_break().  This would usually be called
	// from a message or event handler (see the w32_demo_window.c module) for a complete example.

	#include <eyelink.h>

	switch (message)
	{
	case WM_CLOSE:        // If ALT-F4 pressed, force program to close
	    PostQuitMessage(0);
	    terminal_break(1);// break out of loops
	    break;

	case WM_DESTROY:      // Window being closed by ALT-F4
	    PostQuitMessage( 0 );
	    ...
	    // Code here for graphics cleaning up
	    terminal_break(1);// break out of loops
	    break;

	case WM_QUIT:         // Needs to break out of any loops
	    terminal_break(1);
	    break;
	    ...
	    // Other windows messages and events
	}
	\endcode

	\sa \c break_pressed() and \c getkey()
 */
void ELCALLTYPE terminal_break(INT16 assert);

/*******************************************************************************
 * NEW STANDARD FUNCTIONS
 * Checks state of aplication in message-passing GUI environments
 *******************************************************************************/
/* @internal
	Checks status of terminal break flag directly.
 */
INT16 ELCALLTYPE application_terminated(void);




/************ Real time and priority control **************/

/*******************************************************************************
 * Sets up for realtime execution (minimum delays)
 * This may take some time (assume up to 100 msec)
 * <delay> sets min time so delay may be useful
 * Effects vary by operating system
 * Keyboard, mouse, and sound may be disabled in some OS
 * Has little effect in Win9x/ME
 *******************************************************************************/
/*! @ingroup priority
	Sets the application priority and cleans up pending Windows activity to place the
	application in realtime mode.  This could take up to 100 milliseconds, depending on
	the operation system, to set the application priority.  Use the \c &lt;delay&gt; value to set
	the minimum time this function takes, so that this function can act as a useful delay.

 	@remarks Warning:  Under Windows XP, this call will lock out all keyboard input.  The
 				Task Manager will take about 30 seconds to respond to CTRL-ALT-DEL, so
 				press this once and be patient.  The keyboard functions well  in realtime
 				mode under Windows 2000.  This function has little or no effect under
 				Windows 95/98/Me. Keyboard, mouse, and sound may be disabled in some OS.
 	@param delay Minimum delay in milliseconds (should be about 100).

	\b Example:
	\code
	// This program shows the use of begin_realtime_mode() and end_realtime_mode()
	#include <eyelink.h>

	int error;

	// Start data recording to EDF file, BEFORE DISPLAYING STIMULUS
	error = start_recording(1,1,1,1);
	if(error != 0)  return error;     // return error code if failed

	// Records for 100 msec before displaying stimulus
	// Sets up for realtime execution (minimum delays)
	begin_realtime_mode(100);

	...
	// Code for drawing, updating display and process trial loop
	// including timing, key or button response handling

	// Exits realtime execution mode
	end_realtime_mode();

	...
	// Code for trial clean up and exiting
	\endcode

	\sa \c end_realtime_mode()
 */
void ELCALLTYPE begin_realtime_mode(UINT32 delay);

/*******************************************************************************
 * Exits realtime execution mode
 * Typically just lowers priority
 *******************************************************************************/
/*! @ingroup priority
	Returns the application to a priority slightly above normal, to end realtime mode.  This
	function should execute rapidly, but there is the possibility that Windows will allow
	other tasks to run after this call, causing delays of 1-20 milliseconds.

 	@remarks Warning:  This function has little or no effect under Windows 95/98/Me.

	\b Example: See \c begin_realtime_mode()

	\sa \c begin_realtime_mode()
 */
void ELCALLTYPE end_realtime_mode(void);

/*! @ingroup priority
	Raise application priority.
	May interfere with other applications.
 */
void ELCALLTYPE set_high_priority(void);

/*! @ingroup priority
	Sets application priority to system normal
 */
void ELCALLTYPE set_normal_priority(void);

/*! @ingroup priority
	returns whether the current mode is real-time.

	@return \c 1 if in realtime mode, else \c 0.
 */
INT32 ELCALLTYPE in_realtime_mode(void);



/*
	Internal function. This should not be used externally.
 */
void ELCALLTYPE eyelink_enable_extended_realtime(void);

/************ Display Independent Image transfer to tracker and save*************/

/*!
@ingroup display
\brief Represents an RGB color.
*/
typedef struct
{
    byte r; /*!< Red */
    byte g; /*!< Green */
    byte b; /*!< Blue */
    byte unused;
}EYECOLOR;


/*!@ingroup display
 \brief Represents a palette index. */
typedef struct {
    int       ncolors; /*!< Number of colors in the palette */
    EYECOLOR *colors;  /*!< Actual colors */
}EYEPALETTE;


/*!
@ingroup display
\brief Represents pixel format of an image or surface.
*/
typedef struct
{

    byte colorkey;
    INT32 Rmask;
    INT32 Gmask;
    INT32 Bmask;
    INT32 Amask;
    EYEPALETTE *palette;
}EYEPIXELFORMAT;


/*!@ingroup display
\brief Represents a bitmap image*/
typedef struct
{
    INT32 w;    /*!< width of the image */
    INT32 h;    /*!< height of the image */
    INT32 pitch;/*!< pitch of image lines. This can be 0. if this is 0, then ((depth+7)/8)*width is used */
    INT32 depth;/*!< Depth of the image. possible values are 8,15,16,24,32*/
    void *pixels; /*!< uncompressed pixel data */
    EYEPIXELFORMAT *format; /*!< pixel format of the image. */
}EYEBITMAP;

/*!
@ingroup display
*/
typedef enum
{
  JPEG,
  PNG,
  GIF,
  BMP,
  XPM,
}IMAGETYPE;



/*! @ingroup error_reporting
	Returns error description for given function with error id.
	Example:
	@code

	int rv = open_eyelink_connection(0);
	if(rv)
	{
		char *errmsg = eyelink_get_error(rv,"open_eyelink_connection");
		printf("Error: %s \n", errmsg); // report the error
		return -1;
	}

	@endcode

    @param[in] id Error id
	@param[in] function_name Name of the function that generated the error id.
 */
char *ELCALLTYPE eyelink_get_error(int id, char *function_name);




/***** OPTIONS FOR eyelink_bitmap_xxx FUNCTIONS ****/
/*! 
@addtogroup bitmap_save
@{
*/
#define  BX_AVERAGE     0   /*!< Average combined pixels                          */
#define  BX_DARKEN      1   /*!< Choose darkest (keep thin dark lines)            */
#define  BX_LIGHTEN     2   /*!< Choose darkest (keep thin white lines)           */

#define  BX_MAXCONTRAST 4   /*!< Stretch contrast to black->white                */
#define  BX_NODITHER    8   /*!< No dither, just quantize                        */
#define  BX_GRAYSCALE   16  /*!< Gray scale                                      */
#define  BX_DOTRANSFER  256 /*!< Send bitmap to host                             */

#define SV_NOREPLACE    1   /*!< do not replace if the file already exists        */
#define SV_MAKEPATH     2   /*!< make destination path if does not already exists */

/*! 
@}
*/


/************ FILENAME SUPPORT **************/

     // error codes returned by
/*! @addtogroup datafile
@{
*/
#define BAD_FILENAME -2222 /*!<Bad file name */
#define BAD_ARGUMENT -2223 /*!<Invalid argument */ 
/*!
@}
*/
/*! @ingroup datafile
	Splice 'path' to 'fname', store in 'ffname'.
	Tries to create valid concatenation.
	If 'fname' starts with '\', just adds drive from 'path'.
	If 'fname' contains drive specifier, it is not changed.
 */
void ELCALLTYPE splice_fname(char *fname, char *path, char *ffname);

/*! @ingroup datafile
	Checks file name for legality.
    Attempts to ensure cross-platform for viewer.
	No spaces allowed as this interferes with messages.
	Assume viewer will translate forward/backward slash.
	Windows: don't allow <>:"/\|
	Also, device names, such as aux, con, lpt1, and prn are not allowed in windows.
	forward slashes is missed.
 */
int ELCALLTYPE check_filename_characters(char *name);

/*! @ingroup datafile
	Checks if file and/or path exists.
	@returns \c 0 if does not exist, \c 1 if exists, \c -1 if cannot overwrite.
 */
int ELCALLTYPE file_exists(char *path);

/*! @ingroup datafile
	Checks if path exists.
	Will create directory if 'create'.
	Creates directory from last name in 'path', unless ends with '\' or 'is_dir' nonzero.
	Otherwise, last item is assumed to be filename and is dropped.

    @return \c 0 if exists, \c 1 if created, \c -1 if failed.
 */
int ELCALLTYPE create_path(char *path, INT16 create, INT16 is_dir);





/*! @ingroup bitmap_save
	This function saves the entire bitmap as a .BMP, .JPG, .PNG, or .TIF file, and
	transfers the image to tracker as backdrop for gaze cursors.

	@param hbm Bitmap to save or transfer or both.
	@param xs X position.
	@param ys Y position.
	@param width Width.
	@param height Height.
	@param fname File name to save as. The extension decides the format of the file.
	@param path The directory to which the file will be written.
	@param sv_options If the file exists, it replaces the file unless SV_NOREPLACE is specified.
	@param xd X positon.
	@param yd Y positon.
	@param xferoptions Transfer options set with bitwise OR of the following
	constants, determines how bitmap is processed:
	\arg \c BX_AVERAGE Averaging combined pixels
	\arg \c BX_DARKEN  Choosing darkest and keep thin dark lines.
	\arg \c BX_LIGHTEN Choosing darkest and keep thin white lines and control how
	bitmap size is reduced to fit tracker display.
	\arg \c BX_MAXCONTRAST Maximizes contrast for clearest image.
	\arg \c BX_NODITHER Disables the dithering of the image.
	\arg \c BX_GREYSCALE Converts the image to grayscale (grayscale works
	best for EyeLink I, text, etc.).

	\sa el_bitmap_to_backdrop(), el_bitmap_save(), sdl_bitmap_to_backdrop(),
	sdl_bitmap_save(),sdl_bitmap_save_and_backdrop, gdi_bitmap_to_backdrop(),
	gdi_bitmap_save(),gdi_bitmap_save_and_backdrop, bitmap_save(),
	and bitmap_to_backdrop() for more information.

    @remark This function relies on the writeImageHook set by call to set_write_image_hook()
	to write the images in different formats. By default, if eyelink_core_graphics library is used,
	gd library is used to write the images and if eyelink_gdi_graphics is used FreeImage library is used
	to write the images. If neither one of them is used, call to this function does not write the images unless,
	set_write_image_hook() is used to set the writeImageHook.

	@remark This function should not be called when timing is critical, as this might take very long to return.

 */
int ELCALLTYPE el_bitmap_save_and_backdrop(EYEBITMAP * hbm, INT16 xs, INT16 ys,
                                   INT16 width, INT16 height,char *fname,
                                   char *path, INT16 sv_options,INT16 xd,
                                   INT16 yd, UINT16 xferoptions);

/*! @ingroup bitmap_save
	This function transfers the bitmap to the tracker PC as backdrop for gaze
	cursors.


	@param hbm Bitmap to save or transfer or both.
	@param xs X position.
	@param ys Y position.
	@param width Width.
	@param height Height.
	@param xd X positon.
	@param yd Y positon.
	@param xferoptions Transfer options set with bitwise OR of the following
	constants, determines how bitmap is processed:
	\arg \c BX_AVERAGE Averaging combined pixels
	\arg \c BX_DARKEN  Choosing darkest and keep thin dark lines.
	\arg \c BX_LIGHTEN Choosing darkest and keep thin white lines and control how
	bitmap size is reduced to fit tracker display.
	\arg \c BX_MAXCONTRAST Maximizes contrast for clearest image.
	\arg \c BX_NODITHER Disables the dithering of the image.
	\arg \c BX_GREYSCALE Converts the image to grayscale (grayscale works
	best for EyeLink I, text, etc.).

	\sa sdl_bitmap_to_backdrop(), el_bitmap_save_and_backdrop(),
	sdl_bitmap_save_and_backdrop(), gdi_bitmap_to_backdrop(),
	gdi_bitmap_save_and_backdrop(),	and bitmap_to_backdrop() for more information.

	@remark This function should not be called when timing is critical, as this might take very long to return.
 */
int ELCALLTYPE el_bitmap_to_backdrop(EYEBITMAP * hbm, INT16 xs, INT16 ys,
                                   INT16 width, INT16 height,INT16 xd,
                                   INT16 yd, UINT16 xferoptions);

/*! @ingroup bitmap_save
	This function saves the entire bitmap or selected part of a bitmap in an
	image file (with an extension of .png, .bmp, .jpg, or .tif). It creates
	the specified file if this file does not exist.


	@param hbm Bitmap to save or transfer or both.
	@param xs X position.
	@param ys Y position.
	@param width Width.
	@param height Height.
	@param fname File name to save as. The extension decides the format of the file.
	@param path The directory to which the file will be written.
	@param sv_options If the file exists, it replaces the file unless SV_NOREPLACE is specified.

    @remark This function relies on the writeImageHook set by call to set_write_image_hook()
	to write the images in different formats. By default, if eyelink_core_graphics library is used,
	gd library is used to write the images and if eyelink_gdi_graphics is used FreeImage library is used
	to write the images. If neither one of them is used, call to this function does not write the images unless,
	set_write_image_hook() is used to set the writeImageHook.

    @remark This function should not be called when timing is critical, as this might take very long to return.
 */
int ELCALLTYPE el_bitmap_save(EYEBITMAP * hbm, INT16 xs, INT16 ys,
                                   INT16 width, INT16 height,char *fname,
                                   char *path, INT16 sv_options);



/************ Pluggable display environment support **************/
// modifier defines
// use this to set KeyInput.modifier
/*!
@addtogroup display
@{
*/
#define KEYINPUT_EVENT              0x1 /*!< set to InputEvent.type to notify keyboard input event*/

/*!
	set InputEvent.type to notify mouse input event
 	@remark For future use.
*/
#define MOUSE_INPUT_EVENT           0x4 
/*! 
	set InputEvent.type to notify mouse motion input event
	@remark For future use.
*/
#define MOUSE_MOTION_INPUT_EVENT    0x5 

/*! set InputEvent.type to notify mouse button input event. 
	@remark For future use.
*/
#define MOUSE_BUTTON_INPUT_EVENT    0x6 

#define ELKMOD_NONE   0x0000 /*!< Modifier for KeyInput.modifier; No modifier present */
#define ELKMOD_LSHIFT 0x0001 /*!< Modifier for KeyInput.modifier; Left shift*/
#define ELKMOD_RSHIFT 0x0002 /*!< Modifier for KeyInput.modifier; Right shift*/
#define ELKMOD_LCTRL  0x0040 /*!< Modifier for KeyInput.modifier; left conrol*/
#define ELKMOD_RCTRL  0x0080 /*!< Modifier for KeyInput.modifier; Right control*/
#define ELKMOD_LALT   0x0100 /*!< Modifier for KeyInput.modifier; Left Alt*/
#define ELKMOD_RALT   0x0200 /*!< Modifier for KeyInput.modifier; Right alt*/
#define ELKMOD_LMETA  0x0400 /*!< Modifier for KeyInput.modifier; Left Meta*/
#define ELKMOD_RMETA  0x0800 /*!< Modifier for KeyInput.modifier; Right Meta*/
#define ELKMOD_NUM    0x1000 /*!< Modifier for KeyInput.modifier; Number lock key*/
#define ELKMOD_CAPS   0x2000 /*!< Modifier for KeyInput.modifier; Cap lock key*/
#define ELKMOD_MODE   0x4000 /*!< Modifier for KeyInput.modifier; Mode key*/

/*!
@}
*/

#define ELKEY_DOWN 1
#define ELKEY_UP   0 

/*! @ingroup display
\brief Keyboard input event structure
*/
typedef struct {
    byte type;       /*!< The value of type should have value of KEYINPUT_EVENT */
    byte  state;     /*!< KEYDOWN = 1 or KEYUP    = 0 */
    UINT16 key;      /*!< keys */
	UINT16 modifier; /*!< modifier *//* (New June 21, 2006) */
	UINT16 unicode;  /*!< unicode character value of the key *//* (New June 22, 2006) */
} KeyInput;

/*! @ingroup display
\brief Mouse motion event structure (For future)
*/
typedef struct {
    byte type;    /*!< MOUSE_MOTION_INPUT_EVENT */
    byte which;    /*!< The mouse device index */
    byte state;    /*!< The current button state */
    UINT16 x, y;    /*!< The X/Y coordinates of the mouse */
    UINT16 xrel;    /*!< The relative motion in the X direction */
    UINT16 yrel;    /*!< The relative motion in the Y direction */
} MouseMotionEvent;

/*! @ingroup display
\brief Mouse button event structure (For future)
*/
typedef struct {
    byte type;      /*!< MOUSE_BUTTON_INPUT_EVENT */
    byte which;     /*!< The mouse device index */
    byte button;    /*!< The mouse button index */
    byte state;     /*!< BUTTONDOWN = 0 or BUTTONUP    = 1 */
    UINT16 x, y;    /*!< The X/Y coordinates of the mouse at press time */
} MouseButtonEvent;

/*!
@ingroup display
\brief Union of all input types.

A union that is used by the callback function get_input_key_hook,
to collect input data.  At the moment, it is meant to collect only
keyboard inputs. In the future this may be used to support mouse inputs as well.
*/
typedef union
{
    byte type;
    KeyInput  key;
    MouseMotionEvent motion;
    MouseButtonEvent button;

}InputEvent;


/*!@ingroup display
\brief Structure used set and get callback functions

Structure used set and get callback functions so that the calibration
graphics can be drawn.
*/
typedef struct
{
	/*!
		This function is called to setup calibration/validation display.
		This function called before any calibration routines are called.
	*/
    INT16  (ELCALLBACK * setup_cal_display_hook)(void);

    /*!
	  This is called to release any resources that are not required beyond calibration.
	  Beyond this call, no calibration functions will be called.
 	*/
    void   (ELCALLBACK * exit_cal_display_hook)(void) ;

	/*!
	This function is called if abort of record. It is used to hide display from subject.
	*/
    void   (ELCALLBACK * record_abort_hide_hook)(void) ;

    /*!
		This function is responsible for initializing any resources that are
		required for camera setup.

		@param width width of the source image to expect.
		@param height height of the source image to expect.
		@return -1 if failed,  0 otherwise.
    */
    INT16  (ELCALLBACK * setup_image_display_hook)(INT16 width, INT16 height) ;
    /*!
	  This function is called to update any image title change.
	  @param threshold if -1 the entire tile is in the title string
					   otherwise, the threshold of the current image.
	  @param title     if threshold is -1, the title contains the whole title
					   for the image. Otherwise only the camera name is given.
    */
    void   (ELCALLBACK * image_title_hook)(INT16 threshold, char *cam_name) ;

    /*!
		This function is called to supply the image line by line from top to bottom.
		@param width  width of the picture. Essentially, number of bytes in \c pixels.
		@param line   current line of the image
		@param totlines total number of lines in the image. This will always equal the height of the image.
		@param pixels pixel data.

	    Eg. Say we want to extract pixel at position (20,20) and print it out as rgb values.

		@code
	    if(line == 20) // y = 20
		{
			byte pix = pixels[19];
			// Note the r,g,b arrays come from the call to set_image_palette
			printf("RGB %d %d %d\n",r[pix],g[pix],b[pix]);
		}
		@endcode

		@remark certain display draw the image up side down. eg. GDI.
    */
    void   (ELCALLBACK * draw_image_line_hook)(INT16 width, INT16 line,
                         INT16 totlines, byte *pixels) ;

    /*!
		This function is called after setup_image_display and before the first call to
		draw_image_line. This is responsible to setup the palettes to display the camera
		image.

	    @param ncolors number of colors in the palette.
		@param r       red component of rgb.
		@param g       blue component of rgb.
		@param b       green component of rgb.
    */
    void   (ELCALLBACK * set_image_palette_hook)(INT16 ncolors, byte r[],
                         byte g[], byte b[]) ;

    /*!
		This is called to notify that all camera setup things are complete.  Any
		resources that are allocated in setup_image_display can be released in this
		function.
    */
    void   (ELCALLBACK * exit_image_display_hook)(void) ;

    /*!
	  Called to clear the calibration display.
    */
    void   (ELCALLBACK * clear_cal_display_hook)() ;

    /*!
		This function is responsible for erasing the target that was drawn
		by the last call to draw_cal_target.
    */
    void   (ELCALLBACK * erase_cal_target_hook)();


	/*!
	  This function is responsible for the drawing of the target for calibration,validation
	  and drift correct at the given coordinate.
	  @param x x coordinate of the target.
	  @param y y coordinate of the target.
	  @remark The x and y are relative to what is sent to the tracker for the command screen_pixel_coords.
    */
    void   (ELCALLBACK * draw_cal_target_hook)(INT16 x, INT16 y);

    /*!
	 This function is called to signal new target.
    */
    void   (ELCALLBACK * cal_target_beep_hook)(void) ;

    /*!
	  This function is called to signal end of calibration.
	  @param error if non zero, then the calibration has error.
    */
    void   (ELCALLBACK * cal_done_beep_hook)(INT16 error) ;

    /*!
	  This function is called to singnal the end of drift correct.
	  @param error if non zero, then the drift correction failed.
    */
    void   (ELCALLBACK * dc_done_beep_hook)(INT16 error) ;

    /*!
	  This function is called to signal a new drift correct target.
    */
    void   (ELCALLBACK * dc_target_beep_hook)(void) ;

    /*!
		This is called to check for keyboard input.
		In this function:
		\arg check if there are any input events
		\arg if there are input events, fill key_input and return 1.
			 otherwise return 0. If 1 is returned this will be called
		     again to check for more events.
		@param[out] key_input  fill in the InputEvent structure to return
			 key,modifier values.
		@return if there is a key, return 1 otherwise return 0.

		@remark Special keys and modifiers should match the following code
		Special keys:
		@code

		#define F1_KEY    0x3B00
		#define F2_KEY    0x3C00
		#define F3_KEY    0x3D00
		#define F4_KEY    0x3E00
		#define F5_KEY    0x3F00
		#define F6_KEY    0x4000
		#define F7_KEY    0x4100
		#define F8_KEY    0x4200
		#define F9_KEY    0x4300
		#define F10_KEY   0x4400

		#define PAGE_UP    0x4900
		#define PAGE_DOWN  0x5100
		#define CURS_UP    0x4800
		#define CURS_DOWN  0x5000
		#define CURS_LEFT  0x4B00
		#define CURS_RIGHT 0x4D00

		#define ESC_KEY   0x001B
		#define ENTER_KEY 0x000D

		@endcode

			Modifier: If you are using SDL you do not need to modify the
			modifier value as they match the value.

		@code
		#define ELKMOD_NONE   0x0000
		#define ELKMOD_LSHIFT 0x0001
		#define ELKMOD_RSHIFT 0x0002
		#define ELKMOD_LCTRL  0x0040
		#define ELKMOD_RCTRL  0x0080
		#define ELKMOD_LALT   0x0100
		#define ELKMOD_RALT   0x0200
		#define ELKMOD_LMETA  0x0400
		#define ELKMOD_RMETA  0x0800,
		#define ELKMOD_NUM    0x1000
		#define ELKMOD_CAPS   0x2000
		#define ELKMOD_MODE   0x4000

		@endcode
	*/

    short  (ELCALLBACK * get_input_key_hook)(InputEvent * event);

    /*!

    This function is called whenever alert_printf is called.  In windows, if no
    callback is set calls MessageBox function. In other platforms, if no call back is set
    prints out to console.
    */
    void   (ELCALLBACK * alert_printf_hook)(const char *);
}HOOKFCNS;


/*!@ingroup display
\brief Enum used for calibration beeps

*/
typedef enum _EL_CAL_BEEP
{
	/*! Drift Correct Done error beep*/
	EL_DC_DONE_ERR_BEEP      =-2,
	/*! Calibration Done error beep*/
	EL_CAL_DONE_ERR_BEEP     =-1,

	/*! Calibration Done correct beep*/
	EL_CAL_DONE_GOOD_BEEP    =0,
	/*! Calibration target present beep*/
	EL_CAL_TARG_BEEP         =1,
	
	/*! Drift Correct Done correct beep*/
	EL_DC_DONE_GOOD_BEEP     =2,
	/*! Drift Correct target present beep*/
	EL_DC_TARG_BEEP          =3
	
	
}EL_CAL_BEEP;


/*!@ingroup display
\brief Structure used set and get callback functions

Structure used set and get callback functions so that the calibration
graphics can be drawn.
*/
typedef struct
{
	/*!Major version. At the moment the version number must be set to 1 */
	int major; 

	/*!Minor version. At the moment the version number must be set to 0 */
	int minor;

	/*!The value set here will be passed to every single call back functions */
	void *userData;

	/*!
		This function is called to setup calibration/validation display.
		This function called before any calibration routines are called.
		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

		@return 0 upon success. 
	*/
    INT16  (ELCALLBACK * setup_cal_display_hook)(void *userData);

    /*!
	  This is called to release any resources that are not required beyond calibration.
	  Beyond this call, no calibration functions will be called.
	  @return 0 upon success. 
 	*/
    INT16   (ELCALLBACK * exit_cal_display_hook)(void *userData) ;


    /*!
		This function is responsible for initializing any resources that are
		required for camera setup.

  		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

		@param width width of the source image to expect.
		@param height height of the source image to expect.
		@return -1 if failed,  0 otherwise.
    */
    INT16  (ELCALLBACK * setup_image_display_hook)(void *userData,INT16 width, INT16 height) ;
    /*!
	  This function is called to update any image title change.
	  @param threshold if -1 the entire tile is in the title string
					   otherwise, the threshold of the current image.
	  @param title     if threshold is -1, the title contains the whole title
					   for the image. Otherwise only the camera name is given.
	  @return 0 upon success. 
    */
    INT16   (ELCALLBACK * image_title_hook)(void *userData,char *title) ;

    /*!
		This function is called to supply the image.
		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

		@param width  width of the picture. 
		@param height height of the picture. 
		@param pixels pixels pointer to the pixel data. The size of the pixel data can be calculate by width*height*4. 
		The image comes in RGBA(network order) format.
		@return 0 upon success. 
    */
    INT16   (ELCALLBACK * draw_image)(void *userData,INT16 width, INT16 height,byte *pixels) ;



    /*!
		This is called to notify that all camera setup things are complete.  Any
		resources that are allocated in setup_image_display can be released in this
		function.
		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

		@return 0 upon success. 
    */
    INT16   (ELCALLBACK * exit_image_display_hook)(void *userData) ;

    /*!
	    Called to clear the calibration display.
		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

	    @return 0 upon success. 
    */
    INT16   (ELCALLBACK * clear_cal_display_hook)(void *userData) ;

    /*!
		This function is responsible for erasing the target that was drawn
		by the last call to draw_cal_target.
		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

		@return 0 upon success. 
    */
    INT16   (ELCALLBACK * erase_cal_target_hook)(void *userData);


	/*!
  	    This function is responsible for the drawing of the target for calibration,validation
	    and drift correct at the given coordinate.
		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

	    @param x x coordinate of the target.
	    @param y y coordinate of the target.
	    @remark The x and y are relative to what is sent to the tracker for the command screen_pixel_coords.
	    @return 0 upon success. 
    */
    INT16   (ELCALLBACK * draw_cal_target_hook)(void *userData,float x, float y);

    /*!
		@param userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

	    This function is called to signal new target.
	    @return 0 upon success. 
    */
    INT16   (ELCALLBACK * play_target_beep_hook)(void *userData,EL_CAL_BEEP beep_type) ;

    

    /*!
		This is called to check for keyboard input.
		In this function:
		\arg check if there are any input events
		\arg if there are input events, fill key_input and return 1.
			 otherwise return 0. If 1 is returned this will be called
		     again to check for more events.

		@param[in] userData User context data set on userData field of HOOKFCNS2 set 
		at the time of call to setup_graphic_hook_functions.

		@param[out] key_input  fill in the InputEvent structure to return
			 key,modifier values.
		@return if there is a key, return 1 otherwise return 0. return -1 if there is an error.

		@remark Special keys and modifiers should match the following code
		Special keys:
		@code

		#define F1_KEY    0x3B00
		#define F2_KEY    0x3C00
		#define F3_KEY    0x3D00
		#define F4_KEY    0x3E00
		#define F5_KEY    0x3F00
		#define F6_KEY    0x4000
		#define F7_KEY    0x4100
		#define F8_KEY    0x4200
		#define F9_KEY    0x4300
		#define F10_KEY   0x4400

		#define PAGE_UP    0x4900
		#define PAGE_DOWN  0x5100
		#define CURS_UP    0x4800
		#define CURS_DOWN  0x5000
		#define CURS_LEFT  0x4B00
		#define CURS_RIGHT 0x4D00

		#define ESC_KEY   0x001B
		#define ENTER_KEY 0x000D

		@endcode

			Modifier: If you are using SDL you do not need to modify the
			modifier value as they match the value.

		@code
		#define ELKMOD_NONE   0x0000
		#define ELKMOD_LSHIFT 0x0001
		#define ELKMOD_RSHIFT 0x0002
		#define ELKMOD_LCTRL  0x0040
		#define ELKMOD_RCTRL  0x0080
		#define ELKMOD_LALT   0x0100
		#define ELKMOD_RALT   0x0200
		#define ELKMOD_LMETA  0x0400
		#define ELKMOD_RMETA  0x0800,
		#define ELKMOD_NUM    0x1000
		#define ELKMOD_CAPS   0x2000
		#define ELKMOD_MODE   0x4000

		@endcode
	*/

    INT16  (ELCALLBACK * get_input_key_hook)(void *userData,InputEvent * event);

    /*!

    This function is called whenever alert_printf is called.  In windows, if no
    callback is set calls MessageBox function. In other platforms, if no call back is set
    prints out to console.
    */
    INT16   (ELCALLBACK * alert_printf_hook)(void *userData, const char *msg);

	int reserved1;
	int reserved2;
	int reserved3;
	int reserved4;

}HOOKFCNS2;


/*! @ingroup display
	Primary function to setup display graphics hook functions.

	Example:
	@code
	INT16 ELCALLTYPE init_expt_graphics(HWND hwnd, DISPLAYINFO *info)
	{
	  HOOKFCNS fcns;
	  memset(&fcns,0,sizeof(fcns));
	  fcns.setup_cal_display_hook = setup_cal_display;
	  fcns.exit_cal_display_hook  = exit_cal_display;
	  fcns.record_abort_hide_hook = record_abort_hide;
	  fcns.setup_image_display_hook = setup_image_display;
	  fcns.image_title_hook       = image_title;
	  fcns.draw_image_line_hook   = draw_image_line;
	  fcns.set_image_palette_hook = set_image_palette;
	  fcns.exit_image_display_hook= exit_image_display;
	  fcns.clear_cal_display_hook = clear_cal_display;
	  fcns.erase_cal_target_hook  = erase_cal_target;
	  fcns.draw_cal_target_hook   = draw_cal_target;
	  fcns.cal_target_beep_hook   = cal_target_beep;
	  fcns.cal_done_beep_hook     = cal_done_beep;
	  fcns.dc_done_beep_hook      = dc_done_beep;
	  fcns.dc_target_beep_hook    = dc_target_beep;
	  fcns.get_input_key_hook     = get_input_key;
	  fcns.alert_printf_hook      = alert_printf_hook;

	  setup_graphic_hook_functions(&fcns);

	  return 0;
	}
	@endcode
 */
void ELCALLTYPE setup_graphic_hook_functions( HOOKFCNS *hooks);

/*! @ingroup display
	Returns a pointer to  HOOKFCNS, with values that are set by
	setup_graphic_hook_functions().

	This function with combination of setup_graphic_hook_functions
	can be used to over-ride an existing hook function.
 */
HOOKFCNS * ELCALLTYPE get_all_hook_functions();



/*! @ingroup display
	Primary function to setup display graphics hook functions of the second revision of the interface. 
	One of the major difference between this and setup_graphic_hook_functions is, this has fewer functions to implement
	also, takes extra parameters like the major and minor versions for future enhancements.


	Example:
	@code
	INT16 ELCALLTYPE init_expt_graphics(HWND hwnd, DISPLAYINFO *info)
	{
		HOOKFCNS2 fcns;
		memset(&fcns,0,sizeof(fcns));
		fcns.major = 1;
		fcns.minor = 0;
		fcns.userData = ts;

		// cam image
		fcns.draw_image   = draw_image;
		fcns.exit_image_display_hook= exit_image_display;
		fcns.setup_image_display_hook = setup_image_display;



		fcns.setup_cal_display_hook = setup_cal_display;
		fcns.clear_cal_display_hook = clear_display;
		fcns.erase_cal_target_hook  = clear_display;
		fcns.draw_cal_target_hook   = draw_cal_target;
		fcns.image_title_hook       = image_title;

		fcns.get_input_key_hook     = get_input_key;
		fcns.alert_printf_hook      = alert_printf_hook;
		return setup_graphic_hook_functions_V2(&fcns);
	}
	@endcode
 */
INT16 ELCALLTYPE setup_graphic_hook_functions_V2( HOOKFCNS2 *hooks);



/*! @ingroup display
	Returns a pointer to  HOOKFCNS2, with values that are set by
	setup_graphic_hook_functions_V2().

	This function with combination of setup_graphic_hook_functions
	can be used to over-ride an existing hook function.
 */
HOOKFCNS2 * ELCALLTYPE get_all_hook_functions_V2();


/*! @ingroup bitmap_save
	Use this function to set function pointer, so that the call to
	el_bitmap_save and el_bitmap_save_and_backdrop will use the passed in
	function to write the image to disk.
 */
int ELCALLTYPE set_write_image_hook(
               int (ELCALLBACK * hookfn)(char *outfilename,
                                         int format,
                                         EYEBITMAP *bitmap),
               int options );

/*internal
 */
int ELCALLTYPE eyelink_peep_input_event(InputEvent *event, int mask);

/*internal
 */
int ELCALLTYPE eyelink_get_input_event(InputEvent *event,int mask);

/*internal
 */
int ELCALLTYPE eyelink_peep_last_input_event(InputEvent *event, int mask);

/*internal
 */
void ELCALLTYPE eyelink_flush_input_event();

/************* GAZE HREF MAPPING SUPPORT **********/

        /* gaze coord rectangle setup for href <--> gaze */
        /* these same 4 must be set as HREFPTs           */


/*! @ingroup extra_data
	Function to initialize the gaze to href and href to gaze mapping.
	This function should be called before calling \c eyelink_href_to_gaze()
	or \c eyelink_gaze_to_href().
	@param left Left pixel value (normally 0).
	@param top Top pixel value (normally 0).
	@param right Right pixel value (width of the display).
	@param bottom Bottom pixel value (height of the display).
	@return \c -1 if the tracker does not support the commands,
				href_point_eye
				set_href_point
 */
INT32 ELCALLTYPE eyelink_initialize_mapping(float left, float top, float right, float bottom);

/*! @ingroup extra_data
	Apply href->gaze to point (xp, yp). This function requires HREFPT data in
	FSAMPLE. The function \c eyelink_initialize_mapping() should be called
	before calling this function.
	@param xp X point to apply href->gaze.
	@param yp Y point to apply href->gaze.
	@param sample Sample.
	@return \c 0 if OK, \c -1 if there is a math error, \c -2 if the tracker
			does not support this operation.
 */
INT32 ELCALLTYPE eyelink_href_to_gaze( float *xp, float *yp, FSAMPLE *sample);

/*! @ingroup extra_data
	Apply gaze->href to point (xp, yp). This function requires HREFPT data in
	FSAMPLE. The function \c eyelink_initialize_mapping() should be called
	before calling this function.
	@param xp X point to apply gaze->href.
	@param yp Y point to apply gaze->href.
	@param sample Sample.
	@return \c 0 if OK, \c -1 if there is a math error, \c -2 if the tracker
			does not support this operation.
 */
INT32 ELCALLTYPE eyelink_gaze_to_href( float *xp, float *yp, FSAMPLE *sample);

/*! @ingroup extra_data
	Convenient function to calculate the href angle.
	@param x1 Point 1 x.
	@param y1 Point 1 y.
	@param x2 Point 2 x.
	@param y2 Point 2 y.
 */
float ELCALLTYPE eyelink_href_angle(float x1,float y1, float x2, float y2);


/*! @ingroup extra_data
	Convenient function to calculate the href resolution.
	@param x X value of point.
	@param y Y value of point.
	@param xres Pointer to return the x resolution.
	@param yres Pointer to return the y resolution.
 */
void  ELCALLTYPE eyelink_href_resolution(float x,float y, float *xres, float *yres);

/*! @ingroup display
	\deprecated Use eyelink_draw_cross_hair() instead.

	Value is image coord scaled so l,t=0, r,b=8192
	Values may be negative or beyond image limits
	Value is 0x8000 in X (or L) field if crosshair is not to be drawn
	Head camera: indexes 0..3 are markers
	Eye camera:
		Index 0 is pupil center
		Index 1 is CR center
		Index 2 is pupil-limit box left, top
		Index 3 is pupil-limit box right, bottom
	@param xhairs_on Set to indicate if xhairs enabled on display (may be \c NULL).
	@param x array of 4 to be filled to return x data
	@param y array of 4 to be filled to return y data
	@return Channel number (\c 1 = left, \c 2 = head, \c 3 = right).


 */
int ELCALLTYPE get_image_xhair_data(INT16 x[4], INT16 y[4], INT16 *xhairs_on);



/*@internal
 */
/* as of Dec 1, 2006 this is deprecated. use  eyelink_get_extra_raw_values_v2 */
int ELCALLTYPE eyelink_get_extra_raw_values(FSAMPLE *s,FSAMPLE_RAW *rv);
/*@internal
 */
int ELCALLTYPE eyelink_get_extra_raw_values_v2(FSAMPLE *s,int eye,FSAMPLE_RAW *rv);




/*!
@addtogroup velocity_acceleration
@{
*/
#define FIVE_SAMPLE_MODEL      1
#define NINE_SAMPLE_MODEL      2
#define SEVENTEEN_SAMPLE_MODEL 3
#define EL1000_TRACKER_MODEL   4
/*!
@}
*/



/*! @ingroup velocity_acceleration
	Calculates left x velocity, left y velocity, right x velocity and right y velocity from queue of samples.
	@param[in] slen Sample model to use for velocity calculation. Acceptable models
				are \c FIVE_SAMPLE_MODEL, \c NINE_SAMPLE_MODEL, \c SEVENTEEN_SAMPLE_MODEL and
				\c EL1000_TRACKER_MODEL.
	@param[out] xvel Left and right x velocity.
				Expects an array of 2 elements of floats. The array is filled with left and right
				velocity values.  Upon return of this function xvel[0] contains the left x velocity
				data and xvel[1] contains right x velocity data.  If velocity cannot be calculated
				for any reason(eg. insufficient samples, no data) MISSING_DATA is filled for the given
				velocity.
	@param[out] yvel Left and right y velocity.
				Expects an array of 2 elements of floats. The array is filled with left and right
				velocity values.  Upon return of this function yvel[0] contains the left y velocity
				data and xvel[1] contains right y velocity data.  If velocity cannot be calculated
				for any reason(eg. insufficient samples, no data) MISSING_DATA is filled for the given
				velocity.
	@param[out] vel_sample Velocity for sample.
				Expects a FSAMPLE structure to fill in the sample, the velocity is calculated for.

  @code
#include <stdio.h>
#include <core_expt.h>

int main(int argc, char ** argv)
{
	if(open_eyelink_connection(0)) // connect to tracker
	{
		return -1;
	}

	eyecmd_printf("link_sample_data  = LEFT,RIGHT,GAZE,GAZERES,AREA,STATUS"); // tell the tracker to stuff the sample with
	if(start_recording(0,0,1,0)) // start recording failed.
	{
		close_eyelink_connection();
		return -1;
	}

	if(!eyelink_wait_for_block_start(100, 1, 0)) // wait for block start
	{
		stop_recording();
		close_eyelink_connection();
		return -1; 
	}
	else
	{
		UINT32 st = current_time();
		while(current_time()-st<10000) // record for 10 seconds
		{
			FSAMPLE fs;
			float xvel[2]; 
			float yvel[2];
			if(check_recording()!=0) 
			{
				close_eyelink_connection();
				return -4; // recording aborted.
			}
			eyelink_calculate_velocity_x_y(FIVE_SAMPLE_MODEL,xvel,yvel,&fs);
			printf("%lu %f %f %f %f\n",fs.time,xvel[0],yvel[0], xvel[1], yvel[1]);
			pump_delay(100); // we check the velocity every 100 ms.
		}
		stop_recording();
		close_eyelink_connection();
		return 0;
	}
		

}
  @endcode
 */
int ELCALLTYPE eyelink_calculate_velocity_x_y(int slen, float xvel[2], float yvel[2], FSAMPLE *vel_sample);

/*! @ingroup velocity_acceleration
	Calculates overall velocity for left and right eyes separately.
	@param[in] slen Sample model to use for velocity calculation. Acceptable models
				are \c FIVE_SAMPLE_MODEL, \c NINE_SAMPLE_MODEL, \c SEVENTEEN_SAMPLE_MODEL and
				\c EL1000_TRACKER_MODEL.
	@param[out] vel A float array of 2 to fill in the calculated results. Upon return of this function,
				vel[0] will contain overall velocity for left eye and vel[1] will contain overall velocity
				for right eye.  If velocity cannot be calculated for any reason(eg. insufficient
				samples, no data) MISSING_DATA is filled for the given velocity.
	@param[out] vel_sample Velocity for sample.
				Expects a FSAMPLE structure to fill in the sample, the velocity is calculated for.
 */
int ELCALLTYPE eyelink_calculate_velocity(int slen, float vel[2], FSAMPLE *vel_sample);
/*! @ingroup velocity_acceleration
	Calculates overall velocity and acceleration for left and right eyes separately.
	@param[in] slen Sample model to use for velocity calculation. Acceptable models
				are \c FIVE_SAMPLE_MODEL, \c NINE_SAMPLE_MODEL, \c SEVENTEEN_SAMPLE_MODEL and
				\c EL1000_TRACKER_MODEL.
	@param[out] vel A float array of 2 to fill in the calculated velocity results. Upon return of this function,
				vel[0] will contain overall velocity for left eye and vel[1] will contain overall velocity
				for right eye.  If velocity cannot be calculated for any reason(eg. insufficient
				samples, no data) MISSING_DATA is filled for the given velocity.
	@param[out] acc A float array of 2 to fill in the calculated acceleration results. Upon return of this function,
				acc[0] will contain overall acceleration for left eye and acc[1] will contain overall acceleration
				for right eye.  If acceleration cannot be calculated for any reason(eg. insufficient
				samples, no data) MISSING_DATA is filled for the given acceleration.
	@param[out] vel_sample Velocity for sample.
				Expects a FSAMPLE structure to fill in the sample, the velocity is calculated for.
 */
int ELCALLTYPE eyelink_calculate_overallvelocity_and_acceleration(int slen, float vel[2], float acc[2], FSAMPLE *vel_sample);

/*! @ingroup message_file
	Very similar to eyemsg_printf, with the following features:
	\arg Optionally write the timestamped message to file instead of sending over the link to tracker.
	\arg Optional time of event.
	@param t optional time of event (0 = NOW)
	@param fmt format messages
 */
INT16 ELCALLTYPE timemsg_printf(UINT32 t, char *fmt, ...);
/*! @ingroup message_file
    Creates message file, once open call to timemsg_printf(), will not send messages to tracker.
	Messages are kept in a queue if the application is in realtime mode, and written to disk on
	non real-time mode except when close_message_file() is called while in real-time mode.
	@param[in] fname Message file name
 */
int ELCALLTYPE open_message_file(char *fname);
/*! @ingroup message_file
	Flush and close message file, opened by open_message_file.
 */
void ELCALLTYPE close_message_file(void);



 /*!
@addtogroup display
@{
*/
#define CR_HAIR_COLOR           1 
#define PUPIL_HAIR_COLOR        2 
#define PUPIL_BOX_COLOR         3 
#define SEARCH_LIMIT_BOX_COLOR  4 
#define MOUSE_CURSOR_COLOR      5 
/*!
 @}
*/


typedef struct _CrossHairInfo CrossHairInfo;
/*! @ingroup display
	\brief Structure to hold cross hair information.

	Structure to hold cross hair information that are needed to
	draw the cross hair on camera images.
 */
struct _CrossHairInfo
{
	short majorVersion;/*!< for the moment this should be set to 1. */
	short minorVersion;/*!< for the moment this should be set to 0. */
	/*!
	Display width of the camera image. This need not to match the
	size given at setup_image_display_hook
	*/
	int w;

	/*!
	Display height of the camera image. This need not to match
	the size given at setup_image_display_hook
	*/
	int h;

	/*!
	Private data used by internals of  eyelink_draw_cross_hair.
	This pointer should not be touched externally
	*/
	void *privatedata;

	/*!
	Attribute to hold any user data. Any data set here
	untouched by eyelink_draw_cross_hair.
	*/
	void *userdata;
	/*!
		drawLine shoud draw a line from (x1,y1) -> (x2,y2)
		with the given color
		@param[in] x1
	*/
	void (*drawLine)(CrossHairInfo *dt, int x1, int y1, int x2, int y2, int colorindex);
	/*!
		drawLozenge shoud draw circle if the width and height are equal. otherwise
		find the smallest width and draw semi-circles on the longest side at both ends and connect
		the semi circles with lines.
	*/
	/*
		it looks more like
		  ________
		 /        \
		(          )
		 \________/

	*/
	void (*drawLozenge)(CrossHairInfo *dt, int x, int y, int w, int h, int colorindex);
	/*!
		getMouseState shoud fill values for (x,y) with current mouse position and the
		state = 1 if pressed. the x and y values are respective to top left hand corner of the image
	*/
	void  (*getMouseState)(CrossHairInfo *dt, int *x, int *y, int *state);

	int reserved1; /*!< for future use*/
	int reserved2; /*!< for future use*/
	int reserved3; /*!< for future use*/
	int reserved4; /*!< for future use*/



};

/*! @ingroup display
	Convenient function to draw cross hair on camera image. It is very tedious to draw and maintain
	cross hair drawing. This is due to evolving eyelink camera image protocol and the introduction
	of newer trackers and still single library handle all eyelink trackers.  eyelink_draw_cross_hair
	fixes this issue by allowing the drawing function to draw to the device contexts and does all
	magic of extracting cursor info from the tracker.

	eyelink_draw_cross_hair calls drawLine(), drawEllipse() and getMouseState() to draw the cross hair.
	eyelink_draw_cross_hair expects both width(w) and height(h) are correct and the function pointers to
	drawLine, drawEllipse and getMouseState are not NULL.
	@param[in] chi initialized CrossHairInfo structure.
 */
INT32 ELCALLTYPE eyelink_draw_cross_hair(CrossHairInfo *chi);



// copies pointer to DLL version string
/*! 
	Returns the eyelink_core library version number.
	@param[out] c fills in the version number of th eyelink_core library.  
 */
void ELCALLTYPE eyelink_dll_version(char FARTYPE *c);


#ifdef __cplusplus     /* For C++ compilation */
}
#endif
#endif
