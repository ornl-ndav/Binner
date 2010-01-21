/****************************************************************************
 *   Copyright (C) 2004  by Jian Huang										*
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

#ifndef _VCB_IMAGE_H 
#define _VCB_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbImgWritePPM
 * Version: 0.9
 * Description:
 *  The vcbImgWritePPM function creates a PPM image and writes
 *  the uncompressed image to disk.  There is no return value.
 *  vcbImgWritePPM will always write the first 3 bytes on each pixel
 *  to file. Thus treating them as the red, green, and blue channels.
 * Arguments:
 *  char* filename:  The full name of the image file to create, including the
 *    path.
 *  unsigned char* fbuffer:   A pointer to an array of pixel data that will be
 *    written as image.
 *  int nchannels: The number of values on each pixel.  All types are assumed
 *    to be unsigned char.
 *  int width:     The integer width of the image.
 *  int height:    The integer height of the image.
 * Return: None
 * Known Issues: None
 **/
void vcbImgWritePPM(char *filename, unsigned char * fbuffer, int nchannels, int width, int height);

/** Function
 * Name: vcbImgWritePGM
 * Version: 0.9
 * Description:
 *  The vcbImgWritePGM function creates a PGM image and writes
 *  the uncompressed image to disk.  There is no return value.
 *  vcbImgWritePGM will always write the first bytes on each
 *  pixel to the PGM file, since it is always grey scale.
 * Arguments:
 *  char* filename:  The full name of the image file to create, including the
 *    path.
 *  unsigned char* fbuffer:   A pointer to an array of pixel data that will be
 *    written as image.
 *  int nchannels: The number of values on each pixel.  All types are assumed
 *    to be unsigned char.
 *  int width:     The integer width of the image.
 *  int height:    The integer height of the image.
 * Return: None
 * Known Issues: None
 **/
void vcbImgWritePGM(char *filename, unsigned char * fbuffer, int nchannels, int width, int height);

/** Function
 * Name: vcbImgWriteTGA
 * Version: 0.9
 * Description:
 *  The vcbImgWriteTGA function creates a TGA image and writes
 *  the uncompressed image to disk.  There is no return value.
 *  vcbImgWriteTGA will always write the first 3 bytes on each pixel
 *  to file. Thus treating them as the red, green, and blue channels.
 * Arguments:
 *  char* filename:  The full name of the image file to create, including the
 *    path.
 *  unsigned char* fbuffer:   A pointer to an array of pixel data that will be
 *    written as image.
 *  int nchannels: The number of values on each pixel.  All types are assumed
 *    to be unsigned char.
 *  int width:     The integer width of the image.
 *  int height:    The integer height of the image.
 * Return: None
 * Known Issues: None
 **/
void vcbImgWriteTGA(char *filename, unsigned char * fbuffer, int nchannels, int width, int height);

/** Function
 * Name: vcbImgWriteBMP
 * Version: 0.9
 * Description:
 *  The vcbImgWriteBMP function creates a BMP image and writes
 *  the uncompressed image to disk.  There is no return value.
 *  vcbImgWritePPM will always write the first 3 bytes on each pixel
 *  to file. Thus treating them as the red, green, and blue channels.
 * Arguments:
 *  char* filename:  The full name of the image file to create, including the
 *    path.
 *  unsigned char* fbuffer:   A pointer to an array of pixel data that will be
 *    written as image.
 *  int nchannels: The number of values on each pixel.  All types are assumed
 *    to be unsigned char.
 *  int width:     The integer width of the image.
 *  int height:    The integer height of the image.
 * Return: None
 * Known Issues: None
 **/
void vcbImgWriteBMP(char *filename, unsigned char * fbuffer, int nchannels, int width, int height);

/** Function
 * Name: vcbImgWriteRGB
 * Version: 0.9
 * Description:
 *  The vcbImgWriteRGB function creates a RGB image and writes
 *  the uncompressed image to disk.  There is no return value.
 *  vcbImgWriteRGB will always write the first 3 bytes on each pixel
 *  to file. Thus treating them as the red, green, and blue channels.
 * Arguments:
 *  char* filename:  The full name of the image file to create, including the
 *    path.
 *  unsigned char* fbuffer:   A pointer to an array of pixel data that will be
 *    written as image.
 *  int nchannels: The number of values on each pixel.  All types are assumed
 *    to be unsigned char.
 *  int width:     The integer width of the image.
 *  int height:    The integer height of the image.
 * Return: None
 * Known Issues: None
 **/
void vcbImgWriteRGB(char *filename, unsigned char * fbuffer, int nchannels, int width, int height);

/** Function
 * Name: vcbImgReadPPM
 * Version: 0.9
 * Description:
 *  The vcbImgReadPPM function reads in PPM image files to pixel data.
 *  In the returned array, each pixel has 3 bytes.
 * Arguments:
 *  char* filename:  The full name of the image file to create, including path.
 *  int* width:      A pointer to an integer to store the width of image.
 *  int* height:     A pointer to an integer to store the height of image.
 * Return: unsigned char*; returns NULL for failure and a pointer to an array of pixel data for on success.
 * Known Issues: None
 **/
unsigned char * vcbImgReadPPM(char *filename, int * width, int * height);

/** Function
 * Name: vcbImgReadPGM
 * Version: 0.9
 * Description:
 *  The vcbImgReadPGM function reads in PGM image files to pixel data.
 *  In the returned array, each pixel has 1 byte.
 * Arguments:
 *  char* filename:  The full name of the image file to create, including path.
 *  int* width:      A pointer to an integer to store the width of image.
 *  int* height:     A pointer to an integer to store the height of image.
 * Return: unsigned char*; returns NULL for failure and a pointer to an array of pixel data for on success.
 * Known Issues: None
 **/
unsigned char * vcbImgReadPGM(char *filename, int * width, int * height);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
