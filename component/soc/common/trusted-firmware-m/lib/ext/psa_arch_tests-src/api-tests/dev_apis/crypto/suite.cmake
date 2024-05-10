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

foreach(test ${PSA_TEST_LIST})
	include(${PSA_SUITE_DIR}/${test}/test.cmake)
	foreach(source_file ${CC_SOURCE})
		list(APPEND SUITE_CC_SOURCE
			${PSA_SUITE_DIR}/${test}/${source_file}
		)
	endforeach()
	foreach(asm_file ${AS_SOURCE})
		list(APPEND SUITE_AS_SOURCE
			${PSA_SUITE_DIR}/${test}/${asm_file}
		)
	endforeach()
	unset(CC_SOURCE)
	unset(AS_SOURCE)
endforeach()

add_definitions(${CC_OPTIONS})
add_definitions(${AS_OPTIONS})

set(MISSING_CRYPTO_1_0 0 CACHE INTERNAL "Disable calls to crypto functions missing from Mbed TLS 2.x")
add_definitions(-DMISSING_CRYPTO_1_0=${MISSING_CRYPTO_1_0})

# append common crypto file to list of source collected
list(APPEND SUITE_CC_SOURCE ${PSA_SUITE_DIR}/common/test_crypto_common.c)
add_library(${PSA_TARGET_TEST_COMBINE_LIB} STATIC ${SUITE_CC_SOURCE} ${SUITE_AS_SOURCE})

# Test related Include directories
foreach(test ${PSA_TEST_LIST})
	target_include_directories(${PSA_TARGET_TEST_COMBINE_LIB} PRIVATE ${PSA_SUITE_DIR}/${test})
endforeach()

# PSA Include directories
foreach(psa_inc_path ${PSA_INCLUDE_PATHS})
	target_include_directories(${PSA_TARGET_TEST_COMBINE_LIB} PRIVATE
                ${psa_inc_path}
        )
endforeach()

target_include_directories(${PSA_TARGET_TEST_COMBINE_LIB} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${PSA_ROOT_DIR}/val/common
	${PSA_ROOT_DIR}/val/nspe
	${PSA_ROOT_DIR}/platform/targets/common/nspe
	${PSA_ROOT_DIR}/platform/targets/common/nspe/crypto
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
	${PSA_SUITE_DIR}/common/
)

target_compile_definitions(${PSA_TARGET_TEST_COMBINE_LIB}
	PRIVATE
		$<$<STREQUAL:${TARGET},tgt_dev_apis_tfm_amebadplus>:TGT_DEV_APIS_TFM_AMEBADPLUS>
)