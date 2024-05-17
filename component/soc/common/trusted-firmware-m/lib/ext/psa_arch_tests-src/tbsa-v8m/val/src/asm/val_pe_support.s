///** @file
// * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
// * SPDX-License-Identifier : Apache-2.0
// *
// * Licensed under the Apache License, Version 2.0 (the "License");
// * you may not use this file except in compliance with the License.
// * You may obtain a copy of the License at
// *
// *  http://www.apache.org/licenses/LICENSE-2.0
// *
// * Unless required by applicable law or agreed to in writing, software
// * distributed under the License is distributed on an "AS IS" BASIS,
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// * See the License for the specific language governing permissions and
// * limitations under the License.
//**/


    .syntax unified
    .thumb
    .thumb_func

    .global ldrb_asm
    .global ldrh_asm
    .global ldr_asm
    .global strb_asm
    .global strh_asm
    .global str_asm
    .global tt_asm
    .global ldr_wide_asm
    .global str_wide_asm
    .global dsb_asm
    .global isb_asm
    .global sysread_control_asm
    .global sysread_control_asm
    .global sysread_control_asm
    .global sysread_control_ns_asm
    .global syswrite_control_asm
    .global syswrite_control_ns_asm

ldrb_asm:
  ldrb   r0, [r0]
  bx     lr

ldrh_asm:
  ldrh   r0, [r0]
  bx lr

ldr_asm:
  ldr    r0, [r0]
  bx lr

strb_asm:
  strb  r1,[r0]
  bx lr

strh_asm:
  strh  r1,[r0]
  bx lr

str_asm:
  str  r1,[r0]
  bx lr

tt_asm:
  tt     r0, r0
  bx lr

ldr_wide_asm:
    ldr r0, [r0]
    nop
    bx lr

str_wide_asm:
    str r1, [r0]
    nop
    bx lr

dsb_asm:
    dsb sy
    bx lr

isb_asm:
    isb
    bx lr

sysread_control_asm:
    mrs  r0, control
    bx lr

sysread_control_ns_asm:
    mrs  r0, control_ns
    bx lr

syswrite_control_asm:
    msr  control, r0
    isb
    bx lr

syswrite_control_ns_asm:
    msr  control_ns, r0
    isb
    bx lr

//  END
