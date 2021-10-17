#ifndef _UAPI_LINUX_KERNEL_H
#define _UAPI_LINUX_KERNEL_H

#if defined(__KERNEL__ ) && defined(CONFIG_RLX) //modified by lynn_pu, 2014-10-30
#include <generated/uapi/linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
#include <uapi/linux/sysinfo.h>
#else
#include <linux/sysinfo.h>
#endif
#else
#include <linux/sysinfo.h>
#endif

/*
 * 'kernel.h' contains some often-used function prototypes etc
 */
#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))


#endif /* _UAPI_LINUX_KERNEL_H */
