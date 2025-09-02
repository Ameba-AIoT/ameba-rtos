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

static rtos_sema_t sd_sema;

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
	u8 *RxBuffer;
	u8 *TxBuffer;

	sd_sema_init();
	RxBuffer = rtos_mem_malloc(SD_BLOCK_SIZE * blk_cnt);
	TxBuffer = rtos_mem_malloc(SD_BLOCK_SIZE * blk_cnt);

	/*init tx buffer*/
	for (int idx = 0; idx < SD_BLOCK_SIZE * blk_cnt; idx++) {
		TxBuffer[idx] = (u8)idx;
	}

	/* initialize sd host and sd memory card */
	sdioh_config.sdioh_bus_speed = SD_SPEED_HS;
	sdioh_config.sdioh_bus_width = SDIOH_BUS_WIDTH_4BIT;
	sdioh_config.sdioh_cd_pin = _PC_0;
	ret = SD_Init();
	if (ret != SD_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "SD Init Failed!\n");
		return;
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "SD Host erase/read/write start.\n");
	}

	/* erase test blocks*/
	ret = SD_Erase(start_addr / SD_BLOCK_SIZE, end_addr / SD_BLOCK_SIZE);
	if (ret != HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Erase FAIL, ret: %d\n", (int) ret);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Erase Succeed\n");
	}

	/*read test blocks*/
	_memset(RxBuffer, 0, SD_BLOCK_SIZE * blk_cnt);

	ret = SD_ReadMultiBlocks(RxBuffer, start_addr / SD_BLOCK_SIZE, blk_cnt);
	if (ret != HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Read FAIL, ret: %d\n", (int) ret);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Read Succeed\n");
	}

	/* Check data */
	for (int i = 0; i < SD_BLOCK_SIZE * blk_cnt; i++) {
		if ((RxBuffer[i] != 0) && (RxBuffer[i] != 0xff)) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Data check fail. Something wrong when erase or read data.\n");
			goto exit;
		}
	}

	/*write test blocks*/
	ret = SD_WriteMultiBlocks(TxBuffer, start_addr / SD_BLOCK_SIZE, blk_cnt);
	if (ret != HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Write FAIL, ret: %d\n", (int) ret);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Write Succeed\n");
	}

	/*read test blocks*/
	_memset(RxBuffer, 0, SD_BLOCK_SIZE * blk_cnt);

	ret = SD_ReadMultiBlocks(RxBuffer, start_addr / SD_BLOCK_SIZE, blk_cnt);
	if (ret != HAL_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Read FAIL, ret: %d\n", (int) ret);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Read Succeed\n");
	}

	/* Check data */
	for (int i = 0; i < SD_BLOCK_SIZE * blk_cnt; i++) {
		if (RxBuffer[i] != i % 256) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Data check fail. Something wrong when write or read data.\n");
			goto exit;
		}
	}

exit:
	rtos_mem_free(RxBuffer);
	rtos_mem_free(TxBuffer);
	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_sd_host(void)
{
	DBG_ERR_MSG_ON(MODULE_SDIO);

	if (RTK_SUCCESS != rtos_task_create(NULL, "RAW_SD_HOST_TASK", (rtos_task_t)raw_sd_host_demo, (void *)NULL, (3072), (1))) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Create RAW_SD_HOST_TASK Err!!\n");
	}

	return 0;
}
