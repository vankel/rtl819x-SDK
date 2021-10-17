#ifndef __GETIFSTATS_H__
#define __GETIFSTATS_H__

struct ifdata {
	unsigned long opackets;
	unsigned long ipackets;
	unsigned long obytes;
	unsigned long ibytes;
	unsigned long baudrate;
};

unsigned long getifstats(const char * ifname, struct ifdata * data,int i);
unsigned long getifstats_all(const char * ifname, struct ifdata * data);
int getWanLink(char *interface);

#endif

