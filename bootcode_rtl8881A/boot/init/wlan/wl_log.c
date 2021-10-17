#include <linux/types.h>
#include <linux/synclink.h>
#include "wl_log.h"

u4Byte 
rtl_printf(
    IN  u2Byte FileNum,
    IN  u4Byte Line,
    IN  u1Byte *fmt, ...
)
{
   dprintf("[%d][%d]: ",FileNum, Line);
   (void)vsprintf(0, fmt, ((const int *)&fmt)+1);	
}

