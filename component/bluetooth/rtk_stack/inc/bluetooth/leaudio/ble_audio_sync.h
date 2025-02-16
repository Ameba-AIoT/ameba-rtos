/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BLE_AUDIO_SYNC_H_
#define _BLE_AUDIO_SYNC_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "gap.h"
#include "gap_big_mgr.h"
#include "gap_pa_sync.h"
#include "gap_past_recipient.h"
#include "ble_audio_def.h"
#include "base_data_parse.h"

/**
 * \defgroup    LEA_GAF_LE_Audio_Sync LE Audio Sync
 *
 * \brief   LE audio sync related definition.
 */

/**
 * \defgroup    LE_Audio_Sync_Exported_Macros LE Audio Sync Exported Macros
 *
 * \ingroup LEA_GAF_LE_Audio_Sync
 * @{
 */

/**
 * bap_audio_sync.h
 *
 * \brief  Define type T_BLE_AUDIO_SYNC_HANDLE. LE audio sync handle.
 *
 * \ingroup LE_Audio_Sync_Exported_Macros
 */
typedef void *T_BLE_AUDIO_SYNC_HANDLE;

/**
 * bap_audio_sync.h
 *
 * \defgroup BLE_AUDIO_SYNC_MSG LE Audio Sync Message.
 *
 * \brief  Define LE audio sync message.
 *
 * \ingroup LE_Audio_Sync_Exported_Macros
 * @{
 */
#define MSG_BLE_AUDIO_PA_SYNC_STATE         0x01
#define MSG_BLE_AUDIO_PA_REPORT_INFO        0x02
#define MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO 0x03
#define MSG_BLE_AUDIO_PA_BIGINFO            0x04

#define MSG_BLE_AUDIO_BIG_SYNC_STATE        0x11
#define MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH   0x12
#define MSG_BLE_AUDIO_BIG_REMOVE_DATA_PATH  0x13

#define MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED  0x20
#define MSG_BLE_AUDIO_ADDR_UPDATE           0x21
/**
 * End of BLE_AUDIO_SYNC_MSG
 * @}
 */

/**
 * End of LE_Audio_Sync_Exported_Macros
 * @}
 */

/**
 * \defgroup    LE_Audio_Sync_Exported_Types LE Audio Sync Exported Types
 *
 * \ingroup LEA_GAF_LE_Audio_Sync
 * @{
 */

/**
 * bap_audio_sync.h
 *
 * \brief  Define LE audio action roles.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef enum
{
    BLE_AUDIO_ACTION_ROLE_IDLE      = 0x00,
    BLE_AUDIO_ACTION_ROLE_LOCAL_API = 0x01,
    BLE_AUDIO_ACTION_ROLE_BASS      = 0x02,
} T_BLE_AUDIO_ACTION_ROLE;

/**
 * bap_audio_sync.h
 *
 * \brief  Define LE audio BIG action.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef enum
{
    BLE_AUDIO_BIG_IDLE         = 0x00,
    BLE_AUDIO_BIG_SYNC         = 0x01,
    BLE_AUDIO_BIG_TERMINATE    = 0x02,
    BLE_AUDIO_BIG_LOST         = 0x03,
} T_BLE_AUDIO_BIG_ACTION;

/**
 * bap_audio_sync.h
 *
 * \brief  Parameters for LE audio BIG sync state.
 *
 * The message data for @ref MSG_BLE_AUDIO_BIG_SYNC_STATE.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    T_GAP_BIG_SYNC_RECEIVER_SYNC_STATE       sync_state;
    uint8_t encryption;
    T_BLE_AUDIO_BIG_ACTION     action;
    T_BLE_AUDIO_ACTION_ROLE    action_role;
    uint16_t                   cause;
} T_BLE_AUDIO_BIG_SYNC_STATE;

/**
 * bap_audio_sync.h
 *
 * \brief  Parameters for LE audio BIG setup data path.
 *
 * The message data for @ref MSG_BLE_AUDIO_BIG_SETUP_DATA_PATH.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    uint8_t bis_idx;
    uint16_t bis_conn_handle;
    uint16_t cause;
} T_BLE_AUDIO_BIG_SETUP_DATA_PATH;

/**
 * bap_audio_sync.h
 *
 * \brief  Parameters for LE audio base data modify information.
 *
 * The message data for @ref MSG_BLE_AUDIO_BASE_DATA_MODIFY_INFO.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    T_BASE_DATA_MAPPING *p_base_mapping;
} T_BLE_AUDIO_BASE_DATA_MODIFY_INFO;

/**
 * bap_audio_sync.h
 *
 * \brief  Parameters for LE audio BIG remove data path.
 *
 * The message data for @ref MSG_BLE_AUDIO_BIG_REMOVE_DATA_PATH.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    uint8_t bis_idx;
    uint16_t bis_conn_handle;
    uint16_t cause;
} T_BLE_AUDIO_BIG_REMOVE_DATA_PATH;

/**
 * bap_audio_sync.h
 *
 * \brief  Define LE audio PA action.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef enum
{
    BLE_AUDIO_PA_IDLE         = 0x00,
    BLE_AUDIO_PA_SYNC         = 0x01,
    BLE_AUDIO_PA_TERMINATE    = 0x02,
    BLE_AUDIO_PA_LOST         = 0x03,
} T_BLE_AUDIO_PA_ACTION;

/**
 * bap_audio_sync.h
 *
 * \brief  LE audio PA sync state.
 *
 * The message data for @ref MSG_BLE_AUDIO_PA_SYNC_STATE.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    T_GAP_PA_SYNC_STATE   sync_state;
    T_BLE_AUDIO_PA_ACTION action;
    T_BLE_AUDIO_ACTION_ROLE action_role;
    uint16_t              cause;
} T_BLE_AUDIO_PA_SYNC_STATE;

/**
 * bap_audio_sync.h
 *
 * \brief  LE audio sync handle released parameter.
 *
 * The message data for @ref MSG_BLE_AUDIO_SYNC_HANDLE_RELEASED.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_ACTION_ROLE action_role;
} T_BLE_AUDIO_SYNC_HANDLE_RELEASED;

/**
 * bap_audio_sync.h
 *
 * \brief  LE audio advertiser address update.
 *
 * The message data for @ref MSG_BLE_AUDIO_ADDR_UPDATE.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    uint8_t *advertiser_address;
} T_BLE_AUDIO_ADDR_UPDATE;

/**
 * bap_audio_sync.h
 *
 * \brief  LE audio sync callback data.
 *
 * The message data for @ref MSG_BLE_AUDIO_ADDR_UPDATE.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef union
{
    T_BLE_AUDIO_PA_SYNC_STATE         *p_pa_sync_state;
    T_LE_PERIODIC_ADV_REPORT_INFO     *p_le_periodic_adv_report_info;
    T_BLE_AUDIO_BASE_DATA_MODIFY_INFO *p_base_data_modify_info;

    T_BLE_AUDIO_BIG_SYNC_STATE        *p_big_sync_state;
    T_LE_BIGINFO_ADV_REPORT_INFO      *p_le_biginfo_adv_report_info;
    T_BLE_AUDIO_BIG_SETUP_DATA_PATH   *p_setup_data_path;
    T_BLE_AUDIO_BIG_REMOVE_DATA_PATH  *p_remove_data_path;

    T_BLE_AUDIO_SYNC_HANDLE_RELEASED  *p_sync_handle_released;
    T_BLE_AUDIO_ADDR_UPDATE           *p_addr_update;
} T_BLE_AUDIO_SYNC_CB_DATA;

/**
 * bap_audio_sync.h
 *
 * \brief  LE audio sync information.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    uint8_t advertiser_address_type;
    uint8_t advertiser_address[GAP_BD_ADDR_LEN];
    uint8_t adv_sid;
    uint8_t broadcast_id[BROADCAST_ID_LEN];
    /**< PA Sync information */
    uint8_t sync_id;
    T_GAP_PA_SYNC_STATE pa_state;
    uint16_t pa_interval;
    T_BASE_DATA_MAPPING *p_base_mapping;
    uint8_t pa_sync_addr[GAP_BD_ADDR_LEN];
    uint8_t pa_sync_addr_type;
    bool big_info_received;
    T_LE_BIGINFO_ADV_REPORT_INFO big_info;
    /**< BIG Sync information */
    T_GAP_BIG_SYNC_RECEIVER_SYNC_STATE big_state;
} T_BLE_AUDIO_SYNC_INFO;

/**
 * bap_audio_sync.h
 *
 * \brief  LE audio BIS information.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef struct
{
    uint8_t  bis_num;
    T_BIG_MGR_BIS_CONN_HANDLE_INFO bis_info[GAP_BIG_MGR_MAX_BIS_NUM];
} T_BLE_AUDIO_BIS_INFO;

/**
 * bap_audio_sync.h
 *
 * \brief  P_FUN_BLE_AUDIO_SYNC_CB LE Audio Sync Callback Function Point Definition.
 *
 * Function pointer used in LE audio module, to send events @ref BLE_AUDIO_SYNC_MSG
 * to application.
 *
 * \ingroup LE_Audio_Sync_Exported_Types
 */
typedef void(*P_FUN_BLE_AUDIO_SYNC_CB)(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t cb_type,
                                       void *p_cb_data);
/**
 * End of LE_Audio_Sync_Exported_Types
 * @}
 */

/**
 * \defgroup    LE_Audio_Sync_Exported_Functions LE Audio Sync Exported Functions
 *
 * \ingroup LEA_GAF_LE_Audio_Sync
 * @{
 */

/**
 * bap_audio_sync.h
 *
 * \brief  Allocate LE audio sync.
 *
 * \param[in]  cb_pfn                   Audio sync callback function: @ref P_FUN_BLE_AUDIO_SYNC_CB.
 * \param[in]  advertiser_address_type  Advertiser address type.
 * \param[in]  advertiser_address       Pointer to advertiser address.
 * \param[in]  adv_sid                  Advertising SID.
 * \param[in]  broadcast_id             Broadcast ID.
 *
 * \return         The LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \retval NULL    Allocate LE audio PA synchronization failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
T_BLE_AUDIO_SYNC_HANDLE ble_audio_sync_allocate(P_FUN_BLE_AUDIO_SYNC_CB cb_pfn,
                                                uint8_t advertiser_address_type,
                                                uint8_t *advertiser_address, uint8_t adv_sid,
                                                uint8_t broadcast_id[BROADCAST_ID_LEN]);

/**
 * bap_audio_sync.h
 *
 * \brief  Register LE audio sync update callback function to handle LE audio sync message.
 *
 * \param[in]  handle      LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in]  cb_pfn      LE audio sync callback function: @ref P_FUN_BLE_AUDIO_SYNC_CB.
 *
 * \return         The result of registering LE audio sync update callback function.
 * \retval true    Registering LE audio sync update callback function is successful.
 * \retval false   Registering LE audio sync update callback function failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_sync_update_cb(T_BLE_AUDIO_SYNC_HANDLE handle,
                              P_FUN_BLE_AUDIO_SYNC_CB cb_pfn);

/**
 * bap_audio_sync.h
 *
 * \brief  Update LE audio sync advertiser address.
 *
 * \param[in]  handle              LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in]  advertiser_address  Pointer to advertiser address.
 *
 * \return         The result of updating LE audio sync advertiser address.
 * \retval true    Updating LE audio sync advertiser address is successful.
 * \retval false   Updating LE audio sync advertiser address failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_sync_update_addr(T_BLE_AUDIO_SYNC_HANDLE handle,
                                uint8_t *advertiser_address);

/**
 * bap_audio_sync.h
 *
 * \brief  Find LE audio sync.
 *
 * \param[in]  adv_sid                  Advertising SID.
 * \param[in]  broadcast_id             Broadcast ID.
 *
 * \return         The LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \retval NULL    LE audio sync handle not found.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
T_BLE_AUDIO_SYNC_HANDLE ble_audio_sync_find(uint8_t adv_sid,
                                            uint8_t broadcast_id[BROADCAST_ID_LEN]);

/**
 * bap_audio_sync.h
 *
 * \brief  Get LE audio sync information.
 *
 * \param[in]  handle      LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in,out]  p_info  Pointer to LE audio sync information: @ref T_BLE_AUDIO_SYNC_INFO.
 *
 * \return         The result of getting LE audio sync information.
 * \retval true    Getting LE audio sync information is successful.
 * \retval false   Getting LE audio sync information failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_sync_get_info(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_SYNC_INFO *p_info);

/**
 * bap_audio_sync.h
 *
 * \brief  Release LE audio sync.
 *
 * If this API is successfully invoked, the referred sync handle will be cleared.
 *
 * \param[in]  p_handle     Pointer to LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 *
 * \return         The result of releasing LE audio sync.
 * \retval true    Releasing LE audio sync is successful.
 * \retval false   Releasing LE audio sync failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */

bool ble_audio_sync_release(T_BLE_AUDIO_SYNC_HANDLE *p_handle);

/**
 * bap_audio_sync.h
 *
 * \brief  Send establish LE audio PA sync request.
 *
 * \param[in]  handle         LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in]  options        PA create sync options.
 * \param[in]  sync_cte_type  PA sync CTE type.
 * \param[in]  skip           The maximum number of periodic advertising events that can be
                              skipped after a is successful receive.
                              Range: 0x0000 to 0x01F3
 * \param[in]  sync_timeout   Sync timeout for the PA train.
                              Range: 0x000A to 0x4000
                              Time = N*10 ms
                              Time Range: 100 ms to 163.84 s
 *
 * \return         The result of establishing LE audio PA sync.
 * \retval true    Sending request is successful.
 * \retval false   Sending request failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_pa_sync_establish(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t options,
                                 uint8_t sync_cte_type,
                                 uint16_t skip, uint16_t sync_timeout);

/**
 * bap_audio_sync.h
 *
 * \brief  Send terminate LE audio PA sync request.
 *
 * \param[in]  handle      LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 *
 * \return         The result of terminating LE audio PA sync.
 * \retval true    Sending request is successful.
 * \retval false   Sending request failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_pa_terminate(T_BLE_AUDIO_SYNC_HANDLE handle);

/**
 * bap_audio_sync.h
 *
 * \brief  Send establish LE audio BIG sync request.
 *
 * \param[in]  handle        LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in]  p_param       Pointer to BIG create sync parameter:
 *                           @ref T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM.
 *
 * \return         The result of establishing LE audio BIG sync.
 * \retval true    Sending request is successful.
 * \retval false   Sending request failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_big_sync_establish(T_BLE_AUDIO_SYNC_HANDLE handle,
                                  T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM *p_param);

/**
 * bap_audio_sync.h
 *
 * \brief  Send terminate LE audio BIG sync request.
 *
 * \param[in]  handle      LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 *
 * \return         The result of terminating LE audio BIG sync.
 * \retval true    Sending request is successful.
 * \retval false   Sending request failed.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_big_terminate(T_BLE_AUDIO_SYNC_HANDLE handle);

/**
 * bap_audio_sync.h
 *
 * \brief  Get LE audio BIS sync information.
 *
 * \param[in]  handle      LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in,out]  p_info  Pointer to BIS sync information: @ref T_BLE_AUDIO_BIS_INFO.
 *
 * \return         The result of getting LE audio BIS sync information.
 * \retval true    Successfully got LE audio BIS sync information.
 * \retval false   Failed to get LE audio BIS sync information.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_get_bis_sync_info(T_BLE_AUDIO_SYNC_HANDLE handle, T_BLE_AUDIO_BIS_INFO *p_info);

/**
 * bap_audio_sync.h
 *
 * \brief  Set up LE audio BIS data path.
 *
 * \param[in]  handle            LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in]  bis_idx           BIS index.
 * \param[in]  codec_id          Codec ID.
 * \param[in]  controller_delay  Controller delay in microseconds.
 *                               Range: 0x000000 to 0x3D0900
                                 Time range: 0 s to 4 s
 * \param[in]  codec_len         Length of codec configuration.
 * \param[in]  p_codec_data      Pointer to codec-specific configuration data.
 *
 * \return         The result of set up LE audio BIS data path.
 * \retval true    Successfully set up LE audio BIS data path.
 * \retval false   Failed to set up LE audio BIS data path.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_bis_setup_data_path(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t bis_idx,
                                   uint8_t codec_id[5], uint32_t controller_delay,
                                   uint8_t codec_len, uint8_t *p_codec_data);

/**
 * bap_audio_sync.h
 *
 * \brief  Remove LE audio BIS data path.
 *
 * \param[in]  handle       LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 * \param[in]  bis_idx      BIS index.
 *
 * \return         The result of removing LE audio BIS data path.
 * \retval true    Successfully removed LE audio BIS data path.
 * \retval false   Failed to remove LE audio BIS data path.
 *
 * \ingroup LE_Audio_Sync_Exported_Functions
 */
bool ble_audio_bis_remove_data_path(T_BLE_AUDIO_SYNC_HANDLE handle, uint8_t bis_idx);
/**
 * End of LE_Audio_Sync_Exported_Functions
 * @}
 */

/**
 * End of LEA_GAF_LE_Audio_Sync
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
