/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VOCS_MGR_H_
#define _VOCS_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_audio_def.h"
#include "codec_def.h"
#include "vocs_def.h"

/**
 * \defgroup    LEA_GAF_VOCS_Server Volume Offset Control Server
 *
 * \brief   The server role for Volume Offset Control Service.
 */

/**
 * \defgroup    VOCS_Server_Exported_Macros Volume Offset Control Server Exported Macros
 *
 * \ingroup LEA_GAF_VOCS_Server
 * @{
 */

/**
* vocs_def.h
*
* \defgroup    VOCS_FEATURE_MASK VOCS Service Feature Mask
*
* \brief  Define Volume Offset Control Service feature mask.
*
* \ingroup VOCS_Server_Exported_Macros
* @{
*/
#define VOCS_AUDIO_LOCATION_WRITE_WITHOUT_RSP_SUPPORT   0x01
#define VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT 0x02
#define VOCS_AUDIO_LOCATION_NOTIFY_SUPPORT              0x04
#define VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT            0x08
/**
 * End of VOCS_FEATURE_MASK
 * @}
 */

/**
 * End of VOCS_Server_Exported_Macros
 * @}
 */

/**
 * \defgroup    VOCS_Server_Exported_Types Volume Offset Control Server Exported Types
 *
 * \ingroup LEA_GAF_VOCS_Server
 * @{
 */

/**
 * vocs_def.h
 *
 * \brief  VOCS Volume Offset data.
 *
 * \ingroup VOCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t  srv_instance_id;   /**< Service instance id. Used when the
                                     number of services is greater than 1. */
    int16_t  volume_offset;
} T_VOCS_WRITE_OFFSET_STATE_IND;

/**
 * vocs_def.h
 *
 * \brief  VOCS Audio Locations data.
 *
 * \ingroup VOCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t    conn_handle;
    uint8_t     srv_instance_id;   /**< Service instance id. Used when the
                                        number of services is greater than 1. */
    uint32_t    audio_location;
} T_VOCS_WRITE_AUDIO_LOCATION_IND;

/**
 * vocs_def.h
 *
 * \brief  VOCS Audio Output Description data.
 *
 * \ingroup VOCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t            conn_handle;
    uint8_t             srv_instance_id;   /**< Service instance id. Used when the
                                                number of services is greater than 1. */
    T_VOCS_OUTPUT_DES   output_des;
} T_VOCS_WRITE_OUTPUT_DES_IND;

/**
 * vocs_def.h
 *
 * \brief  Volume Offset Control Service parameters to set.
 *
 * \ingroup VOCS_Server_Exported_Types
 */
typedef struct
{
    uint8_t             set_mask;
    int16_t             volume_offset;
    uint8_t
    change_counter; /**< This parameter takes effect only when @ref vocs_set_param is called at the first time.
                         This parameter is used to initialize the Change_Counter field to an arbitrary value.*/
    uint32_t            audio_location;
    T_VOCS_OUTPUT_DES   output_des;
} T_VOCS_PARAM_SET;

/**
 * vocs_def.h
 *
 * \brief  Volume Offset Control Service parameters to get.
 *
 * \ingroup VOCS_Server_Exported_Types
 */
typedef struct
{
    int16_t     volume_offset;
    uint8_t     change_counter;
    uint32_t    audio_location;
    T_VOCS_OUTPUT_DES output_des;
} T_VOCS_PARAM_GET;

/**
 * End of VOCS_Server_Exported_Types
 * @}
 */

/**
 * \defgroup    VOCS_Server_Exported_Functions Volume Offset Control Server Exported Functions
 *
 * \ingroup LEA_GAF_VOCS_Server
 * @{
 */

/**
 * vocs_mgr.h
 *
 * \brief  Set VOCS service parameters.
 *
 * \param[in]  srv_instance_id  Service index.
 * \param[in]  p_param          Pointer to the location of VOCS service parameter: @ref T_VOCS_PARAM_SET.
 *
 * \return         The result of VOCS service parameter.
 * \retval true    Set VOCS service parameter is successful.
 * \retval false   Set VOCS service parameter failed.
 *
 * \ingroup VOCS_Server_Exported_Functions
 */
bool vocs_set_param(uint8_t srv_instance_id, T_VOCS_PARAM_SET *p_param);

/**
 * vocs_mgr.h
 *
 * \brief  Get VOCS service parameters.
 *
 * \param[in]  srv_instance_id  Service index.
 * \param[in]  p_param          Pointer to the location of VOCS service parameter: @ref T_VOCS_PARAM_GET.
 *
 * \return         The result of get VOCS service parameter.
 * \retval true    Get VOCS service parameter is successful.
 * \retval false   Get VOCS service parameter failed.
 *
 * \ingroup VOCS_Server_Exported_Functions
 */
bool vocs_get_param(uint8_t srv_instance_id, T_VOCS_PARAM_GET *p_param);
/**
 * End of VOCS_Server_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
