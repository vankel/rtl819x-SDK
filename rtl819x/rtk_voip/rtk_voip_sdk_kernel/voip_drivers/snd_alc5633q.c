#include "rtk_voip.h"
#include "voip_init.h"
#include "con_register.h"

static int enable_alc5633q( voip_snd_t *this, int enable )
{
	extern void Stereo_dac_volume_control_MUTE(unsigned short int DACMUTE);
	
	Stereo_dac_volume_control_MUTE( ( enable ? 0 : 1 ) );
	
	return 0;
}


// --------------------------------------------------------
// channel mapping architecture 
// --------------------------------------------------------


static const snd_ops_ac_t snd_alc5633q_ops = {
	// common operation 
	.enable = enable_alc5633q,
	
	
};

#define ALC5633Q_NUM		1

static voip_snd_t snd_alc5633q[ 1 ];//ALC5633Q_NUM ];

static int __init voip_snd_alc5633q_init( void )
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS
	const int law = 0; 
#else
	extern int law;
#endif
	extern void ALC5633Q_init(int law);
	int i;
	
	// bring up ALC 5633Q 
	extern void init_i2c_gpio(void);
	extern unsigned short int read_ALC5633Q(unsigned char Reg);
	init_i2c_gpio();
	i=read_ALC5633Q(0);
	printk("reg0=%x\n", i);
	i=read_ALC5633Q(2);
	printk("reg2=%x\n", i);
	i=read_ALC5633Q(4);
	printk("reg4=%x\n", i);
	i=read_ALC5633Q(6);
	printk("reg6=%x\n", i);
	i=read_ALC5633Q(0x7c);
	printk("reg7c=%x\n", i);



	ALC5633Q_init( law );
	
	// register snd 
	for( i = 0; i < ALC5633Q_NUM; i ++ ) {
		snd_alc5633q[ i ].sch = i;
		snd_alc5633q[ i ].name = "alc5633q";
		snd_alc5633q[ i ].snd_type = SND_TYPE_AC;
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS	// iis
		snd_alc5633q[ i ].bus_type_sup = BUS_TYPE_IIS;
		snd_alc5633q[ i ].TS1 = 0;
		snd_alc5633q[ i ].TS2 = 0;
  #ifdef CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5633Q_WIDEBAND
		snd_alc5633q[ i ].band_mode_sup = BAND_MODE_16K | BAND_MODE_8K;
  #else
		snd_alc5633q[ i ].band_mode_sup = BAND_MODE_8K;
  #endif
#else	// pcm 
		snd_alc5633q[ i ].bus_type_sup = BUS_TYPE_PCM;
		snd_alc5633q[ i ].TS1 = i * 2;
		snd_alc5633q[ i ].TS2 = 0;
		snd_alc5633q[ i ].band_mode_sup = BAND_MODE_8K;
#endif
		snd_alc5633q[ i ].snd_ops = ( const snd_ops_t * )&snd_alc5633q_ops;
	}
	
	register_voip_snd( &snd_alc5633q[ 0 ], ALC5633Q_NUM );
	
	return 0;
}

voip_initcall_snd( voip_snd_alc5633q_init );

