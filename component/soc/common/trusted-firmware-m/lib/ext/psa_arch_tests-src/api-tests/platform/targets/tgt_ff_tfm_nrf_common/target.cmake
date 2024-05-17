#/** @file
# * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Copyright (c) 2021 Nordic Semiconductor ASA.
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

# PAL C source files part of NSPE library
list(APPEND PAL_SRC_C_NSPE )

# PAL ASM source files part of NSPE library
list(APPEND PAL_SRC_ASM_NSPE )

# PAL C source files part of SPE library - driver partition
list(APPEND PAL_SRC_C_DRIVER_SP )

# PAL ASM source files part of SPE library - driver partition
list(APPEND PAL_SRC_ASM_DRIVER_SP )


# Listing all the sources required for given target
if(${SUITE} STREQUAL "IPC")
	list(APPEND PAL_SRC_C_NSPE
		# driver functionalities are implemented as RoT-services
		# and secure and non-secure clients will call to these RoT-services to get appropriate driver services.
		${PSA_ROOT_DIR}/platform/targets/tgt_ff_tfm_nrf_common/nspe/pal_driver_ipc_intf.c
		${PSA_ROOT_DIR}/platform/drivers/nvmem/pal_nvmem.c
	)
	list(APPEND PAL_SRC_C_DRIVER_SP
		# Driver files will be compiled as part of driver partition
		${PSA_ROOT_DIR}/platform/targets/tgt_ff_tfm_nrf_common/spe/pal_driver_intf.c
		${PSA_ROOT_DIR}/platform/drivers/nvmem/pal_nvmem.c
	)

	if(${PSA_API_TEST_TARGET} STREQUAL nrf9160)
		list(APPEND PAL_SRC_C_DRIVER_SP
			${PSA_ROOT_DIR}/platform/drivers/watchdog/nrf/nrf9160_wdt.c
		)
	else()
		list(APPEND PAL_SRC_C_DRIVER_SP
			${PSA_ROOT_DIR}/platform/drivers/watchdog/nrf/nrf_wdt.c
		)
	endif()

endif()

if((${SUITE} STREQUAL "CRYPTO") OR
   (${SUITE} STREQUAL "STORAGE") OR
   (${SUITE} STREQUAL "PROTECTED_STORAGE") OR
   (${SUITE} STREQUAL "INTERNAL_TRUSTED_STORAGE") OR
   (${SUITE} STREQUAL "INITIAL_ATTESTATION"))
	message(FATAL_ERROR "For PSA API - use -DTARGET=tgt_dev_apis_tfm_nrf instead")
endif()

# Create NSPE library
add_library(${PSA_TARGET_PAL_NSPE_LIB} STATIC ${PAL_SRC_C_NSPE} ${PAL_SRC_ASM_NSPE})

# PSA Include directories
foreach(psa_inc_path ${PSA_INCLUDE_PATHS})
	target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE ${psa_inc_path})
endforeach()

list(APPEND PAL_DRIVER_INCLUDE_PATHS
	${PSA_ROOT_DIR}/platform/drivers/nvmem
	${PSA_ROOT_DIR}/platform/drivers/uart/cmsdk
	${PSA_ROOT_DIR}/platform/drivers/watchdog/nrf/
)

message(WARNING ${PSA_ROOT_DIR})

target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE
	${PAL_DRIVER_INCLUDE_PATHS}
	${PSA_ROOT_DIR}/platform/targets/common/nspe
	${PSA_ROOT_DIR}/platform/targets/common/nspe/crypto
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
)
