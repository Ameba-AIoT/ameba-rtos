/**
  ******************************************************************************
  * @file    rtw_task_size_8721da.h
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

#ifndef RTW_TASK_SIZE_8721DA_H
#define RTW_TASK_SIZE_8721DA_H

/**********************************************************************************************
 *                               Advance feature task size
 *********************************************************************************************/
#define EAP_UNBLK_API_SIZE	                      0
#define WPS_UNBLK_API_SIZE	                      0
#define MCC_LITTE_THREAD_SIZE	                    0
#define WTN_SINGLE_THREAD_SIZE                    0

#if defined(CONFIG_WIFI_EAP_ENABLE)
#undef  EAP_UNBLK_API_SIZE
#define EAP_UNBLK_API_SIZE	                      1368  /* enable EAP need 1368 */
#endif
#if defined(CONFIG_ENABLE_WPS)
#undef  WPS_UNBLK_API_SIZE
#define WPS_UNBLK_API_SIZE                        748  /* enable WPS need 748 */
#endif
#if defined(CONFIG_WIFI_MCC_ENABLE)
#undef  MCC_LITTE_THREAD_SIZE
#define MCC_LITTE_THREAD_SIZE                     432  /* enable MCC need 432 */
#endif

#if defined(CONFIG_RMESH_EN)
#undef  WTN_SINGLE_THREAD_SIZE
#define WTN_SINGLE_THREAD_SIZE	                  480
#endif

/**********************************************************************************************
 *                               Common task size
 *********************************************************************************************/
#define WIFI_TASK_SIZE_LITTLE_THREAD		          ((376 + MCC_LITTE_THREAD_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* rtw_if_wifi_create_task: size will *4 */

#if defined (CONFIG_WHC_DEV)
#if defined(CONFIG_WHC_INTF_IPC)
#define WIFI_TASK_SIZE_SINGLE_THREAD		          ((1392 + WTN_SINGLE_THREAD_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* max 1648 in lite, rtw_if_wifi_create_task: size will *4 */
#else
#define WIFI_TASK_SIZE_SINGLE_THREAD		          ((3056 + WTN_SINGLE_THREAD_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* 1k: for sdio fullmac proxy */
#endif
#else
#define WIFI_TASK_SIZE_SINGLE_THREAD		          (1024 + WTN_SINGLE_THREAD_SIZE)		/* considering single core */
#endif

#define WIFI_TASK_SIZE_DRIVER_RESUME_THREAD       ((368 + CONTEXT_SAVE_SIZE_WITH_MARGIN) / 4)	/* rtw_if_wifi_create_task: size will *4 */

/**********************************************************************************************
 *                               IPC mode task size
 *********************************************************************************************/
#define COEX_IPC_HST_API_BASIC_SIZE               288
#define COEX_IPC_DEV_API_BASIC_SIZE               632

#define WIFI_TASK_SIZE_WHC_IPC_HST_API            (376 + CONTEXT_SAVE_SIZE_WITH_MARGIN)	// for psp overflow when update group key: jira: https://jira.realtek.com/browse/RSWLANQC-1027
#define WIFI_TASK_SIZE_WHC_IPC_HST_EVT_API        (1880 + EAP_UNBLK_API_SIZE + WPS_UNBLK_API_SIZE + CONTEXT_SAVE_SIZE_WITH_MARGIN)

#if defined(CONFIG_WHC_HOST)
#define WIFI_TASK_SIZE_WHC_IPC_MSG_Q	            (512 + CONTEXT_SAVE_SIZE_WITH_MARGIN)
#elif defined(CONFIG_WHC_DEV)
#define WIFI_TASK_SIZE_WHC_IPC_MSG_Q	            (688 + CONTEXT_SAVE_SIZE_WITH_MARGIN)
#endif

#if defined(CONFIG_MP_INCLUDED) || defined(CONFIG_PHYDM_CMD) /*halbb debug cmd need bigger stack size, for sscanf format float*/
#define WIFI_TASK_SIZE_WHC_IPC_DEV_API            (1024*4)
#elif defined(CONFIG_NAN)
#define WIFI_TASK_SIZE_WHC_IPC_DEV_API            (256*11)
#elif defined(CONFIG_PLATFORM_ZEPHYR)
#define WIFI_TASK_SIZE_WHC_IPC_DEV_API            (1024*4)
#else
#define WIFI_TASK_SIZE_WHC_IPC_DEV_API            (1072 + CONTEXT_SAVE_SIZE_WITH_MARGIN)	/* max 1072 in lite */
#endif

/**********************************************************************************************
 *                               Card mode task size
 *********************************************************************************************/
/* non-ipc intf host/dev api task size*/
#if defined(CONFIG_PHYDM_CMD) || defined(NAN_CUSTOMER_NANDOW) /*halbb debug cmd need bigger stack size*/
#define WIFI_TASK_SIZE_WHC_DEV_API                (1024*4)
#else
#define WIFI_TASK_SIZE_WHC_DEV_API                (512*4)
#endif

#define WIFI_TASK_SIZE_WHC_HST_API                (2848 + CONTEXT_SAVE_SIZE_WITH_MARGIN)	// for psp overflow when update group key: jira: https://jira.realtek.com/browse/RSWLANQC-1027

#endif /*#ifndef RTW_TASK_SIZE_8721DA_H */
