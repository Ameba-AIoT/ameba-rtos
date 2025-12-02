/*
 * Copyright (c) 2016-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <common/bl_common.h>
#include <context.h>
#include <el3_common_macros.S>
#include <smccc_helpers.h>
#include <smccc_macros.S>
#include <platform_def.h>


.syntax unified
.text
.global bl1_boot_sram
.type   bl1_boot_sram, %function

bl1_boot_sram:
    mov sp, #0
    mov lr, #0

    cps #MODE32_sys
    isb
    mov r0, #0
    mov r1, #0
    mov r2, #0
    mov r3, #0
    mov r4, #0
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    mov r9, #0
    mov r10, #0
    mov r11, #0
    mov r12, #0
    mov sp, #0
    mov lr, #0

    mov_imm r1, BOOT_REASON_SW_REG
    mov_imm r3, BIT_AP_RUNNING
    ldr     r4, [r1]
    tst     r4, r3
    /* Branch if Equal means branch if Z=1, Z=1 when (r4 & r3) == 0 */
    beq     bl1_entrypoint

    cpsid   if                          // disable IRQ/FIQ
    mrc     p15, 0, r0, c1, c0, 0       // Read SCTLR
    bic     r0, r0, #(1 << 0)           // MMU disable (M bit)
    bic     r0, r0, #(1 << 2)           // D-cache disable (C bit)
    bic     r0, r0, #(1 << 12)          // I-cache disable (I bit)
    mcr     p15, 0, r0, c1, c0, 0       // Write SCTLR
    dsb     sy
    isb

    mov     r0, #0
    mcr     p15, 0, r0, c7, c5, 6       // BPIALL - Invalidate branch predictor
    isb

    mcr     p15, 0, r0, c7, c5, 0       // ICIALLU - Invalidate all I-cache
    isb

    mcr     p15, 0, r0, c8, c7, 0       // TLBIALL - Invalidate unified TLB
    dsb     sy
    isb

    bl      dcache_invalidate_all_levels
    dsb     sy
    isb

poll_loop:
    mov_imm r1, BOOT_REASON_SW_REG
    mov_imm r3, BIT_AP_RUNNING
    ldr     r4, [r1]
    dsb     sy
    tst     r4, r3
    beq     bl1_entrypoint

    mov     r0, #1024
1:  nop
    subs    r0, r0, #1
    bne     1b
    b       poll_loop

.globl dcache_invalidate_all_levels
/* void dcache_invalidate_all_levels(void) */
func dcache_invalidate_all_levels
    mrc     p15, 1, r1, c0, c0, 1      /* Read CLIDR -> r1 */
    mov     r2, #0                     /* level i = 0 */

1:  cmp     r2, #7
    beq     9f

    /* cache_type = (CLIDR >> (i*3)) & 7 */
    add     r3, r2, r2, lsl #1         /* r3 = i*3 */
    mov     r4, r1, lsr r3
    and     r4, r4, #7
    cmp     r4, #2
    blt     8f
    cmp     r4, #4
    bgt     8f

    /* Select data/unified cache for this level: CSSELR = (i << 1) */
    mov     r5, r2, lsl #1
    mcr     p15, 2, r5, c0, c0, 0      /* Write CSSELR */
    isb

    mrc     p15, 1, r6, c0, c0, 0      /* Read CCSIDR into r6 */

    /* log2_linesize = (CCSIDR[2:0]) + 4 -> r9 */
    and     r9, r6, #7
    add     r9, r9, #4

    /* num_ways = (((CCSIDR>>3)&0x3FF) + 1) -> r8 */
    mov     r8, r6, lsr #3             /* r8 = CCSIDR >> 3 */
    mov     r10, #1
    lsl     r10, r10, #10              /* r10 = 1<<10 */
    sub     r10, r10, #1               /* r10 = 0x3FF */
    and     r8, r8, r10                /* r8 = (..)&0x3FF */
    add     r8, r8, #1

    /* num_sets = (((CCSIDR>>13)&0x7FFF) + 1) -> r7 */
    mov     r7, r6, lsr #13            /* r7 = CCSIDR >> 13 */
    mov     r10, #1
    lsl     r10, r10, #15              /* r10 = 1<<15 */
    sub     r10, r10, #1               /* r10 = 0x7FFF */
    and     r7, r7, r10                /* r7 = (..)&0x7FFF */
    add     r7, r7, #1

    /* shift_way = 32 - log2_up(num_ways) -> r12 */
    mov     r10, r8
    subs    r10, r10, #1
    beq     3f
    clz     r11, r10
    rsb     r11, r11, #32              /* log2_up(num_ways) */
    rsb     r12, r11, #32              /* shift_way */
    b       4f
3:  mov     r12, #32

4:  /* way = num_ways-1 */
    subs    r8, r8, #1
2:  /* set = num_sets-1 */
    mov     r5, r7
    subs    r5, r5, #1
0:  /* Dummy = (i<<1) | (set<<log2_linesize) | (way<<shift_way) */
    mov     r6, r2, lsl #1
    orr     r6, r6, r5, lsl r9
    orr     r6, r6, r8, lsl r12
    mcr     p15, 0, r6, c7, c6, 2      /* DCISW - Invalidate by Set/Way */

    subs    r5, r5, #1
    bge     0b

    subs    r8, r8, #1
    bge     2b

    dsb     sy                         /* Data synchronization barrier after each level */

8:  add     r2, r2, #1
    b       1b

9:  dsb     sy
    isb
    bx      lr
    .size   dcache_invalidate_all_levels, .-dcache_invalidate_all_levels
endfunc dcache_invalidate_all_levels
