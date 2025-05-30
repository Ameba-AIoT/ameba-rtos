/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_VECTOR_TABLE_H_
#define _AMEBA_VECTOR_TABLE_H_

/* Exported constants --------------------------------------------------------*/
/** @defgroup IRQ_Exported_Constants IRQ Exported Constants
  * @{
  */
#if defined (CONFIG_ARM_CORE_CM4)
/** @defgroup Km4IRQn_enum
  * @{
  */
typedef enum KM4IRQn {
	/******  Processor Exceptions Numbers ********/
	NonMaskableInt_IRQn		= -14,	/*!< 2 Non Maskable Interrupt */
	HardFault_IRQn			= -13,	/*!< 3 Hard Fault, all classes of Fault */
	MemoryManagement_IRQn	= -12,	/*!< 4 Memory Management Interrupt */
	BusFault_IRQn			= -11,	/*!< 5 Bus Fault Interrupt */
	UsageFault_IRQn			= -10,	/*!< 6 Usage Fault Interrupt */
	SecureFault_IRQn		= -9,   /*!< 7 Secure Fault Interrupt */
	SVCall_IRQn				= -5,	/*!< 11 SV Call Interrupt  */
	DebugMonitor_IRQn		= -4,	/*!< 12 Debug Monitor Interrupt */
	PendSV_IRQn				= -2,	/*!< 14 Pend SV Interrupt */
	SysTick_IRQn			= -1,	/*!< 15 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	WIFI_FISR_FESR_IRQ			= 0,
	WIFI_FTSR_MAILBOX_IRQ			= 1,
	WL_DMA_IRQ			= 2,
	WL_PROTOCOL_IRQ			= 3,
	BT_SCB_IRQ			= 4,
	NP_WAKE_IRQ			= 5,
	DSP_WAKE_IRQ			= 6,
	BT_WAKE_HOST_IRQ			= 7,
	RXI300_IRQ			= 8,
	SYS_ILLEGAL_WRITE_IRQ			= 9,
	IPC_KM4_IRQ			= 10,
	IWDG0_IRQ			= 11,
	KM4_NS_WDG_INT_IRQ			= 12,
	KM4_S_WDG_INT_IRQ			= 13,
	KR4_WDG_RST_IRQ			= 14,
	DSP_WDG_RST_IRQ			= 15,
	TIMER0_IRQ			= 16,
	TIMER1_IRQ			= 17,
	TIMER2_IRQ			= 18,
	TIMER3_IRQ			= 19,
	TIMER4_IRQ			= 20,
	TIMER5_IRQ			= 21,
	TIMER6_IRQ			= 22,
	TIMER7_IRQ			= 23,
	TIMER8_IRQ			= 24,
	TIMER9_IRQ			= 25,
	TIMER10_IRQ			= 26,
	TIMER11_IRQ			= 27,
	TIMER12_IRQ			= 28,
	TIMER13_IRQ			= 29,
	TIMER14_IRQ			= 30,
	UART0_IRQ			= 31,
	UART1_IRQ			= 32,
	UART2_IRQ			= 33,
	UART3_BT_IRQ			= 34,
	UART_LOG_IRQ			= 35,
	GPIOA_IRQ			= 36,
	GPIOB_IRQ			= 37,
	I2C0_IRQ			= 38,
	I2C1_IRQ			= 39,
	CTOUCH_IRQ			= 40,
	GDMA0_CHANNEL0_IRQ			= 41,
	GDMA0_CHANNEL1_IRQ			= 42,
	GDMA0_CHANNEL2_IRQ			= 43,
	GDMA0_CHANNEL3_IRQ			= 44,
	GDMA0_CHANNEL4_IRQ			= 45,
	GDMA0_CHANNEL5_IRQ			= 46,
	GDMA0_CHANNEL6_IRQ			= 47,
	GDMA0_CHANNEL7_IRQ			= 48,
	SPI0_IRQ			= 49,
	SPI1_IRQ			= 50,
	SPORT0_IRQ			= 51,
	SPORT1_IRQ			= 52,
	RTC_IRQ			= 53,
	ADC_IRQ			= 54,
	ADC_COMP_IRQ			= 55,
	BOR_IRQ			= 56,
	PWR_DOWN_IRQ			= 57,
	SPI_FLASH_IRQ			= 58,
	THERMAL_IRQ			= 59,
	RSIP_IRQ			= 60,
	AES_IRQ			= 61,
	SHA_IRQ			= 62,
	ECDSA_IRQ			= 63,
	PSRAMC_IRQ			= 64,
	TRNG_IRQ			= 65,
	LEDC_IRQ			= 66,
	AES_S_IRQ			= 67,
	SHA_S_IRQ			= 68,
	AON_TIM_IRQ			= 69,
	AON_WAKEPIN_IRQ			= 70,
	DSP_ERROR_IRQ			= 71,
	DSP_FATAL_ERROR_IRQ			= 72,
	VAD_IRQ			= 73,
	ZIGBEE_IRQ			= 74,
} IRQn_Type;
/**
  * @}
  */
#endif

#if defined (CONFIG_RSICV_CORE_KR4)
#define CONFIG_CPU_RV_RTK_PLIC 1

#define MAX_SYSTEM_EXECPTION_NUM      16
#define MAX_CORE_EXCEPTION_NUM        16
#define NVIC_PLIC_IRQ_OFFSET          (MAX_SYSTEM_EXECPTION_NUM + MAX_CORE_EXCEPTION_NUM)
#define NVIC_PLIC_IRQ_OFFSET_1BASED          (MAX_SYSTEM_EXECPTION_NUM + MAX_CORE_EXCEPTION_NUM + 1)
/** @defgroup KR4IRQn_enum
  * @{
  */
typedef enum KR4IRQn {
	/* Exception from system fault or ECALL instruction */
	Fault_IAddr_IRQn = (0 - MAX_SYSTEM_EXECPTION_NUM),  // Instruction address misaligned
	Fault_IAcces_IRQn = (1 - MAX_SYSTEM_EXECPTION_NUM), // Instruction access fault
	Fault_Instr_IRQn = (2 - MAX_SYSTEM_EXECPTION_NUM),  // Illegal instruction
	Sys_Brk_IRQn = (3 - MAX_SYSTEM_EXECPTION_NUM),      // Breakpoint
	Fault_DAddr_IRQn = (4 - MAX_SYSTEM_EXECPTION_NUM),  // Load address misaligned
	Fault_DAcces_IRQn = (5 - MAX_SYSTEM_EXECPTION_NUM), // Load access fault
	Fault_SAddr_IRQn = (6 - MAX_SYSTEM_EXECPTION_NUM),  // Store/AMO address misaligned
	Fault_SAcces_IRQn = (7 - MAX_SYSTEM_EXECPTION_NUM), // Store/AMO access fault
	Sys_UECall_IRQn = (8 - MAX_SYSTEM_EXECPTION_NUM),   // Environment call from U-mode
	Sys_SECall_IRQn = (9 - MAX_SYSTEM_EXECPTION_NUM),   // Environment call from S-mode
	Sys_Resv1_IRQn = (10 - MAX_SYSTEM_EXECPTION_NUM),   // Reserved
	Sys_MECall_IRQn = (11 - MAX_SYSTEM_EXECPTION_NUM),  // Environment call from M-mode
	Fault_IPage_IRQn = (12 - MAX_SYSTEM_EXECPTION_NUM), // Instruction page fault
	Fault_LPage_IRQn = (13 - MAX_SYSTEM_EXECPTION_NUM), // Load page fault
	Sys_Resv2_IRQn = (14 - MAX_SYSTEM_EXECPTION_NUM),   // Reserved for future standard use
	Fault_SPage_IRQn = (15 - MAX_SYSTEM_EXECPTION_NUM), // Store/AMO page fault

	/* Exception from Interrupts */
	User_SW_IRQn = 0,   // User software interrupt
	Suvi_SW_IRQn = 1,   // Supervisor software interrupt
	Resv1_IRQn = 2,     // Reserved for future standard use
	Mach_SW_IRQn = 3,   // Machine software interrupt
	User_Tmr_IRQn = 4,  // User timer interrupt
	Suvi_Tmr_IRQn = 5,  // Supervisor timer interrupt
	Resv2_IRQn = 6,     // Reserved for future standard use
	Mach_Tmr_IRQn = 7,  // Machine timer interrupt
	User_Ext_IRQn = 8,  // User external interrupt
	Suvi_Ext_IRQn = 9,  // Supervisor external interrupt
	Resv3_IRQn = 10,    // Reserved for future standard use
	Mach_Ext_IRQn = 11, // Machine external interrupt

	/* IRQn 12 ~ 15 is reserved for future RISC-V standard use */

	Core_INT0_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 0),   // CPU Core external interrupt 0
	Core_INT1_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 1),   // CPU Core external interrupt 1
	Core_INT2_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 2),   // CPU Core external interrupt 2
	Core_INT3_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 3),   // CPU Core external interrupt 3
	Core_INT4_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 4),   // CPU Core external interrupt 4
	Core_INT5_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 5),   // CPU Core external interrupt 5
	Core_INT6_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 6),   // CPU Core external interrupt 6
	Core_INT7_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 7),   // CPU Core external interrupt 7
	Core_INT8_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 8),   // CPU Core external interrupt 8
	Core_INT9_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 9),   // CPU Core external interrupt 9
	Core_INT10_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 10), // CPU Core external interrupt 10
	Core_INT11_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 11), // CPU Core external interrupt 11
	Core_INT12_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 12), // CPU Core external interrupt 12
	Core_INT13_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 13), // CPU Core external interrupt 13
	Core_INT14_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 14), // CPU Core external interrupt 14
	Core_INT15_IRQn = (MAX_SYSTEM_EXECPTION_NUM + 15), // CPU Core external interrupt 15

	/******  Specific Interrupt Numbers ************/
	WIFI_FISR_FESR_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 0,
	WIFI_FTSR_MAILBOX_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 1,
	WL_DMA_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 2,
	WL_PROTOCOL_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 3,
	BT_SCB_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 4,
	NP_WAKE_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 5,
	DSP_WAKE_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 6,
	BT_WAKE_HOST_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 7,
	RXI300_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 8,
	SYS_ILLEGAL_WRITE_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 9,
	IPC_KR4_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 10,
	IWDG0_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 11,
	KM4_NS_WDG_RST_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 12,
	KM4_S_WDG_RST_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 13,
	KR4_WDG_INT_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 14,
	DSP_WDG_RST_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 15,
	TIMER0_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 16,
	TIMER1_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 17,
	TIMER2_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 18,
	TIMER3_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 19,
	TIMER4_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 20,
	TIMER5_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 21,
	TIMER6_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 22,
	TIMER7_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 23,
	TIMER8_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 24,
	TIMER9_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 25,
	TIMER10_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 26,
	TIMER11_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 27,
	TIMER12_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 28,
	TIMER13_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 29,
	TIMER14_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 30,
	UART0_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 31,
	UART1_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 32,
	UART2_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 33,
	UART3_BT_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 34,
	UART_LOG_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 35,
	GPIOA_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 36,
	GPIOB_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 37,
	I2C0_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 38,
	I2C1_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 39,
	CTOUCH_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 40,
	GDMA0_CHANNEL0_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 41,
	GDMA0_CHANNEL1_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 42,
	GDMA0_CHANNEL2_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 43,
	GDMA0_CHANNEL3_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 44,
	GDMA0_CHANNEL4_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 45,
	GDMA0_CHANNEL5_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 46,
	GDMA0_CHANNEL6_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 47,
	GDMA0_CHANNEL7_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 48,
	SPI0_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 49,
	SPI1_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 50,
	SPORT0_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 51,
	SPORT1_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 52,
	RTC_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 53,
	ADC_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 54,
	ADC_COMP_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 55,
	BOR_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 56,
	PWR_DOWN_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 57,
	SPI_FLASH_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 58,
	THERMAL_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 59,
	RSIP_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 60,
	AES_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 61,
	SHA_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 62,
	ECDSA_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 63,
	PSRAMC_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 64,
	TRNG_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 65,
	LEDC_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 66,
	AES_S_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 67,
	SHA_S_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 68,
	AON_TIM_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 69,
	AON_WAKEPIN_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 70,
	DSP_ERROR_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 71,
	DSP_FATAL_ERROR_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 72,
	VAD_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 73,
	ZIGBEE_IRQ			= NVIC_PLIC_IRQ_OFFSET_1BASED + 74,
} IRQn_Type;
/**
  * @}
  */
#endif

#if defined (RSICV_CORE_DSP)
/** @defgroup DSPIRQn_enum
  * @{
  */
typedef enum DSPRQn {
	/******  Specific Interrupt Numbers ************/
	WL_DMA_IRQ = 9,
	IPC_DSP_IRQ = 7,
	DSP_WDG_INT_IRQ = 8,
	TIMER7_IRQ = 10,
	TIMER10_IRQ = 11,
	TIMER11_IRQ = 12,
	TIMER12_IRQ = 13,
	TIMER13_IRQ = 14,
	TIMER14_IRQ = 15,
	GPIOA_IRQ = 16,
	GPIOB_IRQ = 17,
	CTOUCH_IRQ = 18,
	GDMA0_CHANNEL0_IRQ = 19,
	GDMA0_CHANNEL1_IRQ = 20,
	GDMA0_CHANNEL2_IRQ = 21,
	GDMA0_CHANNEL3_IRQ = 22,
	SPORT0_IRQ = 23,
	SPORT1_IRQ = 24,
	VAD_IRQ = 25,
	ZIGBEE_IRQ = 26,
	DSP_NMI_IRQ = 28,
} IRQn_Type;
/**
  * @}
  */
#endif

/** @defgroup IRQn_Priority_def
  * @{
  */
#ifdef CONFIG_RSICV_CORE_KR4
enum IRQnPriority {
	INT_PRI7 = 1,	//lower numbers have higher priority in KM4, But higher numbers have higher priority in KR4.
	INT_PRI6 = 1,	//A priority value of 0 is reserved in KR4, Set to 1 if an enabled IQR priority is 0 in the Code.
	INT_PRI_LOWEST	= INT_PRI6,
	INT_PRI5,
	INT_PRI4,
	INT_PRI_MIDDLE	= INT_PRI4,
	INT_PRI3,
	INT_PRI2,
	INT_PRI_HIGH 	= INT_PRI2,
	INT_PRI1,
	INT_PRI0,		//Priority value 7 is the highest active priority
	INT_PRI_HIGHEST	= INT_PRI0,
};
#else
enum IRQnPriority {
	INT_PRI0 = 0,				//KM4 Priority value 0 is the highest active priority
	INT_PRI_HIGHEST	= INT_PRI0,
	INT_PRI1,					//lower numbers have higher priority in KM4
	INT_PRI2,
	INT_PRI_HIGH 	= INT_PRI2,
	INT_PRI3,
	INT_PRI4,
	INT_PRI_MIDDLE	= INT_PRI4,
	INT_PRI5,
	INT_PRI6,
	INT_PRI7,
	INT_PRI_LOWEST	= INT_PRI7,
};
#endif
/**
  * @}
  */

/**
  * @}
  */


#endif //_AMEBA_VECTOR_TABLE_H_
