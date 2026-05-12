/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "example_can_ext.h"
#include <stdio.h>

static u32 can_ram_buffer_map[] = {
	0x0, 0x6, 0xc, 0x12, 0x18, 0x1e, 0x24, 0x2a, 0x30,
	0x36, 0x3c, 0x42, 0x48, 0x4e, 0x54, 0x5a, 0x60
};
/*dump rx message*/
static void dump_rx_message(CAN_RxMsgTypeDef *RxMsg)
{
	u32 i;

	printf("######################RX message start######################\n");
	printf("RxMsg->StdId = %lx\n", RxMsg->StdId);
	printf("RxMsg->ExtId = %lx\n", RxMsg->ExtId);
	printf("RxMsg->MsgBufferIdx = %ld\n", RxMsg->MsgBufferIdx);

	if (RxMsg->IDE == CAN_STANDARD_FRAME) {
		printf("RxMsg->IDE = %lx (standard frame)\n", RxMsg->IDE);
	} else if (RxMsg->IDE == CAN_EXTEND_FRAME) {
		printf("RxMsg->IDE = %lx (extend frame)\n", RxMsg->IDE);
	} else {
		printf("wrong IDE value !!!: RxMsg->IDE = %lx\n", RxMsg->IDE);
	}

	if (RxMsg->RTR == CAN_DATA_FRAME) {
		printf("RxMsg->RTR = %lx (data frame)\n", RxMsg->RTR);
	} else if (RxMsg->RTR == CAN_REMOTE_FRAME) {
		printf("RxMsg->RTR = %lx (remote frame)\n", RxMsg->RTR);
	} else {
		printf("wrong RTR value: RxMsg->RTR = %lx\n", RxMsg->RTR);
	}

	printf("RxMsg->DLC = %ld (data length)\n", RxMsg->DLC);
	for (i = 0; i < 8; i++) {
		printf("RxMsg->data[%ld] = %x\n", i, RxMsg->Data[i]);
	}
	printf("RxMsg->ID_MASK = %lx\n", RxMsg->ID_MASK);
	printf("RxMsg->RTR_Mask = %lx\n", RxMsg->RTR_Mask);
	printf("RxMsg->IDE_Mask = %lx\n", RxMsg->IDE_Mask);
	printf("RxMsg->RxTimStamp = %ld\n", RxMsg->RxTimStamp);
	printf("RxMsg->RxLost = %ld\n", RxMsg->RxLost);
	printf("##########################RX message end######################\n\n\n");
}
//can frame: id = 0x0 or 0x55, std, data, 0x00~0x07
static void check_rx_message(CAN_TypeDef *CANx)
{
	u32 i;
	u32 status = 0;
	CAN_RxMsgTypeDef RxMsg;

	_memset(&RxMsg, 0, sizeof(CAN_RxMsgTypeDef));

	for (i = CAN_MESSAGE_BUFFER_SIZE; i >  0; i--) {
		RxMsg.MsgBufferIdx = i - 1;
		if (CAN_MsgBufRxDoneStatusGet(CANx, RxMsg.MsgBufferIdx)) {
			CAN_MsgBufRxDoneStatusClear(CANx, RxMsg.MsgBufferIdx);
			CAN_ReadMsg(CANx, &RxMsg);
			dump_rx_message(&RxMsg);

			if (RxMsg.StdId != 0x0 && RxMsg.StdId != 0x55) {
				status = 1;
			}
			if (RxMsg.IDE != CAN_STANDARD_FRAME) {
				status = 1;
			}
			if (RxMsg.RTR != CAN_DATA_FRAME) {
				status = 1;
			}
			if (RxMsg.DLC != 8) {
				status = 1;
			}

			for (i = 0; i < 8; i++) {
				if (RxMsg.Data[i] != i) {
					status = 1;
					break;
				}
			}
			if (status == 1) {
				status = 0;
				printf("ERROR\n");
			}
		}
	}
}


static u32 can0_interrupt_handler(void *Data)
{
	(void)Data;
	CAN_TypeDef *CANx = CAN_DEV_TABLE[0].CANx;
	u32 IntStatus, ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning;

	IntStatus = CAN_GetINTStatus(CANx);

	/*ram move done interrupt */
	if (IntStatus & CAN_RAM_MOVE_DONE_INT) {
		CAN_ClearINT(CANx, CAN_BIT_RAM_MOVE_DONE_INT_FLAG);

	}

	/*tx interrupt*/
	if (IntStatus & CAN_TX_INT) {
		CAN_ClearINT(CANx, CAN_BIT_TX_INT_FLAG);

	}

	/*rx interrupt*/
	if (IntStatus & CAN_RX_INT) {
		CAN_ClearINT(CANx, CAN_BIT_RX_INT_FLAG);
		printf("Clear Interrupt Status = %lx\n", CAN_GetINTStatus(CANx));

		/* get current error status */
		TxErCnt = CAN_TXErrCntGet(CANx);
		RxErCnt = CAN_RXErrCntGet(CANx);
		ErrPassive = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;
		printf("****TEC = %ld, REC = %ld, ErrPassive = %ld, ErrBusoff = %ld, ErrWarning = %ld\n", TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning);

		check_rx_message(CANx);

	}

	/* bus off interrupt */
	if (IntStatus & CAN_BUSOFF_INT) {
		CAN_ClearINT(CANx, CAN_BIT_BUSOFF_INT_FLAG);
		printf("CAN0: bus off\n");
	}

	/* wakeup interrupt */
	if (IntStatus & CAN_WKUP_INT) {
		CAN_ClearINT(CANx, CAN_BIT_WAKEUP_INT_FLAG);
		printf("CAN0: wake up\n");
	}

	/* error interrupt */
	if (IntStatus & CAN_ERR_INT) {
		CAN_ClearINT(CANx, CAN_BIT_ERROR_INT_FLAG);
		printf("CAN0: Clear Interrupt Status = %lx\n", CAN_GetINTStatus(CANx));

		ErrStatus = CAN_GetErrStatus(CANx);
		TxErCnt = CAN_TXErrCntGet(CANx);
		RxErCnt = CAN_RXErrCntGet(CANx);
		ErrPassive = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;

		if (ErrStatus & CAN_BIT_ERROR_BIT0) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_BIT0);
			printf("bit 0 error: tx = 0, but rx = 1\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_BIT1) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_BIT1);
			printf("bit 1 error: tx = 1, but rx = 0\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_FORM) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_FORM);
			printf("form error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_CRC) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_CRC);
			printf("CRC error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_STUFF) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_STUFF);
			printf("stuff error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_ACK) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_ACK);
			printf("ACK error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_TX) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_TX);
			printf("tx error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_RX) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_RX);
			printf("rx error\n");
		}

		printf("TEC = %ld, REC = %ld\n", TxErCnt, RxErCnt);
	}
	return 0;
}

static u32 can1_interrupt_handler(void *Data)
{
	(void)Data;
	CAN_TypeDef *CANx = CAN_DEV_TABLE[1].CANx;
	u32 IntStatus, ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning;

	IntStatus = CAN_GetINTStatus(CANx);
	/*ram move done interrupt */
	if (IntStatus & CAN_RAM_MOVE_DONE_INT) {
		CAN_ClearINT(CANx, CAN_BIT_RAM_MOVE_DONE_INT_FLAG);

	}

	/*tx interrupt*/
	if (IntStatus & CAN_TX_INT) {
		CAN_ClearINT(CANx, CAN_BIT_TX_INT_FLAG);
	}

	/*rx interrupt*/
	if (IntStatus & CAN_RX_INT) {
		CAN_ClearINT(CANx, CAN_BIT_RX_INT_FLAG);
		// printf("Clear rx Status = %lx\n", CAN_GetINTStatus(CANx));

		/* get current error status */
		TxErCnt = CAN_TXErrCntGet(CANx);
		RxErCnt = CAN_RXErrCntGet(CANx);
		ErrPassive = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;
		// printf("****TEC = %d, REC = %d, ErrPassive = %d, ErrBusoff = %d, ErrWarning = %d\n", TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning);

		check_rx_message(CANx);

	}

	/* bus off interrupt */
	if (IntStatus & CAN_BUSOFF_INT) {
		CAN_ClearINT(CANx, CAN_BIT_BUSOFF_INT_FLAG);
		printf("CAN1: bus off\n");
	}

	/* wakeup interrupt */
	if (IntStatus & CAN_WKUP_INT) {
		CAN_ClearINT(CANx, CAN_BIT_WAKEUP_INT_FLAG);
		printf("CAN1: wake up\n");
	}

	/* error interrupt */
	if (IntStatus & CAN_ERR_INT) {
		CAN_ClearINT(CANx, CAN_BIT_ERROR_INT_FLAG);
		printf("CAN1: Clear rx Status = %lx\n", CAN_GetINTStatus(CANx));

		ErrStatus = CAN_GetErrStatus(CANx);
		TxErCnt = CAN_TXErrCntGet(CANx);
		RxErCnt = CAN_RXErrCntGet(CANx);
		ErrPassive = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (CANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;

		if (ErrStatus & CAN_BIT_ERROR_BIT0) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_BIT0);
			printf("bit 0 error: tx = 0, but rx = 1\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_BIT1) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_BIT1);
			printf("bit 1 error: tx = 1, but rx = 0\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_FORM) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_FORM);
			printf("form error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_CRC) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_CRC);
			printf("CRC error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_STUFF) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_STUFF);
			printf("stuff error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_ACK) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_ACK);
			printf("ACK error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_TX) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_TX);
			printf("tx error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_RX) {
			CAN_ClearErrStatus(CANx, CAN_BIT_ERROR_RX);
			printf("rx error\n");
		}

		printf("ErrStatus = %lx, TEC = %ld, REC = %ld, ErrPassive = %ld, ErrBusoff = %ld, ErrWarning = %ld\n", ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff,
			   ErrWarning);

	}
	return 0;
}

static void can0_tx_thread(void *param)
{
	u32 i;
	CAN_TypeDef *CANx = (CAN_TypeDef *)param;
	CAN_TxMsgTypeDef TxMsg;
	do {
		_memset(&TxMsg, 0, sizeof(CAN_TxMsgTypeDef));

		TxMsg.RTR = CAN_DATA_FRAME;
		TxMsg.IDE = CAN_STANDARD_FRAME;
		TxMsg.RTR = CAN_DATA_FRAME;
		TxMsg.MsgBufferIdx = 0x0;
		TxMsg.StdId = 0x0;
		TxMsg.DLC = 8;
		for (i = 0; i < TxMsg.DLC; i++) {
			TxMsg.Data[i] = i;
		}

		CAN_WriteMsg(CANx, &TxMsg);
		rtos_time_delay_ms(1000);
	} while (1);
}

static void can1_tx_thread(void *param)
{
	u32 i;
	CAN_TypeDef *CANx = (CAN_TypeDef *)param;
	CAN_TxMsgTypeDef TxMsg;
	do {
		_memset(&TxMsg, 0, sizeof(CAN_TxMsgTypeDef));

		TxMsg.RTR = CAN_DATA_FRAME;
		TxMsg.IDE = CAN_STANDARD_FRAME;
		TxMsg.RTR = CAN_DATA_FRAME;
		TxMsg.MsgBufferIdx = 0x0;
		TxMsg.StdId = 0x55;
		TxMsg.DLC = 8;
		for (i = 0; i < TxMsg.DLC; i++) {
			TxMsg.Data[i] = i;
		}

		CAN_WriteMsg(CANx, &TxMsg);
		rtos_time_delay_ms(1000);
	} while (1);
}
static void EMC_CANInit(void)
{
	CAN_TypeDef *CANx_0, * CANx_1;
	IRQn_Type  CAN_IRQ_0, CAN_IRQ_1;
	CAN_InitTypeDef  CAN_InitStruct_0, CAN_InitStruct_1;
	CAN_RxMsgTypeDef RxMsg_0, RxMsg_1;

	CAN_CoreClockSet();
	RCC_PeriphClockCmd(APBPeriph_CAN0, APBPeriph_CAN0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_CAN1, APBPeriph_CAN1_CLOCK, ENABLE);

	Pinmux_Config(CAN0_TX, PINMUX_FUNCTION_CAN0_TX);
	Pinmux_Config(CAN0_RX, PINMUX_FUNCTION_CAN0_RX);
	Pinmux_Config(CAN1_TX, PINMUX_FUNCTION_CAN1_TX);
	Pinmux_Config(CAN1_RX, PINMUX_FUNCTION_CAN1_RX);
	/*Pull the STB pin low to put the CAN transceiver into normal mode.*/
	PAD_PullCtrl(CAN0_STB, GPIO_PuPd_DOWN);
	PAD_PullCtrl(CAN1_STB, GPIO_PuPd_DOWN);

	CANx_0 = CAN_DEV_TABLE[0].CANx;
	CANx_1 = CAN_DEV_TABLE[1].CANx;
	CAN_IRQ_0 = CAN_DEV_TABLE[0].IrqNum;
	CAN_IRQ_1 = CAN_DEV_TABLE[1].IrqNum;

	//init can0
	CAN_BusCmd(CANx_0, DISABLE);
	CAN_StructInit(&CAN_InitStruct_0);
	CAN_InitStruct_0.CAN_RxFifoEn = DISABLE;
	CAN_CalcBitTiming(1000000, &CAN_InitStruct_0.CAN_Timing); /* 1Mbps */
	CAN_Init(CANx_0, &CAN_InitStruct_0);

	CAN_RamBufferMapConfig(CANx_0, can_ram_buffer_map);

	InterruptDis(CAN_IRQ_0);
	InterruptUnRegister(CAN_IRQ_0);
	InterruptRegister((IRQ_FUN)can0_interrupt_handler, CAN_IRQ_0, NULL, INT_PRI_MIDDLE);
	InterruptEn(CAN_IRQ_0, INT_PRI_MIDDLE);
	CAN_INTConfig(CANx_0, (CAN_TX_INT | CAN_RX_INT | CAN_RAM_MOVE_DONE_INT | CAN_ERR_INT | CAN_BUSOFF_INT | CAN_WKUP_INT), ENABLE);
	CAN_TxMsgBufINTConfig(CANx_0, CAN_MB_TXINT_EN(0xFFFF), ENABLE);
	CAN_RxMsgBufINTConfig(CANx_0, CAN_MB_RXINT_EN(0xFFFF), ENABLE);

	CAN_Cmd(CANx_0, ENABLE);
	CAN_BusCmd(CANx_0, ENABLE);

	while (!CAN_BusStatusGet(CANx_0));
	printf("can0 bus on\n");

	//init can1
	CAN_BusCmd(CANx_1, DISABLE);
	CAN_StructInit(&CAN_InitStruct_1);
	CAN_InitStruct_1.CAN_RxFifoEn = DISABLE;
	CAN_CalcBitTiming(1000000, &CAN_InitStruct_1.CAN_Timing); /* 1Mbps */
	CAN_Init(CANx_1, &CAN_InitStruct_1);

	CAN_RamBufferMapConfig(CANx_1, can_ram_buffer_map);

	InterruptDis(CAN_IRQ_1);
	InterruptUnRegister(CAN_IRQ_1);
	InterruptRegister((IRQ_FUN)can1_interrupt_handler, CAN_IRQ_1, NULL, INT_PRI_MIDDLE);
	InterruptEn(CAN_IRQ_1, INT_PRI_MIDDLE);
	CAN_INTConfig(CANx_1, (CAN_TX_INT | CAN_RX_INT | CAN_RAM_MOVE_DONE_INT | CAN_ERR_INT | CAN_BUSOFF_INT | CAN_WKUP_INT), ENABLE);
	CAN_TxMsgBufINTConfig(CANx_1, CAN_MB_TXINT_EN(0xFFFF), ENABLE);
	CAN_RxMsgBufINTConfig(CANx_1, CAN_MB_RXINT_EN(0xFFFF), ENABLE);

	CAN_Cmd(CANx_1, ENABLE);
	CAN_BusCmd(CANx_1, ENABLE);
	while (!CAN_BusStatusGet(CANx_1));
	printf("can1 bus on\n");

	//cocnfig can0 rx, check rx data in interrupt_handler
	_memset(&RxMsg_0, 0, sizeof(CAN_RxMsgTypeDef));
	RxMsg_0.RTR = CAN_DATA_FRAME;
	RxMsg_0.IDE = CAN_STANDARD_FRAME;
	RxMsg_0.MsgBufferIdx = 15;
	RxMsg_0.ExtId = 0x0;
	RxMsg_0.StdId = 0x0;
	RxMsg_0.ID_MASK = 0x0;
	RxMsg_0.RTR_Mask = 0x0;
	RxMsg_0.IDE_Mask = 0x0;
	CAN_SetRxMsgBuf(CANx_0, &RxMsg_0);

	//config can1 rx, check rx data in interrupt_handler
	_memset(&RxMsg_1, 0, sizeof(CAN_RxMsgTypeDef));
	RxMsg_1.RTR = CAN_DATA_FRAME;
	RxMsg_1.IDE = CAN_STANDARD_FRAME;
	RxMsg_1.MsgBufferIdx = 15;
	RxMsg_1.ExtId = 0x0;
	RxMsg_1.StdId = 0x0;
	RxMsg_1.ID_MASK = 0x0;
	RxMsg_1.RTR_Mask = 0x0;
	RxMsg_1.IDE_Mask = 0x0;
	CAN_SetRxMsgBuf(CANx_1, &RxMsg_1);
}

int example_raw_can_trx(void)
{
	EMC_CANInit();

	//config can0 tx thread: std data frame: id = 0x0, 0x0~0x7
	if (rtos_task_create(NULL, "CAN0_TX_THREAD", can0_tx_thread, (void *)CAN0, 2048, 2) != RTK_SUCCESS) {
		printf("\n%s: Create CAN0 tx thread error\n", __FUNCTION__);
	}

	//config can1 tx thread: std data frame: id = 0xff, 0x55
	if (rtos_task_create(NULL, "CAN1_TX_THREAD", can1_tx_thread, (void *)CAN1, 2048, 2) != RTK_SUCCESS) {
		printf("\n%s: Create can1 tx thread Err!\n", __FUNCTION__);
	}

	return 0;
}