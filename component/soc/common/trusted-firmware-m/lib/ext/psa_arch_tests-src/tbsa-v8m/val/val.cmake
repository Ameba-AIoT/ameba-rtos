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

# Collecting all the required sources from val
list(APPEND VAL_SRC_C
	"${TBSA_ROOT_DIR}/val/src/val_crypto.c"
	"${TBSA_ROOT_DIR}/val/src/val_debug.c"
	"${TBSA_ROOT_DIR}/val/src/val_dispatcher.c"
	"${TBSA_ROOT_DIR}/val/src/val_fuse.c"
	"${TBSA_ROOT_DIR}/val/src/val_infra.c"
	"${TBSA_ROOT_DIR}/val/src/val_interfaces.c"
	"${TBSA_ROOT_DIR}/val/src/val_interrupt.c"
	"${TBSA_ROOT_DIR}/val/src/val_ns_callable.c"
	"${TBSA_ROOT_DIR}/val/src/val_pe.c"
	"${TBSA_ROOT_DIR}/val/src/val_peripherals.c"
	"${TBSA_ROOT_DIR}/val/src/val_test.c"
	)

list(APPEND VAL_SRC_ASM_S
	"${TBSA_ROOT_DIR}/val/src/asm/val_secure_${ARCH}.s"
	"${TBSA_ROOT_DIR}/val/src/asm/val_pe_support.s"
	)

# Create VAL static library
add_library(${TBSA_TARGET_VAL_LIB} STATIC ${VAL_SRC_C} ${VAL_SRC_ASM_S})

target_include_directories(${TBSA_TARGET_VAL_LIB} PUBLIC ${TBSA_ROOT_DIR}/val/include)
