#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# If NS app, secure regression test or non-secure regression test is enabled,
# fetch tf-m-tests repo.
# The conditiions are actually overlapped but it can make the logic more clear.
# Besides, the dependencies between NS app and regression tests will be
# optimized later.
if (NS OR TFM_S_REG_TEST OR TFM_NS_REG_TEST OR TEST_BL2 OR TEST_BL1_1 OR TEST_BL1_2)
    # Set tf-m-tests repo config
    include(${CMAKE_SOURCE_DIR}/lib/ext/tf-m-tests/repo_config_default.cmake)

	if(USE_GIT_PULL)
		fetch_remote_library(
			LIB_NAME                tfm_test_repo
			LIB_SOURCE_PATH_VAR     TFM_TEST_REPO_PATH
			LIB_BASE_DIR            "${CMAKE_BINARY_DIR}/lib/ext"
			FETCH_CONTENT_ARGS
				GIT_REPOSITORY      https://git.trustedfirmware.org/TF-M/tf-m-tests.git
				GIT_TAG             ${TFM_TEST_REPO_VERSION}
				GIT_PROGRESS TRUE
		)
	else()
		set(TFM_TEST_REPO_PATH                    "${CMAKE_BINARY_DIR}/lib/ext/tfm_test_repo-src"  CACHE PATH      "Path to TFM Test (or DOWNLOAD to fetch automatically"  FORCE)
		execute_process(COMMAND cp -r "${CMAKE_SOURCE_DIR}/lib/ext/tfm_test_repo-src/" "${CMAKE_BINARY_DIR}/lib/ext/")
	endif()

    if ("${CMSIS_5_PATH}" STREQUAL DOWNLOAD)
        set(CMSIS_5_PATH ${TFM_TEST_REPO_PATH}/CMSIS CACHE PATH "Path to CMSIS_5 (or DOWNLOAD to fetch automatically" FORCE)
    endif()

    if (NOT TFM_TEST_PATH)
        set(TFM_TEST_PATH ${TFM_TEST_REPO_PATH}/test CACHE PATH "Path to TFM tests" FORCE)
    endif()
endif()
