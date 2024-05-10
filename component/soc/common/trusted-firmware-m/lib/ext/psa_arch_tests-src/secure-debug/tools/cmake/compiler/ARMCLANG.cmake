#/** @file
# * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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

set(_C_TOOLCHAIN_NAME armclang)

if(WIN32)
        if (NOT DEFINED ARMCLANG_PATH)
                set(ARMCLANG_PATH "C:" CACHE PATH "Install directory for ARMCLANG Compiler")
        endif()
else(WIN32)
        if (NOT DEFINED ARMCLANG_PATH)
                set(ARMCLANG_PATH "/" CACHE PATH "Install directory for ARMCLANG Compiler")
        endif()
endif(WIN32)

find_program(
        _C_TOOLCHAIN_PATH
        ${_C_TOOLCHAIN_NAME}
        PATHS env PATH
        HINTS ${ARMCLANG_PATH}
        HINTS bin
)

if(_C_TOOLCHAIN_PATH STREQUAL "_C_TOOLCHAIN_PATH-NOTFOUND")
        message(FATAL_ERROR "[PSA] : Couldn't find ${_C_TOOLCHAIN_NAME}."
                            " Either put ${_C_TOOLCHAIN_NAME} on the PATH or set ARMCLANG_PATH set properly.")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

foreach(_LNG IN ITEMS "C" "ASM")
        set(CMAKE_${_LNG}_COMPILER ${_C_TOOLCHAIN_PATH})
        message(STATUS "[PSA] : ${_LNG}  compiler used '${CMAKE_${_LNG}_COMPILER}'")
endforeach()

if(${CPU_ARCH} STREQUAL armv7m)
	set(TARGET_SWITCH "-march=armv7-m")
elseif(${CPU_ARCH} STREQUAL armv8m_ml)
	set(TARGET_SWITCH "-march=armv8-m.main -mcmse")
elseif(${CPU_ARCH} STREQUAL armv8m_bl)
	set(TARGET_SWITCH "-march=armv8-m.base -mcmse")
endif()

set(CMAKE_C_FLAGS              "--target=arm-arm-none-eabi ${TARGET_SWITCH} -g -Wall -Werror -Wextra -fshort-enums -fshort-wchar -funsigned-char -fdata-sections -ffunction-sections -mno-unaligned-access -mfpu=none")
set(CMAKE_ASM_FLAGS            "${TARGET_SWITCH} -mthumb")
set(CMAKE_EXE_LINKER_FLAGS     "--strict --map --symbols --xref  --info=summarysizes,sizes,totals,unused,veneers --diag_warning=L6204")
