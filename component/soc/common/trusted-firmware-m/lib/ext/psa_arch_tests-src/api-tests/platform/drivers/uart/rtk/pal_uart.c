/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "pal_uart.h"

volatile uint32_t g_uart;

/**
    @brief    - This function initializes the UART
**/
void pal_uart_rtk_init(uint32_t uart_base_addr)
{
    g_uart = uart_base_addr;

    /* Disable RX and TX interrupts, disable overrun interrupts, enable TX,
    * disable RX */
#ifdef AMEBA_NONEED
    ((uart_t *) g_uart)->CTRL = CMSDK_UART_CTRL_TXEN_Msk;
#endif
    /* However, the UART resets to a BAUDDIV of 0, which is invalid. Set it to
    * the minimum valid number. */
    /* Avoiding baud rate reconfiguration */
    /* ((uart_t *) g_uart)->BAUDDIV = 16; */
}

/**
    @brief    - This function checks for empty TX FIFO and writes to FIFO register
**/
static void pal_uart_rtk_putc(uint8_t c)
{
#ifdef AMEBA_NONEED
    /* ensure TX buffer to be empty */
    while (!pal_uart_rtk_is_tx_empty());

    /* write the data (upper 24 bits are reserved) */
    ((uart_t *) g_uart)->DATA = c;
    if (c == '\n')
    {
        pal_uart_rtk_putc('\r');
    }
#else
	LOGUART_PutChar(c);
#endif
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str      : Input String
              - data     : Value for format specifier
**/
void pal_rtk_print(const char *str, int32_t data)
{
    int8_t  j, buffer[16];
    int8_t  i = 0, is_neg = 0, k = 2 * sizeof(data);

    for (; *str != '\0'; ++str)
    {
        if (*str == '%')
        {
            ++str;
            if (*str == 'd')
            {
                if (data < 0)
                {
                    data = -(data);
                    is_neg = 1;
                }

                while (data != 0)
                {
                    j         = data % 10;
                    data      = data /10;
                    buffer[i] = j + 48;
                    i        += 1;
                }

                if (is_neg)
                    buffer[i++] = '-';
            }
            else if (*str == 'x' || *str == 'X')
            {
                while (k--)
                {
                    j         = data & 0xf;
                    data    >>= 4;
                    buffer[i] = j + ((j > 9) ? 55 : 48);
                    i        += 1;
                }
            }
            if (i > 0)
            {
                while (i > 0) {
                    pal_uart_rtk_putc(buffer[--i]);
                }
            }
            else
            {
                pal_uart_rtk_putc(48);
            }
        }
        else
        {
            pal_uart_rtk_putc(*str);
        }
    }
}
