/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_V8M_CRASHDUMP_H_
#define _AMEBA_V8M_CRASHDUMP_H_

#define _get_MMFSR(value)  ((unsigned char)(((value >> 0 ) & 0xFF)))

#define _get_BFSR(value)   ((unsigned char)(((value >> 8 ) & 0xFF)))

#define _get_UFSR(value)   ((unsigned short)(((value >> 16 ) & 0xFFFF)))
/**
 * Cortex-M fault registers
 */
struct cmb_hard_fault_regs {

	unsigned int pc;                     		// Program counter

	union {
		unsigned int value;
		struct {
			unsigned int MEMFAULTACT    : 1;   // Read as 1 if memory management fault is active
			unsigned int BUSFAULTACT    : 1;   // Read as 1 if bus fault exception is active

			unsigned int HARDFAULTACT   : 1;   // Read as 1 if hardfault is active
			unsigned int USGFAULTACT    : 1;   // Read as 1 if usage fault exception is active
			unsigned int SECUREFAULTACT : 1;   // Read as 1 if secure fault exception is active
			unsigned int NMIACT         : 1;   // Read as 1 if NMI exception is active
			unsigned int UnusedBits2    : 1;
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
			unsigned int SECUREFAULTENA : 1;   // Secure fault handler enable
			unsigned int SECUREFAULTPENDED : 1;   // Secure fault pended; Secure fault handler was started but was replaced by a higher-priority exception
			unsigned int HARDFAULTPENDED   : 1;   // Hard fault pended; Hard fault handler was started but was replaced by a higher-priority exception
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
			unsigned short STKOF      : 1;     // Indicates a stack overflow error has occured
			unsigned short UnusedBits : 3;
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

/***
*  @brief	Get the exception stack and modifies the pointer
*
*  @param	msp_stack, main stack pointer
*
*  @param	psp_stack, thread stack pointer
*
*  @param	lr_value, EXC_RETURN value while crash
*
*  @return	Returns the stack where the exception occurred
*
***/
uint32_t crash_GetExStack(uint32_t mstack, uint32_t lr_value);

/***
*  @brief	Gets the fault type
*
*  @param	fault_id, Exception numbers, provided in exception functions such as securefault, memfault, busfault, etc.
*
*  @return	None
*
***/
void crash_GetFaultInfo(uint32_t fault_id);

/***
*  @brief	Get genral regiters
*
*  @param	ex_stack, The stack where the exception occurred stores R0~R3, R12, LR, PC, and xPSR
*
*  @param	extra_reg, R4~R11 is saved in the current MSP stack
*
*  @param	lr_value, EXC_RETURN value while crash
*
*  @return	None
*
***/
void crash_GetGeneralReg(uint32_t *ex_stack, uint32_t *extra_reg, uint32_t lr_value);

/***
*  @brief	Get system status registers
*
*  @return	None
*
***/
void crash_GetSysStatus(void);

/***
*  @brief	Save the system registers to determine the cause of the exception
*
*  @param 	pc, The program counter while crash
*
*  @return	None
*
***/
int crash_GetExReason(uint32_t pc);

/***
*  @brief	The main function that handles crash
*
*  @param	mstack, main stack pointer
*
*  @param	lr_value, EXC_RETURN value while crash
*
*  @param	fault_id, The exception number, which distinguishes what type of error it is.
*
*  @return	None
*
***/
void crash_Main(uint32_t *mstack, uint32_t lr_value, uint32_t fault_id);

/***
*  @brief	fault diagnosis then print cause of fault
*
*  @param	regs, the register value at the time of the error
*
*  @return	None
*
***/
void crash_Diagnosis(struct cmb_hard_fault_regs *sys_regs);

#endif
