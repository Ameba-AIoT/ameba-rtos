/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gmas_mgr.h
  * @brief    Header file for Gaming Audio Server.
  * @details  This file defines Gaming Audio Server related API.
  * @author
  * @date
  * @version
  * **
  */

#ifndef _GMAS_MGR_H_
#define _GMAS_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "gmas_def.h"

/**
 * \defgroup    LEA_USE_CASE_GMAP_Server Gaming Audio Server
 *
 * \brief   Provide status and control of Gaming Audio.
 */

/**
 * \defgroup GMAP_Server_Exported_Types Gaming Audio Server Exported Types
 *
 * \ingroup LEA_USE_CASE_GMAP_Server
 * @{
 */


/**
 * gmas_mgr.h
 *
 * \brief  The message data for @ref LE_AUDIO_MSG_GMAS_READ_IND.
 *
 * \ingroup GMAP_Server_Exported_Types
 */
typedef struct
{
    uint16_t conn_handle;
    uint16_t cid;
    uint16_t char_uuid;
} T_GMAS_READ_IND;

/**
 * End of GMAP_Server_Exported_Types
 * @}
 */

/**
 * \defgroup GMAP_Server_Exported_Functions Gaming Audio Server Exported Functions
 *
 * \ingroup LEA_USE_CASE_GMAP_Server
 * @{
 */

/**
 * gmas_mgr.h
 *
 * \brief  Initialize GMAS service.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  gmap_role      GMAP role: @ref GMAP_ROLE_BIT_VALUE.
 *
 * \return         The result of initializing GMAS service.
 * \retval true    Initialization of GMAS service succeeded.
 * \retval false   Initialization of GMAS service failed.
 *
 * <b>Example usage</b>
 * \code{.c}
     void test(void)
     {
          uint8_t gmap_role = GMAP_ROLE_CHAR_BIT_VALUE_UGG | GMAP_ROLE_CHAR_BIT_VALUE_UGT |
                              GMAP_ROLE_CHAR_BIT_VALUE_BGS | GMAP_ROLE_CHAR_BIT_VALUE_BGR;
          gmas_init(gmap_role);
     }

 * \endcode
 *
 * \ingroup GMAP_Server_Exported_Functions
 */
bool gmas_init(uint8_t gmap_role);

/**
 * gmas_mgr.h
 *
 * \brief  Send GMAS characteristic value read confirm.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  conn_handle    Connection handle.
 * \param[in]  cid            Local CID assigned by Bluetooth Host.
 * \param[in]  char_uuid      Characteristic UUID : @ref GMAS_UUID.
 * \param[in]  features       Features of characteristic:
 * \arg UGG features: @ref UGG_FEATURES_BIT_VALUE.
 * \arg UGT features: @ref UGT_FEATURES_BIT_VALUE.
 * \arg BGS features: @ref BGS_FEATURES_BIT_VALUE.
 * \arg BGR features: @ref BGR_FEATURES_BIT_VALUE.
 *
 * \return         The result of sending GMAS characteristic value read confirm.
 * \retval true    Sending GMAS characteristic value read confirm is successful.
 * \retval false   Sending GMAS characteristic value read confirm failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        ble_audio_cback_register(app_gmas_handle_msg);
    }

    uint16_t app_gmas_handle_msg(T_LE_AUDIO_MSG msg, void *buf)
    {
        uint16_t cb_result  = BLE_AUDIO_CB_RESULT_SUCCESS;

        switch (msg)
        {
        case LE_AUDIO_MSG_GMAS_READ_IND:
            {
                T_GMAS_READ_IND *p_read_ind = (T_GMAS_READ_IND *)buf;
                uint8_t features;

                switch (p_read_ind->char_uuid)
                {
                case GMAS_UUID_CHAR_UGG_FEATURES:
                    {
                        features = UGG_FEATURES_CHAR_BIT_VALUE_UGG_MULTIPLEX_FEATURE_SUP |
                                   UGG_FEATURES_CHAR_BIT_VALUE_UGG_96K_SOURCE_FEATURE_SUP |
                                   UGG_FEATURES_CHAR_BIT_VALUE_UGG_MULTISINK_FEATURE_SUP;
                    }
                    break;

                case GMAS_UUID_CHAR_UGT_FEATURES:
                    {
                        features = UGT_FEATURES_CHAR_BIT_VALUE_UGT_SOURCE_FEATURE_SUP |
                                   UGT_FEATURES_CHAR_BIT_VALUE_UGT_64K_SOURCE_FEATURE_SUP |
                                   UGT_FEATURES_CHAR_BIT_VALUE_UGT_MULTISOURCE_FEATURE_SUP;
                    }
                    break;

                case GMAS_UUID_CHAR_BGS_FEATURES:
                    {
                        features = BGS_FEATURES_CHAR_BIT_VALUE_BGS_96K_FEATURE_SUP;
                    }
                    break;

                case GMAS_UUID_CHAR_BGR_FEATURES:
                    {
                        features = BGR_FEATURES_CHAR_BIT_VALUE_BGR_MULTISINK_FEATURE_SUP |
                                   BGR_FEATURES_CHAR_BIT_VALUE_BGR_MULTIPLEX_FEATURE_SUP;
                    }
                    break;

                default:
                    {
                        cb_result = BLE_AUDIO_CB_RESULT_APP_ERR;
                    }
                    break;
                }

                if (cb_result == BLE_AUDIO_CB_RESULT_SUCCESS)
                {
                    if (!gmas_features_read_confirm(p_read_ind->conn_handle, p_read_ind->cid,
                                               p_read_ind->char_uuid, features))
                    {
                        cb_result = BLE_AUDIO_CB_RESULT_APP_ERR;
                    }
                }
            }
            break;
        default:
            break;
        }
        return cb_result;
    }
 * \endcode
 *
 * \ingroup GMAP_Server_Exported_Functions
 */
bool gmas_features_read_confirm(uint16_t conn_handle, uint16_t cid,
                                uint16_t char_uuid, uint8_t features);

/**
 * End of GMAP_Server_Exported_Functions
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
