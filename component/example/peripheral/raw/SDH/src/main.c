/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

#define start_addr		0x0
#define end_addr		0x3FF
#define blk_cnt			2

SRAM_NOCACHE_DATA_SECTION u8 RxBuffer[SD_BLOCK_SIZE * blk_cnt] __attribute__((aligned(32)));
SRAM_NOCACHE_DATA_SECTION u8 TxBuffer[SD_BLOCK_SIZE * blk_cnt] __attribute__((aligned(32)));
static rtos_sema_t sd_sema;

static SDIOHCFG_TypeDef sd_config = {
	.sdioh_bus_speed = SD_SPEED_HS,				//SD_SPEED_DS or SD_SPEED_HS
	.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT, 	//SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT
	.sdioh_cd_pin = _PC_0,						//_PC_0/_PNC
	.sdioh_wp_pin = _PNC,						//_PB_31/_PNC
};


static int sd_give_sema(u32 timeout)
{
	UNUSED(timeout);
	return (u32) rtos_sema_give(sd_sema);
}

static int sd_take_sema(u32 timeout)
{
	return (u32) rtos_sema_take(sd_sema, timeout);
}

static void sd_sema_init(void)
{
	rtos_sema_create(&sd_sema, 0U, 1U);
	SD_SetSema(sd_take_sema, sd_give_sema);
}

void raw_sd_host_demo(void)
{
	u32 ret;

	sd_sema_init();

	/*init tx buffer*/
	for (int idx = 0; idx < SD_BLOCK_SIZE * blk_cnt; idx++) {
		TxBuffer[idx] = (u8)idx;
	}

	/* initialize sd host and sd memory card */
	ret = SD_Init(&sd_config);
	if (ret != SD_OK) {
		printf("SD Init Failed!\n");
		return;
	} else {
		printf("SD Host erase/read/write start.\n");
	}

	/* erase test blocks*/
	ret = SD_Erase(start_addr / SD_BLOCK_SIZE, end_addr / SD_BLOCK_SIZE);
	if (ret != HAL_OK) {
		printf("Erase FAIL, ret: %d\n", (int) ret);
	} else {
		printf("Erase Succeed\n");
	}

	/*read test blocks*/
	_memset(RxBuffer, 0, SD_BLOCK_SIZE * blk_cnt);

	ret = SD_ReadMultiBlocks(RxBuffer, start_addr / SD_BLOCK_SIZE, blk_cnt);
	if (ret != HAL_OK) {
		printf("Read FAIL, ret: %d\n", (int) ret);
	} else {
		printf("Read Succeed\n");
	}

	/* Check data */
	for (int i = 0; i < SD_BLOCK_SIZE * blk_cnt; i++) {
		if ((RxBuffer[i] != 0) && (RxBuffer[i] != 0xff)) {
			printf("Data check fail. Something wrong when erase or read data.\n");
			goto exit;
		}
	}

	/*write test blocks*/
	ret = SD_WriteMultiBlocks(TxBuffer, start_addr / SD_BLOCK_SIZE, blk_cnt);
	if (ret != HAL_OK) {
		printf("Write FAIL, ret: %d\n", (int) ret);
	} else {
		printf("Write Succeed\n");
	}

	/*read test blocks*/
	_memset(RxBuffer, 0, SD_BLOCK_SIZE * blk_cnt);

	ret = SD_ReadMultiBlocks(RxBuffer, start_addr / SD_BLOCK_SIZE, blk_cnt);
	if (ret != HAL_OK) {
		printf("Read FAIL, ret: %d\n", (int) ret);
	} else {
		printf("Read Succeed\n");
	}

	/* Check data */
	for (int i = 0; i < SD_BLOCK_SIZE * blk_cnt; i++) {
		if (RxBuffer[i] != i % 256) {
			printf("Data check fail. Something wrong when write or read data.\n");
			goto exit;
		}
	}

exit:
	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	DBG_ERR_MSG_ON(MODULE_SDIO);

	if (SUCCESS != rtos_task_create(NULL, "RAW_SD_HOST_TASK", (rtos_task_t)raw_sd_host_demo, (void *)NULL, (3072), (1))) {
		printf("Create RAW_SD_HOST_TASK Err!!\n");
	}

	rtos_sched_start();
}
