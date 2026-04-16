/**
  ******************************************************************************
  * @file    rtw_task_size_8721f.h
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

#ifndef RTW_TASK_SIZE_8721F_H
#define RTW_TASK_SIZE_8721F_H

/**************** configurations for task size **********************/
#define EAP_UNBLK_API_SIZE	                      0
#define WPS_UNBLK_API_SIZE	                      0
#define MCC_LITTE_STACK_SIZE	                    0
#define RCV_WTN_ADD                               0

#if defined(CONFIG_WIFI_EAP_ENABLE)
#undef  EAP_UNBLK_API_SIZE
#define EAP_UNBLK_API_SIZE	                      1880  /* enable EAP need 1880 */
#endif
#if defined(CONFIG_ENABLE_WPS)
#undef  WPS_UNBLK_API_SIZE
#define WPS_UNBLK_API_SIZE                        744  /* enable WPS need 744 */
#endif
#if defined(CONFIG_WIFI_MCC_ENABLE)
#undef  MCC_LITTE_STACK_SIZE
#define MCC_LITTE_STACK_SIZE                      776  /* enable MCC need 776 */
#endif

#if defined(CONFIG_RMESH_EN)
#undef  RCV_WTN_ADD
#define RCV_WTN_ADD	                              480
#endif

#define COEX_IPC_HST_API_BASIC_SIZE               264
#define COEX_IPC_DEV_API_BASIC_SIZE               680

#define WIFI_STACK_SIZE_LITTLE_TASKLET	         	((376 + MCC_LITTE_STACK_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* rtw_if_wifi_create_task: size will *4 */

#if defined (CONFIG_WHC_DEV)
#if defined(CONFIG_WHC_INTF_IPC)
#define WIFI_STACK_SIZE_SINGLE_TASKLET	          ((1344 + RCV_WTN_ADD + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* max 1648 in lite, rtw_if_wifi_create_task: size will *4 */
#else
#define WIFI_STACK_SIZE_SINGLE_TASKLET	          ((3056 + RCV_WTN_ADD + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* 1k: for sdio fullmac proxy */
#endif
#else
#define WIFI_STACK_SIZE_SINGLE_TASKLET	          (1024 + RCV_WTN_ADD)		/* considering single core */
#endif

#define WIFI_STACK_SIZE_DRIVER_RESUME_TASKLET     ((368 + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* rtw_if_wifi_create_task: size will *4 */

#define WIFI_STACK_SIZE_INIC_IPC_HST_API          (336 + CONTEXT_SAVE_SIZE_WITH_MARGIN)	// for psp overflow when update group key: jira: https://jira.realtek.com/browse/RSWLANQC-1027
#define WIFI_STACK_SIZE_INIC_IPC_HST_EVT_API      (1848 + EAP_UNBLK_API_SIZE + WPS_UNBLK_API_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN)

#if defined(CONFIG_WHC_HOST)
#define WIFI_STACK_SIZE_INIC_MSG_Q	              (488 + CONTEXT_SAVE_SIZE_WITH_MARGIN)
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
#define WIFI_STACK_SIZE_INIC_IPC_DEV_API          (1384 + CONTEXT_SAVE_SIZE_WITH_MARGIN)	/* max 1072 in lite */
#endif

#endif /*#ifndef RTW_TASK_SIZE_8721F_H */
