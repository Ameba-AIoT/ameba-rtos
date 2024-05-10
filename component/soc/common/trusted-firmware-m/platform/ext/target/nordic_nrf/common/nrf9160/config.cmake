#-------------------------------------------------------------------------------
# Copyright (c) 2020, Nordic Semiconductor ASA.
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

include(${PLATFORM_PATH}/common/core/config.cmake)

set(SECURE_UART1                        ON         CACHE BOOL      "Enable secure UART1")
set(NRF_NS_STORAGE                      OFF        CACHE BOOL      "Enable non-secure storage partition")
set(BL2                                 ON         CACHE BOOL      "Whether to build BL2")
set(NRF_NS_SECONDARY                    ${BL2}     CACHE BOOL      "Enable non-secure secondary partition")

# Platform-specific configurations
set(PSA_API_TEST_TARGET                 "nrf9160")
