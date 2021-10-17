/*
 *  This file is for reverse_sendfile.c to obtain 
 *  the kernel structure member.
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/file.h>
#include <linux/backing-dev.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/errno.h>
#include <net/sock.h>
#include <linux/freezer.h>
#include <linux/percpu.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
#define vfs_check_frozen(sb, level) do { \
	freezer_do_not_count(); \
	wait_event((sb)->s_wait_unfrozen, ((sb)->s_frozen < (level))); \
	freezer_count(); \
} while (0)
#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,30)
#define skb_walk_frags(skb, iter)	\
	for (iter = skb_shinfo(skb)->frag_list; iter; iter = iter->next)
#endif

void rtl_vfs_check_frozen(struct super_block *sb, int level)
{	
	vfs_check_frozen(sb, level);
}

struct address_space * rtl_get_file_mapping(struct file *file)
{
	return file->f_mapping;
}

fmode_t rtl_get_file_mode(struct file *file)
{
	return file->f_mode;
}

loff_t* rtl_get_file_pos_addr(struct file *file)
{
	return &file->f_pos;
}

void rtl_inode_mutex_lock(struct inode *inode)
{
	mutex_lock(&inode->i_mutex);
}

void rtl_inode_mutex_unlock(struct inode *inode)
{
	mutex_unlock(&inode->i_mutex);
}

struct super_block* rtl_get_inode_sb(struct inode *inode)
{
	return inode->i_sb;
}

umode_t rtl_get_inode_mode(struct inode *inode)
{
	return inode->i_mode;
}

long rtl_get_sk_rcvtimeo(struct socket *sock)
{
	return sock->sk->sk_rcvtimeo;
}

void rtl_set_sk_rcvtimeo(struct socket *sock, long rcvtimeo)
{
	sock->sk->sk_rcvtimeo = rcvtimeo;
}

unsigned char *rtl_get_skb_data_common(struct sk_buff *skb)
{
	return skb->data;
}

unsigned int rtl_get_skb_len_common(struct sk_buff *skb)
{
	return skb->len;
}

