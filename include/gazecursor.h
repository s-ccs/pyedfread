/*******************************************************************************
 * EYELINK EXPT SUPPORT: SDL PLATFORM										   *
 * (c) 1996-2002 by SR Research Ltd.                                           *
 * by Suganthan Subramaniam                                                    *
 * EYELINK II: 3 February 2003                                                 *
 *     For non-commercial use only                                             *
 * Provides hardware acclareted cursor drawing with SDL_Flip                   *
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


#ifndef __SR_RESEARCH_GAZECURSOR_H__
#define __SR_RESEARCH_GAZECURSOR_H__
#ifdef __cplusplus
extern "C"{
#endif

/*******************************************************************************
 * Initialize the cursor sub system
 *******************************************************************************/
void initialize_cursor(SDL_Surface * surface, int size);
void initialize_dynamic_cursor(SDL_Surface * surface, int size, SDL_Surface *copySrc );


/*******************************************************************************
 * Erases a existing cursor on the screen
 *******************************************************************************/
void erase_gaze_cursor();
/*******************************************************************************
 * Draws a new cursor at (x,y) and erases the last drawn cursor.
 *******************************************************************************/
void draw_gaze_cursor(int x, int y);

void free_cursor();

#ifdef __cplusplus
};
#endif
#endif

