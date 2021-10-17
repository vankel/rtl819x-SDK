/*
   Unix SMB/CIFS implementation.
   Check access to files based on security descriptors.
   Copyright (C) Jeremy Allison 2005-2006.
   Copyright (C) Michael Adam 2007.

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

extern struct current_user current_user;

#undef  DBGC_CLASS
#define DBGC_CLASS DBGC_ACLS

static bool can_access_file_acl(struct connection_struct *conn,
				const char * fname, SMB_STRUCT_STAT *psbuf,
				uint32_t access_mask)
{
	bool result;
	NTSTATUS status;
	uint32_t access_granted;
	struct security_descriptor *secdesc = NULL;

	status = SMB_VFS_GET_NT_ACL(conn, fname,
				    (OWNER_SECURITY_INFORMATION |
				     GROUP_SECURITY_INFORMATION |
				     DACL_SECURITY_INFORMATION),
				    &secdesc);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(5, ("Could not get acl: %s\n", nt_errstr(status)));
		return false;
	}

	result = se_access_check(secdesc, current_user.nt_user_token,
				 access_mask, &access_granted, &status);
	TALLOC_FREE(secdesc);
	return result;
}

/****************************************************************************
 Actually emulate the in-kernel access checking for delete access. We need
 this to successfully return ACCESS_DENIED on a file open for delete access.
****************************************************************************/

bool can_delete_file_in_directory(connection_struct *conn, const char *fname)
{
	SMB_STRUCT_STAT sbuf;
	TALLOC_CTX *ctx = talloc_tos();
	char *dname = NULL;

	if (!CAN_WRITE(conn)) {
		return False;
	}

	/* Get the parent directory permission mask and owners. */
	if (!parent_dirname_talloc(ctx,
				fname,
				&dname,
				NULL)) {
		return False;
	}
	if(SMB_VFS_STAT(conn, dname, &sbuf) != 0) {
		return False;
	}

	/* fast paths first */

	if (!S_ISDIR(sbuf.st_mode)) {
		return False;
	}
	if (current_user.ut.uid == 0 || conn->admin_user) {
		/* I'm sorry sir, I didn't know you were root... */
		return True;
	}

	/* Check primary owner write access. */
	if (current_user.ut.uid == sbuf.st_uid) {
		return (sbuf.st_mode & S_IWUSR) ? True : False;
	}

#ifdef S_ISVTX
	/* sticky bit means delete only by owner or root. */
	if (sbuf.st_mode & S_ISVTX) {
		SMB_STRUCT_STAT sbuf_file;
		if(SMB_VFS_STAT(conn, fname, &sbuf_file) != 0) {
			if (errno == ENOENT) {
				/* If the file doesn't already exist then
				 * yes we'll be able to delete it. */
				return True;
			}
			return False;
		}
		/*
		 * Patch from SATOH Fumiyasu <fumiyas@miraclelinux.com>
		 * for bug #3348. Don't assume owning sticky bit
		 * directory means write access allowed.
		 */
		if (current_user.ut.uid != sbuf_file.st_uid) {
			return False;
		}
	}
#endif

	/* now for ACL checks */

	return can_access_file_acl(conn, dname, &sbuf, FILE_WRITE_DATA);
}

/****************************************************************************
 Actually emulate the in-kernel access checking for read/write access. We need
 this to successfully check for ability to write for dos filetimes.
 Note this doesn't take into account share write permissions.
****************************************************************************/

bool can_access_file(connection_struct *conn, const char *fname, SMB_STRUCT_STAT *psbuf, uint32 access_mask)
{
	if (!(access_mask & (FILE_READ_DATA|FILE_WRITE_DATA))) {
		return False;
	}
	access_mask &= (FILE_READ_DATA|FILE_WRITE_DATA);

	/* some fast paths first */

	DEBUG(10,("can_access_file: requesting 0x%x on file %s\n",
		(unsigned int)access_mask, fname ));

	if (current_user.ut.uid == 0 || conn->admin_user) {
		/* I'm sorry sir, I didn't know you were root... */
		return True;
	}

	if (!VALID_STAT(*psbuf)) {
		/* Get the file permission mask and owners. */
		if(SMB_VFS_STAT(conn, fname, psbuf) != 0) {
			return False;
		}
	}

	/* Check primary owner access. */
	if (current_user.ut.uid == psbuf->st_uid) {
		switch (access_mask) {
			case FILE_READ_DATA:
				return (psbuf->st_mode & S_IRUSR) ? True : False;

			case FILE_WRITE_DATA:
				return (psbuf->st_mode & S_IWUSR) ? True : False;

			default: /* FILE_READ_DATA|FILE_WRITE_DATA */

				if ((psbuf->st_mode & (S_IWUSR|S_IRUSR)) == (S_IWUSR|S_IRUSR)) {
					return True;
				} else {
					return False;
				}
		}
	}

	/* now for ACL checks */

	return can_access_file_acl(conn, fname, psbuf, access_mask);
}

/****************************************************************************
 Userspace check for write access.
 Note this doesn't take into account share write permissions.
****************************************************************************/

bool can_write_to_file(connection_struct *conn, const char *fname, SMB_STRUCT_STAT *psbuf)
{
	return can_access_file(conn, fname, psbuf, FILE_WRITE_DATA);
}

