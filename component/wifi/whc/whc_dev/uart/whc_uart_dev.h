#ifndef _WHC_UART_DEV_H_
#define _WHC_UART_DEV_H_

#define whc_dev_init            whc_uart_dev_init
#define whc_dev_api_send_data       whc_uart_dev_send_data
#define _whc_dev_api_bus_is_idle    whc_uart_dev_bus_is_idle

void whc_uart_dev_init(void);

#endif

