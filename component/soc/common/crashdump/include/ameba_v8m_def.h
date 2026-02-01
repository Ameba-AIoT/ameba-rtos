/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_V8M_DEF_H_
#define _AMEBA_V8M_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ============Configure as needed start ============*/
/* Dump runs for number of being displayed */
#define CONFIG_DEBUG_CRASH_DUMP_RUNS 1

/* Dump depth for depth of stack in the event of exception */
#define CONFIG_STACK_DUMP_DEPTH 	 128
#define MIN_DUMP_DEPTH  			 0x80
#define MAX_DUMP_DEPTH 				 0x4000
#define DUMP_STACK_MIN_DEPTH(depth)  ((depth >= MIN_DUMP_DEPTH) && (depth <= MAX_DUMP_DEPTH) ? depth:MIN_DUMP_DEPTH)
/* The number displayed per row*/
#define CONFIG_DISPLAY_NUMBER		 4

/* ============= Configure end ============*/

/* system handler control and state register */
#define CMB_SYSHND_CTRL                (*(volatile unsigned int*)  (0xE000ED24u))

/* memory management fault status register */
#define CMB_NVIC_MFSR                  (*(volatile unsigned char*) (0xE000ED28u))

/* bus fault status register */
#define CMB_NVIC_BFSR                  (*(volatile unsigned char*) (0xE000ED29u))

/* usage fault status register */
#define CMB_NVIC_UFSR                  (*(volatile unsigned short*)(0xE000ED2Au))

/* hard fault status register */
#define CMB_NVIC_HFSR                  (*(volatile unsigned int*)  (0xE000ED2Cu))

/* debug fault status register */
#define CMB_NVIC_DFSR                  (*(volatile unsigned short*)(0xE000ED30u))

/* memory management fault address register */
#define CMB_NVIC_MMAR                  (*(volatile unsigned int*)  (0xE000ED34u))

/* bus fault manage address register */
#define CMB_NVIC_BFAR                  (*(volatile unsigned int*)  (0xE000ED38u))

/* auxiliary fault status register */
#define CMB_NVIC_AFSR                  (*(volatile unsigned short*)(0xE000ED3Cu))

/* secure fault status register */
#define CMB_NVIC_SFSR                  (*(volatile unsigned int*)  (0xE000EDE4u))

/* secure fault address register */
#define CMB_NVIC_SFAR                  (*(volatile unsigned int*)  (0xE000EDE8u))

/**
 * Cortex-M fault registers
 */
struct cmb_hard_fault_regs {
	struct {
		unsigned int r0;                     // Register R0
		unsigned int r1;                     // Register R1
		unsigned int r2;                     // Register R2
		unsigned int r3;                     // Register R3
		unsigned int r4;                     // Register R4
		unsigned int r5;                     // Register R5
		unsigned int r6;                     // Register R6
		unsigned int r7;                     // Register R7
		unsigned int r8;                     // Register R8
		unsigned int r9;                     // Register R9
		unsigned int r10;                    // Register R10
		unsigned int r11;                    // Register R11
		unsigned int r12;                    // Register R12
		unsigned int lr;                     // Link register
		unsigned int pc;                     // Program counter
		union {
			unsigned int value;
			struct {
				unsigned int IPSR : 9;           // Interrupt Program Status register (IPSR)
				unsigned int EPSR : 18;          // Execution Program Status register (EPSR)
				unsigned int APSR : 5;           // Application Program Status register (APSR)
			} bits;
		} psr;                               // Program status register.
		unsigned int excr;
	} saved;

	union {
		unsigned int value;
		struct {
			unsigned int MEMFAULTACT    : 1;   // Read as 1 if memory management fault is active
			unsigned int BUSFAULTACT    : 1;   // Read as 1 if bus fault exception is active
#ifdef CONFIG_ARM_CORE_CM4
			unsigned int HARDFAULTACT   : 1;   // Read as 1 if hardfault is active
#else
			unsigned int UnusedBits1    : 1;
#endif
			unsigned int USGFAULTACT    : 1;   // Read as 1 if usage fault exception is active
#ifdef CONFIG_ARM_CORE_CM4
			unsigned int SECUREFAULTACT : 1;   // Read as 1 if secure fault exception is active
			unsigned int NMIACT         : 1;   // Read as 1 if NMI exception is active
			unsigned int UnusedBits2    : 1;
#else
			unsigned int UnusedBits2    : 3;
#endif
			unsigned int SVCALLACT      : 1;   // Read as 1 if SVC exception is active
			unsigned int MONITORACT     : 1;   // Read as 1 if debug monitor exception is active
			unsigned int UnusedBits3    : 1;
			unsigned int PENDSVACT      : 1;   // Read as 1 if PendSV exception is active
			unsigned int SYSTICKACT     : 1;   // Read as 1 if SYSTICK exception is active
			unsigned int USGFAULTPENDED : 1;   // Usage fault pended; usage fault started but was replaced by a higher-priority exception
			unsigned int MEMFAULTPENDED : 1;   // Memory management fault pended; memory management fault started but was replaced by a higher-priority exception
			unsigned int BUSFAULTPENDED : 1;   // Bus fault pended; bus fault handler was started but was replaced by a higher-priority exception
			unsigned int SVCALLPENDED   : 1;   // SVC pended; SVC was started but was replaced by a higher-priority exception
			unsigned int MEMFAULTENA    : 1;   // Memory management fault handler enable
			unsigned int BUSFAULTENA    : 1;   // Bus fault handler enable
			unsigned int USGFAULTENA    : 1;   // Usage fault handler enable
#ifdef CONFIG_ARM_CORE_CM4
			unsigned int SECUREFAULTENA : 1;   // Secure fault handler enable
			unsigned int SECUREFAULTPENDED : 1;   // Secure fault pended; Secure fault handler was started but was replaced by a higher-priority exception
			unsigned int HARDFAULTPENDED   : 1;   // Hard fault pended; Hard fault handler was started but was replaced by a higher-priority exception
#else
			// None
#endif
		} bits;
	} syshndctrl;                          // System Handler Control and State Register (0xE000ED24)

	union {
		unsigned char value;
		struct {
			unsigned char IACCVIOL    : 1;     // Instruction access violation
			unsigned char DACCVIOL    : 1;     // Data access violation
			unsigned char UnusedBits  : 1;
			unsigned char MUNSTKERR   : 1;     // Unstacking error
			unsigned char MSTKERR     : 1;     // Stacking error
			unsigned char MLSPERR     : 1;     // Floating-point lazy state preservation (M4/M7)
			unsigned char UnusedBits2 : 1;
			unsigned char MMARVALID   : 1;     // Indicates the MMAR is valid
		} bits;
	} mfsr;                                // Memory Management Fault Status Register (0xE000ED28)
	unsigned int mmar;                     // Memory Management Fault Address Register (0xE000ED34)

	union {
		unsigned char value;
		struct {
			unsigned char IBUSERR    : 1;      // Instruction access violation
			unsigned char PRECISERR  : 1;      // Precise data access violation
			unsigned char IMPREISERR : 1;      // Imprecise data access violation
			unsigned char UNSTKERR   : 1;      // Unstacking error
			unsigned char STKERR     : 1;      // Stacking error
			unsigned char LSPERR     : 1;      // Floating-point lazy state preservation (M4/M7)
			unsigned char UnusedBits : 1;
			unsigned char BFARVALID  : 1;      // Indicates BFAR is valid
		} bits;
	} bfsr;                                // Bus Fault Status Register (0xE000ED29)
	unsigned int bfar;                     // Bus Fault Manage Address Register (0xE000ED38)

	union {
		unsigned short value;
		struct {
			unsigned short UNDEFINSTR : 1;     // Attempts to execute an undefined instruction
			unsigned short INVSTATE   : 1;     // Attempts to switch to an invalid state (e.g., ARM)
			unsigned short INVPC      : 1;     // Attempts to do an exception with a bad value in the EXC_RETURN number
			unsigned short NOCP       : 1;     // Attempts to execute a coprocessor instruction
#ifdef CONFIG_ARM_CORE_CM4
			unsigned short STKOF      : 1;     // Indicates a stack overflow error has occured
			unsigned short UnusedBits : 3;
#else
			unsigned short UnusedBits : 4;
#endif
			unsigned short UNALIGNED  : 1;     // Indicates that an unaligned access fault has taken place
			unsigned short DIVBYZERO0 : 1;     // Indicates a divide by zero has taken place (can be set only if DIV_0_TRP is set)
		} bits;
	} ufsr;                                // Usage Fault Status Register (0xE000ED2A)

	union {
		unsigned int value;
		struct {
			unsigned int UnusedBits  : 1;
			unsigned int VECTBL      : 1;      // Indicates hard fault is caused by failed vector fetch
			unsigned int UnusedBits2 : 28;
			unsigned int FORCED      : 1;      // Indicates hard fault is taken because of bus fault/memory management fault/usage fault
			unsigned int DEBUGEVT    : 1;      // Indicates hard fault is triggered by debug event
		} bits;
	} hfsr;                                // Hard Fault Status Register (0xE000ED2C)

	union {
		unsigned int value;
		struct {
			unsigned int HALTED   : 1;         // Halt requested in NVIC
			unsigned int BKPT     : 1;         // BKPT instruction executed
			unsigned int DWTTRAP  : 1;         // DWT match occurred
			unsigned int VCATCH   : 1;         // Vector fetch occurred
			unsigned int EXTERNAL : 1;         // EDBGRQ signal asserted
		} bits;
	} dfsr;                                // Debug Fault Status Register (0xE000ED30)

	unsigned int afsr;                     // Auxiliary Fault Status Register (0xE000ED3C), Vendor controlled (optional)
	union {
		unsigned int value;
		struct {
			unsigned int INVEP     : 1;         // Invalid entry point
			unsigned int INVIS     : 1;         // Invalid integrity signature flag
			unsigned int INVER     : 1;         // Invalid exception return flag
			unsigned int AUVIOL    : 1;         // Attribution unit violation flag
			unsigned int INVTRAN   : 1;         // Invalid transition fla
			unsigned int LSPERR    : 1;         // Lazy state preservation error flag
			unsigned int SFARVALID : 1;         // Secure fault address valid
			unsigned int LSERR     : 1;         // Lazy state error flag
		} bits;
	} sfsr;                                 // Secure Fault Status Register (0xE000EDE4)
	unsigned int sfar;                      // Secure Fault Address Register (0xE000EDE8)
};

// ARM v8M register
enum {
	REG_EXCR = 0,
	REG_R4,
	REG_R5,
	REG_R6,
	REG_R7,
	REG_R8,
	REG_R9,
	REG_R10,
	REG_R11,
	REG_R0,
	REG_R1,
	REG_R2,
	REG_R3,
	REG_R12,
	REG_LR,
	REG_EPC,
	REG_xPSR,
	REG_END
};

/* assert for developer. */
#define CMB_ASSERT(EXPR)                                                       \
if (!(EXPR))                                                                   \
{                                                                              \
    cmb_println("(%s) has assert failed at %s.", #EXPR, __FUNCTION__);         \
    while (1);                                                                 \
}

#ifdef __cplusplus
}
#endif

#endif /* _CMB_DEF_H_ */
