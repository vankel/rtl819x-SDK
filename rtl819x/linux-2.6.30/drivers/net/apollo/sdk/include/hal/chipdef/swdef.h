#ifndef _SW_DEF_H_
#define _SW_DEF_H_

#include <common/type.h>

typedef struct hal_register_s
{
    uint32 reg_addr;
    uint32 write_bit;
    uint32 reset_val;
}hal_register_t;

#if defined(CONFIG_SDK_APOLLO)
extern hal_register_t reg_map_def[];
#define REG_MAP_ENTRY_NUM  4965
#endif

#if defined(CONFIG_SDK_APOLLOMP)
extern hal_register_t apollomp_reg_map_def[];
#define APOLLOMP_REG_MAP_ENTRY_NUM  5385
#endif


#endif /*#ifndef _SW_DEF_H_*/
