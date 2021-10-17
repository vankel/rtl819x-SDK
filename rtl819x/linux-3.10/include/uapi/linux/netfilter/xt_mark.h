#ifndef _XT_MARK_H
#define _XT_MARK_H

#include <linux/types.h>

struct xt_mark_tginfo2 {
	__u32 mark, mask;
};

struct xt_mark_mtinfo1 {
	__u32 mark, mask;
	__u8 invert;
};

/* Version 0 */
struct xt_mark_target_info {
    unsigned long mark;
};

/* Version 1 */
enum {
    XT_MARK_SET=0,
    XT_MARK_AND,
    XT_MARK_OR,
};

struct xt_mark_target_info_v1 {
    unsigned long mark;
    __u8 mode;
};

#endif /*_XT_MARK_H*/
