// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_DEV_POWERSAVE_H__
#define __WHC_DEV_POWERSAVE_H__

void whc_dev_tickps_cmd(struct whc_ps_cmd *ps_cmd);
void whc_dev_ps_set_tickps_cmd(uint8_t subtype);
void whc_dev_ps_resume_cb(void);

#endif
