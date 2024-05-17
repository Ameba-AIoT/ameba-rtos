/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2020 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "ledc_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define LEDC_MODE		LEDC_DMA_MODE

#define LED_NUM			(16)
#define FRAME_NUM		(7)
#define TEST_BUF_SIZE	(LED_NUM * FRAME_NUM)
u32 pixels[TEST_BUF_SIZE];

LEDC_OBJ ledc_obj;
GDMA_InitTypeDef ledc_dma_t;

/**
  * @brief  prepare GRB 24bits format data.
  * @param  red: the brightness of red.
  * @param  green: the brightness of green.
  * @param  blue: the brightness of blue.
  * @return  combined GRB data.
  */
u32 set_rgb(u8 r, u8 g, u8 b)
{
	u32 led_data = (g << 16) | (r << 8) | b;

	return led_data;
}

/**
  * @brief  LEDC tx complete callback.
  * @return NONE.
  */
void ledc_complete_callback(void)
{
	printf("new start\n");
	if (LEDC_GetTransferMode(LEDC_DEV) == LEDC_DMA_MODE) {
		GDMA_Cmd(ledc_dma_t.GDMA_Index, ledc_dma_t.GDMA_ChNum, ENABLE);
	}

	LEDC_Cmd(LEDC_DEV, ENABLE);
}

/**
  * @brief  LEDC interrupt handler.
  * @param  param: LEDC_OBJ param.
  * @return 0.
  */
u32 ledc_irq_handler(void *param)
{
	LEDC_OBJ *ledc_adapter = (LEDC_OBJ *)param;
	u32 ledc_fifothr;
	u32 InterruptStatus;
	u32 *startaddr = NULL;
	static u32 Translen = 0;

	LEDC_INTConfig(LEDC_DEV, LEDC_BIT_GLOBAL_INT_EN, DISABLE);

	InterruptStatus = LEDC_GetINT(LEDC_DEV);

	if (InterruptStatus & LEDC_BIT_FIFO_CPUREQ_INT) {
		printf("[LEDC INT]CPU REQ %lx\n", InterruptStatus);
		LEDC_ClearINT(LEDC_DEV, LEDC_BIT_FIFO_CPUREQ_INT);

		ledc_fifothr = LEDC_GetFIFOLevel(LEDC_DEV);
		startaddr = ledc_adapter->TxData + Translen;

		if ((ledc_adapter->TxLength - Translen) >= ledc_fifothr) {
			Translen += LEDC_SendData(LEDC_DEV, startaddr, ledc_fifothr);
		} else {
			Translen += LEDC_SendData(LEDC_DEV, startaddr, ledc_adapter->TxLength - Translen);
		}

		if ((ledc_adapter->TxLength == Translen)) {
			Translen = 0;
		}

		LEDC_INTConfig(LEDC_DEV, LEDC_BIT_GLOBAL_INT_EN, ENABLE);
		return 0;
	}

	if (InterruptStatus & LEDC_BIT_LED_TRANS_FINISH_INT) {
		printf("[LEDC INT]TRANS DONE %lx\n\n", InterruptStatus);
		LEDC_ClearINT(LEDC_DEV, LEDC_BIT_LED_TRANS_FINISH_INT);

		ledc_adapter->Result = RESULT_COMPLETE;
		LEDC_SoftReset(LEDC_DEV);

		ledc_complete_callback();
	}

	if (InterruptStatus & LEDC_BIT_WAITDATA_TIMEOUT_INT) {
		printf("[LEDC INT]wait data timeout %lx\n", InterruptStatus);
		LEDC_ClearINT(LEDC_DEV, LEDC_BIT_WAITDATA_TIMEOUT_INT);

		ledc_adapter->Result = RESULT_ERR;
		LEDC_SoftReset(LEDC_DEV);
	}

	if (InterruptStatus & LEDC_BIT_FIFO_OVERFLOW_INT) {
		printf("[LEDC INT]FIFO OF %lx\n", InterruptStatus);
		LEDC_ClearINT(LEDC_DEV, LEDC_BIT_FIFO_OVERFLOW_INT);

		ledc_adapter->Result = RESULT_ERR;
		LEDC_SoftReset(LEDC_DEV);
	}

	LEDC_INTConfig(LEDC_DEV, LEDC_BIT_GLOBAL_INT_EN, ENABLE);
	return 0;
}

/**
  * @brief  LEDC GDMA deinit to release DMA channel.
  * @return None.
  */
void ledc_dma_deinit(void)
{
	GDMA_Cmd(0, ledc_dma_t.GDMA_ChNum, DISABLE);
	GDMA_ChnlFree(0, ledc_dma_t.GDMA_ChNum);
}

/**
  * @brief  LEDC irq handler to clear GDMA interrupt bits.
  * @return None.
  */
u32 ledc_dma_irq(void *param)
{
	(void)param;
	printf("__LedcDmaIrqHandler__\n");

	/* Clear Pending ISR */
	GDMA_ClearINT(ledc_dma_t.GDMA_Index, ledc_dma_t.GDMA_ChNum);

	return _TRUE;
}

/**
  * @brief  Init GDMA for LEDC tx.
  * @param GDMA_InitStruct: GDMA Struct to init.
  * @param CallbackFunc: GDMA IRQ FUNCTION
  * @param CallbackData: GDMA IRQ FUNC parameters
  * @param GdmaScrAddr: GDMA source address
  * @return TURE.
  */
bool ledc_dma_init(PGDMA_InitTypeDef GDMA_InitStruct,
				   IRQ_FUN CallbackFunc,
				   void *CallbackData,
				   u32 *GdmaScrAddr)
{
	u8 gdma_chnl;
	u8 ledc_fifothr;

	assert_param(GDMA_InitStruct != NULL);

	// Allocate a GDMA channel for LEDC tx
	gdma_chnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, 4);
	if (gdma_chnl == 0xFF) { /*	No Available DMA channel */
		return _FALSE;
	}

	// alloc and Init GDMA
	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = gdma_chnl;
	GDMA_InitStruct->GDMA_DIR = TTFCMemToPeri_PerCtrl;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA_HANDSHAKE_INTERFACE_LEDC_TX;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_SrcAddr = (u32)(GdmaScrAddr);
	GDMA_InitStruct->GDMA_DstAddr = (u32)&LEDC_DEV->LEDC_DATA_REG;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	ledc_fifothr = LEDC_GetFIFOLevel(LEDC_DEV);
	if (ledc_fifothr == 8) {
		GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
		GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
	} else {
		LEDC_SetFIFOLevel(LEDC_DEV, 15);
		GDMA_InitStruct->GDMA_SrcMsize = MsizeSixteen;
		GDMA_InitStruct->GDMA_DstMsize = MsizeSixteen;
	}
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;

	return _TRUE;
}

/**
  * @brief  Initialize LEDC peripheral.
  * @param  No parameter.
  * @return None
  * @note: Set led timing, reset time, wait data timeout time, transmode, led number
  *       and register interrupt handler.
  */
void ws2812_init(void)
{
	LEDC_InitTypeDef LEDC_InitStruct;

	/* Enable LEDC clock and function */
	RCC_PeriphClockCmd(APBPeriph_LEDC, APBPeriph_LEDC_CLOCK, ENABLE);

	/* Configure LEDC output pin */
	Pinmux_Config(LEDC_PIN, PINMUX_FUNCTION_LEDC);

	LEDC_StructInit(&LEDC_InitStruct);
	LEDC_InitStruct.t0h_ns = NS2VAL(LED_T0H);
	LEDC_InitStruct.t0l_ns = NS2VAL(LED_T0L);
	LEDC_InitStruct.t1h_ns = NS2VAL(LED_T1H);
	LEDC_InitStruct.t1l_ns = NS2VAL(LED_T1L);
	LEDC_InitStruct.reset_ns = NS2VAL(LED_RST);
	LEDC_InitStruct.wait_data_time_ns = NS2VAL(LED_RST);
	LEDC_InitStruct.wait_time1_ns = NS2VAL(F_INTERVAL_NS);

	LEDC_InitStruct.ledc_trans_mode = LEDC_MODE;
	LEDC_InitStruct.led_count = LED_NUM;
	LEDC_InitStruct.data_length = LED_NUM;
	LEDC_Init(LEDC_DEV, &LEDC_InitStruct);

	InterruptRegister((IRQ_FUN)ledc_irq_handler, LEDC_IRQ, (u32)&ledc_obj, INT_PRI_MIDDLE);
	InterruptEn(LEDC_IRQ, INT_PRI_MIDDLE);
}

/**
  * @brief  Initialize LED data.
  * @param  No parameter.
  * @return  None
  * @note  Data for ws2812: red->off->green->off->blue->off.
  */
void ws2812_data(void)
{
	u32 frame_r = set_rgb(0xFF, 0, 0);
	u32 frame_g = set_rgb(0, 0xFF, 0);
	u32 frame_b = set_rgb(0, 0, 0xFF);
	u32 frame_off = set_rgb(0, 0, 0);

	for (u32 i = 0; i < LED_NUM * FRAME_NUM; i++) {
		if (i / LED_NUM % 2) { //odd frames
			pixels[i] = frame_off;
		} else if ((i / LED_NUM % 6) == 0) { //0 6 12 .. frames
			pixels[i] = frame_r;
		} else if ((i / LED_NUM % 6) == 2) { //2 8 14 .. frames
			pixels[i] = frame_g;
		} else { //4 10 16 .. frames
			pixels[i] = frame_b;
		}
	}
}

/**
  * @brief  LEDC starts to work.
  * @param  No parameter.
  * @return None
  * @note LEDC will not stop working until Result changes.
  */
void ws2812_light(void)
{
	/* prepare data */
	ws2812_data();

	/* set transfer data and length */
	ledc_obj.Result = RESULT_RUNNING;
	ledc_obj.TxData = pixels;
	ledc_obj.TxLength = (FRAME_NUM * LED_NUM);
	LEDC_SetTotalLength(LEDC_DEV, ledc_obj.TxLength);

	if (LEDC_MODE) {
		/* DMA mode */
		ledc_dma_init(&ledc_dma_t, ledc_dma_irq, NULL, ledc_obj.TxData);

		// clear and invalidate D_Cache of tx RAM address
		DCache_CleanInvalidate((u32)ledc_obj.TxData, (4 * ledc_obj.TxLength));

		GDMA_Init(ledc_dma_t.GDMA_Index, ledc_dma_t.GDMA_ChNum, &ledc_dma_t);
		GDMA_Cmd(ledc_dma_t.GDMA_Index, ledc_dma_t.GDMA_ChNum, ENABLE);
	}

	/* enable LEDC */
	LEDC_Cmd(LEDC_DEV, ENABLE);

	/* stop working until Result is complete or error */
	while (ledc_obj.Result == RESULT_RUNNING);

	if (ledc_obj.Result == RESULT_COMPLETE) {
		printf("\ntx done!\n");
	}

	if (ledc_obj.Result == RESULT_ERR) {
		printf("\nledc err!\n");
	}

	/* release DMA channel when error or complete*/
	if (LEDC_GetTransferMode(LEDC_DEV) == LEDC_DMA_MODE) {
		ledc_dma_deinit();
	}

	rtos_task_delete(NULL);
}

int main(void)
{
	/* init ledc for ws2812 */
	ws2812_init();
	printf("LEDC ws2812 demo start... \n");

	if (SUCCESS != rtos_task_create(NULL, (const char *const)"LEDC_WS2812_DEMO", (rtos_task_t)ws2812_light, NULL, 1024 * 4,
									1)) {
		printf("Create LEDC_WS2812_DEMO error!!\n");
	}

	rtos_sched_start();

	return 0;
}
