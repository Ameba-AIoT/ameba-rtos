/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PERIPHERALS_DEF_H__
#define __TFM_PERIPHERALS_DEF_H__

#include "platform_irq.h"
#include "common_target_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Quantized default IRQ priority, the value is:
 * (Number of configurable priority) / 4: (1UL << __NVIC_PRIO_BITS) / 4
 */
#define DEFAULT_IRQ_PRIORITY    (1UL << (__NVIC_PRIO_BITS - 2))

#define TFM_TIMER0_IRQ           (TIMER0_IRQn)

extern struct platform_data_t tfm_peripheral_std_uart;
extern struct platform_data_t tfm_peripheral_timer0;

#define TFM_PERIPHERAL_STD_UART  (&tfm_peripheral_std_uart)
#define TFM_PERIPHERAL_TIMER0    (&tfm_peripheral_timer0)

#define TFM_DMA0_CH0_IRQ         (DMA_CHANNEL_0_IRQn)
#define TFM_DMA0_CH1_IRQ         (DMA_CHANNEL_1_IRQn)

extern struct platform_data_t tfm_peripheral_dma0_ch0;
extern struct platform_data_t tfm_peripheral_dma0_ch1;

#define TFM_PERIPHERAL_DMA0_CH0  (&tfm_peripheral_dma0_ch0)
#define TFM_PERIPHERAL_DMA0_CH1  (&tfm_peripheral_dma0_ch1)

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PERIPHERALS_DEF_H__ */
