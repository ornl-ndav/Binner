#include "vcbcolor.h"

/* Input:   h, s, v in vcbRange [0..1]
   Outputs: r, g, b in vcbRange [0..1] */
void vcbHsvToRgb (float h, float s, float v, float * r, float * g, float * b)
{
  int i;
  float aa, bb, cc, f;

  if (s == 0) /* Grayscale */
    *r = *g = *b = v;
  else {
	i = (int)floor (h);
	h -= i;
	h *= 6.0;
	i = (int)floor (h);
	f = h - i;
    aa = v * (1 - s);
    bb = v * (1 - (s * f));
    cc = v * (1 - (s * (1 - f)));
    switch (i) {
    case 0: *r = v;  *g = cc; *b = aa; break;
    case 1: *r = bb; *g = v;  *b = aa; break;
    case 2: *r = aa; *g = v;  *b = cc; break;
    case 3: *r = aa; *g = bb; *b = v;  break;
    case 4: *r = cc; *g = aa; *b = v;  break;
    case 5: *r = v;  *g = aa; *b = bb; break;
    }
  }
}

/* Input:   r, g, b in vcbRange [0..1]
   Outputs: h, s, v in vcbRange [0..1] */
void vcbRgbToHsv (float r, float g, float b, float * h, float * s, float * v)
{
  float max = VCB_MAX (r, VCB_MAX (g, b)), min = VCB_MIN (r, VCB_MIN (g, b));
  float delta = max - min;

  *v = max;
  if (max != 0.0)
    *s = delta / max;
  else
    *s = 0.0;
  if (*s == 0.0) *h = VCB_NO_HUE;
  else {
    if (r == max)
      *h = (g - b) / delta;
    else if (g == max)
      *h = 2 + (b - r) / delta;
    else if (b == max)
      *h = 4 + (r - g) / delta;
    *h *= 60.0;
    if (*h < 0) *h += 360.0;
    *h /= 360.0;
  }
}

/* Input:   r1, g1, b1 in vcbRange [0..1]
            r2, g2, b2 in vcbRange [0..1]
            w          in vcbRange [0..1]
   Outputs: rr, gg, bb in vcbRange [0..1] */
void vcbInterpolateColor(float r1, float g1, float b1,
                      float r2, float g2, float b2,
                      float w,
                      float *rr, float *gg, float *bb)
{
  float h0, s0, v0, h1, s1, v1, dist;
  vcbRgbToHsv(r1, g1, b1, &h0, &s0, &v0);
  vcbRgbToHsv(r2, g2, b2, &h1, &s1, &v1);
  dist = h1 - h0;
  if ((dist < +.5f) & (dist >= 0.f)) h0 += 1.f;
  if ((dist > -.5f) & (dist <  0.f)) h1 += 1.f;
  h0 += w * (h1-h0); s0 += w * (s1-s0); v0 += w * (v1-v0);
  vcbHsvToRgb(h0, s0, v0, rr, gg, bb);
}

void vcbCustomizeColorTable(float* colorTable,
                         float r1, float g1, float b1,
                         float r2, float g2, float b2,
                         int lower, int upper) {
  int i, steps = upper - lower + 1;

  if (!colorTable) return;
  if (lower > upper) return;
  if (lower < 0) return;
  if (upper > 255) return;

  for(i = 0; i < steps; i++)
      vcbInterpolateColor(r1, g1, b1, r2, g2, b2,
                       1.0f* (float)i/(float)(steps-1),
                       colorTable+(lower+i)*4+0,
                       colorTable+(lower+i)*4+1,
                       colorTable+(lower+i)*4+2);
}

void vcbCustomizeColorTableFullColor(float *colorTable, int lower, int upper) {
  vcbCustomizeColorTable(colorTable, 1.0f, 0.0f, 0.0f, 1.0f, 0.2f, 0.0f, lower, upper);
}

void vcbCustomizeColorTableColdHot(float *colorTable, int lower, int upper) {
  vcbCustomizeColorTable(colorTable, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, lower, upper);
}

void vcbCustomizeColorTableBlackWhite(float *colorTable, int lower, int upper) {
  vcbCustomizeColorTable(colorTable, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, lower, upper);
}

void vcbCustomizeColorTableSingeColor(float *colorTable, float r, float g, float b, int lower, int upper) {
  int i, steps = upper - lower + 1;

  if (!colorTable) return;
  if (lower > upper) return;
  if (lower < 0) return;
  if (upper > 255) return;

  for(i = 0; i < steps; i++) colorTable[(lower+i)*4+0] = r, colorTable[(lower+i)*4+1] = g, colorTable[(lower+i)*4+2] = b;
}


