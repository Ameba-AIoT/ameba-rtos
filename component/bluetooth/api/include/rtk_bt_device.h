/**
 * @file      rtk_bt_device.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth device related function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_DEVICE_H__
#define __RTK_BT_DEVICE_H__

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup  BT_device BT Device APIs
 * @brief     BT device function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Check whether BT is enabled.
 * @return
 *            - true: BT is enabled.
 *            - false: BT is not enabled.
 */
bool rtk_bt_is_enable(void);

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
/**
 * @brief     Check whether the BT Mesh stack is enabled.
 * @return
 *            - true: The BT Mesh stack is enabled.
 *            - false: The BT Mesh stack is not enabled.
 */
bool rtk_bt_mesh_is_enable(void);
#endif

/**
 * @brief     Enable BT.
 *            This initializes the BT event task and the stack, and starts the profiles selected in the configuration.
 * @param[in] app_default_conf: Default application configuration.
 * @return
 *            - @ref rtk_bt_err_common
 * @note      - If BT is already enabled, this function fails with @ref RTK_BT_ERR_ALREADY_DONE.
 *            - BT APIs other than the BT enable status checks shall be called only after BT is enabled successfully.
 */
uint16_t rtk_bt_enable(rtk_bt_app_conf_t *app_default_conf);

/**
 * @brief     Disable BT.
 *            This waits for the pending API commands to be sent, then deinitializes the stack and the BT event task.
 * @return
 *            - @ref rtk_bt_err_common
 * @note      - If BT is not enabled, this function fails with @ref RTK_BT_ERR_ALREADY_DONE.
 *            - All registered event callbacks are reset, and need to be registered again after BT is enabled again.
 */
uint16_t rtk_bt_disable(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_DEVICE_H__ */
