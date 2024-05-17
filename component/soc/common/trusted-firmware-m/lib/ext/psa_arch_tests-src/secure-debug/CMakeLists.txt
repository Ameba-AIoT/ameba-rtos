#/** @file
# * Copyright (c) 2021-2022, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.

# Set the minimum required version of CMake for the project
cmake_minimum_required(VERSION 3.10)
# cmake_policy
cmake_policy(SET CMP0057 NEW)
PROJECT (psa_adac_tests)

# Find python interpreter version 3 or greater
find_package(PythonInterp 3 REQUIRED)

get_filename_component(PSA_ROOT_DIR . ABSOLUTE)
include(${PSA_ROOT_DIR}/tools/cmake/common/Utils.cmake)

set(CMAKE_C_STANDARD 99)

# Check for LINK_LAYER
if(NOT DEFINED LINK_LAYER_COMM)
	set(LINK_LAYER_COMM unix_socket)
    message(STATUS "[PSA] : Link layer not specified. Defaulting to ${LINK_LAYER_COMM}")
endif()

set(DEPENDS_INC_PATH ${CMAKE_SOURCE_DIR}/platform/hosts/${TARGET}/${LINK_LAYER_COMM}/include)
if (NOT EXISTS ${DEPENDS_INC_PATH})
    Message(FATAL_ERROR "Link layer ${LINK_LAYER_COMM} not supported for target ${TARGET}.")
endif()

if(NOT DEFINED PSA_ADAC_ROOT)
    get_filename_component(PSA_ADAC_ROOT ${CMAKE_SOURCE_DIR}/psa-adac ABSOLUTE)
endif()
include(${PSA_ADAC_ROOT}/cmake/psa_adac.cmake OPTIONAL)

configure_file(${PSA_ADAC_ROOT}/psa-adac/core/include/psa_adac_config.h.in psa_adac_config.h)
include_directories (
    ${DEPENDS_INC_PATH}
    ${CMAKE_BINARY_DIR}
    ${PSA_ADAC_ROOT}/psa-adac/core/include
    ${PSA_ADAC_ROOT}/ports/include
    )

set(MBEDTLS_CONFIG_FILE "${PSA_ADAC_ROOT}/ports/crypto/manager-crypto-config.h")
add_compile_options(-DMBEDTLS_CONFIG_FILE="${MBEDTLS_CONFIG_FILE}")
if (UNIX)
    add_compile_options(-fPIC -fpic)
else ()
    # Silence warning about standard C APIs not being secure.
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif ()

# Generate ADAC LIB
add_subdirectory(${PSA_ADAC_ROOT}/psa-adac/core adac_core)
add_subdirectory(${PSA_ADAC_ROOT}/psa-adac/sdm adac_sdm)
add_subdirectory(${PSA_ADAC_ROOT}/ports/crypto/psa-crypto psa_adac_psa_crypto)

set(ADAC_LIBS psa_adac_sdm psa_adac_core psa_adac_psa_crypto mbedcrypto)

if(NOT DEFINED TFM_PROFILE)
	message(STATUS "[PSA] : Building Default profile")
list(APPEND PSA_SUITES  #PSA_SUITES
	"ADAC"
)
endif()

# list of VERBOSE options
list(APPEND PSA_VERBOSE_OPTIONS 1 2 3 4 5)

message(STATUS "[PSA] : ----------Process input arguments- start-------------")

# Check for TARGET command line argument
_check_arguments("TARGET")
# Check for SUITE command line argument
_check_arguments("SUITE")

string(TOLOWER ${SUITE} SUITE_LOWER)

# Check for valid targets
_get_sub_dir_list(PSA_TARGET_LIST ${PSA_ROOT_DIR}/platform/hosts)
if(NOT ${TARGET} IN_LIST PSA_TARGET_LIST)
	message(FATAL_ERROR "[PSA] : Error: Unsupported value for -DTARGET=${TARGET}, supported targets are : ${PSA_TARGET_LIST}")
else()
    message(STATUS "[PSA] : TARGET is set to ${TARGET}")
    message(STATUS "[PSA] : LINK_LAYER is set to ${LINK_LAYER_COMM}")
endif()

# Check for the presence of required test suite directories
if(NOT IS_DIRECTORY ${PSA_ROOT_DIR}/tests)
	message(STATUS "[PSA] : Error: Could not find architecture test suite directories in psa root path ${PSA_ROOT_DIR}")
endif()

# Check for valid suite cmake argument passed
if(NOT ${SUITE} IN_LIST PSA_SUITES)
	message(FATAL_ERROR "[PSA] : Error: Unsupported value for -DSUITE=${SUITE}, select one from supported suites which are : ${PSA_SUITES}")
else()
    message(STATUS "[PSA] : SUITE is set to ${SUITE}")
endif()

# Project variables
set(ADAC_HOST_VAL_LIB        psa_adac_val)
set(ADAC_HOST_PAL_LIB        psa_adac_pal)
set(TEST_COMBINE_LIB		 test_combine)
set(ADAC_HOST_EXE		     psa_adac_test)

set(PSA_SUITE_DIR		            ${PSA_ROOT_DIR}/tests/${SUITE_LOWER})
set(PSA_TESTLIST_GENERATOR		    ${PSA_ROOT_DIR}/tools/scripts/gen_tests_list.py)
set(TESTSUITE_DB			        ${PSA_SUITE_DIR}/testsuite.db)
set(PSA_TESTLIST_FILE			    ${CMAKE_CURRENT_BINARY_DIR}/${SUITE_LOWER}_testlist.txt)
set(PSA_TEST_ENTRY_LIST_INC		    ${CMAKE_CURRENT_BINARY_DIR}/test_entry_list.inc)
set(PSA_TEST_ENTRY_FUN_DECLARE_INC	${CMAKE_CURRENT_BINARY_DIR}/test_entry_fn_declare_list.inc)

# Check for VERBOSE
if(NOT DEFINED VERBOSE)
	set(VERBOSE 3 CACHE INTERNAL "Default VERBOSE value" FORCE)
        message(STATUS "[PSA] : Defaulting VERBOSE to ${VERBOSE}")
else()
	if(NOT ${VERBOSE} IN_LIST PSA_VERBOSE_OPTIONS)
		message(FATAL_ERROR "[PSA] : Error: Unsupported value for -DVERBOSE=${VERBOSE}, supported values are : ${PSA_VERBOSE_OPTIONS}")
	endif()
    message(STATUS "[PSA] : VERBOSE is set to ${VERBOSE}")
endif()

if(NOT DEFINED SUITE_TEST_RANGE)
	set(SUITE_TEST_RANGE_MIN None)
	set(SUITE_TEST_RANGE_MAX None)
else()
	list(LENGTH SUITE_TEST_RANGE SUITE_TEST_RANGE_LENGTH)
	if(${SUITE_TEST_RANGE_LENGTH} GREATER "2")
		message(FATAL_ERROR "[PSA] : -DSUITE_TEST_RANGE=<...> value error! accepts two "
                                             " numbers in quotes separated with ';'")
	endif()
	if(${SUITE_TEST_RANGE_LENGTH} EQUAL "2")
		list(GET SUITE_TEST_RANGE 0 SUITE_TEST_RANGE_MIN)
		list(GET SUITE_TEST_RANGE 1 SUITE_TEST_RANGE_MAX)
		message(STATUS "[PSA] : Testing (${SUITE_TEST_RANGE_MIN}, ${SUITE_TEST_RANGE_MAX}) of ${SUITE} suite")
	endif()
	if(${SUITE_TEST_RANGE_LENGTH} EQUAL "1")
		set(SUITE_TEST_RANGE_MIN ${SUITE_TEST_RANGE})
		set(SUITE_TEST_RANGE_MAX ${SUITE_TEST_RANGE})
		message(STATUS "[PSA] : Testing ${SUITE_TEST_RANGE_MIN} of ${SUITE} suite")
	endif()
endif()

message(STATUS "[PSA] : ----------Process input arguments- complete-------------")

# Create PSA clean list
list(APPEND PSA_CLEAN_LIST
	${PSA_TESTLIST_FILE}
	${PSA_TEST_ENTRY_LIST_INC}
	${PSA_TEST_ENTRY_FUN_DECLARE_INC}
)

# Process testsuite.db
message(STATUS "[PSA] : Creating testlist.txt 'available at ${PSA_TESTLIST_FILE}'")
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${PSA_TESTLIST_GENERATOR}
					${SUITE_LOWER}
					${TESTSUITE_DB}
					${PSA_TESTLIST_FILE}
					${PSA_TEST_ENTRY_LIST_INC}
					${PSA_TEST_ENTRY_FUN_DECLARE_INC}
					${SUITE_TEST_RANGE_MIN}
					${SUITE_TEST_RANGE_MAX})

# Creating CMake list variable from file
file(READ ${PSA_TESTLIST_FILE} PSA_TEST_LIST)
if(NOT PSA_TEST_LIST)
	message(FATAL_ERROR "[PSA] : Invalid test number!")
endif()
string(REGEX REPLACE "\n" ";" PSA_TEST_LIST "${PSA_TEST_LIST}")

# Global macro to identify the PSA test suite cmake build
add_definitions(-D${SUITE})
add_definitions(-DVERBOSE=${VERBOSE})
add_definitions(-D${TARGET})

# Build PAL LIB
include(${CMAKE_SOURCE_DIR}/platform/common/pal.cmake)
add_subdirectory(${CMAKE_SOURCE_DIR}/platform/hosts/${TARGET}/${LINK_LAYER_COMM} platform_host)
target_link_libraries(${ADAC_HOST_PAL_LIB} platform_host)

# Generate VAL LIB
include(${CMAKE_SOURCE_DIR}/val/val.cmake)

# Build test suite
include(${PSA_SUITE_DIR}/suite.cmake)

#Create single executable
add_executable(${ADAC_HOST_EXE} ${SUITE_CC_SOURCE})
target_include_directories(${ADAC_HOST_EXE} PRIVATE
    ${CMAKE_SOURCE_DIR}/val/include
	${CMAKE_SOURCE_DIR}/platform/common/include
    )
target_link_libraries (${ADAC_HOST_EXE} ${TEST_COMBINE_LIB}
                                        ${ADAC_HOST_VAL_LIB}
                                        ${ADAC_HOST_PAL_LIB}
                                        ${ADAC_LIBS}
                                        )

# Include the files for make clean
foreach(clean_item ${PSA_CLEAN_LIST})
        set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${clean_item})
endforeach()

