/*
 * IOH daemon
 * Copyright (C)2010, Realtek Semiconductor Corp. All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <syslog.h>

#include <linux/if_packet.h>
#include "ioh.h"
#include "inband_if.h"
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
#include "../hcm/hcd/cmd.h"
#endif

#if 0
struct inband_header {
	struct ioh_header raw_header;
	unsigned char rrcp_type;	// should be RRCP_P_IOH
	unsigned char inband_cmd;
	unsigned short inband_seq;
	unsigned short inband_frag;
	unsigned char inband_index;
	unsigned char inband_reserved;
	unsigned short inband_data_len;
} __attribute__((packed));
#endif

struct fragment_info{
	unsigned char inband_cmd;
	unsigned short inband_seq;
	unsigned short inband_frag;
	char *buf_ptr;
	char *buf;	
	unsigned int data_len;
};

struct inband_class {
	struct ioh_class ioh_obj;
	struct inband_header *tx_header;
	struct inband_header *rx_header;
	unsigned char *tx_data;
	unsigned char *rx_data;
	struct fragment_info frag_info;
	unsigned int in_used;
};

//header : 4 sign , 4 opttion , 4 offset, 4 len
#define FM_HEADER_LEN_OFFSET 12

#define MAX_INBAND_CHAN 2
#define MAX_PREALLOC_INBAND_CHAN MAX_INBAND_CHAN

int inband_rcv_timeout=0; //mark_issue, not implement now

struct inband_class inband_obj[MAX_INBAND_CHAN];
static int inband_ready=0;


struct record_old_pkt_info {
	unsigned int save_pkt_time;
	unsigned char inband_cmd;
	unsigned short inband_seq;
	unsigned short inband_frag; 
	unsigned char sa[ETH_ALEN];
};
#define RECORD_OLD_PKT_NUM 5
struct record_old_pkt_info record_old_pkt[RECORD_OLD_PKT_NUM];
struct ioh_class ioh_obj_ack_old_pkt;

static void init_inband_obj(struct inband_class *ib_obj)
{
	struct ioh_class *obj=&ib_obj->ioh_obj;

	ib_obj->tx_header = (struct inband_header *)obj->tx_header;
	ib_obj->rx_header = (struct inband_header *)obj->rx_header;

	ib_obj->tx_data = (unsigned char *) obj->tx_buffer + sizeof(struct inband_header);
	ib_obj->rx_data = (unsigned char *) obj->rx_buffer + sizeof(struct inband_header);

	//frag_info?
}

static unsigned int get_free_chan()
{	
	int i;
	int chan=-1;
	
	for(i=0;i<MAX_INBAND_CHAN;i++)
	{
		if(inband_obj[i].in_used ==0 )			
		{
			inband_obj[i].in_used =1;
			chan = i;
			break;
		}		
	}
	return chan;	
}

struct inband_class *get_chan_obj(unsigned int chan)
{
	return (struct inband_class *)&inband_obj[chan];
}

static void inband_init_all()
{
	memset(&inband_obj[0],0,sizeof(struct inband_class)*MAX_INBAND_CHAN);
	memset(&record_old_pkt[0],0,sizeof(struct record_old_pkt_info)*RECORD_OLD_PKT_NUM);
}

int inband_open(char *netif_name,char *slave_mac,unsigned short eth_type,int debug)
{	
	int ret;
	int chan=0;
	struct inband_class *inband_obj_p;
	struct ioh_class *ioh_obj_p;

	if(inband_ready == 0)
	{
		inband_init_all();
		inband_ready =1 ;
	}

	chan = get_free_chan();
	if(chan < 0) {
        syslog(LOG_DEBUG, "open channel error! chann=%d\n", chan);
		return -1;
	}

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);

	ioh_obj_p = &inband_obj_p->ioh_obj;	
		
	ret = ioh_open(ioh_obj_p, netif_name, slave_mac,eth_type, debug);

	if(ret < 0 ) {
		return -1;
	}

	init_inband_obj(inband_obj_p);

	return chan;	
}

int get_inband_socket(int chan)
{
	struct inband_class *inband_obj_p;
	struct ioh_class *ioh_obj_p;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);
	ioh_obj_p = &inband_obj_p->ioh_obj;	
	
	return ioh_obj_p->sockfd;
}

int get_inband_destMac(int chan,char *destmac) //mark_test
{
	struct inband_class *inband_obj_p;
	struct ioh_class *ioh_obj_p;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);
	ioh_obj_p = &inband_obj_p->ioh_obj;	
	
	memcpy(destmac,ioh_obj_p->dest_mac,6);
	
	return 0;
}

void inband_close(int chan)
{	
	//clear frag_info
	struct inband_class *inband_obj_p;
	struct ioh_class *ioh_obj_p;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	
	ioh_obj_p = &inband_obj_p->ioh_obj;	
	ioh_close(ioh_obj_p);	
	inband_obj_p->in_used =0; //free the obj
}

static int inband_recv(struct inband_class *ib_obj,int timeout)
{
    struct ioh_class *ioh_obj_p = &ib_obj->ioh_obj;
    int rx_len;

    memset(ioh_obj_p->rx_buffer,0,BUF_SIZE);
    rx_len = ioh_recv(ioh_obj_p, timeout);
    if (rx_len < 0) {
       syslog(LOG_DEBUG, "receive timeout, rx_len=%d", rx_len);
       return ERROR_TIMOUT;
	}

#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
	if(ioh_obj_p->which_fd == 2)
		return rx_len;
#endif

    if(ib_obj->rx_header->rrcp_type != RRCP_P_IOH)
            return -1;

    return rx_len;
}

static int inband_indexed_recv(struct inband_class *ib_obj,unsigned char idx,int timeout)
{
    struct ioh_class *ioh_obj_p = &ib_obj->ioh_obj;
    int rx_len;

    rx_len = ioh_recv(ioh_obj_p, timeout);
    if (rx_len < 0) {
       syslog(LOG_DEBUG, "receive timeout, rx_len=%d", rx_len);
       return ERROR_TIMOUT;
	}
    if(ib_obj->rx_header->rrcp_type != RRCP_P_IOH)
            return -1;

	if(idx != ib_obj->rx_header->inband_index) {
		printf("%s %d idx:%d != inband_index:%d, dropped!!\n",__FUNCTION__,__LINE__);
		syslog(LOG_DEBUG, "%s %d idx:%d != inband_index:%d, dropped!!\n",__FUNCTION__,__LINE__);
		return -1;
	}

    return rx_len;
}

static int SaveOldPktInfo(struct inband_class *ib_obj)
{
	int i;
	unsigned int old_num=0;
	unsigned int new_num=0;
	for(i=0;i<RECORD_OLD_PKT_NUM;i++){
		if(record_old_pkt[old_num].save_pkt_time > record_old_pkt[i].save_pkt_time){
			old_num = i;
		}
		if(record_old_pkt[new_num].save_pkt_time < record_old_pkt[i].save_pkt_time){
			new_num = i;
		}
	}
	record_old_pkt[old_num].save_pkt_time = record_old_pkt[new_num].save_pkt_time + 1;
	record_old_pkt[old_num].save_pkt_time = record_old_pkt[old_num].save_pkt_time == 0 ? 1:record_old_pkt[old_num].save_pkt_time;
 	record_old_pkt[old_num].inband_cmd = ib_obj->rx_header->inband_cmd;
	record_old_pkt[old_num].inband_seq = ib_obj->rx_header->inband_seq;
	record_old_pkt[old_num].inband_frag = ib_obj->rx_header->inband_frag;
	memcpy(record_old_pkt[old_num].sa,ib_obj->rx_header->raw_header.sa,ETH_ALEN);
	return 0;
}

static int send_frag_ack(struct inband_class *ib_obj)
{
	struct ioh_class *ioh_obj_p = &ib_obj->ioh_obj;
	
	ib_obj->tx_header->rrcp_type = RRCP_P_IOH;  //mark_inband
	ib_obj->tx_header->inband_cmd = ib_obj->rx_header->inband_cmd;
	ib_obj->tx_header->inband_seq = ib_obj->rx_header->inband_seq;
	ib_obj->tx_header->inband_frag = ib_obj->rx_header->inband_frag;
	ib_obj->tx_header->inband_data_len = 0;
	SaveOldPktInfo(ib_obj);//save old packet info
	return ioh_send(ioh_obj_p,sizeof(struct inband_header)); //only send header for ack
}

static int CheckReplayOldPkt(struct inband_class *ib_obj)
{
	int i,chan_ack,old_pkt_flag=0;
	char mac_buff[13];
	struct ioh_class *ioh_obj_p = &ib_obj->ioh_obj;
	struct inband_class *ib_obj_p_ack;
		
	for(i=0;i<RECORD_OLD_PKT_NUM;i++){
		if(record_old_pkt[i].save_pkt_time){
			if((ib_obj->rx_header->inband_cmd == record_old_pkt[i].inband_cmd) &&
			(ib_obj->rx_header->inband_seq == record_old_pkt[i].inband_seq) &&
			(ib_obj->rx_header->inband_frag == record_old_pkt[i].inband_frag) &&
			(!memcmp(ib_obj->rx_header->raw_header.sa,record_old_pkt[i].sa,6))&&
			(ib_obj->rx_header->inband_data_len != 0)){	
				old_pkt_flag = 1;	
			}
		}
	}
	if(!old_pkt_flag){
		if(ib_obj->rx_header->inband_reserved & RETRY_PACKET_MASK){
			old_pkt_flag = 1;
			for(i=0;i<RECORD_OLD_PKT_NUM;i++){
				if(record_old_pkt[i].save_pkt_time){
					old_pkt_flag = 0;
					break;
				}
			}
		}
	}
	if(old_pkt_flag){
		printf("Rcv old packet seq=%d, send ACK!\n",ib_obj->rx_header->inband_seq);
		ioh_obj_ack_old_pkt = *ioh_obj_p;
		ioh_obj_ack_old_pkt.tx_header = (struct ioh_header *)(ioh_obj_ack_old_pkt.tx_buffer);
		ioh_obj_ack_old_pkt.tx_data=  (unsigned char *) (ioh_obj_ack_old_pkt.tx_buffer) + sizeof(struct inband_header);
		ioh_obj_ack_old_pkt.rx_header = (struct ioh_header *)(ioh_obj_ack_old_pkt.rx_buffer);
		ioh_obj_ack_old_pkt.rx_data=  (unsigned char *) (ioh_obj_ack_old_pkt.rx_buffer) + sizeof(struct inband_header);
		memcpy(ioh_obj_ack_old_pkt.dest_mac,ib_obj->rx_header->raw_header.sa,6);
		((struct inband_header*)ioh_obj_ack_old_pkt.tx_header)->rrcp_type= RRCP_P_IOH;	
		((struct inband_header*)ioh_obj_ack_old_pkt.tx_header)->inband_cmd= ib_obj->rx_header->inband_cmd;
		((struct inband_header*)ioh_obj_ack_old_pkt.tx_header)->inband_seq = ib_obj->rx_header->inband_seq;
		((struct inband_header*)ioh_obj_ack_old_pkt.tx_header)->inband_frag = ib_obj->rx_header->inband_frag;
		((struct inband_header*)ioh_obj_ack_old_pkt.tx_header)->inband_data_len = 0;
		return ioh_send(&ioh_obj_ack_old_pkt,sizeof(struct inband_header)); //only send header for ack	
	}
	return 0;
}

static int check_frag_ack(struct inband_class *ib_obj)
{
	int rx_len;	
	int RrtryCount=0;
	int old_pkt_flag=0;
RetryRcvAck:	
	rx_len = inband_recv(ib_obj, PER_FRAME_TIMEOUT); // -1 = wait until rec
	if (rx_len < 0)
	{
		perror("check_frag_ack fail,recv ack timeout:");
        syslog(LOG_DEBUG, "check frag ack error,recv ack timeout");
		return 0;
    }	
	if((ib_obj->rx_header->inband_cmd == ib_obj->tx_header->inband_cmd) &&
		(ib_obj->rx_header->inband_seq == ib_obj->tx_header->inband_seq) &&
		(ib_obj->rx_header->inband_frag == ib_obj->tx_header->inband_frag) &&
		(ib_obj->rx_header->inband_data_len == 0))
		return 1; //ok
	if((ib_obj->rx_header->inband_cmd != ib_obj->tx_header->inband_cmd) ||
		(ib_obj->rx_header->inband_seq != ib_obj->tx_header->inband_seq) ||
		(ib_obj->rx_header->inband_frag != ib_obj->tx_header->inband_frag) ||
		(ib_obj->rx_header->inband_data_len != 0)){
		old_pkt_flag = CheckReplayOldPkt(ib_obj);//check and replay ack to old packet 
		if(old_pkt_flag)
			goto RetryRcvAck;
		printf("Rcv worng ack ! rx_frag=%d tx_frag=%d rx_seq=%d tx_seq=%d \n",ntohs(ib_obj->rx_header->inband_frag),ntohs(ib_obj->tx_header->inband_frag),ntohs(ib_obj->rx_header->inband_seq),ntohs(ib_obj->tx_header->inband_seq));
		syslog(LOG_DEBUG, "Rcv worng ack ! rx_frag=%d tx_frag=%d tx_seq=%d\n",ntohs(ib_obj->rx_header->inband_frag),ntohs(ib_obj->tx_header->inband_frag),ntohs(ib_obj->rx_header->inband_seq),ntohs(ib_obj->tx_header->inband_seq));
		if(RrtryCount++ >= 1)
			return 0; //fail
		goto RetryRcvAck; // frag wrong
	}
	return 0; //fail
}
static char *inband_alloc_buf(struct inband_class *ib_obj)
{
	char *buf=NULL;
	unsigned int buf_size=MAX_APP_DATA_SIZE;	 
	unsigned char *image_len,header_len_offset=FM_HEADER_LEN_OFFSET;	 
#if 1 //it's for firmware file 	 
	if(ib_obj->rx_header->inband_cmd == id_firm_upgrade)
	{
#if 0
		//read firmware length from it's header
		image_len = (char *)ib_obj->rx_data;          
		buf_size =(unsigned int)( ( image_len[header_len_offset+0] <<24 ) +
				     ( image_len[header_len_offset+1] <<16 ) +
				     ( image_len[header_len_offset+2] <<8 )   +
				     ( image_len[header_len_offset+3] <<0 )   + 16 )	;						     	   

		//printf("inband_alloc_buf buf_size=%x , image_len[0]= %x\n",buf_size,image_len[header_len_offset+0]);	  
#endif
	 	buf_size = 0x800000; // using 8M for update firmware
	}    
#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
       if(ib_obj->rx_header->inband_cmd == id_get_file )
       {
	        buf_size = 0x100000; // using 1M for get log file
       }	       
#endif
	buf = (char *)malloc(buf_size);	 
	return buf;	
}
//mark_issue,defragment_reset
static int init_defragment_process(struct inband_class *ib_obj)
{
	struct fragment_info *p_frag_info = &ib_obj->frag_info;
	
	//check if the it is first fragment  id
	if(ib_obj->rx_header->inband_frag != FIRST_FRAG_ID)
		return ERROR_DEFRAGMENT;

	if((p_frag_info->buf = (char *)inband_alloc_buf(ib_obj)) == NULL)
  	{
		printf("init_defragment_process : data buffer allocation failed!\n");
		syslog(LOG_DEBUG, "init_defragment_process : data buffer allocation failed!\n");
		return ERROR_DEFRAGMENT;
	}
	p_frag_info->buf_ptr = p_frag_info->buf;
	p_frag_info->inband_frag = FIRST_FRAG_ID;
	p_frag_info->inband_cmd = ib_obj->rx_header->inband_cmd;
	p_frag_info->inband_seq = ntohs(ib_obj->rx_header->inband_seq);
	
	//copy first frame to buffer
	memcpy(p_frag_info->buf,ib_obj->rx_data,ntohs(ib_obj->rx_header->inband_data_len));
	p_frag_info->buf += ntohs(ib_obj->rx_header->inband_data_len);
	p_frag_info->data_len = ntohs(ib_obj->rx_header->inband_data_len);
	send_frag_ack(ib_obj);

	return 0; //init ok
}

static int do_defragment_process(struct inband_class *ib_obj)
{
	int ret=0;
	struct fragment_info *p_frag_info = &ib_obj->frag_info;

	if(p_frag_info->inband_cmd !=  ib_obj->rx_header->inband_cmd){
		printf("Rcv pkt cmd fail! cmd1=%d cmd2=%d\n",p_frag_info->inband_cmd, ib_obj->rx_header->inband_cmd);
		syslog(LOG_DEBUG, "Rcv pkt cmd fail! cmd1=%d cmd2=%d\n",p_frag_info->inband_cmd, ib_obj->rx_header->inband_cmd);
		return ERROR_DECMDTYPE;
	}	
	if(p_frag_info->inband_seq !=  ntohs(ib_obj->rx_header->inband_seq) ){
		printf("Rcv pkt seq fail! seq1=%d seq2=%d\n",p_frag_info->inband_seq, ntohs(ib_obj->rx_header->inband_seq));
		syslog(LOG_DEBUG, "Rcv pkt seq fail! seq1=%d seq2=%d\n",p_frag_info->inband_seq, ntohs(ib_obj->rx_header->inband_seq));
		return ERROR_DESEQTYPE;
	}
	if((ntohs(ib_obj->rx_header->inband_data_len)) == 0){
		printf("Rcv pkt len==0!\n");
		syslog(LOG_DEBUG, "Rcv pkt len==0!\n");
		return ERROR_DELENGHZERO;
	}
	if(p_frag_info->inband_frag == (ntohs(ib_obj->rx_header->inband_frag) & FRAG_ID_MASK)){
		printf("Rcv pkt Same frag! frag=%d \n",p_frag_info->inband_frag);
		syslog(LOG_DEBUG, "Rcv pkt Same frag! frag=%d \n",p_frag_info->inband_frag);
		return ret;
	}
	if( (p_frag_info->inband_frag+1) !=  (ntohs(ib_obj->rx_header->inband_frag) & FRAG_ID_MASK)){
		printf("Rcv pkt frag fail! seq1=%d seq2=%d\n",p_frag_info->inband_frag, (ntohs(ib_obj->rx_header->inband_frag) & FRAG_ID_MASK));
		syslog(LOG_DEBUG, "Rcv pkt frag fail! seq1=%d seq2=%d\n",p_frag_info->inband_frag, (ntohs(ib_obj->rx_header->inband_frag) & FRAG_ID_MASK));
		return ERROR_DEFRAGMENT;
	}
	else
		p_frag_info->inband_frag = ntohs(ib_obj->rx_header->inband_frag);

	memcpy(p_frag_info->buf,ib_obj->rx_data,ntohs(ib_obj->rx_header->inband_data_len));
	p_frag_info->buf += (ntohs(ib_obj->rx_header->inband_data_len));
	p_frag_info->data_len+= (ntohs(ib_obj->rx_header->inband_data_len));

	if( (p_frag_info->inband_frag & EOF_BIT) == EOF_BIT)
		ret = 1;  
	
	return ret; // return 1 means EOF rcv , return 0 means conitiune
}

static int get_defragment_info(struct inband_class *ib_obj,char *cmd_type,char **data)
{
	struct fragment_info *p_frag_info = &ib_obj->frag_info;
	
	*cmd_type = p_frag_info->inband_cmd;
	*data = p_frag_info->buf_ptr; //mark_issue,hwo to free the buffer ?
	return p_frag_info->data_len;
}

static int inband_rcv_fragment(struct inband_class *ib_obj,char *cmd_type,char **data)
{
	int rx_len,ret=0;		

	ret = init_defragment_process(ib_obj) ;
	int rerx_count = 0;

	if(ret < 0)
		return ret;	
	
	while( (inband_rcv_timeout!=1))
	{
	RetryRx:		
		rx_len = inband_recv(ib_obj, PER_FRAME_TIMEOUT);
		if (rx_len < 0){
			printf("Rcv pkt timeout! retry rx count=%d\n",rerx_count);
			syslog(LOG_DEBUG, "Rcv pkt timeout! retry rx count=%d\n",rerx_count);
			if(rerx_count++ < RETRY_TX_RX_COUNT)
				goto RetryRx;
			else	
				return ERROR_TIMOUT;
		}
		ret = do_defragment_process(ib_obj);
		if (ret < 0){
			if(ret == ERROR_DECMDTYPE || ret == ERROR_DESEQTYPE || ret == ERROR_DELENGHZERO)
				goto RetryRx;
			else	
				return ret;
		}
		send_frag_ack(ib_obj);
		if(ret == 1) //ret == 1 means defragment end , ret=0 means contiune			
		{
			ret = get_defragment_info(ib_obj,cmd_type,data);
			break;
		}	
		rerx_count = 0;
	}		
	return ret;
}

void inband_free_buf(char *data_buf,int data_len)
{
	//only free allocated buffer from deframet process.
	if( (data_buf != NULL) && ( data_len > MAX_INBAND_PAYLOAD_LEN ) )
		free(data_buf);
}

int inband_rcv_data(int chan,char *cmd_type,char **data,int timout_ms) //return data length
{	
	int rx_len,data_len=0;
	struct inband_class *inband_obj_p;
	struct ioh_class *ioh_obj_p;
	int retry_count = 0;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	
	ioh_obj_p = &inband_obj_p->ioh_obj;	

	//timout_ms will be used only for the first pkt. if the pkt is fragmented then every packet will
	//follow fragment_timout_ms
RetryRxFirstPkt:	
	rx_len = inband_recv(inband_obj_p, timout_ms); // -1 = wait until rec
	if (rx_len < 0)
	{
        syslog(LOG_DEBUG, "inband receive data error!");
		perror("inband_rcv_data:");
		return -1;
    }
	//printf("inband_rcv_data:\n");
	//hex_dump(ioh_obj_p->rx_buffer, ntohs(inband_obj_p->rx_header->inband_data_len) + sizeof(*inband_obj_p->rx_header)); //mark_test		

	//cache for tx dest mac
	if( memcmp(ioh_obj_p->dest_mac,ioh_obj_p->rx_header->sa,6)) //mark_test
		memcpy(ioh_obj_p->dest_mac,ioh_obj_p->rx_header->sa,6);
	
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
	if(ioh_obj_p->which_fd == 2) {
		*cmd_type = inband_obj_p->ioh_obj.inband_cmd;		
		*data = ioh_obj_p->rx_buffer;
        //printf("[%s:%d]ioh_obj.inband_cmd = %d \n", __FUNCTION__, __LINE__,  inband_obj_p->ioh_obj.inband_cmd);
		return inband_obj_p->ioh_obj.inband_data_len;
	}
#endif

	//check if the rx packet is mine,not other inband demon's rx packet ( in hcd the cmd is 0)
	if((inband_obj_p->frag_info.inband_cmd != 0) && (((inband_obj_p->rx_header->inband_cmd) & (~CMD_ERROR_REPLY_BIT)) != inband_obj_p->frag_info.inband_cmd))
	{
		//printf("Inband RetryRxFirstPkt cmd1=%d cmd2=%d!\n",inband_obj_p->rx_header->inband_cmd,inband_obj_p->frag_info.inband_cmd);
		syslog(LOG_DEBUG, "Inband RetryRxFirstPkt cmd1=%d cmd2=%d!\n",inband_obj_p->rx_header->inband_cmd,inband_obj_p->frag_info.inband_cmd);
		
		if(retry_count++ < RETRY_TX_RX_COUNT)
			goto RetryRxFirstPkt;
		else 
			return -1;
	}
	else if((inband_obj_p->frag_info.inband_cmd != 0) && (ntohs(inband_obj_p->rx_header->inband_seq) != inband_obj_p->frag_info.inband_seq))
	{
		printf("Inband RetryRxSignalPkt seq1=%d seq2=%d!\n",ntohs(inband_obj_p->rx_header->inband_seq),inband_obj_p->frag_info.inband_seq);
		syslog(LOG_DEBUG, "Inband RetryRxSignalPkt seq1=%d seq2=%d!\n",ntohs(inband_obj_p->rx_header->inband_seq),inband_obj_p->frag_info.inband_seq);
		if(retry_count++ < RETRY_TX_RX_COUNT)
			goto RetryRxFirstPkt;
		else 
			return -1;
	}
	//single pkt	
	if( inband_obj_p->rx_header->inband_frag  == ntohs(SINGLE_FRAME)) //mark_endian
	{		
        //printf("[%s:%d]inband_obj_p->rx_header->inband_cmd = %d \n", __FUNCTION__, __LINE__,inband_obj_p->rx_header->inband_cmd);
		*cmd_type = inband_obj_p->rx_header->inband_cmd;
		data_len =  ntohs(inband_obj_p->rx_header->inband_data_len);		
		*data = inband_obj_p->rx_data ; //or memcpy;
		send_frag_ack(inband_obj_p);
		if(data_len > rx_len){
			printf("Rcv more than one packet len1=%d len2=%d!",data_len,rx_len);
			syslog(LOG_DEBUG, "Rcv more than one packet len1=%d len2=%d!",data_len,rx_len);
        }
	}
	else //fragment process
		data_len = inband_rcv_fragment(inband_obj_p,cmd_type,data);

	return data_len;
	
}

int inband_rcv_indexed_data(int chan,char *cmd_type,char **data,int timout_ms,unsigned char idx) //return data length
{	
	int rx_len,data_len=0;
	struct inband_class *inband_obj_p;
	struct ioh_class *ioh_obj_p;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	
	ioh_obj_p = &inband_obj_p->ioh_obj;	

	
	//timout_ms will be used only for the first pkt. if the pkt is fragmented then every packet will
	//follow fragment_timout_ms
	//rx_len = inband_recv(inband_obj_p, timout_ms); // -1 = wait until rec
	rx_len = inband_recv(inband_obj_p, timout_ms); // -1 = wait until rec
	if (rx_len < 0)
	{
		perror("inband_rcv_data:");
		return -1;
    }
	//printf("inband_rcv_data:\n");
	//hex_dump(ioh_obj_p->rx_buffer, ntohs(inband_obj_p->rx_header->inband_data_len) + sizeof(*inband_obj_p->rx_header)); //mark_test		

	if( idx != inband_obj_p->rx_header->inband_index ) {
		printf("Error: if_index:%d != %d \n",idx,inband_obj_p->rx_header->inband_index);
		syslog(LOG_DEBUG, "Error: if_index:%d != %d \n",idx,inband_obj_p->rx_header->inband_index);
		return -1;
	}

	//cache for tx dest mac
	if( memcmp(ioh_obj_p->dest_mac,ioh_obj_p->rx_header->sa,6)) //mark_test
		memcpy(ioh_obj_p->dest_mac,ioh_obj_p->rx_header->sa,6);

	//single pkt	
	if( inband_obj_p->rx_header->inband_frag  == ntohs(SINGLE_FRAME)) //mark_endian
	{		
		*cmd_type = inband_obj_p->rx_header->inband_cmd;
		data_len =  ntohs(inband_obj_p->rx_header->inband_data_len);		
		*data = inband_obj_p->rx_data ; //or memcpy;	
	}
	else //fragment process
		data_len = inband_rcv_fragment(inband_obj_p,cmd_type,data);

	return data_len;
	
}


//if seq is need in your application
int inband_rcv_data_and_seq(int chan,unsigned int *seq,char *cmd_type,char **data,int timout_ms) //return data length
{
	struct inband_class *inband_obj_p;
	int ret=0;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	
	ret = inband_rcv_data(chan,cmd_type,data, timout_ms); //return data length
	if(ret < 0)
		return -1;

	*seq = ntohs(inband_obj_p->rx_header->inband_seq);
	return ret;
}

static int inband_send_data(struct inband_class *ib_obj,char *data,int data_len)
{	
	char *frag_ptr;
	unsigned short id=0,total_frag=0;
	unsigned int last_num;
	struct ioh_class *ioh_obj_p = &ib_obj->ioh_obj;
	int retx_count=0;

	total_frag = (unsigned short)(data_len / MAX_INBAND_PAYLOAD_LEN);
	last_num = data_len % MAX_INBAND_PAYLOAD_LEN;	
	if( total_frag > MAX_FRAG_ID)
		return -1;

	ib_obj->tx_header->inband_frag =0;
	frag_ptr = data;

	for(id=0;id<=total_frag;id++)
	{
		//if(total_frag > 0)
			//hex2bin("ffffffffffff", ioh_obj_p->dest_mac, ETH_MAC_LEN); 
       if(id != total_frag){
           ib_obj->tx_header->inband_data_len = htons(MAX_INBAND_PAYLOAD_LEN);
           ib_obj->tx_header->inband_frag = htons(id );
		   ib_obj->tx_header->inband_reserved &= (~RETRY_PACKET_MASK);
           memcpy(&ib_obj->tx_data[0], frag_ptr,MAX_INBAND_PAYLOAD_LEN );
       }
       else{//EOF fragment
           ib_obj->tx_header->inband_frag = id;
           ib_obj->tx_header->inband_frag |=EOF_BIT;
           ib_obj->tx_header->inband_frag = htons(ib_obj->tx_header->inband_frag);
           ib_obj->tx_header->inband_data_len = htons(last_num);
		   ib_obj->tx_header->inband_reserved &= (~RETRY_PACKET_MASK);
           if(last_num >0)
              memcpy(&ib_obj->tx_data[0], frag_ptr,last_num );
      }	
	RetryTX:	
		if( ioh_send(ioh_obj_p, sizeof(struct inband_header) + MAX_INBAND_PAYLOAD_LEN ) < 0){
			printf("Tx fail! \n");
			syslog(LOG_DEBUG, "Tx fail! \n");
			return -1;
		}
		//if(id>= 1){
		if(check_frag_ack(ib_obj) != 1){
			if(retx_count++ < RETRY_TX_RX_COUNT){
				ib_obj->tx_header->inband_reserved |= RETRY_PACKET_MASK;
				printf("Retry Tx pkt! seq=%d id=%d retx_count=%d \n",ib_obj->tx_header->inband_seq,id,retx_count);
				syslog(LOG_DEBUG, "Retry Tx pkt! seq=%d id=%d retx_count=%d\n",ib_obj->tx_header->inband_seq,id,retx_count);
				goto RetryTX;
			}
			else {	
				return -1;	
			}
		}
		//}	
		frag_ptr += MAX_INBAND_PAYLOAD_LEN;
		retx_count = 0;
	}
	last_num = data_len % MAX_INBAND_PAYLOAD_LEN;
	return 0;		
}

static int inband_send_indexed_data(struct inband_class *ib_obj,char *data, unsigned char idx, int data_len)
{	
	char *frag_ptr;
	unsigned short id=0,total_frag=0;
	unsigned int last_num;
	struct ioh_class *ioh_obj_p = &ib_obj->ioh_obj;

	total_frag = (unsigned short)(data_len / MAX_INBAND_PAYLOAD_LEN);
		
	if( total_frag > MAX_FRAG_ID)
		return -1;

	ib_obj->tx_header->inband_frag =0;
	frag_ptr = data;

	for(id=0;id<total_frag;id++)
	{
		ib_obj->tx_header->inband_data_len = htons(MAX_INBAND_PAYLOAD_LEN);
		ib_obj->tx_header->inband_frag = htons(id );
		memcpy(&ib_obj->tx_data[0], frag_ptr,MAX_INBAND_PAYLOAD_LEN );		
		if( ioh_send(ioh_obj_p, sizeof(struct inband_header) + MAX_INBAND_PAYLOAD_LEN ) < 0)
			return -1;
		//if(id>= 1){
		if(check_frag_ack(ib_obj) != 1)
			return -1;			
		//}	
		frag_ptr += MAX_INBAND_PAYLOAD_LEN;
	}
	last_num = data_len % MAX_INBAND_PAYLOAD_LEN;
	//EOF fragment	
	ib_obj->tx_header->inband_frag = id;
	ib_obj->tx_header->inband_frag |=EOF_BIT;
	ib_obj->tx_header->inband_frag = htons(ib_obj->tx_header->inband_frag);
	ib_obj->tx_header->inband_data_len = htons(last_num);
	if(last_num >0)
		memcpy(&ib_obj->tx_data[0], frag_ptr,last_num );

	ib_obj->tx_header->inband_index = idx;

	return ioh_send(ioh_obj_p,sizeof(struct inband_header)+last_num);		
}


int inband_write(int chan,unsigned int seq,char cmd,char *data,int data_len,int reply)
{
	struct inband_class *inband_obj_p;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	

	inband_obj_p->tx_header->rrcp_type = RRCP_P_IOH;  //mark_inband
	//fill inband header , cmd
	inband_obj_p->tx_header->inband_cmd = cmd;

	//reply = 0(request) ,reply = 1(good reply),reply = 2(bad reply)
	if(reply == 2)
		inband_obj_p->tx_header->inband_cmd |= CMD_ERROR_REPLY_BIT;

	//fill inband header , seq
	if(!reply) 
		inband_obj_p->tx_header->inband_seq = htons(seq);
	else //seq is not used when the packet is for reply
		inband_obj_p->tx_header->inband_seq = inband_obj_p->rx_header->inband_seq;	


	//fill data, data_len , and send
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
    // for mobile domain socket access
	if((inband_obj_p->ioh_obj).which_fd == 2){
		pRTK_INBAND_RESULT inband_result = (pRTK_INBAND_RESULT)malloc(sizeof(RTK_INBAND_RESULT));
		if(inband_result == NULL){
			printf("malloc inband result error!\n");
			syslog(LOG_DEBUG, "malloc inband result error!\n");
			return -1;
		}
        memset((inband_result->header).error_info, 0x0, 64);
		if(reply==1)
			(inband_result->header).status = 0;	//succeed
		else if(reply==2)
			(inband_result->header).status = 1;	//failed
		(inband_result->header).length = data_len;
		inband_result->buf = data;
        (inband_result->header).thread_id = (inband_obj_p->ioh_obj).thread_id;
        //printf("thread id to reply is %d\n", (inband_result->header).thread_id);
        (inband_result->header).cmd_type = (inband_obj_p->ioh_obj).inband_cmd;
        //printf("cmd type id to reply is %d\n", (inband_result->header).cmd_type);
		
		#if defined(CLOUD_INBAND_DEBUG_FLAG)
		int i;
		printf("inband result: %d\n", data_len+sizeof(RTK_INBAND_RESULT_HEADER));
		for(i=0; i<data_len; i++) {
			printf("%02x ", inband_result->buf[i] & 0xFF);
			if((i%16 == 0) && (i!=0))
				printf("\n");
		}
		printf("\n");
		#endif

		char send_buf[BUF_LEN_MAX];	
		memset(send_buf,0x0, BUF_LEN_MAX);
		int written = 0;
		// /TODO: currently support local only!
		int magic_num = htonl((inband_obj_p->ioh_obj).local_magic);
		//printf("magic_num: %x \n", magic_num);
		memcpy(send_buf + written, &magic_num, 4);
		written += 4;
			
		int app_tag = htonl(CLOUD_RTK_INBAND_TAG);
		memcpy(send_buf + written, &app_tag, 4);
		written += 4;
				
		int app_len = htonl(data_len+sizeof(RTK_INBAND_RESULT_HEADER) + 4*4 + 1);
		memcpy(send_buf + written, &app_len, 4);
		written += 4;

		written += 4; //ignore local tag!

		int local_len = htonl(1);
		memcpy(send_buf + written, &local_len, 4);
		written += 4;

		written += 1; // ignore local value

		int inner_tag = htonl(CLOUD_INBAND_CMD);
		memcpy(send_buf + written, &inner_tag, 4);
		written += 4;

		int inner_len = htonl(data_len+sizeof(RTK_INBAND_RESULT_HEADER));
		memcpy(send_buf + written, &inner_len, 4);
		written += 4;

        // actual payload
		memcpy(send_buf + written, &(inband_result->header.thread_id), 4);
		written += 4;
		memcpy(send_buf + written, &(inband_result->header.cmd_type), 4);
		written += 4;
		memcpy(send_buf + written, &(inband_result->header.status), 4);
		written += 4;
		memcpy(send_buf + written, &(inband_result->header.length), 4);
		written += 4;
		memcpy(send_buf + written, inband_result->header.error_info, 64);
		written += 64;
		memcpy(send_buf + written, inband_result->buf, data_len);
		written += data_len;

		int total_send_len = written;

		//#if defined(CLOUD_INBAND_DEBUG_FLAG)
        #if 0
		printf("[%s %d] total_send_len = %d \n", __FUNCTION__, __LINE__, total_send_len);
		int dbg = total_send_len - (data_len+sizeof(RTK_INBAND_RESULT_HEADER)) ;
		for(; dbg<total_send_len; dbg++){
			printf("%02x ", send_buf[dbg]);
			if((dbg%16 == 0) && (dbg!=0))
				printf("\n");
		}
		printf("\n");
		#endif
		
		return rtl_domainSocketWrite((inband_obj_p->ioh_obj).ds_fd, send_buf,total_send_len);
	} else {
		//printf("inband_obj_p->ioh_obj.which_fd = %d\n", inband_obj_p->ioh_obj.which_fd);
	}
#endif

#if defined(CONFIG_REPORT_ERROR)
    // for inband access
    int written = 0;
    int status;
    int length;
    int total_len;
    if(reply==1){       //good reply
        status = 0;
        length = data_len;
        written = 0;
        total_len = 4+4+64+length;
        char result[total_len];
        memset(result, 0x0, total_len);

        memcpy(result+written, &status, 4);
        written += 4;
        memcpy(result+written, &length, 4);
        written += 4 + 64; //64 bytes error_info is empty now.

        memcpy(result+written, data, data_len);
        written += data_len;

        return inband_send_data(inband_obj_p, result,total_len);
    }else if(reply==2){
        status = 1;
        length = 4;
        written = 0;
        total_len = 4+4+64+length;
        char result[total_len];
        memset(result, 0x0, total_len);

        memcpy(result+written, &status, 4);
        written += 4;
        memcpy(result+written, &length, 4);
        written += 4;
        memcpy(result+written, data, (data_len>=64)?64:data_len); //max 64 bytes
        written += 64;

        memcpy(result+written, 0x0, 4);
        written += 4;

        return inband_send_data(inband_obj_p, result,total_len);
    }
#endif
	return inband_send_data(inband_obj_p,data,data_len);
}

int inband_indexed_write(int chan,unsigned int seq,char cmd,char *data,int data_len,int reply,unsigned char idx)
{
	struct inband_class *inband_obj_p;

	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	

	inband_obj_p->tx_header->rrcp_type = RRCP_P_IOH;  //mark_inband
	//fill inband header , cmd
	inband_obj_p->tx_header->inband_cmd = cmd;

	//reply = 0(request) ,reply = 1(good reply),reply = 2(bad reply)
	if(reply == 2)
		inband_obj_p->tx_header->inband_cmd |= CMD_ERROR_REPLY_BIT;

	//fill inband header , seq
	if(!reply) 
		inband_obj_p->tx_header->inband_seq = htons(seq);
	else //seq is not used when the packet is for reply
		inband_obj_p->tx_header->inband_seq = inband_obj_p->rx_header->inband_seq;	

	inband_obj_p->tx_header->inband_index = idx;
	
	//fill data, data_len , and send
	return inband_send_indexed_data(inband_obj_p,data,idx,data_len);
}

void inband_set_cmd_id_zero(int chan)
{
	struct inband_class *inband_obj_p;
	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	
	if(inband_obj_p != NULL)
		inband_obj_p->frag_info.inband_cmd = 0;
}

void inband_set_cmd_seq(int chan,int cmd_id,int seq)
{
	struct inband_class *inband_obj_p;
	struct ioh_class *ioh_obj_p;
	inband_obj_p = (struct inband_class *)get_chan_obj(chan);	
	inband_obj_p->frag_info.inband_cmd = cmd_id;
	inband_obj_p->frag_info.inband_seq = seq;
}
