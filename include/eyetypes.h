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
    \file eyetypes.h
    \brief Declarations of basic data types.
 */

#ifndef __SRRESEARCH__EYETYPES_H__
#define __SRRESEARCH__EYETYPES_H__

/*#define FARTYPE _far */  /* unusual--for some mixed-model builds */
#define FARTYPE            /* make blank for most DOS, 32-bit, ANSI C */

#ifdef WIN32
  #include <windows.h>   /* needed for prior declarations of types */
#endif

#ifdef __cplusplus     /* For C++ definitions */
extern "C" {
#endif

#ifndef BYTEDEF
    #define BYTEDEF 1
        typedef unsigned char  byte;
        typedef signed short   INT16;
        typedef unsigned short UINT16;
        /* VC++ 6.0 defines these types already. mingw32 also defines this*/
    #if !(defined(_BASETSD_H_) || defined (_BASETSD_H))
        typedef signed int    INT32;
        typedef unsigned int  UINT32;
    #endif
#endif

#ifndef MICRODEF            /* Special high-resolution time structure */
    #define MICRODEF 1
/*! @ingroup access_time_local
	Structure to hold msec and usec.
	\sa current_micro
 */
    typedef struct {
        INT32  msec;    /* SIGNED for offset computations */
        INT16  usec;
    } MICRO ;
#endif

#ifdef __cplusplus    /* For C++ definitions */
}
#endif
#endif
