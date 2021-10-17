/* 
   Samba Unix/Linux SMB client library 
   net ads cldap functions 
   Copyright (C) 2001 Andrew Tridgell (tridge@samba.org)
   Copyright (C) 2003 Jim McDonough (jmcd@us.ibm.com)
   Copyright (C) 2008 Guenther Deschner (gd@samba.org)

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

/*
  do a cldap netlogon query
*/
static int send_cldap_netlogon(int sock, const char *domain, 
			       const char *hostname, unsigned ntversion)
{
	ASN1_DATA data;
	char ntver[4];
#ifdef CLDAP_USER_QUERY
	char aac[4];

	SIVAL(aac, 0, 0x00000180);
#endif
	SIVAL(ntver, 0, ntversion);

	memset(&data, 0, sizeof(data));

	asn1_push_tag(&data,ASN1_SEQUENCE(0));
	asn1_write_Integer(&data, 4);
	asn1_push_tag(&data, ASN1_APPLICATION(3));
	asn1_write_OctetString(&data, NULL, 0);
	asn1_write_enumerated(&data, 0);
	asn1_write_enumerated(&data, 0);
	asn1_write_Integer(&data, 0);
	asn1_write_Integer(&data, 0);
	asn1_write_BOOLEAN2(&data, False);
	asn1_push_tag(&data, ASN1_CONTEXT(0));

	if (domain) {
		asn1_push_tag(&data, ASN1_CONTEXT(3));
		asn1_write_OctetString(&data, "DnsDomain", 9);
		asn1_write_OctetString(&data, domain, strlen(domain));
		asn1_pop_tag(&data);
	}

	asn1_push_tag(&data, ASN1_CONTEXT(3));
	asn1_write_OctetString(&data, "Host", 4);
	asn1_write_OctetString(&data, hostname, strlen(hostname));
	asn1_pop_tag(&data);

#ifdef CLDAP_USER_QUERY
	asn1_push_tag(&data, ASN1_CONTEXT(3));
	asn1_write_OctetString(&data, "User", 4);
	asn1_write_OctetString(&data, "SAMBA$", 6);
	asn1_pop_tag(&data);

	asn1_push_tag(&data, ASN1_CONTEXT(3));
	asn1_write_OctetString(&data, "AAC", 4);
	asn1_write_OctetString(&data, aac, 4);
	asn1_pop_tag(&data);
#endif

	asn1_push_tag(&data, ASN1_CONTEXT(3));
	asn1_write_OctetString(&data, "NtVer", 5);
	asn1_write_OctetString(&data, ntver, 4);
	asn1_pop_tag(&data);

	asn1_pop_tag(&data);

	asn1_push_tag(&data,ASN1_SEQUENCE(0));
	asn1_write_OctetString(&data, "NetLogon", 8);
	asn1_pop_tag(&data);
	asn1_pop_tag(&data);
	asn1_pop_tag(&data);

	if (data.has_error) {
		DEBUG(2,("Failed to build cldap netlogon at offset %d\n", (int)data.ofs));
		asn1_free(&data);
		return -1;
	}

	if (write(sock, data.data, data.length) != (ssize_t)data.length) {
		DEBUG(2,("failed to send cldap query (%s)\n", strerror(errno)));
		asn1_free(&data);
		return -1;
	}

	asn1_free(&data);

	return 0;
}

static SIG_ATOMIC_T gotalarm;
                                                                                                                   
/***************************************************************
 Signal function to tell us we timed out.
****************************************************************/
                                                                                                                   
static void gotalarm_sig(void)
{
	gotalarm = 1;
}
                                                                                                                   
/*
  receive a cldap netlogon reply
*/
static int recv_cldap_netlogon(TALLOC_CTX *mem_ctx,
			       int sock,
			       uint32_t *nt_version,
			       union nbt_cldap_netlogon **reply)
{
	int ret;
	ASN1_DATA data;
	DATA_BLOB blob = data_blob_null;
	DATA_BLOB os1 = data_blob_null;
	DATA_BLOB os2 = data_blob_null;
	DATA_BLOB os3 = data_blob_null;
	int i1;
	/* half the time of a regular ldap timeout, not less than 3 seconds. */
	unsigned int al_secs = MAX(3,lp_ldap_timeout()/2);
	union nbt_cldap_netlogon *r = NULL;

	blob = data_blob(NULL, 8192);
	if (blob.data == NULL) {
		DEBUG(1, ("data_blob failed\n"));
		errno = ENOMEM;
		return -1;
	}

	/* Setup timeout */
	gotalarm = 0;
	CatchSignal(SIGALRM, SIGNAL_CAST gotalarm_sig);
	alarm(al_secs);
	/* End setup timeout. */
 
	ret = read(sock, blob.data, blob.length);

	/* Teardown timeout. */
	CatchSignal(SIGALRM, SIGNAL_CAST SIG_IGN);
	alarm(0);

	if (ret <= 0) {
		DEBUG(1,("no reply received to cldap netlogon\n"));
		data_blob_free(&blob);
		return -1;
	}
	blob.length = ret;

	asn1_load(&data, blob);
	asn1_start_tag(&data, ASN1_SEQUENCE(0));
	asn1_read_Integer(&data, &i1);
	asn1_start_tag(&data, ASN1_APPLICATION(4));
	asn1_read_OctetString(&data, &os1);
	asn1_start_tag(&data, ASN1_SEQUENCE(0));
	asn1_start_tag(&data, ASN1_SEQUENCE(0));
	asn1_read_OctetString(&data, &os2);
	asn1_start_tag(&data, ASN1_SET);
	asn1_read_OctetString(&data, &os3);
	asn1_end_tag(&data);
	asn1_end_tag(&data);
	asn1_end_tag(&data);
	asn1_end_tag(&data);
	asn1_end_tag(&data);

	if (data.has_error) {
		data_blob_free(&blob);
		data_blob_free(&os1);
		data_blob_free(&os2);
		data_blob_free(&os3);
		asn1_free(&data);
		DEBUG(1,("Failed to parse cldap reply\n"));
		return -1;
	}

	r = TALLOC_ZERO_P(mem_ctx, union nbt_cldap_netlogon);
	if (!r) {
		errno = ENOMEM;
		data_blob_free(&os1);
		data_blob_free(&os2);
		data_blob_free(&os3);
		data_blob_free(&blob);
		return -1;
	}

	if (!pull_mailslot_cldap_reply(mem_ctx, &os3, r, nt_version)) {
		data_blob_free(&os1);
		data_blob_free(&os2);
		data_blob_free(&os3);
		data_blob_free(&blob);
		TALLOC_FREE(r);
		return -1;
	}

	data_blob_free(&os1);
	data_blob_free(&os2);
	data_blob_free(&os3);
	data_blob_free(&blob);
	
	asn1_free(&data);

	if (reply) {
		*reply = r;
	} else {
		TALLOC_FREE(r);
	}

	return 0;
}

/*******************************************************************
  do a cldap netlogon query.  Always 389/udp
*******************************************************************/

bool ads_cldap_netlogon(TALLOC_CTX *mem_ctx,
			const char *server,
			const char *realm,
			uint32_t *nt_version,
			union nbt_cldap_netlogon **reply)
{
	int sock;
	int ret;

	sock = open_udp_socket(server, LDAP_PORT );
	if (sock == -1) {
		DEBUG(2,("ads_cldap_netlogon: Failed to open udp socket to %s\n", 
			 server));
		return False;
	}

	ret = send_cldap_netlogon(sock, realm, global_myname(), *nt_version);
	if (ret != 0) {
		close(sock);
		return False;
	}
	ret = recv_cldap_netlogon(mem_ctx, sock, nt_version, reply);
	close(sock);

	if (ret == -1) {
		return False;
	}

	return True;
}

/*******************************************************************
  do a cldap netlogon query.  Always 389/udp
*******************************************************************/

bool ads_cldap_netlogon_5(TALLOC_CTX *mem_ctx,
			  const char *server,
			  const char *realm,
			  struct nbt_cldap_netlogon_5 *reply5)
{
	uint32_t nt_version = NETLOGON_VERSION_5 | NETLOGON_VERSION_5EX;
	union nbt_cldap_netlogon *reply = NULL;
	bool ret;

	ret = ads_cldap_netlogon(mem_ctx, server, realm, &nt_version, &reply);
	if (!ret) {
		return false;
	}

	if (nt_version != (NETLOGON_VERSION_5 | NETLOGON_VERSION_5EX)) {
		return false;
	}

	*reply5 = reply->logon5;

	return true;
}

/****************************************************************
****************************************************************/

bool pull_mailslot_cldap_reply(TALLOC_CTX *mem_ctx,
			       const DATA_BLOB *blob,
			       union nbt_cldap_netlogon *r,
			       uint32_t *nt_version)
{
	enum ndr_err_code ndr_err;
	uint32_t nt_version_query = ((*nt_version) & 0x0000001f);
	uint16_t command = 0;

	ndr_err = ndr_pull_struct_blob(blob, mem_ctx, &command,
			(ndr_pull_flags_fn_t)ndr_pull_uint16);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		return false;
	}

	switch (command) {
		case 0x13: /* 19 */
		case 0x15: /* 21 */
		case 0x17: /* 23 */
		case 0x19: /* 25 */
			 break;
		default:
			DEBUG(1,("got unexpected command: %d (0x%08x)\n",
				command, command));
			return false;
	}

	ndr_err = ndr_pull_union_blob_all(blob, mem_ctx, r, nt_version_query,
		       (ndr_pull_flags_fn_t)ndr_pull_nbt_cldap_netlogon);
	if (NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		goto done;
	}

	/* when the caller requested just those nt_version bits that the server
	 * was able to reply to, we are fine and all done. otherwise we need to
	 * assume downgraded replies which are painfully parsed here - gd */

	if (nt_version_query & NETLOGON_VERSION_WITH_CLOSEST_SITE) {
		nt_version_query &= ~NETLOGON_VERSION_WITH_CLOSEST_SITE;
	}
	ndr_err = ndr_pull_union_blob_all(blob, mem_ctx, r, nt_version_query,
		       (ndr_pull_flags_fn_t)ndr_pull_nbt_cldap_netlogon);
	if (NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		goto done;
	}
	if (nt_version_query & NETLOGON_VERSION_5EX_WITH_IP) {
		nt_version_query &= ~NETLOGON_VERSION_5EX_WITH_IP;
	}
	ndr_err = ndr_pull_union_blob_all(blob, mem_ctx, r, nt_version_query,
		       (ndr_pull_flags_fn_t)ndr_pull_nbt_cldap_netlogon);
	if (NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		goto done;
	}
	if (nt_version_query & NETLOGON_VERSION_5EX) {
		nt_version_query &= ~NETLOGON_VERSION_5EX;
	}
	ndr_err = ndr_pull_union_blob_all(blob, mem_ctx, r, nt_version_query,
		       (ndr_pull_flags_fn_t)ndr_pull_nbt_cldap_netlogon);
	if (NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		goto done;
	}
	if (nt_version_query & NETLOGON_VERSION_5) {
		nt_version_query &= ~NETLOGON_VERSION_5;
	}
	ndr_err = ndr_pull_union_blob_all(blob, mem_ctx, r, nt_version_query,
		       (ndr_pull_flags_fn_t)ndr_pull_nbt_cldap_netlogon);
	if (NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		goto done;
	}

	return false;

 done:
	if (DEBUGLEVEL >= 10) {
		NDR_PRINT_UNION_DEBUG(nbt_cldap_netlogon, nt_version_query, r);
	}

	*nt_version = nt_version_query;

	return true;
}
