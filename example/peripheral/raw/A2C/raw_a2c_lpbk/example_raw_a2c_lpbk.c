/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "example_a2c_ext.h"
#include <stdio.h>

static rtos_sema_t g_rx_semaphore;

/**
 * @brief This task waits for a semaphore from the ISR, then reads all data from FIFO
 *
 * @param param, task parameter
 */
static void a2c0_rx_thread(void *param)
{
	A2C_TypeDef *A2Cx = (A2C_TypeDef *)param;
	A2C_RxMsgTypeDef RxMsg;
	u32 rx_count = 0;
	u32 last_print_count = 0;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "A2C RX Thread Started...\n");

	while (1) {
		/*
		 * Wait for the semaphore indefinitely.
		 * The semaphore is given in the ISR when an RX interrupt occurs.
		 */
		if (rtos_sema_take(g_rx_semaphore, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {

			while (A2C_FifoStatusGet(A2Cx) != A2C_BIT_FIFO_MSG_EMPTY) {

				_memset(&RxMsg, 0, sizeof(A2C_RxMsgTypeDef));
				A2C_ReadRxMsgFromFifo(A2Cx, &RxMsg);

				rx_count++;

				/* Optional: Check data integrity (e.g. if ID is sequential) */
				// RTK_LOGS(NOTAG, "RxMsg->RxLost = %ld\n", RxMsg->RxLost);
			}

			/*
			 * Print stats only occasionally to avoid blocking.
			 * Every 1000 frames is safe.
			 */
			if ((rx_count - last_print_count) >= 1000) {
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "[Perf] Total RX: %d frames. Last ID: 0x%x\n", rx_count, RxMsg.StdId);
				last_print_count = rx_count;
			}
		}
	}
}

static u32 a2c0_interrupt_handler(void *Data)
{
	(void)Data;
	A2C_TypeDef *A2Cx = A2C_DEV_TABLE[0].A2Cx;
	u32 IntStatus;
	u32 ErrStatus, TxErCnt, RxErCnt;

	IntStatus = A2C_GetINTStatus(A2Cx);

	/* 1. RX Interrupt: Data Received */
	if (IntStatus & A2C_RX_INT) {
		/* Clear the interrupt flag immediately */
		A2C_ClearINT(A2Cx, A2C_BIT_RX_INT_FLAG);

		/* Signal the RX thread to process data */
		if (g_rx_semaphore != NULL) {
			rtos_sema_give(g_rx_semaphore);
		}
	}

	/* 2. TX Interrupt: Transmission Completed */
	if (IntStatus & A2C_TX_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_TX_INT_FLAG);
		/* Do not print here */
	}

	/* 3. RAM Move Done Interrupt */
	if (IntStatus & A2C_RAM_MOVE_DONE_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_RAM_MOVE_DONE_INT_FLAG);
	}

	/* 4. Bus Off Interrupt */
	if (IntStatus & A2C_BUSOFF_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_BUSOFF_INT_FLAG);
	}

	/* 5. Wakeup Interrupt */
	if (IntStatus & A2C_WKUP_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_WAKEUP_INT_FLAG);
	}

	/* 6. Error Interrupt */
	if (IntStatus & A2C_ERR_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_ERROR_INT_FLAG);
		ErrStatus = A2C_GetErrStatus(A2Cx);
		TxErCnt = A2C_TXErrCntGet(A2Cx);
		RxErCnt = A2C_RXErrCntGet(A2Cx);
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "A2C0:Error Status = %x, TEC = %d, REC = %d\n n", ErrStatus, TxErCnt, RxErCnt);
		A2C_ClearErrStatus(A2Cx, ErrStatus);
	}

	return 0;
}

static void a2c0_tx_thread(void *param)
{
	u32 i;
	u8 j = 0;
	A2C_TypeDef *A2Cx = (A2C_TypeDef *)param;
	A2C_TxMsgTypeDef TxMsg;

	/* Config a2c0 tx thread: std data frame: id = 0x55, payload 0x0~0x7 */
	do {
		/*
		 * Burst: Send 64 frames back-to-back.
		 * This will fill the RX FIFO if the RX thread is slow.
		 */
		for (int burst_idx = 0; burst_idx < 64; burst_idx++) {
			_memset(&TxMsg, 0, sizeof(A2C_TxMsgTypeDef));
			TxMsg.RTR = A2C_DATA_FRAME;
			TxMsg.IDE = A2C_STANDARD_FRAME;
			TxMsg.MsgBufferIdx = 0x0;
			TxMsg.StdId = j++; // Continuous ID
			TxMsg.DLC = 8;
			for (i = 0; i < 8; i++) {
				TxMsg.Data[i] = i;
			}

			A2C_WriteMsg(A2Cx, &TxMsg);

			/*
			 * Tiny delay (optional) to prevent starving other tasks
			 * if priority is same.
			 * 200us is close to the transmission time of 1 frame in 1Mbps.
			 * Remove this delay for maximum stress test.
			 */
			rtos_time_delay_us(200);
		}

		/* Sleep 100ms after a burst to allow other tasks to run */
		rtos_time_delay_ms(100);
	} while (1);
}

static void EMC_A2CInit(void)
{
	A2C_TypeDef *A2Cx_0;
	IRQn_Type  A2C_IRQ_0;
	A2C_InitTypeDef  A2C_InitStruct_0;
	A2C_RxMsgTypeDef RxMsg_0;

	/* 1. Enable Clocks */
	A2C_CoreClockSet();
	RCC_PeriphClockCmd(APBPeriph_A2C0, APBPeriph_A2C0_CLOCK, ENABLE);

	/* 2. Pinmux Configuration */
	Pinmux_Config(A2C0_TX, PINMUX_FUNCTION_A2C0_TX);
	Pinmux_Config(A2C0_RX, PINMUX_FUNCTION_A2C0_RX);

	/* Pull the STB pin low to put the CAN transceiver into normal mode */
	PAD_PullCtrl(A2C0_STB, GPIO_PuPd_DOWN);

	A2Cx_0 = A2C_DEV_TABLE[0].A2Cx;
	A2C_IRQ_0 = A2C_DEV_TABLE[0].IrqNum;

	/* 3. Initialize A2C Hardware */
	A2C_BusCmd(A2Cx_0, DISABLE);
	A2C_StructInit(&A2C_InitStruct_0);
	A2C_CalcBitTiming(1000000, &A2C_InitStruct_0.A2C_Timing); /* 1Mbps */
	A2C_InitStruct_0.A2C_WorkMode = A2C_EXT_LOOPBACK_MODE; /* Loopback for test */
	A2C_InitStruct_0.A2C_RxFifoEn = ENABLE;
	A2C_Init(A2Cx_0, &A2C_InitStruct_0);

	/* 4. Configure Interrupts */
	InterruptDis(A2C_IRQ_0);
	InterruptUnRegister(A2C_IRQ_0);
	InterruptRegister((IRQ_FUN)a2c0_interrupt_handler, A2C_IRQ_0, NULL, INT_PRI_MIDDLE);
	InterruptEn(A2C_IRQ_0, INT_PRI_MIDDLE);

	A2C_INTConfig(A2Cx_0, (A2C_TX_INT | A2C_RX_INT | A2C_RAM_MOVE_DONE_INT | A2C_ERR_INT | A2C_BUSOFF_INT | A2C_WKUP_INT), ENABLE);
	A2C_TxMsgBufINTConfig(A2Cx_0, A2C_MB_TXINT_EN(0xFFFF), ENABLE);
	A2C_RxMsgBufINTConfig(A2Cx_0, A2C_MB_RXINT_EN(0xFFFF), ENABLE);

	/* 5. Enable Bus */
	A2C_Cmd(A2Cx_0, ENABLE);
	A2C_BusCmd(A2Cx_0, ENABLE);

	while (!A2C_BusStatusGet(A2Cx_0));
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "a2c0 bus on\n");

	/* 6. Config A2C0 RX Msg Buffer*/
	for (int i = A2C_RX_FIFO_READ_MSG_IDX; i < A2C_MESSAGE_BUFFER_SIZE; i++) {
		_memset(&RxMsg_0, 0, sizeof(A2C_RxMsgTypeDef));
		RxMsg_0.RTR = A2C_DATA_FRAME;
		RxMsg_0.IDE = A2C_STANDARD_FRAME;
		RxMsg_0.MsgBufferIdx = i;
		RxMsg_0.ExtId = 0x0;
		RxMsg_0.StdId = 0x01;
		RxMsg_0.ID_MASK = 0x0;
		RxMsg_0.RTR_Mask = 0x0;
		RxMsg_0.IDE_Mask = 0x0;
		A2C_SetRxMsgBuf(A2Cx_0, &RxMsg_0);
	}
}

int example_raw_a2c_lpbk(void)
{
	/* 1. Create Synchronization Semaphore */
	rtos_sema_create_binary(&g_rx_semaphore);

	/* 2. Initialize Hardware and Interrupts */
	EMC_A2CInit();

	/* 3. Create RX Task (High Priority recommended for data processing) */
	if (rtos_task_create(NULL, "A2C0_RX_THREAD", a2c0_rx_thread, (void *)A2C_DEV_TABLE[0].A2Cx, 3072, 3) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n%s: Create A2C0 rx thread error\n", __FUNCTION__);
	}

	/* 4. Create TX Task (Test generator) */
	if (rtos_task_create(NULL, "A2C0_TX_THREAD", a2c0_tx_thread, (void *)A2C_DEV_TABLE[0].A2Cx, 2048, 2) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n%s: Create A2C0 tx thread error\n", __FUNCTION__);
	}

	return 0;
}
