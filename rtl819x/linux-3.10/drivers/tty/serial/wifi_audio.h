#ifndef __WFD_NETLINK_H
#define __WFD_NETLINK_H

#define UNIX_SOCK_PATH "/tmp/mysocket"



struct wifi_audio_task {
	struct task_struct *thread;
	struct completion thread_done;
	char name[16];
	void (*loop_ops)(struct wifi_audio_task *);
#if(LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0))
	spinlock_t lock;
#endif
};

enum {
    	NL_RESERVED=0,          // reserved
    	NL_USER_REG=1,          // register a NL service
	NL_GPIO_PROBE=2
};
enum {
    WFD_USERID_WFAUDIO=0
};

enum {
    GPIO_TIME_SHORT=0,
    GPIO_TIME_LONG=1
};

struct wfaudio_nl_hdr {
    unsigned short msgtype;
    unsigned short msglen;
};

struct wfaudio_nl_reg {
    struct wfaudio_nl_hdr hdr;
    unsigned int pid;
    unsigned int userid;
};
struct wfaudio_nl_gpio_state {
    struct wfaudio_nl_hdr hdr;
    unsigned int  gpio_state;
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15)
#define NLMSG_MIN_TYPE		0x10
#endif
#define NLMSG_TYPE_WFAUDIO    (NLMSG_MIN_TYPE+2)
#define NETLINK_RTK_WFAUDIO       26

#define WIFI_AUDIO_GPIO_PROBE_TIMEOUT       (1 << 0)

#endif
