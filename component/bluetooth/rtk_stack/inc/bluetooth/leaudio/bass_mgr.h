/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BASS_MGR_H_
#define _BASS_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_audio_sync.h"
#include "bass_def.h"

/**
 * \defgroup    LEA_GAF_BASS_Server Broadcast Audio Scan Server
 *
 * \brief   The server role for Broadcast Audio Scan Service.
 */

/**
 * \defgroup    BASS_Server_Exported_Types Broadcast Audio Scan Server Exported Types
 *
 * \ingroup LEA_GAF_BASS_Server
 * @{
 */

/**
 * bass_mgr.h
 *
 * \brief  BASS remote write Broadcast Audio Scan Control Point parameter.
 *
 * The message data for LE_AUDIO_MSG_BASS_CP_IND.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    T_BASS_CP_DATA *p_cp_data;
} T_BASS_CP_IND;

/**
 * bass_mgr.h
 *
 * \brief  Broadcast Receive State modify message data.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_BRS_MODIFY.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
    uint8_t source_id;
    T_BASS_BRS_DATA *p_brs_data;
} T_BASS_BRS_MODIFY;

/**
 * bass_mgr.h
 *
 * \brief  Broadcast audio Add Source operation message data.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_BA_ADD_SOURCE.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
    uint8_t source_id;
} T_BASS_BA_ADD_SOURCE;

/**
 * bass_mgr.h
 *
 * \brief  BASS Add Source locally message data.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_LOCAL_ADD_SOURCE.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
    uint8_t source_id;
} T_BASS_LOCAL_ADD_SOURCE;

/**
 * bass_mgr.h
 *
 * \brief  BASS get PA sync parameter.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_GET_PA_SYNC_PARAM.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
    uint8_t  source_id;
    bool     is_past;
    uint16_t pa_interval;
} T_BASS_GET_PA_SYNC_PARAM;

/**
 * bass_mgr.h
 *
 * \brief  BASS get BIG sync parameter.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_GET_BIG_SYNC_PARAM.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
    uint8_t  source_id;
} T_BASS_GET_BIG_SYNC_PARAM;

/**
 * bass_mgr.h
 *
 * \brief  BASS get Broadcast_Code parameter.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_GET_BROADCAST_CODE.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
    uint8_t  source_id;
} T_BASS_GET_BROADCAST_CODE;

/**
 * bass_mgr.h
 *
 * \brief BASS set prefer BIS sync parameter.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_GET_PREFER_BIS_SYNC.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_SYNC_HANDLE handle;
    uint8_t  source_id;
    uint8_t  num_subgroups;
    T_BASS_CP_BIS_INFO  *p_cp_bis_info;
} T_BASS_SET_PREFER_BIS_SYNC;

/**
 * bass_mgr.h
 *
 * \brief  No empty Broadcast Receive State characteristics for Add Source operation.
 *
 * The message data for @ref LE_AUDIO_MSG_BASS_BRS_CHAR_NO_EMPTY.
 * APP can call @ref ble_audio_sync_release to delete BRS data when receive
 * @ref LE_AUDIO_MSG_BASS_BRS_CHAR_NO_EMPTY.
 *
 * \ingroup BASS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    T_BASS_CP_DATA *p_cp_data;   /**< Control point: add source operation */
} T_BASS_BRS_CHAR_NO_EMPTY;
/**
 * End of BASS_Server_Exported_Types
 * @}
 */

/**
 * \defgroup    BASS_Server_Exported_Functions Broadcast Audio Scan Server Exported Functions
 *
 * \ingroup LEA_GAF_BASS_Server
 * @{
 */

/**
 * bass_mgr.h
 *
 * \brief  BASS set PA sync parameters.
 *
 * This API is used when receive @ref LE_AUDIO_MSG_BASS_GET_PA_SYNC_PARAM.
 *
 * \param[in]  source_id             Source id.
 * \param[in]  pa_sync_options       PA create sync options.
 * \param[in]  pa_sync_skip          The maximum number of periodic advertising events
                                     that can be skipped after a is successful receive.
                                     Range: 0x0000 to 0x01F3.
 * \param[in]  pa_sync_timeout_10ms  PA sync timeout.
                                     Range: 0x000A to 0x4000
                                     Time = N*10 ms
                                     Time Range: 100 ms to 163.84 s
 * \param[in]  past_timeout_10ms     PAST timeout.
 *
 * \return         The result of set PA sync parameters.
 * \retval true    Set PA sync parameters is successful.
 * \retval false   Set PA sync parameters failed.
 *
 * \ingroup BASS_Server_Exported_Functions
 */
bool bass_cfg_pa_sync_param(uint8_t source_id, uint8_t pa_sync_options, uint16_t pa_sync_skip,
                            uint16_t pa_sync_timeout_10ms, uint16_t past_timeout_10ms);

/**
 * bass_mgr.h
 *
 * \brief  BASS set BIG sync parameters.
 *
 * This API is used when receive @ref LE_AUDIO_MSG_BASS_GET_BIG_SYNC_PARAM.
 *
 * \param[in]  source_id              Source id.
 * \param[in]  big_mse                BIG mse.
 * \param[in]  big_sync_timeout_10ms  BIG sync timeout.
 *
 * \return         The result of set BIG sync parameters.
 * \retval true    Set BIG sync parameters is successful.
 * \retval false   Set BIG sync parameters failed.
 *
 * \ingroup BASS_Server_Exported_Functions
 */
bool bass_cfg_big_sync_param(uint8_t source_id, uint8_t big_mse, uint16_t big_sync_timeout_10ms);

/**
 * bass_mgr.h
 *
 * \brief  BASS set broadcast code.
 *
 * This API is used when receive @ref LE_AUDIO_MSG_BASS_GET_BROADCAST_CODE.
 *
 * \param[in]  source_id       Source id.
 * \param[in]  broadcast_code  Broadcast code.
 *
 * \return         The result of set broadcast code.
 * \retval true    Set broadcast code is successful.
 * \retval false   Set broadcast code failed.
 *
 * \ingroup BASS_Server_Exported_Functions
 */
bool bass_cfg_broadcast_code(uint8_t source_id, uint8_t broadcast_code[BROADCAST_CODE_LEN]);

/**
 * bass_mgr.h
 *
 * \brief  BASS set preferred bis sync.
 *
 * This API is used when receive @ref LE_AUDIO_MSG_BASS_GET_PREFER_BIS_SYNC.
 *
 * \param[in]  source_id      Source id.
 * \param[in]  subgroup_idx   Subgroup index.
 * \param[in]  bis_sync       BIS sync.
 *
 * \return         The result of set preferred bis sync.
 * \retval true    Set preferred bis sync is successful.
 * \retval false   Setferred bis sync failed.
 *
 * \ingroup BASS_Server_Exported_Functions
 */
bool bass_cfg_prefer_bis_sync(uint8_t source_id, uint8_t subgroup_idx, uint32_t bis_sync);

/**
 * bass_mgr.h
 *
 * \brief  Send Broadcast_Code required request.
 *
 * \param[in]  source_id      Source id.
 *
 * \return         The result of sending Broadcast_Code required request.
 * \retval true    Sending Broadcast_Code required request succeeded.
 * \retval false   Sending Broadcast_Code required request failed.
 *
 * \ingroup BASS_Server_Exported_Functions
 */
bool bass_send_broadcast_code_required(uint8_t source_id);

/**
 * bass_mgr.h
 *
 * \brief  Get broadcast Receive State characteristic data.
 *
 * \param[in]  source_id      Source id.
 * \param[in,out]  p_value    Pointer to broadcast receive state data: @ref T_BASS_BRS_DATA.
 * \param[in,out]  p_handle   Pointer to LE audio sync handle: @ref T_BLE_AUDIO_SYNC_HANDLE.
 *
 * \return         The result of getting broadcast Receive State characteristic data.
 * \retval true    Getting Broadcast Receive State characteristic data succeeded.
 * \retval false   Getting Broadcast Receive State characteristic data failed.
 *
 * \ingroup BASS_Server_Exported_Functions
 */
bool bass_get_brs_char_data(uint8_t source_id, T_BASS_BRS_DATA *p_value,
                            T_BLE_AUDIO_SYNC_HANDLE *p_handle);
/**
 * End of BASS_Server_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
