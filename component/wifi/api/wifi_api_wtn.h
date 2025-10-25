/**
  ******************************************************************************
  * @file    wifi_api_wtn.h
  * @author
  * @version
  * @date
  * @brief   This file provides user interface for Wi-Fi station and AP mode
  *          configuration base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WIFI_API_WTN_H
#define __WIFI_API_WTN_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * @brief  Register callback function to process rmesh ota related packet.
 * @param[in]  ota_callback: the callback function
 * @return  None.
 */
void wifi_rmesh_register_ota_callback(int (*ota_callback)(u8 *buf, u16 len));

/**
 * @brief  Get information of rmesh node.
 * @param[in]  type: rmesh node type:
 *               - @ref RMESH_SELF_NODE
 *               - @ref RMESH_FATHER_NODE
 *               - @ref RMESH_ROOT_NODE
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Get node info fail.
 */
int wifi_rmesh_get_node_info(enum rtw_rmesh_node_type type, struct rtw_rmesh_node_info *node_info);

/**
 * @brief  Get the rmesh child number of this node.
 * @return  The child number.
 */
u8 wifi_rmesh_get_child_num(void);

/**
 * @brief  Get information about child nodes of this node.
 * @param[inout]  child_num: Pointer to store the actual child number of this node.
 *                          - Input: Desired number of child node;
 *                          - Output: Actual number of child node.
 * @param[in]  child_info_list: A buffer stores child info list, each entry as struct rtw_rmesh_node_info.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : Get node info fail.
 */
int wifi_rmesh_get_child_info_list(u8 *child_num, struct rtw_rmesh_node_info *child_info_list);

/**
 * @brief  Update current OTA version to rmesh driver.
 * @param[in]  ota_ver: Pointer to store the OTA version.
 * @param[in]  ota_ver_len: The length of OTA version.
 * @param[in]  ota_ongoing: Update OTA version due to a new OTA is ongoing.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : OTA version update to rmesh driver failed.
 */
int wifi_rmesh_update_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, u8 ota_ongoing);

/**
 * @brief  Check whether rmesh father node or root node's OTA version is same as the target OTA version.
 * @param[in]  ota_ver: Pointer to store the target OTA version.
 * @param[in]  ota_ver_len: The length of target OTA version.
 * @param[in]  node_type: The target node to compare OTA version .
 *                    - @ref RMESH_FATHER_NODE
 *                    - @ref RMESH_ROOT_NODE
 * @return
 *    - @ref RTK_SUCCESS : The target node's OTA version is the same as the target OTA version.
 *    - @ref RTK_FAIL : The target node's OTA version is different from the target OTA version.
 */
int wifi_rmesh_check_node_ota_ver(u8 *ota_ver, u8 ota_ver_len, enum rtw_rmesh_node_type node_type);

/**
 * @brief  Choose the specified node as father if it can be scanned.
 * @param[in]  enable: whether to choose a fixed father.
 * @param[in]  father_mac: The MAC address of the target father node.
 * @return
 *    - @ref RTK_SUCCESS : The API executed successfully.
 *    - @ref RTK_FAIL : The API execution failed.
 */
int wifi_rmesh_fix_father(u8 enable, u8 *father_mac);

/** @} End of Extended_Functions group */
/** @} End of WIFI_Exported_Functions group*/
/** @} End of WIFI_API group*/

#ifdef __cplusplus
}
#endif

#endif // __WIFI_API_WTN_H

