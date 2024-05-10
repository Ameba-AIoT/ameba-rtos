/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     data_uart.h
  * @brief    Head file for Data uart operations.
  * @details  Data uart init and print vital data through data uart.
  * @author   bill
  * @date     2015-03-19
  * @version  v2.0
  * *************************************************************************************
  */

#ifndef _DATA_UART_H_
#define _DATA_UART_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "platform_misc.h"
/****************************************************************************************************************
* macros that other .c files may use all defined here
****************************************************************************************************************/
/**
 * @addtogroup DATA_UART
 * @{
 */

/**
 * @defgroup Data_Uart_Exported_Types Data Uart Exported Types
 * @brief
 * @{
 */

typedef enum
{
    DATA_UART_BAUDRATE_1200,
    DATA_UART_BAUDRATE_9600,
    DATA_UART_BAUDRATE_19200,
    DATA_UART_BAUDRATE_38400,
    DATA_UART_BAUDRATE_115200,
    DATA_UART_BAUDRATE_921600,
    DATA_UART_BAUDRATE_1000000,
    DATA_UART_BAUDRATE_2000000,
    DATA_UART_BAUDRATE_NUM
} data_uart_baudrate_t;
/** @brief  Print data use Data UART. */
typedef void (*pf_send_msg_from_isr_t)(uint8_t data);
/** @} */

/****************************************************************************************************************
* exported functions other .c files may use all defined here.
****************************************************************************************************************/
/**
 * @defgroup Data_Uart_Exported_Functions Data Uart Exported Functions
 * @brief
 * @{
 */
/* Print formated data use Data UART  */
extern void data_uart_debug(char *fmt, ...);
extern void data_uart_dump(uint8_t *pbuffer, uint32_t len);
extern uint32_t data_uart_send_string(const uint8_t *data, uint32_t len);
extern void data_uart_init(uint32_t tx_pin, uint32_t rx_pin, pf_send_msg_from_isr_t pf_send);
extern void data_uart_baudrate(data_uart_baudrate_t baudrate);
extern void data_uart_debug_switch(bool flg);
/** @} */
/** @} */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif

