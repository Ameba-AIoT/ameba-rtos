#ifndef _EXAMPLE_WIFICAST_UART_H_
#define _EXAMPLE_WIFICAST_UART_H_

#include "../wificast_defs.h"
#include "os_wrapper.h"
#include "PinNames.h"
#include "serial_api.h"
#include "serial_ex_api.h"

#define UART_IDX  0
#define UART_BAUD_RATE 1500000
#define UART_TX_PIN    PA_28
#define UART_RX_PIN    PA_29

#define UART_RX_BUF_SZ	200

#endif /* _EXAMPLE_WIFICAST_UART_H_ */