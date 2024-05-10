/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "dma350_checker_layer.h"
#include "platform_s_device_definition.h"

struct dma350_ch_dev_t* const DMA350_DMA0_CHANNELS[] = {
    &DMA350_DMA0_CH0_DEV_S,
    &DMA350_DMA0_CH1_DEV_S
};

struct dma350_checker_channels_t const DMA350_CHECKER_CHANNELS = {
    .channels = DMA350_DMA0_CHANNELS,
    .number_of_channels = sizeof(DMA350_DMA0_CHANNELS) /
                          sizeof(DMA350_DMA0_CHANNELS[0])
};
