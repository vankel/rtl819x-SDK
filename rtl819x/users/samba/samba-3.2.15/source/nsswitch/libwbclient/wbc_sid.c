/*
   Unix SMB/CIFS implementation.

   Winbind client API

   Copyright (C) Gerald (Jerry) Carter 2007


   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Required Headers */

#include "libwbclient.h"


/** @brief Convert a binary SID to a character string
 *
 * @param sid           Binary Security Identifier
 * @param **sid_string  Resulting character string
 *
 * @return #wbcErr
 **/

wbcErr wbcSidToString(const struct wbcDomainSid *sid,
		      char **sid_string)
{
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;
	uint32_t id_auth;
	int i;
	char *tmp = NULL;

	if (!sid) {
		wbc_status = WBC_ERR_INVALID_SID;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	id_auth = sid->id_auth[5] +
		(sid->id_auth[4] << 8) +
		(sid->id_auth[3] << 16) +
		(sid->id_auth[2] << 24);

	tmp = talloc_asprintf(NULL, "S-%d-%d", sid->sid_rev_num, id_auth);
	BAIL_ON_PTR_ERROR(tmp, wbc_status);

	for (i=0; i<sid->num_auths; i++) {
		char *tmp2;
		tmp2 = talloc_asprintf_append(tmp, "-%u", sid->sub_auths[i]);
		BAIL_ON_PTR_ERROR(tmp2, wbc_status);

		tmp = tmp2;
	}

	*sid_string = tmp;
	tmp = NULL;

	wbc_status = WBC_ERR_SUCCESS;

done:
	talloc_free(tmp);

	return wbc_status;
}

/** @brief Convert a character string to a binary SID
 *
 * @param *str          Character string in the form of S-...
 * @param sid           Resulting binary SID
 *
 * @return #wbcErr
 **/

wbcErr wbcStringToSid(const char *str,
		      struct wbcDomainSid *sid)
{
	const char *p;
	char *q;
	uint32_t x;
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;

	if (!sid) {
		wbc_status = WBC_ERR_INVALID_PARAM;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	/* Sanity check for either "S-" or "s-" */

	if (!str
	    || (str[0]!='S' && str[0]!='s')
	    || (str[1]!='-')
	    || (strlen(str)<2))
	{
		wbc_status = WBC_ERR_INVALID_PARAM;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	/* Get the SID revision number */

	p = str+2;
	x = (uint32_t)strtol(p, &q, 10);
	if (x==0 || !q || *q!='-') {
		wbc_status = WBC_ERR_INVALID_SID;
		BAIL_ON_WBC_ERROR(wbc_status);
	}
	sid->sid_rev_num = (uint8_t)x;

	/* Next the Identifier Authority.  This is stored in big-endian
	   in a 6 byte array. */

	p = q+1;
	x = (uint32_t)strtol(p, &q, 10);
	if (!q || *q!='-') {
		wbc_status = WBC_ERR_INVALID_SID;
		BAIL_ON_WBC_ERROR(wbc_status);
	}
	sid->id_auth[5] = (x & 0x000000ff);
	sid->id_auth[4] = (x & 0x0000ff00) >> 8;
	sid->id_auth[3] = (x & 0x00ff0000) >> 16;
	sid->id_auth[2] = (x & 0xff000000) >> 24;
	sid->id_auth[1] = 0;
	sid->id_auth[0] = 0;

	/* now read the the subauthorities */

	p = q +1;
	sid->num_auths = 0;
	while (sid->num_auths < WBC_MAXSUBAUTHS) {
		x=(uint32_t)strtoul(p, &q, 10);
		if (p == q)
			break;
		sid->sub_auths[sid->num_auths++] = x;

		if (q && ((*q!='-') || (*q=='\0')))
			break;
		p = q + 1;
	}

	/* IF we ended early, then the SID could not be converted */

	if (q && *q!='\0') {
		wbc_status = WBC_ERR_INVALID_SID;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	wbc_status = WBC_ERR_SUCCESS;

done:
	return wbc_status;

}

/** @brief Convert a domain and name to SID
 *
 * @param domain      Domain name (possibly "")
 * @param name        User or group name
 * @param *sid        Pointer to the resolved domain SID
 * @param *name_type  Pointet to the SID type
 *
 * @return #wbcErr
 *
 **/

wbcErr wbcLookupName(const char *domain,
		     const char *name,
		     struct wbcDomainSid *sid,
		     enum wbcSidType *name_type)
{
	struct winbindd_request request;
	struct winbindd_response response;
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;

	if (!sid || !name_type) {
		wbc_status = WBC_ERR_INVALID_PARAM;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	/* Initialize request */

	ZERO_STRUCT(request);
	ZERO_STRUCT(response);

	/* dst is already null terminated from the memset above */

	strncpy(request.data.name.dom_name, domain,
		sizeof(request.data.name.dom_name)-1);
	strncpy(request.data.name.name, name,
		sizeof(request.data.name.name)-1);

	wbc_status = wbcRequestResponse(WINBINDD_LOOKUPNAME,
					&request,
					&response);
	BAIL_ON_WBC_ERROR(wbc_status);

	wbc_status = wbcStringToSid(response.data.sid.sid, sid);
	BAIL_ON_WBC_ERROR(wbc_status);

	*name_type = (enum wbcSidType)response.data.sid.type;

	wbc_status = WBC_ERR_SUCCESS;

 done:
	return wbc_status;
}

/** @brief Convert a SID to a domain and name
 *
 * @param *sid        Pointer to the domain SID to be resolved
 * @param domain      Resolved Domain name (possibly "")
 * @param name        Resolved User or group name
 * @param *name_type  Pointet to the resolved SID type
 *
 * @return #wbcErr
 *
 **/

wbcErr wbcLookupSid(const struct wbcDomainSid *sid,
		    char **pdomain,
		    char **pname,
		    enum wbcSidType *pname_type)
{
	struct winbindd_request request;
	struct winbindd_response response;
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;
	char *sid_string = NULL;
	char *domain = NULL;
	char *name = NULL;
	enum wbcSidType name_type;

	if (!sid) {
		wbc_status = WBC_ERR_INVALID_PARAM;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	/* Initialize request */

	ZERO_STRUCT(request);
	ZERO_STRUCT(response);

	/* dst is already null terminated from the memset above */

	wbc_status = wbcSidToString(sid, &sid_string);
	BAIL_ON_WBC_ERROR(wbc_status);

	strncpy(request.data.sid, sid_string, sizeof(request.data.sid)-1);
	wbcFreeMemory(sid_string);

	/* Make request */

	wbc_status = wbcRequestResponse(WINBINDD_LOOKUPSID,
					   &request,
					   &response);
	BAIL_ON_WBC_ERROR(wbc_status);

	/* Copy out result */

	domain = talloc_strdup(NULL, response.data.name.dom_name);
	BAIL_ON_PTR_ERROR(domain, wbc_status);

	name = talloc_strdup(NULL, response.data.name.name);
	BAIL_ON_PTR_ERROR(name, wbc_status);

	name_type = (enum wbcSidType)response.data.name.type;

	wbc_status = WBC_ERR_SUCCESS;

 done:
	if (WBC_ERROR_IS_OK(wbc_status)) {
		if (pdomain != NULL) {
			*pdomain = domain;
		}
		if (pname != NULL) {
			*pname = name;
		}
		if (pname_type != NULL) {
			*pname_type = name_type;
		}
	}
	else {
		if (name != NULL) {
			talloc_free(name);
		}
		if (domain != NULL) {
			talloc_free(domain);
		}
	}

	return wbc_status;
}

/** @brief Translate a collection of RIDs within a domain to names
 *
 **/

wbcErr wbcLookupRids(struct wbcDomainSid *dom_sid,
		     int num_rids,
		     uint32_t *rids,
		     const char **pp_domain_name,
		     const char ***pnames,
		     enum wbcSidType **ptypes)
{
	size_t i, len, ridbuf_size;
	char *ridlist;
	char *p;
	struct winbindd_request request;
	struct winbindd_response response;
	char *sid_string = NULL;
	char *domain_name = NULL;
	const char **names = NULL;
	enum wbcSidType *types = NULL;
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;

	/* Initialise request */

	ZERO_STRUCT(request);
	ZERO_STRUCT(response);

	if (!dom_sid || (num_rids == 0)) {
		wbc_status = WBC_ERR_INVALID_PARAM;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	wbc_status = wbcSidToString(dom_sid, &sid_string);
	BAIL_ON_WBC_ERROR(wbc_status);

	strncpy(request.data.sid, sid_string, sizeof(request.data.sid)-1);
	wbcFreeMemory(sid_string);

	/* Even if all the Rids were of maximum 32bit values,
	   we would only have 11 bytes per rid in the final array
	   ("4294967296" + \n).  Add one more byte for the
	   terminating '\0' */

	ridbuf_size = (sizeof(char)*11) * num_rids + 1;

	ridlist = talloc_zero_array(NULL, char, ridbuf_size);
	BAIL_ON_PTR_ERROR(ridlist, wbc_status);

	len = 0;
	for (i=0; i<num_rids && (len-1)>0; i++) {
		char ridstr[12];

		len = strlen(ridlist);
		p = ridlist + len;

		snprintf( ridstr, sizeof(ridstr)-1, "%u\n", rids[i]);
		strncat(p, ridstr, ridbuf_size-len-1);
	}

	request.extra_data.data = ridlist;
	request.extra_len = strlen(ridlist)+1;

	wbc_status = wbcRequestResponse(WINBINDD_LOOKUPRIDS,
					&request,
					&response);
	talloc_free(ridlist);
	BAIL_ON_WBC_ERROR(wbc_status);

	domain_name = talloc_strdup(NULL, response.data.domain_name);
	BAIL_ON_PTR_ERROR(domain_name, wbc_status);

	names = talloc_array(NULL, const char*, num_rids);
	BAIL_ON_PTR_ERROR(names, wbc_status);

	types = talloc_array(NULL, enum wbcSidType, num_rids);
	BAIL_ON_PTR_ERROR(types, wbc_status);

	p = (char *)response.extra_data.data;

	for (i=0; i<num_rids; i++) {
		char *q;

		if (*p == '\0') {
			wbc_status = WBC_ERR_INVALID_RESPONSE;
			BAIL_ON_WBC_ERROR(wbc_status);
		}

		types[i] = (enum wbcSidType)strtoul(p, &q, 10);

		if (*q != ' ') {
			wbc_status = WBC_ERR_INVALID_RESPONSE;
			BAIL_ON_WBC_ERROR(wbc_status);
		}

		p = q+1;

		if ((q = strchr(p, '\n')) == NULL) {
			wbc_status = WBC_ERR_INVALID_RESPONSE;
			BAIL_ON_WBC_ERROR(wbc_status);
		}

		*q = '\0';

		names[i] = talloc_strdup(names, p);
		BAIL_ON_PTR_ERROR(names[i], wbc_status);

		p = q+1;
	}

	if (*p != '\0') {
		wbc_status = WBC_ERR_INVALID_RESPONSE;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	wbc_status = WBC_ERR_SUCCESS;

 done:
	if (response.extra_data.data) {
		free(response.extra_data.data);
	}

	if (WBC_ERROR_IS_OK(wbc_status)) {
		*pp_domain_name = domain_name;
		*pnames = names;
		*ptypes = types;
	}
	else {
		if (domain_name)
			talloc_free(domain_name);
		if (names)
			talloc_free(names);
		if (types)
			talloc_free(types);
	}

	return wbc_status;
}

/** @brief Get the groups a user belongs to
 *
 **/

wbcErr wbcLookupUserSids(const struct wbcDomainSid *user_sid,
			 bool domain_groups_only,
			 uint32_t *num_sids,
			 struct wbcDomainSid **_sids)
{
	uint32_t i;
	const char *s;
	struct winbindd_request request;
	struct winbindd_response response;
	char *sid_string = NULL;
	struct wbcDomainSid *sids = NULL;
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;
	int cmd;

	/* Initialise request */

	ZERO_STRUCT(request);
	ZERO_STRUCT(response);

	if (!user_sid) {
		wbc_status = WBC_ERR_INVALID_PARAM;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	wbc_status = wbcSidToString(user_sid, &sid_string);
	BAIL_ON_WBC_ERROR(wbc_status);

	strncpy(request.data.sid, sid_string, sizeof(request.data.sid)-1);
	wbcFreeMemory(sid_string);

	if (domain_groups_only) {
		cmd = WINBINDD_GETUSERDOMGROUPS;
	} else {
		cmd = WINBINDD_GETUSERSIDS;
	}

	wbc_status = wbcRequestResponse(cmd,
					&request,
					&response);
	BAIL_ON_WBC_ERROR(wbc_status);

	if (response.data.num_entries &&
	    !response.extra_data.data) {
		wbc_status = WBC_ERR_INVALID_RESPONSE;
		BAIL_ON_WBC_ERROR(wbc_status);
	}

	sids = talloc_array(NULL, struct wbcDomainSid,
			    response.data.num_entries);
	BAIL_ON_PTR_ERROR(sids, wbc_status);

	s = (const char *)response.extra_data.data;
	for (i = 0; i < response.data.num_entries; i++) {
		char *n = strchr(s, '\n');
		if (n) {
			*n = '\0';
		}
		wbc_status = wbcStringToSid(s, &sids[i]);
		BAIL_ON_WBC_ERROR(wbc_status);
		s += strlen(s) + 1;
	}

	*num_sids = response.data.num_entries;
	*_sids = sids;
	sids = NULL;
	wbc_status = WBC_ERR_SUCCESS;

 done:
	if (response.extra_data.data) {
		free(response.extra_data.data);
	}
	if (sids) {
		talloc_free(sids);
	}

	return wbc_status;
}

/** @brief Lists Users
 *
 **/

wbcErr wbcListUsers(const char *domain_name,
		    uint32_t *_num_users,
		    const char ***_users)
{
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;
	struct winbindd_request request;
	struct winbindd_response response;
	uint32_t num_users = 0;
	const char **users = NULL;
	const char *next;

	/* Initialise request */

	ZERO_STRUCT(request);
	ZERO_STRUCT(response);

	if (domain_name) {
		strncpy(request.domain_name, domain_name,
			sizeof(request.domain_name)-1);
	}

	wbc_status = wbcRequestResponse(WINBINDD_LIST_USERS,
					&request,
					&response);
	BAIL_ON_WBC_ERROR(wbc_status);

	/* Look through extra data */

	next = (const char *)response.extra_data.data;
	while (next) {
		const char **tmp;
		const char *current = next;
		char *k = strchr(next, ',');
		if (k) {
			k[0] = '\0';
			next = k+1;
		} else {
			next = NULL;
		}

		tmp = talloc_realloc(NULL, users,
				     const char *,
				     num_users+1);
		BAIL_ON_PTR_ERROR(tmp, wbc_status);
		users = tmp;

		users[num_users] = talloc_strdup(users, current);
		BAIL_ON_PTR_ERROR(users[num_users], wbc_status);

		num_users++;
	}

	*_num_users = num_users;
	*_users = users;
	users = NULL;
	wbc_status = WBC_ERR_SUCCESS;

 done:
	if (response.extra_data.data) {
		free(response.extra_data.data);
	}
	if (users) {
		talloc_free(users);
	}
	return wbc_status;
}

/** @brief Lists Groups
 *
 **/

wbcErr wbcListGroups(const char *domain_name,
		     uint32_t *_num_groups,
		     const char ***_groups)
{
	wbcErr wbc_status = WBC_ERR_UNKNOWN_FAILURE;
	struct winbindd_request request;
	struct winbindd_response response;
	uint32_t num_groups = 0;
	const char **groups = NULL;
	const char *next;

	/* Initialise request */

	ZERO_STRUCT(request);
	ZERO_STRUCT(response);

	if (domain_name) {
		strncpy(request.domain_name, domain_name,
			sizeof(request.domain_name)-1);
	}

	wbc_status = wbcRequestResponse(WINBINDD_LIST_GROUPS,
					&request,
					&response);
	BAIL_ON_WBC_ERROR(wbc_status);

	/* Look through extra data */

	next = (const char *)response.extra_data.data;
	while (next) {
		const char **tmp;
		const char *current = next;
		char *k = strchr(next, ',');
		if (k) {
			k[0] = '\0';
			next = k+1;
		} else {
			next = NULL;
		}

		tmp = talloc_realloc(NULL, groups,
				     const char *,
				     num_groups+1);
		BAIL_ON_PTR_ERROR(tmp, wbc_status);
		groups = tmp;

		groups[num_groups] = talloc_strdup(groups, current);
		BAIL_ON_PTR_ERROR(groups[num_groups], wbc_status);

		num_groups++;
	}

	*_num_groups = num_groups;
	*_groups = groups;
	groups = NULL;
	wbc_status = WBC_ERR_SUCCESS;

 done:
	if (response.extra_data.data) {
		free(response.extra_data.data);
	}
	if (groups) {
		talloc_free(groups);
	}
	return wbc_status;
}
