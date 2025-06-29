#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

static u32 can_ram_buffer_map[] = {
	0x0, 0x6, 0xc, 0x12, 0x18, 0x1e, 0x24, 0x2a, 0x30,
	0x36, 0x3c, 0x42, 0x48, 0x4e, 0x54, 0x5a, 0x60
};
/*dump rx message*/
static void dump_rx_message(RCAN_RxMsgTypeDef *RxMsg)
{
	u32 i;

	printf("######################RX message start######################\n");
	printf("RxMsg->StdId = %lx\n", RxMsg->StdId);
	printf("RxMsg->ExtId = %lx\n", RxMsg->ExtId);
	printf("RxMsg->MsgBufferIdx = %ld\n", RxMsg->MsgBufferIdx);

	if (RxMsg->IDE == RCAN_STANDARD_FRAME) {
		printf("RxMsg->IDE = %lx (standard frame)\n", RxMsg->IDE);
	} else if (RxMsg->IDE == RCAN_EXTEND_FRAME) {
		printf("RxMsg->IDE = %lx (extend frame)\n", RxMsg->IDE);
	} else {
		printf("wrong IDE value !!!: RxMsg->IDE = %lx\n", RxMsg->IDE);
	}

	if (RxMsg->RTR == RCAN_DATA_FRAME) {
		printf("RxMsg->RTR = %lx (data frame)\n", RxMsg->RTR);
	} else if (RxMsg->RTR == RCAN_REMOTE_FRAME) {
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
static void check_rx_message(RCAN_TypeDef *RCANx)
{
	u32 i;
	u32 status = 0;
	RCAN_RxMsgTypeDef RxMsg;

	_memset(&RxMsg, 0, sizeof(RCAN_RxMsgTypeDef));

	for (i = RCAN_MESSAGE_BUFFER_SIZE; i >  0; i--) {
		RxMsg.MsgBufferIdx = i - 1;
		if (RCAN_MsgBufRxDoneStatusGet(RCANx, RxMsg.MsgBufferIdx)) {
			RCAN_MsgBufRxDoneStatusClear(RCANx, RxMsg.MsgBufferIdx);
			RCAN_ReadMsg(RCANx, &RxMsg);
			dump_rx_message(&RxMsg);

			if (RxMsg.StdId != 0x0 && RxMsg.StdId != 0x55) {
				status = 1;
			}
			if (RxMsg.IDE != RCAN_STANDARD_FRAME) {
				status = 1;
			}
			if (RxMsg.RTR != RCAN_DATA_FRAME) {
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
	RCAN_TypeDef *RCANx = CAN_DEV_TABLE[0].RCANx;
	u32 IntStatus, ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning;

	IntStatus = RCAN_GetINTStatus(RCANx);

	/*ram move done interrupt */
	if (IntStatus & RCAN_RAM_MOVE_DONE_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_RAM_MOVE_DONE_INT_FLAG);

	}

	/*tx interrupt*/
	if (IntStatus & RCAN_TX_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_TX_INT_FLAG);

	}

	/*rx interrupt*/
	if (IntStatus & RCAN_RX_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_RX_INT_FLAG);
		printf("Clear Interrupt Status = %lx\n", RCAN_GetINTStatus(RCANx));

		/* get current error status */
		TxErCnt = RCAN_TXErrCntGet(RCANx);
		RxErCnt = RCAN_RXErrCntGet(RCANx);
		ErrPassive = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;
		printf("****TEC = %ld, REC = %ld, ErrPassive = %ld, ErrBusoff = %ld, ErrWarning = %ld\n", TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning);

		check_rx_message(RCANx);

	}

	/* bus off interrupt */
	if (IntStatus & RCAN_BUSOFF_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_BUSOFF_INT_FLAG);
		printf("CAN0: bus off\n");
	}

	/* wakeup interrupt */
	if (IntStatus & RCAN_WKUP_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_WAKEUP_INT_FLAG);
		printf("CAN0: wake up\n");
	}

	/* error interrupt */
	if (IntStatus & RCAN_ERR_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_ERROR_INT_FLAG);
		printf("CAN0: Clear Interrupt Status = %lx\n", RCAN_GetINTStatus(RCANx));

		ErrStatus = RCAN_GetErrStatus(RCANx);
		TxErCnt = RCAN_TXErrCntGet(RCANx);
		RxErCnt = RCAN_RXErrCntGet(RCANx);
		ErrPassive = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;

		if (ErrStatus & CAN_BIT_ERROR_BIT0) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_BIT0);
			printf("bit 0 error: tx = 0, but rx = 1\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_BIT1) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_BIT1);
			printf("bit 1 error: tx = 1, but rx = 0\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_FORM) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_FORM);
			printf("form error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_CRC) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_CRC);
			printf("CRC error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_STUFF) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_STUFF);
			printf("stuff error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_ACK) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_ACK);
			printf("ACK error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_TX) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_TX);
			printf("tx error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_RX) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_RX);
			printf("rx error\n");
		}

		printf("TEC = %ld, REC = %ld\n", TxErCnt, RxErCnt);
	}
	return 0;
}

static u32 can1_interrupt_handler(void *Data)
{
	(void)Data;
	RCAN_TypeDef *RCANx = CAN_DEV_TABLE[1].RCANx;
	u32 IntStatus, ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning;

	IntStatus = RCAN_GetINTStatus(RCANx);
	/*ram move done interrupt */
	if (IntStatus & RCAN_RAM_MOVE_DONE_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_RAM_MOVE_DONE_INT_FLAG);

	}

	/*tx interrupt*/
	if (IntStatus & RCAN_TX_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_TX_INT_FLAG);
	}

	/*rx interrupt*/
	if (IntStatus & RCAN_RX_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_RX_INT_FLAG);
		// printf("Clear rx Status = %lx\n", RCAN_GetINTStatus(RCANx));

		/* get current error status */
		TxErCnt = RCAN_TXErrCntGet(RCANx);
		RxErCnt = RCAN_RXErrCntGet(RCANx);
		ErrPassive = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;
		// printf("****TEC = %d, REC = %d, ErrPassive = %d, ErrBusoff = %d, ErrWarning = %d\n", TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning);

		check_rx_message(RCANx);

	}

	/* bus off interrupt */
	if (IntStatus & RCAN_BUSOFF_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_BUSOFF_INT_FLAG);
		printf("CAN1: bus off\n");
	}

	/* wakeup interrupt */
	if (IntStatus & RCAN_WKUP_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_WAKEUP_INT_FLAG);
		printf("CAN1: wake up\n");
	}

	/* error interrupt */
	if (IntStatus & RCAN_ERR_INT) {
		RCAN_ClearINT(RCANx, CAN_BIT_ERROR_INT_FLAG);
		printf("CAN1: Clear rx Status = %lx\n", RCAN_GetINTStatus(RCANx));

		ErrStatus = RCAN_GetErrStatus(RCANx);
		TxErCnt = RCAN_TXErrCntGet(RCANx);
		RxErCnt = RCAN_RXErrCntGet(RCANx);
		ErrPassive = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (RCANx->CAN_ERR_CNT_STS & CAN_BIT_ERROR_WARNING) >> 30;

		if (ErrStatus & CAN_BIT_ERROR_BIT0) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_BIT0);
			printf("bit 0 error: tx = 0, but rx = 1\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_BIT1) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_BIT1);
			printf("bit 1 error: tx = 1, but rx = 0\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_FORM) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_FORM);
			printf("form error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_CRC) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_CRC);
			printf("CRC error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_STUFF) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_STUFF);
			printf("stuff error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_ACK) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_ACK);
			printf("ACK error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_TX) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_TX);
			printf("tx error\n");
		}
		if (ErrStatus & CAN_BIT_ERROR_RX) {
			RCAN_ClearErrStatus(RCANx, CAN_BIT_ERROR_RX);
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
	RCAN_TypeDef *RCANx = (RCAN_TypeDef *)param;
	RCAN_TxMsgTypeDef TxMsg;
	do {
		_memset(&TxMsg, 0, sizeof(RCAN_TxMsgTypeDef));

		TxMsg.EDL = RCAN_NOT_FD_FRAME_TYPE;
		TxMsg.RTR = RCAN_DATA_FRAME;
		TxMsg.IDE = RCAN_STANDARD_FRAME;
		TxMsg.RTR = RCAN_DATA_FRAME;
		TxMsg.MsgBufferIdx = 0x0;
		TxMsg.StdId = 0x0;
		TxMsg.DLC = 8;
		for (i = 0; i < TxMsg.DLC; i++) {
			TxMsg.Data[i] = i;
		}

		RCAN_WriteMsg(RCANx, &TxMsg);
		rtos_time_delay_ms(1000);
	} while (1);
}

static void can1_tx_thread(void *param)
{
	u32 i;
	RCAN_TypeDef *RCANx = (RCAN_TypeDef *)param;
	RCAN_TxMsgTypeDef TxMsg;
	do {
		_memset(&TxMsg, 0, sizeof(RCAN_TxMsgTypeDef));

		TxMsg.EDL = RCAN_NOT_FD_FRAME_TYPE;
		TxMsg.RTR = RCAN_DATA_FRAME;
		TxMsg.IDE = RCAN_STANDARD_FRAME;
		TxMsg.RTR = RCAN_DATA_FRAME;
		TxMsg.MsgBufferIdx = 0x0;
		TxMsg.StdId = 0x55;
		TxMsg.DLC = 8;
		for (i = 0; i < TxMsg.DLC; i++) {
			TxMsg.Data[i] = i;
		}

		RCAN_WriteMsg(RCANx, &TxMsg);
		rtos_time_delay_ms(1000);
	} while (1);
}
static void EMC_CANInit(void)
{
	RCAN_TypeDef *RCANx_0, * RCANx_1;
	IRQn_Type  CAN_IRQ_0, CAN_IRQ_1;
	RCAN_InitTypeDef  RCAN_InitStruct_0, RCAN_InitStruct_1;
	RCAN_RxMsgTypeDef RxMsg_0, RxMsg_1;

	RCC_PeriphClockCmd(APBPeriph_CAN0, APBPeriph_CAN0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_CAN1, APBPeriph_CAN1_CLOCK, ENABLE);
	RCC_PeriphClockDividerFENSet(USB_PLL_CAN, DISABLE);
	RCC_PeriphClockDividerFENSet(SYS_PLL_CAN, DISABLE);
	RCC_PeriphClockSourceSet(CAN, XTAL);
	Pinmux_Config(_PA_13, PINMUX_FUNCTION_CAN0_TX);
	Pinmux_Config(_PA_12, PINMUX_FUNCTION_CAN0_RX);
	Pinmux_Config(_PA_15, PINMUX_FUNCTION_CAN1_TX);
	Pinmux_Config(_PA_14, PINMUX_FUNCTION_CAN1_RX);

	RCANx_0 = CAN_DEV_TABLE[0].RCANx;
	RCANx_1 = CAN_DEV_TABLE[1].RCANx;
	CAN_IRQ_0 = CAN_DEV_TABLE[0].IrqNum;
	CAN_IRQ_1 = CAN_DEV_TABLE[1].IrqNum;

	//init can0
	RCAN_BusCmd(RCANx_0, DISABLE);
	RCAN_StructInit(&RCAN_InitStruct_0);
	RCAN_Init(RCANx_0, &RCAN_InitStruct_0);

	RCAN_RamBufferMapConfig(RCANx_0, can_ram_buffer_map);

	InterruptDis(CAN_IRQ_0);
	InterruptUnRegister(CAN_IRQ_0);
	InterruptRegister((IRQ_FUN)can0_interrupt_handler, CAN_IRQ_0, NULL, INT_PRI_MIDDLE);
	InterruptEn(CAN_IRQ_0, INT_PRI_MIDDLE);
	RCAN_INTConfig(RCANx_0, (RCAN_TX_INT | RCAN_RX_INT | RCAN_RAM_MOVE_DONE_INT | RCAN_ERR_INT | RCAN_BUSOFF_INT | RCAN_WKUP_INT), ENABLE);
	RCAN_TxMsgBufINTConfig(RCANx_0, CAN_MB_TXINT_EN(0xFFFF), ENABLE);
	RCAN_RxMsgBufINTConfig(RCANx_0, CAN_MB_RXINT_EN(0xFFFF), ENABLE);

	RCAN_Cmd(RCANx_0, ENABLE);
	RCAN_BusCmd(RCANx_0, ENABLE);

	while (!RCAN_BusStatusGet(RCANx_0));
	printf("can0 bus on\n");

	//init can1
	RCAN_BusCmd(RCANx_1, DISABLE);
	RCAN_StructInit(&RCAN_InitStruct_1);
	RCAN_Init(RCANx_1, &RCAN_InitStruct_1);

	RCAN_RamBufferMapConfig(RCANx_1, can_ram_buffer_map);

	InterruptDis(CAN_IRQ_1);
	InterruptUnRegister(CAN_IRQ_1);
	InterruptRegister((IRQ_FUN)can1_interrupt_handler, CAN_IRQ_1, NULL, INT_PRI_MIDDLE);
	InterruptEn(CAN_IRQ_1, INT_PRI_MIDDLE);
	RCAN_INTConfig(RCANx_1, (RCAN_TX_INT | RCAN_RX_INT | RCAN_RAM_MOVE_DONE_INT | RCAN_ERR_INT | RCAN_BUSOFF_INT | RCAN_WKUP_INT), ENABLE);
	RCAN_TxMsgBufINTConfig(RCANx_1, CAN_MB_TXINT_EN(0xFFFF), ENABLE);
	RCAN_RxMsgBufINTConfig(RCANx_1, CAN_MB_RXINT_EN(0xFFFF), ENABLE);

	RCAN_Cmd(RCANx_1, ENABLE);
	RCAN_BusCmd(RCANx_1, ENABLE);
	while (!RCAN_BusStatusGet(RCANx_1));
	printf("can1 bus on\n");

	//cocnfig can0 rx, check rx data in interrupt_handler
	_memset(&RxMsg_0, 0, sizeof(RCAN_RxMsgTypeDef));
	RxMsg_0.RTR = RCAN_DATA_FRAME;
	RxMsg_0.IDE = RCAN_STANDARD_FRAME;
	RxMsg_0.MsgBufferIdx = 15;
	RxMsg_0.ExtId = 0x0;
	RxMsg_0.StdId = 0x0;
	RxMsg_0.ID_MASK = 0x0;
	RxMsg_0.RTR_Mask = 0x0;
	RxMsg_0.IDE_Mask = 0x0;
	RCAN_SetRxMsgBuf(RCANx_0, &RxMsg_0);

	//config can1 rx, check rx data in interrupt_handler
	_memset(&RxMsg_1, 0, sizeof(RCAN_RxMsgTypeDef));
	RxMsg_1.RTR = RCAN_DATA_FRAME;
	RxMsg_1.IDE = RCAN_STANDARD_FRAME;
	RxMsg_1.MsgBufferIdx = 15;
	RxMsg_1.ExtId = 0x0;
	RxMsg_1.StdId = 0x0;
	RxMsg_1.ID_MASK = 0x0;
	RxMsg_1.RTR_Mask = 0x0;
	RxMsg_1.IDE_Mask = 0x0;
	RCAN_SetRxMsgBuf(RCANx_1, &RxMsg_1);
}

int example_raw_can_trx(void)
{
	EMC_CANInit();

	//config can0 tx thread: std data frame: id = 0x0, 0x0~0x7
	if (rtos_task_create(NULL, "CAN0_TX_THREAD", can0_tx_thread, (void *)RCAN0, 2048, 2) != RTK_SUCCESS) {
		printf("\n%s: Create CAN0 tx thread error\n", __FUNCTION__);
	}

	//config can1 tx thread: std data frame: id = 0xff, 0x55
	if (rtos_task_create(NULL, "CAN1_TX_THREAD", can1_tx_thread, (void *)RCAN1, 2048, 2) != RTK_SUCCESS) {
		printf("\n%s: Create can1 tx thread Err!\n", __FUNCTION__);
	}
	rtos_sched_start();
	return 0;
}