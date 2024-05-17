#/** @file
# * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

# This file will have global functions which are called by secure code
    .syntax unified

    .thumb
    .thumb_func
    .extern  __TBSA_S_StackTop
    .extern  __TBSA_NS_StackTop
    .extern  __TBSA_S_StackLimit
    .global tbsa_entry
tbsa_entry:
    ldr r0, =__TBSA_S_StackTop
    msr msp, r0
    ldr r0, =__TBSA_S_StackLimit
    msr msplim, r0
    ldr r0, =__TBSA_NS_StackTop
    msr msp_ns, r0
    b tbsa_main

    .syntax unified

    .thumb
    .thumb_func
    .global val_s_to_ns
val_s_to_ns:
    push  {r7, lr}
    sub   sp, #52
    add   r7, sp, #0
    str   r2, [r7, #40]
    str   r3, [r7, #36]
    str   r4, [r7, #32]
    str   r5, [r7, #28]
    str   r6, [r7, #24]
    mov   r6, r8
    str   r6, [r7, #16]
    mov   r6, r9
    str   r6, [r7, #12]
    mov   r6, r10
    str   r6, [r7, #8]
    mov   r6, r11
    str   r6, [r7, #4]
    mov   r6, r12
    str   r6, [r7, #0]
    mov   r2, #0
    mov   r3, r2
    mov   r4, r2
    mov   r5, r2
    mov   r6, r2
    mov   r8, r2
    mov   r9, r2
    mov   r10, r2
    mov   r11, r2
    mov   r12, r2
    mov   r2, #1
    bics  r0, r2
    mov   r2, r0
    mov   r0, r1
    blxns r2
    ldr   r2, [r7, #40]
    ldr   r3, [r7, #36]
    ldr   r4, [r7, #32]
    ldr   r5, [r7, #28]
    ldr   r6, [r7, #24]
    push  {r0}
    ldr   r0, [r7, #16]
    mov   r8, r0
    ldr   r0, [r7, #12]
    mov   r9, r0
    ldr   r0, [r7, #8]
    mov   r10, r0
    ldr   r0, [r7, #4]
    mov   r11, r0
    ldr   r0, [r7, #4]
    mov   r12, r0
    pop {r0}
    adds  r7, #52
    mov   sp, r7
    pop   {r7, pc}
    nop
