// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_REGD_H__
#define __RTW_REGD_H__

#define CHPLAN_REGD_ENT(_chplan, _regd) \
	{.chplan = (_chplan), \
	 .regd = (_regd) \
	}

struct chplan_regd {
	u8 chplan;
	const struct ieee80211_regdomain *regd;
};

#endif //__RTW_REGD_H__
