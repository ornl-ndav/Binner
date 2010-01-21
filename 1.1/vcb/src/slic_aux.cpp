#include "slic.h"


double Ripple(SLIC *lic, double a, double b, int speed) {
    /*
     * Compute the integral bewteen points a and b, a <= b, of a phase shifted
     * Hanning low pass filter multiplied by a Hanning window function:
     *
     *    k(s) = (cos(d*s + phase) + 1)/2 * (cos(c*s) + 1)/2
     *
     * We need to scale the filter to the length of the filter support.  This
     * requires scaling each occurance of s by Pi/L.  Things fall out in the
     * integral which allow us to slip the scaling factors into c and d.
     */
   double p;            /* phase of the Hanning ripple function */
   double f;            /* frequency of the Hanning ripple function */
   double c;            /* Hanning window function dilation constant */
   double d;            /* Hanning ripple filter dilation constant */
   double w;            /* integral value */

   double Frequency = 2.0; 

   if (lic->Length == 0 || a == b){
     //printf(" NO!\n"); 
      return(0.0);
   }
   /*
    * Implement ``speed'' variations by scaling the frequency inversely
    * to speed.  This causes low magnitude vectors to be convolved using
    * a higher frequency filter which will yield less distinct output
    * features.  As the frequency increases the filter begins to approximate
    * the Hanning window filter.
    */
   
    p = lic->Phase;

    f = Frequency * (6.0 - 5.0 * (double)speed/(LIC_INTEGRAL_SPEEDS - 1));
    c = 0.6 / lic->Length  * VCB_M_PI;
    d = 1.0 / lic->Length  * VCB_M_PI * f;
    
    w =   b                                    - a
        + sin(b * c) / c                       - sin(a * c) / c
        + sin(b * d + p) / d                   - sin(a * d + p) / d
        + sin(b * (c + d) + p) / (2 * (c + d)) - sin(a * (c + d) + p)/(2 * (c + d));
    if (c != d)
        w +=  sin(b * (c - d) - p) / (2 * (c - d))
            - sin(a * (c - d) - p) / (2 * (c - d));
    w /= 4;
    return (w);
}
