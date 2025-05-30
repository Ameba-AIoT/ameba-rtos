/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     mesh_node.h
  * @brief    Head file for mesh node management.
  * @details  Data types and external functions declaration.
  * @author   bill
  * @date     2016-1-20
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _MESH_NODE_H
#define _MESH_NODE_H

/* Add Includes here */
#include "platform_misc.h"
#include "mesh_config.h"

BEGIN_DECLS

/** @addtogroup Mesh_Node
  * @{
  */

/** @defgroup Mesh_Node_Exported_Types Exported Types
  * @{
  */

typedef enum
{
    MESH_ROLE_DEVICE,
    MESH_ROLE_PROVISIONER
} mesh_role_t;

enum
{
    UNPROV_DEVICE,
    PROV_NODE
} _SHORT_ENUM_;
typedef uint8_t mesh_node_state_t;

/*
 * used in composition data page0 (little endian) & heartbeat msg (big endian)
 */
typedef struct
{
    uint16_t relay: 1;
    uint16_t proxy: 1;
    uint16_t fn: 1;
    uint16_t lpn: 1;
    uint16_t rfu: 12;
} _PACKED4_ mesh_features_t, *mesh_features_p;

typedef struct
{
    //uint8_t element_num;
    //uint8_t model_num;
    uint16_t dev_key_num;
    uint16_t net_key_num;
    uint16_t master_key_num; //!< the number of master key, shall <= net_key_num
    uint16_t app_key_num;
    uint16_t vir_addr_num; //!< the number of virtual address
    uint16_t rpl_num; //!< the number of replay protection list entries
    uint16_t sub_addr_num; //!< the number of subscribe address per model
    uint8_t proxy_num; //!< the number of proxy server, only support at most one now

    uint16_t udb_interval; //!< unit: 100 millisecond, 0: use stack default value
    uint16_t snb_interval; //!< unit: 100 millisecond, 0: use stack default value
    uint16_t prov_interval; //!< unit: 100 millisecond, 0: use stack default value
    uint16_t proxy_interval; //!< unit: 100 millisecond, 0: use stack default value
    uint16_t identity_interval; //!< unit: 100 millisecond, 0: use stack default value
#if MESH_PRB
    uint16_t prb_interval; //!< unit: 100 millisecond, 0: use stack default value
    uint32_t prb_random_update_interval; //!< unit: 100 millisecond, 0: use stack default value
#endif
#if MESH_SBR
    uint16_t bridging_table_size; //!< the number bridging table
#endif
#if MESH_DF
    uint8_t dependent_addr_size;
    uint16_t df_fixed_path_size;
#endif
} mesh_node_cfg_t, *mesh_node_cfg_p;

typedef struct
{
    uint32_t used: 1; //!< flag used for flash
    uint32_t role: 1; //!< 1: provisioner, 0: node @ref mesh_role_t

    /**
     *  0x00 Feature or Service is supported but disabled
     *  0x01 Feature or Service is supported and enabled
     *  0x02 Feature or Service is not supported
     */
    uint32_t relay: 2; //!< relay
    uint32_t proxy: 2; //!< Mesh Proxy Service & Proxy feature, binding with node identity
    uint32_t fn: 2; //!< friend feature
    uint32_t lpn: 2; //!< low power feature
    uint32_t prov: 2; //!< 1: start sending prov adv (pb-gatt) at first

    uint32_t udb: 1; //!< 1: start udb at first
    uint32_t snb: 1; //!< state can be changed by Config Beacon Set
    uint32_t bg_scan: 1; //!< 1: start background scan at first

    uint32_t cfg_model_use_app_key: 1;//!< 1: can use app key in configuration models

    uint32_t flash: 1; //!< enable/disable flash storage
    uint32_t flash_rpl: 1; //!< rpl has an cascade flash storage control above flash bit

    uint32_t prb : 1; //!< state can be changed by private beacon set
    uint32_t private_proxy: 2; //!< Mesh Private Proxy

    uint32_t sbr : 1; //!< state can be changed by subnet bridge set

    uint32_t df : 1; //!< enable or disable directed forwarding feature

    uint32_t rfu: 9;
} mesh_node_features_t;

/** @defgroup Mesh_Address Mesh Address
  * @brief  Mesh Address definition.
  * @{
  */
typedef enum
{
    MESH_ADDR_TYPE_UNASSIGNED,
    MESH_ADDR_TYPE_UNICAST,
    MESH_ADDR_TYPE_VIRTUAL,
    MESH_ADDR_TYPE_GROUP
} mesh_addr_type_t;

#define MESH_UNICAST_ADDR_MASK                  0x7FFF
#define MESH_VIRTUAL_ADDR_MASK                  0x3FFF
#define MESH_GROUP_ADDR_MASK                    0x3FFF

#define MESH_UNASSIGNED_ADDR                    0x0000
#define MESH_GROUP_ADDR_DYNAMIC_START           0xC000
#define MESH_GROUP_ADDR_DYNAMIC_END             0xFEFF
#define MESH_GROUP_ADDR_FIX_START               0xFF00
#define MESH_GROUP_ADDR_ALL_DF                  0xFFFB
#define MESH_GROUP_ADDR_ALL_PROXY               0xFFFC
#define MESH_GROUP_ADDR_ALL_FRND                0xFFFD
#define MESH_GROUP_ADDR_ALL_RELAY               0xFFFE
#define MESH_GROUP_ADDR_ALL_NODE                0xFFFF

#define MESH_IS_UNASSIGNED_ADDR(addr)           ((addr) == MESH_UNASSIGNED_ADDR)
#define MESH_NOT_UNASSIGNED_ADDR(addr)          ((addr) != MESH_UNASSIGNED_ADDR)
#define MESH_IS_UNICAST_ADDR(addr)              ((addr) != MESH_UNASSIGNED_ADDR && ((addr) & 0x8000) == 0x0000)
#define MESH_NOT_UNICAST_ADDR(addr)             ((addr) == MESH_UNASSIGNED_ADDR || ((addr) & 0x8000) != 0x0000)
#define MESH_IS_VIRTUAL_ADDR(addr)              (((addr) & 0xC000) == 0x8000)
#define MESH_NOT_VIRTUAL_ADDR(addr)             (((addr) & 0xC000) != 0x8000)
#define MESH_IS_GROUP_ADDR(addr)                (((addr) & 0xC000) == 0xC000)
#define MESH_NOT_GROUP_ADDR(addr)               (((addr) & 0xC000) != 0xC000)
#define MESH_IS_RFU_GROUP_ADDR(addr)            ((addr) >= MESH_GROUP_ADDR_FIX_START && (addr) < MESH_GROUP_ADDR_ALL_DF)
#define MESH_IS_BROADCAST_ADDR(addr)            ((addr) == MESH_GROUP_ADDR_ALL_NODE)
#define MESH_NOT_BROADCAST_ADDR(addr)           ((addr) != MESH_GROUP_ADDR_ALL_NODE)
#define MESH_IS_SUBSCRIBE_ADDR(addr)            ((addr) != MESH_GROUP_ADDR_ALL_NODE && ((addr) & 0x8000) == 0x8000)
#define MESH_NOT_SUBSCRIBE_ADDR(addr)           ((addr) == MESH_GROUP_ADDR_ALL_NODE || ((addr) & 0x8000) != 0x8000)
#define MESH_IS_MY_ADDR(addr)                   ((addr) >= mesh_node.unicast_addr && (addr) - mesh_node.unicast_addr < mesh_node.element_queue.count)
#define MESH_NOT_MY_ADDR(addr)                  ((addr) < mesh_node.unicast_addr || (addr) - mesh_node.unicast_addr >= mesh_node.element_queue.count)

typedef struct
{
    union
    {
        struct
        {
            uint16_t range_start : 15;
            uint16_t len_present : 1;
        };
        struct
        {
            uint16_t len_present_access : 1;
            uint16_t range_start_access : 15;
        };
        uint16_t addr;
    };
    uint8_t range_len;
} _PACKED4_ mesh_addr_range_t;

typedef struct _mesh_addr_member_t
{
    struct _mesh_addr_member_t *pnext;
    uint16_t mesh_addr;
} mesh_addr_member_t, *mesh_addr_member_p;
/** @} End of Mesh_Address */

/** @defgroup Model Model
  * @brief  types used for model.
  * @{
  */

/** @defgroup Model_ID Model ID
  * @brief
  * @{
  */
#define MESH_IS_SIG_MODEL(model_id)             (((model_id) & 0xffff) == 0xffff)
#define MESH_IS_VENDOR_MODEL(model_id)          (((model_id) & 0xffff) != 0xffff)
/* from spec to implement */
#define MESH_MODEL_TRANSFORM(model_id)          (0xffff + ((model_id) << 16)) //LE_EXTRN2WORD((uint8_t *)&(model_id))
/* from implement to spec */
#define MESH_MODEL_CONVERT(model_id)            (MESH_IS_SIG_MODEL(model_id) ? ((model_id) >> 16) : (model_id))
/** @} End of Model_ID */

typedef struct _mesh_msg_t *mesh_msg_p;
typedef struct _mesh_model_info_t *mesh_model_info_p;
typedef enum _mesh_msg_send_stat_t mesh_msg_send_stat_t;
typedef bool (*model_receive_pf)(mesh_msg_p pmesh_msg);
typedef int32_t (*model_pub_cb_pf)(mesh_model_info_p pmodel_info, bool retrans);
typedef void (*model_send_cb_pf)(mesh_model_info_p pmodel_info, mesh_msg_send_stat_t stat,
                                 uint32_t access_opcode);
typedef int32_t (*model_data_cb_pf)(const mesh_model_info_p pmodel_info, uint32_t type,
                                    void *pargs);
#if MESH_MODEL_ENABLE_DEINIT
typedef void (*model_deinit_cb_pf)(mesh_model_info_p pmodel_info);
#endif
typedef struct _mesh_model_info_t
{
    /** provided by application */
    uint32_t model_id; //!< being equal or greater than 0xffff0000 means that the model is a sig model.
    /** callback to receive related access msg
        If the model don't recognise the access opcode, it should return false! */
    model_receive_pf model_receive;
    model_send_cb_pf model_send_cb; //!< indicates the msg transmitted state
    model_pub_cb_pf model_pub_cb; //!< indicates it is time to publishing
    model_data_cb_pf model_data_cb;
#if MESH_MODEL_ENABLE_DEINIT
    model_deinit_cb_pf model_deinit;
#endif
    /** point to the bound model, sharing the subscription list with the binding model */
    struct _mesh_model_info_t *pmodel_bound;
    struct
    {
        uint8_t corresponding_present : 1;
        uint8_t format : 1;
        uint8_t extended_item_count : 6;
        uint8_t corresponding_group_id;
        struct _mesh_model_info_t *pmodel_extended_info;
    };
    /** configured by stack */
    uint8_t element_index;
    uint8_t model_index;
    void *pelement;
    void *pmodel;
    void *pargs;
} mesh_model_info_t;

typedef struct
{
    uint16_t pub_addr;
    pub_key_info_t pub_key_info;
    uint8_t pub_ttl;
    pub_period_t pub_period;
    pub_retrans_info_t pub_retrans_info;
#if MESH_DF
    /* publish policy */
    uint8_t pub_policy; //!< @ref directed_publish_policy_t
#endif
} mesh_model_pub_params_t;

typedef struct _mesh_model_t
{
    struct _mesh_model_t *pnext;
    mesh_model_info_p pmodel_info;
    uint8_t model_index; //!< global index
    mesh_model_pub_params_t pub_params;
    plt_timer_t pub_timer;
    uint8_t retrans_count; //!< counter
    //uint16_t app_key_index; //key maybe deleted
    uint8_t *app_key_binding; //!< dumm...
    plt_list_t sub_queue; //!< subscribe addr list
} mesh_model_t, *mesh_model_p;

/** @} End of Model */

/** @defgroup Element Element
  * @brief
  * @{
  */
typedef enum
{
    GATT_NS_DESC_UNKNOWN = 0x0000,
    GATT_NS_DESC_FIRST = 0x0001,
    GATT_NS_DESC_SECOND = 0x0002,
    GATT_NS_DESC_THIRD = 0x0003,
    GATT_NS_DESC_255 = 0x00ff,
    GATT_NS_DESC_FRONT = 0x0100,
    GATT_NS_DESC_BACK = 0x0101,
    GATT_NS_DESC_TOP = 0x0102,
    GATT_NS_DESC_BOTTOM = 0x0103,
    GATT_NS_DESC_UPPER = 0x0104,
    GATT_NS_DESC_LOWER = 0x0105,
    GATT_NS_DESC_MAIN = 0x0106,
    GATT_NS_DESC_BACKUP = 0x0107,
    GATT_NS_DESC_AUXILIARY = 0x0108,
    GATT_NS_DESC_SUPPLEMENTARY = 0x0109,
    GATT_NS_DESC_FLASH = 0x010A,
    GATT_NS_DESC_INSIDE = 0x010B,
    GATT_NS_DESC_OUTSIDE = 0x010C,
    GATT_NS_DESC_LEFT = 0x010D,
    GATT_NS_DESC_RIGHT = 0x010E,
    GATT_NS_DESC_INTERNAL = 0x010F,
    GATT_NS_DESC_EXTERNAL = 0x0110
} gatt_ns_desc_t; //!< GATT namespace description

typedef void (*mesh_attn_cb_pf)(uint8_t element_index, uint32_t timer_ms);

typedef struct _mesh_element_t
{
    struct _mesh_element_t *pnext;
    uint8_t element_index;
    uint16_t loc; //!< element location @ref gatt_ns_desc_t
    plt_list_t model_queue; //!< @ref mesh_model_t
    uint32_t attn_timer; //!< in unit of millisecond
} mesh_element_t, *mesh_element_p;
/** @} End of Element */

#define MESH_COMMON_KEY_SIZE        16
typedef struct
{
    uint8_t key[MESH_COMMON_KEY_SIZE];
} mesh_key_t, *mesh_key_p;

typedef enum
{
    MESH_KEY_STATE_INVALID, //!< invalid state
    /** current 0, old 0, new 1 */
    MESH_KEY_STATE_NORMAL1, //!< phase 0: Normal state, Tx is the same as Rx
    MESH_KEY_STATE_UPDATE1, //!< phase 1: Tx old, Rx old & new
    MESH_KEY_STATE_REFRESH1, //!< phase 2: Tx new, Rx old & new
    /** current 1, old 1, new 0 */
    MESH_KEY_STATE_NORMAL2, //!< Normal state, Tx is the same as Rx
    MESH_KEY_STATE_UPDATE2, //!< Tx old, Rx old & new
    MESH_KEY_STATE_REFRESH2 //!< Tx new, Rx old & new
} mesh_key_state_t;

typedef struct
{
    /** origin NetKey */
    uint8_t net_key[MESH_COMMON_KEY_SIZE];
    /** keys used to secure nwk pdu */
    uint8_t nid; //!< 7 bit value
    uint8_t encrypt_key[MESH_COMMON_KEY_SIZE];
    uint8_t privacy_key[MESH_COMMON_KEY_SIZE];
    /** keys used in snb & proxy adv */
    uint8_t net_id[8];
    uint8_t beacon_key[MESH_COMMON_KEY_SIZE];
    uint8_t identity_key[MESH_COMMON_KEY_SIZE];
#if MESH_PRB
    /** keys used in prb */
    uint8_t private_beacon_key[MESH_COMMON_KEY_SIZE];
#endif
} net_key_t, *net_key_p;

typedef struct
{
    mesh_key_state_t key_state;
    uint8_t identity; //!< Binding with GATT Proxy state
#if MESH_PRB
    uint8_t private_identity; //!< Binding with GATT Private Proxy state
#endif
    uint16_t net_key_index_g; //!< index of global NetKey list, bit15 represent whether it is a frnd key
    net_key_p pnet_key[2];
    uint8_t rcvd_snb_count;
    uint16_t start_obs_point;
    uint16_t snb_interval;
    uint16_t last_snb_point;
} net_key_list_t, *net_key_list_p;

/* network key index flag */
#define NET_KEY_INDEX_FLAG_MASK            0xF000

#define NET_KEY_INDEX_FLAG_MASTER          0x0000
#define NET_KEY_INDEX_FLAG_FRND            0x8000
#define NET_KEY_INDEX_FLAG_DF              0x4000

typedef enum
{
    NET_KEY_TYPE_INVALID = 0x00,
    NET_KEY_TYPE_MASTER = 0x01,   // managed flooding security material
    NET_KEY_TYPE_FRND = 0x02,     // friendship security material
#if MESH_DF
    NET_KEY_TYPE_DF = 0x03,       // directed security material
#endif
} net_key_type_t;

#if MESH_DF
enum
{
    DF_TAG_TYPE_DEFAULT,               //!< default, use the df key if the path exists, otherwise use the master key
    DF_TAG_TYPE_USE_DIRECTED,          //!< use directed, key type may be changed and df path init may start
    DF_TAG_TYPE_IMMUTABLE_CREDENTIALS, //!< immutable credentials, key type shall be not changed
} _SHORT_ENUM_;
typedef uint8_t df_tag_t;
#endif

typedef struct
{
    uint8_t app_key[MESH_COMMON_KEY_SIZE];
    uint8_t aid; //!< the least significant 6 bits
} app_key_t, *app_key_p;

typedef struct
{
    mesh_key_state_t key_state;
    uint16_t net_key_binding; //!< index of local NetKey list
    uint16_t app_key_index_g; //!< index of global AppKey list
    app_key_p papp_key[2];
} app_key_list_t, *app_key_list_p;

typedef struct
{
    uint16_t unicast_addr;
    uint8_t element_num;
    uint8_t used;
    uint8_t dev_key[MESH_COMMON_KEY_SIZE];
} _PACKED4_ dev_key_t;

typedef struct
{
    uint16_t vir_addr; //!< used when bigger than 0
    uint16_t count; //!< in case reuse
    uint8_t label_uuid[MESH_COMMON_KEY_SIZE];
} _PACKED4_ vir_addr_t;

#define MESH_COMPO_DATA_PAGE128   3
#define MESH_COMPO_DATA_PAGE_NUM  4

/** @defgroup Compo_Data_Page0 Composition Data Page 0
  * @brief
  * @{
  */
typedef struct
{
    uint16_t company_id;
    uint16_t product_id;
    uint16_t version_id;
    //uint16_t rpl_num; //!< the number of replay protection list entries
    //mesh_features_t features;
} _PACKED4_ compo_data_page0_header_t, *compo_data_page0_header_p;
/** @} End of Compo_Data_Page0 */

/** @defgroup Compo_Data_Page2 Composition Data Page 2
  * @brief
  * @{
  */
enum
{
    MESH_PROFILE_NLC_AMBIENT_LIGHT_SENSOR_V_1_0       = 0X1600,
    MESH_PROFILE_NLC_BASIC_LIGHTNESS_CONTROLLER_V_1_0 = 0X1601,
    MESH_PROFILE_NLC_BASIC_SCENE_SELECTOR_V_1_0       = 0X1602,
    MESH_PROFILE_NLC_DIMMING_CONTROL_V_1_0            = 0X1603,
    MESH_PROFILE_NLC_ENERGY_MONITOR_V_1_0             = 0X1604,
    MESH_PROFILE_NLC_OCCUPANCY_SENSOR_V_1_0           = 0X1605,
};
typedef uint16_t mesh_profile_identifier_t;

typedef struct
{
    uint8_t version_x;  // Specification major revision number
    uint8_t version_y;  // Specification minor revision number
    uint8_t version_z;  // Specification .Z revision number
} _PACKED4_ mesh_profile_version_t, *mesh_profile_version_p;

/** @} End of Compo_Data_Page2 */

#define MESH_MSG_RESERVED_NET_TRANS_COUNT         0xFF
#define MESH_MSG_RESERVED_NET_TRANS_STEPS         0xFF
#define MESH_MSG_RESERVED_DELAY_TIME              0xFFFFFFFF

typedef struct _mesh_msg_t
{
    mesh_model_info_p pmodel_info; //!< indicate which model is sending/receiving this msg
    uint8_t *pbuffer; //!< pointer to the msg
    uint16_t msg_offset;
    uint16_t msg_len;
    uint32_t access_opcode; //!< opcode of access layer msg used when rx, access opcode is parsed by stack
    uint16_t src;
    uint16_t dst;
    uint32_t seq;
    uint32_t iv_index;
    uint8_t ttl: 7; //!< Time To Live
    uint8_t ctl: 1; //!< Access/Control Message Type & NetMIC Size Indicator
    uint8_t aid: 6; //!< Least significant 6 bits of application key identifier
    uint8_t akf: 1; //!< Application Key Flag
    uint8_t seg: 1; //!< Transport Segment Flag
    uint8_t trans_opcode: 7; //!< Opcode of the Transport Control message
    uint8_t szmic: 1; //!< Size of TransMIC bit
    uint8_t net_key_loop: 1; //!< old or new key, used in lpn frnd update
    uint8_t frnd_flag: 1; //!< controlling the credentials when publish by lpn
    uint8_t rfu : 6;
    uint8_t trans_mic[8]; //!< 4 or 8 bytes according to szmic field
    uint16_t app_key_index;
    uint16_t net_key_index;
    int8_t rssi; //!< used when rx in adv bearer
    uint8_t net_trans_count;
    uint8_t net_trans_steps;
    uint32_t delay_time; //!< message send delay time, unit is ms
    bearer_field_t bearer_field; //!< indicate which bearers sending to, or which bearer receiving from
    gap_sched_link_t link; //!< indicate which links sending to, or which link receiving from
    net_key_type_t net_key_type; //!< indicate which network key used to send or received from
#if MESH_DF
    df_tag_t df_tag; //!< directed forwarding tag
#endif
} mesh_msg_t;

typedef struct
{
    /** network keys */
    net_key_list_p net_key_list;
    uint16_t net_key_num; //!< 12 valid bits, sum of master key, df key, frnd key
    uint16_t master_key_num; //!< save for flooding key
    uint16_t frnd_key_num; //!< save for frnd
    /** application keys */
    app_key_list_p app_key_list;
    uint16_t app_key_num; //!< 12 valid bits
    /** device key */
    dev_key_t *dev_key_list;
    uint16_t dev_key_num; //!< 15 valid bits
    /** virtual address */
    vir_addr_t *vir_addr_list;
    uint16_t vir_addr_num; //!< 14 valid bits
    /** misc parameters */
    uint32_t seq; //!< 3 bytes used, unique per node
    uint32_t seq_siv; //!< default value @ref MESH_SEQ_START_IV_UPDATE
    uint32_t iv_index;
    plt_timer_t iv_timer;
    uint32_t iv_timer_count;
    uint8_t ttl;
    uint8_t dev_uuid[16];
    mesh_node_state_t node_state;
    mesh_node_features_t features;
    uint16_t udb_interval; //!< 100ms
    uint16_t snb_interval; //!< 100ms
    uint16_t prov_interval; //!< 100ms
    uint16_t proxy_interval; //!< 100ms
    uint16_t identity_interval; //!< 100ms
    uint16_t identity_credit; //!< identity times in each interval
#if MESH_PRB
    uint16_t prb_interval; //!< unit: 100 millisecond, 0: use stack default value
    uint32_t prb_random_update_interval; //!< unit: 1 second, 0: use stack default value
    uint16_t private_proxy_interval; //!< 100ms
    uint16_t private_identity_interval; //!< 100ms
#endif
    uint16_t unicast_addr;
    plt_list_t element_queue; //!< @ref mesh_element_t
    uint8_t model_num;
    uint16_t sub_addr_num; //!< per model
    uint8_t *compo_data[MESH_COMPO_DATA_PAGE_NUM];      //!< page0 : 0, page1 : 1, page2 : 2, page128 : MESH_COMPO_DATA_PAGE128
    uint16_t compo_data_size[MESH_COMPO_DATA_PAGE_NUM];
    /** element attention */
    plt_timer_t attn_timer;
    uint32_t attn_interval; //!< ms, range of 10ms ~ 1000ms
    mesh_attn_cb_pf attn_cb;
    /** bearer parameters */
    uint8_t pb_adv_delay_min; //!< ms, default value 0ms
    uint8_t pb_adv_delay_max; //!< ms, default value 0ms
    uint8_t pb_adv_msg_ack_delay_min; //!< ms, default value 20ms
    uint8_t pb_adv_msg_ack_delay_max; //!< ms, default value 50ms
    uint8_t pb_adv_retrans_count; //!< transmit (pb_adv_retrans_count + 1) times
    uint8_t pb_adv_retrans_steps; //!< retransmission interval = (pb_adv_retrans_steps + 1) * 10ms
    /** net parameters */
    uint8_t net_trans_count_base; //!< can't be modified by the cfg client
    uint8_t net_trans_count; //!< :3 transmit (net_trans_count + net_trans_count_base + 1) times
    uint8_t net_trans_steps; //!< :5 retransmission interval = (net_trans_steps + 1) * 10ms
    uint32_t net_delay_time_min; //!< ms, default value 0ms
    uint32_t net_delay_time_max; //!< ms, default value 0ms, shall not less than net_delay_time_min
    uint8_t relay_retrans_count_base; //!< can't be modified by the cfg client
    uint8_t relay_retrans_count; //!< :3 transmit (relay_retrans_count + relay_retrans_count_base + 1) times
    uint8_t relay_retrans_steps; //!< :5 retransmission interval = (relay_retrans_steps + 1) * 10ms
    uint32_t relay_delay_time_min; //!< ms, default value 0ms
    uint32_t relay_delay_time_max; //!< ms, default value 0ms, shall not less than relay_delay_time_min
    uint16_t nmc_size; //!< net msg cache size default value @ref MESH_NET_MSG_CACHE_SIZE
    uint8_t relay_parallel_num;
    uint8_t relay_parallel_max;  //!< default 3
    /** trans parameters */
    uint8_t trans_tx_queue_size; //!< the cache size of tx seg msgs, default value @ref MESH_TRANS_TX_QUEUE_SIZE
    uint8_t trans_rx_ctx_count; //!< the count of seg msgs rx simultaneously, default value @ref MESH_TRANS_RX_CTX_COUNT
    uint8_t trans_retrans_count; //!< the retransmit times of seg msg, default value @ref MESH_TRANS_RETRANSMIT_TIMES
    uint16_t trans_retrans_base; //!< segment transmission timer base, unit is ms, default value 200ms
    uint16_t trans_retrans_ttl_factor; //!< segment transmission timer ttl factor, unit is ms, default value 50ms
    uint16_t trans_retrans_seg_factor; //!< segment transmission timer seg num factor, unit is ms, default value 30ms
    uint16_t trans_ack_base; //!< segment transmission timer base, unit is ms, default value 150ms
    uint16_t trans_ack_ttl_factor; //!< segment transmission timer ttl factor, unit is ms, default value 50ms
    uint16_t trans_ack_seg_factor; //!< segment transmission timer seg num factor, unit is ms, default value 30ms
    uint16_t trans_seg_ack_delay_min; //!< segment acknowledge send delay minimum time, unit is ms, default value 30ms
    uint16_t trans_seg_ack_delay_max; //!< segment acknowledge send delay maximum time, unit is ms, default value 30ms
    uint32_t trans_seg_delay_time_min; //!< ms, default value 0ms
    uint32_t trans_seg_delay_time_max; //!< ms, default value 0ms, shall not less than trans_seg_delay_time_min
    uint16_t tsmc_size; //!< trans seg msg cache size default value @ref MESH_TRANS_SEG_MSG_CACHE_SIZE
    uint8_t trans_tx_seg_credit; //!< trans tx seg credit, valid when GAP_SCHED_ADV_PARALLEL on, default value @ref MESH_TRANS_TX_SEG_CREDIT
    /** friendship parameters */
    uint8_t frnd_rx_window; //!< range: 0x01-0xFF ms (default 20ms), set by the fn
    uint8_t frnd_rx_delay; //!< range: 0x0A-0xFF ms (default 10ms), set by the lpn
    uint8_t frnd_rx_widen; //!< range: 0x00-0xFF ms (default 0ms), set by the lpn
    uint8_t frnd_tx_ahead; //!< range: 0x00-0xFF ms (default 15ms), set by the fn
    uint8_t frnd_poll_times; //!< default 1 times, set by the lpn
    uint8_t frnd_upd_times; //!< default 11 times, set by the fn
    uint16_t frnd_offer_rx_delay; //!< range: 0x01-0xFFFF ms (default 90ms), set by the lpn
    uint16_t frnd_offer_wait_period; //!< range: 0x01-0xFFFF ms (default 1100ms), set by the lpn
    uint16_t frnd_offer_choose_delay; //!< range: 0x01-0xFFFF ms (default 400ms), set by the lpn
    uint16_t frnd_poll_retry_interval; //!< range: 0x01-0xFFFF ms (default 300ms), set by the lpn
    uint8_t frnd_poll_retry_times; //!< range: 0x01-0xFF times (default 8times), set by the lpn
    uint8_t frnd_poll_failed_times; //!< range: 0x01-0xFF times (default 1times), set by the lpn
    uint32_t frnd_scan_delay_time; //!< range: 0x00-0xFFFFFFFF us (default 0 us), set by the lpn
    /** beacon params */
    union
    {
        uint16_t oob_info_alias;
        prov_oob_info_t oob_info;
    };
    uint8_t uri_hash[4];
    /** platform params */
    uint32_t flash_offset;
    uint32_t flash_size;
    /** flags */
    /* Normal Operation: IV Update Flag = 0, IV Update in Progress: IV Update Flag = 1 */
    uint8_t iv_update_flag : 1;
    uint8_t iv_pend_flag : 1; //!< pend state change from update to normal
    uint8_t iv_test_flag : 1;
    uint8_t uri_flag : 1; //!< flag to indicate URI Hash field in udb
    uint8_t adv_bearer: 1; //!< default on
    uint8_t node_uncheck_group_addr: 1; //!< default off
    uint8_t check_reprov: 1; //!< default off
    uint8_t cccd_not_check: 1; //!< default off
#if MESH_SBR
    uint16_t bridging_table_size; //!< the number bridging table
#endif
#if MESH_DF
    /* network param */
    uint8_t df_net_trans_count_base; //!< can't be modified by the df client
    uint8_t df_net_trans_count; //!< :3 transmit (df_net_trans_count + df_net_trans_count_base + 1) times
    uint8_t df_net_trans_steps; //!< :5 retransmission interval = (df_net_trans_steps + 1) * 10ms
    uint8_t df_relay_retrans_count_base; //!< can't be modified by the df client
    uint8_t df_relay_retrans_count; //!< :3 transmit (df_relay_retrans_count + df_relay_retrans_count_base + 1) times
    uint8_t df_relay_retrans_steps; //!< :5 retransmission interval = (df_relay_retrans_steps + 1) * 10ms
    uint8_t df_net_ctl_trans_count_base; //!< can't be modified by the df client
    uint8_t df_net_ctl_trans_count; //!< :3 transmit (df_net_ctl_trans_count + df_net_ctl_trans_count_base + 1) times
    uint8_t df_net_ctl_trans_steps; //!< :5 retransmission interval = (df_net_ctl_trans_steps + 1) * 10ms
    uint8_t df_relay_ctl_retrans_count_base; //!< can't be modified by the df client
    uint8_t df_relay_ctl_retrans_count; //!< :3 transmit (df_relay_ctl_retrans_count + df_relay_ctl_retrans_count_base + 1) times
    uint8_t df_relay_ctl_retrans_steps; //!< :5 retransmission interval = (df_relay_ctl_retrans_steps + 1) * 10ms
    /* path rssi threshold */
    int8_t df_default_rssi_threshold; //!< dbm
    uint8_t df_rssi_margin; //!< dbm
    /* path discovery timing */
    uint16_t path_monitoring_interval; //!< 1000ms
    uint16_t path_discovery_retry_interval; //!< 1000ms
    uint8_t path_discovery_interval; //!< 0: 5sec, 1: 30sec
    uint8_t lane_discovery_guard_interval; //!< 0: 2sec, 1: 10sec
    /* directed paths */
    uint16_t directed_node_paths; //!< minimum number of paths that the node supports when acting as a Path Origin or as a Path Target.
    uint16_t directed_relay_paths; //!< minimum number of paths that the node supports when acting as an intermediate Directed Relay node.
    uint16_t directed_proxy_paths; //!< minimum number of paths that the node supports when acting as a Directed Proxy node.
    uint16_t directed_friend_paths; //!< minimum number of paths that the node supports when acting as a Directed Friend node.
    /* dependents addr num */
    uint16_t dependent_addr_size; //!< max dependent addr num per path
    /* fixed path num */
    uint16_t df_fixed_path_size; //!< max fixed paths
    /* non-fixed path num */
    uint16_t df_non_fixed_path_size; //!< max non fixed paths
#endif
    /* configurable parameters */
#if MESH_PARAM_CONFIGURABLE
    uint16_t inner_msg_num;
    uint32_t pb_generic_timeout;
    uint32_t pb_adv_link_idle_timeout;
    uint16_t pb_adv_retry_period;
    uint32_t prov_timeout;
    uint32_t proxy_sar_timeout;
    uint8_t proxy_sar_buffer_len;
    uint8_t frnd_queue_min_ttl;
    uint8_t frnd_sub_list_size;
    uint16_t frnd_timeout_period;
    uint16_t seq_num_step;
#endif
} mesh_node_t, *mesh_node_p;

extern mesh_node_t mesh_node;

typedef enum
{
    MESH_IV_INDEX_UPDATE,
} iv_index_cb_type_t;

typedef struct
{
    uint32_t iv_index;
    bool iv_index_update_flag;
} iv_index_cb_data_t;

typedef bool (*iv_index_cb_pf)(iv_index_cb_type_t cb_type, iv_index_cb_data_t cb_data);

/** @} */

/** @defgroup Mesh_Node_Exported_Functions Exported Functions
  * @brief
  * @{
  */

/**
  * @brief set the device uuid
  * @param[in] dev_uuid: the device uuid
  * @return none
  */
void device_uuid_set(const uint8_t dev_uuid[16]);

/** @brief
  * @{
  */
mesh_addr_type_t mesh_addr_type_classify(uint16_t addr);
bool unicast_addr_range_contain(bool access, mesh_addr_range_t addr_range1,
                                mesh_addr_range_t addr_range2);
bool unicast_addr_range_overlap(bool access, mesh_addr_range_t addr_range1,
                                mesh_addr_range_t addr_range2);
bool unicast_addr_range_valid(bool access, mesh_addr_range_t addr_range);
mesh_addr_range_t unicast_addr_range_transform(bool access, uint16_t unicast_addr,
                                               uint8_t element_num);

bool fixed_group_addr_check_by_model(uint16_t addr, mesh_model_p pmodel);
bool fixed_group_addr_check_by_node(uint16_t addr);

uint16_t vir_addr_get_index(const uint8_t label_uuid[16]);
uint16_t vir_addr_get(uint16_t va_index);
bool vir_addr_available(void);
uint16_t vir_addr_add(const uint8_t label_uuid[16]);
void vir_addr_delete(uint16_t va_index);
/** @} */

/** @brief
  * @{
  */

/**
  * @brief set the device key
  * @param[in] dev_key_index: the dev key index
  * @param[in] unicast_addr: the node primary address
  * @param[in] element_num: the element num of the node
  * @param[in] pdev_key: the device key
  * @return none
  */
void dev_key_set(uint16_t dev_key_index, uint16_t unicast_addr, uint8_t element_num,
                 uint8_t *pdev_key);
/**
  * @brief get the device key
  * @param[in] dev_key_index: the dev key index
  * @param[out] pdev_key: the device key
  * @return none
  */
void dev_key_get(uint16_t dev_key_index, uint8_t *pdev_key);

/**
  * @brief get the available device key index
  * @return operation result
  * @retval >=0: the dev key index
  * @retval -1: not available
  */
int dev_key_get_available_idx(void);

/**
  * @brief find the device key of the node with the addr
  * @param[in] unicast_addr: the node primary address
  * @return operation result
  * @retval >=0: the dev key index
  * @retval -1: not found
  */
int dev_key_find(uint16_t unicast_addr);

/**
  * @brief add a device key
  * @param[in] unicast_addr: the node primary address
  * @param[in] element_num: the element num of the node
  * @param[in] pdev_key: the device key
  * @return operation result
  * @retval >=0: the dev key index
  * @retval -1: address collision
  * @retval -2: no space for the new key
  */
int dev_key_add(uint16_t unicast_addr, uint8_t element_num, uint8_t *pdev_key);

/**
  * @brief delete the device key
  * @param[in] dev_key_index: the dev key index
  * @return none
  */
void dev_key_delete(uint16_t dev_key_index);

/** @} */

/** @brief
  * @{
  */
uint8_t key_state_to_tx_loop(mesh_key_state_t key_state);
uint8_t key_state_to_new_loop(mesh_key_state_t key_state);
uint8_t key_state_to_frnd_key_loop(mesh_key_state_t key_state);
bool key_state_to_key_refresh_flag(mesh_key_state_t key_state);
uint8_t key_state_to_key_refresh_phase(mesh_key_state_t key_state);
/** @} */

/** @brief
  * @{
  */
uint16_t net_key_valid_num(void);
uint16_t net_key_index_to_global(uint16_t net_key_index);
uint16_t net_key_index_from_global(uint16_t net_key_index_g);
bool net_key_update(uint16_t net_key_index, uint16_t net_key_index_g, const uint8_t net_key[16]);
bool net_key_refresh(uint16_t net_key_index);
uint8_t net_key_refresh_check(uint16_t net_key_index, uint8_t net_key_loop, bool key_refresh_flag);
uint16_t net_key_available(void);
uint16_t net_key_add(uint16_t net_key_index_g, const uint8_t net_key[16]);
void net_key_delete(uint16_t net_key_index);
uint16_t net_key_dump(uint16_t net_key_indexes[]);
bool net_key_primary_subnet_check(void);
bool net_key_is_frnd_key(uint16_t net_key_index);
net_key_type_t net_key_type_get(uint16_t net_key_index);
uint16_t master_key_index_get(uint16_t net_key_index);
/** @} */

/** @brief
  * @{
  */
bool frnd_key_update(uint16_t frnd_key_index, uint16_t master_key_index, uint8_t p[9]);
/** @} */

/** @brief
  * @{
  */
uint16_t app_key_index_to_global(uint16_t app_key_index);
uint16_t app_key_index_from_global(uint16_t app_key_index_g);
bool app_key_update(uint16_t app_key_index, uint16_t net_key_index, uint16_t app_key_index_g,
                    const uint8_t app_key[16]);
bool app_key_refresh(uint16_t app_key_index);
uint16_t app_key_add(uint16_t net_key_index, uint16_t app_key_index_g, const uint8_t app_key[16]);
void app_key_delete(uint16_t app_key_index);
uint16_t app_key_dump(uint16_t net_key_index, uint16_t app_key_index_g[]);
/** @} */

/**
  * @brief key refresh
  * @param[in] key_refresh: step
  * @param[in] net_key_index: key index
  * @return none
  */
void mesh_key_refresh(uint8_t key_refresh, uint16_t net_key_index);

/**
  * @brief generate the composition data page 0
  * @param[in] pcompo_data_page0_header: info provided by app
  * @return operation result
  * @retval true: success
  * @retval false: fail
  */
bool compo_data_page0_gen(compo_data_page0_header_p pcompo_data_page0_header);
bool compo_data_page128_gen(compo_data_page0_header_t *pcompo_data_page0_header);
bool compo_data_page128_valid(void);
bool compo_data_page128_to_page0(void);

/**
 * @brief generate composition data page 1 about the relationships among models
 *
 * @return true
 * @return false
 */
bool compo_data_page1_gen(void);

/**
 * @brief add mesh profile entry to composition data page 2
 *
 * @param[in] identifier: Mesh profile UUID identifying the supported mesh profile specification
 * @param[in] pversion: Version of the mesh profile specification
 * @param[in] num_element_offsets: Total number of element offsets in the Element_Offset_List field
 * @param[in] pelement_offset_list: List of element offsets containing models related to the supported mesh profile specification
 * @param[in] add_data_len: Length of the additional data
 * @param[in] padd_data: Additional data specified in a mesh profile specification that is indicated by the Mesh_Profil
 * @return true: add success
 * @return false: add fail
 */
bool compo_data_page2_entry_add(mesh_profile_identifier_t identifier,
                                mesh_profile_version_p pversion,
                                uint8_t num_element_offsets, uint8_t *pelement_offset_list,
                                uint16_t add_data_len, uint8_t *padd_data);

/** @brief
  * @{
  */
void mesh_seq_set(uint32_t seq);
void mesh_seq_clear(void);
uint32_t mesh_seq_use(void);
/** @} */

/** @brief
  * @{
  */
void gap_sched_support_hw_timer(bool enable);
/** @} */

/** @brief
  * @{
  */
/**
  * @brief get random delay for net trans
  * @return random delay time(ms)
  */
uint32_t net_random_delay_time_get(void);

/**
  * @brief get random delay for net relay
  * @return random delay time(ms)
  */
uint32_t relay_random_delay_time_get(void);

/**
  * @brief get random delay for trans seg
  * @return random delay time(ms)
  */
uint32_t trans_seg_random_delay_time_get(void);

/** @} */

/** @brief
  * @{
  */
static inline uint32_t iv_index_get_tx_value(void)
{
    return mesh_node.iv_index - (mesh_node.iv_update_flag ? 1 : 0);
}
void iv_index_cb_reg(iv_index_cb_pf p_function);
uint32_t iv_index_get(void);
void iv_index_set(uint32_t iv_index);
void iv_index_update(uint32_t iv_index, bool iv_update_flag);
uint32_t iv_index_update_try(uint32_t iv_index, bool iv_update_flag);
void iv_index_transit_to_iv_update(void);
void iv_index_transit_to_normal(void);
void iv_index_handle_timeout(void);
void iv_index_test_mode_set(bool test);
void iv_index_timer_start(void);
void iv_index_timer_stop(void);
/** @} */

void rpl_clear(void);
void rpl_clear_per_loop(uint8_t rpl_loop);
/**
 * @brief set rpl list
 * @param[in] src:      mesh address
 * @param[in] seq:      seq will be set
 * @param[in] rpl_loop: loop of rpl list
 */
void rpl_set_seq(uint16_t src, uint32_t seq, uint8_t rpl_loop);

#define MESH_RPL_TYPE_CHECK_FAIL  0x00
#define MESH_RPL_TYPE_ADD_ENTRY   0x01

typedef enum
{
    MESH_RPL_LOWER_SEQ,
    MESH_RPL_LOWER_IV_INDEX,
    MESH_RPL_LIST_FULL,
} mesh_rpl_fail_type_t;

typedef struct
{
    mesh_rpl_fail_type_t type;  // fail type
    uint8_t rpl_loop;           // loop of rpl list
    uint16_t src;               // mesh address in received msg
    uint32_t iv_index;          // iv index in received msg
    uint32_t rpl_seq;           // seq stored in rpl list
    uint32_t seq;               // seq used in received msg
    bool *p_ignore;             // ignore rpl check, true: ignore rpl check fail, receive message normally; false: don't receive the message
} rpl_check_fail_t;

typedef struct
{
    uint8_t rpl_loop;           // loop of rpl list
    uint16_t src;               // mesh address in received msg
    uint32_t seq;               // seq used in received msg
    uint16_t remain_entry_num;  // remain entry count
} rpl_add_entry_t;

/**
 * @brief rpl function type definition
 * @param[in] type:    rpl callback type
 * @param[in] pdata:   pointer to rpl callback data
 */
typedef void (*rpl_cb_t)(uint8_t type, void *pdata);

/**
 * @brief register rpl callback
 * @param[in] cb: callback
 * @return none
 */
void rpl_cb_reg(rpl_cb_t cb);

/** @brief
  * @{
  */
void attn_timer_set(uint32_t interval_ms, mesh_attn_cb_pf attn_cb);
uint8_t attn_timer_get(uint8_t element_index);
void attn_timer_start(uint8_t element_index, uint8_t second);
void attn_timer_stop(void);
void attn_timer_handle_timeout(void);
/** @} */

/** @brief
  * @{
  */

///@cond
bool mesh_node_sub_check(uint16_t addr);
void mesh_node_init(void);
void mesh_node_deinit(void);
bool mesh_node_check_reprov(uint16_t unicast_address, uint32_t iv_index, bool iv_update_flag,
                            uint16_t net_key_index_g, uint8_t key[MESH_COMMON_KEY_SIZE]);
///@endcond

/**
  * @brief configure the mesh parameters
  *
  * This function can be invoked multiple times.
  * It can be used in conjunction with mesh_flash_sign to get the flash signature.
  * @param[in] features: the supported mesh features
  * @param[in] pnode_cfg: the configurable device parameters
  * @return none
  */
void mesh_node_cfg_update(mesh_node_features_t features, mesh_node_cfg_p pnode_cfg);

/**
  * @brief provide the old parameters to migrate the flash data
  *
  * The old parameters is used to check data overflow
  * @param[in] features: the old supported mesh features
  * @param[in] pnode_cfg: the old configurable device parameters
  * @return none
  */
void mesh_node_cfg_check(mesh_node_features_t features, mesh_node_cfg_p pnode_cfg);

/**
  * @brief configure the mesh parameters and allocate resources
  *
  * This function invokes mesh_node_cfg_update to configure the mesh parameters.
  * @param[in] features: the supported mesh features
  * @param[in] pnode_cfg: the configurable device parameters
  * @return none
  */
void mesh_node_cfg(mesh_node_features_t features, mesh_node_cfg_p pnode_cfg);

/**
  * @brief clear the mesh parameters stored in the nvm
  * @return none
  */
void mesh_node_clear(void);

/**
  * @brief clear the mesh parameters stored in the ram
  * @return none
  */
void mesh_node_clean(void);

/**
  * @brief clear all the mesh parameters in the nvm and ram
  * @return none
  */
void mesh_node_reset(void);

/**
  * @brief restore the node after @ref mesh_node_clean has been invoked
  *
  * The node may restore the provisioned state.
  * @return none
  */
void mesh_node_restore(void);

/**
  * @brief When the feature is enabled, the node will receive all group messages even the models haven't
  * subscribe the group address.
  *
  * @return none
  */
void mesh_node_uncheck_group_addr(bool enable);

/**
  * @brief When the feature is enabled, the node will reuse the old mesh parameters if it is reprovisioned.
  *
  * The device can be reprovisioned with the same mesh address without worry about the seq number is duplicate so that
  * the message is filtered by other device.
  * The api shall be called after @ref mesh_node_cfg.
  * @return none
  */
void mesh_node_set_reprov(bool enable);

/**
  * @brief enable to uncheck the cccd value of the mesh service
  * @param[in] enable: true: on, false: off
  * @return none
  */
void mesh_node_set_cccd_not_check(bool enable);

/**
  * @brief set transport segmented message acknowledge delay time
  *
  * If the maximum value is bigger than the minimum value, the delay will be randomly generated between them.
  * @param[in] delay_min: minimum delay
  * @param[in] delay_max: maximum delay
  * @return none
  */
void mesh_node_set_trans_seg_ack_delay(uint16_t delay_min, uint16_t delay_max);

/** @} */

/** @brief
  * @{
  */

/**
  * @brief create a new element
  * @param[in] loc: the element location
  * @return operation result
  */
bool mesh_element_create(uint16_t loc);
mesh_element_p mesh_element_get(uint8_t element_index);
mesh_element_p mesh_element_get_by_addr(uint16_t addr);
/** @} */

/** @brief
  * @{
  */

/**
  * @brief register a model to the addressed element
  * @param[in] element_index: the element index
  * @param[in] pmodel_info: model info provided by the upper layer
  * @return operation result
  */
bool mesh_model_reg(uint8_t element_index, mesh_model_info_p pmodel_info);

/**
  * @brief bind/unbind one app key to the specific model
  * @param[in] pmodel_info: pointer of the model info
  * @param[in] app_key_index: the local index of the app key
  * @param[in] bind: bind or unbind
  * @return none
  */
void mesh_model_bind_one(mesh_model_info_t *pmodel_info, uint16_t app_key_index, bool bind);

/**
  * @brief bind/unbind all app keys to the specific model
  * @param[in] pmodel_info: pointer of the model info
  * @param[in] bind: bind or unbind
  * @return none
  */
void mesh_model_bind_all(mesh_model_info_t *pmodel_info, bool bind);

/**
  * @brief bind/unbind all app keys to all models
  * @param[in] bind: bind or unbind
  * @return none
  */
void mesh_model_bind(bool bind);
#define mesh_model_bind_all_key()   mesh_model_bind(true)

/**
  * @brief dump all app keys binded by the model
  * @param[in] pmodel_info: pointer of the model info
  * @param[out] app_key_index_g: the global indexes of app keys
  * @param[in] max_count: the size of app_key_index_g to put the indexes
  * @return the realistic count of dumped app keys
  */
uint16_t mesh_model_bind_dump(mesh_model_info_t *pmodel_info, uint16_t app_key_index_g[],
                              uint16_t max_count);

/**
  * @brief get the index of the first available app key of the model
  * @param[in] pmodel_info: pointer of model info
  * @return the local app key index
  * @retval >=0: the available app key index
  * @retval <0: no available app key
  */
int mesh_model_get_available_key(mesh_model_info_p pmodel_info);

/**
  * @brief subscribe the group address of the model
  * @param[in] pmodel: pointer of model
  * @param[in] addr: the group address
  * @return operation result
  */
bool mesh_model_sub(mesh_model_p pmodel, uint16_t addr);

/**
  * @brief unsubscribe the group address of the model
  * @param[in] pmodel: pointer of model
  * @param[in] addr: the group address
  * @return operation result
  */
bool mesh_model_unsub(mesh_model_p pmodel, uint16_t addr);

/**
  * @brief all the models subscribe the group address
  *
  * Some models may subscribe fail due to the subscribe list is full or other reasons.
  * @param[in] addr: the group address
  * @return the succeed model num
  */
uint8_t mesh_model_sub_all(uint16_t addr);

/**
  * @brief check whether the group address is subscribed by the model
  * @param[in] pmodel: pointer of model
  * @param[in] addr: the group address
  * @return operation result
  */
bool mesh_model_sub_check(mesh_model_p pmodel, uint16_t addr);

/**
  * @brief dump the group addresses subscribed by the model
  * @param[in] pmodel: pointer of model
  * @param[out] addr: the group addresses
  * @param[in] max_count: the count of addr to put the group address
  * @return the realistic count of group addresses subscribed
  */
uint16_t mesh_model_sub_dump(mesh_model_p pmodel, uint16_t addr[], uint16_t max_count);
uint16_t mesh_model_sub_dump_internal(mesh_model_p pmodel, uint16_t addr[], uint16_t max_count);

/**
  * @brief subscribe many group addresses of the model
  * @param[in] pmodel: pointer of model
  * @param[in] addr: the group addresses
  * @param[in] num: the number of addresses
  * @return none
  */
void mesh_model_sub_load(mesh_model_p pmodel, uint16_t addr[], uint16_t num);

/**
  * @brief clear all group addresses subscribed by the model
  * @param[in] pmodel: pointer of model
  * @return none
  */
void mesh_model_sub_clear(mesh_model_p pmodel);

/**
  * @brief start the publish of the model
  * @param[in] pmodel: pointer of model
  * @return none
  */
void mesh_model_pub_start(mesh_model_p pmodel);

/**
  * @brief get the publish period of the model
  * @param[in] pmodel: pointer of model
  * @return publish period in millisecond
  */
uint32_t mesh_model_pub_period_get(mesh_model_p pmodel);

/**
  * @brief set the publish parameters
  * @param[in] pmodel: pointer of model
  * @param[in] pub_params: the publish parameters
  * @return operation result
  */
void mesh_model_pub_params_set(mesh_model_t *pmodel, mesh_model_pub_params_t pub_params);

/**
  * @brief get the publish parameters
  * @param[in] pmodel: pointer of model
  * @return the publish parameters
  */
mesh_model_pub_params_t mesh_model_pub_params_get(const mesh_model_t *pmodel);

/**
  * @brief check whether the publish addr is set
  * @param[in] pmodel_info: pointer of model info
  * @return operation result
  */
bool mesh_model_pub_check(mesh_model_info_p pmodel_info);

mesh_model_p mesh_model_get(uint8_t element_index, uint8_t model_index);
mesh_model_p mesh_model_get_by_model_id(mesh_element_p pelement, uint32_t model_id);
mesh_model_p mesh_model_get_by_num(uint8_t model_num);
mesh_model_info_p mesh_model_info_get_by_model_id(uint8_t element_index, uint32_t model_id);
uint8_t mesh_model_get_index(mesh_model_p pmodel_in);
uint8_t mesh_model_num(void);
void mesh_model_handle_pub_timeout(mesh_model_p pmodel);
bool mesh_model_sub_check_internal(mesh_model_p pmodel, uint16_t addr);
bool mesh_model_sub_vir_addr_available(mesh_model_p pmodel);

/**
 * @brief configure mesh message
 * @param[in out] pmsg: mesh message need to configured
 */
void mesh_msg_cfg(mesh_msg_t *pmsg);

/** @} */

/** @} */
/** @} */

END_DECLS

#endif /* _MESH_NODE_H */
