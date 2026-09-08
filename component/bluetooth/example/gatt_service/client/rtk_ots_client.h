/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __RTK_OTS_CLIENT_H__
#define __RTK_OTS_CLIENT_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_common.h>
#include <bt_api_config.h>

/* ------------------------------ OTS UUIDs (OTP over OTS v1.0) ------------------- */
#define OTS_CLIENT_SRV_UUID                 0x1825
#define OTS_CLIENT_FEATURE_CHAR_UUID        0x2ABD
#define OTS_CLIENT_OBJ_NAME_CHAR_UUID       0x2ABE
#define OTS_CLIENT_OBJ_TYPE_CHAR_UUID       0x2ABF
#define OTS_CLIENT_OBJ_SIZE_CHAR_UUID       0x2AC0
#define OTS_CLIENT_OBJ_FIRST_CREATED_UUID   0x2AC1
#define OTS_CLIENT_OBJ_LAST_MODIFIED_UUID   0x2AC2
#define OTS_CLIENT_OBJ_ID_CHAR_UUID         0x2AC3
#define OTS_CLIENT_OBJ_PROPERTIES_UUID      0x2AC4
#define OTS_CLIENT_OACP_CHAR_UUID           0x2AC5
#define OTS_CLIENT_OLCP_CHAR_UUID           0x2AC6
#define OTS_CLIENT_OBJ_LIST_FILTER_UUID     0x2AC7
#define OTS_CLIENT_OBJ_CHANGED_CHAR_UUID    0x2AC8

/* ------------------------------- Object Transfer Channel ------------------------ */
#define OTS_CLIENT_PSM                      0x0025
#define OTS_CLIENT_COC_LOCAL_MTU            247
#define OTS_CLIENT_OBJ_ID_LEN               6      /* uint48 */

/* --------------------------------- OACP op codes -------------------------------- */
#define OTS_CLIENT_OACP_OP_CREATE           0x01
#define OTS_CLIENT_OACP_OP_DELETE           0x02
#define OTS_CLIENT_OACP_OP_CALC_CHECKSUM    0x03
#define OTS_CLIENT_OACP_OP_EXECUTE          0x04
#define OTS_CLIENT_OACP_OP_READ             0x05
#define OTS_CLIENT_OACP_OP_WRITE            0x06
#define OTS_CLIENT_OACP_OP_ABORT            0x07
#define OTS_CLIENT_OACP_OP_RESP             0x60

/* OACP result codes */
#define OTS_CLIENT_OACP_RES_SUCCESS             0x01
#define OTS_CLIENT_OACP_RES_OP_NOT_SUPPORTED    0x02
#define OTS_CLIENT_OACP_RES_INVALID_PARAM       0x03
#define OTS_CLIENT_OACP_RES_INSUFFICIENT_RES    0x04
#define OTS_CLIENT_OACP_RES_INVALID_OBJECT      0x05
#define OTS_CLIENT_OACP_RES_CHANNEL_UNAVAILABLE 0x06
#define OTS_CLIENT_OACP_RES_UNSUPPORTED_TYPE    0x07
#define OTS_CLIENT_OACP_RES_PROC_NOT_PERMITTED  0x08
#define OTS_CLIENT_OACP_RES_OBJECT_LOCKED       0x09
#define OTS_CLIENT_OACP_RES_OPERATION_FAILED    0x0A

/* --------------------------------- OLCP op codes -------------------------------- */
#define OTS_CLIENT_OLCP_OP_FIRST            0x01
#define OTS_CLIENT_OLCP_OP_LAST             0x02
#define OTS_CLIENT_OLCP_OP_PREVIOUS         0x03
#define OTS_CLIENT_OLCP_OP_NEXT             0x04
#define OTS_CLIENT_OLCP_OP_GO_TO            0x05
#define OTS_CLIENT_OLCP_OP_ORDER            0x06
#define OTS_CLIENT_OLCP_OP_REQ_NUM_OBJ      0x07
#define OTS_CLIENT_OLCP_OP_CLEAR_MARKING    0x08
#define OTS_CLIENT_OLCP_OP_RESP             0x70

/* OLCP result codes */
#define OTS_CLIENT_OLCP_RES_SUCCESS             0x01
#define OTS_CLIENT_OLCP_RES_OP_NOT_SUPPORTED    0x02
#define OTS_CLIENT_OLCP_RES_INVALID_PARAM       0x03
#define OTS_CLIENT_OLCP_RES_OPERATION_FAILED    0x04
#define OTS_CLIENT_OLCP_RES_OUT_OF_BOUNDS       0x05
#define OTS_CLIENT_OLCP_RES_TOO_MANY_OBJECTS    0x06
#define OTS_CLIENT_OLCP_RES_NO_OBJECT           0x07
#define OTS_CLIENT_OLCP_RES_OBJECT_ID_NOT_FOUND 0x08

/* Object metadata identifiers (for the read-metadata API) */
typedef enum {
	OTS_CLIENT_META_FEATURE = 0,
	OTS_CLIENT_META_OBJ_NAME,
	OTS_CLIENT_META_OBJ_TYPE,
	OTS_CLIENT_META_OBJ_SIZE,
	OTS_CLIENT_META_OBJ_ID,
	OTS_CLIENT_META_OBJ_PROPERTIES,
	OTS_CLIENT_META_OBJ_LIST_FILTER,
} ots_client_meta_type_t;

/* -------------------------------- Public interface ------------------------------ */
uint16_t ots_client_add(void);

uint16_t ots_client_delete(void);

uint16_t ots_client_attach_conn(uint16_t conn_handle);

uint16_t ots_client_detach_conn(uint16_t conn_handle);

rtk_bt_evt_cb_ret_t ots_client_app_callback(uint8_t event, void *data);

#if !defined(RTK_BLE_MGR_LIB) || !RTK_BLE_MGR_LIB
uint16_t ots_client_srv_discover(uint16_t conn_handle);
#endif

/* Feature Discovery / metadata read */
uint16_t ots_client_read_metadata(uint16_t conn_handle, ots_client_meta_type_t type);
uint16_t ots_client_read_long_name(uint16_t conn_handle, uint16_t offset);

/* Object List Control Point procedures */
uint16_t ots_client_olcp_first(uint16_t conn_handle);
uint16_t ots_client_olcp_last(uint16_t conn_handle);
uint16_t ots_client_olcp_previous(uint16_t conn_handle);
uint16_t ots_client_olcp_next(uint16_t conn_handle);
uint16_t ots_client_select_by_id(uint16_t conn_handle, uint8_t obj_id[OTS_CLIENT_OBJ_ID_LEN]);
uint16_t ots_client_olcp_write(uint16_t conn_handle, uint8_t op, uint8_t *param, uint16_t param_len);

/* Object Action Control Point procedures */
uint16_t ots_client_read_contents(uint16_t conn_handle, uint32_t offset, uint32_t length);
uint16_t ots_client_write_contents(uint16_t conn_handle, uint32_t offset, uint32_t length, uint8_t mode);
uint16_t ots_client_oacp_write(uint16_t conn_handle, uint8_t op, uint8_t *param, uint16_t param_len);

/* Object Transfer Channel */
uint16_t ots_client_open_channel(uint16_t conn_handle);
uint16_t ots_client_close_channel(uint16_t conn_handle);

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
/* Object Transfer Channel (LE CoC) event hooks, dispatched from the LE GAP callback. */
void ots_client_coc_connect_hdl(void *data);
void ots_client_coc_disconnect_hdl(void *data);
void ots_client_coc_recv_data_hdl(void *data);
void ots_client_coc_send_result_hdl(void *data);
#endif

/* Forward an OACP/OLCP/ObjChanged indication that arrived on the GCS profile
 * (profile_id 0) because CCCD was enabled via bare ATT write rather than through
 * the OTS client profile.  Call this from the GCS indication branch in central.c. */
void ots_client_forward_gcs_indication(uint16_t conn_handle, uint16_t value_handle,
									   uint8_t *value, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_OTS_CLIENT_H__ */
