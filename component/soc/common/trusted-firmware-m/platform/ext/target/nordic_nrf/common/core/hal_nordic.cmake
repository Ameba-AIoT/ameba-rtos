
#-------------------------------------------------------------------------------
# Copyright (c) 2021, Nordic Semiconductor ASA.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

fetch_remote_library(
    LIB_NAME                hal_nordic
    LIB_SOURCE_PATH_VAR     HAL_NORDIC_PATH
    FETCH_CONTENT_ARGS
        GIT_TAG             ${HAL_NORDIC_VERSION}
        GIT_REPOSITORY      ${HAL_NORDIC_REMOTE}
)
