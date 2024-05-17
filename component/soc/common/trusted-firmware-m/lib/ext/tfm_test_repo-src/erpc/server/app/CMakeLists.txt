#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

set(CONFIG_ENABLE_NS_UART_TX_RX_CONTROL      ON         CACHE BOOL      "Whether to enable UART TX RX")
set(ERPC_DIR     ${CMAKE_CURRENT_LIST_DIR}/../..)

add_subdirectory(${ERPC_DIR}/server ${CMAKE_CURRENT_BINARY_DIR}/server)

add_library(erpc_app STATIC EXCLUDE_FROM_ALL)

target_sources(erpc_app
    PRIVATE
        erpc_app.c
        ${ERPC_REPO_PATH}/erpc_c/setup/erpc_setup_uart_cmsis.cpp
        ${ERPC_REPO_PATH}/erpc_c/transports/erpc_uart_cmsis_transport.cpp
)

target_include_directories(erpc_app
    PRIVATE
        ${ERPC_REPO_PATH}/erpc_c/transports
        ${ERPC_DIR}/platform/${TFM_PLATFORM}
)

target_link_libraries(erpc_app
    PRIVATE
        erpc_server
        tfm_log
)
