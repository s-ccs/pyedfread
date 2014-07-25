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
    \file eyelink.h
    \brief Declarations and constants for basic EyeLink functions, Ethernet link, and timing.
 */



#ifndef __SRRESEARCH__EYELINK_H__
#define __SRRESEARCH__EYELINK_H__

#ifndef SIMTYPESINCL
  #include "eye_data.h"
#endif

#ifndef SIMLINKINCL
#define SIMLINKINCL

#ifdef __cplusplus     /* For C++ compilation */
extern "C" {
#endif

/****** ERROR AND RETURN CODES *********/

/* LINK RETURN CODES                   */
/* These are produced from the link interface */
/*! \addtogroup eyelink_mode
  *  @{
  */
#define OK_RESULT                 0		/*!< LINK RETURN CODES: OK */
#define NO_REPLY                  1000	/*!< LINK RETURN CODES: no reply yet (for polling test)  */
#define LINK_TERMINATED_RESULT   -100	/*!< LINK RETURN CODES: can't send or link closed        */


/*! EyeLink TRACKER RETURN CODES: operation aborted (calibration) */
#define ABORT_RESULT         27


/* these are returned from the eye tracker upon command execution */

#define UNEXPECTED_EOL_RESULT   -1	/*!< COMMAND PARSE ERRORS: not enough data                     */
#define SYNTAX_ERROR_RESULT     -2	/*!< COMMAND PARSE ERRORS: unknown command, unknown variable etc. */
#define BAD_VALUE_RESULT        -3	/*!< COMMAND PARSE ERRORS: value is not right for command or tracker state */
#define EXTRA_CHARACTERS_RESULT -4	/*!< COMMAND PARSE ERRORS: bad format or too many values       */
/*!
  *  @}
  */
#ifndef ELCALLTYPE                   /* ELCALLTYPE used for Windows platform
                                        or other special calling formats */
 #if defined(WIN32) || defined(WIN16) /* some sort of Windows: using DLL */
  #include <windows.h>
  #define ELCALLTYPE WINAPI
 #else
  #define ELCALLTYPE                  /* delete if not used */
 #endif
#endif

#ifndef ELCALLBACK                    /* ELCALLTYPE used for Windows platform
                                         or other special calling formats */
 #if defined(WIN32) || defined(WIN16)  /* some sort of Windows: using DLL */
  #include <windows.h>
  #define ELCALLBACK CALLBACK
 #else
  #define ELCALLBACK    /* delete if not used */
 #endif
#endif


/****** INITIALIZE EYELINK LIBRARY ******/


/*! @ingroup init_eyelink
	Use this function to initialize the EyeLink library.  This will also start
	the millisecond clock.  No connection is attempted to the eyetracker yet.
	It is preferable to call open_eyelink_connection(\c -1)  instead, as this
	prepares other parts of the DLL for use.

	@remarks MUST BE FIRST CALL TO LINK INTERFACE.
	@param bufsize Size of sample buffer.  \c 60000 is the maximum allowed. (\c 0 for default)
	@param options Text specifying initialization options.  Currently no options are supported.
					(\c NULL or "" for defaults)
	@return \c 0 if failed, nonzero (\c -1 or TSR SWI number) if success.

	\b Example:
	\code
	// This program shows the use of open_eyelink_system() and close_eyelink_system ()

	#include <eyelink.h>
	int dummy = 0;  // Sets the connection type

	// Initialize the EyeLink library
	if(!open_eyelink_system(0, ""))
	{
	    printf("\nERROR: Cannot initialize eyelink library\n");
	    return -1;
	}

	if(dummy)
	    i = eyelink_dummy_open(); // Starts a dummy connection
	else
	    i = eyelink_open();  // Connects to the tracker

	...
	// Code for the setup, recording, and cleanups

	// Now, the following code closes the eyelink connection
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

	\sa \c close_eyelink_connection(), \c close_eyelink_system(), \c eyelink_close() and \c eyelink_open()
 */
UINT16 ELCALLTYPE open_eyelink_system(UINT16 bufsize, char FARTYPE *options);

/*! @ingroup init_eyelink
	Sets the node name of this computer (up to 35 characters).

	@param name String to become new name.

	\b Example:
	\code
	// This program illustrates the use of eyelink_set_name()
	#include <eyelink.h>

	ELINKNODE node;  // This will hold application name and address

	// Sets the EyeLink host address, if tracker IP address is different
	// from the default "100.1.1.1"
	if (set_eyelink_address("100.1.1.7"))
	    return -1;

	// Initializes EyeLink library, and opens connection to the tracker
	if(open_eyelink_connection(0))
	    return -1;

	// set our EyeLink node name
	eyelink_set_name("Newapplication");

	// Verify the name set by getting copy of node data
	// Since we are checking our own data, set resp = 0
	if(eyelink_get_node(0, &node) == OK_RESULT)
	    eyemsg_printf("%s", node.name);
	\endcode

	\b Output:
	\code
	MSG	2248248 Newapplication
	\endcode

	\sa \c eyelink_get_node(), \c open_eyelink_connection() and \c set_eyelink_address()
 */
void ELCALLTYPE eyelink_set_name(char FARTYPE *name);

/*! @ingroup init_eyelink
	Resets the EyeLink library, releases the system resources used by the millisecond clock.

	@remarks MUST DO BEFORE EXITING.

	\b Example: See \c open_eyelink_system()

	\sa \c open_eyelink_system(), \c eyelink_close() and \c set_offline_mode()
 */
void ELCALLTYPE close_eyelink_system(void);

/****** MICROSECOND CLOCK ******/
/* These time features are available with the TSR. */
/* On some platforms, the millisecond timing may   */
/* be coarse, and there is no microsecond timing   */

/*! @ingroup access_time_local
	See \c current_time()
 */
#define current_msec() current_time()


/*! @ingroup access_time_local
	Returns the current millisecond since the initialization.

 	@remarks If the eyelink_exptkit library is not initialized, or initialized multiple times,
 				the return value is invalid and the return value is unpredictable.  The call
 				to \c current_msec() is always equivalent to \c current_time().
	@return The current millisecond since the initialization of the library.

	\b Example:
	\code
	// This program illustrates the use of current_msec
	#include <eyelink.h>
	#include <stdio.h>

	eyemsg_printf("Delay test starts: %ld", current_msec());
	msec_delay(100);
	eyemsg_printf("Delay test ends: %ld", current_time());
	\endcode

	\b Output:
	\code
	MSG	4532575 Delay test starts: 5236
	MSG	4532671 Delay test ends: 5336
	\endcode

	\sa \c current_msec(), \c current_micro(), \c current_usec(), \c eyelink_tracker_time() and \c msec_delay()
 */
UINT32 ELCALLTYPE current_time(void);

/*! @ingroup access_time_local
	Returns the current microsecond since the initialization.
	Equivalent to \c current_usec() and \c current_double_usec().

	@param m Pointer to MICRO structure.
	@return The current microsecond since the initialization of the library, modulo 2^32.
			It can also fill in the \c MICRO structure if the pointer is not \c NULL.

	\b Example: See \c current_usec()

	\sa \c current_usec(), \c current_double_usec(), \c current_msec(), \c current_time() and \c msec_delay()
 */
UINT32 ELCALLTYPE current_micro(MICRO FARTYPE *m);

/*! @ingroup access_time_local
	Returns the current microsecond since the initialization.
	Equivalent to \c current_micro() and \c current_double_usec().

	@remarks If the eyelink_exptkit library is not initialized, or initialized multiple times,
				the return value is invalid and unpredictable.  The call to
				\c current_usec() is equivalent to \c current_micro(NULL).  The function is very
				platform dependent. Platforms such as windows 95, 98, ME may not return usec
				properly. Ie. It may return <tt>current_time() * 1000</tt>.
	@return The current microsecond since the initialization of the library, modulo 2^32.

	\b Example:
	\code
	// This program illustrates the use of current_micro and current_usec

	#include <eyelink.h>
	#include <stdio.h>

	MICRO m1, m2;  // Special high-resolution time structure

	// Get the current subject PC time in microseconds
	current_micro(&m1);
	eyemsg_printf("Delay test starts: %ld", current_usec());

	// Delay for 100 msec
	msec_delay(100);

	// Get the current subject PC time again
	current_micro(&m2);
	eyemsg_printf("Delay test ends: %ld", current_usec());

	// Calculate the actual amount of delay
	eyemsg_printf("Total Delay: %6.3f",
	    m2.msec + m2.usec/1000.0 - m1.msec + m1.usec/1000.0);
	\endcode

	\b Output:
	\code
	MSG	5441107 Delay test starts: 4610094
	MSG	5441206 Delay test ends: 4710005
	MSG	5441206 Total Delay: 100.003
	\endcode

	\sa \c current_micro(), \c current_double_usec(), \c current_msec(), \c current_time() and \c msec_delay()
 */
UINT32 ELCALLTYPE current_usec(void);

/* delays n msec using current_msec() */
/*! @ingroup access_time_local
	Does a unblocked delay using \c current_time().

	@param n n milliseconds to delay.

	\b Example:
	\code
	// This program illustrates the use of msec_delay
	#include <eyelink.h>
	#include <stdio.h>

	eyemsg_printf("Delay test starts: %ld", current_msec());
	// delay for 100 msec
	msec_delay(100);
	eyemsg_printf("Delay test ends: %ld", current_time());
	\endcode

	\b Output:
	\code
	MSG	4346690 Delay test starts: 12768
	MSG	4346791 Delay test ends: 12868
	\endcode

	\sa \c current_msec() and \c pump_delay()
 */
void ELCALLTYPE msec_delay(UINT32 n);

/* NOTE: not in SIMTSR--introduced for v3.0 of Windows DLL */
/*! @ingroup access_time_local
	Returns the current microsecond as double (56 bits) since the initialization.
	Equivalent to \c current_micro() and \c current_usec().

	@return The current microsecond as a double value since the initialization of the library, modulo 2^32.

	\b Example: See \c current_usec()


	\sa \c current_micro(), \c current_usec(), \c current_msec(), \c current_time() and \c msec_delay()
 */
double current_double_usec(void);


/****** INITIALIZE EYE TRACKER CONNECTION ******/
/* ELINKADDR: a 16-byte address, works with any link type */

/*! @ingroup init_eyelink
	The broadcast address for the eye trackers.
 */
extern ELINKADDR eye_broadcast_address;

/*! @ingroup init_eyelink
	The broadcast address for the remotes.
 */
extern ELINKADDR rem_broadcast_address;

/*! @ingroup init_eyelink
	This EyeLink node's address for the link implementation.
 */
extern ELINKADDR our_address;

/* ERROR CODES returned by connection functions */

#define LINK_INITIALIZE_FAILED -200	/*!< @ingroup init_eyelink
											CONNECTION ERROR CODES: can't use link */
#define CONNECT_TIMEOUT_FAILED -201	/*!< @ingroup init_eyelink
											CONNECTION ERROR CODES: timed out waiting for reply */
#define WRONG_LINK_VERSION     -202	/*!< @ingroup init_eyelink
											CONNECTION ERROR CODES: wrong TSR or source version */
#define TRACKER_BUSY           -203	/*!< @ingroup init_eyelink
											CONNECTION ERROR CODES: tracker already connected   */

/* make connection using an ELINKADDR tracker node address */
/* can be broadcast_address to connect to single tracker   */
/* connect will fail if <busytest> is 1                    */
/* and tracker is connected to another remote              */
/*! @ingroup init_eyelink
	Allows the computer to connect to tracker, where the tracker is on the same network.

	@param node Must be an address returned by \c eyelink_poll_trackers() and
				\c eyelink_poll_responses() to connect to any tracker.
	@param busytest If non-zero the call to \c eyelink_open_node() will not disconnect an existing connection.
	@return \c 0 if successful.\n
			\c LINK_INITIALIZE_FAILED  if link could not be established.\n
			\c CONNECT_TIMEOUT_FAILED  if tracker did not respond.\n
			\c WRONG_LINK_VERSION if the versions of the EyeLink library and tracker are incompatible.\n
			\c TRACKER_BUSY if \c &lt;busytest&gt; is set, and tracker is connected to another computer.\n

	\b Example:
	\code
	// This program illustrates the case of making connection using an ELINKADDR tracker node address

	ELINKADDR node;				// EyeLink address node

	// Initialize the EyeLink DLL
	if(open_eyelink_connection(-1))
	    return -1;

	// Converts tracker IP address to an EyeLink node
	text_to_elinkaddr("100.1.1.1", node, 0);

	// Makes connection using an ELINKADDR tracker node address; checks
	// whether the tracker is already connected with another application
	if (eyelink_open_node(node, 1))
	    return -1;
	\endcode

	\sa \c eyelink_node_receive(), \c eyelink_node_send(), \c eyelink_node_send_message() and \c eyelink_open()
 */
INT16 ELCALLTYPE eyelink_open_node(ELINKADDR node, INT16 busytest);

/*! @ingroup init_eyelink
	Attempts to open a link connection to the EyeLink tracker.  Simple connect to single Eyelink tracker.
	Equivalent to \c eyelink_open_node(broadcast_address, 0).

	@remarks The tracker address can be set by calling the function \c set_eyelink_address().  If this
				address was "255.255.255.255" the call to \c eyelink_open() will "broadcast" a request
				to any tracker, however this may fail if multiple Ethernet cards are installed.
				For the broadcast option to work use EyeLink I v2.1 or higher, EyeLink II v1.1 or higher.
				Before using this command, call either \c open_eyelink_connection(-1) or
				\c open_eyelink_system () to prepare the link for use.

	@return \c 0 if successful.\n
			\c LINK_INITIALIZE_FAILED  if link could not be established.\n
			\c CONNECT_TIMEOUT_FAILED  if tracker did not respond.\n
			\c WRONG_LINK_VERSION if the versions of the EyeLink library and tracker are incompatible.\n

	\b Example:
	\code
	// This program illustrates the use of eyelink_open()

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

	\sa \c eyelink_broadcast_open(), \c eyelink_close(), \c eyelink_dummy_open(), \c eyelink_open_node(),
		\c open_eyelink_connection(), \c open_eyelink_system() and \c set_eyelink_address()
 */
INT16 ELCALLTYPE eyelink_open(void);

/* used by second remote to eavedrop on tracker */
/* disables all control functions               */
/* can only be used with single tracker on link */
/*! @ingroup init_eyelink
	Allows a third computer to listen in on a session between the eye tracker and a controlling
	remote machine.  This allows it to receive data during recording and playback, and to monitor
	the eye tracker mode.  The local computer will not be able to send commands to the eye tracker,
	but may be able to send messages or request the tracker time.

	@remarks May not function properly, if there are more than one Ethernet cards installed.
	@return \c 0 if successful.\n
			\c LINK_INITIALIZE_FAILED  if link could not be established.\n
			\c CONNECT_TIMEOUT_FAILED  if tracker did not respond.\n
			\c WRONG_LINK_VERSION if the versions of the EyeLink library and tracker are incompatible.\n

	\b Example:
	\code
	// This program illustrates the use of eyelink_broadcast_open(); see the COMM_SIMPLE and
	// BROADCAST templates for more examples

	#include <eyelink.h>
	#include <stdio.h>

	// Initialize the EyeLink DLL
	if(open_eyelink_connection(-1))
	    return -1;

	// set our network name
	eyelink_set_name("Broadcast");

	...
	// Extra code here to check for the tracker status or
	// wait for the go-signal from the other application

	// Starts the broadcast connection to the tracker
	if(eyelink_broadcast_open())
	{
	    printf("Cannot open broadcast connection to tracker");
	    return -1;
	}

	// enable link data reception by EyeLink DLL
	eyelink_reset_data(1);
	// NOTE: this function can discard some link data
	eyelink_data_switch(RECORD_LINK_SAMPLES | RECORD_LINK_EVENTS);
	\endcode

	\sa \c eyelink_close(), \c eyelink_dummy_open(), \c eyelink_is_connected(), \c eyelink_open() and
	\c eyelink_open_node()
 */
INT16 ELCALLTYPE eyelink_broadcast_open(void);

/* used for debugging: all functions return a valid code */
/* does not connect to tracker */
/*! @ingroup init_eyelink
	Sets the EyeLink library to simulate an eyetracker connection.  Functions will return
	plausible values, but no data.

	@remarks The function \c eyelink_is_connected() will return \c -1 to indicate a simulated connection.
	@return Always returns \c 0.

	\b Example:
	\code
	// This program illustrates the use of eyelink_dummy_open

	#include <eyelink.h>

	// Initialize the EyeLink DLL; otherwise the application will crash
	// when using the eyelink_dummy_open()function!
	if(open_eyelink_connection(-1))
	    return -1;

	// Opens a "dummy" connection for debugging the application
	eyelink_dummy_open();
	\endcode

	\sa \c eyelink_broadcast_open(), \c eyelink_open() and \c eyelink_open_node()
 */
INT16 ELCALLTYPE eyelink_dummy_open(void);

/* close connection to tracker.                                */
/* <send_msg> must be 0 if broadcast (eavedropping) connection */
/*! @ingroup init_eyelink
	Sends a disconnect message to the EyeLink tracker, resets the link data system.

	@param send_msg Usually \c &lt;send_msg&gt; is \c 1. However, \c 0 can be used to reset the eyelink system
			if it is listening on a broadcast session.
	@return \c 0 if successful, otherwise link error.

	\b Example:
	\code
	// This program illustrates the use of eyelink_close

	#include <eyelink.h>

	// Checks whether the tracker is still connected
	if(eyelink_is_connected())
	{
	   set_offline_mode();      	// off-line mode
	   eyecmd_printf("close_data_file");
	   eyelink_close(1);         // disconnect from tracker
	}
	\endcode

	\sa \c eyelink_broadcast_open(), \c eyelink_open(), \c eyelink_dummy_open() and \c eyelink_open_node()
 */
INT16 ELCALLTYPE eyelink_close(INT16 send_msg);

/*! @ingroup access_time_local
	Initializes the high frequency clock.

	With TSR interface under msdos, Start/stop timing resources.

	@param enable Usually \c &lt;enable&gt; is \c 1. However, in MS-DOS passing \c 0 will cleanup the timing resources.
	In other platforms passing \c 0 has no effect.
 */
INT16 ELCALLTYPE eyelink_reset_clock(INT16 enable);

/*! @ingroup init_eyelink
	Checks whether the connection to the tracker is alive.

	@remarks Call this routine during loops and wherever the experiment might lock up if
				the tracker is shut down.  Exit the experiment (by terminating loops and
				returning from all calls) if this returns \c 0.
	@return \c 0 if link closed.\n
			\c -1 if simulating connection.\n
		 	\c 1 for normal connection.\n
 			\c 2 for broadcast connection (NEW for v2.1 and later).\n

	\b Example:
	\code
	// This program illustrates the use of eyelink_is_connected( when performing a pre-trial drift correction

	#include <eyelink.h>

	while(1)
	{
	    // Checks link often so we can exit if tracker stopped
	    if(!eyelink_is_connected())
	        return ABORT_EXPT;

	    // Performs drift correction with target drawn in the center
	    error = do_drift_correct(SCRWIDTH/2, SCRHEIGHT/2, 1, 1);

	    // repeat if ESC was pressed to access Setup menu
	    if(error!=27) break;
	}
	\endcode

	\sa \c eyelink_close() and \c eyelink_open()
 */
INT16 ELCALLTYPE eyelink_is_connected(void);


/* sets quiet mode (for use with broadcast)                            */
/*! @ingroup init_eyelink
	Controls the level of control an application has over the tracker.

	@remarks This is used in combination with broadcast mode (multiple applications connected to one tracker)
				to ensure that "listener" applications do not inadvertently send commands, key presses, or
				messages to the tracker.  This is mostly useful when quickly converting an existing
				application into a listener.
	@param mode \c 0 to allow all communication.\n
				\c 1 to block commands (allows only key presses, messages, and time or variable read requests).\n
				\c 2 to disable all commands, requests and messages.\n
				\c -1 to just return current setting.\n
	@return The previous settings.

	\b Example:
	\code
	// This program illustrates the use of eyelink_quiet_mode() to control message sending from a Broadcast
	// application to the tracker

	#include <eyelink.h>
	#include <stdio.h>

	// Starts the broadcast connection to the tracker
	if(eyelink_broadcast_open())
	{
	    printf("Cannot open broadcast connection to tracker");
	    return -1;
	}

	// enable link data reception by EyeLink DLL
	eyelink_reset_data(1);
	// NOTE: this function can discard some link data
	eyelink_data_switch(RECORD_LINK_SAMPLES | RECORD_LINK_EVENTS);

	// The following codes test the message sending from the BROADCAST
	// application to tracker with eyelink_quiet_mode()
	eyelink_quiet_mode(0);	// Allows for message sending
	eyelink_send_message("This should be recorded in the EDF file");
	eyelink_quiet_mode(2);   // Disables message sending
	eyelink_send_message("This should not appear in the EDF file");
	eyelink_quiet_mode(0);   // Allows for message sending again
	eyelink_send_message("This should appear in the EDF file");
	\endcode

	\b Output:
	\code
	MSG	3304947 This message should be recorded in the EDF file
	MSG	3304947 This message should appear in the EDF file
	\endcode

	\sa \c eyelink_broadcast_open() and \c eyelink_open()
 */
INT16 ELCALLTYPE eyelink_quiet_mode(INT16 mode);

/********** LINK NODE FUNCTIONS *********/
/* sends polling request to all trackers for ID */
/* returns: OK_RESULT or LINK_TERMINATED_RESULT */
/*! @ingroup init_eyelink
	Asks all trackers (with EyeLink software running) on the network to send their names and node address.

	@return \c 0 if successful, otherwise link error.

	\b Example: See \c eyelink_poll_remotes()

	\sa \c eyelink_poll_remotes() and \c eyelink_poll_responses()
 */
INT16 ELCALLTYPE eyelink_poll_trackers(void);

/* sends polling request to all remotes for ID  */
/* returns: OK_RESULT or LINK_TERMINATED_RESULT */
/*! @ingroup init_eyelink
	Asks all non-tracker computers (with EyeLink software running) on the network to send their names
	and node address.

	@return \c 0 if successful, otherwise link error.

	\b Example:
	\code
	// This program illustrates the use of eyelink_poll_remotes(). See COMM_SIMPLE/COMM_LISTERNER
	// templates for the complete example

	#include <eyelink.h>

	ELINKADDR listener_address;  // Address of listener application

	// Finds the listener application
	int check_for_listener(void)
	{
	    int i, n;
	    ELINKNODE node;  // This will hold application name and address

	    eyelink_poll_remotes(); // Polls network for EyeLink applications
	    pump_delay(500);        // Gives applications time to respond

	    n = eyelink_poll_responses();  // How many responses?
	    //  Responses 1 to n are from other applications
	    for(i = 1; i<= n; i++)
	    {
	        if(eyelink_get_node(i, &node) < 0)
	            return -1;  // error: no such data

	        // Found the "Comm_listener" application
	        if(!_stricmp(node.name, "comm_listener"))
	        {
	            // Stores the listener's address
	            memcpy(listener_address, node.addr, sizeof(ELINKADDR));

	            // Send a message to the listener
	            eyelink_node_send(listener_address, "NAME comm_simple", 40);

	            ...
	            // Other code here to check responses from the listener
	            ...

	            return 0;   // all communication checks out
	        }
	    }
	    return -1;    // no listener node found
	}
	\endcode

	\sa \c eyelink_poll_responses() and \c eyelink_poll_trackers()
 */
INT16 ELCALLTYPE eyelink_poll_remotes(void);

/* checks for polling responses received    */
/*! @ingroup init_eyelink
	Returns the count of node addresses received so far following the call of
	\c eyelink_poll_remotes() or \c eyelink_poll_trackers().

	@remarks You should allow about 100 milliseconds for all nodes to respond.  Up to 4 node
				responses are saved.
	@return Number of nodes responded.  \c 0 if no responses.

	\b Example: See \c eyelink_poll_remotes()

	\sa \c eyelink_poll_remotes() and \c eyelink_poll_trackers()
 */
INT16 ELCALLTYPE eyelink_poll_responses(void);

/* gets copy of node data (in order of reception)            */
/* returns: OK_RESULT if node data exists, else -1           */
/*! @ingroup init_eyelink
	Reads the responses returned by other trackers or remotes in response to
	\c eyelink_poll_trackers() or \c eyelink_poll_remotes(). It can also read the tracker
	broadcast address and remote broadcast addresses.

	@param resp Nmber of responses to read:\c 0 gets our data, \c 1 get first response, \c 2 gets the
				second response, etc. \c -1 to read the tracker broadcast address. \c -2 to read
				remote broadcast addresses.
	@param data Pointer to buffer of \c ELINKNODE type, to hold name and \c ELINKADDR of the respondent.
	@return \c 0 if OK, \c -1 if node response number is too high.

	\b Example: See \c eyelink_poll_remotes()

	\sa \c eyelink_node_receive(), \c eyelink_node_send() and \c eyelink_poll_remotes()
 */
INT16 ELCALLTYPE eyelink_get_node(INT16 resp, void FARTYPE *data);


/********** INTER-REMOTE COMMUNICATIONS ********/
/* NOTE: old packets are discarded if new packet received before reading! */

/* send an unformatted packet to another remote     */
/* returns: OK_RESULT or LINK_TERMINATED_RESULT     */
/*! @ingroup messaging
	Sends a given data to the given node.

	@param node \c ELINKADDR node address.
	@param data Pointer to buffer containing data to send.
	@param dsize Number of bytes of data. Maximum \c ELREMBUFSIZE bytes.
	@return \c 0 if successful, otherwise link error.

	\b Example:
	\code
	// The following two code pieces show the exchanges of messaging between two
	// remote applications (SENDER and LISTENER).  In the first program, the SENDER sends
	// a "START_RECORD" message to the LISTENER application and wait for an "echo" message.
	// The second program shows the LISTENER application receives the "START_RECORD" message
	// and sends an "echo" message

	#include <eyelink.h>

	ELINKADDR listener_address; // Address of the listener application
	char text_message[256], echo_message[256];

	...
	// Code for retrieving the listener's address; See COMM_SIMPLE
	...

	sprintf(text_message, "START_RECORD");
	eyelink_node_send(listener_address, text_message, 40);

	// Now checks for the echo response from the listener
	while (1)
	{
	    // get the message from the listener application
	    k = eyelink_node_receive(listener_address, echo_message);
	    if (k > 0 && strstr(echo_message, text_message))
				break;
	    ...
	    // Other code for error handling
	    ...
	}
	\endcode

	\code
	// This program shows the LISTENER application receives the "START_RECORD" message and sends an "echo" message

	#include <eyelink.h>

	ELINKADDR sender_address; // Address of the sender application
	char text_message[256], echo_message[256];

	// get the message from the sender application
	k = eyelink_node_receive(sender_address, text_message);

	if(k > 0 && !_strnicmp(text_message, "START_RECORD", 12))
	{
	    eyemsg_printf(text_message);
	    error = start_recording(1,1,1,1); // Starts recording
	    if(error != 0)
	        return error;        // Return error code if failed

	    sprintf(echo_message, "ECHO %s", text_message);
	    // Sends the echo message to the sender application
	    eyelink_node_send(sender_address, echo_message, 60);
	}
	\endcode

	\sa \c eyelink_get_node(), \c eyelink_node_receive() and \c eyelink_open_node()
 */
INT16 ELCALLTYPE eyelink_node_send(ELINKADDR node, void FARTYPE *data,
                                   UINT16 dsize);

/* checks for packet from another remote        */
/* packet data is copied into <data> buffer     */
/* sender address is copied into <addr> buffer  */
/* returns: 0 if no data, else packet size      */
/*! @ingroup messaging
	Checks for and gets the last packet received, stores the data and the node address sent from.

	@remarks Data can only be read once, and is overwritten if a new packet arrives before the
				last packet has been read.
	@param node Address of the sender.
	@param data Pointer to a buffer to hold the data.
	@return \c 0 if successful, otherwise link error.

	\b Example: See \c eyelink_node_send()

	\sa \c eyelink_get_float_data(), \c eyelink_open_node() and \c eyelink_node_send()
 */
INT16 ELCALLTYPE eyelink_node_receive(ELINKADDR node, void FARTYPE *data);


/********* MESSAGES AND COMMANDS *******/

/* See error return codes above                            */
/* in general, 0 or OK_RESULT means success                */
/* and LINK_TERMINATED_RESULT means data could not be sent */
/* returns: OK_RESULT or LINK_TERMINATED_RESULT            */
/*! @ingroup messaging
	Sends a command to the connected eye tracker.

	@remarks If \c eyelink_send_commnd() is used, the text command will be executed, and a
				result code returned that can be read with \c eyelink_command_result().
	@param text String command to send.
	@return \c 0 if successful, otherwise link error.

	\b Example:
	\code
	// This program illustrates the use of eyelink_send_command

	#include <eyelink.h>

	UINT32 t;
	int i, j;
	char buf[256];

	// Change the assignment of the button functions so that pressing
	// button 1 will accept target fixation
	eyelink_send_command("button_function 1 'accept_target_fixation'");

	t = current_msec();
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
	MSG	4608038 Command executed successfully: OK
	\endcode

	\sa \c eyelink_command_result() and \c eyelink_timed_command()
 */
INT16 ELCALLTYPE eyelink_send_command(char FARTYPE *text);

/*! @ingroup messaging
	Check for and retrieves the numeric result code sent by the tracker from the last command.

	@return \c NO_REPLY if no reply to last command.\n
			<tt>OK_RESULT (0)</tt> if OK.\n
			Other error codes represent tracker execution error.\n

	\b Example: See \c eyelink_send_command()

	\sa \c eyelink_last_message(), \c eyelink_send_command() and \c eyelink_timed_command()
 */
INT16 ELCALLTYPE eyelink_command_result(void);

/*! @ingroup messaging
	Sends a command to the connected eye tracker, wait for reply.

	@remarks If \c eyelink_send_command() is used, the text command will be executed, and a result
				code returned that can be read with \c eyelink_command_result().
	@param text String command to send.
	@param msec Maximum milliseconds to wait for reply.
	@return <tt>OK_RESULT (0)</tt> if OK.\n
			\c NO_REPLY if timed out.\n
			\c LINK_TERMINATED_RESULT if cannot send.\n
			other error codes represent tracker execution error.\n

	\b Example:
	\code
	// This program illustrates the use of eyelink_timed_command

	#include <eyelink.h>

	INT16 error;
	char buf[256];

	// send command string to tracker, wait for reply
	error = eyelink_timed_command(1000, "button_function 5
	     'accept_target_fixation'");

	// Checks for the command result
	if (error)
	{
	    eyelink_last_message(buf);
	    eyemsg_printf("Error in excuting the command: %s", buf);
	}
	\endcode

	\sa \c eyelink_command_result() and \c eyelink_send_command()
 */
INT16 ELCALLTYPE eyelink_timed_command(UINT32 msec, char FARTYPE *text);

/* use to get more information on tracker result           */
/*! @ingroup messaging
	Checks for and gets the last packet received, stores the data and the node address sent from.

	@param buf String buffer to return text message.
	@return \c 0 if no message since last command sent, otherwise length of string.

	\b Example: See \c eyelink_timed_command()

	\sa \c eyelink_send_command() and \c eyelink_timed_command()
 */
INT16 ELCALLTYPE eyelink_last_message(char FARTYPE *buf);

/* returns: OK_RESULT or LINK_TERMINATED_RESULT            */
/*! @ingroup messaging
	Sends a text message the connected eye tracker.  The text will be added to the EDF file.

	@param msg Text to send to the tracker.
	@return \c 0 if no error, else link error code.

	\b Example: See \c eyelink_quiet_mode()

	\sa \c eyelink_node_send_message(), \c eyelink_quiet_mode() and \c set_eyelink_address()
 */
INT16 ELCALLTYPE eyelink_send_message(char FARTYPE *msg);

/* send a data file message to any or all trackers         */
/* returns: OK_RESULT or LINK_TERMINATED_RESULT            */
/*! @ingroup messaging
	Sends a text message the connected eye tracker.  The text will be added to the EDF file.

	@remarks NEW (v2.1): If the link is initialized but not connected to a tracker, the
				message will be sent to the tracker set by \c set_eyelink_address().
	@param msg Text to send to the tracker.
	@param node Address of the tracker.
	@return \c 0 if no error, else link error code.

	\b Example: See \c eyelink_quiet_mode()

	\sa \c eyelink_send_message(), \c eyelink_quiet_mode() and \c set_eyelink_address()
 */
INT16 ELCALLTYPE eyelink_node_send_message(ELINKADDR node, char FARTYPE *msg);


/*
The following two functions are same as as without _ex function.
The only difference is in eyelink_send_message, and eyelink_send_message
the exectime is 0.
*/

/*! @ingroup messaging
	Send a data file message string to connected tracker with time offset.
	@param exectime time offset. If the offset is 0, this function is the same as eyelink_send_message()
	@param msg Message to be sent
	@return \c OK_RESULT or \c LINK_TERMINATED_RESULT.
	@sa eyemsg_printf_ex(), eyelink_send_message()
 */
INT16 ELCALLTYPE eyelink_send_message_ex(UINT32 exectime, char FARTYPE *msg);

/*! @ingroup messaging
	Send a data file message to any or all trackers with time offset.
	@param exectime time offset. If the offset is 0, this function is the same as eyelink_node_send_message()
	@param node Node address
	@param msg Message to be sent
	@return \c OK_RESULT or \c LINK_TERMINATED_RESULT.
	@sa eyelink_node_send_message()
 */
INT16 ELCALLTYPE eyelink_node_send_message_ex(UINT32 exectime,ELINKADDR node, char FARTYPE *msg);



/* returns: OK_RESULT or LINK_TERMINATED_RESULT            */
/*! @ingroup messaging
	Sends a text variable name whose value is to be read and returned by the tracker as a text string.

	@remarks NEW (v2.1): If the link is initialized but not connected to a tracker, the message will
				be sent to the tracker set by \c set_eyelink_address(). However, these requests will be
				ignored by tracker versions older than EyeLink I v2.1 and EyeLink II v1.1.
	@param text String with message to send.
	@return \c 0 if success, otherwise link error code.

	\b Example:
	\code
	// This code illustrates the use of eyelink_read_request() to get the coordinate information of the screen coordinate

	int read_tracker_pixel_coords(void)
	{
	    char buf[100] = "";
	    UINT32 t;

	    // Checks whether we are still connected
	    if(!eyelink_is_connected() || break_pressed())
	        return 1;

	    // Tries to retrieve the info about screen_pixel_coords
	    eyelink_read_request("screen_pixel_coords");

	    t = current_msec();
	    // Waits for a maximum of 500 msec
	    while(current_msec()-t < 500)
	    {
	        if(eyelink_read_reply(buf) == OK_RESULT)
	        {
	            eyemsg_printf("%s", buf);
	            return 0;
	        }
	        message_pump(NULL);  // Keeps Windows happy
	        if(!eyelink_is_connected) return 1;
	        if(break_pressed()) return 1;
	    }
	    return -1;  // Timed out
	}
	\endcode

	\b Output:
	\code
	MSG	374986 0.000000,0.000000,1023.000000,767.000000
	\endcode

	\sa \c eyelink_read_reply()
 */
INT16 ELCALLTYPE eyelink_read_request(char FARTYPE *text);

/* checks for reply to eyelink_read_request()              */
/* returns OK_RESULT if we have it, else NO_REPLY          */
/* copies result to <buf>                                  */
/*! @ingroup messaging
	Returns text with reply to last read request.

	@param buf String to contain text.
	@return <tt>OK_RESULT (0)</tt> if response received.\n
			\c NO_REPLY if no response yet.

	\b Example: See \c eyelink_read_request()

	\sa \c eyelink_read_request()
 */
INT16 ELCALLTYPE eyelink_read_reply(char FARTYPE *buf);

/* returns: OK_RESULT or LINK_TERMINATED_RESULT            */
/*! @ingroup access_time_tracker
	Sends a request the connected eye tracker to return its current time.

	@remarks The time reply can be read with \c eyelink_read_time().
	@return \c 0 if no error, else link error code.

	\b Example:
	\code
	// This program illustrates the use of eyelink_request_time to retrieve the current tracker time

	#include <eyelink.h>

	// This routine is used to retrieve the concurrent tracker PC time.
	// If successful, this function returns the tracker PC time, else 0;
	// Note, the waiting time is set as 50 msec, although normally the function should return within 1 msec
	UINT32 current_tracker_time()
	{
	    UINT32  time_subject=0, time_tracker=0;

	    eyelink_request_time();        // request the tracker PC time
	    time_subject = current_msec();
	    // wait for a maximum of 50 msec
	    while(!time_tracker && current_msec() -time_subject < 50)
	        time_tracker = eyelink_read_time();  // read tracker PC time

	    return time_tracker;
	}
	\endcode

	\sa \c eyelink_node_request_time() and \c eyelink_read_time()
	@deprecated  This function is depricated. It is left for compatibility reasons.
 */
UINT32 ELCALLTYPE eyelink_request_time(void);

/* sends request to specific tracker for time update       */
/* broadcast requests are not allowed                      */
/* returns: OK_RESULT or LINK_TERMINATED_RESULT            */
/*! @ingroup access_time_tracker
	Sends a request the connected eye tracker to return its current time.

	@remarks The time reply can be read with \c eyelink_read_time().
	@param node Address of a specific tracker.
	@return \c 0 if no error, else link error code.

	\b Example: See \c eyelink_request_time()

	\sa \c eyelink_request_time() and \c eyelink_read_time()
 */
UINT32 ELCALLTYPE eyelink_node_request_time(ELINKADDR node);

/* checks for reply to eyelink_request_time()              */
/* returns: 0 if no reply, else time                       */
/*! @ingroup access_time_tracker
	Returns the tracker time requested by \c eyelink_request_time() or \c eyelink_node_request_time().

	@return \c 0 if no response yet,  else timestamp in millisecond.

	\b Example: See \c eyelink_request_time()

	\sa \c eyelink_node_request_time() and \c eyelink_request_time()
 */
UINT32 ELCALLTYPE eyelink_read_time(void);



/********* CALIBRATION, SETUP, DRIFT CORRECT *********/
/* stop data flow, by returning tracker to offline mode    */
/* also aborts any other operations (setup, drift corr.)   */
/*! @ingroup setup_eyelink
	Places EyeLink tracker in off-line (idle) mode.

	@remarks Use before attempting to draw graphics on the tracker display,
				transferring files, or closing link. Always call \c eyelink_wait_for_mode_ready()
				afterwards to ensure tracker has finished the mode transition. This function
				pair is implemented by the eyelink_exptkit library function \c set_offline_mode().
	@return \c 0 if mode switch begun, else link error.

	\b Example:
	\code
	// The following is the functional implementation of the stop_recording(), using the eyelink_abort()

	#include <eyelink.h>

	eyecmd_printf("heuristic_filter = ON");
	eyelink_abort();    // stop data flow
	eyelink_wait_for_mode_ready(500); // wait till finished mode switch
	\endcode

	\sa \c eyelink_wait_for_mode_ready() and \c set_offline_mode()
 */
INT16 ELCALLTYPE eyelink_abort(void);

/*! @ingroup setup_eyelink
	Enters setup mode
 */
INT16 ELCALLTYPE eyelink_start_setup(void);

/*! @ingroup setup_eyelink
	Checks if tracker is still in a Setup menu activity (includes camera image view, calibration,
	and validation).  Used to terminate the subject setup loop.

	@return \c 0 if no longer in setup mode.

	\b Example:
	\code
	// This program illustrates the use of eyelink_in_setup

	#include <eyelink.h>

	int current_mode;
	int prev_mode =0;
	UINT start_time  = current_msec();

	// Checks for 10 seconds
	while(current_msec() < start_time + 10000)
	{
	    if(!eyelink_is_connected())
	        return -1;

	    current_mode =eyelink_in_setup();
	    if (current_mode!=prev_mode)
	        eyemsg_printf("%s", current_mode?"In setup":"Not in setup");

	    prev_mode = current_mode;
	}
	\endcode

	\b Output:
	\code
	MSG	905992 In setup
	MSG	909596 Not in setup
	\endcode

	\sa \c eyelink_current_mode()
 */
INT16 ELCALLTYPE eyelink_in_setup(void);


/*! @ingroup setup_eyelink
	Returns the current target position and state.

	@param x Pointer to variable to hold target X position.
	@param y Pointer to variable to hold target Y position.
	@return \c 1 if target is visible, 0 if not.

	\b Example:
	\code
	INT16 target_mode_display(void)
	{
	  int target_visible = 0;	// target currently drawn
	  INT16 tx;		// new target position
	  INT16 ty;

	  INT16 otx=MISSING;    // current target position
	  INT16 oty=MISSING;

	  unsigned key;		// local key pressed
	  int i;
	  int result = NO_REPLY;

	  // LOOP WHILE WE ARE DISPLAYING TARGETS
	  while(eyelink_current_mode() & IN_TARGET_MODE)
	    {
	      if(!eyelink_is_connected()) return -1;
	      key = getkey();
		  // HANDLE LOCAL KEY PRESS
	      if(key)
			{
			  switch(key)
				{
				  case TERMINATE_KEY:       // breakout key code
				clear_cal_display();
				return TERMINATE_KEY;
				  case 32:	         	// Spacebar: accept fixation
				eyelink_accept_trigger();
				break;
				  case 0: 		// No key
				  case JUNK_KEY: 	// No key
				break;
				  case ESC_KEY: if(eyelink_is_connected()==-1) goto exit;
				  default:          // Echo to tracker for remote control
				if(allow_local_control)
					eyelink_send_keybutton(key,0,KB_PRESS);
				break;
				}
			}

	      result = eyelink_cal_result();
	      if(result != NO_REPLY) break;

		  // HANDLE TARGET CHANGES
	      i = eyelink_target_check(&tx, &ty);
					// erased or moved: erase target
	      if( (target_visible && i==0) || tx!=otx || ty!=oty)
		{
		  erase_cal_target();
		  target_visible = 0;
		}
					// redraw if visible
	      if(!target_visible && i)
		{
		  draw_cal_target(tx, ty);

		  target_visible = 1;
		  otx = tx;		// record position for future tests
		  oty = ty;
		}
	    }

	exit:

	  if(target_visible)
		  erase_cal_target();   // erase target on exit

	  clear_cal_display();
	  return result;
	}
	\endcode

	\sa \c eyelink_accept_trigger(), \c eyelink_apply_driftcorr(), \c eyelink_current_mode()
		and \c eyelink_driftcorr_start()
 */
INT16 ELCALLTYPE eyelink_target_check(INT16 FARTYPE *x, INT16 FARTYPE *y);

/*! @ingroup setup_eyelink
	Returns the current target position and state.  Same as eyelink_target_check() except this function returns data in floating point values.

	@param x Pointer to variable to hold target X position.
	@param y Pointer to variable to hold target Y position.
	@return \c 1 if target is visible, 0 if not.
*/
INT16 ELCALLTYPE eyelink_target_checkf(float FARTYPE *x, float FARTYPE *y);
/*! @ingroup setup_eyelink
	Triggers the EyeLink tracker to accept a fixation on a target, similar to the 'Enter'
	key or spacebar on the tracker.

	@return \c NO_REPLY if drift correction not completed yet.\n
			<tt>OK_RESULT (0)</tt> if success.\n
			<tt>ABORT_REPLY (27)</tt> if 'ESC' key aborted operation.\n
			\c -1 if operation failed.\n
			\c 1 if poor calibration or excessive validation error.

	\b Example: See \c eyelink_driftcorr_start()

	\sa \c eyelink_apply_driftcorr(), \c eyelink_current_mode(), \c eyelink_driftcorr_start()
		and \c eyelink_target_check()
 */
INT16 ELCALLTYPE eyelink_accept_trigger(void);

/*! @ingroup setup_eyelink
	Sets the position of the drift correction target, and switches the tracker to
	drift-correction mode.  Should be followed by a call to \c eyelink_wait_for_mode_ready().

	@param x X position of the target.
	@param y Y position of the target.
	@return \c 0 if command sent OK, else link error.

	\b Example:
	\code
	// This program illustrates the use of eyelink_driftcorr_start() for the
	// implementation of a drift correction mechanism

	#include <eyelink.h>

	unsigned key;
	int result = 0;
	int x, y; // position of the drift correction target

	if(eyelink_is_connected())
	{
	  eyecmd_printf("heuristic_filter = ON");
	  ...
	  // Code to draw the target here
	  while(getkey()) {};  // dump the keys

	  eyelink_driftcorr_start(x, y); // start the drift correction
	  do {
	       result = eyelink_cal_result();

	       key = getkey();
	       switch(key)
		 {
		   case TERMINATE_KEY:  // breakout code
		     return TERMINATE_KEY;
		   case 0: 		// no key
		   case JUNK_KEY: 	// no key
		     break;
		   case ESC_KEY:       // ESC key: we flag abort from our end
		     result = 27;
		     break;
		   case 32: 		// Spacebar: we trigger ourselves
		     eyelink_accept_trigger();
		     break;
		   default:
		     eyelink_send_keybutton(key,0,KB_PRESS);
		     break;
		 }
	    } while(result == NO_REPLY);

	     // Applies the drift correction result
	     if (result != 27 && result != -1)
		eyelink_apply_driftcorr();
	    else
	       ; // Other code for handling

	    return result;
	}
	\endcode

	\sa \c eyelink_accept_trigger() and \c eyelink_send_keybutton()
 */
INT16 ELCALLTYPE eyelink_driftcorr_start(INT16 x, INT16 y);


/*! @ingroup setup_eyelink
	Sets the position of the drift correction target, and switches the tracker to
	drift-correction mode.  Should be followed by a call to \c eyelink_wait_for_mode_ready().
	Same as eyelink_driftcorr_start() except the x,y parameters take floating point values.

	@param x X position of the target.
	@param y Y position of the target.
	@return \c 0 if command sent OK, else link error.

	\b Example:
	\code
	// This program illustrates the use of eyelink_driftcorr_start() for the
	// implementation of a drift correction mechanism

	#include <eyelink.h>

	unsigned key;
	int result = 0;
	int x, y; // position of the drift correction target

	if(eyelink_is_connected())
	{
	  eyecmd_printf("heuristic_filter = ON");
	  ...
	  // Code to draw the target here
	  while(getkey()) {};  // dump the keys

	  eyelink_driftcorr_start(x, y); // start the drift correction
	  do {
	       result = eyelink_cal_result();

	       key = getkey();
	       switch(key)
		 {
		   case TERMINATE_KEY:  // breakout code
		     return TERMINATE_KEY;
		   case 0: 		// no key
		   case JUNK_KEY: 	// no key
		     break;
		   case ESC_KEY:       // ESC key: we flag abort from our end
		     result = 27;
		     break;
		   case 32: 		// Spacebar: we trigger ourselves
		     eyelink_accept_trigger();
		     break;
		   default:
		     eyelink_send_keybutton(key,0,KB_PRESS);
		     break;
		 }
	    } while(result == NO_REPLY);

	     // Applies the drift correction result
	     if (result != 27 && result != -1)
		eyelink_apply_driftcorr();
	    else
	       ; // Other code for handling

	    return result;
	}
	\endcode

	\sa \c eyelink_accept_trigger() and \c eyelink_send_keybutton()
 */
INT16 ELCALLTYPE eyelink_driftcorr_startf(float x, float y);

/* Reading result resets it to NO_REPLY                   */
/*! @ingroup setup_eyelink
	Checks for a numeric result code returned by calibration, validation, or drift correction.

	@return \c NO_REPLY if drift correction not completed yet.\n
			<tt>OK_RESULT (0)</tt> if success.\n
			<tt>ABORT_REPLY (27)</tt> if 'ESC' key aborted operation.\n
			\c -1 if operation failed.\n
			\c 1 if poor calibration or excessive validation error.\n

	\b Example: See \c eyelink_driftcorr_start()

	\sa \c eyelink_accept_trigger(), \c eyelink_apply_driftcorr(), \c eyelink_cal_message() and
	\c eyelink_driftcorr_start()
 */
INT16 ELCALLTYPE eyelink_cal_result(void);

/*! @ingroup setup_eyelink
	Applies the results of the last drift correction.  This is not done automatically after a
	drift correction, allowing the message returned by \c eyelink_cal_message() to be examined first.

	@return \c 0 if command sent OK, else link error.

	\b Example: See \c eyelink_driftcorr_start()

	\sa \c eyelink_accept_trigger(), \c eyelink_cal_result(), \c eyelink_current_mode(), \c eyelink_driftcorr_start()
	and \c eyelink_target_check()
 */
INT16 ELCALLTYPE eyelink_apply_driftcorr(void);

/*! @ingroup setup_eyelink
	Returns text associated with result of last calibration, validation, or drift correction.
	This usually specifies errors or other statistics.

	@param msg Buffer to return back the message.
	@return \c 0 if no message since last command sent, else length of string.

	\b Example:
	\code
	// This programs illustrates the use of eyelink_cal_message()
	#include <eyelink.h>
	char message[256];

	// Performs a drift correction
	while(1)
	{   // Check link often so we can exit if tracker stopped
	    if(!eyelink_is_connected()) return ABORT_EXPT;

	    // Performs drift correction with target pre-drawn
	    error = do_drift_correct(SCRWIDTH/2, SCRHEIGHT/2, 1, 1);

	    // repeat if ESC was pressed to access Setup menu
	    if(error!=27) break;
	}
	// Retrieves and writes out the calibration result message
	eyelink_cal_message(message);
	eyemsg_printf(message);
	\endcode

	\b Output:
	\code
	MSG	1896559 DRIFTCORRECT R RIGHT at 320,40  OFFSET 0.11 deg.  -1.0,-4.0 pix.
	MSG	1896560 drift_correction: 0.11 -1.00 -4.00
	\endcode

	\sa \c eyelink_accept_trigger(), \c eyelink_apply_driftcorr() and \c eyelink_cal_result()
 */
INT16 ELCALLTYPE eyelink_cal_message(char FARTYPE *msg);


/********* TRACKER MODE AND STATE **********/

/*Tracker state bit: AND with flag word to test functionality */
 /*! \addtogroup eyelink_mode
   *  @{
   */

#define IN_DISCONNECT_MODE 16384	/*!< Tracker state bit: disconnected.                          */
#define IN_UNKNOWN_MODE    0		/*!< Tracker state bit: mode fits no class (i.e setup menu)   */
#define IN_IDLE_MODE       1		/*!< Tracker state bit: off-line                              */
#define IN_SETUP_MODE      2		/*!< Tracker state bit: setup or cal/val/dcorr                */
#define IN_RECORD_MODE     4		/*!< Tracker state bit: data flowing                          */
#define IN_TARGET_MODE     8		/*!< Tracker state bit: some mode that needs fixation targets */
#define IN_DRIFTCORR_MODE  16		/*!< Tracker state bit: drift correction                      */
#define IN_IMAGE_MODE      32		/*!< Tracker state bit: image-display mode                    */
#define IN_USER_MENU       64		/*!< Tracker state bit: user menu                             */
#define IN_PLAYBACK_MODE  256		/*!< Tracker state bit: tracker sending playback data         */
/*! @} */


/*! @ingroup eyelink_mode
	This function tests the current tracker mode, and returns a set of flags based of
	what the mode is doing.  The most useful flag using the EXPTSPPT toolkit is \c IN_USER_MENU
	to test if the EyeLink Abort menu has been activated.

	@return Set of bitflags that mark mode function:
		\c IN_DISCONNECT_MODE if disconnected.
		\c IN_IDLE_MODE if off-line (Idle mode).
		\c IN_SETUP_MODE if in Setup-menu related mode.
		\c IN_RECORD_MODE if tracking is in progress.
		\c IN_PLAYBACK_MODE if currently playing back data.
		\c IN_TARGET_MODE if in mode that requires a fixation target.
		\c IN_DRIFTCORR_MODE if in drift-correction.
		\c IN_IMAGE_MODE if displaying grayscale camera image.
		\c IN_USER_MENU if displaying Abort or user-defined menu.


	\b Example: See \c echo_key()

	\sa \c echo_key(), \c eyelink_tracker_mode() and \c eyelink_user_menu_selection()
 */
INT16 ELCALLTYPE eyelink_current_mode(void);


/* EYELINK RAW MODE CODES                 */
/* These specify modes, not functionality */

#define EL_IDLE_MODE         1
#define EL_IMAGE_MODE        2
#define EL_SETUP_MENU_MODE   3

#define EL_USER_MENU_1       5
#define EL_USER_MENU_2       6
#define EL_USER_MENU_3       7

#define EL_OPTIONS_MENU_MODE  8  /* NEW FOR EYELIKN II */

#define EL_OUTPUT_MENU_MODE  9

#define EL_DEMO_MENU_MODE    10
#define EL_CALIBRATE_MODE    11
#define EL_VALIDATE_MODE     12
#define EL_DRIFT_CORR_MODE   13
#define EL_RECORD_MODE       14

   /* ADDED in EYELINK II V2.0 */
#define SCENECAM_ALIGN_MODE 15
#define SCENECAM_DEPTH_MODE 16


#define USER_MENU_NUMBER(mode) ((mode)-4)

/*! @ingroup eyelink_mode
	Returns raw EyeLink mode numbers, defined in eyelink.h as EL_xxxx definitions.

	@return Raw EyeLink mode, \c -1 if link disconnected

	\b Example:
	\code
	// This programs illustrates the use of track_mode_loop() function. See BROADCAST for the complete program code

	#include <eyelink.h>

	// Follow and process tracker modes
	// Displays calibration and drift correction targets
	// Also detects start of recording
	// Black backgrounds would be transparent as video overlay
	void track_mode_loop(void)
	{
	    int oldmode = -1;

	    while(eyelink_is_connected())
	    {
	        int mode = eyelink_tracker_mode();
	        unsigned key = getkey();

	        if(key==27 || break_pressed() || !eyelink_is_connected())
	            return;
	        else if(key) // Echo to tracker
	            eyelink_send_keybutton(key,0,KB_PRESS);

	        if(mode == oldmode)
	            continue;
	        switch(mode)
	        {
	        case EL_RECORD_MODE:     // Recording mode
	            // Code for processing recording mode ;
	            break;
	        case EL_IMAGE_MODE:      // Image mode
	            // Code for processing image mode ;
	            break;
	        case EL_SETUP_MENU_MODE: // Setup menu mode
	            // Code for processing setup menu mode ;
	            break;
	        case EL_CALIBRATE_MODE:  // Calibration, validation, DC mode
	        case EL_VALIDATE_MODE:
	        case EL_DRIFT_CORR_MODE:
	            // Code for processing calibration, validation, dc mode ;
	            break;
	        case EL_OPTIONS_MENU_MODE: // Option menu mode
	            // Code for processing calibration mode ;
	            break;
	        default:
	            // Code for processing default case ;
	            break;
	        }
	        oldmode = mode;
	    }
	}
	\endcode

	\sa \c eyelink_current_mode() and \c eyelink_is_connected()
 */
INT16 ELCALLTYPE eyelink_tracker_mode(void);

/* waits till new mode is finished setup                */
/* maxwait = 0 to just test flag                        */
/* rtns current state of switching flag: 0 if not ready */
/*! @ingroup eyelink_mode
	After a mode-change command is given to the EyeLink tracker, an additional
	5 to 30 milliseconds may be needed to complete mode setup.  Call this function
	after mode change functions.

	@remarks If it does not return \c 0, assume a tracker error has occurred.
	@param maxwait Maximum milliseconds to wait for the mode to change.
	@return \c 0 if mode switching is done, else still waiting.

	\b Example: See \c eyelink_data_start()

	\sa \c eyelink_data_start() and \c set_offline_mode()
 */
INT16 ELCALLTYPE eyelink_wait_for_mode_ready(UINT32 maxwait);

/* returns number of last user-menu selection           */
/* cleared on entry to user menu, or on reading         */
/*! @ingroup eyelink_mode
	Checks for a user-menu selection, clears response for next call.

	@return \c 0 if no selection made since last call, else code of selection.

	\b Example:
	\code
	// This programs illustrates the use of eyelink_user_menu_selection

	#include <eyelink.h>
	int i;

	i = eyelink_current_mode();
	if(i & IN_USER_MENU)     // handle user menu selections
	{
	    switch(eyelink_user_menu_selection()
	    {
	    case 1:     // SETUP selected
	        break;
	    case 2:     // REPEAT trial
	        return REPEAT_TRIAL;
	    case 3:	// SKIP trial
		return SKIP_TRIAL;
	    case 4:	// Abort experiment
		eyelink_abort();// this cleans up by erasing menu
	        return ABORT_EXPT;
	    default:	// no selection: continue
		break;
	    }
	}
	\endcode

	\sa \c eyelink_current_mode()
 */
INT16 ELCALLTYPE eyelink_user_menu_selection(void);



/************* DATA CHANNEL *************/

#define SAMPLE_TYPE 200

/* returns data (sample-to-pixels) prescaler            */
/*! @ingroup messaging
	Returns the divisor used to convert integer eye data to floating point data.

	@return Divisor (usually 10).

	\b Example:
	\code
	// This program uses the eyecmd_printf() and eyelink_position_prescaler() to set
	// and retrieve the screen_write_prescale value.  See eyelink_get_sample() for
	// another example of using eyelink_position_prescaler()

	#include <eyelink.h>
	UINT32 start_time;

	// Sets the value by which gaze position data is multiplied before writing to EDF file or link as integer
	eyecmd_printf("screen_write_prescale = 10");

	start_time = current_msec();
	while(current_msec() < start + 1000)
	    if (eyelink_command_result() == OK_RESULT)
	    {
	        // Checks out the value set
	        eyemsg_printf("Position scalar %d",
	            eyelink_position_prescaler());
	        break;
	    }
	\endcode
 */
INT16 ELCALLTYPE eyelink_position_prescaler(void);

/* resets data flow at start                            */
/*! @ingroup messaging
	Prepares link buffers to receive new data.  If \c &lt;clear&gt; is nonzero, removes old data from buffer.

	@param clear If clear is non-zero, any buffer data is discarded.
	@return Always returns \c 0.

	\b Example: See \c eyelink_in_data_block()

	\sa \c eyelink_data_status() and \c eyelink_in_data_block()
 */
INT16 ELCALLTYPE eyelink_reset_data(INT16 clear);

/* Update all data items                                */
/* return pointer to TSR/link state (ILINKDATA type)    */
/*! @ingroup messaging
	Updates buffer status (data count, etc), returns pointer to internal \c ILINKDATA structure.

	@return Pointer to \c ILINKDATA structure.

	\b Example:
	\code
	Example 1:
	// This program illustrates the use of eyelink_data_status to retrieve the name of the current application

	ILINKDATA* current_data;  // Stores the link state data

	// Sets up the EyeLink system and connect to tracker
	if(open_eyelink_connection(0))  return -1;

	// Updates the link data
	current_data = eyelink_data_status();
	eyemsg_printf("the subject PC name %s ", current_data -> our_name);
	\endcode

	\code
	Example 2:

	// Checks link state of tracker
	int preview_tracker_connection(void)
	{
	    UINT32 t, tt;
	    ILINKDATA *idata = eyelink_data_status();
	    // Accesses link status info

	    // Forces tracker to send status and time
	    eyelink_request_time();
	    t = current_msec();
	    while(current_msec()- t < 500)   // Waits for response
	    {
	        tt = eyelink_read_time();   // Will be nonzero if reply

	        if(tt != 0)
	        {   // Retrieves the current connection state
	            if(idata->link_flags & LINK_BROADCAST)
	                return LINK_BROADCAST;
	            if(idata->link_flags & LINK_CONNECTED)
	                return LINK_CONNECTED;
	            else
	                return 0;
	        }

	        message_pump(NULL);         // Keeps Windows happy
	        if(break_pressed())
	            return 1;   // Stops if program terminated
	    }
	    return -1;  // Failed (timed out)
	}
	\endcode

	\b Output:
	\code
	MSG	15252571 the subject PC name GOLDBERG
	\endcode

	\sa \c eyelink_in_data_block() and \c eyelink_reset_data()
 */
#ifndef MSORDER    /* some Microsoft function call
                      extensions need different order   */
  void FARTYPE * ELCALLTYPE eyelink_data_status(void);
#else
  void ELCALLTYPE FARTYPE * eyelink_data_status(void);
#endif

/* tests for block with samples or events (or both)     */
/* rtns 1 if any of the selected types on, 0 if none on */
/*! @ingroup recording
	Checks to see if framing events read from queue indicate that the data
	is in a block containing  samples, events, or both.

	@remarks The first item in queue may not be a block start even, so this should
				be used in a loop while discarding items using \c eyelink_get_next_data(NULL).
				NOTE: this function did not work reliably in versions of the SLL before
				v2.0 (did not detect end of blocks).
	@param samples If non-zero, check if in a block with samples.
	@param events If non-zero, check if in a block with events.
	@return \c 0 if no data of either masked type is being sent.

	\b Example:
	\code
	// This program illustrates the use of eyelink_in_data_block in a broadcast connection.
	// First a broadcast connection is opened and data reception from the tracker is reset.
	// Following that, checks whether the block contains samples or events and make use of this information.

	#include <eyelink.h>
	#include <stdio.h>

	// Initializes the link
	if(open_eyelink_connection(-1))
	    return -1;

	...
	// Extra code here to check for the tracker status or
	// wait for the go-signal from the other application

	// Starts the broadcast connection to the tracker
	if(eyelink_broadcast_open())
	{
	    printf("Cannot open broadcast connection to tracker");
	    return -1;
	}
	// Enables link data reception by EyeLink DLL
	eyelink_reset_data(1);

	// NOTE: this function can discard some link data
	eyelink_data_switch(RECORD_LINK_SAMPLES | RECORD_LINK_EVENTS);

	// Makes use of the link data
	while(eyelink_is_connected())
	{
	    if(escape_pressed() || break_pressed()) return;

	    //  check for new data item
	    i = eyelink_get_next_data(NULL);
	    if(i == 0) continue;

	    // link data block available?
	    if(eyelink_in_data_block(1, 1))
	    {
	        ...
	        // Code to read the link data, etc.
	    }
	}
	\endcode

	\sa \c eyelink_data_status() and \c eyelink_wait_for_block_start()
 */
INT16 ELCALLTYPE eyelink_in_data_block(INT16 samples,INT16 events);

/* waits till a block of samples, events, or both is begun */
/* maxwait = 0 to just test                                */
/* rtns 1 if in block, 0 if not ready                      */
/*! @ingroup recording
	Reads and discards events in data queue until in a recording block.  Waits for up to
	\c  &lt;timeout&gt; milliseconds for a block containing samples, events, or both to be
	opened.  Items in the queue are discarded until the block start events are found and
	processed.  This function will fail if both samples and events are selected but only
	one of link samples and events were enabled by \c start_recording().

	@remarks This function did not work in versions previous to 2.0.
	@param maxwait Time in milliseconds to wait.
	@param samples If non-zero, check if in a block with samples.
	@param events If non-zero, check if in a block with events.
	@return \c 0 if time expired without any data of masked types available.

	\b Example:
	\code
	// This program illustrates the use of eyelink_wait_for_block_start

	#include <eyelink.h>
	#include <stdio.h>

	// Starts recording with both sample and events to the file and link
	if(start_recording(1,1,1,1)!= 0)
	    return -1;          // ERROR: couldn't start recording

	// record for 100 msec before displaying stimulus
	begin_realtime_mode(100);

	// wait for link sample data
	if(!eyelink_wait_for_block_start(100, 1, 0))
	{
	      printf("ERROR: No link samples received!");
	      return TRIAL_ERROR;
	}

	// determine which eye(s) are available
	eye_used = eyelink_eye_available();
	switch(eye_used) // select eye, add annotation to EDF file
	{
	      case RIGHT_EYE:
	        eyemsg_printf("EYE_USED 1 RIGHT");
	        break;
	      case BINOCULAR:  // both eye's data present: use left eye only
	        eye_used = LEFT_EYE;
	      case LEFT_EYE:
	        eyemsg_printf("EYE_USED 0 LEFT");
	        break;
	}
	\endcode
 */
INT16 ELCALLTYPE eyelink_wait_for_block_start(UINT32 maxwait,
                   INT16 samples, INT16 events);

/*! @ingroup messaging
	Fetches next data item from link buffer.  Usually called with \c &lt;buf&gt; = \c NULL,
	and returns the data item type.  If the item is not wanted, simply ignore it.
	Otherwise, call \c eyelink_get_float_data() to read it into a buffer.

	@param buf If \c NULL, saves data, else copies integer data into buffer.
	@return \c 0 if no data, \c SAMPLE_TYPE if sample, else event type.

	\b Example:
	\code
	// This program illustrates the use of eyelink_get_next_data() and eyelink_get_float_data()

	#include <eyelink.h>

	ALLF_DATA evt;        // buffer to hold sample and event data
	int eye_used = -1;    // indicates which eye's data to display
	int i;

	// Determines which eye(s) are available
	eye_used = eyelink_eye_available();

	// Selects eye, add annotation to EDF file
	switch(eye_used)
	{
	    case RIGHT_EYE:
	        eyemsg_printf("EYE_USED 1 RIGHT");
	        break;
	    case BINOCULAR:   // both eye's data present: use left eye only
	        eye_used = LEFT_EYE;
	    case LEFT_EYE:
	        eyemsg_printf("EYE_USED 0 LEFT");
	        break;
	}
	while(1)
	{
	    // Check for data from link and process fixation update events
	    i = eyelink_get_next_data(NULL);
	    if(i == FIXUPDATE)
	    {
	        // get a copy of the FIXUPDATE event
	        eyelink_get_float_data(&evt);
	        // only process if it's from the desired eye?
	        if(evt.fe.eye == eye_used)
	        {
	            // Records the average position and duration of update
	            eyemsg_printf("Fixupdate: avg_x %8.2f, y %8.2f, dur %d",
	                evt.fe.gavx, evt.fe.gavy, evt.fe.entime-evt.fe.sttime);
	        }
	    }
	    ...
	    // Other code for drawing and exiting
	}
	\endcode

	\b Output:
	\code
	MSG	30244970 Fixupdate: avg_x   863.10, y   244.10, dur 48
	MSG	30245018 Fixupdate: avg_x   863.10, y   245.60, dur 48
	MSG	30245070 Fixupdate: avg_x   863.10, y   247.30, dur 48
	\endcode

	\sa \c eyelink_get_float_data(), \c eyelink_get_last_data(), \c eyelink_newest_float_sample()
		and \c eyelink_newest_sample()
 */
INT16 ELCALLTYPE eyelink_get_next_data(void FARTYPE *buf);

/* makes copy of last item from eyelink_get_next_data      */
/*! @ingroup messaging
	Gets an integer (unconverted) copy of the last/newest link data (sample or event)
	seen by \c eyelink_get_next_data().

	@param buf Pointer to buffer (\c ISAMPLE, \c IEVENT, or \c ALL_DATA type).
	@return \c 0 if no data, \c SAMPLE_TYPE if sample, else event type code.

	\b Example:
	\code
	ALLF_DATA evt;        // buffer to hold sample and event data
	int eye_used = -1;    // indicates which eye's data to display
	int i;
	UINT32 prev_event_time = -1;

	i = eyelink_get_next_data(NULL);  // Checks for data from link
	if(i == ENDFIX)	  // only process ENDFIX events
	{
	    eyelink_get_float_data(&evt);  // get a copy of the ENDFIX event
	    if(evt.fe.eye == eye_used)	// only process data from desired eye?
	        eyemsg_printf("NEXT Event: %ld %ld", evt.fe.sttime,
			evt.fe.entime);
	}

	// makes copy of last item from eyelink_get_next_data
	i = eyelink_get_last_data(&evt);
	if (i == ENDFIX && evt.fe.eye == eye_used
	    && evt.fe.entime != prev_event_time)
	{
	    eyemsg_printf("LAST Event: %ld %ld", evt.fe.sttime, evt.fe.entime);
	    // Makes sure that we do not write out redundant information
	    prev_event_time = evt.fe.entime;
	}
	\endcode

	\b Output:
	\code
	SFIX L   7812616
	SFIX R   7812616
	EFIX L   7812616	7813232	620	  124.7	  274.0	     50
	EFIX R   7812616	7813232	620	  118.2	  262.4	     50
	SSACC L  7813236
	SSACC R  7813236
	MSG	7813254 NEXT Event: 7812616 7813232
	MSG	7813254 LAST Event: 7812616 7813232
	\endcode

	\sa \c eyelink_get_float_data(), \c eyelink_get_next_data() and \c eyelink_newest_float_sample()
 */
INT16 ELCALLTYPE eyelink_get_last_data(void FARTYPE *buf);

/* makes copy of most recent sample received               */
/* if <buff> is NULL, just checks if new                   */
/* returns -1 if none or error, 0 if old, 1 if new         */
/*! @ingroup messaging
	Gets an integer (unconverted) copy of the last/newest link data (sample or event)
	seen by \c eyelink_get_next_data().

	@param buf Pointer to buffer (\c ISAMPLE, \c ALL_DATA type).
	@return \c 0 if no data, else \c SAMPLE_TYPE.

	\b Example:
	\code
	ISAMPLE isample;
	float x, y;		 // gaze position

	if(eyelink_newest_sample(NULL)>0)  // check for new sample update
	{
	    eyelink_newest_sample(&isample);

	    if (x!=MISSING_DATA)
	        x = ((float) isample.gx[eye_used])/((float) eyelink_position_prescaler());    // yes: get gaze position from sample
	    else
	        x = (float) MISSING_DATA;

	    if (y!=MISSING_DATA)
	        y = ((float) isample.gy[eye_used])/((float) eyelink_position_prescaler());
	    else
	        y = (float) MISSING_DATA;

	    ...
	    //  code for processing the sample
	    ...
	}
	\endcode

	\sa \c eyelink_get_float_data(), \c eyelink_get_last_data(), \c eyelink_get_next_data(),
		\c eyelink_get_sample() and \c eyelink_newest_float_sample()
 */
INT16 ELCALLTYPE eyelink_newest_sample(void FARTYPE *buf);

/*! @ingroup messaging
	Reads the last item fetched by \c eyelink_get_next_data() into a buffer.  The event is
	converted to a floating-point format (\c FSAMPLE or \c FEVENT).  This can handle both samples
	and events.  The buffer type can be \c ALLF_DATA for both samples and events, \c FSAMPLE for
	a sample, or a specific event buffer.

	@param buf Pointer to buffer for floating-point data: type is \c ALLF_DATA or \c FSAMPLE.
	@return \c 0 if no data, \c SAMPLE_TYPE if sample, else event type code.

	\b Example: See \c eyelink_get_next_data()

	\sa \c eyelink_get_last_data(), \c eyelink_get_next_data(), \c eyelink_newest_float_sample()
		\c eyelink_newest_double_sample() \c eyelink_get_double_data() and \c eyelink_newest_sample()
 */
INT16 ELCALLTYPE eyelink_get_float_data(void FARTYPE *buf);

/*! @ingroup messaging
	Reads the last item fetched by \c eyelink_get_next_data() into a buffer.  The event is
	converted to a floating-point format with floating point time (\c DSAMPLE or \c DEVENT).  
	This can handle both samples and events.  The buffer type can be \c ALLD_DATA for both 
	samples and events, \c DSAMPLE for a sample, or a specific event buffer.

	@param buf Pointer to buffer for floating-point data: type is \c ALLD_DATA or \c DSAMPLE.
	@return \c 0 if no data, \c SAMPLE_TYPE if sample, else event type code.

	\b Example: See \c eyelink_get_next_data()

	\sa \c eyelink_get_last_data(), \c eyelink_get_next_data(), \c eyelink_newest_float_sample()
		\c eyelink_newest_double_sample() \c eyelink_get_float_data() and \c eyelink_newest_sample()
 */
INT16 ELCALLTYPE eyelink_get_double_data(void FARTYPE *buf);
/* makes FSAMPLE copy of most recent sample                */
/*! @ingroup messaging
	Check if a new sample has arrived from the link.  This is the latest sample, not the
	oldest sample that is read by \c eyelink_get_next_data(), and is intended to drive gaze
	cursors and gaze-contingent displays.
	Typically the function is called with a \c NULL buffer pointer, to test if new data has
	arrived.  If a value of \c 1 is returned, the function is called with a \c FSAMPLE buffer to
	get the new sample.

	@param buf Pointer to sample buffer type \c FSAMPLE.  If \c NULL, just checks new-sample status.
	@return \c -1 if no samples, \c 0 if no new data, \c 1 if new sample.

	\b Example:
	\code
	// This program illustrates the use of eyelink_newest_float_sample
	#include <eyelink.h>

	ALLF_DATA evt;         // buffer to hold sample and event data
	int eye_used = 0;      // indicates which eye's data to display
	float x, y;		// gaze position

	// Recording with link data enabled
	error = start_recording(1,1,1,0);
	if(error != 0) return error;   // ERROR: couldn't start recording

	// Determines which eye(s) are available
	eye_used = eyelink_eye_available();

	// Selects eye, add annotation to EDF file
	switch(eye_used)
	{
	    case RIGHT_EYE:
	        eyemsg_printf("EYE_USED 1 RIGHT");
	        break;
	    case BINOCULAR:   // both eye's data present: use left eye only
	        eye_used = LEFT_EYE;
	    case LEFT_EYE:
	        eyemsg_printf("EYE_USED 0 LEFT");
	        break;
	}
	while (1)
	{
	    // check for new sample update
	    if(eyelink_newest_float_sample(NULL)>0)
	    {
	        // get the sample
	        eyelink_newest_float_sample(&evt);

	        x = evt.fs.gx[eye_used];
	        y = evt.fs.gy[eye_used];

	        // make sure pupil is present
	        if(x!=MISSING_DATA && y!=MISSING_DATA && evt.fs.pa[eye_used]>0)
	            eyemsg_printf("Sample: %ld %8.2f %8.2f",
	                evt.fs.time, x, y);
	    }
	    ...
	    // Code for exiting, display drawing etc.
	}
	\endcode

	\b Output:
	\code
	MSG	23701980 EYE_USED 0 LEFT
	MSG	23703426 Sample: 23703424   412.90   217.90
	MSG	23703430 Sample: 23703426   433.20   216.20
	MSG	23703430 Sample: 23703428   453.40   214.40
	MSG	23703434 Sample: 23703430   473.60   212.60
	MSG	23703434 Sample: 23703432   493.80   210.80
	MSG	23703438 Sample: 23703434   514.00   209.00
	MSG	23703438 Sample: 23703436   534.20   207.10
	MSG	23703442 Sample: 23703438   554.30   205.20
	MSG	23703442 Sample: 23703440   574.40   203.30
	MSG	23703446 Sample: 23703442   594.50   201.30
	MSG	23703446 Sample: 23703444   614.60   199.30
	MSG	23703450 Sample: 23703446   634.70   197.20
	MSG	23703450 Sample: 23703448   634.70   197.20
	\endcode

	\sa \c eyelink_get_float_data(), \c eyelink_get_last_data(), \c eyelink_get_next_data(),
		\c eyelink_get_sample() \c eyelink_newest_double_sample() and \c eyelink_newest_sample()
 */
INT16 ELCALLTYPE eyelink_newest_float_sample(void FARTYPE *buf);

/*! @ingroup messaging
	Check if a new sample has arrived from the link.  This is the latest sample, not the
	oldest sample that is read by \c eyelink_get_next_data(), and is intended to drive gaze
	cursors and gaze-contingent displays.
	Typically the function is called with a \c NULL buffer pointer, to test if new data has
	arrived.  If a value of \c 1 is returned, the function is called with a \c DSAMPLE buffer to
	get the new sample.

	@param buf Pointer to sample buffer type \c DSAMPLE.  If \c NULL, just checks new-sample status.
	@return \c -1 if no samples, \c 0 if no new data, \c 1 if new sample.

	\sa \c eyelink_get_float_data(), \c eyelink_get_last_data(), \c eyelink_get_next_data(),
		\c eyelink_get_sample() \c eyelink_newest_float_sample() and \c eyelink_newest_sample()
*/
INT16 ELCALLTYPE eyelink_newest_double_sample(void FARTYPE *buf);
/* ONLY VALID AFTER STARTSAMPLES EVENT READ                */
/*! @ingroup messaging
	After calling \c eyelink_wait_for_block_start(), or after at least one sample or
	eye event has been read, can be used to check which eyes data is available for.

	@return One of these constants, defined in EYE_DATA.H:
			\c LEFT_EYE if left eye data.
			\c RIGHT_EYE if right eye data.
			\c BINOCULAR if both left and right eye data.
			\c -1 if no eye data is available.

	\b Example:
	\code
	// This program illustrates the use of eyelink_eye_available()

	#include <eyelink.h>

	int eye_used = 0;      // indicates which eye's data to display

	// Determines which eye(s) are available
	eye_used = eyelink_eye_available();

	// Selects eye, add annotation to EDF file
	switch(eye_used)
	{
	    case RIGHT_EYE:
	        eyemsg_printf("EYE_USED 1 RIGHT");
	        break;
	    case BINOCULAR:   // both eye's data present: use left eye only
	        eye_used = LEFT_EYE;
	    case LEFT_EYE:
	        eyemsg_printf("EYE_USED 0 LEFT");
	        break;
	}
	\endcode

	\b Output:
	\code
	MSG	22157314 EYE_USED 0 LEFT
	\endcode

	\sa \c eyelink_wait_for_block_start()
 */
INT16 ELCALLTYPE eyelink_eye_available(void);

/*  ONLY VALID AFTER STARTSAMPLES EVENT READ */
/*! @ingroup messaging
	Gets sample data content flag (\c 0 if not in sample block).
 */
UINT16 ELCALLTYPE eyelink_sample_data_flags(void);

/* gets event data content flag (0 if not in event block)   */
/* ONLY VALID AFTER STARTEVENTS EVENT READ                  */
/*! @ingroup messaging
	Returns the event data content flags.

	@remarks This will be \c 0 if the data being read from queue is not in a block with events.
	@return Event data content flags:
			\c EVENT_VELOCITY if has velocity data.
			\c EVENT_PUPILSIZE if has pupil size data.
			\c EVENT_GAZERES if has gaze resolution.
			\c EVENT_STATUS if has status flags.
			\c EVENT_GAZEXY if has gaze xy position.
			\c EVENT_HREFXY if has head-ref xy position.
			\c EVENT_PUPILXY if has pupil xy position.
			\c FIX_AVG_ONLY if only avg. data to fixation events.
			\c START_TIME_ONLY if only start-time in start events.
			\c PARSEDBY_GAZE if how events were generated.
			\c PARSEDBY_HREF.
			\c PARSEDBY_PUPIL.

	\b Example:
	\code
	// This program illustrates the use of eyelink_get_sample()

	#include <eyelink.h>
	#include <stdio.h>
	int error;

	// Recording with link data enabled
	error = start_recording(1,1,1,0);
	if(error != 0) return error;   // ERROR: couldn't start recording

	// Wait for link sample data
	if(!eyelink_wait_for_data(100, 1, 0))
	{
	    printf("ERROR: No link samples received!");
	    return -1;
	}

	// gets event data/type content flag
	emsg_printf("Event data%d Event type %d",
	    eyelink_event_data_flags(), eyelink_event_type_flags());
	\endcode

	\b Output:
	\code
	MSG	2689937 Event data 26316 Event type 29760
	\endcode

	\sa \c eyelink_event_type_flags()
 */
UINT16 ELCALLTYPE eyelink_event_data_flags(void);

/* gets event type content flag (0 if not in event block)   */
/* ONLY VALID AFTER STARTEVENTS EVENT READ                  */
/*! @ingroup messaging
	After at least one button or eye event has been read, can be used to check what type of
	events will be available.

	@return A set of bit flags:
			\c LEFTEYE_EVENTS if has left eye events.
			\c RIGHTEYE_EVENTS if has right eye events.
			\c BLINK_EVENTS if has blink events.
			\c FIXATION_EVENTS if has fixation events.
			\c FIXUPDATE_EVENTS if has fixation updates.
			\c SACCADE_EVENTS if has saccade events.
			\c MESSAGE_EVENTS if has message events.
			\c BUTTON_EVENTS if has button events.
			\c INPUT_EVENTS if has input port events.

	\b Example: See \c eyelink_event_data_flags()

	\sa \c eyelink_event_data_flags()
 */
UINT16 ELCALLTYPE eyelink_event_type_flags(void);

/*! @ingroup messaging
	Counts total items in queue: samples, events, or both.

	@param samples If non-zero count the samples.
	@param events If non-zero count the events.
	@return Total number of samples and events is in the queue.

	\b Example:
	\code
	// This program uses eyelink_data_count() to check whether the desired sample/event data
	// have arrived after the start_recording()

	// Check whether the desired sample/event data have arrived.  This function should be
	// called after start_recording()
	// link_samples: Whether samples should be sent over the link
	// link_events: Whether events should be sent over the link

	int check_sample_event_status(int link_samples, int link_events)
	{
	    UINT32 t = current_time();
	    int i;

	    // Checks the sample and event availability in 300 msec
	    while(current_time() < t + 300)
	    {
	        int arrived = 1;
	        // check that recording still OK
	        if((i=check_recording())!=0) return i;

	        // Checks whether the samples, if set, have arrived
	        if(link_samples)
	            if(eyelink_data_count(1,0)==0)
	                arrived = 0;

	        // Checks whether the samples, if set, have arrived
	        if(link_events)
	            if(eyelink_data_count(0,1)==0)
	                arrived = 0;

	        // Exit if desired data type(s) have arrived
	        if(arrived) return 0;
	    }

	    // If the desired data type(s) didn't arrive within 300 msec
	    return -1;
	}
	\endcode
 */
INT16 ELCALLTYPE eyelink_data_count(INT16 samples, INT16 events);

/* maxwait = 0 to just test                                 */
/*! @ingroup messaging
	Waits for data to be received from the eye tracker.  Can wait for an event, a sample, or
	either.  Typically used after record start to check if data is being sent.

	@param maxwait Time in milliseconds to wait for data.
	@param samples If \c 1, return when first sample available.
	@param events If \c 1, return when first event available.
	@return \c 1 if data is available. \c 0 if timed out.

	\b Example:
	\code
	// This program illustrates the use of eyelink_wait_for_data()

	#include <eyelink.h>
	#include <stdio.h>
	int error;

	// Recording with link data enabled
	error = start_recording(1,1,1,0);
	if(error != 0) return error;   // ERROR: couldn't start recording

	// Wait for link sample data
	if(!eyelink_wait_for_data(100, 1, 0))
	{
	    printf("ERROR: No link samples received!");
	    return -1;
	}
	\endcode

	\sa \c eyelink_wait_for_block_start()
 */
INT16 ELCALLTYPE eyelink_wait_for_data(UINT32 maxwait,
                INT16 samples, INT16 events);

/*! @ingroup messaging
	Gets an integer (unconverted) sample from end of queue, discards any events encountered.

	@param sample Pointer to buffer (\c ISAMPLE or \c ALL_DATA type).
	@return \c 0 if no data. \c 1 if data retrieved.

	\b Example:
	\code
	// This program illustrates the use of eyelink_get_sample()

	#include <eyelink.h>

	ISAMPLE   isample; // INTEGER SAMPLE DATA
	int eye_used = 0;  // indicates which eye's data to display
	float x, y;		   // gaze position
	int i;

	// wait for link sample data
	if(!eyelink_wait_for_block_start(100, 1, 0))
	{
	    end_trial();
	    return TRIAL_ERROR;
	}

	// determine which eye(s) are available
	eye_used = eyelink_eye_available();
	switch(eye_used) // select eye, add annotation to EDF file
	{
	case RIGHT_EYE:
	    eyemsg_printf("EYE_USED 1 RIGHT");
	    break;
	case BINOCULAR: // both eye's data present: use left eye only
	    eye_used = LEFT_EYE;
	case LEFT_EYE:
	    eyemsg_printf("EYE_USED 0 LEFT");
	    break;
	}

	while(1)
	{
	    // get an integer copy of sample: skips any events
	    i = eyelink_get_sample(&isample);
	    if(i)
	    {
	        // convert the integrer eye data to float data
	        // eyelink_position_prescaler() is used as a divisor
	        if (x!=MISSING_DATA)
	            x = ((float) isample.gx[eye_used])/
	                ((float) eyelink_position_prescaler());
	        else
	            x = (float) MISSING_DATA;

	        if (y!=MISSING_DATA)
	            y = ((float) isample.gy[eye_used])/
	                ((float) eyelink_position_prescaler());
	        else
	            y = (float) MISSING_DATA;

	        eyemsg_printf("Sample: %ld %6.2f %6.2f", isample.time, x, y);
	    }

	    ...
	    // Code for exiting, display drawing etc.
	}
	\endcode

	\b Output:
	\code
	MSG	14839670 Sample: 14839666 539.20 372.60
	MSG	14839670 Sample: 14839668 539.20 372.60
	MSG	14839674 Sample: 14839670 539.20 372.60
	MSG	14839674 Sample: 14839672 539.20 372.60
	MSG	14839678 Sample: 14839674 547.90 367.60
	MSG	14839678 Sample: 14839676 556.60 362.50
	MSG	14839682 Sample: 14839678 565.30 357.40
	MSG	14839682 Sample: 14839680 574.10 352.30
	MSG	14839686 Sample: 14839682 582.80 347.20
	MSG	14839686 Sample: 14839684 591.50 342.00
	MSG	14839690 Sample: 14839686 600.30 336.80
	MSG	14839690 Sample: 14839688 609.00 331.60
	MSG	14839694 Sample: 14839690 617.80 326.40
	MSG	14839694 Sample: 14839692 626.60 321.20
	MSG	14839698 Sample: 14839694 635.30 315.90
	MSG	14839698 Sample: 14839696 644.10 310.70
	MSG	14839702 Sample: 14839698 652.90 305.40
	MSG	14839702 Sample: 14839700 661.70 300.00
	MSG	14839706 Sample: 14839702 670.50 294.70
	MSG	14839706 Sample: 14839704 679.30 289.40
	MSG	14839710 Sample: 14839706 688.10 284.00
	MSG	14839710 Sample: 14839708 696.90 278.60
	MSG	14839714 Sample: 14839710 705.80 273.20
	MSG	14839714 Sample: 14839712 714.60 267.70
	MSG	14839718 Sample: 14839714 723.40 262.30
	MSG	14839718 Sample: 14839716 732.30 256.80
	MSG	14839722 Sample: 14839718 741.20 251.30
	MSG	14839722 Sample: 14839720 750.00 245.80
	\endcode

	\sa \c eyelink_get_float_data(), \c eyelink_get_last_data(), \c eyelink_get_next_data(),
		\c eyelink_newest_float_sample() and \c eyelink_newest_sample()
 */
INT16 ELCALLTYPE eyelink_get_sample(void FARTYPE *sample);

/*! \addtogroup recording
 *  @{
 */
#define RECORD_FILE_SAMPLES  1	/*!< only active if file open */
#define RECORD_FILE_EVENTS   2	/*!< only active if file open */
#define RECORD_LINK_SAMPLES  4	/*!< accept samples from link */
#define RECORD_LINK_EVENTS   8	/*!< accept events from link */
/*!
  *  @}
  */
/* controls what is accepted from link                      */
/* use eyelink_data_start() if tracker mode must be changed */
/*! @ingroup messaging

	Sets what data from tracker will be accepted and placed in queue.

	@remarks This does not start the tracker recording, and so can be used with
				\c eyelink_broadcast_open().  It also does not clear old data from the
				queue.  The data is set with a bitwise OR of these flags:
					\c RECORD_LINK_SAMPLES	- send samples on link.
					\c RECORD_LINK_EVENTS	- send events on link.
	@param flags Bitwise OR flags.
	@return \c 0 if OK else link error.

	\b Example: See \c eyelink_in_data_block()

	\sa \c eyelink_in_data_block()
 */
INT16 ELCALLTYPE eyelink_data_switch(UINT16 flags);

/* start data flow                                          */
/* also aborts any other operations (setup, drift corr.)    */
/*! @ingroup messaging
	Switches tracker to Record mode, enables data types for recording to EDF file or sending to
	link.  These types are set with a bitwise OR of these flags:
		\arg <tt>RECORD_FILE_SAMPLES (1)</tt> - only active if file open.
		\arg <tt>RECORD_FILE_EVENTS (2)</tt> - only active if file open.
		\arg <tt>RECORD_LINK_SAMPLES (4)</tt> - accept samples from link.
		\arg <tt>RECORD_LINK_EVENTS (8)</tt> - accept events from link.

	@remarks If \c &lt;lock&gt; is nonzero, the recording may only be terminated through
				\c stop_recording() or \c eyelink_data_stop(), or by the Abort menu
				(‘Ctrl’ ’Alt’ ’A’ keys on the eye tracker).  If zero, the tracker 'ESC'
				key may be used to halt recording.
	@param flags Bitwise OR of flags to control what data is recorded.  If \c 0, recording will be stopped.
	@param lock If nonzero, prevents 'ESC' key from ending recording.
	@return \c 0 if command sent OK, else link error.

	\b Example:
	\code
	// This program illustrates the use of eyelink_data_start() and eyelink_data_stop()
	// functions for trial recording

	#include <eyelink.h>

	// data types requested for the EDF file and link data
	INT16 file_samples,  file_events, link_samples, link_events;

	...
	// Functions to set the file and link data type
	// f(file_samples,  file_events, link_samples, link_events);
	...

	// Checks whether we still have the connection
	if(eyelink_is_connected()) 	return ABORT_EXPT;

	i = eyelink_data_start((file_samples?RECORD_FILE_SAMPLES:0) |
	                       (file_events?RECORD_FILE_EVENTS:0)   |
	                       (link_samples?RECORD_LINK_SAMPLES:0) |
	                       (link_events?RECORD_LINK_EVENTS:0) , 1);

	if(i) return i;

	// wait for mode change completion
	i = eyelink_wait_for_mode_ready(500);
	if(i==0) return TRIAL_ERROR;

	// Checks that recording started OK
	if((i = check_recording())!=0)
	    return i;
	...
	 // Recording code here
	...

	// Stops data flow and ends recording
	eyelink_data_stop();
	// wait for mode change completion
	eyelink_wait_for_mode_ready(500);
	\endcode

	\sa \c eyelink_data_stop(), \c start_recording() and \c stop_recording()
 */
INT16 ELCALLTYPE eyelink_data_start(UINT16 flags, INT16 lock);

/* stop data flow                                           */
/* also aborts any other operations (setup, drift corr.)    */
/*! @ingroup messaging
	Places tracker in idle (off-line) mode, does not flush data from queue.

	@remarks Should be followed by a call to \c eyelink_wait_for_mode_ready().
	@return \c 0 if command sent OK, else link error.

	\b Example: See \c eyelink_data_start()

	\sa \c eyelink_data_start() and \c eyelink_wait_for_mode_ready()
 */
INT16 ELCALLTYPE eyelink_data_stop(void);


/************* DATA PLAYBACK *************/
/* All data in file is played back over link,               */
/* with file (not link) data selected.                      */
/* During playback, the IN_PLAYBACK_MODE bit is set         */
/* End of playback is signalled when there is no data       */
/* available AND the IN_PLAYBACK_MODE bit is cleared        */
/*! @ingroup playback
	Flushes data from queue and starts data playback.  An EDF file must be open and have at
	least one recorded trial.  Use \c eyelink_wait_for_data() to wait for data: this will time
	out if the playback failed.  Playback begins from start of file or from just after the
	end of the next-but-last recording block.  Link data is determined by file contents,
	not by link sample and event settings.

 	@return \c 0 if command sent OK, else link error.

	\b Example:
	\code
	// This program illustrates the use of eyelink_playback_start() and eyelink_playback_stop()
	// functions for trial data playback.  See EYEDATA template for a complete example

	#include <eyelink.h>
	#include <stdio.h>

	set_offline_mode();           // set up eye tracker for playback
	eyelink_playback_start();     // start data playback

	// wait for first data to arrive
	if(!eyelink_wait_for_block_start(2000, 1, 1))
	{
	    printf("ERROR: playback data did not start!");
	    return -1;
	}

	while(1)   // Loop while data available
	{
	    // Exit if ESC, ALT-F4, CTRL-C, or tracker button pressed
	    if(escape_pressed() || break_pressed() ||
		eyelink_last_button_press(NULL))
	    {
	        eyelink_playback_stop(); // stop playback
	        return 0;
	    }

	    // Process playback data from the link
	    i = eyelink_get_next_data(NULL);   // check for new data item
	    if(i==0)	                       // 0: no new data
	    {   // Checks if playback has completed
	        if((eyelink_current_mode() & IN_PLAYBACK_MODE)==0) break;
	    }
	    ...
	    // Code for processing data and delay handling here
	    ...
	}

	// End of playback loop
	eyelink_playback_stop();
	\endcode

	\sa \c eyelink_playback_stop() and \c eyelink_wait_for_block_start()
 */
INT16 ELCALLTYPE eyelink_playback_start(void);

/*! @ingroup playback
	Stops playback if in progress.  Flushes any data in queue.

	@return \c 0 if mode switched, else link error.

	\b Example: See \c eyelink_playback_start()

	\sa \c eyelink_playback_start() and \c eyelink_wait_for_block_start()
 */
INT16 ELCALLTYPE eyelink_playback_stop(void);


/************ READ IMAGE DATA **********/

/* IMAGE TYPES: */
/* 0 = monochrome, packed (8 pixels/byte)                    */
/* 1 = 16 color, packed (2 pixels/byte)                      */
/* 2 = 16 color, 4 planes (8 pixels/byte, 4 planes of data
    in seq per line)                                         */
/* 3 = 256 color, (1 pixel/byte)                             */

#define ELIMAGE_2   0    /* 1 plane, 1 bit per pixel (2 colors)   */
#define ELIMAGE_16  1    /* 4 bits per pixel, packed (16 colors)  */
#define ELIMAGE_16P 2    /* 1 bit per pixel, 4 planes (16 colors) */
#define ELIMAGE_256 3    /* 8 bits per pixel (256 colors, EL-II)   */
#define ELIMAGE_128HV  4    /* 7 bits per pixel, 2-D compression, EL-II V2.0 and later)   */
#define ELIMAGE_128HVX 5    /* same as previous, but no crosshairs in image  */


/*! @internal

	Request an image of \c &lt;type&gt; with size less than or eqaul to \c &lt;xsize&gt;
	and \c &lt;ysize&gt;.

	@return \c 0 if sent OK, else send error code.
 */
INT16 ELCALLTYPE eyelink_request_image(INT16 type, INT16 xsize, INT16 ysize);

/*! @internal

	Test image-reception status.
	@return \c 0 if not receiving, \c -1 if aborted, \c 1 if receiving.
 */
INT16 ELCALLTYPE eyelink_image_status(void);

/*! @internal

	Forces image transmissin to halt.
 */
void ELCALLTYPE eyelink_abort_image(void);

/*! @internal
	Get data at start of new image.
	@return \c -1 if aborted/not in receive, \c 0 if old palette, \c 1 if new palette.
	ptrs to size: may be NULL
 */
INT16 ELCALLTYPE eyelink_image_data(INT16 FARTYPE *xsize, INT16 FARTYPE *ysize, INT16 FARTYPE *type);

/*! @internal
	Gets unpacked line data.
	@return \c -1 if error/not rx, else line number.
 */
INT16 ELCALLTYPE eyelink_get_line(void FARTYPE *buf);



#ifndef PALDATADEF
                /* NEW palette def: max 48 bytes */
/*! @internal

	image data and palette structure uses brightness ramp plus special colors 
	to compress and make remapping easier 	fits in old palette's 48-byte area 
 */
typedef struct {
    byte palette_id;    /* palette id number */
    byte ncolors;        /* colors-1 (256 colors = 255) */

    byte camera;            /* camera: 1-3 */
    byte threshold;         /* image threshold */
    UINT16 flags;           /* flags: polarity, pupil present, etc. */
    UINT16 image_number;    /* sequence mod 65536 */
    byte extra[10];         /* future expansion */

    byte rfirst_color;        /* brightness ramp */
    byte rfirst_brite;      /* brite is 0-255 */
    byte rlast_color;
    byte rlast_brite;
    INT16 nspecial;         /* number of special colors */

    struct {
         byte index;        /* palette entry */
         byte r;            /* rgb is 0-255 */
         byte g;
         byte b;
    } spcolors[6];            /* up to 6 special colors */
} IMAGE_PALDATA;

#define PALDATADEF
#endif

/*! @internal

	Get palette: always ramp definition.
	@return \c -1 if no image in progress, \c 0 if old, \c 1 if new palette.

	non-palette data in *pal may change even if 0 returned.
 */
INT16 ELCALLTYPE eyelink_get_palette(void FARTYPE *pal);


/********** KEYS/BUTTONS **********/

/* key/button state flag indicates type of key/button action: */
#define KB_PRESS   10
#define KB_RELEASE -1
#define KB_REPEAT  1

/* Key modifiers are similar to BIOS INT 16h function 2   */
/* DOS key modifiers: a set of flag bits.  By using these */
/* to test the <mods> flag, you can tell if special       */
/* key combinations were used.                            */

#define NUM_LOCK_ON      0x20
#define CAPS_LOCK_ON     0x40
#define ALT_KEY_DOWN     0x08
#define CTRL_KEY_DOWN    0x04
#define SHIFT_KEY_DOWN   0x03   /* left, right shift keys */

/* The key code itself is what BIOS INT 16h function 0             */
/* reads: ASCII for most keys, with other codes for ENTER,ESC etc. */
/* For the extended DOS keys, the code has the extended            */
/* code in the MSByte, with the LSbyte 0                           */

/* A SPECIAL CASE: */
/* codes of either 0 or 0xFF00 mean the mod is a button number */

#define KB_BUTTON 0xFF00U

#define F1_KEY    0x3B00    /* some samples */
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


/* returns: 0 if none, keycode if key,           */
/* KB_BUTTON (mods=button number) if button      */
/*! @ingroup elbutton
	Reads any queued key or button events from tracker.

	@remarks Any of the parameters(mods/state/kcode/time) can be null to ignore.
	@param mods Pointer to variable to hold button number or key modifier (Shift, Alt and Ctrl key states).
	@param state Pointer to variable to hold key or button change (\c KB_PRESS, \c KB_RELEASE, or \c KB_REPEAT).
	@param kcode Pointer to variable to hold key scan code.
	@param time Pointer to a variable to hold tracker time of the key or button change.
	@return Key character is key press/release/repeat, <tt>KB_BUTTON (0xFF00)</tt> if button press or release. \c 0 if none.

	\b Example:
	\code
	// This program illustrates the use of eyelink_read_keybutton to read key press events from the tracker keyboard

	#include <eyelink.h>

	UINT16 	key;
	INT16 	state;

	// Reads any queued key or button events from the tracker keyboard
	key = eyelink_read_keybutton(NULL, &state, NULL, NULL);

	// Makes sure that we checks only the key press
	if (key && state == KB_PRESS && key != KB_BUTTON)
	{
	    // Writes out the pressed key id
	    if(key < 256 && isprint(key))
	        eyemsg_printf("KEY '%c'", key);
	    else
	        eyemsg_printf("WAITKEY 0x%04X", key);
	}
	\endcode

	\sa \c eyelink_send_keybutton()
 */
UINT16 ELCALLTYPE eyelink_read_keybutton(INT16 FARTYPE *mods,
                  INT16 FARTYPE *state, UINT16 *kcode, UINT32 FARTYPE *time);

/*! @ingroup elbutton
	Sends a key or button event to tracker.  Only key events are handled for remote control.

	@param code Key character, or <tt>KB_BUTTON (0xFF00)</tt> if sending button event.
	@param mods Button number, or key modifier (Shift, Alt and Ctrl key states).
	@param state Key or button change (\c KB_PRESS or \c KB_RELEASE).
	@return \c 0 if OK, else send link error.

	\b Example:
	\code
	// This program illustrates the implementation of echo_key() function using the eyelink_send_keybutton()

	// ECHO_KEY() function is similar to getkey() but also echoes key to tracker
	UINT16 echo_key(void)
	{
	    UINT16 k = getkey();

	    if(k!=0 && k!=1)
	        eyelink_send_keybutton(k, 0, KB_PRESS);
	    return k;
	}
	\endcode

	\sa \c eyelink_read_keybutton()
 */
INT16 ELCALLTYPE eyelink_send_keybutton(UINT16 code, UINT16 mods, INT16 state);

/* reads the currently-known state of all buttons */
/* the bits in the returned value will be         */
/* set (1) if corresponding button is pressed     */
/* LSB is button 1, MSB is button 16              */
/* currently only 8 buttons available             */
/*! @ingroup elbutton
	Returns a flag word with bits set to indicate which tracker buttons are currently pressed.
	This is button 1 for the LSB, up to button 16 for the MSB.

	@remarks Buttons above 8 are not realized on the EyeLink tracker.
	@return Flag bits for buttons currently pressed.

	\b Example:
	\code
	// This program illustrates the use of eyelink_button_states

	#include <eyelink.h>
	int   state =0;
	int 	prev_state = 0;
	UINT32 start_time = current_time();

	// Exits when the tracker is not connected or times out
	while(eyelink_is_connected()
	      && current_time() > start_time + 5000)
	{
	    // reads the currently-known state of all buttons
	    state = eyelink_button_states();
	    if (state != prev_state)
	    {
	        eyemsg_printf("Button 1:%s 2:%s 3:%s 4:%s 5:%s",
	            state & 0x01 ? "Pressed" : "Released",
	            (state & 0x02) >> 1 ? "Pressed" : "Released",
	            (state & 0x04) >> 2 ? "Pressed" : "Released",
	            (state & 0x08) >> 3 ? "Pressed" : "Released",
	            (state & 0x10) >> 4 ? "Pressed" : "Released");

	       prev_state = state;
	    }
	}
	\endcode

	\b Output:
	\code
	BUTTON	4144034	1	1
	MSG	4144035 Button 1:Pressed 2:Released 3:Released 4:Released 5:Released
	BUTTON	4144266	1	0
	MSG	4144267 Button 1:Released 2:Released 3:Released 4:Released 5:Released
	BUTTON	4144650	2	1
	MSG	4144651 Button 1:Released 2:Pressed 3:Released 4:Released 5:Released
	BUTTON	4144898	2	0
	MSG	4144899 Button 1:Released 2:Released 3:Released 4:Released 5:Released
	BUTTON	4145260	3	1
	MSG	4145261 Button 1:Released 2:Released 3:Pressed 4:Released 5:Released
	BUTTON	4145492	3	0
	MSG	4145493 Button 1:Released 2:Released 3:Released 4:Released 5:Released
	BUTTON	4145834	4	1
	MSG	4145835 Button 1:Released 2:Released 3:Released 4:Pressed 5:Released
	BUTTON	4146106	4	0
	MSG	4146107 Button 1:Released 2:Released 3:Released 4:Released 5:Released
	BUTTON	4146498	5	1
	MSG	4146499 Button 1:Released 2:Released 3:Released 4:Released 5:Pressed
	BUTTON	4146778	5	0
	MSG	4146779 Button 1:Released 2:Released 3:Released 4:Released 5:Released
	\endcode

	\sa \c eyelink_last_button_press()
 */
UINT16 ELCALLTYPE eyelink_button_states(void);


/*! @ingroup elbutton
	Returns a flag word with bits set to indicate which tracker buttons are currently pressed.
	This is button 1 for the LSB, up to button 16 for the MSB.  Same as eyelink_button_states()
	except, optionally time of the button states can be acquired.

    @param[out] time pointer to return time of the button states.
	@return Flag bits for buttons currently pressed.
	\sa \c eyelink_send_keybutton()
 */
UINT16 ELCALLTYPE eyelink_last_button_states(UINT32 FARTYPE *time);

/* returns the last button pressed (1-16, 0 if none) */
/* clears button so reads 0 till another pressed     */
/*! @ingroup elbutton
	Reads the number of the last button detected by the EyeLink tracker.  This is \c 0
	if no buttons were pressed since the last call, or since the buttons were flushed.
	If a pointer to a variable is supplied the eye-tracker timestamp of the button may
	be read.  This could be used to see if a new button has been pressed since the last
	read.  If multiple buttons were pressed since the last call, only the last button
	is reported.

	@param time Far pointer to a variable to hold tracker time of last button press.
				Usually left as \c NULL to ignore time.
	@return Button last pressed, \c 0 if no button pressed since last read, or call to \c eyelink_flush_keybuttons().

	\b Example:
	\code
	// This program illustrates the use of eyelink_flush_keybuttons

	#include <eyelink.h>
	int   button;

	// reset keys and buttons from tracker
	eyelink_flush_keybuttons(0);

	while(1)
	{
	     // Check for eye-tracker buttons pressed
	     button = eyelink_last_button_press(NULL);

	     // Disables button 6 and 7 and process all other button events
	     if(button != 0 && button != 6 && button != 7)
	     {
	         // Presses button 5 to break the loop
	         if (button == 5)
	         {
	             eyemsg_printf("ENDBUTTON %d", button);
	             break;
	         }
	         // Records all other button press messages
	         else
	             eyemsg_printf("BUTTON PRESSED %d", button);
	     }
	}
	\endcode

	\b Output:
	\code
	BUTTON	19753748	2	1
	MSG	19753749 BUTTON PRESSED 2
	BUTTON	19754018	2	0
	BUTTON	19755595	5	1
	MSG	19755595 ENDBUTTON 5
	BUTTON	19755808	5	0
	\endcode

	\sa \c eyelink_flush_keybuttons(), \c eyelink_button_states(), \c eyelink_read_keybutton()
		and \c eyelink_send_keybutton()
 */
UINT16 ELCALLTYPE eyelink_last_button_press(UINT32 FARTYPE *time);


/* also updates button state flags, but these        */
/*  may not be valid for several milliseconds        */
/* if <enable_buttons> also stores buttons           */
/*  otherwise just updates for last_button_press()   */
/*! @ingroup elbutton
	Causes the EyeLink tracker and the EyeLink library to flush any stored button or key
	events.  This should be used before a trial to get rid of old button responses.
	The \c &lt;enable_buttons&gt; argument controls whether the EyeLink library will store
	button press and release events.  It always stores tracker key events.  Even if
	disabled, the last button pressed and button flag bits are updated.

	@param enable_buttons Set to \c 0 to monitor last button press only, \c 1 to queue button events.
	@return Always \c 0.

	\b Example:
	\code
	// This program illustrates the use of eyelink_flush_keybuttons.

	UINT32 wait_time = 5000;
	int i;

	// Flushes any waiting keys or buttons
	eyelink_flush_keybuttons(0);

	// Sets the time-out duration
	wait_time += current_msec();

	// Makes sure that the tracker connection is still alive
	while(eyelink_is_connected())
	{
	    // handle the error or abort situations
	    if(getkey()==27 || !eyelink_is_connected())
	        break;

	    // Checks the button response
	    i = eyelink_last_button_press(NULL);
	    if(i)
	    {
	        eyemsg_printf("WAITBUTTON %d", i);
	        break;
	    }

	    // Times out if no button is pressed
	    if(current_time() > wait_time)
	    {
	        eyemsg_printf("WAITBUTTON TIMEOUT");
	        break;
	    }
	}
	\endcode

	\b Output:
	\code
	BUTTON	19585661	5	1
	MSG	19585662 WAITBUTTON 5
	BUTTON	19586005	5	0
	\endcode

	\sa \c eyelink_button_states(), \c eyelink_last_button_press(), \c eyelink_read_keybutton()
		and \c eyelink_send_keybutton()
 */
INT16 ELCALLTYPE eyelink_flush_keybuttons(INT16 enable_buttons);



/************* FILE TRANSFER ************/

/*! @ingroup datafile
	Request send of file "src". If "", gets last data file.
	@return \c 0 if OK, else send error.
	@remark Internal function. use receive_data_file()
 */
INT16 ELCALLTYPE eyelink_request_file_read(char FARTYPE *src);

/* error codes returned  in size field */
#define FILE_XFER_ABORTED  -110
#define FILE_CANT_OPEN     -111
#define FILE_NO_REPLY      -112  /* no-data returned */
#define FILE_BAD_DATA      -113

#define FILEDATA_SIZE_FLAG  999  /* start block has name, offset=total size   */
#define FILE_BLOCK_SIZE     512  /* full block size: if less, it's last block */

/*! @ingroup datafile
	Get next block of file.  If \c &lt;offset&gt; is not \c NULL, will be filled
	with block-start offset in file.

	@return negative if error, \c NO_REPLY if waiting for packet, else block size (0..512).
	size is < 512 (can be 0) if at EOF.

	@remark Internal function. use receive_data_file()
 */
INT16 ELCALLTYPE eyelink_get_file_block(void FARTYPE *buf, INT32 FARTYPE *offset);

/*! @ingroup datafile
	Ask for next block of file, reads from \c &lt;offset&gt;.
	@remark Internal function. use receive_data_file()
 */
INT16 ELCALLTYPE eyelink_request_file_block(UINT32 offset);

/*! @ingroup datafile
	Aborts send of file.
	@return \c 0 if OK, else send error.
	@remark Internal function. use receive_data_file()
 */
INT16 ELCALLTYPE eyelink_end_file_transfer(void);


/***************** EYELINK II EXTENSIONS *****************/

/*! @ingroup init_eyelink
	After connection, determines if the connected tracker is an EyeLink I or II.

	@remarks For the EyeLink II tracker, it can optionally retrieve the tracker software version.
	@param c \c NULL, or pointer to a string (at least 40 characters) to hold the version string.
				This will be "EYELINK I" or "EYELINK II x.xx", where "x.xx" is the software version.
	@return \c 0 if not connected, \c 1 for EyeLink I, \c 2 for EyeLink II.

	\b Example:
	\code
	// This program illustrates the use of eyelink_get_tracker_version

	#include <eyelink.h>

	int i;
	char version_info[256];

	i = eyelink_get_tracker_version(version_info);
	eyemsg_printf("EyeLink %d version %s", i, version_info);
	\endcode

	\b Output:
	\code
	MSG	500850 EyeLink 2 version EYELINK II 1.10
	\endcode
 */
INT16 ELCALLTYPE eyelink_get_tracker_version(char FARTYPE *c);

/*! @ingroup recording
	\deprecated Use \c eyelink_mode_data() instead.
 */
INT16 ELCALLTYPE eyelink2_mode_data(INT16 *sample_rate, INT16 *crmode,
                                  INT16 *file_filter, INT16 *link_filter);

/*! @ingroup recording
	After calling \c eyelink_wait_for_block_start(), or after at least one sample or eye event has
 	been read, returns EyeLink II extended mode data.

  	@param sample_rate \c NULL, or pointer to variable to be filled with samples per second.
 	@param crmode \c NULL, or pointer to variable to be filled with CR mode flag (\c 0 if pupil-only mode,
 			else pupil-CR mode).
 	@param file_filter \c NULL, or pointer to variable to be filled with filter level to be applied to
 			file samples (\c 0 = off, \c 1 = std, \c 2 = double filter).
 	@param link_filter \c NULL, or pointer to variable to be filled with filter level to be applied to
 			link and analog output samples (\c 0 = off, \c 1 = std, \c 2 = double filter).
 	@return If no data available -1 else 0.

 	\b Example:
 	\code
 	// This program illustrates the use of eyelink2_mode_data to check the sample rate and tracking
 	// mode of the application

 	#inlcude <eyelink.h>
 	#include <stdio.h>

 	int is_eyelink2;

 	// waits till a block of samples, events, or both is begun
 	if(!eyelink_wait_for_block_start(2000, 1, 1))
 	{
 	    printf("ERROR: No sample or event been detected!");
 	    return -1;
 	}

 	// Gets tracker version
 	is_eyelink2 = (2 == eyelink_get_tracker_version(NULL));

 	// For EyeLink II, determine sample rate and tracking mode
 	if(is_eyelink2 && !eyelink2_mode_data(&sample_rate, &crmode, NULL, NULL))
 	{
 	    eyemsg_printf("Sample rate: %d", sample_rate);
 	    eyemsg_printf("Tracking mode: %s", crmode?"CR":"Pupil only");
 	}
 	\endcode

 	\b Output:
 	\code
 	MSG	1151024 Sample rate: 250
 	MSG	1151024 Tracking mode: CR
 	\endcode
 */
INT16 ELCALLTYPE eyelink_mode_data(INT16 *sample_rate, INT16 *crmode,
                                  INT16 *file_filter, INT16 *link_filter);

/************ BITMAP TRANSFER ***************/

/*! @internal
	Send bitmap data packet to tracker.
	@remark This function is strictly internal. If you need to send bitmap to the tracker,
	use el_bitmap_to_backdrop(), gdi_bitmap_to_backdrop() or sdl_bitmap_to_backdrop().
	@param data pointer of the data to be sent.
	@param size size of the data to be sent.
	@param seq \c 1 for first packet, increases thereafter.
*/
INT16 ELCALLTYPE eyelink_bitmap_packet(void *data, UINT16 size, UINT16 seq);

/*! @internal
	Get bitmap ack count.
	Negative: special code or sequence number to restart at.
	Reading resets count to 0.
	@remark This function is strictly internal. If you need to send bitmap to the tracker,
	use el_bitmap_to_backdrop(), gdi_bitmap_to_backdrop() or sdl_bitmap_to_backdrop().
 */
INT16 ELCALLTYPE eyelink_bitmap_ack_count(void);

/* codes returned by ACK */
#define ABORT_BX -32000  /* signal to abort bitmap send                  */
#define PAUSE_BX -32001  /* signal that last packet dropped (full queue) */
#define DONE_BX  -32002  /* last block received OK                       */




/*********** NEW CONNECTION ADDRESS ************/

/*! @ingroup init_eyelink

	Address used for non-connected time requests and message sends. the "proper"
	way to do this is with the "node" type of functions but we allow a "back door"
	to simplify higher level support functions.  This is also the address used
	under Windows for looking for tracker (an IP broadcast is used on all
	other platforms). There is a bug in the Windows networking, causing broadcasts
	sent on all cards to have the IP source addres of only the first card. This
	means the tracker sends its connection reply to the wrong address. So the exact
	address or a subnet broadcast address (i.e. 100.1.1.255 for a subnet mask of
	255.255.255.0) needs to be set to that of the tracker.
 */
void ELCALLTYPE eyelink_set_tracker_node(ELINKADDR node);



/************** TRACKER TIME ESTIMATION ***************/

/*********************************************************************************
 * These function produce a local estimate of the eye tracker clock. There are
 * several timebases in the system: the eye tracker time, the local eyelink DLL
 * time, and the OS system time.  All of these may not run at the same speed,
 * differing by up to 200 ppm (2 milliseconds every 10 seconds).
 *
 * These functions give a reasonable esimate of the EyeLink trackertime that is
 * used in EDF files and for sample and event timestamps over the link.
 * The time estimate is perturbed by link delays, but will never decrease,
 * but may jump forwards sometimes or "freeze" for a few milliseconds.
 * Two resolution of time are given: milliseconds as returned by current_time(),
 * and microseconds as returned by current_double_usec().
 *********************************************************************************/

/*! @ingroup access_time_tracker
	Returns the current tracker time (in micro seconds) since the tracker application started.

	@return Each of these functions returns the current tracker time (in microseconds)
			since tracker initialization.

	\b Example:
	\code
	// This program illustrates the use of eyelink_tracker_double_usec()
	#include <eyelink.h>
	#include <stdio.h>

	int i;
	ALLF_DATA evt;

	i = eyelink_get_next_data(NULL);   // Checks for new data item
	switch(i)
	{
	    case SAMPLE_TYPE:
	    {
	        // Gets the new data
	        eyelink_get_float_data(&evt);
	        // Checks for the difference of the current tracker time and the
	        // time returned from the latest sample
	        if(eyelink_tracker_double_usec()/1000000 >(evt.time +2) )
	        printf("Oops it took longer than 2 milliseconds for the data to get
			here.\n");
	    }
	    break;
	}
	\endcode

	\sa \c eyelink_tracker_msec(), \c current_time(), \c eyelink_msec_offset() and
	\c eyelink_double_usec_offset()

 */
double ELCALLTYPE eyelink_tracker_double_usec(void);

/*! @ingroup access_time_tracker
	Returns the current tracker time (in milliseconds) since the tracker application started.

	@return Each of these functions returns the current tracker time (in microseconds)
			since tracker initialization.

	\b Example:
	\code
	// This program illustrates the use of eyelink_tracker_time()
	#include <eyelink.h>
	#include <stdio.h>

	int i;
	ALLF_DATA evt;

	i = eyelink_get_next_data(NULL);   // Checks for new data item
	switch(i)
	{
	    case SAMPLE_TYPE:
	    {
	        // Gets the new data
	        eyelink_get_float_data(&evt);
	        // Checks for the difference of the current tracker time and the
	        // time returned from the latest sample
	        if(eyelink_tracker_msec()/1000 >(evt.time +2) )
	        printf("Oops it took longer than 2 milliseconds for the data to get
			here.\n");
	    }
	    break;
	}
	\endcode

	\sa \c current_time(), \c eyelink_msec_offset(), \c eyelink_double_usec_offset() and
		\c eyelink_tracker_double_usec()
 */
UINT32 ELCALLTYPE eyelink_tracker_msec(void);
/*! @ingroup access_time_tracker
	See \c eyelink_tracker_msec()
 */
#define eyelink_tracker_time() eyelink_tracker_msec()

/*
    Current offset between local and tracker time in microseconds
    May "jiggle" by 50 usec or more.
*/
/*! @ingroup access_time_tracker
	Returns the time difference between the tracker time and display pc time.

	@return Returns the time difference between the tracker time and display pc
			time in microseconds.

	\sa \c current_time() and \c eyelink_tracker_msec()
 */
double ELCALLTYPE eyelink_double_usec_offset(void);

/*
  Current offset between local time and tracker time in milliseconds
  May rarely "jiggle" forwards and backwards by 1 unit.
*/
/*! @ingroup access_time_tracker
	Returns the time difference between the tracker time and display pc time.

	@return Returns the time difference between the tracker time and display pc time.

	\sa \c current_time(), \c eyelink_tracker_msec() and \c eyelink_tracker_double_usec()
 */
UINT32 ELCALLTYPE eyelink_msec_offset(void);
/*! @ingroup access_time_tracker
	See \c eyelink_time_offset()
 */
#define eyelink_time_offset() eyelink_msec_offset()




#endif /* SIMLINKINCL */

#ifdef __cplusplus    /* For C++ compilation */
}
#endif
#endif /* __SRRESEARCH__EYELINK_H__*/

