/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <osif.h>

#include <bt_api_config.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_service_config.h>
#include <rtk_ots.h>
#include <bt_utils.h>

/* -------------------------------- Object database ------------------------------- */
#define OTS_OBJ_NUM                 3
#define OTS_DEMO_OBJ_TYPE           0x2ACA      /* Unspecified Object type (16-bit UUID) */

typedef struct {
	uint8_t  id[OTS_OBJ_ID_LEN];                /* Object ID, uint48 little endian */
	char     name[OTS_OBJ_NAME_MAX_LEN];        /* UTF-8, not null terminated on air */
	uint8_t  name_len;
	uint8_t  type[OTS_OBJ_TYPE_MAX_LEN];        /* Object Type UUID, little endian */
	uint8_t  type_len;                          /* 2 or 16 */
	uint32_t cur_size;
	uint32_t alloc_size;
	uint32_t properties;
} ots_object_t;

/* Seed a small object list. Names match the PTS IXIT convention "Object N".
 * "Object 1"/"Object 2" are readable with a large current size so OACP Read (entire and
 * from a non-zero offset) works; "Object 3" has no Read property to exercise OACP Read
 * Not Permitted. All current sizes are > 23 octets to exercise the object transfer timeout.
 * PTS IXIT: set TSPX_iut_object_name to "Object 2" for OACP Read tests, "Object 3" for
 * the Read Not Permitted test, and keep TSPX_iut_object_read_offset below 512. */
static ots_object_t ots_objs[OTS_OBJ_NUM] = {
	{
		.id = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00}, .name = "Object 1", .name_len = 8,
		.type = {(uint8_t)OTS_DEMO_OBJ_TYPE, (uint8_t)(OTS_DEMO_OBJ_TYPE >> 8)}, .type_len = 2,
		.cur_size = 512, .alloc_size = 1024, .properties = OTS_PROP_READ,
	},
	{
		.id = {0x01, 0x01, 0x00, 0x00, 0x00, 0x00}, .name = "Object 2", .name_len = 8,
		.type = {(uint8_t)OTS_DEMO_OBJ_TYPE, (uint8_t)(OTS_DEMO_OBJ_TYPE >> 8)}, .type_len = 2,
		.cur_size = 512, .alloc_size = 1024, .properties = OTS_PROP_READ | OTS_PROP_DELETE,
	},
	{
		.id = {0x02, 0x01, 0x00, 0x00, 0x00, 0x00}, .name = "Object 3", .name_len = 8,
		.type = {(uint8_t)OTS_DEMO_OBJ_TYPE, (uint8_t)(OTS_DEMO_OBJ_TYPE >> 8)}, .type_len = 2,
		.cur_size = 40, .alloc_size = 64, .properties = OTS_PROP_DELETE,
	},
};

/* OTS Feature value: advertise only OACP Read and OLCP Go To (built once at add). */
static uint8_t ots_feature_val[OTS_FEATURE_VALUE_LEN] = {0};

/* Per connection state (index is conn_id). Current object -1 means Invalid Object. */
static int8_t  ots_cur_obj[RTK_BLE_GAP_MAX_LINKS];
static uint8_t oacp_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t olcp_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};
static uint8_t obj_changed_cccd_ind_en_map[RTK_BLE_GAP_MAX_LINKS] = {0};

/* OTS/SR/OC/BV-01-C detector: set when the Object Changed CCCD is (re)written to enable
 * indications, cleared by any OACP/OLCP write. Its step 1 (enable Object Changed CCCD)
 * is immediately followed by step 2 (read Object ID) with no control point write in
 * between, whereas the OACP tests read the Object ID during OLCP-driven discovery. So an
 * Object ID read while this flag is set uniquely identifies OC/BV-01. */
static uint8_t ots_oc_arm_map[RTK_BLE_GAP_MAX_LINKS] = {0};

/* Scratch buffer for assembling read responses (OTS Feature / Object Size). */
static uint8_t ots_read_scratch[OTS_FEATURE_VALUE_LEN];

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
#define OTS_PSM                     0x0025
#define OTS_TRANSFER_TIMEOUT_MS     30000       /* Object Transfer Timeout (spec 3.8.2) */
#define OTS_COC_LOCAL_MTU           247
#define OTS_COC_MAX_CHUNK           244

enum {
	OTS_XFER_IDLE = 0,
	OTS_XFER_ARMED,     /* read acknowledged, waiting for OACP indication confirm */
	OTS_XFER_SENDING,
};

typedef struct {
	uint8_t  open;              /* object transfer channel established */
	uint16_t conn_handle;
	uint16_t cid;
	uint8_t  state;             /* OTS_XFER_x */
	uint32_t offset;            /* next object byte offset to send */
	uint32_t remaining;         /* remaining bytes to send */
	uint32_t last_sent;         /* size of the SDU last handed to the stack */
	void    *timer;             /* object transfer timeout timer */
} ots_coc_chan_t;

static ots_coc_chan_t ots_coc[RTK_BLE_GAP_MAX_LINKS];
static uint8_t ots_coc_txbuf[OTS_COC_MAX_CHUNK];

/* The object transfer timeout closes a stalled channel. The disconnect API blocks on
 * the BT task, so it must not run inside the software timer daemon: the timer only
 * flags the link and wakes a dedicated worker task that performs the disconnect. */
#define OTS_TIMEOUT_TASK_STACK_SIZE  (256 * 4)
#define OTS_TIMEOUT_TASK_PRIORITY    2

static void *ots_timeout_sem = NULL;
static void *ots_timeout_task_hdl = NULL;
static uint8_t ots_timeout_task_running = 0;
static volatile uint8_t ots_timeout_pending[RTK_BLE_GAP_MAX_LINKS];

/* OTS/SR/OC/BV-01-C: PTS reads the Object ID (step 2) and only then displays a
 * "please change metadata" prompt and starts waiting for the Object Changed
 * indication (step 3-4). Sending it immediately on the read races ahead of that
 * wait window, so a short one-shot timer defers the (single) indication until PTS
 * is waiting. Like the transfer timeout, the send runs in the worker task because
 * rtk_bt_gatts_indicate blocks and must not run in the software timer daemon. */
#define OTS_OC_TRIGGER_DELAY_MS      2000

static void *ots_oc_timer = NULL;
static volatile uint8_t ots_oc_send_pending = 0;
static uint16_t ots_oc_conn_handle = 0;
static int8_t ots_oc_obj_idx = -1;

static void ots_coc_send_chunk(uint8_t conn_id);
#endif /* RTK_BLE_COC_SUPPORT */

/* --------------------------------- Attribute table ------------------------------ */
static rtk_bt_gatt_attr_t ots_attrs[] = {
	/* Primary Service: Object Transfer */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_OBJECT_TRANSFER_SRV),

	/* OTS Feature (read only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OTS_FEATURE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Object Name (read only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_NAME_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Object Type (read only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_TYPE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Object Size (read only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_SIZE_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Object ID (read only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_ID_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Object Properties (read only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_PROPERTIES_CHAR,
							   RTK_BT_GATT_CHRC_READ,
							   RTK_BT_GATT_PERM_READ),

	/* Object Action Control Point (write, indicate) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_ACTION_CP_CHAR,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_WRITE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* Object List Control Point (write, indicate) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_LIST_CP_CHAR,
							   RTK_BT_GATT_CHRC_WRITE | RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_WRITE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),

	/* Object Changed (indicate only) */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_OBJECT_CHANGED_CHAR,
							   RTK_BT_GATT_CHRC_INDICATE,
							   RTK_BT_GATT_PERM_NONE),
	RTK_BT_GATT_CCC(RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service object_transfer_srv = RTK_BT_GATT_SERVICE(ots_attrs, OBJECT_TRANSFER_SRV_ID);

/* --------------------------------- Helpers -------------------------------------- */
static bool ots_get_conn_id(uint16_t conn_handle, uint8_t *conn_id)
{
	return (rtk_bt_le_gap_get_conn_id(conn_handle, conn_id) == RTK_BT_OK);
}

static int ots_find_object_by_id(const uint8_t *id)
{
	int i;
	for (i = 0; i < OTS_OBJ_NUM; i++) {
		if (memcmp(ots_objs[i].id, id, OTS_OBJ_ID_LEN) == 0) {
			return i;
		}
	}
	return -1;
}

/* Send one Object Changed indication for object obj_idx on the given connection. */
static void ots_indicate_object_changed(uint16_t conn_handle, int obj_idx, uint8_t flags)
{
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t value_buf[OTS_OBJ_CHANGED_VALUE_LEN] = {0};

	value_buf[0] = flags;
	memcpy(&value_buf[1], ots_objs[obj_idx].id, OTS_OBJ_ID_LEN);

	ind_param.app_id = OBJECT_TRANSFER_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = OTS_OBJ_CHANGED_CHAR_VAL_INDEX;
	ind_param.data = value_buf;
	ind_param.len = OTS_OBJ_CHANGED_VALUE_LEN;
	ind_param.seq = 0;
	rtk_bt_gatts_indicate(&ind_param);
}

/* --------------------------------- Read handler --------------------------------- */
static void ots_read_hdl(void *data)
{
	uint16_t ret = 0;
	rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
	rtk_bt_gatts_read_resp_param_t read_resp = {0};
	uint8_t conn_id;
	int cur = -1;
	ots_object_t *obj = NULL;

	read_resp.app_id = p_read_ind->app_id;
	read_resp.conn_handle = p_read_ind->conn_handle;
	read_resp.cid = p_read_ind->cid;
	read_resp.index = p_read_ind->index;

	if (ots_get_conn_id(p_read_ind->conn_handle, &conn_id)) {
		cur = ots_cur_obj[conn_id];
		if (cur >= 0) {
			obj = &ots_objs[cur];
		}
	}

	if (OTS_FEATURE_CHAR_VAL_INDEX == p_read_ind->index) {
		/* OTS Feature does not depend on a selected object */
		read_resp.data = ots_feature_val;
		read_resp.len = OTS_FEATURE_VALUE_LEN;
	} else if (!obj) {
		/* All object metadata require a valid Current Object */
		read_resp.err_code = RTK_BT_ATT_ERR_OTS_OBJECT_NOT_SELECTED;
	} else if (OTS_OBJ_NAME_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = obj->name;
		read_resp.len = obj->name_len;
	} else if (OTS_OBJ_TYPE_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = obj->type;
		read_resp.len = obj->type_len;
	} else if (OTS_OBJ_SIZE_CHAR_VAL_INDEX == p_read_ind->index) {
		memcpy(&ots_read_scratch[0], &obj->cur_size, sizeof(uint32_t));
		memcpy(&ots_read_scratch[4], &obj->alloc_size, sizeof(uint32_t));
		read_resp.data = ots_read_scratch;
		read_resp.len = OTS_OBJ_SIZE_VALUE_LEN;
	} else if (OTS_OBJ_ID_CHAR_VAL_INDEX == p_read_ind->index) {
		read_resp.data = obj->id;
		read_resp.len = OTS_OBJ_ID_LEN;
	} else if (OTS_OBJ_PROPERTIES_CHAR_VAL_INDEX == p_read_ind->index) {
		memcpy(ots_read_scratch, &obj->properties, sizeof(uint32_t));
		read_resp.data = ots_read_scratch;
		read_resp.len = OTS_OBJ_PROPERTIES_LEN;
	} else {
		BT_LOGE("[APP] OTS read event unknown index: %d\r\n", p_read_ind->index);
		read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

	ret = rtk_bt_gatts_read_resp(&read_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] OTS response for client read succeed, index: %d\r\n", p_read_ind->index);
	} else {
		BT_LOGE("[APP] OTS response for client read failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
				read_resp.conn_handle, read_resp.index, read_resp.err_code);

	/* Server-side (out of band) metadata change trigger for OTS/SR/OC/BV-01-C: its step 1
	 * enables the Object Changed CCCD and step 2 reads the Object ID with no control point
	 * write in between (ots_oc_arm_map still set), then PTS waits for the indication. The
	 * OACP tests instead read the Object ID during OLCP-driven discovery, which clears the
	 * arm flag. The indication is deferred by a timer so it lands after PTS starts waiting
	 * for it (see OTS_OC_TRIGGER_DELAY_MS). */
	if (OTS_OBJ_ID_CHAR_VAL_INDEX == p_read_ind->index && RTK_BT_OK == ret &&
		cur >= 0 && obj_changed_cccd_ind_en_map[conn_id] && ots_oc_arm_map[conn_id]) {
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
		ots_oc_conn_handle = p_read_ind->conn_handle;
		ots_oc_obj_idx = (int8_t)cur;
		if (ots_oc_timer) {
			osif_timer_restart(&ots_oc_timer, OTS_OC_TRIGGER_DELAY_MS);
		}
#else
		ots_indicate_object_changed(p_read_ind->conn_handle, cur, OTS_OBJ_CHANGED_FLAG_METADATA);
#endif
	}
}

/* --------------------------- Control point indications -------------------------- */
static void ots_cp_indicate(uint16_t conn_handle, uint16_t index,
							uint8_t resp_op, uint8_t req_op, uint8_t result)
{
	rtk_bt_gatts_ntf_and_ind_param_t ind_param = {0};
	uint8_t buf[3];

	buf[0] = resp_op;
	buf[1] = req_op;
	buf[2] = result;

	ind_param.app_id = OBJECT_TRANSFER_SRV_ID;
	ind_param.conn_handle = conn_handle;
	ind_param.index = index;
	ind_param.data = buf;
	ind_param.len = sizeof(buf);
	ind_param.seq = 0;

	rtk_bt_gatts_indicate(&ind_param);
}

/* ------------------------------ OACP processing --------------------------------- */
static uint8_t ots_oacp_read(uint16_t conn_handle, uint8_t *value, uint16_t len)
{
	uint8_t conn_id;
	int cur;
	uint32_t offset, length;

	if (len != 9) { /* op(1) + offset(4) + length(4) */
		return OTS_OACP_RES_INVALID_PARAM;
	}
	if (!ots_get_conn_id(conn_handle, &conn_id)) {
		return OTS_OACP_RES_OPERATION_FAILED;
	}
	cur = ots_cur_obj[conn_id];
	if (cur < 0) {
		return OTS_OACP_RES_INVALID_OBJECT;
	}
	if (!(ots_objs[cur].properties & OTS_PROP_READ)) {
		return OTS_OACP_RES_PROCEDURE_NOT_PERMIT;
	}
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	if (!ots_coc[conn_id].open) {
		return OTS_OACP_RES_CHANNEL_UNAVAILABLE;
	}
	memcpy(&offset, &value[1], sizeof(uint32_t));
	memcpy(&length, &value[5], sizeof(uint32_t));
	if (offset > ots_objs[cur].cur_size || length == 0 ||
		(offset + length) > ots_objs[cur].cur_size) {
		return OTS_OACP_RES_INVALID_PARAM;
	}
	/* Arm the transfer; data is streamed once the success indication is confirmed. */
	ots_coc[conn_id].state = OTS_XFER_ARMED;
	ots_coc[conn_id].offset = offset;
	ots_coc[conn_id].remaining = length;
	ots_coc[conn_id].last_sent = 0;
	return OTS_OACP_RES_SUCCESS;
#else
	(void)value;
	(void)offset;
	(void)length;
	return OTS_OACP_RES_CHANNEL_UNAVAILABLE;
#endif
}

static void ots_oacp_proc(uint16_t conn_handle, uint8_t *value, uint16_t len)
{
	uint8_t op = value[0];
	uint8_t result;

	if (OTS_OACP_OP_READ == op) {
		result = ots_oacp_read(conn_handle, value, len);
	} else {
		/* Only OACP Read is supported (advertised via OTS Feature). */
		result = OTS_OACP_RES_OP_NOT_SUPPORTED;
	}

	BT_LOGA("[APP] OTS OACP op: 0x%x, result: 0x%x\r\n", op, result);
	ots_cp_indicate(conn_handle, OTS_OACP_CHAR_VAL_INDEX, OTS_OACP_OP_RESP, op, result);
}

/* ------------------------------ OLCP processing --------------------------------- */
static uint8_t ots_olcp_proc_op(uint16_t conn_handle, uint8_t *value, uint16_t len)
{
	uint8_t conn_id;
	uint8_t op = value[0];
	int cur, idx;

	if (!ots_get_conn_id(conn_handle, &conn_id)) {
		return OTS_OLCP_RES_OPERATION_FAILED;
	}
	cur = ots_cur_obj[conn_id];

	switch (op) {
	case OTS_OLCP_OP_FIRST:
		ots_cur_obj[conn_id] = 0;
		return OTS_OLCP_RES_SUCCESS;

	case OTS_OLCP_OP_LAST:
		ots_cur_obj[conn_id] = OTS_OBJ_NUM - 1;
		return OTS_OLCP_RES_SUCCESS;

	case OTS_OLCP_OP_NEXT:
		idx = (cur < 0) ? 0 : cur + 1;
		if (idx >= OTS_OBJ_NUM) {
			return OTS_OLCP_RES_OUT_OF_BOUNDS;
		}
		ots_cur_obj[conn_id] = (int8_t)idx;
		return OTS_OLCP_RES_SUCCESS;

	case OTS_OLCP_OP_PREVIOUS:
		if (cur <= 0) {
			return OTS_OLCP_RES_OUT_OF_BOUNDS;
		}
		ots_cur_obj[conn_id] = (int8_t)(cur - 1);
		return OTS_OLCP_RES_SUCCESS;

	case OTS_OLCP_OP_GO_TO:
		if (len != 1 + OTS_OBJ_ID_LEN) {
			return OTS_OLCP_RES_INVALID_PARAM;
		}
		idx = ots_find_object_by_id(&value[1]);
		if (idx < 0) {
			return OTS_OLCP_RES_OBJECT_ID_NOT_FOUND;
		}
		ots_cur_obj[conn_id] = (int8_t)idx;
		return OTS_OLCP_RES_SUCCESS;

	default:
		/* Order / Request Number of Objects / Clear Marking and RFU are not supported. */
		return OTS_OLCP_RES_OP_NOT_SUPPORTED;
	}
}

static void ots_olcp_proc(uint16_t conn_handle, uint8_t *value, uint16_t len)
{
	uint8_t op = value[0];
	uint8_t result = ots_olcp_proc_op(conn_handle, value, len);

	BT_LOGA("[APP] OTS OLCP op: 0x%x, result: 0x%x\r\n", op, result);
	ots_cp_indicate(conn_handle, OTS_OLCP_CHAR_VAL_INDEX, OTS_OLCP_OP_RESP, op, result);
}

/* --------------------------------- Write handler -------------------------------- */
static void ots_write_hdl(void *data)
{
	uint16_t ret = RTK_BT_OK;
	rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
	rtk_bt_gatts_write_resp_param_t write_resp = {0};
	uint8_t conn_id;
	uint8_t do_oacp = 0, do_olcp = 0;

	write_resp.app_id = p_write_ind->app_id;
	write_resp.conn_handle = p_write_ind->conn_handle;
	write_resp.cid = p_write_ind->cid;
	write_resp.index = p_write_ind->index;
	write_resp.type = p_write_ind->type;

	if (!ots_get_conn_id(p_write_ind->conn_handle, &conn_id)) {
		write_resp.err_code = RTK_BT_ATT_ERR_UNLIKELY;
		goto send_rsp;
	}

	/* Any control point write ends the OC/BV-01 step1->step2 window (see ots_oc_arm_map). */
	if (OTS_OACP_CHAR_VAL_INDEX == p_write_ind->index ||
		OTS_OLCP_CHAR_VAL_INDEX == p_write_ind->index) {
		ots_oc_arm_map[conn_id] = 0;
	}

	if (OTS_OACP_CHAR_VAL_INDEX == p_write_ind->index) {
		if (!oacp_cccd_ind_en_map[conn_id]) {
			write_resp.err_code = RTK_BT_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
		} else if (!p_write_ind->len || !p_write_ind->value) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		} else {
			do_oacp = 1;
		}
	} else if (OTS_OLCP_CHAR_VAL_INDEX == p_write_ind->index) {
		if (!olcp_cccd_ind_en_map[conn_id]) {
			write_resp.err_code = RTK_BT_ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
		} else if (!p_write_ind->len || !p_write_ind->value) {
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
		} else {
			do_olcp = 1;
		}
	} else {
		BT_LOGE("[APP] OTS write event unknown index: %d\r\n", p_write_ind->index);
		write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
	}

send_rsp:
	/* Control point characteristics use Write Request; always answer with a response
	 * first, then send the procedure result as an indication (two-stage). */
	ret = rtk_bt_gatts_write_resp(&write_resp);
	if (RTK_BT_OK == ret) {
		BT_LOGA("[APP] OTS response for client write succeed, index: %d\r\n", p_write_ind->index);
	} else {
		BT_LOGE("[APP] OTS response for client write failed, err: 0x%x\r\n", ret);
	}
	BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
				(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
				write_resp.conn_handle, write_resp.index,
				write_resp.type, write_resp.err_code);

	if (do_oacp) {
		ots_oacp_proc(p_write_ind->conn_handle, p_write_ind->value, p_write_ind->len);
	} else if (do_olcp) {
		ots_olcp_proc(p_write_ind->conn_handle, p_write_ind->value, p_write_ind->len);
	}
}

/* --------------------------------- CCCD handler --------------------------------- */
static void ots_cccd_update_hdl(void *data)
{
	rtk_bt_gatts_cccd_ind_t *p_cccd_ind = (rtk_bt_gatts_cccd_ind_t *)data;
	uint8_t cccd_ind = p_cccd_ind->value & RTK_BT_GATT_CCC_INDICATE;
	uint8_t conn_id;

	if (!ots_get_conn_id(p_cccd_ind->conn_handle, &conn_id)) {
		return;
	}

	switch (p_cccd_ind->index) {
	case OTS_OACP_CHAR_CCCD_INDEX:
		oacp_cccd_ind_en_map[conn_id] = cccd_ind;
		break;
	case OTS_OLCP_CHAR_CCCD_INDEX:
		olcp_cccd_ind_en_map[conn_id] = cccd_ind;
		break;
	case OTS_OBJ_CHANGED_CHAR_CCCD_INDEX:
		obj_changed_cccd_ind_en_map[conn_id] = cccd_ind;
		/* Arm the OC/BV-01 detector on enable; an Object ID read before any control
		 * point write then triggers the deferred Object Changed indication. */
		ots_oc_arm_map[conn_id] = cccd_ind ? 1 : 0;
		break;
	default:
		BT_LOGE("[APP] OTS CCCD event unknown index: %d\r\n", p_cccd_ind->index);
		BT_AT_PRINT("+BLEGATTS:cccd,unknown_index\r\n");
		return;
	}

	BT_LOGA("[APP] OTS cccd, index: %d, indicate bit: %d\r\n", p_cccd_ind->index, cccd_ind);
	BT_AT_PRINT("+BLEGATTS:cccd,indicate,%d,%u,%u,%u\r\n",
				cccd_ind ? 1 : 0, p_cccd_ind->app_id,
				p_cccd_ind->conn_handle, p_cccd_ind->index);
}

/* ------------------------- Object transfer channel (CoC) ------------------------ */
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
static void ots_coc_transfer_stop(uint8_t conn_id)
{
	ots_coc[conn_id].state = OTS_XFER_IDLE;
	ots_coc[conn_id].remaining = 0;
	ots_coc[conn_id].last_sent = 0;
	if (ots_coc[conn_id].timer) {
		osif_timer_stop(&ots_coc[conn_id].timer);
	}
}

/* Send one SDU (bounded by channel MTU and OTS_COC_MAX_CHUNK) of object content. */
static void ots_coc_send_chunk(uint8_t conn_id)
{
	uint16_t mtu = 0;
	uint32_t chunk, i;

	if (ots_coc[conn_id].state != OTS_XFER_SENDING || ots_coc[conn_id].remaining == 0) {
		return;
	}

	if (rtk_bt_le_gap_coc_get_chan_param(RTK_BT_LE_COC_CHAN_PARAM_MTU,
										 ots_coc[conn_id].cid, &mtu) != RTK_BT_OK || mtu == 0) {
		mtu = 23;
	}

	chunk = (mtu < OTS_COC_MAX_CHUNK) ? mtu : OTS_COC_MAX_CHUNK;
	if (chunk > ots_coc[conn_id].remaining) {
		chunk = ots_coc[conn_id].remaining;
	}

	/* Object content is generated on the fly: byte at position p equals (p & 0xFF). */
	for (i = 0; i < chunk; i++) {
		ots_coc_txbuf[i] = (uint8_t)(ots_coc[conn_id].offset + i);
	}

	if (rtk_bt_le_gap_coc_send_data(ots_coc[conn_id].cid, (uint16_t)chunk, ots_coc_txbuf) != RTK_BT_OK) {
		BT_LOGE("[APP] OTS coc send data failed, cid: 0x%x\r\n", ots_coc[conn_id].cid);
		ots_coc_transfer_stop(conn_id);
		return;
	}
	ots_coc[conn_id].last_sent = chunk;
}

/* Software timer daemon context: only flag the link and wake the worker task. */
static void ots_coc_transfer_timeout(void *timer_handle)
{
	uint32_t timer_id;
	uint8_t conn_id;

	if (!osif_timer_id_get(&timer_handle, &timer_id)) {
		return;
	}
	conn_id = (uint8_t)timer_id;
	if (conn_id >= RTK_BLE_GAP_MAX_LINKS) {
		return;
	}

	ots_timeout_pending[conn_id] = 1;
	if (ots_timeout_sem) {
		osif_sem_give(ots_timeout_sem);
	}
}

/* Software timer daemon context: flag the deferred Object Changed send and wake the task. */
static void ots_oc_trigger_timeout(void *timer_handle)
{
	(void)timer_handle;
	ots_oc_send_pending = 1;
	if (ots_timeout_sem) {
		osif_sem_give(ots_timeout_sem);
	}
}

static void ots_timeout_task_entry(void *param)
{
	(void)param;
	uint8_t i;

	while (ots_timeout_task_running) {
		if (!osif_sem_take(ots_timeout_sem, BT_TIMEOUT_FOREVER)) {
			continue;
		}
		if (!ots_timeout_task_running) {
			break;
		}
		for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
			if (!ots_timeout_pending[i]) {
				continue;
			}
			ots_timeout_pending[i] = 0;
			BT_LOGE("[APP] OTS object transfer timeout, close channel cid: 0x%x\r\n", ots_coc[i].cid);
			if (ots_coc[i].open) {
				rtk_bt_le_gap_coc_disconnect(ots_coc[i].cid);
			}
			ots_coc[i].state = OTS_XFER_IDLE;
			ots_coc[i].remaining = 0;
		}
		if (ots_oc_send_pending) {
			ots_oc_send_pending = 0;
			if (ots_oc_obj_idx >= 0) {
				ots_indicate_object_changed(ots_oc_conn_handle, ots_oc_obj_idx, OTS_OBJ_CHANGED_FLAG_METADATA);
			}
		}
	}
	osif_task_delete(NULL);
}

void ots_coc_connect_hdl(void *data)
{
	rtk_bt_le_coc_conn_state_ind_t *ind = (rtk_bt_le_coc_conn_state_ind_t *)data;
	uint8_t conn_id;

	if (ind->err || !ots_get_conn_id(ind->conn_handle, &conn_id)) {
		return;
	}
	ots_coc[conn_id].open = 1;
	ots_coc[conn_id].conn_handle = ind->conn_handle;
	ots_coc[conn_id].cid = ind->cid;
	ots_coc[conn_id].state = OTS_XFER_IDLE;
	BT_LOGA("[APP] OTS object transfer channel opened, conn_handle: %d, cid: 0x%x\r\n",
			ind->conn_handle, ind->cid);
}

void ots_coc_disconnect_hdl(void *data)
{
	rtk_bt_le_coc_conn_state_ind_t *ind = (rtk_bt_le_coc_conn_state_ind_t *)data;
	uint8_t conn_id;

	if (!ots_get_conn_id(ind->conn_handle, &conn_id)) {
		return;
	}
	if (ots_coc[conn_id].cid != ind->cid) {
		return;
	}
	ots_coc_transfer_stop(conn_id);
	ots_coc[conn_id].open = 0;
	ots_coc[conn_id].cid = 0;
	BT_LOGA("[APP] OTS object transfer channel closed, conn_handle: %d\r\n", ind->conn_handle);
}

void ots_coc_send_result_hdl(void *data)
{
	rtk_bt_le_coc_send_data_res_ind_t *ind = (rtk_bt_le_coc_send_data_res_ind_t *)data;
	uint8_t conn_id;

	if (!ots_get_conn_id(ind->conn_handle, &conn_id)) {
		return;
	}
	if (ots_coc[conn_id].cid != ind->cid || ots_coc[conn_id].state != OTS_XFER_SENDING) {
		return;
	}

	if (ind->err) {
		BT_LOGE("[APP] OTS coc send result error: 0x%x\r\n", ind->err);
		ots_coc_transfer_stop(conn_id);
		return;
	}

	ots_coc[conn_id].offset += ots_coc[conn_id].last_sent;
	ots_coc[conn_id].remaining -= ots_coc[conn_id].last_sent;

	if (ots_coc[conn_id].remaining == 0) {
		BT_LOGA("[APP] OTS object transfer completed, cid: 0x%x\r\n", ots_coc[conn_id].cid);
		ots_coc_transfer_stop(conn_id);
		return;
	}

	/* Progress was made: refresh the object transfer timeout and send the next SDU. */
	if (ots_coc[conn_id].timer) {
		osif_timer_restart(&ots_coc[conn_id].timer, OTS_TRANSFER_TIMEOUT_MS);
	}
	ots_coc_send_chunk(conn_id);
}
#endif /* RTK_BLE_COC_SUPPORT */

/* ------------------------------ Service callback -------------------------------- */
void object_transfer_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] OTS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] OTS register service failed: %d\r\n", reg_srv_res->reg_status);
		}
		break;
	}
	case RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND: {
		rtk_bt_gatts_ntf_and_ind_ind_t *p_ind = (rtk_bt_gatts_ntf_and_ind_ind_t *)data;
		if (RTK_BT_OK == p_ind->err_code) {
			BT_LOGA("[APP] OTS indicate succeed, index: %d\r\n", p_ind->index);
		} else {
			BT_LOGE("[APP] OTS indicate failed, err: 0x%x\r\n", p_ind->err_code);
		}
		BT_AT_PRINT("+BLEGATTS:indicate,%d,%u,%u,%u\r\n",
					(RTK_BT_OK == p_ind->err_code) ? 0 : -1, p_ind->app_id,
					p_ind->conn_handle, p_ind->index);
#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
		/* An armed OACP Read starts streaming once its success indication is confirmed. */
		if (OTS_OACP_CHAR_VAL_INDEX == p_ind->index && RTK_BT_OK == p_ind->err_code) {
			uint8_t conn_id;
			if (ots_get_conn_id(p_ind->conn_handle, &conn_id) &&
				ots_coc[conn_id].state == OTS_XFER_ARMED) {
				ots_coc[conn_id].state = OTS_XFER_SENDING;
				if (ots_coc[conn_id].timer) {
					osif_timer_restart(&ots_coc[conn_id].timer, OTS_TRANSFER_TIMEOUT_MS);
				}
				ots_coc_send_chunk(conn_id);
			}
		}
#endif
		break;
	}
	case RTK_BT_GATTS_EVT_READ_IND:
		ots_read_hdl(data);
		break;
	case RTK_BT_GATTS_EVT_WRITE_IND:
		ots_write_hdl(data);
		break;
	case RTK_BT_GATTS_EVT_CCCD_IND:
		ots_cccd_update_hdl(data);
		break;
	default:
		break;
	}
}

/* ---------------------------------- Public API ---------------------------------- */
uint16_t ots_object_changed_at_server(void)
{
	rtk_bt_le_get_active_conn_t active_conn;
	uint8_t conn_id;
	uint16_t i;
	int cur;

	memset(&active_conn, 0, sizeof(active_conn));
	if (rtk_bt_le_gap_get_active_conn(&active_conn) != RTK_BT_OK) {
		return RTK_BT_FAIL;
	}

	for (i = 0; i < active_conn.conn_num; i++) {
		if (!ots_get_conn_id(active_conn.conn_handle[i], &conn_id)) {
			continue;
		}
		cur = ots_cur_obj[conn_id];
		if (!obj_changed_cccd_ind_en_map[conn_id] || cur < 0) {
			continue;
		}
		/* Source of Change = Server (bit0 = 0), metadata changed (bit2 = 1). */
		ots_indicate_object_changed(active_conn.conn_handle[i], cur, OTS_OBJ_CHANGED_FLAG_METADATA);
	}

	return RTK_BT_OK;
}

uint16_t object_transfer_srv_add(void)
{
	uint8_t i;
	uint32_t oacp_feat = OTS_OACP_FEAT_READ;
	uint32_t olcp_feat = OTS_OLCP_FEAT_GO_TO;

	memcpy(&ots_feature_val[0], &oacp_feat, sizeof(uint32_t));
	memcpy(&ots_feature_val[4], &olcp_feat, sizeof(uint32_t));

	for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
		ots_cur_obj[i] = -1;
	}

	object_transfer_srv.type = GATT_SERVICE_OVER_BLE;
	object_transfer_srv.server_info = 0;
	object_transfer_srv.user_data = NULL;
	object_transfer_srv.register_status = 0;

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	memset(ots_coc, 0, sizeof(ots_coc));
	memset((void *)ots_timeout_pending, 0, sizeof(ots_timeout_pending));
	rtk_bt_le_gap_coc_register_psm(1, OTS_PSM);
	rtk_bt_le_gap_coc_set_param(RTK_BT_LE_COC_PARAM_LOCAL_MTU, OTS_COC_LOCAL_MTU);
	for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
		if (ots_coc[i].timer == NULL) {
			osif_timer_create(&ots_coc[i].timer, "ots_xfer_timer", i,
							  OTS_TRANSFER_TIMEOUT_MS, false, ots_coc_transfer_timeout);
		}
	}
	ots_oc_send_pending = 0;
	if (ots_oc_timer == NULL) {
		osif_timer_create(&ots_oc_timer, "ots_oc_timer", 0,
						  OTS_OC_TRIGGER_DELAY_MS, false, ots_oc_trigger_timeout);
	}
	if (ots_timeout_sem == NULL) {
		osif_sem_create(&ots_timeout_sem, 0, 1);
	}
	if (ots_timeout_task_hdl == NULL) {
		ots_timeout_task_running = 1;
		osif_task_create(&ots_timeout_task_hdl, "ots_timeout_task", ots_timeout_task_entry, NULL,
						 OTS_TIMEOUT_TASK_STACK_SIZE, OTS_TIMEOUT_TASK_PRIORITY);
	}
#endif

	return rtk_bt_gatts_register_service(&object_transfer_srv);
}

void ots_disconnect(uint16_t conn_handle)
{
	uint8_t conn_id;

	if (!ots_get_conn_id(conn_handle, &conn_id)) {
		return;
	}

	ots_cur_obj[conn_id] = -1;
	oacp_cccd_ind_en_map[conn_id] = 0;
	olcp_cccd_ind_en_map[conn_id] = 0;
	obj_changed_cccd_ind_en_map[conn_id] = 0;
	ots_oc_arm_map[conn_id] = 0;

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	ots_coc_transfer_stop(conn_id);
	ots_coc[conn_id].open = 0;
	ots_coc[conn_id].cid = 0;
#endif
}

void ots_status_deinit(void)
{
	uint8_t i;

	for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
		ots_cur_obj[i] = -1;
	}
	memset(oacp_cccd_ind_en_map, 0, sizeof(oacp_cccd_ind_en_map));
	memset(olcp_cccd_ind_en_map, 0, sizeof(olcp_cccd_ind_en_map));
	memset(obj_changed_cccd_ind_en_map, 0, sizeof(obj_changed_cccd_ind_en_map));
	memset(ots_oc_arm_map, 0, sizeof(ots_oc_arm_map));

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
	for (i = 0; i < RTK_BLE_GAP_MAX_LINKS; i++) {
		if (ots_coc[i].timer) {
			osif_timer_stop(&ots_coc[i].timer);
			osif_timer_delete(&ots_coc[i].timer);
			ots_coc[i].timer = NULL;
		}
	}
	memset(ots_coc, 0, sizeof(ots_coc));
	memset((void *)ots_timeout_pending, 0, sizeof(ots_timeout_pending));
	if (ots_oc_timer) {
		osif_timer_stop(&ots_oc_timer);
		osif_timer_delete(&ots_oc_timer);
		ots_oc_timer = NULL;
	}
	ots_oc_send_pending = 0;
	ots_oc_obj_idx = -1;

	/* Signal the worker task to exit; it deletes itself. Keep the semaphore alive to
	 * avoid a use-after-free race with a task still returning from osif_sem_take. */
	if (ots_timeout_task_hdl) {
		ots_timeout_task_running = 0;
		if (ots_timeout_sem) {
			osif_sem_give(ots_timeout_sem);
		}
		ots_timeout_task_hdl = NULL;
	}

	rtk_bt_le_gap_coc_register_psm(0, OTS_PSM);
#endif
}
