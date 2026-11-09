/**
 * @file      rtk_bt_le_iso_def.h
 * @author    atonio_liu@realsil.com.cn
 * @brief     Bluetooth LE ISO default parameter definitions.
 * @copyright Copyright (c) 2025. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_LE_ISO_DEF_H__
#define __RTK_BT_LE_ISO_DEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#define RTK_BLE_CIG_MAX_NUM 1                              /*!< Maximum number of CIGs. */
/* CIS IDs must be unique across all CIGs due to a BT stack restriction, even when the CISes belong to different CIGs.
 * The CISes are distributed evenly among the CIGs, e.g. with two CIGs of two CISes each:
 * CIG 1 -> CIS 1 and CIS 2
 * CIG 2 -> CIS 3 and CIS 4 */
#define RTK_BLE_CIS_MAX_NUM 2                              /*!< Maximum number of CISes in a CIGs. */
#define RTK_BLE_BIG_MAX_NUM 1                              /*!< Maximum number of BIGs. */
#define RTK_BLE_BIS_MAX_NUM 1                              /*!< Maximum number of BISes in a BIG. */
#define RTK_BLE_ISO_DEFAULT_PHY_1M (1)                     /*!< Default PHY bit mask: LE 1M PHY (bit 0). */
#define RTK_BLE_ISO_DEFAULT_PHY_2M (2)                     /*!< Default PHY bit mask: LE 2M PHY (bit 1). */
#define RTK_BLE_ISO_DEFAULT_PHY_CODED (4)                  /*!< Default PHY bit mask: LE Coded PHY (bit 2). */

#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S (10)          /*!< Default SDU interval from central to peripheral.
                                                            *   Units: ms. */
#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_S_M (10)          /*!< Default SDU interval from peripheral to central.
                                                            *   Units: ms. */
#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_M_S_US (10000)    /*!< Default SDU interval from central to peripheral.
                                                            *   Units: microseconds. */
#define RTK_BLE_ISO_DEFAULT_SDU_INTERVAL_S_M_US (10000)    /*!< Default SDU interval from peripheral to central.
                                                            *   Units: microseconds. */
#define RTK_BLE_ISO_DEFAULT_LATENCY_M_S (10)               /*!< Default maximum transport latency from central to peripheral.
                                                            *   Units: ms. */
#define RTK_BLE_ISO_DEFAULT_LATENCY_S_M (10)               /*!< Default maximum transport latency from peripheral to central.
                                                            *   Units: ms. */
#define RTK_BLE_ISO_MAX_SDU_M_S (128)                      /*!< Default maximum SDU size from central to peripheral.
                                                            *   Units: octets. */
#define RTK_BLE_ISO_MAX_SDU_S_M (128)                      /*!< Default maximum SDU size from peripheral to central.
                                                            *   Units: octets. */
#define RTK_BLE_ISO_DEFAULT_RTN_M_S (2)                    /*!< Default number of retransmissions from central to peripheral. */
#define RTK_BLE_ISO_DEFAULT_RTN_S_M (2)                    /*!< Default number of retransmissions from peripheral to central. */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_LE_ISO_DEF_H__ */
