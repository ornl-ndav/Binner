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

#ifndef _VCB_COLOR_H
#define _VCB_COLOR_H

#include <math.h>


#ifndef VCB_MIN
#define VCB_MIN(x,y)  ((x)<(y)?(x):(y))
#endif
#ifndef VCB_MAX
#define VCB_MAX(x,y)  ((x)>(y)?(x):(y))
#endif

#ifndef VCB_NO_HUE
#define VCB_NO_HUE 0
#endif

/* start function declarations */

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbHsvToRgb
 * Version: 0.9
 * Description:
 *   Converts an HSV color (h, s, v) to an RGB color (r, g, b)
 * Arguments:
 *  float h: The hue, or color type, in range [0, 1] from input HSV color.
 *  float s: The saturation in range [0, 1] from input HSV color.
 *  float v: The value, or brightness, in range [0, 1] from input HSV color.
 *  float* r: Pointer to red value in range [0, 1] for the output RGB color
 *  float* g: Pointer to green value in range [0, 1] for the output RGB color
 *  float* b: Pointer to blue value in range [0, 1] for the output RGB color
 * Return: None
 * Known Issues: None
 **/
void vcbHsvToRgb (float h, float s, float v, float * r, float * g, float * b);

/** Function
 * Name: vcbRgbToHsv
 * Version: 0.9
 * Description:
 *   Converts a RGB color (r, g, b) to a HSV color (h, s, v)
 * Arguments:
 *  float r: The red value in range [0, 1] from input RGB color
 *  float g: The green value in range [0, 1] from input RGB color
 *  float b: The blue value in range [0, 1] from input RGB color
 *  float* h: Pointer to hue, or color type, in range [0, 1] for the output HSV color.
 *  float* s: Pointer to saturation in range [0, 1] for the output HSV color.
 *  float* v: Pointer to value, or brightness, in range [0, 1] for the output HSV color.
 * Return: None
 * Known Issues: None
 **/
void vcbRgbToHsv (float r, float g, float b, float * h, float * s, float * v);

/** Function
 * Name: vcbInterpolateColor
 * Version: 0.9
 * Description:
 *   This function performs an RGB color interpolation using two input RGB colors and
 *   an interpolation weight.
 * Arguments:
 *  float r1: The red value in range [0, 1] for first input RGB color.
 *  float g1: The green value in range [0, 1] for first input RGB color.
 *  float b1: The blue value in range [0, 1] for first input RGB color.
 *
 *  float r2: The red value in range [0, 1] for second input RGB color.
 *  float g2: The green value in range [0, 1] for second input RGB color.
 *  float b2: The blue value in range [0, 1] for second input RGB color.
 *
 *  float w: The weight in range [0,1] to use in the interpolation of the two given RGB colors.
 *
 *  float* rr: Pointer to red value in range [0, 1] for interpolated RGB color.
 *  float* gg: Pointer to green value in range [0, 1] for interpolated RGB color.
 *  float* bb: Pointer to blue value in range [0, 1] for interpolated RGB color.
 * Return: None
 * Known Issues: None
 **/
void vcbInterpolateColor(float r1, float g1, float b1,
                      float r2, float g2, float b2,
                      float w,
                      float *rr, float *gg, float *bb);

/** Function
 * Name: vcbCustomizeColorTable
 * Version: 0.9
 * Description:
 *   This function customizes a color table of size float[256*4] with 256 colors
 *   in RGBA format. The table is one of the input arguments and will only be customized
 *   in the given range, while values outside of the range are left unchanged.
 *   The color table is customized by interpolating between the two given RGB colors
 *   in the range of elements from lower to upper. If the color table is NULL or
 *   the range is invalid, then no changes are made.
 * Arguments:
 *  float** colorTable: Table of 256 colors in RGBA format with size float[256*4].
 *   The customization directly alters colors in the table for a given range.
 *
 *  float r1: The red value in range [0, 1] for first input RGB color used to interpolate.
 *  float g1: The green value in range [0, 1] for first input RGB color used to interpolate.
 *  float b1: The blue value in range [0, 1] for first input RGB color used to interpolate.
 *
 *  float r2: The red value in range [0, 1] for second input RGB color used to interpolate.
 *  float g2: The green value in range [0, 1] for second input RGB color used to interpolate.
 *  float b2: The blue value in range [0, 1] for second input RGB color used to interpolate.
 *
 *  int lower: The first element of the table to customize. Valid range: [0, 255].
 *  int upper: The last element of the table to customize. Valid range: [0, 255].
 * Return: None
 * Known Issues: None
 **/
void vcbCustomizeColorTable(float* colorTable,
                         float r1, float g1, float b1,
                         float r2, float g2, float b2,
                         int lower, int upper);

/** Function
 * Name: vcbCustomizeColorTableFullColor
 * Version: 0.9
 * Description:
 *  Full Color Interpolation Scheme
 *  This funciton takes a color table of 256 RGBA colors and fills an area with
 *  full color range.
 * Arguments:
 *  float** colorTable: Table of 256 colors in RGBA format with size float[256*4].
 *   The customization directly alters colors in the table for a given range.
 *  int lower: The first element of the table to customize. Valid range: [0, 255].
 *  int upper: The last element of the table to customize. Valid range: [0, 255].
 * Return: None
 * Known Issues: None
 **/
void vcbCustomizeColorTableFullColor(float *colorTable, int lower, int upper);

/** Function
 * Name: vcbCustomizeColorTableColdHot
 * Version: 0.9
 * Description:
 *  Cold-Hot Color Interpolation Scheme (BLUE to RED)
 *  This funciton takes a color table of 256 RGBA colors and fills an area
 *  with an interpolation from blue to red.
 * Arguments:
 *  float** colorTable: Table of 256 colors in RGBA format with size float[256*4].
 *   The customization directly alters colors in the table for a given range.
 *  int lower: The first element of the table to customize. Valid range: [0, 255].
 *  int upper: The last element of the table to customize. Valid range: [0, 255].
 * Return: None
 * Known Issues: None
 **/
void vcbCustomizeColorTableColdHot(float *colorTable, int lower, int upper);

/** Function
 * Name: vcbCustomizeColorTableBlackWhite
 * Version: 0.9
 * Description:
 *  Black-White Color Interpolation Scheme
 *  This funciton takes a color table of 256 RGBA colors and fills an area
 *  with an interpolation from black to white.
 * Arguments:
 *  float** colorTable: Table of 256 colors in RGBA format with size float[256*4].
 *   The customization directly alters colors in the table for a given range.
 *  int lower: The first element of the table to customize. Valid range: [0, 255].
 *  int upper: The last element of the table to customize. Valid range: [0, 255].
 * Return: None
 * Known Issues: None
 **/
void vcbCustomizeColorTableBlackWhite(float *colorTable, int lower, int upper);

/** Function
 * Name: vcbCustomizeColorTableSingleColor
 * Version: 0.9
 * Description:
 *  This funciton takes a color table of 256 RGBA colors and fills an area
 *  with a certain RGB color.
 * Arguments:
 *  float** colorTable: Table of 256 colors in RGBA format with size float[256*4].
 *   The customization directly alters colors in the table for a given range.
 *  float r: The red value in range [0, 1] for the fill RGB color.
 *  float g: The green value in range [0, 1] for the fill RGB color.
 *  float b: The blue value in range [0, 1] for the fill RGB color.
 *  int lower: The first element of the table to customize. Valid range: [0, 255].
 *  int upper: The last element of the table to customize. Valid range: [0, 255].
 * Return: None
 * Known Issues: None
 **/
void vcbCustomizeColorTableSingeColor(float *colorTable, float r, float g, float b, int lower, int upper);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif

