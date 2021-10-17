#ifndef INCLUDE_INBAND_IF_H
#define INCLUDE_INBAND_IF_H

#define MAX_APP_DATA_SIZE 65535
#define CMD_ERROR_REPLY_BIT 0x80
#define MAX_INBAND_PAYLOAD_LEN 1480
#define MAX_FRAG_ID (0x8000 -1)
#define FRAG_ID_MASK (~(0x8000))

#define EOF_BIT 0x8000
#define SINGLE_FRAME 0x8000
#define FIRST_FRAG_ID 0x0000
#define PER_FRAME_TIMEOUT 2000 //ms

#define ERROR_TIMOUT -1
#define ERROR_DEFRAGMENT -2
#define ERROR_DECMDTYPE -3
#define ERROR_DESEQTYPE -4
#define ERROR_DELENGHZERO -5

#define RRCP_P_IOH		0x41	// RRCP IOH

#define RETRY_TX_RX_COUNT 5 //retry count
#define RETRY_PACKET_MASK 0x80//retry packet mask in field inband_reserved
//mark_fm
#define id_firm_upgrade						0x20
#ifndef INBAND_GET_FILE_SUPPOPRT
#define INBAND_GET_FILE_SUPPOPRT
#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
#define id_get_file			0x0e
#endif
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
#include "../cloud/ipc/cloud_ipc.h"
#include "../cloud/ipc/cloud_inband_app.h"
#include "../cloud/ipc/local_access.h"
#include "../hcm/hcd/cmd.h"
#endif
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

int inband_open(char *netif_name,char *slave_mac,unsigned short eth_type,int debug);
void inband_close(int chan);
void inband_free_buf(char *data_buf,int data_len);
int inband_rcv_data(int chan,char *cmd_type,char **data,int timout_ms); //return data length
int inband_rcv_indexed_data(int chan,char *cmd_type,char **data,int timout_ms,unsigned char idx); //return data length
int inband_rcv_data_and_seq(int chan,unsigned int *seq,char *cmd_type,char **data,int timout_ms); //return data length
int inband_write(int chan,unsigned int seq,char cmd,char *data,int data_len,int reply);
int inband_indexed_write(int chan,unsigned int seq,char cmd,char *data,int data_len,int reply,unsigned char idx);

#endif
