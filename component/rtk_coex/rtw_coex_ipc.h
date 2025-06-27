/**
  ******************************************************************************
  * @file    rtw_coex_ipc.h
  * @author
  * @version
  * @date
  * @brief   Routines to access hardware
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __RTW_COEX_IPC_H__
#define __RTW_COEX_IPC_H__

/* -------------------------------- Includes -------------------------------- */
#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rtw_coex_api_ext.h"

/* -------------------------------- Defines --------------------------------- */

/* -------------------------------- Macros ---------------------------------- */
#define HOST_MSG_PARAM_NUM		(9)
#define HOST_MSG_DUMY_NUM		(64 - (HOST_MSG_PARAM_NUM + 2) * 4)
#define DEV_MSG_PARAM_NUM		(8)
#define DEV_MSG_DUMY_NUM		(64 - (DEV_MSG_PARAM_NUM + 2) * 4)


/* ------------------------------- Data Types ------------------------------- */
struct coex_ipc_dev_req_msg {
	u32	enevt_id;
	u32	param_buf[DEV_MSG_PARAM_NUM];
	int	ret;
	u8	dummy[DEV_MSG_DUMY_NUM]; /* add for 64B size alignment */
};

struct coex_ipc_host_req_msg {
	u32	api_id;
	u32	param_buf[HOST_MSG_PARAM_NUM];
	int	ret;
	u8 dummy[HOST_MSG_DUMY_NUM];//add for 64B size alignment
};

/* -------------------------- Function declaration -------------------------- */
/*for ipc host*/
void coex_ipc_host_init(void);

/*for ipc device*/
void coex_ipc_dev_init(void);

/*for ipc host api*/
void coex_ipc_api_init_host(void);
void coex_ipc_api_host_int_hdl(void *Data, u32 IrqStatus, u32 ChanNum);
int coex_ipc_api_host_message_send(u32 id, u32 *param_buf, u32 buf_len);
void coex_ipc_api_send_init_host(void);

/*for ipc dev api*/
void coex_ipc_api_init_dev(void);
void coex_ipc_api_dev_int_hdl(void *Data, u32 IrqStatus, u32 ChanNum);
int coex_ipc_api_dev_message_send(u32 id, u32 *param_buf, u32 buf_len);

/*for ipc init entry*/
static inline void coex_ipc_entry(void)
{
// ipc@coex init
#if defined(CONFIG_WHC_INTF_IPC)
#if defined(CONFIG_COEXIST_HOST)
	coex_ipc_host_init();
#endif
#if defined(CONFIG_COEXIST_DEV)
	coex_ipc_dev_init();
#endif
#endif

#if defined(CONFIG_COEX_EXT_CHIP_SUPPORT)
// ipc case-rtos:
// ext-paras init start from ap
#if defined(CONFIG_WHC_INTF_IPC) && defined(CONFIG_COEXIST_HOST)
	// init from ap->np
	extern struct extchip_para_t g_extchip_para_ap;
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[COEX][Host] Ext paras init.\r\n");
	coex_extc_paras_config(&g_extchip_para_ap);
	coex_ipc_api_send_init_host();
#endif

// ipc case-linux:
// no-ipc case-fullmac/bridge/singlecore:
// ext-paras init start from np
#if (((defined(CONFIG_WHC_INTF_IPC) && defined(CONFIG_LINUX_FW_EN)) \
    || (!defined(CONFIG_WHC_INTF_IPC))) && defined(CONFIG_COEXIST_DEV))
	// ipc@coex not support, temporarily solution for smart linux
	extern struct extchip_para_t g_extchip_para;
	extern void rtk_coex_extc_set_enable(bool enable);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[COEX][Dev] Ext paras init.\r\n");
	coex_extc_paras_config(&g_extchip_para);
	rtk_coex_extc_set_enable(true);
#endif
#endif
}
#endif /* __RTW_COEX_IPC_H__ */
