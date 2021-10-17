/*
 */
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/net.h>
#include <asm/byteorder.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/file.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>

#include "bspchip.h"
#include "wifi_audio.h"

#define EU_VE3 4
#define E_VE3 7

#define BIT(nr)                 (1UL << (nr))
#define RTL_GPIO_DIR_GPIOB5 (1<<13)

#define GPIO_PROBE_TIMER_GAP 200 //in ms

extern int RTLWIFINIC_GPIO_read(unsigned int gpio_num);
extern void RTLWIFINIC_GPIO_config(unsigned int gpio_num, unsigned int direction);
#define EU_VE3 4
#define E_VE3 7
#if defined(CONFIG_RTL_8196E)
extern unsigned int RTL96E_BOND;
#endif
#define RTL_R32(addr)		(*(volatile unsigned long *)(addr))
#define RTL_W32(addr, l)	((*(volatile unsigned long*)(addr)) = (l))
struct timer_list wifi_audio_gpio_probe_timer;
void wifi_audio_nl_send_msg(unsigned int pid, struct wfaudio_nl_hdr *hdr);
void send_nl_gpio_state(unsigned int pid, unsigned int state);
void wifi_audio_sink_init(void);
struct sock *wifi_audio_nl_sock = NULL;
EXPORT_SYMBOL_GPL(wifi_audio_nl_sock);
struct netlink_kernel_cfg *nl_cfg = NULL;
struct wifi_audio_task wifi_audio_gpio_probe;
wait_queue_head_t wifi_audio_probe_waitq;
unsigned int probe_flag=0;
static unsigned int    probe_counter;
static unsigned int    probe_state;
unsigned int user_pid=0;


/* GPIO Register Set */
#define GPIO_BASE                           (0xB8003500)	
#define PAB_IMR                             (GPIO_BASE + 0x14)
#define PABCD_ISR                           (GPIO_BASE + 0x10)
#define PABCD_CNR                           (GPIO_BASE + 0x00)
#define PABCD_PTYPE                         (GPIO_BASE + 0x04)
#define PABCD_DIR                           (GPIO_BASE + 0x08)
#define PABCD_DAT                           (GPIO_BASE + 0x0C)

#define RESET_PIN_IOBASE 	PABCD_CNR	//RTL_GPIO_PABCD_CNR
#define RESET_PIN_DIRBASE 	PABCD_DIR //RTL_GPIO_PABCD_DIR 
#define RESET_PIN_DATABASE 	PABCD_DAT //RTL_GPIO_PABCD_DATA
#define RESET_PIN_IMR		 (GPIO_BASE + 0x14)

#define RESET_BTN_PIN	13

#define PROBE_TIME	4 //1seconds


#define PROBE_NULL	0
#define PROBE_ACTIVE	1
#define PROBE_RESET	2
#define PROBE_RELOAD	3
#define RTL_R32(addr)	(*(volatile unsigned long *)(addr))
#define RTL_W32(addr, l)	((*(volatile unsigned long*)(addr)) = (l))
 
 
struct wfaudio_gpio_device
{
	unsigned int name;
};
struct wfaudio_gpio_device priv_wfaudio_gpio_device;
 static void wifi_audio_gpio_probe_task(unsigned long data)
{
	unsigned int pressed=1;
	int gpio_status=0;
#if defined(CONFIG_RTL_8196E)
	if(RTL96E_BOND == EU_VE3)
		gpio_status = (RTL_R32(RESET_PIN_DATABASE) & (1 << RESET_BTN_PIN));
	else if(RTL96E_BOND == E_VE3){
		gpio_status = RTLWIFINIC_GPIO_read(0);
		gpio_status = !gpio_status;
	}
#else	
	gpio_status =1;
#endif
	
	if (gpio_status)
	{
		pressed = 0;
	}
	else
	{
		//printk("Key pressed %d!\n", probe_counter+1);
	}

	if (probe_state == PROBE_NULL)
	{
		if (pressed)
		{
			probe_state = PROBE_ACTIVE;
			probe_counter++;
		}
		else
			probe_counter = 0;
	}
	else if (probe_state == PROBE_ACTIVE)
	{
		if (pressed)
		{
			probe_counter++;
			
			//push button has been pressed
			if ((probe_counter >=2 ) && (probe_counter <=PROBE_TIME))
			{
				
			}
			else if (probe_counter >= PROBE_TIME)
			{
				
			}
		}
		else
		{
			//push button has been release
			if (probe_counter > PROBE_TIME)
			{
				probe_state = PROBE_NULL;
				probe_counter = 0;
				send_nl_gpio_state(user_pid, GPIO_TIME_SHORT);
			}
#if 0
	
			
			if (probe_counter < PROBE_TIME)
			{
				//printk("result <%d\n", PROBE_TIME);
				
				
			}
			else
			{
				//printk("result >=%d\n", PROBE_TIME);
				probe_counter = 0;
				probe_state = PROBE_NULL;
				//send_nl_gpio_state(user_pid, GPIO_TIME_LONG);
			}
#endif			
		}
	}
	mod_timer(&wifi_audio_gpio_probe_timer, jiffies + msecs_to_jiffies(GPIO_PROBE_TIMER_GAP));
}
 
 
 static void timer_wifi_audio_gpio_probe(unsigned long data)
{
	probe_flag |= WIFI_AUDIO_GPIO_PROBE_TIMEOUT;
	wake_up(&wifi_audio_probe_waitq);
}
void Init_wifi_audio_probe_timer(void)
{
 	init_timer(&wifi_audio_gpio_probe_timer);
    	wifi_audio_gpio_probe_timer.function = timer_wifi_audio_gpio_probe;
    	wifi_audio_gpio_probe_timer.data = (unsigned long)NULL;
    	wifi_audio_gpio_probe_timer.expires = jiffies + msecs_to_jiffies(GPIO_PROBE_TIMER_GAP);
    	add_timer(&wifi_audio_gpio_probe_timer);
}


void send_nl_gpio_state(unsigned int pid, unsigned int state)
{
    struct wfaudio_nl_gpio_state nl_msg;

    memset(&nl_msg, 0, sizeof(nl_msg));

    nl_msg.hdr.msgtype = NL_GPIO_PROBE;
    nl_msg.hdr.msglen = sizeof (struct wfaudio_nl_gpio_state);

    nl_msg.gpio_state = state;
    wifi_audio_nl_send_msg(pid, &nl_msg.hdr);
}



static void wifi_audio_nl_rcv_msg(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	struct wfaudio_nl_hdr *hdr;
	//unsigned long flags;
	u32 rlen;
	int err;
	//int table_idx;
	
	union {
	    struct wfaudio_nl_reg *preg;
	    struct wfaudio_nl_gpio_state *gpioState;
	} nl_cmd;

	while (skb->len >= NLMSG_SPACE(0)) {
		err = 0;

		nlh = nlmsg_hdr(skb);
		if ((nlh->nlmsg_len < (sizeof(*nlh) + sizeof(*hdr))) ||
		    (skb->len < nlh->nlmsg_len)) {
			printk(KERN_WARNING "%s: discarding partial skb(#1)\n",
				 __func__);
			return;
		}

		rlen = NLMSG_ALIGN(nlh->nlmsg_len);
		if (rlen > skb->len)
			rlen = skb->len;

		if (nlh->nlmsg_type != NLMSG_TYPE_WFAUDIO) 
		{
			err = -EBADMSG;
			goto next_msg;
		}

		hdr = NLMSG_DATA(nlh);
		nl_cmd.preg = (struct wfaudio_nl_reg *)hdr;


        switch (hdr->msgtype)
        {
        case NL_USER_REG:
            //panic_printk("\n\nWFD Knl Rcv NL_USER_REG: %d %d\n", nl_cmd.preg->pid, nl_cmd.preg->userid);
            user_pid=nl_cmd.preg->pid;
            wifi_audio_sink_init();
            break;
             
        default:
            err = 1;
            printk("WFD netlink rcv an unknown command %d\n", hdr->msgtype);
        }
        
next_msg:
		if ((err) || (nlh->nlmsg_flags & NLM_F_ACK))
			netlink_ack(skb, nlh, err);

		skb_pull(skb, rlen);
	}
}

void wifi_audio_nl_send_msg(u32 pid, struct wfaudio_nl_hdr *hdr)
{
	struct sk_buff *skb;
	struct nlmsghdr	*nlh;
	const char *fn;
	char *datab;
	u32 len, skblen;
	int err;
	uint16_t msglen;

	if (!wifi_audio_nl_sock) 
	{
		err = -ENOENT;
		fn = "netlink socket";
		goto msg_fail;
	}

    msglen = hdr->msglen;
	len = NLMSG_SPACE(msglen);
	skblen = NLMSG_SPACE(len);

	skb = alloc_skb(skblen, GFP_KERNEL);
	if (!skb) 
	{
		err = -ENOBUFS;
		fn = "alloc_skb";
		goto msg_fail;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
	nlh = nlmsg_put(skb, pid, 0, NLMSG_TYPE_WFAUDIO, len - sizeof(*nlh), 0);
#else
	nlh = __nlmsg_put(skb, pid, 0, NLMSG_TYPE_WFAUDIO, len - sizeof(*nlh));
#endif	
	if (!nlh) 
	{
		err = -ENOBUFS;
		fn = "nlmsg_put";
		goto msg_fail_skb;
	}
	datab = NLMSG_DATA(nlh);	
	memcpy(datab, hdr, msglen);

	err = nlmsg_unicast(wifi_audio_nl_sock, skb, pid);
	if (err < 0) {
		fn = "nlmsg_unicast";
		/* nlmsg_unicast already kfree_skb'd */
		goto msg_fail;
	}

	return;

msg_fail_skb:
	kfree_skb(skb);
msg_fail:
	printk(KERN_WARNING
		"%s: Dropped Message : pid %d , msgtype x%x, "
		"msglen %d: %s : err %d\n",
		__func__, pid, hdr->msgtype, hdr->msglen,
		fn, err);
	return;
}
EXPORT_SYMBOL_GPL(wifi_audio_nl_send_msg);

#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
static void wifi_audio_netlink_rcv(struct sock *sk, int len)
{
	unsigned int qlen = skb_queue_len(&sk->sk_receive_queue);

	do {
		struct sk_buff *skb;

		if (qlen > skb_queue_len(&sk->sk_receive_queue))
			qlen = skb_queue_len(&sk->sk_receive_queue);

		for (; qlen; qlen--) 
		{
	    	skb = skb_dequeue(&sk->sk_receive_queue);
		wifi_audio_nl_rcv_msg(skb);
	        	kfree_skb(skb);
		}
	}while (qlen);
}
#endif	/* #if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)) */

void wifi_audio_nl_init(void)
{
#if(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
	wifi_audio_nl_sock = netlink_kernel_create(NETLINK_RTK_WFAUDIO, wifi_audio_netlink_rcv);
#elif(LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
	wifi_audio_nl_sock = netlink_kernel_create(NETLINK_RTK_WFAUDIO, 0, wifi_audio_netlink_rcv, 
			NULL, THIS_MODULE);
#elif(LINUX_VERSION_CODE < KERNEL_VERSION(3,6,0))
	wifi_audio_nl_sock = netlink_kernel_create(&init_net, NETLINK_RTK_WFAUDIO,
				0, wifi_audio_nl_rcv_msg, NULL,
				THIS_MODULE);
#elif(LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))
	nl_cfg = (struct netlink_kernel_cfg *)kmalloc(sizeof(struct netlink_kernel_cfg), GFP_KERNEL);
	nl_cfg->groups = 0;
	nl_cfg->input = wifi_audio_nl_rcv_msg;
	nl_cfg->cb_mutex = NULL;
	nl_cfg->bind = NULL;
	wifi_audio_nl_sock = netlink_kernel_create(&init_net, NETLINK_RTK_WFAUDIO,
				THIS_MODULE, nl_cfg);
#else
	nl_cfg = (struct netlink_kernel_cfg *)kmalloc(sizeof(struct netlink_kernel_cfg), GFP_KERNEL);
	nl_cfg->groups = 0;
	nl_cfg->flags = NL_CFG_F_NONROOT_RECV | NL_CFG_F_NONROOT_SEND;
	nl_cfg->input = wifi_audio_nl_rcv_msg;
	nl_cfg->cb_mutex = NULL;
	nl_cfg->bind = NULL;
	wifi_audio_nl_sock = netlink_kernel_create(&init_net, NETLINK_RTK_WFAUDIO,
				 nl_cfg);
#endif

	if (!wifi_audio_nl_sock) 
	{
		printk("%s: netlink create failed\n", __func__);
	}
	
#if defined(CONFIG_RTL_8196E)	
	if(RTL96E_BOND == EU_VE3){
		//set shared pin GPIO PIN, GPIOB5 in GPIO mode
		RTL_W32(0xB8000044, (RTL_R32(0xB8000044) | 0x600));
		//set GPIO PIN, GPIOB[5]
		RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(RTL_GPIO_DIR_GPIOB5))));
		//set direction, GPIOB[5] INPUT
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOB5))));
	}else if(RTL96E_BOND ==E_VE3){
		RTLWIFINIC_GPIO_config(0, 0x01); //read direction
	}
#endif	

	return;
}

void wifi_audio_nl_exit(void)
{
    if (wifi_audio_nl_sock)
    {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,24)
        sock_release (wifi_audio_nl_sock->sk_socket);
#else
        netlink_kernel_release(wifi_audio_nl_sock);
#endif
        wifi_audio_nl_sock = NULL;
    }

	if(nl_cfg) {
		kfree(nl_cfg);
		nl_cfg = NULL;
	}
}

int wifi_audio_thread(void *param)
{
	struct wifi_audio_task *ut = (struct wifi_audio_task *) param;

	if (!ut)
		return -EINVAL;
	
#if(LINUX_VERSION_CODE == KERNEL_VERSION(2,6,30))
	lock_kernel();
	daemonize(ut->name);
#else
	spin_lock(&ut->lock);
#endif
	allow_signal(SIGKILL);
	ut->thread = current;
	set_user_nice(current, -19);
#if(LINUX_VERSION_CODE == KERNEL_VERSION(2,6,30))
	unlock_kernel();
#else
	spin_unlock(&ut->lock);
#endif

	/* srv.rb must wait for rx_thread starting */
	complete(&ut->thread_done);

	/* start of while loop */
	ut->loop_ops(ut);

	/* end of loop */
	ut->thread = NULL;

	complete_and_exit(&ut->thread_done, 0);
}

void wifi_audio_task_init(struct wifi_audio_task *ut, char *name,void (*loop_ops)(struct wifi_audio_task *))
{
	ut->thread = NULL;
	init_completion(&ut->thread_done);
	strncpy (ut->name, name, 16);
	ut->loop_ops = loop_ops;
}
EXPORT_SYMBOL_GPL(wifi_audio_task_init);


static void stop_wifi_audio_timer_thread(void) {
	if (wifi_audio_gpio_probe.thread != NULL) {
		send_sig(SIGKILL, wifi_audio_gpio_probe.thread, 1);
		wait_for_completion(&wifi_audio_gpio_probe.thread_done);
	}
}
int wifi_audio_start_threads(void)
{
	struct task_struct *th;
    	int err = 0;
	
	th = kthread_run(wifi_audio_thread, (void *)&wifi_audio_gpio_probe, "gpio_probe");
	if (IS_ERR(th)) {
		printk(KERN_WARNING"Unable to start control thread\n");
        		err = PTR_ERR(th);
        		goto thread_err;
	}

	/* confirm threads are starting */
	wait_for_completion(&wifi_audio_gpio_probe.thread_done);
	
	return 0;

    thread_err:
        return err;
	
}
 
void wifi_audio_timer_loop(struct wifi_audio_task *wifi_audio_gpio_probe)
{
	Init_wifi_audio_probe_timer();
	while (1) {
		if (signal_pending(current)) {
			break;
		}
	
	        if (probe_flag & WIFI_AUDIO_GPIO_PROBE_TIMEOUT)
	        {
	            wifi_audio_gpio_probe_task(0);
	            probe_flag&= ~WIFI_AUDIO_GPIO_PROBE_TIMEOUT;
	        }
		wait_event_interruptible(wifi_audio_probe_waitq,probe_flag);
	}
}


void wifi_audio_sink_init(void)
{
	wifi_audio_task_init(&wifi_audio_gpio_probe, "gpio_probe", wifi_audio_timer_loop);
	
	init_waitqueue_head(&wifi_audio_probe_waitq);
	wifi_audio_start_threads();
}

void wifi_audio_stop_threads(void)
{
   
    stop_wifi_audio_timer_thread();
}





