/****************************************************************************
 *   Copyright (C) 2004  by Markus Glatter									*
 *	 seelab@cs.utk.edu														*
 *																			*
 *   This library is free software; you can redistribute it and/or			*
 *   modify it under the terms of the GNU Lesser General Public				*
 *   License as published by the Free Software Foundation; either			*
 *   version 2.1 of the License, or (at your option) any later version.		*
 *																			*
 *   This library is distributed in the hope that it will be useful,		*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of			*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*
 *   Lesser General Public License for more details.						*
 *																			*
 *   You should have received a copy of the GNU Lesser General Public		*
 *   License along with this library; if not, write to the					*
 *   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,			*
 *   Boston, MA  02110-1301  USA											*
 ****************************************************************************/

#include <stdarg.h>

#ifndef _VCB_DEBUG_H
#define _VCB_DEBUG_H

#ifndef VCB_DEBUG_OFF
#define VCB_DEBUG_OFF 0
#endif
#define VCB_DEBUG_ERROR 10
#ifndef VCB_DEBUG_WARNING
#define VCB_DEBUG_WARNING 20
#endif
#ifndef VCB_DEBUG_INFO
#define VCB_DEBUG_INFO 30
#endif
#ifndef VCB_DEBUG_VERBOSE
#define VCB_DEBUG_VERBOSE 40
#endif
#ifndef VCB_DEBUG_ALL
#define VCB_DEBUG_ALL 50
#endif

#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || !defined(_WIN32)

/** Define
 * Name: VCB_ERROR
 * Version: 0.9
 * Description:  This call will only execute if vcbDebugLevel >= VCB_DEBUG_ERROR.<br>
 *	Call this macro with an exit error code (ERRORCODE) and a formatted string as if it were a printf.
 *  It prints "ERROR: ..." to stderr and exits with the given ERRORCODE.
 *  Example&#58;<br><pre>
 *   VCB_ERROR(1, "Error expected on line %d\n", 128 );</pre>
 *   This will print "ERROR: Error expected at line 128" to stderr and exit with error code 1.<br><br>
 *   See vcbDebugLevelSet for definitions of vcbDebugLevel values.
 **/

#ifndef VCB_ERROR
#define VCB_ERROR(ERRORCODE, ...)   do {if(vcbDebugLevel>=VCB_DEBUG_ERROR) {fprintf(stderr, "     ERROR: "); fprintf(stderr, __VA_ARGS__);} if(ERRORCODE) exit(ERRORCODE); } while(0)
#endif

/** Define
 * Name: VCB_WARNING
 * Version: 0.9
 * Description:  This call will only execute if vcbDebugLevel >= VCB_DEBUG_WARNING.<br>
 *	Call this macro with a formatted string as if it were a printf.
 *  It prints "WARNING: ..." to stderr.
 *  Example&#58;<br><pre>
 *   VCB_WARNING("%s have been warned\n", "you" );</pre>
 *   This will print "WARNING: you have been warned" to stderr. <br><br>
 *   See vcbDebugLevelSet for definitions of vcbDebugLevel values.
 **/
#ifndef VCB_WARNING
#define VCB_WARNING(...) do {if(vcbDebugLevel>=VCB_DEBUG_WARNING) {fprintf(stderr, "   WARNING: "); fprintf(stderr, __VA_ARGS__);}} while(0)
#endif

/** Define
 * Name: VCB_INFO
 * Version: 0.9
 * Description:  This call will only execute if vcbDebugLevel >= VCB_DEBUG_INFO.<br>
 *	Call this macro with a formatted string as if it were a printf.
 *  It prints "DEBUG INFO: ..." to stderr.
 *  Example&#58;<br><pre>
 *   VCB_INFO("Avg FPS = %lf\n", 0.326 );</pre>
 *   This will print "DEBUG INFO: Avg FPS = 0.326" to stderr. <br><br>
 *   See vcbDebugLevelSet for definitions of vcbDebugLevel values.
 **/
#ifndef VCB_INFO
#define VCB_INFO(...) do {if(vcbDebugLevel>=VCB_DEBUG_INFO) {fprintf(stderr, "DEBUG INFO: "); fprintf(stderr, __VA_ARGS__);}} while(0)
#endif

/** Define
 * Name: VCB_VERBOSE
 * Version: 0.9
 * Description:  This call will only execute if vcbDebugLevel >= VCB_DEBUG_VERBOSE.<br>
 *	Call this macro with a formatted string as if it were a printf.
 *  It prints "DEBUG INFO: ..." to stderr.
 *  Example&#58;<br><pre>
 *   VCB_VERBOSE("malloc(%d) successfull.\n", 999999 );</pre>
 *   This will print "DEBUG INFO: malloc(999999) successfull." to stderr. <br><br>
 *   See vcbDebugLevelSet for definitions of vcbDebugLevel values.
 **/
#ifndef VCB_VERBOSE
#define VCB_VERBOSE(...) do {if(vcbDebugLevel>=VCB_DEBUG_VERBOSE) {fprintf(stderr, "DEBUG INFO: "); fprintf(stderr, __VA_ARGS__);}} while(0)
#endif

/** Define
 * Name: VCB_DEBUG
 * Version: 0.9
 * Description:  This call will only execute if vcbDebugLevel >= VCB_DEBUG_ALL.<br>
 *	Call this macro with a formatted string as if it were a printf.
 *  It prints "DEBUG INFO: ..." to stderr.
 *  Example&#58;<br><pre>
 *   VCB_DEBUG("Showing all debug info.\n" );</pre>
 *   This will print "DEBUG INFO: Showing all debug info." to stderr. <br><br>
 *   See vcbDebugLevelSet for definitions of vcbDebugLevel values.
 **/
#ifndef VCB_DEBUG
#define VCB_DEBUG(...) do {if(vcbDebugLevel>=VCB_DEBUG_ALL) {fprintf(stderr, "DEBUG INFO: "); fprintf(stderr, __VA_ARGS__);}} while(0)
#endif

/** Define
 * Name: VCB_DEBUG_LEVEL
 * Version: 0.9
 * Description:  This call will only execute if vcbDebugLevel >= x (the value you specify).<br>
 *	Call this macro with defined VCB_DEBUG_* and a formatted string as if it were a printf.
 *  It prints "DEBUG INFO: ..." to stderr.
 *  Example&#58;<br><pre>
 *   VCB_DEBUG_LEVEL( VCB_DEBUG_ERROR, "malloc(%d) successfull.\n", 999999 );</pre>
 *   This will print "DEBUG INFO: malloc(999999) successfull." to stderr only if vcbDebugLevel >= VCB_DEBUG_ERROR. <br><br>
 *  The debug levels are defined as follows&#58;<br><br><table>
 * <TR><TD>VCB_DEBUG_OFF		<TD>0
 * <TR><TD>VCB_DEBUG_ERROR		<TD>10
 * <TR><TD>VCB_DEBUG_WARNING	<TD>20
 * <TR><TD>VCB_DEBUG_INFO		<TD>30
 * <TR><TD>VCB_DEBUG_VERBOSE	<TD>40
 * <TR><TD>VCB_DEBUG_ALL		<TD>50  </table>
 **/
#ifndef VCB_DEBUG_LEVEL
#define VCB_DEBUG_LEVEL(x, ...) do {if(vcbDebugLevel>=x) {fprintf(stderr, "DEBUG INFO: "); fprintf(stderr, __VA_ARGS__);}} while(0)
#endif

#else

#ifndef VCB_ERROR
#define VCB_ERROR _db_e_win
#endif
#ifndef VCB_WARNING
#define VCB_WARNING _db_w_win
#endif
#ifndef VCB_INFO
#define VCB_INFO _db_i_win
#endif
#ifndef VCB_VERBOSE
#define VCB_VERBOSE _db_v_win
#endif
#ifndef VCB_DEBUG
#define VCB_DEBUG _db_d_win
#endif
#ifndef VCB_DEBUG_LEVEL
#define VCB_DEBUG_LEVEL _db_dl_win
#endif

#endif
/** Define
 * Name: VCB_DEBUG_CODE
 * Version: 0.9
 * Description:  Use this macro if you only want the following code to be run if
 *  VCB_DEBUG_ALL is set.<br>
 *  Example&#58;<br><pre>
 *  VCB_DEBUG_CODE{
 *     block of code here...
 *     printf( "You will only see this if VCB_DEBUG_ALL is set.\n");
 *  }</pre>
 **/
#ifndef VCB_DEBUG_CODE
#define VCB_DEBUG_CODE if(vcbDebugLevel>=VCB_DEBUG_ALL)
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int vcbDebugLevel;

/** Function
 * Name: vcbDebugLevelSet
 * Version: 0.9
 * Description: This function sets the vcbDebugLevel (global variable).<br>
 *  The debug levels are defined as follows&#58;<br><br><table>
 * <TR><TD>VCB_DEBUG_OFF		<TD>0
 * <TR><TD>VCB_DEBUG_ERROR		<TD>10
 * <TR><TD>VCB_DEBUG_WARNING	<TD>20
 * <TR><TD>VCB_DEBUG_INFO		<TD>30
 * <TR><TD>VCB_DEBUG_VERBOSE	<TD>40
 * <TR><TD>VCB_DEBUG_ALL		<TD>50  </table>
 * Arguments:
 *  int level: The desired debug level.
 * Return: None
 * Known Issues: None
 **/
void vcbDebugLevelSet(int);

void _db_e_win(int errorcode, char *fmt, ...);
void _db_w_win(char *fmt, ...);
void _db_i_win(char *fmt, ...);
void _db_v_win(char *fmt, ...);
void _db_d_win(char *fmt, ...);
void _db_dl_win(int level, char *fmt, ...);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif


