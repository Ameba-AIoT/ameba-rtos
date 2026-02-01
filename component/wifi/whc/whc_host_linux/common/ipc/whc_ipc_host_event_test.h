// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __WHC_EVENT_TEST__
#define __WHC_EVENT_TEST__

void whc_host_event_auth_req(u8 *buf, s32 buf_len);
void whc_host_event_auth_resp(u8 *buf, s32 buf_len);
void whc_host_event_assoc_req(u8 *buf, s32 buf_len);
void whc_host_event_assoc_resp(u8 *buf, s32 buf_len);

#endif /* __WHC_EVENT_TEST__ */
