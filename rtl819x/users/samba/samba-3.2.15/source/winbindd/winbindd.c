/* 
   Unix SMB/CIFS implementation.

   Winbind daemon for ntdom nss module

   Copyright (C) by Tim Potter 2000-2002
   Copyright (C) Andrew Tridgell 2002
   Copyright (C) Jelmer Vernooij 2003
   Copyright (C) Volker Lendecke 2004
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "includes.h"
#include "winbindd.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_WINBIND

bool opt_nocache = False;
static bool interactive = False;

extern bool override_logfile;

struct event_context *winbind_event_context(void)
{
	static struct event_context *ctx;

	if (!ctx && !(ctx = event_context_init(NULL))) {
		smb_panic("Could not init winbind event context");
	}
	return ctx;
}

struct messaging_context *winbind_messaging_context(void)
{
	static struct messaging_context *ctx;

	if (!ctx && !(ctx = messaging_init(NULL, server_id_self(),
					   winbind_event_context()))) {
		smb_panic("Could not init winbind messaging context");
	}
	return ctx;
}

/* Reload configuration */

static bool reload_services_file(const char *logfile)
{
	bool ret;

	if (lp_loaded()) {
		const char *fname = lp_configfile();

		if (file_exist(fname,NULL) && !strcsequal(fname,get_dyn_CONFIGFILE())) {
			set_dyn_CONFIGFILE(fname);
		}
	}

	/* if this is a child, restore the logfile to the special
	   name - <domain>, idmap, etc. */
	if (logfile && *logfile) {
		lp_set_logfile(logfile);
	}

	reopen_logs();
	ret = lp_load(get_dyn_CONFIGFILE(),False,False,True,True);

	reopen_logs();
	load_interfaces();

	return(ret);
}


/**************************************************************************** **
 Handle a fault..
 **************************************************************************** */

static void fault_quit(void)
{
	dump_core();
}

static void winbindd_status(void)
{
	struct winbindd_cli_state *tmp;

	DEBUG(0, ("winbindd status:\n"));

	/* Print client state information */
	
	DEBUG(0, ("\t%d clients currently active\n", winbindd_num_clients()));
	
	if (DEBUGLEVEL >= 2 && winbindd_num_clients()) {
		DEBUG(2, ("\tclient list:\n"));
		for(tmp = winbindd_client_list(); tmp; tmp = tmp->next) {
			DEBUGADD(2, ("\t\tpid %lu, sock %d\n",
				  (unsigned long)tmp->pid, tmp->sock));
		}
	}
}

/* Print winbindd status to log file */

static void print_winbindd_status(void)
{
	winbindd_status();
}

/* Flush client cache */

static void flush_caches(void)
{
	/* We need to invalidate cached user list entries on a SIGHUP 
           otherwise cached access denied errors due to restrict anonymous
           hang around until the sequence number changes. */

	if (!wcache_invalidate_cache()) {
		DEBUG(0, ("invalidating the cache failed; revalidate the cache\n"));
		if (!winbindd_cache_validate_and_initialize()) {
			exit(1);
		}
	}
}

/* Handle the signal by unlinking socket and exiting */

static void terminate(bool is_parent)
{
	if (is_parent) {
		/* When parent goes away we should
		 * remove the socket file. Not so
		 * when children terminate.
		 */ 
		char *path = NULL;

		if (asprintf(&path, "%s/%s",
			get_winbind_pipe_dir(), WINBINDD_SOCKET_NAME) > 0) {
			unlink(path);
			SAFE_FREE(path);
		}
	}

	idmap_close();
	
	trustdom_cache_shutdown();

#if 0
	if (interactive) {
		TALLOC_CTX *mem_ctx = talloc_init("end_description");
		char *description = talloc_describe_all(mem_ctx);

		DEBUG(3, ("tallocs left:\n%s\n", description));
		talloc_destroy(mem_ctx);
	}
#endif

	exit(0);
}

static SIG_ATOMIC_T do_sigterm = 0;

static void termination_handler(int signum)
{
	do_sigterm = 1;
	sys_select_signal(signum);
}

static SIG_ATOMIC_T do_sigusr2 = 0;

static void sigusr2_handler(int signum)
{
	do_sigusr2 = 1;
	sys_select_signal(SIGUSR2);
}

static SIG_ATOMIC_T do_sighup = 0;

static void sighup_handler(int signum)
{
	do_sighup = 1;
	sys_select_signal(SIGHUP);
}

static SIG_ATOMIC_T do_sigchld = 0;

static void sigchld_handler(int signum)
{
	do_sigchld = 1;
	sys_select_signal(SIGCHLD);
}

/* React on 'smbcontrol winbindd reload-config' in the same way as on SIGHUP*/
static void msg_reload_services(struct messaging_context *msg,
				void *private_data,
				uint32_t msg_type,
				struct server_id server_id,
				DATA_BLOB *data)
{
        /* Flush various caches */
	flush_caches();
	reload_services_file((const char *) private_data);
}

/* React on 'smbcontrol winbindd shutdown' in the same way as on SIGTERM*/
static void msg_shutdown(struct messaging_context *msg,
			 void *private_data,
			 uint32_t msg_type,
			 struct server_id server_id,
			 DATA_BLOB *data)
{
	do_sigterm = 1;
}


static void winbind_msg_validate_cache(struct messaging_context *msg_ctx,
				       void *private_data,
				       uint32_t msg_type,
				       struct server_id server_id,
				       DATA_BLOB *data)
{
	uint8 ret;
	pid_t child_pid;
	struct sigaction act;
	struct sigaction oldact;

	DEBUG(10, ("winbindd_msg_validate_cache: got validate-cache "
		   "message.\n"));

	/*
	 * call the validation code from a child:
	 * so we don't block the main winbindd and the validation
	 * code can safely use fork/waitpid...
	 */
	CatchChild();
	child_pid = sys_fork();

	if (child_pid == -1) {
		DEBUG(1, ("winbind_msg_validate_cache: Could not fork: %s\n",
			  strerror(errno)));
		return;
	}

	if (child_pid != 0) {
		/* parent */
		DEBUG(5, ("winbind_msg_validate_cache: child created with "
			  "pid %d.\n", child_pid));
		return;
	}

	/* child */

	/* install default SIGCHLD handler: validation code uses fork/waitpid */
	ZERO_STRUCT(act);
	act.sa_handler = SIG_DFL;
#ifdef SA_RESTART
	/* We *want* SIGALRM to interrupt a system call. */
	act.sa_flags = SA_RESTART;
#endif
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask,SIGCHLD);
	sigaction(SIGCHLD,&act,&oldact);

	ret = (uint8)winbindd_validate_cache_nobackup();
	DEBUG(10, ("winbindd_msg_validata_cache: got return value %d\n", ret));
	messaging_send_buf(msg_ctx, server_id, MSG_WINBIND_VALIDATE_CACHE, &ret,
			   (size_t)1);
	_exit(0);
}

static struct winbindd_dispatch_table {
	enum winbindd_cmd cmd;
	void (*fn)(struct winbindd_cli_state *state);
	const char *winbindd_cmd_name;
} dispatch_table[] = {
	
	/* User functions */

	{ WINBINDD_GETPWNAM, winbindd_getpwnam, "GETPWNAM" },
	{ WINBINDD_GETPWUID, winbindd_getpwuid, "GETPWUID" },

	{ WINBINDD_SETPWENT, winbindd_setpwent, "SETPWENT" },
	{ WINBINDD_ENDPWENT, winbindd_endpwent, "ENDPWENT" },
	{ WINBINDD_GETPWENT, winbindd_getpwent, "GETPWENT" },

	{ WINBINDD_GETGROUPS, winbindd_getgroups, "GETGROUPS" },
	{ WINBINDD_GETUSERSIDS, winbindd_getusersids, "GETUSERSIDS" },
	{ WINBINDD_GETUSERDOMGROUPS, winbindd_getuserdomgroups,
	  "GETUSERDOMGROUPS" },

	/* Group functions */

	{ WINBINDD_GETGRNAM, winbindd_getgrnam, "GETGRNAM" },
	{ WINBINDD_GETGRGID, winbindd_getgrgid, "GETGRGID" },
	{ WINBINDD_SETGRENT, winbindd_setgrent, "SETGRENT" },
	{ WINBINDD_ENDGRENT, winbindd_endgrent, "ENDGRENT" },
	{ WINBINDD_GETGRENT, winbindd_getgrent, "GETGRENT" },
	{ WINBINDD_GETGRLST, winbindd_getgrent, "GETGRLST" },

	/* PAM auth functions */

	{ WINBINDD_PAM_AUTH, winbindd_pam_auth, "PAM_AUTH" },
	{ WINBINDD_PAM_AUTH_CRAP, winbindd_pam_auth_crap, "AUTH_CRAP" },
	{ WINBINDD_PAM_CHAUTHTOK, winbindd_pam_chauthtok, "CHAUTHTOK" },
	{ WINBINDD_PAM_LOGOFF, winbindd_pam_logoff, "PAM_LOGOFF" },
	{ WINBINDD_PAM_CHNG_PSWD_AUTH_CRAP, winbindd_pam_chng_pswd_auth_crap, "CHNG_PSWD_AUTH_CRAP" },

	/* Enumeration functions */

	{ WINBINDD_LIST_USERS, winbindd_list_users, "LIST_USERS" },
	{ WINBINDD_LIST_GROUPS, winbindd_list_groups, "LIST_GROUPS" },
	{ WINBINDD_LIST_TRUSTDOM, winbindd_list_trusted_domains,
	  "LIST_TRUSTDOM" },
	{ WINBINDD_SHOW_SEQUENCE, winbindd_show_sequence, "SHOW_SEQUENCE" },

	/* SID related functions */

	{ WINBINDD_LOOKUPSID, winbindd_lookupsid, "LOOKUPSID" },
	{ WINBINDD_LOOKUPNAME, winbindd_lookupname, "LOOKUPNAME" },
	{ WINBINDD_LOOKUPRIDS, winbindd_lookuprids, "LOOKUPRIDS" },

	/* Lookup related functions */

	{ WINBINDD_SID_TO_UID, winbindd_sid_to_uid, "SID_TO_UID" },
	{ WINBINDD_SID_TO_GID, winbindd_sid_to_gid, "SID_TO_GID" },
	{ WINBINDD_UID_TO_SID, winbindd_uid_to_sid, "UID_TO_SID" },
	{ WINBINDD_GID_TO_SID, winbindd_gid_to_sid, "GID_TO_SID" },
#if 0   /* DISABLED until we fix the interface in Samba 3.0.26 --jerry */
	{ WINBINDD_SIDS_TO_XIDS, winbindd_sids_to_unixids, "SIDS_TO_XIDS" },
#endif  /* end DISABLED */
	{ WINBINDD_ALLOCATE_UID, winbindd_allocate_uid, "ALLOCATE_UID" },
	{ WINBINDD_ALLOCATE_GID, winbindd_allocate_gid, "ALLOCATE_GID" },
	{ WINBINDD_SET_MAPPING, winbindd_set_mapping, "SET_MAPPING" },
	{ WINBINDD_SET_HWM, winbindd_set_hwm, "SET_HWMS" },

	/* Miscellaneous */

	{ WINBINDD_CHECK_MACHACC, winbindd_check_machine_acct, "CHECK_MACHACC" },
	{ WINBINDD_PING, winbindd_ping, "PING" },
	{ WINBINDD_INFO, winbindd_info, "INFO" },
	{ WINBINDD_INTERFACE_VERSION, winbindd_interface_version,
	  "INTERFACE_VERSION" },
	{ WINBINDD_DOMAIN_NAME, winbindd_domain_name, "DOMAIN_NAME" },
	{ WINBINDD_DOMAIN_INFO, winbindd_domain_info, "DOMAIN_INFO" },
	{ WINBINDD_NETBIOS_NAME, winbindd_netbios_name, "NETBIOS_NAME" },
	{ WINBINDD_PRIV_PIPE_DIR, winbindd_priv_pipe_dir,
	  "WINBINDD_PRIV_PIPE_DIR" },
	{ WINBINDD_GETDCNAME, winbindd_getdcname, "GETDCNAME" },
	{ WINBINDD_DSGETDCNAME, winbindd_dsgetdcname, "DSGETDCNAME" },

	/* Credential cache access */
	{ WINBINDD_CCACHE_NTLMAUTH, winbindd_ccache_ntlm_auth, "NTLMAUTH" },

	/* WINS functions */

	{ WINBINDD_WINS_BYNAME, winbindd_wins_byname, "WINS_BYNAME" },
	{ WINBINDD_WINS_BYIP, winbindd_wins_byip, "WINS_BYIP" },
	
	/* End of list */

	{ WINBINDD_NUM_CMDS, NULL, "NONE" }
};

static void process_request(struct winbindd_cli_state *state)
{
	struct winbindd_dispatch_table *table = dispatch_table;

	/* Free response data - we may be interrupted and receive another
	   command before being able to send this data off. */

	SAFE_FREE(state->response.extra_data.data);  

	ZERO_STRUCT(state->response);

	state->response.result = WINBINDD_PENDING;
	state->response.length = sizeof(struct winbindd_response);

	state->mem_ctx = talloc_init("winbind request");
	if (state->mem_ctx == NULL)
		return;

	/* Remember who asked us. */
	state->pid = state->request.pid;

	/* Process command */

	for (table = dispatch_table; table->fn; table++) {
		if (state->request.cmd == table->cmd) {
			DEBUG(10,("process_request: request fn %s\n",
				  table->winbindd_cmd_name ));
			table->fn(state);
			break;
		}
	}

	if (!table->fn) {
		DEBUG(10,("process_request: unknown request fn number %d\n",
			  (int)state->request.cmd ));
		request_error(state);
	}
}

/*
 * A list of file descriptors being monitored by select in the main processing
 * loop. fd_event->handler is called whenever the socket is readable/writable.
 */

static struct fd_event *fd_events = NULL;

void add_fd_event(struct fd_event *ev)
{
	struct fd_event *match;

	/* only add unique fd_event structs */

	for (match=fd_events; match; match=match->next ) {
#ifdef DEVELOPER
		SMB_ASSERT( match != ev );
#else
		if ( match == ev )
			return;
#endif
	}

	DLIST_ADD(fd_events, ev);
}

void remove_fd_event(struct fd_event *ev)
{
	DLIST_REMOVE(fd_events, ev);
}

/*
 * Handler for fd_events to complete a read/write request, set up by
 * setup_async_read/setup_async_write.
 */

static void rw_callback(struct fd_event *event, int flags)
{
	size_t todo;
	ssize_t done = 0;

	todo = event->length - event->done;

	if (event->flags & EVENT_FD_WRITE) {
		SMB_ASSERT(flags == EVENT_FD_WRITE);
		done = sys_write(event->fd,
				 &((char *)event->data)[event->done],
				 todo);

		if (done <= 0) {
			event->flags = 0;
			event->finished(event->private_data, False);
			return;
		}
	}

	if (event->flags & EVENT_FD_READ) {
		SMB_ASSERT(flags == EVENT_FD_READ);
		done = sys_read(event->fd, &((char *)event->data)[event->done],
				todo);

		if (done <= 0) {
			event->flags = 0;
			event->finished(event->private_data, False);
			return;
		}
	}

	event->done += done;

	if (event->done == event->length) {
		event->flags = 0;
		event->finished(event->private_data, True);
	}
}

/*
 * Request an async read/write on a fd_event structure. (*finished) is called
 * when the request is completed or an error had occurred.
 */

void setup_async_read(struct fd_event *event, void *data, size_t length,
		      void (*finished)(void *private_data, bool success),
		      void *private_data)
{
	SMB_ASSERT(event->flags == 0);
	event->data = data;
	event->length = length;
	event->done = 0;
	event->handler = rw_callback;
	event->finished = finished;
	event->private_data = private_data;
	event->flags = EVENT_FD_READ;
}

void setup_async_write(struct fd_event *event, void *data, size_t length,
		       void (*finished)(void *private_data, bool success),
		       void *private_data)
{
	SMB_ASSERT(event->flags == 0);
	event->data = data;
	event->length = length;
	event->done = 0;
	event->handler = rw_callback;
	event->finished = finished;
	event->private_data = private_data;
	event->flags = EVENT_FD_WRITE;
}

/*
 * This is the main event loop of winbind requests. It goes through a
 * state-machine of 3 read/write requests, 4 if you have extra data to send.
 *
 * An idle winbind client has a read request of 4 bytes outstanding,
 * finalizing function is request_len_recv, checking the length. request_recv
 * then processes the packet. The processing function then at some point has
 * to call request_finished which schedules sending the response.
 */

static void request_len_recv(void *private_data, bool success);
static void request_recv(void *private_data, bool success);
static void request_main_recv(void *private_data, bool success);
static void request_finished(struct winbindd_cli_state *state);
void request_finished_cont(void *private_data, bool success);
static void response_main_sent(void *private_data, bool success);
static void response_extra_sent(void *private_data, bool success);

static void response_extra_sent(void *private_data, bool success)
{
	struct winbindd_cli_state *state =
		talloc_get_type_abort(private_data, struct winbindd_cli_state);

	if (state->mem_ctx != NULL) {
		talloc_destroy(state->mem_ctx);
		state->mem_ctx = NULL;
	}

	if (!success) {
		state->finished = True;
		return;
	}

	SAFE_FREE(state->response.extra_data.data);

	setup_async_read(&state->fd_event, &state->request, sizeof(uint32),
			 request_len_recv, state);
}

static void response_main_sent(void *private_data, bool success)
{
	struct winbindd_cli_state *state =
		talloc_get_type_abort(private_data, struct winbindd_cli_state);

	if (!success) {
		state->finished = True;
		return;
	}

	if (state->response.length == sizeof(state->response)) {
		if (state->mem_ctx != NULL) {
			talloc_destroy(state->mem_ctx);
			state->mem_ctx = NULL;
		}

		setup_async_read(&state->fd_event, &state->request,
				 sizeof(uint32), request_len_recv, state);
		return;
	}

	setup_async_write(&state->fd_event, state->response.extra_data.data,
			  state->response.length - sizeof(state->response),
			  response_extra_sent, state);
}

static void request_finished(struct winbindd_cli_state *state)
{
	/* Make sure request.extra_data is freed when finish processing a request */
	SAFE_FREE(state->request.extra_data.data);
	setup_async_write(&state->fd_event, &state->response,
			  sizeof(state->response), response_main_sent, state);
}

void request_error(struct winbindd_cli_state *state)
{
	SMB_ASSERT(state->response.result == WINBINDD_PENDING);
	state->response.result = WINBINDD_ERROR;
	request_finished(state);
}

void request_ok(struct winbindd_cli_state *state)
{
	SMB_ASSERT(state->response.result == WINBINDD_PENDING);
	state->response.result = WINBINDD_OK;
	request_finished(state);
}

void request_finished_cont(void *private_data, bool success)
{
	struct winbindd_cli_state *state =
		talloc_get_type_abort(private_data, struct winbindd_cli_state);

	if (success)
		request_ok(state);
	else
		request_error(state);
}

static void request_len_recv(void *private_data, bool success)
{
	struct winbindd_cli_state *state =
		talloc_get_type_abort(private_data, struct winbindd_cli_state);

	if (!success) {
		state->finished = True;
		return;
	}

	if (*(uint32 *)(&state->request) != sizeof(state->request)) {
		DEBUG(0,("request_len_recv: Invalid request size received: %d (expected %u)\n",
			 *(uint32_t *)(&state->request), (uint32_t)sizeof(state->request)));
		state->finished = True;
		return;
	}

	setup_async_read(&state->fd_event, (uint32 *)(&state->request)+1,
			 sizeof(state->request) - sizeof(uint32),
			 request_main_recv, state);
}

static void request_main_recv(void *private_data, bool success)
{
	struct winbindd_cli_state *state =
		talloc_get_type_abort(private_data, struct winbindd_cli_state);

	if (!success) {
		state->finished = True;
		return;
	}

	if (state->request.extra_len == 0) {
		state->request.extra_data.data = NULL;
		request_recv(state, True);
		return;
	}

	if ((!state->privileged) &&
	    (state->request.extra_len > WINBINDD_MAX_EXTRA_DATA)) {
		DEBUG(3, ("Got request with %d bytes extra data on "
			  "unprivileged socket\n", (int)state->request.extra_len));
		state->request.extra_data.data = NULL;
		state->finished = True;
		return;
	}

	state->request.extra_data.data =
		SMB_MALLOC_ARRAY(char, state->request.extra_len + 1);

	if (state->request.extra_data.data == NULL) {
		DEBUG(0, ("malloc failed\n"));
		state->finished = True;
		return;
	}

	/* Ensure null termination */
	state->request.extra_data.data[state->request.extra_len] = '\0';

	setup_async_read(&state->fd_event, state->request.extra_data.data,
			 state->request.extra_len, request_recv, state);
}

static void request_recv(void *private_data, bool success)
{
	struct winbindd_cli_state *state =
		talloc_get_type_abort(private_data, struct winbindd_cli_state);

	if (!success) {
		state->finished = True;
		return;
	}

	process_request(state);
}

/* Process a new connection by adding it to the client connection list */

static void new_connection(int listen_sock, bool privileged)
{
	struct sockaddr_un sunaddr;
	struct winbindd_cli_state *state;
	socklen_t len;
	int sock;
	
	/* Accept connection */
	
	len = sizeof(sunaddr);

	do {
		sock = accept(listen_sock, (struct sockaddr *)&sunaddr, &len);
	} while (sock == -1 && errno == EINTR);

	if (sock == -1)
		return;
	
	DEBUG(6,("accepted socket %d\n", sock));
	
	/* Create new connection structure */
	
	if ((state = TALLOC_ZERO_P(NULL, struct winbindd_cli_state)) == NULL) {
		close(sock);
		return;
	}
	
	state->sock = sock;

	state->last_access = time(NULL);	

	state->privileged = privileged;

	state->fd_event.fd = state->sock;
	state->fd_event.flags = 0;
	add_fd_event(&state->fd_event);

	setup_async_read(&state->fd_event, &state->request, sizeof(uint32),
			 request_len_recv, state);

	/* Add to connection list */
	
	winbindd_add_client(state);
}

/* Remove a client connection from client connection list */

static void remove_client(struct winbindd_cli_state *state)
{
	char c = 0;
	int nwritten;

	/* It's a dead client - hold a funeral */
	
	if (state == NULL) {
		return;
	}

	/* tell client, we are closing ... */
	nwritten = write(state->sock, &c, sizeof(c));
	if (nwritten == -1) {
		DEBUG(2, ("final write to client failed: %s\n",
			  strerror(errno)));
	}

	/* Close socket */
		
	close(state->sock);
		
	/* Free any getent state */
		
	free_getent_state(state->getpwent_state);
	free_getent_state(state->getgrent_state);
		
	/* We may have some extra data that was not freed if the client was
	   killed unexpectedly */

	SAFE_FREE(state->response.extra_data.data);

	if (state->mem_ctx != NULL) {
		talloc_destroy(state->mem_ctx);
		state->mem_ctx = NULL;
	}

	remove_fd_event(&state->fd_event);
		
	/* Remove from list and free */
		
	winbindd_remove_client(state);
	TALLOC_FREE(state);
}

/* Shutdown client connection which has been idle for the longest time */

static bool remove_idle_client(void)
{
	struct winbindd_cli_state *state, *remove_state = NULL;
	time_t last_access = 0;
	int nidle = 0;

	for (state = winbindd_client_list(); state; state = state->next) {
		if (state->response.result != WINBINDD_PENDING &&
		    state->fd_event.flags == EVENT_FD_READ &&
		    !state->getpwent_state && !state->getgrent_state) {
			nidle++;
			if (!last_access || state->last_access < last_access) {
				last_access = state->last_access;
				remove_state = state;
			}
		}
	}

	if (remove_state) {
		DEBUG(5,("Found %d idle client connections, shutting down sock %d, pid %u\n",
			nidle, remove_state->sock, (unsigned int)remove_state->pid));
		remove_client(remove_state);
		return True;
	}

	return False;
}

/* check if HUP has been received and reload files */
void winbind_check_sighup(const char *logfile)
{
	if (do_sighup) {

		DEBUG(3, ("got SIGHUP\n"));

		flush_caches();
		reload_services_file(logfile);

		do_sighup = 0;
	}
}

/* check if TERM has been received */
void winbind_check_sigterm(bool is_parent)
{
	if (do_sigterm)
		terminate(is_parent);
}

/* Process incoming clients on listen_sock.  We use a tricky non-blocking,
   non-forking, non-threaded model which allows us to handle many
   simultaneous connections while remaining impervious to many denial of
   service attacks. */

static void process_loop(void)
{
	struct winbindd_cli_state *state;
	struct fd_event *ev;
	fd_set r_fds, w_fds;
	int maxfd, listen_sock, listen_priv_sock, selret;
	struct timeval timeout, ev_timeout;

	/* Open Sockets here to get stuff going ASAP */
	listen_sock = open_winbindd_socket();
	listen_priv_sock = open_winbindd_priv_socket();

	if (listen_sock == -1 || listen_priv_sock == -1) {
		perror("open_winbind_socket");
		exit(1);
	}

	/* We'll be doing this a lot */

	/* Handle messages */

	message_dispatch(winbind_messaging_context());

	run_events(winbind_event_context(), 0, NULL, NULL);

	/* refresh the trusted domain cache */

	rescan_trusted_domains();

	/* Initialise fd lists for select() */

	maxfd = MAX(listen_sock, listen_priv_sock);

	FD_ZERO(&r_fds);
	FD_ZERO(&w_fds);
	FD_SET(listen_sock, &r_fds);
	FD_SET(listen_priv_sock, &r_fds);

	timeout.tv_sec = WINBINDD_ESTABLISH_LOOP;
	timeout.tv_usec = 0;

	/* Check for any event timeouts. */
	if (get_timed_events_timeout(winbind_event_context(), &ev_timeout)) {
		timeout = timeval_min(&timeout, &ev_timeout);
	}

	/* Set up client readers and writers */

	state = winbindd_client_list();

	while (state) {

		struct winbindd_cli_state *next = state->next;

		/* Dispose of client connection if it is marked as 
		   finished */ 

		if (state->finished)
			remove_client(state);

		state = next;
	}

	for (ev = fd_events; ev; ev = ev->next) {
		if (ev->flags & EVENT_FD_READ) {
			FD_SET(ev->fd, &r_fds);
			maxfd = MAX(ev->fd, maxfd);
		}
		if (ev->flags & EVENT_FD_WRITE) {
			FD_SET(ev->fd, &w_fds);
			maxfd = MAX(ev->fd, maxfd);
		}
	}

	/* Call select */
        
	selret = sys_select(maxfd + 1, &r_fds, &w_fds, NULL, &timeout);

	if (selret == 0) {
		goto no_fds_ready;
	}

	if (selret == -1) {
		if (errno == EINTR) {
			goto no_fds_ready;
		}

		/* Select error, something is badly wrong */

		perror("select");
		exit(1);
	}

	/* selret > 0 */

	ev = fd_events;
	while (ev != NULL) {
		struct fd_event *next = ev->next;
		int flags = 0;
		if (FD_ISSET(ev->fd, &r_fds))
			flags |= EVENT_FD_READ;
		if (FD_ISSET(ev->fd, &w_fds))
			flags |= EVENT_FD_WRITE;
		if (flags)
			ev->handler(ev, flags);
		ev = next;
	}

	if (FD_ISSET(listen_sock, &r_fds)) {
		while (winbindd_num_clients() >
		       WINBINDD_MAX_SIMULTANEOUS_CLIENTS - 1) {
			DEBUG(5,("winbindd: Exceeding %d client "
				 "connections, removing idle "
				 "connection.\n",
				 WINBINDD_MAX_SIMULTANEOUS_CLIENTS));
			if (!remove_idle_client()) {
				DEBUG(0,("winbindd: Exceeding %d "
					 "client connections, no idle "
					 "connection found\n",
					 WINBINDD_MAX_SIMULTANEOUS_CLIENTS));
				break;
			}
		}
		/* new, non-privileged connection */
		new_connection(listen_sock, False);
	}
            
	if (FD_ISSET(listen_priv_sock, &r_fds)) {
		while (winbindd_num_clients() >
		       WINBINDD_MAX_SIMULTANEOUS_CLIENTS - 1) {
			DEBUG(5,("winbindd: Exceeding %d client "
				 "connections, removing idle "
				 "connection.\n",
				 WINBINDD_MAX_SIMULTANEOUS_CLIENTS));
			if (!remove_idle_client()) {
				DEBUG(0,("winbindd: Exceeding %d "
					 "client connections, no idle "
					 "connection found\n",
					 WINBINDD_MAX_SIMULTANEOUS_CLIENTS));
				break;
			}
		}
		/* new, privileged connection */
		new_connection(listen_priv_sock, True);
	}

 no_fds_ready:

#if 0
	winbindd_check_cache_size(time(NULL));
#endif

	/* Check signal handling things */

	winbind_check_sigterm(true);
	winbind_check_sighup(NULL);

	if (do_sigusr2) {
		print_winbindd_status();
		do_sigusr2 = 0;
	}

	if (do_sigchld) {
		pid_t pid;

		do_sigchld = 0;

		while ((pid = sys_waitpid(-1, NULL, WNOHANG)) > 0) {
			winbind_child_died(pid);
		}
	}
}

/* Main function */

int main(int argc, char **argv, char **envp)
{
	static bool is_daemon = False;
	static bool Fork = True;
	static bool log_stdout = False;
	static bool no_process_group = False;
	enum {
		OPT_DAEMON = 1000,
		OPT_FORK,
		OPT_NO_PROCESS_GROUP,
		OPT_LOG_STDOUT
	};
	struct poptOption long_options[] = {
		POPT_AUTOHELP
		{ "stdout", 'S', POPT_ARG_NONE, NULL, OPT_LOG_STDOUT, "Log to stdout" },
		{ "foreground", 'F', POPT_ARG_NONE, NULL, OPT_FORK, "Daemon in foreground mode" },
		{ "no-process-group", 0, POPT_ARG_NONE, NULL, OPT_NO_PROCESS_GROUP, "Don't create a new process group" },
		{ "daemon", 'D', POPT_ARG_NONE, NULL, OPT_DAEMON, "Become a daemon (default)" },
		{ "interactive", 'i', POPT_ARG_NONE, NULL, 'i', "Interactive mode" },
		{ "no-caching", 'n', POPT_ARG_NONE, NULL, 'n', "Disable caching" },
		POPT_COMMON_SAMBA
		POPT_TABLEEND
	};
	poptContext pc;
	int opt;
	TALLOC_CTX *frame = talloc_stackframe();

	/* glibc (?) likes to print "User defined signal 1" and exit if a
	   SIGUSR[12] is received before a handler is installed */

 	CatchSignal(SIGUSR1, SIG_IGN);
 	CatchSignal(SIGUSR2, SIG_IGN);

	fault_setup((void (*)(void *))fault_quit );
	dump_core_setup("winbindd");

	load_case_tables();

	db_tdb2_setup_messaging(NULL, false);

	/* Initialise for running in non-root mode */

	sec_init();

	set_remote_machine_name("winbindd", False);

	/* Set environment variable so we don't recursively call ourselves.
	   This may also be useful interactively. */

	if ( !winbind_off() ) {
		DEBUG(0,("Failed to disable recusive winbindd calls.  Exiting.\n"));
		exit(1);
	}

	/* Initialise samba/rpc client stuff */

	pc = poptGetContext("winbindd", argc, (const char **)argv, long_options, 0);

	while ((opt = poptGetNextOpt(pc)) != -1) {
		switch (opt) {
			/* Don't become a daemon */
		case OPT_DAEMON:
			is_daemon = True;
			break;
		case 'i':
			interactive = True;
			log_stdout = True;
			Fork = False;
			break;
                case OPT_FORK:
			Fork = false;
			break;
		case OPT_NO_PROCESS_GROUP:
			no_process_group = true;
			break;
		case OPT_LOG_STDOUT:
			log_stdout = true;
			break;
		case 'n':
			opt_nocache = true;
			break;
		default:
			d_fprintf(stderr, "\nInvalid option %s: %s\n\n",
				  poptBadOption(pc, 0), poptStrerror(opt));
			poptPrintUsage(pc, stderr, 0);
			exit(1);
		}
	}

	if (is_daemon && interactive) {
		d_fprintf(stderr,"\nERROR: "
			  "Option -i|--interactive is not allowed together with -D|--daemon\n\n");
		poptPrintUsage(pc, stderr, 0);
		exit(1);
	}

	if (log_stdout && Fork) {
		d_fprintf(stderr, "\nERROR: "
			  "Can't log to stdout (-S) unless daemon is in foreground +(-F) or interactive (-i)\n\n");
		poptPrintUsage(pc, stderr, 0);
		exit(1);
	}

	poptFreeContext(pc);

	if (!override_logfile) {
		char *logfile = NULL;
		if (asprintf(&logfile,"%s/log.winbindd",
				get_dyn_LOGFILEBASE()) > 0) {
			lp_set_logfile(logfile);
			SAFE_FREE(logfile);
		}
	}
	setup_logging("winbindd", log_stdout);
	reopen_logs();

	DEBUG(0,("winbindd version %s started.\n", SAMBA_VERSION_STRING));
	DEBUGADD(0,("%s\n", COPYRIGHT_STARTUP_MESSAGE));

	if (!lp_load_initial_only(get_dyn_CONFIGFILE())) {
		DEBUG(0, ("error opening config file\n"));
		exit(1);
	}

	/* Initialise messaging system */

	if (winbind_messaging_context() == NULL) {
		DEBUG(0, ("unable to initialize messaging system\n"));
		exit(1);
	}

	db_tdb2_setup_messaging(winbind_messaging_context(), true);

	if (!reload_services_file(NULL)) {
		DEBUG(0, ("error opening config file\n"));
		exit(1);
	}

	if (!directory_exist(lp_lockdir(), NULL)) {
		mkdir(lp_lockdir(), 0755);
	}

	/* Setup names. */

	if (!init_names())
		exit(1);

  	load_interfaces();

	if (!secrets_init()) {

		DEBUG(0,("Could not initialize domain trust account secrets. Giving up\n"));
		return False;
	}

	/* Enable netbios namecache */

	namecache_enable();

	/* Winbind daemon initialisation */

	if ( ! NT_STATUS_IS_OK(idmap_init_cache()) ) {
		DEBUG(1, ("Could not init idmap cache!\n"));		
	}

	/* Unblock all signals we are interested in as they may have been
	   blocked by the parent process. */

	BlockSignals(False, SIGINT);
	BlockSignals(False, SIGQUIT);
	BlockSignals(False, SIGTERM);
	BlockSignals(False, SIGUSR1);
	BlockSignals(False, SIGUSR2);
	BlockSignals(False, SIGHUP);
	BlockSignals(False, SIGCHLD);

	/* Setup signal handlers */
	
	CatchSignal(SIGINT, termination_handler);      /* Exit on these sigs */
	CatchSignal(SIGQUIT, termination_handler);
	CatchSignal(SIGTERM, termination_handler);
	CatchSignal(SIGCHLD, sigchld_handler);

	CatchSignal(SIGPIPE, SIG_IGN);                 /* Ignore sigpipe */

	CatchSignal(SIGUSR2, sigusr2_handler);         /* Debugging sigs */
	CatchSignal(SIGHUP, sighup_handler);

	if (!interactive)
		become_daemon(Fork, no_process_group);

	pidfile_create("winbindd");

#if HAVE_SETPGID
	/*
	 * If we're interactive we want to set our own process group for
	 * signal management.
	 */
	if (interactive && !no_process_group)
		setpgid( (pid_t)0, (pid_t)0);
#endif

	TimeInit();

	/* Don't use winbindd_reinit_after_fork here as
	 * we're just starting up and haven't created any
	 * winbindd-specific resources we must free yet. JRA.
	 */

	if (!reinit_after_fork(winbind_messaging_context(),
			       winbind_event_context(), false)) {
		DEBUG(0,("reinit_after_fork() failed\n"));
		exit(1);
	}

	/*
	 * Ensure all cache and idmap caches are consistent
	 * and initialized before we startup.
	 */
	if (!winbindd_cache_validate_and_initialize()) {
		exit(1);
	}

	/* get broadcast messages */
	claim_connection(NULL,"",FLAG_MSG_GENERAL|FLAG_MSG_DBWRAP);

	/* React on 'smbcontrol winbindd reload-config' in the same way
	   as to SIGHUP signal */
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_SMB_CONF_UPDATED, msg_reload_services);
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_SHUTDOWN, msg_shutdown);

	/* Handle online/offline messages. */
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_OFFLINE, winbind_msg_offline);
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_ONLINE, winbind_msg_online);
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_ONLINESTATUS, winbind_msg_onlinestatus);

	messaging_register(winbind_messaging_context(), NULL,
			   MSG_DUMP_EVENT_LIST, winbind_msg_dump_event_list);

	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_VALIDATE_CACHE,
			   winbind_msg_validate_cache);

	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_DUMP_DOMAIN_LIST,
			   winbind_msg_dump_domain_list);

	/* Register handler for MSG_DEBUG. */
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_DEBUG,
			   winbind_msg_debug);
	
	netsamlogon_cache_init(); /* Non-critical */
	
	/* clear the cached list of trusted domains */

	wcache_tdc_clear();	
	
	if (!init_domain_list()) {
		DEBUG(0,("unable to initialize domain list\n"));
		exit(1);
	}

	init_idmap_child();
	init_locator_child();

	smb_nscd_flush_user_cache();
	smb_nscd_flush_group_cache();

	/* Loop waiting for requests */

	TALLOC_FREE(frame);
	while (1) {
		frame = talloc_stackframe();
		process_loop();
		TALLOC_FREE(frame);
	}

	return 0;
}
