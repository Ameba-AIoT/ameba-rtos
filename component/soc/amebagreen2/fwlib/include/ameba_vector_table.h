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
/* AUTO_GEN_START */
/** @defgroup APIRQn_enum
  * @{
  */
#if defined (CONFIG_ARM_CORE_CM4_KM4TZ)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum APIRQn {
	/******  Processor Exceptions Numbers ********/
	NonMaskableInt_IRQn    			= -14,	/*!< 2 Non Maskable Interrupt */
	HardFault_IRQn         			= -13,	/*!< 3 Hard Fault, all classes of Fault */
	MemoryManagement_IRQn  			= -12,	/*!< 4 Cortex-M3 Memory Management Interrupt */
	BusFault_IRQn          			= -11,	/*!< 5 Cortex-M3 Bus Fault Interrupt */
	UsageFault_IRQn        			= -10,	/*!< 6 Cortex-M3 Usage Fault Interrupt */
	SVCall_IRQn            			= -5,	/*!< 11 Cortex-M3 SV Call Interrupt  */
	DebugMonitor_IRQn      			= -4,	/*!< 12 Cortex-M3 Debug Monitor Interrupt */
	PendSV_IRQn            			= -2,	/*!< 14 Cortex-M3 Pend SV Interrupt */
	SysTick_IRQn           			= -1,	/*!< 15 Cortex-M3 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	WIFI_FISR_FESR_IRQ  			= 0,
	WIFI_FTSR_MAILBOX_IRQ			= 1,
	WL_DMA_IRQ          			= 2,
	WL_PROTOCOL_IRQ     			= 3,
	AP_WAKE_IRQ         			= 4,
	IPC_CPU0_IRQ        			= 5,
	IWDG_IRQ            			= 6,
	TIMER0_IRQ          			= 7,
	TIMER1_IRQ          			= 8,
	TIMER2_IRQ          			= 9,
	TIMER3_IRQ          			= 10,
	TIMER4_IRQ          			= 11,
	TIMER5_IRQ          			= 12,
	TIMER6_IRQ          			= 13,
	TIMER7_IRQ          			= 14,
	TIMER8_IRQ          			= 15,
	COEX_MAILBOX_IRQ    			= 16,
	RSVD_IRQ            			= 17,
	PMC_TIMER0_IRQ      			= 18,
	PMC_TIMER1_IRQ      			= 19,
	UART0_IRQ           			= 20,
	UART1_IRQ           			= 21,
	UART2_IRQ           			= 22,
	UART3_IRQ           			= 23,
	UART_LOG_IRQ        			= 24,
	GPIOA_IRQ           			= 25,
	GPIOB_IRQ           			= 26,
	GPIOC_IRQ           			= 27,
	I2C0_IRQ            			= 28,
	I2C1_IRQ            			= 29,
	GDMA0_CHANNEL0_IRQ  			= 30,
	GDMA0_CHANNEL1_IRQ  			= 31,
	GDMA0_CHANNEL2_IRQ  			= 32,
	GDMA0_CHANNEL3_IRQ  			= 33,
	GDMA0_CHANNEL4_IRQ  			= 34,
	GDMA0_CHANNEL5_IRQ  			= 35,
	GDMA0_CHANNEL6_IRQ  			= 36,
	GDMA0_CHANNEL7_IRQ  			= 37,
	SPI0_IRQ            			= 38,
	SPI1_IRQ            			= 39,
	SPORT0_IRQ          			= 40,
	RTC_IRQ             			= 41,
	ADC_IRQ             			= 42,
	ADC_COMP_IRQ        			= 43,
	CAPTOUCH_IRQ        			= 44,
	THERMAL_IRQ         			= 45,
	BOR_IRQ             			= 46,
	PWR_DOWN_IRQ        			= 47,
	RMII_IRQ            			= 48,
	LCDC_IRQ            			= 49,
	MJPEG_IRQ           			= 50,
	PPE_IRQ             			= 51,
	PKE_IRQ             			= 52,
	TRNG_IRQ            			= 53,
	AON_TIM_IRQ         			= 54,
	AON_WAKEPIN_IRQ     			= 55,
	SDIO_WIFI_IRQ       			= 56,
	SDIO_BT_IRQ         			= 57,
	SDIO_HOST_IRQ       			= 58,
	USB_IRQ             			= 59,
	A2C0_IRQ            			= 60,
	A2C1_IRQ            			= 61,
	IR_IRQ              			= 62,
	RXI300_IRQ          			= 63,
	PSRAMC_IRQ          			= 64,
	SPI_FLASH_IRQ       			= 65,
	RSIP_IRQ            			= 66,
	AES_IRQ             			= 67,
	SHA_IRQ             			= 68,
	CPU0_NS_WDG_IRQ     			= 69,
	CPU0_S_WDG_IRQ      			= 70,
	OCP_IRQ             			= 71,
	SPIC_ECC_IRQ        			= 72,
	UVC_DEC_IRQ         			= 73,
	RTC_DET_IRQ         			= 74,
	BT_MAILBOX_IRQ      			= 75,
	BT_SCB_IRQ          			= 76,
	BT_WAKE_HOST_IRQ    			= 77,
	CPU1_WDG_RST_IRQ    			= 78,
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
	MemoryManagement_IRQn  			= -12,	/*!< 4 Cortex-M3 Memory Management Interrupt */
	BusFault_IRQn          			= -11,	/*!< 5 Cortex-M3 Bus Fault Interrupt */
	UsageFault_IRQn        			= -10,	/*!< 6 Cortex-M3 Usage Fault Interrupt */
	SVCall_IRQn            			= -5,	/*!< 11 Cortex-M3 SV Call Interrupt  */
	DebugMonitor_IRQn      			= -4,	/*!< 12 Cortex-M3 Debug Monitor Interrupt */
	PendSV_IRQn            			= -2,	/*!< 14 Cortex-M3 Pend SV Interrupt */
	SysTick_IRQn           			= -1,	/*!< 15 Cortex-M3 System Tick Interrupt */

	/******  Specific Interrupt Numbers ************/
	WIFI_FISR_FESR_IRQ  			= 0,
	WIFI_FTSR_MAILBOX_IRQ			= 1,
	WL_DMA_IRQ          			= 2,
	WL_PROTOCOL_IRQ     			= 3,
	AP_WAKE_IRQ         			= 4,
	IPC_CPU1_IRQ        			= 5,
	IWDG_IRQ            			= 6,
	TIMER0_IRQ          			= 7,
	TIMER1_IRQ          			= 8,
	TIMER2_IRQ          			= 9,
	TIMER3_IRQ          			= 10,
	TIMER4_IRQ          			= 11,
	TIMER5_IRQ          			= 12,
	TIMER6_IRQ          			= 13,
	TIMER7_IRQ          			= 14,
	TIMER8_IRQ          			= 15,
	COEX_MAILBOX_IRQ    			= 16,
	RSVD_IRQ            			= 17,
	PMC_TIMER0_IRQ      			= 18,
	PMC_TIMER1_IRQ      			= 19,
	UART0_IRQ           			= 20,
	UART1_IRQ           			= 21,
	UART2_IRQ           			= 22,
	UART3_IRQ           			= 23,
	UART_LOG_IRQ        			= 24,
	GPIOA_IRQ           			= 25,
	GPIOB_IRQ           			= 26,
	GPIOC_IRQ           			= 27,
	I2C0_IRQ            			= 28,
	I2C1_IRQ            			= 29,
	GDMA0_CHANNEL0_IRQ  			= 30,
	GDMA0_CHANNEL1_IRQ  			= 31,
	GDMA0_CHANNEL2_IRQ  			= 32,
	GDMA0_CHANNEL3_IRQ  			= 33,
	GDMA0_CHANNEL4_IRQ  			= 34,
	GDMA0_CHANNEL5_IRQ  			= 35,
	GDMA0_CHANNEL6_IRQ  			= 36,
	GDMA0_CHANNEL7_IRQ  			= 37,
	SPI0_IRQ            			= 38,
	SPI1_IRQ            			= 39,
	SPORT0_IRQ          			= 40,
	RTC_IRQ             			= 41,
	ADC_IRQ             			= 42,
	ADC_COMP_IRQ        			= 43,
	CAPTOUCH_IRQ        			= 44,
	THERMAL_IRQ         			= 45,
	BOR_IRQ             			= 46,
	PWR_DOWN_IRQ        			= 47,
	RMII_IRQ            			= 48,
	LCDC_IRQ            			= 49,
	MJPEG_IRQ           			= 50,
	PPE_IRQ             			= 51,
	PKE_IRQ             			= 52,
	TRNG_IRQ            			= 53,
	AON_TIM_IRQ         			= 54,
	AON_WAKEPIN_IRQ     			= 55,
	SDIO_WIFI_IRQ       			= 56,
	SDIO_BT_IRQ         			= 57,
	SDIO_HOST_IRQ       			= 58,
	USB_IRQ             			= 59,
	A2C0_IRQ            			= 60,
	A2C1_IRQ            			= 61,
	IR_IRQ              			= 62,
	RXI300_IRQ          			= 63,
	PSRAMC_IRQ          			= 64,
	SPI_FLASH_IRQ       			= 65,
	RSIP_IRQ            			= 66,
	AES_IRQ             			= 67,
	SHA_IRQ             			= 68,
	CPU1_WDG_IRQ        			= 69,
	BT_MAILBOX_IRQ      			= 70,
	BT_SCB_IRQ          			= 71,
	BT_WAKE_HOST_IRQ    			= 72,
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
