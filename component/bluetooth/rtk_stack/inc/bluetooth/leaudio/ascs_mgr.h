/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _ASCS_MGR_H_
#define _ASCS_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ascs_def.h"
#include "codec_def.h"

/**
 * \defgroup    LEA_GAF_ASCS_Server Audio Stream Control Server
 *
 * \brief   The server role for Audio Stream Control Service.
 */

/**
 * \defgroup    ASCS_Server_Exported_Types Audio Stream Control Server Exported Types
 *
 * \ingroup LEA_GAF_ASCS_Server
 * @{
 */

/**
 * ascs_mgr.h
 *
 * \brief  ASE Control Point Update Metadata operation data.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint8_t ase_id;
    uint8_t metadata_length;
    uint8_t *p_metadata;
} T_ASE_CP_METADATA_DATA;

/**
 * ascs_mgr.h
 *
 * \brief  ASE Control Point Config Codec operation data.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    T_ASE_CP_CODEC_CFG_ARRAY_PARAM data;
    uint8_t *p_codec_spec_cfg;
    T_CODEC_CFG codec_parsed_data;
} T_ASE_CP_CONFIG_CODEC_DATA;

/**
 * ascs_mgr.h
 *
 * \brief  ASE State parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_ASE_STATE.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    T_ASE_CHAR_DATA ase_data;
} T_ASCS_ASE_STATE;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS setup data path parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_SETUP_DATA_PATH.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t ase_id;
    uint8_t path_direction;        /**< @ref DATA_PATH_INPUT_FLAG or @ref DATA_PATH_OUTPUT_FLAG */
    uint16_t cis_conn_handle;
    T_CODEC_CFG codec_parsed_data;
} T_ASCS_SETUP_DATA_PATH;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS remove data path parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_REMOVE_DATA_PATH.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t ase_id;
    uint8_t path_direction;        /**< @ref DATA_PATH_INPUT_FLAG or @ref DATA_PATH_OUTPUT_FLAG */
    uint16_t cis_conn_handle;
} T_ASCS_REMOVE_DATA_PATH;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS Control Point Config Codec operation parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CP_CONFIG_CODEC_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t number_of_ase;
    T_ASE_CP_CONFIG_CODEC_DATA param[ASCS_AES_CHAR_MAX_NUM];
} T_ASCS_CP_CONFIG_CODEC_IND;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS Control Point Config QoS operation parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CP_CONFIG_QOS_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t number_of_ase;
    T_ASE_CP_QOS_CFG_ARRAY_PARAM param[ASCS_AES_CHAR_MAX_NUM];
} T_ASCS_CP_CONFIG_QOS_IND;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS Control Point Enable operation parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CP_ENABLE_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t number_of_ase;
    T_ASE_CP_METADATA_DATA param[ASCS_AES_CHAR_MAX_NUM];
} T_ASCS_CP_ENABLE_IND;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS Control Point Disable operation parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CP_DISABLE_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t number_of_ase;
    uint8_t ase_id[ASCS_AES_CHAR_MAX_NUM];
} T_ASCS_CP_DISABLE_IND;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS Control Point Release operation parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CP_RELEASE_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t number_of_ase;
    uint8_t ase_id[ASCS_AES_CHAR_MAX_NUM];
} T_ASCS_CP_RELEASE_IND;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS Control Point Update Metadata operation parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CP_UPDATE_METADATA_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t number_of_ase;
    T_ASE_CP_METADATA_DATA param[ASCS_AES_CHAR_MAX_NUM];
} T_ASCS_CP_UPDATE_METADATA_IND;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS preferred QoS parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_GET_PREFER_QOS.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t             conn_handle;
    uint8_t              ase_id;
    uint8_t              direction;
    T_CODEC_CFG          codec_cfg;
    T_ASE_TARGET_LATENCY target_latency;
    T_ASE_TARGET_PHY     target_phy;
} T_ASCS_GET_PREFER_QOS;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS handle CIS request indication parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CIS_REQUEST_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cis_conn_handle;
    uint8_t  snk_ase_id;
    uint8_t  snk_ase_state;
    uint8_t  src_ase_id;
    uint8_t  src_ase_state;
} T_ASCS_CIS_REQUEST_IND;

/**
 * ascs_mgr.h
 *
 * \brief  CIS disconnection information.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CIS_DISCONN_INFO.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cis_conn_handle;
    uint16_t cause;
    uint8_t  cig_id;
    uint8_t  cis_id;
} T_ASCS_CIS_DISCONN_INFO;

/**
 * ascs_mgr.h
 *
 * \brief  CIS established connection information.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_CIS_CONN_INFO.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cis_conn_handle;
    uint8_t  cig_id;
    uint8_t  cis_id;
} T_ASCS_CIS_CONN_INFO;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS check Audio Contexts parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_ASCS_AUDIO_CONTEXTS_CHECK_IND.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint16_t             conn_handle;
    bool                 is_update_metadata;
    uint8_t              ase_id;
    uint8_t              direction;
    uint16_t             available_contexts;
} T_ASCS_AUDIO_CONTEXTS_CHECK_IND;

/**
 * ascs_mgr.h
 *
 * \brief  ASE information.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint8_t ase_id;
    uint8_t ase_state;
} T_ASE_INFO;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS ASE information.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint8_t snk_ase_num;                           /**< Sink ASE number. */
    uint8_t src_ase_num;                           /**< Source ASE number. */
    T_ASE_INFO snk_info[ASCS_AES_CHAR_MAX_NUM];    /**< Sink ASE information. */
    T_ASE_INFO src_info[ASCS_AES_CHAR_MAX_NUM];    /**< Source ASE information. */
} T_ASCS_INFO;

/**
 * ascs_mgr.h
 *
 * \brief  ASCS preferred QoS data.
 *
 * \ingroup ASCS_Server_Exported_Types
 */
typedef struct
{
    uint8_t  supported_framing;
    uint8_t  preferred_phy;
    uint8_t  preferred_retrans_number;
    uint16_t max_transport_latency;
    uint32_t presentation_delay_min;
    uint32_t presentation_delay_max;
    uint32_t preferred_presentation_delay_min;
    uint32_t preferred_presentation_delay_max;
} T_ASCS_PREFER_QOS_DATA;
/**
 * End of ASCS_Server_Exported_Types
 * @}
 */

/**
 * \defgroup    ASCS_Server_Exported_Functions Audio Stream Control Server Exported Functions
 *
 * \ingroup LEA_GAF_ASCS_Server
 * @{
 */

/**
 * ascs_mgr.h
 *
 * \brief  Get ASCS ASE data.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 * \param[in,out]  p_ase_data   Pointer to ASE data: @ref T_ASE_CHAR_DATA.
 *
 * \return         The result of getting ASE data.
 * \retval true    Getting ASE data is successful.
 * \retval false   Getting ASE data failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_get_ase_data(uint16_t conn_handle, uint8_t ase_id, T_ASE_CHAR_DATA *p_ase_data);

/**
 * ascs_mgr.h
 *
 * \brief  Get codec configuration.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 * \param[in,out]  p_cfg        Pointer to ASE codec configuration:
 *                              @ref T_ASE_DATA_CODEC_CONFIGURED.
 *
 * \return         The result of getting ASE codec configuration.
 * \retval true    Getting ASE codec configuration is successful.
 * \retval false   Getting ASE codec configuration failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_get_codec_cfg(uint16_t conn_handle, uint8_t ase_id, T_ASE_DATA_CODEC_CONFIGURED *p_cfg);

/**
 * ascs_mgr.h
 *
 * \brief  Get QoS configuration.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 * \param[in,out]  p_cfg        Pointer to ASE QoS configuration:
 *                              @ref T_ASE_QOS_CFG_STATE_DATA.
 *
 * \return         The result of getting ASE QoS configuration.
 * \retval true    Getting ASE QoS configuration is successful.
 * \retval false   Getting ASE QoS configuration failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_get_qos_cfg(uint16_t conn_handle, uint8_t ase_id, T_ASE_QOS_CFG_STATE_DATA *p_cfg);

/**
 * ascs_mgr.h
 *
 * \brief  Get ASE information.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in,out]  p_info       Pointer to ASE information: @ref T_ASCS_INFO.
 *
 * \return         The result of getting ASE information.
 * \retval true    Getting ASE information is successful.
 * \retval false   Getting ASE information failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_get_ase_info(uint16_t conn_handle, T_ASCS_INFO *p_info);

/**
 * ascs_mgr.h
 *
 * \brief  Get ASE preferred QoS data.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 * \param[in]  p_qos            Pointer to ASE preferred QoS data:
 *                              @ref T_ASCS_PREFER_QOS_DATA.
 *
 * \return         The result of getting ASE preferred QoS data.
 * \retval true    Getting ASE preferred QoS data is successful.
 * \retval false   Getting ASE preferred QoS data failed.
 *
 * \note    Only used for LC3_CODEC_ID.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_get_ase_prefer_qos(uint16_t conn_handle, uint8_t ase_id, T_ASCS_PREFER_QOS_DATA *p_qos);

/**
 * ascs_mgr.h
 *
 * \brief  Set ASE preferred QoS data.
 *
 * This api shall be called when handle @ref LE_AUDIO_MSG_ASCS_GET_PREFER_QOS.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE ID.
 * \param[in]  p_qos            Pointer to ASE preferred QoS data:
 *                              @ref T_ASCS_PREFER_QOS_DATA.
 *
 * \return         The result of set ASE preferred QoS data.
 * \retval true    Set ASE preferred QoS data is successful.
 * \retval false   Set ASE preferred QoS data failed.
 *
 * \note    The APP must use this API for configuration when the codec is not LC3_CODEC_ID.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_cfg_ase_prefer_qos(uint16_t conn_handle, uint8_t ase_id, T_ASCS_PREFER_QOS_DATA *p_qos);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS performs the disable operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE ID.
 *
 * \return         The result of ASCS performing the disable operation.
 * \retval true    ASCS performing the disable operation is successful.
 * \retval false   ASCS performing the disable operation failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_action_disable(uint16_t conn_handle, uint8_t ase_id);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS performs the update metadata operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE ID.
 * \param[in]  metadata_len     Metadata length.
 * \param[in]  p_metadata       Pointer to metadata.
 *
 * \return         The result of performing the update metadata operation.
 * \retval true    Performing the update metadata operation is successful.
 * \retval false   Performing the update metadata operation failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_action_update_metadata(uint16_t conn_handle, uint8_t ase_id,
                                 uint8_t metadata_len, uint8_t *p_metadata);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS performs the release operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE ID.
 *
 * \return         The result of performing the release operation.
 * \retval true    Performing the release operation is successful.
 * \retval false   Performing the release operation failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_action_release(uint16_t conn_handle, uint8_t ase_id);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS performs the release operation by CIG ID.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  cig_id           CIG ID.
 *
 * \return         The result of performing the release operation by CIG ID.
 * \retval true    Performing the release operation by CIG ID is successful.
 * \retval false   Performing the release operation by CIG ID failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_action_release_by_cig(uint16_t conn_handle, uint8_t cig_id);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS sends CIS request confirmation.
 *
 * \param[in]  cis_conn_handle      Connection handle.
 * \param[in]  accept               Whether to accept the CIS request.
 * \arg    true    Accept the CIS request.
 * \arg    false   Reject the CIS request.
 * \param[in]  reject_reason        The reason to reject the CIS request.
 *
 * \return         The result of ASCS sending CIS request confirmation.
 * \retval true    Sending CIS request confirmation is successful.
 * \retval false   Sending CIS request confirmation failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_cis_request_cfm(uint16_t cis_conn_handle, bool accept, uint8_t reject_reason);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS configures Application control start/stop ready operation.
 *
 * This API is only used when the unicast server is in the audio sink role.
 * This API is used to control whether the LE audio library sends the receiver stop ready operation
 * or receiver start ready operation automatically.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE ID.
 * \param[in]  app_handshake    Application controls the handshake. The default value is false.
 * \arg    true    Application controls the start/stop ready operation.
 * \arg    false   Application does not control the start/stop ready operation.
 *
 * \return         The result of ASCS configuring Application control handshake.
 * \retval true    ASCS configuring Application control handshake is successful.
 * \retval false   ASCS configuring Application control handshake failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_app_ctl_handshake(uint16_t conn_handle, uint8_t ase_id, bool app_handshake);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS performs receiver start ready operation.
 *
 * This API is only used when the parameter app_handshake of ascs_app_ctl_handshake is true
 * and the application has received ASE_CP_OP_ENABLE.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE ID.
 *
 * \return         The result of ASCS performing receiver start ready operation.
 * \retval true    Performing receiver start ready operation is successful.
 * \retval false   Performing receiver start ready operation failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_action_rec_start_ready(uint16_t conn_handle, uint8_t ase_id);

/**
 * ascs_mgr.h
 *
 * \brief  ASCS performs receiver stop ready operation.
 *
 * This API is only used when the parameter app_handshake of ascs_app_ctl_handshake is true
 * and the application has received ASE_CP_OP_DISABLE.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE ID.
 *
 * \return         The result of ASCS performing receiver stop ready operation.
 * \retval true    Performing receiver stop ready operation is successful.
 * \retval false   Performing receiver stop ready operation failed.
 *
 * \ingroup ASCS_Server_Exported_Functions
 */
bool ascs_action_rec_stop_ready(uint16_t conn_handle, uint8_t ase_id);
/**
 * End of ASCS_Server_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
