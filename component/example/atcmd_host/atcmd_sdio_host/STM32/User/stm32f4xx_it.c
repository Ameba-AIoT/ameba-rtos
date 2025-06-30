/*
* @attention
*
* <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "sdio_host_common.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{}

///**
//  * @brief  This function handles PendSV_Handler exception.
//  * @param  None
//  * @retval None
//  */
//void PendSV_Handler(void)
//{}



/**
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
*/
extern void xPortSysTickHandler(void);
//systick中断服务函数
void SysTick_Handler(void)
{
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
#endif  /* INCLUDE_xTaskGetSchedulerState */
		xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
	}
#endif  /* INCLUDE_xTaskGetSchedulerState */
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{}


unsigned char RxBuffer[256];
unsigned char RxBufferBackup[256];
uint32_t Uart1_Rx_Cnt = 0;
uint32_t Uart1_Rx_Cnt_Backup = 0;
extern rtw_semaphore_type_t uart_recv_sema;

void DEBUG_USART_IRQHandler(void)
{
	uint8_t ucTemp;
	if (USART_GetITStatus(DEBUG_USART, USART_IT_RXNE) != RESET) {
		ucTemp = USART_ReceiveData(DEBUG_USART);
		USART_SendData(DEBUG_USART, ucTemp);
	}

	if (Uart1_Rx_Cnt >= 254) { // copy before over flow
		Uart1_Rx_Cnt_Backup = Uart1_Rx_Cnt;
		memcpy(RxBufferBackup, RxBuffer, Uart1_Rx_Cnt);
		rtw_rtos_set_semaphore(&uart_recv_sema, RTW_TRUE);
		Uart1_Rx_Cnt = 0;
		memset(RxBuffer, 0x00, sizeof(RxBuffer));

	} else {
		RxBuffer[Uart1_Rx_Cnt++] = ucTemp;   //

		if ((RxBuffer[Uart1_Rx_Cnt - 1] == 0x0A) && (RxBuffer[Uart1_Rx_Cnt - 2] == 0x0D)) {
			Uart1_Rx_Cnt_Backup = Uart1_Rx_Cnt;
			memcpy(RxBufferBackup, RxBuffer, Uart1_Rx_Cnt);
			rtw_rtos_set_semaphore(&uart_recv_sema, RTW_TRUE);

			Uart1_Rx_Cnt = 0;
			memset(RxBuffer, 0x00, sizeof(RxBuffer));
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
