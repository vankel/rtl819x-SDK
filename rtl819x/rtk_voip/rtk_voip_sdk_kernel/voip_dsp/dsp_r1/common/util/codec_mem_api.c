//#include <linux/config.h>
#include "codec_mem.h"
#include "voip_feature.h"

extern void g726_dmen_memcpy( unsigned char rate);
extern int imem_size;
extern int dmem_size;
extern unsigned long* __imem_type;
extern unsigned long __iram;
extern unsigned long __nat_speedup_start;

void set_g726_dmem(unsigned char rate)
{
#ifdef CONFIG_RTK_VOIP_G726
	#ifdef CONFIG_RTK_VOIP_MODULE
	#else
	g726_dmen_memcpy(rate);
	#endif

#else

#endif
}

void set_imem_size(void)
{
	//imem_size = IMEM_SIZE;
	imem_size = RTK_VOIP_IMEM_SIZE(gVoipFeature);
	//printk("imem_size = 0x%x\n", RTK_VOIP_IMEM_SIZE(gVoipFeature));
}

void set_dmem_size(void)
{
	//dmem_size = DMEM_SIZE;
	dmem_size = RTK_VOIP_DMEM_SIZE(gVoipFeature);
	//printk("dmem_size = 0x%x\n", RTK_VOIP_DMEM_SIZE(gVoipFeature));	

	if( dmem_size == 0x2000 )	// 8k
		codec_dmem_start_sel = ( unsigned long )&__codec_dmem_start;
	else
		codec_dmem_start_sel = ( unsigned long )&__codec_dmem_4k_start;
}

void set_system_imem_type(void)
{
#if defined(CONFIG_RTK_VOIP_DRIVERS_PCM8651) || defined(CONFIG_RTK_VOIP_DRIVERS_PCM8671) || defined(CONFIG_RTK_VOIP_DRIVERS_PCM865xC) ||defined(CONFIG_RTK_VOIP_DRIVERS_PCM8672) || defined(CONFIG_RTK_VOIP_DRIVERS_PCM8972B_FAMILY) || defined(CONFIG_RTK_VOIP_DRIVERS_PCM89xxC) ||defined(CONFIG_RTK_VOIP_DRIVERS_PCM8676)	\
	||defined(CONFIG_RTK_VOIP_PLATFORM_8686) || defined(CONFIG_RTK_VOIP_DRIVERS_PCM89xxE)
	__imem_type = &__iram;
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM8186
	__imem_type = &__nat_speedup_start;
#endif
}
