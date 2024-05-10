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
 * \{
 */

/**
 * gmas_client.h
 *
 * \brief  GMAS Client Discover Service Result.
 *         The message data for LE_AUDIO_MSG_GMAS_CLIENT_DIS_DONE.
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
 * \brief  GMAS Client Read GMAP Role Result.
 *         The message data for LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT.
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
 * \brief  GMAS Client Read GMAP Role Features Result.
 *         The message data for LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT.
 *
 * \ingroup GMAP_Client_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t char_uuid;  /**< GMAS_UUID_CHAR_UGG_FEATURES, GMAS_UUID_CHAR_UGT_FEATURES,
                              GMAS_UUID_CHAR_BGS_FEATURES, GMAS_UUID_CHAR_BGR_FEATURES. */
    uint8_t  features;
    uint16_t cause;
} T_GMAS_CLIENT_READ_FEATURES_RESULT;
/**
 * End of GMAP_Client_Exported_Types
 * \}
 */

/**
 * \defgroup    GMAP_Client_Exported_Functions Gaming Audio Client Exported Functions
 *
 * \ingroup LEA_USE_CASE_GMAP_Client
 * \{
 */

/**
 * gmas_client.h
 *
 * \brief  GMAS client read gmap role.
 *         If the read procedure is completed, the Application will
 *         receive LE_AUDIO_MSG_GMAS_CLIENT_READ_ROLE_RESULT.
 *
 * \param[in]  conn_handle      Connection handle.
 *
 * \return         The result of GMAS client read gmap role.
 * \retval true    GMAS client read gmap role request success.
 * \retval false   GMAS client read gmap role request failed.
 *
 * \ingroup TMAP_Client_Exported_Functions
 */
bool gmas_read_gmap_role(uint16_t conn_handle);

/**
 * gmas_client.h
 *
 * \brief  GMAS client read gmap role features.
 *         If the read procedure is completed, the Application will
 *         receive LE_AUDIO_MSG_GMAS_CLIENT_READ_FEATURE_RESULT.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  char_uuid        Characteristic UUID : @ref GMAS_UUID.
 *                              GMAS_UUID_CHAR_UGG_FEATURES,
 *                              GMAS_UUID_CHAR_UGT_FEATURES,
 *                              GMAS_UUID_CHAR_BGS_FEATURES,
 *                              GMAS_UUID_CHAR_BGR_FEATURES.
 *
 * \return         The result of GMAS client read GMAP role.
 * \retval true    GMAS client read GMAP role request success.
 * \retval false   GMAS client read GMAP role request failed.
 *
 * \ingroup GMAP_Client_Exported_Functions
 */
bool gmas_read_features(uint16_t conn_handle, uint16_t char_uuid);

/**
 * gmas_client.h
 *
 * \brief  Initialize GMAS client.
 *
 * \return         The result of initialize GMAS client.
 * \retval true    Initialize GMAS client success.
 * \retval false   Initialize GMAS client failed.
 *
 * \ingroup GMAP_Client_Exported_Functions
 */
bool gmas_client_init(void);
/**
 * End of GMAP_Client_Exported_Functions
 * \}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
