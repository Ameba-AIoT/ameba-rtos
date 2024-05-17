/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code which tailors the port to a specific RISC-V chip:
 *
 * + The code that is common to all RISC-V chips is implemented in
 *   FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S.  There is only one
 *   portASM.S file because the same file is used no matter which RISC-V chip is
 *   in use.
 *
 * + The code that tailors the kernel's RISC-V port to a specific RISC-V
 *   chip is implemented in freertos_risc_v_chip_specific_extensions.h.  There
 *   is one freertos_risc_v_chip_specific_extensions.h that can be used with any
 *   RISC-V chip that both includes a standard CLINT and does not add to the
 *   base set of RISC-V registers.  There are additional
 *   freertos_risc_v_chip_specific_extensions.h files for RISC-V implementations
 *   that do not include a standard CLINT or do add to the base set of RISC-V
 *   registers.
 *
 * CARE MUST BE TAKEN TO INCLDUE THE CORRECT
 * freertos_risc_v_chip_specific_extensions.h HEADER FILE FOR THE CHIP
 * IN USE.  To include the correct freertos_risc_v_chip_specific_extensions.h
 * header file ensure the path to the correct header file is in the assembler's
 * include path.
 *
 * This freertos_risc_v_chip_specific_extensions.h is for use on RISC-V chips
 * that include a standard CLINT and do not add to the base set of RISC-V
 * registers.
 *
 */
#include "FreeRTOSConfig.h"

#if __riscv_xlen == 64
	#define portWORD_SIZE 8
	#define store_x sd
	#define load_x ld
#if defined( __riscv_32e ) && ( __riscv_32e == 1 )
	#error "!!! RV32E xlen must be 32!!"
#endif
#elif __riscv_xlen == 32
	#define store_x sw
	#define load_x lw
	#define portWORD_SIZE 4
#else
	#error Assembler did not define __riscv_xlen
#endif

#if (__riscv_flen == 32)
    #define load_f flw
    #define store_f fsw
#elif (__riscv_flen == 64)
    #define load_f fld
    #define store_f fsd
#endif

#include "freertos_risc_v_chip_specific_extensions.h"

/* Check the freertos_risc_v_chip_specific_extensions.h and/or command line
definitions. */
#ifndef portasmHAS_CLINT
	#error freertos_risc_v_chip_specific_extensions.h must define portasmHAS_CLINT to either 1 (CLINT present) or 0 (clint not present).
#endif

#ifndef portasmHANDLE_INTERRUPT
	#error portasmHANDLE_INTERRUPT must be defined to the function to be called to handle external/peripheral interrupts.  portasmHANDLE_INTERRUPT can be defined on the assmbler command line or in the appropriate freertos_risc_v_chip_specific_extensions.h header file.
#endif

/* Only the standard core registers are stored by default.  Any additional
registers must be saved by the portasmSAVE_ADDITIONAL_REGISTERS and
portasmRESTORE_ADDITIONAL_REGISTERS macros - which can be defined in a chip
specific version of freertos_risc_v_chip_specific_extensions.h.  See the notes
at the top of this file. */
#if defined( __riscv_flen )
#if defined( configNESTED_IRQ ) && ( configNESTED_IRQ == 1)
    #define portMIE_OFFSET   (66 * portWORD_SIZE)
    #define portCONTEXT_SIZE ( 67 * portWORD_SIZE )
#else
    #define portCONTEXT_SIZE ( 66 * portWORD_SIZE )
#endif
    #define portMStatus_OFFSET ( 65 * portWORD_SIZE )
#else
#if defined( __riscv_32e ) && ( __riscv_32e == 1 )
#if defined( configNESTED_IRQ ) && ( configNESTED_IRQ == 1)
    #define portMIE_OFFSET   (17 * portWORD_SIZE)
    #define portCONTEXT_SIZE ( 18 * portWORD_SIZE )
#else
    #define portCONTEXT_SIZE ( 17 * portWORD_SIZE )
#endif /* configNESTED_IRQ */
    #define portMStatus_OFFSET ( 16 * portWORD_SIZE )
#else
#if defined( configNESTED_IRQ ) && ( configNESTED_IRQ == 1)
    #define portMIE_OFFSET   (33 * portWORD_SIZE)
    #define portCONTEXT_SIZE ( 34 * portWORD_SIZE )
#else
    #define portCONTEXT_SIZE ( 33 * portWORD_SIZE )
#endif /* configNESTED_IRQ */
    #define portMStatus_OFFSET ( 32 * portWORD_SIZE )
#endif
#endif

.global xPortStartFirstTask
.global freertos_risc_v_trap_handler
.global pxPortInitialiseStack
.extern pxCurrentTCB
.extern ulPortTrapHandler
.extern vTaskSwitchContext
.extern xTaskIncrementTick
.extern Timer_IRQHandler
.extern pullMachineTimerCompareRegister
.extern pullNextTime
.extern uxTimerIncrementsForOneTick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
.extern xISRStack
.extern xISRStackTop
.extern portasmHANDLE_INTERRUPT
#if( configENABLE_FPU == 1 )
.extern prvSetupFPU
#endif
#if( portUSING_MPU_WRAPPERS == 1 )
	.extern vPortPmpSwitch
	.extern xPmpInfo
    .extern privilege_status
#endif
.extern vPortMExceptionHanler

#ifdef CONFIG_DEBUG_RPROBE
.extern RegArryBase

.macro Save_RegArry
	load_x  t0, RegArryBase
	store_x x0, 0 ( t0 )
	store_x x1, 1 * portWORD_SIZE( t0 )    /* ra */
	store_x x2, 2 * portWORD_SIZE( t0 )    /* sp */
	store_x x3, 3 * portWORD_SIZE( t0 )    /* gp */
	store_x x4, 4 * portWORD_SIZE( t0 )    /* tp */
	store_x x6, 6 * portWORD_SIZE( t0 )    /* t1 */
	store_x x7, 7 * portWORD_SIZE( t0 )    /* t2 */
	store_x x8, 8 * portWORD_SIZE( t0 )    /* s0/fp */
	store_x x9, 9 * portWORD_SIZE( t0 )    /* s1 */
	store_x x10, 10 * portWORD_SIZE( t0 )   /* a0 */
	store_x x11, 11 * portWORD_SIZE( t0 )   /* a1 */
	store_x x12, 12 * portWORD_SIZE( t0 )   /* a2 */
	store_x x13, 13 * portWORD_SIZE( t0 )  /* a3 */
	store_x x14, 14 * portWORD_SIZE( t0 )  /* a4 */
	store_x x15, 15 * portWORD_SIZE( t0 )  /* a5 */
#if !defined( __riscv_32e ) || ( __riscv_32e == 0 )
	store_x x16, 16 * portWORD_SIZE( t0 )  /* a6 */
	store_x x17, 17 * portWORD_SIZE( t0 )  /* a7 */
	store_x x18, 18 * portWORD_SIZE( t0 )  /* s2 */
	store_x x19, 19 * portWORD_SIZE( t0 )  /* s3 */
	store_x x20, 20 * portWORD_SIZE( t0 )  /* s4 */
	store_x x21, 21 * portWORD_SIZE( t0 )  /* s5 */
	store_x x22, 22 * portWORD_SIZE( t0 )  /* s6 */
	store_x x23, 23 * portWORD_SIZE( t0 )  /* s7 */
	store_x x24, 24 * portWORD_SIZE( t0 )  /* s8 */
	store_x x25, 25 * portWORD_SIZE( t0 )  /* s9 */
	store_x x26, 26 * portWORD_SIZE( t0 )  /* s10 */
	store_x x27, 27 * portWORD_SIZE( t0 )  /* s11 */
	store_x x28, 28 * portWORD_SIZE( t0 )  /* t3 */
	store_x x29, 29 * portWORD_SIZE( t0 )  /* t4 */
	store_x x30, 30 * portWORD_SIZE( t0 )  /* t5 */
	store_x x31, 31 * portWORD_SIZE( t0 )  /* t6 */
#endif /* #if !defined( __riscv_32e ) || ( __riscv_32e == 0 ) */
	csrr a1, mepc
	store_x a1, 32 * portWORD_SIZE( t0 )
	/* store x5 */
	mv      t1,t0
	load_x  t0, 5 * portWORD_SIZE(sp)
	store_x t0, 5 * portWORD_SIZE(t1)
	.endm

.macro Restore_RegArry
	load_x t0, RegArryBase
	load_x a1, 32 * portWORD_SIZE( t0 )
	csrw mepc,a1
	load_x x0, 0 ( t0 )
	load_x x1, 1 * portWORD_SIZE( t0 )    /* ra */
	load_x x2, 2 * portWORD_SIZE( t0 )    /* sp */
	load_x x3, 3 * portWORD_SIZE( t0 )    /* gp */
	load_x x4, 4 * portWORD_SIZE( t0 )    /* tp */
	load_x x6, 6 * portWORD_SIZE( t0 )    /* t1 */
	load_x x7, 7 * portWORD_SIZE( t0 )    /* t2 */
	load_x x8, 8 * portWORD_SIZE( t0 )    /* s0/fp */
	load_x x9, 9 * portWORD_SIZE( t0 )    /* s1 */
	load_x x10, 10 * portWORD_SIZE( t0 )   /* a0 */
	load_x x11, 11 * portWORD_SIZE( t0 )   /* a1 */
	load_x x12, 12 * portWORD_SIZE( t0 )   /* a2 */
	load_x x13, 13 * portWORD_SIZE( t0 )  /* a3 */
	load_x x14, 14 * portWORD_SIZE( t0 )  /* a4 */
	load_x x15, 15 * portWORD_SIZE( t0 )  /* a5 */
#if !defined( __riscv_32e ) || ( __riscv_32e == 0 )
	load_x x16, 16 * portWORD_SIZE( t0 )  /* a6 */
	load_x x17, 17 * portWORD_SIZE( t0 )  /* a7 */
	load_x x18, 18 * portWORD_SIZE( t0 )  /* s2 */
	load_x x19, 19 * portWORD_SIZE( t0 )  /* s3 */
	load_x x20, 20 * portWORD_SIZE( t0 )  /* s4 */
	load_x x21, 21 * portWORD_SIZE( t0 )  /* s5 */
	load_x x22, 22 * portWORD_SIZE( t0 )  /* s6 */
	load_x x23, 23 * portWORD_SIZE( t0 )  /* s7 */
	load_x x24, 24 * portWORD_SIZE( t0 )  /* s8 */
	load_x x25, 25 * portWORD_SIZE( t0 )  /* s9 */
	load_x x26, 26 * portWORD_SIZE( t0 )  /* s10 */
	load_x x27, 27 * portWORD_SIZE( t0 )  /* s11 */
	load_x x28, 28 * portWORD_SIZE( t0 )  /* t3 */
	load_x x29, 29 * portWORD_SIZE( t0 )  /* t4 */
	load_x x30, 30 * portWORD_SIZE( t0 )  /* t5 */
	load_x x31, 31 * portWORD_SIZE( t0 )  /* t6 */
#endif /* #if !defined( __riscv_32e ) || ( __riscv_32e == 0 ) */
	load_x  t0, 5 * portWORD_SIZE(t0)
	.endm
#endif

/*------------------------------------------------------------------*/
/*
 * Register		ABI Name	Description						Saver
 * x0			zero		Hard-wired zero					-
 * x1			ra			Return address					Caller
 * x2			sp			Stack pointer					Callee
 * x3			gp			Global pointer					-
 * x4			tp			Thread pointer					-
 * x5-7			t0-2		Temporaries						Caller
 * x8			s0/fp		Saved register/Frame pointer	Callee
 * x9			s1			Saved register					Callee
 * x10-11		a0-1		Function arguments/return values Caller
 * x12-17		a2-7		Function arguments				Caller
 * x18-27		s2-11		Saved registers					Callee
 * x28-31		t3-6		Temporaries						Caller
 *
 * RV32E Arch. only have x0 - x15
 */

/*-----------------------------------------------------------*/
.macro portSAVE_BaseReg
	addi sp, sp, -portCONTEXT_SIZE
	store_x x1, 1 * portWORD_SIZE( sp )    /* ra */
	store_x x5, 5 * portWORD_SIZE( sp )    /* t0 */
	store_x x6, 6 * portWORD_SIZE( sp )    /* t1 */
	store_x x7, 7 * portWORD_SIZE( sp )    /* t2 */
	store_x x8, 8 * portWORD_SIZE( sp )    /* s0/fp */
	store_x x9, 9 * portWORD_SIZE( sp )    /* s1 */
	store_x x10, 10 * portWORD_SIZE( sp )   /* a0 */
	store_x x11, 11 * portWORD_SIZE( sp )   /* a1 */
	store_x x12, 12 * portWORD_SIZE( sp )   /* a2 */
	store_x x13, 13 * portWORD_SIZE( sp )  /* a3 */
	store_x x14, 14 * portWORD_SIZE( sp )  /* a4 */
	store_x x15, 15 * portWORD_SIZE( sp )  /* a5 */
#if !defined( __riscv_32e ) || ( __riscv_32e == 0 )
	store_x x16, 16 * portWORD_SIZE( sp )  /* a6 */
	store_x x17, 17 * portWORD_SIZE( sp )  /* a7 */
	store_x x18, 18 * portWORD_SIZE( sp )  /* s2 */
	store_x x19, 19 * portWORD_SIZE( sp )  /* s3 */
	store_x x20, 20 * portWORD_SIZE( sp )  /* s4 */
	store_x x21, 21 * portWORD_SIZE( sp )  /* s5 */
	store_x x22, 22 * portWORD_SIZE( sp )  /* s6 */
	store_x x23, 23 * portWORD_SIZE( sp )  /* s7 */
	store_x x24, 24 * portWORD_SIZE( sp )  /* s8 */
	store_x x25, 25 * portWORD_SIZE( sp )  /* s9 */
	store_x x26, 26 * portWORD_SIZE( sp )  /* s10 */
	store_x x27, 27 * portWORD_SIZE( sp )  /* s11 */
	store_x x28, 28 * portWORD_SIZE( sp )  /* t3 */
	store_x x29, 29 * portWORD_SIZE( sp )  /* t4 */
	store_x x30, 30 * portWORD_SIZE( sp )  /* t5 */
	store_x x31, 31 * portWORD_SIZE( sp )  /* t6 */
#endif /* #if !defined( __riscv_32e ) || ( __riscv_32e == 0 ) */
#if defined( __riscv_flen )
	csrr    t0, mstatus
	li      t1, 0x6000 /* get mstatus.FS */
	and     t0,t0,t1
	li      t1, 0x2000
	beq     t0,t1,1f /* save fpu if it's not in init state */
	frcsr   t0
	store_f f0, 32 * portWORD_SIZE( sp )
	store_f f1, 33 * portWORD_SIZE( sp )
	store_f f2, 34 * portWORD_SIZE( sp )
	store_f f3, 35 * portWORD_SIZE( sp )
	store_f f4, 36 * portWORD_SIZE( sp )
	store_f f5, 37 * portWORD_SIZE( sp )
	store_f f6, 38 * portWORD_SIZE( sp )
	store_f f7, 39 * portWORD_SIZE( sp )
	store_f f8, 40 * portWORD_SIZE( sp )
	store_f f9, 41 * portWORD_SIZE( sp )
	store_f f10, 42 * portWORD_SIZE( sp )
	store_f f11, 43 * portWORD_SIZE( sp )
	store_f f12, 44 * portWORD_SIZE( sp )
	store_f f13, 45 * portWORD_SIZE( sp )
	store_f f14, 46 * portWORD_SIZE( sp )
	store_f f15, 47 * portWORD_SIZE( sp )
	store_f f16, 48 * portWORD_SIZE( sp )
	store_f f17, 49 * portWORD_SIZE( sp )
	store_f f18, 50 * portWORD_SIZE( sp )
	store_f f19, 51 * portWORD_SIZE( sp )
	store_f f20, 52 * portWORD_SIZE( sp )
	store_f f21, 53 * portWORD_SIZE( sp )
	store_f f22, 54 * portWORD_SIZE( sp )
	store_f f23, 55 * portWORD_SIZE( sp )
	store_f f24, 56 * portWORD_SIZE( sp )
	store_f f25, 57 * portWORD_SIZE( sp )
	store_f f26, 58 * portWORD_SIZE( sp )
	store_f f27, 59 * portWORD_SIZE( sp )
	store_f f28, 60 * portWORD_SIZE( sp )
	store_f f29, 61 * portWORD_SIZE( sp )
	store_f f30, 62 * portWORD_SIZE( sp )
	store_f f31, 63 * portWORD_SIZE( sp )
	store_x t0, 64 * portWORD_SIZE( sp )
1:
#endif
	.endm

/*-----------------------------------------------------------*/

.macro portRESTORE_BaseReg
#if defined( __riscv_flen )
	csrr    t0, mstatus
	li      t1, 0x6000 /* get mstatus.FS */
	and     t0,t0,t1
	li      t1, 0x2000
	beq     t0,t1,1f /* restore fpu if it's not in init state */
	load_f f0, 32 * portWORD_SIZE( sp )
	load_f f1, 33 * portWORD_SIZE( sp )
	load_f f2, 34 * portWORD_SIZE( sp )
	load_f f3, 35 * portWORD_SIZE( sp )
	load_f f4, 36 * portWORD_SIZE( sp )
	load_f f5, 37 * portWORD_SIZE( sp )
	load_f f6, 38 * portWORD_SIZE( sp )
	load_f f7, 39 * portWORD_SIZE( sp )
	load_f f8, 40 * portWORD_SIZE( sp )
	load_f f9, 41 * portWORD_SIZE( sp )
	load_f f10, 42 * portWORD_SIZE( sp )
	load_f f11, 43 * portWORD_SIZE( sp )
	load_f f12, 44 * portWORD_SIZE( sp )
	load_f f13, 45 * portWORD_SIZE( sp )
	load_f f14, 46 * portWORD_SIZE( sp )
	load_f f15, 47 * portWORD_SIZE( sp )
	load_f f16, 48 * portWORD_SIZE( sp )
	load_f f17, 49 * portWORD_SIZE( sp )
	load_f f18, 50 * portWORD_SIZE( sp )
	load_f f19, 51 * portWORD_SIZE( sp )
	load_f f20, 52 * portWORD_SIZE( sp )
	load_f f21, 53 * portWORD_SIZE( sp )
	load_f f22, 54 * portWORD_SIZE( sp )
	load_f f23, 55 * portWORD_SIZE( sp )
	load_f f24, 56 * portWORD_SIZE( sp )
	load_f f25, 57 * portWORD_SIZE( sp )
	load_f f26, 58 * portWORD_SIZE( sp )
	load_f f27, 59 * portWORD_SIZE( sp )
	load_f f28, 60 * portWORD_SIZE( sp )
	load_f f29, 61 * portWORD_SIZE( sp )
	load_f f30, 62 * portWORD_SIZE( sp )
	load_f f31, 63 * portWORD_SIZE( sp )
	load_x t0, 64 * portWORD_SIZE( sp )
	fscsr t0
	li      t1, 0x6000 /* set FS = 0x2, fpu register status clean */
	csrc    mstatus, t1
	li      t1, 0x4000
	csrs    mstatus, t1
1:
#endif
	load_x  x1, 1 * portWORD_SIZE( sp )
	load_x  x5, 5 * portWORD_SIZE( sp )		/* t0 */
	load_x  x6, 6 * portWORD_SIZE( sp )		/* t1 */
	load_x  x7, 7 * portWORD_SIZE( sp )		/* t2 */
	load_x  x8, 8 * portWORD_SIZE( sp )		/* s0/fp */
	load_x  x9, 9 * portWORD_SIZE( sp )		/* s1 */
	load_x  x10, 10 * portWORD_SIZE( sp )	/* a0 */
	load_x  x11, 11 * portWORD_SIZE( sp )	/* a1 */
	load_x  x12, 12 * portWORD_SIZE( sp )	/* a2 */
	load_x  x13, 13 * portWORD_SIZE( sp )	/* a3 */
	load_x  x14, 14 * portWORD_SIZE( sp )	/* a4 */
	load_x  x15, 15 * portWORD_SIZE( sp )	/* a5 */
#if !defined( __riscv_32e ) || ( __riscv_32e == 0 )
	load_x  x16, 16 * portWORD_SIZE( sp )	/* a6 */
	load_x  x17, 17 * portWORD_SIZE( sp )	/* a7 */
	load_x  x18, 18 * portWORD_SIZE( sp )	/* s2 */
	load_x  x19, 19 * portWORD_SIZE( sp )	/* s3 */
	load_x  x20, 20 * portWORD_SIZE( sp )	/* s4 */
	load_x  x21, 21 * portWORD_SIZE( sp )	/* s5 */
	load_x  x22, 22 * portWORD_SIZE( sp )	/* s6 */
	load_x  x23, 23 * portWORD_SIZE( sp )	/* s7 */
	load_x  x24, 24 * portWORD_SIZE( sp )	/* s8 */
	load_x  x25, 25 * portWORD_SIZE( sp )	/* s9 */
	load_x  x26, 26 * portWORD_SIZE( sp )	/* s10 */
	load_x  x27, 27 * portWORD_SIZE( sp )	/* s11 */
	load_x  x28, 28 * portWORD_SIZE( sp )	/* t3 */
	load_x  x29, 29 * portWORD_SIZE( sp )	/* t4 */
	load_x  x30, 30 * portWORD_SIZE( sp )	/* t5 */
	load_x  x31, 31 * portWORD_SIZE( sp )	/* t6 */
#endif /* #if !defined( __riscv_32e ) || ( __riscv_32e == 0 ) */
	.endm
/*-----------------------------------------------------------*/

.align 3
.type freertos_risc_v_trap_handler, "function"
/* a0 is reserved for ECALL(SVC) argument */
freertos_risc_v_trap_handler:
#if defined( configVECTORED_INTERRRUPT ) && ( configVECTORED_INTERRRUPT == 1 )
#if defined(__riscv_compressed) && ( __riscv_compressed == 1 )
#define CPUNOP nop
#else
#define CPUNOP
#endif
	j non_vector_handler
	CPUNOP
	j int_1
	CPUNOP
	j int_2
	CPUNOP
	j int_3
	CPUNOP
	j int_4
	CPUNOP
	j int_5
	CPUNOP
	j int_6
	CPUNOP
	j int_7
	CPUNOP
	j int_8
	CPUNOP
	j int_9
	CPUNOP
	j int_10
	CPUNOP
	j int_11
	CPUNOP
	j int_12
	CPUNOP
	j int_13
	CPUNOP
	j int_14
	CPUNOP
	j int_15
	CPUNOP
	j clint_0
	CPUNOP
	j clint_1
	CPUNOP
	j clint_2
	CPUNOP
	j clint_3
	CPUNOP
	j clint_4
	CPUNOP
	j clint_5
	CPUNOP
	j clint_6
	CPUNOP
	j clint_7
	CPUNOP
	j clint_8
	CPUNOP
	j clint_9
	CPUNOP
	j clint_10
	CPUNOP
	j clint_11
	CPUNOP
	j clint_12
	CPUNOP
	j clint_13
	CPUNOP
	j clint_14
	CPUNOP
	j clint_15
	CPUNOP
int_1:
	csrrw t0, mscratch, t0
	li   t0, 1
	j    vector_handler
int_2:
	csrrw t0, mscratch, t0
	li   t0, 2
	j    vector_handler
int_3:
	csrrw t0, mscratch, t0
	li   t0, 3
	j    vector_handler
int_4:
	csrrw t0, mscratch, t0
	li   t0, 4
	j    vector_handler
int_5:
	csrrw t0, mscratch, t0
	li   t0, 5
	j    vector_handler
int_6:
	csrrw t0, mscratch, t0
	li   t0, 6
	j    vector_handler
int_7:
	csrrw t0, mscratch, t0
	li   t0, 7
	j    machine_timer_handler
int_8:
	csrrw t0, mscratch, t0
	li   t0, 8
	j    vector_handler
int_9:
	csrrw t0, mscratch, t0
	li   t0, 9
	j    vector_handler
int_10:
	csrrw t0, mscratch, t0
	li   t0, 10
	j    vector_handler
int_11:
	csrrw t0, mscratch, t0
	li   t0, 11
	j    vector_handler
int_12:
	csrrw t0, mscratch, t0
	li   t0, 12
	j    vector_handler
int_13:
	csrrw t0, mscratch, t0
	li   t0, 13
	j    vector_handler
int_14:
	csrrw t0, mscratch, t0
	li   t0, 14
	j    vector_handler
int_15:
	csrrw t0, mscratch, t0
	li   t0, 15
	j    vector_handler
clint_0:
	csrrw t0, mscratch, t0
	li   t0, 16
	j    vector_handler
clint_1:
	csrrw t0, mscratch, t0
	li   t0, 17
	j    vector_handler
clint_2:
	csrrw t0, mscratch, t0
	li   t0, 18
	j    vector_handler
clint_3:
	csrrw t0, mscratch, t0
	li   t0, 19
	j    vector_handler
clint_4:
	csrrw t0, mscratch, t0
	li   t0, 20
	j    vector_handler
clint_5:
	csrrw t0, mscratch, t0
	li   t0, 21
	j    vector_handler
clint_6:
	csrrw t0, mscratch, t0
	li   t0, 22
	j    vector_handler
clint_7:
	csrrw t0, mscratch, t0
	li   t0, 23
	j    vector_handler
clint_8:
	csrrw t0, mscratch, t0
	li   t0, 24
	j    vector_handler
clint_9:
	csrrw t0, mscratch, t0
	li   t0, 25
	j    vector_handler
clint_10:
	csrrw t0, mscratch, t0
	li   t0, 26
	j    vector_handler
clint_11:
	csrrw t0, mscratch, t0
	li   t0, 27
	j    vector_handler
clint_12:
	csrrw t0, mscratch, t0
	li   t0, 28
	j    vector_handler
clint_13:
	csrrw t0, mscratch, t0
	li   t0, 29
	j    vector_handler
clint_14:
	csrrw t0, mscratch, t0
	li   t0, 30
	j    vector_handler
clint_15:
	csrrw t0, mscratch, t0
	li   t0, 31
	j    vector_handler

machine_timer_handler:
	csrrw t0, mscratch, t0
	portSAVE_BaseReg

	csrr t0, mstatus                                        /* Required for MPIE bit. */
	store_x t0, portMStatus_OFFSET( sp )
#if defined( configNESTED_IRQ ) && ( configNESTED_IRQ == 1)
	csrr t0, mie                                            /* Required for MIE bit. */
	store_x t0, portMIE_OFFSET( sp )
#endif /* configNESTED_IRQ */

	portasmSAVE_ADDITIONAL_REGISTERS        /* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

	csrr a1, mepc
	store_x a1, 0( sp )                             /* Asynch so save unmodified exception return address. */

#if defined(configNESTED_IRQ) && (configNESTED_IRQ == 1)
	load_x  t1, Nested_IRQ_LEVEL
	beqz    t1, store_to_tcb                 /* Store sp to TCB if it is the first level */
	slli    t1, t1, 0x02                     /* t1 = t1 << 2 */
	la      t0, NestedIRQ_Context
	add     t0, t0, t1
	store_x sp, -4( t0 )                     /* Store sp to NestedIRQ_Context */
	j vec_timer_handle
store_to_tcb:
#endif /* configNESTED_IRQ */

	load_x  t0, pxCurrentTCB                 /* Load pxCurrentTCB. */
	store_x  sp, 0( t0 )                     /* Write sp to first TCB member. */
	load_x sp, xISRStackTop                 /* Switch to ISR stack before function call. */

vec_timer_handle:
	jal machine_timer_update

	jal xTaskIncrementTick
	beqz a0, processed_source               /* Don't switch context if incrementing tick didn't unblock a task. */
	j task_context_switch

vector_handler:
	csrrw t0, mscratch, t0
	portSAVE_BaseReg

	csrr t0, mstatus                                        /* Required for MPIE bit. */
	store_x t0, portMStatus_OFFSET( sp )

#if defined( configNESTED_IRQ ) && ( configNESTED_IRQ == 1)
	csrr t0, mie                                            /* Required for MIE bit. */
	store_x t0, portMIE_OFFSET( sp )
#endif /* configNESTED_IRQ */

	portasmSAVE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

	csrr a1, mepc
	store_x a1, 0( sp )                             /* Asynch so save unmodified exception return address. */

#if defined(configNESTED_IRQ) && (configNESTED_IRQ == 1)
	load_x  t1, Nested_IRQ_LEVEL
	beqz    t1, vec_store_to_tcb             /* Store sp to TCB if it is the first level */
	slli    t1, t1, 0x02                     /* t1 = t1 << 2 */
	la      t0, NestedIRQ_Context
	add     t0, t0, t1
	store_x sp, -4( t0 )                     /* Store sp to NestedIRQ_Context */
	j vec_handle
vec_store_to_tcb:
#endif /* configNESTED_IRQ */

	load_x  t0, pxCurrentTCB                 /* Load pxCurrentTCB. */
	store_x  sp, 0( t0 )                     /* Write sp to first TCB member. */
	load_x  sp, xISRStackTop                 /* Switch to ISR stack before function call. */

vec_handle:
	jal portasmHANDLE_INTERRUPT                     /* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
	j processed_source

non_vector_handler:
#endif /* configVECTORED_INTERRRUPT */

	portSAVE_BaseReg
#ifdef CONFIG_DEBUG_RPROBE
	Save_RegArry
#endif

	csrr t0, mstatus					/* Required for MPIE bit. */
	store_x t0, portMStatus_OFFSET( sp )

#if defined( configNESTED_IRQ ) && ( configNESTED_IRQ == 1)
	csrr t0, mie
	store_x t0, portMIE_OFFSET( sp )
#endif /* configNESTED_IRQ */

	portasmSAVE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to save any registers unique to the RISC-V implementation. */

	csrr a0, mcause
	csrr a1, mepc

test_if_asynchronous:
	srli a2, a0, __riscv_xlen - 1		/* MSB of mcause is 1 if handing an asynchronous interrupt - shift to LSB to clear other bits. */
	bne a2, x0, handle_asynchronous		/* Branch past interrupt handing if not asynchronous. */

handle_synchronous:
	addi a1, a1, 4						/* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
	store_x a1, 0( sp )					/* Save updated exception return address. */

test_if_environment_call:
	li t0, 11
	bgt a0, t0, is_exception			/* mcause > 11 */
	li t0, 8
	blt a0, t0, is_exception			/* mcause < 8 */
#if( portUSING_MPU_WRAPPERS == 1 )
	j environment_switch
#else
	j ecall_yield
#endif

#if( portUSING_MPU_WRAPPERS == 1 )
environment_switch:
	li t0, 4
	/* restore a0, a1 */
	load_x  x10, 10 * portWORD_SIZE( sp )	/* a0 */
	load_x  x11, 11 * portWORD_SIZE( sp )	/* a1 */
	bgt	a0,	t0,	ecall_end
	la t0, 1f
	slli	a0, a0, 2
	add t0, t0, a0
	jr t0
1:
	jal x0, ecall_yield
	jal x0, ecall_disable_interrupt
	jal x0, ecall_enable_interrupt
	jal x0, ecall_switch_to_machine
	jal x0, ecall_switch_to_user

ecall_disable_interrupt:
	/* Clear mpie */
	li a0, 0x80
	csrc mstatus, a0
	j ecall_mret

ecall_enable_interrupt:
	/* Set mpie */
	li a0, 0x80
	csrs mstatus, a0
	j ecall_mret

ecall_switch_to_machine:
	/* Set mpp */
	li a0, 0x1800
	csrs mstatus, a0
	j ecall_mret

ecall_switch_to_user:
	/* Clear mpp */
	li a0, 0x1800
	csrc mstatus, a0
	j ecall_mret

ecall_mret:
	/* update privilege_status value */
    csrr t0, mstatus
    li a0, 0x1800
    and t0, t0, a0
    srli t0, t0, 11
    la a0, privilege_status
    sw t0, 0(a0)

	/* Load mret with the address of the next instruction in the task to run next. */
	load_x  t0, 0( sp )
	csrw mepc, t0

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

	j _stack_pop

ecall_end:
    /* unauthorized ecall */
    j unrecoverable_error
#endif

ecall_yield:
	load_x  t0, pxCurrentTCB			/* Load pxCurrentTCB. */
	store_x  sp, 0( t0 )				/* Write sp to first TCB member. */
	load_x  sp, xISRStackTop				/* Switch to ISR stack before function call. */
	j task_context_switch

is_exception:
#ifdef CONFIG_DEBUG_RPROBE
	li      t0, 3
	beq     a0, t0, is_break
#endif
#if !defined(configFAULT_CRASH_DUMP) || (configFAULT_CRASH_DUMP == 0)
	csrr a0, mcause						/* For viewing in the debugger only. */
	csrr a1, mepc						/* For viewing in the debugger only */
	csrr a2, mstatus
	csrr a3, mtval
	load_x sp, xISRStackTop
	j vPortMExceptionHanler
#else
_crash_dump:							/* call crash_dump(uint32_t *pc, uint32_t *sp, uint32_t *reg) */

	csrr a0, mepc
	addi a1, sp, portCONTEXT_SIZE
	mv a2, sp

	store_x x0, 0*4(sp)      			// zero -> x0
	store_x a1, 2*4(sp)      			// store original sp -> x2
	store_x gp, 3*4(sp)      			// gp -> x3
	store_x tp, 4*4(sp)      			// tp -> x4

	load_x t1, xISRStackTop    			// Get exceptions stack  start addr, High addr
	la t2, xISRStack				    // Get exceptions stack  end addr, low addr
	addi t2, t2, 128					// Reserve 128Bytes stack space for crash_dump

	bge  sp, t1, not_isrstack
	blt  sp, t2, not_isrstack    		// if sp >= xISRStackTop, or sp < xISRStack,  jump to not_isrstack
	/*
	High addr	 |    0(unused)    | <--- xISRStack + portWORD_SIZE * (configISR_STACK_SIZE_WORDS - 1)
				 |   ...(unused)   |
				 |    0(unused)    |
				 |   	xxx   	   | <--- xISRStackTop, portBYTE_ALIGNMENT_MASK aligned --|
			| 	 |   	...   	   |                                                      | --> last time exception/interrupt stack content
			| 	 |      xxx        | <--- sp, before executing portSAVE_BaseReg	        --|
			| 	 |    mstatus      |
			| 	 |      x31        |
			| 	 |      ...        |
			v 	 |       x1        |
				 |       x0        | <--- sp, after executing portSAVE_BaseReg
				 |    0(empty)     |
				 |   ...(empty)    |
	Low addr	 |    0(empty)     | <--- xISRStack
	*/
	j	is_isrstack					    /* The sp is using xISRStack, which indicates that an exception occurred in the interrupt/exception.*/

not_isrstack:							/* If enter the exception handler at first time, sp needs to point to xISRStackTop */
	load_x sp, xISRStackTop

is_isrstack:
	la t0, crash_dump					/* Fixed the issue that the jump distance was too far */
	jalr t0
	// do crash dump again and again for user can see it anytime
	//j _exit

/* park CPU here after crash dump */
crash_dump_end:
	j crash_dump_end
#endif

unrecoverable_error:
	ebreak
//	wfi
	j unrecoverable_error

#ifdef CONFIG_DEBUG_RPROBE
is_break:
	load_x a2, RegArryBase
	load_x t0, pxCurrentTCB                        /* Load pxCurrentTCB. */
	store_x sp, 0( t0 )                            /* Write sp to first TCB member. */
	load_x sp, xISRStackTop

	jal     rprobe

	load_x  t1, pxCurrentTCB                        /* Load pxCurrentTCB. */
	load_x  sp, 0( t1 )                             /* Read sp from first TCB member. */

	portasmRESTORE_ADDITIONAL_REGISTERS
	load_x  t0, portMStatus_OFFSET( sp )
	csrw mstatus, t0

	portRESTORE_BaseReg
	Restore_RegArry

	addi sp, sp, portCONTEXT_SIZE
	mret
#endif

handle_asynchronous:

	store_x a1, 0( sp )					/* Asynch so save unmodified exception return address. */

#if defined(configNESTED_IRQ) && (configNESTED_IRQ == 1)
	load_x  t1, Nested_IRQ_LEVEL
	beqz    t1, nonvec_store_to_tcb          /* Store sp to TCB if it is the first level */
	slli    t1, t1, 0x02                     /* t1 = t1 << 2 */
	la      t0, NestedIRQ_Context
	add     t0, t0, t1
	store_x sp, -4( t0 )                     /* Store sp to NestedIRQ_Context */
	j nonvec_handle
nonvec_store_to_tcb:
#endif /* configNESTED_IRQ */

	load_x  t0, pxCurrentTCB                 /* Load pxCurrentTCB. */
	store_x sp, 0( t0 )                     /* Write sp to first TCB member. */
	load_x sp, xISRStackTop			/* Switch to ISR stack before function call. */

nonvec_handle:

#if( portasmHAS_CLINT != 0 )

	test_if_mtimer:						/* If there is a CLINT then the mtimer is used to generate the tick interrupt. */

		addi t0, x0, 1

		slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
		addi t1, t0, 7					/* 0x8000[]0007 == machine timer interrupt. */
		bne a0, t1, test_if_external_interrupt

		jal machine_timer_update

		jal xTaskIncrementTick
		beqz a0, processed_source		/* Don't switch context if incrementing tick didn't unblock a task. */
		j task_context_switch

	test_if_external_interrupt:

#endif /* portasmHAS_CLINT */

	jal portasmHANDLE_INTERRUPT			/* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that an external interrupt is pending. */
	j processed_source

task_context_switch:
	jal vTaskSwitchContext
	j processed_source

as_yet_unhandled:
	ebreak
	j as_yet_unhandled

processed_source:
#if defined(configNESTED_IRQ) && (configNESTED_IRQ == 1)
	load_x  t1, Nested_IRQ_LEVEL
	beqz    t1, restore_sp_from_tcb
	slli    t1, t1, 0x02                            /* t1 = t1 << 2 */
	la      t0, NestedIRQ_Context
	add     t1, t0, t1
	load_x  sp, -4(t1)
	j       restore_regs

restore_sp_from_tcb:
#endif /* configNESTED_IRQ */

	load_x  t1, pxCurrentTCB			/* Load pxCurrentTCB. */
	load_x  sp, 0( t1 )				/* Read sp from first TCB member. */

#if( portUSING_MPU_WRAPPERS == 1 )
	load_x a0, xPmpInfo
    li a1, 3	/* 3 PMPs is reserved for Kernel protection */
    sub a0, a0, a1	/* a0 = number of configurable PMP */
# if __riscv_xlen == 32
    addi a1, t1, 4	/* a1 =  pxCurrentTCB->xMPUSettings */
# elif __riscv_xlen == 64
    addi a1, t1, 8
# endif /* __riscv_xlen == 64 */
    jal vPortPmpSwitch
	fence.i
#endif
restore_regs:

	/* Load mret with the address of the next instruction in the task to run next. */
	load_x t0, 0( sp )
	csrw mepc, t0

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

#if defined(configNESTED_IRQ) && (configNESTED_IRQ == 1)
        load_x t0, portMIE_OFFSET( sp )
        csrw mie, t0
#endif /* configNESTED_IRQ */

	/* Load mstatus with the interrupt enable bits used by the task. */
	load_x t0, portMStatus_OFFSET( sp )
	csrw mstatus, t0						/* Required for MPIE bit. */

#if( portUSING_MPU_WRAPPERS == 1 )
    li a0, 0x1800
    and t0, t0, a0
    srli t0, t0, 11
    la a0, privilege_status
    sw t0, 0(a0)
#endif

_stack_pop:
	portRESTORE_BaseReg
	addi sp, sp, portCONTEXT_SIZE

	mret
.Lfreertos_risc_v_trap_handler_end0:
	.size freertos_risc_v_trap_handler, .Lfreertos_risc_v_trap_handler_end0-freertos_risc_v_trap_handler
/*-----------------------------------------------------------*/
.align 3
.type machine_timer_update, "function"
machine_timer_update:
	load_x t0, pullMachineTimerCompareRegister  /* Load address of compare register into t0. */
	load_x t1, pullNextTime  		/* Load the address of ullNextTime into t1. */

#if( __riscv_xlen == 32 )

/* Update the 64-bit mtimer compare match value in two 32-bit writes. */
#if defined( __riscv_32e ) && ( __riscv_32e == 1 )
	li t2, -1
	load_x s0, 0(t1)				/* Load the low word of ullNextTime into s0. */
	load_x s1, 4(t1)				/* Load the high word of ullNextTime into s1. */
	store_x t2, 0(t0)				/* Low word no smaller than old value. */
	store_x s1, 4(t0)				/* Store high word of ullNextTime into compare register.  No smaller than new value. */
	store_x s0, 0(t0)				/* Store low word of ullNextTime into compare register. */
	load_x t0, uxTimerIncrementsForOneTick	/* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
	add t2, t0, s0				/* Add the low word of ullNextTime to the timer increments for one tick (assumes timer increment for one tick fits in 32-bits). */
	sltu t0, t2, s0				/* See if the sum of low words overflowed (what about the zero case?). */
	add s0, s1, t0				/* Add overflow to high word of ullNextTime. */
	store_x t2, 0(t1)				/* Store new low word of ullNextTime. */
	store_x s0, 4(t1)				/* Store new high word of ullNextTime. */
#else
	li t4, -1
	load_x t2, 0(t1)				/* Load the low word of ullNextTime into t2. */
	load_x t3, 4(t1)				/* Load the high word of ullNextTime into t3. */
	store_x t4, 0(t0)				/* Low word no smaller than old value. */
	store_x t3, 4(t0)				/* Store high word of ullNextTime into compare register.  No smaller than new value. */
	store_x t2, 0(t0)				/* Store low word of ullNextTime into compare register. */
	load_x t0, uxTimerIncrementsForOneTick	/* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
	add t4, t0, t2				/* Add the low word of ullNextTime to the timer increments for one tick (assumes timer increment for one tick fits in 32-bits). */
	sltu t5, t4, t2				/* See if the sum of low words overflowed (what about the zero case?). */
	add t6, t3, t5				/* Add overflow to high word of ullNextTime. */
	store_x t4, 0(t1)				/* Store new low word of ullNextTime. */
	store_x t6, 4(t1)				/* Store new high word of ullNextTime. */
#endif
#endif /* __riscv_xlen == 32 */

#if( __riscv_xlen == 64 )

	/* Update the 64-bit mtimer compare match value. */
	ld t2, 0(t1)			 	/* Load ullNextTime into t2. */
	sd t2, 0(t0)				/* Store ullNextTime into compare register. */
	ld t0, uxTimerIncrementsForOneTick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to pullNextTime?). */
	add t4, t0, t2				/* Add ullNextTime to the timer increments for one tick. */
	sd t4, 0(t1)				/* Store ullNextTime. */

#endif /* __riscv_xlen == 64 */
	ret
.Lmachine_timer_update_end0:
	.size machine_timer_update, .Lmachine_timer_update_end0-machine_timer_update
/*-----------------------------------------------------------*/
.align 3
.type vPortHandleInterrupt, "function"
vPortHandleInterrupt:
     addi t0, x0, 0xf
     xori t0, t0, -1
     and  sp, sp, t0                 // stack pointer should be 128-bit aligned
     addi sp, sp, -16                // reserve space for ra in 128-bit aligned address
     sw   ra, 0(sp)
#if defined( configVECTORED_INTERRRUPT ) && ( configVECTORED_INTERRRUPT == 1 )
     csrr t2, mscratch
#else
     csrr t1, mcause                 // t1 = mcause
     lui  t2, 0x80000
     not  t2, t2                     // t2 = 0x7FFFFFFF
     and  t2, t1, t2                 // t2 = mcause & 0x7FFFFFFF
#endif
     add  a0, t2, x0
     la   a5, plic_interrupt_handler
     jalr a5

vPortHandleInterrupt_Exit:
     load_x  ra, 0(sp)
     addi sp, sp, 16
     ret
.LvPortHandleInterrupt_end0:
	.size vPortHandleInterrupt, .LvPortHandleInterrupt_end0-vPortHandleInterrupt

.align 3
.type xPortStartFirstTask, "function"
xPortStartFirstTask:

#if( portasmHAS_CLINT != 0 )
	/* If there is a clint then interrupts can branch directly to the FreeRTOS
	trap handler.  Otherwise the interrupt controller will need to be configured
	outside of this file. */
	la t0, freertos_risc_v_trap_handler
#if defined( configVECTORED_INTERRRUPT ) && ( configVECTORED_INTERRRUPT == 1 )
	ori t0, t0, 0x1
#endif
	csrw mtvec, t0
#endif /* portasmHAS_CLILNT */

	load_x  t2, pxCurrentTCB			/* Load pxCurrentTCB. */
	load_x  sp, 0( t2 )				 	/* Read sp from first TCB member. */

#if( portUSING_MPU_WRAPPERS == 1 )
	load_x  t1, portMStatus_OFFSET(sp)	/* mstatus */
    li a0, 0x1800
    and t1, t1, a0
    srli t1, t1, 11
    la a0, privilege_status
    sw t1, 0(a0)

	/* get number of configurable PMP */
	load_x a0, xPmpInfo	/* number of PMP */
    li a1, 3	/* 3 PMPs is reserved for Kernel protection */
    sub a0, a0, a1	/* a0 = xPmpInfo.nb_pmp - 3 */
# if __riscv_xlen == 32
    addi a1, t2, 4		/* a1 =  pxCurrentTCB->xMPUSettings */
# elif __riscv_xlen == 64
    addi a1, t2, 8
# endif /* __riscv_xlen == 64 */
	jal vPortPmpSwitch
# endif

	portasmRESTORE_ADDITIONAL_REGISTERS	/* Defined in freertos_risc_v_chip_specific_extensions.h to restore any registers unique to the RISC-V implementation. */

	portRESTORE_BaseReg

	load_x  t0, portMStatus_OFFSET( sp )    /* mstatus */
	ori    t0, t0, 0x08
	csrrw  x0, mstatus, t0                                  /* Interrupts enabled from here! */
	load_x  t0, 5 * portWORD_SIZE( sp )

	load_x  x1, 0( sp ) /* Note for starting the scheduler the exception return address is used as the function return address. */
	addi	sp, sp, portCONTEXT_SIZE
	ret
.LxPortStartFirstTask_end0:
	.size xPortStartFirstTask, .LxPortStartFirstTask_end0-xPortStartFirstTask
/*-----------------------------------------------------------*/

/*
 * Unlike other ports pxPortInitialiseStack() is written in assembly code as it
 * needs access to the portasmADDITIONAL_CONTEXT_SIZE constant.  The prototype
 * for the function is as per the other ports:
 * StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
 *
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register		ABI Name	Description						Saver
 * x0			zero		Hard-wired zero					-
 * x1			ra			Return address					Caller
 * x2			sp			Stack pointer					Callee
 * x3			gp			Global pointer					-
 * x4			tp			Thread pointer					-
 * x5-7			t0-2		Temporaries						Caller
 * x8			s0/fp		Saved register/Frame pointer	Callee
 * x9			s1			Saved register					Callee
 * x10-11		a0-1		Function Arguments/return values Caller
 * x12-17		a2-7		Function arguments				Caller
 * x18-27		s2-11		Saved registers					Callee
 * x28-31		t3-6		Temporaries						Caller
 *
 * The RISC-V context is saved t FreeRTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * x4
 * x3
 * x2
 * portTASK_RETURN_ADDRESS
 * [chip specific registers go here]
 * pxCode
 */
.align 3
.type pxPortInitialiseStack, "function"
pxPortInitialiseStack:
	csrr t0, mstatus					/* Obtain current mstatus value. */
	andi t0, t0, ~0x8					/* Ensure interrupts are disabled when the stack is restored within an ISR.  Required when a task is
											created after the schedulre has been started, otherwise interrupts would be disabled anyway. */
	addi t1, x0, 0x188					/* Generate the value 0x1880, which are the MPIE and MPP bits to set in mstatus. */
	slli t1, t1, 4
	or t0, t0, t1						/* Set MPIE and MPP bits in mstatus value. */

#if defined(portUSING_MPU_WRAPPERS) && portUSING_MPU_WRAPPERS
	li t1,1
	beq a3, t1, 1f						/* a3 = xRunPrivileged */

	li t1, 0x1800
	xor t0, t0, t1						/* set to u-mode*/
#endif
1:
	addi a0, a0, -portWORD_SIZE			        /* a0 = StackTop */
	store_x t0, 0(a0)					/* mstatus onto the stack. */
#if defined( __riscv_flen )
	addi a0, a0, -(55 * portWORD_SIZE)
#else
#if defined( __riscv_32e ) && ( __riscv_32e == 1 )
	addi a0, a0, -(6 * portWORD_SIZE)	/* Skip Space for registers x11-x15 to make a0 = stack for X10(a0) */
#else
	addi a0, a0, -(22 * portWORD_SIZE)	/* Skip Space for registers x11-x31 to make a0 = stack for X10(a0) */
#endif
#endif
	store_x a2, 0(a0)					/* Task parameters (pvParameters parameter) goes into register X10/a0 on the stack. */
	addi a0, a0, -(9 * portWORD_SIZE)	/* Skip Space for registers x2-x9 to make a0 = stack for X1(RA) */
	store_x x0, 0(a0)					/* Return address onto the stack, could be portTASK_RETURN_ADDRESS */
	addi t0, x0, portasmADDITIONAL_CONTEXT_SIZE /* The number of chip specific additional registers. */
chip_specific_stack_frame:				/* First add any chip specific registers to the stack frame being created. */
	beq t0, x0, 1f						/* No more chip specific registers to save. */
	addi a0, a0, -portWORD_SIZE			/* Make space for chip specific register. */
	store_x x0, 0(a0)					/* Give the chip specific register an initial value of zero. */
	addi t0, t0, -1						/* Decrement the count of chip specific registers remaining. */
	j chip_specific_stack_frame			/* Until no more chip specific registers. */
1:
	addi a0, a0, -portWORD_SIZE
	store_x a1, 0(a0)					/* mret value (pxCode parameter) onto the stack. */
	ret
.LpxPortInitialiseStack_end0:
	.size pxPortInitialiseStack, .LpxPortInitialiseStack_end0-pxPortInitialiseStack
/*-----------------------------------------------------------*/

/* void vPortBackupRegs( BaseType_t *pxGPRBaseAddr, BaseType_t *pxCSRBaseAddr )
 * Backup Registers.
 * Parameter pxPostFunction is stored to the context for power off use, so user
 * can return back the right place with it.
 */
.global vPortBackupRegs
.type   vPortBackupRegs, %function
vPortBackupRegs:
	store_x x1, 1 * portWORD_SIZE( a0 )        /* ra */
	store_x x2, 2 * portWORD_SIZE( a0 )        /* sp */
	store_x x3, 3 * portWORD_SIZE( a0 )        /* gp */
	store_x x4, 4 * portWORD_SIZE( a0 )        /* tp */
	store_x x5, 5 * portWORD_SIZE( a0 )        /* t0 */
	store_x x6, 6 * portWORD_SIZE( a0 )        /* t1 */
	store_x x7, 7 * portWORD_SIZE( a0 )        /* t2 */
	store_x x8, 8 * portWORD_SIZE( a0 )        /* s0/fp */
	store_x x9, 9 * portWORD_SIZE( a0 )        /* s1 */
	store_x x10, 10 * portWORD_SIZE( a0 )      /* a0 */
	store_x x11, 11 * portWORD_SIZE( a0 )      /* a1 */
	store_x x12, 12 * portWORD_SIZE( a0 )      /* a2 */
	store_x x13, 13 * portWORD_SIZE( a0 )      /* a3 */
	store_x x14, 14 * portWORD_SIZE( a0 )      /* a4 */
	store_x x15, 15 * portWORD_SIZE( a0 )      /* a5 */
#if !defined( __riscv_32e ) || ( __riscv_32e == 0 )
	store_x x16, 16 * portWORD_SIZE( a0 )      /* a6 */
	store_x x17, 17 * portWORD_SIZE( a0 )      /* a7 */
	store_x x18, 18 * portWORD_SIZE( a0 )      /* s2 */
	store_x x19, 19 * portWORD_SIZE( a0 )      /* s3 */
	store_x x20, 20 * portWORD_SIZE( a0 )      /* s4 */
	store_x x21, 21 * portWORD_SIZE( a0 )      /* s5 */
	store_x x22, 22 * portWORD_SIZE( a0 )      /* s6 */
	store_x x23, 23 * portWORD_SIZE( a0 )      /* s7 */
	store_x x24, 24 * portWORD_SIZE( a0 )      /* s8 */
	store_x x25, 25 * portWORD_SIZE( a0 )      /* s9 */
	store_x x26, 26 * portWORD_SIZE( a0 )      /* s10 */
	store_x x27, 27 * portWORD_SIZE( a0 )      /* s11 */
	store_x x28, 28 * portWORD_SIZE( a0 )      /* t3 */
	store_x x29, 29 * portWORD_SIZE( a0 )      /* t4 */
	store_x x30, 30 * portWORD_SIZE( a0 )      /* t5 */
	store_x x31, 31 * portWORD_SIZE( a0 )      /* t6 */
#endif /* #if !defined( __riscv_32e ) || ( __riscv_32e == 0 ) */
	csrr t0, mstatus                           /* read mstatus to t0 */
	store_x t0, 0 * portWORD_SIZE( a1 )
	csrr t0, mie                               /* Required for MIE bit. */
	store_x t0, 1 * portWORD_SIZE( a1 )
	csrr t0, mtvec                             /* Required for trap base */
	store_x t0, 2 * portWORD_SIZE( a1 )
	store_x ra, 3 * portWORD_SIZE( a1 )
	jr ra

#if ( configENABLE_FPU == 1 )
.global vPortBackupfloatRegs
.type   vPortBackupfloatRegs, %function
vPortBackupfloatRegs:
	csrr    t0, mstatus
	li      t1, 0x6000 /* get mstatus.FS */
	and     t0,t0,t1
	bne     t0,t1,1f /* save float point registers if dirty */
	store_f f0, 0 * portWORD_SIZE( a0 )
	store_f f1, 1 * portWORD_SIZE( a0 )
	store_f f2, 2 * portWORD_SIZE( a0 )
	store_f f3, 3 * portWORD_SIZE( a0 )
	store_f f4, 4 * portWORD_SIZE( a0 )
	store_f f5, 5 * portWORD_SIZE( a0 )
	store_f f6, 6 * portWORD_SIZE( a0 )
	store_f f7, 7 * portWORD_SIZE( a0 )
	store_f f8, 8 * portWORD_SIZE( a0 )
	store_f f9, 9 * portWORD_SIZE( a0 )
	store_f f10, 10 * portWORD_SIZE( a0 )
	store_f f11, 11 * portWORD_SIZE( a0 )
	store_f f12, 12 * portWORD_SIZE( a0 )
	store_f f13, 13 * portWORD_SIZE( a0 )
	store_f f14, 14 * portWORD_SIZE( a0 )
	store_f f15, 15 * portWORD_SIZE( a0 )
	store_f f16, 16 * portWORD_SIZE( a0 )
	store_f f17, 17 * portWORD_SIZE( a0 )
	store_f f18, 18 * portWORD_SIZE( a0 )
	store_f f19, 19 * portWORD_SIZE( a0 )
	store_f f20, 20 * portWORD_SIZE( a0 )
	store_f f21, 21 * portWORD_SIZE( a0 )
	store_f f22, 22 * portWORD_SIZE( a0 )
	store_f f23, 23 * portWORD_SIZE( a0 )
	store_f f24, 24 * portWORD_SIZE( a0 )
	store_f f25, 25 * portWORD_SIZE( a0 )
	store_f f26, 26 * portWORD_SIZE( a0 )
	store_f f27, 27 * portWORD_SIZE( a0 )
	store_f f28, 28 * portWORD_SIZE( a0 )
	store_f f29, 29 * portWORD_SIZE( a0 )
	store_f f30, 30 * portWORD_SIZE( a0 )
	store_f f31, 31 * portWORD_SIZE( a0 )
	frcsr   t0
	store_x t0, 0 * portWORD_SIZE( a1 )
1:
	jr ra
#endif

/*-----------------------------------------------------------*/

/* void vPortBackupRegs( BaseType_t *pxGPRBaseAddr, BaseType_t *pxCSRBaseAddr )
 * Restore GPR
 */
.global vPortRestoreRegs
.type   vPortRestoreRegs, %function
vPortRestoreRegs:
	/* ra is not restored */
	load_x x2, 2 * portWORD_SIZE( a0 )         /* sp */
	load_x x3, 3 * portWORD_SIZE( a0 )         /* gp */
	load_x x4, 4 * portWORD_SIZE( a0 )         /* tp */
	load_x x5, 5 * portWORD_SIZE( a0 )         /* t0 */
	load_x x5, 5 * portWORD_SIZE( a0 )         /* t0 */
	load_x x6, 6 * portWORD_SIZE( a0 )         /* t1 */
	load_x x7, 7 * portWORD_SIZE( a0 )         /* t2 */
	load_x x8, 8 * portWORD_SIZE( a0 )         /* s0/fp */
	load_x x9, 9 * portWORD_SIZE( a0 )         /* s1 */
	load_x x10, 10 * portWORD_SIZE( a0 )       /* a0 */
	load_x x11, 11 * portWORD_SIZE( a0 )       /* a1 */
	load_x x12, 12 * portWORD_SIZE( a0 )       /* a2 */
	load_x x13, 13 * portWORD_SIZE( a0 )       /* a3 */
	load_x x14, 14 * portWORD_SIZE( a0 )       /* a4 */
	load_x x15, 15 * portWORD_SIZE( a0 )       /* a5 */
#if !defined( __riscv_32e ) || ( __riscv_32e == 0 )
	load_x x16, 16 * portWORD_SIZE( a0 )       /* a6 */
	load_x x17, 17 * portWORD_SIZE( a0 )       /* a7 */
	load_x x18, 18 * portWORD_SIZE( a0 )       /* s2 */
	load_x x19, 19 * portWORD_SIZE( a0 )       /* s3 */
	load_x x20, 20 * portWORD_SIZE( a0 )       /* s4 */
	load_x x21, 21 * portWORD_SIZE( a0 )       /* s5 */
	load_x x22, 22 * portWORD_SIZE( a0 )       /* s6 */
	load_x x23, 23 * portWORD_SIZE( a0 )       /* s7 */
	load_x x24, 24 * portWORD_SIZE( a0 )       /* s8 */
	load_x x25, 25 * portWORD_SIZE( a0 )       /* s9 */
	load_x x26, 26 * portWORD_SIZE( a0 )       /* s10 */
	load_x x27, 27 * portWORD_SIZE( a0 )       /* s11 */
	load_x x28, 28 * portWORD_SIZE( a0 )       /* t3 */
	load_x x29, 29 * portWORD_SIZE( a0 )       /* t4 */
	load_x x30, 30 * portWORD_SIZE( a0 )       /* t5 */
	load_x x31, 31 * portWORD_SIZE( a0 )       /* t6 */
#endif /* #if !defined( __riscv_32e ) || ( __riscv_32e == 0 ) */
	/* Load mstatus with the interrupt enable bits used by the task. */
	load_x ra, 0 * portWORD_SIZE( a1 )
	csrw mstatus, ra
	load_x ra, 1 * portWORD_SIZE( a1 )
	csrw mie, ra
	load_x ra, 2 * portWORD_SIZE( a1 )
	csrw mtvec, ra
	load_x ra, 3 * portWORD_SIZE( a1 )
	jr ra

#if ( configENABLE_FPU == 1 )
.global vPortRestorefloatRegs
.type   vPortfloatRestorefloatRegs, %function
vPortRestorefloatRegs:
	load_x  t0, 0 * portWORD_SIZE( a2 )        /* get mstatus */
	li      t1, 0x6000                         /* mstatus.FS */
	and     t0,t0,t1
	bne     t0,t1,1f /* restore float point registers if dirty */
	load_f f0, 0 * portWORD_SIZE( a0 )
	load_f f1, 1 * portWORD_SIZE( a0 )
	load_f f2, 2 * portWORD_SIZE( a0 )
	load_f f3, 3 * portWORD_SIZE( a0 )
	load_f f4, 4 * portWORD_SIZE( a0 )
	load_f f5, 5 * portWORD_SIZE( a0 )
	load_f f6, 6 * portWORD_SIZE( a0 )
	load_f f7, 7 * portWORD_SIZE( a0 )
	load_f f8, 8 * portWORD_SIZE( a0 )
	load_f f9, 9 * portWORD_SIZE( a0 )
	load_f f10, 10 * portWORD_SIZE( a0 )
	load_f f11, 11 * portWORD_SIZE( a0 )
	load_f f12, 12 * portWORD_SIZE( a0 )
	load_f f13, 13 * portWORD_SIZE( a0 )
	load_f f14, 14 * portWORD_SIZE( a0 )
	load_f f15, 15 * portWORD_SIZE( a0 )
	load_f f16, 16 * portWORD_SIZE( a0 )
	load_f f17, 17 * portWORD_SIZE( a0 )
	load_f f18, 18 * portWORD_SIZE( a0 )
	load_f f19, 19 * portWORD_SIZE( a0 )
	load_f f20, 20 * portWORD_SIZE( a0 )
	load_f f21, 21 * portWORD_SIZE( a0 )
	load_f f22, 22 * portWORD_SIZE( a0 )
	load_f f23, 23 * portWORD_SIZE( a0 )
	load_f f24, 24 * portWORD_SIZE( a0 )
	load_f f25, 25 * portWORD_SIZE( a0 )
	load_f f26, 26 * portWORD_SIZE( a0 )
	load_f f27, 27 * portWORD_SIZE( a0 )
	load_f f28, 28 * portWORD_SIZE( a0 )
	load_f f29, 29 * portWORD_SIZE( a0 )
	load_f f30, 30 * portWORD_SIZE( a0 )
	load_f f31, 31 * portWORD_SIZE( a0 )
	load_x t0, 0 * portWORD_SIZE( a1 )
	fscsr t0
1:
	jr ra
#endif

/*-----------------------------------------------------------*/

/* void vPortBackupPmp( BaseType_t *pxPmpBaseAddr )
 * Restore pmp related csr, it need to be done in privilege mode.
 */
.global vPortBackupPmp
.type   vPortBackupPmp, %function
vPortBackupPmp:
	csrr t0, pmpaddr0
	store_x t0, 0 * portWORD_SIZE( a0 )        /* pmpaddr0 */
	csrr t0, pmpaddr1
	store_x t0, 1 * portWORD_SIZE( a0 )        /* pmpaddr1 */
	csrr t0, pmpaddr2
	store_x t0, 2 * portWORD_SIZE( a0 )        /* pmpaddr2 */
	csrr t0, pmpaddr3
	store_x t0, 3 * portWORD_SIZE( a0 )        /* pmpaddr3 */
	csrr t0, pmpaddr4
	store_x t0, 4 * portWORD_SIZE( a0 )        /* pmpaddr4 */
	csrr t0, pmpaddr5
	store_x t0, 5 * portWORD_SIZE( a0 )        /* pmpaddr5 */
	csrr t0, pmpaddr6
	store_x t0, 6 * portWORD_SIZE( a0 )        /* pmpaddr6 */
	csrr t0, pmpaddr7
	store_x t0, 7 * portWORD_SIZE( a0 )        /* pmpaddr7 */
	csrr t0, pmpaddr8
	store_x t0, 8 * portWORD_SIZE( a0 )        /* pmpaddr8 */
	csrr t0, pmpaddr9
	store_x t0, 9 * portWORD_SIZE( a0 )        /* pmpaddr9 */
	csrr t0, pmpaddr10
	store_x t0, 10 * portWORD_SIZE( a0 )       /* pmpaddr10 */
	csrr t0, pmpaddr11
	store_x t0, 11 * portWORD_SIZE( a0 )       /* pmpaddr11 */
	csrr t0, pmpaddr12
	store_x t0, 12 * portWORD_SIZE( a0 )       /* pmpaddr12 */
	csrr t0, pmpaddr13
	store_x t0, 13 * portWORD_SIZE( a0 )       /* pmpaddr13 */
	csrr t0, pmpaddr14
	store_x t0, 14 * portWORD_SIZE( a0 )       /* pmpaddr14 */
	csrr t0, pmpaddr15
	store_x t0, 15 * portWORD_SIZE( a0 )       /* pmpaddr15 */
	csrr t0, pmpcfg0
	store_x t0, 16 * portWORD_SIZE( a0 )       /* pmpcfg0 */
	csrr t0, pmpcfg2
	store_x t0, 17 * portWORD_SIZE( a0 )       /* pmpcfg2 */
#if __riscv_xlen == 32
	csrr t0, pmpcfg1
	store_x t0, 18 * portWORD_SIZE( a0 )       /* pmpcfg1 */
	csrr t0, pmpcfg3
	store_x t0, 19 * portWORD_SIZE( a0 )       /* pmpcfg3 */
#endif
	jr ra
/*-----------------------------------------------------------*/

/* void vPortRestorePmp( BaseType_t *pxPmpBaseAddr )
 * Restore GPR
 */
.global vPortRestorePmp
.type   vPortRestorePmp, %function
vPortRestorePmp:
	load_x t0, 0 * portWORD_SIZE( a0 )        /* pmpaddr0 */
	csrw pmpaddr0, t0
	load_x t0, 1 * portWORD_SIZE( a0 )        /* pmpaddr1 */
	csrw pmpaddr1, t0
	load_x t0, 2 * portWORD_SIZE( a0 )        /* pmpaddr2 */
	csrw pmpaddr2, t0
	load_x t0, 3 * portWORD_SIZE( a0 )        /* pmpaddr3 */
	csrw pmpaddr3, t0
	load_x t0, 4 * portWORD_SIZE( a0 )        /* pmpaddr4 */
	csrw pmpaddr4, t0
	load_x t0, 5 * portWORD_SIZE( a0 )        /* pmpaddr5 */
	csrw pmpaddr5, t0
	load_x t0, 6 * portWORD_SIZE( a0 )        /* pmpaddr6 */
	csrw pmpaddr6, t0
	load_x t0, 7 * portWORD_SIZE( a0 )        /* pmpaddr7 */
	csrw pmpaddr7, t0
	load_x t0, 8 * portWORD_SIZE( a0 )        /* pmpaddr8 */
	csrw pmpaddr8, t0
	load_x t0, 9 * portWORD_SIZE( a0 )        /* pmpaddr9 */
	csrw pmpaddr9, t0
	load_x t0, 10 * portWORD_SIZE( a0 )        /* pmpaddr10 */
	csrw pmpaddr10, t0
	load_x t0, 11 * portWORD_SIZE( a0 )        /* pmpaddr11 */
	csrw pmpaddr11, t0
	load_x t0, 12 * portWORD_SIZE( a0 )        /* pmpaddr12 */
	csrw pmpaddr12, t0
	load_x t0, 13 * portWORD_SIZE( a0 )        /* pmpaddr13 */
	csrw pmpaddr13, t0
	load_x t0, 14 * portWORD_SIZE( a0 )        /* pmpaddr14 */
	csrw pmpaddr14, t0
	load_x t0, 15 * portWORD_SIZE( a0 )        /* pmpaddr15 */
	csrw pmpaddr15, t0
	load_x t0, 16 * portWORD_SIZE( a0 )        /* pmpcfg0 */
	csrw pmpcfg0, t0
	load_x t0, 17 * portWORD_SIZE( a0 )        /* pmpcfg2 */
	csrw pmpcfg2, t0
#if __riscv_xlen == 32
	load_x t0, 18 * portWORD_SIZE( a0 )        /* pmpcfg1 */
	csrw pmpcfg1, t0
	load_x t0, 19 * portWORD_SIZE( a0 )        /* pmpcfg3 */
	csrw pmpcfg3, t0
#endif
	jr ra
