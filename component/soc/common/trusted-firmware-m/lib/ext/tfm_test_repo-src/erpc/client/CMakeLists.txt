#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

if (NOT DEFINED TFM_INSTALL_PATH)
    if (DEFINED ENV{TFM_INSTALL_PATH})
        set(TFM_INSTALL_PATH $ENV{TFM_INSTALL_PATH})
    else()
        message(FATAL_ERROR "TFM_INSTALL_PATH not found. Please set TFM_INSTALL_PATH environment variable or pass -DTFM_INSTALL_PATH flag.")
    endif()
endif()

if (NOT DEFINED ERPC_REPO_PATH)
    if (DEFINED ENV{ERPC_REPO_PATH})
        set(ERPC_REPO_PATH $ENV{ERPC_REPO_PATH})
    else()
        message(FATAL_ERROR "ERPC_REPO_PATH not found. Please set ERPC_REPO_PATH environment variable or pass -DERPC_REPO_PATH flag.")
    endif()
endif()

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

add_library(erpc_client STATIC)

target_sources(erpc_client
    PRIVATE
        erpc_client_wrapper.c
        erpc_client_start.c
        # eRPC files
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_basic_codec.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_client_manager.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_crc16.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_framed_transport.cpp
        ${ERPC_REPO_PATH}/erpc_c/infra/erpc_message_buffer.cpp
        ${ERPC_REPO_PATH}/erpc_c/port/erpc_serial.cpp
        ${ERPC_REPO_PATH}/erpc_c/setup/erpc_client_setup.cpp
        ${ERPC_REPO_PATH}/erpc_c/setup/erpc_setup_mbf_dynamic.cpp
        # Generated files
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated_files/tfm_erpc_psa_client_api_client.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated_files/tfm_erpc_psa_connection_api_client.cpp
)

target_include_directories(erpc_client
    PUBLIC
        ${ERPC_REPO_PATH}/erpc_c/port
        ${ERPC_REPO_PATH}/erpc_c/infra
        ${ERPC_REPO_PATH}/erpc_c/transports
        ${ERPC_REPO_PATH}/erpc_c/setup
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/../generated_files
        ${TFM_INSTALL_PATH}/interface/include
        ${ERPC_CONFIG_FILE_PATH}/
)
