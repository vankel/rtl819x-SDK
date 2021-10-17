#include "voip_manager.h"
/*
 *  T38 sample code.
 *  Send T38 to specific IP address and port number when 
 *  off-hook handset. with or without ring the phone. 
 *  Please killall solar_monitor before running this sample code.
 *
 */


int main(int argc, char *argv[])
{
	SIGNSTATE val;

	t38udp_config_t t38udp_config;
	t38_payloadtype_config_t t38_config;

	int ActiveSession;
	int bStartT38;
	int chid = 0;
	int ring_on_off=0;
	int mid, dsp_ent, dsp_data;


	if (argc != 7)
	{
		printf("usage: %s chid src_ip dest_ip src_port dest_port ring_on_off\n", argv[0]);
		printf("       ring_on_off: 0: ring off, 1~30 ring on time\n");
		return 0;
	}

	ring_on_off = atoi(argv[6]);

	// init channel
	chid = atoi(argv[1]);

	// init dsp
	rtk_InitDSP(chid);

	//rtk_SetDTMFMODE(chid, DTMF_RFC2833);	// set dtmf mode
	rtk_SetDTMFMODE(chid, DTMF_INBAND);	// set dtmf mode

	// init flag
	bStartT38 = 0;
	ActiveSession = 0;

	if (ring_on_off) {
		rtk_SetRingFXS(chid, 1);
		sleep(ring_on_off);
		rtk_SetRingFXS(chid, 0);
	}

	while (1)
	{
		for (mid=0; mid <2; mid++)
		{
			rtk_GetDspEvent(chid, mid, &dsp_ent, &dsp_data);
			if (dsp_ent == EVENT_RTP_PAYLOAD_MISMATCH)
				printf("Get PT mis-matched event, ch%d, mid%d, PT=%d\n", chid, mid, dsp_data);
		}
		rtk_GetFxsEvent(chid, &val);
		switch (val)
		{

		case SIGN_OFFHOOK:
			// call rtk_Offhook_Action at first
			rtk_Offhook_Action(chid);
			//ActiveSession = 0;
			//rtk_SetTranSessionID(chid, ActiveSession);

			//  set T.38 UDP session
			memset(&t38udp_config, 0, sizeof(t38udp_config));
			t38udp_config.chid = chid;
			t38udp_config.sid = 0;
			t38udp_config.isTcp = 0;		// use udp
			t38udp_config.extIp = inet_addr(argv[2]);
			t38udp_config.remIp = inet_addr(argv[3]);
			t38udp_config.extPort = htons(atoi(argv[4]));
			t38udp_config.remPort = htons(atoi(argv[5]));
			t38udp_config.tos = 0;
	
			rtk_SetT38UdpConfig(&t38udp_config);


			// set t38 payload, and other session parameters.
			memset(&t38_config, 0, sizeof(t38_config));
			t38_config.chid = chid;
			t38_config.sid = 0;
			t38_config.bT38ParamEnable = 0;
			t38_config.nT38MaxBuffer = 0;
			t38_config.nT38RateMgt = 0;
			t38_config.nT38MaxRate = 0;
			t38_config.bT38EnableECM = 0;
			t38_config.nT38ECCSignal = 0;
			t38_config.nT38ECCData = 0;
			t38_config.bT38EnableSpoof = 0;
			t38_config.nT38DuplicateNum = 0;
			t38_config.nPcmMode = 2;	// narrow-band

			rtk_SetT38PayloadType(&t38_config);

			// start rtp (channel number = 0, session number = 0)
			//rtk_SetRtpSessionState(chid, ActiveSession, rtp_session_sendrecv);
			bStartT38 = 1;

			printf("%s:%d -> %s:%d (ring_on_off %d)\n", 
				argv[2], atoi(argv[4]), argv[3], atoi(argv[5]), ring_on_off);
			break;

		case SIGN_ONHOOK:
			// close rtp
			rtk_SetRtpSessionState(chid, 0, rtp_session_inactive);
			rtk_SetRtpSessionState(chid, 1, rtp_session_inactive);
			if (bStartT38) {
				rtk_Onhook_Action(chid);
				goto out;
			}
			bStartT38 = 0;

			// call rtk_Offhook_Action at last
			rtk_Onhook_Action(chid);
			break;


		default:
			break;
		}

		usleep(100000); // 100ms
	}

out:

	return 0;
}

