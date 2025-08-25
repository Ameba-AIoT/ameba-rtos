#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

static u32 a2c_ram_buffer_map[] = {
	0x0, 0x6, 0xc, 0x12, 0x18, 0x1e, 0x24, 0x2a, 0x30,
	0x36, 0x3c, 0x42, 0x48, 0x4e, 0x54, 0x5a, 0x60
};
/*dump rx message*/
static void dump_rx_message(A2C_RxMsgTypeDef *RxMsg)
{
	u32 i;

	printf("######################RX message start######################\n");
	printf("RxMsg->StdId = %lx\n", RxMsg->StdId);
	printf("RxMsg->ExtId = %lx\n", RxMsg->ExtId);
	printf("RxMsg->MsgBufferIdx = %ld\n", RxMsg->MsgBufferIdx);

	if (RxMsg->IDE == A2C_STANDARD_FRAME) {
		printf("RxMsg->IDE = %lx (standard frame)\n", RxMsg->IDE);
	} else if (RxMsg->IDE == A2C_EXTEND_FRAME) {
		printf("RxMsg->IDE = %lx (extend frame)\n", RxMsg->IDE);
	} else {
		printf("wrong IDE value !!!: RxMsg->IDE = %lx\n", RxMsg->IDE);
	}

	if (RxMsg->RTR == A2C_DATA_FRAME) {
		printf("RxMsg->RTR = %lx (data frame)\n", RxMsg->RTR);
	} else if (RxMsg->RTR == A2C_REMOTE_FRAME) {
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
//a2c frame: id = 0x0 or 0x55, std, data, 0x00~0x07
static void check_rx_message(A2C_TypeDef *A2Cx)
{
	u32 i;
	u32 status = 0;
	A2C_RxMsgTypeDef RxMsg;

	_memset(&RxMsg, 0, sizeof(A2C_RxMsgTypeDef));

	for (i = A2C_MESSAGE_BUFFER_SIZE; i >  0; i--) {
		RxMsg.MsgBufferIdx = i - 1;
		if (A2C_MsgBufRxDoneStatusGet(A2Cx, RxMsg.MsgBufferIdx)) {
			A2C_MsgBufRxDoneStatusClear(A2Cx, RxMsg.MsgBufferIdx);
			A2C_ReadMsg(A2Cx, &RxMsg);
			dump_rx_message(&RxMsg);

			if (RxMsg.StdId != 0x0 && RxMsg.StdId != 0x55) {
				status = 1;
			}
			if (RxMsg.IDE != A2C_STANDARD_FRAME) {
				status = 1;
			}
			if (RxMsg.RTR != A2C_DATA_FRAME) {
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


static u32 a2c0_interrupt_handler(void *Data)
{
	(void)Data;
	A2C_TypeDef *A2Cx = A2C_DEV_TABLE[0].A2Cx;
	u32 IntStatus, ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning;

	IntStatus = A2C_GetINTStatus(A2Cx);

	/*ram move done interrupt */
	if (IntStatus & A2C_RAM_MOVE_DONE_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_RAM_MOVE_DONE_INT_FLAG);

	}

	/*tx interrupt*/
	if (IntStatus & A2C_TX_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_TX_INT_FLAG);

	}

	/*rx interrupt*/
	if (IntStatus & A2C_RX_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_RX_INT_FLAG);
		printf("Clear Interrupt Status = %lx\n", A2C_GetINTStatus(A2Cx));

		/* get current error status */
		TxErCnt = A2C_TXErrCntGet(A2Cx);
		RxErCnt = A2C_RXErrCntGet(A2Cx);
		ErrPassive = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_WARNING) >> 30;
		printf("****TEC = %ld, REC = %ld, ErrPassive = %ld, ErrBusoff = %ld, ErrWarning = %ld\n", TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning);

		check_rx_message(A2Cx);

	}

	/* bus off interrupt */
	if (IntStatus & A2C_BUSOFF_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_BUSOFF_INT_FLAG);
		printf("A2C0: bus off\n");
	}

	/* wakeup interrupt */
	if (IntStatus & A2C_WKUP_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_WAKEUP_INT_FLAG);
		printf("A2C0: wake up\n");
	}

	/* error interrupt */
	if (IntStatus & A2C_ERR_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_ERROR_INT_FLAG);
		printf("A2C0: Clear Interrupt Status = %lx\n", A2C_GetINTStatus(A2Cx));

		ErrStatus = A2C_GetErrStatus(A2Cx);
		TxErCnt = A2C_TXErrCntGet(A2Cx);
		RxErCnt = A2C_RXErrCntGet(A2Cx);
		ErrPassive = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_WARNING) >> 30;

		if (ErrStatus & A2C_BIT_ERROR_BIT0) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_BIT0);
			printf("bit 0 error: tx = 0, but rx = 1\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_BIT1) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_BIT1);
			printf("bit 1 error: tx = 1, but rx = 0\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_FORM) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_FORM);
			printf("form error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_CRC) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_CRC);
			printf("CRC error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_STUFF) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_STUFF);
			printf("stuff error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_ACK) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_ACK);
			printf("ACK error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_TX) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_TX);
			printf("tx error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_RX) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_RX);
			printf("rx error\n");
		}

		printf("TEC = %ld, REC = %ld\n", TxErCnt, RxErCnt);
	}
	return 0;
}

static u32 a2c1_interrupt_handler(void *Data)
{
	(void)Data;
	A2C_TypeDef *A2Cx = A2C_DEV_TABLE[1].A2Cx;
	u32 IntStatus, ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning;

	IntStatus = A2C_GetINTStatus(A2Cx);
	/*ram move done interrupt */
	if (IntStatus & A2C_RAM_MOVE_DONE_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_RAM_MOVE_DONE_INT_FLAG);

	}

	/*tx interrupt*/
	if (IntStatus & A2C_TX_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_TX_INT_FLAG);
	}

	/*rx interrupt*/
	if (IntStatus & A2C_RX_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_RX_INT_FLAG);
		// printf("Clear rx Status = %lx\n", A2C_GetINTStatus(A2Cx));

		/* get current error status */
		TxErCnt = A2C_TXErrCntGet(A2Cx);
		RxErCnt = A2C_RXErrCntGet(A2Cx);
		ErrPassive = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_WARNING) >> 30;
		// printf("****TEC = %d, REC = %d, ErrPassive = %d, ErrBusoff = %d, ErrWarning = %d\n", TxErCnt, RxErCnt, ErrPassive, ErrBusoff, ErrWarning);

		check_rx_message(A2Cx);

	}

	/* bus off interrupt */
	if (IntStatus & A2C_BUSOFF_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_BUSOFF_INT_FLAG);
		printf("A2C1: bus off\n");
	}

	/* wakeup interrupt */
	if (IntStatus & A2C_WKUP_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_WAKEUP_INT_FLAG);
		printf("A2C1: wake up\n");
	}

	/* error interrupt */
	if (IntStatus & A2C_ERR_INT) {
		A2C_ClearINT(A2Cx, A2C_BIT_ERROR_INT_FLAG);
		printf("A2C1: Clear rx Status = %lx\n", A2C_GetINTStatus(A2Cx));

		ErrStatus = A2C_GetErrStatus(A2Cx);
		TxErCnt = A2C_TXErrCntGet(A2Cx);
		RxErCnt = A2C_RXErrCntGet(A2Cx);
		ErrPassive = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_PASSIVE) >> 28;
		ErrBusoff = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_BUSOFF) >> 29;
		ErrWarning = (A2Cx->A2C_ERR_CNT_STS & A2C_BIT_ERROR_WARNING) >> 30;

		if (ErrStatus & A2C_BIT_ERROR_BIT0) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_BIT0);
			printf("bit 0 error: tx = 0, but rx = 1\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_BIT1) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_BIT1);
			printf("bit 1 error: tx = 1, but rx = 0\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_FORM) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_FORM);
			printf("form error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_CRC) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_CRC);
			printf("CRC error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_STUFF) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_STUFF);
			printf("stuff error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_ACK) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_ACK);
			printf("ACK error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_TX) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_TX);
			printf("tx error\n");
		}
		if (ErrStatus & A2C_BIT_ERROR_RX) {
			A2C_ClearErrStatus(A2Cx, A2C_BIT_ERROR_RX);
			printf("rx error\n");
		}

		printf("ErrStatus = %lx, TEC = %ld, REC = %ld, ErrPassive = %ld, ErrBusoff = %ld, ErrWarning = %ld\n", ErrStatus, TxErCnt, RxErCnt, ErrPassive, ErrBusoff,
			   ErrWarning);

	}
	return 0;
}

static void a2c0_tx_thread(void *param)
{
	u32 i;
	A2C_TypeDef *A2Cx = (A2C_TypeDef *)param;
	A2C_TxMsgTypeDef TxMsg;
	do {
		_memset(&TxMsg, 0, sizeof(A2C_TxMsgTypeDef));

		TxMsg.EDL = A2C_NOT_FD_FRAME_TYPE;
		TxMsg.RTR = A2C_DATA_FRAME;
		TxMsg.IDE = A2C_STANDARD_FRAME;
		TxMsg.RTR = A2C_DATA_FRAME;
		TxMsg.MsgBufferIdx = 0x0;
		TxMsg.StdId = 0x0;
		TxMsg.DLC = 8;
		for (i = 0; i < TxMsg.DLC; i++) {
			TxMsg.Data[i] = i;
		}

		A2C_WriteMsg(A2Cx, &TxMsg);
		rtos_time_delay_ms(1000);
	} while (1);
}

static void a2c1_tx_thread(void *param)
{
	u32 i;
	A2C_TypeDef *A2Cx = (A2C_TypeDef *)param;
	A2C_TxMsgTypeDef TxMsg;
	do {
		_memset(&TxMsg, 0, sizeof(A2C_TxMsgTypeDef));

		TxMsg.EDL = A2C_NOT_FD_FRAME_TYPE;
		TxMsg.RTR = A2C_DATA_FRAME;
		TxMsg.IDE = A2C_STANDARD_FRAME;
		TxMsg.RTR = A2C_DATA_FRAME;
		TxMsg.MsgBufferIdx = 0x0;
		TxMsg.StdId = 0x55;
		TxMsg.DLC = 8;
		for (i = 0; i < TxMsg.DLC; i++) {
			TxMsg.Data[i] = i;
		}

		A2C_WriteMsg(A2Cx, &TxMsg);
		rtos_time_delay_ms(1000);
	} while (1);
}
static void EMC_A2CInit(void)
{
	A2C_TypeDef *A2Cx_0, * A2Cx_1;
	IRQn_Type  A2C_IRQ_0, A2C_IRQ_1;
	A2C_InitTypeDef  A2C_InitStruct_0, A2C_InitStruct_1;
	A2C_RxMsgTypeDef RxMsg_0, RxMsg_1;

	RCC_PeriphClockCmd(APBPeriph_A2C0, APBPeriph_A2C0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_A2C1, APBPeriph_A2C1_CLOCK, ENABLE);
	RCC_PeriphClockDividerFENSet(USB_PLL_A2C, DISABLE);
	RCC_PeriphClockDividerFENSet(SYS_PLL_A2C, DISABLE);
	RCC_PeriphClockSourceSet(A2C, XTAL);
	Pinmux_Config(_PA_13, PINMUX_FUNCTION_A2C0_TX);
	Pinmux_Config(_PA_12, PINMUX_FUNCTION_A2C0_RX);
	Pinmux_Config(_PA_15, PINMUX_FUNCTION_A2C1_TX);
	Pinmux_Config(_PA_14, PINMUX_FUNCTION_A2C1_RX);

	A2Cx_0 = A2C_DEV_TABLE[0].A2Cx;
	A2Cx_1 = A2C_DEV_TABLE[1].A2Cx;
	A2C_IRQ_0 = A2C_DEV_TABLE[0].IrqNum;
	A2C_IRQ_1 = A2C_DEV_TABLE[1].IrqNum;

	//init a2c0
	A2C_BusCmd(A2Cx_0, DISABLE);
	A2C_StructInit(&A2C_InitStruct_0);
	A2C_Init(A2Cx_0, &A2C_InitStruct_0);

	A2C_RamBufferMapConfig(A2Cx_0, a2c_ram_buffer_map);

	InterruptDis(A2C_IRQ_0);
	InterruptUnRegister(A2C_IRQ_0);
	InterruptRegister((IRQ_FUN)a2c0_interrupt_handler, A2C_IRQ_0, NULL, INT_PRI_MIDDLE);
	InterruptEn(A2C_IRQ_0, INT_PRI_MIDDLE);
	A2C_INTConfig(A2Cx_0, (A2C_TX_INT | A2C_RX_INT | A2C_RAM_MOVE_DONE_INT | A2C_ERR_INT | A2C_BUSOFF_INT | A2C_WKUP_INT), ENABLE);
	A2C_TxMsgBufINTConfig(A2Cx_0, A2C_MB_TXINT_EN(0xFFFF), ENABLE);
	A2C_RxMsgBufINTConfig(A2Cx_0, A2C_MB_RXINT_EN(0xFFFF), ENABLE);

	A2C_Cmd(A2Cx_0, ENABLE);
	A2C_BusCmd(A2Cx_0, ENABLE);

	while (!A2C_BusStatusGet(A2Cx_0));
	printf("a2c0 bus on\n");

	//init a2c1
	A2C_BusCmd(A2Cx_1, DISABLE);
	A2C_StructInit(&A2C_InitStruct_1);
	A2C_Init(A2Cx_1, &A2C_InitStruct_1);

	A2C_RamBufferMapConfig(A2Cx_1, a2c_ram_buffer_map);

	InterruptDis(A2C_IRQ_1);
	InterruptUnRegister(A2C_IRQ_1);
	InterruptRegister((IRQ_FUN)a2c1_interrupt_handler, A2C_IRQ_1, NULL, INT_PRI_MIDDLE);
	InterruptEn(A2C_IRQ_1, INT_PRI_MIDDLE);
	A2C_INTConfig(A2Cx_1, (A2C_TX_INT | A2C_RX_INT | A2C_RAM_MOVE_DONE_INT | A2C_ERR_INT | A2C_BUSOFF_INT | A2C_WKUP_INT), ENABLE);
	A2C_TxMsgBufINTConfig(A2Cx_1, A2C_MB_TXINT_EN(0xFFFF), ENABLE);
	A2C_RxMsgBufINTConfig(A2Cx_1, A2C_MB_RXINT_EN(0xFFFF), ENABLE);

	A2C_Cmd(A2Cx_1, ENABLE);
	A2C_BusCmd(A2Cx_1, ENABLE);
	while (!A2C_BusStatusGet(A2Cx_1));
	printf("a2c1 bus on\n");

	//cocnfig a2c0 rx, check rx data in interrupt_handler
	_memset(&RxMsg_0, 0, sizeof(A2C_RxMsgTypeDef));
	RxMsg_0.RTR = A2C_DATA_FRAME;
	RxMsg_0.IDE = A2C_STANDARD_FRAME;
	RxMsg_0.MsgBufferIdx = 15;
	RxMsg_0.ExtId = 0x0;
	RxMsg_0.StdId = 0x0;
	RxMsg_0.ID_MASK = 0x0;
	RxMsg_0.RTR_Mask = 0x0;
	RxMsg_0.IDE_Mask = 0x0;
	A2C_SetRxMsgBuf(A2Cx_0, &RxMsg_0);

	//config a2c1 rx, check rx data in interrupt_handler
	_memset(&RxMsg_1, 0, sizeof(A2C_RxMsgTypeDef));
	RxMsg_1.RTR = A2C_DATA_FRAME;
	RxMsg_1.IDE = A2C_STANDARD_FRAME;
	RxMsg_1.MsgBufferIdx = 15;
	RxMsg_1.ExtId = 0x0;
	RxMsg_1.StdId = 0x0;
	RxMsg_1.ID_MASK = 0x0;
	RxMsg_1.RTR_Mask = 0x0;
	RxMsg_1.IDE_Mask = 0x0;
	A2C_SetRxMsgBuf(A2Cx_1, &RxMsg_1);
}

int example_raw_a2c_trx(void)
{
	EMC_A2CInit();

	//config a2c0 tx thread: std data frame: id = 0x0, 0x0~0x7
	if (rtos_task_create(NULL, "A2C0_TX_THREAD", a2c0_tx_thread, (void *)A2C0, 2048, 2) != RTK_SUCCESS) {
		printf("\n%s: Create A2C0 tx thread error\n", __FUNCTION__);
	}

	//config a2c1 tx thread: std data frame: id = 0xff, 0x55
	if (rtos_task_create(NULL, "A2C1_TX_THREAD", a2c1_tx_thread, (void *)A2C1, 2048, 2) != RTK_SUCCESS) {
		printf("\n%s: Create a2c1 tx thread Err!\n", __FUNCTION__);
	}
	rtos_sched_start();
	return 0;
}