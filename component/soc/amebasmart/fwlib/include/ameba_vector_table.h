/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_VECTOR_TABLE_H_
#define _AMEBA_VECTOR_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

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
	TIMER0_IRQ			= 0,
	TIMER1_IRQ			= 1,
	TIMER2_IRQ			= 2,
	TIMER3_IRQ			= 3,
	TIMER4_IRQ			= 4,
	TIMER5_IRQ			= 5,
	TIMER6_IRQ			= 6,
	TIMER7_IRQ			= 7,
	UART_LOG_IRQ		= 8,
	GPIOA_IRQ			= 9,
	GPIOB_IRQ			= 10,
	GPIOC_IRQ			= 11,
	RTC_IRQ				= 12,
	CTOUCH_IRQ			= 13,
	ADC_IRQ				= 14,
	ADC_COMP_IRQ		= 15,
	BOR_IRQ				= 16,
	PWR_DOWN_IRQ		= 17,
	VADBT_OR_VADPC_IRQ	= 18,
	SPI_FLASH_IRQ		= 19,
	AP_WAKE_IRQ_LP_IRQ	= 20,
	THERMAL_IRQ			= 21,
	I2C0_IRQ			= 22,
	RSIP_IRQ			= 23,
	IPC_NP_IRQ			= 24,
	GDMA0_CHANNEL0_IRQ	= 25,
	GDMA0_CHANNEL1_IRQ	= 26,
	GDMA0_CHANNEL2_IRQ	= 27,
	GDMA0_CHANNEL3_IRQ	= 28,
	GDMA0_CHANNEL4_IRQ	= 29,
	GDMA0_CHANNEL5_IRQ	= 30,
	GDMA0_CHANNEL6_IRQ	= 31,
	GDMA0_CHANNEL7_IRQ	= 32,
	LCDC_IRQ			= 33,
	WL_DMA_IRQ			= 34,
	WL_PROTOCOL_IRQ		= 35,
	AES_IRQ				= 36,
	SHA_IRQ				= 37,
	SDIO_HOST_IRQ		= 38,
	USB_OTG_IRQ			= 39,
	SPI0_IRQ			= 40,
	SPI1_IRQ			= 41,
	RSVD_IRQ			= 42,
	RSA_IRQ				= 43,
	ECDSA_IRQ			= 44,
	ED25519_IRQ			= 45,
	PSRAMC_IRQ			= 46,
	DDR_IRQ				= 47,
	RXI300_HP_IRQ		= 48,
	IR_IRQ				= 49,
	UART0_IRQ			= 50,
	UART1_IRQ			= 51,
	UART2_IRQ			= 52,
	UART3_BT_IRQ		= 53,
	TRNG_IRQ			= 54,
	I2C1_IRQ			= 55,
	I2C2_IRQ			= 56,
	TIMER8_IRQ			= 57,
	TIMER9_IRQ			= 58,
	WDG1_IRQ			= 59,
	WDG2_IRQ			= 60,
	WDG3_IRQ			= 61,
	WDG4_IRQ			= 62,
	SPORT0_IRQ			= 63,
	SPORT1_IRQ			= 64,
	SPORT2_IRQ			= 65,
	SPORT3_IRQ			= 66,
	BT_SCB_IRQ			= 67,
	LEDC_IRQ			= 68,
	MIPI_DSI_IRQ		= 69,
	AES_S_IRQ			= 70,
	SHA_S_IRQ			= 71,
	AON_WAKEPIN_IRQ		= 72,
	ZIGBEE_IRQ			= 73,
	BT_WAKE_HOST_IRQ	= 74,
	TIMER10_IRQ			= 75,
	TIMER11_IRQ			= 76,
	TIMER12_IRQ			= 77,
	TIMER13_IRQ			= 78,
	AON_TIM_IRQ			= 79,
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
	IPC_IRQ					= 0,
	RXI300_IRQ				= 1,
	WIFI_FISR_FESR_IRQ		= 2,
	WIFI_FTSR_MAILBOX_IRQ	= 3,
	AON_TIM_IRQ				= 4,
	NP_WAKE_IRQ				= 5,
	AP_WAKE_IRQ				= 6,
	WDG0_IRQ				= 7,
	GDMA0_CHANNEL2_IRQ		= 8,
	GDMA0_CHANNEL3_IRQ		= 9,
	TIMER2_IRQ				= 10,
	TIMER3_IRQ				= 11,
	TIMER4_IRQ				= 12,
	TIMER5_IRQ				= 13,
	TIMER6_IRQ				= 14,
	TIMER7_IRQ				= 15,
	UART_LOG_IRQ			= 16,
	GPIOA_IRQ				= 17,
	GPIOB_IRQ				= 18,
	GPIOC_IRQ				= 19,
	RTC_IRQ					= 20,
	CTOUCH_IRQ				= 21,
	ADC_IRQ					= 22,
	ADC_COMP_IRQ			= 23,
	BOR_IRQ					= 24,
	PWR_DOWN_IRQ			= 25,
	VADBT_OR_VADPC_IRQ		= 26,
	SPI_FLASH_IRQ			= 27,
	DEBUG_TIMER_IRQ			= 28,
	THERMAL_IRQ				= 29,
	I2C0_IRQ				= 30,
	RSIP_IRQ				= 31,
	AON_WAKEPIN_IRQ			= 32,
	BT_WAKE_HOST_IRQ		= 33,
	NFIQOUT0_OR_NIRQOUT0_IRQ	= 34,
	NFIQOUT1_OR_NIRQOUT1_IRQ	= 35,
	WL_DMA_IRQ				= 36,
	WL_PROTOCOL_IRQ			= 37,
	TIMER10_IRQ				= 38,
	TIMER11_IRQ				= 39,
	TIMER12_IRQ				= 40,
	TIMER13_IRQ				= 41,
	I2C1_IRQ				= 42,
	I2C2_IRQ				= 43,

} IRQn_Type;
/**
  * @}
  */
#endif
#if defined (CONFIG_ARM_CORE_CA32)
#if defined(__ZEPHYR__) || defined(CONFIG_PLATFORM_ZEPHYR)
#define GIC_SPI_OFFSET	32
#else
#define GIC_SPI_OFFSET	0
#endif
/** @defgroup CA32IRQn_enum
  * @{
  */
typedef enum CA32IRQn {
	/******  Processor Exceptions Numbers ********/
	SGI0_Edge	=	-32 + GIC_SPI_OFFSET,	/* software generated interrupt 0 (recommended non-secure) */
	SGI1_Edge	=	-31 + GIC_SPI_OFFSET,	/* software generated interrupt 1 (recommended non-secure) */
	SGI2_Edge	=	-30 + GIC_SPI_OFFSET,	/* software generated interrupt 2 (recommended non-secure) */
	SGI3_Edge	=	-29 + GIC_SPI_OFFSET,	/* software generated interrupt 3 (recommended non-secure) */
	SGI4_Edge	=	-28 + GIC_SPI_OFFSET,	/* software generated interrupt 4 (recommended non-secure) */
	SGI5_Edge	=	-27 + GIC_SPI_OFFSET,	/* software generated interrupt 5 (recommended non-secure) */
	SGI6_Edge	=	-26 + GIC_SPI_OFFSET,	/* software generated interrupt 6 (recommended non-secure) */
	SGI7_Edge	=	-25 + GIC_SPI_OFFSET,	/* software generated interrupt 7 (recommended non-secure) */
	SGI8_Edge	=	-24 + GIC_SPI_OFFSET,	/* software generated interrupt 8 (recommended secure) */
	SGI9_Edge	=	-23 + GIC_SPI_OFFSET,	/* software generated interrupt 9 (recommended secure) */
	SGI10_Edge	=	-22 + GIC_SPI_OFFSET,	/* software generated interrupt 10 (recommended secure) */
	SGI11_Edge	=	-21 + GIC_SPI_OFFSET,	/* software generated interrupt 11 (recommended secure) */
	SGI12_Edge	=	-20 + GIC_SPI_OFFSET,	/* software generated interrupt 12 (recommended secure) */
	SGI13_Edge	=	-19 + GIC_SPI_OFFSET,	/* software generated interrupt 13 (recommended secure) */
	SGI14_Edge	=	-18 + GIC_SPI_OFFSET,	/* software generated interrupt 14 (recommended secure) */
	SGI15_Edge	=	-17 + GIC_SPI_OFFSET,	/* software generated interrupt 15 (recommended secure) */

	PPI6_IRQ	=	-7  + GIC_SPI_OFFSET,	/* Virtual maintenance interrupt. */
	PPI5_IRQ	=	-6  + GIC_SPI_OFFSET,	/* Hypervisor timer event. */
	PPI4_IRQ	=	-5  + GIC_SPI_OFFSET,	/* Virtual timer event. */
	PPI0_IRQ	=	-4  + GIC_SPI_OFFSET,	/* Legacy nFIQ signal. Not used. */
	PPI1_IRQ	=	-3  + GIC_SPI_OFFSET,	/* Secure physical timer event. */
	PPI2_IRQ	=	-2  + GIC_SPI_OFFSET,	/* Non-secure physical timer event. */
	PPI3_IRQ	=	-1  + GIC_SPI_OFFSET,	/* Legacy nIRQ signal. Not used. */

	/******  Specific Interrupt Numbers ************/
	AON_TIM_IRQ			= 95 + GIC_SPI_OFFSET,
	TIMER0_IRQ			=  0 + GIC_SPI_OFFSET,
	TIMER1_IRQ			=  1 + GIC_SPI_OFFSET,
	TIMER2_IRQ			=  2 + GIC_SPI_OFFSET,
	TIMER3_IRQ			=  3 + GIC_SPI_OFFSET,
	TIMER4_IRQ			=  4 + GIC_SPI_OFFSET,
	TIMER5_IRQ			=  5 + GIC_SPI_OFFSET,
	TIMER6_IRQ			=  6 + GIC_SPI_OFFSET,
	TIMER7_IRQ			=  7 + GIC_SPI_OFFSET,
	UART_LOG_IRQ			=  8 + GIC_SPI_OFFSET,
	GPIOA_IRQ			=  9 + GIC_SPI_OFFSET,
	GPIOB_IRQ			= 10 + GIC_SPI_OFFSET,
	GPIOC_IRQ			= 11 + GIC_SPI_OFFSET,
	RTC_IRQ				= 12 + GIC_SPI_OFFSET,
	CTOUCH_IRQ			= 13 + GIC_SPI_OFFSET,
	ADC_IRQ				= 14 + GIC_SPI_OFFSET,
	ADC_COMP_IRQ			= 15 + GIC_SPI_OFFSET,
	BOR_IRQ				= 16 + GIC_SPI_OFFSET,
	PWR_DOWN_IRQ			= 17 + GIC_SPI_OFFSET,
	VADBT_OR_VADPC_IRQ		= 18 + GIC_SPI_OFFSET,
	SPI_FLASH_IRQ			= 19 + GIC_SPI_OFFSET,
	DEBUG_TIMER_IRQ			= 20 + GIC_SPI_OFFSET,
	THERMAL_IRQ			= 21 + GIC_SPI_OFFSET,
	I2C0_IRQ			= 22 + GIC_SPI_OFFSET,
	RSIP_IRQ			= 23 + GIC_SPI_OFFSET,
	IPC_AP_IRQ			= 24 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL0_IRQ		= 25 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL1_IRQ		= 26 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL2_IRQ		= 27 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL3_IRQ		= 28 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL4_IRQ		= 29 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL5_IRQ		= 30 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL6_IRQ		= 31 + GIC_SPI_OFFSET,
	GDMA0_CHANNEL7_IRQ		= 32 + GIC_SPI_OFFSET,
	LCDC_IRQ			= 33 + GIC_SPI_OFFSET,
	WL_DMA_IRQ			= 34 + GIC_SPI_OFFSET,
	WL_PROTOCOL_IRQ			= 35 + GIC_SPI_OFFSET,
	AES_IRQ				= 36 + GIC_SPI_OFFSET,
	SHA_IRQ				= 37 + GIC_SPI_OFFSET,
	SDIO_HOST_IRQ			= 38 + GIC_SPI_OFFSET,
	USB_OTG_IRQ			= 39 + GIC_SPI_OFFSET,
	SPI0_IRQ			= 40 + GIC_SPI_OFFSET,
	SPI1_IRQ			= 41 + GIC_SPI_OFFSET,
	RSVD_IRQ			= 42 + GIC_SPI_OFFSET,
	RSA_IRQ				= 43 + GIC_SPI_OFFSET,
	ECDSA_IRQ			= 44 + GIC_SPI_OFFSET,
	ED25519_IRQ			= 45 + GIC_SPI_OFFSET,
	PSRAMC_IRQ			= 46 + GIC_SPI_OFFSET,
	DDR_IRQ				= 47 + GIC_SPI_OFFSET,
	RXI300_HP_IRQ			= 48 + GIC_SPI_OFFSET,
	IR_IRQ				= 49 + GIC_SPI_OFFSET,
	UART0_IRQ			= 50 + GIC_SPI_OFFSET,
	UART1_IRQ			= 51 + GIC_SPI_OFFSET,
	UART2_IRQ			= 52 + GIC_SPI_OFFSET,
	UART3_BT_IRQ			= 53 + GIC_SPI_OFFSET,
	TRNG_IRQ			= 54 + GIC_SPI_OFFSET,
	I2C1_IRQ			= 55 + GIC_SPI_OFFSET,
	I2C2_IRQ			= 56 + GIC_SPI_OFFSET,
	TIMER8_IRQ			= 57 + GIC_SPI_OFFSET,
	TIMER9_IRQ			= 58 + GIC_SPI_OFFSET,
	WDG1_IRQ			= 59 + GIC_SPI_OFFSET,
	WDG2_IRQ			= 60 + GIC_SPI_OFFSET,
	WDG3_IRQ			= 61 + GIC_SPI_OFFSET,
	WDG4_IRQ			= 62 + GIC_SPI_OFFSET,
	SPORT0_IRQ			= 63 + GIC_SPI_OFFSET,
	SPORT1_IRQ			= 64 + GIC_SPI_OFFSET,
	SPORT2_IRQ			= 65 + GIC_SPI_OFFSET,
	SPORT3_IRQ			= 66 + GIC_SPI_OFFSET,
	BT_SCB_IRQ			= 67 + GIC_SPI_OFFSET,
	LEDC_IRQ			= 68 + GIC_SPI_OFFSET,
	PMUIRQ0_IRQ			= 69 + GIC_SPI_OFFSET,
	MIPI_DSI_IRQ			= 70 + GIC_SPI_OFFSET,
	AXIERIRQ_IRQ			= 71 + GIC_SPI_OFFSET,
	AES_S_IRQ			= 72 + GIC_SPI_OFFSET,
	SHA_S_IRQ			= 73 + GIC_SPI_OFFSET,
	AON_WAKEPIN_IRQ			= 74 + GIC_SPI_OFFSET,
	ZIGBEE_IRQ			= 75 + GIC_SPI_OFFSET,
	BT_WAKE_HOST_IRQ		= 76 + GIC_SPI_OFFSET,
	CTIIRQ_IRQ			= 77 + GIC_SPI_OFFSET,
	TIMER10_IRQ			= 78 + GIC_SPI_OFFSET,
	TIMER11_IRQ			= 79 + GIC_SPI_OFFSET,
	TIMER12_IRQ			= 80 + GIC_SPI_OFFSET,
	TIMER13_IRQ			= 81 + GIC_SPI_OFFSET,
} IRQn_Type;
/**
  * @}
  */
#endif

/** @defgroup IRQn_Priority_def
  * @{
  */
#ifdef CONFIG_ARM_CORE_CM4
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
#elif defined (CONFIG_ARM_CORE_CM0)
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
#elif defined (CONFIG_ARM_CORE_CA32)
enum NS_IRQnPriority {
	INT_PRI0 = 0,
	INT_PRI_HIGHEST = INT_PRI0,		//CA32 Priority in NS World, lower numbers have higher priority
	INT_PRI1 = 0x10,				//valid bit in GICD_IPRIORITYRn is [7:3], But NS world can only use [7:4] for each field
	INT_PRI2 = 0x20,
	INT_PRI3 = 0x30,
	INT_PRI4 = 0x40,
	INT_PRI5 = 0x50,
	INT_PRI_HIGH	= INT_PRI5,
	INT_PRI6 = 0x60,
	INT_PRI7 = 0x70,
	INT_PRI8 = 0x80,
	INT_PRI9 = 0x90,
	INT_PRI10 = 0xA0,
	INT_PRI_MIDDLE	= INT_PRI10,
	INT_PRI11 = 0xB0,
	INT_PRI12 = 0xC0,
	INT_PRI13 = 0xD0,
	INT_PRI14 = 0xE0,
	INT_PRI15 = 0xF0,
	//INT_PRI15 shall not be used for The GIC always masks an interrupt that has the largest supported priority field value.
	INT_PRI_LOWEST	= INT_PRI14,
};
#endif
/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif //_AMEBA_VECTOR_TABLE_H_
