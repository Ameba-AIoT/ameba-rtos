/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _AMEBA_VECTOR_TABLE_H_
#define _AMEBA_VECTOR_TABLE_H_

/* Exported constants --------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup IRQ_Exported_Constants IRQ Exported Constants
  * @{
  */
/* AUTO_GEN_START */
/** @defgroup APIRQn_enum
  * @{
  */
#if defined (CONFIG_ARM_CORE_CM4_KM4TZ)
typedef enum APIRQn {
	/******  Processor Exceptions Numbers ********/
	NonMaskableInt_IRQn    			= -14,	/*!< 2 Non Maskable Interrupt */
	HardFault_IRQn         			= -13,	/*!< 3 Hard Fault, all classes of Fault */
	MemoryManagement_IRQn  			= -12,	/*!< 4 Memory Management Interrupt */
	BusFault_IRQn          			= -11,	/*!< 5 Bus Fault Interrupt */
	UsageFault_IRQn        			= -10,	/*!< 6 Usage Fault Interrupt */
	SecureFault_IRQn        		= -9,   /*!< 7 Secure Fault Interrupt */
	SVCall_IRQn            			= -5,	/*!< 11 SV Call Interrupt  */
	DebugMonitor_IRQn      			= -4,	/*!< 12 Debug Monitor Interrupt */
	PendSV_IRQn            			= -2,	/*!< 14 Pend SV Interrupt */
	SysTick_IRQn           			= -1,	/*!< 15 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	WIFI_FISR_FESR_IRQ  			= 0,
	WL_PROTOCOL_IRQ     			= 1,
	AP_WAKE_IRQ         			= 2,
	IPC_KM4TZ_IRQ       			= 3,
	IWDG_IRQ            			= 4,
	TIMER0_IRQ          			= 5,
	TIMER1_IRQ          			= 6,
	TIMER2_IRQ          			= 7,
	TIMER3_IRQ          			= 8,
	TIMER4_IRQ          			= 9,
	TIMER5_IRQ          			= 10,
	TIMER6_IRQ          			= 11,
	PMC_TIMER0_IRQ      			= 12,
	PMC_TIMER1_IRQ      			= 13,
	UART0_IRQ           			= 14,
	UART1_IRQ           			= 15,
	UART2_IRQ           			= 16,
	UART_LOG_IRQ        			= 17,
	GPIOA_IRQ           			= 18,
	RSVD_IRQ            			= 19,
	I2C0_IRQ            			= 20,
	I2C1_IRQ            			= 21,
	GDMA0_CHANNEL0_IRQ  			= 22,
	GDMA0_CHANNEL1_IRQ  			= 23,
	GDMA0_CHANNEL2_IRQ  			= 24,
	GDMA0_CHANNEL3_IRQ  			= 25,
	GDMA0_CHANNEL4_IRQ  			= 26,
	GDMA0_CHANNEL5_IRQ  			= 27,
	GDMA0_CHANNEL6_IRQ  			= 28,
	GDMA0_CHANNEL7_IRQ  			= 29,
	SPI0_IRQ            			= 30,
	SPI1_IRQ            			= 31,
	SPORT_IRQ           			= 32,
	RTC_IRQ             			= 33,
	ADC_IRQ             			= 34,
	BOR_IRQ             			= 35,
	PWR_DOWN_IRQ        			= 36,
	PKE_IRQ             			= 37,
	TRNG_IRQ            			= 38,
	AON_TIM_IRQ         			= 39,
	AON_WAKEPIN_IRQ     			= 40,
	SDIO_WIFI_IRQ       			= 41,
	SDIO_BT_IRQ         			= 42,
	RXI300_IRQ          			= 43,
	PSRAMC_IRQ          			= 44,
	SPI_FLASH_IRQ       			= 45,
	RSIP_IRQ            			= 46,
	AES_IRQ             			= 47,
	SHA_IRQ             			= 48,
	AES_S_IRQ           			= 49,
	SHA_S_IRQ           			= 50,
	KM4NS_WDG_RST_IRQ   			= 51,
	KM4TZ_NS_WDG_IRQ    			= 52,
	KM4TZ_S_WDG_IRQ     			= 53,
	OCP_IRQ             			= 54,
	BT_CTRL_HIGH_IRQ    			= 55,
	BT_CTRL_LOW_IRQ     			= 56,
	IR_IRQ              			= 57,
	PERI_IRQ_MAX        			= 58,
} IRQn_Type;
#endif
/**
  * @}
  */


/** @defgroup NPIRQn_enum
  * @{
  */
#if defined (CONFIG_ARM_CORE_CM4_KM4NS)
typedef enum NPIRQn {
	/******  Processor Exceptions Numbers ********/
	NonMaskableInt_IRQn    			= -14,	/*!< 2 Non Maskable Interrupt */
	HardFault_IRQn         			= -13,	/*!< 3 Hard Fault, all classes of Fault */
	MemoryManagement_IRQn  			= -12,	/*!< 4 Memory Management Interrupt */
	BusFault_IRQn          			= -11,	/*!< 5 Bus Fault Interrupt */
	UsageFault_IRQn        			= -10,	/*!< 6 Usage Fault Interrupt */
	SecureFault_IRQn        		= -9,   /*!< 7 Secure Fault Interrupt */
	SVCall_IRQn            			= -5,	/*!< 11 SV Call Interrupt  */
	DebugMonitor_IRQn      			= -4,	/*!< 12 Debug Monitor Interrupt */
	PendSV_IRQn            			= -2,	/*!< 14 Pend SV Interrupt */
	SysTick_IRQn           			= -1,	/*!< 15 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	WIFI_FISR_FESR_IRQ  			= 0,
	WL_PROTOCOL_IRQ     			= 1,
	AP_WAKE_IRQ         			= 2,
	IPC_KM4NS_IRQ       			= 3,
	IWDG_IRQ            			= 4,
	TIMER0_IRQ          			= 5,
	TIMER1_IRQ          			= 6,
	TIMER2_IRQ          			= 7,
	TIMER3_IRQ          			= 8,
	TIMER4_IRQ          			= 9,
	TIMER5_IRQ          			= 10,
	TIMER6_IRQ          			= 11,
	PMC_TIMER0_IRQ      			= 12,
	PMC_TIMER1_IRQ      			= 13,
	UART0_IRQ           			= 14,
	UART1_IRQ           			= 15,
	UART2_IRQ           			= 16,
	UART_LOG_IRQ        			= 17,
	GPIOA_IRQ           			= 18,
	RSVD_IRQ            			= 19,
	I2C0_IRQ            			= 20,
	I2C1_IRQ            			= 21,
	GDMA0_CHANNEL0_IRQ  			= 22,
	GDMA0_CHANNEL1_IRQ  			= 23,
	GDMA0_CHANNEL2_IRQ  			= 24,
	GDMA0_CHANNEL3_IRQ  			= 25,
	GDMA0_CHANNEL4_IRQ  			= 26,
	GDMA0_CHANNEL5_IRQ  			= 27,
	GDMA0_CHANNEL6_IRQ  			= 28,
	GDMA0_CHANNEL7_IRQ  			= 29,
	SPI0_IRQ            			= 30,
	SPI1_IRQ            			= 31,
	SPORT_IRQ           			= 32,
	RTC_IRQ             			= 33,
	ADC_IRQ             			= 34,
	BOR_IRQ             			= 35,
	PWR_DOWN_IRQ        			= 36,
	PKE_IRQ             			= 37,
	TRNG_IRQ            			= 38,
	AON_TIM_IRQ         			= 39,
	AON_WAKEPIN_IRQ     			= 40,
	SDIO_WIFI_IRQ       			= 41,
	SDIO_BT_IRQ         			= 42,
	RXI300_IRQ          			= 43,
	PSRAMC_IRQ          			= 44,
	SPI_FLASH_IRQ       			= 45,
	RSIP_IRQ            			= 46,
	AES_IRQ             			= 47,
	SHA_IRQ             			= 48,
	KM4NS_WDG_IRQ       			= 49,
	BT_CTRL_HIGH_IRQ    			= 50,
	BT_CTRL_LOW_IRQ     			= 51,
	IR_IRQ              			= 52,
	PERI_IRQ_MAX        			= 58,
} IRQn_Type;
#endif
/**
  * @}
  */


/* AUTO_GEN_END */
/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/
/** @defgroup IRQn_Priority_def
  * @{
  */
enum IRQnPriority {
	INT_PRI0 = 0,	//Priority value 0 is the highest active priority
	INT_PRI_HIGHEST	= INT_PRI0,
	INT_PRI1,		//lower numbers have higher priority.
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
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif //_AMEBA_VECTOR_TABLE_H_
