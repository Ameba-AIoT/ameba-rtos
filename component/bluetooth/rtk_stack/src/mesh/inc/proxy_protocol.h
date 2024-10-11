/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     proxy_protocol.h
  * @brief    Head file for proxy protocol.
  * @details
  * @author   bill
  * @date     2016-11-26
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _PROXY_PROTOCOL_H_
#define _PROXY_PROTOCOL_H_

/* Add Includes here */
#include "mesh_api.h"

BEGIN_DECLS

/** @addtogroup Proxy_Protocol
  * @{
  */

/** @defgroup Proxy_Protocol_Exported_Macros Exported Macros
  * @{
  */
#define MESH_PROXY_PKT_OFFSET                       3 //!< opcode + handle
#define MESH_PROXY_PKT_HEADER_SIZE                  1 //!< @ref proxy_pkt_header_t
/** @} */

/** @defgroup Proxy_Protocol_Exported_Types Exported Types
  * @{
  */
typedef enum
{
    PROXY_PDU_TYPE_NET_PDU,
    PROXY_PDU_TYPE_MESH_BEACON,
    PROXY_PDU_TYPE_PROXY_CFG,
    PROXY_PDU_TYPE_PROV_PDU
} proxy_pdu_type_t;

typedef enum
{
    PROXY_SAR_COMPLETE,
    PROXY_SAR_FIRST,
    PROXY_SAR_CONTINUATE,
    PROXY_SAR_LAST
} proxy_sar_t;

typedef struct
{
    uint8_t type: 6; //!< @ref proxy_pdu_type_t
    uint8_t sar: 2; //!< @ref proxy_sar_t
} _PACKED4_ proxy_pkt_header_t, *proxy_pkt_header_p;

enum
{
    PROXY_CFG_OPCODE_SET_FILTER_TYPE = 0x00,
    PROXY_CFG_OPCODE_ADD_ADDR = 0x01,
    PROXY_CFG_OPCODE_REMOVE_ADDR = 0x02,
    PROXY_CFG_OPCODE_FILTER_STAT = 0x03,
#if MESH_DF
    PROXY_CFG_OPCODE_DIRECTED_PROXY_CAP_STAT = 0x04,
    PROXY_CFG_OPCODE_DIRECTED_PROXY_CONTROL = 0x05,
#endif
} _SHORT_ENUM_;
typedef uint8_t proxy_cfg_opcode_t;

enum
{
    PROXY_CFG_FILTER_TYPE_WHITE_LIST,
    PROXY_CFG_FILTER_TYPE_BLACK_LIST
} _SHORT_ENUM_;
typedef uint8_t proxy_cfg_filter_type_t;

#if MESH_DF
enum
{
    PROXY_CFG_USE_DIRECTED_DISABLE = 0x00,
    PROXY_CFG_USE_DIRECTED_ENABLE = 0x01,
    PROXY_CFG_USE_DIRECTED_DEFAULT = 0xFF,
} _SHORT_ENUM_;
typedef uint8_t proxy_cfg_use_directed_t;
#endif

typedef struct
{
    proxy_cfg_filter_type_t filter_type;
} _PACKED4_ proxy_cfg_set_filter_type_t, *proxy_cfg_set_filter_type_p;

typedef struct
{
    uint8_t addr[2]; //!< variable length
} _PACKED4_ proxy_cfg_add_addr_t, *proxy_cfg_add_addr_p;

typedef struct
{
    uint8_t addr[2]; //!< variable length
} _PACKED4_ proxy_cfg_remove_addr_t, *proxy_cfg_remove_addr_p;

typedef struct
{
    proxy_cfg_filter_type_t filter_type;
    uint8_t list_size[2];
} _PACKED4_ proxy_cfg_filter_stat_t, *proxy_cfg_filter_stat_p;

#if MESH_DF
typedef struct
{
    directed_control_state_t directed_proxy;
    proxy_cfg_use_directed_t use_directed;
} _PACKED4_ proxy_cfg_directed_proxy_cap_stat_t, *proxy_cfg_directed_proxy_cap_stat_p;

typedef struct
{
    proxy_cfg_use_directed_t use_directed;
    uint8_t addr_range[3]; //!< variable length
} _PACKED4_ proxy_cfg_directed_proxy_control_t, *proxy_cfg_directed_proxy_control_p;
#endif

typedef struct
{
    proxy_cfg_opcode_t opcode;
    union
    {
        proxy_cfg_set_filter_type_t set_filter_type;
        proxy_cfg_add_addr_t add_addr;
        proxy_cfg_remove_addr_t remove_addr;
        proxy_cfg_filter_stat_t filter_stat;
#if MESH_DF
        proxy_cfg_directed_proxy_cap_stat_t directed_proxy_cap_stat;
        proxy_cfg_directed_proxy_control_t directed_proxy_control;
#endif
    };
} _PACKED4_ proxy_cfg_msg_t, *proxy_cfg_msg_p;

#define MESH_PROXY_PROTOCOL_ALL_CTX_ID          0xFF
#define MESH_PROXY_PROTOCOL_RSVD_CTX_ID         0xFE
typedef bool (*proxy_send_sar_pf)(gap_sched_link_t link, uint8_t *pvalue, uint16_t len);
typedef bool (*proxy_receive_cb_pf)(uint8_t ctx_id, proxy_pdu_type_t type, uint8_t *pvalue,
                                    uint16_t len);

typedef enum
{
    PROXY_CTX_TYPE_INVALID,
    PROXY_CTX_TYPE_PROV,
    PROXY_CTX_TYPE_PROXY,
} proxy_ctx_type_t;

#if MESH_DF
typedef enum
{
    PROXY_CLIENT_TYPE_PROXY_CLIENT,
    PROXY_CLIENT_TYPE_DIRECTED_PROXY_CLIENT,
    PROXY_CLIENT_TYPE_BLACK_LIST_CLIENT,
    PROXY_CLIENT_TYPE_UNSET,
} proxy_client_type_t;

typedef struct _directed_proxy_t
{
    struct _directed_proxy_t *pnext;
    uint16_t net_key_index;
    proxy_cfg_use_directed_t use_directed;
    mesh_addr_range_t addr_range;
} directed_proxy_param_t, *directed_proxy_param_p;
#endif
typedef struct
{
    proxy_ctx_type_t ctx_type;
    bool data_out_cccd_enabled;
    gap_sched_link_t link;
    uint16_t net_key_index;
    uint16_t rx_len;
    uint8_t *rx_buffer;
    plt_timer_t rx_timer;
    proxy_receive_cb_pf pf_rx;
    proxy_send_sar_pf pf_tx;
    proxy_cfg_filter_type_t filter_type;
    uint16_t addr_count;
    mesh_addr_member_p addr_list;
#if MESH_DF
    proxy_client_type_t proxy_client_type;
    mesh_addr_range_t proxy_client_addr_range;
    plt_list_t directed_proxy_list;
#endif
} proxy_ctx_t, *proxy_ctx_p;
/** @} */

extern proxy_ctx_t *proxy_ctx;
extern uint8_t proxy_ctx_count;

/** @defgroup Proxy_Protocol_Exported_Functions Exported Functions
  * @{
  */

///@cond
void proxy_send(uint8_t ctx_id, proxy_pdu_type_t type, uint8_t *pvalue, uint16_t len);
bool proxy_receive(uint8_t ctx_id, uint8_t *pvalue, uint16_t len);
bool proxy_cfg_receive(uint8_t ctx_id, uint8_t *pbuffer, uint16_t len);
bool proxy_cfg_handle_msg(uint8_t ctx_id, uint8_t *pvalue, uint16_t len);

void proxy_ctx_init(uint8_t proxy_count);
void proxy_ctx_deinit(void);

uint8_t proxy_ctx_allocate(proxy_send_sar_pf proxy_send_sar, proxy_receive_cb_pf proxy_receive_cb,
                           proxy_ctx_type_t ctx_type);
bool proxy_ctx_set_filter_type(uint8_t ctx_id, proxy_cfg_filter_type_t filter_type);
bool proxy_ctx_set_cb(uint8_t ctx_id, proxy_send_sar_pf proxy_send_sar,
                      proxy_receive_cb_pf proxy_receive_cb);
bool proxy_ctx_set_link(uint8_t ctx_id, gap_sched_link_t link_id);
gap_sched_link_t proxy_ctx_get_link(uint8_t ctx_id);
gap_sched_link_t proxy_ctx_get_link_def(proxy_ctx_type_t ctx_type);
bool proxy_ctx_enable_data_out_cccd(uint8_t ctx_id, bool enable);
bool proxy_ctx_is_data_out_cccd_enabled(uint8_t ctx_id);
uint8_t proxy_ctx_id_get(gap_sched_link_t link, proxy_ctx_type_t ctx_type);
void proxy_ctx_disconnect_all(proxy_ctx_type_t ctx_type);
uint8_t proxy_ctx_active_count(proxy_ctx_type_t ctx_type);
uint8_t proxy_ctx_idle_count(void);
bool proxy_ctx_free(uint8_t ctx_id);

void proxy_filter_free(uint8_t ctx_id);
bool proxy_filter(uint8_t ctx_id, uint16_t addr);
void proxy_filter_update(uint8_t ctx_id, uint16_t addr);

void proxy_handle_timeout(uint8_t ctx_id);
///@endcond

/**
  * @brief set the proxy filter type
  * @param[in] ctx_id: the context idx
  * @param[in] filter_type: the filter type
  * @return send result
  */
bool proxy_cfg_set_filter_type(uint8_t ctx_id, proxy_cfg_filter_type_t filter_type);

/**
  * @brief add / remove proxy filter addr
  * @param[in] ctx_id: the context idx
  * @param[in] add_remove: 1 - add, 0 - remove
  * @param[in] addr: the address list
  * @param[in] count: the address count
  * @return send result
  */
bool proxy_cfg_add_remove_addr(uint8_t ctx_id, bool add_remove, uint16_t addr[], uint16_t count);

#if MESH_DF
/**
  * @brief directed proxy state
  * @param[in] ctx_id: the context idx
  * @param[in] net_key_index: net key index
  * @return send result
  */
bool proxy_cfg_directed_proxy_cap_stat(uint8_t ctx_id, uint16_t net_key_index);

/**
  * @brief contorl directed proxy
  * @param[in] ctx_id: the context idx
  * @param[in] use_directed: 0 - disable, 1 - enable
  * @param[in] paddr_range: unicast + second elem
  * @return send result
  */
bool proxy_cfg_directed_proxy_control(uint8_t ctx_id, uint8_t use_directed,
                                      mesh_addr_range_t *paddr_range);

/**
 * @brief add or update directed proxy list
 * @param[in] ctx_id:
 * @param[in] net_key_index:
 * @param[in] use_directed:
 * @param[in] unicast_addr:
 * @param[in] elem_num:
 * @return true
 * @return false
 */
bool proxy_add_directed_proxy(uint8_t ctx_id, uint16_t net_key_index, uint8_t use_directed,
                              uint16_t unicast_addr, uint8_t elem_num);

directed_proxy_param_p proxy_directed_proxy_get(uint8_t ctx_id, uint16_t net_key_index);
bool proxy_directed_proxy_addr_contain(uint8_t ctx_id, uint16_t net_key_index, uint16_t src_addr);
bool proxy_ctx_directed_proxy_free(uint8_t ctx_id);
#endif

/** @} */
/** @} */

END_DECLS

#endif /* _PROXY_PROTOCOL_H_ */
