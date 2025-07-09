/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *TAG = "CAN";

const struct CAN_DevTable CAN_DEV_TABLE[2] = {
	{RCAN0, CAN0_IRQ, GDMA_HANDSHAKE_INTERFACE_CAN0_RX},
	{RCAN1, CAN1_IRQ, GDMA_HANDSHAKE_INTERFACE_CAN1_RX},
};

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup CAN
* @brief CAN driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup CAN_Exported_Functions CAN Exported Functions
  * @{
  */

/**
  * @brief  Fills each RCAN_InitStruct member with its default value.
  * @param  RCAN_InitStruct: pointer to an RCAN_InitTypeDef structure which will be initialized.
  * @retval   None
  */
void RCAN_StructInit(RCAN_InitTypeDef *RCAN_InitStruct)
{
	RCAN_InitStruct->CAN_AutoReTxEn = DISABLE;
	RCAN_InitStruct->CAN_BitPrescaler = 2; //256;
	RCAN_InitStruct->CAN_ErrCntThreshold = 64;
	RCAN_InitStruct->CAN_RxFifoEn = DISABLE;
	RCAN_InitStruct->CAN_SJW = 1; //8;
	RCAN_InitStruct->CAN_TimStampDiv = 128;
	RCAN_InitStruct->CAN_TimStampEn = ENABLE;
	RCAN_InitStruct->CAN_TriSampleEn = DISABLE;
	RCAN_InitStruct->CAN_TSEG1 = 15; //16;
	RCAN_InitStruct->CAN_TSEG2 = 4; //16;
	RCAN_InitStruct->CAN_WorkMode = RCAN_NORMAL_MODE;
}

/**
  * @brief    Initializes the CAN peripheral according to the specified
  *               parameters in the RCAN_InitStruct.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RCAN_InitStruct: pointer to a RCAN_InitTypeDef structure that contains
  *              the configuration information for the specified CAN peripheral.
  * @retval None
  */
void RCAN_Init(RCAN_TypeDef *RCANx, RCAN_InitTypeDef *RCAN_InitStruct)
{
	u32 can_ctrl = 0;
	u32 can_bit_timing = 0;
	u32 can_error_cnt_ctl = 0;
	u32 can_time_stamp = 0;
	u32 can_test = 0;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	can_ctrl = RCANx->CAN_CTL;
	can_bit_timing = RCANx->CAN_BIT_TIMING;
	can_error_cnt_ctl = RCANx->CAN_ERR_CNT_CTL;
	can_time_stamp = RCANx->CAN_TIME_STAMP;
	can_test = RCANx->CAN_TEST;
	/* 1.CAN 2.0 A/B bit prescaler*/
	can_bit_timing &= ~(CAN_MASK_BRP | CAN_MASK_SJW | CAN_MASK_TSEG2 | CAN_MASK_TSEG1);
	can_bit_timing |= CAN_BRP(RCAN_InitStruct->CAN_BitPrescaler - 1) |
					  CAN_SJW(RCAN_InitStruct->CAN_SJW) |
					  CAN_TSEG2(RCAN_InitStruct->CAN_TSEG2 - 1) |
					  CAN_TSEG1(RCAN_InitStruct->CAN_TSEG1 - 1);

	/*tripple sample*/
	if (RCAN_InitStruct->CAN_TriSampleEn) {
		can_ctrl |= CAN_BIT_TRI_SAMPLE;
	} else {
		can_ctrl &= ~ CAN_BIT_TRI_SAMPLE;
	}
	/*auto tx trans*/
	if (RCAN_InitStruct->CAN_AutoReTxEn) {
		can_ctrl |= CAN_BIT_AUTO_RE_TX_EN;
	} else {
		can_ctrl &= ~ CAN_BIT_AUTO_RE_TX_EN;
	}
	/*fifo mode*/
	if (RCAN_InitStruct->CAN_RxFifoEn) {
		can_ctrl |= CAN_BIT_RX_FIFO_EN;
	} else {
		can_ctrl &= ~ CAN_BIT_RX_FIFO_EN;
	}

	/* 2. work mode or test mode*/
	if (RCAN_InitStruct->CAN_WorkMode == RCAN_NORMAL_MODE) {
		can_ctrl &= ~ CAN_BIT_TEST_MODE_EN;
	} else {
		/* test mdoe */
		can_ctrl |= CAN_BIT_TEST_MODE_EN;
		can_test &= ~ CAN_MASK_TEST_CFG;
		can_test |= CAN_TEST_CFG(RCAN_InitStruct->CAN_WorkMode);
	}

	/* 3. Error count threshold*/
	can_error_cnt_ctl &= ~CAN_MASK_ERROR_WARN_TH;
	can_error_cnt_ctl |= CAN_ERROR_WARN_TH(RCAN_InitStruct->CAN_ErrCntThreshold);

	/* 4. Stamp timer*/
	if (RCAN_InitStruct->CAN_TimStampEn) {
		can_time_stamp &= ~(CAN_MASK_TIME_STAMP_DIV | CAN_BIT_TIME_STAMP_EN);
		can_time_stamp |= CAN_TIME_STAMP_DIV(RCAN_InitStruct->CAN_TimStampDiv);
		can_time_stamp |= CAN_BIT_TIME_STAMP_EN;
	} else {
		can_time_stamp &= (~CAN_BIT_TIME_STAMP_EN);
	}

	RCANx->CAN_CTL = can_ctrl;
	RCANx->CAN_BIT_TIMING = can_bit_timing;
	RCANx->CAN_ERR_CNT_CTL = can_error_cnt_ctl;
	RCANx->CAN_TIME_STAMP = can_time_stamp;
	RCANx->CAN_TEST = can_test;
}

/**
  * @brief  wake filter configuration and enable
  * @param  RCANx: where RCANx can be RCAN.
  * @param  FltNum: filter threshold.
  * @retval None
  */
void RCAN_WakeFilterConfig(RCAN_TypeDef *RCANx, u32 FltNum)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	RCANx->CAN_SLEEP_MODE &= ~CAN_MASK_WAKEPIN_FLT_LENGTH;
	RCANx->CAN_SLEEP_MODE |= CAN_WAKEPIN_FLT_LENGTH(FltNum);

}
/**
  * @brief    Low power filter ENABLE or DISABLE.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  NewState: where RCANx can be ENABLE or DISABLE.
  * @retval None
  */
void RCAN_LpFilterCmd(RCAN_TypeDef *RCANx, u32 NewState)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if (NewState == ENABLE) {
		RCANx->CAN_SLEEP_MODE |= CAN_BIT_WAKEPIN_FLT_EN;
	} else {
		RCANx->CAN_SLEEP_MODE &= (~CAN_BIT_WAKEPIN_FLT_EN);
	}
}

void RCAN_RequestSleepOrWake(RCAN_TypeDef *RCANx, bool sleep)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	if (sleep) {
		RCANx->CAN_SLEEP_MODE |= CAN_BIT_SLEEP_EN;
	} else {
		RCANx->CAN_SLEEP_MODE &= ~CAN_BIT_SLEEP_EN;
	}
}

u32 RCAN_SleepStatusGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	return (RCANx->CAN_SLEEP_MODE & CAN_BIT_SLEEP_STATE);
}

/**
  * @brief    bus request ENABLE or DISABLE.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  NewState: where RCANx can be ENABLE or DISABLE.
  * @retval None
  */
void RCAN_BusCmd(RCAN_TypeDef *RCANx, u32 NewState)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if (NewState == ENABLE) {
		RCANx->CAN_CTL |= CAN_BIT_BUS_ON_REQ;
	} else {
		RCANx->CAN_CTL &= (~CAN_BIT_BUS_ON_REQ);
	}
}
/**
  * @brief    CAN ENABLE or DISABLE.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  NewState: where RCANx can be ENABLE or DISABLE.
  * @retval None
  */
void RCAN_Cmd(RCAN_TypeDef *RCANx, u32 NewState)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if (NewState == ENABLE) {
		RCANx->CAN_CTL |= CAN_BIT_EN;
	} else {
		RCANx->CAN_CTL &= (~CAN_BIT_EN);
	}
}

/**
  * @brief    get CAN bus status.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval bus status
  --1: on
  --0: off
  */
u32 RCAN_BusStatusGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return (RCANx->CAN_STS & CAN_BIT_BUS_ON_STATE);
}

/**
  * @brief    get CAN fifo status.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval fifo status, this value can be:
--RCAN_FIFO_MSG_FULL
--RCAN_FIFO_MSG_EMTY
--RCAN_FIFO_MSG_OVERFLW
  */
u32 RCAN_FifoStatusGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return (RCANx->CAN_FIFO_STS & (CAN_BIT_FIFO_MSG_OVERFLOW | CAN_BIT_FIFO_MSG_EMPTY | CAN_BIT_FIFO_MSG_FULL));
}

/**
  * @brief    get CAN fifo level.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval rx fifo level
  */
u32 RCAN_FifoLvlGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return CAN_GET_FIFO_MSG_LVL(RCANx->CAN_FIFO_STS);
}

/**
  * @brief    Read Rx message from FIFO.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RxMsg: rx message pointer.
  * @retval rx fifo level
  */
void RCAN_ReadRxMsgFromFifo(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg)
{
	RxMsg->MsgBufferIdx = RCAN_RX_FIFO_READ_MSG_IDX;

	RCAN_ReadMsg(RCANx, RxMsg);
}

/**
  * @brief    get CAN tx error counter.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval tx error counter
  */
u32 RCAN_TXErrCntGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return CAN_GET_TEC(RCANx->CAN_ERR_CNT_STS);
}

/**
  * @brief    get CAN rx error counter.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval rx error counter
  */
u32 RCAN_RXErrCntGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return CAN_GET_REC(RCANx->CAN_ERR_CNT_STS);
}

/**
  * @brief    CAN tx error counter clear.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval none
  */
void RCAN_TXErrCntClear(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	RCANx->CAN_ERR_CNT_CTL |= CAN_BIT_TX_ERR_CNT_CLR;
}

/**
  * @brief    CAN rx error counter clear.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval none
  */
void RCAN_RXErrCntClear(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	RCANx->CAN_ERR_CNT_CTL |= CAN_BIT_RX_ERR_CNT_CLR;
}

/**
  * @brief    CAN rx error counter clear.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval error counter status
--RCAN_TX_ERR_PASSIVE
--RCAN_TX_ERR_BUSOFF
--RCAN_TX_ERR_WARNING
  */
u32 RCAN_RXErrCntSTS(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return (RCANx->CAN_ERR_CNT_STS & (CAN_BIT_ERROR_WARNING | CAN_BIT_ERROR_BUSOFF | CAN_BIT_ERROR_PASSIVE));
}

/**
  * @brief  write frame to message ram buffer.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void RCAN_WriteMsg(RCAN_TypeDef *RCANx, RCAN_TxMsgTypeDef *TxMsg)
{
	u32 can_ram_arb = 0;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs = 0;
	u32 i;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(IS_RCAN_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_RCAN_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= TxMsg->MsgBufferIdx;
	RCANx->CAN_RAM_CMD = can_ram_cmd;

	/*configure frame ARB register*/
	can_ram_arb = RCANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= TxMsg->RTR;
	can_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == RCAN_STANDARD_FRAME) {
		can_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= TxMsg->ExtId;
	}
	RCANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = RCANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_MASK_RAM_DLC | CAN_BIT_RAM_AUTOREPLY | CAN_BIT_RAM_EDL | CAN_BIT_RAM_BRS | CAN_BIT_RAM_ESI));
	can_ram_cs |= TxMsg->DLC;
	can_ram_cs |= CAN_BIT_RAM_RXTX;
	RCANx->CAN_RAM_CS = can_ram_cs;

	/*fill data, can2.0 8 bytes, can fd 64 bytes*/
	if (TxMsg->RTR == RCAN_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			HAL_WRITE32(&RCANx->CAN_RAM_FDDATA_x[i], 0, *(u32 *)(&TxMsg->Data[(16 - i - 1) * 4]));
		}
	}

	/*finally, operate cmd register to write frame info in register into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	RCANx->CAN_RAM_CMD = can_ram_cmd;
	while (RCANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/**
  * @brief    set rx message ram buffer.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RxMsg: rx message pointer.
  * @retval none
  */
void RCAN_SetRxMsgBuf(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg)
{
	u32 can_ram_arb = 0;
	u32 can_ram_cmd = 0;
	u32 can_ram_mask = 0;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(IS_RCAN_FRAME_TYPE(RxMsg->RTR));
	assert_param(IS_RCAN_ID_TYPE(RxMsg->IDE));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= RxMsg->MsgBufferIdx;

	/*configure frame ARB register*/
	can_ram_arb |= RxMsg->RTR;
	can_ram_arb |= RxMsg->IDE;

	if (RxMsg->IDE == RCAN_STANDARD_FRAME) {
		can_ram_arb |= CAN_RAM_ID((RxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= CAN_RAM_ID(RxMsg->ExtId);
	}
	RCANx->CAN_RAM_ARB = can_ram_arb;


	/*configure MASK ID register*/
	can_ram_mask |= CAN_RAM_ID_MASK(RxMsg->ID_MASK);
	can_ram_mask |= RxMsg->IDE_Mask;
	can_ram_mask |= RxMsg->RTR_Mask;
	RCANx->CAN_RAM_MASK = can_ram_mask;

	/*finally, operate cmd register to write RX setting info in register into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	RCANx->CAN_RAM_CMD = can_ram_cmd;
	while (RCANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/**
  * @brief    read frame from message ram buffer.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RxMsg: rx message pointer.
  * @retval none
  */
void RCAN_ReadMsg(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg)
{
	u32 can_ram_arb = 0;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs = 0;
	u32 can_ram_mask = 0;
	u32 i;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | CAN_BIT_RAM_ACC_DATA_MASK);
	can_ram_cmd |= RxMsg->MsgBufferIdx;
	can_ram_cmd |= CAN_BIT_RAM_START;
	RCANx->CAN_RAM_CMD = can_ram_cmd;/*firstly, operate cmd register to read frame into register from ram message buffer*/
	while (RCANx->CAN_RAM_CMD & CAN_BIT_RAM_START);

	/*read ARB register*/
	can_ram_arb = RCANx->CAN_RAM_ARB;
	if (can_ram_arb & CAN_BIT_RAM_RTR) {
		RxMsg->RTR = RCAN_REMOTE_FRAME;
	} else {
		RxMsg->RTR = RCAN_DATA_FRAME;
	}
	if (can_ram_arb & CAN_BIT_RAM_IDE) {
		RxMsg->IDE = RCAN_EXTEND_FRAME;
	} else {
		RxMsg->IDE = RCAN_STANDARD_FRAME;
	}

	if (RxMsg->IDE == RCAN_STANDARD_FRAME) {
		RxMsg->StdId = CAN_GET_RAM_ID(can_ram_arb) >> 18;
	} else {
		RxMsg->ExtId = CAN_GET_RAM_ID(can_ram_arb);
	}

	/*read CS register*/
	can_ram_cs = RCANx->CAN_RAM_CS;
	RxMsg->RxTimStamp = CAN_GET_RAM_TIMESTAMP(can_ram_cs);
	if (can_ram_cs & CAN_BIT_RAM_LOST) {
		RxMsg->RxLost = 1;
	} else {
		RxMsg->RxLost = 0;
	}
	RxMsg->DLC = CAN_GET_RAM_DLC(can_ram_cs);

	/*read mask ID*/
	can_ram_mask = RCANx->CAN_RAM_MASK;
	RxMsg->ID_MASK = CAN_GET_RAM_ID_MASK(can_ram_mask);
	RxMsg->IDE_Mask = (can_ram_mask & CAN_BIT_RAM_IDE_MASK);
	RxMsg->RTR_Mask = (can_ram_mask & CAN_BIT_RAM_RTR_MASK);

	/*configure DATA register: can2.0 8 bytes, can fd 64 bytes*/
	if (RxMsg->RTR == RCAN_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			*((u32 *)(&RxMsg->Data[(16 - i - 1) * 4])) = HAL_READ32(&RCANx->CAN_RAM_FDDATA_x[i], 0);
		}
	}
}

/**
  * @brief    send auto reply message.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void RCAN_TxAutoReply(RCAN_TypeDef *RCANx, RCAN_TxMsgTypeDef *TxMsg)
{
	u32 can_ram_arb;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs;
	u32 i;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(IS_RCAN_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_RCAN_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= TxMsg->MsgBufferIdx;
	RCANx->CAN_RAM_CMD = can_ram_cmd;

	/*configure frame ARB register*/
	can_ram_arb = RCANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= TxMsg->RTR;
	can_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == RCAN_STANDARD_FRAME) {
		can_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= TxMsg->ExtId;
	}
	RCANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = RCANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_MASK_RAM_DLC | CAN_BIT_RAM_AUTOREPLY | CAN_BIT_RAM_ESI | CAN_BIT_RAM_BRS | CAN_BIT_RAM_EDL));

	can_ram_cs |= TxMsg->DLC;
	/*enable auto-reply*/
	can_ram_cs |= CAN_BIT_RAM_RXTX | CAN_BIT_RAM_AUTOREPLY;

	RCANx->CAN_RAM_CS = can_ram_cs;

	/*configure DATA register: can2.0 8 bytes, can fd 64 bytes*/
	if (TxMsg->RTR == RCAN_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			RCANx->CAN_RAM_FDDATA_x[i] = TxMsg->Data[(16 - i - 1) * 4];
		}
	}

	/*finally, operate cmd register to write frame info in register into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	RCANx->CAN_RAM_CMD = can_ram_cmd;
	while (RCANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/**
  * @brief    receive auto reply message.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void RCAN_RxAutoReply(RCAN_TypeDef *RCANx, RCAN_RxMsgTypeDef *RxMsg)
{
	u32 can_ram_arb;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs;
	u32 can_ram_mask;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(IS_RCAN_FRAME_TYPE(RxMsg->RTR));
	assert_param(IS_RCAN_ID_TYPE(RxMsg->IDE));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= RxMsg->MsgBufferIdx;
	//RCANx->CAN_RAM_CMD = can_ram_cmd;

	/*configure frame ARB register*/
	can_ram_arb = RCANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= RxMsg->RTR;
	can_ram_arb |= RxMsg->IDE;

	if (RxMsg->IDE == RCAN_STANDARD_FRAME) {
		can_ram_arb |= ((RxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= RxMsg->ExtId;
	}
	RCANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = RCANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_BIT_RAM_RXTX | CAN_BIT_RAM_AUTOREPLY | CAN_BIT_RAM_ESI | CAN_BIT_RAM_BRS | CAN_BIT_RAM_BRS));
	/*enable auto-reply*/
	can_ram_cs |= CAN_BIT_RAM_AUTOREPLY;

	RCANx->CAN_RAM_CS = can_ram_cs;

	/*configure MASK ID register*/
	can_ram_mask = RCANx->CAN_RAM_MASK;
	can_ram_mask &= (~CAN_MASK_RAM_ID_MASK | CAN_BIT_RAM_IDE_MASK | CAN_BIT_RAM_RTR_MASK);
	can_ram_mask |= RxMsg->ID_MASK;
	can_ram_mask |= RxMsg->IDE_Mask;
	can_ram_mask |= RxMsg->RTR_Mask;
	RCANx->CAN_RAM_MASK = can_ram_mask;

	/*finally, operate cmd register to write RX setting info in register into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	RCANx->CAN_RAM_CMD = can_ram_cmd;
	while (RCANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/**
  * @brief  Enables or disables the specified CAN interrupts.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RCAN_IT: specifies the CAN interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg RCAN_TX_INT
  *     @arg RCAN_RX_INT
  *     @arg RCAN_ERR_INT
  *     @arg RCAN_WKUP_INT
  *     @arg RCAN_BUSOFF_INT
  * @param  NewState: new state of the specified CAN interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCAN_INTConfig(RCAN_TypeDef *RCANx, u32 RCAN_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if (NewState != DISABLE) {
		/* Enable the selected CAN interrupts */
		RCANx->CAN_INT_EN |= RCAN_IT;
	} else {
		/* Disable the selected CAN interrupts */
		RCANx->CAN_INT_EN &= (~RCAN_IT);
	}
}

/**
  * @brief  Get CAN interrupt status.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval interrupt status
  */
u32 RCAN_GetINTStatus(RCAN_TypeDef *RCANx)
{
	/*check the parameters*/
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return RCANx->CAN_INT_FLAG;
}

/**
  * @brief  Clears the CAN interrupt pending bits.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RCAN_IT: specifies the interrupt to be cleared.
  * @retval None
  */
void RCAN_ClearINT(RCAN_TypeDef *RCANx, u32 RCAN_IT)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(IS_RCAN_CLEAR_IT(RCAN_IT));

	/*clear the specified interrupt*/
	RCANx->CAN_INT_FLAG = RCAN_IT;
}

/**
  * @brief  Clears the CAN interrupt pending bits.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RCAN_IT: specifies the interrupt to be cleared.
  * @retval None
  */
void RCAN_ClearAllINT(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	RCANx->CAN_INT_FLAG = 0xFFFFFFFF;
}

/**
  * @brief  Get the CAN error status.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval error status
  */
u32 RCAN_GetErrStatus(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return (RCANx->CAN_ERR_STATUS);
}

/**
  * @brief  Clears the CAN error pending bits.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  ERR_STS: specifies the error status to be cleared.
  * @retval None
  */
void RCAN_ClearErrStatus(RCAN_TypeDef *RCANx, u32 ERR_STS)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	/*clear the specified error status*/
	RCANx->CAN_ERR_STATUS = ERR_STS;
}

/**
  * @brief  Enables or disables the specified CAN Rx message buffer interrupts.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RCAN_IT: specifies the CAN interrupts sources to be enabled or disabled.
   * @param  NewState: new state of the specified CAN interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCAN_RxMsgBufINTConfig(RCAN_TypeDef *RCANx, u32 BUF_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if (NewState != DISABLE) {
		/* Enable the selected CAN rx msg buffer interrupts */
		RCANx->CAN_MB_RXINT_EN |= BUF_IT;
	} else {
		/* Disable the selected CAN rx msg buffer interrupts */
		RCANx->CAN_MB_RXINT_EN &= (~BUF_IT);
	}
}

/**
  * @brief  Enables or disables the specified CAN Tx message buffer interrupts.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  RCAN_IT: specifies the CAN interrupts sources to be enabled or disabled.
   * @param  NewState: new state of the specified CAN interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCAN_TxMsgBufINTConfig(RCAN_TypeDef *RCANx, u32 BUF_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if (NewState != DISABLE) {
		/* Enable the selected CAN tx msg buffer interrupts */
		RCANx->CAN_MB_TXINT_EN |= BUF_IT;
	} else {
		/* Disable the selected CAN tx msg buffer interrupts */
		RCANx->CAN_MB_TXINT_EN &= (~BUF_IT);
	}
}

/**
  * @brief  get tx message buffer error status.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval tx message error status
  */
u32 RCAN_TxMsgBufErrGet(RCAN_TypeDef *RCANx)
{
	/*check the parameters*/
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return (RCANx->CAN_TX_ERROR_FLAG);
}

/**
  * @brief  clear tx message buffer error status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  ERR: the specified interrupt source.
  * @retval None
  */
void RCAN_TxMsgBufErrClear(RCAN_TypeDef *RCANx, u32 ERR_FLAG)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	/*clear the specified msg buffer error*/
	RCANx->CAN_TX_ERROR_FLAG = ERR_FLAG;
}

/**
  * @brief  get message buffer status register value.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval register value
  */
u32 RCAN_MsgBufStatusRegGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(MsgBufIdx <= 15);

	return (RCANx->CAN_MBx_STS[MsgBufIdx]);
}

/**
  * @brief  get message buffer status register value.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--RCAN_FRAME_PENDING_TX
	--RCAN_FRAME_FINISAH_TX
	--RCAN_FRAME_OVWRITR_SEND_TX
	--RCAN_FRAME_PENDING_RX
	--RCAN_FRAME_FINISH_RX
  */
u32 RCAN_MsgBufStatusGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(MsgBufIdx <= 15);

	return (RCANx->CAN_MBx_STS[MsgBufIdx] & 0xf);
}

/**
  * @brief  check message buffer available or not.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--1: available
	--0: not available
  */
u32 RCAN_CheckMsgBufAvailable(RCAN_TypeDef *RCANx, u32 MsgBufIdx)
{
	u32 Temp;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	Temp = (RCANx->CAN_MBx_STS[MsgBufIdx] & 0xf);

	if ((Temp != RCAN_FRAME_PENDING_TX) && (Temp != RCAN_FRAME_PENDING_RX)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  get CAN tx done status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
u32 RCAN_TxDoneStatusGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return (RCANx->CAN_TX_DONE);
}

/**
  * @brief  get message buffer tx done status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--1: tx complete
	--0: tx pending
  */
u32 RCAN_MsgBufTxDoneStatusGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if ((RCANx->CAN_TX_DONE) & BIT(MsgBufIdx)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  get CAN rx done status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
u32 RCAN_RxDoneStatusGet(RCAN_TypeDef *RCANx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	return (RCANx->CAN_RX_DONE);
}
/**
  * @brief  wait message buffer rx done.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval status, can be the following values:
	--1: rx complete
	--0: rx pending
  */
u32 RCAN_MsgBufRxDoneStatusGet(RCAN_TypeDef *RCANx, u32 MsgBufIdx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	if ((RCANx->CAN_RX_DONE) & BIT(MsgBufIdx)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  clear CAN tx done status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  Status: clear status.
  * @retval None
  */
void RCAN_TxDoneStatusClear(RCAN_TypeDef *RCANx, u32 Status)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	/*clear the specified msg buffer tx done status*/
	RCANx->CAN_TX_DONE = Status;
}

/**
  * @brief  clear CAN tx done message buffer status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
void RCAN_MsgBufTxDoneStatusClear(RCAN_TypeDef *RCANx, u32 MsgBufIdx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	/*clear the specified msg buffer tx done status*/
	RCANx->CAN_TX_DONE = BIT(MsgBufIdx);
}

/**
  * @brief  clear CAN rx done status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  Status: clear status.
  * @retval None
  */
void RCAN_RxDoneStatusClear(RCAN_TypeDef *RCANx, u32 Status)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	/*clear the specified msg buffer tx done status*/
	RCANx->CAN_RX_DONE = Status;
}

/**
  * @brief  clear CAN rx done message buffer status.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgBufIdx: message buffer index.
  * @retval None
  */
void RCAN_MsgBufRxDoneStatusClear(RCAN_TypeDef *RCANx, u32 MsgBufIdx)
{
	assert_param(IS_RCAN_ALL_PERIPH(RCANx));

	/*clear the specified msg buffer tx done status*/
	RCANx->CAN_RX_DONE = BIT(MsgBufIdx);
}


/**
  * @brief    CAN TX message trigger ENABLE or DISABLE.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  State: where RCANx can be ENABLE or DISABLE.
  * @retval None
  */
void RCAN_TxMsgTriggerCmd(RCAN_TypeDef *RCANx, u32 State)
{
	if (State == DISABLE) {
		RCANx->CAN_MB_TRIGGER &= ~CAN_BIT_TX_TRIGGER_EN;
	} else {
		RCANx->CAN_MB_TRIGGER |= CAN_BIT_TX_TRIGGER_EN;
	}
}

/**
  * @brief    CAN TX message trigger configure.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  CloseOffset: close offset.
  * @param  Begin: trigger begin.
  * @retval None
  */
void RCAN_TxMsgTriggerConfig(RCAN_TypeDef *RCANx, u32 CloseOffset, u32 Begin)
{
	u32 MbTrigger;

	MbTrigger = RCANx->CAN_MB_TRIGGER;

	MbTrigger &= (~(CAN_MASK_TX_TRIGGER_CLOSE_OFFSET | CAN_MASK_TX_TRIGGER_BEGIN));

	MbTrigger |= (Begin | CAN_TX_TRIGGER_CLOSE_OFFSET(CloseOffset));

	RCANx->CAN_MB_TRIGGER = MbTrigger;
}

/**
  * @brief    CAN DMA destination base address configure.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  Addr: base address.
  * @retval None
  */
void RCAN_RxDmaDestBaseAddrConfig(RCAN_TypeDef *RCANx, u32 Addr)
{
	RCANx->CAN_RXDMA_CFG = Addr;
}

/**
  * @brief    CAN get RX DMA data.
  * @param  RCANx: where RCANx can be RCAN.
  * @retval DMA data
  */
u32 RCAN_GetRxDmaData(RCAN_TypeDef *RCANx)
{
	return RCANx->CAN_RX_DMA_DATA;
}

/**
  * @brief    CAN control register configure.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgIdx: message buffer index.
  * @param  RxDmaEn: RX DMA enable.
  * @param  RxDmaOffset: RX DMA offset.
  * @retval none
  */
void RCAN_MsgCtrlRegConfig(RCAN_TypeDef *RCANx, u32 MsgIdx, u32 RxDmaEn, u32 DmaOffset)
{
	u32 MsgBufferCtrl;

	MsgBufferCtrl = RCANx->CAN_MBx_CTRL[MsgIdx];

	MsgBufferCtrl &= (~(CAN_BIT_MSGx_RXDMA_EN | CAN_MASK_MSGx_BA));

	if (RxDmaEn == DISABLE) {
		MsgBufferCtrl &= (~CAN_BIT_MSGx_RXDMA_EN);
	} else {
		MsgBufferCtrl |= CAN_BIT_MSGx_RXDMA_EN;
	}

	MsgBufferCtrl |= (DmaOffset << 10);
	RCANx->CAN_MBx_CTRL[MsgIdx] = MsgBufferCtrl;
}

/**
  * @brief    CAN control register configure.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  MsgIdx: message buffer index.
  * @param  BaseAddr: message buffer base address.
  * @retval none
  */
void RCAN_MsgBaseAddrConfig(RCAN_TypeDef *RCANx, u32 MsgIdx, u32 BaseAddr)
{
	u32 MsgBufferCtrl;

	MsgBufferCtrl = RCANx->CAN_MBx_CTRL[MsgIdx];

	MsgBufferCtrl &= ~(CAN_MASK_MSGx_BA);

	MsgBufferCtrl |= CAN_MSGx_BA(BaseAddr);

	RCANx->CAN_MBx_CTRL[MsgIdx] = MsgBufferCtrl;
}

/**
  * @brief    CAN message buffer end address.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  EndAddr: message buffer end address.
  * @retval none
  */
void RCAN_MsgBaseAddrEndReg(RCAN_TypeDef *RCANx, u32 EndAddr)
{
	RCANx->CAN_MB_BA_END &= (~(CAN_MASK_MSG_BA_END));

	RCANx->CAN_MB_BA_END |= CAN_MSG_BA_END(EndAddr);
}

/**
  * @brief    Init and Enable CAN RX GDMA.
  * @param  UartIndex: 0 or 1.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @retval   TRUE/FLASE
  */
bool RCAN_RXGDMA_Init(
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
	GDMA_InitStruct->GDMA_SrcAddr = CAN_DEV_TABLE[Index].RCANx->CAN_RX_DMA_DATA;
	GDMA_InitStruct->GDMA_DstAddr = (u32)pRxBuf;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = CAN_DEV_TABLE[Index].Rx_HandshakeInterface;
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
  * @param  RCANxBufer: where RCANxBufer can be dma ram buffer.
  * @param  TxMsg: tx message pointer.
  * @retval none
  */
void RCAN_FillTXDmaBuffer(RCAN_TypeDef *RCANx, RCAN_TxMsgTypeDef *TxMsg)
{
	u32 can_ram_arb;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs;
	u32 i;

	assert_param(IS_RCAN_ALL_PERIPH(RCANx));
	assert_param(IS_RCAN_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_RCAN_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					/* RCAN_RAM_ACC_DAT_MASK  | */ CAN_BIT_RAM_DIR);
	can_ram_cmd |= TxMsg->MsgBufferIdx;
	RCANx->CAN_RAM_CMD = can_ram_cmd;

	/*configure frame ARB register*/
	can_ram_arb = RCANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= TxMsg->RTR;
	can_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == RCAN_STANDARD_FRAME) {
		can_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= TxMsg->ExtId;
	}
	RCANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = RCANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_MASK_RAM_DLC | CAN_BIT_RAM_AUTOREPLY | CAN_BIT_RAM_EDL | CAN_BIT_RAM_BRS | CAN_BIT_RAM_ESI));
	if (TxMsg->RTR == RCAN_DATA_FRAME) {
		can_ram_cs |= TxMsg->DLC;
	}
	can_ram_cs |= CAN_BIT_RAM_RXTX;

	RCANx->CAN_RAM_CS = can_ram_cs;

	/*fill data, can2.0 8 bytes, can fd 64 bytes*/
	if (TxMsg->RTR == RCAN_DATA_FRAME) {
		for (i = 0; i < 2; i++) {
			// RCANx->CAN_RAM_FDDATA_x[i] = TxMsg->Data[(16 - i - 1) * 4];
			RCANx->CAN_RAM_FDDATA_x[i] = TxMsg->Data[(2 - i - 1) * 4];
		}
	}

	/*finally, operate cmd register to write frame info in register into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	RCANx->CAN_RAM_CMD = can_ram_cmd;
	//while(RCANxBufer->CAN_RAM_CMD&CAN_BIT_RAM_START);
}


/**
  * @brief    configure ram buffer mapping.
  * @param  RCANx: where RCANx can be RCAN.
  * @param  pPara: mapping parameters.
  * @retval DMA data
  */
void RCAN_RamBufferMapConfig(RCAN_TypeDef *RCANx, u32 *pPara)
{
	u32 i;

	/*message buffer address*/
	for (i = 0; i < RCAN_MESSAGE_BUFFER_SIZE; i++) {
		RCAN_MsgBaseAddrConfig(RCANx, i, *(pPara + i));
	}

	/*end address*/
	RCAN_MsgBaseAddrEndReg(RCANx, *(pPara + RCAN_MESSAGE_BUFFER_SIZE));
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