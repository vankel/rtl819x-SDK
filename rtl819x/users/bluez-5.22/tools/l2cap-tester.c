/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2013  Intel Corporation. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include <glib.h>

#include "lib/bluetooth.h"
#include "lib/l2cap.h"
#include "lib/mgmt.h"

#include "monitor/bt.h"
#include "emulator/bthost.h"

#include "src/shared/tester.h"
#include "src/shared/mgmt.h"
#include "src/shared/hciemu.h"

struct test_data {
	const void *test_data;
	struct mgmt *mgmt;
	uint16_t mgmt_index;
	struct hciemu *hciemu;
	enum hciemu_type hciemu_type;
	unsigned int io_id;
	uint16_t handle;
	uint16_t scid;
	uint16_t dcid;
};

struct l2cap_data {
	uint16_t client_psm;
	uint16_t server_psm;
	uint16_t cid;
	int expect_err;

	uint8_t send_cmd_code;
	const void *send_cmd;
	uint16_t send_cmd_len;
	uint8_t expect_cmd_code;
	const void *expect_cmd;
	uint16_t expect_cmd_len;

	uint16_t data_len;
	const void *read_data;
	const void *write_data;

	bool enable_ssp;
	int sec_level;
	bool reject_ssp;

	bool expect_pin;
	uint8_t pin_len;
	const void *pin;
	uint8_t client_pin_len;
	const void *client_pin;
};

static void mgmt_debug(const char *str, void *user_data)
{
	const char *prefix = user_data;

	tester_print("%s%s", prefix, str);
}

static void read_info_callback(uint8_t status, uint16_t length,
					const void *param, void *user_data)
{
	struct test_data *data = tester_get_data();
	const struct mgmt_rp_read_info *rp = param;
	char addr[18];
	uint16_t manufacturer;
	uint32_t supported_settings, current_settings;

	tester_print("Read Info callback");
	tester_print("  Status: 0x%02x", status);

	if (status || !param) {
		tester_pre_setup_failed();
		return;
	}

	ba2str(&rp->bdaddr, addr);
	manufacturer = btohs(rp->manufacturer);
	supported_settings = btohl(rp->supported_settings);
	current_settings = btohl(rp->current_settings);

	tester_print("  Address: %s", addr);
	tester_print("  Version: 0x%02x", rp->version);
	tester_print("  Manufacturer: 0x%04x", manufacturer);
	tester_print("  Supported settings: 0x%08x", supported_settings);
	tester_print("  Current settings: 0x%08x", current_settings);
	tester_print("  Class: 0x%02x%02x%02x",
			rp->dev_class[2], rp->dev_class[1], rp->dev_class[0]);
	tester_print("  Name: %s", rp->name);
	tester_print("  Short name: %s", rp->short_name);

	if (strcmp(hciemu_get_address(data->hciemu), addr)) {
		tester_pre_setup_failed();
		return;
	}

	tester_pre_setup_complete();
}

static void index_added_callback(uint16_t index, uint16_t length,
					const void *param, void *user_data)
{
	struct test_data *data = tester_get_data();

	tester_print("Index Added callback");
	tester_print("  Index: 0x%04x", index);

	data->mgmt_index = index;

	mgmt_send(data->mgmt, MGMT_OP_READ_INFO, data->mgmt_index, 0, NULL,
					read_info_callback, NULL, NULL);
}

static void index_removed_callback(uint16_t index, uint16_t length,
					const void *param, void *user_data)
{
	struct test_data *data = tester_get_data();

	tester_print("Index Removed callback");
	tester_print("  Index: 0x%04x", index);

	if (index != data->mgmt_index)
		return;

	mgmt_unregister_index(data->mgmt, data->mgmt_index);

	mgmt_unref(data->mgmt);
	data->mgmt = NULL;

	tester_post_teardown_complete();
}

static void read_index_list_callback(uint8_t status, uint16_t length,
					const void *param, void *user_data)
{
	struct test_data *data = tester_get_data();

	tester_print("Read Index List callback");
	tester_print("  Status: 0x%02x", status);

	if (status || !param) {
		tester_pre_setup_failed();
		return;
	}

	mgmt_register(data->mgmt, MGMT_EV_INDEX_ADDED, MGMT_INDEX_NONE,
					index_added_callback, NULL, NULL);

	mgmt_register(data->mgmt, MGMT_EV_INDEX_REMOVED, MGMT_INDEX_NONE,
					index_removed_callback, NULL, NULL);

	data->hciemu = hciemu_new(data->hciemu_type);
	if (!data->hciemu) {
		tester_warn("Failed to setup HCI emulation");
		tester_pre_setup_failed();
	}

	tester_print("New hciemu instance created");
}

static void test_pre_setup(const void *test_data)
{
	struct test_data *data = tester_get_data();

	data->mgmt = mgmt_new_default();
	if (!data->mgmt) {
		tester_warn("Failed to setup management interface");
		tester_pre_setup_failed();
		return;
	}

	if (tester_use_debug())
		mgmt_set_debug(data->mgmt, mgmt_debug, "mgmt: ", NULL);

	mgmt_send(data->mgmt, MGMT_OP_READ_INDEX_LIST, MGMT_INDEX_NONE, 0, NULL,
					read_index_list_callback, NULL, NULL);
}

static void test_post_teardown(const void *test_data)
{
	struct test_data *data = tester_get_data();

	if (data->io_id > 0) {
		g_source_remove(data->io_id);
		data->io_id = 0;
	}

	hciemu_unref(data->hciemu);
	data->hciemu = NULL;
}

static void test_data_free(void *test_data)
{
	struct test_data *data = test_data;

	free(data);
}

#define test_l2cap_bredr(name, data, setup, func) \
	do { \
		struct test_data *user; \
		user = malloc(sizeof(struct test_data)); \
		if (!user) \
			break; \
		user->hciemu_type = HCIEMU_TYPE_BREDR; \
		user->io_id = 0; \
		user->test_data = data; \
		tester_add_full(name, data, \
				test_pre_setup, setup, func, NULL, \
				test_post_teardown, 2, user, test_data_free); \
	} while (0)

#define test_l2cap_le(name, data, setup, func) \
	do { \
		struct test_data *user; \
		user = malloc(sizeof(struct test_data)); \
		if (!user) \
			break; \
		user->hciemu_type = HCIEMU_TYPE_LE; \
		user->io_id = 0; \
		user->test_data = data; \
		tester_add_full(name, data, \
				test_pre_setup, setup, func, NULL, \
				test_post_teardown, 2, user, test_data_free); \
	} while (0)

static uint8_t pair_device_pin[] = { 0x30, 0x30, 0x30, 0x30 }; /* "0000" */

static const struct l2cap_data client_connect_success_test = {
	.client_psm = 0x1001,
	.server_psm = 0x1001,
};

static const struct l2cap_data client_connect_ssp_success_test_1 = {
	.client_psm = 0x1001,
	.server_psm = 0x1001,
	.enable_ssp = true,
};

static const struct l2cap_data client_connect_ssp_success_test_2 = {
	.client_psm = 0x1001,
	.server_psm = 0x1001,
	.enable_ssp = true,
	.sec_level  = BT_SECURITY_HIGH,
};

static const struct l2cap_data client_connect_pin_success_test = {
	.client_psm = 0x1001,
	.server_psm = 0x1001,
	.sec_level  = BT_SECURITY_MEDIUM,
	.pin = pair_device_pin,
	.pin_len = sizeof(pair_device_pin),
	.client_pin = pair_device_pin,
	.client_pin_len = sizeof(pair_device_pin),
};

static uint8_t l2_data[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

static const struct l2cap_data client_connect_read_success_test = {
	.client_psm = 0x1001,
	.server_psm = 0x1001,
	.read_data = l2_data,
	.data_len = sizeof(l2_data),
};

static const struct l2cap_data client_connect_write_success_test = {
	.client_psm = 0x1001,
	.server_psm = 0x1001,
	.write_data = l2_data,
	.data_len = sizeof(l2_data),
};

static const struct l2cap_data client_connect_nval_psm_test_1 = {
	.client_psm = 0x1001,
	.expect_err = ECONNREFUSED,
};

static const struct l2cap_data client_connect_nval_psm_test_2 = {
	.client_psm = 0x0001,
	.expect_err = ECONNREFUSED,
};

static const struct l2cap_data client_connect_nval_psm_test_3 = {
	.client_psm = 0x0001,
	.expect_err = ECONNREFUSED,
	.enable_ssp = true,
};

static const uint8_t l2cap_connect_req[] = { 0x01, 0x10, 0x41, 0x00 };

static const struct l2cap_data l2cap_server_success_test = {
	.server_psm = 0x1001,
	.send_cmd_code = BT_L2CAP_PDU_CONN_REQ,
	.send_cmd = l2cap_connect_req,
	.send_cmd_len = sizeof(l2cap_connect_req),
	.expect_cmd_code = BT_L2CAP_PDU_CONN_RSP,
};

static const struct l2cap_data l2cap_server_read_success_test = {
	.server_psm = 0x1001,
	.send_cmd_code = BT_L2CAP_PDU_CONN_REQ,
	.send_cmd = l2cap_connect_req,
	.send_cmd_len = sizeof(l2cap_connect_req),
	.expect_cmd_code = BT_L2CAP_PDU_CONN_RSP,
	.read_data = l2_data,
	.data_len = sizeof(l2_data),
};

static const struct l2cap_data l2cap_server_write_success_test = {
	.server_psm = 0x1001,
	.send_cmd_code = BT_L2CAP_PDU_CONN_REQ,
	.send_cmd = l2cap_connect_req,
	.send_cmd_len = sizeof(l2cap_connect_req),
	.expect_cmd_code = BT_L2CAP_PDU_CONN_RSP,
	.write_data = l2_data,
	.data_len = sizeof(l2_data),
};

static const uint8_t l2cap_sec_block_rsp[] = {	0x00, 0x00,	/* dcid */
						0x41, 0x00,	/* scid */
						0x03, 0x00,	/* Sec Block */
						0x00, 0x00	/* status */
					};

static const struct l2cap_data l2cap_server_sec_block_test = {
	.server_psm = 0x1001,
	.send_cmd_code = BT_L2CAP_PDU_CONN_REQ,
	.send_cmd = l2cap_connect_req,
	.send_cmd_len = sizeof(l2cap_connect_req),
	.expect_cmd_code = BT_L2CAP_PDU_CONN_RSP,
	.expect_cmd = l2cap_sec_block_rsp,
	.expect_cmd_len = sizeof(l2cap_sec_block_rsp),
	.enable_ssp = true,
};

static const uint8_t l2cap_nval_psm_rsp[] = {	0x00, 0x00,	/* dcid */
						0x41, 0x00,	/* scid */
						0x02, 0x00,	/* nval PSM */
						0x00, 0x00	/* status */
					};

static const struct l2cap_data l2cap_server_nval_psm_test = {
	.send_cmd_code = BT_L2CAP_PDU_CONN_REQ,
	.send_cmd = l2cap_connect_req,
	.send_cmd_len = sizeof(l2cap_connect_req),
	.expect_cmd_code = BT_L2CAP_PDU_CONN_RSP,
	.expect_cmd = l2cap_nval_psm_rsp,
	.expect_cmd_len = sizeof(l2cap_nval_psm_rsp),
};

static const uint8_t l2cap_nval_conn_req[] = { 0x00 };
static const uint8_t l2cap_nval_pdu_rsp[] = { 0x00, 0x00 };

static const struct l2cap_data l2cap_server_nval_pdu_test1 = {
	.send_cmd_code = BT_L2CAP_PDU_CONN_REQ,
	.send_cmd = l2cap_nval_conn_req,
	.send_cmd_len = sizeof(l2cap_nval_conn_req),
	.expect_cmd_code = BT_L2CAP_PDU_CMD_REJECT,
	.expect_cmd = l2cap_nval_pdu_rsp,
	.expect_cmd_len = sizeof(l2cap_nval_pdu_rsp),
};

static const uint8_t l2cap_nval_dc_req[] = { 0x12, 0x34, 0x56, 0x78 };
static const uint8_t l2cap_nval_cid_rsp[] = { 0x02, 0x00,
						0x12, 0x34, 0x56, 0x78 };

static const struct l2cap_data l2cap_server_nval_cid_test1 = {
	.send_cmd_code = BT_L2CAP_PDU_DISCONN_REQ,
	.send_cmd = l2cap_nval_dc_req,
	.send_cmd_len = sizeof(l2cap_nval_dc_req),
	.expect_cmd_code = BT_L2CAP_PDU_CMD_REJECT,
	.expect_cmd = l2cap_nval_cid_rsp,
	.expect_cmd_len = sizeof(l2cap_nval_cid_rsp),
};

static const uint8_t l2cap_nval_cfg_req[] = { 0x12, 0x34, 0x00, 0x00 };
static const uint8_t l2cap_nval_cfg_rsp[] = { 0x02, 0x00,
						0x12, 0x34, 0x00, 0x00 };

static const struct l2cap_data l2cap_server_nval_cid_test2 = {
	.send_cmd_code = BT_L2CAP_PDU_CONFIG_REQ,
	.send_cmd = l2cap_nval_cfg_req,
	.send_cmd_len = sizeof(l2cap_nval_cfg_req),
	.expect_cmd_code = BT_L2CAP_PDU_CMD_REJECT,
	.expect_cmd = l2cap_nval_cfg_rsp,
	.expect_cmd_len = sizeof(l2cap_nval_cfg_rsp),
};

static const struct l2cap_data le_client_connect_success_test_1 = {
	.client_psm = 0x0080,
	.server_psm = 0x0080,
};

static const struct l2cap_data le_client_connect_success_test_2 = {
	.client_psm = 0x0080,
	.server_psm = 0x0080,
	.sec_level  = BT_SECURITY_MEDIUM,
};

static const uint8_t cmd_reject_rsp[] = { 0x01, 0x01, 0x02, 0x00, 0x00, 0x00 };

static const struct l2cap_data le_client_connect_reject_test_1 = {
	.client_psm = 0x0080,
	.send_cmd = cmd_reject_rsp,
	.send_cmd_len = sizeof(cmd_reject_rsp),
	.expect_err = ECONNREFUSED,
};

static const struct l2cap_data le_client_connect_nval_psm_test = {
	.client_psm = 0x0080,
	.expect_err = ECONNREFUSED,
};

static const uint8_t le_connect_req[] = {	0x80, 0x00, /* PSM */
						0x41, 0x00, /* SCID */
						0x20, 0x00, /* MTU */
						0x20, 0x00, /* MPS */
						0x05, 0x00, /* Credits */
};

static const struct l2cap_data le_server_success_test = {
	.server_psm = 0x0080,
	.send_cmd_code = BT_L2CAP_PDU_LE_CONN_REQ,
	.send_cmd = le_connect_req,
	.send_cmd_len = sizeof(le_connect_req),
	.expect_cmd_code = BT_L2CAP_PDU_LE_CONN_RSP,
};

static const struct l2cap_data le_att_client_connect_success_test_1 = {
	.cid = 0x0004,
	.sec_level = BT_SECURITY_LOW,
};

static const struct l2cap_data le_att_server_success_test_1 = {
	.cid = 0x0004,
};

static void client_cmd_complete(uint16_t opcode, uint8_t status,
					const void *param, uint8_t len,
					void *user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *test = data->test_data;
	struct bthost *bthost;

	bthost = hciemu_client_get_host(data->hciemu);

	switch (opcode) {
	case BT_HCI_CMD_WRITE_SCAN_ENABLE:
	case BT_HCI_CMD_LE_SET_ADV_ENABLE:
		tester_print("Client set connectable status 0x%02x", status);
		if (!status && test && test->enable_ssp) {
			bthost_write_ssp_mode(bthost, 0x01);
			return;
		}
		break;
	case BT_HCI_CMD_WRITE_SIMPLE_PAIRING_MODE:
		tester_print("Client enable SSP status 0x%02x", status);
		break;
	default:
		return;
	}


	if (status)
		tester_setup_failed();
	else
		tester_setup_complete();
}

static void server_cmd_complete(uint16_t opcode, uint8_t status,
					const void *param, uint8_t len,
					void *user_data)
{
	switch (opcode) {
	case BT_HCI_CMD_WRITE_SIMPLE_PAIRING_MODE:
		tester_print("Server enable SSP status 0x%02x", status);
		break;
	default:
		return;
	}

	if (status)
		tester_setup_failed();
	else
		tester_setup_complete();
}

static void setup_powered_client_callback(uint8_t status, uint16_t length,
					const void *param, void *user_data)
{
	struct test_data *data = tester_get_data();
	struct bthost *bthost;

	if (status != MGMT_STATUS_SUCCESS) {
		tester_setup_failed();
		return;
	}

	tester_print("Controller powered on");

	bthost = hciemu_client_get_host(data->hciemu);
	bthost_set_cmd_complete_cb(bthost, client_cmd_complete, user_data);
	if (data->hciemu_type == HCIEMU_TYPE_LE)
		bthost_set_adv_enable(bthost, 0x01, 0x00);
	else
		bthost_write_scan_enable(bthost, 0x03);
}

static void setup_powered_server_callback(uint8_t status, uint16_t length,
					const void *param, void *user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *test = data->test_data;
	struct bthost *bthost;

	if (status != MGMT_STATUS_SUCCESS) {
		tester_setup_failed();
		return;
	}

	tester_print("Controller powered on");

	if (!test->enable_ssp) {
		tester_setup_complete();
		return;
	}

	bthost = hciemu_client_get_host(data->hciemu);
	bthost_set_cmd_complete_cb(bthost, server_cmd_complete, user_data);
	bthost_write_ssp_mode(bthost, 0x01);
}

static void user_confirm_request_callback(uint16_t index, uint16_t length,
							const void *param,
							void *user_data)
{
	const struct mgmt_ev_user_confirm_request *ev = param;
	struct test_data *data = tester_get_data();
	const struct l2cap_data *test = data->test_data;
	struct mgmt_cp_user_confirm_reply cp;
	uint16_t opcode;

	memset(&cp, 0, sizeof(cp));
	memcpy(&cp.addr, &ev->addr, sizeof(cp.addr));

	if (test->reject_ssp)
		opcode = MGMT_OP_USER_CONFIRM_NEG_REPLY;
	else
		opcode = MGMT_OP_USER_CONFIRM_REPLY;

	mgmt_reply(data->mgmt, opcode, data->mgmt_index, sizeof(cp), &cp,
							NULL, NULL, NULL);
}

static void pin_code_request_callback(uint16_t index, uint16_t length,
					const void *param, void *user_data)
{
	const struct mgmt_ev_pin_code_request *ev = param;
	struct test_data *data = user_data;
	const struct l2cap_data *test = data->test_data;
	struct mgmt_cp_pin_code_reply cp;

	memset(&cp, 0, sizeof(cp));
	memcpy(&cp.addr, &ev->addr, sizeof(cp.addr));

	if (!test->pin) {
		mgmt_reply(data->mgmt, MGMT_OP_PIN_CODE_NEG_REPLY,
				data->mgmt_index, sizeof(cp.addr), &cp.addr,
				NULL, NULL, NULL);
		return;
	}

	cp.pin_len = test->pin_len;
	memcpy(cp.pin_code, test->pin, test->pin_len);

	mgmt_reply(data->mgmt, MGMT_OP_PIN_CODE_REPLY, data->mgmt_index,
			sizeof(cp), &cp, NULL, NULL, NULL);
}

static void bthost_send_rsp(const void *buf, uint16_t len, void *user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;
	struct bthost *bthost;

	if (l2data->expect_cmd_len && len != l2data->expect_cmd_len) {
		tester_test_failed();
		return;
	}

	if (l2data->expect_cmd && memcmp(buf, l2data->expect_cmd,
						l2data->expect_cmd_len)) {
		tester_test_failed();
		return;
	}

	if (!l2data->send_cmd)
		return;

	bthost = hciemu_client_get_host(data->hciemu);
	bthost_send_cid(bthost, data->handle, data->dcid,
				l2data->send_cmd, l2data->send_cmd_len);
}

static void send_rsp_new_conn(uint16_t handle, void *user_data)
{
	struct test_data *data = user_data;
	struct bthost *bthost;

	tester_print("New connection with handle 0x%04x", handle);

	data->handle = handle;

	if (data->hciemu_type == HCIEMU_TYPE_LE)
		data->dcid = 0x0005;
	else
		data->dcid = 0x0001;

	bthost = hciemu_client_get_host(data->hciemu);
	bthost_add_cid_hook(bthost, data->handle, data->dcid,
						bthost_send_rsp, NULL);
}

static void setup_powered_common(void)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *test = data->test_data;
	struct bthost *bthost = hciemu_client_get_host(data->hciemu);
	unsigned char param[] = { 0x01 };

	mgmt_register(data->mgmt, MGMT_EV_USER_CONFIRM_REQUEST,
			data->mgmt_index, user_confirm_request_callback,
			NULL, NULL);

	if (test && (test->pin || test->expect_pin))
		mgmt_register(data->mgmt, MGMT_EV_PIN_CODE_REQUEST,
				data->mgmt_index, pin_code_request_callback,
				data, NULL);

	if (test && test->client_pin)
		bthost_set_pin_code(bthost, test->client_pin,
							test->client_pin_len);
	if (test && test->reject_ssp)
		bthost_set_reject_user_confirm(bthost, true);

	if (data->hciemu_type == HCIEMU_TYPE_LE)
		mgmt_send(data->mgmt, MGMT_OP_SET_LE, data->mgmt_index,
				sizeof(param), param, NULL, NULL, NULL);

	if (test && test->enable_ssp)
		mgmt_send(data->mgmt, MGMT_OP_SET_SSP, data->mgmt_index,
				sizeof(param), param, NULL, NULL, NULL);

	mgmt_send(data->mgmt, MGMT_OP_SET_BONDABLE, data->mgmt_index,
				sizeof(param), param, NULL, NULL, NULL);
}

static void setup_powered_client(const void *test_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *test = data->test_data;
	unsigned char param[] = { 0x01 };

	setup_powered_common();

	tester_print("Powering on controller");

	if (test && (test->expect_cmd || test->send_cmd)) {
		struct bthost *bthost = hciemu_client_get_host(data->hciemu);
		bthost_set_connect_cb(bthost, send_rsp_new_conn, data);
	}

	mgmt_send(data->mgmt, MGMT_OP_SET_POWERED, data->mgmt_index,
			sizeof(param), param, setup_powered_client_callback,
			NULL, NULL);
}

static void setup_powered_server(const void *test_data)
{
	struct test_data *data = tester_get_data();
	unsigned char param[] = { 0x01 };

	setup_powered_common();

	tester_print("Powering on controller");

	mgmt_send(data->mgmt, MGMT_OP_SET_CONNECTABLE, data->mgmt_index,
				sizeof(param), param, NULL, NULL, NULL);

	if (data->hciemu_type != HCIEMU_TYPE_BREDR)
		mgmt_send(data->mgmt, MGMT_OP_SET_ADVERTISING,
				data->mgmt_index, sizeof(param), param, NULL,
				NULL, NULL);

	mgmt_send(data->mgmt, MGMT_OP_SET_POWERED, data->mgmt_index,
			sizeof(param), param, setup_powered_server_callback,
			NULL, NULL);
}

static void test_basic(const void *test_data)
{
	int sk;

	sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (sk < 0) {
		tester_warn("Can't create socket: %s (%d)", strerror(errno),
									errno);
		tester_test_failed();
		return;
	}

	close(sk);

	tester_test_passed();
}

static gboolean client_received_data(GIOChannel *io, GIOCondition cond,
							gpointer user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;
	char buf[1024];
	int sk;

	sk = g_io_channel_unix_get_fd(io);
	if (read(sk, buf, l2data->data_len) != l2data->data_len) {
		tester_warn("Unable to read %u bytes", l2data->data_len);
		tester_test_failed();
		return FALSE;
	}

	if (memcmp(buf, l2data->read_data, l2data->data_len))
		tester_test_failed();
	else
		tester_test_passed();

	return FALSE;
}

static gboolean server_received_data(GIOChannel *io, GIOCondition cond,
							gpointer user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;
	char buf[1024];
	int sk;

	sk = g_io_channel_unix_get_fd(io);
	if (read(sk, buf, l2data->data_len) != l2data->data_len) {
		tester_warn("Unable to read %u bytes", l2data->data_len);
		tester_test_failed();
		return FALSE;
	}

	if (memcmp(buf, l2data->read_data, l2data->data_len))
		tester_test_failed();
	else
		tester_test_passed();

	return FALSE;
}

static void bthost_received_data(const void *buf, uint16_t len,
							void *user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;

	if (len != l2data->data_len) {
		tester_test_failed();
		return;
	}

	if (memcmp(buf, l2data->write_data, l2data->data_len))
		tester_test_failed();
	else
		tester_test_passed();
}

static void server_bthost_received_data(const void *buf, uint16_t len,
							void *user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;

	if (len != l2data->data_len) {
		tester_test_failed();
		return;
	}

	if (memcmp(buf, l2data->write_data, l2data->data_len))
		tester_test_failed();
	else
		tester_test_passed();
}

static bool check_mtu(struct test_data *data, int sk)
{
	const struct l2cap_data *l2data = data->test_data;
	struct l2cap_options l2o;
	socklen_t len;

	memset(&l2o, 0, sizeof(l2o));

	if (data->hciemu_type == HCIEMU_TYPE_LE &&
				(l2data->client_psm || l2data->server_psm)) {
		/* LE CoC enabled kernels should support BT_RCVMTU and
		 * BT_SNDMTU.
		 */
		len = sizeof(l2o.imtu);
		if (getsockopt(sk, SOL_BLUETOOTH, BT_RCVMTU,
							&l2o.imtu, &len) < 0) {
			tester_warn("getsockopt(BT_RCVMTU): %s (%d)",
					strerror(errno), errno);
			return false;
		}

		len = sizeof(l2o.omtu);
		if (getsockopt(sk, SOL_BLUETOOTH, BT_SNDMTU,
							&l2o.omtu, &len) < 0) {
			tester_warn("getsockopt(BT_SNDMTU): %s (%d)",
					strerror(errno), errno);
			return false;
		}
	} else {
		/* For non-LE CoC enabled kernels we need to fall back to
		 * L2CAP_OPTIONS, so test support for it as well */
		len = sizeof(l2o);
		if (getsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &l2o, &len) < 0) {
			 tester_warn("getsockopt(L2CAP_OPTIONS): %s (%d)",
						strerror(errno), errno);
			 return false;
		 }
	}

	return true;
}

static gboolean l2cap_connect_cb(GIOChannel *io, GIOCondition cond,
							gpointer user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;
	int err, sk_err, sk;
	socklen_t len = sizeof(sk_err);

	data->io_id = 0;

	sk = g_io_channel_unix_get_fd(io);

	if (getsockopt(sk, SOL_SOCKET, SO_ERROR, &sk_err, &len) < 0)
		err = -errno;
	else
		err = -sk_err;

	if (err < 0) {
		tester_warn("Connect failed: %s (%d)", strerror(-err), -err);
		goto failed;
	}

	tester_print("Successfully connected");

	if (!check_mtu(data, sk)) {
		tester_test_failed();
		return FALSE;
	}

	if (l2data->read_data) {
		struct bthost *bthost;

		bthost = hciemu_client_get_host(data->hciemu);
		g_io_add_watch(io, G_IO_IN, client_received_data, NULL);

		bthost_send_cid(bthost, data->handle, data->dcid,
					l2data->read_data, l2data->data_len);

		return FALSE;
	} else if (l2data->write_data) {
		struct bthost *bthost;
		ssize_t ret;

		bthost = hciemu_client_get_host(data->hciemu);
		bthost_add_cid_hook(bthost, data->handle, data->dcid,
					bthost_received_data, NULL);

		ret = write(sk, l2data->write_data, l2data->data_len);
		if (ret != l2data->data_len) {
			tester_warn("Unable to write all data");
			tester_test_failed();
		}

		return FALSE;
	}

failed:
	if (-err != l2data->expect_err)
		tester_test_failed();
	else
		tester_test_passed();

	return FALSE;
}

static int create_l2cap_sock(struct test_data *data, uint16_t psm,
						uint16_t cid, int sec_level)
{
	const uint8_t *master_bdaddr;
	struct sockaddr_l2 addr;
	int sk, err;

	sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET | SOCK_NONBLOCK,
							BTPROTO_L2CAP);
	if (sk < 0) {
		err = -errno;
		tester_warn("Can't create socket: %s (%d)", strerror(errno),
									errno);
		return err;
	}

	master_bdaddr = hciemu_get_master_bdaddr(data->hciemu);
	if (!master_bdaddr) {
		tester_warn("No master bdaddr");
		close(sk);
		return -ENODEV;
	}

	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	addr.l2_psm = htobs(psm);
	addr.l2_cid = htobs(cid);
	bacpy(&addr.l2_bdaddr, (void *) master_bdaddr);
	if (data->hciemu_type == HCIEMU_TYPE_LE)
		addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
	else
		addr.l2_bdaddr_type = BDADDR_BREDR;

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		err = -errno;
		tester_warn("Can't bind socket: %s (%d)", strerror(errno),
									errno);
		close(sk);
		return err;
	}

	if (sec_level) {
		struct bt_security sec;

		memset(&sec, 0, sizeof(sec));
		sec.level = sec_level;

		if (setsockopt(sk, SOL_BLUETOOTH, BT_SECURITY, &sec,
							sizeof(sec)) < 0) {
			err = -errno;
			tester_warn("Can't set security level: %s (%d)",
						strerror(errno), errno);
			close(sk);
			return err;
		}
	}

	return sk;
}

static int connect_l2cap_sock(struct test_data *data, int sk, uint16_t psm,
								uint16_t cid)
{
	const uint8_t *client_bdaddr;
	struct sockaddr_l2 addr;
	int err;

	client_bdaddr = hciemu_get_client_bdaddr(data->hciemu);
	if (!client_bdaddr) {
		tester_warn("No client bdaddr");
		return -ENODEV;
	}

	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, (void *) client_bdaddr);
	addr.l2_psm = htobs(psm);
	addr.l2_cid = htobs(cid);
	if (data->hciemu_type == HCIEMU_TYPE_LE)
		addr.l2_bdaddr_type = BDADDR_LE_PUBLIC;
	else
		addr.l2_bdaddr_type = BDADDR_BREDR;

	err = connect(sk, (struct sockaddr *) &addr, sizeof(addr));
	if (err < 0 && !(errno == EAGAIN || errno == EINPROGRESS)) {
		err = -errno;
		tester_warn("Can't connect socket: %s (%d)", strerror(errno),
									errno);
		return err;
	}

	return 0;
}

static void client_l2cap_connect_cb(uint16_t handle, uint16_t cid,
							void *user_data)
{
	struct test_data *data = user_data;

	data->dcid = cid;
	data->handle = handle;
}

static void test_connect(const void *test_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;
	GIOChannel *io;
	int sk;

	if (l2data->server_psm) {
		struct bthost *bthost = hciemu_client_get_host(data->hciemu);

		if (!l2data->data_len)
			bthost_add_l2cap_server(bthost, l2data->server_psm,
						NULL, NULL);
		else
			bthost_add_l2cap_server(bthost, l2data->server_psm,
						client_l2cap_connect_cb, data);
	}

	sk = create_l2cap_sock(data, 0, l2data->cid, l2data->sec_level);
	if (sk < 0) {
		tester_test_failed();
		return;
	}

	if (connect_l2cap_sock(data, sk, l2data->client_psm,
							l2data->cid) < 0) {
		close(sk);
		tester_test_failed();
		return;
	}

	io = g_io_channel_unix_new(sk);
	g_io_channel_set_close_on_unref(io, TRUE);

	data->io_id = g_io_add_watch(io, G_IO_OUT, l2cap_connect_cb, NULL);

	g_io_channel_unref(io);

	tester_print("Connect in progress");
}

static gboolean l2cap_listen_cb(GIOChannel *io, GIOCondition cond,
							gpointer user_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;
	int sk, new_sk;

	data->io_id = 0;

	sk = g_io_channel_unix_get_fd(io);

	new_sk = accept(sk, NULL, NULL);
	if (new_sk < 0) {
		tester_warn("accept failed: %s (%u)", strerror(errno), errno);
		tester_test_failed();
		return FALSE;
	}

	if (!check_mtu(data, new_sk)) {
		tester_test_failed();
		return FALSE;
	}

	if (l2data->read_data) {
		struct bthost *bthost;
		GIOChannel *new_io;

		new_io = g_io_channel_unix_new(new_sk);
		g_io_channel_set_close_on_unref(new_io, TRUE);

		bthost = hciemu_client_get_host(data->hciemu);
		g_io_add_watch(new_io, G_IO_IN, server_received_data, NULL);
		bthost_send_cid(bthost, data->handle, data->dcid,
					l2data->read_data, l2data->data_len);

		g_io_channel_unref(new_io);

		return FALSE;
	} else if (l2data->write_data) {
		struct bthost *bthost;
		ssize_t ret;

		bthost = hciemu_client_get_host(data->hciemu);
		bthost_add_cid_hook(bthost, data->handle, data->scid,
					server_bthost_received_data, NULL);

		ret = write(new_sk, l2data->write_data, l2data->data_len);
		close(new_sk);

		if (ret != l2data->data_len) {
			tester_warn("Unable to write all data");
			tester_test_failed();
		}

		return FALSE;
	}

	tester_print("Successfully connected");

	close(new_sk);

	tester_test_passed();

	return FALSE;
}

static void client_l2cap_rsp(uint8_t code, const void *data, uint16_t len,
							void *user_data)
{
	struct test_data *test_data = user_data;
	const struct l2cap_data *l2data = test_data->test_data;

	tester_print("Client received response code 0x%02x", code);

	if (code != l2data->expect_cmd_code) {
		tester_warn("Unexpected L2CAP response code (expected 0x%02x)",
						l2data->expect_cmd_code);
		goto failed;
	}

	if (code == BT_L2CAP_PDU_CONN_RSP) {

		const struct bt_l2cap_pdu_conn_rsp *rsp = data;
		if (len == sizeof(rsp) && !rsp->result && !rsp->status)
			return;

		test_data->dcid = rsp->dcid;
		test_data->scid = rsp->scid;

		if (l2data->data_len)
			return;
	}

	if (!l2data->expect_cmd) {
		tester_test_passed();
		return;
	}

	if (l2data->expect_cmd_len != len) {
		tester_warn("Unexpected L2CAP response length (%u != %u)",
						len, l2data->expect_cmd_len);
		goto failed;
	}

	if (memcmp(l2data->expect_cmd, data, len) != 0) {
		tester_warn("Unexpected L2CAP response");
		goto failed;
	}

	tester_test_passed();
	return;

failed:
	tester_test_failed();
}

static void send_req_new_conn(uint16_t handle, void *user_data)
{
	struct test_data *data = user_data;
	const struct l2cap_data *l2data = data->test_data;
	struct bthost *bthost;

	tester_print("New client connection with handle 0x%04x", handle);

	data->handle = handle;

	if (l2data->send_cmd) {
		bthost_l2cap_rsp_cb cb;

		if (l2data->expect_cmd_code)
			cb = client_l2cap_rsp;
		else
			cb = NULL;

		tester_print("Sending L2CAP Request from client");

		bthost = hciemu_client_get_host(data->hciemu);
		bthost_l2cap_req(bthost, handle, l2data->send_cmd_code,
					l2data->send_cmd, l2data->send_cmd_len,
					cb, data);
	}
}

static void test_server(const void *test_data)
{
	struct test_data *data = tester_get_data();
	const struct l2cap_data *l2data = data->test_data;
	const uint8_t *master_bdaddr;
	uint8_t addr_type;
	struct bthost *bthost;
	GIOChannel *io;
	int sk;

	if (l2data->server_psm || l2data->cid) {
		sk = create_l2cap_sock(data, l2data->server_psm,
					l2data->cid, l2data->sec_level);
		if (sk < 0) {
			tester_test_failed();
			return;
		}

		if (listen(sk, 5) < 0) {
			tester_warn("listening on socket failed: %s (%u)",
					strerror(errno), errno);
			tester_test_failed();
			close(sk);
			return;
		}

		io = g_io_channel_unix_new(sk);
		g_io_channel_set_close_on_unref(io, TRUE);

		data->io_id = g_io_add_watch(io, G_IO_IN, l2cap_listen_cb,
									NULL);
		g_io_channel_unref(io);

		tester_print("Listening for connections");
	}

	master_bdaddr = hciemu_get_master_bdaddr(data->hciemu);
	if (!master_bdaddr) {
		tester_warn("No master bdaddr");
		tester_test_failed();
		return;
	}

	bthost = hciemu_client_get_host(data->hciemu);
	bthost_set_connect_cb(bthost, send_req_new_conn, data);

	if (data->hciemu_type == HCIEMU_TYPE_BREDR)
		addr_type = BDADDR_BREDR;
	else
		addr_type = BDADDR_LE_PUBLIC;

	bthost_hci_connect(bthost, master_bdaddr, addr_type);
}

static void test_getpeername_not_connected(const void *test_data)
{
	struct test_data *data = tester_get_data();
	struct sockaddr_l2 addr;
	socklen_t len;
	int sk;

	sk = create_l2cap_sock(data, 0, 0, 0);
	if (sk < 0) {
		tester_test_failed();
		return;
	}

	len = sizeof(addr);
	if (getpeername(sk, (struct sockaddr *) &addr, &len) == 0) {
		tester_warn("getpeername succeeded on non-connected socket");
		tester_test_failed();
		goto done;
	}

	if (errno != ENOTCONN) {
		tester_warn("Unexpexted getpeername error: %s (%d)",
						strerror(errno), errno);
		tester_test_failed();
		goto done;
	}

	tester_test_passed();

done:
	close(sk);
}

int main(int argc, char *argv[])
{
	tester_init(&argc, &argv);

	test_l2cap_bredr("Basic L2CAP Socket - Success", NULL,
					setup_powered_client, test_basic);
	test_l2cap_bredr("Non-connected getpeername - Failure", NULL,
					setup_powered_client,
					test_getpeername_not_connected);

	test_l2cap_bredr("L2CAP BR/EDR Client - Success",
					&client_connect_success_test,
					setup_powered_client, test_connect);

	test_l2cap_bredr("L2CAP BR/EDR Client SSP - Success 1",
					&client_connect_ssp_success_test_1,
					setup_powered_client, test_connect);
	test_l2cap_bredr("L2CAP BR/EDR Client SSP - Success 2",
					&client_connect_ssp_success_test_2,
					setup_powered_client, test_connect);
	test_l2cap_bredr("L2CAP BR/EDR Client PIN Code - Success",
					&client_connect_pin_success_test,
					setup_powered_client, test_connect);

	test_l2cap_bredr("L2CAP BR/EDR Client - Read Success",
					&client_connect_read_success_test,
					setup_powered_client, test_connect);

	test_l2cap_bredr("L2CAP BR/EDR Client - Write Success",
					&client_connect_write_success_test,
					setup_powered_client, test_connect);

	test_l2cap_bredr("L2CAP BR/EDR Client - Invalid PSM 1",
					&client_connect_nval_psm_test_1,
					setup_powered_client, test_connect);

	test_l2cap_bredr("L2CAP BR/EDR Client - Invalid PSM 2",
					&client_connect_nval_psm_test_2,
					setup_powered_client, test_connect);

	test_l2cap_bredr("L2CAP BR/EDR Client - Invalid PSM 3",
					&client_connect_nval_psm_test_3,
					setup_powered_client, test_connect);

	test_l2cap_bredr("L2CAP BR/EDR Server - Success",
					&l2cap_server_success_test,
					setup_powered_server, test_server);

	test_l2cap_bredr("L2CAP BR/EDR Server - Read Success",
					&l2cap_server_read_success_test,
					setup_powered_server, test_server);

	test_l2cap_bredr("L2CAP BR/EDR Server - Write Success",
					&l2cap_server_write_success_test,
					setup_powered_server, test_server);

	test_l2cap_bredr("L2CAP BR/EDR Server - Security Block",
					&l2cap_server_sec_block_test,
					setup_powered_server, test_server);

	test_l2cap_bredr("L2CAP BR/EDR Server - Invalid PSM",
					&l2cap_server_nval_psm_test,
					setup_powered_server, test_server);
	test_l2cap_bredr("L2CAP BR/EDR Server - Invalid PDU",
				&l2cap_server_nval_pdu_test1,
				setup_powered_server, test_server);
	test_l2cap_bredr("L2CAP BR/EDR Server - Invalid Disconnect CID",
				&l2cap_server_nval_cid_test1,
				setup_powered_server, test_server);
	test_l2cap_bredr("L2CAP BR/EDR Server - Invalid Config CID",
				&l2cap_server_nval_cid_test2,
				setup_powered_server, test_server);

	test_l2cap_le("L2CAP LE Client - Success",
				&le_client_connect_success_test_1,
				setup_powered_client, test_connect);
	test_l2cap_le("L2CAP LE Client SMP - Success",
				&le_client_connect_success_test_2,
				setup_powered_client, test_connect);
	test_l2cap_le("L2CAP LE Client - Command Reject",
					&le_client_connect_reject_test_1,
					setup_powered_client, test_connect);
	test_l2cap_le("L2CAP LE Client - Invalid PSM",
					&le_client_connect_nval_psm_test,
					setup_powered_client, test_connect);
	test_l2cap_le("L2CAP LE Server - Success", &le_server_success_test,
					setup_powered_server, test_server);


	test_l2cap_le("L2CAP LE ATT Client - Success",
				&le_att_client_connect_success_test_1,
				setup_powered_client, test_connect);
	test_l2cap_le("L2CAP LE ATT Server - Success",
				&le_att_server_success_test_1,
				setup_powered_server, test_server);

	return tester_run();
}
