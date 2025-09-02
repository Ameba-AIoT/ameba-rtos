/**
 * @file      rtk_bt_le_iso_def.h
 * @author    benling_xu@realsil.com.cn
 * @brief     Bluetooth LE GAP part type and function deffinition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_LE_ISO_DEF_H__
#define __RTK_BT_LE_ISO_DEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#define RTK_BLE_CIG_MAX_NUM 1
/* due to bt stack restrict, CIS ID need to be different even within different CIG Group */
/* so current CIG Group Num Should be 2, each group contains 2 cis number*/
/* CIG 1 -> CIS 1 and CIS 2
   CIG 2 -> CIS 3 and CIS 4 */
#define RTK_BLE_CIS_MAX_NUM 2
#define RTK_BLE_BIG_MAX_NUM 1
#define RTK_BLE_BIS_MAX_NUM 1
#define RTK_BLE_ISO_DEFAULT_PHY_1M (1) /**< bit 0:LE PHY 1M used. */
#define RTK_BLE_ISO_DEFAULT_PHY_2M (2) /**< bit 1:LE PHY 2M used. */
#define RTK_BLE_ISO_DEFAULT_PHY_CODED (4) /**< bit 2:LE Coded PHY used. */

#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S (10) //units: ms
#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_S_M (10) //units: ms 
#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S_US (10000) //units: us
#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_S_M_US (10000) //units: us 
#define RTK_BLE_ISO_DEFAULT_LATENCY_M_S (10)
#define RTK_BLE_ISO_DEFAULT_LATENCY_S_M (10)
#define RTK_BLE_ISO_MAX_SDU_M_S (128)   //units: byte
#define RTK_BLE_ISO_MAX_SDU_S_M (128)   //units: byte
#define RTK_BLE_ISO_DEFAULT_RTN_M_S (2)
#define RTK_BLE_ISO_DEFAULT_RTN_S_M (2)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_LE_ISO_DEF_H__ */