// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_LLHW_EVENT_H__
#define __RTW_LLHW_EVENT_H__

/**
 * @brief  The enumeration is join block param.
 */
struct internal_block_param {
	struct completion	sema;
};

#endif //__RTW_LLHW_EVENT_H__
