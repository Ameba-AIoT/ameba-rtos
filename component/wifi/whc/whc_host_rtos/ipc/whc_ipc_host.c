/**
  ******************************************************************************
  * @file    inic_ipc_host.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "whc_ipc_host_trx.h"
#include "whc_ipc_msg_queue.h"
#include "whc_ipc_cfg.h"
#define CONFIG_ENABLE_CACHE

/* -------------------------------- Defines --------------------------------- */

/* -------------------------------- Macros ---------------------------------- */

/* --------------------------- Private Variables ---------------------------- */

/* ---------------------------- Private Functions --------------------------- */

extern struct host_priv g_inic_host_priv;

/**
 * @brief  to initialize the ipc host for the inic.
 * @param  none.
 * @return none.
 */
void whc_ipc_host_init(void)
{
	whc_ipc_host_heap_statistics(1);
	/* Initialize the parameters of ipc host. */
	whc_ipc_host_init_priv();

	whc_ipc_host_api_init();

	/* wait for inic_ipc_device ready, after that send WIFI_ON ipc msg to device */
	while ((HAL_READ32(REG_AON_WIFI_IPC, 0) & AON_BIT_WIFI_INIC_NP_READY) == 0) {
		rtos_time_delay_ms(1);
	}
	u32 val32 = HAL_READ32(REG_AON_WIFI_IPC, 0);
	val32 &= ~ AON_BIT_WIFI_INIC_NP_READY;
	HAL_WRITE32(REG_AON_WIFI_IPC, 0, val32);

	g_inic_host_priv.host_init_done = 1;
}

void whc_ipc_host_heap_statistics(u8 start)
{
	static u32 start_heap = 0;

	if (start) {
		start_heap = rtos_mem_get_free_heap_size();
		RTK_LOGS(TAG_WLAN_DRV, RTK_LOG_INFO, "AP heap: %d\n", start_heap);
	} else {
		u32 end_heap = rtos_mem_get_free_heap_size();
		RTK_LOGS(TAG_WLAN_DRV, RTK_LOG_INFO, "AP heap: %d, wifi used: %d\n", end_heap, start_heap - end_heap);
	}
}
/* ---------------------------- Global Variables ---------------------------- */
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_host_event_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = whc_ipc_host_trx_int_hdl,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_DIR_MSG_RX,
	.IPC_Channel = IPC_D2H_WIFI_TRX_TRAN
};

IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE ipc_api_host_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = whc_ipc_host_api_int_hdl,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_DIR_MSG_RX,
	.IPC_Channel = IPC_D2H_WIFI_API_TRAN
};
