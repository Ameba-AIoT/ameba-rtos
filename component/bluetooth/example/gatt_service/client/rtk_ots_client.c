/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gattc.h>
#include <rtk_client_config.h>
#include <rtk_ots_client.h>
#include <rtk_gcs_client.h>
#include <bt_utils.h>

/* Control point (OACP/OLCP) timeout, and object transfer stall timeout (OTP 4.4.4 / 4.5.4.2). */
#define OTS_CLIENT_CP_TIMEOUT_MS        30000
#define OTS_CLIENT_XFER_TIMEOUT_MS      30000
#define OTS_CLIENT_TASK_STACK_SIZE      (256 * 4)
#define OTS_CLIENT_TASK_PRIORITY        2
#define OTS_CLIENT_COC_MAX_CHUNK        244

enum {
	OTS_CLIENT_CP_IDLE = 0,
	OTS_CLIENT_CP_OACP,
	OTS_CLIENT_CP_OLCP,
};

enum {
	OTS_CLIENT_XFER_IDLE = 0,
	OTS_CLIENT_XFER_READ,     /* receiving object contents from server via CoC */
	OTS_CLIENT_XFER_WRITE,    /* sending object contents to server via CoC */
};

typedef struct {
	uint16_t conn_handle;
	client_disc_state_t disc_state;
	uint16_t start_handle, end_handle;
	/* characteristic value handles (0 = not present) */
	uint16_t feature_handle;
	uint16_t obj_name_handle;
	uint16_t obj_type_handle;
	uint16_t obj_size_handle;
	uint16_t obj_first_created_handle;
	uint16_t obj_last_modified_handle;
	uint16_t obj_id_handle;
	uint16_t obj_prop_handle;
	uint16_t oacp_handle, oacp_cccd_handle;
	uint16_t olcp_handle, olcp_cccd_handle;
	uint16_t obj_list_filter_handle;
	uint16_t obj_changed_handle, obj_changed_cccd_handle;
	/* caches */
	uint32_t oacp_features, olcp_features;
	uint32_t cur_size, alloc_size;
	uint32_t properties;
	/* control point state */
	uint8_t  cp_state;            /* OTS_CLIENT_CP_x */
	uint8_t  cp_timed_out;        /* no new CP op until reconnect (OTP 4.4.4) */
	void    *cp_timer;
	/* object transfer channel */
	uint8_t  coc_open;
	uint16_t coc_cid;
	/* object transfer state */
	uint8_t  xfer_state;          /* OTS_CLIENT_XFER_x */
	uint32_t xfer_offset, xfer_remaining, xfer_sent;
	void    *xfer_timer;
	/* CCCD ownership tracking during descriptor discovery (non-MGR): which
	 * indicate-capable characteristic the next CCCD belongs to (0=none/1=OACP/2=OLCP/3=ObjChanged) */
	uint8_t  cccd_pending;
} ots_client_db_t;

static ots_client_db_t *ots_client_db[RTK_BLE_GAP_MAX_LINKS] = {0};

/* worker task: timer callbacks flag here, task performs the blocking follow-up */
static void   *ots_client_sem = NULL;
static void   *ots_client_task_hdl = NULL;
static uint8_t ots_client_task_running = 0;
static volatile uint8_t ots_cp_to_pending[RTK_BLE_GAP_MAX_LINKS];
static volatile uint8_t ots_xfer_to_pending[RTK_BLE_GAP_MAX_LINKS];
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
static uint8_t ots_client_coc_txbuf[OTS_CLIENT_COC_MAX_CHUNK];
#endif

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
static rtk_bt_gattc_uuid_t ots_srv_uuid = { .is_uuid16 = true, .p.uuid16 = OTS_CLIENT_SRV_UUID };
static rtk_bt_gattc_uuid_t ots_uuid16(uint16_t u)
{
	rtk_bt_gattc_uuid_t x = { .is_uuid16 = true };
	x.p.uuid16 = u;
	return x;
}
#endif

/* --------------------------------- helpers -------------------------------------- */
static ots_client_db_t *ots_client_get_db(uint16_t conn_handle, uint8_t *p_conn_id)
{
	uint8_t conn_id;
	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return NULL;
	}
	if (p_conn_id) {
		*p_conn_id = conn_id;
	}
	return ots_client_db[conn_id];
}

/* generic read by handle */
static uint16_t ots_client_read_handle(uint16_t conn_handle, uint16_t handle)
{
	rtk_bt_gattc_read_param_t read_param = {0};
	if (!handle) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	read_param.profile_id = OTS_CLIENT_PROFILE_ID;
	read_param.conn_handle = conn_handle;
	read_param.type = RTK_BT_GATT_CHAR_READ_BY_HANDLE;
	read_param.by_handle.handle = handle;
	return rtk_bt_gattc_read(&read_param);
}

/* generic write (request) to a value handle */
static uint16_t ots_client_write_handle(uint16_t conn_handle, uint16_t handle,
										uint8_t *data, uint16_t len)
{
	rtk_bt_gattc_write_param_t write_param = {0};
	if (!handle) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	write_param.profile_id = OTS_CLIENT_PROFILE_ID;
	write_param.conn_handle = conn_handle;
	write_param.type = RTK_BT_GATT_CHAR_WRITE_REQ;
	write_param.handle = handle;
	write_param.length = len;
	write_param.data = data;
	return rtk_bt_gattc_write(&write_param);
}

/* --------------------------- control point write -------------------------------- */
uint16_t ots_client_oacp_write(uint16_t conn_handle, uint8_t op, uint8_t *param, uint16_t param_len)
{
	uint8_t conn_id;
	ots_client_db_t *db = ots_client_get_db(conn_handle, &conn_id);
	uint8_t buf[20];

	if (!db || db->disc_state != DISC_DONE) {
		return RTK_BT_ERR_STATE_INVALID;
	}
	if (db->cp_timed_out) {
		BT_LOGE("[APP] OTS client CP timed out, reconnect required before new op\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}
	if (db->cp_state != OTS_CLIENT_CP_IDLE) {
		BT_LOGE("[APP] OTS client CP busy\r\n");
		return RTK_BT_ERR_BUSY;
	}
	if (!db->oacp_handle || ((size_t)param_len + 1) > sizeof(buf)) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	buf[0] = op;
	if (param_len && param) {
		memcpy(&buf[1], param, param_len);
	}
	db->cp_state = OTS_CLIENT_CP_OACP;
	return ots_client_write_handle(conn_handle, db->oacp_handle, buf, 1 + param_len);
}

uint16_t ots_client_olcp_write(uint16_t conn_handle, uint8_t op, uint8_t *param, uint16_t param_len)
{
	uint8_t conn_id;
	ots_client_db_t *db = ots_client_get_db(conn_handle, &conn_id);
	uint8_t buf[8];

	if (!db || db->disc_state != DISC_DONE) {
		return RTK_BT_ERR_STATE_INVALID;
	}
	if (db->cp_timed_out) {
		BT_LOGE("[APP] OTS client CP timed out, reconnect required before new op\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}
	if (db->cp_state != OTS_CLIENT_CP_IDLE) {
		BT_LOGE("[APP] OTS client CP busy\r\n");
		return RTK_BT_ERR_BUSY;
	}
	if (!db->olcp_handle || ((size_t)param_len + 1) > sizeof(buf)) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	buf[0] = op;
	if (param_len && param) {
		memcpy(&buf[1], param, param_len);
	}
	db->cp_state = OTS_CLIENT_CP_OLCP;
	return ots_client_write_handle(conn_handle, db->olcp_handle, buf, 1 + param_len);
}

uint16_t ots_client_olcp_first(uint16_t conn_handle)
{
	return ots_client_olcp_write(conn_handle, OTS_CLIENT_OLCP_OP_FIRST, NULL, 0);
}
uint16_t ots_client_olcp_last(uint16_t conn_handle)
{
	return ots_client_olcp_write(conn_handle, OTS_CLIENT_OLCP_OP_LAST, NULL, 0);
}
uint16_t ots_client_olcp_previous(uint16_t conn_handle)
{
	return ots_client_olcp_write(conn_handle, OTS_CLIENT_OLCP_OP_PREVIOUS, NULL, 0);
}
uint16_t ots_client_olcp_next(uint16_t conn_handle)
{
	return ots_client_olcp_write(conn_handle, OTS_CLIENT_OLCP_OP_NEXT, NULL, 0);
}
uint16_t ots_client_select_by_id(uint16_t conn_handle, uint8_t obj_id[OTS_CLIENT_OBJ_ID_LEN])
{
	return ots_client_olcp_write(conn_handle, OTS_CLIENT_OLCP_OP_GO_TO, obj_id, OTS_CLIENT_OBJ_ID_LEN);
}

static uint16_t ots_client_oacp_abort(uint16_t conn_handle)
{
	return ots_client_oacp_write(conn_handle, OTS_CLIENT_OACP_OP_ABORT, NULL, 0);
}

/* ------------------------------ read APIs --------------------------------------- */
uint16_t ots_client_read_metadata(uint16_t conn_handle, ots_client_meta_type_t type)
{
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);
	uint16_t handle = 0;

	if (!db || db->disc_state != DISC_DONE) {
		return RTK_BT_ERR_STATE_INVALID;
	}
	switch (type) {
	case OTS_CLIENT_META_FEATURE:
		handle = db->feature_handle;
		break;
	case OTS_CLIENT_META_OBJ_NAME:
		handle = db->obj_name_handle;
		break;
	case OTS_CLIENT_META_OBJ_TYPE:
		handle = db->obj_type_handle;
		break;
	case OTS_CLIENT_META_OBJ_SIZE:
		handle = db->obj_size_handle;
		break;
	case OTS_CLIENT_META_OBJ_ID:
		handle = db->obj_id_handle;
		break;
	case OTS_CLIENT_META_OBJ_PROPERTIES:
		handle = db->obj_prop_handle;
		break;
	case OTS_CLIENT_META_OBJ_LIST_FILTER:
		handle = db->obj_list_filter_handle;
		break;
	default:
		break;
	}
	return ots_client_read_handle(conn_handle, handle);
}

uint16_t ots_client_read_long_name(uint16_t conn_handle, uint16_t offset)
{
	(void)offset;
	/* The stack performs GATT Read Long automatically for values exceeding the ATT_MTU
	 * and delivers the full value via consecutive RTK_BT_STATUS_CONTINUE read events. */
	return ots_client_read_metadata(conn_handle, OTS_CLIENT_META_OBJ_NAME);
}

/* ------------------------ object transfer channel (CoC) ------------------------- */
uint16_t ots_client_open_channel(uint16_t conn_handle)
{
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);
	if (!db) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	if (db->coc_open) {
		return RTK_BT_OK;
	}
	return rtk_bt_le_gap_coc_connect(conn_handle, OTS_CLIENT_PSM);
#else
	(void)conn_handle;
	return RTK_BT_ERR_UNSUPPORTED;
#endif
}

uint16_t ots_client_close_channel(uint16_t conn_handle)
{
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);
	if (!db || !db->coc_open) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	return rtk_bt_le_gap_coc_disconnect(db->coc_cid);
#else
	(void)conn_handle;
	return RTK_BT_ERR_UNSUPPORTED;
#endif
}

/* ------------------------------ read/write contents ----------------------------- */
uint16_t ots_client_read_contents(uint16_t conn_handle, uint32_t offset, uint32_t length)
{
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);
	uint8_t param[8];

	if (!db || db->disc_state != DISC_DONE) {
		return RTK_BT_ERR_STATE_INVALID;
	}
	if (!db->coc_open) {
		BT_LOGE("[APP] OTS client object transfer channel not open\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}
	memcpy(&param[0], &offset, sizeof(uint32_t));
	memcpy(&param[4], &length, sizeof(uint32_t));
	db->xfer_state = OTS_CLIENT_XFER_READ;
	db->xfer_offset = offset;
	db->xfer_remaining = length;
	db->xfer_sent = 0;
	return ots_client_oacp_write(conn_handle, OTS_CLIENT_OACP_OP_READ, param, sizeof(param));
}

uint16_t ots_client_write_contents(uint16_t conn_handle, uint32_t offset, uint32_t length, uint8_t mode)
{
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);
	uint8_t param[9];

	if (!db || db->disc_state != DISC_DONE) {
		return RTK_BT_ERR_STATE_INVALID;
	}
	if (!db->coc_open) {
		BT_LOGE("[APP] OTS client object transfer channel not open\r\n");
		return RTK_BT_ERR_STATE_INVALID;
	}
	memcpy(&param[0], &offset, sizeof(uint32_t));
	memcpy(&param[4], &length, sizeof(uint32_t));
	param[8] = mode;
	db->xfer_state = OTS_CLIENT_XFER_WRITE;
	db->xfer_offset = offset;
	db->xfer_remaining = length;
	db->xfer_sent = 0;
	return ots_client_oacp_write(conn_handle, OTS_CLIENT_OACP_OP_WRITE, param, sizeof(param));
}

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
static void ots_client_coc_send_chunk(uint8_t conn_id)
{
	ots_client_db_t *db = ots_client_db[conn_id];
	uint16_t mtu = 0;
	uint32_t chunk, i;

	if (!db || db->xfer_state != OTS_CLIENT_XFER_WRITE || db->xfer_remaining == 0) {
		return;
	}
	if (rtk_bt_le_gap_coc_get_chan_param(RTK_BT_LE_COC_CHAN_PARAM_MTU, db->coc_cid, &mtu) != RTK_BT_OK || mtu == 0) {
		mtu = 23;
	}
	chunk = (mtu < OTS_CLIENT_COC_MAX_CHUNK) ? mtu : OTS_CLIENT_COC_MAX_CHUNK;
	if (chunk > db->xfer_remaining) {
		chunk = db->xfer_remaining;
	}
	/* content sent Little Endian; demo pattern byte = position & 0xFF */
	for (i = 0; i < chunk; i++) {
		ots_client_coc_txbuf[i] = (uint8_t)(db->xfer_offset + db->xfer_sent + i);
	}
	if (rtk_bt_le_gap_coc_send_data(db->coc_cid, (uint16_t)chunk, ots_client_coc_txbuf) != RTK_BT_OK) {
		BT_LOGE("[APP] OTS client coc send data failed\r\n");
		db->xfer_state = OTS_CLIENT_XFER_IDLE;
	}
}
#endif

/* --------------------------------- discovery ------------------------------------ */
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
static uint16_t ots_client_find_char(uint16_t conn_handle, uint16_t char_uuid, uint16_t *p_handle)
{
	rtk_bt_gattc_find_param_t find_param = {0};
	*p_handle = 0;
	find_param.conn_handle = conn_handle;
	find_param.type = RTK_BT_GATT_FIND_CHARACTERISTIC_HANDLE;
	find_param.find_char.srv_uuid = ots_srv_uuid;
	find_param.find_char.char_uuid = ots_uuid16(char_uuid);
	find_param.find_char.p_handle = p_handle;
	return rtk_bt_gattc_find(&find_param);
}

static uint16_t ots_client_find_cccd(uint16_t conn_handle, uint16_t char_uuid, uint16_t *p_handle)
{
	rtk_bt_gattc_find_param_t find_param = {0};
	bool support_notify = false, support_indicate = false;
	*p_handle = 0;
	find_param.conn_handle = conn_handle;
	find_param.type = RTK_BT_GATT_FIND_CHARACTERISTIC_CCCD_HANDLE;
	find_param.find_char_cccd.srv_uuid = ots_srv_uuid;
	find_param.find_char_cccd.char_uuid = ots_uuid16(char_uuid);
	find_param.find_char_cccd.p_handle = p_handle;
	find_param.find_char_cccd.p_notify = &support_notify;
	find_param.find_char_cccd.p_indicate = &support_indicate;
	return rtk_bt_gattc_find(&find_param);
}

static void ots_client_resolve_handles(uint16_t conn_handle, ots_client_db_t *db)
{
	ots_client_find_char(conn_handle, OTS_CLIENT_FEATURE_CHAR_UUID, &db->feature_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_NAME_CHAR_UUID, &db->obj_name_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_TYPE_CHAR_UUID, &db->obj_type_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_SIZE_CHAR_UUID, &db->obj_size_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_FIRST_CREATED_UUID, &db->obj_first_created_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_LAST_MODIFIED_UUID, &db->obj_last_modified_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_ID_CHAR_UUID, &db->obj_id_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_PROPERTIES_UUID, &db->obj_prop_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OACP_CHAR_UUID, &db->oacp_handle);
	ots_client_find_cccd(conn_handle, OTS_CLIENT_OACP_CHAR_UUID, &db->oacp_cccd_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OLCP_CHAR_UUID, &db->olcp_handle);
	ots_client_find_cccd(conn_handle, OTS_CLIENT_OLCP_CHAR_UUID, &db->olcp_cccd_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_LIST_FILTER_UUID, &db->obj_list_filter_handle);
	ots_client_find_char(conn_handle, OTS_CLIENT_OBJ_CHANGED_CHAR_UUID, &db->obj_changed_handle);
	ots_client_find_cccd(conn_handle, OTS_CLIENT_OBJ_CHANGED_CHAR_UUID, &db->obj_changed_cccd_handle);
}

static void ots_client_mgr_enable_indications(uint16_t conn_handle, ots_client_db_t *db)
{
	rtk_bt_gattc_update_cccd_param_t cccd = {0};
	uint16_t ret;
	static const uint16_t indicate_uuids[3] = {
		OTS_CLIENT_OACP_CHAR_UUID,
		OTS_CLIENT_OLCP_CHAR_UUID,
		OTS_CLIENT_OBJ_CHANGED_CHAR_UUID,
	};
	uint8_t i;

	cccd.profile_id = OTS_CLIENT_PROFILE_ID;
	cccd.conn_handle = conn_handle;
	cccd.srv_cfg = false;
	cccd.srv_uuid = ots_srv_uuid;
	cccd.bindicate = true;

	for (i = 0; i < 3; i++) {
		cccd.char_uuid = ots_uuid16(indicate_uuids[i]);
		ret = rtk_bt_gattc_enable_notify_or_indicate(&cccd);
		if (RTK_BT_OK != ret) {
			BT_LOGE("[APP] OTS client enable indication uuid:0x%04x failed, err:0x%x\r\n",
					indicate_uuids[i], ret);
			db->disc_state = DISC_FAILED;
			return;
		}
		BT_LOGA("[APP] OTS client enable indication uuid:0x%04x ok\r\n", indicate_uuids[i]);
	}
}
#else
static uint16_t ots_client_char_discover(uint16_t conn_handle, ots_client_db_t *db)
{
	rtk_bt_gattc_discover_param_t disc_param = {0};
	disc_param.profile_id = OTS_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL;
	disc_param.disc_char_all.start_handle = db->start_handle;
	disc_param.disc_char_all.end_handle = db->end_handle;
	return rtk_bt_gattc_discover(&disc_param);
}

static uint16_t ots_client_desc_discover(uint16_t conn_handle, ots_client_db_t *db)
{
	rtk_bt_gattc_discover_param_t disc_param = {0};
	disc_param.profile_id = OTS_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL;
	disc_param.disc_descriptor.start_handle = db->start_handle;
	disc_param.disc_descriptor.end_handle = db->end_handle;
	return rtk_bt_gattc_discover(&disc_param);
}

static void ots_client_store_char(ots_client_db_t *db, uint16_t uuid, uint16_t value_handle)
{
	switch (uuid) {
	case OTS_CLIENT_FEATURE_CHAR_UUID:
		db->feature_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_NAME_CHAR_UUID:
		db->obj_name_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_TYPE_CHAR_UUID:
		db->obj_type_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_SIZE_CHAR_UUID:
		db->obj_size_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_FIRST_CREATED_UUID:
		db->obj_first_created_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_LAST_MODIFIED_UUID:
		db->obj_last_modified_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_ID_CHAR_UUID:
		db->obj_id_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_PROPERTIES_UUID:
		db->obj_prop_handle = value_handle;
		break;
	case OTS_CLIENT_OACP_CHAR_UUID:
		db->oacp_handle = value_handle;
		break;
	case OTS_CLIENT_OLCP_CHAR_UUID:
		db->olcp_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_LIST_FILTER_UUID:
		db->obj_list_filter_handle = value_handle;
		break;
	case OTS_CLIENT_OBJ_CHANGED_CHAR_UUID:
		db->obj_changed_handle = value_handle;
		break;
	default:
		break;
	}
}

/* Track CCCD ownership while walking the service attributes in handle order.
 * A CCCD belongs to the characteristic declared right before it; we only care about
 * the three indicate-capable ones (OACP/OLCP/Object Changed). Resetting on each
 * characteristic declaration (0x2803) avoids mis-assigning a later characteristic's
 * CCCD (e.g. a vendor char) to Object Changed. */
static void ots_client_track_cccd(ots_client_db_t *db, uint16_t uuid, uint16_t handle)
{
	switch (uuid) {
	case BT_UUID_GATT_CHRC_VAL:            /* 0x2803: characteristic boundary */
		db->cccd_pending = 0;
		break;
	case OTS_CLIENT_OACP_CHAR_UUID:
		db->cccd_pending = 1;
		break;
	case OTS_CLIENT_OLCP_CHAR_UUID:
		db->cccd_pending = 2;
		break;
	case OTS_CLIENT_OBJ_CHANGED_CHAR_UUID:
		db->cccd_pending = 3;
		break;
	case BT_UUID_GATT_CCC_VAL:             /* 0x2902 */
		if (db->cccd_pending == 1) {
			db->oacp_cccd_handle = handle;
		} else if (db->cccd_pending == 2) {
			db->olcp_cccd_handle = handle;
		} else if (db->cccd_pending == 3) {
			db->obj_changed_cccd_handle = handle;
		}
		db->cccd_pending = 0;
		break;
	default:
		break;
	}
}

uint16_t ots_client_srv_discover(uint16_t conn_handle)
{
	uint16_t srv_uuid = OTS_CLIENT_SRV_UUID;
	rtk_bt_gattc_discover_param_t disc_param = {0};
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);

	if (!db) {
		return RTK_BT_ERR_NO_ENTRY;
	}
	disc_param.profile_id = OTS_CLIENT_PROFILE_ID;
	disc_param.conn_handle = conn_handle;
	disc_param.type = RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID;
	disc_param.disc_primary_by_uuid.uuid_type = BT_UUID_TYPE_16;
	memcpy(disc_param.disc_primary_by_uuid.uuid, &srv_uuid, BT_UUID_SIZE_16);
	db->disc_state = DISC_START;
	return rtk_bt_gattc_discover(&disc_param);
}
#endif /* RTK_BLE_MGR_LIB */

/* --------------------------------- event handlers ------------------------------- */
static void ots_client_discover_res_hdl(void *data)
{
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
	ots_client_db_t *db;

	if (!disc_res->is_found) {
		return;
	}
	/* In ble_mgr mode the stack auto-discovers on connect; allocate the per-conn db here. */
	ots_client_attach_conn(disc_res->conn_handle);
	db = ots_client_get_db(disc_res->conn_handle, NULL);
	if (!db) {
		return;
	}
	ots_client_resolve_handles(disc_res->conn_handle, db);
	ots_client_mgr_enable_indications(disc_res->conn_handle, db);
	if (db->disc_state != DISC_FAILED) {
		db->disc_state = DISC_DONE;
		BT_LOGA("[APP] OTS client discover success\r\n");
		BT_LOGA("[APP] OTS cccd handles oacp:0x%04x olcp:0x%04x obj_changed:0x%04x\r\n",
				db->oacp_cccd_handle, db->olcp_cccd_handle, db->obj_changed_cccd_handle);
	}
#else
	uint16_t uuid = 0;
	rtk_bt_gattc_discover_ind_t *disc_res = (rtk_bt_gattc_discover_ind_t *)data;
	uint16_t conn_handle = disc_res->conn_handle;
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);

	if (!db || db->disc_state != DISC_START) {
		return;
	}

	if (RTK_BT_STATUS_CONTINUE == disc_res->status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
			db->start_handle = disc_res->disc_primary_by_uuid_per.start_handle;
			db->end_handle = disc_res->disc_primary_by_uuid_per.end_handle;
			break;
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL:
			memcpy(&uuid, disc_res->disc_char_all_per.uuid, sizeof(uint16_t));
			ots_client_store_char(db, uuid, disc_res->disc_char_all_per.value_handle);
			break;
		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL:
			memcpy(&uuid, disc_res->disc_descriptor_per.uuid, sizeof(uint16_t));
			ots_client_track_cccd(db, uuid, disc_res->disc_descriptor_per.handle);
			break;
		default:
			break;
		}
	} else if (RTK_BT_STATUS_DONE == disc_res->status) {
		switch (disc_res->type) {
		case RTK_BT_GATT_DISCOVER_PRIMARY_BY_UUID:
			if (!db->start_handle && !db->end_handle) {
				db->disc_state = DISC_FAILED;
			} else {
				ots_client_char_discover(conn_handle, db);
			}
			break;
		case RTK_BT_GATT_DISCOVER_CHARACTERISTIC_ALL:
			ots_client_desc_discover(conn_handle, db);
			break;
		case RTK_BT_GATT_DISCOVER_DESCRIPTORS_ALL:
			db->disc_state = DISC_DONE;
			BT_LOGA("[APP] OTS client discover success\r\n");
			BT_LOGA("[APP] OTS cccd handles oacp:0x%04x olcp:0x%04x obj_changed:0x%04x\r\n",
					db->oacp_cccd_handle, db->olcp_cccd_handle, db->obj_changed_cccd_handle);
			break;
		default:
			break;
		}
	} else {
		db->disc_state = DISC_FAILED;
	}

	general_client_discover_res_hdl(data);
#endif /* RTK_BLE_MGR_LIB */
}

static void ots_client_read_res_hdl(void *data)
{
	rtk_bt_gattc_read_ind_t *read_res = (rtk_bt_gattc_read_ind_t *)data;
	ots_client_db_t *db = ots_client_get_db(read_res->conn_handle, NULL);
#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	uint16_t handle = read_res->handle;
	uint8_t *value = read_res->value;
	uint16_t len = read_res->len;
#else
	uint16_t handle = read_res->by_handle.handle;
	uint8_t *value = read_res->by_handle.value;
	uint16_t len = read_res->by_handle.len;
#endif

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	if (!db || RTK_BT_STATUS_DONE != read_res->status || !len || !value) {
		return;
	}
#else
	if (!db || RTK_BT_STATUS_CONTINUE != read_res->status || !len || !value) {
		return;
	}
#endif

	if (handle == db->feature_handle && len >= 8) {
		memcpy(&db->oacp_features, &value[0], sizeof(uint32_t));
		memcpy(&db->olcp_features, &value[4], sizeof(uint32_t));
		BT_LOGA("[APP] OTS client feature oacp:0x%08x olcp:0x%08x\r\n", db->oacp_features, db->olcp_features);
	} else if (handle == db->obj_size_handle && len >= 8) {
		memcpy(&db->cur_size, &value[0], sizeof(uint32_t));
		memcpy(&db->alloc_size, &value[4], sizeof(uint32_t));
		BT_LOGA("[APP] OTS client object size cur:%u alloc:%u\r\n", db->cur_size, db->alloc_size);
	} else if (handle == db->obj_prop_handle && len >= 4) {
		memcpy(&db->properties, value, sizeof(uint32_t));
		BT_LOGA("[APP] OTS client object properties:0x%08x\r\n", db->properties);
	} else if (handle == db->obj_name_handle) {
		/* Object Name is a UTF-8 string (up to 120 octets), not null-terminated on air */
		char name[128] = {0};
		uint16_t cpy = (len < sizeof(name) - 1) ? len : (uint16_t)(sizeof(name) - 1);
		memcpy(name, value, cpy);
		BT_LOGA("[APP] OTS client object name:%s (len %d)\r\n", name, len);
	} else if (handle == db->obj_type_handle) {
		/* Object Type is a 16-bit (2 octets) or 128-bit (16 octets) UUID */
		if (len == 2) {
			uint16_t u16 = 0;
			memcpy(&u16, value, 2);
			BT_LOGA("[APP] OTS client object type (16-bit uuid):0x%04x\r\n", u16);
		} else {
			BT_LOGA("[APP] OTS client object type (128-bit uuid), len %d\r\n", len);
			BT_DUMPA("[APP] OTS client object type: ", value, len);
		}
	} else {
		BT_LOGA("[APP] OTS client read handle 0x%04x len %d\r\n", handle, len);
		BT_DUMPA("[APP] OTS client read value: ", value, len);
	}
	BT_AT_PRINT("+BLEGATTC:read,%u,0x%04x,%u\r\n", read_res->conn_handle, handle, len);
}

static void ots_client_write_res_hdl(void *data)
{
	rtk_bt_gattc_write_ind_t *write_res = (rtk_bt_gattc_write_ind_t *)data;
	ots_client_db_t *db = ots_client_get_db(write_res->conn_handle, NULL);

	if (!db) {
		return;
	}

	/* Control point: ATT Write Response received (OTP 4.4.4).
	 * Arm the 30-second procedure timeout; cp_state stays OACP/OLCP until the
	 * OACP/OLCP Response Code indication arrives (forwarded via
	 * ots_client_forward_gcs_indication from central.c) or the timer fires
	 * (BI-07 procedure-timeout path).  CoC data transfer is triggered inside
	 * ots_client_cp_indication to avoid double-send. */
	if ((write_res->handle == db->oacp_handle && db->cp_state == OTS_CLIENT_CP_OACP) ||
		(write_res->handle == db->olcp_handle && db->cp_state == OTS_CLIENT_CP_OLCP)) {
		if (RTK_BT_STATUS_DONE == write_res->status) {
			/* Arm procedure timeout; stopped in ots_client_cp_indication or on timeout. */
			if (db->cp_timer) {
				osif_timer_restart(&db->cp_timer, OTS_CLIENT_CP_TIMEOUT_MS);
			}
			BT_LOGA("[APP] OTS client CP write ok, awaiting indication\r\n");
		} else {
			db->cp_state = OTS_CLIENT_CP_IDLE;
			db->xfer_state = OTS_CLIENT_XFER_IDLE;
			BT_LOGE("[APP] OTS client CP write failed, err:0x%x (op not started)\r\n", write_res->err_code);
		}
	}
	BT_AT_PRINT("+BLEGATTC:write,%u,0x%04x,%u\r\n",
				write_res->conn_handle, write_res->handle, write_res->status);
}

/* parse an OACP/OLCP Response Code indication: [resp_op, req_op, result, params...] */
static void ots_client_cp_indication(ots_client_db_t *db, uint16_t conn_handle,
									 uint8_t *value, uint16_t len)
{
	uint8_t resp_op, req_op, result;
	(void)conn_handle;

	if (len < 3) {
		return;
	}
	resp_op = value[0];
	req_op = value[1];
	result = value[2];

	if (db->cp_timer) {
		osif_timer_stop(&db->cp_timer);
	}
	db->cp_state = OTS_CLIENT_CP_IDLE;

	if (OTS_CLIENT_OACP_OP_RESP == resp_op) {
		BT_LOGA("[APP] OTS client OACP resp: req_op:0x%x result:0x%x\r\n", req_op, result);
		BT_AT_PRINT("+BLEGATTC:ots_oacp,%u,0x%x,0x%x\r\n", conn_handle, req_op, result);
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
		if (result == OTS_CLIENT_OACP_RES_SUCCESS &&
			req_op == OTS_CLIENT_OACP_OP_READ && db->xfer_state == OTS_CLIENT_XFER_READ) {
			/* object data will arrive on the CoC; arm the stall timeout */
			if (db->xfer_timer) {
				osif_timer_restart(&db->xfer_timer, OTS_CLIENT_XFER_TIMEOUT_MS);
			}
		} else if (result == OTS_CLIENT_OACP_RES_SUCCESS &&
				   req_op == OTS_CLIENT_OACP_OP_WRITE && db->xfer_state == OTS_CLIENT_XFER_WRITE) {
			uint8_t conn_id;
			if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) == RTK_BT_OK) {
				if (db->xfer_timer) {
					osif_timer_restart(&db->xfer_timer, OTS_CLIENT_XFER_TIMEOUT_MS);
				}
				ots_client_coc_send_chunk(conn_id);
			}
		} else if (req_op == OTS_CLIENT_OACP_OP_READ || req_op == OTS_CLIENT_OACP_OP_WRITE) {
			db->xfer_state = OTS_CLIENT_XFER_IDLE;
		}
#endif
	} else if (OTS_CLIENT_OLCP_OP_RESP == resp_op) {
		BT_LOGA("[APP] OTS client OLCP resp: req_op:0x%x result:0x%x\r\n", req_op, result);
		BT_AT_PRINT("+BLEGATTC:ots_olcp,%u,0x%x,0x%x\r\n", conn_handle, req_op, result);
	}
}

static void ots_client_notify_indicate_hdl(void *data)
{
	rtk_bt_gattc_cccd_value_ind_t *ind = (rtk_bt_gattc_cccd_value_ind_t *)data;
	ots_client_db_t *db = ots_client_get_db(ind->conn_handle, NULL);

	if (!db || !ind->len || !ind->value) {
		return;
	}
	if (ind->value_handle == db->oacp_handle || ind->value_handle == db->olcp_handle) {
		ots_client_cp_indication(db, ind->conn_handle, ind->value, ind->len);
	} else if (ind->value_handle == db->obj_changed_handle) {
		BT_LOGA("[APP] OTS client Object Changed, flags:0x%x\r\n", ind->value[0]);
		BT_AT_PRINT("+BLEGATTC:ots_obj_changed,%u,0x%x\r\n", ind->conn_handle, ind->value[0]);
	}
}

/* Forward an OACP/OLCP/ObjChanged indication that arrived on the GCS profile
 * (profile_id 0) because CCCD was enabled via bare ATT write rather than through
 * the OTS client profile.  Call this from the GCS indication branch in central.c
 * so that cp_timer is stopped and CoC data transfer is started correctly. */
void ots_client_forward_gcs_indication(uint16_t conn_handle, uint16_t value_handle,
									   uint8_t *value, uint16_t len)
{
	ots_client_db_t *db = ots_client_get_db(conn_handle, NULL);
	if (!db || !value || !len) {
		return;
	}
	if (value_handle == db->oacp_handle || value_handle == db->olcp_handle) {
		ots_client_cp_indication(db, conn_handle, value, len);
	} else if (db->obj_changed_handle && value_handle == db->obj_changed_handle) {
		BT_LOGA("[APP] OTS client Object Changed (via GCS), flags:0x%x\r\n", value[0]);
		BT_AT_PRINT("+BLEGATTC:ots_obj_changed,%u,0x%x\r\n", conn_handle, value[0]);
	}
}

static void ots_client_cccd_update_hdl(void *data, bool enable)
{
	rtk_bt_gattc_cccd_update_ind_t *cccd_update = (rtk_bt_gattc_cccd_update_ind_t *)data;
	if (RTK_BT_STATUS_DONE == cccd_update->status) {
		BT_LOGA("[APP] OTS client cccd %s success\r\n", enable ? "enable" : "disable");
	} else {
		BT_LOGE("[APP] OTS client cccd %s failed, err:0x%x\r\n", enable ? "enable" : "disable", cccd_update->err_code);
	}
	BT_AT_PRINT("+BLEGATTC:ots_cccd,%d,%u\r\n", enable ? 1 : 0, cccd_update->conn_handle);
}

rtk_bt_evt_cb_ret_t ots_client_app_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND:
		ots_client_discover_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_READ_RESULT_IND:
		ots_client_read_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_WRITE_RESULT_IND:
		ots_client_write_res_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_NOTIFY_IND:
	case RTK_BT_GATTC_EVT_INDICATE_IND:
		ots_client_notify_indicate_hdl(data);
		break;
	case RTK_BT_GATTC_EVT_CCCD_ENABLE_IND:
		ots_client_cccd_update_hdl(data, true);
		break;
	case RTK_BT_GATTC_EVT_CCCD_DISABLE_IND:
		ots_client_cccd_update_hdl(data, false);
		break;
	default:
		break;
	}
	return RTK_BT_EVT_CB_OK;
}

/* ------------------------------ CoC event hooks --------------------------------- */
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
void ots_client_coc_connect_hdl(void *data)
{
	rtk_bt_le_coc_conn_state_ind_t *ind = (rtk_bt_le_coc_conn_state_ind_t *)data;
	ots_client_db_t *db = ots_client_get_db(ind->conn_handle, NULL);
	if (!db || ind->err) {
		return;
	}
	db->coc_open = 1;
	db->coc_cid = ind->cid;
	BT_LOGA("[APP] OTS client object transfer channel opened, cid:0x%x\r\n", ind->cid);
}

void ots_client_coc_disconnect_hdl(void *data)
{
	rtk_bt_le_coc_conn_state_ind_t *ind = (rtk_bt_le_coc_conn_state_ind_t *)data;
	ots_client_db_t *db = ots_client_get_db(ind->conn_handle, NULL);
	if (!db || db->coc_cid != ind->cid) {
		return;
	}
	if (db->xfer_timer) {
		osif_timer_stop(&db->xfer_timer);
	}
	db->coc_open = 0;
	db->coc_cid = 0;
	db->xfer_state = OTS_CLIENT_XFER_IDLE;
	BT_LOGA("[APP] OTS client object transfer channel closed\r\n");
}

void ots_client_coc_recv_data_hdl(void *data)
{
	rtk_bt_le_coc_receive_data_ind_t *ind = (rtk_bt_le_coc_receive_data_ind_t *)data;
	ots_client_db_t *db = ots_client_get_db(ind->conn_handle, NULL);

	if (!db || db->coc_cid != ind->cid || db->xfer_state != OTS_CLIENT_XFER_READ) {
		return;
	}
	if ((uint32_t)ind->len >= db->xfer_remaining) {
		db->xfer_remaining = 0;
	} else {
		db->xfer_remaining -= ind->len;
	}
	db->xfer_sent += ind->len;
	BT_LOGA("[APP] OTS client read contents recv %d, remaining %u\r\n", ind->len, db->xfer_remaining);

	if (db->xfer_remaining == 0) {
		if (db->xfer_timer) {
			osif_timer_stop(&db->xfer_timer);
		}
		db->xfer_state = OTS_CLIENT_XFER_IDLE;
		BT_LOGA("[APP] OTS client read contents complete, total %u\r\n", db->xfer_sent);
		BT_AT_PRINT("+BLEGATTC:ots_read_done,%u,%u\r\n", ind->conn_handle, db->xfer_sent);
	} else if (db->xfer_timer) {
		osif_timer_restart(&db->xfer_timer, OTS_CLIENT_XFER_TIMEOUT_MS);
	}
}

void ots_client_coc_send_result_hdl(void *data)
{
	rtk_bt_le_coc_send_data_res_ind_t *ind = (rtk_bt_le_coc_send_data_res_ind_t *)data;
	uint8_t conn_id;
	ots_client_db_t *db = ots_client_get_db(ind->conn_handle, &conn_id);
	uint16_t mtu = 0;
	uint32_t chunk;

	if (!db || db->coc_cid != ind->cid || db->xfer_state != OTS_CLIENT_XFER_WRITE) {
		return;
	}
	if (ind->err) {
		db->xfer_state = OTS_CLIENT_XFER_IDLE;
		if (db->xfer_timer) {
			osif_timer_stop(&db->xfer_timer);
		}
		return;
	}
	/* advance by the chunk just sent */
	if (rtk_bt_le_gap_coc_get_chan_param(RTK_BT_LE_COC_CHAN_PARAM_MTU, db->coc_cid, &mtu) != RTK_BT_OK || mtu == 0) {
		mtu = 23;
	}
	chunk = (mtu < OTS_CLIENT_COC_MAX_CHUNK) ? mtu : OTS_CLIENT_COC_MAX_CHUNK;
	if (chunk > db->xfer_remaining) {
		chunk = db->xfer_remaining;
	}
	db->xfer_sent += chunk;
	db->xfer_remaining -= chunk;
	if (db->xfer_remaining == 0) {
		if (db->xfer_timer) {
			osif_timer_stop(&db->xfer_timer);
		}
		db->xfer_state = OTS_CLIENT_XFER_IDLE;
		BT_LOGA("[APP] OTS client write contents complete, total %u\r\n", db->xfer_sent);
		BT_AT_PRINT("+BLEGATTC:ots_write_done,%u,%u\r\n", ind->conn_handle, db->xfer_sent);
	} else {
		if (db->xfer_timer) {
			osif_timer_restart(&db->xfer_timer, OTS_CLIENT_XFER_TIMEOUT_MS);
		}
		ots_client_coc_send_chunk(conn_id);
	}
}
#endif /* RTK_BLE_COC_SUPPORT */

/* ------------------------------- timeout timers --------------------------------- */
static void ots_client_cp_timeout(void *timer_handle)
{
	uint32_t timer_id;
	if (!osif_timer_id_get(&timer_handle, &timer_id)) {
		return;
	}
	if (timer_id < RTK_BLE_GAP_MAX_LINKS) {
		ots_cp_to_pending[timer_id] = 1;
		if (ots_client_sem) {
			osif_sem_give(ots_client_sem);
		}
	}
}

static void ots_client_xfer_timeout(void *timer_handle)
{
	uint32_t timer_id;
	if (!osif_timer_id_get(&timer_handle, &timer_id)) {
		return;
	}
	if (timer_id >= RTK_BLE_GAP_MAX_LINKS && timer_id < 2 * RTK_BLE_GAP_MAX_LINKS) {
		ots_xfer_to_pending[timer_id - RTK_BLE_GAP_MAX_LINKS] = 1;
		if (ots_client_sem) {
			osif_sem_give(ots_client_sem);
		}
	}
}

static void ots_client_task_entry(void *param)
{
	(void)param;
	uint8_t i;
	uint16_t conn_handle;

	while (ots_client_task_running) {
		if (!osif_sem_take(ots_client_sem, BT_TIMEOUT_FOREVER)) {
			continue;
		}
		if (!ots_client_task_running) {
			break;
		}
		for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
			ots_client_db_t *db = ots_client_db[i];
			if (!db) {
				ots_cp_to_pending[i] = 0;
				ots_xfer_to_pending[i] = 0;
				continue;
			}
			if (ots_cp_to_pending[i]) {
				ots_cp_to_pending[i] = 0;
				/* OTP 4.4.4: operation failed; no new CP op until reconnect */
				db->cp_state = OTS_CLIENT_CP_IDLE;
				db->cp_timed_out = 1;
				BT_LOGE("[APP] OTS client control point timeout (conn_id %d)\r\n", i);
			}
			if (ots_xfer_to_pending[i]) {
				ots_xfer_to_pending[i] = 0;
				if (db->xfer_state == OTS_CLIENT_XFER_READ) {
					/* stalled read: abort so the server also aborts (OTP 4.5.4.2) */
					BT_LOGE("[APP] OTS client read stall timeout, sending OACP Abort\r\n");
					db->xfer_state = OTS_CLIENT_XFER_IDLE;
					conn_handle = db->conn_handle;
					ots_client_oacp_abort(conn_handle);
				} else if (db->xfer_state == OTS_CLIENT_XFER_WRITE) {
					BT_LOGE("[APP] OTS client write stall timeout\r\n");
					db->xfer_state = OTS_CLIENT_XFER_IDLE;
				}
			}
		}
	}
	osif_task_delete(NULL);
}

/* ------------------------------- attach / add ----------------------------------- */
uint16_t ots_client_attach_conn(uint16_t conn_handle)
{
	uint8_t conn_id;
	ots_client_db_t *db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}
	db = (ots_client_db_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(ots_client_db_t));
	if (!db) {
		return RTK_BT_FAIL;
	}
	memset(db, 0, sizeof(ots_client_db_t));
	db->conn_handle = conn_handle;
	ots_client_db[conn_id] = db;

	osif_timer_create(&db->cp_timer, "ots_cli_cp", conn_id,
					  OTS_CLIENT_CP_TIMEOUT_MS, false, ots_client_cp_timeout);
	osif_timer_create(&db->xfer_timer, "ots_cli_xfer", RTK_BLE_GAP_MAX_LINKS + conn_id,
					  OTS_CLIENT_XFER_TIMEOUT_MS, false, ots_client_xfer_timeout);
	return RTK_BT_OK;
}

uint16_t ots_client_detach_conn(uint16_t conn_handle)
{
	uint8_t conn_id;
	ots_client_db_t *db;

	if (rtk_bt_le_gap_get_conn_id(conn_handle, &conn_id) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}
	db = ots_client_db[conn_id];
	if (db) {
		if (db->cp_timer) {
			osif_timer_delete(&db->cp_timer);
		}
		if (db->xfer_timer) {
			osif_timer_delete(&db->xfer_timer);
		}
		osif_mem_free(db);
		ots_client_db[conn_id] = NULL;
	}
	ots_cp_to_pending[conn_id] = 0;
	ots_xfer_to_pending[conn_id] = 0;
	return RTK_BT_OK;
}

uint16_t ots_client_add(void)
{
	/* TODO: temporary debug log, remove after RTK_BLE_MGR_LIB mode verification */
	BT_LOGA("[APP] OTS client RTK_BLE_MGR_LIB = %d\r\n", RTK_BLE_MGR_LIB);

	memset((void *)ots_cp_to_pending, 0, sizeof(ots_cp_to_pending));
	memset((void *)ots_xfer_to_pending, 0, sizeof(ots_xfer_to_pending));

	if (ots_client_sem == NULL) {
		osif_sem_create(&ots_client_sem, 0, 1);
	}
	if (ots_client_task_hdl == NULL) {
		ots_client_task_running = 1;
		osif_task_create(&ots_client_task_hdl, "ots_client_task", ots_client_task_entry, NULL,
						 OTS_CLIENT_TASK_STACK_SIZE, OTS_CLIENT_TASK_PRIORITY);
	}

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	/* register the object transfer channel PSM before initiating any CoC connection */
	rtk_bt_le_gap_coc_register_psm(1, OTS_CLIENT_PSM);
	rtk_bt_le_gap_coc_set_param(RTK_BT_LE_COC_PARAM_LOCAL_MTU, OTS_CLIENT_COC_LOCAL_MTU);
#endif

#if defined(RTK_BLE_MGR_LIB) && RTK_BLE_MGR_LIB
	return rtk_bt_gattc_register_profile(OTS_CLIENT_PROFILE_ID, ots_srv_uuid);
#else
	return rtk_bt_gattc_register_profile(OTS_CLIENT_PROFILE_ID);
#endif
}

uint16_t ots_client_delete(void)
{
	uint8_t i;

	for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
		if (ots_client_db[i]) {
			if (ots_client_db[i]->cp_timer) {
				osif_timer_delete(&ots_client_db[i]->cp_timer);
			}
			if (ots_client_db[i]->xfer_timer) {
				osif_timer_delete(&ots_client_db[i]->xfer_timer);
			}
			osif_mem_free(ots_client_db[i]);
			ots_client_db[i] = NULL;
		}
	}

	if (ots_client_task_hdl) {
		ots_client_task_running = 0;
		if (ots_client_sem) {
			osif_sem_give(ots_client_sem);
		}
		ots_client_task_hdl = NULL;
	}

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	rtk_bt_le_gap_coc_register_psm(0, OTS_CLIENT_PSM);
#endif

	return RTK_BT_OK;
}
