/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __DMA_PRIV_CONFIG_H__
#define __DMA_PRIV_CONFIG_H__

#include "dma350_lib.h"
#include "dma350_checker_layer.h"

#include <stdint.h>


/**
 * \brief Pass the parameters to a function, which will forward it to \ref
 *        config_dma350_for_unprivileged_actor in the checker layer.
 *
 * \param[in] config_type   The DMA350 command's type
 * \param[in] channel       The DMA channel, the operation should use
 * \param[in] config        The config for the DMA350 command. Its type has to
 *                          be in sync with the command.
 *
 * \return Result of the operation \ref dma350_lib_error_t
 *
 * \note This might contains platform-dependant codes.
 */
extern enum dma350_lib_error_t request_dma350_priv_config(
    enum dma350_config_type_t config_type, uint8_t channel, void *config);

#endif /* __DMA_PRIV_CONFIG_H__ */
