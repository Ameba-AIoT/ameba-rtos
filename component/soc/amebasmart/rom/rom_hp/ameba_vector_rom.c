/**
  ******************************************************************************
  * @file    rtl8721dhp_vector_rom.c
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the IRQ firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "ameba_soc.h"

RAM_VECTOR_TABLE1_SECTION
HAL_VECTOR_FUN  NewVectorTable[MAX_VECTOR_TABLE_NUM];

RAM_VECTOR_TABLE2_SECTION
IRQ_FUN         UserIrqFunTable[MAX_PERIPHERAL_IRQ_NUM];

RAM_VECTOR_TABLE2_SECTION
Fault_Patch     FaultPatchHandler;

RAM_VECTOR_TABLE3_SECTION
u32             UserIrqDataTable[MAX_PERIPHERAL_IRQ_NUM];

/*
	0xE000ED28 CFSR Configurable Fault Status Register
	0xE000ED28 MMFSR MemManage Fault Status Register
	0xE000ED29 BFSR BusFault Status Register
	0xE000ED2A UFSR UsageFault Status Register
	0xE000ED2C HFSR HardFault Status Register
	0xE000ED30 DFSR Debug Fault Status Register
	0xE000ED34 MMFAR MemManage Fault Address Register
	0xE000ED38 BFAR BusFault Address Register
	0xE000ED3C AFSR Auxiliary Fault Status Register

Secure software can access the Non-secure view of these registers via:
	0xE002ED28 CFSR_NS Configurable Fault Status Register (NS)
	0xE002ED28 MMFSR_NS MemManage Fault Status Register (NS)
	0xE002ED29 BFSR_NS BusFault Status Register (NS)
	0xE002ED2A UFSR_NS UsageFault Status Register (NS)
	0xE002ED2C HFSR_NS HardFault Status Register (NS)
	0xE002ED30 DFSR_NS Debug Fault Status Register (NS)
	0xE002ED34 MMFAR_NS MemManage Fault Address Register (NS)
	0xE002ED38 BFAR_NS BusFault Address Register (NS)
	0xE002ED3C AFSR_NS Auxiliary Fault Status Register (NS)

The location 0xE002_EDXX is RES0 to software executing in Non-secure state and
the debugger.
*/
HAL_ROM_TEXT_SECTION
void INT_HardFault_C(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id)
{
	uint32_t *stack;
	cmse_address_info_t cmse_address_info = cmse_TT((void *)INT_HardFault_C);
	uint32_t SecureFlag = cmse_address_info.flags.secure;

	u8 IsPstack = 0;
	u32 control_ns = 0;

	/* Hardfault Log is disabled in OTP */
	if (SYSCFG_OTP_FaultLogDis() == TRUE) {
		DBG_8195A("\r\nHard Fault happens \r\n");
		DBG_8195A("\r\nDo not dump log dure to security \r\n");

		while (1);
	}

	if (FaultPatchHandler) {
		FaultPatchHandler(mstack, pstack, lr_value, fault_id);
	}

	if (lr_value & BIT_EXCRETURN_ES) {  				//Exception is Secure
		if (lr_value & BIT_EXCRETURN_S) {				//Taken from S
			if (lr_value & BIT_EXCRETURN_MODE) {		//Thread Mode
				if (lr_value & BIT_EXCRETURN_SPSEL) { 	//PSP
					IsPstack = 1;
				}
			}
		} else {										//Taken from NS
			__ASM volatile("MRS %0, control_ns" : "=r"(control_ns));

			if (lr_value & BIT_EXCRETURN_MODE) {		//Thread Mode
				if (control_ns & BIT_CONTROL_SPSEL) {	//PSP
					IsPstack = 1;
				}
			}
		}
	} else {											//Exception is Non-secure
		if (lr_value & BIT_EXCRETURN_S) {				//Taken from S
			DBG_8195A("\nException taken from Secure to Non-secure.\nSecure stack is used to store context."
					  "It can not be dumped from non-secure side for security reason!!!\n");

			while (1);
		} else {										//Taken from NS
			if (lr_value & BIT_EXCRETURN_MODE) {		//Thread Mode
				if (lr_value & BIT_EXCRETURN_SPSEL) {	//PSP
					IsPstack = 1;
				}
			}
		}
	}

	if (IsPstack) {
		stack = pstack;
	} else {
		stack = mstack;
	}

	enum { r0 = 0, r1, r2, r3, r12, lr, pc, psr};

	if (fault_id == 0) {
		DBG_8195A("\r\nHard Fault: \r\n");
	} else if (fault_id == 1) {
		DBG_8195A("\r\nUsage Fault: \r\n");
	} else if (fault_id == 2) {
		DBG_8195A("\r\nBus Fault: \r\n");
	} else if (fault_id == 3) {
		DBG_8195A("\r\nMemManage Fault: \r\n");
	} else if (fault_id == 4) {
		DBG_8195A("\r\nSecurity Fault: \r\n");
	} else {
		DBG_8195A("\r\nDefault Hard Fault: \r\n");
	}
	DBG_8195A("Secure State: %x\r\n", SecureFlag);

	DBG_8195A("\r\nStacked: \r\n");
	DBG_8195A("R0 = 0x%08x\r\n", stack[r0]);
	DBG_8195A("R1 = 0x%08x\r\n", stack[r1]);
	DBG_8195A("R2 = 0x%08x\r\n", stack[r2]);
	DBG_8195A("R3 = 0x%08x\r\n", stack[r3]);
	DBG_8195A("R12 = 0x%08x\r\n", stack[r12]);
	DBG_8195A("LR = 0x%08x\r\n", stack[lr]);
	DBG_8195A("PC = 0x%08x\r\n", stack[pc]);
	DBG_8195A("PSR = 0x%08x\r\n", stack[psr]);

	DBG_8195A("\r\nCurrent: \r\n");
	DBG_8195A("EXC_RETURN = 0x%08x\r\n", lr_value);

	DBG_8195A("MSP = 0x%08x\r\n", mstack);
	DBG_8195A("PSP = 0x%08x\r\n", pstack);

	DBG_8195A("xPSR = 0x%08x\r\n", __get_xPSR());
	DBG_8195A("CFSR  = 0x%08x\r\n", HAL_READ32(0xE000ED28, 0x0));
	DBG_8195A("HFSR  = 0x%08x\r\n", HAL_READ32(0xE000ED2C, 0x0));
	DBG_8195A("DFSR  = 0x%08x\r\n", HAL_READ32(0xE000ED30, 0x0));
	DBG_8195A("MMFAR = 0x%08x\r\n", HAL_READ32(0xE000ED34, 0x0));
	DBG_8195A("BFAR  = 0x%08x\r\n",  HAL_READ32(0xE000ED38, 0x0));
	DBG_8195A("AFSR  = 0x%08x\r\n", HAL_READ32(0xE000ED3C, 0x0));

	DBG_8195A("PriMask = 0x%08x\r\n", __get_PRIMASK());
	DBG_8195A("SVC priority: 0x%02x\r\n", HAL_READ8(0xE000ED1F, 0));
	DBG_8195A("PendSVC priority: 0x%02x\r\n", HAL_READ8(0xE000ED22, 0));
	DBG_8195A("Systick priority: 0x%02x\r\n", HAL_READ8(0xE000ED23, 0));

	if (SecureFlag) {
		DBG_8195A("MSP_NS = 0x%08x\r\n", __TZ_get_MSP_NS());
		DBG_8195A("PSP_NS = 0x%08x\r\n", __TZ_get_PSP_NS());

		DBG_8195A("CFSR_NS  = 0x%08x\r\n", HAL_READ32(0xE002ED28, 0x0));
		DBG_8195A("HFSR_NS  = 0x%08x\r\n", HAL_READ32(0xE002ED2C, 0x0));
		DBG_8195A("DFSR_NS  = 0x%08x\r\n", HAL_READ32(0xE002ED30, 0x0));
		DBG_8195A("MMFAR_NS = 0x%08x\r\n", HAL_READ32(0xE002ED34, 0x0));
		DBG_8195A("BFAR_NS  = 0x%08x\r\n", HAL_READ32(0xE002ED38, 0x0));
		DBG_8195A("AFSR_NS  = 0x%08x\r\n", HAL_READ32(0xE002ED3C, 0x0));

		DBG_8195A("SVC priority NS: 0x%02x\r\n", HAL_READ8(0xE002ED1F, 0));
		DBG_8195A("PendSVC priority NS: 0x%02x\r\n", HAL_READ8(0xE002ED22, 0));
		DBG_8195A("Systick priority NS: 0x%02x\r\n", HAL_READ8(0xE002ED23, 0));
	}

	for (;;);
}

//Exception Number = 2
HAL_ROM_TEXT_SECTION
void
INT_NMI(void)
{
	//u32 CtrlReg;

	DBG_8195A("INT_NMI:NMI Error!!!!\n");
}

//Exception Number = 3
HAL_ROM_TEXT_SECTION
void
INT_HardFault(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #0\n\t"
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"   // Move MSP to upper to for we can dump current stack contents without chage contents
		"LDR R4,=INT_HardFault_C\n\t"
		"BX R4\n\t"
		".LTORG\n\t"
	);
}

//Exception Number = 4
HAL_ROM_TEXT_SECTION
void
INT_MemManage(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #3\n\t"
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_C\n\t"
		"BX R4\n\t"
		".LTORG\n\t"
	);
}

//Exception Number = 5
HAL_ROM_TEXT_SECTION
void
INT_BusFault(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #2\n\t"
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_C\n\t"
		"BX R4\n\t"
		".LTORG\n\t"
	);
}

//Exception Number = 6
HAL_ROM_TEXT_SECTION
void
INT_UsageFault(void)
{
	__ASM volatile(
		"MRS R0, MSP\n\t"
		"MRS R1, PSP\n\t"
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #1\n\t"
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_C\n\t"
		"BX R4\n\t"
		".LTORG\n\t"
	);
}

//Exception Number = 7
HAL_ROM_TEXT_SECTION
void
INT_SecureFault(void)
{
	__ASM volatile(
		"MRS R0, MSP_NS\n\t" /*note: print Non security is more usefully */
		"MRS R1, PSP_NS\n\t" /*note: print Non security is more usefully */
		"MOV R2, LR\n\t" /* second parameter is LR current value */
		"MOV R3, #4\n\t"
		"SUB.W	R4, R0, #128\n\t"
		"MSR MSP, R4\n\t"
		"LDR R4,=INT_HardFault_C\n\t"
		"BX R4\n\t"
		".LTORG\n\t"
	);
}

//following is External Interrupts & Exception Number >= 16

HAL_ROM_TEXT_SECTION
_LONG_CALL_ static void
INT_Entry(u32 IntIndex)
{
	//Clear ARM Cortex-M3 IRQ
	NVIC_ClearPendingIRQ(IntIndex);

	if (UserIrqFunTable[IntIndex] != NULL) {
		UserIrqFunTable[IntIndex]((void *)(UserIrqDataTable[IntIndex]));
	} else {
		DBG_8195A("INT_Entry Irq %d Fun Not Assign!!!!!", IntIndex);
	}
}

/* NVIC_AUTO_GEN_FUNC_START */
HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_AON_TIM(void)
{
	INT_Entry(AON_TIM_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER0(void)
{
	INT_Entry(TIMER0_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER1(void)
{
	INT_Entry(TIMER1_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER2(void)
{
	INT_Entry(TIMER2_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER3(void)
{
	INT_Entry(TIMER3_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER4(void)
{
	INT_Entry(TIMER4_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER5(void)
{
	INT_Entry(TIMER5_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER6(void)
{
	INT_Entry(TIMER6_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER7(void)
{
	INT_Entry(TIMER7_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_UART_LOG(void)
{
	INT_Entry(UART_LOG_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GPIOA(void)
{
	INT_Entry(GPIOA_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GPIOB(void)
{
	INT_Entry(GPIOB_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GPIOC(void)
{
	INT_Entry(GPIOC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_RTC(void)
{
	INT_Entry(RTC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_CTOUCH(void)
{
	INT_Entry(CTOUCH_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_ADC(void)
{
	INT_Entry(ADC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_ADC_COMP(void)
{
	INT_Entry(ADC_COMP_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_BOR(void)
{
	INT_Entry(BOR_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_PWR_DOWN(void)
{
	INT_Entry(PWR_DOWN_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_VADBT_OR_VADPC(void)
{
	INT_Entry(VADBT_OR_VADPC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SPI_FLASH(void)
{
	INT_Entry(SPI_FLASH_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_DEBUG_TIMER(void)
{
	INT_Entry(DEBUG_TIMER_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_THERMAL(void)
{
	INT_Entry(THERMAL_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_I2C0(void)
{
	INT_Entry(I2C0_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_RSIP(void)
{
	INT_Entry(RSIP_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_IPC_NP(void)
{
	INT_Entry(IPC_NP_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL0(void)
{
	INT_Entry(GDMA0_CHANNEL0_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL1(void)
{
	INT_Entry(GDMA0_CHANNEL1_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL2(void)
{
	INT_Entry(GDMA0_CHANNEL2_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL3(void)
{
	INT_Entry(GDMA0_CHANNEL3_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL4(void)
{
	INT_Entry(GDMA0_CHANNEL4_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL5(void)
{
	INT_Entry(GDMA0_CHANNEL5_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL6(void)
{
	INT_Entry(GDMA0_CHANNEL6_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_GDMA0_CHANNEL7(void)
{
	INT_Entry(GDMA0_CHANNEL7_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_LCDC(void)
{
	INT_Entry(LCDC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WL_DMA(void)
{
	INT_Entry(WL_DMA_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WL_PROTOCOL(void)
{
	INT_Entry(WL_PROTOCOL_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_AES(void)
{
	INT_Entry(AES_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SHA(void)
{
	INT_Entry(SHA_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SDIO_HOST(void)
{
	INT_Entry(SDIO_HOST_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_USB_OTG(void)
{
	INT_Entry(USB_OTG_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SPI0(void)
{
	INT_Entry(SPI0_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SPI1(void)
{
	INT_Entry(SPI1_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_RSVD(void)
{
	INT_Entry(RSVD_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_RSA(void)
{
	INT_Entry(RSA_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_ECDSA(void)
{
	INT_Entry(ECDSA_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_ED25519(void)
{
	INT_Entry(ED25519_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_PSRAMC(void)
{
	INT_Entry(PSRAMC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_DDR(void)
{
	INT_Entry(DDR_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_RXI300_HP(void)
{
	INT_Entry(RXI300_HP_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_IR(void)
{
	INT_Entry(IR_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_UART0(void)
{
	INT_Entry(UART0_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_UART1(void)
{
	INT_Entry(UART1_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_UART2(void)
{
	INT_Entry(UART2_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_UART3_BT(void)
{
	INT_Entry(UART3_BT_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TRNG(void)
{
	INT_Entry(TRNG_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_I2C1(void)
{
	INT_Entry(I2C1_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_I2C2(void)
{
	INT_Entry(I2C2_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER8(void)
{
	INT_Entry(TIMER8_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER9(void)
{
	INT_Entry(TIMER9_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WDG1(void)
{
	INT_Entry(WDG1_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WDG2(void)
{
	INT_Entry(WDG2_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WDG3(void)
{
	INT_Entry(WDG3_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WDG4(void)
{
	INT_Entry(WDG4_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SPORT0(void)
{
	INT_Entry(SPORT0_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SPORT1(void)
{
	INT_Entry(SPORT1_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SPORT2(void)
{
	INT_Entry(SPORT2_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SPORT3(void)
{
	INT_Entry(SPORT3_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_BT_SCB(void)
{
	INT_Entry(BT_SCB_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_LEDC(void)
{
	INT_Entry(LEDC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_MIPI_DSI(void)
{
	INT_Entry(MIPI_DSI_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_AES_S(void)
{
	INT_Entry(AES_S_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_SHA_S(void)
{
	INT_Entry(SHA_S_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_AON_WAKEPIN(void)
{
	INT_Entry(AON_WAKEPIN_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_ZIGBEE(void)
{
	INT_Entry(ZIGBEE_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_BT_WAKE_HOST(void)
{
	INT_Entry(BT_WAKE_HOST_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER10(void)
{
	INT_Entry(TIMER10_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER11(void)
{
	INT_Entry(TIMER11_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER12(void)
{
	INT_Entry(TIMER12_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_TIMER13(void)
{
	INT_Entry(TIMER13_IRQ);
}

/* NVIC_AUTO_GEN_FUNC_END */

/**
  * @brief  Init vector table.
  * @param  StackP: MSP TOP.
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
void irq_table_init(u32 StackP)
{
	NewVectorTable[0] = (HAL_VECTOR_FUN)StackP;
	NewVectorTable[2] = (HAL_VECTOR_FUN)INT_NMI;
	NewVectorTable[3] = (HAL_VECTOR_FUN)INT_HardFault;

	NewVectorTable[4] = (HAL_VECTOR_FUN)INT_MemManage;
	NewVectorTable[5] = (HAL_VECTOR_FUN)INT_BusFault;
	NewVectorTable[6] = (HAL_VECTOR_FUN)INT_UsageFault;
	NewVectorTable[7] = (HAL_VECTOR_FUN)INT_SecureFault;

	/* NVIC_AUTO_GEN_INIT_START */
	NewVectorTable[16 + AON_TIM_IRQ] = (HAL_VECTOR_FUN) INT_AON_TIM;
	NewVectorTable[16 + TIMER0_IRQ] = (HAL_VECTOR_FUN) INT_TIMER0;
	NewVectorTable[16 + TIMER1_IRQ] = (HAL_VECTOR_FUN) INT_TIMER1;
	NewVectorTable[16 + TIMER2_IRQ] = (HAL_VECTOR_FUN) INT_TIMER2;
	NewVectorTable[16 + TIMER3_IRQ] = (HAL_VECTOR_FUN) INT_TIMER3;
	NewVectorTable[16 + TIMER4_IRQ] = (HAL_VECTOR_FUN) INT_TIMER4;
	NewVectorTable[16 + TIMER5_IRQ] = (HAL_VECTOR_FUN) INT_TIMER5;
	NewVectorTable[16 + TIMER6_IRQ] = (HAL_VECTOR_FUN) INT_TIMER6;
	NewVectorTable[16 + TIMER7_IRQ] = (HAL_VECTOR_FUN) INT_TIMER7;
	NewVectorTable[16 + UART_LOG_IRQ] = (HAL_VECTOR_FUN) INT_UART_LOG;
	NewVectorTable[16 + GPIOA_IRQ] = (HAL_VECTOR_FUN) INT_GPIOA;
	NewVectorTable[16 + GPIOB_IRQ] = (HAL_VECTOR_FUN) INT_GPIOB;
	NewVectorTable[16 + GPIOC_IRQ] = (HAL_VECTOR_FUN) INT_GPIOC;
	NewVectorTable[16 + RTC_IRQ] = (HAL_VECTOR_FUN) INT_RTC;
	NewVectorTable[16 + CTOUCH_IRQ] = (HAL_VECTOR_FUN) INT_CTOUCH;
	NewVectorTable[16 + ADC_IRQ] = (HAL_VECTOR_FUN) INT_ADC;
	NewVectorTable[16 + ADC_COMP_IRQ] = (HAL_VECTOR_FUN) INT_ADC_COMP;
	NewVectorTable[16 + BOR_IRQ] = (HAL_VECTOR_FUN) INT_BOR;
	NewVectorTable[16 + PWR_DOWN_IRQ] = (HAL_VECTOR_FUN) INT_PWR_DOWN;
	NewVectorTable[16 + VADBT_OR_VADPC_IRQ] = (HAL_VECTOR_FUN) INT_VADBT_OR_VADPC;
	NewVectorTable[16 + SPI_FLASH_IRQ] = (HAL_VECTOR_FUN) INT_SPI_FLASH;
	NewVectorTable[16 + DEBUG_TIMER_IRQ] = (HAL_VECTOR_FUN) INT_DEBUG_TIMER;
	NewVectorTable[16 + THERMAL_IRQ] = (HAL_VECTOR_FUN) INT_THERMAL;
	NewVectorTable[16 + I2C0_IRQ] = (HAL_VECTOR_FUN) INT_I2C0;
	NewVectorTable[16 + RSIP_IRQ] = (HAL_VECTOR_FUN) INT_RSIP;
	NewVectorTable[16 + IPC_NP_IRQ] = (HAL_VECTOR_FUN) INT_IPC_NP;
	NewVectorTable[16 + GDMA0_CHANNEL0_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL0;
	NewVectorTable[16 + GDMA0_CHANNEL1_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL1;
	NewVectorTable[16 + GDMA0_CHANNEL2_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL2;
	NewVectorTable[16 + GDMA0_CHANNEL3_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL3;
	NewVectorTable[16 + GDMA0_CHANNEL4_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL4;
	NewVectorTable[16 + GDMA0_CHANNEL5_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL5;
	NewVectorTable[16 + GDMA0_CHANNEL6_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL6;
	NewVectorTable[16 + GDMA0_CHANNEL7_IRQ] = (HAL_VECTOR_FUN) INT_GDMA0_CHANNEL7;
	NewVectorTable[16 + LCDC_IRQ] = (HAL_VECTOR_FUN) INT_LCDC;
	NewVectorTable[16 + WL_DMA_IRQ] = (HAL_VECTOR_FUN) INT_WL_DMA;
	NewVectorTable[16 + WL_PROTOCOL_IRQ] = (HAL_VECTOR_FUN) INT_WL_PROTOCOL;
	NewVectorTable[16 + AES_IRQ] = (HAL_VECTOR_FUN) INT_AES;
	NewVectorTable[16 + SHA_IRQ] = (HAL_VECTOR_FUN) INT_SHA;
	NewVectorTable[16 + SDIO_HOST_IRQ] = (HAL_VECTOR_FUN) INT_SDIO_HOST;
	NewVectorTable[16 + USB_OTG_IRQ] = (HAL_VECTOR_FUN) INT_USB_OTG;
	NewVectorTable[16 + SPI0_IRQ] = (HAL_VECTOR_FUN) INT_SPI0;
	NewVectorTable[16 + SPI1_IRQ] = (HAL_VECTOR_FUN) INT_SPI1;
	NewVectorTable[16 + RSVD_IRQ] = (HAL_VECTOR_FUN) INT_RSVD;
	NewVectorTable[16 + RSA_IRQ] = (HAL_VECTOR_FUN) INT_RSA;
	NewVectorTable[16 + ECDSA_IRQ] = (HAL_VECTOR_FUN) INT_ECDSA;
	NewVectorTable[16 + ED25519_IRQ] = (HAL_VECTOR_FUN) INT_ED25519;
	NewVectorTable[16 + PSRAMC_IRQ] = (HAL_VECTOR_FUN) INT_PSRAMC;
	NewVectorTable[16 + DDR_IRQ] = (HAL_VECTOR_FUN) INT_DDR;
	NewVectorTable[16 + RXI300_HP_IRQ] = (HAL_VECTOR_FUN) INT_RXI300_HP;
	NewVectorTable[16 + IR_IRQ] = (HAL_VECTOR_FUN) INT_IR;
	NewVectorTable[16 + UART0_IRQ] = (HAL_VECTOR_FUN) INT_UART0;
	NewVectorTable[16 + UART1_IRQ] = (HAL_VECTOR_FUN) INT_UART1;
	NewVectorTable[16 + UART2_IRQ] = (HAL_VECTOR_FUN) INT_UART2;
	NewVectorTable[16 + UART3_BT_IRQ] = (HAL_VECTOR_FUN) INT_UART3_BT;
	NewVectorTable[16 + TRNG_IRQ] = (HAL_VECTOR_FUN) INT_TRNG;
	NewVectorTable[16 + I2C1_IRQ] = (HAL_VECTOR_FUN) INT_I2C1;
	NewVectorTable[16 + I2C2_IRQ] = (HAL_VECTOR_FUN) INT_I2C2;
	NewVectorTable[16 + TIMER8_IRQ] = (HAL_VECTOR_FUN) INT_TIMER8;
	NewVectorTable[16 + TIMER9_IRQ] = (HAL_VECTOR_FUN) INT_TIMER9;
	NewVectorTable[16 + WDG1_IRQ] = (HAL_VECTOR_FUN) INT_WDG1;
	NewVectorTable[16 + WDG2_IRQ] = (HAL_VECTOR_FUN) INT_WDG2;
	NewVectorTable[16 + WDG3_IRQ] = (HAL_VECTOR_FUN) INT_WDG3;
	NewVectorTable[16 + WDG4_IRQ] = (HAL_VECTOR_FUN) INT_WDG4;
	NewVectorTable[16 + SPORT0_IRQ] = (HAL_VECTOR_FUN) INT_SPORT0;
	NewVectorTable[16 + SPORT1_IRQ] = (HAL_VECTOR_FUN) INT_SPORT1;
	NewVectorTable[16 + SPORT2_IRQ] = (HAL_VECTOR_FUN) INT_SPORT2;
	NewVectorTable[16 + SPORT3_IRQ] = (HAL_VECTOR_FUN) INT_SPORT3;
	NewVectorTable[16 + BT_SCB_IRQ] = (HAL_VECTOR_FUN) INT_BT_SCB;
	NewVectorTable[16 + LEDC_IRQ] = (HAL_VECTOR_FUN) INT_LEDC;
	NewVectorTable[16 + MIPI_DSI_IRQ] = (HAL_VECTOR_FUN) INT_MIPI_DSI;
	NewVectorTable[16 + AES_S_IRQ] = (HAL_VECTOR_FUN) INT_AES_S;
	NewVectorTable[16 + SHA_S_IRQ] = (HAL_VECTOR_FUN) INT_SHA_S;
	NewVectorTable[16 + AON_WAKEPIN_IRQ] = (HAL_VECTOR_FUN) INT_AON_WAKEPIN;
	NewVectorTable[16 + ZIGBEE_IRQ] = (HAL_VECTOR_FUN) INT_ZIGBEE;
	NewVectorTable[16 + BT_WAKE_HOST_IRQ] = (HAL_VECTOR_FUN) INT_BT_WAKE_HOST;
	NewVectorTable[16 + TIMER10_IRQ] = (HAL_VECTOR_FUN) INT_TIMER10;
	NewVectorTable[16 + TIMER11_IRQ] = (HAL_VECTOR_FUN) INT_TIMER11;
	NewVectorTable[16 + TIMER12_IRQ] = (HAL_VECTOR_FUN) INT_TIMER12;
	NewVectorTable[16 + TIMER13_IRQ] = (HAL_VECTOR_FUN) INT_TIMER13;
	/* NVIC_AUTO_GEN_INIT_END */


	//2 3) Set Vector Table Offset Register
	SCB->VTOR = (uint32_t)NewVectorTable;
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
void irq_enable(IRQn_Type irqn)
{
	__NVIC_EnableIRQ(irqn);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
void irq_disable(IRQn_Type irqn)
{
	__NVIC_DisableIRQ(irqn);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
void irq_set_priority(IRQn_Type irqn, uint32_t priority)
{
	__NVIC_SetPriority(irqn, priority);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
uint32_t irq_get_priority(IRQn_Type irqn)
{
	return __NVIC_GetPriority(irqn);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
void irq_set_pending(IRQn_Type irqn)
{
	__NVIC_SetPendingIRQ(irqn);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
uint32_t irq_get_pending(IRQn_Type irqn)
{
	return __NVIC_GetPendingIRQ(irqn);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
void irq_clear_pending(IRQn_Type irqn)
{
	__NVIC_ClearPendingIRQ(irqn);
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
bool irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data,  u32 Priority)
{
	UserIrqFunTable[IrqNum] = (IRQ_FUN)((u32)IrqFun | 0x1);
	UserIrqDataTable[IrqNum] = (u32)(Data);

	__NVIC_SetPriority(IrqNum, Priority);

	return TRUE;
}

/**
  * @brief  Unregister IRQ Handler.
  * @param  IrqNum: a value of @ref IRQn_enum.
  */
HAL_ROM_TEXT_SECTION _LONG_CALL_
bool irq_unregister(IRQn_Type   IrqNum)
{
	UserIrqFunTable[IrqNum] = (IRQ_FUN)NULL;
	UserIrqDataTable[IrqNum] = (u32)NULL;

	return TRUE;
}

HAL_ROM_TEXT_SECTION _LONG_CALL_
bool FaultPatch_register(Fault_Patch PatchFun)
{
	FaultPatchHandler = PatchFun;

	return TRUE;
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
