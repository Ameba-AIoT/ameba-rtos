/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *TAG = "A2C";

const struct A2C_DevTable A2C_DEV_TABLE[2] = {
	{A2C0, A2C0_IRQ, GDMA_HANDSHAKE_INTERFACE_A2C0_RX},
	{A2C1, A2C1_IRQ, GDMA_HANDSHAKE_INTERFACE_A2C1_RX},
};

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup A2C
* @brief A2C driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup A2C_Exported_Functions A2C Exported Functions
  * @{
  */

/**
  * @brief  Fills each A2C_InitStruct member with its default value.
  * @param  A2C_InitStruct: pointer to an A2C_InitTypeDef structure which will be initialized.
  * @retval   None
  */
void A2C_StructInit(A2C_InitTypeDef *A2C_InitStruct)
{
	A2C_InitStruct->A2C_AutoReTxEn = DISABLE;
	A2C_InitStruct->A2C_Timing.Prescaler = 2; //256;
	A2C_InitStruct->A2C_ErrCntThreshold = 64;
	A2C_InitStruct->A2C_RxFifoEn = ENABLE;
	A2C_InitStruct->A2C_Timing.SJW = 1; //8;
	A2C_InitStruct->A2C_TimStampDiv = 128;
	A2C_InitStruct->A2C_TimStampEn = ENABLE;
	A2C_InitStruct->A2C_TriSampleEn = DISABLE;
	A2C_InitStruct->A2C_Timing.PropSeg = 2;
	A2C_InitStruct->A2C_Timing.PhaseSeg1 = 13; //16;
	A2C_InitStruct->A2C_Timing.PhaseSeg2 = 4; //16;
	A2C_InitStruct->A2C_WorkMode = A2C_NORMAL_MODE;
}

/**
  * @brief    Initializes the A2C peripheral according to the specified
  *               parameters in the A2C_InitStruct.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  A2C_InitStruct: pointer to a A2C_InitTypeDef structure that contains
  *              the configuration information for the specified A2C peripheral.
  * @retval None
  */
void A2C_Init(A2C_TypeDef *A2Cx, A2C_InitTypeDef *A2C_InitStruct)
{
	u32 a2c_ctrl = 0;
	u32 a2c_bit_timing = 0;
	u32 a2c_error_cnt_ctl = 0;
	u32 a2c_time_stamp = 0;
	u32 a2c_test = 0;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	a2c_ctrl = A2Cx->A2C_CTL;
	a2c_bit_timing = A2Cx->A2C_BIT_TIMING;
	a2c_error_cnt_ctl = A2Cx->A2C_ERR_CNT_CTL;
	a2c_time_stamp = A2Cx->A2C_TIME_STAMP;
	a2c_test = A2Cx->A2C_TEST;
	/* 1.A2C 2.0 A/B bit prescaler*/
	a2c_bit_timing &= ~(A2C_MASK_BRP | A2C_MASK_SJW | A2C_MASK_TSEG2 | A2C_MASK_TSEG1);
	a2c_bit_timing |= A2C_BRP(A2C_InitStruct->A2C_Timing.Prescaler - 1) |
					  A2C_SJW(A2C_InitStruct->A2C_Timing.SJW) |
					  A2C_TSEG2(A2C_InitStruct->A2C_Timing.PhaseSeg2 - 1) |
					  A2C_TSEG1(A2C_InitStruct->A2C_Timing.PropSeg + A2C_InitStruct->A2C_Timing.PhaseSeg1 - 1);

	/*tripple sample*/
	if (A2C_InitStruct->A2C_TriSampleEn) {
		a2c_ctrl |= A2C_BIT_TRI_SAMPLE;
	} else {
		a2c_ctrl &= ~ A2C_BIT_TRI_SAMPLE;
	}
	/*auto tx trans*/
	if (A2C_InitStruct->A2C_AutoReTxEn) {
		a2c_ctrl |= A2C_BIT_AUTO_RE_TX_EN;
	} else {
		a2c_ctrl &= ~ A2C_BIT_AUTO_RE_TX_EN;
	}
	/*fifo mode*/
	if (A2C_InitStruct->A2C_RxFifoEn) {
		a2c_ctrl |= A2C_BIT_RX_FIFO_EN;
	} else {
		a2c_ctrl &= ~ A2C_BIT_RX_FIFO_EN;
	}

	/* 2. work mode or test mode*/
	if (A2C_InitStruct->A2C_WorkMode == A2C_NORMAL_MODE) {
		a2c_ctrl &= ~ A2C_BIT_TEST_MODE_EN;
	} else {
		/* test mdoe */
		a2c_ctrl |= A2C_BIT_TEST_MODE_EN;
		a2c_test &= ~ A2C_MASK_TEST_CFG;
		a2c_test |= A2C_TEST_CFG(A2C_InitStruct->A2C_WorkMode);
	}

	/* 3. Error count threshold*/
	a2c_error_cnt_ctl &= ~A2C_MASK_ERROR_WARN_TH;
	a2c_error_cnt_ctl |= A2C_ERROR_WARN_TH(A2C_InitStruct->A2C_ErrCntThreshold);

	/* 4. Stamp timer*/
	if (A2C_InitStruct->A2C_TimStampEn) {
		a2c_time_stamp &= ~(A2C_MASK_TIME_STAMP_DIV | A2C_BIT_TIME_STAMP_EN);
		a2c_time_stamp |= A2C_TIME_STAMP_DIV(A2C_InitStruct->A2C_TimStampDiv);
		a2c_time_stamp |= A2C_BIT_TIME_STAMP_EN;
	} else {
		a2c_time_stamp &= (~A2C_BIT_TIME_STAMP_EN);
	}

	A2Cx->A2C_CTL = a2c_ctrl;
	A2Cx->A2C_BIT_TIMING = a2c_bit_timing;
	A2Cx->A2C_ERR_CNT_CTL = a2c_error_cnt_ctl;
	A2Cx->A2C_TIME_STAMP = a2c_time_stamp;
	A2Cx->A2C_TEST = a2c_test;
}

/**
  * @brief  Low power filter configuration
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  FltNum: filter threshold.
  * @retval None
  */
void A2C_LpFilterConfig(A2C_TypeDef *A2Cx, u32 FltNum)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	A2Cx->A2C_SLEEP_MODE &= ~A2C_MASK_WAKEPIN_FLT_LENGTH;
	A2Cx->A2C_SLEEP_MODE |= A2C_WAKEPIN_FLT_LENGTH(FltNum);

}
/**
  * @brief    Low power filter ENABLE or DISABLE.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  NewState: where A2Cx can be ENABLE or DISABLE.
  * @retval None
  */
void A2C_LpFilterCmd(A2C_TypeDef *A2Cx, u32 NewState)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if (NewState == ENABLE) {
		A2Cx->A2C_SLEEP_MODE |= A2C_BIT_WAKEPIN_FLT_EN;
	} else {
		A2Cx->A2C_SLEEP_MODE &= (~A2C_BIT_WAKEPIN_FLT_EN);
	}
}

void A2C_RequestSleepOrWake(A2C_TypeDef *A2Cx, bool sleep)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	if (sleep) {
		A2Cx->A2C_SLEEP_MODE |= A2C_BIT_SLEEP_EN;
	} else {
		A2Cx->A2C_SLEEP_MODE &= ~A2C_BIT_SLEEP_EN;
	}
}

u32 A2C_SleepStatusGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	return (A2Cx->A2C_SLEEP_MODE & A2C_BIT_SLEEP_STATE);
}

/**
  * @brief    bus request ENABLE or DISABLE.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  NewState: where A2Cx can be ENABLE or DISABLE.
  * @retval None
  */
void A2C_BusCmd(A2C_TypeDef *A2Cx, u32 NewState)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if (NewState == ENABLE) {
		A2Cx->A2C_CTL |= A2C_BIT_BUS_ON_REQ;
	} else {
		A2Cx->A2C_CTL &= (~A2C_BIT_BUS_ON_REQ);
	}
}
/**
  * @brief    A2C ENABLE or DISABLE.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  NewState: where A2Cx can be ENABLE or DISABLE.
  * @retval None
  */
void A2C_Cmd(A2C_TypeDef *A2Cx, u32 NewState)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if (NewState == ENABLE) {
		A2Cx->A2C_CTL |= A2C_BIT_EN;
	} else {
		A2Cx->A2C_CTL &= (~A2C_BIT_EN);
	}
}

/**
  * @brief    get A2C bus status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval bus status
  --1: on
  --0: off
  */
u32 A2C_BusStatusGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return (A2Cx->A2C_STS & A2C_BIT_BUS_ON_STATE);
}

/**
  * @brief    get A2C fifo status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval fifo status, this value can be:
--A2C_FIFO_MSG_FULL
--A2C_FIFO_MSG_EMTY
--A2C_FIFO_MSG_OVERFLW
  */
u32 A2C_FifoStatusGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return (A2Cx->A2C_FIFO_STS & (A2C_BIT_FIFO_MSG_OVERFLOW | A2C_BIT_FIFO_MSG_EMPTY | A2C_BIT_FIFO_MSG_FULL));
}

/**
  * @brief    get A2C fifo level.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval rx fifo level
  */
u32 A2C_FifoLvlGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return A2C_GET_FIFO_MSG_LVL(A2Cx->A2C_FIFO_STS);
}

/**
  * @brief    Read Rx message from FIFO.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  RxMsg: rx message pointer.
  * @retval rx fifo level
  */
void A2C_ReadRxMsgFromFifo(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg)
{
	RxMsg->MsgBufferIdx = A2C_RX_FIFO_READ_MSG_IDX;

	A2C_ReadMsg(A2Cx, RxMsg);
}

/**
  * @brief    get A2C tx error counter.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval tx error counter
  */
u32 A2C_TXErrCntGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return A2C_GET_TEC(A2Cx->A2C_ERR_CNT_STS);
}

/**
  * @brief    get A2C rx error counter.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval rx error counter
  */
u32 A2C_RXErrCntGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return A2C_GET_REC(A2Cx->A2C_ERR_CNT_STS);
}

/**
  * @brief    A2C tx error counter clear.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval none
  */
void A2C_TXErrCntClear(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	A2Cx->A2C_ERR_CNT_CTL |= A2C_BIT_TX_ERR_CNT_CLR;
}

/**
  * @brief    A2C rx error counter clear.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval none
  */
void A2C_RXErrCntClear(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	A2Cx->A2C_ERR_CNT_CTL |= A2C_BIT_RX_ERR_CNT_CLR;
}

/**
  * @brief    A2C rx error counter clear.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval error counter status
--A2C_TX_ERR_PASSIVE
--A2C_TX_ERR_BUSOFF
--A2C_TX_ERR_WARNING
  */
u32 A2C_RXErrCntSTS(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return (A2Cx->A2C_ERR_CNT_STS & (A2C_BIT_ERROR_WARNING | A2C_BIT_ERROR_BUSOFF | A2C_BIT_ERROR_PASSIVE));
}

/**
  * @brief  write frame to message ram buffer.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void A2C_WriteMsg(A2C_TypeDef *A2Cx, A2C_TxMsgTypeDef *TxMsg)
{
	u32 a2c_ram_arb = 0;
	u32 a2c_ram_cmd = 0;
	u32 a2c_ram_cs = 0;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(IS_A2C_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_A2C_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/* Enable Msg buf[x] access*/
	a2c_ram_cmd |= (A2C_BIT_RAM_BUFFER_EN | A2C_BIT_RAM_ACC_ARB | A2C_BIT_RAM_ACC_CS | A2C_BIT_RAM_ACC_MASK | \
					A2C_BIT_RAM_ACC_DATA_MASK | A2C_BIT_RAM_DIR);
	a2c_ram_cmd |= A2C_RAM_ACC_NUM(TxMsg->MsgBufferIdx);
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;

	/*configure frame ARB register*/
	a2c_ram_arb = A2Cx->A2C_RAM_ARB;
	a2c_ram_arb &= (~(A2C_BIT_RAM_RTR | A2C_BIT_RAM_IDE | A2C_MASK_RAM_ID));
	a2c_ram_arb |= TxMsg->RTR;
	a2c_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == A2C_STANDARD_FRAME) {
		a2c_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		a2c_ram_arb |= TxMsg->ExtId;
	}
	A2Cx->A2C_RAM_ARB = a2c_ram_arb;

	/*configure CS register*/
	a2c_ram_cs = A2Cx->A2C_RAM_CS;
	a2c_ram_cs &= (~(A2C_MASK_RAM_DLC | A2C_BIT_RAM_AUTOREPLY | A2C_BIT_RAM_EDL | A2C_BIT_RAM_BRS | A2C_BIT_RAM_ESI));
	a2c_ram_cs |= TxMsg->DLC;
	a2c_ram_cs |= A2C_BIT_RAM_RXTX;
	A2Cx->A2C_RAM_CS = a2c_ram_cs;

	/*fill data, can2.0 8 bytes, can fd 64 bytes*/
	if (TxMsg->RTR == A2C_DATA_FRAME) {
		for (int i = 0; i < 16; i++) {
			A2Cx->A2C_RAM_FDDATA_x[16 - i - 1] = TxMsg->Data_32[i];
		}
	}

	/* Write frame info in register into the ram message buffer */
	a2c_ram_cmd |= A2C_BIT_RAM_START;
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;
	while (A2Cx->A2C_RAM_CMD & A2C_BIT_RAM_START);
}

/**
  * @brief    set rx message ram buffer.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  RxMsg: rx message pointer.
  * @retval none
  */
void A2C_SetRxMsgBuf(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg)
{
	u32 a2c_ram_arb = 0;
	u32 a2c_ram_cmd = 0;
	u32 a2c_ram_mask = 0;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(IS_A2C_FRAME_TYPE(RxMsg->RTR));
	assert_param(IS_A2C_ID_TYPE(RxMsg->IDE));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	a2c_ram_cmd |= (A2C_BIT_RAM_BUFFER_EN | A2C_BIT_RAM_ACC_ARB | A2C_BIT_RAM_ACC_CS | A2C_BIT_RAM_ACC_MASK | \
					A2C_BIT_RAM_ACC_DATA_MASK | A2C_BIT_RAM_DIR);
	a2c_ram_cmd |= A2C_RAM_ACC_NUM(RxMsg->MsgBufferIdx);

	/*configure frame ARB register*/
	a2c_ram_arb |= RxMsg->RTR;
	a2c_ram_arb |= RxMsg->IDE;

	if (RxMsg->IDE == A2C_STANDARD_FRAME) {
		a2c_ram_arb |= A2C_RAM_ID((RxMsg->StdId & 0x7ff) << 18);
	} else {
		a2c_ram_arb |= A2C_RAM_ID(RxMsg->ExtId);
	}
	A2Cx->A2C_RAM_ARB = a2c_ram_arb;


	/*configure MASK ID register*/
	if (RxMsg->IDE == A2C_STANDARD_FRAME) {
		a2c_ram_mask = A2C_RAM_ID_MASK(RxMsg->ID_MASK << 18);
	} else {
		a2c_ram_mask = A2C_RAM_ID_MASK(RxMsg->ID_MASK);
	}

	if (RxMsg->IDE_Mask) {
		a2c_ram_mask |= A2C_BIT_RAM_IDE_MASK;
	}
	if (RxMsg->RTR_Mask) {
		a2c_ram_mask |= A2C_BIT_RAM_RTR_MASK;
	}

	A2Cx->A2C_RAM_MASK = a2c_ram_mask;

	/* Write RX setting info in register into the ram message buffer */
	a2c_ram_cmd |= A2C_BIT_RAM_START;
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;
	while (A2Cx->A2C_RAM_CMD & A2C_BIT_RAM_START);
}

/**
  * @brief    read frame from message ram buffer.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  RxMsg: rx message pointer.
  * @retval none
  */
void A2C_ReadMsg(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg)
{
	u32 a2c_ram_arb = 0;
	u32 a2c_ram_cmd = 0;
	u32 a2c_ram_cs = 0;
	u32 a2c_ram_mask = 0;
	u32 i;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/* Enable Msg buf[x] access */
	a2c_ram_cmd |= (A2C_BIT_RAM_BUFFER_EN | A2C_BIT_RAM_ACC_ARB | A2C_BIT_RAM_ACC_CS | A2C_BIT_RAM_ACC_MASK | A2C_BIT_RAM_ACC_DATA_MASK);
	a2c_ram_cmd |= A2C_RAM_ACC_NUM(RxMsg->MsgBufferIdx);
	a2c_ram_cmd |= A2C_BIT_RAM_START;
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;
	/* Read frame into register from ram message buffer */
	while (A2Cx->A2C_RAM_CMD & A2C_BIT_RAM_START);

	/*read ARB register*/
	a2c_ram_arb = A2Cx->A2C_RAM_ARB;
	if (a2c_ram_arb & A2C_BIT_RAM_RTR) {
		RxMsg->RTR = A2C_REMOTE_FRAME;
	} else {
		RxMsg->RTR = A2C_DATA_FRAME;
	}
	if (a2c_ram_arb & A2C_BIT_RAM_IDE) {
		RxMsg->IDE = A2C_EXTEND_FRAME;
	} else {
		RxMsg->IDE = A2C_STANDARD_FRAME;
	}

	if (RxMsg->IDE == A2C_STANDARD_FRAME) {
		RxMsg->StdId = A2C_GET_RAM_ID(a2c_ram_arb) >> 18;
	} else {
		RxMsg->ExtId = A2C_GET_RAM_ID(a2c_ram_arb);
	}

	/*read CS register*/
	a2c_ram_cs = A2Cx->A2C_RAM_CS;
	RxMsg->RxTimStamp = A2C_GET_RAM_TIMESTAMP(a2c_ram_cs);
	if (a2c_ram_cs & A2C_BIT_RAM_LOST) {
		RxMsg->RxLost = 1;
	} else {
		RxMsg->RxLost = 0;
	}
	RxMsg->DLC = A2C_GET_RAM_DLC(a2c_ram_cs);

	/*read mask ID*/
	a2c_ram_mask = A2Cx->A2C_RAM_MASK;
	RxMsg->ID_MASK = A2C_GET_RAM_ID_MASK(a2c_ram_mask);
	RxMsg->IDE_Mask = (a2c_ram_mask & A2C_BIT_RAM_IDE_MASK);
	RxMsg->RTR_Mask = (a2c_ram_mask & A2C_BIT_RAM_RTR_MASK);

	/*configure DATA register: can2.0 8 bytes, can fd 64 bytes*/
	if (RxMsg->RTR == A2C_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			RxMsg->Data_32[i] = A2Cx->A2C_RAM_FDDATA_x[16 - i - 1];
		}
	}
}

/**
  * @brief    send auto reply message.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void A2C_TxAutoReply(A2C_TypeDef *A2Cx, A2C_TxMsgTypeDef *TxMsg)
{
	u32 a2c_ram_arb;
	u32 a2c_ram_cmd = 0;
	u32 a2c_ram_cs;
	u32 i;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(IS_A2C_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_A2C_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/* Enable Msg buf[x] access */
	a2c_ram_cmd |= (A2C_BIT_RAM_BUFFER_EN | A2C_BIT_RAM_ACC_ARB | A2C_BIT_RAM_ACC_CS | A2C_BIT_RAM_ACC_MASK | \
					A2C_BIT_RAM_ACC_DATA_MASK | A2C_BIT_RAM_DIR);
	a2c_ram_cmd |= A2C_RAM_ACC_NUM(TxMsg->MsgBufferIdx);
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;

	/*configure frame ARB register*/
	a2c_ram_arb = A2Cx->A2C_RAM_ARB;
	a2c_ram_arb &= (~(A2C_BIT_RAM_RTR | A2C_BIT_RAM_IDE | A2C_MASK_RAM_ID));
	a2c_ram_arb |= TxMsg->RTR;
	a2c_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == A2C_STANDARD_FRAME) {
		a2c_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		a2c_ram_arb |= TxMsg->ExtId;
	}
	A2Cx->A2C_RAM_ARB = a2c_ram_arb;

	/*configure CS register*/
	a2c_ram_cs = A2Cx->A2C_RAM_CS;
	a2c_ram_cs &= (~(A2C_MASK_RAM_DLC | A2C_BIT_RAM_AUTOREPLY | A2C_BIT_RAM_ESI | A2C_BIT_RAM_BRS | A2C_BIT_RAM_EDL));

	a2c_ram_cs |= TxMsg->DLC;
	/*enable auto-reply*/
	a2c_ram_cs |= A2C_BIT_RAM_RXTX | A2C_BIT_RAM_AUTOREPLY;

	A2Cx->A2C_RAM_CS = a2c_ram_cs;

	/*configure DATA register: can2.0 8 bytes, can fd 64 bytes*/
	if (TxMsg->RTR == A2C_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			A2Cx->A2C_RAM_FDDATA_x[i] = TxMsg->Data_32[(16 - i - 1)];
		}
	}

	/* Write frame info in register into the ram message buffer*/
	a2c_ram_cmd |= A2C_BIT_RAM_START;
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;
	while (A2Cx->A2C_RAM_CMD & A2C_BIT_RAM_START);
}

/**
  * @brief    receive auto reply message.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void A2C_RxAutoReply(A2C_TypeDef *A2Cx, A2C_RxMsgTypeDef *RxMsg)
{
	u32 a2c_ram_arb;
	u32 a2c_ram_cmd = 0;
	u32 a2c_ram_cs;
	u32 a2c_ram_mask;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(IS_A2C_FRAME_TYPE(RxMsg->RTR));
	assert_param(IS_A2C_ID_TYPE(RxMsg->IDE));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	a2c_ram_cmd |= (A2C_BIT_RAM_BUFFER_EN | A2C_BIT_RAM_ACC_ARB | A2C_BIT_RAM_ACC_CS | A2C_BIT_RAM_ACC_MASK | \
					A2C_BIT_RAM_ACC_DATA_MASK | A2C_BIT_RAM_DIR);
	a2c_ram_cmd |= A2C_RAM_ACC_NUM(RxMsg->MsgBufferIdx);

	/*configure frame ARB register*/
	a2c_ram_arb = A2Cx->A2C_RAM_ARB;
	a2c_ram_arb &= (~(A2C_BIT_RAM_RTR | A2C_BIT_RAM_IDE | A2C_MASK_RAM_ID));
	a2c_ram_arb |= RxMsg->RTR;
	a2c_ram_arb |= RxMsg->IDE;

	if (RxMsg->IDE == A2C_STANDARD_FRAME) {
		a2c_ram_arb |= ((RxMsg->StdId & 0x7ff) << 18);
	} else {
		a2c_ram_arb |= RxMsg->ExtId;
	}
	A2Cx->A2C_RAM_ARB = a2c_ram_arb;

	/*configure CS register*/
	a2c_ram_cs = A2Cx->A2C_RAM_CS;
	a2c_ram_cs &= (~(A2C_BIT_RAM_RXTX | A2C_BIT_RAM_AUTOREPLY | A2C_BIT_RAM_ESI | A2C_BIT_RAM_BRS | A2C_BIT_RAM_BRS));
	/*enable auto-reply*/
	a2c_ram_cs |= A2C_BIT_RAM_AUTOREPLY;

	A2Cx->A2C_RAM_CS = a2c_ram_cs;

	/*configure MASK ID register*/
	a2c_ram_mask = A2Cx->A2C_RAM_MASK;
	a2c_ram_mask &= (~A2C_MASK_RAM_ID_MASK | A2C_BIT_RAM_IDE_MASK | A2C_BIT_RAM_RTR_MASK);
	if (RxMsg->IDE == A2C_STANDARD_FRAME) {
		a2c_ram_mask = A2C_RAM_ID_MASK(RxMsg->ID_MASK << 18);
	} else {
		a2c_ram_mask = A2C_RAM_ID_MASK(RxMsg->ID_MASK);
	}

	if (RxMsg->IDE_Mask) {
		a2c_ram_mask |= A2C_BIT_RAM_IDE_MASK;
	}
	if (RxMsg->RTR_Mask) {
		a2c_ram_mask |= A2C_BIT_RAM_RTR_MASK;
	}
	A2Cx->A2C_RAM_MASK = a2c_ram_mask;

	/* Write RX setting into the ram message buffer*/
	a2c_ram_cmd |= A2C_BIT_RAM_START;
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;
	while (A2Cx->A2C_RAM_CMD & A2C_BIT_RAM_START);
}

/**
  * @brief  Enables or disables the specified A2C interrupts.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  A2C_IT: specifies the A2C interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg A2C_TX_INT
  *     @arg A2C_RX_INT
  *     @arg A2C_ERR_INT
  *     @arg A2C_WKUP_INT
  *     @arg A2C_BUSOFF_INT
  * @param  NewState: new state of the specified A2C interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void A2C_INTConfig(A2C_TypeDef *A2Cx, u32 A2C_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if (NewState != DISABLE) {
		/* Enable the selected A2C interrupts */
		A2Cx->A2C_INT_EN |= A2C_IT;
	} else {
		/* Disable the selected A2C interrupts */
		A2Cx->A2C_INT_EN &= (~A2C_IT);
	}
}

/**
  * @brief  Get A2C interrupt status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval interrupt status
  */
u32 A2C_GetINTStatus(A2C_TypeDef *A2Cx)
{
	/*check the parameters*/
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return A2Cx->A2C_INT_FLAG;
}

/**
  * @brief  Clears the A2C interrupt pending bits.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  A2C_IT: specifies the interrupt to be cleared.
  * @retval None
  */
void A2C_ClearINT(A2C_TypeDef *A2Cx, u32 A2C_IT)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(IS_A2C_CLEAR_IT(A2C_IT));

	/*clear the specified interrupt*/
	A2Cx->A2C_INT_FLAG = A2C_IT;
}

/**
  * @brief  Clears the A2C interrupt pending bits.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  A2C_IT: specifies the interrupt to be cleared.
  * @retval None
  */
void A2C_ClearAllINT(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	A2Cx->A2C_INT_FLAG = 0xFFFFFFFF;
}

/**
  * @brief  Get the A2C error status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval error status
  */
u32 A2C_GetErrStatus(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return (A2Cx->A2C_ERR_STATUS);
}

/**
  * @brief  Clears the A2C error pending bits.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  ERR_STS: specifies the error status to be cleared.
  * @retval None
  */
void A2C_ClearErrStatus(A2C_TypeDef *A2Cx, u32 ERR_STS)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	/*clear the specified error status*/
	A2Cx->A2C_ERR_STATUS = ERR_STS;
}

/**
  * @brief  Enables or disables the specified A2C Rx message buffer interrupts.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  A2C_IT: specifies the A2C interrupts sources to be enabled or disabled.
   * @param  NewState: new state of the specified A2C interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void A2C_RxMsgBufINTConfig(A2C_TypeDef *A2Cx, u32 BUF_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if (NewState != DISABLE) {
		/* Enable the selected A2C rx msg buffer interrupts */
		A2Cx->A2C_MB_RXINT_EN |= BUF_IT;
	} else {
		/* Disable the selected A2C rx msg buffer interrupts */
		A2Cx->A2C_MB_RXINT_EN &= (~BUF_IT);
	}
}

/**
  * @brief  Enables or disables the specified A2C Tx message buffer interrupts.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  A2C_IT: specifies the A2C interrupts sources to be enabled or disabled.
   * @param  NewState: new state of the specified A2C interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void A2C_TxMsgBufINTConfig(A2C_TypeDef *A2Cx, u32 BUF_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if (NewState != DISABLE) {
		/* Enable the selected A2C tx msg buffer interrupts */
		A2Cx->A2C_MB_TXINT_EN |= BUF_IT;
	} else {
		/* Disable the selected A2C tx msg buffer interrupts */
		A2Cx->A2C_MB_TXINT_EN &= (~BUF_IT);
	}
}

/**
  * @brief  get tx message buffer error status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval tx message error status
  */
u32 A2C_TxMsgBufErrGet(A2C_TypeDef *A2Cx)
{
	/*check the parameters*/
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return (A2Cx->A2C_TX_ERROR_FLAG);
}

/**
  * @brief  clear tx message buffer error status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  ERR: the specified interrupt source.
  * @retval None
  */
void A2C_TxMsgBufErrClear(A2C_TypeDef *A2Cx, u32 ERR_FLAG)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	/*clear the specified msg buffer error*/
	A2Cx->A2C_TX_ERROR_FLAG = ERR_FLAG;
}

/**
  * @brief  get message buffer status register value.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval register value
  */
u32 A2C_MsgBufStatusRegGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(MsgBufIdx <= 15);

	return (A2Cx->A2C_MBx_STS[MsgBufIdx]);
}

/**
  * @brief  get message buffer status register value.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--A2C_FRAME_PENDING_TX
	--A2C_FRAME_FINISAH_TX
	--A2C_FRAME_OVWRITR_SEND_TX
	--A2C_FRAME_PENDING_RX
	--A2C_FRAME_FINISH_RX
  */
u32 A2C_MsgBufStatusGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(MsgBufIdx <= 15);

	return (A2Cx->A2C_MBx_STS[MsgBufIdx] & 0xf);
}

/**
  * @brief  check message buffer available or not.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--1: available
	--0: not available
  */
u32 A2C_CheckMsgBufAvailable(A2C_TypeDef *A2Cx, u32 MsgBufIdx)
{
	u32 Temp;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	Temp = (A2Cx->A2C_MBx_STS[MsgBufIdx] & 0xf);

	if ((Temp != A2C_FRAME_PENDING_TX) && (Temp != A2C_FRAME_PENDING_RX)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  get A2C tx done status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
u32 A2C_TxDoneStatusGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return (A2Cx->A2C_TX_DONE);
}

/**
  * @brief  get message buffer tx done status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--1: tx complete
	--0: tx pending
  */
u32 A2C_MsgBufTxDoneStatusGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if ((A2Cx->A2C_TX_DONE) & BIT(MsgBufIdx)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  get A2C rx done status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
u32 A2C_RxDoneStatusGet(A2C_TypeDef *A2Cx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	return (A2Cx->A2C_RX_DONE);
}
/**
  * @brief  wait message buffer rx done.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--1: rx complete
	--0: rx pending
  */
u32 A2C_MsgBufRxDoneStatusGet(A2C_TypeDef *A2Cx, u32 MsgBufIdx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	if ((A2Cx->A2C_RX_DONE) & BIT(MsgBufIdx)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  clear A2C tx done status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  Status: clear status.
  * @retval None
  */
void A2C_TxDoneStatusClear(A2C_TypeDef *A2Cx, u32 Status)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	/*clear the specified msg buffer tx done status*/
	A2Cx->A2C_TX_DONE = Status;
}

/**
  * @brief  clear A2C tx done message buffer status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
void A2C_MsgBufTxDoneStatusClear(A2C_TypeDef *A2Cx, u32 MsgBufIdx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	/*clear the specified msg buffer tx done status*/
	A2Cx->A2C_TX_DONE = BIT(MsgBufIdx);
}

/**
  * @brief  clear A2C rx done status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  Status: clear status.
  * @retval None
  */
void A2C_RxDoneStatusClear(A2C_TypeDef *A2Cx, u32 Status)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	/*clear the specified msg buffer tx done status*/
	A2Cx->A2C_RX_DONE = Status;
}

/**
  * @brief  clear A2C rx done message buffer status.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
void A2C_MsgBufRxDoneStatusClear(A2C_TypeDef *A2Cx, u32 MsgBufIdx)
{
	assert_param(IS_A2C_ALL_PERIPH(A2Cx));

	/*clear the specified msg buffer tx done status*/
	A2Cx->A2C_RX_DONE = BIT(MsgBufIdx);
}


/**
  * @brief    A2C TX message trigger ENABLE or DISABLE.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  State: where A2Cx can be ENABLE or DISABLE.
  * @retval None
  */
void A2C_TxMsgTriggerCmd(A2C_TypeDef *A2Cx, u32 State)
{
	if (State == DISABLE) {
		A2Cx->A2C_MB_TRIGGER &= ~A2C_BIT_TX_TRIGGER_EN;
	} else {
		A2Cx->A2C_MB_TRIGGER |= A2C_BIT_TX_TRIGGER_EN;
	}
}

/**
  * @brief    A2C TX message trigger configure.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  CloseOffset: close offset.
  * @param  Begin: trigger begin.
  * @retval None
  */
void A2C_TxMsgTriggerConfig(A2C_TypeDef *A2Cx, u32 CloseOffset, u32 Begin)
{
	u32 MbTrigger;

	MbTrigger = A2Cx->A2C_MB_TRIGGER;

	MbTrigger &= (~(A2C_MASK_TX_TRIGGER_CLOSE_OFFSET | A2C_MASK_TX_TRIGGER_BEGIN));

	MbTrigger |= (Begin | A2C_TX_TRIGGER_CLOSE_OFFSET(CloseOffset));

	A2Cx->A2C_MB_TRIGGER = MbTrigger;
}

/**
  * @brief    A2C control register configure.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgIdx: message buffer index.
  * @param  RxDmaEn: RX DMA enable.
  * @param  RxDmaOffset: RX DMA offset.
  * @retval none
  */
void A2C_MsgCtrlRegConfig(A2C_TypeDef *A2Cx, u32 MsgIdx, u32 RxDmaEn, u32 DmaOffset)
{
	u32 MsgBufferCtrl;

	MsgBufferCtrl = A2Cx->A2C_MBx_CTRL[MsgIdx];

	MsgBufferCtrl &= (~(A2C_BIT_MSGx_RXDMA_EN | A2C_MASK_MSGx_BA));

	if (RxDmaEn == DISABLE) {
		MsgBufferCtrl &= (~A2C_BIT_MSGx_RXDMA_EN);
	} else {
		MsgBufferCtrl |= A2C_BIT_MSGx_RXDMA_EN;
	}

	MsgBufferCtrl |= (DmaOffset << 10);
	A2Cx->A2C_MBx_CTRL[MsgIdx] = MsgBufferCtrl;
}

/**
  * @brief    A2C control register configure.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  MsgIdx: message buffer index.
  * @param  BaseAddr: message buffer base address.
  * @retval none
  */
void A2C_MsgBaseAddrConfig(A2C_TypeDef *A2Cx, u32 MsgIdx, u32 BaseAddr)
{
	u32 MsgBufferCtrl;

	MsgBufferCtrl = A2Cx->A2C_MBx_CTRL[MsgIdx];

	MsgBufferCtrl &= ~(A2C_MASK_MSGx_BA);

	MsgBufferCtrl |= A2C_MSGx_BA(BaseAddr);

	A2Cx->A2C_MBx_CTRL[MsgIdx] = MsgBufferCtrl;
}

/**
  * @brief    A2C message buffer end address.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  EndAddr: message buffer end address.
  * @retval none
  */
void A2C_MsgBaseAddrEndReg(A2C_TypeDef *A2Cx, u32 EndAddr)
{
	A2Cx->A2C_MB_BA_END &= (~(A2C_MASK_MSG_BA_END));

	A2Cx->A2C_MB_BA_END |= A2C_MSG_BA_END(EndAddr);
}

/**
  * @brief    Init and Enable A2C RX GDMA.
  * @param  UartIndex: 0 or 1.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @retval   TRUE/FLASE
  */
bool A2C_RXGDMA_Init(
	u8 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	int RxCount
)
{
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32) pRxBuf, RxCount);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		return FALSE;
	}
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_SrcAddr = A2C_DEV_TABLE[Index].A2Cx->A2C_RX_DMA_DATA;
	GDMA_InitStruct->GDMA_DstAddr = (u32)pRxBuf;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = A2C_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	if (((u32)(pRxBuf) & 0x03) == 0) {
		/*	4-bytes aligned, move 4 bytes each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize	 = MsizeOne;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		RTK_LOGW(TAG, "Dst is 4-bytes aligned...\n");
	} else {
		/*	move 1 byte each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize	 = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes; //TrWidthOneByte
	}

	GDMA_InitStruct->GDMA_BlockSize = RxCount >> (GDMA_InitStruct->GDMA_SrcDataWidth);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief    write frame to dma message ram buffer.
  * @param  A2CxBufer: where A2CxBufer can be dma ram buffer.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void A2C_FillTXDmaBuffer(A2C_TypeDef *A2Cx, A2C_TxMsgTypeDef *TxMsg)
{
	u32 a2c_ram_arb;
	u32 a2c_ram_cmd = 0;
	u32 a2c_ram_cs;
	u32 i;

	assert_param(IS_A2C_ALL_PERIPH(A2Cx));
	assert_param(IS_A2C_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_A2C_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	a2c_ram_cmd |= (A2C_BIT_RAM_BUFFER_EN | A2C_BIT_RAM_ACC_ARB | A2C_BIT_RAM_ACC_CS | A2C_BIT_RAM_ACC_MASK | \
					A2C_BIT_RAM_ACC_DATA_MASK  |  A2C_BIT_RAM_DIR);
	a2c_ram_cmd |= A2C_RAM_ACC_NUM(TxMsg->MsgBufferIdx);
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;

	/*configure frame ARB register*/
	a2c_ram_arb = A2Cx->A2C_RAM_ARB;
	a2c_ram_arb &= (~(A2C_BIT_RAM_RTR | A2C_BIT_RAM_IDE | A2C_MASK_RAM_ID));
	a2c_ram_arb |= TxMsg->RTR;
	a2c_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == A2C_STANDARD_FRAME) {
		a2c_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		a2c_ram_arb |= TxMsg->ExtId;
	}
	A2Cx->A2C_RAM_ARB = a2c_ram_arb;

	/*configure CS register*/
	a2c_ram_cs = A2Cx->A2C_RAM_CS;
	a2c_ram_cs &= (~(A2C_MASK_RAM_DLC | A2C_BIT_RAM_AUTOREPLY | A2C_BIT_RAM_EDL | A2C_BIT_RAM_BRS | A2C_BIT_RAM_ESI));
	if (TxMsg->RTR == A2C_DATA_FRAME) {
		a2c_ram_cs |= TxMsg->DLC;
	}
	a2c_ram_cs |= A2C_BIT_RAM_RXTX;

	A2Cx->A2C_RAM_CS = a2c_ram_cs;

	/*fill data, can2.0 8 bytes, can fd 64 bytes*/
	if (TxMsg->RTR == A2C_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			A2Cx->A2C_RAM_FDDATA_x[16 - i - 1] = TxMsg->Data_32[i];
		}
	}

	/* Write frame info into the ram message buffer*/
	a2c_ram_cmd |= A2C_BIT_RAM_START;
	A2Cx->A2C_RAM_CMD = a2c_ram_cmd;
	while (A2Cx->A2C_RAM_CMD & A2C_BIT_RAM_START);
}

/**
  * @brief    A2C DMA destination base address configure.
  * @param  A2Cx: where A2Cx can be A2C.
  * @param  Addr: base address.
  * @retval None
  */
void A2C_RxDmaDestBaseAddrConfig(A2C_TypeDef *A2Cx, u32 Addr)
{
	A2Cx->A2C_RXDMA_CFG = Addr;
}

/**
  * @brief    A2C get RX DMA data.
  * @param  A2Cx: where A2Cx can be A2C.
  * @retval DMA data
  */
u32 A2C_GetRxDmaData(A2C_TypeDef *A2Cx)
{
	return A2Cx->A2C_RX_DMA_DATA;
}

/**
  * @brief  Configure the RAM mapping addresses for message buffers.
  *
  * @note   This function assigns physical memory addresses to the A2C message
  *         buffers. The array must contain one extra address for the end register.
  *
  * @param  A2Cx: Pointer to A2C peripheral instance (e.g., A2C0).
  * @param  AddrList: Pointer to an array containing buffer addresses.
  *                   Size must be at least (A2C_MESSAGE_BUFFER_SIZE + 1).
  * @retval None
  */
void A2C_RamBufferMapConfig(A2C_TypeDef *A2Cx, uint32_t *AddrList)
{
	uint32_t i;

	/* Configure base address for each individual message buffer */
	for (i = 0; i < A2C_MESSAGE_BUFFER_SIZE; i++) {
		A2C_MsgBaseAddrConfig(A2Cx, i, AddrList[i]);
	}

	/* Configure the end address register (using the element after the last buffer) */
	A2C_MsgBaseAddrEndReg(A2Cx, AddrList[A2C_MESSAGE_BUFFER_SIZE]);
}

/**
  * @brief  Initialize the A2C core clock settings.
  * @note   Sets the source to XTAL and disables PLL dividers for direct clocking.
  * @retval None
  */
void A2C_CoreClockSet(void)
{
	RCC_PeriphClockSourceSet(A2C, XTAL);
	RCC_PeriphClockDividerFENSet(USB_PLL_A2C, DISABLE);
	RCC_PeriphClockDividerFENSet(SYS_PLL_A2C, DISABLE);
}

/**
  * @brief  Get the current A2C core clock frequency.
  *
  * @param  Rate: [Out] Pointer to a variable where the frequency (in Hz) will be stored.
  * @retval RTK_SUCCESS: Clock rate retrieved successfully.
  * @retval RTK_FAIL: Invalid parameter (NULL pointer).
  */
int A2C_CoreClockGet(uint32_t *Rate)
{
	if (Rate == NULL) {
		return RTK_FAIL;
	}
	/* Currently fixed to 40MHz XTAL. Logic can be expanded to read registers. */
	*Rate = XTAL_40M;
	return RTK_SUCCESS;
}

/**
 * @brief  [Internal] Calculate time segment distribution based on Total TQ.
 *
 * @details
 *  Bit Timing Model:
 *  +-----------+----------+-----------+-----------+
 *  | Sync_Seg  | Prop_Seg | Phase_Seg1| Phase_Seg2|
 *  +-----------+----------+-----------+-----------+
 *  | 1 TQ      |       Tseg1          |   Tseg2   |
 *  +-----------+----------------------+-----------+
 *                                     ^ Sample Point
 *
 * @param  TotalTq     Total number of time quanta per bit.
 * @param  SamplePoint Target sample point in permille (e.g., 875 for 87.5%).
 * @param  Result      [Out] Struct to store the calculated segments.
 * @param  Min         Pointer to hardware minimum limits.
 * @param  Max         Pointer to hardware maximum limits.
 *
 * @return Positive value: Absolute error between calculated and target sample point.
 * @return Negative value: Error code (e.g., -RTK_ERR_BADARG) if limits cannot be met.
 */
static int A2C_CalcTimeSegments(uint32_t TotalTq, uint32_t SamplePoint, A2C_BitTimingTypeDef *Result,
								const A2C_BitTimingTypeDef *Min, const A2C_BitTimingTypeDef *Max)
{
	/* Calculate allowed range for Tseg1 (PropSeg + PhaseSeg1) */
	uint16_t tseg1_max = Max->PhaseSeg1 + Max->PropSeg;
	uint16_t tseg1_min = Min->PhaseSeg1 + Min->PropSeg;
	uint32_t sample_pnt_res;
	uint16_t tseg1;
	uint16_t tseg2;

	/* ----------------------------------------------------------- */
	/* 1. Calculate Tseg2 (PhaseSeg2)                              */
	/* Formula: tseg2 = TotalTq * (1 - SamplePoint)                */
	/* ----------------------------------------------------------- */
	tseg2 = TotalTq - (TotalTq * SamplePoint) / 1000;

	/* Clamp Tseg2 to hardware limits */
	tseg2 = A2C_CLAMP(tseg2, Min->PhaseSeg2, Max->PhaseSeg2);

	/* ----------------------------------------------------------- */
	/* 2. Calculate Tseg1 (PropSeg + PhaseSeg1)                    */
	/* Formula: TotalTq = SyncSeg(1) + Tseg1 + Tseg2               */
	/* ----------------------------------------------------------- */
	tseg1 = TotalTq - A2C_SYNC_SEG - tseg2;

	/* ----------------------------------------------------------- */
	/* 3. Validate and Adjust Tseg1                                */
	/* If Tseg1 is out of bounds, trade off Tseg2 (Sample Point)   */
	/* to satisfy Tseg1 constraints.                               */
	/* ----------------------------------------------------------- */
	if (tseg1 > tseg1_max) {
		/* Tseg1 is too large; clamp to max and increase Tseg2 */
		tseg1 = tseg1_max;
		tseg2 = TotalTq - A2C_SYNC_SEG - tseg1;

		/* Check if Tseg2 now exceeds its own max limit */
		if (tseg2 > Max->PhaseSeg2) {
			return -RTK_ERR_BADARG;
		}

	} else if (tseg1 < tseg1_min) {
		/* Tseg1 is too small; clamp to min and decrease Tseg2 */
		tseg1 = tseg1_min;
		tseg2 = TotalTq - A2C_SYNC_SEG - tseg1;

		/* Check if Tseg2 now falls below its own min limit */
		if (tseg2 < Min->PhaseSeg2) {
			return -RTK_ERR_BADARG;
		}
	}

	/* Save calculated PhaseSeg2 */
	Result->PhaseSeg2 = tseg2;

	/* ----------------------------------------------------------- */
	/* 4. Distribute Tseg1 into PropSeg and PhaseSeg1              */
	/* Strategy: Split evenly for balanced propagation delay handling */
	/* ----------------------------------------------------------- */
	Result->PropSeg = A2C_CLAMP(tseg1 / 2, Min->PropSeg, Max->PropSeg);
	Result->PhaseSeg1 = tseg1 - Result->PropSeg;

	/* Re-balance if PhaseSeg1 is out of hardware limits */
	if (Result->PhaseSeg1 > Max->PhaseSeg1) {
		Result->PhaseSeg1 = Max->PhaseSeg1;
		Result->PropSeg = tseg1 - Result->PhaseSeg1;
	} else if (Result->PhaseSeg1 < Min->PhaseSeg1) {
		Result->PhaseSeg1 = Min->PhaseSeg1;
		Result->PropSeg = tseg1 - Result->PhaseSeg1;
	}

	/* ----------------------------------------------------------- */
	/* 5. Calculate Actual Sample Point                            */
	/* ----------------------------------------------------------- */
	sample_pnt_res = (A2C_SYNC_SEG + tseg1) * 1000 / TotalTq;

	/* Return the absolute error */
	return (sample_pnt_res > SamplePoint) ?
		   (sample_pnt_res - SamplePoint) : (SamplePoint - sample_pnt_res);
}

/**
 * @brief  [Internal] Get default sample point recommendation for a bitrate.
 *         Based on CiA (CAN in Automation) recommendations.
 *
 * @param  BitRate: Target bitrate in bits per second.
 * @return Sample point in permille (e.g., 875 = 87.5%).
 */
static uint16_t A2C_GetDefaultSamplePoint(uint32_t BitRate)
{
	uint16_t sample_pnt;

	if (BitRate > 800000) {
		/* > 800Kbps, 75.0% */
		sample_pnt = 750;
	} else if (BitRate > 500000) {
		/* > 500Kbps, 80.0% */
		sample_pnt = 800;
	} else {
		/* others, 87.5% */
		sample_pnt = 875;
	}

	return sample_pnt;
}

/**
 * @brief  Calculate optimal A2C Bit Timing parameters.
 *
 * @details
 *  This function iterates through possible prescaler values to find a configuration
 *  that minimizes the deviation from the desired sample point.
 *  The results (Prescaler, Segments, SJW) are written to the Result structure.
 *
 * @param  BitRate: Target bitrate (e.g., 500000 for 500kbps).
 * @param  Result:  [Out] Pointer to structure where timing params will be saved.
 *
 * @retval RTK_SUCCESS:      Valid timing configuration found.
 * @retval -RTK_ERR_BADARG:  Calculated parameters are invalid or out of range.
 * @retval RTK_FAIL:         System clock could not be retrieved.
 */
int A2C_CalcBitTiming(uint32_t BitRate, A2C_BitTimingTypeDef *Result)
{
	/* Hardware limits defined in header */
	const A2C_BitTimingTypeDef MinLimits = A2C_TIMING_MIN;
	const A2C_BitTimingTypeDef MaxLimits = A2C_TIMING_MAX;

	/* Max possible Total TQ to determine minimum prescaler */
	uint32_t TotalTq = A2C_SYNC_SEG + MaxLimits.PropSeg + MaxLimits.PhaseSeg1 + MaxLimits.PhaseSeg2;
	A2C_BitTimingTypeDef TempRes = {0};

	/* Initialize SamplePoint to 0 to trigger default selection */
	uint16_t SamplePoint = 0;

	int ErrMin = INT_MAX;
	uint32_t CoreClock;
	int Err;
	int Prescaler;

	/* Basic validation */
	if (BitRate == 0 || Result == NULL) {
		return -RTK_ERR_BADARG;
	}

	if (A2C_CoreClockGet(&CoreClock) != RTK_SUCCESS) {
		return RTK_FAIL;
	}

	/* Select default sample point if not provided */
	if (SamplePoint == 0U) {
		SamplePoint = A2C_GetDefaultSamplePoint(BitRate);
	}

	/* ----------------------------------------------------------- */
	/* Iterate over Prescaler values to find best fit              */
	/* ----------------------------------------------------------- */

	/* Optimization: Start from the smallest valid prescaler to reduce loop count */
	/* Prescaler = Clock / (TotalTq * BitRate) */
	int StartPrescaler = MAX((int)(CoreClock / (TotalTq * BitRate)), (int)MinLimits.Prescaler);

	for (Prescaler = StartPrescaler; Prescaler <= MaxLimits.Prescaler; Prescaler++) {

		/* Check for integer division (TotalTq must be an integer) */
		if (CoreClock % (Prescaler * BitRate) != 0) {
			continue;
		}

		/* Calculate Total Time Quanta for this prescaler */
		TotalTq = CoreClock / (Prescaler * BitRate);

		/* Attempt to calculate segments for this TQ configuration */
		Err = A2C_CalcTimeSegments(TotalTq, SamplePoint, &TempRes, &MinLimits, &MaxLimits);

		/* If calculation failed (e.g., TQ out of bounds), skip this prescaler */
		if (Err < 0) {
			continue;
		}

		/* Check if this configuration is better (lower error) */
		if (Err < ErrMin) {
			ErrMin = Err;

			/* Store best results */
			Result->PropSeg   = TempRes.PropSeg;
			Result->PhaseSeg1 = TempRes.PhaseSeg1;
			Result->PhaseSeg2 = TempRes.PhaseSeg2;
			Result->Prescaler = (uint16_t)Prescaler;

			/* If error is 0, we found a perfect match. Stop searching. */
			if (Err == 0) {
				break;
			}
		}
	}

	if (ErrMin != 0) {
		RTK_LOGW(TAG, "A2C BitRate warning: Sample Point Error %d/1000", ErrMin);
	}

	/* ----------------------------------------------------------- */
	/* Post-processing: Calculate SJW (Sync Jump Width)            */
	/* Standard Rule: SJW = min(PhaseSeg1, PhaseSeg2 / 2, 4)       */
	/* ----------------------------------------------------------- */
	Result->SJW = MIN(Result->PhaseSeg1, Result->PhaseSeg2 / 2);
	Result->SJW = A2C_CLAMP(Result->SJW, MinLimits.SJW, MaxLimits.SJW);

	/* Check if a valid solution was found */
	if (ErrMin == INT_MAX) {
		return -RTK_ERR_BADARG;
	}
	/* Optional: Log info if the result is within acceptable margin */
	else if (ErrMin <= A2C_SAMPLE_POINT_MARGIN) {
		RTK_LOGI(TAG, "A2C Config: Pre=%u, SJW=%u, Prop=%u, Ph1=%u, Ph2=%u (Err=%d)\n",
				 Result->Prescaler, Result->SJW, Result->PropSeg,
				 Result->PhaseSeg1, Result->PhaseSeg2, ErrMin);
	}

	return RTK_SUCCESS;
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */