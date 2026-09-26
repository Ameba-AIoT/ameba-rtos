#include "stm32f4xx_rcc.h"
#include <string.h>
#include "sdio_host.h"
#include "FreeRTOSConfig.h"

#define F0_WORKING_TIMEOUT_MS (500)

/* GPIO peripheral clocks */

rtw_result_t rtw_sdio_cmd52(rtw_sdio_transfer_direction_t direction, rtw_sdio_function_t function, uint32_t address, uint8_t value,
							rtw_sdio_response_needed_t response_expected, uint8_t *response)
{
	uint32_t sdio_response;
	rtw_result_t result;
	sdio_cmd_argument_t arg;

	arg.value = 0;
	arg.cmd52.function_number = (unsigned int)(function & 0x7);
	arg.cmd52.register_address = (unsigned int)(address & 0x00001ffff);
	arg.cmd52.rw_flag = (unsigned int)((direction == BUS_WRITE) ? 1 : 0);
	arg.cmd52.write_data = value;
	result = stm32_sdio_transfer(direction, SDIO_CMD_52, SDIO_BYTE_MODE, SDIO_1B_BLOCK, arg.value, 0, 0, response_expected, &sdio_response);
	if (response != NULL) {
		*response = (uint8_t)(sdio_response & 0x00000000ff);
	}
	return result;
}

rtw_result_t rtw_sdio_cmd53(rtw_sdio_transfer_direction_t direction, rtw_sdio_function_t function, rtw_sdio_transfer_mode_t mode, uint32_t address,
							uint16_t data_size, uint8_t *data, rtw_sdio_response_needed_t response_expected, uint32_t *response)
{
	sdio_cmd_argument_t arg;
	rtw_result_t result;

	arg.value = 0;
	arg.cmd53.function_number = (unsigned int)(function & BUS_FUNCTION_MASK);
	arg.cmd53.register_address = (unsigned int)(address & BIT_MASK(17));
	arg.cmd53.op_code = (unsigned int)1;
	arg.cmd53.rw_flag = (unsigned int)((direction == BUS_WRITE) ? 1 : 0);

	if (mode == SDIO_BYTE_MODE) {
		rtw_assert("rtw_sdio_cmd53: data_size > 512 for byte mode", (data_size <= (uint16_t)512));
		arg.cmd53.count = (unsigned int)(data_size & 0x1FF);
	} else {
		arg.cmd53.count = (unsigned int)((data_size / (uint16_t)SDIO_64B_BLOCK) & BIT_MASK(9));
		if ((uint32_t)(arg.cmd53.count * (uint16_t)SDIO_64B_BLOCK) < data_size) {
			++arg.cmd53.count;
		}
		arg.cmd53.block_mode = (unsigned int)1;
	}

	result = stm32_sdio_transfer(direction, SDIO_CMD_53, mode, SDIO_64B_BLOCK, arg.value, (uint32_t *)data, data_size, response_expected, response);
	if (result != RTW_SUCCESS) {
		goto done;
	}

done:
	return result;
}

rtw_result_t rtw_rtos_init_semaphore(/*@special@*/ /*@out@*/ rtw_semaphore_type_t *semaphore)
{
	*semaphore = xSemaphoreCreateCounting((unsigned portBASE_TYPE) 0x7fffffff, (unsigned portBASE_TYPE) 0);

	return (*semaphore != NULL) ? RTW_SUCCESS : RTW_SEMAPHORE_ERROR;
}

rtw_result_t rtw_rtos_get_semaphore(rtw_semaphore_type_t *semaphore, uint32_t timeout_ms, rtw_bool_t will_set_in_isr)
{
	UNUSED_PARAMETER(will_set_in_isr);
	if (pdTRUE == xSemaphoreTake(*semaphore, (TickType_t)(timeout_ms * (1000 / configTICK_RATE_HZ)))) {
		return RTW_SUCCESS;
	} else {
		return RTW_TIMEOUT;
	}
}

rtw_result_t rtw_rtos_set_semaphore(rtw_semaphore_type_t *semaphore, rtw_bool_t called_from_ISR)
{
	signed portBASE_TYPE result;

	if (called_from_ISR == RTW_TRUE) {
		signed portBASE_TYPE xHigherPriorityTaskWoken;
		result = xSemaphoreGiveFromISR(*semaphore, &xHigherPriorityTaskWoken);

		rtw_assert("Unable to set semaphore", result == pdTRUE);

		/* If xSemaphoreGiveFromISR() unblocked a task, and the unblocked task has
		 * a higher priority than the currently executing task, then
		 * xHigherPriorityTaskWoken will have been set to pdTRUE and this ISR should
		 * return directly to the higher priority unblocked task.
		 */
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

	} else {
		result = xSemaphoreGive(*semaphore);
		rtw_assert("Unable to set semaphore", result == pdTRUE);
	}

	return (result == pdPASS) ? RTW_SUCCESS : RTW_SEMAPHORE_ERROR;
}

rtw_result_t rtw_rtos_deinit_semaphore(rtw_semaphore_type_t *semaphore)
{
	if (semaphore != NULL) {
		vQueueDelete(*semaphore);
		*semaphore = NULL;
	}
	return RTW_SUCCESS;
}

rtw_result_t rtw_rtos_delay_milliseconds(uint32_t num_ms)
{
	uint32_t remainder;

	if ((num_ms / (1000 / configTICK_RATE_HZ)) != 0) {
		vTaskDelay((TickType_t)(num_ms / (1000 / configTICK_RATE_HZ)));
	}

	remainder = num_ms % (1000 / configTICK_RATE_HZ);

	if (remainder != 0) {
		volatile uint32_t clock_in_kilohertz = (uint32_t)(configCPU_CLOCK_HZ / 1000);
		for (; clock_in_kilohertz != 0; clock_in_kilohertz--) {
			volatile uint32_t tmp_ms = remainder;
			for (; tmp_ms != 0; tmp_ms--) {
				/* do nothing */
			}
		}
	}
	return RTW_SUCCESS;
}

rtw_result_t rtw_sdio_transfer(rtw_sdio_transfer_direction_t direction, rtw_sdio_function_t function, uint32_t address, uint16_t data_size, uint8_t *data,
							   rtw_sdio_response_needed_t response_expected)
{

	/* Note: this function had broken retry logic (never retried), which has been removed.
	*  Failing fast helps problems on the bus get brought to light more quickly and preserves the original behavior.
	*/
	if (data_size == (uint16_t)1) {
		return rtw_sdio_cmd52(direction, function, address, *data, response_expected, data);
	} else {
		return rtw_sdio_cmd53(direction, function, (data_size >= (uint16_t)64) ? SDIO_BLOCK_MODE : SDIO_BYTE_MODE, address, data_size, data, response_expected, NULL);
	}
}

rtw_result_t rtw_read_register_value(rtw_sdio_function_t function, uint32_t address, uint8_t value_length, uint8_t *value)
{
	memset(value, 0, (size_t)value_length);
	return rtw_sdio_transfer(BUS_READ, function, address, value_length, value, RESPONSE_NEEDED);
}

rtw_result_t rtw_write_register_value(rtw_sdio_function_t function, uint32_t address, uint8_t value_length, uint32_t value)
{
	return rtw_sdio_transfer(BUS_WRITE, function, address, value_length, (uint8_t *)&value, RESPONSE_NEEDED);
}
