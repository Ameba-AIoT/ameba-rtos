#-------------------------------------------------------------------------------
# Copyright (c) 2020-2021, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

set(INSTALL_MEMORY_LAYOUT_DIR    ${TFM_INSTALL_PATH}/memory_layout)

install(FILES       ${CMAKE_CURRENT_LIST_DIR}/mailbox/platform_multicore.c
                    ${CMAKE_CURRENT_LIST_DIR}/mailbox/platform_ns_mailbox.c
        DESTINATION ${INSTALL_INTERFACE_SRC_DIR})

install(FILES       ${CMAKE_CURRENT_LIST_DIR}/mailbox/platform_multicore.h
                    ${CMAKE_CURRENT_LIST_DIR}/mailbox/ns_ipc_config.h
                    ${CMAKE_CURRENT_LIST_DIR}/Device/Config/device_cfg.h
        DESTINATION ${INSTALL_INTERFACE_INC_DIR})

install(FILES       ${CMAKE_CURRENT_LIST_DIR}/partition/flash_layout.h
                    ${CMAKE_CURRENT_LIST_DIR}/partition/region_defs.h
        DESTINATION ${INSTALL_MEMORY_LAYOUT_DIR})

install(FILES       ${CMAKE_CURRENT_LIST_DIR}/security/policy/policy_multi_CM0_CM4_tfm_dev_certs.json
        DESTINATION ${INSTALL_IMAGE_SIGNING_DIR})
