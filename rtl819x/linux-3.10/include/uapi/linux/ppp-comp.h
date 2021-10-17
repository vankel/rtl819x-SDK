/*
 * ppp-comp.h - Definitions for doing PPP packet compression.
 *
 * Copyright 1994-1998 Paul Mackerras.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  version 2 as published by the Free Software Foundation.
 */
#ifndef _UAPI_NET_PPP_COMP_H
#define _UAPI_NET_PPP_COMP_H


/*
 * CCP codes.
 */

#define CCP_CONFREQ	1
#define CCP_CONFACK	2
#define CCP_TERMREQ	5
#define CCP_TERMACK	6
#define CCP_RESETREQ	14
#define CCP_RESETACK	15

/*
 * Max # bytes for a CCP option
 */

#define CCP_MAX_OPTION_LENGTH	32

/*
 * Parts of a CCP packet.
 */

#define CCP_CODE(dp)		((dp)[0])
#define CCP_ID(dp)		((dp)[1])
#define CCP_LENGTH(dp)		(((dp)[2] << 8) + (dp)[3])
#define CCP_HDRLEN		4

#define CCP_OPT_CODE(dp)	((dp)[0])
#define CCP_OPT_LENGTH(dp)	((dp)[1])
#define CCP_OPT_MINLEN		2

/*
 * Definitions for BSD-Compress.
 */

#define CI_BSD_COMPRESS		21	/* config. option for BSD-Compress */
#define CILEN_BSD_COMPRESS	3	/* length of config. option */

/* Macros for handling the 3rd byte of the BSD-Compress config option. */
#define BSD_NBITS(x)		((x) & 0x1F)	/* number of bits requested */
#define BSD_VERSION(x)		((x) >> 5)	/* version of option format */
#define BSD_CURRENT_VERSION	1		/* current version number */
#define BSD_MAKE_OPT(v, n)	(((v) << 5) | (n))

#define BSD_MIN_BITS		9	/* smallest code size supported */
#define BSD_MAX_BITS		15	/* largest code size supported */

/*
 * Definitions for Deflate.
 */

#define CI_DEFLATE		26	/* config option for Deflate */
#define CI_DEFLATE_DRAFT	24	/* value used in original draft RFC */
#define CILEN_DEFLATE		4	/* length of its config option */

#define DEFLATE_MIN_SIZE	9
#define DEFLATE_MAX_SIZE	15
#define DEFLATE_METHOD_VAL	8
#define DEFLATE_SIZE(x)		(((x) >> 4) + 8)
#define DEFLATE_METHOD(x)	((x) & 0x0F)
#define DEFLATE_MAKE_OPT(w)	((((w) - 8) << 4) + DEFLATE_METHOD_VAL)
#define DEFLATE_CHK_SEQUENCE	0

/*
 * Definitions for MPPE.
 */

#define CI_MPPE                18      /* config option for MPPE */
#define CILEN_MPPE              6      /* length of config option */

#if defined(CONFIG_PPP_MPPE_MPPC)
#define MPPE_OVHD              4       /* MPPE overhead */
#define MPPE_MAX_KEY_LEN       16      /* largest key length (128-bit) */

#define MPPE_STATELESS          0x01   /* configuration bit H */
#define MPPE_40BIT              0x20   /* configuration bit L */
#define MPPE_56BIT              0x80   /* configuration bit M */
#define MPPE_128BIT             0x40   /* configuration bit S */
#define MPPE_MPPC               0x01   /* configuration bit C */

/*
 * Definitions for Stac LZS.
 */

#define CI_LZS                 17      /* config option for Stac LZS */
#define CILEN_LZS              5       /* length of config option */

#define LZS_OVHD               4       /* max. LZS overhead */
#define LZS_HIST_LEN           2048    /* LZS history size */
#define LZS_MAX_CCOUNT         0x0FFF  /* max. coherency counter value */

#define LZS_MODE_NONE          0
#define LZS_MODE_LCB           1
#define LZS_MODE_CRC           2
#define LZS_MODE_SEQ           3
#define LZS_MODE_EXT           4

#define LZS_EXT_BIT_FLUSHED    0x80    /* bit A */
#define LZS_EXT_BIT_COMP       0x20    /* bit C */
#endif


/*
 * Definitions for other, as yet unsupported, compression methods.
 */

#define CI_PREDICTOR_1		1	/* config option for Predictor-1 */
#define CILEN_PREDICTOR_1	2	/* length of its config option */
#define CI_PREDICTOR_2		2	/* config option for Predictor-2 */
#define CILEN_PREDICTOR_2	2	/* length of its config option */


#endif /* _UAPI_NET_PPP_COMP_H */
