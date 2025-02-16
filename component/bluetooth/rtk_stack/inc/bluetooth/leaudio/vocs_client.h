/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VOCS_CLIENT_H_
#define _VOCS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "vocs_def.h"
#include "ble_audio.h"
#include "ble_audio_group.h"

/**
 * \defgroup    LEA_GAF_VOCS_Client Volume Offset Control Client
 *
 * \brief   The client role for Volume Offset Control Profile.
 */

/**
 * \defgroup    VOCS_Client_Exported_Types Volume Offset Control Client Exported Types
 *
 * \ingroup LEA_GAF_VOCS_Client
 * @{
 */

/**
 * vocs_client.h
 *
 * \brief  VOCS client discover service result.
 *
 * The message data for @ref LE_AUDIO_MSG_VOCS_CLIENT_DIS_DONE.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    bool    is_found;      /**< Whether to find service. */
    bool    load_from_ftl; /**< Whether the service table is loaded from FTL. */
    uint8_t srv_num;       /**< Service instance number. */
} T_VOCS_CLIENT_DIS_DONE;

/**
 * vocs_client.h
 *
 * \brief  VOCS characteristics types.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef enum
{
    VOCS_CHAR_OFFSET_STATE,
    VOCS_CHAR_AUDIO_LOCATION,
    VOCS_CHAR_AUDIO_OUTPUT_DESC,
} T_VOCS_CHAR_TYPE;

/**
 * vocs_client.h
 *
 * \brief  Volume Offset Control Point parameters.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef struct
{
    int16_t volume_offset;  /**< volume_offset, used only for @ref VOCS_CP_SET_VOLUME_OFFSET. */
} T_VOCS_CP_PARAM;

/**
 * vocs_client.h
 *
 * \brief  Volume Offset Control Service data.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef union
{
    T_VOCS_VOLUME_OFFSET_STATE volume_offset;
    uint32_t                   audio_location;
    T_VOCS_OUTPUT_DES          output_des;
} T_VOCS_DATA;

/**
 * vocs_client.h
 *
 * \brief  VOCS client read characteristic value result.
 *
 * The message data for @ref LE_AUDIO_MSG_VOCS_CLIENT_READ_RESULT.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef struct
{
    uint16_t            conn_handle;
    uint8_t             srv_instance_id;   /**< Service instance id. Used when the
                                                number of services is greater than 1. */
    T_VOCS_CHAR_TYPE    type;
    T_VOCS_DATA         data;
    uint16_t            cause;
} T_VOCS_CLIENT_READ_RESULT;

/**
 * vocs_client.h
 *
 * \brief  VOCS client receive service notification data.
 *
 * The message data for @ref LE_AUDIO_MSG_VOCS_CLIENT_NOTIFY.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef struct
{
    uint16_t            conn_handle;
    uint8_t             srv_instance_id;   /**< Service instance id. Used when the
                                                number of services is greater than 1. */
    T_VOCS_CHAR_TYPE    type;
    T_VOCS_DATA         data;
} T_VOCS_CLIENT_NOTIFY;

/**
 * vocs_client.h
 *
 * \brief  VOCS client write Volume Offset Control Point result.
 *
 * The message for @ref LE_AUDIO_MSG_VOCS_CLIENT_CP_RESULT.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef struct
{
    uint16_t     conn_handle;
    uint8_t      srv_instance_id;   /**< Service instance id. Used when the
                                         number of services is greater than 1. */
    uint16_t     cause;
    T_VOCS_CP_OP cp_op;
} T_VOCS_CLIENT_CP_RESULT;

/**
 * vocs_client.h
 *
 * \brief  Volume Offset Control Service data.
 *
 * \ingroup VOCS_Client_Exported_Types
 */
typedef struct
{
    uint8_t                     srv_instance_id;   /**< Service instance id. Used when the
                                                        number of services is greater than 1. */
    uint8_t                     type_exist;
    T_VOCS_VOLUME_OFFSET_STATE  volume_offset;
    uint32_t                    audio_location;
    T_VOCS_OUTPUT_DES           output_des;
} T_VOCS_SRV_DATA;
/**
 * End of VOCS_Client_Exported_Types
 * @}
 */

/**
 * \defgroup    VOCS_Client_Exported_Functions Volume Offset Control Client Exported Functions
 *
 * \ingroup LEA_GAF_VOCS_Client
 * @{
 */

/**
 * vocs_client.h
 *
 * \brief  Config the VOCS service CCCD data.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  VOCS service index.
 * \param[in]  cfg_flags        VOCS CCCD flag: @ref VOCS_CCCD_FLAGS.
 * \param[in]  enable           Whether to enable CCCD.
 * \arg    true    Enable CCCD.
 * \arg    false   Disable CCCD.
 *
 * \return         The result of config VOCS service CCCD data.
 * \retval true    Config VOCS service CCCD data is successful.
 * \retval false   Config VOCS service CCCD data failed.
 *
 * \ingroup VOCS_Client_Exported_Functions
 */
void vocs_cfg_cccd(uint16_t conn_handle, uint8_t srv_instance_id, uint8_t cfg_flags, bool enable);

/**
 * vocs_client.h
 *
 * \brief  VOCS client read VOCS characteristic value.
 *
 * If this API is called successfully, the Application will receive @ref LE_AUDIO_MSG_VOCS_CLIENT_READ_RESULT.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  VOCS service index.
 * \param[in]  type             VOCS characteristic type: @ref T_VOCS_CHAR_TYPE.
 *
 * \return         The result of VOCS client read VOCS characteristic value.
 * \retval true    VOCS client read VOCS characteristic value is successful.
 * \retval false   VOCS client read VOCS characteristic value failed.
 *
 * \ingroup VOCS_Client_Exported_Functions
 */
bool vocs_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, T_VOCS_CHAR_TYPE type);

/**
 * vocs_client.h
 *
 * \brief  VOCS client send Volume Offset Control Point.
 *
 * If this API is called successfully, the Application will receive @ref LE_AUDIO_MSG_VOCS_CLIENT_CP_RESULT.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  VOCS service index.
 * \param[in]  op               Volume offset control point opcode: @ref T_VOCS_CP_OP.
 * \param[in]  p_param          Pointer to Volume Offset Control Point parameter:
 *                              @ref T_VOCS_CP_PARAM.
 *
 * \return         The result of VOCS client send Volume Offset Control Point.
 * \retval true    VOCS client send Volume Offset Control Point is successful.
 * \retval false   VOCS client send Volume Offset Control Point failed.
 *
 * \ingroup VOCS_Client_Exported_Functions
 */
bool vocs_write_cp(uint16_t conn_handle, uint8_t srv_instance_id, T_VOCS_CP_OP op,
                   T_VOCS_CP_PARAM *p_param);

/**
 * vocs_client.h
 *
 * \brief  Get VOCS service data.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  VOCS service index.
 * \param[in,out]  p_value      Pointer to VOCS data: @ref T_VOCS_SRV_DATA.
 *
 * \return         The result of get VOCS service data.
 * \retval true    Get VOCS service data is successful.
 * \retval false   Get VOCS service data failed.
 *
 * \ingroup VOCS_Client_Exported_Functions
 */
bool vocs_get_srv_data(uint16_t conn_handle, uint8_t srv_instance_id, T_VOCS_SRV_DATA *p_value);

/**
 * vocs_client.h
 *
 * \brief  VOCS client send Volume Offset Control Point by group.
 *
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  group_handle     LE audio group handle.
 * \param[in]  srv_instance_id  VOCS service index.
 * \param[in]  op               Volume offset control point opcode: @ref T_VOCS_CP_OP.
 * \param[in]  p_param          Pointer to volume offset control point parameter: @ref T_VOCS_CP_PARAM.
 *
 * \return                           The result of send Volume Offset Control Point by group.
 * \retval LE_AUDIO_CAUSE_SUCCESS    Send Volume Offset Control Point by group is successful.
 * \retval Others                    Send Volume Offset Control Point by group failed.
 *
 * \ingroup VOCS_Client_Exported_Functions
 */
T_LE_AUDIO_CAUSE vocs_write_cp_by_group(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                        uint8_t srv_instance_id,
                                        T_VOCS_CP_OP op,
                                        T_VOCS_CP_PARAM *p_param);

/**
 * vocs_client.h
 *
 * \brief  Write the Audio Locations characteristic.
 *
 * If the audio location characteristic changes, the Application will receive
 * @ref LE_AUDIO_MSG_VOCS_CLIENT_NOTIFY.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  VOCS service index.
 * \param[in]  audio_location   Audio location: @ref AUDIO_LOC.
 *
 * \return         The result of VOCS client send write without response packet.
 * \retval true    VOCS client send write without response packet is successful.
 * \retval false   VOCS client send write without response packet failed.
 *
 * \ingroup VOCS_Client_Exported_Functions
 */
bool vocs_write_audio_location(uint16_t conn_handle, uint8_t srv_instance_id,
                               uint32_t audio_location);

/**
 * vocs_client.h
 *
 * \brief  Write the Audio Output Description characteristic.
 *
 * If the audio output description characteristic changes, the Application will receive
 * @ref LE_AUDIO_MSG_VOCS_CLIENT_NOTIFY.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  VOCS service index.
 * \param[in]  p_output_des     Point to the Audio Output Description value.
 * \param[in]  output_des_len   The length of the Audio Output Description value.
 *
 * \return         The result of VOCS client send write without response packet.
 * \retval true    VOCS client send write without response packet is successful.
 * \retval false   VOCS client send write without response packet failed.
 *
 * \ingroup VOCS_Client_Exported_Functions
 */
bool vocs_write_output_des(uint16_t conn_handle, uint8_t srv_instance_id,
                           uint8_t *p_output_des, uint16_t output_des_len);
/**
 * End of VOCS_Client_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
