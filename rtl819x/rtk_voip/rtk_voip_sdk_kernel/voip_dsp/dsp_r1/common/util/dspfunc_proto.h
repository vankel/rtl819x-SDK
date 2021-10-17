#ifndef _DSP_FUNC_PROTO_H_
#define _DSP_FUNC_PROTO_H_

#include "typedef.h"


/*-------------------------------*
 * Mathematic functions.         *
 *-------------------------------*/
 
/* AMS function prototype */
Word32 Inv_sqrts(   /* (o) Q30 : output value   (range: 0<=val<1)           */
  Word32 L_x       /* (i) Q0  : input value    (range: 0<=val<=7fffffff)   */
);

void Log2s(
  Word32 L_x,       /* (i) Q0 : input value                                 */
  Word16 *exponent, /* (o) Q0 : Integer part of Log2.   (range: 0<=val<=30) */
  Word16 *fraction  /* (o) Q15: Fractionnal part of Log2. (range: 0<=val<1) */
);

Word32 Pow2s(        /* (o) Q0  : result       (range: 0<=val<=0x7fffffff) */
  Word16 exponent,  /* (i) Q0  : Integer part.      (range: 0<=val<=30)   */
  Word16 fraction   /* (i) Q15 : Fractionnal part.  (range: 0.0<=val<1.0) */
);


/* C function prototype */

Word32 Isqrt_c(		/* (o) Q31 : output value (range: 0<=val<1)         */
     Word32 L_x		/* (i) Q0  : input value  (range: 0<=val<=7fffffff) */
);

void Isqrt_n_c(
     Word32 * frac,	/* (i/o) Q31: normalized value (1.0 < frac <= 0.5) */
     Word16 * exp	/* (i/o)    : exponent (value = frac x 2^exponent) */
);

Word32 Inv_sqrt_c(   /* (o) Q30 : output value   (range: 0<=val<1)           */
  Word32 L_x       /* (i) Q0  : input value    (range: 0<=val<=7fffffff)   */
);

void Log2_c(
  Word32 L_x,       /* (i) Q0 : input value                                 */
  Word16 *exponent, /* (o) Q0 : Integer part of Log2.   (range: 0<=val<=30) */
  Word16 *fraction  /* (o) Q15: Fractionnal part of Log2. (range: 0<=val<1) */
);

Word32 Pow2_c(        /* (o) Q0  : result       (range: 0<=val<=0x7fffffff) */
  Word16 exponent,  /* (i) Q0  : Integer part.      (range: 0<=val<=30)   */
  Word16 fraction   /* (i) Q15 : Fractionnal part.  (range: 0.0<=val<1.0) */
);

long FFracSqrt_c(long x, int nfbits);

Word32 Dot_product12_c(                      /* (o) Q31: normalized result (1 < val <= -1) */
     Word16 x[],                           /* (i) 12bits: x vector                       */
     Word16 y[],                           /* (i) 12bits: y vector                       */
     Word16 lg,                            /* (i)    : vector length                     */
     Word16 * exp                          /* (o)    : exponent of result (0..+30)       */
);

#endif /* _DSP_FUNC_PROTO_H_ */

