/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     configuration.h
* @brief    Head file for configuration models.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2016-3-24
* @version  v1.0
* *************************************************************************************
*/

/* Define to prevent recursive inclusion */
#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

/* Add Includes here */
#include "mesh_api.h"
#include "generic_types.h"

BEGIN_DECLS

/**
 * @addtogroup CONFIGURATION
 * @{
 */

/**
 * @defgroup CONFIGURATION_ACCESS_OPCODE Access Opcode
 * @brief Mesh message access opcode
 * @{
 */
#define MESH_MSG_CFG_APP_KEY_ADD                        0x00
#define MESH_MSG_CFG_APP_KEY_UPDATE                     0x01
#define MESH_MSG_CFG_APP_KEY_DELETE                     0x8000
#define MESH_MSG_CFG_APP_KEY_GET                        0x8001
#define MESH_MSG_CFG_APP_KEY_LIST                       0x8002
#define MESH_MSG_CFG_APP_KEY_STAT                       0x8003
#define MESH_MSG_CFG_COMPO_DATA_GET                     0x8008
#define MESH_MSG_CFG_COMPO_DATA_STAT                    0x02
#define MESH_MSG_CFG_BEACON_GET                         0x8009
#define MESH_MSG_CFG_BEACON_SET                         0x800A
#define MESH_MSG_CFG_BEACON_STAT                        0x800B
#define MESH_MSG_CFG_DEFAULT_TTL_GET                    0x800C
#define MESH_MSG_CFG_DEFAULT_TTL_SET                    0x800D
#define MESH_MSG_CFG_DEFAULT_TTL_STAT                   0x800E
#define MESH_MSG_CFG_FRND_GET                           0x800F
#define MESH_MSG_CFG_FRND_SET                           0x8010
#define MESH_MSG_CFG_FRND_STAT                          0x8011
#define MESH_MSG_CFG_PROXY_GET                          0x8012
#define MESH_MSG_CFG_PROXY_SET                          0x8013
#define MESH_MSG_CFG_PROXY_STAT                         0x8014
#define MESH_MSG_CFG_KEY_REFRESH_PHASE_GET              0x8015
#define MESH_MSG_CFG_KEY_REFRESH_PHASE_SET              0x8016
#define MESH_MSG_CFG_KEY_REFRESH_PHASE_STAT             0x8017
#define MESH_MSG_CFG_MODEL_PUB_GET                      0x8018
#define MESH_MSG_CFG_MODEL_PUB_SET                      0x03
#define MESH_MSG_CFG_MODEL_PUB_STAT                     0x8019
#define MESH_MSG_CFG_MODEL_PUB_VA_SET                   0x801A
#define MESH_MSG_CFG_MODEL_SUB_ADD                      0x801B
#define MESH_MSG_CFG_MODEL_SUB_DELETE                   0x801C
#define MESH_MSG_CFG_MODEL_SUB_DELETE_ALL               0x801D
#define MESH_MSG_CFG_MODEL_SUB_OVERWRITE                0x801E
#define MESH_MSG_CFG_MODEL_SUB_STAT                     0x801F
#define MESH_MSG_CFG_MODEL_SUB_VA_ADD                   0x8020
#define MESH_MSG_CFG_MODEL_SUB_VA_DELETE                0x8021
#define MESH_MSG_CFG_MODEL_SUB_VA_OVERWRITE             0x8022
#define MESH_MSG_CFG_NET_TRANS_GET                      0x8023
#define MESH_MSG_CFG_NET_TRANS_SET                      0x8024
#define MESH_MSG_CFG_NET_TRANS_STAT                     0x8025
#define MESH_MSG_CFG_RELAY_GET                          0x8026
#define MESH_MSG_CFG_RELAY_SET                          0x8027
#define MESH_MSG_CFG_RELAY_STAT                         0x8028
#define MESH_MSG_CFG_SIG_MODEL_SUB_GET                  0x8029
#define MESH_MSG_CFG_SIG_MODEL_SUB_LIST                 0x802A
#define MESH_MSG_CFG_VENDOR_MODEL_SUB_GET               0x802B
#define MESH_MSG_CFG_VENDOR_MODEL_SUB_LIST              0x802C
#define MESH_MSG_CFG_LPN_POLL_TO_GET                    0x802D
#define MESH_MSG_CFG_LPN_POLL_TO_STAT                   0x802E
#define MESH_MSG_CFG_HB_PUB_GET                         0x8038
#define MESH_MSG_CFG_HB_PUB_SET                         0x8039
#define MESH_MSG_CFG_HB_PUB_STAT                        0x06
#define MESH_MSG_CFG_HB_SUB_GET                         0x803A
#define MESH_MSG_CFG_HB_SUB_SET                         0x803B
#define MESH_MSG_CFG_HB_SUB_STAT                        0x803C
#define MESH_MSG_CFG_MODEL_APP_BIND                     0x803D
#define MESH_MSG_CFG_MODEL_APP_STAT                     0x803E
#define MESH_MSG_CFG_MODEL_APP_UNBIND                   0x803F
#define MESH_MSG_CFG_NET_KEY_ADD                        0x8040
#define MESH_MSG_CFG_NET_KEY_DELETE                     0x8041
#define MESH_MSG_CFG_NET_KEY_GET                        0x8042
#define MESH_MSG_CFG_NET_KEY_LIST                       0x8043
#define MESH_MSG_CFG_NET_KEY_STAT                       0x8044
#define MESH_MSG_CFG_NET_KEY_UPDATE                     0x8045
#define MESH_MSG_CFG_NODE_IDENTITY_GET                  0x8046
#define MESH_MSG_CFG_NODE_IDENTITY_SET                  0x8047
#define MESH_MSG_CFG_NODE_IDENTITY_STAT                 0x8048
#define MESH_MSG_CFG_NODE_RESET                         0x8049
#define MESH_MSG_CFG_NODE_RESET_STAT                    0x804A
#define MESH_MSG_CFG_SIG_MODEL_APP_GET                  0x804B
#define MESH_MSG_CFG_SIG_MODEL_APP_LIST                 0x804C
#define MESH_MSG_CFG_VENDOR_MODEL_APP_GET               0x804D
#define MESH_MSG_CFG_VENDOR_MODEL_APP_LIST              0x804E
/** @} */

/**
 * @defgroup CONFIGURATION_MODEL_ID Model ID
 * @brief Mesh model id
 * @{
 */
#define MESH_MODEL_CFG_SERVER                           0x0000FFFF
#define MESH_MODEL_CFG_CLIENT                           0x0001FFFF
/** @} */

/**
 * @defgroup CONFIGURATION_MESH_MSG Mesh Msg
 * @brief Mesh message types used by models
 * @{
 */

/** @brief message status */
typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_BEACON_GET)];
} _PACKED4_ cfg_beacon_get_t, *cfg_beacon_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_BEACON_SET)];
    uint8_t state;
} _PACKED4_ cfg_beacon_set_t, *cfg_beacon_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_BEACON_STAT)];
    uint8_t state;
} _PACKED4_ cfg_beacon_stat_t, *cfg_beacon_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_COMPO_DATA_GET)];
    uint8_t page;
} _PACKED4_ cfg_compo_data_get_t, *cfg_compo_data_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_COMPO_DATA_STAT)];
    uint8_t page;
    uint8_t data[0]; //!< variable length
} _PACKED4_ cfg_compo_data_stat_t, *cfg_compo_data_stat_p;

//RTK porting:add new structure for call app layer
typedef struct {
	uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_COMPO_DATA_STAT)];
	uint8_t page;
	uint16_t msg_len;
	uint16_t src;
	uint8_t flag;
	uint32_t length; //!< for ipc test
	uint8_t *data; //!< variable length
} _PACKED4_ cfg_compo_data_stat_call_t;

typedef struct {
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_DEFAULT_TTL_GET)];
} _PACKED4_ cfg_default_ttl_get_t, *cfg_default_ttl_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_DEFAULT_TTL_SET)];
    uint8_t ttl;
} _PACKED4_ cfg_default_ttl_set_t, *cfg_default_ttl_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_DEFAULT_TTL_STAT)];
    uint8_t ttl;
} _PACKED4_ cfg_default_ttl_stat_t, *cfg_default_ttl_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_PROXY_GET)];
} _PACKED4_ cfg_proxy_get_t, *cfg_proxy_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_PROXY_SET)];
    uint8_t state;
} _PACKED4_ cfg_proxy_set_t, *cfg_proxy_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_PROXY_STAT)];
    uint8_t state;
} _PACKED4_ cfg_proxy_stat_t, *cfg_proxy_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_RELAY_GET)];
} _PACKED4_ cfg_relay_get_t, *cfg_relay_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_RELAY_SET)];
    uint8_t state;
    uint8_t count : 3;
    uint8_t steps : 5;
} _PACKED4_ cfg_relay_set_t, *cfg_relay_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_RELAY_STAT)];
    uint8_t state;
    uint8_t count : 3;
    uint8_t steps : 5;
} _PACKED4_ cfg_relay_stat_t, *cfg_relay_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_TRANS_GET)];

} _PACKED4_ cfg_net_transmit_get_t, *cfg_net_transmit_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_TRANS_SET)];
    uint8_t count : 3;
    uint8_t steps : 5;
} _PACKED4_ cfg_net_transmit_set_t, *cfg_net_transmit_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_TRANS_STAT)];
    uint8_t count : 3;
    uint8_t steps : 5;
} _PACKED4_ cfg_net_transmit_stat_t, *cfg_net_transmit_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_PUB_GET)];
    uint16_t element_addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_pub_get_t, *cfg_model_pub_get_p;

typedef struct
{
    union
    {
        uint16_t value;
        struct
        {
            uint16_t app_key_index: 12; //!< global index
            uint16_t frnd_flag: 1;
            uint16_t rfu: 3;
        };
    };
} _PACKED4_ pub_key_info_t, *pub_key_info_p;

typedef struct
{
    uint8_t steps : 6; //!< 0x00 Publish Period is disabled
    uint8_t resol : 2; //!< The resolution of the Number of Steps field
} _PACKED4_ pub_period_t;

typedef struct
{
    uint8_t count : 3;
    uint8_t steps : 5;
} _PACKED4_ pub_retrans_info_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_PUB_SET)];
    uint16_t element_addr;
    uint16_t pub_addr;
    pub_key_info_t pub_key_info;
    uint8_t pub_ttl;
    pub_period_t pub_period;
    pub_retrans_info_t pub_retrans_info;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_pub_set_t, *cfg_model_pub_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_PUB_VA_SET)];
    uint16_t element_addr;
    uint8_t pub_addr[16];
    pub_key_info_t pub_key_info;
    uint8_t pub_ttl;
    pub_period_t pub_period;
    pub_retrans_info_t pub_retrans_info;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_pub_va_set_t, *cfg_model_pub_va_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_PUB_STAT)];
    mesh_msg_stat_t stat;
    uint16_t element_addr;
    uint16_t pub_addr;
    pub_key_info_t pub_key_info;
    uint8_t pub_ttl;
    pub_period_t pub_period;
    pub_retrans_info_t pub_retrans_info;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_pub_stat_t, *cfg_model_pub_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_SUB_ADD)];
    uint16_t element_addr;
    uint16_t addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_sub_add_t, *cfg_model_sub_add_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_SUB_VA_ADD)];
    uint16_t element_addr;
    uint8_t addr[16];
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_sub_va_add_t, *cfg_model_sub_va_add_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_SUB_DELETE_ALL)];
    uint16_t element_addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_sub_delete_all_t, *cfg_model_sub_delete_all_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_SUB_VA_DELETE)];
    uint16_t element_addr;
    uint8_t addr[16];
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_sub_delete_t, *cfg_model_sub_delete_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_SUB_VA_OVERWRITE)];
    uint16_t element_addr;
    uint8_t addr[16];
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_sub_overwrite_t, *cfg_model_sub_overwrite_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_SUB_STAT)];
    mesh_msg_stat_t stat;
    uint16_t element_addr;
    uint16_t addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_sub_stat_t, *cfg_model_sub_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_SIG_MODEL_SUB_GET)];
    uint16_t element_addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_sub_get_t, *cfg_model_sub_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_SIG_MODEL_SUB_LIST)];
    mesh_msg_stat_t stat;
    uint16_t element_addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
    uint8_t data[2]; //!< variable length
} _PACKED4_ cfg_model_sub_list_t, *cfg_model_sub_list_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_KEY_ADD)];
    uint16_t net_key_index;
    uint8_t net_key[16];
} _PACKED4_ cfg_net_key_add_t, *cfg_net_key_add_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_KEY_UPDATE)];
    uint16_t net_key_index;
    uint8_t net_key[16];
} _PACKED4_ cfg_net_key_update_t, *cfg_net_key_update_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_KEY_DELETE)];
    uint16_t net_key_index;
} _PACKED4_ cfg_net_key_delete_t, *cfg_net_key_delete_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_KEY_STAT)];
    mesh_msg_stat_t stat;
    uint16_t net_key_index;
} _PACKED4_ cfg_net_key_stat_t, *cfg_net_key_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_KEY_GET)];
} _PACKED4_ cfg_net_key_get_t, *cfg_net_key_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NET_KEY_LIST)];
    uint8_t net_key_indexes[0]; //!< variable length
} _PACKED4_ cfg_net_key_list_t, *cfg_net_key_list_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_APP_KEY_ADD)];
    uint8_t key_index[3]; //!< NetKeyIndex, AppKeyIndex
    uint8_t app_key[16];
} _PACKED4_ cfg_app_key_add_t, *cfg_app_key_add_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_APP_KEY_UPDATE)];
    uint8_t key_index[3]; //!< NetKeyIndex, AppKeyIndex
    uint8_t app_key[16];
} _PACKED4_ cfg_app_key_update_t, *cfg_app_key_update_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_APP_KEY_DELETE)];
    uint8_t key_index[3]; //!< NetKeyIndex, AppKeyIndex
} _PACKED4_ cfg_app_key_delete_t, *cfg_app_key_delete_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_APP_KEY_STAT)];
    mesh_msg_stat_t stat;
    uint8_t key_index[3]; //!< NetKeyIndex, AppKeyIndex
} _PACKED4_ cfg_app_key_stat_t, *cfg_app_key_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_APP_KEY_GET)];
    uint16_t net_key_index;
} _PACKED4_ cfg_app_key_get_t, *cfg_app_key_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_APP_KEY_LIST)];
    mesh_msg_stat_t stat;
    uint16_t net_key_index;
    uint8_t app_key_indexes[0]; //!< variable length
} _PACKED4_ cfg_app_key_list_t, *cfg_app_key_list_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NODE_IDENTITY_GET)];
    uint16_t net_key_index;
} _PACKED4_ cfg_node_identity_get_t, *cfg_node_identity_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NODE_IDENTITY_SET)];
    uint16_t net_key_index;
    uint8_t identity;
} _PACKED4_ cfg_node_identity_set_t, *cfg_node_identity_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NODE_IDENTITY_STAT)];
    mesh_msg_stat_t stat;
    uint16_t net_key_index;
    uint8_t identity;
} _PACKED4_ cfg_node_identity_stat_t, *cfg_node_identity_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_APP_BIND)];
    uint16_t element_addr;
    uint16_t app_key_index;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_app_bind_t, *cfg_model_app_bind_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_APP_UNBIND)];
    uint16_t element_addr;
    uint16_t app_key_index;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_app_unbind_t, *cfg_model_app_unbind_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_MODEL_APP_STAT)];
    mesh_msg_stat_t stat;
    uint16_t element_addr;
    uint16_t app_key_index;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_app_stat_t, *cfg_model_app_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_SIG_MODEL_APP_GET)];
    uint16_t element_addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
} _PACKED4_ cfg_model_app_get_t, *cfg_model_app_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_SIG_MODEL_APP_LIST)];
    mesh_msg_stat_t stat;
    uint16_t element_addr;
    uint32_t model_id; //!< 2 bytes or 4 bytes
    uint8_t app_key_indexes[3]; //!< variable length
} _PACKED4_ cfg_model_app_list_t, *cfg_model_app_list_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NODE_RESET)];
} _PACKED4_ cfg_node_reset_t, *cfg_node_reset_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_NODE_RESET_STAT)];
} _PACKED4_ cfg_node_reset_stat_t, *cfg_node_reset_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_FRND_GET)];
} _PACKED4_ cfg_frnd_get_t, *cfg_frnd_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_FRND_SET)];
    uint8_t state;
} _PACKED4_ cfg_frnd_set_t, *cfg_frnd_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_FRND_STAT)];
    uint8_t state;
} _PACKED4_ cfg_frnd_stat_t, *cfg_frnd_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_LPN_POLL_TO_GET)];
    uint16_t lpn_addr;
} _PACKED4_ cfg_lpn_poll_timeout_get_t, *cfg_lpn_poll_timeout_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_LPN_POLL_TO_STAT)];
    uint16_t lpn_addr;
    uint8_t poll_to[3];
} _PACKED4_ cfg_lpn_poll_timeout_stat_t, *cfg_lpn_poll_timeout_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_KEY_REFRESH_PHASE_GET)];
    uint16_t net_key_index;
} _PACKED4_ cfg_key_refresh_phase_get_t, *cfg_key_refresh_phase_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_KEY_REFRESH_PHASE_SET)];
    uint16_t net_key_index;
    uint8_t state;
} _PACKED4_ cfg_key_refresh_phase_set_t, *cfg_key_refresh_phase_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_KEY_REFRESH_PHASE_STAT)];
    mesh_msg_stat_t stat;
    uint16_t net_key_index;
    uint8_t state;
} _PACKED4_ cfg_key_refresh_phase_stat_t, *cfg_key_refresh_phase_stat_p;

//RTK porting:add new structure for call app layer and callback src
typedef struct {
	uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_KEY_REFRESH_PHASE_STAT)];
	mesh_msg_stat_t stat;
	uint16_t net_key_index;
	uint8_t state;
	uint16_t src;
} _PACKED4_ cfg_key_refresh_phase_stat_t_call;

typedef struct {
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_HB_PUB_GET)];
} _PACKED4_ cfg_hb_pub_get_t, *cfg_hb_pub_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_HB_PUB_SET)];
    uint16_t dst;
    uint8_t count_log;
    uint8_t period_log;
    uint8_t ttl;
    hb_pub_features_t features;
    uint16_t net_key_index;
} _PACKED4_ cfg_hb_pub_set_t, *cfg_hb_pub_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_HB_PUB_STAT)];
    mesh_msg_stat_t stat;
    uint16_t dst;
    uint8_t count_log;
    uint8_t period_log;
    uint8_t ttl;
    hb_pub_features_t features;
    uint16_t net_key_index;
} _PACKED4_ cfg_hb_pub_stat_t, *cfg_hb_pub_stat_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_HB_SUB_GET)];
} _PACKED4_ cfg_hb_sub_get_t, *cfg_hb_sub_get_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_HB_SUB_SET)];
    uint16_t src;
    uint16_t dst;
    uint8_t period_log;
} _PACKED4_ cfg_hb_sub_set_t, *cfg_hb_sub_set_p;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_CFG_HB_SUB_STAT)];
    mesh_msg_stat_t stat;
    uint16_t src;
    uint16_t dst;
    uint8_t period_log;
    uint8_t count_log;
    uint8_t min_hops;
    uint8_t max_hops;
} _PACKED4_ cfg_hb_sub_stat_t, *cfg_hb_sub_stat_p;

/** @} */

/**
 * @defgroup CONFIGURATION_SERVER_API Server API
 * @brief Functions declaration
 * @{
 */

/**
  * @brief register the configuration server. It is invoked by the stack itself.
  * @return operation result
  * @retval true: success
  * @retval false: fail
  */
bool cfg_server_reg(void);

/**
  * @brief the receive callback of the configuration server model
  * @param[in] pmesh_msg: the received mesh message
  * @return operation result
  * @retval true: the msg is processed by the configuration server model
  * @retval false: the msg isn't processed by the configuration server model
  */
bool cfg_server_receive(mesh_msg_p pmesh_msg);

/**
  * @brief the cfg server may response all status msgs via segmented transport msgs
  *        to add the reliability of the response
  * @param[in] seg: the seg flag value
  * @return none
  */
void cfg_server_resp_with_seg_msg(bool seg);

/**
  * @brief the cfg server may response need to use separated network transmit
  * @param[in] count: retransmit count
  * @param[in] steps: retransmit steps
  * @return none
  */
void cfg_server_set_net_trans(uint8_t count, uint8_t steps);

/*
 * @brief The configuration server model is implemented in the mesh stack.
 * But the application can still get the configuration message from the server
 * by patch the model receive function. For example, the application can attain
 * the source address of the configuration client.
 *
 * <b>Example usage</b>
 * \code{.c}
    bool cfg_server_receive_peek(mesh_msg_p pmesh_msg)
    {
        bool ret = cfg_server_receive(pmesh_msg);
        if(ret)
        {
            // The configuration client message can be peeked now!
            printi("cfg_server_receive_peek: cfg client addr = 0x%04x", pmesh_msg->src);}
        }
        return ret;
    }

    void mesh_stack_init(void)
    {
        ...
        mesh_element_create(GATT_NS_DESC_UNKNOWN);
        cfg_server.model_receive = cfg_server_receive_peek;
        ...
    }
 * \endcode
 */
typedef struct _mesh_model_info_t mesh_model_info_t;
extern mesh_model_info_t cfg_server;

/** @} */

/**
 * @defgroup CONFIGURATION_CLIENT_API Client API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief register configuration client model
 *
 * @return true
 * @return false
 */
bool cfg_client_reg(void);

/**
 * @brief configuration client key set
 *
 * @param[in] key_index: key index
 * @return true
 * @return false
 */
bool cfg_client_key_set(uint16_t key_index);

/**
 * @brief config composition data get
 *
 * @param[in] dst: destination address
 * @param[in] page: page
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_compo_data_get(uint16_t dst, uint8_t page);

/**
 * @brief config beacon get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_beacon_get(uint16_t dst);

/**
 * @brief config beacon set
 *
 * @param[in] dst: destination address
 * @param[in] state: state
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_beacon_set(uint16_t dst, uint8_t state);

/**
 * @brief config default ttl get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_default_ttl_get(uint16_t dst);

/**
 * @brief config default ttl set
 *
 * @param[in] dst: destination address
 * @param[in] ttl: ttl
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_default_ttl_set(uint16_t dst, uint8_t ttl);

/**
 * @brief config proxy get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_proxy_get(uint16_t dst);

/**
 * @brief config proxy set
 *
 * @param[in] dst: destination address
 * @param[in] state: state
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_proxy_set(uint16_t dst, uint8_t state);

/**
 * @brief config relay get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_relay_get(uint16_t dst);

/**
 * @brief config relay set
 *
 * @param[in] dst: destination address
 * @param[in] state: state
 * @param[in] count: count
 * @param[in] steps: steps
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_relay_set(uint16_t dst, uint8_t state, uint8_t count, uint8_t steps);

/**
 * @brief config network transmit get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_net_transmit_get(uint16_t dst);

/**
 * @brief config network transmit set
 *
 * @param[in] dst: destination address
 * @param[in] count: number of transmissions for each Network PDU originating from the node
 * @param[in] steps: number of 10-millisecond steps between transmissions
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_net_transmit_set(uint16_t dst, uint8_t count, uint8_t steps);

/**
 * @brief config model publication get
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_pub_get(uint16_t dst, uint16_t element_addr, uint32_t model_id);

/**
 * @brief config model publication set
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] va_flag: virtual address flag
 * @param[in] pub_addr: pointer to publication address
 * @param[in] pub_key_info: publication key information
 * @param[in] pub_ttl: publication ttl
 * @param[in] pub_period: publication period
 * @param[in] pub_retrans_info: publication retrans information
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_pub_set(uint16_t dst, uint16_t element_addr, bool va_flag,
                                        uint8_t *pub_addr, pub_key_info_t pub_key_info, uint8_t pub_ttl, pub_period_t pub_period,
                                        pub_retrans_info_t pub_retrans_info, uint32_t model_id);

/**
 * @brief config model subscription add
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] va_flag: virtual address flag
 * @param[in] addr: pointer to subscription address
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_sub_add(uint16_t dst, uint16_t element_addr, bool va_flag,
                                        uint8_t *addr, uint32_t model_id);

/**
 * @brief config model subscription delete
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] va_flag: virtual address flag
 * @param[in] addr: pointer to subscription address
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_sub_delete(uint16_t dst, uint16_t element_addr, bool va_flag,
                                           uint8_t *addr, uint32_t model_id);

/**
 * @brief config model subscription delete all
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_sub_delete_all(uint16_t dst, uint16_t element_addr,
                                               uint32_t model_id);

/**
 * @brief config model subscription overwrite
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] va_flag: virtual address flag
 * @param[in] addr: pointer to subscription address
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_sub_overwrite(uint16_t dst, uint16_t element_addr, bool va_flag,
                                              uint8_t *addr, uint32_t model_id);

/**
 * @brief config model subscription get
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_sub_get(uint16_t dst, uint16_t element_addr, uint32_t model_id);

/**
 * @brief config NetKey add
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @param[in] net_key: NetKey
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_net_key_add(uint16_t dst, uint16_t net_key_index, uint8_t net_key[16]);

/**
 * @brief config NetKey update
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @param[in] net_key: NetKey
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_net_key_update(uint16_t dst, uint16_t net_key_index, uint8_t net_key[16]);

/**
 * @brief config NetKey delete
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_net_key_delete(uint16_t dst, uint16_t net_key_index);

/**
 * @brief config NetKey get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_net_key_get(uint16_t dst);

/**
 * @brief config AppKey add
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @param[in] app_key_index: AppKey index
 * @param[in] app_key: AppKey
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_app_key_add(uint16_t dst, uint16_t net_key_index, uint16_t app_key_index,
                                      uint8_t app_key[16]);

/**
 * @brief config AppKey update
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @param[in] app_key_index: AppKey index
 * @param[in] app_key: AppKey
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_app_key_update(uint16_t dst, uint16_t net_key_index,
                                         uint16_t app_key_index, uint8_t app_key[16]);

/**
 * @brief config AppKey delete
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @param[in] app_key_index: AppKey index
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_app_key_delete(uint16_t dst, uint16_t net_key_index,
                                         uint16_t app_key_index);

/**
 * @brief config AppKey get
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_app_key_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief config node identity get
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_node_identity_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief config node identity set
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @param[in] identity: new Node Identity state
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_node_identity_set(uint16_t dst, uint16_t net_key_index, uint8_t identity);

/**
 * @brief config model app bind
 *
 * @param[in] dst: destination address destination address
 * @param[in] element_addr: address of the element
 * @param[in] app_key_index: AppKey index
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_app_bind(uint16_t dst, uint16_t element_addr,
                                         uint16_t app_key_index,
                                         uint32_t model_id);

/**
 * @brief config model app unbind
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] app_key_index: AppKey index
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_app_unbind(uint16_t dst, uint16_t element_addr,
                                           uint16_t app_key_index,
                                           uint32_t model_id);

/**
 * @brief config model app get
 *
 * @param[in] dst: destination address
 * @param[in] element_addr: address of the element
 * @param[in] model_id: Model ID
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_model_app_get(uint16_t dst,  uint16_t element_addr, uint32_t model_id);

/**
 * @brief config node reset
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_node_reset(uint16_t dst);

/**
 * @brief config friend get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_frnd_get(uint16_t dst);

/**
 * @brief config friend set
 *
 * @param[in] dst: destination address
 * @param[in] state: new Friend state
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_frnd_set(uint16_t dst, uint8_t state);

/**
 * @brief config low power mode PollTimeout get
 *
 * @param[in] dst: destination address
 * @param[in] lpn_addr: the unicast address of the Low Power node
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_lpn_poll_timeout_get(uint16_t dst, uint16_t lpn_addr);

/**
 * @brief config key refresh phase get
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_key_refresh_phase_get(uint16_t dst, uint16_t net_key_index);

/**
 * @brief config key refresh phase set
 *
 * @param[in] dst: destination address
 * @param[in] net_key_index: NetKey index
 * @param[in] state: new Key Refresh Phase Transition
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_key_refresh_phase_set(uint16_t dst, uint16_t net_key_index,
                                                uint8_t state);

/**
 * @brief config Heartbeat publication get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_hb_pub_get(uint16_t dst);

/**
 * @brief config Heartbeat publication set
 *
 * @param[in] dst: destination address
 * @param[in] dst_pub: destination address for Heartbeat messages
 * @param[in] count_log: number of Heartbeat messages to be sent
 * @param[in] period_log: period for sending Heartbeat messages
 * @param[in] ttl: TTL to be used when sending Heartbeat messages
 * @param[in] features: bit field indicating features that trigger Heartbeat messages when changed
 * @param[in] net_key_index: NetKey index
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_hb_pub_set(uint16_t dst, uint16_t dst_pub, uint8_t count_log,
                                     uint8_t period_log,
                                     uint8_t ttl,
                                     hb_pub_features_t features, uint16_t net_key_index);

/**
 * @brief config Heartbeat subscription get
 *
 * @param[in] dst: destination address
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_hb_sub_get(uint16_t dst);

/**
 * @brief config Heartbeat subscription set
 *
 * @param[in] dst: destination address
 * @param[in] src: source address for Heartbeat messages
 * @param[in] dst_set: destination address for Heartbeat messages
 * @param[in] period_log: period for receiving Heartbeat messages
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t cfg_hb_sub_set(uint16_t dst, uint16_t src, uint16_t dst_set,
                                     uint8_t period_log);
/** @} */
/** @} */

END_DECLS

#endif /* _CONFIGURATION_H */
