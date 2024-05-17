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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../tools/cmake)
include("common/CMakeSettings")
include("common/Utils")

# Causes toolchain to be re-evaluated
unset(ENV{CC})

# Let the CMake look for C compiler
project(TargetConfigGen LANGUAGES C)

# Check whether required arguments are passed to CMake
_check_arguments("OUT_DIR"
		"TARGET"
		"GENERATOR_FILE"
		"INCLUDE_DIR"
		"TARGET_CONFIGURATION_FILE"
		"TGT_CONFIG_SOURCE_C"
		"OUTPUT_HEADER"
		"DATABASE_TABLE_NAME"
		"DATABASE_TABLE_SECTION_NAME"
		"TARGET_HEADER_GEN_INCLUDE_PATHS"
)

# add_custom_command to generate intermediate source file
add_custom_command(
	OUTPUT
	${TGT_CONFIG_SOURCE_C}
	COMMENT "[PSA] : Creating generator source ${TGT_CONFIG_SOURCE_C}"
	COMMAND ${PYTHON_EXECUTABLE} ${GENERATOR_FILE} ${TARGET} ${INCLUDE_DIR}/val_target.h ${TARGET_CONFIGURATION_FILE} ${TGT_CONFIG_SOURCE_C} ${OUTPUT_HEADER} ${DATABASE_TABLE_NAME} ${DATABASE_TABLE_SECTION_NAME}
)

# Adding command to execute the generator
add_custom_command(
	OUTPUT
	${OUTPUT_HEADER}
	COMMENT "[PSA] : Creating output header ${OUTPUT_HEADER}"
	COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
	DEPENDS ${TGT_CONFIG_SOURCE_C}
)

# Adding executable
add_executable(${PROJECT_NAME} ${TGT_CONFIG_SOURCE_C})
foreach(include_path ${TARGET_HEADER_GEN_INCLUDE_PATHS})
	target_include_directories(${PROJECT_NAME} PRIVATE ${include_path})
endforeach()

# Adding target to tell we want OUTPUT_HEADER
add_custom_target(
	run_generator_output ALL
	SOURCES ${OUTPUT_HEADER}
	DEPENDS ${PROJECT_NAME}
)

# install target to put the OUTPUT_HEADER to it's final location
get_filename_component(INSTALL_DST "${OUT_DIR}" ABSOLUTE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_HEADER} DESTINATION ${INSTALL_DST})
