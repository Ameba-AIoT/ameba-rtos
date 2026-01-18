/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IPCCFG_H_
#define _AMEBA_IPCCFG_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @addtogroup IPC
  * @brief IPC driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/** @addtogroup IPC_Exported_Constants IPC Exported Constants
  * @{
  */

/** @defgroup IPC_KM0_Tx_Channel
  * @{
  */
#define IPC_N2A_TICKLESS_INDICATION		0	/*!<  KM0 -->  KM4 Tickless indicate */
#define IPC_N2A_WAKE_AP					1	/*!<  KM0 -->  KM4 Wakeup*/
#define IPC_N2A_WIFI_FW_INFO				2	/*!<  KM0 -->  KM4 FW Info*/
#define IPC_N2A_FLASHPG_REQ				3	/*!<  KM0 -->  KM4 Flash Program REQUEST*/
#define IPC_N2A_LOGUART_RX_SWITCH			4	/*!<  KM0 -->  KM4 Loguart Rx Switch*/
#define IPC_N2A_COEX_API_TRAN				5	/*!<  KM0 -->  KM4 COEX API Exchange */
// #define IPC_N2A_BT_DATA_TRAN			5	/*!<  KM0 -->  KM4 BT DATA Exchange */
#define IPC_N2A_WIFI_TRX_TRAN				6	/*!<  KM0 -->  KM4 WIFI Message Exchange */
#define IPC_N2A_WIFI_API_TRAN				7	/*!<  KM0 -->  KM4 API WIFI Message Exchange */
//#define IPC_N2A_Channel8				8
//#define IPC_N2A_Channel9				9
//#define IPC_N2A_Channel10				10
//#define IPC_N2A_Channel11				11
//#define IPC_N2A_Channel12				12
//#define IPC_N2A_Channel13				13
//#define IPC_N2A_Channel14				14
//#define IPC_N2A_Channel15				15
/**
  * @}
  */

/** @defgroup IPC_KM4_Tx_Channel
  * @{
  */
#define IPC_A2N_TICKLESS_INDICATION				0	/*!<  KM4 -->  KM0 Tickless indicate */
#define IPC_A2N_WIFI_FW_INFO						1	/*!<  KM4 -->  KM0 FW Info*/
#define IPC_A2N_WAKE_AP							2	/*!<  KM4 -->  KM0 Wakeup*/
//#define IPC_A2N_Channel3						3
#define IPC_A2N_FLASHPG_REQ						4	/*!<  KM4 -->  KM0 IFlash Program Request*/
#define IPC_A2N_COEX_API_TRAN						5	/*!<  KM4 -->  KM0 COEX API Exchange */
// #define IPC_A2N_BT_DATA_TRAN					5	/*!<  KM4 -->  KM0 BT DATA Exchange */
#define IPC_A2N_WIFI_TRX_TRAN						6	/*!<  KM4 -->  KM0WIFI Message Exchange */
#define IPC_A2N_WIFI_API_TRAN						7	/*!<  KM4 -->  KM0 WIFI API Message Exchange */
#define IPC_A2N_DIAGNOSE						8	/*!<  KM4 -->  KM0 Diagnose API Message Exchange */
//#define IPC_A2N_Channel9						9
//#define IPC_A2N_Channel10						10
//#define IPC_A2N_Channel11						11
//#define IPC_A2N_Channel12						12
//#define IPC_A2N_Channel13						13
//#define IPC_A2N_Channel14						14
//#define IPC_A2N_Channel15						15

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif

