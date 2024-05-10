#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(USE_GIT_PULL)
	fetch_remote_library(
		LIB_NAME                psa_arch_tests
		LIB_SOURCE_PATH_VAR     PSA_ARCH_TESTS_PATH
		LIB_PATCH_DIR           ${CMAKE_CURRENT_LIST_DIR}
		LIB_FORCE_PATCH         PSA_ARCH_TESTS_FORCE_PATCH
		FETCH_CONTENT_ARGS
			GIT_REPOSITORY      https://github.com/ARM-software/psa-arch-tests.git
			GIT_TAG             ${PSA_ARCH_TESTS_VERSION}
			GIT_SHALLOW         FALSE
			GIT_PROGRESS        TRUE
			GIT_SUBMODULES      ""
	)
else()
	set(PSA_ARCH_TESTS_PATH                    "${CMAKE_BINARY_DIR}/lib/ext/psa_arch_tests-src"  CACHE PATH      "Path to psa_arch_tests-src (or DOWNLOAD to fetch automatically"  FORCE)
	execute_process(COMMAND cp -r "${CMAKE_SOURCE_DIR}/lib/ext/psa_arch_tests-src/" "${CMAKE_BINARY_DIR}/lib/ext/")
	if((${TEST_PSA_API} STREQUAL "INITIAL_ATTESTATION"))
		execute_process(COMMAND mkdir -p "${CMAKE_BINARY_DIR}/tf-m-tests/app/psa_api_tests/")
		execute_process(COMMAND cp -r "${CMAKE_SOURCE_DIR}/lib/ext/psa_qcbor-src/" "${CMAKE_BINARY_DIR}/tf-m-tests/app/psa_api_tests/psa_qcbor/")
	endif()
endif()
