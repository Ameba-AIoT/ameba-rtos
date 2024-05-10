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
		LIB_NAME                mbedcrypto
		LIB_SOURCE_PATH_VAR     MBEDCRYPTO_PATH
		LIB_PATCH_DIR           ${CMAKE_CURRENT_LIST_DIR}
		LIB_FORCE_PATCH         ${MBEDCRYPTO_FORCE_PATCH}
		FETCH_CONTENT_ARGS
			GIT_REPOSITORY      ${MBEDCRYPTO_GIT_REMOTE}
			GIT_TAG             ${MBEDCRYPTO_VERSION}
			GIT_SHALLOW         TRUE
			GIT_PROGRESS        TRUE
			GIT_SUBMODULES      ""
	)
else()
	set(MBEDCRYPTO_PATH                    "${CMAKE_BINARY_DIR}/lib/ext/mbedcrypto-src"  CACHE PATH      "Path to Mbed Crypto (or DOWNLOAD to fetch automatically"  FORCE)
	execute_process(COMMAND cp -r "${CMAKE_SOURCE_DIR}/lib/ext/mbedcrypto-src/" "${CMAKE_BINARY_DIR}/lib/ext/")
endif()
