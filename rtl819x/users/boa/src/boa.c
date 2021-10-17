/*
 *  Boa, an http server
 *  Copyright (C) 1995 Paul Phillips <paulp@go2net.com>
 *  Some changes Copyright (C) 1996 Charles F. Randall <crandall@goldsys.com>
 *  Copyright (C) 1996-1999 Larry Doolittle <ldoolitt@boa.org>
 *  Copyright (C) 1996-2005 Jon Nelson <jnelson@boa.org>
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

/* $Id: boa.c,v 1.99.2.26 2005/02/22 14:11:29 jnelson Exp $*/

#include "boa.h"
#ifdef SERVER_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#define SSL_KEYF "/etc/privateKey.key"
#define SSL_CERTF "/etc/certificate.crt"
int server_ssl;       /*ssl socket */
int do_ssl = 1;         /*We want to actually perform all of the ssl stuff.*/
int do_sock = 1;        /*We may not want to actually connect to normal sockets*/
SSL_CTX *ctx;       /*SSL context information*/
SSL_METHOD *meth;     /*SSL method information*/
int ssl_server_port = 443;    /*The port that the server should listen on*/
/*Note that per socket ssl information is stored in */
#ifdef INET6
struct sockaddr_in6 server_sockaddr;    /* boa ssl socket address */
#else
//struct sockaddr_in ssl_server_sockaddr;   /* boa ssl socket address */
struct sockaddr_in server_sockaddr;   /* boa ssl socket address */
#endif/*INET6*/
int InitSSLStuff(void);
extern void get_ssl_request(void);
#endif

/* globals */
int backlog = SO_MAXCONN;
time_t start_time;

int debug_level = 0;
int sighup_flag = 0;            /* 1 => signal has happened, needs attention */
int sigchld_flag = 0;           /* 1 => signal has happened, needs attention */
int sigalrm_flag = 0;           /* 1 => signal has happened, needs attention */
int sigterm_flag = 0;           /* lame duck mode */
time_t current_time;
int pending_requests = 0;
unsigned char boa_start=0;

extern const char *config_file_name;

/* static to boa.c */
static void usage(const char *programname);
static void parse_commandline(int argc, char *argv[]);
static void fixup_server_root(void);
static int create_server_socket(void);
static void drop_privs(void);

static int sock_opt = 1;
static int do_fork = 1;

int main(int argc, char *argv[])
{
	int server_s;               /* boa socket */
	pid_t pid;

	/* set umask to u+rw, u-x, go-rwx */
	/* according to the man page, umask always succeeds */
	umask(077);

	/* but first, update timestamp, because log_error_time uses it */
	(void) time(&current_time);

	/* set timezone right away */
	tzset();

	{
		int devnullfd = -1;
		devnullfd = open("/dev/null", 0);

		/* make STDIN point to /dev/null */
		if (devnullfd == -1) {
			DIE("can't open /dev/null");
		}

		if (dup2(devnullfd, STDIN_FILENO) == -1) {
			DIE("can't dup2 /dev/null to STDIN_FILENO");
		}

		(void) close(devnullfd);
	}

	parse_commandline(argc, argv);
	fixup_server_root();

#ifdef SUPPORT_ASP
	extern void asp_init(int argc,char **argv); // davidhsu
	asp_init(argc,argv);
#endif

	read_config_files();
	create_common_env();
	open_logs();

#if SERVER_SSL
	if (do_ssl) {
		if (InitSSLStuff() != 1) {
			/*TO DO - emit warning the SSL stuff will not work*/
			printf("Failure initialising SSL support - ");
			if (do_sock == 0) {
				printf("    normal sockets disabled, so exiting");fflush(NULL);
				return 0;
			} else {
				printf("    supporting normal (unencrypted) sockets only");fflush(NULL);
				do_sock = 2;
			}
		}
	}else
		do_sock = 2;
#endif

#if SERVER_SSL
	if(do_sock){
		//printf("<%s:%d>do_sock=%d, do_ssl=%d\n", __FUNCTION__, __LINE__, do_sock, do_ssl);
#endif /*SERVER_SSL*/
		server_s = create_server_socket();
#if SERVER_SSL
	}
#endif
	//init_signals(); //Brad comment out, move to later 
	//printf("<%s:%d>\n", __FUNCTION__, __LINE__);
	build_needs_escape();

	/* background ourself */
	//printf("<%s:%d>do_fork=%d\n", __FUNCTION__, __LINE__, do_fork);
	if (do_fork) {
		pid = fork();
	} else {
		pid = getpid();
	}

	//printf("<%s:%d>pid=%d\n", __FUNCTION__, __LINE__,pid);
	switch (pid) {
		case -1:
			/* error */
			perror("fork/getpid");
			exit(EXIT_FAILURE);
		case 0:
			/* child, success */
			break;
		default:
			/* parent, success */
			//printf("<%s:%d>pid_file=%s\n", __FUNCTION__,__LINE__,pid_file);
			if (pid_file != NULL) {
				FILE *PID_FILE = fopen(pid_file, "w");
				if (PID_FILE != NULL) {
					//printf("<%s:%d>PID_FILE=%d\n",__FUNCTION__,__LINE__,PID_FILE);
					fprintf(PID_FILE, "%d\n", pid);
					fclose(PID_FILE);
				} else {
					perror("fopen pid file");
				}
			}

			//printf("<%s:%d>do_fork=%d\n",__FUNCTION__,__LINE__,do_fork);
			if (do_fork)
				exit(EXIT_SUCCESS);//exit the process
			break;
	}

	boa_start = 1;
#ifdef VOIP_SUPPORT
	web_voip_init();
#endif 
	//printf("<%s:%d>\n", __FUNCTION__, __LINE__);
	init_signals(); //Brad move here
	drop_privs();
	/* main loop */
	timestamp();

	status.requests = 0;
	status.errors = 0;

	start_time = current_time;
	alarm(1);
	//printf("<%s:%d>\n", __FUNCTION__, __LINE__);
	loop(server_s);
	return 0;
}

static void usage(const char *programname)
{
	fprintf(stderr,
			"Usage: %s [-c serverroot] [-d] [-f configfile] [-r chroot]%s\n",
			programname,
#ifndef DISABLE_DEBUG
			" [-l debug_level]"
#else
			""
#endif
	       );
#ifndef DISABLE_DEBUG
	print_debug_usage();
#endif
	exit(EXIT_FAILURE);

}

static void parse_commandline(int argc, char *argv[])
{
	int c;                      /* command line arg */

	while ((c = getopt(argc, argv, "c:dl:f:r:")) != -1) {
		switch (c) {
			case 'c':
				if (server_root)
					free(server_root);
				server_root = strdup(optarg);
				if (!server_root) {
					perror("strdup (for server_root)");
					exit(EXIT_FAILURE);
				}
				break;
			case 'd':
				do_fork = 0;
				break;
#if SERVER_SSL
			case 'n':
				do_sock = 0;    /*We don't want to do normal sockets*/
				break;
			case 's':
				do_ssl = 0;   /*We don't want to do ssl sockets*/
				break;
#endif /*SERVER_SSL*/
			case 'f':
				config_file_name = optarg;
				break;
			case 'r':
				if (chdir(optarg) == -1) {
					log_error_time();
					perror("chdir (to chroot)");
					exit(EXIT_FAILURE);
				}
				if (chroot(optarg) == -1) {
					log_error_time();
					perror("chroot");
					exit(EXIT_FAILURE);
				}
				if (chdir("/") == -1) {
					log_error_time();
					perror("chdir (after chroot)");
					exit(EXIT_FAILURE);
				}
				break;
#ifndef DISABLE_DEBUG
			case 'l':
				parse_debug(optarg);
				break;
#endif
			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}
}

static int create_server_socket(void)
{
	int server_s;

	server_s = socket(SERVER_PF, SOCK_STREAM, IPPROTO_TCP);
	if (server_s == -1) {
		DIE("unable to create socket");
	}

	/* server socket is nonblocking */
	if (set_nonblock_fd(server_s) == -1) {
		DIE("fcntl: unable to set server socket to nonblocking");
	}

	/* close server socket on exec so CGIs can't write to it */
	if (fcntl(server_s, F_SETFD, 1) == -1) {
		DIE("can't set close-on-exec on server socket!");
	}

	/* reuse socket addr */
	if ((setsockopt(server_s, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
					sizeof (sock_opt))) == -1) {
		DIE("setsockopt");
	}

	/* Internet family-specific code encapsulated in bind_server()  */
	if (bind_server(server_s, server_ip, server_port) == -1) {
		DIE("unable to bind");
	}

	/* listen: large number just in case your kernel is nicely tweaked */
	if (listen(server_s, backlog) == -1) {
		DIE("unable to listen");
	}
	return server_s;
}

static void drop_privs(void)
{
	// davidhsu
	server_gid = getgid();
	server_uid = getuid();

#if 0	
	/* give away our privs if we can */
	if (getuid() == 0) {
		struct passwd *passwdbuf;
		passwdbuf = getpwuid(server_uid);
		if (passwdbuf == NULL) {
			DIE("getpwuid");
		}
		if (initgroups(passwdbuf->pw_name, passwdbuf->pw_gid) == -1) {
			DIE("initgroups");
		}
		if (setgid(server_gid) == -1) {
			DIE("setgid");
		}
		if (setuid(server_uid) == -1) {
			DIE("setuid");
		}
		/* test for failed-but-return-was-successful setuid
		 * http://www.securityportal.com/list-archive/bugtraq/2000/Jun/0101.html
		 */
		if (server_uid != 0 && setuid(0) != -1) {
			DIE("icky Linux kernel bug!");
		}
	} else {
		if (server_gid || server_uid) {
			log_error_time();
			fprintf(stderr, "Warning: "
					"Not running as root: no attempt to change"
					" to uid %u gid %u\n", server_uid, server_gid);
		}
		server_gid = getgid();
		server_uid = getuid();
	}
#endif    
}

/*
 * Name: fixup_server_root
 *
 * Description: Makes sure the server root is valid.
 *
 */

static void fixup_server_root()
{
	if (!server_root) {
#ifdef SERVER_ROOT
		server_root = strdup(SERVER_ROOT);
		if (!server_root) {
			perror("strdup (SERVER_ROOT)");
			exit(EXIT_FAILURE);
		}
#else
		fputs("boa: don't know where server root is.  Please #define "
				"SERVER_ROOT in boa.h\n"
				"and recompile, or use the -c command line option to "
				"specify it.\n", stderr);
		exit(EXIT_FAILURE);
#endif
	}

	if (chdir(server_root) == -1) {
		fprintf(stderr, "Could not chdir to \"%s\": aborting\n",
				server_root);
		exit(EXIT_FAILURE);
	}
}

#ifdef SERVER_SSL
int InitSSLStuff(void)
{
	printf("Enabling SSL security system\n");
#ifdef INET6
	if ((server_ssl = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		DIE(NO_CREATE_SOCKET);
		return 0;
	}
#else
	if ((server_ssl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
		printf("Couldn't create socket for ssl");
		DIE(NO_CREATE_SOCKET);
		return 0;
	}
#endif /*INET6*/

	/* server socket is nonblocking */
	if (fcntl(server_ssl, F_SETFL, NOBLOCK) == -1){
		printf("%s, %i:Couldn't fcntl", __FILE__, __LINE__);
		DIE(NO_FCNTL);
		return 0;
	}

	if ((setsockopt(server_ssl, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt,
		sizeof(sock_opt))) == -1){
		printf("%s, %i:Couldn't sockopt", __FILE__,__LINE__);
		DIE(NO_SETSOCKOPT);
		return 0;
	}

	/* internet socket */
#ifdef INET6
	server_sockaddr.sin6_family = AF_INET6;
	memcpy(&server_sockaddr.sin6_addr,&in6addr_any,sizeof(in6addr_any));
	server_sockaddr.sin6_port = htons(ssl_server_port);
#else
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_sockaddr.sin_port = htons(ssl_server_port);
#endif

	if (bind(server_ssl, (struct sockaddr *) &server_sockaddr,
		sizeof(server_sockaddr)) == -1){
		printf("Couldn't bind ssl to port %d", ntohs(server_sockaddr.sin_port));
		DIE(NO_BIND);
		return 0;
	}

	/* listen: large number just in case your kernel is nicely tweaked */
	if (listen(server_ssl, backlog) == -1){
		DIE(NO_LISTEN);
		return 0;		
	}

	if (server_ssl > max_fd)
		max_fd = server_ssl;

	/*Init all of the ssl stuff*/
//	i don't know why this line is commented out... i found it like that - damion may-02 
/*	SSL_load_error_strings();*/
	SSLeay_add_ssl_algorithms();
	meth = SSLv23_server_method();
	if(meth == NULL){
		ERR_print_errors_fp(stderr);
		printf("Couldn't create the SSL method");
		DIE(NO_SSL);
		return 0;
	}
	ctx = SSL_CTX_new(meth);
	if(!ctx){
		printf( "Couldn't create a connection context\n");
		ERR_print_errors_fp(stderr);
		DIE(NO_SSL);
		return 0;
	}

	if (SSL_CTX_use_certificate_file(ctx, SSL_CERTF, SSL_FILETYPE_PEM) <= 0) {
		printf("Failure reading SSL certificate file: %s",SSL_CERTF);fflush(NULL);
		close(server_ssl);
		return 0;
	}
	printf("Loaded SSL certificate file: %s\n",SSL_CERTF);fflush(NULL);

	if (SSL_CTX_use_PrivateKey_file(ctx, SSL_KEYF, SSL_FILETYPE_PEM) <= 0) {
		printf("Failure reading private key file: %s",SSL_KEYF);fflush(NULL);
		close(server_ssl);
		return 0;
	}
	printf("Opened private key file: %s",SSL_KEYF);fflush(NULL);

	if (!SSL_CTX_check_private_key(ctx)) {
		printf( "Private key does not match the certificate public key\n");fflush(NULL);
		close(server_ssl);
		return 0;
	}

	/*load and check that the key files are appropriate.*/
	printf("SSL security system enabled, server_ssl=%d\n",server_ssl);
	return 1;
}
#endif
