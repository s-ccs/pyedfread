/*******************************************************************************
 * EYELINK EXPT SUPPORT: SDL PLATFORM (Coppied from WIN32 PLATFORM)			   *
 * (c) 1996-2002 by SR Research Ltd.                                           *
 * by Suganthan Subramaniam                                                    *
 * EYELINK II: 3 February 2003                                                 *
 *     For non-commercial use only                                             *
 * Provides text support for sdl							                   *
 * This module is for user applications. Use is granted for non-commercial     *
 * applications by Eyelink licencees only                                      *
 *                                                                             *
 *                                                                             *
 **************************************** WARNING ******************************
 *                                                                             *
 * UNDER NO CIRCUMSTANCES SHOULD PARTS OF THESE FILES BE COPIED OR COMBINED.   *
 * This will make your code impossible to upgrade to new releases in the future*
 * and SR Research will not give tech support for reorganized code.            *
 *                                                                             *
 * This file should not be modified. If you must modify it, copy the entire    *
 * file with a new name, and change the the new file.                          *
 *******************************************************************************/

 

#ifndef __SR_RESEARCH_SDL_TEXT_SUPPORT_H__
#define __SR_RESEARCH_SDL_TEXT_SUPPORT_H__
#include <SDL/SDL.h>

#ifdef __cplusplus     /* For C++ compilation */
extern "C" {
#endif

typedef enum
{
	NONE          = 0x000, /* no justification. just place the text a the given point */
	JUSTIFY_RIGHT = 0x001, /* right justify the text */
	CENTER        = 0x010, /* horizontally center the text */
	WRAP          = 0x100  /* wrap the text if it cannot fit in one line. */
} PRINTFORMAT;


/*****************************************************************************
 * Function: set_margin
 * Parameters:
 *		left:	Left margin
 *		right:	Right margin
 *      top:	Top margin
 *      bottom: Bottom margin
 * Purpose:
 *		set margin for the text to print.
 *****************************************************************************/
void set_margin(int left, int right, int top, int bottom);

/*****************************************************************************
 * Function: set_line_spacing
 * Parameters:
 *		linespace:	line spacing - percentage of line height
 * Purpose:
 *		Adjust the line spacing
 *****************************************************************************/
void set_line_spacing(double linespace);

/*****************************************************************************
 * Function: get_new_font
 * Parameters:
 *		fontname:	name of the font. Not the name of the font file name
 *		font_size:	font size
 *      bold:	bold !=0 means bold font otherwise regular font.
 * Purpose:
 *		close the current font and open up a new font.
 *****************************************************************************/
int get_new_font(char *fontname, int font_size, int bold);

/*****************************************************************************
 * Function: graphic_printf
 * Parameters:
 *		surface: sdl surface to print on
 *		fg: foreground color
 *		format: format type. use any of the following and combined. NONE,
 *				JUSTIFY_RIGHT, CENTER, WRAP
 *		x,y: x,y position
 *		fmt: printf like format
 *		...: any parameter applicable to the fmt.
 * Purpose:
 *		Printf like print utility function
 *****************************************************************************/
void graphic_printf(SDL_Surface *surface, SDL_Color fg, int format, int x, 
					int y, const char *fmt, ...);

/*****************************************************************************
 * Function: get_font_height
 * Parameters:
 *		None
 * Purpose:
 *		Returns the font size.
 *****************************************************************************/
int get_font_height();
/*
 * Returns the resources path for the application 
 */
char * get_resources_path();

#ifdef __cplusplus     /* For C++ compilation */
}
#endif

#endif
	
