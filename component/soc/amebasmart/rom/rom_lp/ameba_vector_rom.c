/**
  ******************************************************************************
  * @file    rtl8721dlp_vector_rom.c
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

RAM_VECTOR_TABLE3_SECTION
u32             UserIrqDataTable[MAX_PERIPHERAL_IRQ_NUM];

HAL_ROM_TEXT_SECTION
void INT_HardFault_C(u32 *stack)
{
	DBG_8195A("Hard Fault Error!!!!\n");

	DBG_8195A("R0 = 0x%x\n", stack[0]);
	DBG_8195A("R1 = 0x%x\n", stack[1]);
	DBG_8195A("R2 = 0x%x\n", stack[2]);
	DBG_8195A("R3 = 0x%x\n", stack[3]);
	DBG_8195A("R12 = 0x%x\n", stack[4]);
	DBG_8195A("LR = 0x%x\n", stack[5]);
	DBG_8195A("PC = 0x%x\n", stack[6]);
	DBG_8195A("xPSR = 0x%x\n", stack[7]);
	DBG_8195A("PriMask: 0x%x\n", __get_PRIMASK());

	while (1);
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
		"MOVS r0, #4\n\t"
		"MOV r1, LR\n\t"
		"TST r0, r1\n\t"
		"BEQ stacking_used_MSP\n\t"
		"MRS R0, PSP\n\t" /* stacking was using PSP */
		"B branch\n\t"
		"stacking_used_MSP:\n\t"
		"MRS R0, MSP\n\t" /* stacking was using MSP */
		"branch:\n\t"
		"LDR R2,=INT_HardFault_C\n\t"
		"BX R2\n\t"
	);
}

//Exception Number = 4
HAL_ROM_TEXT_SECTION
void
INT_MemManage(void)
{
	DBG_8195A("Memory Manage Error!!!!\n");
	for (;;);
}

//Exception Number = 5
HAL_ROM_TEXT_SECTION
void
INT_BusFault(void)
{
	DBG_8195A("Bus Fault Error!!!!\n");
	for (;;);
}

//Exception Number = 6
HAL_ROM_TEXT_SECTION
void
INT_UsageFault(void)
{
	DBG_8195A("Usage Fault Error!!!!\n");
	for (;;);
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
INT_IPC(void)
{
	INT_Entry(IPC_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_RXI300(void)
{
	INT_Entry(RXI300_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WIFI_FISR_FESR(void)
{
	INT_Entry(WIFI_FISR_FESR_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WIFI_FTSR_MAILBOX(void)
{
	INT_Entry(WIFI_FTSR_MAILBOX_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_AON_TIM(void)
{
	INT_Entry(AON_TIM_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_NP_WAKE(void)
{
	INT_Entry(NP_WAKE_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_AP_WAKE(void)
{
	INT_Entry(AP_WAKE_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_WDG0(void)
{
	INT_Entry(WDG0_IRQ);
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
INT_AON_WAKEPIN(void)
{
	INT_Entry(AON_WAKEPIN_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_BT_WAKE_HOST(void)
{
	INT_Entry(BT_WAKE_HOST_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_NFIQOUT0_OR_NIRQOUT0(void)
{
	INT_Entry(NFIQOUT0_OR_NIRQOUT0_IRQ);
}

HAL_ROM_TEXT_SECTION
_LONG_CALL_  static void
INT_NFIQOUT1_OR_NIRQOUT1(void)
{
	INT_Entry(NFIQOUT1_OR_NIRQOUT1_IRQ);
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
	NewVectorTable[1] = (HAL_VECTOR_FUN)HAL_READ32(0, 0x4);
	NewVectorTable[2] = (HAL_VECTOR_FUN)HAL_READ32(0, 0x8);
	NewVectorTable[3] = (HAL_VECTOR_FUN)HAL_READ32(0, 0xC);

	/* NVIC_AUTO_GEN_INIT_START */
	NewVectorTable[16 + IPC_IRQ] = (HAL_VECTOR_FUN) INT_IPC;
	NewVectorTable[16 + RXI300_IRQ] = (HAL_VECTOR_FUN) INT_RXI300;
	NewVectorTable[16 + WIFI_FISR_FESR_IRQ] = (HAL_VECTOR_FUN) INT_WIFI_FISR_FESR;
	NewVectorTable[16 + WIFI_FTSR_MAILBOX_IRQ] = (HAL_VECTOR_FUN) INT_WIFI_FTSR_MAILBOX;
	NewVectorTable[16 + AON_TIM_IRQ] = (HAL_VECTOR_FUN) INT_AON_TIM;
	NewVectorTable[16 + NP_WAKE_IRQ] = (HAL_VECTOR_FUN) INT_NP_WAKE;
	NewVectorTable[16 + AP_WAKE_IRQ] = (HAL_VECTOR_FUN) INT_AP_WAKE;
	NewVectorTable[16 + WDG0_IRQ] = (HAL_VECTOR_FUN) INT_WDG0;
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
	NewVectorTable[16 + AON_WAKEPIN_IRQ] = (HAL_VECTOR_FUN) INT_AON_WAKEPIN;
	NewVectorTable[16 + BT_WAKE_HOST_IRQ] = (HAL_VECTOR_FUN) INT_BT_WAKE_HOST;
	NewVectorTable[16 + NFIQOUT0_OR_NIRQOUT0_IRQ] = (HAL_VECTOR_FUN) INT_NFIQOUT0_OR_NIRQOUT0;
	NewVectorTable[16 + NFIQOUT1_OR_NIRQOUT1_IRQ] = (HAL_VECTOR_FUN) INT_NFIQOUT1_OR_NIRQOUT1;
	NewVectorTable[16 + WL_DMA_IRQ] = (HAL_VECTOR_FUN) INT_WL_DMA;
	NewVectorTable[16 + WL_PROTOCOL_IRQ] = (HAL_VECTOR_FUN) INT_WL_PROTOCOL;
	NewVectorTable[16 + TIMER10_IRQ] = (HAL_VECTOR_FUN) INT_TIMER10;
	NewVectorTable[16 + TIMER11_IRQ] = (HAL_VECTOR_FUN) INT_TIMER11;
	NewVectorTable[16 + TIMER12_IRQ] = (HAL_VECTOR_FUN) INT_TIMER12;
	NewVectorTable[16 + TIMER13_IRQ] = (HAL_VECTOR_FUN) INT_TIMER13;
	/* NVIC_AUTO_GEN_INIT_END */

	//2 3) Set Vector Table Offset Register
	SCB->VTOR = (uint32_t)(NewVectorTable);
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

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
