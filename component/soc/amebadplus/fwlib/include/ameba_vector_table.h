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
	SYS_ILLEGAL_WRITE_IRQ			= 5,
	BT_WAKE_HOST_IRQ			= 6,
	RXI300_IRQ			= 7,
	IPC_KM4_IRQ			= 8,
	IWDG_IRQ			= 9,
	TIMER0_IRQ			= 10,
	TIMER1_IRQ			= 11,
	TIMER2_IRQ			= 12,
	TIMER3_IRQ			= 13,
	TIMER4_IRQ			= 14,
	TIMER5_IRQ			= 15,
	TIMER6_IRQ			= 16,
	TIMER7_IRQ			= 17,
	TIMER8_IRQ			= 18,
	TIMER9_IRQ			= 19,
	TIMER10_IRQ			= 20,
	TIMER11_IRQ			= 21,
	PMC_TIMER0_IRQ			= 22,
	PMC_TIMER1_IRQ			= 23,
	UART0_IRQ			= 24,
	UART1_IRQ			= 25,
	UART2_BT_IRQ			= 26,
	UART_LOG_IRQ			= 27,
	GPIOA_IRQ			= 28,
	GPIOB_IRQ			= 29,
	I2C0_IRQ			= 30,
	I2C1_IRQ			= 31,
	CTOUCH_IRQ			= 32,
	GDMA0_CHANNEL0_IRQ			= 33,
	GDMA0_CHANNEL1_IRQ			= 34,
	GDMA0_CHANNEL2_IRQ			= 35,
	GDMA0_CHANNEL3_IRQ			= 36,
	GDMA0_CHANNEL4_IRQ			= 37,
	GDMA0_CHANNEL5_IRQ			= 38,
	GDMA0_CHANNEL6_IRQ			= 39,
	GDMA0_CHANNEL7_IRQ			= 40,
	PPE_IRQ			= 41,
	SPI0_IRQ			= 42,
	SPI1_IRQ			= 43,
	SPORT0_IRQ			= 44,
	SPORT1_IRQ			= 45,
	RTC_IRQ			= 46,
	ADC_IRQ			= 47,
	ADC_COMP_IRQ			= 48,
	BOR_IRQ			= 49,
	PWR_DOWN_IRQ			= 50,
	SPI_FLASH_IRQ			= 51,
	KEYSCAN_IRQ			= 52,
	RSIP_IRQ			= 53,
	AES_IRQ			= 54,
	SHA_IRQ			= 55,
	PSRAMC_IRQ			= 56,
	TRNG_IRQ			= 57,
	AES_S_IRQ			= 58,
	SHA_S_IRQ			= 59,
	AON_TIM_IRQ			= 60,
	AON_WAKEPIN_IRQ			= 61,
	LEDC_IRQ			= 62,
	IR_IRQ			= 63,
	SDIO_IRQ			= 64,
	KM4_NS_WDG_IRQ			= 65,
	KM4_S_WDG_IRQ			= 66,
	QSPI_INT_IRQ			= 67,
	USB_INT_IRQ			= 68,
	OCP_INT_IRQ			= 69,
} IRQn_Type;
/**
  * @}
  */
#endif

#if defined (CONFIG_ARM_CORE_CM0)
/** @defgroup KM0IRQn_enum
  * @{
  */
typedef enum KM0IRQn {
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
	KM4_WAKE_IRQ			= 5,
	BT_WAKE_HOST_IRQ			= 6,
	RXI300_IRQ			= 7,
	IPC_KM0_IRQ			= 8,
	IWDG_IRQ			= 9,
	TIMER0_IRQ			= 10,
	TIMER1_IRQ			= 11,
	TIMER2_IRQ			= 12,
	TIMER3_IRQ			= 13,
	TIMER4_IRQ			= 14,
	TIMER5_IRQ			= 15,
	TIMER6_IRQ			= 16,
	TIMER7_IRQ			= 17,
	TIMER8_IRQ			= 18,
	TIMER9_IRQ			= 19,
	TIMER10_IRQ			= 20,
	TIMER11_IRQ			= 21,
	PMC_TIMER0_IRQ			= 22,
	PMC_TIMER1_IRQ			= 23,
	UART0_IRQ			= 24,
	UART1_IRQ			= 25,
	UART2_BT_IRQ			= 26,
	UART_LOG_IRQ			= 27,
	GPIOA_IRQ			= 28,
	GPIOB_IRQ			= 29,
	I2C0_IRQ			= 30,
	I2C1_IRQ			= 31,
	CTOUCH_IRQ			= 32,
	GDMA0_CHANNEL0_IRQ			= 33,
	GDMA0_CHANNEL1_IRQ			= 34,
	GDMA0_CHANNEL2_IRQ			= 35,
	GDMA0_CHANNEL3_IRQ			= 36,
	GDMA0_CHANNEL4_IRQ			= 37,
	GDMA0_CHANNEL5_IRQ			= 38,
	GDMA0_CHANNEL6_IRQ			= 39,
	GDMA0_CHANNEL7_IRQ			= 40,
	PPE_IRQ			= 41,
	SPI0_IRQ			= 42,
	SPI1_IRQ			= 43,
	SPORT0_IRQ			= 44,
	SPORT1_IRQ			= 45,
	RTC_IRQ			= 46,
	ADC_IRQ			= 47,
	ADC_COMP_IRQ			= 48,
	BOR_IRQ			= 49,
	PWR_DOWN_IRQ			= 50,
	SPI_FLASH_IRQ			= 51,
	KEYSCAN_IRQ			= 52,
	RSIP_IRQ			= 53,
	AES_IRQ			= 54,
	SHA_IRQ			= 55,
	PSRAMC_IRQ			= 56,
	TRNG_IRQ			= 57,
	AES_S_IRQ			= 58,
	SHA_S_IRQ			= 59,
	AON_TIM_IRQ			= 60,
	AON_WAKEPIN_IRQ			= 61,
	LEDC_IRQ			= 62,
	IR_IRQ			= 63,
	SDIO_IRQ			= 64,
	KM0_WDG_IRQ			= 65,
	USB_INT_IRQ			= 66,
} IRQn_Type;
/**
  * @}
  */
#endif

/**
  * @}
  */

/** @defgroup IRQn_Priority_def
  * @{
  */
#ifdef CONFIG_ARM_CORE_CM4
enum IRQnPriority {
	INT_PRI0 = 0,				//KM4 Priority value 0 is the highest active priority
	INT_PRI_HIGHEST	= INT_PRI0,
	INT_PRI1,					//lower numbers have higher priority in KM4
	INT_PRI2,
	INT_PRI_HIGH	= INT_PRI2,
	INT_PRI3,
	INT_PRI4,
	INT_PRI_MIDDLE	= INT_PRI4,
	INT_PRI5,
	INT_PRI6,
	INT_PRI7,
	INT_PRI_LOWEST	= INT_PRI7,
};
#else
enum IRQnPriority {
	INT_PRI0 = 0,
	INT_PRI_HIGHEST = INT_PRI0,		//KM0 Priority
	INT_PRI1,
	INT_PRI_HIGH	= INT_PRI1,
	INT_PRI2,
	INT_PRI_MIDDLE	= INT_PRI2,
	INT_PRI3,
	INT_PRI_LOWEST	= INT_PRI3,		//KM0 only have 4 priority level
};
#endif
/**
  * @}
  */

#endif //_AMEBA_VECTOR_TABLE_H_
