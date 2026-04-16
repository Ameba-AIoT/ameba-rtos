/**
  ******************************************************************************
  * @file    rtw_task_size_8730e.h
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

#ifndef RTW_TASK_SIZE_8730E_H
#define RTW_TASK_SIZE_8730E_H

/**************** configurations for task size **********************/
#define EAP_UNBLK_API_SIZE	                      0
#define WPS_UNBLK_API_SIZE	                      0
#define MCC_LITTE_STACK_SIZE	                    0
#define RCV_WTN_ADD                               0

#if defined(CONFIG_WIFI_EAP_ENABLE)
#undef  EAP_UNBLK_API_SIZE
#define EAP_UNBLK_API_SIZE	                      1448  /* enable EAP need 1448 */
#endif
#if defined(CONFIG_ENABLE_WPS)
#undef  WPS_UNBLK_API_SIZE
#define WPS_UNBLK_API_SIZE                        772  /* enable WPS need 772 */
#endif
#if defined(CONFIG_WIFI_MCC_ENABLE)
#undef  MCC_LITTE_STACK_SIZE
#define MCC_LITTE_STACK_SIZE                      616  /* enable MCC need 616 */
#endif

#if defined(CONFIG_RMESH_EN)
#undef  RCV_WTN_ADD
#define RCV_WTN_ADD	                              480
#endif

#define COEX_IPC_HST_API_BASIC_SIZE               376
#define COEX_IPC_DEV_API_BASIC_SIZE               664

#define WIFI_STACK_SIZE_LITTLE_TASKLET	         	((368 + MCC_LITTE_STACK_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* rtw_if_wifi_create_task: size will *4 */

#if defined (CONFIG_WHC_DEV)
#if defined(CONFIG_WHC_INTF_IPC)
#define WIFI_STACK_SIZE_SINGLE_TASKLET	          ((1360 + RCV_WTN_ADD + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* max 1648 in lite, rtw_if_wifi_create_task: size will *4 */
#else
#define WIFI_STACK_SIZE_SINGLE_TASKLET	          ((3056 + RCV_WTN_ADD + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* 1k: for sdio fullmac proxy */
#endif
#else
#define WIFI_STACK_SIZE_SINGLE_TASKLET	          (1024 + RCV_WTN_ADD)		/* considering single core */
#endif

#define WIFI_STACK_SIZE_DRIVER_RESUME_TASKLET     ((WIFI_DRIVER_RESUME_TASKLET_BASIC_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* rtw_if_wifi_create_task: size will *4 */

#define WIFI_STACK_SIZE_INIC_IPC_HST_API          (440 + CONTEXT_SAVE_SIZE_WITH_MARGIN)	// for psp overflow when update group key: jira: https://jira.realtek.com/browse/RSWLANQC-1027
#define WIFI_STACK_SIZE_INIC_IPC_HST_EVT_API      (1856 + EAP_UNBLK_API_SIZE + WPS_UNBLK_API_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN)

#if defined(CONFIG_WHC_HOST)
#define WIFI_STACK_SIZE_INIC_MSG_Q	              (576 + CONTEXT_SAVE_SIZE_WITH_MARGIN)
#elif defined(CONFIG_WHC_DEV)
#define WIFI_STACK_SIZE_INIC_MSG_Q	              (688 + CONTEXT_SAVE_SIZE_WITH_MARGIN)
#endif

#if defined(CONFIG_MP_INCLUDED) || defined(CONFIG_PHYDM_CMD) /*halbb debug cmd need bigger stack size, for sscanf format float*/
#define WIFI_STACK_SIZE_INIC_IPC_DEV_API          (1024*4)
#elif defined(CONFIG_NAN)
#define WIFI_STACK_SIZE_INIC_IPC_DEV_API          (256*11)
#elif defined(CONFIG_PLATFORM_ZEPHYR)
#define WIFI_STACK_SIZE_INIC_IPC_DEV_API          (1024*4)
#else
#define WIFI_STACK_SIZE_INIC_IPC_DEV_API          (1280 + CONTEXT_SAVE_SIZE_WITH_MARGIN)	/* max 1072 in lite */
#endif

#endif /*#ifndef RTW_TASK_SIZE_8730A_H */
