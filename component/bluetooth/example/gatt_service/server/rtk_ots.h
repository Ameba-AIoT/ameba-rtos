/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __OTS_SERVICE_H__
#define __OTS_SERVICE_H__

#ifdef __cplusplus
extern "C"  {
#endif

#include <rtk_bt_def.h>
#include <bt_api_config.h>

/* ------------------------------ UUIDs (OTS v1.0) ------------------------------ */
#define OBJECT_TRANSFER_SRV_UUID            0x1825
#define OTS_FEATURE_CHAR_UUID               0x2ABD
#define OBJECT_NAME_CHAR_UUID               0x2ABE
#define OBJECT_TYPE_CHAR_UUID               0x2ABF
#define OBJECT_SIZE_CHAR_UUID               0x2AC0
#define OBJECT_ID_CHAR_UUID                 0x2AC3
#define OBJECT_PROPERTIES_CHAR_UUID         0x2AC4
#define OBJECT_ACTION_CP_CHAR_UUID          0x2AC5
#define OBJECT_LIST_CP_CHAR_UUID            0x2AC6
#define OBJECT_CHANGED_CHAR_UUID            0x2AC8

#define RTK_BT_UUID_OBJECT_TRANSFER_SRV     BT_UUID_DECLARE_16(OBJECT_TRANSFER_SRV_UUID)
#define RTK_BT_UUID_OTS_FEATURE_CHAR        BT_UUID_DECLARE_16(OTS_FEATURE_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_NAME_CHAR        BT_UUID_DECLARE_16(OBJECT_NAME_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_TYPE_CHAR        BT_UUID_DECLARE_16(OBJECT_TYPE_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_SIZE_CHAR        BT_UUID_DECLARE_16(OBJECT_SIZE_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_ID_CHAR          BT_UUID_DECLARE_16(OBJECT_ID_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_PROPERTIES_CHAR  BT_UUID_DECLARE_16(OBJECT_PROPERTIES_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_ACTION_CP_CHAR   BT_UUID_DECLARE_16(OBJECT_ACTION_CP_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_LIST_CP_CHAR     BT_UUID_DECLARE_16(OBJECT_LIST_CP_CHAR_UUID)
#define RTK_BT_UUID_OBJECT_CHANGED_CHAR     BT_UUID_DECLARE_16(OBJECT_CHANGED_CHAR_UUID)

/* --------------------------- Attribute table indices --------------------------- */
#define OTS_FEATURE_CHAR_VAL_INDEX          (2)    /* read */
#define OTS_OBJ_NAME_CHAR_VAL_INDEX         (4)    /* read */
#define OTS_OBJ_TYPE_CHAR_VAL_INDEX         (6)    /* read */
#define OTS_OBJ_SIZE_CHAR_VAL_INDEX         (8)    /* read */
#define OTS_OBJ_ID_CHAR_VAL_INDEX           (10)   /* read */
#define OTS_OBJ_PROPERTIES_CHAR_VAL_INDEX   (12)   /* read */
#define OTS_OACP_CHAR_VAL_INDEX             (14)   /* write, indicate */
#define OTS_OACP_CHAR_CCCD_INDEX            (15)   /* cccd indicate */
#define OTS_OLCP_CHAR_VAL_INDEX             (17)   /* write, indicate */
#define OTS_OLCP_CHAR_CCCD_INDEX            (18)   /* cccd indicate */
#define OTS_OBJ_CHANGED_CHAR_VAL_INDEX      (20)   /* indicate */
#define OTS_OBJ_CHANGED_CHAR_CCCD_INDEX     (21)   /* cccd indicate */

/* --------------------------------- Value lengths -------------------------------- */
#define OTS_FEATURE_VALUE_LEN               8      /* OACP features(4) + OLCP features(4) */
#define OTS_OBJ_SIZE_VALUE_LEN              8      /* current(4) + allocated(4) */
#define OTS_OBJ_ID_LEN                      6      /* uint48 */
#define OTS_OBJ_PROPERTIES_LEN             4      /* uint32 */
#define OTS_OBJ_CHANGED_VALUE_LEN           7      /* flags(1) + object id(6) */
#define OTS_OBJ_NAME_MAX_LEN                26     /* fits within default ATT_MTU */
#define OTS_OBJ_TYPE_MAX_LEN                16     /* 2 or 16 octet UUID */

/* ----------------------------- OTS Feature bit masks ---------------------------- */
/* OACP features (octets 0-3) */
#define OTS_OACP_FEAT_READ                  (1UL << 4)
/* OLCP features (octets 4-7) */
#define OTS_OLCP_FEAT_GO_TO                 (1UL << 0)

/* ----------------------------- Object Properties bits --------------------------- */
/* OTS v1.0 Table 3.10 (note: this layout differs from the OTS Feature OACP bits). */
#define OTS_PROP_DELETE                     (1UL << 0)
#define OTS_PROP_EXECUTE                    (1UL << 1)
#define OTS_PROP_READ                       (1UL << 2)
#define OTS_PROP_WRITE                      (1UL << 3)
#define OTS_PROP_APPEND                     (1UL << 4)
#define OTS_PROP_TRUNCATE                   (1UL << 5)
#define OTS_PROP_PATCH                      (1UL << 6)
#define OTS_PROP_MARK                       (1UL << 7)

/* ---------------------------------- OACP op codes ------------------------------- */
#define OTS_OACP_OP_CREATE                  0x01
#define OTS_OACP_OP_DELETE                  0x02
#define OTS_OACP_OP_CALC_CHECKSUM           0x03
#define OTS_OACP_OP_EXECUTE                 0x04
#define OTS_OACP_OP_READ                    0x05
#define OTS_OACP_OP_WRITE                   0x06
#define OTS_OACP_OP_ABORT                   0x07
#define OTS_OACP_OP_RESP                    0x60

/* OACP result codes */
#define OTS_OACP_RES_SUCCESS                0x01
#define OTS_OACP_RES_OP_NOT_SUPPORTED       0x02
#define OTS_OACP_RES_INVALID_PARAM          0x03
#define OTS_OACP_RES_INSUFFICIENT_RES       0x04
#define OTS_OACP_RES_INVALID_OBJECT         0x05
#define OTS_OACP_RES_CHANNEL_UNAVAILABLE    0x06
#define OTS_OACP_RES_UNSUPPORTED_TYPE       0x07
#define OTS_OACP_RES_PROCEDURE_NOT_PERMIT   0x08
#define OTS_OACP_RES_OBJECT_LOCKED          0x09
#define OTS_OACP_RES_OPERATION_FAILED       0x0A

/* ---------------------------------- OLCP op codes ------------------------------- */
#define OTS_OLCP_OP_FIRST                   0x01
#define OTS_OLCP_OP_LAST                    0x02
#define OTS_OLCP_OP_PREVIOUS                0x03
#define OTS_OLCP_OP_NEXT                    0x04
#define OTS_OLCP_OP_GO_TO                   0x05
#define OTS_OLCP_OP_ORDER                   0x06
#define OTS_OLCP_OP_REQ_NUM_OBJ             0x07
#define OTS_OLCP_OP_CLEAR_MARKING           0x08
#define OTS_OLCP_OP_RESP                    0x70

/* OLCP result codes */
#define OTS_OLCP_RES_SUCCESS                0x01
#define OTS_OLCP_RES_OP_NOT_SUPPORTED       0x02
#define OTS_OLCP_RES_INVALID_PARAM          0x03
#define OTS_OLCP_RES_OPERATION_FAILED       0x04
#define OTS_OLCP_RES_OUT_OF_BOUNDS          0x05
#define OTS_OLCP_RES_TOO_MANY_OBJECTS       0x06
#define OTS_OLCP_RES_NO_OBJECT              0x07
#define OTS_OLCP_RES_OBJECT_ID_NOT_FOUND    0x08

/* --------------------- OTS specific ATT application error codes ------------------ */
#define RTK_BT_ATT_ERR_OTS_WRITE_REQ_REJECTED   0x80
#define RTK_BT_ATT_ERR_OTS_OBJECT_NOT_SELECTED  0x81
#define RTK_BT_ATT_ERR_OTS_CONCURRENCY_LIMIT    0x82
#define RTK_BT_ATT_ERR_OTS_OBJECT_NAME_EXISTS   0x83

/* Object Changed flags */
#define OTS_OBJ_CHANGED_FLAG_SOURCE_CLIENT  (1U << 0)
#define OTS_OBJ_CHANGED_FLAG_CONTENTS       (1U << 1)
#define OTS_OBJ_CHANGED_FLAG_METADATA       (1U << 2)
#define OTS_OBJ_CHANGED_FLAG_CREATION       (1U << 3)
#define OTS_OBJ_CHANGED_FLAG_DELETION       (1U << 4)

uint16_t object_transfer_srv_add(void);

void object_transfer_srv_callback(uint8_t event, void *data);

/* Trigger an Object Changed indication as a result of a server-side (out of band)
 * metadata change. Used for OTS/SR/OC/BV-01-C. */
uint16_t ots_object_changed_at_server(void);

void ots_disconnect(uint16_t conn_handle);

void ots_status_deinit(void);

#if defined(RTK_BLE_COC_SUPPORT) && RTK_BLE_COC_SUPPORT
/* Object Transfer Channel (LE CoC) event hooks, dispatched from the LE GAP callback. */
void ots_coc_connect_hdl(void *data);
void ots_coc_disconnect_hdl(void *data);
void ots_coc_send_result_hdl(void *data);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __OTS_SERVICE_H__ */
