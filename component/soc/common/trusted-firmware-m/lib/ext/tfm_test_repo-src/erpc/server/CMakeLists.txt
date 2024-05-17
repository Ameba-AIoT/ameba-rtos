#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0097 NEW)

# Set eRPC config file. Need to provide config file with an absolute path.
if (ERPC_CONFIG_FILE)
    if (NOT EXISTS ${ERPC_CONFIG_FILE})
        message(FATAL_ERROR "ERPC_CONFIG_FILE does not exist. Please provide it with an absolute path.")
    endif()
    # Get the path of the customized eRPC config file
    get_filename_component(ERPC_CONFIG_FILE_PATH ${ERPC_CONFIG_FILE} DIRECTORY)
else()
    # Use default one
    set(ERPC_CONFIG_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")
endif()

add_library(erpc_server STATIC)

target_sources(erpc_server
    PRIVATE
        erpc_server_start.c
        erpc_server_wrapper.c
        # eRPC files
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_basic_codec.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_crc16.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_framed_transport.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_message_buffer.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_server.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_simple_server.cpp
        ${ERPC_REPO_PATH}/erpc_c/port/erpc_port_stdlib.cpp
        ${ERPC_REPO_PATH}/erpc_c/setup/erpc_setup_mbf_dynamic.cpp
        ${ERPC_REPO_PATH}/erpc_c/setup/erpc_server_setup.cpp
        # Generated files
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated_files/tfm_erpc_psa_client_api_server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated_files/tfm_erpc_psa_connection_api_server.cpp
)

target_include_directories(erpc_server
    PUBLIC
        ${ERPC_REPO_PATH}/erpc_c/port
        ${ERPC_REPO_PATH}/erpc_c/infra
        ${ERPC_REPO_PATH}/erpc_c/transports
        ${ERPC_REPO_PATH}/erpc_c/setup
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated_files
        ${ERPC_CONFIG_FILE_PATH}/
)

target_link_libraries(erpc_server
    PUBLIC
        tfm_api_ns
        platform_ns # UART driver header and target config
)
