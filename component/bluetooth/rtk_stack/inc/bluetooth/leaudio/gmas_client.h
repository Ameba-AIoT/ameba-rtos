/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _GMAS_CLIENT_H_
#define _GMAS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdbool.h>
#include "gmas_def.h"

/**
 * \defgroup    LEA_USE_CASE_GMAP_Client Gaming Audio Client
 *
 * \brief   The client role for Gaming Audio Profile.
 */

/**
 * \defgroup    GMAP_Client_Exported_Types Gaming Audio Client Exported Types
 *
 * \ingroup LEA_USE_CASE_GMAP_Client
 * @{
 */

/**
 * gmas_client.h
 *
 * \brief  GMAS client discover service result.
 *
 * The message data for @ref LE_AUDIO_MSG_GMAS_CLIENT_DIS_DONE.
 *
 * \ingroup GMAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    bool    is_found;      /**< Whether to find service. */
    bool    load_from_ftl; /**< Whether the service table is loaded from FTL. */
} T_GMAS_CLIENT_DIS_DONE;

/**
 * gmas_client.h
 *
 * \brief  GMAS client read GMAP Role result.
 *
 * The message data for @ref LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT.
 *
 * \ingroup GMAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t  gmap_role;
    uint16_t cause;
} T_GMAS_CLIENT_READ_ROLE_RESULT;

/**
 * gmas_client.h
 *
 * \brief  GMAS client read GMAP Role features result.
 *
 * The message data for @ref LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT.
 *
 * \ingroup GMAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t char_uuid;  /**< Characteristic UUID.
                           * \arg GMAS_UUID_CHAR_UGG_FEATURES
                           * \arg GMAS_UUID_CHAR_UGT_FEATURES
                           * \arg GMAS_UUID_CHAR_BGS_FEATURES
                           * \arg GMAS_UUID_CHAR_BGR_FEATURES */
    uint8_t  features;
    uint16_t cause;
} T_GMAS_CLIENT_READ_FEATURES_RESULT;
/**
 * End of GMAP_Client_Exported_Types
 * @}
 */

/**
 * \defgroup    GMAP_Client_Exported_Functions Gaming Audio Client Exported Functions
 *
 * \ingroup LEA_USE_CASE_GMAP_Client
 * @{
 */

/**
 * gmas_client.h
 *
 * \brief  GMAS client reads GMAP Role.
 *
 * If the read procedure is completed, the Application will
 * receive @ref LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  conn_handle      Connection handle.
 *
 * \return         The result of GMAS client reading GMAP Role.
 * \retval true    Reading GMAP Role request is successful.
 * \retval false   Reading GMAP Role request failed.
 *
 * \ingroup GMAP_Client_Exported_Functions
 */
bool gmas_read_gmap_role(uint16_t conn_handle);

/**
 * gmas_client.h
 *
 * \brief  GMAS client reads GMAP Role features.
 *
 * If the read procedure is completed, the Application will
 * receive @ref LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  char_uuid        Characteristic UUID : @ref GMAS_UUID.
 * \arg   @ref GMAS_UUID_CHAR_UGG_FEATURES
 * \arg   @ref GMAS_UUID_CHAR_UGT_FEATURES
 * \arg   @ref GMAS_UUID_CHAR_BGS_FEATURES
 * \arg   @ref GMAS_UUID_CHAR_BGR_FEATURES
 *
 * \return         The result of GMAS client reading GMAP Role features.
 * \retval true    Reading GMAP Role features request is successful.
 * \retval false   Reading GMAP Role features request failed.
 *
 * \ingroup GMAP_Client_Exported_Functions
 */
bool gmas_read_features(uint16_t conn_handle, uint16_t char_uuid);

/**
 * gmas_client.h
 *
 * \brief  Initialize GMAS client.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \return         The result of initializing GMAS client.
 * \retval true    Initializing GMAS client succeeded.
 * \retval false   Initializing GMAS client failed.
 *
 * \ingroup GMAP_Client_Exported_Functions
 */
bool gmas_client_init(void);
/**
 * End of GMAP_Client_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
