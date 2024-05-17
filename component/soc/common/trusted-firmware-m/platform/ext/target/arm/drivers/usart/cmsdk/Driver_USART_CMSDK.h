/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DRIVER_USART_CMSDK_H__
#define __DRIVER_USART_CMSDK_H__

#include "Driver_USART_Common.h"
#include "uart_cmsdk_drv.h"

extern uint32_t PeripheralClock;

/* Driver Capabilities */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USARTx_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USARTx_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USARTx_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USARTx_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USARTx_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USARTx_EVENT_RI */
    0  /* Reserved */
};

typedef struct {
    struct uart_cmsdk_dev_t *dev;        /* UART device structure */
    uint32_t tx_nbr_bytes;             /* Number of bytes transfered */
    uint32_t rx_nbr_bytes;             /* Number of bytes recevied */
    ARM_USART_SignalEvent_t cb_event;  /* Callback function for events */
} UARTx_Resources;

static inline ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void)
{
    return DriverCapabilities;
}

static inline int32_t ARM_USARTx_Initialize(UARTx_Resources *uart_dev)
{
    /* Initializes generic UART driver */
    uart_cmsdk_init(uart_dev->dev, PeripheralClock);

    return ARM_DRIVER_OK;
}

static inline int32_t ARM_USARTx_PowerControl(UARTx_Resources *uart_dev,
                                              ARM_POWER_STATE state)
{
    ARG_UNUSED(uart_dev);

    switch (state) {
    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all the enumeration values are
     *           covered in the switch.*/
    }
}

static inline int32_t ARM_USARTx_Send(UARTx_Resources *uart_dev,
                                      const void *data,
                                      uint32_t num)
{
    const uint8_t *p_data = (const uint8_t *)data;

    if ((data == NULL) || (num == 0U)) {
        /* Invalid parameters */
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Resets previous TX counter */
    uart_dev->tx_nbr_bytes = 0;

    while (uart_dev->tx_nbr_bytes != num) {
        /* Waits until UART is ready to transmit */
        while (!uart_cmsdk_tx_ready(uart_dev->dev)) {
        }

        /* As UART is ready to transmit at this point, the write function can
         * not return any transmit error */
        (void)uart_cmsdk_write(uart_dev->dev, *p_data);

        uart_dev->tx_nbr_bytes++;
        p_data++;
    }

    if (uart_dev->cb_event != NULL) {
        uart_dev->cb_event(ARM_USART_EVENT_SEND_COMPLETE);
    }

    /* Waits until character is transmited */
    while (!uart_cmsdk_tx_ready(uart_dev->dev)) {
    }

    return ARM_DRIVER_OK;
}

static inline int32_t ARM_USARTx_Receive(UARTx_Resources *uart_dev,
                                         void *data, uint32_t num)
{
    uint8_t *p_data = (uint8_t *)data;

    if ((data == NULL) || (num == 0U)) {
        // Invalid parameters
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Resets previous RX counter */
    uart_dev->rx_nbr_bytes = 0;

    while (uart_dev->rx_nbr_bytes != num) {
        /* Waits until one character is received */
        while (!uart_cmsdk_rx_ready(uart_dev->dev)){
        }

        /* As UART has received one byte, the read can not
         * return any receive error at this point */
        (void)uart_cmsdk_read(uart_dev->dev, p_data);

        uart_dev->rx_nbr_bytes++;
        p_data++;
    }

    if (uart_dev->cb_event != NULL) {
        uart_dev->cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
    }

    return ARM_DRIVER_OK;
}

static inline uint32_t ARM_USARTx_GetTxCount(UARTx_Resources *uart_dev)
{
    return uart_dev->tx_nbr_bytes;
}

static inline uint32_t ARM_USARTx_GetRxCount(UARTx_Resources *uart_dev)
{
    return uart_dev->rx_nbr_bytes;
}

static inline int32_t ARM_USARTx_Control(UARTx_Resources *uart_dev,
                                         uint32_t control,
                                         uint32_t arg)
{
    switch (control & ARM_USART_CONTROL_Msk) {
#ifdef UART_TX_RX_CONTROL_ENABLED
        case ARM_USART_CONTROL_TX:
            if (arg == 0) {
                uart_cmsdk_tx_disable(uart_dev->dev);
            } else if (arg == 1) {
                if (uart_cmsdk_tx_enable(uart_dev->dev)
                                                    != UART_CMSDK_ERR_NONE) {
                    return ARM_DRIVER_ERROR;
                }
            } else {
                return ARM_DRIVER_ERROR_PARAMETER;
            }
            break;
        case ARM_USART_CONTROL_RX:
            if (arg == 0) {
                uart_cmsdk_rx_disable(uart_dev->dev);
            } else if (arg == 1) {
                if (uart_cmsdk_rx_enable(uart_dev->dev)
                                                    != UART_CMSDK_ERR_NONE) {
                    return ARM_DRIVER_ERROR;
                }
            } else {
                return ARM_DRIVER_ERROR_PARAMETER;
            }
            break;
#endif
        case ARM_USART_MODE_ASYNCHRONOUS:
            if (uart_cmsdk_set_baudrate(uart_dev->dev, arg) != UART_CMSDK_ERR_NONE) {
                return ARM_USART_ERROR_BAUDRATE;
            }
            break;
        /* Unsupported command */
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* UART Data bits */
    if (control & ARM_USART_DATA_BITS_Msk) {
        /* Data bit is not configurable */
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    /* UART Parity */
    if (control & ARM_USART_PARITY_Msk) {
        /* Parity is not configurable */
        return ARM_USART_ERROR_PARITY;
    }

    /* USART Stop bits */
    if (control & ARM_USART_STOP_BITS_Msk) {
        /* Stop bit is not configurable */
        return ARM_USART_ERROR_STOP_BITS;
    }

    return ARM_DRIVER_OK;
}

/*
 * \brief Macro for USART CMSDK Driver
 *
 * \param[in]  USART_DEV          Native driver device
 *                                \ref uart_cmsdk_dev_t
 * \param[out] USART_DRIVER_NAME  Resulting Driver name
 */
#define ARM_DRIVER_USART_CMSDK(USART_DEV, USART_DRIVER_NAME)                  \
static UARTx_Resources USART_DRIVER_NAME##_DEV = {                            \
    .dev = &USART_DEV,                                                        \
    .tx_nbr_bytes = 0,                                                        \
    .rx_nbr_bytes = 0,                                                        \
    .cb_event = NULL,                                                         \
};                                                                            \
                                                                              \
static int32_t USART_DRIVER_NAME##_Initialize(                                \
                                        ARM_USART_SignalEvent_t cb_event)     \
{                                                                             \
    USART_DRIVER_NAME##_DEV.cb_event = cb_event;                              \
                                                                              \
    return ARM_USARTx_Initialize(&USART_DRIVER_NAME##_DEV);                   \
}                                                                             \
                                                                              \
static int32_t USART_DRIVER_NAME##_Uninitialize(void)                         \
{                                                                             \
    /* Nothing to be done */                                                  \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t USART_DRIVER_NAME##_PowerControl(ARM_POWER_STATE state)        \
{                                                                             \
    return ARM_USARTx_PowerControl(&USART_DRIVER_NAME##_DEV, state);          \
}                                                                             \
                                                                              \
static int32_t USART_DRIVER_NAME##_Send(const void *data, uint32_t num)       \
{                                                                             \
    return ARM_USARTx_Send(&USART_DRIVER_NAME##_DEV, data, num);              \
}                                                                             \
                                                                              \
static int32_t USART_DRIVER_NAME##_Receive(void *data, uint32_t num)          \
{                                                                             \
    return ARM_USARTx_Receive(&USART_DRIVER_NAME##_DEV, data, num);           \
}                                                                             \
                                                                              \
static int32_t USART_DRIVER_NAME##_Transfer(const void *data_out,             \
                                            void *data_in,                    \
                                            uint32_t num)                     \
{                                                                             \
    ARG_UNUSED(data_out);                                                     \
    ARG_UNUSED(data_in);                                                      \
    ARG_UNUSED(num);                                                          \
                                                                              \
    return ARM_DRIVER_ERROR_UNSUPPORTED;                                      \
}                                                                             \
                                                                              \
static uint32_t USART_DRIVER_NAME##_GetTxCount(void)                          \
{                                                                             \
    return ARM_USARTx_GetTxCount(&USART_DRIVER_NAME##_DEV);                   \
}                                                                             \
                                                                              \
static uint32_t USART_DRIVER_NAME##_GetRxCount(void)                          \
{                                                                             \
    return ARM_USARTx_GetRxCount(&USART_DRIVER_NAME##_DEV);                   \
}                                                                             \
static int32_t USART_DRIVER_NAME##_Control(uint32_t control, uint32_t arg)    \
{                                                                             \
    return ARM_USARTx_Control(&USART_DRIVER_NAME##_DEV, control, arg);        \
}                                                                             \
                                                                              \
static ARM_USART_STATUS USART_DRIVER_NAME##_GetStatus(void)                   \
{                                                                             \
    ARM_USART_STATUS status = {0, 0, 0, 0, 0, 0, 0, 0};                       \
    return status;                                                            \
}                                                                             \
                                                                              \
static int32_t USART_DRIVER_NAME##_SetModemControl(                           \
                                             ARM_USART_MODEM_CONTROL control) \
{                                                                             \
    ARG_UNUSED(control);                                                      \
    return ARM_DRIVER_ERROR_UNSUPPORTED;                                      \
}                                                                             \
                                                                              \
static ARM_USART_MODEM_STATUS USART_DRIVER_NAME##_GetModemStatus(void)        \
{                                                                             \
    ARM_USART_MODEM_STATUS modem_status = {0, 0, 0, 0, 0};                    \
    return modem_status;                                                      \
}                                                                             \
                                                                              \
extern ARM_DRIVER_USART USART_DRIVER_NAME;                                    \
ARM_DRIVER_USART USART_DRIVER_NAME   = {                                      \
    ARM_USART_GetVersion,                                                     \
    ARM_USART_GetCapabilities,                                                \
    USART_DRIVER_NAME##_Initialize,                                           \
    USART_DRIVER_NAME##_Uninitialize,                                         \
    USART_DRIVER_NAME##_PowerControl,                                         \
    USART_DRIVER_NAME##_Send,                                                 \
    USART_DRIVER_NAME##_Receive,                                              \
    USART_DRIVER_NAME##_Transfer,                                             \
    USART_DRIVER_NAME##_GetTxCount,                                           \
    USART_DRIVER_NAME##_GetRxCount,                                           \
    USART_DRIVER_NAME##_Control,                                              \
    USART_DRIVER_NAME##_GetStatus,                                            \
    USART_DRIVER_NAME##_SetModemControl,                                      \
    USART_DRIVER_NAME##_GetModemStatus                                        \
}

#endif  /* __DRIVER_USART_CMSDK_H__ */
