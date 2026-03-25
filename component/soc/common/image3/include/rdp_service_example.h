/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RDP_SERVICE_EXAMPLE_H
#define RDP_SERVICE_EXAMPLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback function type for RDP service.
 * @param data Protected data passed from secure world.
 * @return Processing result.
 */
typedef uint32_t rdp_callback_t(uint32_t data);

/**
 * @brief Initialize RDP service and register callback.
 *
 * This function must be called before using rdp_callback_process().
 * It registers the non-secure callback function for later use.
 *
 * @param callback Your processing function
 */
NS_ENTRY void rdp_service_init(rdp_callback_t *callback);

/**
 * @brief Perform computation in secure world.
 *
 * This function retrieves protected data and computes result in secure world.
 * Non-secure code cannot access the raw data directly.
 *
 * @param idx Data index (0-3)
 * @return Computed result
 */
NS_ENTRY uint32_t rdp_secure_compute(uint32_t idx);

/**
 * @brief Pass protected data to registered callback for processing.
 *
 * Secure world retrieves the data and calls the callback registered via rdp_service_init().
 * Use this when you want to process protected data in non-secure world.
 *
 * @param idx Data index (0-3)
 * @return Result from your callback
 */
NS_ENTRY uint32_t rdp_callback_process(uint32_t idx);

#ifdef __cplusplus
}
#endif

#endif /* RDP_SERVICE_EXAMPLE_H */
