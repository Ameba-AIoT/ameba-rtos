#-------------------------------------------------------------------------------
# Copyright (c) 2022-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Load multiple config files and merge into one, generates CMake config file and config header file.
# The first loaded configs would be overridden by later ones. That's how the "merge" works.
# Check CONFIG_FILE_LIST for the loading order.
# Configurations not set by any of the config files would be set to the default values in Kconfig
# files with dependencies respected.
# If a ".config" already exists in the output folder, then the CONFIG_FILE_LIST is ignored.
# For more details, check the kconfig_system.rst.

set(KCONFIG_OUTPUT_DIR  ${CMAKE_BINARY_DIR}/kconfig)

set(DOTCONFIG_FILE      ${KCONFIG_OUTPUT_DIR}/.config)
set(ROOT_KCONFIG        ${CMAKE_SOURCE_DIR}/Kconfig)
set(PLATFORM_KCONFIG    ${TARGET_PLATFORM_PATH}/Kconfig)

# This function parses the input ${cmake_file} to get normal CMake variables and their values in the
# format of "set(_VAR_ _VALUE_)". The format could be split into multiple lines.
# Note that CMake does not allow the "(" to be in a different line as "set" and no white spaces are
# recommanded between "set" and "(". So the function only covers format of "set(".
function(convert_normal_cmake_config_to_kconfig cmake_file out_var)
    # Operate on a local var and write back to the "out_var" later.
    set(local_var "")

    # Read out the strings of the file. Binary data in the file are ignored
    file(STRINGS ${cmake_file} CONTENTS)

    # Exclude lines of comments (started with "#")
    set(CONTENTS_WITHOUT_COMMENTS "")

    foreach(LINE ${CONTENTS})
        string(REGEX MATCH "^#.*" OUT_STRING ${LINE})
        if(NOT OUT_STRING)
            string(APPEND CONTENTS_WITHOUT_COMMENTS "${LINE}\n")
        endif()

        string(REGEX MATCH "^include\\((.*)\\)$" OUT_STRING ${LINE})
        if(OUT_STRING AND CMAKE_MATCH_COUNT EQUAL 1)
            message(FATAL_ERROR "Including another file in config file is not supported yet: ${LINE}")
        endif()
    endforeach()

    # Search for strings match set(_VAR_ _VALUE_) with support of multi-line format
    string(REGEX MATCHALL
           "set\\([ \t\r\n]*([A-Za-z0-9_]*)[ \t\r\n]*([^ \t\r\n]*)[ \t\r\n]*\\)"
           OUT_STRINGS ${CONTENTS_WITHOUT_COMMENTS})

    foreach(MATCHED_ITEM ${OUT_STRINGS})
        # Try to convert CMake format to Kconfig one
        # If the format does not match, the content will not be changed and fall down to the next

        # Bool types
        string(REGEX REPLACE
               "set\\([ \t\r\n]*([A-Za-z0-9_]*)[ \t\r\n]*(TRUE|ON)[ \t\r\n]*\\)"
               "config \\1\n default y\n"
               MATCHED_ITEM ${MATCHED_ITEM})
        string(REGEX REPLACE
               "set\\([ \t\r\n]*([A-Za-z0-9_]*)[ \t\r\n]*(FALSE|OFF)[ \t\r\n]*\\)"
               "config \\1\n default n\n"
               MATCHED_ITEM ${MATCHED_ITEM})
        # Hex int
        string(REGEX REPLACE
               "set\\([ \t\r\n]*([A-Za-z0-9_]*)[ \t\r\n]*(0x[0-9a-fA-F]+[ \t\r\n]*)\\)"
               "config \\1\n default \\2\n"
               MATCHED_ITEM ${MATCHED_ITEM})
        # Decimal int
        string(REGEX REPLACE
               "set\\([ \t\r\n]*([A-Za-z0-9_]*)[ \t\r\n]*([0-9]+)[ \t\r\n]*\\)"
               "config \\1\n default \\2\n"
               MATCHED_ITEM ${MATCHED_ITEM})
        # Quoted string
        string(REGEX REPLACE
               "set\\([ \t\r\n]*([A-Za-z0-9_]*)[ \t\r\n]*(\".*\")[ \t\r\n]*\\)"
               "config \\1\n default \\2\n"
               MATCHED_ITEM ${MATCHED_ITEM})
        # If none of the above matches, must be a non-quoted string
        string(REGEX REPLACE
               "set\\([ \t\r\n]*([A-Za-z0-9_]*)[ \t\r\n]*(.*)[ \t\r\n]*\\)"
               "config \\1\n default \"\\2\"\n"
               MATCHED_ITEM ${MATCHED_ITEM})

        string(APPEND local_var ${MATCHED_ITEM})
    endforeach()

    set(${out_var} ${local_var} PARENT_SCOPE)
endfunction()

# This function goes through the CMake cache variables and convert them to .config format.
# The function distinguishes command-line variables and other ones and it can only handle one of
# them in the same time.
function(convert_cache_config_to_dotconfig convert_cl_var out_var)
    # Operate on a local var and write back to the out_var later
    set(local_var "")

    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    foreach(CACHE_VAR ${CACHE_VARS})
        get_property(HELP_STRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)

        if("${HELP_STRING}" MATCHES "variable specified on the command line")
            # Command-line variables have the help string above by default
            set(IS_CL_VAR TRUE)
        else()
            set(IS_CL_VAR FALSE)
        endif()

        if((IS_CL_VAR AND NOT ${convert_cl_var}) OR (NOT IS_CL_VAR AND ${convert_cl_var}))
            continue()
        endif()

        set(CACHE_VAR_VAL ${${CACHE_VAR}})
        STRING(TOUPPER "${CACHE_VAR_VAL}" CACHE_VAR_VAL_UPPER)

        set(CACHE_VAR "CONFIG_${CACHE_VAR}")

        set(KCONFIG_OPTION_ITEM "")

        # False CMAKE values
        if(CACHE_VAR_VAL_UPPER STREQUAL "OFF" OR CACHE_VAR_VAL_UPPER STREQUAL "FALSE")
            set(KCONFIG_OPTION_ITEM "${CACHE_VAR}=n\r\n")
        # True CMAKE Values
        elseif(CACHE_VAR_VAL_UPPER STREQUAL "ON" OR CACHE_VAR_VAL_UPPER STREQUAL "TRUE")
            set(KCONFIG_OPTION_ITEM "${CACHE_VAR}=y\r\n")
        # Non-quoted values (hex and decimal numbers)
        elseif(CACHE_VAR_VAL MATCHES "^0x[a-fA-F0-9]+$" OR CACHE_VAR_VAL MATCHES "^[0-9]+$" )
            set(KCONFIG_OPTION_ITEM "${CACHE_VAR}=${CACHE_VAR_VAL}\r\n")
        # Everything else is a quoted string
        else()
            if(${CACHE_VAR} STREQUAL "CONFIG_TEST_PSA_API" AND CACHE_VAR_VAL_UPPER)
                # Turn on the corresponding "choice" option for psa-arch-test
                list(APPEND _LEGAL_PSA_API_TEST_LIST "IPC" "CRYPTO" "INITIAL_ATTESTATION" "INTERNAL_TRUSTED_STORAGE" "PROTECTED_STORAGE" "STORAGE")
                list(FIND _LEGAL_PSA_API_TEST_LIST ${CACHE_VAR_VAL_UPPER} _RET_VAL)
                if(NOT ${_RET_VAL} EQUAL -1)
                    set(KCONFIG_OPTION_ITEM "CONFIG_PSA_API_TEST_${CACHE_VAR_VAL_UPPER}=y\r\n")

                    if(${CACHE_VAR_VAL_UPPER} STREQUAL "IPC")
                        # PSA API IPC test requires IPC model to be enabled while
                        # the CONFIG_TFM_SPM_BACKEND_IPC cannot be selected or implied because it is a choice.
                        # It can be only enabled in a Kconfig config file. So append it here.
                        string(APPEND KCONFIG_OPTION_ITEM "CONFIG_CONFIG_TFM_SPM_BACKEND_IPC=y\r\n")
                    endif()
                endif()
            elseif(${CACHE_VAR} STREQUAL "CONFIG_CONFIG_TFM_SPM_BACKEND")
                # Turn on the corresponding "choice" option for SPM backend
                set(KCONFIG_OPTION_ITEM "CONFIG_CONFIG_TFM_SPM_BACKEND_${CACHE_VAR_VAL_UPPER}=y\r\n")
            else()
                set(KCONFIG_OPTION_ITEM "${CACHE_VAR}=\"${CACHE_VAR_VAL}\"\r\n")
            endif()
        endif()

        string(APPEND local_var ${KCONFIG_OPTION_ITEM})
    endforeach()

    set(${out_var} ${local_var} PARENT_SCOPE)
endfunction()

# Initialize the .cl_config
set(COMMAND_LINE_CONFIG_TO_FILE ${KCONFIG_OUTPUT_DIR}/.cl_config)
file(REMOVE ${COMMAND_LINE_CONFIG_TO_FILE})

# Initialize the .cache_var_config
set(CACHE_VAR_CONFIG_FILE ${KCONFIG_OUTPUT_DIR}/.cache_var_config)
file(REMOVE ${CACHE_VAR_CONFIG_FILE})

if(NOT EXISTS ${PLATFORM_KCONFIG} AND NOT EXISTS ${DOTCONFIG_FILE})
    # Parse platform's preload.cmake and config.cmake to get config options.
    set(PLATFORM_KCONFIG_OPTIONS "")
    set(PLATFORM_KCONFIG ${KCONFIG_OUTPUT_DIR}/platform/Kconfig)

    convert_normal_cmake_config_to_kconfig(${TARGET_PLATFORM_PATH}/preload.cmake PLATFORM_KCONFIG_OPTIONS)
    file(WRITE ${PLATFORM_KCONFIG} ${PLATFORM_KCONFIG_OPTIONS})

    if(EXISTS ${TARGET_PLATFORM_PATH}/config.cmake)
        include(${TARGET_PLATFORM_PATH}/config.cmake)
        convert_normal_cmake_config_to_kconfig(${TARGET_PLATFORM_PATH}/config.cmake PLATFORM_KCONFIG_OPTIONS)
        file(APPEND ${PLATFORM_KCONFIG} ${PLATFORM_KCONFIG_OPTIONS})

        set(PLATFORM_CMAKE_CONFIGS "")
        set(CONVERT_CL_VAR FALSE)
        convert_cache_config_to_dotconfig(CONVERT_CL_VAR PLATFORM_CMAKE_CONFIGS)
        file(APPEND ${CACHE_VAR_CONFIG_FILE} ${PLATFORM_CMAKE_CONFIGS})
    endif()
endif()
get_filename_component(PLATFORM_KCONFIG_PATH ${PLATFORM_KCONFIG} DIRECTORY)

# Build type Kconfig file, for example 'Kconfig.minsizerel', the suffix passed
# by Kconfig environment variables and it shall be lowercase.
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWERCASE)

# TF-M profile config file
if(TFM_PROFILE)
    set(TFM_PROFILE_KCONFIG_FILE ${CMAKE_SOURCE_DIR}/config/profile/${TFM_PROFILE}.conf)
    if(NOT EXISTS ${TFM_PROFILE_KCONFIG_FILE})
        message(FATAL_ERROR "No such file: ${TFM_PROFILE_KCONFIG_FILE}, please check ${TFM_PROFILE} is right.")
    endif()

    set(TFM_PROFILE_TEST_KCONFIG_FILE ${CMAKE_SOURCE_DIR}/lib/ext/tf-m-tests/${TFM_PROFILE}_test.conf)
    if(NOT EXISTS ${TFM_PROFILE_TEST_KCONFIG_FILE})
        message(FATAL_ERROR "No such file: ${TFM_PROFILE_TEST_KCONFIG_FILE}, please check ${TFM_PROFILE} is right.")
    endif()
endif()

# Parse command-line variables
set(CL_CONFIGS "")
set(CONVERT_CL_VAR TRUE)
convert_cache_config_to_dotconfig(CONVERT_CL_VAR CL_CONFIGS)
file(APPEND ${COMMAND_LINE_CONFIG_TO_FILE} ${CL_CONFIGS})

if(NOT EXISTS ${CACHE_VAR_CONFIG_FILE})
    set(CACHE_VAR_CONFIG_FILE "")
endif()

# User customized config file
if(DEFINED KCONFIG_CONFIG_FILE AND NOT EXISTS ${KCONFIG_CONFIG_FILE})
    message(FATAL_ERROR "No such file: ${KCONFIG_CONFIG_FILE}")
endif()

# Note the order of CONFIG_FILE_LIST, as the first loaded configs would be
# overridden by later ones.
list(APPEND CONFIG_FILE_LIST
            ${TFM_PROFILE_KCONFIG_FILE}
            ${TFM_PROFILE_TEST_KCONFIG_FILE}
            ${CACHE_VAR_CONFIG_FILE}
            ${KCONFIG_CONFIG_FILE}
            ${COMMAND_LINE_CONFIG_TO_FILE})

# Set up ENV variables for the tfm_kconfig.py which are then consumed by Kconfig files.
set(KCONFIG_ENV_VARS "TFM_SOURCE_DIR=${CMAKE_SOURCE_DIR} \
                      TFM_VERSION=${TFM_VERSION} \
                      PLATFORM_PATH=${PLATFORM_KCONFIG_PATH} \
                      CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE_LOWERCASE}")

if(MENUCONFIG)
    # Note: Currently, only GUI menuconfig can be supported with CMake integration
    set(MENUCONFIG_ARG "-u=gui")
else()
    set(MENUCONFIG_ARG "")
endif()

if(DEFINED PROJECT_CONFIG_HEADER_FILE)
    get_property(HELP_STRING CACHE PROJECT_CONFIG_HEADER_FILE PROPERTY HELPSTRING)

    # It is not supported to set PROJECT_CONFIG_HEADER_FILE while using Kconfig, either from
    # command line or CMake files. It should be set to the file generated the Kconfig system.
    # As this file set it itself, if the user re-run the CMake config command, the
    # PROJECT_CONFIG_HEADER_FILE will be already defined set.
    # So the existence of the ${DOTCONFIG_FILE} is used to indicate if it is a re-configuration.
    if("${HELP_STRING}" MATCHES "variable specified on the command line" OR NOT EXISTS ${DOTCONFIG_FILE})
        message(FATAL_ERROR "It is NOT supported to manually set PROJECT_CONFIG_HEADER_FILE while using Kconfig.")
    endif()
endif()

find_package(Python3)

execute_process(
    COMMAND
    ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/kconfig/tfm_kconfig.py
    -k ${ROOT_KCONFIG} -o ${KCONFIG_OUTPUT_DIR}
    --envs ${KCONFIG_ENV_VARS}
    --config-files ${CONFIG_FILE_LIST}
    ${MENUCONFIG_ARG}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE ret
)

if(NOT ret EQUAL 0)
    message(FATAL_ERROR "Kconfig tool failed!")
endif()

# Component configs generated by tfm_kconfig.py
set(PROJECT_CONFIG_HEADER_FILE ${KCONFIG_OUTPUT_DIR}/project_config.h CACHE FILEPATH "User defined header file for TF-M config")

# Load project cmake configs generated by tfm_kconfig.py
include(${KCONFIG_OUTPUT_DIR}/project_config.cmake)
