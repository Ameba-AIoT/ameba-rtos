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

set(CMAKE_SYSTEM_NAME Generic)
set(CMKE_SYSTEM_PROCESSOR ARM)

if (DEFINED CROSS_COMPILE)
	set(_C_TOOLCHAIN_NAME ${CROSS_COMPILE}-gcc)
else()
	set(_C_TOOLCHAIN_NAME arm-none-eabi-gcc)
endif()

if (WIN32)
	if (NOT DEFINED GNUARM_PATH)
		set(GNUARM_PATH "C:" CACHE PATH "Install directory for GNUARM Compiler")
	endif()
else (WIN32)
	if (NOT DEFINED GNUARM_PATH)
		set(GNUARM_PATH "/" CACHE PATH "Install directory for GNUARM Compiler")
	endif()
endif (WIN32)

find_program(
	_C_TOOLCHAIN_PATH
	${_C_TOOLCHAIN_NAME}
	PATHS env PATH
	HINTS ${GNUARM_PATH}
	HINTS bin
)

if (_C_TOOLCHAIN_PATH STREQUAL "_C_TOOLCHAIN_PATH-NOTFOUND")
        message(FATAL_ERROR "[TBSA] : Couldn't find ${_C_TOOLCHAIN_NAME}."
			    " Either put ${_C_TOOLCHAIN_NAME} on the PATH or set GNUARM_PATH or CROSS_COMPILE properly.")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

foreach(_LNG IN ITEMS "C" "ASM")
	set(CMAKE_${_LNG}_COMPILER ${_C_TOOLCHAIN_PATH})
	message(STATUS "[TBSA] : ${_LNG}  compiler used '${CMAKE_${_LNG}_COMPILER}'")
endforeach()

set(CMAKE_C_FLAGS              "-march=armv8-m.${ARCH} -mcmse -mthumb -Wall -Werror -O0 -nostartfiles -fdata-sections -ffunction-sections")
set(CMAKE_ASM_FLAGS            "-march=armv8-m.${ARCH} -mthumb")
set(CMAKE_EXE_LINKER_FLAGS     "-Xlinker --fatal-warnings -Xlinker --gc-sections -z max-page-size=0x400 -lgcc -lc -lnosys")

set(TBSA_LINKER_TEMPLATE       ${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_linker_scripts/tbsa.linker.template)
set(TBSA_TEST_LINKER_TEMPLATE  ${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_linker_scripts/test.linker.template)
set(TBSA_LINKER_SCRIPT         ${CMAKE_CURRENT_BINARY_DIR}/tbsa.linker)
set(TBSA_TEST_S_LINKER_SCRIPT  ${CMAKE_CURRENT_BINARY_DIR}/test_s.linker)
set(TBSA_TEST_NS_LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/test_ns.linker)

function(compiler_set_linker_options)
	set(options)
        set(oneValueArgs TARGET_NAME ENTRY_FUNCTION LINKER_SCRIPT MAP_FILE)   #Single option arguments.
	set(multiValueArgs)                                                   #List arguments
        cmake_parse_arguments(_MY_PARAMS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	# Check passed arguments
	if(NOT _MY_PARAMS_TARGET_NAME)
		message(FATAL_ERROR "[TBSA] : compiler_set_linker_options: mandatory parameter 'TARGET_NAME' is missing!")
	endif()
	if(NOT _MY_PARAMS_ENTRY_FUNCTION)
		message(FATAL_ERROR "[TBSA] : compiler_set_linker_options: mandatory parameter 'ENTRY_FUNCTION' is missing!")
	endif()
	if(NOT _MY_PARAMS_LINKER_SCRIPT)
		message(FATAL_ERROR "[TBSA] : compiler_set_linker_options: mandatory parameter 'LINKER_SCRIPT' is missing!")
	endif()
	if(NOT _MY_PARAMS_MAP_FILE)
		message(FATAL_ERROR "[TBSA] : compiler_set_linker_options: mandatory parameter 'MAP_FILE' is missing!")
	endif()

	set_property(TARGET ${_MY_PARAMS_TARGET_NAME} APPEND_STRING PROPERTY LINK_FLAGS "--entry=${_MY_PARAMS_ENTRY_FUNCTION} -T${_MY_PARAMS_LINKER_SCRIPT} -Xlinker -Map=${_MY_PARAMS_MAP_FILE}")
endfunction(compiler_set_linker_options)
