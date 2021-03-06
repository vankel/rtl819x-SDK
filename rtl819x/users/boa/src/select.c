/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <paulp@go2net.com>
 *  Some changes Copyright (C) 1996 Charles F. Randall <crandall@goldsys.com>
 *  Copyright (C) 1996-1999 Larry Doolittle <ldoolitt@boa.org>
 *  Copyright (C) 1996-2003 Jon Nelson <jnelson@boa.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 1, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* $Id: select.c,v 1.1.2.17 2005/02/22 14:11:29 jnelson Exp $*/

/* algorithm:
 * handle any signals
 * if we still want to accept new connections, add the server to the
 * list.
 * if there are any blocked requests or the we are still accepting new
 * connections, determine appropriate timeout and select, then move
 * blocked requests back into the active list.
 * handle active connections
 * repeat
 */



#include "boa.h"

#ifdef SERVER_SSL
#include <openssl/ssl.h>
extern int server_ssl;
extern int do_sock;
extern SSL_CTX *ctx;
#define MAX(a,b)	((a)>(b))?(a):(b)
#endif

extern int firmware_len;
extern char *firmware_data;
static void fdset_update(void);
fd_set block_read_fdset;
fd_set block_write_fdset;
int max_fd = 0;
extern int isFWUPGRADE;
extern int isREBOOTASP;
extern int isCFGUPGRADE;

int req_after_upgrade=0;
int last_req_after_upgrade=0;
int confirm_last_req=0;
extern int reboot_time;
void loop(int server_s)
{
	//printf("<%s:%d>enter\n", __FUNCTION__,__LINE__);
	FD_ZERO(BOA_READ);
	FD_ZERO(BOA_WRITE);

	max_fd = -1;

	while (1) {
		/* handle signals here */
		if (sighup_flag)
			sighup_run();
		if (sigchld_flag)
			sigchld_run();
		if (sigalrm_flag)
			sigalrm_run();

		if (sigterm_flag) {
			/* sigterm_flag:
			 * 1. caught, unprocessed.
			 * 2. caught, stage 1 processed
			 */
			if (sigterm_flag == 1) {
				sigterm_stage1_run();
				BOA_FD_CLR(req, server_s, BOA_READ);
				close(server_s);
				/* make sure the server isn't in the block list */
				server_s = -1;
			}
			if (sigterm_flag == 2 && !request_ready && !request_block) {
				sigterm_stage2_run(); /* terminal */
			}
		} else {
			if (total_connections > max_connections) {
				/* FIXME: for poll we don't subtract 20. why? */
				BOA_FD_CLR(req, server_s, BOA_READ);
			} else {
				BOA_FD_SET(req, server_s, BOA_READ); /* server always set */
			}
		}
		if (isREBOOTASP == 1) {
			if(last_req_after_upgrade != req_after_upgrade){
				last_req_after_upgrade = req_after_upgrade;
			}
		}

		pending_requests = 0;
		/* max_fd is > 0 when something is blocked */

#ifdef SERVER_SSL
		//printf("<%s:%d>do_sock=%d\n",__FUNCTION__,__LINE__,do_sock);
		if (do_sock < 2)
			max_fd = MAX(server_ssl, max_fd);
		fdset_update();
#endif

		//printf("max_fd=%d, server_ssl=%d\n",max_fd, server_ssl);
		if (max_fd) {
			struct timeval req_timeout; /* timeval for select */

			req_timeout.tv_sec = (request_ready ? 0 : default_timeout);
			req_timeout.tv_usec = 0l; /* reset timeout */

			if (select(max_fd + 1, BOA_READ,
						BOA_WRITE, NULL,
						(request_ready || request_block ?
						 &req_timeout : NULL)) == -1) 
				{
				//printf("<%s:%d>errno=%d\n",__FUNCTION__,__LINE__,errno);
				/* what is the appropriate thing to do here on EBADF */
				if (errno == EINTR) {
					//fprintf(stderr,"####%s:%d isFWUPGRADE=%d isREBOOTASP=%d###\n",  __FILE__, __LINE__ ,isFWUPGRADE , isREBOOTASP);
					//fprintf(stderr,"####%s:%d last_req_after_upgrade=%d req_after_upgrade=%d confirm_last_req=%d###\n",  __FILE__, __LINE__ ,last_req_after_upgrade , req_after_upgrade, confirm_last_req);
					if (isFWUPGRADE !=0 && isREBOOTASP == 1 ) {
						if (last_req_after_upgrade == req_after_upgrade)
							confirm_last_req++;
						//printf("<%s:%d>confirm_last_req=%d\n",__FUNCTION__,__LINE__,confirm_last_req);
						if (confirm_last_req >3)
							goto ToUpgrade;
					} else if(isCFGUPGRADE ==2  && isREBOOTASP == 1 ) {
						goto ToReboot;
					}
					else if (isFWUPGRADE ==0 && isREBOOTASP == 1) {
						if (last_req_after_upgrade == req_after_upgrade)
							confirm_last_req++;
						if (confirm_last_req >3) {
							isFWUPGRADE = 0;
							isREBOOTASP = 0;
							//isFAKEREBOOT = 0;
							confirm_last_req=0;
						}
					}
#if defined(CONFIG_APP_FWD)
					{
						extern int isCountDown;
						//printf("<%s:%d>isCountDown=%d\n", __FUNCTION__, __LINE__, isCountDown);
						if (isCountDown == 2) {
							goto ToUpgrade;
						}
					}
#endif
					continue;       /* while(1) */                
				}
				else if (errno != EBADF) {
					DIE("select");
				}
			}//end if select
			/* FIXME: optimize for when select returns 0 (timeout).
			 * Thus avoiding many operations in fdset_update
			 * and others.
			 */
#ifdef SERVER_SSL
			//printf("<%s:%d>do_sock=%d\n",__FUNCTION__,__LINE__,do_sock);
			if(do_sock){
				if (!sigterm_flag && FD_ISSET(server_s, BOA_READ)) {
					pending_requests = 1;
				}
			}
#else
			if (!sigterm_flag && FD_ISSET(server_s, BOA_READ)) {
				pending_requests = 1;
			}
#endif

#ifdef SERVER_SSL
			//printf("<%s:%d>do_sock=%d, server_ssl=%d\n",__FUNCTION__,__LINE__,do_sock, server_ssl);
			if (do_sock < 2) {
				//printf("<%s:%d>\n",__FUNCTION__,__LINE__);
				if(FD_ISSET(server_ssl, BOA_READ)){ /*If we have the main SSL server socket*/
					//printf("SSL request received!!\n");
					get_ssl_request();
				}   
			}   
#endif /*SERVER_SSL*/

			time(&current_time); /* for "new" requests if we've been in
					      * select too long */
			/* if we skip this section (for example, if max_fd == 0),
			 * then we aren't listening anyway, so we can't accept
			 * new conns.  Don't worry about it.
			 */
		}//if max_fd

		/* reset max_fd */
		max_fd = -1;

		if (request_block) {
			/* move selected req's from request_block to request_ready */
			fdset_update();
		}

		/* any blocked req's move from request_ready to request_block */
		if (pending_requests || request_ready) {
			if (isFWUPGRADE !=0 && isREBOOTASP == 1 ){
				req_after_upgrade++;
			}else if(isFWUPGRADE ==0 && isREBOOTASP == 1){
				req_after_upgrade++;
			}
			process_requests(server_s);
			continue;
		}

ToUpgrade:
		printf("ToUpgrade\n");
		if (isFWUPGRADE !=0 && isREBOOTASP == 1 ) {
			char buffer[200];
			//fprintf(stderr,"\r\n [%s-%u] FirmwareUpgrade start",__FILE__,__LINE__);
			FirmwareUpgrade(firmware_data, firmware_len, 0, buffer);
			//fprintf(stderr,"\r\n [%s-%u] FirmwareUpgrade end",__FILE__,__LINE__);
			//system("echo 7 > /proc/gpio"); // disable system LED
			isFWUPGRADE=0;
			isREBOOTASP=0;
			//reboot_time = 5;
			break;
		}

ToReboot:
		printf("ToReboot\n");
		if(isCFGUPGRADE == 2 && isREBOOTASP ==1) {
			isCFGUPGRADE=0;
			isREBOOTASP=0;
			system("reboot");
			for(;;);
		}
		
		//printf("<%s:%d>end while\n",__FUNCTION__,__LINE__);
	}//end while
}

/*
 * Name: fdset_update
 *
 * Description: iterate through the blocked requests, checking whether
 * that file descriptor has been set by select.  Update the fd_set to
 * reflect current status.
 *
 * Here, we need to do some things:
 *  - keepalive timeouts simply close
 *    (this is special:: a keepalive timeout is a timeout where
 keepalive is active but nothing has been read yet)
 *  - regular timeouts close + error
 *  - stuff in buffer and fd ready?  write it out
 *  - fd ready for other actions?  do them
 */

static void fdset_update(void)
{
	//printf("%s\n",__FUNCTION__);
	request *current, *next;

	time(&current_time);
	for (current = request_block; current; current = next) {
		time_t time_since = current_time - current->time_last;
		next = current->next;

		/* hmm, what if we are in "the middle" of a request and not
		 * just waiting for a new one... perhaps check to see if anything
		 * has been read via header position, etc... */
		if (current->kacount < ka_max && /* we *are* in a keepalive */
				(time_since >= ka_timeout) && /* ka timeout */
				!current->logline) { /* haven't read anything yet */
			log_error_doc(current);
			fputs("connection timed out\n", stderr);
			current->status = TIMED_OUT; /* connection timed out */
		} else if (time_since > REQUEST_TIMEOUT) {
			log_error_doc(current);
			fputs("connection timed out\n", stderr);
			current->status = TIMED_OUT; /* connection timed out */
		}
		if (current->buffer_end && /* there is data to write */
				current->status < DONE) {
			if (FD_ISSET(current->fd, BOA_WRITE))
				ready_request(current);
			else {
				BOA_FD_SET(current, current->fd, BOA_WRITE);
			}
		} else {
			switch (current->status) {
				case IOSHUFFLE:
#ifndef HAVE_SENDFILE
					if (current->buffer_end - current->buffer_start == 0) {
						if (FD_ISSET(current->data_fd, BOA_READ))
							ready_request(current);
						break;
					}
#endif
				case WRITE:
				case PIPE_WRITE:
					if (FD_ISSET(current->fd, BOA_WRITE))
						ready_request(current);
					else {
						BOA_FD_SET(current, current->fd, BOA_WRITE);
					}
					break;
				case BODY_WRITE:
					// davidhsu ------------------------------
#ifndef NEW_POST
					if (FD_ISSET(current->post_data_fd, BOA_WRITE))
						ready_request(current);
					else {
						BOA_FD_SET(current, current->post_data_fd,
								BOA_WRITE);
					}
#else
					ready_request(current);
#endif
					//--------------------------------------				
					break;
				case PIPE_READ:
					if (FD_ISSET(current->data_fd, BOA_READ))
						ready_request(current);
					else {
						BOA_FD_SET(current, current->data_fd,
								BOA_READ);
					}
					break;
				case DONE:
					if (FD_ISSET(current->fd, BOA_WRITE))
						ready_request(current);
					else {
						BOA_FD_SET(current, current->fd, BOA_WRITE);
					}
					break;
				case TIMED_OUT:
				case DEAD:
					ready_request(current);
					break;
				default:
					if (FD_ISSET(current->fd, BOA_READ))
						ready_request(current);
					else {
						BOA_FD_SET(current, current->fd, BOA_READ);
					}
					break;
			}
		}
		current = next;
	}//end for

#ifdef SERVER_SSL
	//printf("<%s:%d>do_sock=%d\n",__FUNCTION__,__LINE__,do_sock);
	if (do_sock < 2) {
		FD_SET(server_ssl, BOA_READ);
		//printf("Added server_ssl to fdset\n");
	} 	
#endif
}
