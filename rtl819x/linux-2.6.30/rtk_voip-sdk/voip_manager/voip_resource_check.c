#include "rtk_voip.h"
#include "voip_types.h"
#include "voip_control.h"

#include "voip_resource_check.h"

#ifdef VOIP_RESOURCE_CHECK

int resource_weight[DSP_RTK_SS_NUM] = {0};

int GetCurrentVoipResourceStatus(int pkt_fmt)
{
	int ss_id, weight_sum=0, weight, pt, result;
	for (ss_id=0; ss_id < DSP_RTK_SS_NUM; ss_id++)
	{
		weight_sum += resource_weight[ss_id];
	}

	pt = pkt_fmt;

	if ((pt == rtpPayloadPCMU) || (pt == rtpPayloadPCMA))
		weight = G711_WEIGHT;
	else if ((pt == rtpPayloadGSM) ||(pt == rtpPayloadG723) || (pt == rtpPayloadG729) )
		weight = G729_WEIGHT;
	else if ((pt == rtpPayloadG726_16) || (pt == rtpPayloadG726_24) || (pt == rtpPayloadG726_32) || (pt == rtpPayloadG726_40))
		weight = G726_WEIGHT;
	else if ((pt == rtpPayload_iLBC) || (pt == rtpPayload_iLBC_20ms))
		weight = ILBC_WEIGHT;
	else if ((pt == rtpPayload_SPEEX_NB_RATE2P15) || (pt == rtpPayload_SPEEX_NB_RATE5P95) || (pt == rtpPayload_SPEEX_NB_RATE8) || (pt == rtpPayload_SPEEX_NB_RATE11)
	      || (pt == rtpPayload_SPEEX_NB_RATE15) || (pt == rtpPayload_SPEEX_NB_RATE18P2) || (pt == rtpPayload_SPEEX_NB_RATE24P6) || (pt == rtpPayload_SPEEX_NB_RATE3P95))
		weight = SPEEXNB_WEIGHT;
	else if (pt == rtpPayloadT38_Virtual)
		weight = T38_WEIGHT;
	else if ((pt >= rtpPayload_AMR_NB_RATE4P75) && (pt <= rtpPayload_AMR_NB_RATE12P2))
		weight = AMRNB_WEIGHT;
	else if ((pt >= rtpPayload_AMR_WB_RATE6P6) && (pt <= rtpPayload_AMR_WB_RATE23P85))
		weight = AMRWB_WEIGHT;
	else
		weight = DEFAULT_WEIGHT;

	//printk("weight_sum=%d, weight=%d, ===>", weight_sum, weight);

	if ((weight_sum + weight) > RES_WEIGHT_THS)
	{
		result = VOIP_RESOURCE_UNAVAILABLE;
		//printk("VOIP_RESOURCE_UNAVAILABLE\n");
	}
	else
	{
		result = VOIP_RESOURCE_AVAILABLE;
		//printk("VOIP_RESOURCE_AVAILABLE\n");
	}

	return 	result;
}

int SetVoipResourceWeight(uint32 s_id, int pltype)
{
	if ((pltype == rtpPayloadPCMU) || (pltype == rtpPayloadPCMA))
		resource_weight[s_id] = G711_WEIGHT;
	else if ((pltype == rtpPayloadGSM) ||(pltype == rtpPayloadG723) || (pltype == rtpPayloadG729))
		resource_weight[s_id] = G729_WEIGHT;
	else if ((pltype == rtpPayloadG726_16) || (pltype == rtpPayloadG726_24)|| (pltype == rtpPayloadG726_32) || (pltype == rtpPayloadG726_40))
		resource_weight[s_id] = G726_WEIGHT;
	else if ((pltype == rtpPayload_iLBC) || (pltype == rtpPayload_iLBC_20ms))
		resource_weight[s_id] = ILBC_WEIGHT;
	else if (pltype == rtpPayloadT38_Virtual)
		resource_weight[s_id] = T38_WEIGHT;
	else if (pltype == rtpPayloadG722)
		resource_weight[s_id] = G722_WEIGHT;
	else if ((pltype == rtpPayload_SPEEX_NB_RATE2P15) || 
		(pltype == rtpPayload_SPEEX_NB_RATE5P95) || 
		(pltype == rtpPayload_SPEEX_NB_RATE8) || 
		(pltype == rtpPayload_SPEEX_NB_RATE11) || 
		(pltype == rtpPayload_SPEEX_NB_RATE15) ||
		(pltype == rtpPayload_SPEEX_NB_RATE18P2) || 
		(pltype == rtpPayload_SPEEX_NB_RATE24P6) || 
		(pltype == rtpPayload_SPEEX_NB_RATE3P95))
		resource_weight[s_id] = SPEEXNB_WEIGHT;
	else if ((pltype == rtpPayloadPCMU_WB) || (pltype == rtpPayloadPCMA_WB))
		resource_weight[s_id] = G711_WB_WEIGHT;
	else if ((pltype >= rtpPayload_AMR_NB_RATE4P75) && (pltype <= rtpPayload_AMR_NB_RATE12P2))
		resource_weight[s_id] = AMRNB_WEIGHT;
	else if ((pltype >= rtpPayload_AMR_WB_RATE6P6) && (pltype <= rtpPayload_AMR_WB_RATE23P85))
		resource_weight[s_id] = AMRWB_WEIGHT;
	else
	{
		DBG_ERROR("%s: set pltype%d weight = %d\n", __FUNCTION__, pltype, 1);
		resource_weight[s_id] = 1;
	}

	return resource_weight[s_id];
}

#endif // VOIP_RESOURCE_CHECK

