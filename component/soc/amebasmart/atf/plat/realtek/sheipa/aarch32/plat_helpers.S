/*
 * Copyright (c) 2020 Realtek Semiconductor Corp. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch.h>
#include <asm_macros.S>
#include <assert_macros.S>
#include <platform_def.h>

	.globl	plat_my_core_pos
	.globl	plat_report_exception
	.globl	plat_report_prefetch_abort
	.globl	plat_report_data_abort
	.globl	platform_mem_init
	.globl	plat_sheipa_calc_core_pos
	.globl	plat_crash_console_init
	.globl	plat_crash_console_putc
	.globl	plat_crash_console_flush
	.globl	plat_secondary_cold_boot_setup
	.globl	plat_get_my_entrypoint
	.globl	plat_is_my_cpu_primary
	.globl	plat_panic_handler


func plat_my_core_pos
	ldcopr	r0, MPIDR
	b	plat_sheipa_calc_core_pos
endfunc plat_my_core_pos

func plat_report_exception
	mov	r8, lr

	/*
	 * Test if an abort occurred
	 * In this case the error message has already been displayed
	 * by dedicated functions
	 */
	cmp	r0, #MODE32_abt
	beq	1f

	/* Test for an undefined instruction */
	cmp	r0, #MODE32_und
	bne	other_exception_lbl
	ldr	r4, =undefined_str
	bl	asm_print_str
	mrs	r4, lr_und
	b	print_exception_info

other_exception_lbl:
	/* Other exceptions */
	mov	r9, r0
	ldr	r4, =exception_start_str
	bl	asm_print_str
	mov	r4, r9
	bl	asm_print_hex
	ldr	r4, =exception_end_str
	bl	asm_print_str
	mov	r4, r6

print_exception_info:
	bl	asm_print_hex

	ldr	r4, =end_error_str
	bl	asm_print_str

1:
	bx	r8
endfunc plat_report_exception

func plat_report_prefetch_abort
	mov	r8, lr
	mov	r9, r0

	ldr	r4, =prefetch_abort_str
	bl	asm_print_str

	mov	r4, r9
	sub	r4, r4, #4
	bl	asm_print_hex

	ldr	r4, =ifsr_str
	bl	asm_print_str

	ldcopr r4, IFSR
	bl	asm_print_hex

	ldr	r4, =ifar_str
	bl	asm_print_str

	ldcopr	r4, IFAR
	bl	asm_print_hex

	ldr	r4, =end_error_str
	bl	asm_print_str

	bx	r8
endfunc plat_report_prefetch_abort

func plat_report_data_abort
	mov	r8, lr
	mov	r9, r0

	ldr	r4, =data_abort_str
	bl	asm_print_str

	mov	r4, r9
	sub	r4, r4, #8
	bl	asm_print_hex

	ldr	r4, =dfsr_str
	bl	asm_print_str

	ldcopr	r4, DFSR
	bl	asm_print_hex

	ldr	r4, =dfar_str
	bl	asm_print_str

	ldcopr	r4, DFAR
	bl	asm_print_hex

	ldr	r4, =end_error_str
	bl	asm_print_str

	bx	r8
endfunc plat_report_data_abort


#if CPU_ARMv8_2
/*
 *  unsigned int plat_sheipa_calc_core_pos(u_register_t mpidr);
 *  With this function: CorePos = (ClusterId * 4) + CoreId
 */
func plat_sheipa_calc_core_pos
	lsr	r0, r0, #8
	and	r1, r0, #MPIDR_CPU_MASK
	and	r0, r0, #MPIDR_CLUSTER_MASK
	add	r0, r1, r0, LSR #5
	bx	lr
endfunc plat_sheipa_calc_core_pos
#else
/*
 *  unsigned int plat_sheipa_calc_core_pos(u_register_t mpidr);
 *  With this function: CorePos = (ClusterId * 4) + CoreId
 */
func plat_sheipa_calc_core_pos
	and	r1, r0, #MPIDR_CPU_MASK
	and	r0, r0, #MPIDR_CLUSTER_MASK
	add	r0, r1, r0, LSR #6
	bx	lr
endfunc plat_sheipa_calc_core_pos
#endif
	/* -----------------------------------------------------
	 * unsigned int plat_is_my_cpu_primary (void);
	 *
	 * Find out whether the current cpu is the primary
	 * cpu.
	 * -----------------------------------------------------
	 */
func plat_is_my_cpu_primary
	ldcopr	r0, MPIDR
	ldr	r1, =(MPIDR_CLUSTER_MASK | MPIDR_CPU_MASK)
	and	r0, r0, r1
	cmp	r0, #SHEIPA_PRIMARY_CPU
	moveq	r0, #1
	movne	r0, #0
	bx	lr
endfunc plat_is_my_cpu_primary

	/* -----------------------------------------------------
	 * void plat_secondary_cold_boot_setup (void);
	 *
	 * This function performs any platform specific actions
	 * needed for a secondary cpu after a cold reset e.g
	 * mark the cpu's presence, mechanism to place it in a
	 * holding pen etc.
	 * -----------------------------------------------------
	 */
func plat_secondary_cold_boot_setup
	/* Calculate address of our hold entry */
	bl	plat_my_core_pos
	mov_imm	r2, PLAT_SHEIPA_HOLD_BASE
	mov	r1, #0x0
	str	r1, [r2, #0x8]
	/* Wait until we have a go */
poll_mailbox:
	ldr	r1, [r2, #0x8]
	cmp	r1, #0
	beq	1f
	mov_imm	r0, PLAT_SHEIPA_TRUSTED_MAILBOX_BASE
	ldr	r1, [r0]
	bx	r1
1:
	wfe
	b	poll_mailbox
endfunc plat_secondary_cold_boot_setup

func plat_get_my_entrypoint
	/* TODO support warm boot */
	mov	r2, lr
	bl	plat_is_my_cpu_primary
	cmp	r0, #0
	beq	1f
	mov_imm	r1, BOOT_REASON_SW_REG
	mov_imm	r3, BIT_AP_WAKE_FROM_PG_FLAG
	ldr	r4, [r1]
	tst	r4, r3
	beq	1f
	mov_imm	r1, PLAT_SHEIPA_WARM_BOOT_BASE
	ldr	r0, [r1]
	mov	lr, r2
	bx	lr
1:
	mov	r0, #0
	mov	lr, r2
	bx	lr
endfunc plat_get_my_entrypoint

func platform_mem_init
	bx	lr
endfunc platform_mem_init

	/* ---------------------------------------------
	 * int plat_crash_console_init(void)
	 * Function to initialize the crash console
	 * without a C Runtime to print crash report.
	 * Clobber list : r0, r1, r2
	 * ---------------------------------------------
	 */
func plat_crash_console_init
	mov_imm	r0, PLAT_SHEIPA_CRASH_UART_BASE
	mov_imm	r1, PLAT_SHEIPA_CRASH_UART_CLK_IN_HZ
	mov_imm	r2, PLAT_SHEIPA_CONSOLE_BAUDRATE
	b	console_core_init
endfunc plat_crash_console_init

	/* ---------------------------------------------
	 * int plat_crash_console_putc(int c)
	 * Function to print a character on the crash
	 * console without a C Runtime.
	 * Clobber list : r1, r2
	 * ---------------------------------------------
	 */
func plat_crash_console_putc
	mov_imm	r1, PLAT_SHEIPA_CRASH_UART_BASE
	b	console_putc
endfunc plat_crash_console_putc

	/* ---------------------------------------------
	 * int plat_crash_console_flush(int c)
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * Out : return -1 on error else return 0.
	 * Clobber list : r0, r1
	 * ---------------------------------------------
	 */
func plat_crash_console_flush
	mov_imm	r0, PLAT_SHEIPA_CRASH_UART_BASE
	b	console_core_flush
endfunc plat_crash_console_flush

	/* ----------------------------------------------------------
	 * void plat_panic_handler(void) __dead2;
	 * Report exception + endless loop.
	 *
	 * r6 holds the address where the fault occurred.
	 * Filling lr with this value allows debuggers to reconstruct
	 * the backtrace.
	 * ----------------------------------------------------------
	 */
func plat_panic_handler
	mrs	r0, cpsr
	and	r0, #MODE32_MASK
	bl	plat_report_exception
	mov	lr, r6
	b	.
endfunc plat_panic_handler

.section .rodata.rev_err_str, "aS"
prefetch_abort_str:
	.asciz "\nPrefetch Abort at: 0x"
data_abort_str:
	.asciz "\nData Abort at: 0x"
undefined_str:
	.asciz "\nUndefined instruction at: 0x"
exception_start_str:
	.asciz "\nException mode=0x"
exception_end_str:
	.asciz " at: 0x"
dfsr_str:
	.asciz " DFSR = 0x"
dfar_str:
	.asciz " DFAR = 0x"
ifsr_str:
	.asciz " IFSR = 0x"
ifar_str:
	.asciz " IFAR = 0x"
end_error_str:
	.asciz "\n\r"