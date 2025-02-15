/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _PACS_MGR_H_
#define _PACS_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_audio_def.h"
#include "codec_def.h"

/**
 * \defgroup    LEA_GAF_PACS_Server Published Audio Capabilities Server
 *
 * \brief   The server role for Published Audio Capabilities Service.
 */

/**
 * \defgroup    PACS_Server_Exported_Types Published Audio Capabilities Server Exported Types
 *
 * \ingroup LEA_GAF_PACS_Server
 * @{
 */

/**
 * pasc_mgr.h
 *
 * \brief  Write Sink Audio Locations characteristic parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_PACS_WRITE_SINK_AUDIO_LOC_IND.
 *
 * \ingroup PACS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint32_t sink_audio_locations;
} T_PACS_WRITE_SINK_AUDIO_LOC_IND;

/**
 * pasc_mgr.h
 *
 * \brief  Write Source Audio Locations characteristic parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_PACS_WRITE_SOURCE_AUDIO_LOC_IND.
 *
 * \ingroup PACS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint32_t source_audio_locations;
} T_PACS_WRITE_SOURCE_AUDIO_LOC_IND;

/**
 * pasc_mgr.h
 *
 * \brief  Available Audio Contexts read indication parameters.
 *
 * The message data for @ref LE_AUDIO_MSG_PACS_READ_AVAILABLE_CONTEXTS_IND.
 *
 * \ingroup PACS_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cid;
} T_PACS_READ_AVAILABLE_CONTEXTS_IND;

/**
 * pasc_mgr.h
 *
 * \brief  PACS service parameters.
 *
 * \ingroup PACS_Server_Exported_Types
 */
typedef struct
{
    struct
    {
        bool is_exist;                       /**< Whether the sink location exists. */
        bool is_notify;                      /**< Whether notify is supported. */
        bool is_write;                       /**< Whether write is supported. */
        uint32_t sink_audio_location;        /**< Sink Audio Locations value. */
    } sink_locations;                        /**< Sink Audio Locations parameters. */
    struct
    {
        bool is_exist;
        bool is_notify;
        bool is_write;
        uint32_t source_audio_location;      /**< Source Audio Locations value. */
    } source_locations;                      /**< Source Audio Locations parameters. */
    struct
    {
        bool is_notify;
        uint16_t sink_supported_contexts;    /**< Sink Supported Audio Contexts value. */
        uint16_t source_supported_contexts;  /**< Source Supported Audio Contexts value. */
    } supported_contexts;                    /**< Supported Audio Contexts parameters. */
} T_PACS_PARAMS;
/**
 * End of PACS_Server_Exported_Types
 * @}
 */

/**
 * \defgroup    PACS_Server_Exported_Functions Published Audio Capabilities Server Exported Functions
 *
 * \ingroup LEA_GAF_PACS_Server
 * @{
 */

/**
 * pasc_mgr.h
 *
 * \brief  Register PAC record. This API shall be called before @ref pacs_init.
 *
 * \param[in]  direction      Audio direction: @ref T_AUDIO_DIRECTION.
 * \param[in]  pac_data       Pointer to the PAC data.
 *                            This parameter shall not use local variables, and the audio lib only holds pointers internally.
 * \param[in]  pac_data_len   PAC data length. This parameter shall not use local variables.
 * \param[in]  notify         Whether notify is supported.
 * \arg    true    Notify is supported.
 * \arg    false   Notify is not supported.
 *
 * \return                          PAC ID.
 * \retval PAC_RECORD_INVALID_ID    Register PAC record failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
uint8_t pacs_pac_add(T_AUDIO_DIRECTION direction, const uint8_t *pac_data, uint8_t pac_data_len,
                     bool notify);

/**
 * pasc_mgr.h
 *
 * \brief  Initialize PACS Service.
 *
 * This API shall be called before @ref gap_start_bt_stack.
 *
 * \param[in]  p_param      Pointer to PACS Service parameters: @ref T_PACS_PARAMS.
 *
 * \return         The result of initializing PACS.
 * \retval true    Initializing PACS Service is successful.
 * \retval false   Initializing PACS Service failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_init(T_PACS_PARAMS *p_param);

/**
 * pasc_mgr.h
 *
 * \brief  Update PAC record.
 *
 * \param[in]  pac_id        PAC ID.
 * \param[in]  pac_data      Pointer to the PAC data.
 *                           This parameter shall not use local variables,
 *                           and the audio lib only holds pointers internally.
 * \param[in]  pac_data_len  PAC data length. This parameter shall not use local variables.
 *
 * \return         The result of updating PAC record.
 * \retval true    Updating PAC record is successful.
 * \retval false   Updating PAC record failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_pac_update(uint8_t pac_id, const uint8_t *pac_data, uint8_t pac_data_len);

/**
 * pasc_mgr.h
 *
 * \brief  Set sink audio location.
 *
 * \param[in]  sink_audio_location      Sink audio location: @ref AUDIO_LOC.
 *
 * \return         The result of setting sink audio location.
 * \retval true    Setting sink audio location is successful.
 * \retval false   Setting sink audio location failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_update_sink_audio_locations(uint32_t sink_audio_location);

/**
 * pasc_mgr.h
 *
 * \brief  Set source audio location.
 *
 * \param[in]  source_audio_location      Source audio location: @ref AUDIO_LOC.
 *
 * \return         The result of setting source audio location.
 * \retval true    Setting source audio location is successful.
 * \retval false   Setting source audio location failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_update_source_audio_locations(uint32_t source_audio_location);

/**
 * pasc_mgr.h
 *
 * \brief  Send available contexts read confirm.
 *
 * \param[in]  conn_handle                Connection handle.
 * \param[in]  cid                        Local channel ID assigned by the Bluetooth host.
 * \param[in]  sink_available_contexts    Available sink contexts: @ref AUDIO_CONTEXT.
 * \param[in]  source_available_contexts  Available source contexts: @ref AUDIO_CONTEXT.
 *
 * \return         The result of sending available contexts read confirm.
 * \retval true    Sending available contexts read confirm is successful.
 * \retval false   Sending available contexts read confirm failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_available_contexts_read_cfm(uint16_t conn_handle, uint16_t cid,
                                      uint16_t sink_available_contexts, uint16_t source_available_contexts);

/**
 * pasc_mgr.h
 *
 * \brief  Update available contexts notification.
 *
 * \param[in]  conn_handle                Connection handle.
 * \param[in]  sink_available_contexts    Available sink contexts: @ref AUDIO_CONTEXT.
 * \param[in]  source_available_contexts  Available source contexts: @ref AUDIO_CONTEXT.
 *
 * \return         The result of updating available contexts notification.
 * \retval true    Updating available contexts notification is successful.
 * \retval false   Updating available contexts notification failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_update_available_contexts(uint16_t conn_handle, uint16_t sink_available_contexts,
                                    uint16_t source_available_contexts);

/**
 * pasc_mgr.h
 *
 * \brief  Update supported audio contexts.
 *
 * \param[in]  sink_supported_contexts    Supported sink contexts: @ref AUDIO_CONTEXT.
 * \param[in]  source_supported_contexts  Supported source contexts: @ref AUDIO_CONTEXT.
 *
 * \return         The result of updating supported audio contexts.
 * \retval true    Updating supported audio contexts is successful.
 * \retval false   Updating supported audio contexts failed.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_update_supported_contexts(uint16_t sink_supported_contexts,
                                    uint16_t source_supported_contexts);

/**
 * pasc_mgr.h
 *
 * \brief  Check PAC record codec configuration.
 *
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 *
 * \param[in]  direction    Audio direction: @ref T_AUDIO_DIRECTION.
 * \param[in]  codec_id     Pointer to codec ID.
 * \param[in]  p_cfg        Pointer to codec configuration: @ref T_CODEC_CFG.
 *
 * \return         The result of checking PAC record codec configuration.
 * \retval true    PAC record codec configuration check is successful.
 * \retval false   PAC record codec configuration check failed.
 *
 * \note    Only used for LC3_CODEC_ID.
 *
 * \ingroup PACS_Server_Exported_Functions
 */
bool pacs_check_codec_cfg(uint8_t direction, uint8_t *codec_id, T_CODEC_CFG *p_cfg);
/**
 * End of PACS_Server_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
