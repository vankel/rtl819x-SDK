#include <signal.h>
#include "prmt_igd.h"
#ifdef CONFIG_USER_CWMP_WITH_TR181
#include "tr181_device.h"
#include "tr181_mgmtServer.h"
#endif
#ifdef CONFIG_MIDDLEWARE
#include <rtk/midwaredefs.h>
#endif
#ifdef CONFIG_APP_TR104
#include "cwmp_main_tr104.h"
#endif
void cwmp_show_help( void )
{
	fprintf( stderr, "cwmpClient:\n" );
	fprintf( stderr, "	-SendGetRPC:	send GetRPCMethods to ACS\n" );
	fprintf( stderr, "	-SSLAuth:	ACS need certificate the CPE\n" );
	fprintf( stderr, "	-SkipMReboot:	do not send 'M Reboot' event code\n" );
	fprintf( stderr, "	-Delay: 	delay some seconds to start\n" );
	fprintf( stderr, "	-NoDebugMsg: 	do not show debug message\n" );
	fprintf( stderr, "	-h or -help: 	show help\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "	if no arguments, read the setting from mib\n" );
	fprintf( stderr, "\n" );
}

/*refer to climenu.c*/
#define CWMP_RUNFILE	"/var/run/cwmp.pid"
static void log_pid()
{
	FILE *f;
	pid_t pid;
	char *pidfile = CWMP_RUNFILE;

	pid = getpid();
	if((f = fopen(pidfile, "w")) == NULL)
		return;
	fprintf(f, "%d\n", pid);
	fclose(f);
}

static void clr_pid()
{
		unlink(CWMP_RUNFILE);
#ifdef CONFIG_MIDDLEWARE
		unlink(CWMP_MIDPROC_RUNFILE);
#endif
}

#ifdef CONFIG_MIDDLEWARE
extern void handle_alarm(int sig);
extern void updateMidprocTimer();

void cwmp_handle_alarm(int sig)
{
	handle_alarm(0);
	updateMidprocTimer();
}

void handle_x_ct_event(int sig)
{
	unsigned char vChar;
	unsigned int vUint=0;
	
	printf("\n%s\n",__FUNCTION__);
	mib_get(CWMP_TR069_ENABLE,(void*)&vChar);
	if(vChar == 1 || vChar == 2){
		if( mib_get(MIB_CWMP_INFORM_EVENTCODE, &vUint)!=0 ){
			mib_get(MIB_MIDWARE_INFORM_EVENT,(void *)&vChar);
			switch(vChar){
			case CTEVENT_ACCOUNTCHANGE:
				vUint = vUint|(EC_X_CT_COM_ACCOUNT);
				break;
#ifdef _PRMT_X_CT_COM_USERINFO_
			case CTEVENT_BIND:
				vUint = vUint|(EC_X_CT_COM_BIND);
				break;
#endif
#ifdef E8B_NEW_DIAGNOSE
			case CTEVENT_SEND_INFORM:
				vUint = vUint|(EC_X_CT_COM_SEND_INFORM);
				break;
#endif
			default:
				break;
			}
			mib_set(MIB_CWMP_INFORM_EVENTCODE, &vUint);
		}
	}
}
#else
void handle_x_ct_account(int sig)
{
	unsigned int vUint=0;

	warn("%s():%d ", __FUNCTION__, __LINE__);
	if (mib_get(MIB_CWMP_INFORM_EVENTCODE, &vUint)) {
		vUint = vUint | EC_X_CT_COM_ACCOUNT;
		mib_set(MIB_CWMP_INFORM_EVENTCODE, &vUint);
	}
}

#ifdef _PRMT_X_CT_COM_USERINFO_
void handle_x_ct_bind(int sig)
{
	unsigned int vUint;

	warn("%s():%d ", __FUNCTION__, __LINE__);
	if (mib_get(MIB_CWMP_INFORM_EVENTCODE, &vUint)) {
		vUint = vUint | EC_X_CT_COM_BIND;
		mib_set(MIB_CWMP_INFORM_EVENTCODE, &vUint);
	}	
}
#endif
#endif //#ifdef CONFIG_MIDDLEWARE


/*star:20091229 START send signal to cwmp process to let it know that wan connection ip changed*/
void sigusr1_handler()
{
	notify_set_wan_changed();
/*star:20100305 START add qos rule to set tr069 packets to the first priority queue*/
#if defined(IP_QOS) || defined(NEW_IP_QOS_SUPPORT)
	setTr069QosFlag(0);
#endif
/*star:20100305 END*/
}
/*star:20091229 END*/

void clear_child(int i)
{
	int status;
	pid_t chidpid = 0;

	//chidpid=wait( &status );
#ifdef _PRMT_TR143_
#ifdef CONFIG_USER_FTP_FTP_FTP
	//if(chidpid!=-1)
		checkPidforFTPDiag( chidpid );
#endif //CONFIG_USER_FTP_FTP_FTP
#endif //_PRMT_TR143_
#ifdef _SUPPORT_TRACEROUTE_PROFILE_
//	if(chidpid!=-1)
		checkPidforTraceRouteDiag( chidpid );
#endif //_SUPPORT_TRACEROUTE_PROFILE_
	return;
}

void handle_term()
{
	clr_pid();
#ifdef CONFIG_APP_TR104	
	tr104_main_exit();
#endif
	exit(0);
}

int main(int argc, char **argv)
{
	log_pid();

	apmib_init();
	
#ifdef CONFIG_BOA_WEB_E8B_CH
#ifdef CONFIG_MIDDLEWARE
	signal(SIGUSR1, handle_x_ct_event);		//xl_yue: SIGUSR2 is used by midware 
#else
	signal( SIGUSR1,handle_x_ct_account);
#ifdef _PRMT_X_CT_COM_USERINFO_
	signal(SIGUSR2, handle_x_ct_bind);
#endif
#endif
#else
	signal(SIGUSR1, sigusr1_handler);
#endif

	signal(SIGCHLD, clear_child);	//set this signal process function according to CWMP_TR069_ENABLE below if MIDDLEWARE is defined
	signal( SIGTERM,handle_term);
	
/*star:20100305 START add qos rule to set tr069 packets to the first priority queue*/
#if defined(IP_QOS) || defined(NEW_IP_QOS_SUPPORT)
	setTr069QosFlag(0);
#endif
/*star:20100305 END*/

	if( argc >= 2 )
	{
		int i;
		for(i=1;i<argc;i++)
		{
			if( strcmp( argv[i], "-SendGetRPC" )==0 )
			{
				cwmpinit_SendGetRPC(1);
				fprintf( stderr, "<%s>Send GetPRCMethods to ACS\n",__FILE__ );
			}else if( strcmp( argv[i], "-SSLAuth" )==0 )
			{
				cwmpinit_SSLAuth(1);
				fprintf( stderr, "<%s>Set using certificate auth.\n",__FILE__ );
			}else if( strcmp( argv[i], "-SkipMReboot" )==0 )
			{
				cwmpinit_SkipMReboot(1);
				fprintf( stderr, "<%s>Set skipping MReboot event code\n",__FILE__ );
			}else if( strcmp( argv[i], "-Delay" )==0 )
			{
				cwmpinit_DelayStart(30);
				fprintf( stderr, "<%s>Set Delay!\n", __FILE__ );
			}else if( strcmp( argv[i], "-NoDebugMsg" )==0 )
			{
				cwmpinit_NoDebugMsg(1);
				fprintf( stderr, "<%s>Set No Debug Message!\n", __FILE__ );
			}else if( strcmp( argv[i], "-h" )==0 || strcmp( argv[i], "-help" )==0 )
			{
				cwmp_show_help();
				exit(0);
			}else
			{
				fprintf( stderr, "<%s>Error argument: %s\n", __FILE__,argv[i] );
			}
		}
	}else{
		unsigned int cwmp_flag=0;
		//read the flag, CWMP_FLAG, from mib
		if ( mib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag)!=0 )
		{
			printf("\ncwmp_flag=0x%x\n",cwmp_flag);
			if( (cwmp_flag&CWMP_FLAG_DEBUG_MSG)==0 )
			{
				fprintf( stderr, "<%s>Set No Debug Message!\n", __FILE__ );
				cwmpinit_NoDebugMsg(1);
			}
				
			if( cwmp_flag&CWMP_FLAG_CERT_AUTH )
			{
				fprintf( stderr, "<%s>Set using certificate auth.\n",__FILE__ );
				cwmpinit_SSLAuth(1);
			}
				
			if( cwmp_flag&CWMP_FLAG_SENDGETRPC )
			{
				fprintf( stderr, "<%s>Send GetPRCMethods to ACS\n",__FILE__ );
				cwmpinit_SendGetRPC(1);
			}
			
			if( cwmp_flag&CWMP_FLAG_SKIPMREBOOT )
			{
				fprintf( stderr, "<%s>Set skipping MReboot event code\n",__FILE__ );
				cwmpinit_SkipMReboot(1);
			}
				
			if( cwmp_flag&CWMP_FLAG_DELAY )
			{
				fprintf( stderr, "<%s>Set Delay!\n", __FILE__ );
				cwmpinit_DelayStart(30);
			}

			if( cwmp_flag&CWMP_FLAG_SELFREBOOT)
			{
				fprintf( stderr, "<%s>Set SelfReboot!\n", __FILE__ );
				cwmpinit_SelfReboot(1);
			}
				
		}

		if ( mib_get( MIB_CWMP_FLAG2, (void *)&cwmp_flag)!=0 )
		{
#if defined(CONFIG_BOA_WEB_E8B_CH) || defined(_TR069_CONREQ_AUTH_SELECT_)
			if( cwmp_flag&CWMP_FLAG2_DIS_CONREQ_AUTH)
			{
				fprintf( stderr, "<%s>Set DisConReqAuth!\n", __FILE__ );
				cwmpinit_DisConReqAuth(1);
			}
#endif

			if( cwmp_flag&CWMP_FLAG2_DEFAULT_WANIP_IN_INFORM)
				cwmpinit_OnlyDefaultWanIPinInform(1);
			else
				cwmpinit_OnlyDefaultWanIPinInform(0);

		}		
	}

#ifdef CONFIG_USER_CWMP_WITH_TR181
#ifdef TR069_ANNEX_G
	{
		unsigned int stunEn = 0;
		
		if (mib_get(MIB_CWMP_STUN_EN, (void *)&stunEn) != 0)
		{
			if (stunEn)
				cwmpStartStun();
			else
				cwmpStopStun();
		}
	}
#endif
#endif

#ifdef TELEFONICA_DEFAULT_CFG
//#ifdef WLAN_SUPPORT
	cwmpinit_BringLanMacAddrInInform(1);
//#endif //WLAN_SUPPORT
	cwmpinit_SslSetAllowSelfSignedCert(0);
#endif //TELEFONICA_DEFAULT_CFG

/*star:20100105 START when there is already a session, if return 503 after receive connection request*/
	cwmpinit_Return503(0);
/*star:20100105 END*/
#ifdef CONFIG_APP_TR104
	//cwmp_solarOpen();
	tr104_main_init();
#endif
//startRip();
	printf("\nenter cwmp_main!\n");

#ifdef CONFIG_MIDDLEWARE
	cwmp_main(mw_tROOT);
#else
#ifdef CONFIG_USER_CWMP_WITH_TR181
	cwmp_main( tDevROOT );
#else
	cwmp_main( tROOT );
#endif
#endif

	return 0;
}
