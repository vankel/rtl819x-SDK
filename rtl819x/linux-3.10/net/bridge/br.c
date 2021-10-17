/*
 *	Generic parts
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/llc.h>
#include <net/llc.h>
#include <net/stp.h>

#include "br_private.h"

#if defined (CONFIG_RTL865X_LANPORT_RESTRICTION)
#include <net/rtl/features/lan_restrict.h>
#endif

#if 0//LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
#define CONFIG_RTL_PROC_NEW		1
#endif

#if defined(CONFIG_RTL_819X)&&defined(CONFIG_PROC_FS)&&defined(CONFIG_BRIDGE_IGMP_SNOOPING)
#include <net/rtl/rtl_types.h>
#include <net/rtl/rtl865x_multicast.h>
#include <net/rtl/rtl865x_netif.h>
#include <net/rtl/rtl_nic.h>
extern struct net_bridge *bridge0;
struct proc_dir_entry *procIgmpVersion=NULL;
int igmpVersion=2;
struct proc_dir_entry *procIgmpSnoop=NULL;
#endif

#if defined (CONFIG_RTL_IGMP_SNOOPING)
#include <linux/inetdevice.h>
#include <net/rtl/rtl865x_igmpsnooping_glue.h>
#include <net/rtl/rtl865x_igmpsnooping.h>
#include <net/rtl/rtl865x_multicast.h>
#include <net/rtl/rtl865x_netif.h>
#include <net/rtl/rtl_nic.h>
#endif
int IGMPProxyOpened = 0;

static const struct stp_proto br_stp_proto = {
	.rcv	= br_stp_rcv,
};

static struct pernet_operations br_net_ops = {
	.exit	= br_net_exit,
};
#if defined (CONFIG_RTL_IGMP_SNOOPING)
struct proc_dir_entry *procIgmpSnoop=NULL;
int igmpsnoopenabled=0;	// Should be 0(default), set 1 when igmpproxy up!
extern struct net_bridge *bridge0;
extern int32 rtl_configMulticastSnoopingFastLeave(int enableFastLeave, int ageTime);

extern unsigned int maxUnknownMcastPPS;
extern unsigned int chkUnknownMcastEnable;

extern struct rtl865x_ReservedMCastRecord reservedMCastRecord[MAX_RESERVED_MULTICAST_NUM];

#ifdef CONFIG_RTL_PROC_NEW
static int br_igmpSnoopRead(struct seq_file *s, void *v)
#else
static int br_igmpSnoopRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
    int i,j,k;
#if defined (CONFIG_RTL8196C_REVISION_B) || defined (CONFIG_RTL8198_REVISION_B) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) ||defined(CONFIG_RTL_8198C)
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
    unsigned int currHashMethod; 
#endif
#endif
	int cnt;
	

#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s, "igmpsnoopenabled:%c\n\n",igmpsnoopenabled + '0');
#else
	int len;
    len = sprintf(page, "igmpsnoopenabled:%c\n\n", igmpsnoopenabled + '0');
#endif
#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s,  "Block Info :%d,%d\n",chkUnknownMcastEnable,maxUnknownMcastPPS); 
	seq_printf(s,  "Reserved multicast address:\n");	
	for(i=0; i<MAX_RESERVED_MULTICAST_NUM; i++)
	{
		if(reservedMCastRecord[i].valid==1)
		{		
			
			cnt++;
			seq_printf(s, "    [%d] Group address:%d.%d.%d.%d\n",cnt,
			reservedMCastRecord[i].groupAddr>>24, (reservedMCastRecord[i].groupAddr&0x00ff0000)>>16,
			(reservedMCastRecord[i].groupAddr&0x0000ff00)>>8, (reservedMCastRecord[i].groupAddr&0xff));
		}
	}
	if(cnt==0)
		seq_printf(s, "NULL\n");
	seq_printf(s, "\n");
#else
	len += sprintf(page+len, "Block Info :%d,%d\n",chkUnknownMcastEnable,maxUnknownMcastPPS); 
	len += sprintf(page+len, "Reserved multicast address:\n");	
	for(i=0; i<MAX_RESERVED_MULTICAST_NUM; i++)
	{
		if(reservedMCastRecord[i].valid==1)
		{		
			
			cnt++;
			len += sprintf(page+len, "    [%d] Group address:%d.%d.%d.%d\n",cnt,
			reservedMCastRecord[i].groupAddr>>24, (reservedMCastRecord[i].groupAddr&0x00ff0000)>>16,
			(reservedMCastRecord[i].groupAddr&0x0000ff00)>>8, (reservedMCastRecord[i].groupAddr&0xff));
		}
	}
	if(cnt==0)
		len += sprintf(page+len, "NULL\n");
	len += sprintf(page+len, "\n");
#endif

#if defined (CONFIG_RTL8196C_REVISION_B) || defined (CONFIG_RTL8198_REVISION_B) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)  ||defined(CONFIG_RTL_8198C)
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
    rtl865x_getMCastHashMethod(&currHashMethod);
    if((currHashMethod==0) || (currHashMethod==1))
    { 
#ifdef CONFIG_RTL_PROC_NEW
		seq_printf(s,"hash method:sip&dip \n\n");
#else
        len += sprintf(page+len, "hash method:sip&dip\n\n");
#endif
    }
    else if (currHashMethod==2)
    {
#ifdef CONFIG_RTL_PROC_NEW
		seq_printf(s,"hash method:sip[6:0] \n\n");
#else
        len += sprintf(page+len, "hash method:sip[6:0]\n\n");
#endif
    }
    else if (currHashMethod==3)
    {
#ifdef CONFIG_RTL_PROC_NEW
		seq_printf(s,"hash method:dip[6:0] \n\n");
#else
        len += sprintf(page+len, "hash method:dip[6:0]\n\n");
#endif
    }
#endif
#endif

#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s,"bridge multicast fdb:\n");
#else
    len += sprintf(page+len, "bridge multicast fdb:\n");
#endif
	for (i = 0; i < BR_HASH_SIZE; i++) {
		struct net_bridge_fdb_entry *f;
		struct hlist_node *n; //, *h;
		
		j=0;
		hlist_for_each_entry_safe(f, n, &bridge0->hash[i], hlist) {
			if(MULTICAST_MAC(f->addr.addr) )
			{
#ifdef CONFIG_RTL_PROC_NEW
				seq_printf(s,"[%d][%d]mCastMac:0x%x:%x:%x:%x:%x:%x,use_count is %lu,ageing_timer is %lu\n",
				i,j,f->addr.addr[0],f->addr.addr[1],f->addr.addr[2],f->addr.addr[3],f->addr.addr[4],f->addr.addr[5],f->used,(jiffies<f->updated) ?0:(jiffies-f->updated));
#else
				len += sprintf(page+len,"[%d][%d]mCastMac:0x%x:%x:%x:%x:%x:%x,use_count is %lu,ageing_timer is %lu\n",
					i,j,f->addr.addr[0],f->addr.addr[1],f->addr.addr[2],f->addr.addr[3],f->addr.addr[4],f->addr.addr[5],f->used,(jiffies<f->updated) ?0:(jiffies-f->updated));
#endif				
				for(k=0;k<FDB_IGMP_EXT_NUM;k++)
				{
					if(f->igmp_fdb_arr[k].valid)
					{
#ifdef CONFIG_RTL_PROC_NEW
						seq_printf(s,"\t<%d>clientMac:0x%x:%x:%x:%x:%x:%x,ageing_time:%lu\n",
						k,f->igmp_fdb_arr[k].SrcMac[0],f->igmp_fdb_arr[k].SrcMac[1],f->igmp_fdb_arr[k].SrcMac[2],f->igmp_fdb_arr[k].SrcMac[3],f->igmp_fdb_arr[k].SrcMac[4],f->igmp_fdb_arr[k].SrcMac[5],(jiffies<f->igmp_fdb_arr[k].ageing_time) ?0:(jiffies-f->igmp_fdb_arr[k].ageing_time));
#else
						len += sprintf(page+len,"\t<%d>clientMac:0x%x:%x:%x:%x:%x:%x,ageing_time:%lu\n",
						k,f->igmp_fdb_arr[k].SrcMac[0],f->igmp_fdb_arr[k].SrcMac[1],f->igmp_fdb_arr[k].SrcMac[2],f->igmp_fdb_arr[k].SrcMac[3],f->igmp_fdb_arr[k].SrcMac[4],f->igmp_fdb_arr[k].SrcMac[5],(jiffies<f->igmp_fdb_arr[k].ageing_time) ?0:(jiffies-f->igmp_fdb_arr[k].ageing_time));
#endif
					}
				}

				j++;
#ifdef CONFIG_RTL_PROC_NEW
				seq_printf(s,"--------------------------------------------\n");
#else
				len += sprintf(page+len,"--------------------------------------------\n");
#endif
			}

		}
		
	}
#ifdef CONFIG_RTL_PROC_NEW
	return 0;
#else	
    if (len <= off+count) *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count) len = count;
    if (len<0) len = 0;
    return len;
#endif
}

static int br_igmpSnoopWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	//unsigned char br_tmp;
	char        tmpbuf[512];
	char        *strptr = NULL;
	char        *tokptr = NULL;
#if defined (CONFIG_RTL8196C_REVISION_B) || defined (CONFIG_RTL8198_REVISION_B) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) ||defined(CONFIG_RTL_8198C)        
	unsigned int newHashMethod=0;
#endif
	uint32  ipAddr[4];	  
	uint32 groupAddr;
	int flag;
	int cnt;
	int fastLeave=0;
	int ageTime=0;
      
      if (count < 2) 
	    return -EFAULT;
      
      if (buffer && !copy_from_user(tmpbuf, buffer, count)) {
          tmpbuf[count] = '\0';
          strptr = tmpbuf;
          
		tokptr = strsep(&strptr," ");
          
          if (tokptr != NULL) 
          {
			if(!memcmp(tokptr,"enable",6))
			{
				tokptr = strsep(&strptr," ");
				if (tokptr==NULL)
				{
					return -EFAULT;
				}
				igmpsnoopenabled = simple_strtol(tokptr, NULL, 0);
			}
			else if(!memcmp(tokptr, "fastleave", 9))
			{
				tokptr = strsep(&strptr," ");
				if (tokptr==NULL)
				{
					return -EFAULT;
				}
				
				fastLeave = simple_strtol(tokptr, NULL, 0);

				if(fastLeave)
				{
					fastLeave=1;
				}
				
				tokptr = strsep(&strptr," ");
				if (tokptr==NULL)
				{
					return -EFAULT;
				}
				
				ageTime = simple_strtol(tokptr, NULL, 0);
				rtl_configMulticastSnoopingFastLeave(fastLeave,ageTime);
			}
			else if(!memcmp(tokptr,"block",5))
			{
				tokptr = strsep(&strptr," ");
				if (tokptr==NULL)
				{
					return -EFAULT;
				}
				
				chkUnknownMcastEnable = simple_strtol(tokptr, NULL, 0);

				if(chkUnknownMcastEnable)
				{
					chkUnknownMcastEnable=1;
				}
				
				tokptr = strsep(&strptr," ");
				if (tokptr==NULL)
				{
					return -EFAULT;
				}
				
				maxUnknownMcastPPS = simple_strtol(tokptr, NULL, 0);
			}
			else if(!memcmp(tokptr, "reserve", 7)) 
			{
				
				tokptr = strsep(&strptr," ");
				if (tokptr==NULL)
				{
					return -EFAULT;
				}
				if(	(!memcmp(tokptr, "add", 3)) 
				|| (!memcmp(tokptr, "Add", 3))
				||(!memcmp(tokptr, "ADD", 3)))	
				{
					flag = 1;
				}
				else if((!memcmp(tokptr, "del", 3)) 
				|| (!memcmp(tokptr, "Del", 3))
				||(!memcmp(tokptr, "DEL", 3)))	
				{
					flag = 0;
				}
				
				tokptr = strsep(&strptr," ");
				if (tokptr==NULL)
				{
					return -EFAULT;
				}
				cnt = sscanf(tokptr, "%d.%d.%d.%d", &ipAddr[0], &ipAddr[1], &ipAddr[2], &ipAddr[3]);

				groupAddr=(ipAddr[0]<<24)|(ipAddr[1]<<16)|(ipAddr[2]<<8)|(ipAddr[3]);
				rtl_add_ReservedMCastAddr(groupAddr,flag);
			}
#if defined (CONFIG_RTL8196C_REVISION_B) || defined (CONFIG_RTL8198_REVISION_B) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198C)
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)  
			else if(!memcmp(tokptr, "hash", 4)) 
			{
				tokptr = strsep(&strptr," ");
				if (tokptr!=NULL )
				{
					newHashMethod = simple_strtol(tokptr, NULL, 0);
					if(newHashMethod >3)
					{
						return -1;
					}
					
					rtl865x_setMCastHashMethod(newHashMethod);

				}
			}	
#endif           
#endif		  	          
			else
			{	         			
				igmpsnoopenabled = simple_strtol(tokptr, NULL, 0);
			}  
			if(igmpsnoopenabled)
			{
				igmpsnoopenabled=1;
			}
			else
			{
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)		
				rtl865x_reinitMulticast();
#endif
				rtl_flushAllIgmpRecord();
			}
		}
	    return count;
      }
      return -EFAULT;
}

static struct proc_dir_entry *procIgmpDb;
extern int igmp_show(struct seq_file *s, void *v);
extern int igmp_write(struct file *file, const char __user *buffer, size_t count, loff_t *data);
int igmp_db_open(struct inode *inode, struct file *file)
{
        return(single_open(file, igmp_show, NULL));
}

int igmp_db_write(struct file *file, const char __user *buffer, size_t count, loff_t *data)
{
         igmp_write(file, buffer, count,data);
}

struct file_operations igmp_db_seq_file_operations = {
        .open           = igmp_db_open,
        .read           = seq_read,
        .write		= igmp_db_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};

struct proc_dir_entry *procMCastFastFwd=NULL;
int ipMulticastFastFwd=0;
int needCheckMfc=0;
#ifdef CONFIG_RTL_PROC_NEW
static int br_mCastFastFwdRead(struct seq_file *s, void *v)
#else
static int br_mCastFastFwdRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s,"%c,%c\n",ipMulticastFastFwd + '0',needCheckMfc + '0');
	return 0;
#else
      int len;
    len = sprintf(page, "%c,%c\n", ipMulticastFastFwd + '0',needCheckMfc + '0');

    if (len <= off+count) *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count) len = count;
    if (len<0) len = 0;
    return len;
#endif
}

static int br_mCastFastFwdWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	unsigned int tmp=0; 
	char 		tmpbuf[512];
	char		*strptr;
	char		*tokptr;
	

	if (count < 2) 
		return -EFAULT;

	if (buffer && !copy_from_user(tmpbuf, buffer, count)) 
	{
		tmpbuf[count] = '\0';

		strptr=tmpbuf;

		
		tokptr = strsep(&strptr,",");
		if (tokptr==NULL)
		{
			tmp=simple_strtol(strptr, NULL, 0);
			printk("tmp=%d\n",tmp);
			if(tmp==0)
			{
				ipMulticastFastFwd=0;
			}
			return -EFAULT;
		}
		
		ipMulticastFastFwd = simple_strtol(tokptr, NULL, 0);
		//printk("ipMulticastFastFwd=%d\n",ipMulticastFastFwd);
		if(ipMulticastFastFwd)
		{
			ipMulticastFastFwd=1;
		}

		tokptr = strsep(&strptr,",");
		if (tokptr==NULL)
		{
			return -EFAULT;
		}
		
		needCheckMfc = simple_strtol(tokptr, NULL, 0);

		if(needCheckMfc)
		{
			needCheckMfc=1;
		}

		return count;
	}
	return -EFAULT;
}

struct proc_dir_entry *procIgmpQuery=NULL;
int igmpQueryEnabled=0;	
#ifdef CONFIG_RTL_PROC_NEW
static int br_igmpQueryRead(struct seq_file *s, void *v)
#else
static int br_igmpQueryRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif		
{
#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s,"%c\n",igmpQueryEnabled + '0');
	return 0;
#else
	int len;
    len = sprintf(page, "%c\n", igmpQueryEnabled + '0');
    if (len <= off+count) *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count) len = count;
    if (len<0) len = 0;
    return len;
#endif
}

static int br_igmpQueryWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
      unsigned char tmp; 
      if (count < 2) 
	    return -EFAULT;
      
	if (buffer && !copy_from_user(&tmp, buffer, 1)) {
		igmpQueryEnabled = tmp - '0';
		if(igmpQueryEnabled)
		{
			igmpQueryEnabled=1;
		}
		else
		{
			igmpQueryEnabled=0;
		}
	    return count;
      }
      return -EFAULT;
}


#if defined (CONFIG_RTL_QUERIER_SELECTION)

#define MAX_QUERIER_RECORD 8
#define QUERIER_EXPIRED_TIME 255

struct querierInfo
{
	unsigned int version;
	unsigned char devName[IFNAMSIZ];
	unsigned int querierIp[4];
	unsigned int lastJiffies;
};

struct querierInfo querierInfoList[MAX_QUERIER_RECORD];

int br_initQuerierInfo(void)
{

	memset(querierInfoList, 0, sizeof(querierInfoList));
	return 0;
	
}

int br_updateQuerierInfo(unsigned int version, unsigned char *devName, unsigned int* querierIp)
{
	int i;
	unsigned long oldestJiffies;
	unsigned long oldestIdx=0;
	
	if(querierIp==NULL)
	{
		return -1;	
	}
	
	if((version!=4) && (version!=6))
	{
		return -1;	
	}
	
	/*find matched one*/
	for(i=0; i<MAX_QUERIER_RECORD; i++)
	{
		if((querierInfoList[i].version==version))
		{
			if( (version==4) && (querierInfoList[i].querierIp[0]==querierIp[0]))
			{
				strcpy(querierInfoList[i].devName,devName);
				querierInfoList[i].lastJiffies=jiffies;
				return 0;
	
			}

			if((version ==6) && (memcmp(querierInfoList[i].querierIp, querierIp, 16) ==0)) 
			{
				strcpy(querierInfoList[i].devName,devName);
				querierInfoList[i].lastJiffies=jiffies;
				return 0;
			}
		}
	
	}
	
	/*no matched one, find an empty one*/
	for(i=0; i<MAX_QUERIER_RECORD; i++)
	{
		if(querierInfoList[i].version==0)
		{
			querierInfoList[i].version=version;
			if(version==4)
			{
				querierInfoList[i].querierIp[0]=querierIp[0];
			}
			else if (version ==6)
			{
				memcpy(querierInfoList[i].querierIp, querierIp, 16);
			}
			strcpy(querierInfoList[i].devName,devName);
			querierInfoList[i].lastJiffies=jiffies;
			
			return 0;
		}
	}
	
	/*all entries are used, find oldest one*/
	oldestJiffies=querierInfoList[0].lastJiffies;
	oldestIdx=0;
	for(i=0; i<MAX_QUERIER_RECORD; i++)
	{
		if(time_before((unsigned long)(querierInfoList[i].lastJiffies),oldestJiffies))
		{
			oldestJiffies=querierInfoList[i].lastJiffies;
			oldestIdx=i;
		}
	}
	
	querierInfoList[oldestIdx].version=version;
	if(version==4)
	{
		querierInfoList[oldestIdx].querierIp[0]=querierIp[0];
	}
	else if (version ==6)
	{
		memcpy(querierInfoList[oldestIdx].querierIp, querierIp, 16);
	}
	
	strcpy(querierInfoList[oldestIdx].devName,devName);
	querierInfoList[oldestIdx].lastJiffies=jiffies;
	
	return 0;

	
}

int br_querierSelection(struct net_bridge *br,unsigned int ipVer)	
{
	int i;
	int ret=1;
	struct net_device* brDev = NULL;
	struct in_device *in_dev;	
	struct net_device *landev;
	struct in_ifaddr *ifap = NULL;
	unsigned int brIpAddr=0;
	unsigned char brIpv6Addr[16]={	0xfe,0x80,0x00,0x00,				/*source address*/
									0x00,0x00,0x00,0x00,				/*be zero*/	
									0x00,0x00,0x00,					/*upper 3 bytes mac address |0x02*/ 
									0xff,0xfe,						/*fixed*/
									0x00,0x00,0x00					/*lowert 3 bytes mac address*/	};

	
	if(br==NULL)
	{
		return 1;
	}
	
	brDev = br->dev;

	if(ipVer==4)
	{
		/*get bridge ip address*/
		if ((landev = __dev_get_by_name(&init_net, RTL_PS_BR0_DEV_NAME)) != NULL){
			in_dev=(struct in_device *)(landev->ip_ptr);
			if (in_dev != NULL) {
				for (ifap=in_dev->ifa_list; ifap != NULL; ifap=ifap->ifa_next) {
					if (strcmp(RTL_PS_BR0_DEV_NAME, ifap->ifa_label) == 0)
					{
							memcpy(&brIpAddr,&ifap->ifa_address,4);
					}
				}
				
			}
		}

		for(i=0; i<MAX_QUERIER_RECORD; i++)
		{
			if(	(querierInfoList[i].version==4)&&
				time_after((unsigned long)(querierInfoList[i].lastJiffies+QUERIER_EXPIRED_TIME*HZ), jiffies)&&
				(querierInfoList[i].querierIp[0] < brIpAddr))
			{
				ret=0;
			}
		}
		
	}
	else if (ipVer==6)
	{
	
		memcpy(&brIpv6Addr[8],brDev->dev_addr,3);		/*generate br link-local ipv6 address*/
		brIpv6Addr[8]=brIpv6Addr[8]|0x02;		
		memcpy(&brIpv6Addr[13],&brDev->dev_addr[3],3);	
		#if 0
		printk("br0 ipv6 address is:\n");

		{
			int j;
			for(j=0; j<16; j++)	
			{
				printk("%x",brIpv6Addr[j]);
				if((j!=0) &&(j%4==0))
				{
					printk("-");
				}
			}
			printk("\n");
		}
		#endif
		
		for(i=0; i<MAX_QUERIER_RECORD; i++)
		{
			if(	(querierInfoList[i].version==6)&&
				time_after((unsigned long)(querierInfoList[i].lastJiffies+QUERIER_EXPIRED_TIME*HZ),jiffies))
			{
				if(memcmp(querierInfoList[i].querierIp, brIpv6Addr, 16)<0)
				{
					ret=0;
				}
			}
		}
		
		
	}
	
	return ret;
}


struct proc_dir_entry *procIgmpQuerierInfo=NULL;

#ifdef CONFIG_RTL_PROC_NEW
static int br_igmpQuerierInfoRead(struct seq_file *s, void *v)
#else
static int br_igmpQuerierInfoRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
#ifndef CONFIG_RTL_PROC_NEW
	int len=0;
#endif
	int i;
	unsigned long elapseJiffies;
	for(i=0; i<MAX_QUERIER_RECORD; i++)
	{
		if((querierInfoList[i].version==4) && time_after((unsigned long)(querierInfoList[i].lastJiffies+QUERIER_EXPIRED_TIME*HZ),jiffies))
		{
			if(jiffies > querierInfoList[i].lastJiffies)
			{
				elapseJiffies=jiffies-querierInfoList[i].lastJiffies;
			}
			else
			{
				elapseJiffies=jiffies+((unsigned long)0xFFFFFFFF-querierInfoList[i].lastJiffies)+1;
			}
#ifdef CONFIG_RTL_PROC_NEW
			seq_printf(s, "%s %d.%d.%d.%d %lu\n",
				querierInfoList[i].devName,
				((querierInfoList[i].querierIp[0]>>24)&0xFF) ,
				((querierInfoList[i].querierIp[0]>>16)&0xFF),
				((querierInfoList[i].querierIp[0]>>8)&0xFF),
				(querierInfoList[i].querierIp[0]&0xFF),elapseJiffies/HZ);
#else
			len += sprintf(page+len, "%s %d.%d.%d.%d %u\n",
				querierInfoList[i].devName,
				((querierInfoList[i].querierIp[0]>>24)&0xFF) ,
				((querierInfoList[i].querierIp[0]>>16)&0xFF),
				((querierInfoList[i].querierIp[0]>>8)&0xFF),
				(querierInfoList[i].querierIp[0]&0xFF),elapseJiffies/HZ);
#endif
		}
	}
#ifdef CONFIG_RTL_PROC_NEW
	return 0;
#else	
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;
#endif
}

static int br_igmpQuerierInfoWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	unsigned char tmp[64]; 

	if (count < 2) 
	{
		return -EFAULT;
	}
	
	if(count >sizeof(tmp))
	{
		return -EFAULT;
	}
	  
	
    return 0;
}



struct proc_dir_entry *procMldQuerierInfo=NULL;
#ifdef CONFIG_RTL_PROC_NEW
static int br_mldQuerierInfoRead(struct seq_file *s,void *v)
#else
static int br_mldQuerierInfoRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
#ifndef CONFIG_RTL_PROC_NEW
	int len=0;
#endif
	int i;
	unsigned long elapseJiffies;

	for(i=0; i<MAX_QUERIER_RECORD; i++)
	{
		if((querierInfoList[i].version==6) && time_after((unsigned long)(querierInfoList[i].lastJiffies+QUERIER_EXPIRED_TIME*HZ),jiffies))
		{
			if(jiffies > querierInfoList[i].lastJiffies)
			{
				elapseJiffies=jiffies-querierInfoList[i].lastJiffies;
			}
			else
			{
				elapseJiffies=jiffies+((unsigned long)0xFFFFFFFF-querierInfoList[i].lastJiffies)+1;
			}
#ifdef CONFIG_RTL_PROC_NEW
			seq_printf(s, "%s %x-%x-%x-%x %lu\n",
				querierInfoList[i].devName,
				querierInfoList[i].querierIp[0],
				querierInfoList[i].querierIp[1],
				querierInfoList[i].querierIp[2],
				querierInfoList[i].querierIp[3],elapseJiffies/HZ);
#else
			len += sprintf(page+len, "%s %x-%x-%x-%x %u\n",
				querierInfoList[i].devName,
				querierInfoList[i].querierIp[0],
				querierInfoList[i].querierIp[1],
				querierInfoList[i].querierIp[2],
				querierInfoList[i].querierIp[3],elapseJiffies/HZ);
#endif
		}
	}
#ifdef CONFIG_RTL_PROC_NEW
	return 0;
#else
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;
#endif
}

static int br_mldQuerierInfoWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	unsigned char tmp[64]; 

	if (count < 2) 
	{
		return -EFAULT;
	}
	
	if(count >sizeof(tmp))
	{
		return -EFAULT;
	}
	  
	
    return 0;
}

#endif


struct proc_dir_entry *procIgmpVersion=NULL;
int igmpVersion=2;
#ifdef CONFIG_RTL_PROC_NEW
static int br_igmpVersionRead(struct seq_file *s, void *v)
#else
static int br_igmpVersionRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s,"%c\n",igmpVersion + '0');
	return 0;
#else
	int len;
    len = sprintf(page, "%c\n", igmpVersion + '0');

    if (len <= off+count) *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count) len = count;
    if (len<0) len = 0;
    return len;
#endif
}

static int br_igmpVersionWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
      unsigned char tmp; 
      if (count < 2) 
	    return -EFAULT;
      
	if (buffer && !copy_from_user(&tmp, buffer, 1)) {
		igmpVersion = tmp - '0';
		if(igmpVersion>=3)
		{
			igmpVersion=3;
		}
		else if (igmpVersion<=2)
		{
			igmpVersion=2;
		}
		else
		{
			igmpVersion=2;
		}
	    return count;
      }
      return -EFAULT;
}


/*igmpv3 general query*/
static unsigned char igmpV3QueryBuf[64]={	0x01,0x00,0x5e,0x00,0x00,0x01,		/*destination mac*/
									0x00,0x00,0x00,0x00,0x00,0x00,		/*offset:6*/
									0x08,0x00,						/*offset:12*/
									0x46,0x00,0x00,0x24,				/*offset:14*/
									0x00,0x00,0x40,0x00,				/*offset:18*/
									0x01,0x02,0x00,0x00,				/*offset:22*/
									0x00,0x00,0x00,0x00,				/*offset:26,source ip*/
									0xe0,0x00,0x00,0x01,				/*offset:30,destination ip*/
									0x94,0x04,0x00,0x00,				/*offset:34,router alert option*/
									0x11,0x01,0x00,0x00,				/*offset:38*/
									0x00,0x00,0x00,0x00,				/*offset:42,queried multicast ip address*/
									0x0a,0x3c,0x00,0x00,				/*offset:46*/
									0x00,0x00,0x00,0x00,				/*offset:50*/
									0x00,0x00,0x00,0x00,				/*offset:54*/
									0x00,0x00,0x00,0x00,				/*offset:58*/
									0x00,0x00							/*offset:62*/
									
								};			



/*igmpv2 general query*/
static unsigned char igmpV2QueryBuf[64]={	0x01,0x00,0x5e,0x00,0x00,0x01,		/*destination mac*/
									0x00,0x00,0x00,0x00,0x00,0x00,		/*offset:6*/
									0x08,0x00,						/*offset:12*/
									0x45,0x00,0x00,0x1c,				/*offset:14*/
									0x00,0x00,0x40,0x00,				/*offset:18*/
									0x01,0x02,0x00,0x00,				/*offset:22*/
									0x00,0x00,0x00,0x00,				/*offset:26*/
									0xe0,0x00,0x00,0x01,				/*offset:30*/
									0x11,0x01,0x0c,0xfa,				/*offset:34*/
									0x00,0x00,0x00,0x00,				/*offset:38*/
									0x00,0x00,0x00,0x00,				/*offset:42*/
									0x00,0x00,0x00,0x00,				/*offset:46*/
									0x00,0x00,0x00,0x00,				/*offset:50*/
									0x00,0x00,0x00,0x00,				/*offset:54*/
									0x00,0x00,0x00,0x00,				/*offset:58*/
									0x00,0x00							/*offset:62*/
									
								};			



static unsigned short  br_ipv4Checksum(unsigned char *pktBuf, unsigned int pktLen)
{
	/*note: the first bytes of  packetBuf should be two bytes aligned*/
	unsigned int  checksum=0;
	unsigned int  count=pktLen;
	unsigned short   *ptr= (unsigned short *)pktBuf;	
	
	 while(count>1)
	 {
		  checksum+= ntohs(*ptr);
		  ptr++;
		  count -= 2;
	 }
	 
	if(count>0)
	{
		checksum+= *(pktBuf+pktLen-1)<<8; /*the last odd byte is treated as bit 15~8 of unsigned short*/
	}

	/* Roll over carry bits */
	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum += (checksum >> 16);

	/* Return checksum */
	return ((unsigned short) ~ checksum);

}

static unsigned char* br_generateIgmpQuery(struct net_bridge * br)
{
	struct net_device* brDev = NULL;
	unsigned short checkSum=0;
	struct in_device *in_dev;	
	struct net_device *landev;
	struct in_ifaddr *ifap = NULL;
	
      
	if(br==NULL)
	{
		return NULL;
	}
	
	brDev = br->dev;
	if(igmpVersion==3)
	{
		memcpy(&igmpV3QueryBuf[6],brDev->dev_addr,6);			/*set source mac address*/
	}
	else
	{
		memcpy(&igmpV2QueryBuf[6],brDev->dev_addr,6);			/*set source mac address*/
	}
	
	/*set source ip address*/
	if ((landev = __dev_get_by_name(&init_net, RTL_PS_BR0_DEV_NAME)) != NULL){
		in_dev=(struct in_device *)(landev->ip_ptr);
		if (in_dev != NULL) {
			for (ifap=in_dev->ifa_list; ifap != NULL; ifap=ifap->ifa_next) {
				if (strcmp(RTL_PS_BR0_DEV_NAME, ifap->ifa_label) == 0){
					if(igmpVersion==3)
					{
						memcpy(&igmpV3QueryBuf[26],&ifap->ifa_address,4);
					}
					else
					{
						memcpy(&igmpV2QueryBuf[26],&ifap->ifa_address,4);
					}
					
				}
			}
			
		}
	}
	else
	{
		return NULL;
	}
	
    #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if(!strcmp(RTL_PS_BR1_DEV_NAME, br->dev->name))
	{
		if (landev = brDev){
			in_dev=(struct net_device *)(landev->ip_ptr);
			if (in_dev != NULL) {
				for (ifap=in_dev->ifa_list; ifap != NULL; ifap=ifap->ifa_next) {
					if (strcmp(br->dev->name, ifap->ifa_label) == 0){
						if(igmpVersion==3)
						{
							memcpy(&igmpV3QueryBuf[26],&ifap->ifa_address,4);
						}
						else
						{
							memcpy(&igmpV2QueryBuf[26],&ifap->ifa_address,4);
						}
					}
				}
			}
		}
	}
        #endif
	if(igmpVersion==3)
	{
		igmpV3QueryBuf[24]=0;
		igmpV3QueryBuf[25]=0;
	}
	else
	{
		igmpV2QueryBuf[24]=0;
		igmpV2QueryBuf[25]=0;
	}
	
	if(igmpVersion==3)
	{
		checkSum=br_ipv4Checksum(&igmpV3QueryBuf[14],24);
	}
	else
	{
		checkSum=br_ipv4Checksum(&igmpV2QueryBuf[14],20);
	}

	if(igmpVersion==3)
	{
		igmpV3QueryBuf[24]=(checkSum&0xff00)>>8;
		igmpV3QueryBuf[25]=(checkSum&0x00ff);

	}
	else
	{
		igmpV2QueryBuf[24]=(checkSum&0xff00)>>8;
		igmpV2QueryBuf[25]=(checkSum&0x00ff);

	}
	

	if(igmpVersion==3)
	{
		igmpV3QueryBuf[40]=0;
		igmpV3QueryBuf[41]=0;
		checkSum=br_ipv4Checksum(&igmpV3QueryBuf[38],12);
		igmpV3QueryBuf[40]=(checkSum&0xff00)>>8;
		igmpV3QueryBuf[41]=(checkSum&0x00ff);
	}
	else
	{
		igmpV2QueryBuf[36]=0;
		igmpV2QueryBuf[37]=0;
		checkSum=br_ipv4Checksum(&igmpV2QueryBuf[34],8);
		igmpV2QueryBuf[36]=(checkSum&0xff00)>>8;
		igmpV2QueryBuf[37]=(checkSum&0x00ff);
	}

	if(igmpVersion==3)
	{
		return igmpV3QueryBuf;
	}
	else
	{
		return igmpV2QueryBuf;
	}
	
	return NULL;
}

extern int32 rtl_getGroupNum(uint32 ipVersion);

void br_igmpQueryTimerExpired(unsigned long arg)
{
	struct net_bridge *br = (struct net_bridge*) arg;
	unsigned char *igmpBuf=NULL;
	struct sk_buff *skb;
	struct sk_buff *skb2;
	struct net_bridge_port *p, *n;
	struct net_bridge_port *prev;
	unsigned int fwdCnt=0;

#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if(IGMPProxyOpened && strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME))
#else
	if(IGMPProxyOpened)
#endif
	{
		return ;
	}
	
	if(igmpQueryEnabled==0)
	{
		return;
	}

	if(rtl_getGroupNum(IP_VERSION4)==0)
		return;
#if defined (CONFIG_RTL_QUERIER_SELECTION)
	if(br_querierSelection(br,4)==0)
	{
		return;
	}
#endif	

	skb=dev_alloc_skb(1024);
	if(skb==NULL)
	{
		return;
	}

	memset(skb->data,64,0);
	igmpBuf=br_generateIgmpQuery(br);
	if(igmpBuf==NULL)
	{
		return;
	}

	memcpy(skb->data,igmpBuf,64);

	skb->len = 0;
	if(igmpVersion==3)
	{
		skb_put(skb, 50);
	}
	else
	{
		skb_put(skb, 42);
	}
	
	skb->dev=br->dev;
	
	prev = NULL;
	fwdCnt=0;
	list_for_each_entry_safe(p, n, &br->port_list, list) 
	{ 
		if ((p->state == BR_STATE_FORWARDING) && (strncmp(p->dev->name, "peth",4)!=0) && (strncmp(p->dev->name, "pwlan",5)!=0)) 
		{
			if (prev != NULL) 
			{   
				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) 
				{
					br->dev->stats.tx_dropped++;
					kfree_skb(skb);
					return;
				} 
				skb2->dev=prev->dev;
				#if defined(CONFIG_COMPAT_NET_DEV_OPS)
				prev->dev->hard_start_xmit(skb2, prev->dev);
				#else
				prev->dev->netdev_ops->ndo_start_xmit(skb2,prev->dev);
				#endif                  
				fwdCnt++;
			}
				                                                                               
			prev = p;
		}
	}

	if (prev != NULL) 
	{
		skb->dev=prev->dev;
	       #if defined(CONFIG_COMPAT_NET_DEV_OPS)
		prev->dev->hard_start_xmit(skb, prev->dev);
		#else
		prev->dev->netdev_ops->ndo_start_xmit(skb,prev->dev);
		#endif                            
		fwdCnt++;
	}

	if(fwdCnt==0)
	{
		/*to avoid memory leak*/
		kfree_skb(skb);
	}
	return;
}


#if defined (CONFIG_RTL_MLD_SNOOPING)
struct proc_dir_entry *procMldQuery=NULL;
int mldQueryEnabled=0;	
#ifdef CONFIG_RTL_PROC_NEW
static int br_mldQueryRead(struct seq_file *s,void *v)
#else
static int br_mldQueryRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s,"%c\n",mldQueryEnabled + '0');
	return 0;
#else
	int len;
    len = sprintf(page, "%c\n", mldQueryEnabled + '0');
    if (len <= off+count) *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count) len = count;
    if (len<0) len = 0;
    return len;
#endif
}

static int br_mldQueryWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
      unsigned char tmp; 
      if (count < 2) 
	    return -EFAULT;
      
	if (buffer && !copy_from_user(&tmp, buffer, 1)) {
		mldQueryEnabled = tmp - '0';
		if(mldQueryEnabled)
		{
			mldQueryEnabled=1;
		}
		else
		{
			mldQueryEnabled=0;
		}
	    return count;
      }
      return -EFAULT;
}

struct proc_dir_entry *procMldVersion=NULL;
int mldVersion=2;
#ifdef CONFIG_RTL_PROC_NEW
static int br_mldVersionRead(struct seq_file *s, void *v)
#else
static int br_mldVersionRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s,"%c\n", mldVersion + '0');
	return 0;
#else
	int len;
    len = sprintf(page, "%c\n", mldVersion + '0');
    if (len <= off+count) *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count) len = count;
    if (len<0) len = 0;
    return len;
#endif
}

static int br_mldVersionWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
      unsigned char tmp; 
      if (count < 2) 
	    return -EFAULT;
      
	if (buffer && !copy_from_user(&tmp, buffer, 1)) {
		mldVersion = tmp - '0';
		if(mldVersion>=2)
		{
			mldVersion=2;
		}
		else if (mldVersion<=1)
		{
			mldVersion=1;
		}
		else
		{
			mldVersion=2;
		}
	    return count;
      }
      return -EFAULT;
}

static unsigned char mldQueryBuf[90]={	0x33,0x33,0x00,0x00,0x00,0x01,		/*destination mac*/
									0x00,0x00,0x00,0x00,0x00,0x00,		/*source mac*/	/*offset:6*/
									0x86,0xdd,						/*ether type*/	/*offset:12*/
									0x60,0x00,0x00,0x00,				/*version(1 byte)-traffic cliass(1 byte)- flow label(2 bytes)*/	/*offset:14*/
									0x00,0x20,0x00,0x01,				/*payload length(2 bytes)-next header(1 byte)-hop limit(value:1 1byte)*//*offset:18*/
									0xfe,0x80,0x00,0x00,				/*source address*/	/*offset:22*/
									0x00,0x00,0x00,0x00,				/*be zero*/	/*offset:26*/
									0x00,0x00,0x00,					/*upper 3 bytes mac address |0x02*/ /*offset:30*/
									0xff,0xfe,						/*fixed*/
									0x00,0x00,0x00,					/*lowert 3 bytes mac address*/	 /*offset:35*/
									0xff,0x02,0x00,0x00,				/*destination address is fixed as FF02::1*/	/*offset:38*/
									0x00,0x00,0x00,0x00,			
									0x00,0x00,0x00,0x00,			
									0x00,0x00,0x00,0x01,			
									0x3a,0x00,						/*icmp type(1 byte)-length(1 byte)*/	 /*offset:54*/
									0x05,0x02,0x00,0x00,				/*router alert option*/
									0x01,0x00,						/*padN*/
									0x82,0x00,						/*type(query:0x82)-code(0)*/	/*offset:62*/
									0x00,0x00,						/*checksum*/	/*offset:64*/
									0x00,0x0a,						/*maximum reponse code*/
									0x00,0x00,						/*reserved*/
									0x00,0x00,0x00,0x00,				/*multicast address,fixed as 0*/
									0x00,0x00,0x00,0x00,			
									0x00,0x00,0x00,0x00,			
									0x00,0x00,0x00,0x00,
									0x0a,0x3c,0x00,0x00
								};			

static unsigned char ipv6PseudoHdrBuf[40]=	{	
									0xfe,0x80,0x00,0x00,				/*source address*/
									0x00,0x00,0x00,0x00,			
									0x00,0x00,0x00,0xff,			
									0xfe,0x00,0x00,0x00,			 	
									0xff,0x02,0x00,0x00,				/*destination address*/
									0x00,0x00,0x00,0x00,		
									0x00,0x00,0x00,0x00,			
									0x00,0x00,0x00,0x01,				
									0x00,0x00,0x00,0x18,				/*upper layer packet length*/
									0x00,0x00,0x00,0x3a					/*zero padding(3 bytes)-next header(1 byte)*/
									};		

static unsigned short br_ipv6Checksum(unsigned char *pktBuf, unsigned int pktLen, unsigned char  *ipv6PseudoHdrBuf)
{
	unsigned int  checksum=0;
	unsigned int count=pktLen;
	unsigned short   *ptr;

	/*compute ipv6 pseudo-header checksum*/
	ptr= (unsigned short  *) (ipv6PseudoHdrBuf);	
	for(count=0; count<20; count++) /*the pseudo header is 40 bytes long*/
	{
		  checksum+= ntohs(*ptr);
		  ptr++;
	}
	
	/*compute the checksum of mld buffer*/
	 count=pktLen;
	 ptr=(unsigned short  *) (pktBuf);	
	 while(count>1)
	 {
		  checksum+= ntohs(*ptr);
		  ptr++;
		  count -= 2;
	 }
	 
	if(count>0)
	{
		checksum+= *(pktBuf+pktLen-1)<<8; /*the last odd byte is treated as bit 15~8 of unsigned short*/
	}

	/* Roll over carry bits */
	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum += (checksum >> 16);

	/* Return checksum */
	return ((uint16) ~ checksum);
	
}
static unsigned char* br_generateMldQuery(struct net_bridge * br)
{
	struct net_device* brDev = NULL;
	unsigned short checkSum=0;
	if(br==NULL)
	{
		return NULL;
	}
	
	brDev = br->dev;
	
	memcpy(&mldQueryBuf[6],brDev->dev_addr,6);			/*set source mac address*/
	
	memcpy(&mldQueryBuf[30],brDev->dev_addr,3);		/*set  mld query packet source ip address*/
	mldQueryBuf[30]=mldQueryBuf[30]|0x02;		
	memcpy(&mldQueryBuf[35],&brDev->dev_addr[3],3);		

	
	memcpy(ipv6PseudoHdrBuf,&mldQueryBuf[22],16);			/*set pseudo-header source ip*/
	if(mldVersion==2)
	{
		mldQueryBuf[19]=	0x24;
	}
	else
	{
		mldQueryBuf[19]=	0x20;
	}

	mldQueryBuf[64]=0;/*reset checksum*/
	mldQueryBuf[65]=0;
	if(mldVersion==2)
	{
		ipv6PseudoHdrBuf[35]=28;
		checkSum=br_ipv6Checksum(&mldQueryBuf[62],28,ipv6PseudoHdrBuf);
	}
	else
	{
		ipv6PseudoHdrBuf[35]=24;
		checkSum=br_ipv6Checksum(&mldQueryBuf[62],24,ipv6PseudoHdrBuf);
	}
	
	
	mldQueryBuf[64]=(checkSum&0xff00)>>8;
	mldQueryBuf[65]=(checkSum&0x00ff);
	return mldQueryBuf;
	
	
}


void br_mldQueryTimerExpired(unsigned long arg)
{
	struct net_bridge *br = (struct net_bridge*) arg;
	struct sk_buff *skb;
	struct sk_buff *skb2;
	struct net_bridge_port *p, *n;
	struct net_bridge_port *prev;
	unsigned int fwdCnt=0;
	unsigned char *mldBuf=NULL;

	if(mldQueryEnabled==0)
	{
		return;
	}
	if(rtl_getGroupNum(IP_VERSION6)==0)
		return;
#if defined (CONFIG_RTL_QUERIER_SELECTION)
	if(br_querierSelection(br,6)==0)
	{
		return;
	}
#endif	

	skb=dev_alloc_skb(1024);
	if(skb==NULL)
	{
		return;
	}
	
	memset(skb->data,86,0);
	mldBuf=br_generateMldQuery(br);
	if(mldBuf==NULL)
	{
		return;
	}
	
	if(mldVersion==2)
	{
		memcpy(skb->data,mldBuf,90);
		skb->len = 0;
		skb_put(skb, 90);
	}
	else
	{
		memcpy(skb->data,mldBuf,86);
		skb->len = 0;
		skb_put(skb, 86);
	}
 
	prev = NULL;
	fwdCnt=0;
	list_for_each_entry_safe(p, n, &br->port_list, list) 
	{ 
		if ((p->state == BR_STATE_FORWARDING) && (strncmp(p->dev->name, "peth",4)!=0) && (strncmp(p->dev->name, "pwlan",5)!=0)) 
		{
			if (prev != NULL) 
			{                                                                                       
				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) 
				{
					br->dev->stats.tx_dropped++;
					kfree_skb(skb);
					return;
				} 
				skb2->dev=prev->dev;
				#if defined(CONFIG_COMPAT_NET_DEV_OPS)
				prev->dev->hard_start_xmit(skb2, prev->dev);
				#else
				prev->dev->netdev_ops->ndo_start_xmit(skb2,prev->dev);
				#endif                  
				fwdCnt++;
			}
				                                                                               
			prev = p;
		}
	}

	if (prev != NULL) 
	{
		skb->dev=prev->dev;
	       #if defined(CONFIG_COMPAT_NET_DEV_OPS)
		prev->dev->hard_start_xmit(skb, prev->dev);
		#else
		prev->dev->netdev_ops->ndo_start_xmit(skb,prev->dev);
		#endif                            
		fwdCnt++;
	}

	if(fwdCnt==0)
	{
		/*to avoid memory leak*/
		kfree_skb(skb);
	}
	
	return;
}
#endif

static unsigned int mCastQueryTimerCnt=0;
void br_mCastQueryTimerExpired(unsigned long arg)
{
	struct net_bridge *br = (struct net_bridge*) arg;
	
	mod_timer(&br->mCastQuerytimer, jiffies+MCAST_QUERY_INTERVAL*HZ);
    #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
		if(!strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME))
		{
			br_igmpQueryTimerExpired(arg);
			return;
		}
	#endif
	
	if(mCastQueryTimerCnt%2==0)
	{
		br_igmpQueryTimerExpired(arg);
	}
	else
	{
		#if defined (CONFIG_RTL_MLD_SNOOPING)
		br_mldQueryTimerExpired(arg);
		#endif
	}
	mCastQueryTimerCnt++;
	
	return;
}

extern struct net_bridge *find_br_by_name(char *name);

void br_signal_igmpProxy(void)
{

	struct task_struct *task;
	struct net_bridge *br;

	br = find_br_by_name(RTL_PS_BR0_DEV_NAME);
	if(br==NULL)
	{
		return;
	}

	if(br->igmpProxy_pid==0)
	{
		return;
	}
	
	read_lock(&tasklist_lock);
//	task = find_task_by_pid(br->igmpProxy_pid);
	task = find_task_by_vpid(br->igmpProxy_pid);
	read_unlock(&tasklist_lock);
	if(task)
	{
		send_sig(SIGUSR2,task,0);
	}
	else {
	    //printk("Path selection daemon pid: %d does not exist\n", br->mesh_pathsel_pid);
	}
}

#endif/*CONFIG_RTL_IGMP_SNOOPING*/


struct proc_dir_entry *procIgmpProxy = NULL;
#ifdef CONFIG_RTL_PROC_NEW
static int br_igmpProxyRead(struct seq_file *s, void *v)
#else
static int br_igmpProxyRead(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
#endif
{
#ifdef CONFIG_RTL_PROC_NEW
	seq_printf(s, "%c\n",IGMPProxyOpened + '0');
	return 0;
#else
    int len;
    len = sprintf(page, "%c\n", IGMPProxyOpened + '0');
    if (len <= off+count) *eof = 1;
    *start = page + off;
    len -= off;
    if (len>count) len = count;
    if (len<0) len = 0;
    return len;
#endif
}
static int br_igmpProxyWrite(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
    unsigned char chartmp; 
	  
    if (count > 1) {	//call from shell
      	if (buffer && !copy_from_user(&chartmp, buffer, 1)) {
	    	IGMPProxyOpened = chartmp - '0';			
	    }
	}else if(count==1){//call from demon(demon direct call br's ioctl)
			//memcpy(&chartmp,buffer,1);
			if(buffer){
				get_user(chartmp,buffer);	
		    		IGMPProxyOpened = chartmp - '0';
			}else
				return -EFAULT;

	}else{

		return -EFAULT;
	}
	return count;
}

#ifdef CONFIG_RTL_PROC_NEW
extern struct proc_dir_entry proc_root;
#if defined(CONFIG_RTL_IGMP_SNOOPING)
/*igmp snoop proc*/
int rlx_br_igmpsnoop_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_igmpSnoopRead,NULL));
}
static ssize_t rlx_br_igmpsnoop_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_igmpSnoopWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_igmpsnoop_proc_fops= {
        .open           = rlx_br_igmpsnoop_single_open,
        .write		    = rlx_br_igmpsnoop_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
/*igmp FastFwd proc*/
int rlx_br_mcast_fast_fwd_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_mCastFastFwdRead,NULL));
}
static ssize_t rlx_br_mcast_fast_fwd_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_mCastFastFwdWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_mcast_fast_fwd_proc_fops= {
        .open           = rlx_br_mcast_fast_fwd_single_open,
        .write		    = rlx_br_mcast_fast_fwd_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
/*igmp version proc*/
int rlx_br_igmpsnoop_version_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_igmpVersionRead,NULL));
}
static ssize_t rlx_br_igmpsnoop_version_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_igmpVersionWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_igmpsnoop_version_proc_fops= {
        .open           = rlx_br_igmpsnoop_version_single_open,
        .write		    = rlx_br_igmpsnoop_version_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
/*igmp querry proc*/
int rlx_br_igmp_query_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_igmpQueryRead,NULL));
}
static ssize_t rlx_br_igmp_query_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_igmpQueryWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_igmp_query_proc_fops= {
        .open           = rlx_br_igmp_query_single_open,
        .write		    = rlx_br_igmp_query_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
/*igmp querier info proc*/
int rlx_br_igmp_querier_info_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_igmpQuerierInfoRead,NULL));
}
static ssize_t rlx_br_igmp_querier_info_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_igmpQuerierInfoWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_igmp_querier_info_proc_fops= {
        .open           = rlx_br_igmp_querier_info_single_open,
        .write		    = rlx_br_igmp_querier_info_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#ifdef CONFIG_RTL_MLD_SNOOPING
/*mld querier info proc*/
int rlx_br_mld_querier_info_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_mldQuerierInfoRead,NULL));
}
static ssize_t rlx_br_mld_querier_info_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_mldQuerierInfoWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_mld_querier_info_proc_fops= {
        .open           = rlx_br_mld_querier_info_single_open,
        .write		    = rlx_br_mld_querier_info_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
/*mld version proc*/
int rlx_br_mld_version_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_mldVersionRead,NULL));
}
static ssize_t rlx_br_mld_version_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_mldVersionWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_mld_version_proc_fops= {
        .open           = rlx_br_mld_version_single_open,
        .write		    = rlx_br_mld_version_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};

/*mld query proc*/
int rlx_br_mld_query_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_mldQueryRead,NULL));
}
static ssize_t rlx_br_mld_query_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_mldQueryWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_mld_query_proc_fops= {
        .open           = rlx_br_mld_query_single_open,
        .write		    = rlx_br_mld_query_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif
/*igmp proxy proc*/
int rlx_br_igmp_proxy_single_open(struct inode *inode, struct file *file)
{
	return(single_open(file, br_igmpProxyRead,NULL));
}
static ssize_t rlx_br_igmp_proxy_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return br_igmpProxyWrite(file, userbuf,count, off);
}

struct file_operations rlx_br_igmp_proxy_proc_fops= {
        .open           = rlx_br_igmp_proxy_single_open,
        .write		    = rlx_br_igmp_proxy_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};

#endif
#endif


void br_create_proc_files(void)
{
#if defined (CONFIG_PROC_FS) && defined (CONFIG_RTL_IGMP_SNOOPING)
#ifdef CONFIG_RTL_PROC_NEW
	proc_create_data("br_igmpsnoop",0,&proc_root,&rlx_br_igmpsnoop_proc_fops,NULL);
	proc_create_data("br_igmpDb",0,&proc_root,&igmp_db_seq_file_operations,NULL);
	proc_create_data("br_mCastFastFwd",0,&proc_root,&rlx_br_mcast_fast_fwd_proc_fops,NULL);
	proc_create_data("br_igmpVersion",0,&proc_root,&rlx_br_igmpsnoop_version_proc_fops,NULL);
	proc_create_data("br_igmpquery",0,&proc_root,&rlx_br_igmp_query_proc_fops,NULL);
#if defined (CONFIG_RTL_QUERIER_SELECTION)	
	br_initQuerierInfo();
	proc_create_data("br_igmpQuerierInfo",0,&proc_root,&rlx_br_igmp_querier_info_proc_fops,NULL);
#ifdef CONFIG_RTL_MLD_SNOOPING
	proc_create_data("br_mldQuerierInfo",0,&proc_root,&rlx_br_mld_querier_info_proc_fops,NULL);
#endif
#endif
#if defined (CONFIG_RTL_MLD_SNOOPING)
	proc_create_data("br_mldVersion",0,&proc_root,&rlx_br_mld_version_proc_fops,NULL);
	proc_create_data("br_mldquery",0,&proc_root,&rlx_br_mld_query_proc_fops,NULL);
#endif
#else
	procIgmpSnoop = create_proc_entry("br_igmpsnoop", 0, NULL);
	if (procIgmpSnoop) {
		procIgmpSnoop->read_proc = br_igmpSnoopRead;
		procIgmpSnoop->write_proc = br_igmpSnoopWrite;
	}
	procIgmpDb=create_proc_entry("br_igmpDb", 0, NULL);
	if(procIgmpDb != NULL)
	{
		procIgmpDb->proc_fops = &igmp_db_seq_file_operations;
	}
	procMCastFastFwd= create_proc_entry("br_mCastFastFwd", 0, NULL);
	if (procMCastFastFwd) {
		procMCastFastFwd->read_proc = br_mCastFastFwdRead;
		procMCastFastFwd->write_proc = br_mCastFastFwdWrite;
	}
	procIgmpVersion= create_proc_entry("br_igmpVersion", 0, NULL);
	if (procIgmpVersion) {
		procIgmpVersion->read_proc = br_igmpVersionRead;
		procIgmpVersion->write_proc = br_igmpVersionWrite;
	}
					
	procIgmpQuery= create_proc_entry("br_igmpquery", 0, NULL);
	if (procIgmpQuery) {
		procIgmpQuery->read_proc = br_igmpQueryRead;
		procIgmpQuery->write_proc = br_igmpQueryWrite;
	}
#if defined (CONFIG_RTL_QUERIER_SELECTION)		
	br_initQuerierInfo();
	procIgmpQuerierInfo = create_proc_entry("br_igmpQuerierInfo", 0, NULL);
	if (procIgmpQuerierInfo) {
		procIgmpQuerierInfo->read_proc = br_igmpQuerierInfoRead;
		procIgmpQuerierInfo->write_proc = br_igmpQuerierInfoWrite;
	}
			
	procMldQuerierInfo = create_proc_entry("br_mldQuerierInfo", 0, NULL);
	if (procMldQuerierInfo) {
		procMldQuerierInfo->read_proc = br_mldQuerierInfoRead;
		procMldQuerierInfo->write_proc = br_mldQuerierInfoWrite;
	}
#endif
			
#if defined (CONFIG_RTL_MLD_SNOOPING)
	procMldVersion= create_proc_entry("br_mldVersion", 0, NULL);
	if (procMldVersion) {
		procMldVersion->read_proc = br_mldVersionRead;
		procMldVersion->write_proc = br_mldVersionWrite;
	}
	procMldQuery= create_proc_entry("br_mldquery", 0, NULL);
	if (procMldQuery) {
		procMldQuery->read_proc = br_mldQueryRead;
		procMldQuery->write_proc = br_mldQueryWrite;
	}
#endif
#endif
#endif
#if defined(CONFIG_RTL_IGMP_SNOOPING)
#if defined (CONFIG_RTL_PROC_NEW) 
	proc_create_data("br_igmpProxy",0,&proc_root,&rlx_br_igmp_proxy_proc_fops,NULL);
#else
	procIgmpProxy = create_proc_entry("br_igmpProxy", 0, NULL);
	if (procIgmpProxy) {
		procIgmpProxy->read_proc = br_igmpProxyRead;
		procIgmpProxy->write_proc = br_igmpProxyWrite;
	}
#endif
#endif
}
void br_destroy_proc_files(void)
{
#if  defined (CONFIG_PROC_FS) && defined (CONFIG_RTL_IGMP_SNOOPING)
	if (procIgmpSnoop) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_igmpsnoop", &proc_root);		
#else
		remove_proc_entry("br_igmpsnoop", procIgmpSnoop);		
		procIgmpSnoop = NULL;
#endif
	}
				
	if(procIgmpDb!=NULL)
	{
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_igmpDb", &proc_root); 	
#else
		remove_proc_entry("br_igmpDb", procIgmpDb);
		procIgmpDb = NULL;
#endif
	}
	if (procMCastFastFwd) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_mCastFastFwd", &proc_root);	
#else
		remove_proc_entry("br_mCastFastFwd", procMCastFastFwd); 	
		procMCastFastFwd = NULL;
#endif
	}
				
	if (procIgmpVersion) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_igmpVersion", &proc_root);	
#else
		remove_proc_entry("br_igmpVersion", procIgmpVersion);		
		procIgmpVersion = NULL;
#endif
	}
					
	if (procIgmpQuery) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_query", &proc_root);	
#else
		remove_proc_entry("br_igmpquery", procIgmpQuery);		
		procIgmpQuery = NULL;
#endif
	}
				
#if defined (CONFIG_RTL_MLD_SNOOPING)
	if (procMldVersion) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_mldVersion", &proc_root); 	
#else
		remove_proc_entry("br_mldVersion", procMldVersion); 	
		procMldVersion = NULL;
#endif
	}
	if (procMldQuery) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_mldquery", &proc_root);	
#else
		remove_proc_entry("br_mldquery", procMldQuery); 	
		procMldQuery = NULL;
#endif
	}
#endif	
			
#if defined (CONFIG_RTL_QUERIER_SELECTION)		
	if (procIgmpQuerierInfo) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_igmpQuerierInfo", &proc_root);	
#else
		remove_proc_entry("br_igmpQuerierInfo", procIgmpQuerierInfo);	
		procIgmpQuerierInfo=NULL;
#endif
	}
			
	if (procMldQuerierInfo) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_mldQuerierInfo", &proc_root); 	
#else
		remove_proc_entry("br_mldQuerierInfo", procMldQuerierInfo); 
		procMldQuerierInfo=NULL;
#endif
	}
#endif
#endif
#if  defined (CONFIG_PROC_FS) 
	if (procIgmpProxy) {
#ifdef CONFIG_RTL_PROC_NEW
		remove_proc_entry("br_igmpProxy", &proc_root);	
#else
		remove_proc_entry("br_igmpProxy", procIgmpProxy);		
		procIgmpProxy = NULL;
#endif
	}
#endif
}

static int __init br_init(void)
{
	int err;

	err = stp_proto_register(&br_stp_proto);
	if (err < 0) {
		pr_err("bridge: can't register sap for STP\n");
		return err;
	}

	err = br_fdb_init();
	if (err)
		goto err_out;

	err = register_pernet_subsys(&br_net_ops);
	if (err)
		goto err_out1;

#if defined (CONFIG_RTL865X_LANPORT_RESTRICTION)
	lan_restrict_init();
#endif

	err = br_netfilter_init();
	if (err)
		goto err_out2;

	err = register_netdevice_notifier(&br_device_notifier);
	if (err)
		goto err_out3;

	err = br_netlink_init();
	if (err)
		goto err_out4;

	brioctl_set(br_ioctl_deviceless_stub);

#if IS_ENABLED(CONFIG_ATM_LANE)
	br_fdb_test_addr_hook = br_fdb_test_addr;
#endif
	br_create_proc_files();
	return 0;
err_out4:
	unregister_netdevice_notifier(&br_device_notifier);
err_out3:
	br_netfilter_fini();
err_out2:
	unregister_pernet_subsys(&br_net_ops);
err_out1:
	br_fdb_fini();
err_out:
	stp_proto_unregister(&br_stp_proto);
	return err;
}

static void __exit br_deinit(void)
{
	br_destroy_proc_files();
	stp_proto_unregister(&br_stp_proto);

	br_netlink_fini();
	unregister_netdevice_notifier(&br_device_notifier);
	brioctl_set(NULL);

	unregister_pernet_subsys(&br_net_ops);

	rcu_barrier(); /* Wait for completion of call_rcu()'s */

	br_netfilter_fini();
#if IS_ENABLED(CONFIG_ATM_LANE)
	br_fdb_test_addr_hook = NULL;
#endif

	br_fdb_fini();
}

module_init(br_init)
module_exit(br_deinit)
MODULE_LICENSE("GPL");
MODULE_VERSION(BR_VERSION);
MODULE_ALIAS_RTNL_LINK("bridge");
