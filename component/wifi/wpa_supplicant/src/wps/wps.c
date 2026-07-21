/*
 * Wi-Fi Protected Setup
 * Copyright (c) 2007-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "crypto/dh_group5.h"
#include "wps/wps_i.h"
#include "wps_dev_attr.h"
#include "../wpa_supplicant/wps_protocol_handler.h"

//#ifdef CONFIG_WPS

/**
 * wps_init - Initialize WPS Registration protocol data
 * @cfg: WPS configuration
 * Returns: Pointer to allocated data or %NULL on failure
 *
 * This function is used to initialize WPS data for a registration protocol
 * instance (i.e., each run of registration protocol as a Registrar of
 * Enrollee. The caller is responsible for freeing this data after the
 * registration run has been completed by calling wps_deinit().
 */
extern void wpas_get_wifi_mac(u8 *dev_mac);
struct wps_data *wps_init(const struct wps_config *cfg)
{
	struct wps_data *data = NULL;
	u8 temp_uuid_e[WPS_UUID_LEN] = {0};
	u8 hwaddr[ETH_ALEN];

	wpas_get_wifi_mac(hwaddr);
	data = (struct wps_data *)os_zalloc(sizeof(*data));
	if (data == NULL) {
		return NULL;
	}

	// cfg->registrar = 0
	data->registrar = cfg->registrar;

	// use uuid same with dev config
	//os_memcpy(&temp_uuid_e[10], hwaddr, ETH_ALEN);
	os_memcpy(temp_uuid_e, g_wps_context.wps->uuid, WPS_UUID_LEN);

	os_memcpy(data->mac_addr_e, hwaddr, ETH_ALEN);
	if (cfg->registrar) {
		os_memcpy(data->uuid_r, temp_uuid_e, WPS_UUID_LEN);
	} else {
		os_memcpy(data->uuid_e, temp_uuid_e, WPS_UUID_LEN);
	}

	// cfg->pin = 0
	if (cfg->pin) {
		data->dev_pw_id = DEV_PW_DEFAULT;//cfg->dev_pw_id;
		if (data->dev_password != NULL) {
			os_free(data->dev_password, 0);
		}
		data->dev_password = (u8 *)os_malloc((cfg->pin_len));
		wpa_printf(MSG_DEBUG, "cfg->pin_len = 0x%x", cfg->pin_len);
		wpa_printf(MSG_DEBUG, "cfg->pin = %s", cfg->pin);
		if (data->dev_password == NULL) {
			os_free(data, 0);
			return NULL;
		}
		os_memcpy(data->dev_password, cfg->pin, cfg->pin_len);
		data->dev_password_len = cfg->pin_len;
	}


	data->pbc = cfg->pbc;
	if (cfg->pbc) {
		/* Use special PIN code'00000000' for PBC */
		data->dev_pw_id = DEV_PW_PUSHBUTTON;

		if (data->dev_password != NULL) {
			os_free(data->dev_password, 0);
		}

		data->dev_password = (u8 *)os_strdup("00000000");
		if (data->dev_password == NULL) {
			os_free(data, 0);
			return NULL;
		}
		data->dev_password_len = 8;
	}


	data->state = data->registrar ? RECV_M1 : SEND_M1;

	data->use_psk_key = 0;
	data->pbc_in_m1 = 0;

	return data;
}


/**
 * wps_deinit - Deinitialize WPS Registration protocol data
 * @data: WPS Registration protocol data from wps_init()
 */
void wps_deinit(struct wps_data *data)
{

	wpabuf_free(data->dh_privkey);
	wpabuf_free(data->dh_pubkey_e);
	wpabuf_free(data->dh_pubkey_r);
	wpabuf_free(data->last_msg);
	os_free(data->dev_password, 0);
	os_free(data->new_psk, 0);
	wps_device_data_free(&data->peer_dev);
	os_free(data->new_ap_settings, 0);
	dh5_free(data->dh_ctx);
	os_free(data, 0);
}


/**
 * wps_process_msg - Process a WPS message
 * @wps: WPS Registration protocol data from wps_init()
 * @op_code: Message OP Code
 * @msg: Message data
 * Returns: Processing result
 *
 * This function is used to process WPS messages with OP Codes WSC_ACK,
 * WSC_NACK, WSC_MSG, and WSC_Done. The caller (e.g., EAP server/peer) is
 * responsible for reassembling the messages before calling this function.
 * Response to this message is built by calling wps_get_msg().
 */		// wps = data->wps,     msg = data->in_buf
enum wps_process_res wps_process_msg(struct wps_data *wps,
									 enum wsc_op_code op_code, const struct wpabuf *msg)
{
	if (wps->registrar) {
		return (enum wps_process_res)wps_registrar_process_msg((void *)wps, op_code, (const void *)msg);
	} else {
		return wps_enrollee_process_msg(wps, op_code, msg);
	}
}


/**
 * wps_get_msg - Build a WPS message
 * @wps: WPS Registration protocol data from wps_init()
 * @op_code: Buffer for returning message OP Code
 * Returns: The generated WPS message or %NULL on failure
 *
 * This function is used to build a response to a message processed by calling
 * wps_process_msg(). The caller is responsible for freeing the buffer.
 */
struct wpabuf *wps_get_msg(struct wps_data *wps, enum wsc_op_code *op_code)
{
	if (wps->registrar) {
		return (struct wpabuf *)wps_registrar_get_msg((void *)wps, op_code);
	} else {
		return wps_enrollee_get_msg(wps, op_code);
	}
}

/**
 * wps_build_assoc_req_ie - Build WPS IE for (Re)Association Request
 * @req_type: Value for Request Type attribute
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
struct wpabuf *wps_build_assoc_req_ie(enum wps_request_type req_type)
{
	struct wpabuf *ie;
	u8 *len;

	wpa_printf(MSG_DEBUG, "WPS: Building WPS IE for (Re)Association "
			   "Request");
	ie = wpabuf_alloc(100);
	if (ie == NULL) {
		return NULL;
	}

	wpabuf_put_u8(ie, WLAN_EID_VENDOR_SPECIFIC);
	len = wpabuf_put(ie, 1);
	wpabuf_put_be32(ie, WPS_DEV_OUI_WFA);

	if (wps_build_version(ie) ||
		wps_build_req_type(ie, req_type) ||
		wps_build_wfa_ext(ie, 0, NULL, 0)) {
		wpabuf_free(ie);
		return NULL;
	}

	*len = wpabuf_len(ie) - 2;

	return ie;
}

#if 0
/**
 * wps_build_assoc_resp_ie - Build WPS IE for (Re)Association Response
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
struct wpabuf *wps_build_assoc_resp_ie(void)
{
	struct wpabuf *ie;
	u8 *len;

	wpa_printf(MSG_DEBUG, "WPS: Building WPS IE for (Re)Association "
			   "Response");
	ie = wpabuf_alloc(100);
	if (ie == NULL) {
		return NULL;
	}

	wpabuf_put_u8(ie, WLAN_EID_VENDOR_SPECIFIC);
	len = wpabuf_put(ie, 1);
	wpabuf_put_be32(ie, WPS_DEV_OUI_WFA);

	if (wps_build_version(ie) ||
		wps_build_resp_type(ie, WPS_RESP_AP) ||
		wps_build_wfa_ext(ie, 0, NULL, 0)) {
		wpabuf_free(ie);
		return NULL;
	}

	*len = wpabuf_len(ie) - 2;

	return ie;
}
#endif

/**
 * wps_build_probe_req_ie - Build WPS IE for Probe Request
 * @pw_id: Password ID (DEV_PW_PUSHBUTTON for active PBC and DEV_PW_DEFAULT for
 * most other use cases)
 * @dev: Device attributes
 * @uuid: Own UUID
 * @req_type: Value for Request Type attribute
 * @num_req_dev_types: Number of requested device types
 * @req_dev_types: Requested device types (8 * num_req_dev_types octets) or
 *	%NULL if none
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
struct wpabuf *wps_build_probe_req_ie(u16 pw_id, struct wps_device_data *dev,
									  const u8 *uuid,
									  enum wps_request_type req_type,
									  unsigned int num_req_dev_types,
									  const u8 *req_dev_types)
{
	struct wpabuf *ie;

	wpa_printf(MSG_DEBUG, "WPS: Building WPS IE for Probe Request");

	ie = wpabuf_alloc(500);
	if (ie == NULL) {
		return NULL;
	}

	if (wps_build_version(ie) ||
		wps_build_req_type(ie, req_type) ||
		wps_build_config_methods(ie, dev->config_methods) ||
		wps_build_uuid_e(ie, uuid) ||
		wps_build_primary_dev_type(dev, ie) ||
		wps_build_rf_bands(dev, ie) ||
		wps_build_assoc_state(NULL, ie) ||
		wps_build_config_error(ie, WPS_CFG_NO_ERROR) ||
		wps_build_dev_password_id(ie, pw_id) ||
#ifdef CONFIG_WPS2
		wps_build_manufacturer(dev, ie) ||
		wps_build_model_name(dev, ie) ||
		wps_build_model_number(dev, ie) ||
		wps_build_dev_name(dev, ie) ||
		wps_build_wfa_ext(ie, req_type == WPS_REQ_ENROLLEE, NULL, 0) ||
#endif // CONFIG_WPS2
		wps_build_req_dev_type(dev, ie, num_req_dev_types, req_dev_types)
		||
		wps_build_secondary_dev_type(dev, ie)
	   ) {
		wpabuf_free(ie);
		return NULL;
	}

	return wps_ie_encapsulate(ie);
}


//#endif
