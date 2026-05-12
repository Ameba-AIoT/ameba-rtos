/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "example_can_ext.h"
#include <stdio.h>

static rtos_sema_t g_rx_semaphore;

/**
 * @brief This task waits for a semaphore from the ISR, then reads all data from FIFO
 *
 * @param param, task parameter
 */
static void can0_rx_thread(void *param)
{
	CAN_TypeDef *CANx = (CAN_TypeDef *)param;
	CAN_RxMsgTypeDef RxMsg;
	u32 rx_count = 0;
	u32 last_print_count = 0;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "CAN RX Thread Started...\n");

	while (1) {
		/*
		 * Wait for the semaphore indefinitely.
		 * The semaphore is given in the ISR when an RX interrupt occurs.
		 */
		if (rtos_sema_take(g_rx_semaphore, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {

			while (CAN_FifoStatusGet(CANx) != CAN_BIT_FIFO_MSG_EMPTY) {

				_memset(&RxMsg, 0, sizeof(CAN_RxMsgTypeDef));
				CAN_ReadRxMsgFromFifo(CANx, &RxMsg);

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

static u32 can0_interrupt_handler(void *Data)
{
	(void)Data;
	CAN_TypeDef *CANx = CAN_DEV_TABLE[0].CANx;
	u32 IntStatus;
	u32 ErrStatus, TxErCnt, RxErCnt;

	IntStatus = CAN_GetINTStatus(CANx);

	/* 1. RX Interrupt: Data Received */
	if (IntStatus & CAN_RX_INT) {
		/* Clear the interrupt flag immediately */
		CAN_ClearINT(CANx, CAN_BIT_RX_INT_FLAG);

		/* Signal the RX thread to process data */
		if (g_rx_semaphore != NULL) {
			rtos_sema_give(g_rx_semaphore);
		}
	}

	/* 2. TX Interrupt: Transmission Completed */
	if (IntStatus & CAN_TX_INT) {
		CAN_ClearINT(CANx, CAN_BIT_TX_INT_FLAG);
		/* Do not print here */
	}

	/* 3. RAM Move Done Interrupt */
	if (IntStatus & CAN_RAM_MOVE_DONE_INT) {
		CAN_ClearINT(CANx, CAN_BIT_RAM_MOVE_DONE_INT_FLAG);
	}

	/* 4. Bus Off Interrupt */
	if (IntStatus & CAN_BUSOFF_INT) {
		CAN_ClearINT(CANx, CAN_BIT_BUSOFF_INT_FLAG);
	}

	/* 5. Wakeup Interrupt */
	if (IntStatus & CAN_WKUP_INT) {
		CAN_ClearINT(CANx, CAN_BIT_WAKEUP_INT_FLAG);
	}

	/* 6. Error Interrupt */
	if (IntStatus & CAN_ERR_INT) {
		CAN_ClearINT(CANx, CAN_BIT_ERROR_INT_FLAG);
		ErrStatus = CAN_GetErrStatus(CANx);
		TxErCnt = CAN_TXErrCntGet(CANx);
		RxErCnt = CAN_RXErrCntGet(CANx);
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "CAN0:Error Status = %x, TEC = %d, REC = %d\n n", ErrStatus, TxErCnt, RxErCnt);
		CAN_ClearErrStatus(CANx, ErrStatus);
	}

	return 0;
}

static void can0_tx_thread(void *param)
{
	u32 i;
	u8 j = 0;
	CAN_TypeDef *CANx = (CAN_TypeDef *)param;
	CAN_TxMsgTypeDef TxMsg;

	/* Config can0 tx thread: std data frame: id = 0x55, payload 0x0~0x7 */
	do {
		/*
		 * Burst: Send 64 frames back-to-back.
		 * This will fill the RX FIFO if the RX thread is slow.
		 */
		for (int burst_idx = 0; burst_idx < 64; burst_idx++) {
			_memset(&TxMsg, 0, sizeof(CAN_TxMsgTypeDef));
			TxMsg.RTR = CAN_DATA_FRAME;
			TxMsg.IDE = CAN_STANDARD_FRAME;
			TxMsg.MsgBufferIdx = 0x0;
			TxMsg.StdId = j++; // Continuous ID
			TxMsg.DLC = 8;
			for (i = 0; i < 8; i++) {
				TxMsg.Data[i] = i;
			}

			CAN_WriteMsg(CANx, &TxMsg);

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

static void EMC_CANInit(void)
{
	CAN_TypeDef *CANx_0;
	IRQn_Type  CAN_IRQ_0;
	CAN_InitTypeDef  CAN_InitStruct_0;
	CAN_RxMsgTypeDef RxMsg_0;

	/* 1. Enable Clocks */
	CAN_CoreClockSet();
	RCC_PeriphClockCmd(APBPeriph_CAN0, APBPeriph_CAN0_CLOCK, ENABLE);

	/* 2. Pinmux Configuration */
	Pinmux_Config(CAN0_TX, PINMUX_FUNCTION_CAN0_TX);
	Pinmux_Config(CAN0_RX, PINMUX_FUNCTION_CAN0_RX);

	/* Pull the STB pin low to put the CAN transceiver into normal mode */
	PAD_PullCtrl(CAN0_STB, GPIO_PuPd_DOWN);

	CANx_0 = CAN_DEV_TABLE[0].CANx;
	CAN_IRQ_0 = CAN_DEV_TABLE[0].IrqNum;

	/* 3. Initialize CAN Hardware */
	CAN_BusCmd(CANx_0, DISABLE);
	CAN_StructInit(&CAN_InitStruct_0);
	CAN_CalcBitTiming(1000000, &CAN_InitStruct_0.CAN_Timing); /* 1Mbps */
	CAN_InitStruct_0.CAN_WorkMode = CAN_INT_LOOPBACK_MODE; /* Loopback for test */
	CAN_InitStruct_0.CAN_RxFifoEn = ENABLE;
	CAN_Init(CANx_0, &CAN_InitStruct_0);

	/* 4. Configure Interrupts */
	InterruptDis(CAN_IRQ_0);
	InterruptUnRegister(CAN_IRQ_0);
	InterruptRegister((IRQ_FUN)can0_interrupt_handler, CAN_IRQ_0, NULL, INT_PRI_MIDDLE);
	InterruptEn(CAN_IRQ_0, INT_PRI_MIDDLE);

	CAN_INTConfig(CANx_0, (CAN_TX_INT | CAN_RX_INT | CAN_RAM_MOVE_DONE_INT | CAN_ERR_INT | CAN_BUSOFF_INT | CAN_WKUP_INT), ENABLE);
	CAN_TxMsgBufINTConfig(CANx_0, CAN_MB_TXINT_EN(0xFFFF), ENABLE);
	CAN_RxMsgBufINTConfig(CANx_0, CAN_MB_RXINT_EN(0xFFFF), ENABLE);

	/* 5. Enable Bus */
	CAN_Cmd(CANx_0, ENABLE);
	CAN_BusCmd(CANx_0, ENABLE);

	while (!CAN_BusStatusGet(CANx_0));
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "can0 bus on\n");

	/* 6. Config CAN0 RX Msg Buffer*/
	for (int i = CAN_RX_FIFO_READ_MSG_IDX; i < CAN_MESSAGE_BUFFER_SIZE; i++) {
		_memset(&RxMsg_0, 0, sizeof(CAN_RxMsgTypeDef));
		RxMsg_0.RTR = CAN_DATA_FRAME;
		RxMsg_0.IDE = CAN_STANDARD_FRAME;
		RxMsg_0.MsgBufferIdx = i;
		RxMsg_0.ExtId = 0x0;
		RxMsg_0.StdId = 0x01;
		RxMsg_0.ID_MASK = 0x0;
		RxMsg_0.RTR_Mask = 0x0;
		RxMsg_0.IDE_Mask = 0x0;
		CAN_SetRxMsgBuf(CANx_0, &RxMsg_0);
	}
}

int example_raw_can_lpbk(void)
{
	/* 1. Create Synchronization Semaphore */
	rtos_sema_create_binary(&g_rx_semaphore);

	/* 2. Initialize Hardware and Interrupts */
	EMC_CANInit();

	/* 3. Create RX Task (High Priority recommended for data processing) */
	if (rtos_task_create(NULL, "CAN0_RX_THREAD", can0_rx_thread, (void *)CAN_DEV_TABLE[0].CANx, 3072, 3) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n%s: Create CAN0 rx thread error\n", __FUNCTION__);
	}

	/* 4. Create TX Task (Test generator) */
	if (rtos_task_create(NULL, "CAN0_TX_THREAD", can0_tx_thread, (void *)CAN_DEV_TABLE[0].CANx, 2048, 2) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n%s: Create CAN0 tx thread error\n", __FUNCTION__);
	}

	return 0;
}
