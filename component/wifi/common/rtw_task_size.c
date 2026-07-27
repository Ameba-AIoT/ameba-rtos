/******************************************************************************
* @file    rtw_task_size.c
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

#include "rtw_inic_common.h"

struct rtw_task_size g_rtw_task_size = {0};

void wifi_set_task_size(void)
{
	/* Common task size: IPC mode, single core, card mode all need */
#if defined(CONFIG_WHC_NONE) || defined(CONFIG_WHC_DEV)
	g_rtw_task_size.little_task = WIFI_LITTLE_TASK_SIZE;
	g_rtw_task_size.single_task = WIFI_SINGLE_TASK_SIZE;
#if defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
	g_rtw_task_size.resume_task = WIFI_DRIVER_RESUME_TASK_SIZE;
#endif
#endif

	/* IPC mode task size */
#if defined(CONFIG_WHC_INTF_IPC)
#if defined(CONFIG_WHC_HOST)
	g_rtw_task_size.ipc_unblk_api_task = WIFI_WHC_IPC_HST_EVT_API_TASK_SIZE;
	g_rtw_task_size.ipc_blk_api_task = WIFI_WHC_IPC_HST_API_TASK_SIZE;
	g_rtw_task_size.ipc_msg_q_task = WIFI_WHC_IPC_MSG_Q_TASK_SIZE;
#elif defined(CONFIG_WHC_DEV)
	g_rtw_task_size.ipc_dev_api_task = WIFI_WHC_IPC_DEV_API_TASK_SIZE;
#endif
	/* Card mode task size */
#else
#if defined(CONFIG_WHC_HOST)
	g_rtw_task_size.whc_hst_api_task = WIFI_WHC_HST_API_TASK_SIZE;
#elif defined(CONFIG_WHC_DEV)
	g_rtw_task_size.whc_dev_api_task = WIFI_WHC_DEV_API_TASK_SIZE;
#endif
#endif
}
