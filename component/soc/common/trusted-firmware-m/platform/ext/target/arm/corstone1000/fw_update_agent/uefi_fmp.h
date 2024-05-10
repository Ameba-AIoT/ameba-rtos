/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef UEFI_FMP_H
#define UEFI_FMP_H


#include <stdint.h>
#include "fwu_agent.h"
#include "../fip_parser/external/uuid.h"

/*
 * Last Attempt Status Value
 */

#define LAST_ATTEMPT_STATUS_SUCCESS                                     0x00000000
#define LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL                          0x00000001
#define LAST_ATTEMPT_STATUS_ERROR_INSUFFICIENT_RESOURCES                0x00000002
#define LAST_ATTEMPT_STATUS_ERROR_INCORRECT_VERSION                     0x00000003
#define LAST_ATTEMPT_STATUS_ERROR_INVALID_FORMAT                        0x00000004
#define LAST_ATTEMPT_STATUS_ERROR_AUTH_ERROR                            0x00000005
#define LAST_ATTEMPT_STATUS_ERROR_PWR_EVT_AC                            0x00000006
#define LAST_ATTEMPT_STATUS_ERROR_PWR_EVT_BATT                          0x00000007
#define LAST_ATTEMPT_STATUS_ERROR_UNSATISFIED_DEPENDENCIES              0x00000008
/* The LastAttemptStatus values of 0x1000 - 0x4000 are reserved for vendor usage. */
#define LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL_VENDOR_RANGE_MIN         0x00001000
#define LAST_ATTEMPT_STATUS_ERROR_UNSUCCESSFUL_VENDOR_RANGE_MAX         0x00004000



/*
 * Updates FMP information for the image matched by guid.
 *
 * guid : guid of the image
 * current_version: current versions for the image
 * attempt_version: attempted versions for the image
 *
 */
enum fwu_agent_error_t fmp_set_image_info(struct efi_guid *guid,
                     uint32_t current_version, uint32_t attempt_version,
                     uint32_t last_attempt_status);

/*
 * Return fmp image information for all the updable images.
 *
 * buffer : pointer to the out buffer
 * size : size of the buffer
 *
 */
enum fwu_agent_error_t fmp_get_image_info(void *buffer, uint32_t size);

#endif /* UEFI_FMP_H */
