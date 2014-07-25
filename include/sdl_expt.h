/**********************************************************************************
 * EYELINK SDL EXPT SUPPORT      (c) 1996, 2003 by SR Research                    *
 *     15 July 2003 by Suganthan Subramaniam       For non-commercial use only    *
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
	\file sdl_expt.h
        \brief Declarations of eyelink_core_graphics functions and types. 
        
        This file will also reference the other EyeLink header files.
 */

#ifndef __SRRESEARCH__SDL_EXPT_H__
#define __SRRESEARCH__SDL_EXPT_H__
#ifdef __SRRESEARCH__GDI_EXPT_H__
#error sdl_expt.h should not be used with gdi_expt.h
#endif

#include <SDL/SDL.h>
#ifdef MACOSX
#include <eyelink_core/eyelink.h>
#include <eyelink_core/core_expt.h>	
#else
#include "eyelink.h"
#include "core_expt.h"
#endif
#ifdef __cplusplus     /* For C++ compilation */
extern "C" {
#endif

/*! @defgroup sdl_graphics SDL Graphics Functions
 *  @addtogroup sdl_graphics
 *  @{
 */
/**************************Convenient Macros *********************************/
#define    SDLRGB(x,y)   SDL_MapRGB(x->format,(y).r,(y).g,(y).b)
#define SCREEN_LEFT   dispinfo.left
#define SCREEN_TOP    dispinfo.top
#define SCREEN_RIGHT  dispinfo.right
#define SCREEN_BOTTOM dispinfo.bottom
#define SCRHEIGHT     dispinfo.height
#define SCRWIDTH      dispinfo.width


/*! Passes the colors of the display background and fixation target to the
	EXPTSPPT library.  During calibration, camera image display, and drift
	correction, the display background should match the brightness of the
	experimental stimuli as closely as possible, in order to maximize tracking
	accuracy.  This function passes the colors of the display background and
	fixation target to the EXPTSPPT library.  This also prevents flickering of
	the display at the beginning and end of drift correction.

 	@param fg Color used for drawing calibration target.
	@param bg Color used for drawing calibration background.

	\b Example: See \c do_tracker_setup()

	\sa \c do_tracker_setup()
 */
void  ELCALLTYPE  set_calibration_colors(SDL_Color *fg, SDL_Color* bg);

/*! The standard calibration and drift correction target is a disk (for
	peripheral delectability) with a central "hole" target (for accurate
	fixation).  The sizes of these features may be set with this function.

  	@param diameter Size of outer disk, in pixels.
	@param holesize Size of central feature.  If \c &lt;holesize&gt; is \c 0,
					no central feature will be drawn.

	\b Example: See \c do_tracker_setup()

	\sa \c do_tracker_setup()
 */
void  ELCALLTYPE  set_target_size(UINT16 diameter, UINT16 holesize);

/*! Selects the sounds to be played during \c do_tracker_setup(), including
	calibration, validation and drift correction.  These events are the
	display or movement of the target, successful conclusion of calibration
	or good validation, and failure or interruption of calibration or validation.

	@remarks If no sound card is installed, the sounds are produced as "beeps"
	from the PC speaker.  Otherwise, sounds can be selected by passing a string.
	If the string is "" (empty), the default sounds are played.  If the string
	is "off", no sound will be played for that event.  Otherwise, the string
	should be the name of a .WAV file to play.
	@param ontarget Sets sound to play when target moves.
	@param ongood Sets sound to play on successful operation.
	@param onbad Sets sound to play on failure or interruption.

	\b Example: See \c do_tracker_setup()

	\sa \c do_tracker_setup() and \c set_dcorr_sounds()
 */
void  ELCALLTYPE  set_cal_sounds(char *ontarget, char *ongood, char *onbad);

/*! Selects the sounds to be played during \c do_drift_correct().  These events
	are the display or movement of the target, successful conclusion of drift
	correction, and pressing the 'ESC' key to start the Setup menu.

	@remarks If no sound card is installed, the sounds are produced as "beeps"
	from the PC speaker.  Otherwise, sounds can be selected by passing a string.
	If the string is "" (empty), the default sounds are played.  If the string
	is "off", no sound will be played for that event.  Otherwise, the string
	should be the name of a .WAV file to play.
	@param ontarget Sets sound to play when target moves.
	@param ongood Sets sound to play on successful operation.
	@param onbad Sets sound to play on failure or interruption.

	\b Example: See \c do_tracker_setup()

	\sa \c do_tracker_setup() and \c set_cal_sounds()
 */
void  ELCALLTYPE  set_dcorr_sounds(char *ontarget, char *ongood, char *onbad);


/*!
 	To adjust camera image position. By default the camera is placed at the
 	centre of the screen.

	@param left Left position.
	@param top Top position.
	@param right Right position.
	@param bottom Bottom position.
 */
INT16 ELCALLTYPE  set_camera_image_position(INT16 left, INT16 top,
                                          INT16 right, INT16 bottom);


/*! Measures parameters of the current display mode, and fills a \c DISPLAYINFO
	structure with the data.  This process may take over 100 milliseconds, as
	it measures actual refresh rate.  The returned data can be used to compute
	sizes for drawing, and to check that the current display mode matches the
	requirements of the experiment.  A global \c DISPLAYINFO structure called
	dispinfo should be set up at the start of the program if you wish to use
	the \c SCRWIDTH and \c SCRHEIGHT macros.

	@remarks This is the contents of the \c DISPLAYINFO structure:
	\code
	typedef struct
	{
	     INT32 left;      // left of display
	     INT32 top;       // top of display
	     INT32 right;     // right of display
	     INT32 bottom;    // bottom of display
	     INT32 width;     // width of display
	     INT32 height;    // height of display
	     INT32 bits;      // bits per pixel
	     INT32 palsize;   // total entries in palette (0 if not indexed)
	     INT32 palrsvd;   // number of static entries in palette
	     INT32 pages;     // pages supported
	     float refresh;   // refresh rate in Hz
	     INT32 winnt;     // 0 for 9x/Me, 1 for NT, 2 for 2000, 3 for XP
	} DISPLAYINFO;
	\endcode
	If refresh cannot be measured, the "refresh" field will contain a value less than 40.
	@param di Pointer to \c DISPLAYINFO structure to fill.
 */
void  ELCALLTYPE  get_display_information(DISPLAYINFO *di);

/*! You must always create a borderless, full-screen window for your experiment.
	This function registers the window with EXPTSPPT so it may be used for
	calibration and drift correction.  The window should not be destroyed until
	it is released with \c close_expt_graphics(). This window will be subclassed
	(some messages intercepted) during calibration and drift correction.

	@param hwnd Handle of window that is to be used for calibration and drift
				correction.  If \c NULL is passed in, SDL initialized and 
				requested display mode is set.
	@param info \c NULL or pointer to a \c DISPLAYINFO structure to fill with
				display mode data.  If \c NULL is passed in, current display mode is
				used.
	@return \c 0 if success, \c -1 if error occurred internally.
	
	Default initialization of eyelinkn_core_library:
	\code
	int defaultGraphicsSetup()
	{
		DISPLAYINFO disp;
		memset(&disp,0,sizeof(DISPLAYINFO));
		
		disp.width =640;
		disp.height = 480;
		disp.bits =32;
		disp.refresh = 60;
		if(init_expt_graphics(NULL, &disp))
		{
			printf("init_expt_graphics failed \n");
			return -1;
		}
		return 0;
	}
	\endcode
	
	
	Custom initialization of SDL can be done in the following manner. 
	\code
	int customGraphicsSetup()
	{
		SDL_Surface *mainwindow = NULL;
		if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) // initialize SDL
		{
			printf("Couldn't initialize SDL: %s!",SDL_GetError());
			return -1;
		}
		
		mainwindow = SDL_SetVideoMode(800,600,32,SDL_SWSURFACE|SDL_FULLSCREEN); // set video mode
		if(!mainwindow)
		{
			printf("Failed to set video mode: %s! ",SDL_GetError());
			return -1;
		}
		if(init_expt_graphics(mainwindow, NULL)) // tell core graphics to use the set video mode.
			return -1;
		return 0;
	}
	@remark eyelink_core_graphics library does not support OPENGL.
	\endcode

 */
INT16 ELCALLTYPE  init_expt_graphics(SDL_Surface * hwnd, DISPLAYINFO *info);

/*! Call this function at the end of the experiment or before destroying the
	window registered with \c init_expt_graphics().  This call will disable
	calibration and drift correction until a new window is registered.
 */
void  ELCALLTYPE  close_expt_graphics(void);

/*! This function saves the entire bitmap as a .BMP, .JPG, .PNG, or .TIF file,
	and transfers the image to tracker as backdrop for gaze cursors (See
	\c bitmap_save() and \c bitmap_to_backdrop() for more information).

	@param hbm Handle to the bitmap image.
	@param xs Specifies the x-coordinate of the upper-left corner of the source
				bitmap.
	@param ys Specifies the y-coordinate of the upper-left corner of the source
				bitmap.
	@param width Specify the width of the source image to be copied  (set to \c 0
				to use all).
	@param height Specify the height of the source image to be copied  (set to
				\c 0 to use all).
	@param fname Name of the image file to be saved.  Currently, only .PNG, .BMP,
				.JPG, and .TIF files are saved.
	@param path Directory or drive path in quotes ("." for current directory).
	@param sv_options Use \c SV_NOREPLACE if not to replace an existing file;
				use \c SV_MAKEPATH to create a new path.
	@param xd Specifies the x-coordinate of the upper-left corner of the tracker
				screen.
	@param yd Specifies the y-coordinate of the upper-left corner of the tracker
				screen.
	@param bx_options Set with a bitwise OR of the following constants:
				\c BX_MAXCONTRAST: Maximizes contrast for clearest image;
				\c BX_AVERAGE: averages combined pixels;
				\c BX_DARKEN: chooses darkest (keep thin dark lines);
				\c BX_LIGHTEN: chooses darkest (keep thin white lines);
				\c BX_NODITHER: disables dithering to get clearest text;
				\c BX_GREYSCALE: converts to grayscale.
	@return \c 0 if successful, \c -1 if couldn't save, \c -2 if couldn't transfer
 */
int ELCALLTYPE sdl_bitmap_save_and_backdrop(SDL_Surface * hbm, INT16 xs, INT16 ys,
                                   INT16 width, INT16 height,char *fname,
                                   char *path, INT16 sv_options,INT16 xd,
                                   INT16 yd, UINT16 bx_options);

/*! This function transfers the bitmap to the tracker PC as backdrop for gaze cursors.
	The field "bx_options", set with bitwise OR of the following constants, determines
	how bitmap is processed: \c BX_AVERAGE (averaging combined pixels), \c BX_DARKEN
	(choosing darkest and keep thin dark lines), and \c BX_LIGHTEN (choosing darkest
	and keep thin white lines) control how bitmap size is reduced to fit tracker display;
	\c BX_MAXCONTRAST maximizes contrast for clearest image; \c BX_NODITHER disables
	the dithering of the image; \c BX_GREYSCALE converts the image to grayscale
	(grayscale works best for EyeLink I, text, etc.)

	@param hbm Handle to the bitmap image.
	@param xs Specifies the x-coordinate of the upper-left corner of the source bitmap.
	@param ys Specifies the y-coordinate of the upper-left corner of the source bitmap.
	@param width Specify the width of the source image to be copied  (set to \c 0 to
				use all).
	@param height Specify the height of the source image to be copied  (set to \c 0 to
				use all).
	@param xd Specifies the x-coordinate of the upper-left corner of the tracker screen.
	@param yd Specifies the y-coordinate of the upper-left corner of the tracker screen.
	@param bx_options Set with a bitwise OR of the following constants:
				\c BX_MAXCONTRAST: Maximizes contrast for clearest image;
				\c BX_AVERAGE: averages combined pixels;
				\c BX_DARKEN: chooses darkest (keep thin dark lines);
				\c BX_LIGHTEN: chooses darkest (keep thin white lines);
				\c BX_NODITHER: disables dithering to get clearest text;
				\c BX_GREYSCALE: converts to grayscale.
	@return \c 0 if successful, else \c -1 or \c -2;
 */
int ELCALLTYPE sdl_bitmap_to_backdrop(SDL_Surface * hbm, INT16 xs, INT16 ys,
                                   INT16 width, INT16 height,INT16 xd,
                                   INT16 yd, UINT16 bx_options);

/*! This function saves the entire bitmap or selected part of a bitmap in an image file
	(with an extension of .png, .bmp, .jpg, or .tif). It creates the specified file if
	this file does not exist.  If the file exists, it replaces the file unless
	\c SV_NOREPLACE is specified in the field of "sv_options". The directory to which
	the file will be written is specified in the path field.

	@param hbm Handle to the bitmap image.
	@param xs Specifies the x-coordinate of the upper-left corner of the source bitmap.
	@param ys Specifies the y-coordinate of the upper-left corner of the source bitmap.
	@param width Specify the width of the source image to be copied  (set to \c 0 to use all).
	@param height Specify the height of the source image to be copied  (set to \c 0 to use all).
	@param fname Name of the image file to be saved.  Currently, only .PNG, .BMP, .JPG,
				and .TIF files are saved.
	@param path Directory or drive path in quotes ("." for current directory).
	@param sv_options Use \c SV_NOREPLACE if not to replace an existing file; use \c SV_MAKEPATH to create a new path.
	@return \c 0 if successful, else \c -1.
 */
int ELCALLTYPE sdl_bitmap_save(SDL_Surface * hbm, INT16 xs, INT16 ys,
                                   INT16 width, INT16 height,char *fname,
                                   char *path, INT16 sv_options);



/*!
	Allow one to set arbituary target in place of circle target. Eg. a custom cursor.
	@param surface
 */
void ELCALLBACK set_cal_target_surface(SDL_Surface *surface);


/*!
	Allow one to set arbituary background in place of flat background
 	@param surface
 */
void ELCALLBACK set_cal_background_surface(SDL_Surface *surface);

/*!
	Removes the custom background. equivalent of calling set_cal_background_surface(NULL);
 */
void ELCALLBACK reset_background_surface();

/*! @internal
 */
void ELCALLBACK disable_custombackground_on_imagemode();// disables the use of custom background on image mode.


/*!
	Allow one to set target with animation.  The expected video can be loadable using VFW(type 1 avi) also,
	both audio and video streams must be present. The audio stream must be of pcm type.

	@param aviname  Name of the avi to use
	@param playCount How many time to loop through the video. Specify -1 to loop indefinitely.
	@options for future use.
 */
int ELCALLBACK set_cal_animation_target(const char *aviName,int playCount, int options);


/* convenient and compatibility macros*/
/*! See \c sdl_bitmap_save_and_backdrop()
 */
#define bitmap_save_and_backdrop sdl_bitmap_save_and_backdrop

/*! See \c sdl_bitmap_to_backdrop()
 */
#define bitmap_to_backdrop       sdl_bitmap_to_backdrop

/*! See \c sdl_bitmap_save()
 */
#define bitmap_save              sdl_bitmap_save

/*
    Some video cards blocks the second flip if there is already one flip is
    scheduled, eg.  most nvidia cards.  However, some video cards do not do this.
    That is, if they cannot schedule a flip, they return an error. eg. most ATI cards.
    This flip macro checks whether the flip is not scheduled, if it is not scheduled,
    try till a flip can be scheduled.
*/
#ifdef WIN32
#define Flip(x) while(SDL_FlipEx(x)<0)
#else
#define Flip(x) while(SDL_Flip(x)<0)
#endif



typedef struct _CCDBS { // calibration control device button state structure
	void *userdata;   // user data passed in with enable_external_calibration_device
	char buttons[256];  // set button[i] to 1 for pressed 0 to released
	void *internal;   // for internal use.
}CCDBS;

typedef int (ELCALLBACK *getExButtonStates)(CCDBS *);



#define EXTERNAL_DEV_NONE ((getExButtonStates)0)
#define EXTERNAL_DEV_CEDRUS ((getExButtonStates)1)
#define EXTERNAL_DEV_SYS_KEYBOARD ((getExButtonStates)2)

/*!
	Enables non keyboard devices to be used for calibration control.
 	@param[in] buttonStatesfcn   callback function reads the device and returns appropriate data.
	@remark: Use EXTERNAL_DEV_NONE to disable the device.
			 Use EXTERNAL_DEV_CEDRUS for built-in cedrus device support.

	@param[in] config - A character string of the config file content or config file name. Whether the  config is 
	the content or a file name is determined by looking for a new line character. If there is a new line character the
	content is assumed.  To use the default config, 
	set the parameter to NULL. The default config will be:
	@code
# User mode definition
# [MODE n] 
# Defines a 'user' mode, where n is the mode ID
# Valid Commands for user mode:
#	TEXT_LINE "This is a line of text"
# 	DISPLAY_IMAGE
# 	BUTTON id button_command [command args]
# 		Valid BUTTON button_commands:
# 			NO_ACTION
#
# 			AUTO_THRESH
# 			PUPIL_THRESH_UP
# 			PUPIL_THRESH_DOWN
# 			CR_THRESH_UP
# 			CR_THRESH_DOWN
#
# 			START_CALIBRATION
# 			START_VALIDATION
#			START_DRIFT_CORRECT
#
# 			GOTO_MODE
# 			EXIT
#
# 			NEXT_IMAGE (only makes sense when DISPLAY_IMAGE is set for mode)
# 			PREV_IMAGE (only makes sense when DISPLAY_IMAGE is set for mode)
#
# Predefined modes
# [MODE C] 
# Calibration mode
# Valid Commands
# 	REDO_LAST_TARGET
# 	ACCEPT_TARGET
#
# [MODE V]
# Validation mode
# Valid Commands
# 	REDO_LAST_TARGET
# 	ACCEPT_TARGET
#
# [MODE D]
# Drift Correction mode
# Valid Commands
# 	ACCEPT_TARGET

[MODE 1]
TEXT_LINE "EyeLink Setup:"
TEXT_LINE "1 -> View Camera Images"
TEXT_LINE "2 -> Start Calibration"
TEXT_LINE "3 -> Start Validation"
TEXT_LINE "4 -> Exit EyeLink Setup"

BUTTON 1 GOTO_MODE 2
BUTTON 2 GOTO_MODE C
BUTTON 3 GOTO_MODE V
BUTTON 4 EXIT




[MODE 2]
TEXT_LINE "Camera Views:"
TEXT_LINE "1 -> Next Camera View"
TEXT_LINE "2 -> Previous Camera View"
TEXT_LINE "3 -> Go To Pupil Threshold Adjustment Mode"
TEXT_LINE "4 -> Exit EyeLink Setup"


BUTTON 1 NEXT_IMAGE
BUTTON 2 PREV_IMAGE
BUTTON 3 GOTO_MODE 3
BUTTON 4 EXIT
DISPLAY_IMAGE TRUE


[MODE 3]
TEXT_LINE "Pupil Threshold Adjustment:"
TEXT_LINE "1 -> Increase Threshold"
TEXT_LINE "2 -> Decrease Threshold"
TEXT_LINE "3 -> Auto Threshold"
TEXT_LINE "4 -> Go To CR Threshold Adjustment Mode"

BUTTON 1 PUPIL_THRESH_UP
BUTTON 2 PUPIL_THRESH_DOWN
BUTTON 3 AUTO_THRESH
BUTTON 4 GOTO_MODE 4
DISPLAY_IMAGE TRUE


[MODE 4]
TEXT_LINE "CR Threshold Adjustment:"
TEXT_LINE "1 -> Increase Threshold" 
TEXT_LINE "2 -> Decrease Threshold" 
TEXT_LINE "3 -> Auto Threshold" 
TEXT_LINE "4 -> Go To EyeLink Setup Mode" 


BUTTON 1 CR_THRESH_UP
BUTTON 2 CR_THRESH_DOWN
BUTTON 3 AUTO_THRESH
BUTTON 4 GOTO_MODE 1
DISPLAY_IMAGE TRUE


[MODE C]
BUTTON 1 ACCEPT_TARGET
BUTTON 2 REDO_LAST_TARGE
BUTTON 4 EXIT

[MODE V]
BUTTON 1 ACCEPT_TARGET
BUTTON 2 REDO_LAST_TARGE
BUTTON 4 EXIT

[MODE D]
BUTTON 1 ACCEPT_TARGET
BUTTON 4 EXIT

	@endcode

	@param[in] userData  user data to pass back in the callback.
	@return 1 upon success, 0 otherwise


 */
int ELCALLTYPE enable_external_calibration_device(getExButtonStates buttonStatesfcn, const char * config, void *userData);

/*!
 *  @}
 */ // end of sdl_graphics


	
/*
 for internal use only.  Do not use this function.
 */
void ELCALLBACK set_cal_font(const char * fontPath, int size);
#ifdef __cplusplus     /* For C++ compilation */
};
#endif
#endif
