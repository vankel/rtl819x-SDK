#ifndef IPV6_MANAGE_INET_H
#define IPV6_MANAGE_INET_H

#define IPV6_MANG_PID_FILE "/var/run/ipv6_manage_inet.pid"

typedef struct RA_INFO_ITEM_
{
	int enable;
	int pid;                      //input
	int slaacFail;
	int icmp6_addrconf_managed;	  //output
	int icmp6_addrconf_other;	  //output
} RA_INFO_ITEM_T, *RA_INFO_ITEM_Tp;

#endif
