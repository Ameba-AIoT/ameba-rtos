/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BLE_AUDIO_GROUP_H_
#define _BLE_AUDIO_GROUP_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "gap.h"
#include "gap_msg.h"

/**
 * \defgroup    LEA_GAF_LE_Audio_Group LE Audio Group
 *
 * \brief   LE audio group related definition.
 */

/**
 * \defgroup    LE_Audio_Group_Exported_Types LE Audio Group Exported Types
 *
 * \ingroup LEA_GAF_LE_Audio_Group
 * @{
 */

/**
 * bap_audio_group.h
 *
 * \brief  Define type T_BLE_AUDIO_GROUP_HANDLE. LE audio group handle.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef void *T_BLE_AUDIO_GROUP_HANDLE;

/**
 * bap_audio_group.h
 *
 * \brief  Define type T_BLE_AUDIO_DEV_HANDLE. LE audio device handle.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef void *T_BLE_AUDIO_DEV_HANDLE;

/**
 * bap_audio_group.h
 *
 * \brief  LE audio group message type.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef enum
{
    AUDIO_GROUP_MSG_BAP_STATE                         = 0x01,
    AUDIO_GROUP_MSG_BAP_SESSION_REMOVE                = 0x02,
    AUDIO_GROUP_MSG_BAP_START_QOS_CFG                 = 0x04,
    AUDIO_GROUP_MSG_BAP_CREATE_CIS                    = 0x05,
    AUDIO_GROUP_MSG_BAP_START_METADATA_CFG            = 0x06,
    AUDIO_GROUP_MSG_BAP_SETUP_DATA_PATH               = 0x07,
    AUDIO_GROUP_MSG_BAP_REMOVE_DATA_PATH              = 0x08,
    AUDIO_GROUP_MSG_BAP_METADATA_UPDATE               = 0x09,
    AUDIO_GROUP_MSG_BAP_CIS_DISCONN                   = 0x0A,

    AUDIO_GROUP_MSG_DEV_CONN                          = 0x20,
    AUDIO_GROUP_MSG_DEV_DISCONN                       = 0x21,
    AUDIO_GROUP_MSG_DEV_BOND_REMOVE                   = 0x22,
    AUDIO_GROUP_MSG_DEV_BOND_CLEAR                    = 0x23,
    AUDIO_GROUP_MSG_DEV_EMPTY                         = 0x24,    /**< Triggered by @ref ble_audio_group_remove_dev. */
} T_AUDIO_GROUP_MSG;

/**
 * bap_audio_group.h
 *
 * \brief  LE audio device connection message data.
 *
 * The message data for @ref AUDIO_GROUP_MSG_DEV_CONN.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
} T_AUDIO_GROUP_MSG_DEV_CONN;

/**
 * bap_audio_group.h
 *
 * \brief  LE audio device disconnect message data.
 *
 * The message data for @ref AUDIO_GROUP_MSG_DEV_DISCONN.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    uint16_t               cause;
} T_AUDIO_GROUP_MSG_DEV_DISCONN;

/**
 * bap_audio_group.h
 *
 * \brief  LE audio device remove bond information message data.
 *
 * The message data for @ref AUDIO_GROUP_MSG_DEV_BOND_REMOVE.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef struct
{
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
} T_AUDIO_GROUP_MSG_DEV_BOND_REMOVE;

/**
 * bap_audio_group.h
 *
 * \brief  LE audio device information.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef struct
{
    bool                   is_used;
    T_GAP_CONN_STATE       conn_state;
    uint16_t               conn_handle;
    T_GAP_REMOTE_ADDR_TYPE addr_type;
    uint8_t                bd_addr[6];
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
} T_AUDIO_DEV_INFO;

/**
 * bap_audio_group.h
 *
 * \brief  P_FUN_GATT_CLIENT_CB Audio Group Callback Function Point Definition.
 *
 * Function pointer used in LE audio group module, to send events
 * @ref T_AUDIO_GROUP_MSG to application.
 *
 * \ingroup LE_Audio_Group_Exported_Types
 */
typedef void(*P_FUN_AUDIO_GROUP_CB)(T_AUDIO_GROUP_MSG msg, T_BLE_AUDIO_GROUP_HANDLE handle,
                                    void *buf);
/**
 * End of LE_Audio_Group_Exported_Types
 * @}
 */

/**
 * \defgroup    LE_Audio_Group_Exported_Functions LE Audio Group Exported Functions
 *
 * \ingroup LEA_GAF_LE_Audio_Group
 * @{
 */

/**
 * bap_audio_group.h
 *
 * \brief  Allocate LE audio group.
 *
 * \return         LE audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \retval NULL    Allocate LE audio group failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
T_BLE_AUDIO_GROUP_HANDLE ble_audio_group_allocate(void);

/**
 * bap_audio_group.h
 *
 * \brief  Register LE audio group callback function.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  p_fun_cb          Audio group callback function: @ref P_FUN_AUDIO_GROUP_CB.
 *
 * \return         The result of registering LE audio group callback function.
 * \retval true    Registering LE audio group callback function is successful.
 * \retval false   Registering LE audio group callback function failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
bool ble_audio_group_reg_cb(T_BLE_AUDIO_GROUP_HANDLE group_handle, P_FUN_AUDIO_GROUP_CB p_fun_cb);

/**
 * bap_audio_group.h
 *
 * \brief  Release LE audio group.
 *
 * If this API is successfully invoked, the referred group handle will be cleared.
 *
 * \param[in]  p_group_handle      Pointer to audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 *
 * \return         The result of releasing LE audio group.
 * \retval true    Releasing LE audio group is successful.
 * \retval false   Releasing LE audio group failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
bool ble_audio_group_release(T_BLE_AUDIO_GROUP_HANDLE *p_group_handle);

/**
 * bap_audio_group.h
 *
 * \brief  Add device into LE audio group.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  bd_addr           Pointer to Bluetooth device address.
 * \param[in]  addr_type         Address type.
 *
 * \return         The LE audio device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \retval NULL    Add device to LE audio group failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
T_BLE_AUDIO_DEV_HANDLE ble_audio_group_add_dev(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                               uint8_t *bd_addr, uint8_t addr_type);

/**
 * bap_audio_group.h
 *
 * \brief  Delete device from LE audio group.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  p_dev_handle      Pointer to device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 *
 * \return         The result of Deleting device from LE audio group.
 * \retval true    Deleting device from LE audio group succeeded.
 * \retval false   Deleting device from LE audio group failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
bool ble_audio_group_del_dev(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                             T_BLE_AUDIO_DEV_HANDLE *p_dev_handle);

/**
 * bap_audio_group.h
 *
 * \brief  Find device from LE audio group.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  bd_addr           Pointer to Bluetooth device address.
 * \param[in]  addr_type         Address type.
 *
 * \return         The LE audio device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \retval NULL    Finding device from LE audio group failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
T_BLE_AUDIO_DEV_HANDLE ble_audio_group_find_dev(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                                uint8_t *bd_addr, uint8_t addr_type);

/**
 * bap_audio_group.h
 *
 * \brief  Find device from LE audio group by connection handle.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  conn_handle       Connection handle.
 *
 * \return         The LE audio device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \retval NULL    Finding device from LE audio group by connection handle failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
T_BLE_AUDIO_DEV_HANDLE ble_audio_group_find_dev_by_conn_handle(T_BLE_AUDIO_GROUP_HANDLE
                                                               group_handle,
                                                               uint16_t conn_handle);

/**
 * bap_audio_group.h
 *
 * \brief  Get device information from LE audio group.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  dev_handle        Device handle: @ref T_BLE_AUDIO_DEV_HANDLE.
 * \param[in,out]  p_info        Pointer to audio device information: @ref T_AUDIO_DEV_INFO.
 *
 * \return         The result of getting device information.
 * \retval true    Getting device information succeeded.
 * \retval false   Getting device information failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
bool ble_audio_group_get_dev_info(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                  T_BLE_AUDIO_DEV_HANDLE dev_handle,
                                  T_AUDIO_DEV_INFO *p_info);

/**
 * bap_audio_group.h
 *
 * \brief  Get the number of used devices from LE audio group.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in]  check_conn        Whether to check if the connection state is connected.
 * \arg    true    Check the connection state.
 * \arg    false   Do not check the connection state.
 *
 * \return         The number of used LE audio devices.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */


uint8_t ble_audio_group_get_used_dev_num(T_BLE_AUDIO_GROUP_HANDLE group_handle, bool check_conn);

/**
 * bap_audio_group.h
 *
 * \brief  Get the number of devices from LE audio group.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 *
 * \return         The number of LE audio devices.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
uint8_t ble_audio_group_get_dev_num(T_BLE_AUDIO_GROUP_HANDLE group_handle);

/**
 * bap_audio_group.h
 *
 * \brief  Get device information from LE audio group.
 *
 * \param[in]  group_handle      Audio group handle: @ref T_BLE_AUDIO_GROUP_HANDLE.
 * \param[in,out]  p_dev_num     Pointer to device number.
 * \param[in,out]  p_dev_tbl     Pointer to device information table: @ref T_AUDIO_DEV_INFO.
 *
 * \return         The result of getting device information.
 * \retval true    Getting device information succeeded.
 * \retval false   Getting device information failed.
 *
 * \ingroup LE_Audio_Group_Exported_Functions
 */
bool ble_audio_group_get_info(T_BLE_AUDIO_GROUP_HANDLE group_handle, uint8_t *p_dev_num,
                              T_AUDIO_DEV_INFO *p_dev_tbl);
/**
 * End of LE_Audio_Group_Exported_Functions
 * @}
 */

/**
 * End of LEA_GAF_LE_Audio_Group
 * @}
 */
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
