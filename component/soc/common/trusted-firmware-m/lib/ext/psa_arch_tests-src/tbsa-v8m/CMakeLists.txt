#/** @file
# * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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
#**/

# Set the minimum required version of CMake for the project
cmake_minimum_required(VERSION 3.10)

# cmake_policy
cmake_policy(SET CMP0057 NEW)

# Find python interpreter version 3 or greater
find_package(PythonInterp 3 REQUIRED)
# Find Git package
find_package(Git REQUIRED)

get_filename_component(TBSA_ROOT_DIR . ABSOLUTE)

list(APPEND CMAKE_MODULE_PATH ${TBSA_ROOT_DIR}/cmake)
include("common/Utils")
include(${TBSA_ROOT_DIR}/cmake/common/CMakeSettings.cmake)

# Check for TARGET command line argument
_check_arguments("TARGET")

# Check for COMPILER command line argument
if(NOT DEFINED COMPILER)
	set(COMPILER "GNUARM" CACHE INTERNAL "Compiler used" FORCE)
	message(STATUS "[TBSA] : Defaulting compiler to ${COMPILER}")
endif()

# Check for ARCH command line argument
if(NOT DEFINED ARCH)
	message(STATUS "[TBSA] : Defaulting architecture to armv8-m.main")
	set(ARCH "main")
endif()

# Check for CMSIS command line argument which drives platform driver selection
if(NOT DEFINED CMSIS_DRIVER)
	set(CMSIS_DRIVER OFF CACHE INTERNAL "CMSIS Driver selection" FORCE)
endif()

# Check for SUITE command line argument
if(NOT DEFINED SUITE)
	set(SUITE "ALL" CACHE INTERNAL "Test Component selection" FORCE)
endif()

# Supported architectures
list(APPEND ARCHITECTURE_SUPPORT
	main
	base
)

# Supported toolchains
list(APPEND TOOLCHAIN_SUPPORT
	GNUARM
)

# list of supported CROSS_COMPILE toolchains
list(APPEND CROSS_COMPILE_TOOLCHAIN_SUPPORT
	GNUARM
)

# Variables of the project
set(TBSA_PROJECT_NAME                   tbsa)
set(TARGET_CONFIGURATION_FILE           ${TBSA_ROOT_DIR}/platform/board/${TARGET}/tbsa_tgt.cfg)
set(TBSA_MAIN_APP_ENTRY                 tbsa_entry)
set(TBSA_TEST_APP_ENTRY                 tbsa_test_info)
set(TBSA_TARGET_GENERATE_DATABASE       generate_database)
set(TBSA_TARGET_GENERATE_DATABASE_POST  generate_database_cleanup)
set(TBSA_TARGET_CMSIS                   cmsis_setup)
set(TBSA_TARGET_GENERATE_LINKER_SCRIPTS generate_linker_scripts)
set(TBSA_TARGET_TEST_ELF_COMBINE        test_elf_combine)
set(TBSA_TARGET_VAL_LIB                 tbsa_val)
set(TBSA_TARGET_PAL_LIB                 tbsa_pal)
set(TBSA_CMSIS_GIT_REPO_LINK            https://github.com/ARM-software/CMSIS_5.git CACHE INTERNAL "CMSIS Git Repo path" FORCE)
set(TBSA_CMSIS_GIT_REPO_TAG             49ac527aa7406cecbba46d4d3bdbc7f60c6c6d42 CACHE INTERNAL "CMSIS Git Repo Tag Id" FORCE)
set(TBSA_TARGET_CONFIG_HEADER_GENERATOR ${TBSA_ROOT_DIR}/tools/tgt_cfg_parser/targetConfigGen.py)
set(TBSA_LINKER_SCRIPT_GENERATOR        ${TBSA_ROOT_DIR}/tools/gen_linker_scripts/gen_linker_scripts.py)
set(TBSA_TEST_ELF_COMBINE_GENERATOR     ${TBSA_ROOT_DIR}/tools/test_elf_combine.py)
set(TEST_COMBINED_BINARY_FILE_NAME      ${CMAKE_CURRENT_BINARY_DIR}/tbsa_test_combined.bin CACHE INTERNAL "TBSA Test Binary Name" FORCE)
set(TGT_CONFIG_SOURCE_C                 ${CMAKE_CURRENT_BINARY_DIR}/targetConfigGen.c)
set(OUTPUT_HEADER                       pal_database.h)
set(DATABASE_TABLE_NAME                 database)
set(DATABASE_TABLE_SECTION_NAME         .ns_target_database)
set(TBSA_MAP_FILE                       ${CMAKE_CURRENT_BINARY_DIR}/${TBSA_PROJECT_NAME}.map)
set(TBSA_EXECUTABLE_SUFFIX              ".elf" CACHE INTERNAL "Executable suffix" FORCE)

if(NOT DEFINED CMSIS_REPO_PATH)
	get_filename_component(CMSIS_5_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/${TBSA_TARGET_CMSIS} ABSOLUTE)
else()
	set(CMSIS_5_DIR ${CMSIS_REPO_PATH})
endif()

function(validate_target_directory)
	# Get the list of available directories under platform/board
	_get_sub_dir_list(BOARD_LIST ${TBSA_ROOT_DIR}/platform/board)
	# Removing include directory which is not a target
	list(REMOVE_ITEM BOARD_LIST include)
	if(${TARGET} IN_LIST BOARD_LIST)
		message(STATUS "[TBSA] : Working on target '${TARGET}'")
	else()
		message(FATAL_ERROR "[TBSA] : Specified target directory '${TARGET}' not found!")
	endif()
endfunction()

function(validate_suite_directory)
	# Get the list of available directories under test_pool
	_get_sub_dir_list(SUITE_LIST ${TBSA_ROOT_DIR}/test_pool)
	if(${SUITE} IN_LIST SUITE_LIST)
		message(STATUS "[TBSA] : Testing '${SUITE}' suite")
	else()
		message(FATAL_ERROR "[TBSA] : Specified suite directory '${SUITE}' not found!")
	endif()
endfunction()

# test pool source directory
set(TEST_POOL_SOURCE_DIR ${TBSA_ROOT_DIR}/test_pool)
if(SUITE STREQUAL "ALL")
	# Get all the test pool components
	_get_sub_dir_list(TEST_POOL_COMP_LIST ${TEST_POOL_SOURCE_DIR})
else()
	validate_suite_directory()
	set(TEST_POOL_COMP_LIST ${SUITE})
endif()

# Project starts
project(${TBSA_PROJECT_NAME} C ASM)

# Name of final executable
set(EXE_NAME ${TBSA_PROJECT_NAME})

message(STATUS "[TBSA] : CMake is running on '${CMAKE_HOST_SYSTEM_NAME}'")
validate_target_directory()

# Check for supported architectues
if(NOT ${ARCH} IN_LIST ARCHITECTURE_SUPPORT)
	message(FATAL_ERROR "[TBSA] : armv8-m.${ARCH} architecture support not available")
endif()

# Check for supported toolchain/s
if(${COMPILER} IN_LIST TOOLCHAIN_SUPPORT)
	if(CROSS_COMPILE AND NOT (${TOOLCHAIN} IN_LIST CROSS_COMPILE_TOOLCHAIN_SUPPORT))
		message(FATAL_ERROR "[PSA] : Error: CROSS_COMPILE not supported for this toolchain, supported toolchain are : ${CROSS_COMPILE_TOOLCHAIN_SUPPORT}")
	endif()
	include(${TBSA_ROOT_DIR}/cmake/compiler/${COMPILER}.cmake)
else()
	message(FATAL_ERROR "[TBSA] : ${COMPILER} toolchain support not available")
endif()

# Files to clean
list(APPEND TBSA_CLEAN_LIST
	${TEST_COMBINED_BINARY_FILE_NAME}
	${TBSA_MAP_FILE}
	${TGT_CONFIG_SOURCE_C}
	${TBSA_LINKER_SCRIPT}
	${TBSA_TEST_S_LINKER_SCRIPT}
	${TBSA_TEST_NS_LINKER_SCRIPT}
	${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_HEADER}
)

# Adding files to clean for incremental build
_get_sub_dir_list(TEST_POOL_CLEAN_LIST ${TEST_POOL_SOURCE_DIR})
foreach(test_comp ${TEST_POOL_CLEAN_LIST})
	# Set source directory
	set(TEST_COMP_SOURCE_DIR ${TEST_POOL_SOURCE_DIR}/${test_comp})
	# Get all the test folders from a given test component
	_get_sub_dir_list(COMP_TEST_LIST ${TEST_COMP_SOURCE_DIR})
	foreach(test ${COMP_TEST_LIST})
		set(TEST_S_EXE_NAME "${test}.secure${TBSA_EXECUTABLE_SUFFIX}")
		set(TEST_NS_EXE_NAME "${test}.non_secure${TBSA_EXECUTABLE_SUFFIX}")
		list(APPEND TBSA_CLEAN_LIST
				${CMAKE_CURRENT_BINARY_DIR}/${TEST_S_EXE_NAME}
				${CMAKE_CURRENT_BINARY_DIR}/${test}.secure.map)
		list(APPEND TBSA_CLEAN_LIST
				${CMAKE_CURRENT_BINARY_DIR}/${TEST_NS_EXE_NAME}
				${CMAKE_CURRENT_BINARY_DIR}/${test}.non_secure.map)
	endforeach()
endforeach()

# Target configuration parsing
include(ExternalProject)
ExternalProject_Add(
	${TBSA_TARGET_GENERATE_DATABASE}
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}
	DOWNLOAD_COMMAND ""
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	BUILD_COMMAND ""
	SOURCE_DIR "${TBSA_ROOT_DIR}/tools/tgt_cfg_parser"
	CMAKE_ARGS -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
		-DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
		-DTARGET=${TARGET}
		-DGENERATOR_FILE=${TBSA_TARGET_CONFIG_HEADER_GENERATOR}
		-DINCLUDE_DIR=${TBSA_ROOT_DIR}/val/include
		-DTARGET_CONFIGURATION_FILE=${TARGET_CONFIGURATION_FILE}
		-DTGT_CONFIG_SOURCE_C=${TGT_CONFIG_SOURCE_C}
		-DOUTPUT_HEADER=${OUTPUT_HEADER}
		-DDATABASE_TABLE_NAME=${DATABASE_TABLE_NAME}
		-DDATABASE_TABLE_SECTION_NAME=${DATABASE_TABLE_SECTION_NAME}
	TEST_COMMAND ""
)

# Add custom target to clean generated files of the external project
add_custom_target(
	${TBSA_TARGET_GENERATE_DATABASE_POST}
	COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/src/${TBSA_TARGET_GENERATE_DATABASE}-build/ -- clean
)

if(NOT DEFINED CMSIS_REPO_PATH)
# Cloning CMSIS_5 repo
ExternalProject_Add(
	${TBSA_TARGET_CMSIS}
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}
	GIT_REPOSITORY ${TBSA_CMSIS_GIT_REPO_LINK}
	GIT_TAG ${TBSA_CMSIS_GIT_REPO_TAG}
	CONFIGURE_COMMAND ""
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	BUILD_COMMAND ""
	TEST_COMMAND ""
	INSTALL_COMMAND ""
)
endif()

# Linker scripts generation
add_custom_target(
	${TBSA_TARGET_GENERATE_LINKER_SCRIPTS}
	COMMENT "[TBSA] : Generating linker scripts for the target"
	COMMAND ${PYTHON_EXECUTABLE} ${TBSA_LINKER_SCRIPT_GENERATOR}
					${TARGET_CONFIGURATION_FILE}
					${TBSA_LINKER_TEMPLATE}
					${TBSA_TEST_LINKER_TEMPLATE}
					${TBSA_LINKER_SCRIPT}
					${TBSA_TEST_S_LINKER_SCRIPT}
					${TBSA_TEST_NS_LINKER_SCRIPT}
)

#Combine test ELFs into binary file
add_custom_target(
	${TBSA_TARGET_TEST_ELF_COMBINE}
	COMMENT "[TBSA] : Combining TEST ELFs"
	COMMAND ${PYTHON_EXECUTABLE} ${TBSA_TEST_ELF_COMBINE_GENERATOR}
					${CMAKE_CURRENT_BINARY_DIR}
					${TEST_COMBINED_BINARY_FILE_NAME}
)


# Generate VAL LIB
include(${TBSA_ROOT_DIR}/val/val.cmake)
# Generate PAL LIB
include(${TBSA_ROOT_DIR}/platform/board/${TARGET}/target.cmake)

# Final executable of the build process
add_executable(${EXE_NAME} ${TBSA_ROOT_DIR}/tbsa_app/tbsa_main.c)

target_link_libraries(${EXE_NAME} ${TBSA_TARGET_VAL_LIB} ${TBSA_TARGET_PAL_LIB})
set_target_properties(${EXE_NAME} PROPERTIES SUFFIX ${TBSA_EXECUTABLE_SUFFIX})
compiler_set_linker_options(TARGET_NAME ${EXE_NAME}
				ENTRY_FUNCTION ${TBSA_MAIN_APP_ENTRY}
				LINKER_SCRIPT ${TBSA_LINKER_SCRIPT}
				MAP_FILE ${TBSA_MAP_FILE})
target_include_directories(${EXE_NAME} PRIVATE ${TBSA_ROOT_DIR}/tbsa_app)

# Dependency chain
add_dependencies(${TBSA_TARGET_GENERATE_DATABASE_POST}  ${TBSA_TARGET_GENERATE_DATABASE})
if(NOT DEFINED CMSIS_REPO_PATH)
	add_dependencies(${TBSA_TARGET_CMSIS}                   ${TBSA_TARGET_GENERATE_DATABASE_POST})
	add_dependencies(${TBSA_TARGET_GENERATE_LINKER_SCRIPTS} ${TBSA_TARGET_CMSIS})
else()
	add_dependencies(${TBSA_TARGET_GENERATE_LINKER_SCRIPTS} ${TBSA_TARGET_GENERATE_DATABASE_POST})
endif()
add_dependencies(${TBSA_TARGET_PAL_LIB}                 ${TBSA_TARGET_GENERATE_LINKER_SCRIPTS})
add_dependencies(${TBSA_TARGET_VAL_LIB}                 ${TBSA_TARGET_PAL_LIB})
add_dependencies(${TBSA_TARGET_TEST_ELF_COMBINE}        ${TBSA_TARGET_VAL_LIB})
add_dependencies(${EXE_NAME}                            ${TBSA_TARGET_TEST_ELF_COMBINE})

# Generate test ELFs for all the relevant components
foreach(test_comp ${TEST_POOL_COMP_LIST})
	# Set source directory
	set(TEST_COMP_SOURCE_DIR ${TEST_POOL_SOURCE_DIR}/${test_comp})
	# Get all the test folders from a given test component
	_get_sub_dir_list(COMP_TEST_LIST ${TEST_COMP_SOURCE_DIR})
	foreach(test ${COMP_TEST_LIST})
		set(TEST_S_EXE_NAME "${test}.secure")
		set(TEST_NS_EXE_NAME "${test}.non_secure")
		set(TEST_S_SRC_C "${TEST_POOL_SOURCE_DIR}/${test_comp}/${test}/secure.c")
		set(TEST_NS_SRC_C "${TEST_POOL_SOURCE_DIR}/${test_comp}/${test}/non_secure.c")
		# Secure Test ELF
		add_executable(${TEST_S_EXE_NAME} ${TEST_S_SRC_C})
		target_link_libraries(${TEST_S_EXE_NAME} ${TBSA_TARGET_VAL_LIB} ${TBSA_TARGET_PAL_LIB})
		set_target_properties(${TEST_S_EXE_NAME} PROPERTIES SUFFIX ${TBSA_EXECUTABLE_SUFFIX})
		compiler_set_linker_options(TARGET_NAME ${TEST_S_EXE_NAME}
						ENTRY_FUNCTION ${TBSA_TEST_APP_ENTRY}
						LINKER_SCRIPT ${TBSA_TEST_S_LINKER_SCRIPT}
						MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${test}.secure.map)
		add_dependencies(${EXE_NAME} ${TEST_S_EXE_NAME})
		add_dependencies(${TBSA_TARGET_TEST_ELF_COMBINE} ${TEST_S_EXE_NAME})
		# Non secure Test ELF
		add_executable(${TEST_NS_EXE_NAME} ${TEST_NS_SRC_C})
		target_link_libraries(${TEST_NS_EXE_NAME} ${TBSA_TARGET_VAL_LIB} ${TBSA_TARGET_PAL_LIB})
		set_target_properties(${TEST_NS_EXE_NAME} PROPERTIES SUFFIX ${TBSA_EXECUTABLE_SUFFIX})
		compiler_set_linker_options(TARGET_NAME ${TEST_NS_EXE_NAME}
						ENTRY_FUNCTION ${TBSA_TEST_APP_ENTRY}
						LINKER_SCRIPT ${TBSA_TEST_NS_LINKER_SCRIPT}
						MAP_FILE ${CMAKE_CURRENT_BINARY_DIR}/${test}.non_secure.map)
		add_dependencies(${EXE_NAME} ${TEST_NS_EXE_NAME})
		add_dependencies(${TBSA_TARGET_TEST_ELF_COMBINE} ${TEST_NS_EXE_NAME})
	endforeach()
endforeach()

# Include the files for make clean
foreach(clean_item ${TBSA_CLEAN_LIST})
	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${clean_item})
endforeach()
