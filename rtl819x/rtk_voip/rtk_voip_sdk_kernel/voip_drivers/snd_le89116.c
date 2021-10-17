#include "rtk_voip.h"
#include "voip_init.h"

#include "con_register.h"
#include "con_defer_init.h"
#include "snd_pin_cs.h"
#include "snd_help_ipc.h"
#include "snd_zarlink_ioc_op.h"

#include "zarlink_int.h"
#include "zarlink_api.h"

#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS_USE_SW
#include "gpio.h"
#endif
#include "spi.h"

static voip_snd_t			snd_zarlink_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR];
static rtl_spi_dev_t		spi_dev_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR];
/*
** Realtek memory space that API-2 requires 
** for the Device/Line contexts and objects
*/
static RTKDevObj	  	  	DevObj_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR]; //
static RTKLineObj	  	  	LineObj_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR]; //1FXS

/*
** Application memory space that API-2 requires 
** for the Device/Line contexts and objects
*/
static Vp890DeviceObjectType  VpDevObj_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR];
static Vp890LineObjectType    VpLineObj_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR];

static VpDevCtxType           VpDevCtx_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR];

static VpLineCtxType          VpLineCtx_le89116[CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR];

#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS_USE_SW
#if 1
static const uint32 * const pin_cs_le89116 = 
		&snd_pin_cs[ CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS - 1 ];
#else
static uint32 pin_cs_le89116[] = {
	PIN_CS1,
	PIN_CS2,
	PIN_CS3,
	PIN_CS4,
};

#define NUM_OF_PIN_CS_LE89116		( sizeof( pin_cs_le89116 ) / sizeof( pin_cs_le89116[ 0 ] ) )

CT_ASSERT( NUM_OF_PIN_CS_LE89116 >= CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR );
#endif
#endif

static int SLIC_init_le89116(int pcm_mode, int initonly)
{
	int i;
	RTKDevObj *pDev;
	int PcnType;
   	int rtn;
   	//int ch;

// setup pcm fs, slic reset
#ifdef CONFIG_RTK_VOIP_PLATFORM_8686
	extern void aipc_module_voip_set_pcm_fs(void);
	extern void aipc_module_voip_slic_reset(void);
	extern unsigned int soc_set_pcm_fs;
    if( soc_set_pcm_fs == 0 ){
		aipc_module_voip_set_pcm_fs();
#ifdef CONFIG_RTL8686_SLIC_RESET 
		aipc_module_voip_slic_reset();
#endif
		soc_set_pcm_fs = 1;
	}
#endif
	gpioResetSLIC();
	PRINT_MSG("================================================\n");
	PRINT_MSG("Zarlink SLIC_init_le89116 API-II Lite Version %d.%d.%d\n", 
				VP_API_VERSION_MAJOR_NUM, 
				VP_API_VERSION_MINOR_NUM, 
				VP_API_VERSION_MINI_NUM);
			
	if( initonly )
		goto label_do_init_only;

	/* ******************* */
	/* Init Realtek obj    */
	/* ******************* */
		
	// setup GPIO for SPI 
	for( i = 0; i < CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR; i ++ ) {
		#ifdef CONFIG_RTK_VOIP_8676_SHARED_SPI
		PRINT_MSG( "le89116[%d] CS=%08X\n", i, CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS - 1 + i);
		init_spi_pins( &spi_dev_le89116[ i ], CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS - 1 + i, PIN_CLK, PIN_DO, PIN_DI);
		#else
	#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS_USE_SW
		PRINT_MSG( "le89116[%d] CS=%08X\n", i, pin_cs_le89116[ i ] );
		init_spi_pins( &spi_dev_le89116[ i ], pin_cs_le89116[ i ], PIN_CLK, PIN_DO, PIN_DI);
	#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS_USE_HW )
		PRINT_MSG( "le89116[%d] CS=%08X\n", i, 
								CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS + i );
		_init_rtl_spi_dev_type_hw( &spi_dev_le89116[ i ], 
								CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS + i);
	#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS_USE_HW3 )
			PRINT_MSG( "le89116[%d] CS=%08X\n", i, 
								CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS + i );
		_init_rtl_spi_dev_type_hw3( &spi_dev_le89116[ i ], 
								CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_PIN_CS/* CS special mode use the same CS pin*/,
								PIN_CSEN, i);
	#endif
		#endif
	}
	
	// Create Dev & Line Object 
	for( i = 0; i < CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR; i ++ ) 
	{
		Ve890CreateDevObj( 
			LINE_FXS, i, 
			&DevObj_le89116[ i ], &LineObj_le89116[ i ], 
			&spi_dev_le89116[ i ], VP_DEV_890_SERIES,
			&VpDevObj_le89116[ i ], &VpDevCtx_le89116[ i ],
			&VpLineObj_le89116[ i ], &VpLineCtx_le89116[ i ]);
	
		// FXS
		Ve890CreateLineObj( rtkGetNewChID(), 0, LINE_FXS, DevObj_le89116[i].pLine[0],
							pcm_mode, snd_zarlink_le89116[ i ].TS1 );
	}
	
label_do_init_only:
	/* ******************* */
	/* Init Zarlink API-II */
	/* ******************* */
	
	for( i = 0; i < CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR; i ++ ) 
	{
		/* deviceId imply spi device id */
		pDev = &DevObj_le89116[ i ];
		PcnType = Ve890GetRev( pDev );
		
		if ( PcnType == FAILED ) {
			PRINT_R("Error (%d:x) %s Read version fail\n", i, __FUNCTION__);
			return FAILED;
		}
		
		/* Initialize API-2 device settings */
		rtn = zarlinkInitDevice( pDev );
		if ( rtn == FAILED ) {
			PRINT_R("Error (%d:x) %s\n", i, __FUNCTION__);
			return FAILED;
		}
	}
	
	return SUCCESS;
}

static void SLIC_reset_le89116( voip_snd_t *this, int codec_law )
{
	extern int law;	// FIXME: chmap 
	
	SLIC_init_le89116( law, 1 /* init only */ );
}

static void SLIC_show_ID_le89116( voip_snd_t *this )
{
	RTKLineObj * const pLine = (RTKLineObj * )this ->priv;
	RTKDevObj * const pDev = pLine ->pDev;
	
	Ve890GetRev( pDev );
}

// --------------------------------------------------------
// channel mapping architecture 
// --------------------------------------------------------
__attribute__ ((section(".snd_desc_data")))
static snd_ops_fxs_t snd_le89116_fxs_ops;

static void __init fill_le89116_register_info( 
	voip_snd_t snd_zarlink_le89xxx[],
	int n_fxs, int m_daa, uint16 TS_base,
	RTKLineObj LineObj_le89xxx[]
	)
{
	// once call this function:
	//  - one control interface 
	//  - n fxs
	//  - m daa 
	int sch;
	int daa = 0;
	
	for( sch = 0; sch < n_fxs + m_daa; sch ++ ) {
	
		if( sch == n_fxs )
			daa = 1;

		snd_zarlink_le89xxx[ sch ].sch = sch;
		snd_zarlink_le89xxx[ sch ].name = "le89116";
		snd_zarlink_le89xxx[ sch ].snd_type = SND_TYPE_FXS;
		snd_zarlink_le89xxx[ sch ].bus_type_sup = BUS_TYPE_PCM;
		snd_zarlink_le89xxx[ sch ].TS1 = TS_base + sch * 2;
#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_WIDEBAND
		snd_zarlink_le89xxx[ sch ].TS2 = ( daa || TS_base + sch * 2 >= 16 ? 0 : TS_base + ( sch + 8 ) * 2 );
		snd_zarlink_le89xxx[ sch ].band_mode_sup = ( daa ? BAND_MODE_8K : BAND_MODE_8K | BAND_MODE_16K );
#else
		snd_zarlink_le89xxx[ sch ].TS2 = 0;
		snd_zarlink_le89xxx[ sch ].band_mode_sup = BAND_MODE_8K;
#endif
		snd_zarlink_le89xxx[ sch ].snd_flags.b.attenuateTx_6dB = ( daa ? 1 : 0 );
		snd_zarlink_le89xxx[ sch ].snd_ops = ( const snd_ops_t * )&snd_le89116_fxs_ops;
		snd_zarlink_le89xxx[ sch ].priv = &LineObj_le89xxx[ sch ];
		
		// DAA port  
		if( daa ) {
			snd_zarlink_le89xxx[ sch ].snd_type = SND_TYPE_DAA;
#if 0
			snd_zarlink_le89xxx[ sch ].snd_ops = ( const snd_ops_t * )&snd_zarlink_daa_ops;
#else
			printk( "No snd_ops for DAA!!\n" );
#endif
		}		
	}
}

static int __init voip_snd_zarlink_init_le89116( void )
{
	extern int law;	// FIXME: chmap 
	extern const snd_ops_fxs_t snd_zarlink_fxs_ops;
	int i;//, sch, daa;
	int TS_base, TS_cur;
	int rtn;
#ifdef CONFIG_RTK_VOIP_DEFER_SNDDEV_INIT
	static defer_init_t le89116_defer;
#endif
	
	// le89116 override proslic base ops 
	snd_le89116_fxs_ops = snd_zarlink_fxs_ops;

	snd_le89116_fxs_ops.SLIC_reset = SLIC_reset_le89116;
	snd_le89116_fxs_ops.SLIC_show_ID = SLIC_show_ID_le89116;
	
	// get TS base 
	TS_base = get_snd_free_timeslot();
	if( TS_base < 0 )
		TS_base = 0;
	
	TS_cur = TS_base;
	
	// common port definition 
	for( i = 0; i < CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR; i ++ ) {
		
		fill_le89116_register_info( &snd_zarlink_le89116[ i ], 
				1 /* fxs */, 0 /* daa */, (uint16)TS_cur,
				&LineObj_le89116[ i ] );

#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_OWNER_ALL_DSP
		if( fill_ipc_dsp_cpuid( &snd_zarlink_le89116[ i ], 
				1 /* fxs */, 0 /* daa */, 
				i, 
				SND_PER_DSPCPU( CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR, 
								CONFIG_RTK_VOIP_DSP_DEVICE_NR ) ) )
		{
			TS_cur = TS_base;
		} else
#endif
		{
			TS_cur += 2;
		}
		
	}
	
	// SLIC init use ops 
#ifdef CONFIG_RTK_VOIP_DEFER_SNDDEV_INIT
	le89116_defer.fn_defer_func = ( fn_defer_func_t )SLIC_init_le89116;
	le89116_defer.p0 = law;
	le89116_defer.p1 = 0;
	
	add_defer_initialization( &le89116_defer );
	rtn = 0; //success
#else
	rtn = SLIC_init_le89116( law, 0 /* allocate */ );
#endif

	if ( rtn == FAILED ) /* init Device fail */
		return -1;

	for( i = 0; i < CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR; i ++ ) 
		register_voip_snd( &snd_zarlink_le89116[ i ], 1 );	
	
	return 0;
}

voip_initcall_snd( voip_snd_zarlink_init_le89116 );

// --------------------------------------------------------
// IOC support 
// --------------------------------------------------------
#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_IO
#define LE89116_IO_NUM		3

static voip_ioc_t le89116_ioc[ CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR ][ LE89116_IO_NUM ];
static zarlink_ioc_priv_t le89116_ioc_priv[ CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR ][ LE89116_IO_NUM ];

#ifdef CONFIG_RTK_VOIP_DEFER_SNDDEV_INIT
static int voip_ioc_zarlink_init_le89116_defer( void )
{
	int i, j;
	for( i = 0; i < CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR; i ++ ) {
		for( j = 0; j < LE89116_IO_NUM; j ++ ) {
			voip_ioc_t * const p_ioc = &le89116_ioc[ i ][ j ];
			
			InitializeZarlinkIO( p_ioc );
		}
	}
	return SUCCESS;
}
#endif

static int __init voip_ioc_zarlink_init_le89116( void )
{
	int i, j;

#ifdef CONFIG_RTK_VOIP_DEFER_SNDDEV_INIT
	static defer_init_t le89116_ioc_defer;
#endif
	for( i = 0; i < CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89116_NR; i ++ ) {
		for( j = 0; j < LE89116_IO_NUM; j ++ ) {
			
			voip_ioc_t * const p_ioc = &le89116_ioc[ i ][ j ];
			zarlink_ioc_priv_t * const p_ioc_priv = &le89116_ioc_priv[ i ][ j ];
			voip_snd_t * const p_snd = &snd_zarlink_le89116[ i ];
			
			p_ioc ->ioch = ( i << 8 ) | j;
			p_ioc ->name = "le89116";
			p_ioc ->ioc_type = ( j == 2 ? IOC_TYPE_RELAY : IOC_TYPE_LED );
			p_ioc ->pre_assigned_snd_ptr = ( j < 3 ? p_snd : NULL );
			p_ioc ->ioc_ops = ( p_ioc ->ioc_type == IOC_TYPE_RELAY ? &ioc_relay_ops_zarlink : &ioc_led_ops_zarlink );
			p_ioc ->priv = p_ioc_priv;
			
			p_ioc ->state_var = ( p_ioc ->ioc_type == IOC_TYPE_RELAY ? IOC_STATE_RELAY_CLOSE : IOC_STATE_LED_OFF );
			
			p_ioc_priv ->IO = zarlink_VPIO_list[ j ];
			p_ioc_priv ->snd_priv = p_snd ->priv;

		#ifndef CONFIG_RTK_VOIP_DEFER_SNDDEV_INIT
			InitializeZarlinkIO( p_ioc );
		#endif
		}
		
		register_voip_ioc( &le89116_ioc[ i ][ 0 ], LE89116_IO_NUM );
	}
	#if defined(CONFIG_RTK_VOIP_DEFER_SNDDEV_INIT)
		le89116_ioc_defer.fn_defer_func = ( fn_defer_func_t )voip_ioc_zarlink_init_le89116_defer;
		le89116_ioc_defer.p0 = 0;
		le89116_ioc_defer.p1 = 0;

		add_defer_initialization( &le89116_ioc_defer );
	#endif

	return 0;
}

voip_initcall_ioc( voip_ioc_zarlink_init_le89116 );
#endif

