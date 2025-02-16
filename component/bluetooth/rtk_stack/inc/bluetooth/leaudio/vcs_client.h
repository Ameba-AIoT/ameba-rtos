/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VCS_CLIENT_H_
#define _VCS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "vcs_def.h"
#include "ble_audio.h"
#include "ble_audio_group.h"
#include "bt_gatt_client.h"

/**
 * \defgroup    LEA_GAF_VCP_Client Volume Control Client
 *
 * \brief   The client role for Volume Control Profile.
 */

/**
 * \defgroup    VCP_Client_Exported_Macros Volume Control Client Exported Macros
 *
 * \ingroup LEA_GAF_VCP_Client
 * @{
 */

/**
 * vcs_client.h
 *
 * \defgroup    VCS_CHAR_EXIST VCS Service Characteristic Exist Flag
 *
 * \brief  Define VCS service characteristic exist flags.
 *
 * \ingroup VCP_Client_Exported_Macros
 * @{
 */
#define VCS_VOLUME_STATE_FLAG 0x01    /**< Volume State exist flag. */
#define VCS_VOLUME_FLAGS_FLAG 0x02    /**< Volume Flags exist flag. */
/**
 * End of VCS_CHAR_EXIST
 * @}
 */

/**
 * End of VCP_Client_Exported_Macros
 * @}
 */

/**
 * \defgroup    VCP_Client_Exported_Types Volume Control Client Exported Types
 *
 * \ingroup LEA_GAF_VCP_Client
 * @{
 */

/**
 * vcs_client.h
 *
 * \brief  VCS client discover service result.
 *
 * The message data for @ref LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE.
 *
 * \ingroup VCP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    bool     is_found;      /**< Whether to find service. */
    bool     load_from_ftl; /**< Whether the service table is loaded from FTL. */
    uint8_t  type_exist;
} T_VCS_CLIENT_DIS_DONE;

/**
 * vcs_client.h
 *
 * \brief  VCS client send Volume Control Point parameters.
 *
 * \ingroup VCP_Client_Exported_Types
 */
typedef struct
{
    uint8_t volume_setting;   /**< volume_setting, used only for @ref VCS_CP_SET_ABSOLUTE_VOLUME. */
} T_VCS_VOLUME_CP_PARAM;

/**
 * vcs_client.h
 *
 * \brief  Volume Control Service characteristic types.
 *
 * \ingroup VCP_Client_Exported_Types
 */
typedef enum
{
    VCS_CHAR_VOLUME_STATE,
    VCS_CHAR_VOLUME_FLAGS,
} T_VCS_CHAR_TYPE;

/**
 * vcs_client.h
 *
 * \brief  Volume Control Service data.
 *
 * \ingroup VCP_Client_Exported_Types
 */
typedef union
{
    uint8_t volume_flags;
    T_VOLUME_STATE volume_state;
} T_VCS_DATA;

/**
 * vcs_client.h
 *
 * \brief  VCS client read characteristic value result.
 *
 * The message data for @ref LE_AUDIO_MSG_VCS_CLIENT_VOLUME_STATE_DATA.
 *
 * \ingroup VCP_Client_Exported_Types
 */
typedef struct
{
    uint16_t        conn_handle;
    bool            is_notify;
    T_VCS_CHAR_TYPE type;
    T_VCS_DATA      data;
} T_VCS_CLIENT_VOLUME_STATE_DATA;

/**
 * vcs_client.h
 *
 * \brief  VCS client write control point result.
 *
 * \ingroup VCP_Client_Exported_Types
 */
typedef struct
{
    uint16_t    conn_handle;
    uint16_t    cause;
    T_VCS_CP_OP cp_op;
} T_VCS_CLIENT_CP_RESULT;
/**
 * End of VCP_Client_Exported_Types
 * @}
 */

/**
 * \defgroup    VCP_Client_Exported_Functions Volume Control Client Exported Functions
 *
 * \ingroup LEA_GAF_VCP_Client
 * @{
 */

/**
 * vcs_client.h
 *
 * \brief  VCS client write Volume Control Point.
 *
 * If this API is called successfully, the Application will receive @ref LE_AUDIO_MSG_VCS_CLIENT_CP_RESULT.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  op               Volume control point opcode: @ref T_VCS_CP_OP.
 * \param[in]  p_param          Pointer to Volume Control Point parameter: @ref T_VCS_VOLUME_CP_PARAM.
 *
 * \return         The result of VCS client write Volume Control Point.
 * \retval true    VCS client write Volume Control Point is successful.
 * \retval false   VCS client write Volume Control Point failed.
 *
 * \ingroup VCP_Client_Exported_Functions
 */
bool vcs_write_cp(uint16_t conn_handle, T_VCS_CP_OP op, T_VCS_VOLUME_CP_PARAM *p_param);

/**
 * vcs_client.h
 *
 * \brief  Get Volume State characteristic value.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in,out]  p_value      Pointer to Volume State data: @ref T_VOLUME_STATE.
 *
 * \return         The result of getting Volume State.
 * \retval true    Getting Volume State is successful.
 * \retval false   Getting Volume State failed.
 *
 * \ingroup VCP_Client_Exported_Functions
 */
bool vcs_get_volume_state(uint16_t conn_handle, T_VOLUME_STATE *p_value);

/**
 * vcs_client.h
 *
 * \brief  Get all included AICS services.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  p_aics_instance  Pointer to AICS services attribute instance:
 *                              @ref T_ATTR_INSTANCE.
 *
 * \return         The result of getting all included AICS services.
 * \retval true    Getting all included AICS services is successful.
 * \retval false   Getting all included AICS services failed.
 *
 * \ingroup VCP_Client_Exported_Functions
 */
bool vcs_get_all_inc_aics(uint16_t conn_handle, T_ATTR_INSTANCE *p_aics_instance);

/**
 * vcs_client.h
 *
 * \brief  Get Volume Flags characteristic value.
 *
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in,out]  p_value      Pointer to Volume Flags.
 *
 * \return         The result of getting Volume Flags.
 * \retval true    Getting Volume Flags is successful.
 * \retval false   Getting Volume Flags failed.
 *
 * \ingroup VCP_Client_Exported_Functions
 */
bool vcs_get_volume_flags(uint16_t conn_handle, uint8_t *p_value);

/**
 * vcs_client.h
 *
 * \brief  VCS client sends Volume Control Point by LE audio group.
 *
 * If this API is called successfully, the Application will receive @ref LE_AUDIO_MSG_VCS_CLIENT_CP_RESULT.
 * The parameter op can't be set to @ref VCS_CP_SET_ABSOLUTE_VOLUME, @ref VCS_CP_UNMUTE, and @ref VCS_CP_MUTE.
 * Application can call @ref cap_vcs_change_mute and @ref cap_vcs_change_volume instead.
 *
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  group_handle   LE audio group: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  op             Volume control point opcode: @ref T_VCS_CP_OP.
 * \param[in]  p_param        Pointer to Volume Control Point parameter: @ref T_VCS_VOLUME_CP_PARAM.
 *
 * \return         The result of VCS client sending Volume Control Point by LE audio group.
 * \retval true    VCS client sending Volume Control Point by LE audio group is successful.
 * \retval false   VCS client sending Volume Control Point by LE audio group failed.
 *
 * \ingroup VCP_Client_Exported_Functions
 */
T_LE_AUDIO_CAUSE vcs_write_cp_by_group(T_BLE_AUDIO_GROUP_HANDLE group_handle, T_VCS_CP_OP op,
                                       T_VCS_VOLUME_CP_PARAM *p_param);
/**
 * End of VCP_Client_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
