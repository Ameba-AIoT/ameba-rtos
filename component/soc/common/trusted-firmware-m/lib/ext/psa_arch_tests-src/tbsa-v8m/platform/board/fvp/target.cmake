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

# Listing all the sources required for given target
FILE(GLOB PAL_CRYPTO_SRC_C ${TBSA_ROOT_DIR}/platform/peripherals/crypto/cc312/src/*.c)
FILE(GLOB PAL_DPM_SRC_C    ${TBSA_ROOT_DIR}/platform/peripherals/dpm/fvp_basic/src/*.c)
FILE(GLOB PAL_I2C_SRC_C    ${TBSA_ROOT_DIR}/platform/peripherals/i2c/c6510/src/*.c)
FILE(GLOB PAL_MPC_SRC_C    ${TBSA_ROOT_DIR}/platform/peripherals/mpc/sie200/src/*.c)
FILE(GLOB PAL_NVRAM_SRC_C  ${TBSA_ROOT_DIR}/platform/peripherals/nvram/nv_sim/src/*.c)
FILE(GLOB PAL_SPI_SRC_C    ${TBSA_ROOT_DIR}/platform/peripherals/spi/c6524/src/*.c)
FILE(GLOB PAL_TIMER_SRC_C  ${TBSA_ROOT_DIR}/platform/peripherals/timer/arm/src/*.c)
FILE(GLOB PAL_WDOG_SRC_C   ${TBSA_ROOT_DIR}/platform/peripherals/watchdog/arm/src/*.c)

list(APPEND PAL_SRC_C
	${PAL_CRYPTO_SRC_C}
	${PAL_DPM_SRC_C}
	${PAL_I2C_SRC_C}
	${PAL_MPC_SRC_C}
	${PAL_NVRAM_SRC_C}
	${PAL_SPI_SRC_C}
	${PAL_TIMER_SRC_C}
	${PAL_WDOG_SRC_C}
	)

if(${CMSIS_DRIVER})
FILE(GLOB PAL_UART_SRC_C ${TBSA_ROOT_DIR}/platform/peripherals/uart/cmsis/src/*.c)
else()
FILE(GLOB PAL_UART_SRC_C ${TBSA_ROOT_DIR}/platform/peripherals/uart/arm/src/*.c)
endif()

if(${CMSIS_DRIVER})
list(APPEND PAL_SRC_C
	${PAL_UART_SRC_C}
	"${TBSA_ROOT_DIR}/platform/board/${TARGET}/src/pal_cmsis_intf.c"
	)
else()
list(APPEND PAL_SRC_C
	${PAL_UART_SRC_C}
	"${TBSA_ROOT_DIR}/platform/board/${TARGET}/src/pal_baremetal_intf.c"
	)
endif()

# Create Platform static library
add_library(${TBSA_TARGET_PAL_LIB} STATIC ${PAL_SRC_C})

target_include_directories(${TBSA_TARGET_PAL_LIB} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${TBSA_ROOT_DIR}/platform/include
	${TBSA_ROOT_DIR}/platform/peripherals/crypto/cc312/include
	${TBSA_ROOT_DIR}/platform/peripherals/dpm/fvp_basic/include
	${TBSA_ROOT_DIR}/platform/peripherals/i2c/c6510/include
	${TBSA_ROOT_DIR}/platform/peripherals/mpc/sie200/include
	${TBSA_ROOT_DIR}/platform/peripherals/nvram/nv_sim/include
	${TBSA_ROOT_DIR}/platform/peripherals/spi/c6524/include
	${TBSA_ROOT_DIR}/platform/peripherals/timer/arm/include
	${TBSA_ROOT_DIR}/platform/peripherals/watchdog/arm/include
	${CMSIS_5_DIR}/CMSIS/Core/Include
	${CMSIS_5_DIR}/CMSIS/Driver/Include
	${CMSIS_5_DIR}/Device/ARM/ARMv8MML/Include
)
if(${CMSIS_DRIVER})
target_include_directories(${TBSA_TARGET_PAL_LIB} PRIVATE
	${TBSA_ROOT_DIR}/platform/peripherals/uart/cmsis/include
)
else()
target_include_directories(${TBSA_TARGET_PAL_LIB} PRIVATE
	${TBSA_ROOT_DIR}/platform/peripherals/uart/arm/include
)
endif()
