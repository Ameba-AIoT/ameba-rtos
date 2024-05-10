/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BT_BOND_MGR_
#define _BT_BOND_MGR_

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

/**
 * \defgroup    LEA_LIB BLE Audio Lib
 *
 * \brief   General interface accessing to LE Audio Lib.
 */

/**
 * \defgroup    LEA_LIB_Exported_Types BLE Audio Lib Exported Types
 *
 * \ingroup LEA_LIB
 * \{
 */

/**
 * bt_bond_mgr.h
 *
 * \brief  Define Bluetooth Bond Check Function Type.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef bool (*P_BT_BOND_CHECK)(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle);

/**
 * bt_bond_mgr.h
 *
 * \brief  Define Bluetooth Bond Get Key Function Type.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef bool (*P_BT_BOND_GET_KEY)(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle,
                                  bool remote, uint8_t *p_key_len, uint8_t *p_key);

/**
 * bt_bond_mgr.h
 *
 * \brief  Define Bluetooth Bond Get Max Bond Number Function Type.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef uint8_t (*P_BT_BOND_GET_MAX_NUM)(bool is_le);

/**
 * bt_bond_mgr.h
 *
 * \brief  Define Bluetooth Bond Get Address Function Type.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef bool (*P_BT_BOND_GET_ADDR)(bool is_le, uint8_t bond_idx, uint8_t *bd_addr,
                                   uint8_t *p_bd_type, uint8_t *local_bd_addr,
                                   uint8_t *p_local_bd_type);

/**
 * bt_bond_mgr.h
 *
 * \brief  Define Bluetooth Bond Set CCCD Flag Function Type.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef bool (*P_BT_BOND_SET_CCCD_FLAG)(uint8_t *bd_addr, uint8_t bd_type, uint8_t *local_bd_addr,
                                        uint8_t local_bd_type,
                                        uint16_t cccd_handle, uint16_t flags);

/**
 * bt_bond_mgr.h
 *
 * \brief  Define Bluetooth Bond Clear CCCD Flag Function Type.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef bool (*P_BT_BOND_CLEAR_CCCD_FLAG)(uint8_t *bd_addr, uint8_t bd_type, uint16_t conn_handle,
                                          uint16_t cccd_handle);

/**
 * bt_bond_mgr.h
 *
 * \brief  Define Bluetooth Bond Resolve Resolvable Private Address Function Type.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef bool (*P_BT_BOND_LE_RESOLVE_RPA)(uint8_t *unresolved_addr, uint8_t *identity_addr,
                                         uint8_t *p_identity_addr_type);

/**
 * bt_bond_mgr.h
 *
 * \brief  Bluetooth bond manager types.
 *
 * \ingroup LEA_LIB_Exported_Types
 */
typedef struct
{
    P_BT_BOND_CHECK             bond_check;
    P_BT_BOND_GET_KEY           bond_get_key;
    P_BT_BOND_GET_MAX_NUM       bond_get_max_num;
    P_BT_BOND_GET_ADDR          bond_get_addr;
    P_BT_BOND_SET_CCCD_FLAG     bond_set_cccd_flag;
    P_BT_BOND_CLEAR_CCCD_FLAG   bond_clear_cccd_flag;
    P_BT_BOND_LE_RESOLVE_RPA    bond_le_resolve_rpa;
} T_BT_BOND_MGR;
/**
 * End of LEA_LIB_Exported_Types
 * \}
 */

/**
 * \defgroup    LEA_LIB_Exported_Functions BLE Audio Lib Exported Functions
 *
 * \ingroup LEA_LIB
 * \{
 */

/**
 * bt_bond_mgr.h
 *
 * \brief  Register bt bond manager callback functions.
 *
 *         If Application initializes the app bond mgr module by calling bt_bond_init(),
 *         the application shall to call this function to register the bond manager callback.
 *
 * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
 *
 * \param[in]  p_mgr      BT bond manager callback types: @ref T_BT_BOND_MGR.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        bt_bond_init();
        ble_audio_bond_init();
    }

    const T_BT_BOND_MGR ble_audio_bond =
    {
        .bond_check             = ble_audio_bond_check,
        .bond_get_key           = ble_audio_bond_get_key,
        .bond_get_max_num       = ble_audio_bond_get_max_num,
        .bond_get_addr          = ble_audio_bond_get_addr,
        .bond_set_cccd_flag     = ble_audio_bond_set_cccd_flag,
        .bond_clear_cccd_flag   = ble_audio_bond_clear_cccd_flag,
        .bond_le_resolve_rpa    = ble_audio_bond_le_resolve_rpa,
    };

    void ble_audio_bond_init(void)
    {
        ble_audio_reg_bond_mgr(&ble_audio_bond);
        bt_bond_register_app_cb(ble_audio_bt_bond_cb);
    }
 * \endcode

 * \ingroup LEA_LIB_Exported_Functions
 */
void ble_audio_reg_bond_mgr(const T_BT_BOND_MGR *p_mgr);
/**
 * End of LEA_LIB_Exported_Functions
 * \}
 */

#ifdef  __cplusplus
}
#endif
#endif
