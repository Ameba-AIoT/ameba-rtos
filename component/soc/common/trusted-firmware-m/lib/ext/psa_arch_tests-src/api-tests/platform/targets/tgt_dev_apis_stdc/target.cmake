#/** @file
# * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
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

###############################################################################
# FUNCTION: _create_psa_stdc_exe()
#  Function for generating PSA stdc test binaries linking with libraries
#  external to the psa-arch-tests project. This function requires
#  PSA_STORAGE_LIB_FILENAME to be specificed on the cmake command line,
#  where the symbol is defined as the full path to the external PSA storage
#  library to test. e.g.
#    cmake ... -DPSA_STORAGE_LIB_FILENAME=/wdir/usr/lib/libpsastorage.so
#  If the function is being used to generate a test binary for testing
#  the mbed-crypto library then the function requires
#  PSA_CRYPTO_LIB_FILENAME to be specificed on the cmake command line,
#  where the symbol is defined as the full path to the external PSA crypto
#  library to test. e.g.
#    cmake ... -DPSA_CRYPTO_LIB_FILENAME=/wdir/mbed-crypto/library/    \
#                                                               libmbedcrypto.a
# ARGUMENTS:
#   _exe_name     Name of the test binary to generate.
#   _api_dir      PSA API directory name e.g. crypto,
#                 internal_trusted_storage or protected_storage.
###############################################################################
function(_create_psa_stdc_exe _exe_name _api_dir)

	# Create the PSA test binary.
	set(EXE_NAME ${_exe_name})

	# Create list of test binary source files.
	list(APPEND EXE_SRC ${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/main.c)

	# Create list of libraries to link to test binary
	list(APPEND EXE_LIBS
		${PROJECT_BINARY_DIR}/val/val_nspe.a
		${PROJECT_BINARY_DIR}/platform/pal_nspe.a
		${PROJECT_BINARY_DIR}/dev_apis/${_api_dir}/test_combine.a
	)

	add_executable(${EXE_NAME} ${EXE_SRC})
	target_link_libraries(${EXE_NAME} ${EXE_LIBS} ${PSA_CRYPTO_LIB_FILENAME} ${PSA_STORAGE_LIB_FILENAME})
	add_dependencies(${EXE_NAME} ${PSA_TARGET_TEST_COMBINE_LIB})
endfunction(_create_psa_stdc_exe)

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
	message(FATAL_ERROR "IPC not supported")
else()
	list(APPEND PAL_SRC_C_NSPE
		# driver files will be compiled as part of NSPE
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/pal_driver_intf.c
	)
endif()
if(${SUITE} STREQUAL "CRYPTO")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/crypto/pal_crypto_intf.c
	)
	if(NOT DEFINED PSA_CRYPTO_LIB_FILENAME)
		message(FATAL_ERROR "ERROR: PSA_CRYPTO_LIB_FILENAME undefined.")
	endif()
	_create_psa_stdc_exe(psa-arch-tests-crypto crypto)
endif()
if(${SUITE} STREQUAL "PROTECTED_STORAGE")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/protected_storage/pal_protected_storage_intf.c
	)
	if(NOT DEFINED PSA_STORAGE_LIB_FILENAME)
		message(FATAL_ERROR "ERROR: PSA_STORAGE_LIB_FILENAME undefined.")
	endif()
	_create_psa_stdc_exe(psa-arch-tests-storage storage)
endif()
if(${SUITE} STREQUAL "INTERNAL_TRUSTED_STORAGE")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/internal_trusted_storage/pal_internal_trusted_storage_intf.c
	)
	if(NOT DEFINED PSA_STORAGE_LIB_FILENAME)
		message(FATAL_ERROR "ERROR: PSA_STORAGE_LIB_FILENAME undefined.")
	endif()
	_create_psa_stdc_exe(psa-arch-tests-storage storage)
endif()
if(${SUITE} STREQUAL "STORAGE")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/internal_trusted_storage/pal_internal_trusted_storage_intf.c
		${PSA_ROOT_DIR}/platform/targets/common/nspe/protected_storage/pal_protected_storage_intf.c
	)
	if(NOT DEFINED PSA_STORAGE_LIB_FILENAME)
		message(FATAL_ERROR "ERROR: PSA_STORAGE_LIB_FILENAME undefined.")
	endif()
	_create_psa_stdc_exe(psa-arch-tests-storage storage)
endif()
if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/initial_attestation/pal_attestation_intf.c
		${PSA_ROOT_DIR}/platform/targets/common/nspe/initial_attestation/pal_attestation_crypto.c
                ${PSA_TARGET_QCBOR}/src/UsefulBuf.c
                ${PSA_TARGET_QCBOR}/src/ieee754.c
                ${PSA_TARGET_QCBOR}/src/qcbor_decode.c
                ${PSA_TARGET_QCBOR}/src/qcbor_encode.c
	)
	if(NOT DEFINED PSA_INITIAL_ATTESTATION_LIB_FILENAME)
		message(FATAL_ERROR "ERROR: PSA_STORAGE_LIB_FILENAME undefined.")
	endif()
_create_psa_stdc_exe(psa-arch-tests-initial-attestation initial-attestation)

endif()

# Create NSPE library
add_library(${PSA_TARGET_PAL_NSPE_LIB} STATIC ${PAL_SRC_C_NSPE} ${PAL_SRC_ASM_NSPE})

# PSA Include directories
foreach(psa_inc_path ${PSA_INCLUDE_PATHS})
	target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE ${psa_inc_path})
endforeach()

target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE
	${PSA_ROOT_DIR}/platform/targets/common/nspe
	${PSA_ROOT_DIR}/platform/targets/common/nspe/crypto
	${PSA_ROOT_DIR}/platform/targets/common/nspe/protected_storage
	${PSA_ROOT_DIR}/platform/targets/common/nspe/internal_trusted_storage
	${PSA_ROOT_DIR}/platform/targets/common/nspe/initial_attestation
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
)

if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
	target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE
		${PSA_QCBOR_INCLUDE_PATH}
	)
#	if(NOT DEFINED PSA_INITIAL_ATTESTATION_LIB_FILENAME)
#		message(FATAL_ERROR "ERROR: PSA_STORAGE_LIB_FILENAME undefined.")
#	endif()
#_create_psa_stdc_exe(psa-arch-tests-initial-attestation initial-attestation)
endif()
