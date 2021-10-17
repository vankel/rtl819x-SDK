/*****************************************************************************/

/*
 *	flatfs.h -- support for flat FLASH file systems.
 *
 *	(C) Copyright 1999, Greg Ungerer (gerg@snapgear.com).
 *	(C) Copyright 2000, Lineo Inc. (www.lineo.com)
 *	(C) Copyright 2001-2002, SnapGear (www.snapgear.com)
 */

/*****************************************************************************/
#ifndef flatfs_h
#define flatfs_h
/*****************************************************************************/

/*
 * Hardwire the source and destination directories :-(
 */
//#define	FILEFS		"/dev/flash/config"
//#include "../../config/autoconf.h"
//#ifdef CONFIG_BOA_WEB_E8B_CH
#define	FILEFS		"/dev/mtd4"
//#else
//#define	FILEFS		"/dev/config"
//#endif

#define	DEFAULTDIR	"/etc/cwmp_default"
#define	SRCDIR		"/var/cwmp_config"
#define	DSTDIR		SRCDIR

#define FLATFSD_CONFIG	".flatfsd"


/*
 * Globals for file and byte count.
 */
extern int numfiles;
extern int numbytes;
extern int numdropped;
extern int numversion;

extern int flat_restorefs(void);
extern int flat_savefs(int version);
extern int flat_new(const char *dir);
extern int flat_clean(int realclean);
extern int flat_filecount(void);
extern int flat_needinit(void);
extern int flat_check(void);

#define ERROR_CODE()	(-(__LINE__)) /* unique failure codes :-) */

/*****************************************************************************/
#endif
