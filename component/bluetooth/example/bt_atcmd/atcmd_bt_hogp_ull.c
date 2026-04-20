/*
*******************************************************************************
* Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <osif.h>

#include "platform_autoconf.h"
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <atcmd_bt_impl.h>

#include "../gatt_service/server/rtk_service_config.h"
#include "../gatt_service/server/rtk_hids_iso.h"
#include "../gatt_service/server/rtk_hids_gamepad.h"

#include "../gatt_service/server/rtk_scan_param_srv.h"
#include "../gatt_service/server/rtk_bas.h"
#include "../gatt_service/client/rtk_hids_iso_client.h"
#include "../gatt_service/client/rtk_bas_client.h"
#include "../gatt_service/client/rtk_hids_gp_client.h"
#include "../gatt_service/client/rtk_scan_param_client.h"

/* ============================================================================
 *                           External Function Declarations
 * ============================================================================ */

/* Report Host functions */
extern uint16_t hogp_ull_report_host_connect(rtk_bt_le_addr_t *peer_addr);
extern uint16_t hogp_ull_report_host_disconnect(void);
extern uint16_t hogp_ull_report_host_switch_hybrid_mode(uint8_t cig_id, uint8_t cis_id,
														uint8_t input_conf, uint8_t input_rep);
extern uint16_t hogp_ull_report_host_switch_default_mode(void);

/* Report Host context type and getter (defined in report_host_main.c) */
typedef struct {
	uint8_t conn_id;
	uint16_t conn_handle;
	uint8_t current_mode;
	uint8_t cig_id;
	uint8_t cis_id;
	uint16_t cis_conn_handle;
} report_host_context_t;

extern report_host_context_t *hogp_ull_report_host_get_ctx(void);

/* HID Device functions */
extern uint16_t hogp_ull_device_switch_hybrid_mode(uint16_t conn_handle,
												   uint8_t cig_id, uint8_t cis_id,
												   uint8_t input_conf, uint8_t input_rep);

/* ============================================================================
 *                           Type Definitions
 * ============================================================================ */

/* Note: gamepad_input_report_t is defined in rtk_hids_gp_client.h */

/* ============================================================================
 *                           HID Device Subcommands
 * ============================================================================ */

/**
 * @brief Send gamepad input report
 * @param argc Argument count (should be 7: conn_handle, buttons, x, y, z, rz, hat)
 * @param argv Argument values
 * @return 0 on success, -1 on failure
 */
static int atcmd_bt_hogp_ull_send_gamepad(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	gamepad_input_report_t report = {0};

	/* Parse connection handle (first parameter) */
	conn_handle = (uint16_t)str_to_int(argv[0]);

	/* Parse gamepad report data */
	report.buttons = (uint16_t)str_to_int(argv[1]);
	report.x = (int8_t)str_to_int(argv[2]);
	report.y = (int8_t)str_to_int(argv[3]);
	report.z = (int8_t)str_to_int(argv[4]);
	report.rz = (int8_t)str_to_int(argv[5]);
	report.hat_switch = (uint8_t)str_to_int(argv[6]);

	if (gamepad_hid_srv_send_input_report(conn_handle, &report)) {
		BT_LOGE("HOGP ULL send gamepad report fail (conn_handle=0x%04x)\r\n", conn_handle);
		return -1;
	}

	BT_LOGA("HOGP ULL gamepad report sent: conn_handle=0x%04x, buttons=0x%04x, x=%d, y=%d, z=%d, rz=%d, hat=%d\r\n",
			conn_handle, report.buttons, report.x, report.y, report.z, report.rz, report.hat_switch);
	return 0;
}

/** @brief External declaration for device send report API (auto-selects ISO/GATT path) */
extern uint16_t hogp_ull_device_send_report(uint16_t conn_handle, uint8_t report_id,
											const uint8_t *data, uint16_t len);

/**
 * @brief Send HID report via device send report API (auto-selects ISO/GATT path)
 * @note  Differs from hids_gp_send_gamepad which only sends via GATT notification.
 *        This command routes through hogp_ull_device_send_report() which automatically
 *        selects ISO path in Hybrid Mode or GATT path in Default Mode.
 */
static int atcmd_bt_hogp_ull_device_send_report(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint8_t report_id;
	gamepad_input_report_t report = {0};
	uint16_t ret;

	/* Parse parameters */
	conn_handle = (uint16_t)str_to_int(argv[0]);
	report_id = (uint8_t)str_to_int(argv[1]);
	report.buttons = (uint16_t)str_to_int(argv[2]);
	report.x = (int8_t)str_to_int(argv[3]);
	report.y = (int8_t)str_to_int(argv[4]);
	report.z = (int8_t)str_to_int(argv[5]);
	report.rz = (int8_t)str_to_int(argv[6]);
	report.hat_switch = (uint8_t)str_to_int(argv[7]);

	ret = hogp_ull_device_send_report(conn_handle, report_id,
									  (const uint8_t *)&report, sizeof(report));
	if (ret != RTK_BT_OK) {
		BT_LOGE("HOGP ULL device send report fail: conn_handle=0x%04x, report_id=%d, err=0x%x\r\n",
				conn_handle, report_id, ret);
		return -1;
	}

	BT_LOGA("HOGP ULL device report sent: conn_handle=0x%04x, report_id=%d, buttons=0x%04x, x=0x%x, y=0x%x, z=0x%x, rz=0x%x, hat=0x%x, reserved=0x%x\r\n",
			conn_handle, report_id, report.buttons, report.x, report.y, report.z, report.rz, report.hat_switch, report.reserved);
	return 0;
}

/**
 * @brief Send HID report with Report ID = 0 (No Report ID) via ISO/GATT
 * @note  Fixed Report ID = 0 for PTS HDHGIPSBV-03-C test.
 *        Accepts raw hex data instead of gamepad-specific format.
 */
static int atcmd_bt_hogp_ull_device_send_report_no_id(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle;
	uint16_t data_len;
	uint8_t data[64] = {0};
	uint16_t ret;

	/* Parse parameters */
	conn_handle = (uint16_t)str_to_int(argv[0]);
	data_len = strlen(argv[1]) / 2;

	if (data_len > sizeof(data)) {
		BT_LOGE("HOGP ULL send report no ID: data too long (max %d)\r\n", (int)sizeof(data));
		return -1;
	}

	hexdata_str_to_array(argv[1], data, data_len);

	ret = hogp_ull_device_send_report(conn_handle, 0, data, data_len);
	if (ret != RTK_BT_OK) {
		BT_LOGE("HOGP ULL device send report no ID fail: conn_handle=0x%04x, err=0x%x\r\n",
				conn_handle, ret);
		return -1;
	}

	BT_LOGA("HOGP ULL device report no ID sent: conn_handle=0x%04x, len=%d\r\n",
			conn_handle, data_len);
	return 0;
}

/**
 * @brief Get device status
 */
static int atcmd_bt_hogp_ull_device_status(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	BT_LOGA("HOGP ULL Device Status:\r\n");
	BT_LOGA("  Protocol Mode: %s\r\n",
			gamepad_hid_srv_get_protocol_mode() ? "Report" : "Boot");

	hid_iso_properties_t *props = hid_iso_srv_get_properties();
	uint8_t i;
	if (props != NULL) {
		BT_LOGA("  ISO Service Properties:\r\n");
		BT_LOGA("    Features: 0x%02x\r\n", props->features);
		BT_LOGA("    Supported Report Intervals: 0x%04x\r\n", props->supported_report_intervals);
		BT_LOGA("    Max SDU Size Input: %d\r\n", props->max_sdu_size_input);
		BT_LOGA("    Preferred SDU Size Input: %d\r\n", props->preferred_sdu_size_input);
		BT_LOGA("    Max SDU Size Output: %d\r\n", props->max_sdu_size_output);
		BT_LOGA("    Preferred SDU Size Output: %d\r\n", props->preferred_sdu_size_output);
		for (i = 0; i < HID_ISO_MAX_HYBRID_REPORTS; i++) {
			BT_LOGA("    Hybrid Report[%d]: id=0x%02x, additional_info=0x%02x\r\n", i,
					props->hybrid_mode_iso_reports[i].report_id,
					props->hybrid_mode_iso_reports[i].additional_info);
		}
	}

	return 0;
}

/**
 * @brief Send battery level notification (Battery Service - Server)
 */
static int atcmd_bt_hogp_ull_bas_send_notify(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	battery_cccd_notify(conn_handle);

	BT_LOGA("HOGP ULL battery notification sent to conn_handle=0x%04x\r\n", conn_handle);
	return 0;
}

/**
 * @brief Get hybrid mode Report IDs (HID ISO Service)
 */
static int atcmd_bt_hogp_ull_iso_get_report_ids(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint8_t num_reports = 0;
	uint8_t report_ids[8] = {0};
	uint8_t i;

	num_reports = hid_iso_srv_get_hybrid_report_ids(report_ids, 8);

	if (num_reports == 0) {
		BT_LOGA("HOGP ULL ISO: Not in hybrid mode or no Report IDs configured\r\n");
	} else {
		BT_LOGA("HOGP ULL ISO Hybrid Mode Report IDs (count=%d):\r\n", num_reports);
		for (i = 0; i < num_reports; i++) {
			BT_LOGA("  Report ID: 0x%02x\r\n", report_ids[i]);
		}
	}

	return 0;
}

/**
 * @brief Send Hybrid Mode indication (HID ISO Service)
 * @note Delegates to hogp_ull_device_switch_hybrid_mode() for parameter building
 */
static int atcmd_bt_hogp_ull_iso_send_hybrid(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	uint8_t cig_id = (uint8_t)str_to_int(argv[1]);
	uint8_t cis_id = (uint8_t)str_to_int(argv[2]);
	uint8_t input_conf = (uint8_t)str_to_int(argv[3]);
	uint8_t input_rep = (uint8_t)str_to_int(argv[4]);

	if (hogp_ull_device_switch_hybrid_mode(conn_handle, cig_id, cis_id, input_conf, input_rep)) {
		BT_LOGE("HOGP ULL device switch to hybrid mode fail\r\n");
		return -1;
	}

	BT_LOGA("HOGP ULL device switching to hybrid mode (CIG=%d, CIS=%d, Conf=%d, Rep=%d)...\r\n",
			cig_id, cis_id, input_conf, input_rep);
	return 0;
}

/**
 * @brief Send Default Mode indication (HID ISO Service)
 * @note Per HOGP v1.1 spec Section 6.5.2, Default Mode indication only contains opcode (1 byte)
 */
static int atcmd_bt_hogp_ull_iso_send_default(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = hid_iso_srv_send_default_mode_indication(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HOGP ULL ISO send default mode failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("HOGP ULL ISO Default Mode indication sent\r\n");
	return 0;
}

/**
 * @brief Get HID ISO service status
 */
static int atcmd_bt_hogp_ull_iso_status(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	hid_iso_properties_t *props = hid_iso_srv_get_properties();
	uint8_t num_reports = 0;
	uint8_t report_ids[8] = {0};
	uint8_t i;

	BT_LOGA("==========================================\r\n");
	BT_LOGA("HOGP ULL HID ISO Service Status:\r\n");
	BT_LOGA("==========================================\r\n");

	if (props != NULL) {
		BT_LOGA("  Features: 0x%02x\r\n", props->features);
		BT_LOGA("  Report Intervals: 0x%04x\r\n", props->supported_report_intervals);
		BT_LOGA("  Max SDU Input: %d\r\n", props->max_sdu_size_input);
		BT_LOGA("  Preferred SDU Input: %d\r\n", props->preferred_sdu_size_input);
		BT_LOGA("  Max SDU Output: %d\r\n", props->max_sdu_size_output);
		BT_LOGA("  Preferred SDU Output: %d\r\n", props->preferred_sdu_size_output);

		num_reports = hid_iso_srv_get_hybrid_report_ids(report_ids, 8);
		if (num_reports > 0) {
			BT_LOGA("  Hybrid Mode Report IDs:\r\n");
			for (i = 0; i < num_reports; i++) {
				BT_LOGA("    Report ID[%d]: 0x%02x\r\n", i, report_ids[i]);
			}
		} else {
			BT_LOGA("  Hybrid Mode: Not active\r\n");
		}
	} else {
		BT_LOGA("  HID ISO Service: Not initialized\r\n");
	}

	BT_LOGA("==========================================\r\n");
	return 0;
}

/**
 * @brief Get HID protocol mode (always Report Protocol Mode, read-only)
 */
static int atcmd_bt_hogp_ull_hid_get_protocol(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	BT_LOGA("HOGP ULL HID protocol mode: Report (fixed, read-only)\r\n");

	return 0;
}

/**
 * @brief Check if HID notification is enabled (Gamepad HID Service)
 */
static int atcmd_bt_hogp_ull_hid_is_notify_enabled(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	bool enabled;

	enabled = gamepad_hid_srv_is_notify_enabled(conn_handle);
	BT_LOGA("HOGP ULL HID notification: %s (conn_handle=0x%04x)\r\n",
			enabled ? "ENABLED" : "DISABLED", conn_handle);

	return 0;
}

/**
 * @brief Request scan parameters refresh (Scan Parameters Service)
 */
static int atcmd_bt_hogp_ull_scan_request_refresh(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = scan_param_srv_request_refresh(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HOGP ULL scan request refresh failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("HOGP ULL scan refresh notification sent to conn_handle=0x%04x\r\n", conn_handle);
	return 0;
}

/**
 * @brief Get scan parameters (Scan Parameters Service)
 */
static int atcmd_bt_hogp_ull_scan_get_params(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	scan_interval_window_t params = {0};

	ret = scan_param_srv_get_scan_params(conn_handle, &params);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HOGP ULL scan get params failed! err: 0x%x\r\n", ret);
		return -1;
	}

	BT_LOGA("HOGP ULL scan parameters (conn_handle=0x%04x):\r\n", conn_handle);
	BT_LOGA("  Interval: 0x%04x (%d ms)\r\n", params.le_scan_interval, (int)(params.le_scan_interval * 0.625));
	BT_LOGA("  Window: 0x%04x (%d ms)\r\n", params.le_scan_window, (int)(params.le_scan_window * 0.625));

	return 0;
}

/**
 * @brief Check if scan refresh notification is enabled (Scan Parameters Service)
 */
static int atcmd_bt_hogp_ull_scan_is_notify_enabled(int argc, char **argv)
{
	(void)argc;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	bool enabled;

	enabled = scan_param_srv_is_refresh_notify_enabled(conn_handle);
	BT_LOGA("HOGP ULL scan refresh notification: %s (conn_handle=0x%04x)\r\n",
			enabled ? "ENABLED" : "DISABLED", conn_handle);

	return 0;
}

/* ============================================================================
 *                           Report Host Subcommands
 * ============================================================================ */

/**
 * @brief Connect to HID device
 */
static int atcmd_bt_hogp_ull_connect(int argc, char **argv)
{
	(void)argc;
	rtk_bt_le_addr_t peer_addr = {0};
	char addr_str[30] = {0};

	hexdata_str_to_bd_addr(argv[0], peer_addr.addr_val, RTK_BD_ADDR_LEN);
	peer_addr.type = RTK_BT_LE_ADDR_TYPE_PUBLIC;

	if (hogp_ull_report_host_connect(&peer_addr)) {
		BT_LOGE("HOGP ULL connect fail\r\n");
		return -1;
	}

	rtk_bt_le_addr_to_str(&peer_addr, addr_str, sizeof(addr_str));
	BT_LOGA("HOGP ULL connecting to device %s ...\r\n", addr_str);

	return 0;
}

/**
 * @brief Disconnect from HID device
 */
static int atcmd_bt_hogp_ull_disconnect(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (hogp_ull_report_host_disconnect()) {
		BT_LOGE("HOGP ULL disconnect fail\r\n");
		return -1;
	}

	BT_LOGA("HOGP ULL disconnecting ...\r\n");
	return 0;
}

/**
 * @brief Switch to hybrid mode
 */
static int atcmd_bt_hogp_ull_switch_hybrid(int argc, char **argv)
{
	(void)argc;
	uint8_t cig_id = (uint8_t)str_to_int(argv[0]);
	uint8_t cis_id = (uint8_t)str_to_int(argv[1]);
	uint8_t input_conf = (uint8_t)str_to_int(argv[2]);
	uint8_t input_rep = (uint8_t)str_to_int(argv[3]);

	if (hogp_ull_report_host_switch_hybrid_mode(cig_id, cis_id, input_conf, input_rep)) {
		BT_LOGE("HOGP ULL switch to hybrid mode fail\r\n");
		return -1;
	}

	BT_LOGA("HOGP ULL switching to hybrid mode (CIG=%d, CIS=%d, Conf=%d, Rep=%d)...\r\n",
			cig_id, cis_id, input_conf, input_rep);
	return 0;
}

/**
 * @brief Switch to default mode
 */
static int atcmd_bt_hogp_ull_switch_default(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	if (hogp_ull_report_host_switch_default_mode()) {
		BT_LOGE("HOGP ULL switch to default mode fail\r\n");
		return -1;
	}

	BT_LOGA("HOGP ULL switching to default mode...\r\n");
	return 0;
}

/**
 * @brief Get host status
 */
static int atcmd_bt_hogp_ull_host_status(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	report_host_context_t *ctx = hogp_ull_report_host_get_ctx();
	hid_iso_client_ctx_t *iso_ctx = hid_iso_client_get_ctx(ctx->conn_handle);

	BT_LOGA("==========================================\r\n");
	BT_LOGA("HOGP ULL Host Status:\r\n");
	BT_LOGA("==========================================\r\n");

	/* Connection Status */
	BT_LOGA("[Connection]\r\n");
	if (ctx->conn_handle != 0) {
		BT_LOGA("  conn_handle: 0x%04x\r\n", ctx->conn_handle);
		BT_LOGA("  conn_id: %d\r\n", ctx->conn_id);
		BT_LOGA("  state: CONNECTED\r\n");
	} else {
		BT_LOGA("  state: DISCONNECTED\r\n");
	}

	/* Operation Mode */
	BT_LOGA("[Operation Mode]\r\n");
	const char *mode_str[] = {"DEFAULT", "DEFAULT_PENDING", "HYBRID_PENDING", "HYBRID"};
	if (ctx->current_mode <= 3) {
		BT_LOGA("  current_mode: %s\r\n", mode_str[ctx->current_mode]);
	} else {
		BT_LOGA("  current_mode: UNKNOWN (%d)\r\n", ctx->current_mode);
	}
	if (ctx->current_mode != 0) {
		BT_LOGA("  cig_id: %d, cis_id: %d\r\n", ctx->cig_id, ctx->cis_id);
		BT_LOGA("  cis_handle: 0x%04x\r\n", ctx->cis_conn_handle);
	}

	/* HID ISO Client Status */
	BT_LOGA("[HID ISO Client]\r\n");
	if (iso_ctx != NULL && iso_ctx->used) {
		uint8_t i;
		BT_LOGA("  service_found: %s\r\n", iso_ctx->service_found ? "YES" : "NO");
		if (iso_ctx->service_found) {
			BT_LOGA("  service_handle: 0x%04x-0x%04x\r\n",
					iso_ctx->srv_start_handle, iso_ctx->srv_end_handle);
			BT_LOGA("  properties_handle: 0x%04x\r\n", iso_ctx->properties_handle);
			BT_LOGA("  operation_mode_handle: 0x%04x\r\n", iso_ctx->operation_mode_handle);
			BT_LOGA("  operation_mode_cccd_handle: 0x%04x\r\n", iso_ctx->operation_mode_cccd_handle);
			BT_LOGA("  iso_properties:\r\n");
			BT_LOGA("    features: 0x%02x\r\n", iso_ctx->iso_properties.features);
			BT_LOGA("    supported_report_intervals: 0x%04x\r\n",
					iso_ctx->iso_properties.supported_report_intervals);
			BT_LOGA("    max_sdu_size_input: %d\r\n", iso_ctx->iso_properties.max_sdu_size_input);
			BT_LOGA("    preferred_sdu_size_input: %d\r\n",
					iso_ctx->iso_properties.preferred_sdu_size_input);
			BT_LOGA("    max_sdu_size_output: %d\r\n", iso_ctx->iso_properties.max_sdu_size_output);
			BT_LOGA("    preferred_sdu_size_output: %d\r\n",
					iso_ctx->iso_properties.preferred_sdu_size_output);
			for (i = 0; i < HID_ISO_MAX_HYBRID_REPORTS; i++) {
				BT_LOGA("    hybrid_report[%d]: id=0x%02x, additional_info=0x%02x\r\n", i,
						iso_ctx->iso_properties.hybrid_mode_iso_reports[i].report_id,
						iso_ctx->iso_properties.hybrid_mode_iso_reports[i].additional_info);
			}
		}
	} else {
		BT_LOGA("  state: NOT INITIALIZED\r\n");
	}

	BT_LOGA("==========================================\r\n");
	return 0;
}

/* ============================================================================
 *                           BAS Client Subcommands
 * ============================================================================ */

/**
 * @brief BAS Client discover service
 */
static int atcmd_bt_hogp_ull_bas_discover(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
	ret = bas_client_srv_discover(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("BAS client discover failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("BAS client discovering ...\r\n");
#else
	(void)ret;
	(void)conn_handle;
	BT_LOGA("BAS client discover not needed (BLE MGR mode)\r\n");
#endif
	return 0;
}

/**
 * @brief BAS Client read battery level
 */
static int atcmd_bt_hogp_ull_bas_read(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = bas_client_read_battery_level(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("BAS client read battery level failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("BAS client reading battery level ...\r\n");
	return 0;
}

/**
 * @brief BAS Client enable/disable notification
 */
static int atcmd_bt_hogp_ull_bas_notify(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	bool enable = (bool)str_to_int(argv[1]);

	ret = bas_client_set_notify(conn_handle, enable);
	if (RTK_BT_OK != ret) {
		BT_LOGE("BAS client set notify failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("BAS client %s notification ...\r\n", enable ? "enabling" : "disabling");
	return 0;
}

/* ============================================================================
 *                           HIDS GP Client Subcommands
 * ============================================================================ */

/**
 * @brief HIDS GP Client discover service
 */
static int atcmd_bt_hogp_ull_hids_discover(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = hids_gp_client_discover(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HIDS GP client discover failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HIDS GP client discovering ...\r\n");
	return 0;
}

/**
 * @brief HIDS GP Client set protocol mode
 */
static int atcmd_bt_hogp_ull_hids_set_protocol(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	uint8_t mode = (uint8_t)str_to_int(argv[1]);

	ret = hids_gp_client_set_protocol_mode(conn_handle, mode);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HIDS GP client set protocol mode failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HIDS GP client setting protocol mode to %s ...\r\n",
			mode ? "Report" : "Boot");
	return 0;
}

/**
 * @brief HIDS GP Client read report map
 */
static int atcmd_bt_hogp_ull_hids_read_report_map(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = hids_gp_client_read_report_map(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HIDS GP client read report map failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HIDS GP client reading report map ...\r\n");
	return 0;
}

/**
 * @brief HIDS GP Client read report references
 */
static int atcmd_bt_hogp_ull_hids_read_report_refs(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = hids_gp_client_read_report_refs(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HIDS GP client read report refs failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HIDS GP client reading report references ...\r\n");
	return 0;
}

/**
 * @brief HIDS GP Client enable/disable input report notification
 */
static int atcmd_bt_hogp_ull_hids_set_notify(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	uint8_t report_idx = (uint8_t)str_to_int(argv[1]);
	bool enable = (bool)str_to_int(argv[2]);

	ret = hids_gp_client_set_notify(conn_handle, report_idx, enable);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HIDS GP client set notify failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HIDS GP client %s input report notification (idx=%d) ...\r\n",
			enable ? "enabling" : "disabling", report_idx);
	return 0;
}

/**
 * @brief HIDS GP Client write output report
 */
static int atcmd_bt_hogp_ull_hids_write_output(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	uint8_t report_idx = (uint8_t)str_to_int(argv[1]);
	uint16_t data_len = strlen(argv[2]) / 2;
	uint8_t data[HIDS_GP_MAX_REPORT_DATA_LEN] = {0};

	if (data_len > HIDS_GP_MAX_REPORT_DATA_LEN) {
		BT_LOGE("HIDS GP write output: data too long (max %d)\r\n", HIDS_GP_MAX_REPORT_DATA_LEN);
		return -1;
	}

	hexdata_str_to_array(argv[2], data, data_len);

	ret = hids_gp_client_write_output_report(conn_handle, report_idx, data, data_len);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HIDS GP client write output report failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HIDS GP client writing output report (idx=%d, len=%d) ...\r\n",
			report_idx, data_len);
	return 0;
}

/* ============================================================================
 *                           HID ISO Client Subcommands
 * ============================================================================ */

/**
 * @brief HID ISO Client discover service
 */
static int atcmd_bt_hogp_ull_iso_discover(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = hid_iso_client_discover(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HID ISO client discover failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HID ISO client discovering ...\r\n");
	return 0;
}

/**
 * @brief HID ISO Client read properties
 */
static int atcmd_bt_hogp_ull_iso_read_props(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = hid_iso_client_read_properties(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HID ISO client read properties failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HID ISO client reading properties ...\r\n");
	return 0;
}

/**
 * @brief HID ISO Client enable/disable indication
 */
static int atcmd_bt_hogp_ull_iso_enable_indicate(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	bool enable = (bool)str_to_int(argv[1]);

	ret = hid_iso_client_enable_indication(conn_handle, enable);
	if (RTK_BT_OK != ret) {
		BT_LOGE("HID ISO client enable indication failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("HID ISO client %s indication ...\r\n", enable ? "enabling" : "disabling");
	return 0;
}

/* ============================================================================
 *                           Scan Param Client Subcommands
 * ============================================================================ */

/**
 * @brief Scan Param Client discover service
 */
static int atcmd_bt_hogp_ull_scan_discover(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);

	ret = scan_param_client_discover(conn_handle);
	if (RTK_BT_OK != ret) {
		BT_LOGE("Scan Param client discover failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("Scan Param client discovering ...\r\n");
	return 0;
}

/**
 * @brief Scan Param Client write scan interval/window
 */
static int atcmd_bt_hogp_ull_scan_write(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	scan_interval_window_t params;

	params.le_scan_interval = (uint16_t)str_to_int(argv[1]);
	params.le_scan_window = (uint16_t)str_to_int(argv[2]);

	ret = scan_param_client_write_scan_interval_window(conn_handle, &params);
	if (RTK_BT_OK != ret) {
		BT_LOGE("Scan Param client write scan interval/window failed! err: 0x%x\r\n", ret);
		return -1;
	}
	return 0;
}

/**
 * @brief Scan Param Client enable/disable refresh notification
 */
static int atcmd_bt_hogp_ull_scan_refresh_notify(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint16_t conn_handle = (uint16_t)str_to_int(argv[0]);
	bool enable = (bool)str_to_int(argv[1]);

	ret = scan_param_client_set_refresh_notify(conn_handle, enable);
	if (RTK_BT_OK != ret) {
		BT_LOGE("Scan Param client set refresh notify failed! err: 0x%x\r\n", ret);
		return -1;
	}
	BT_LOGA("Scan Param client %s refresh notification ...\r\n",
			enable ? "enabling" : "disabling");
	return 0;
}

#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
/**
 * @brief Extended scan start/stop
 * @param argc 2, argv[0]: 0=stop, 1=start
 */
static int atcmd_bt_hogp_ull_ext_scan(int argc, char **argv)
{
	(void)argc;
	uint16_t ret;
	uint8_t op = (uint8_t)str_to_int(argv[0]);

	if (op == 1) {
		rtk_bt_le_ext_scan_param_t ext_scan_param = {0};
		ext_scan_param.own_addr_type = RTK_BT_LE_ADDR_TYPE_PUBLIC;
		ext_scan_param.phys[0] = true;   /* 1M PHY */
		ext_scan_param.phys[1] = true;   /* Coded PHY */
		ext_scan_param.type[0] = RTK_BT_LE_SCAN_TYPE_ACTIVE;
		ext_scan_param.type[1] = RTK_BT_LE_SCAN_TYPE_ACTIVE;
		ext_scan_param.interval[0] = 0x30;
		ext_scan_param.interval[1] = 0x30;
		ext_scan_param.window[0] = 0x30;
		ext_scan_param.window[1] = 0x30;
		ext_scan_param.duration = 0;
		ext_scan_param.period = 0;
		ext_scan_param.filter_policy = RTK_BT_LE_SCAN_FILTER_ALLOW_ALL;
		ext_scan_param.duplicate_opt = RTK_BT_LE_SCAN_DUPLICATE_ENABLE;

		ret = rtk_bt_le_gap_set_ext_scan_param(&ext_scan_param);
		if (ret != RTK_BT_OK) {
			BT_LOGE("Set ext scan param failed: %d\r\n", ret);
			return -1;
		}

		ret = rtk_bt_le_gap_start_ext_scan();
		if (ret != RTK_BT_OK) {
			BT_LOGE("Start ext scan failed: %d\r\n", ret);
			return -1;
		}

		BT_LOGA("Ext scan started\r\n");
	} else {
		ret = rtk_bt_le_gap_stop_ext_scan();
		if (ret != RTK_BT_OK) {
			BT_LOGE("Stop ext scan failed: %d\r\n", ret);
			return -1;
		}

		BT_LOGA("Ext scan stopped\r\n");
	}

	return 0;
}
#endif
/* ============================================================================
 *                           AT Command Table
 * ============================================================================ */

static const cmd_table_t hogp_ull_cmd_table[] = {
	/* HOGP v1.1 ULL HID Device commands */
	/* HID ISO service server commands */
	{"hid_iso_status",            atcmd_bt_hogp_ull_iso_status,             1, 1},
	{"device_status",             atcmd_bt_hogp_ull_device_status,          1, 1},
	{"device_get_report_ids",     atcmd_bt_hogp_ull_iso_get_report_ids,     1, 1},
	{"device_switch_hybrid",      atcmd_bt_hogp_ull_iso_send_hybrid,        6, 6},
	{"device_switch_default",     atcmd_bt_hogp_ull_iso_send_default,       2, 2},
	{"device_send_report",        atcmd_bt_hogp_ull_device_send_report,     9, 9},
	{"device_send_report_no_id",  atcmd_bt_hogp_ull_device_send_report_no_id, 3, 3},
	/* BAS Server commands */
	{"bas_send_notify",           atcmd_bt_hogp_ull_bas_send_notify,        2, 2},
	/* HIDS GP Server commands */
	{"hids_gp_send_gamepad",         atcmd_bt_hogp_ull_send_gamepad,           8, 8},
	{"hid_gp_get_protocol",          atcmd_bt_hogp_ull_hid_get_protocol,       1, 1},
	{"hid_gp_is_notify_enabled",     atcmd_bt_hogp_ull_hid_is_notify_enabled,  2, 2},
	/* Scan Param Server commands */
	{"scan_request_refresh",      atcmd_bt_hogp_ull_scan_request_refresh,   2, 2},
	{"scan_get_params",           atcmd_bt_hogp_ull_scan_get_params,        2, 2},
	{"scan_is_notify_enabled",    atcmd_bt_hogp_ull_scan_is_notify_enabled, 2, 2},

	/* HOGP v1.1 ULL Report Host commands */
	{"connect",              atcmd_bt_hogp_ull_connect,          2, 2},
	{"disconnect",           atcmd_bt_hogp_ull_disconnect,       1, 1},
#if defined(RTK_BLE_5_0_USE_EXTENDED_ADV) && RTK_BLE_5_0_USE_EXTENDED_ADV
	{"ext_scan",             atcmd_bt_hogp_ull_ext_scan,         2, 2},
#endif
	/* BAS Client commands */
	{"bas_discover",    atcmd_bt_hogp_ull_bas_discover,     2, 2},
	{"bas_read",        atcmd_bt_hogp_ull_bas_read,         2, 2},
	{"bas_notify",      atcmd_bt_hogp_ull_bas_notify,       3, 3},
	/* HIDS GP Client commands */
	{"hids_gp_discover",        atcmd_bt_hogp_ull_hids_discover,        2, 2},
	{"hids_gp_set_protocol",    atcmd_bt_hogp_ull_hids_set_protocol,    3, 3},
	{"hids_gp_read_report_map", atcmd_bt_hogp_ull_hids_read_report_map, 2, 2},
	{"hids_gp_read_report_refs", atcmd_bt_hogp_ull_hids_read_report_refs, 2, 2},
	{"hids_gp_set_notify",      atcmd_bt_hogp_ull_hids_set_notify,      4, 4},
	{"hids_gp_write_output",    atcmd_bt_hogp_ull_hids_write_output,    4, 4},
	/* HID ISO Client commands */
	{"host_switch_hybrid",      atcmd_bt_hogp_ull_switch_hybrid,       5, 5},
	{"host_switch_default",     atcmd_bt_hogp_ull_switch_default,      1, 1},
	{"host_status",             atcmd_bt_hogp_ull_host_status,         1, 1},
	{"hid_iso_discover",        atcmd_bt_hogp_ull_iso_discover,        2, 2},
	{"hid_iso_read_props",      atcmd_bt_hogp_ull_iso_read_props,      2, 2},
	{"hid_iso_enable_indicate", atcmd_bt_hogp_ull_iso_enable_indicate, 3, 3},
	/* Scan Param Client commands */
	{"scan_discover",       atcmd_bt_hogp_ull_scan_discover,       2, 2},
	{"scan_write",          atcmd_bt_hogp_ull_scan_write,          4, 4},
	{"scan_refresh_notify", atcmd_bt_hogp_ull_scan_refresh_notify, 3, 3},

	{NULL,},
};

int atcmd_bt_hogp_ull_cmd(int argc, char *argv[])
{
	return atcmd_bt_excute(argc, argv, hogp_ull_cmd_table, "[AT+BTDEMO=hogp_ull]");
}