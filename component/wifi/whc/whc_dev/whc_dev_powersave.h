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

#include "ameba_soc.h"

enum whc_tickps_cmd_subtype {
	WHC_CMD_TICKPS_R = 0,
	WHC_CMD_TICKPS_A = 1,
	WHC_CMD_TICKPS_TYPE_CG = 2,
	WHC_CMD_TICKPS_TYPE_PG = 3,
};

void whc_dev_ps_set_tickps_cmd(u8 subtype);
void whc_dev_ps_resume_cb(void);

#endif
