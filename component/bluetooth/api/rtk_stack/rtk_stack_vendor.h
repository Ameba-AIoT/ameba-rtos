/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __RTK_STACK_VENDOR_H__
#define __RTK_STACK_VENDOR_H__

#include <bt_vendor_config.h>

#if defined(VENDOR_CMD_LE_EXTENSION_FEATURE_SUPPORT) && VENDOR_CMD_LE_EXTENSION_FEATURE_SUPPORT
#define HCI_EXT_SUB_SET_CONTROLLER_PREFERRED_TX_CI     0x12

typedef enum {
	GAP_AE_CODING_SCHEME_S8 = 2,
	GAP_AE_CODING_SCHEME_S2 = 3,
} T_GAP_AE_CODING_SCHEME;

#endif

#if defined(VENDOR_CMD_PLATFORM_EXTENSION_FEATURE_SUPPORT) && VENDOR_CMD_PLATFORM_EXTENSION_FEATURE_SUPPORT
#define HCI_VENDOR_EXT_SUB_SET_FEATURE                       5
#define HCI_VENDOR_BT_FW_FEATURE_LEGACY_ADV_ON_CODED_PHY     0x10
#endif

#endif
