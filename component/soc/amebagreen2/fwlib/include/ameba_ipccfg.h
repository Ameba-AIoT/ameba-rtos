/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IPCCFG_H_
#define _AMEBA_IPCCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

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

/** @defgroup IPC_NP_Tx_Channel
  * @{
  */
#define IPC_N2A_TICKLESS_INDICATION		0	/*!<  NP -->  AP Tickless indicate */
#define IPC_N2A_WAKE_AP					1	/*!<  NP -->  AP Wakeup*/
#define IPC_N2A_WIFI_FW_INFO				2	/*!<  NP -->  AP FW Info*/
#define IPC_N2A_FLASHPG_REQ				3	/*!<  NP -->  AP Flash Program REQUEST*/
#define IPC_N2A_LOGUART_RX_SWITCH		4	/*!<  NP -->  AP Loguart Rx Switch*/
#define IPC_N2A_COEX_API_TRAN				5	/*!<  NP -->  AP COEX API Exchange */
// #define IPC_N2A_BT_DATA_TRAN			5	/*!<  NP -->  AP BT DATA Exchange */
#define IPC_N2A_WIFI_TRX_TRAN				6	/*!<  NP -->  AP WIFI Message Exchange */
#define IPC_N2A_WIFI_API_TRAN				7	/*!<  NP -->  AP API WIFI Message Exchange */
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

/** @defgroup IPC_AP_Tx_Channel
  * @{
  */
#define IPC_A2N_TICKLESS_INDICATION				0	/*!<  AP -->  NP Tickless indicate */
#define IPC_A2N_WIFI_FW_INFO						1	/*!<  AP -->  NP FW Info*/
#define IPC_A2N_WAKE_AP							2	/*!<  AP -->  NP Wakeup*/
#define IPC_A2N_LOGUART_RX_SWITCH				3	/*!<  AP -->  NP Loguart Rx Switch*/
#define IPC_A2N_FLASHPG_REQ						4	/*!<  AP -->  NP IFlash Program Request*/
#define IPC_A2N_COEX_API_TRAN						5	/*!<  AP -->  NP COEX API Exchange */
// #define IPC_A2N_BT_DATA_TRAN					5	/*!<  AP -->  NP BT DATA Exchange */
#define IPC_A2N_WIFI_TRX_TRAN						6	/*!<  AP -->  NP WIFI Message Exchange */
#define IPC_A2N_WIFI_API_TRAN						7	/*!<  AP -->  NP WIFI API Message Exchange */
#define IPC_A2N_DIAGNOSE								8	/*!<  AP -->  NP Diagnose API Message Exchange */
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

#ifdef __cplusplus
}
#endif

#endif

