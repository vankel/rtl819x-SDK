/*
 * Unix SMB/CIFS implementation. 
 * SMB parameters and setup
 * Copyright (C) Andrew Tridgell   1992-1998
 * Copyright (C) Simo Sorce        2000-2003
 * Copyright (C) Gerald Carter     2000-2006
 * Copyright (C) Jeremy Allison    2001-2009
 * Copyright (C) Andrew Bartlett   2002
 * Copyright (C) Jim McDonough <jmcd@us.ibm.com> 2005
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "includes.h"

#if 0 /* when made a module use this */

static int tdbsam_debug_level = DBGC_ALL;
#undef DBGC_CLASS
#define DBGC_CLASS tdbsam_debug_level

#else

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_PASSDB

#endif

#define TDBSAM_VERSION	3	/* Most recent TDBSAM version */
#define TDBSAM_MINOR_VERSION	1	/* Most recent TDBSAM minor version */
#define TDBSAM_VERSION_STRING	"INFO/version"
#define TDBSAM_MINOR_VERSION_STRING	"INFO/minor_version"
#define PASSDB_FILE_NAME	"passdb.tdb"
#define USERPREFIX		"USER_"
#define USERPREFIX_LEN		5
#define RIDPREFIX		"RID_"
#define PRIVPREFIX		"PRIV_"

/* GLOBAL TDB SAM CONTEXT */

static struct db_context *db_sam;
static char *tdbsam_filename;

/**********************************************************************
 Marshall/unmarshall struct samu structs.
 *********************************************************************/

#define TDB_FORMAT_STRING_V0       "ddddddBBBBBBBBBBBBddBBwdwdBwwd"
#define TDB_FORMAT_STRING_V1       "dddddddBBBBBBBBBBBBddBBwdwdBwwd"
#define TDB_FORMAT_STRING_V2       "dddddddBBBBBBBBBBBBddBBBwwdBwwd"

/*********************************************************************
*********************************************************************/

static bool init_sam_from_buffer_v0(struct samu *sampass, uint8 *buf, uint32 buflen)
{

	/* times are stored as 32bit integer
	   take care on system with 64bit wide time_t
	   --SSS */
	uint32	logon_time,
		logoff_time,
		kickoff_time,
		pass_last_set_time,
		pass_can_change_time,
		pass_must_change_time;
	char *username = NULL;
	char *domain = NULL;
	char *nt_username = NULL;
	char *dir_drive = NULL;
	char *unknown_str = NULL;
	char *munged_dial = NULL;
	char *fullname = NULL;
	char *homedir = NULL;
	char *logon_script = NULL;
	char *profile_path = NULL;
	char *acct_desc = NULL;
	char *workstations = NULL;
	uint32	username_len, domain_len, nt_username_len,
		dir_drive_len, unknown_str_len, munged_dial_len,
		fullname_len, homedir_len, logon_script_len,
		profile_path_len, acct_desc_len, workstations_len;
		
	uint32	user_rid, group_rid, remove_me, hours_len, unknown_6;
	uint16	acct_ctrl, logon_divs;
	uint16	bad_password_count, logon_count;
	uint8	*hours = NULL;
	uint8	*lm_pw_ptr = NULL, *nt_pw_ptr = NULL;
	uint32		len = 0;
	uint32		lm_pw_len, nt_pw_len, hourslen;
	bool ret = True;
	
	if(sampass == NULL || buf == NULL) {
		DEBUG(0, ("init_sam_from_buffer_v0: NULL parameters found!\n"));
		return False;
	}

/* TDB_FORMAT_STRING_V0       "ddddddBBBBBBBBBBBBddBBwdwdBwwd" */

	/* unpack the buffer into variables */
	len = tdb_unpack (buf, buflen, TDB_FORMAT_STRING_V0,
		&logon_time,						/* d */
		&logoff_time,						/* d */
		&kickoff_time,						/* d */
		&pass_last_set_time,					/* d */
		&pass_can_change_time,					/* d */
		&pass_must_change_time,					/* d */
		&username_len, &username,				/* B */
		&domain_len, &domain,					/* B */
		&nt_username_len, &nt_username,				/* B */
		&fullname_len, &fullname,				/* B */
		&homedir_len, &homedir,					/* B */
		&dir_drive_len, &dir_drive,				/* B */
		&logon_script_len, &logon_script,			/* B */
		&profile_path_len, &profile_path,			/* B */
		&acct_desc_len, &acct_desc,				/* B */
		&workstations_len, &workstations,			/* B */
		&unknown_str_len, &unknown_str,				/* B */
		&munged_dial_len, &munged_dial,				/* B */
		&user_rid,						/* d */
		&group_rid,						/* d */
		&lm_pw_len, &lm_pw_ptr,					/* B */
		&nt_pw_len, &nt_pw_ptr,					/* B */
		&acct_ctrl,						/* w */
		&remove_me, /* remove on the next TDB_FORMAT upgarde */	/* d */
		&logon_divs,						/* w */
		&hours_len,						/* d */
		&hourslen, &hours,					/* B */
		&bad_password_count,					/* w */
		&logon_count,						/* w */
		&unknown_6);						/* d */
		
	if (len == (uint32) -1)  {
		ret = False;
		goto done;
	}

	pdb_set_logon_time(sampass, logon_time, PDB_SET);
	pdb_set_logoff_time(sampass, logoff_time, PDB_SET);
	pdb_set_kickoff_time(sampass, kickoff_time, PDB_SET);
	pdb_set_pass_can_change_time(sampass, pass_can_change_time, PDB_SET);
	pdb_set_pass_must_change_time(sampass, pass_must_change_time, PDB_SET);
	pdb_set_pass_last_set_time(sampass, pass_last_set_time, PDB_SET);

	pdb_set_username(sampass, username, PDB_SET); 
	pdb_set_domain(sampass, domain, PDB_SET);
	pdb_set_nt_username(sampass, nt_username, PDB_SET);
	pdb_set_fullname(sampass, fullname, PDB_SET);

	if (homedir) {
		pdb_set_homedir(sampass, homedir, PDB_SET);
	}
	else {
		pdb_set_homedir(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_home()),
			PDB_DEFAULT);
	}

	if (dir_drive) 	
		pdb_set_dir_drive(sampass, dir_drive, PDB_SET);
	else {
		pdb_set_dir_drive(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_drive()),
			PDB_DEFAULT);
	}

	if (logon_script) 
		pdb_set_logon_script(sampass, logon_script, PDB_SET);
	else {
		pdb_set_logon_script(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_script()),
			PDB_DEFAULT);
	}
	
	if (profile_path) {	
		pdb_set_profile_path(sampass, profile_path, PDB_SET);
	} else {
		pdb_set_profile_path(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_path()),
			PDB_DEFAULT);
	}

	pdb_set_acct_desc(sampass, acct_desc, PDB_SET);
	pdb_set_workstations(sampass, workstations, PDB_SET);
	pdb_set_munged_dial(sampass, munged_dial, PDB_SET);

	if (lm_pw_ptr && lm_pw_len == LM_HASH_LEN) {
		if (!pdb_set_lanman_passwd(sampass, lm_pw_ptr, PDB_SET)) {
			ret = False;
			goto done;
		}
	}

	if (nt_pw_ptr && nt_pw_len == NT_HASH_LEN) {
		if (!pdb_set_nt_passwd(sampass, nt_pw_ptr, PDB_SET)) {
			ret = False;
			goto done;
		}
	}

	pdb_set_pw_history(sampass, NULL, 0, PDB_SET);
	pdb_set_user_sid_from_rid(sampass, user_rid, PDB_SET);
	pdb_set_group_sid_from_rid(sampass, group_rid, PDB_SET);
	pdb_set_hours_len(sampass, hours_len, PDB_SET);
	pdb_set_bad_password_count(sampass, bad_password_count, PDB_SET);
	pdb_set_logon_count(sampass, logon_count, PDB_SET);
	pdb_set_unknown_6(sampass, unknown_6, PDB_SET);
	pdb_set_acct_ctrl(sampass, acct_ctrl, PDB_SET);
	pdb_set_logon_divs(sampass, logon_divs, PDB_SET);
	pdb_set_hours(sampass, hours, PDB_SET);

done:

	SAFE_FREE(username);
	SAFE_FREE(domain);
	SAFE_FREE(nt_username);
	SAFE_FREE(fullname);
	SAFE_FREE(homedir);
	SAFE_FREE(dir_drive);
	SAFE_FREE(logon_script);
	SAFE_FREE(profile_path);
	SAFE_FREE(acct_desc);
	SAFE_FREE(workstations);
	SAFE_FREE(munged_dial);
	SAFE_FREE(unknown_str);
	SAFE_FREE(lm_pw_ptr);
	SAFE_FREE(nt_pw_ptr);
	SAFE_FREE(hours);

	return ret;
}

/*********************************************************************
*********************************************************************/

static bool init_sam_from_buffer_v1(struct samu *sampass, uint8 *buf, uint32 buflen)
{

	/* times are stored as 32bit integer
	   take care on system with 64bit wide time_t
	   --SSS */
	uint32	logon_time,
		logoff_time,
		kickoff_time,
		bad_password_time,
		pass_last_set_time,
		pass_can_change_time,
		pass_must_change_time;
	char *username = NULL;
	char *domain = NULL;
	char *nt_username = NULL;
	char *dir_drive = NULL;
	char *unknown_str = NULL;
	char *munged_dial = NULL;
	char *fullname = NULL;
	char *homedir = NULL;
	char *logon_script = NULL;
	char *profile_path = NULL;
	char *acct_desc = NULL;
	char *workstations = NULL;
	uint32	username_len, domain_len, nt_username_len,
		dir_drive_len, unknown_str_len, munged_dial_len,
		fullname_len, homedir_len, logon_script_len,
		profile_path_len, acct_desc_len, workstations_len;
		
	uint32	user_rid, group_rid, remove_me, hours_len, unknown_6;
	uint16	acct_ctrl, logon_divs;
	uint16	bad_password_count, logon_count;
	uint8	*hours = NULL;
	uint8	*lm_pw_ptr = NULL, *nt_pw_ptr = NULL;
	uint32		len = 0;
	uint32		lm_pw_len, nt_pw_len, hourslen;
	bool ret = True;
	
	if(sampass == NULL || buf == NULL) {
		DEBUG(0, ("init_sam_from_buffer_v1: NULL parameters found!\n"));
		return False;
	}

/* TDB_FORMAT_STRING_V1       "dddddddBBBBBBBBBBBBddBBwdwdBwwd" */

	/* unpack the buffer into variables */
	len = tdb_unpack (buf, buflen, TDB_FORMAT_STRING_V1,
		&logon_time,						/* d */
		&logoff_time,						/* d */
		&kickoff_time,						/* d */
		/* Change from V0 is addition of bad_password_time field. */
		&bad_password_time,					/* d */
		&pass_last_set_time,					/* d */
		&pass_can_change_time,					/* d */
		&pass_must_change_time,					/* d */
		&username_len, &username,				/* B */
		&domain_len, &domain,					/* B */
		&nt_username_len, &nt_username,				/* B */
		&fullname_len, &fullname,				/* B */
		&homedir_len, &homedir,					/* B */
		&dir_drive_len, &dir_drive,				/* B */
		&logon_script_len, &logon_script,			/* B */
		&profile_path_len, &profile_path,			/* B */
		&acct_desc_len, &acct_desc,				/* B */
		&workstations_len, &workstations,			/* B */
		&unknown_str_len, &unknown_str,				/* B */
		&munged_dial_len, &munged_dial,				/* B */
		&user_rid,						/* d */
		&group_rid,						/* d */
		&lm_pw_len, &lm_pw_ptr,					/* B */
		&nt_pw_len, &nt_pw_ptr,					/* B */
		&acct_ctrl,						/* w */
		&remove_me,						/* d */
		&logon_divs,						/* w */
		&hours_len,						/* d */
		&hourslen, &hours,					/* B */
		&bad_password_count,					/* w */
		&logon_count,						/* w */
		&unknown_6);						/* d */
		
	if (len == (uint32) -1)  {
		ret = False;
		goto done;
	}

	pdb_set_logon_time(sampass, logon_time, PDB_SET);
	pdb_set_logoff_time(sampass, logoff_time, PDB_SET);
	pdb_set_kickoff_time(sampass, kickoff_time, PDB_SET);

	/* Change from V0 is addition of bad_password_time field. */
	pdb_set_bad_password_time(sampass, bad_password_time, PDB_SET);
	pdb_set_pass_can_change_time(sampass, pass_can_change_time, PDB_SET);
	pdb_set_pass_must_change_time(sampass, pass_must_change_time, PDB_SET);
	pdb_set_pass_last_set_time(sampass, pass_last_set_time, PDB_SET);

	pdb_set_username(sampass, username, PDB_SET); 
	pdb_set_domain(sampass, domain, PDB_SET);
	pdb_set_nt_username(sampass, nt_username, PDB_SET);
	pdb_set_fullname(sampass, fullname, PDB_SET);

	if (homedir) {
		pdb_set_homedir(sampass, homedir, PDB_SET);
	}
	else {
		pdb_set_homedir(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_home()),
			PDB_DEFAULT);
	}

	if (dir_drive) 	
		pdb_set_dir_drive(sampass, dir_drive, PDB_SET);
	else {
		pdb_set_dir_drive(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_drive()),
			PDB_DEFAULT);
	}

	if (logon_script) 
		pdb_set_logon_script(sampass, logon_script, PDB_SET);
	else {
		pdb_set_logon_script(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_script()),
			PDB_DEFAULT);
	}
	
	if (profile_path) {	
		pdb_set_profile_path(sampass, profile_path, PDB_SET);
	} else {
		pdb_set_profile_path(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_path()),
			PDB_DEFAULT);
	}

	pdb_set_acct_desc(sampass, acct_desc, PDB_SET);
	pdb_set_workstations(sampass, workstations, PDB_SET);
	pdb_set_munged_dial(sampass, munged_dial, PDB_SET);

	if (lm_pw_ptr && lm_pw_len == LM_HASH_LEN) {
		if (!pdb_set_lanman_passwd(sampass, lm_pw_ptr, PDB_SET)) {
			ret = False;
			goto done;
		}
	}

	if (nt_pw_ptr && nt_pw_len == NT_HASH_LEN) {
		if (!pdb_set_nt_passwd(sampass, nt_pw_ptr, PDB_SET)) {
			ret = False;
			goto done;
		}
	}

	pdb_set_pw_history(sampass, NULL, 0, PDB_SET);

	pdb_set_user_sid_from_rid(sampass, user_rid, PDB_SET);
	pdb_set_group_sid_from_rid(sampass, group_rid, PDB_SET);
	pdb_set_hours_len(sampass, hours_len, PDB_SET);
	pdb_set_bad_password_count(sampass, bad_password_count, PDB_SET);
	pdb_set_logon_count(sampass, logon_count, PDB_SET);
	pdb_set_unknown_6(sampass, unknown_6, PDB_SET);
	pdb_set_acct_ctrl(sampass, acct_ctrl, PDB_SET);
	pdb_set_logon_divs(sampass, logon_divs, PDB_SET);
	pdb_set_hours(sampass, hours, PDB_SET);

done:

	SAFE_FREE(username);
	SAFE_FREE(domain);
	SAFE_FREE(nt_username);
	SAFE_FREE(fullname);
	SAFE_FREE(homedir);
	SAFE_FREE(dir_drive);
	SAFE_FREE(logon_script);
	SAFE_FREE(profile_path);
	SAFE_FREE(acct_desc);
	SAFE_FREE(workstations);
	SAFE_FREE(munged_dial);
	SAFE_FREE(unknown_str);
	SAFE_FREE(lm_pw_ptr);
	SAFE_FREE(nt_pw_ptr);
	SAFE_FREE(hours);

	return ret;
}

bool init_sam_from_buffer_v2(struct samu *sampass, uint8 *buf, uint32 buflen)
{

	/* times are stored as 32bit integer
	   take care on system with 64bit wide time_t
	   --SSS */
	uint32	logon_time,
		logoff_time,
		kickoff_time,
		bad_password_time,
		pass_last_set_time,
		pass_can_change_time,
		pass_must_change_time;
	char *username = NULL;
	char *domain = NULL;
	char *nt_username = NULL;
	char *dir_drive = NULL;
	char *unknown_str = NULL;
	char *munged_dial = NULL;
	char *fullname = NULL;
	char *homedir = NULL;
	char *logon_script = NULL;
	char *profile_path = NULL;
	char *acct_desc = NULL;
	char *workstations = NULL;
	uint32	username_len, domain_len, nt_username_len,
		dir_drive_len, unknown_str_len, munged_dial_len,
		fullname_len, homedir_len, logon_script_len,
		profile_path_len, acct_desc_len, workstations_len;
		
	uint32	user_rid, group_rid, hours_len, unknown_6;
	uint16	acct_ctrl, logon_divs;
	uint16	bad_password_count, logon_count;
	uint8	*hours = NULL;
	uint8	*lm_pw_ptr = NULL, *nt_pw_ptr = NULL, *nt_pw_hist_ptr = NULL;
	uint32		len = 0;
	uint32		lm_pw_len, nt_pw_len, nt_pw_hist_len, hourslen;
	uint32 pwHistLen = 0;
	bool ret = True;
	fstring tmp_string;
	bool expand_explicit = lp_passdb_expand_explicit();
	
	if(sampass == NULL || buf == NULL) {
		DEBUG(0, ("init_sam_from_buffer_v2: NULL parameters found!\n"));
		return False;
	}
									
/* TDB_FORMAT_STRING_V2       "dddddddBBBBBBBBBBBBddBBBwwdBwwd" */

	/* unpack the buffer into variables */
	len = tdb_unpack (buf, buflen, TDB_FORMAT_STRING_V2,
		&logon_time,						/* d */
		&logoff_time,						/* d */
		&kickoff_time,						/* d */
		&bad_password_time,					/* d */
		&pass_last_set_time,					/* d */
		&pass_can_change_time,					/* d */
		&pass_must_change_time,					/* d */
		&username_len, &username,				/* B */
		&domain_len, &domain,					/* B */
		&nt_username_len, &nt_username,				/* B */
		&fullname_len, &fullname,				/* B */
		&homedir_len, &homedir,					/* B */
		&dir_drive_len, &dir_drive,				/* B */
		&logon_script_len, &logon_script,			/* B */
		&profile_path_len, &profile_path,			/* B */
		&acct_desc_len, &acct_desc,				/* B */
		&workstations_len, &workstations,			/* B */
		&unknown_str_len, &unknown_str,				/* B */
		&munged_dial_len, &munged_dial,				/* B */
		&user_rid,						/* d */
		&group_rid,						/* d */
		&lm_pw_len, &lm_pw_ptr,					/* B */
		&nt_pw_len, &nt_pw_ptr,					/* B */
		/* Change from V1 is addition of password history field. */
		&nt_pw_hist_len, &nt_pw_hist_ptr,			/* B */
		&acct_ctrl,						/* w */
		/* Also "remove_me" field was removed. */
		&logon_divs,						/* w */
		&hours_len,						/* d */
		&hourslen, &hours,					/* B */
		&bad_password_count,					/* w */
		&logon_count,						/* w */
		&unknown_6);						/* d */
		
	if (len == (uint32) -1)  {
		ret = False;
		goto done;
	}

	pdb_set_logon_time(sampass, logon_time, PDB_SET);
	pdb_set_logoff_time(sampass, logoff_time, PDB_SET);
	pdb_set_kickoff_time(sampass, kickoff_time, PDB_SET);
	pdb_set_bad_password_time(sampass, bad_password_time, PDB_SET);
	pdb_set_pass_can_change_time(sampass, pass_can_change_time, PDB_SET);
	pdb_set_pass_must_change_time(sampass, pass_must_change_time, PDB_SET);
	pdb_set_pass_last_set_time(sampass, pass_last_set_time, PDB_SET);

	pdb_set_username(sampass, username, PDB_SET); 
	pdb_set_domain(sampass, domain, PDB_SET);
	pdb_set_nt_username(sampass, nt_username, PDB_SET);
	pdb_set_fullname(sampass, fullname, PDB_SET);

	if (homedir) {
		fstrcpy( tmp_string, homedir );
		if (expand_explicit) {
			standard_sub_basic( username, domain, tmp_string,
					    sizeof(tmp_string) );
		}
		pdb_set_homedir(sampass, tmp_string, PDB_SET);
	}
	else {
		pdb_set_homedir(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_home()),
			PDB_DEFAULT);
	}

	if (dir_drive) 	
		pdb_set_dir_drive(sampass, dir_drive, PDB_SET);
	else
		pdb_set_dir_drive(sampass, lp_logon_drive(), PDB_DEFAULT );

	if (logon_script) {
		fstrcpy( tmp_string, logon_script );
		if (expand_explicit) {
			standard_sub_basic( username, domain, tmp_string,
					    sizeof(tmp_string) );
		}
		pdb_set_logon_script(sampass, tmp_string, PDB_SET);
	}
	else {
		pdb_set_logon_script(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_script()),
			PDB_DEFAULT);
	}
	
	if (profile_path) {	
		fstrcpy( tmp_string, profile_path );
		if (expand_explicit) {
			standard_sub_basic( username, domain, tmp_string,
					    sizeof(tmp_string) );
		}
		pdb_set_profile_path(sampass, tmp_string, PDB_SET);
	} 
	else {
		pdb_set_profile_path(sampass, 
			talloc_sub_basic(sampass, username, domain,
					 lp_logon_path()),
			PDB_DEFAULT);
	}

	pdb_set_acct_desc(sampass, acct_desc, PDB_SET);
	pdb_set_workstations(sampass, workstations, PDB_SET);
	pdb_set_munged_dial(sampass, munged_dial, PDB_SET);

	if (lm_pw_ptr && lm_pw_len == LM_HASH_LEN) {
		if (!pdb_set_lanman_passwd(sampass, lm_pw_ptr, PDB_SET)) {
			ret = False;
			goto done;
		}
	}

	if (nt_pw_ptr && nt_pw_len == NT_HASH_LEN) {
		if (!pdb_set_nt_passwd(sampass, nt_pw_ptr, PDB_SET)) {
			ret = False;
			goto done;
		}
	}

	/* Change from V1 is addition of password history field. */
	pdb_get_account_policy(AP_PASSWORD_HISTORY, &pwHistLen);
	if (pwHistLen) {
		uint8 *pw_hist = SMB_MALLOC_ARRAY(uint8, pwHistLen * PW_HISTORY_ENTRY_LEN);
		if (!pw_hist) {
			ret = False;
			goto done;
		}
		memset(pw_hist, '\0', pwHistLen * PW_HISTORY_ENTRY_LEN);
		if (nt_pw_hist_ptr && nt_pw_hist_len) {
			int i;
			SMB_ASSERT((nt_pw_hist_len % PW_HISTORY_ENTRY_LEN) == 0);
			nt_pw_hist_len /= PW_HISTORY_ENTRY_LEN;
			for (i = 0; (i < pwHistLen) && (i < nt_pw_hist_len); i++) {
				memcpy(&pw_hist[i*PW_HISTORY_ENTRY_LEN],
					&nt_pw_hist_ptr[i*PW_HISTORY_ENTRY_LEN],
					PW_HISTORY_ENTRY_LEN);
			}
		}
		if (!pdb_set_pw_history(sampass, pw_hist, pwHistLen, PDB_SET)) {
			SAFE_FREE(pw_hist);
			ret = False;
			goto done;
		}
		SAFE_FREE(pw_hist);
	} else {
		pdb_set_pw_history(sampass, NULL, 0, PDB_SET);
	}

	pdb_set_user_sid_from_rid(sampass, user_rid, PDB_SET);
	pdb_set_group_sid_from_rid(sampass, group_rid, PDB_SET);
	pdb_set_hours_len(sampass, hours_len, PDB_SET);
	pdb_set_bad_password_count(sampass, bad_password_count, PDB_SET);
	pdb_set_logon_count(sampass, logon_count, PDB_SET);
	pdb_set_unknown_6(sampass, unknown_6, PDB_SET);
	pdb_set_acct_ctrl(sampass, acct_ctrl, PDB_SET);
	pdb_set_logon_divs(sampass, logon_divs, PDB_SET);
	pdb_set_hours(sampass, hours, PDB_SET);

done:

	SAFE_FREE(username);
	SAFE_FREE(domain);
	SAFE_FREE(nt_username);
	SAFE_FREE(fullname);
	SAFE_FREE(homedir);
	SAFE_FREE(dir_drive);
	SAFE_FREE(logon_script);
	SAFE_FREE(profile_path);
	SAFE_FREE(acct_desc);
	SAFE_FREE(workstations);
	SAFE_FREE(munged_dial);
	SAFE_FREE(unknown_str);
	SAFE_FREE(lm_pw_ptr);
	SAFE_FREE(nt_pw_ptr);
	SAFE_FREE(nt_pw_hist_ptr);
	SAFE_FREE(hours);

	return ret;
}


/**********************************************************************
 Intialize a struct samu struct from a BYTE buffer of size len
 *********************************************************************/

static bool init_sam_from_buffer(struct samu *sampass, uint8 *buf, uint32 buflen)
{
	return init_sam_from_buffer_v3(sampass, buf, buflen);
}

/**********************************************************************
 Intialize a BYTE buffer from a struct samu struct
 *********************************************************************/

static uint32 init_buffer_from_sam (uint8 **buf, struct samu *sampass, bool size_only)
{
	return init_buffer_from_sam_v3(buf, sampass, size_only);
}

/**********************************************************************
 Struct and function to update an old record.
 *********************************************************************/

struct tdbsam_convert_state {
	int32_t from;
	bool success;
};

static int tdbsam_convert_one(struct db_record *rec, void *priv)
{
	struct tdbsam_convert_state *state =
		(struct tdbsam_convert_state *)priv;
	struct samu *user;
	TDB_DATA data;
	NTSTATUS status;
	bool ret;

	if (rec->key.dsize < USERPREFIX_LEN) {
		return 0;
	}
	if (strncmp((char *)rec->key.dptr, USERPREFIX, USERPREFIX_LEN) != 0) {
		return 0;
	}

	user = samu_new(talloc_tos());
	if (user == NULL) {
		DEBUG(0,("tdbsam_convert: samu_new() failed!\n"));
		state->success = false;
		return -1;
	}

	DEBUG(10,("tdbsam_convert: Try unpacking a record with (key:%s) "
		  "(version:%d)\n", rec->key.dptr, state->from));

	switch (state->from) {
	case 0:
		ret = init_sam_from_buffer_v0(user, (uint8 *)rec->value.dptr,
					      rec->value.dsize);
		break;
	case 1:
		ret = init_sam_from_buffer_v1(user, (uint8 *)rec->value.dptr,
					      rec->value.dsize);
		break;
	case 2:
		ret = init_sam_from_buffer_v2(user, (uint8 *)rec->value.dptr,
					      rec->value.dsize);
		break;
	case 3:
		ret = init_sam_from_buffer_v3(user, (uint8 *)rec->value.dptr,
					      rec->value.dsize);
		break;
	default:
		/* unknown tdbsam version */
		ret = False;
	}
	if (!ret) {
		DEBUG(0,("tdbsam_convert: Bad struct samu entry returned "
			 "from TDB (key:%s) (version:%d)\n", rec->key.dptr,
			 state->from));
		TALLOC_FREE(user);
		state->success = false;
		return -1;
	}

	data.dsize = init_buffer_from_sam(&data.dptr, user, false);
	TALLOC_FREE(user);

	if (data.dsize == -1) {
		DEBUG(0,("tdbsam_convert: cannot pack the struct samu into "
			 "the new format\n"));
		state->success = false;
		return -1;
	}

	status = rec->store(rec, data, TDB_MODIFY);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Could not store the new record: %s\n",
			  nt_errstr(status)));
		state->success = false;
		return -1;
	}

	return 0;
}

/**********************************************************************
 Struct and function to backup an old record.
 *********************************************************************/

struct tdbsam_backup_state {
	struct db_context *new_db;
	bool success;
};

static int backup_copy_fn(struct db_record *orig_rec, void *state)
{
	struct tdbsam_backup_state *bs = (struct tdbsam_backup_state *)state;
	struct db_record *new_rec;
	NTSTATUS status;

	new_rec = bs->new_db->fetch_locked(bs->new_db, talloc_tos(), orig_rec->key);
	if (new_rec == NULL) {
		bs->success = false;
		return 1;
	}

	status = new_rec->store(new_rec, orig_rec->value, TDB_INSERT);

	TALLOC_FREE(new_rec);

	if (!NT_STATUS_IS_OK(status)) {
		bs->success = false;
                return 1;
        }
        return 0;
}

/**********************************************************************
 Make a backup of an old passdb and replace the new one with it. We
 have to do this as between 3.0.x and 3.2.x the hash function changed
 by mistake (used unsigned char * instead of char *). This means the
 previous simple update code will fail due to not being able to find
 existing records to replace in the tdbsam_convert_one() function. JRA.
 *********************************************************************/

static bool tdbsam_convert_backup(const char *dbname, struct db_context **pp_db)
{
	TALLOC_CTX *frame = talloc_stackframe();
	const char *tmp_fname = NULL;
	struct db_context *tmp_db = NULL;
	struct db_context *orig_db = *pp_db;
	struct tdbsam_backup_state bs;
	int ret;

	tmp_fname = talloc_asprintf(frame, "%s.tmp", dbname);
	if (!tmp_fname) {
		TALLOC_FREE(frame);
		return false;
	}

	unlink(tmp_fname);

	/* Remember to open this on the NULL context. We need
	 * it to stay around after we return from here. */

	tmp_db = db_open_trans(NULL, tmp_fname, 0,
				TDB_DEFAULT, O_CREAT|O_RDWR, 0600);
	if (tmp_db == NULL) {
		DEBUG(0, ("tdbsam_convert_backup: Failed to create backup TDB passwd "
			  "[%s]\n", tmp_fname));
		TALLOC_FREE(frame);
		return false;
	}

	if (orig_db->transaction_start(orig_db) != 0) {
		DEBUG(0, ("tdbsam_convert_backup: Could not start transaction (1)\n"));
		unlink(tmp_fname);
		TALLOC_FREE(tmp_db);
		TALLOC_FREE(frame);
		return false;
	}
	if (tmp_db->transaction_start(tmp_db) != 0) {
		DEBUG(0, ("tdbsam_convert_backup: Could not start transaction (2)\n"));
		orig_db->transaction_cancel(orig_db);
		unlink(tmp_fname);
		TALLOC_FREE(tmp_db);
		TALLOC_FREE(frame);
		return false;
	}

	bs.new_db = tmp_db;
	bs.success = true;

        ret = orig_db->traverse(orig_db, backup_copy_fn, (void *)&bs);
        if (ret < 0) {
                DEBUG(0, ("tdbsam_convert_backup: traverse failed\n"));
                goto cancel;
        }

	if (!bs.success) {
		DEBUG(0, ("tdbsam_convert_backup: Rewriting records failed\n"));
		goto cancel;
	}

	if (orig_db->transaction_commit(orig_db) != 0) {
		smb_panic("tdbsam_convert_backup: orig commit failed\n");
	}
	if (tmp_db->transaction_commit(tmp_db) != 0) {
		smb_panic("tdbsam_convert_backup: orig commit failed\n");
	}

	/* be sure to close the DBs _before_ renaming the file */

	TALLOC_FREE(orig_db);
	TALLOC_FREE(tmp_db);

	/* This is safe from other users as we know we're
 	 * under a mutex here. */

	if (rename(tmp_fname, dbname) == -1) {
		DEBUG(0, ("tdbsam_convert_backup: rename of %s to %s failed %s\n",
			tmp_fname,
			dbname,
			strerror(errno)));
		smb_panic("tdbsam_convert_backup: replace passdb failed\n");
	}

	TALLOC_FREE(frame);

	/* re-open the converted TDB */

	orig_db = db_open_trans(NULL, dbname, 0,
				TDB_DEFAULT, O_CREAT|O_RDWR, 0600);
	if (orig_db == NULL) {
		DEBUG(0, ("tdbsam_convert_backup: Failed to re-open "
			  "converted passdb TDB [%s]\n", dbname));
		return false;
	}

	DEBUG(1, ("tdbsam_convert_backup: updated %s file.\n",
		dbname ));

	/* Replace the global db pointer. */
	*pp_db = orig_db;
	return true;

  cancel:

	if (orig_db->transaction_cancel(orig_db) != 0) {
		smb_panic("tdbsam_convert: transaction_cancel failed");
	}

	if (tmp_db->transaction_cancel(tmp_db) != 0) {
		smb_panic("tdbsam_convert: transaction_cancel failed");
	}

	unlink(tmp_fname);
	TALLOC_FREE(tmp_db);
	TALLOC_FREE(frame);
	return false;
}

static bool tdbsam_convert(struct db_context **pp_db, const char *name, int32 from)
{
	struct tdbsam_convert_state state;
	struct db_context *db = NULL;
	int ret;

	/* We only need the update backup for local db's. */
	if (db_is_local(name) && !tdbsam_convert_backup(name, pp_db)) {
		DEBUG(0, ("tdbsam_convert: Could not backup %s\n", name));
		return false;
	}

	db = *pp_db;
	state.from = from;
	state.success = true;

	if (db->transaction_start(db) != 0) {
		DEBUG(0, ("tdbsam_convert: Could not start transaction\n"));
		return false;
	}

	ret = db->traverse(db, tdbsam_convert_one, &state);
	if (ret < 0) {
		DEBUG(0, ("tdbsam_convert: traverse failed\n"));
		goto cancel;
	}

	if (!state.success) {
		DEBUG(0, ("tdbsam_convert: Converting records failed\n"));
		goto cancel;
	}

	if (dbwrap_store_int32(db, TDBSAM_VERSION_STRING,
			       TDBSAM_VERSION) != 0) {
		DEBUG(0, ("tdbsam_convert: Could not store tdbsam version\n"));
		goto cancel;
	}

	if (dbwrap_store_int32(db, TDBSAM_MINOR_VERSION_STRING,
			       TDBSAM_MINOR_VERSION) != 0) {
		DEBUG(0, ("tdbsam_convert: Could not store tdbsam minor version\n"));
		goto cancel;
	}

	if (db->transaction_commit(db) != 0) {
		DEBUG(0, ("tdbsam_convert: Could not commit transaction\n"));
		goto cancel;
	}

	return true;

 cancel:
	if (db->transaction_cancel(db) != 0) {
		smb_panic("tdbsam_convert: transaction_cancel failed");
	}

	return false;
}

/*********************************************************************
 Open the tdbsam file based on the absolute path specified.
 Uses a reference count to allow multiple open calls.
*********************************************************************/

static bool tdbsam_open( const char *name )
{
	int32	version;
	int32	minor_version;

	/* check if we are already open */

	if ( db_sam ) {
		return true;
	}

	/* Try to open tdb passwd.  Create a new one if necessary */

	db_sam = db_open_trans(NULL, name, 0, TDB_DEFAULT, O_CREAT|O_RDWR, 0600);
	if (db_sam == NULL) {
		DEBUG(0, ("tdbsam_open: Failed to open/create TDB passwd "
			  "[%s]\n", name));
		return false;
	}

	/* Check the version */
	version = dbwrap_fetch_int32(db_sam, TDBSAM_VERSION_STRING);
	if (version == -1) {
		version = 0;	/* Version not found, assume version 0 */
	}

	/* Get the minor version */
	minor_version = dbwrap_fetch_int32(db_sam, TDBSAM_MINOR_VERSION_STRING);
	if (minor_version == -1) {
		minor_version = 0; /* Minor version not found, assume 0 */
	}

	/* Compare the version */
	if (version > TDBSAM_VERSION) {
		/* Version more recent than the latest known */
		DEBUG(0, ("tdbsam_open: unknown version => %d\n", version));
		TALLOC_FREE(db_sam);
		return false;
	}

	if ( version < TDBSAM_VERSION ||
			(version == TDBSAM_VERSION &&
			 minor_version < TDBSAM_MINOR_VERSION) ) {
		/*
		 * Ok - we think we're going to have to convert.
		 * Due to the backup process we now must do to
		 * upgrade we have to get a mutex and re-check
		 * the version. Someone else may have upgraded
		 * whilst we were checking.
		 */

		struct named_mutex *mtx = grab_named_mutex(NULL,
						"tdbsam_upgrade_mutex",
						600);

		if (!mtx) {
			DEBUG(0, ("tdbsam_open: failed to grab mutex.\n"));
			TALLOC_FREE(db_sam);
			return false;
		}

		/* Re-check the version */
		version = dbwrap_fetch_int32(db_sam, TDBSAM_VERSION_STRING);
		if (version == -1) {
			version = 0;	/* Version not found, assume version 0 */
		}

		/* Re-check the minor version */
		minor_version = dbwrap_fetch_int32(db_sam, TDBSAM_MINOR_VERSION_STRING);
		if (minor_version == -1) {
			minor_version = 0; /* Minor version not found, assume 0 */
		}

		/* Compare the version */
		if (version > TDBSAM_VERSION) {
			/* Version more recent than the latest known */
			DEBUG(0, ("tdbsam_open: unknown version => %d\n", version));
			TALLOC_FREE(db_sam);
			TALLOC_FREE(mtx);
			return false;
		}

		if ( version < TDBSAM_VERSION ||
				(version == TDBSAM_VERSION &&
				 minor_version < TDBSAM_MINOR_VERSION) ) {
			/*
			 * Note that minor versions we read that are greater
			 * than the current minor version we have hard coded
			 * are assumed to be compatible if they have the same
			 * major version. That allows previous versions of the
			 * passdb code that don't know about minor versions to
			 * still use this database. JRA.
			 */

			DEBUG(1, ("tdbsam_open: Converting version %d.%d database to "
				  "version %d.%d.\n",
					version,
					minor_version,
					TDBSAM_VERSION,
					TDBSAM_MINOR_VERSION));

			if ( !tdbsam_convert(&db_sam, name, version) ) {
				DEBUG(0, ("tdbsam_open: Error when trying to convert "
					  "tdbsam [%s]\n",name));
				TALLOC_FREE(db_sam);
				TALLOC_FREE(mtx);
				return false;
			}

			DEBUG(3, ("TDBSAM converted successfully.\n"));
		}
		TALLOC_FREE(mtx);
	}

	DEBUG(4,("tdbsam_open: successfully opened %s\n", name ));

	return true;
}

/******************************************************************
 Lookup a name in the SAM TDB
******************************************************************/

static NTSTATUS tdbsam_getsampwnam (struct pdb_methods *my_methods,
				    struct samu *user, const char *sname)
{
	TDB_DATA 	data;
	fstring 	keystr;
	fstring		name;

	if ( !user ) {
		DEBUG(0,("pdb_getsampwnam: struct samu is NULL.\n"));
		return NT_STATUS_NO_MEMORY;
	}

	/* Data is stored in all lower-case */
	fstrcpy(name, sname);
	strlower_m(name);

	/* set search key */
	slprintf(keystr, sizeof(keystr)-1, "%s%s", USERPREFIX, name);

	/* open the database */

	if ( !tdbsam_open( tdbsam_filename ) ) {
		DEBUG(0,("tdbsam_getsampwnam: failed to open %s!\n", tdbsam_filename));
		return NT_STATUS_ACCESS_DENIED;
	}

	/* get the record */

	data = dbwrap_fetch_bystring(db_sam, talloc_tos(), keystr);
	if (!data.dptr) {
		DEBUG(5,("pdb_getsampwnam (TDB): error fetching database.\n"));
		DEBUGADD(5, (" Key: %s\n", keystr));
		return NT_STATUS_NO_SUCH_USER;
	}

  	/* unpack the buffer */

	if (!init_sam_from_buffer(user, data.dptr, data.dsize)) {
		DEBUG(0,("pdb_getsampwent: Bad struct samu entry returned from TDB!\n"));
		SAFE_FREE(data.dptr);
		return NT_STATUS_NO_MEMORY;
	}

	/* success */

	TALLOC_FREE(data.dptr);

	return NT_STATUS_OK;
}

/***************************************************************************
 Search by rid
 **************************************************************************/

static NTSTATUS tdbsam_getsampwrid (struct pdb_methods *my_methods,
				    struct samu *user, uint32 rid)
{
	NTSTATUS                nt_status = NT_STATUS_UNSUCCESSFUL;
	TDB_DATA 		data;
	fstring 		keystr;
	fstring			name;

	if ( !user ) {
		DEBUG(0,("pdb_getsampwrid: struct samu is NULL.\n"));
		return nt_status;
	}

	/* set search key */

	slprintf(keystr, sizeof(keystr)-1, "%s%.8x", RIDPREFIX, rid);

	/* open the database */

	if ( !tdbsam_open( tdbsam_filename ) ) {
		DEBUG(0,("tdbsam_getsampwnam: failed to open %s!\n", tdbsam_filename));
		return NT_STATUS_ACCESS_DENIED;
	}

	/* get the record */

	data = dbwrap_fetch_bystring(db_sam, talloc_tos(), keystr);
	if (!data.dptr) {
		DEBUG(5,("pdb_getsampwrid (TDB): error looking up RID %d by key %s.\n", rid, keystr));
		return NT_STATUS_UNSUCCESSFUL;
	}

	fstrcpy(name, (const char *)data.dptr);
	TALLOC_FREE(data.dptr);

	return tdbsam_getsampwnam (my_methods, user, name);
}

static NTSTATUS tdbsam_getsampwsid(struct pdb_methods *my_methods,
				   struct samu * user, const DOM_SID *sid)
{
	uint32 rid;

	if ( !sid_peek_check_rid(get_global_sam_sid(), sid, &rid) )
		return NT_STATUS_UNSUCCESSFUL;

	return tdbsam_getsampwrid(my_methods, user, rid);
}

static bool tdb_delete_samacct_only( struct samu *sam_pass )
{
	fstring 	keystr;
	fstring		name;
	NTSTATUS status;

	fstrcpy(name, pdb_get_username(sam_pass));
	strlower_m(name);

  	/* set the search key */

	slprintf(keystr, sizeof(keystr)-1, "%s%s", USERPREFIX, name);

	/* it's outaa here!  8^) */

	status = dbwrap_delete_bystring(db_sam, keystr);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(5, ("Error deleting entry from tdb passwd "
			  "database: %s!\n", nt_errstr(status)));
		return false;
	}

	return true;
}

/***************************************************************************
 Delete a struct samu records for the username and RID key
****************************************************************************/

static NTSTATUS tdbsam_delete_sam_account(struct pdb_methods *my_methods,
					  struct samu *sam_pass)
{
	NTSTATUS        nt_status = NT_STATUS_UNSUCCESSFUL;
	fstring 	keystr;
	uint32		rid;
	fstring		name;

	/* open the database */

	if ( !tdbsam_open( tdbsam_filename ) ) {
		DEBUG(0,("tdbsam_delete_sam_account: failed to open %s!\n",
			 tdbsam_filename));
		return NT_STATUS_ACCESS_DENIED;
	}

	fstrcpy(name, pdb_get_username(sam_pass));
	strlower_m(name);

  	/* set the search key */

	slprintf(keystr, sizeof(keystr)-1, "%s%s", USERPREFIX, name);

	rid = pdb_get_user_rid(sam_pass);

	/* it's outaa here!  8^) */

	if (db_sam->transaction_start(db_sam) != 0) {
		DEBUG(0, ("Could not start transaction\n"));
		return NT_STATUS_UNSUCCESSFUL;
	}

	nt_status = dbwrap_delete_bystring(db_sam, keystr);
	if (!NT_STATUS_IS_OK(nt_status)) {
		DEBUG(5, ("Error deleting entry from tdb passwd "
			  "database: %s!\n", nt_errstr(nt_status)));
		goto cancel;
	}

  	/* set the search key */

	slprintf(keystr, sizeof(keystr)-1, "%s%.8x", RIDPREFIX, rid);

	/* it's outaa here!  8^) */

	nt_status = dbwrap_delete_bystring(db_sam, keystr);
	if (!NT_STATUS_IS_OK(nt_status)) {
		DEBUG(5, ("Error deleting entry from tdb rid "
			  "database: %s!\n", nt_errstr(nt_status)));
		goto cancel;
	}

	if (db_sam->transaction_commit(db_sam) != 0) {
		DEBUG(0, ("Could not commit transaction\n"));
		goto cancel;
	}

	return NT_STATUS_OK;

 cancel:
	if (db_sam->transaction_cancel(db_sam) != 0) {
		smb_panic("transaction_cancel failed");
	}

	return nt_status;
}


/***************************************************************************
 Update the TDB SAM account record only
 Assumes that the tdbsam is already open 
****************************************************************************/
static bool tdb_update_samacct_only( struct samu* newpwd, int flag )
{
	TDB_DATA 	data;
	uint8		*buf = NULL;
	fstring 	keystr;
	fstring		name;
	bool		ret = false;
	NTSTATUS status;

	/* copy the struct samu struct into a BYTE buffer for storage */

	if ( (data.dsize=init_buffer_from_sam (&buf, newpwd, False)) == -1 ) {
		DEBUG(0,("tdb_update_sam: ERROR - Unable to copy struct samu info BYTE buffer!\n"));
		goto done;
	}
	data.dptr = buf;

	fstrcpy(name, pdb_get_username(newpwd));
	strlower_m(name);

	DEBUG(5, ("Storing %saccount %s with RID %d\n",
		  flag == TDB_INSERT ? "(new) " : "", name,
		  pdb_get_user_rid(newpwd)));

  	/* setup the USER index key */
	slprintf(keystr, sizeof(keystr)-1, "%s%s", USERPREFIX, name);

	/* add the account */

	status = dbwrap_store_bystring(db_sam, keystr, data, flag);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Unable to modify passwd TDB: %s!",
			  nt_errstr(status)));
		goto done;
	}

	ret = true;

done:
	/* cleanup */
	SAFE_FREE(buf);
	return ret;
}

/***************************************************************************
 Update the TDB SAM RID record only
 Assumes that the tdbsam is already open
****************************************************************************/
static bool tdb_update_ridrec_only( struct samu* newpwd, int flag )
{
	TDB_DATA 	data;
	fstring 	keystr;
	fstring		name;
	NTSTATUS status;

	fstrcpy(name, pdb_get_username(newpwd));
	strlower_m(name);

	/* setup RID data */
	data = string_term_tdb_data(name);

	/* setup the RID index key */
	slprintf(keystr, sizeof(keystr)-1, "%s%.8x", RIDPREFIX,
		 pdb_get_user_rid(newpwd));

	/* add the reference */
	status = dbwrap_store_bystring(db_sam, keystr, data, flag);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Unable to modify TDB passwd: %s!\n",
			  nt_errstr(status)));
		return false;
	}

	return true;

}

/***************************************************************************
 Update the TDB SAM
****************************************************************************/

static bool tdb_update_sam(struct pdb_methods *my_methods, struct samu* newpwd,
			   int flag)
{
	uint32_t oldrid;
	uint32_t newrid;

	if (!(newrid = pdb_get_user_rid(newpwd))) {
		DEBUG(0,("tdb_update_sam: struct samu (%s) with no RID!\n",
			 pdb_get_username(newpwd)));
		return False;
	}

	oldrid = newrid;

	/* open the database */

	if ( !tdbsam_open( tdbsam_filename ) ) {
		DEBUG(0,("tdbsam_getsampwnam: failed to open %s!\n", tdbsam_filename));
		return False;
	}

	if (db_sam->transaction_start(db_sam) != 0) {
		DEBUG(0, ("Could not start transaction\n"));
		return false;
	}

	/* If we are updating, we may be changing this users RID. Retrieve the old RID
	   so we can check. */

	if (flag == TDB_MODIFY) {
		struct samu *account = samu_new(talloc_tos());
		if (account == NULL) {
			DEBUG(0,("tdb_update_sam: samu_new() failed\n"));
			goto cancel;
		}
		if (!NT_STATUS_IS_OK(tdbsam_getsampwnam(my_methods, account, pdb_get_username(newpwd)))) {
			DEBUG(0,("tdb_update_sam: tdbsam_getsampwnam() for %s failed\n",
				pdb_get_username(newpwd)));
			TALLOC_FREE(account);
			goto cancel;
		}
		if (!(oldrid = pdb_get_user_rid(account))) {
			DEBUG(0,("tdb_update_sam: pdb_get_user_rid() failed\n"));
			TALLOC_FREE(account);
			goto cancel;
		}
		TALLOC_FREE(account);
	}

	/* Update the new samu entry. */
	if (!tdb_update_samacct_only(newpwd, flag)) {
		goto cancel;
	}

	/* Now take care of the case where the RID changed. We need
	 * to delete the old RID key and add the new. */

	if (flag == TDB_MODIFY && newrid != oldrid) { 
		fstring keystr;

		/* Delete old RID key */
		DEBUG(10, ("tdb_update_sam: Deleting key for RID %u\n", oldrid));
		slprintf(keystr, sizeof(keystr) - 1, "%s%.8x", RIDPREFIX, oldrid);
		if (!NT_STATUS_IS_OK(dbwrap_delete_bystring(db_sam, keystr))) {
			DEBUG(0, ("tdb_update_sam: Can't delete %s\n", keystr));
			goto cancel;
		}
		/* Insert new RID key */
		DEBUG(10, ("tdb_update_sam: Inserting key for RID %u\n", newrid));
		if (!tdb_update_ridrec_only(newpwd, TDB_INSERT)) {
			goto cancel;
		}
	} else {
		DEBUG(10, ("tdb_update_sam: %s key for RID %u\n",
			flag == TDB_MODIFY ? "Updating" : "Inserting", newrid));
		if (!tdb_update_ridrec_only(newpwd, flag)) {
			goto cancel;
		}
	}

	if (db_sam->transaction_commit(db_sam) != 0) {
		DEBUG(0, ("Could not commit transaction\n"));
		goto cancel;
	}

	return true;

 cancel:
	if (db_sam->transaction_cancel(db_sam) != 0) {
		smb_panic("transaction_cancel failed");
	}
	return false;
}

/***************************************************************************
 Modifies an existing struct samu
****************************************************************************/

static NTSTATUS tdbsam_update_sam_account (struct pdb_methods *my_methods, struct samu *newpwd)
{
	if ( !tdb_update_sam(my_methods, newpwd, TDB_MODIFY) )
		return NT_STATUS_UNSUCCESSFUL;
	
	return NT_STATUS_OK;
}

/***************************************************************************
 Adds an existing struct samu
****************************************************************************/

static NTSTATUS tdbsam_add_sam_account (struct pdb_methods *my_methods, struct samu *newpwd)
{
	if ( !tdb_update_sam(my_methods, newpwd, TDB_INSERT) )
		return NT_STATUS_UNSUCCESSFUL;
		
	return NT_STATUS_OK;
}

/***************************************************************************
 Renames a struct samu
 - check for the posix user/rename user script
 - Add and lock the new user record
 - rename the posix user
 - rewrite the rid->username record
 - delete the old user
 - unlock the new user record
***************************************************************************/
static NTSTATUS tdbsam_rename_sam_account(struct pdb_methods *my_methods,
					  struct samu *old_acct,
					  const char *newname)
{
	struct samu      *new_acct = NULL;
	char *rename_script = NULL;
	int              rename_ret;
	fstring          oldname_lower;
	fstring          newname_lower;

	/* can't do anything without an external script */

	if ( !(new_acct = samu_new( talloc_tos() )) ) {
		return NT_STATUS_NO_MEMORY;
	}

	rename_script = talloc_strdup(new_acct, lp_renameuser_script());
	if (!rename_script) {
		TALLOC_FREE(new_acct);
		return NT_STATUS_NO_MEMORY;
	}
	if (!*rename_script) {
		TALLOC_FREE(new_acct);
		return NT_STATUS_ACCESS_DENIED;
	}

	if ( !pdb_copy_sam_account(new_acct, old_acct)
		|| !pdb_set_username(new_acct, newname, PDB_CHANGED))
	{
		TALLOC_FREE(new_acct);
		return NT_STATUS_NO_MEMORY;
	}

	/* open the database */
	if ( !tdbsam_open( tdbsam_filename ) ) {
		DEBUG(0, ("tdbsam_getsampwnam: failed to open %s!\n",
			  tdbsam_filename));
		TALLOC_FREE(new_acct);
		return NT_STATUS_ACCESS_DENIED;
	}

	if (db_sam->transaction_start(db_sam) != 0) {
		DEBUG(0, ("Could not start transaction\n"));
		TALLOC_FREE(new_acct);
		return NT_STATUS_ACCESS_DENIED;

	}

	/* add the new account and lock it */
	if ( !tdb_update_samacct_only(new_acct, TDB_INSERT) ) {
		goto cancel;
	}

	/* Rename the posix user.  Follow the semantics of _samr_create_user()
	   so that we lower case the posix name but preserve the case in passdb */

	fstrcpy( oldname_lower, pdb_get_username(old_acct) );
	strlower_m( oldname_lower );

	fstrcpy( newname_lower, newname );
	strlower_m( newname_lower );

	rename_script = talloc_string_sub2(new_acct,
				rename_script,
				"%unew",
				newname_lower,
				true,
				false,
				true);
	if (!rename_script) {
		goto cancel;
	}
	rename_script = talloc_string_sub2(new_acct,
				rename_script,
				"%uold",
				oldname_lower,
				true,
				false,
				true);
	if (!rename_script) {
		goto cancel;
	}
	rename_ret = smbrun(rename_script, NULL);

	DEBUG(rename_ret ? 0 : 3,("Running the command `%s' gave %d\n",
				rename_script, rename_ret));

	if (rename_ret != 0) {
		goto cancel;
	}

#if 0 /* tonywu */
	smb_nscd_flush_user_cache();
#endif

	/* rewrite the rid->username record */

	if ( !tdb_update_ridrec_only( new_acct, TDB_MODIFY) ) {
		goto cancel;
	}

	tdb_delete_samacct_only( old_acct );

	if (db_sam->transaction_commit(db_sam) != 0) {
		/*
		 * Ok, we're screwed. We've changed the posix account, but
		 * could not adapt passdb.tdb. Shall we change the posix
		 * account back?
		 */
		DEBUG(0, ("transaction_commit failed\n"));
		goto cancel;
	}

	TALLOC_FREE(new_acct );
	return NT_STATUS_OK;

 cancel:
	if (db_sam->transaction_cancel(db_sam) != 0) {
		smb_panic("transaction_cancel failed");
	}

	TALLOC_FREE(new_acct);

	return NT_STATUS_ACCESS_DENIED;	
}

static bool tdbsam_rid_algorithm(struct pdb_methods *methods)
{
	return False;
}

/*
 * Historically, winbind was responsible for allocating RIDs, so the next RID
 * value was stored in winbindd_idmap.tdb. It has been moved to passdb now,
 * but for compatibility reasons we still keep the the next RID counter in
 * winbindd_idmap.tdb.
 */

/*****************************************************************************
 Initialise idmap database. For now (Dec 2005) this is a copy of the code in
 sam/idmap_tdb.c. Maybe at a later stage we can remove that capability from
 winbind completely and store the RID counter in passdb.tdb.

 Dont' fully initialize with the HWM values, if it's new, we're only
 interested in the RID counter.
*****************************************************************************/

static bool init_idmap_tdb(TDB_CONTEXT *tdb)
{
	int32 version;

	if (tdb_lock_bystring(tdb, "IDMAP_VERSION") != 0) {
		DEBUG(0, ("Could not lock IDMAP_VERSION\n"));
		return False;
	}

	version = tdb_fetch_int32(tdb, "IDMAP_VERSION");

	if (version == -1) {
		/* No key found, must be a new db */
		if (tdb_store_int32(tdb, "IDMAP_VERSION",
				    IDMAP_VERSION) != 0) {
			DEBUG(0, ("Could not store IDMAP_VERSION\n"));
			tdb_unlock_bystring(tdb, "IDMAP_VERSION");
			return False;
		}
		version = IDMAP_VERSION;
	}

	if (version != IDMAP_VERSION) {
		DEBUG(0, ("Expected IDMAP_VERSION=%d, found %d. Please "
			  "start winbind once\n", IDMAP_VERSION, version));
		tdb_unlock_bystring(tdb, "IDMAP_VERSION");
		return False;
	}

	tdb_unlock_bystring(tdb, "IDMAP_VERSION");
	return True;
}

static bool tdbsam_new_rid(struct pdb_methods *methods, uint32 *prid)
{
	TDB_CONTEXT *tdb;
	uint32 rid;
	bool ret = False;

	tdb = tdb_open_log(state_path("winbindd_idmap.tdb"), 0,
			   TDB_DEFAULT, O_RDWR | O_CREAT, 0644);

	if (tdb == NULL) {
		DEBUG(1, ("Could not open idmap: %s\n", strerror(errno)));
		goto done;
	}

	if (!init_idmap_tdb(tdb)) {
		DEBUG(1, ("Could not init idmap\n"));
		goto done;
	}

	rid = BASE_RID;		/* Default if not set */

	if (!tdb_change_uint32_atomic(tdb, "RID_COUNTER", &rid, 1)) {
		DEBUG(3, ("tdbsam_new_rid: Failed to increase RID_COUNTER\n"));
		goto done;
	}

	*prid = rid;
	ret = True;

 done:
	if ((tdb != NULL) && (tdb_close(tdb) != 0)) {
		smb_panic("tdb_close(idmap_tdb) failed");
	}

	return ret;
}

struct tdbsam_search_state {
	struct pdb_methods *methods;
	uint32_t acct_flags;

	uint32_t *rids;
	uint32_t num_rids;
	ssize_t array_size;
	uint32_t current;
};

static int tdbsam_collect_rids(struct db_record *rec, void *private_data)
{
	struct tdbsam_search_state *state = talloc_get_type_abort(
		private_data, struct tdbsam_search_state);
	size_t prefixlen = strlen(RIDPREFIX);
	uint32 rid;

	if ((rec->key.dsize < prefixlen)
	    || (strncmp((char *)rec->key.dptr, RIDPREFIX, prefixlen))) {
		return 0;
	}

	rid = strtoul((char *)rec->key.dptr+prefixlen, NULL, 16);

	ADD_TO_LARGE_ARRAY(state, uint32, rid, &state->rids, &state->num_rids,
			   &state->array_size);

	return 0;
}

static void tdbsam_search_end(struct pdb_search *search)
{
	struct tdbsam_search_state *state = talloc_get_type_abort(
		search->private_data, struct tdbsam_search_state);
	TALLOC_FREE(state);
}

static bool tdbsam_search_next_entry(struct pdb_search *search,
				     struct samr_displayentry *entry)
{
	struct tdbsam_search_state *state = talloc_get_type_abort(
		search->private_data, struct tdbsam_search_state);
	struct samu *user = NULL;
	NTSTATUS status;
	uint32_t rid;

 again:
	TALLOC_FREE(user);
	user = samu_new(talloc_tos());
	if (user == NULL) {
		DEBUG(0, ("samu_new failed\n"));
		return false;
	}

	if (state->current == state->num_rids) {
		return false;
	}

	rid = state->rids[state->current++];

	status = tdbsam_getsampwrid(state->methods, user, rid);

	if (NT_STATUS_EQUAL(status, NT_STATUS_NO_SUCH_USER)) {
		/*
		 * Someone has deleted that user since we listed the RIDs
		 */
		goto again;
	}

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("tdbsam_getsampwrid failed: %s\n",
			   nt_errstr(status)));
		TALLOC_FREE(user);
		return false;
	}

	if ((state->acct_flags != 0) &&
	    ((state->acct_flags & pdb_get_acct_ctrl(user)) == 0)) {
		goto again;
	}

	entry->acct_flags = pdb_get_acct_ctrl(user);
	entry->rid = rid;
	entry->account_name = talloc_strdup(
		search->mem_ctx, pdb_get_username(user));
	entry->fullname = talloc_strdup(
		search->mem_ctx, pdb_get_fullname(user));
	entry->description = talloc_strdup(
		search->mem_ctx, pdb_get_acct_desc(user));

	TALLOC_FREE(user);

	if ((entry->account_name == NULL) || (entry->fullname == NULL)
	    || (entry->description == NULL)) {
		DEBUG(0, ("talloc_strdup failed\n"));
		return false;
	}

	return true;
}

static bool tdbsam_search_users(struct pdb_methods *methods,
				struct pdb_search *search,
				uint32 acct_flags)
{
	struct tdbsam_search_state *state;

	if (!tdbsam_open(tdbsam_filename)) {
		DEBUG(0,("tdbsam_getsampwnam: failed to open %s!\n",
			 tdbsam_filename));
		return false;
	}

	state = TALLOC_ZERO_P(search->mem_ctx, struct tdbsam_search_state);
	if (state == NULL) {
		DEBUG(0, ("talloc failed\n"));
		return false;
	}
	state->acct_flags = acct_flags;
	state->methods = methods;

	db_sam->traverse_read(db_sam, tdbsam_collect_rids, state);

	search->private_data = state;
	search->next_entry = tdbsam_search_next_entry;
	search->search_end = tdbsam_search_end;

	return true;
}

/*********************************************************************
 Initialize the tdb sam backend.  Setup the dispath table of methods,
 open the tdb, etc...
*********************************************************************/

static NTSTATUS pdb_init_tdbsam(struct pdb_methods **pdb_method, const char *location)
{
	NTSTATUS nt_status;
	char *tdbfile = NULL;
	const char *pfile = location;

	if (!NT_STATUS_IS_OK(nt_status = make_pdb_method( pdb_method ))) {
		return nt_status;
	}

	(*pdb_method)->name = "tdbsam";

	(*pdb_method)->getsampwnam = tdbsam_getsampwnam;
	(*pdb_method)->getsampwsid = tdbsam_getsampwsid;
	(*pdb_method)->add_sam_account = tdbsam_add_sam_account;
	(*pdb_method)->update_sam_account = tdbsam_update_sam_account;
	(*pdb_method)->delete_sam_account = tdbsam_delete_sam_account;
	(*pdb_method)->rename_sam_account = tdbsam_rename_sam_account;
	(*pdb_method)->search_users = tdbsam_search_users;

	(*pdb_method)->rid_algorithm = tdbsam_rid_algorithm;
	(*pdb_method)->new_rid = tdbsam_new_rid;

	/* save the path for later */

	if (!location) {
		if (asprintf(&tdbfile, "%s/%s", lp_private_dir(),
			     PASSDB_FILE_NAME) < 0) {
			return NT_STATUS_NO_MEMORY;
		}
		pfile = tdbfile;
	}
	tdbsam_filename = SMB_STRDUP(pfile);
	if (!tdbsam_filename) {
		return NT_STATUS_NO_MEMORY;
	}
	SAFE_FREE(tdbfile);

	/* no private data */

	(*pdb_method)->private_data      = NULL;
	(*pdb_method)->free_private_data = NULL;

	return NT_STATUS_OK;
}

NTSTATUS pdb_tdbsam_init(void)
{
	return smb_register_passdb(PASSDB_INTERFACE_VERSION, "tdbsam", pdb_init_tdbsam);
}
