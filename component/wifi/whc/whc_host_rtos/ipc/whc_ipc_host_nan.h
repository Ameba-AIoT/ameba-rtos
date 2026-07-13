/**
  ******************************************************************************
  * @file    whc_ipc_host_nan.h
  * @author
  * @version
  * @date
  * @brief   AP-core (whc_host_rtos) NAN cfgvendor command/event glue over IPC.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __WHC_IPC_HOST_NAN_H__
#define __WHC_IPC_HOST_NAN_H__

#include <stdint.h>

#ifdef CONFIG_WIFI_NAN_HOST_APP

/* forward decl: full type lives in whc_ipc.h (included via rtw_inic_common.h
 * before this header in whc_ipc_host_api.c). Callers of the public APIs below
 * do not need the full definition. */
struct whc_ipc_host_unblk_api_node;

/* ---- H2C: AP core -> NP core ---- */
/* Forward a NAN vendor (sub)command to the NP-core NAN driver. The IPC send is
 * blocking, so a temporary copy of @data is valid until NP has consumed it. */
int whc_host_nan_cfgvendor_cmd(uint16_t vendor_cmd, const void *data, uint32_t len);
int whc_host_nan_init(void);
int whc_host_nan_deinit(void);
int whc_host_nan_start(uint8_t master_pref, uint8_t band_support);
int whc_host_nan_stop(void);

/* ---- C2H: NP core -> AP core ---- */
/* Register the NAN control-layer callbacks. Payload arrives as a binary struct
 * already copied into a host-owned buffer; it is freed after the callback. */
void whc_host_nan_register_evt_cb(void (*cb)(uint16_t event_id, void *buf, uint32_t len));
void whc_host_nan_register_cmd_reply_cb(void (*cb)(void *buf, uint32_t len));

/* internal: C2H dispatch-table handlers, referenced from whc_ipc_host_api.c */
void whc_ipc_host_api_nan_cfgvendor_event_handler(struct whc_ipc_host_unblk_api_node *p_node);
void whc_ipc_host_api_nan_cfgvendor_cmd_reply_handler(struct whc_ipc_host_unblk_api_node *p_node);

#endif /* CONFIG_WIFI_NAN_HOST_APP */

#endif /* __WHC_IPC_HOST_NAN_H__ */
