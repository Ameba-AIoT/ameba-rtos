/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "GPIO";

#define GPIO_IRQ_TABLE_MAX_NUM		0x4 /*!< GPIO_IRQ_TABLE_MAX_NUM */
struct GPIO_IrqContext GPIO_IrqTable[GPIO_IRQ_TABLE_MAX_NUM] = {0};

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup GPIO
* @brief GPIO driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup GPIO_Exported_Functions GPIO Exported Functions
  * @{
  */

/**
  * @brief  The common GPIO interrupt handler
  * @param  pData: the data pointer from the registered IRQ handler
  * @retval None
  */
u32 GPIO_INTHandler(void *pData)
{
	GPIO_TypeDef *GPIO = (GPIO_TypeDef *)pData;
	u32 port_x_in, IsrMapPin;
	u32 i, idx, event;
	u8 port_num;
	u8 pin_max = 32;
	u16 pin_name;

	port_x_in = GPIO->GPIO_EXT_PORT[0];
	IsrMapPin = GPIO->GPIO_INT_STATUS;

	/* Clear pending edge interrupt */
	GPIO->GPIO_INT_EOI = IsrMapPin;

	if (GPIO == GPIOA_BASE || GPIO == GPIOA_BASE_S) {
		port_num = GPIO_PORT_A;
	} else if (GPIO == GPIOB_BASE || GPIO == GPIOB_BASE_S) {
		port_num = GPIO_PORT_B;
	} else if (GPIO == GPIOC_BASE || GPIO == GPIOC_BASE_S) {
		port_num = GPIO_PORT_C;

		/* PortC: PinNum 0 ~ 29 */
		pin_max = 30;
	} else {
		RTK_LOGW(TAG, "Unknown GPIO_PORT: 0x%08x \r\n", GPIO);

		return 0xFFFFFFFF;
	}

	for (i = 0; i < pin_max; i++) {
		if (IsrMapPin & BIT(i)) {
			pin_name = PinName(port_num, i);

			for (idx = 0; idx < GPIO_IRQ_TABLE_MAX_NUM; idx++) {
				/* Search UsrIrqHandler which match the IrqPinName in IrqTable */
				if (pin_name == GPIO_IrqTable[idx].IrqPinName) {
					if ((GPIO_IrqTable[idx].IrqIdleEntryFlag) == GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER) {/* fix for entry PinName initial value is 0x0 */
						if (GPIO_IrqTable[idx].UsrIrqHandler != NULL) {
							/* register irq event to pass it to UsrIrqHandler */
							if (port_x_in & BIT(i)) {
								event = (HAL_IRQ_RISE | (i << 16) | (port_num << 21)); /* (event | port_pin << 16 */
							} else {
								event = (HAL_IRQ_FALL | (i << 16) | (port_num << 21)); /* (event | port_pin << 16 */
							}

							GPIO_IrqTable[idx].UsrIrqHandler(GPIO_IrqTable[idx].UsrIrqData, event);
						} else {
							RTK_LOGW(TAG, "Assert Port[%d]_Pin[%d] interrupt, but entry[%d] UsrIrqHandler is NULL \r\n", port_num, i, idx);
						}
					}
				}
			}
		}
	}

	return 0;
}

/**
  * @brief  To register a user interrupt handler for a specified pin
  * @param  GPIO_Pin: GPIO pin num from PinName.
  * @param  IrqHandler: The IRQ handler to be assigned to the specified pin
  * @param  IrqData: The pointer will be pass the the IRQ handler
  *
  * @retval None
  */
void GPIO_UserRegIrq(u32 GPIO_Pin, void *IrqHandler, void *IrqData)
{
	u32 idx;

	/* step1: check whether the GPIO_Pin has owned one entry */
	for (idx = 0; idx < GPIO_IRQ_TABLE_MAX_NUM; idx++) {
		if ((GPIO_Pin == (u32)(GPIO_IrqTable[idx].IrqPinName)) && (GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER == (GPIO_IrqTable[idx].IrqIdleEntryFlag))) {
			/* found and reuse this entry */
			break;
		}
	}

	/* step2: find an idle entry when the the GPIO_Pin hasn't owned one entry */
	if (idx == GPIO_IRQ_TABLE_MAX_NUM) {
		for (idx = 0; idx < GPIO_IRQ_TABLE_MAX_NUM; idx++) {
			if ((GPIO_IrqTable[idx].IrqIdleEntryFlag) != GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER) {
				/* found an idle entry */
				break;
			}
		}
	}

	/* step3: register info to IrqTable for idle or reused entry */
	if (idx != GPIO_IRQ_TABLE_MAX_NUM) {
		GPIO_IrqTable[idx].IrqPinName = (u16)GPIO_Pin;
		GPIO_IrqTable[idx].UsrIrqHandler = (GPIO_IRQ_FUN)IrqHandler;
		GPIO_IrqTable[idx].UsrIrqData = IrqData;

		GPIO_IrqTable[idx].IrqIdleEntryFlag = GPIO_IRQ_ENTRY_USED_MAGIC_NUMBER;
	} else {
		RTK_LOGE(TAG, "No idle entry in GPIO_IrqTable, please enlarge GPIO_IRQ_TABLE_MAX_NUM !!! \r\n");
	}
}

/**
  * @brief  To Unregister a user interrupt handler for a specified pin
  *
  * @retval None
  */
void GPIO_UserUnRegIrq(u32 GPIO_Pin)
{
	GPIO_TypeDef *GPIO = NULL;
	u8 port_num, pin_num;
	u32 idx, RegValue;

	port_num = PORT_NUM(GPIO_Pin);
	pin_num = PIN_NUM(GPIO_Pin);

	if (TrustZone_IsSecure()) {
		GPIO = GPIO_PORTx_S[port_num];
	} else {
		GPIO = GPIO_PORTx[port_num];
	}

	/* step1: Disable Interrupt */
	RegValue =  GPIO->GPIO_INT_EN;
	RegValue &= ~(1 << pin_num);
	GPIO->GPIO_INT_EN = RegValue;

	/* step2: Clear pending interrupt */
	GPIO->GPIO_INT_EOI = (1 << pin_num);

	/* step3: UnRegister IrqTable */
	for (idx = 0; idx < GPIO_IRQ_TABLE_MAX_NUM; idx++) {
		if (GPIO_Pin == (u32)(GPIO_IrqTable[idx].IrqPinName)) {
			GPIO_IrqTable[idx].IrqPinName = 0x0;
			GPIO_IrqTable[idx].UsrIrqHandler = (GPIO_IRQ_FUN)NULL;
			GPIO_IrqTable[idx].UsrIrqData = NULL;

			GPIO_IrqTable[idx].IrqIdleEntryFlag = 0x0;

			break;
		}
	}

}
/**
  * @}
  */

/** @} */

/** @} */