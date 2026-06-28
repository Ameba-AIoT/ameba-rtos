/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *TAG = "CAN";

const struct CAN_DevTable CAN_DEV_TABLE[2] = {
	{CAN0, CAN0_IRQ, GDMA_HANDSHAKE_INTERFACE_CAN0_RX},
	{CAN1, CAN1_IRQ, GDMA_HANDSHAKE_INTERFACE_CAN1_RX},
};

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup CAN CAN
* @brief CAN driver modules
* @{
*/

void CAN_RequestSleepOrWake(CAN_TypeDef *CANx, bool sleep)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));
	if (sleep) {
		CANx->CAN_SLEEP_MODE |= CAN_BIT_SLEEP_EN;
	} else {
		CANx->CAN_SLEEP_MODE &= ~CAN_BIT_SLEEP_EN;
	}
}

u32 CAN_SleepStatusGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));
	return (CANx->CAN_SLEEP_MODE & CAN_BIT_SLEEP_STATE);
}

/**
  * @brief    Write frame to DMA message RAM buffer.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  TxMsg Tx message pointer.
  */
void CAN_FillTXDmaBuffer(CAN_TypeDef *CANx, CAN_TxMsgTypeDef *TxMsg)
{
	u32 can_ram_arb;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs;
	u32 i;

	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(IS_CAN_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_CAN_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK  |  CAN_BIT_RAM_DIR);
	can_ram_cmd |= CAN_RAM_ACC_NUM(TxMsg->MsgBufferIdx);
	CANx->CAN_RAM_CMD = can_ram_cmd;

	/*configure frame ARB register*/
	can_ram_arb = CANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= TxMsg->RTR;
	can_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == CAN_STANDARD_FRAME) {
		can_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= TxMsg->ExtId;
	}
	CANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = CANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_MASK_RAM_DLC | CAN_BIT_RAM_AUTOREPLY));
	if (TxMsg->RTR == CAN_DATA_FRAME) {
		can_ram_cs |= TxMsg->DLC;
	}
	can_ram_cs |= CAN_BIT_RAM_RXTX;

	CANx->CAN_RAM_CS = can_ram_cs;

	/*fill data, can2.0 8 bytes*/
	if (TxMsg->RTR == CAN_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			CANx->CAN_RAM_DATA_x[16 - i - 1] = TxMsg->Data_32[i];
		}
	}

	/* Write frame info into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	CANx->CAN_RAM_CMD = can_ram_cmd;
	while (CANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/* Exported functions --------------------------------------------------------*/
/** @defgroup CAN_Exported_Functions CAN Exported Functions
  * @{
  */
/** @defgroup CAN_global_functions CAN Global Functions
  * @{
  */

/**
  * @brief  Fills each CAN_InitStruct member with its default value.
  * @param  CAN_InitStruct Pointer to a @ref CAN_InitTypeDef structure which will be initialized.
  */
void CAN_StructInit(CAN_InitTypeDef *CAN_InitStruct)
{
	CAN_InitStruct->CAN_AutoReTxEn = DISABLE;
	CAN_InitStruct->CAN_Timing.Prescaler = 2; //256;
	CAN_InitStruct->CAN_ErrCntThreshold = 64;
	CAN_InitStruct->CAN_RxFifoEn = ENABLE;
	CAN_InitStruct->CAN_Timing.SJW = 1; //8;
	CAN_InitStruct->CAN_TimStampDiv = 128;
	CAN_InitStruct->CAN_TimStampEn = ENABLE;
	CAN_InitStruct->CAN_TriSampleEn = DISABLE;
	CAN_InitStruct->CAN_Timing.PropSeg = 2;
	CAN_InitStruct->CAN_Timing.PhaseSeg1 = 13; //16;
	CAN_InitStruct->CAN_Timing.PhaseSeg2 = 4; //16;
	CAN_InitStruct->CAN_WorkMode = CAN_NORMAL_MODE;
}

/**
  * @brief    Initializes the CAN peripheral according to the specified
  *               parameters in the CAN_InitStruct.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  CAN_InitStruct Pointer to a @ref CAN_InitTypeDef structure that contains
  *              the configuration information for the specified CAN peripheral.
  */
void CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct)
{
	u32 can_ctrl = 0;
	u32 can_bit_timing = 0;
	u32 can_error_cnt_ctl = 0;
	u32 can_time_stamp = 0;
	u32 can_test = 0;

	assert_param(IS_CAN_ALL_PERIPH(CANx));

	can_ctrl = CANx->CAN_CTL;
	can_bit_timing = CANx->CAN_BIT_TIMING;
	can_error_cnt_ctl = CANx->CAN_ERR_CNT_CTL;
	can_time_stamp = CANx->CAN_TIME_STAMP;
	can_test = CANx->CAN_TEST;
	/* 1.CAN 2.0 A/B bit prescaler*/
	can_bit_timing &= ~(CAN_MASK_BRP | CAN_MASK_SJW | CAN_MASK_TSEG2 | CAN_MASK_TSEG1);
	can_bit_timing |= CAN_BRP(CAN_InitStruct->CAN_Timing.Prescaler - 1) |
					  CAN_SJW(CAN_InitStruct->CAN_Timing.SJW) |
					  CAN_TSEG2(CAN_InitStruct->CAN_Timing.PhaseSeg2 - 1) |
					  CAN_TSEG1(CAN_InitStruct->CAN_Timing.PropSeg + CAN_InitStruct->CAN_Timing.PhaseSeg1 - 1);

	/*triple sample*/
	if (CAN_InitStruct->CAN_TriSampleEn) {
		can_ctrl |= CAN_BIT_TRI_SAMPLE;
	} else {
		can_ctrl &= ~ CAN_BIT_TRI_SAMPLE;
	}
	/*auto tx trans*/
	if (CAN_InitStruct->CAN_AutoReTxEn) {
		can_ctrl |= CAN_BIT_AUTO_RE_TX_EN;
	} else {
		can_ctrl &= ~ CAN_BIT_AUTO_RE_TX_EN;
	}
	/*fifo mode*/
	if (CAN_InitStruct->CAN_RxFifoEn) {
		can_ctrl |= CAN_BIT_RX_FIFO_EN;
	} else {
		can_ctrl &= ~ CAN_BIT_RX_FIFO_EN;
	}

	/* 2. work mode or test mode*/
	if (CAN_InitStruct->CAN_WorkMode == CAN_NORMAL_MODE) {
		can_ctrl &= ~ CAN_BIT_TEST_MODE_EN;
	} else {
		/* test mode */
		can_ctrl |= CAN_BIT_TEST_MODE_EN;
		can_test &= ~ CAN_MASK_TEST_CFG;
		can_test |= CAN_TEST_CFG(CAN_InitStruct->CAN_WorkMode);
	}

	/* 3. Error count threshold*/
	can_error_cnt_ctl &= ~CAN_MASK_ERROR_WARN_TH;
	can_error_cnt_ctl |= CAN_ERROR_WARN_TH(CAN_InitStruct->CAN_ErrCntThreshold);

	/* 4. Stamp timer*/
	if (CAN_InitStruct->CAN_TimStampEn) {
		can_time_stamp &= ~(CAN_MASK_TIME_STAMP_DIV | CAN_BIT_TIME_STAMP_EN);
		can_time_stamp |= CAN_TIME_STAMP_DIV(CAN_InitStruct->CAN_TimStampDiv);
		can_time_stamp |= CAN_BIT_TIME_STAMP_EN;
	} else {
		can_time_stamp &= (~CAN_BIT_TIME_STAMP_EN);
	}

	CANx->CAN_CTL = can_ctrl;
	CANx->CAN_BIT_TIMING = can_bit_timing;
	CANx->CAN_ERR_CNT_CTL = can_error_cnt_ctl;
	CANx->CAN_TIME_STAMP = can_time_stamp;
	CANx->CAN_TEST = can_test;
}

/**
  * @brief  Low power filter configuration
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  FltNum Filter threshold.
  */
void CAN_LpFilterConfig(CAN_TypeDef *CANx, u32 FltNum)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));
	CANx->CAN_SLEEP_MODE &= ~CAN_MASK_WAKEPIN_FLT_LENGTH;
	CANx->CAN_SLEEP_MODE |= CAN_WAKEPIN_FLT_LENGTH(FltNum);

}
/**
  * @brief    Low power filter ENABLE or DISABLE.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  NewState New state of the low power filter. This parameter can be: ENABLE or DISABLE.
  */
void CAN_LpFilterCmd(CAN_TypeDef *CANx, u32 NewState)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if (NewState == ENABLE) {
		CANx->CAN_SLEEP_MODE |= CAN_BIT_WAKEPIN_FLT_EN;
	} else {
		CANx->CAN_SLEEP_MODE &= (~CAN_BIT_WAKEPIN_FLT_EN);
	}
}


/**
  * @brief    Bus request ENABLE or DISABLE.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  NewState New state of the bus request. This parameter can be: ENABLE or DISABLE.
  */
void CAN_BusCmd(CAN_TypeDef *CANx, u32 NewState)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if (NewState == ENABLE) {
		CANx->CAN_CTL |= CAN_BIT_BUS_ON_REQ;
	} else {
		CANx->CAN_CTL &= (~CAN_BIT_BUS_ON_REQ);
	}
}
/**
  * @brief    CAN ENABLE or DISABLE.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  NewState New state of the CAN peripheral. This parameter can be: ENABLE or DISABLE.
  */
void CAN_Cmd(CAN_TypeDef *CANx, u32 NewState)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if (NewState == ENABLE) {
		CANx->CAN_CTL |= CAN_BIT_EN;
	} else {
		CANx->CAN_CTL &= (~CAN_BIT_EN);
	}
}

/**
  * @brief    Get CAN bus status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return Bus status:
  *         - 1: on
  *         - 0: off
  */
u32 CAN_BusStatusGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return (CANx->CAN_STS & CAN_BIT_BUS_ON_STATE);
}

/**
  * @brief    Get CAN FIFO status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return FIFO status, this value can be:
  *         - CAN_BIT_FIFO_MSG_FULL
  *         - CAN_BIT_FIFO_MSG_EMPTY
  *         - CAN_BIT_FIFO_MSG_OVERFLOW
  */
u32 CAN_FifoStatusGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return (CANx->CAN_FIFO_STS & (CAN_BIT_FIFO_MSG_OVERFLOW | CAN_BIT_FIFO_MSG_EMPTY | CAN_BIT_FIFO_MSG_FULL));
}

/**
  * @brief    Get CAN FIFO level.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return Rx FIFO level
  */
u32 CAN_FifoLvlGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return CAN_GET_FIFO_MSG_LVL(CANx->CAN_FIFO_STS);
}

/**
  * @brief    Read Rx message from FIFO.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  RxMsg Rx message pointer.
  */
void CAN_ReadRxMsgFromFifo(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg)
{
	RxMsg->MsgBufferIdx = CAN_RX_FIFO_READ_MSG_IDX;

	CAN_ReadMsg(CANx, RxMsg);
}

/**
  * @brief    Get CAN TX error counter.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return TX error counter
  */
u32 CAN_TXErrCntGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return CAN_GET_TEC(CANx->CAN_ERR_CNT_STS);
}

/**
  * @brief    Get CAN RX error counter.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return RX error counter
  */
u32 CAN_RXErrCntGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return CAN_GET_REC(CANx->CAN_ERR_CNT_STS);
}

/**
  * @brief    CAN TX error counter clear.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  */
void CAN_TXErrCntClear(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	CANx->CAN_ERR_CNT_CTL |= CAN_BIT_TX_ERR_CNT_CLR;
}

/**
  * @brief    CAN RX error counter clear.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  */
void CAN_RXErrCntClear(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	CANx->CAN_ERR_CNT_CTL |= CAN_BIT_RX_ERR_CNT_CLR;
}

/**
  * @brief    Get CAN error counter status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return Error counter status:
  *         - CAN_TX_ERR_PASSIVE
  *         - CAN_TX_ERR_BUSOFF
  *         - CAN_TX_ERR_WARNING
  */
u32 CAN_RXErrCntSTS(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return (CANx->CAN_ERR_CNT_STS & (CAN_BIT_ERROR_WARNING | CAN_BIT_ERROR_BUSOFF | CAN_BIT_ERROR_PASSIVE));
}

/**
  * @brief  Write frame to message ram buffer.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  TxMsg Tx message pointer.
  */
void CAN_WriteMsg(CAN_TypeDef *CANx, CAN_TxMsgTypeDef *TxMsg)
{
	u32 can_ram_arb = 0;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs = 0;

	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(IS_CAN_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_CAN_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/* Enable Msg buf[x] access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= CAN_RAM_ACC_NUM(TxMsg->MsgBufferIdx);
	CANx->CAN_RAM_CMD = can_ram_cmd;

	/*configure frame ARB register*/
	can_ram_arb = CANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= TxMsg->RTR;
	can_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == CAN_STANDARD_FRAME) {
		can_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= TxMsg->ExtId;
	}
	CANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = CANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_MASK_RAM_DLC | CAN_BIT_RAM_AUTOREPLY));
	can_ram_cs |= TxMsg->DLC;
	can_ram_cs |= CAN_BIT_RAM_RXTX;
	CANx->CAN_RAM_CS = can_ram_cs;

	/*fill data, can2.0 8 bytes*/
	if (TxMsg->RTR == CAN_DATA_FRAME) {
		for (int i = 0; i < 16; i++) {
			CANx->CAN_RAM_DATA_x[16 - i - 1] = TxMsg->Data_32[i];
		}
	}

	/* Write frame info in register into the ram message buffer */
	can_ram_cmd |= CAN_BIT_RAM_START;
	CANx->CAN_RAM_CMD = can_ram_cmd;
	while (CANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/**
  * @brief    Set rx message ram buffer.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  RxMsg Rx message pointer.
  */
void CAN_SetRxMsgBuf(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg)
{
	u32 can_ram_arb = 0;
	u32 can_ram_cmd = 0;
	u32 can_ram_mask = 0;

	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(IS_CAN_FRAME_TYPE(RxMsg->RTR));
	assert_param(IS_CAN_ID_TYPE(RxMsg->IDE));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= CAN_RAM_ACC_NUM(RxMsg->MsgBufferIdx);

	/*configure frame ARB register*/
	can_ram_arb |= RxMsg->RTR;
	can_ram_arb |= RxMsg->IDE;

	if (RxMsg->IDE == CAN_STANDARD_FRAME) {
		can_ram_arb |= CAN_RAM_ID((RxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= CAN_RAM_ID(RxMsg->ExtId);
	}
	CANx->CAN_RAM_ARB = can_ram_arb;


	/*configure MASK ID register*/
	if (RxMsg->IDE == CAN_STANDARD_FRAME) {
		can_ram_mask = CAN_RAM_ID_MASK(RxMsg->ID_MASK << 18);
	} else {
		can_ram_mask = CAN_RAM_ID_MASK(RxMsg->ID_MASK);
	}

	if (RxMsg->IDE_Mask) {
		can_ram_mask |= CAN_BIT_RAM_IDE_MASK;
	}
	if (RxMsg->RTR_Mask) {
		can_ram_mask |= CAN_BIT_RAM_RTR_MASK;
	}

	CANx->CAN_RAM_MASK = can_ram_mask;

	/* Write RX setting info in register into the ram message buffer */
	can_ram_cmd |= CAN_BIT_RAM_START;
	CANx->CAN_RAM_CMD = can_ram_cmd;
	while (CANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/**
  * @brief    Read frame from message ram buffer.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  RxMsg Rx message pointer.
  */
void CAN_ReadMsg(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg)
{
	u32 can_ram_arb = 0;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs = 0;
	u32 can_ram_mask = 0;
	u32 i;

	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/* Enable Msg buf[x] access */
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | CAN_BIT_RAM_ACC_DATA_MASK);
	can_ram_cmd |= CAN_RAM_ACC_NUM(RxMsg->MsgBufferIdx);
	can_ram_cmd |= CAN_BIT_RAM_START;
	CANx->CAN_RAM_CMD = can_ram_cmd;
	/* Read frame into register from ram message buffer */
	while (CANx->CAN_RAM_CMD & CAN_BIT_RAM_START);

	/*read ARB register*/
	can_ram_arb = CANx->CAN_RAM_ARB;
	if (can_ram_arb & CAN_BIT_RAM_RTR) {
		RxMsg->RTR = CAN_REMOTE_FRAME;
	} else {
		RxMsg->RTR = CAN_DATA_FRAME;
	}
	if (can_ram_arb & CAN_BIT_RAM_IDE) {
		RxMsg->IDE = CAN_EXTEND_FRAME;
	} else {
		RxMsg->IDE = CAN_STANDARD_FRAME;
	}

	if (RxMsg->IDE == CAN_STANDARD_FRAME) {
		RxMsg->StdId = CAN_GET_RAM_ID(can_ram_arb) >> 18;
	} else {
		RxMsg->ExtId = CAN_GET_RAM_ID(can_ram_arb);
	}

	/*read CS register*/
	can_ram_cs = CANx->CAN_RAM_CS;
	RxMsg->RxTimStamp = CAN_GET_RAM_TIMESTAMP(can_ram_cs);
	if (can_ram_cs & CAN_BIT_RAM_LOST) {
		RxMsg->RxLost = 1;
	} else {
		RxMsg->RxLost = 0;
	}
	RxMsg->DLC = CAN_GET_RAM_DLC(can_ram_cs);

	/*read mask ID*/
	can_ram_mask = CANx->CAN_RAM_MASK;
	RxMsg->ID_MASK = CAN_GET_RAM_ID_MASK(can_ram_mask);
	RxMsg->IDE_Mask = (can_ram_mask & CAN_BIT_RAM_IDE_MASK);
	RxMsg->RTR_Mask = (can_ram_mask & CAN_BIT_RAM_RTR_MASK);

	/*configure DATA register: can2.0 8 bytes*/
	if (RxMsg->RTR == CAN_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			RxMsg->Data_32[i] = CANx->CAN_RAM_DATA_x[16 - i - 1];
		}
	}
}

/**
  * @brief    Send auto reply message.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  TxMsg Tx message pointer.
  */
void CAN_TxAutoReply(CAN_TypeDef *CANx, CAN_TxMsgTypeDef *TxMsg)
{
	u32 can_ram_arb;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs;
	u32 i;

	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(IS_CAN_FRAME_TYPE(TxMsg->RTR));
	assert_param(IS_CAN_ID_TYPE(TxMsg->IDE));
	assert_param(TxMsg->DLC <= 64);
	assert_param(TxMsg->MsgBufferIdx <= 15);

	/* Enable Msg buf[x] access */
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= CAN_RAM_ACC_NUM(TxMsg->MsgBufferIdx);
	CANx->CAN_RAM_CMD = can_ram_cmd;

	/*configure frame ARB register*/
	can_ram_arb = CANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= TxMsg->RTR;
	can_ram_arb |= TxMsg->IDE;

	if (TxMsg->IDE == CAN_STANDARD_FRAME) {
		can_ram_arb |= ((TxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= TxMsg->ExtId;
	}
	CANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = CANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_MASK_RAM_DLC | CAN_BIT_RAM_AUTOREPLY));

	can_ram_cs |= TxMsg->DLC;
	/*enable auto-reply*/
	can_ram_cs |= CAN_BIT_RAM_RXTX | CAN_BIT_RAM_AUTOREPLY;

	CANx->CAN_RAM_CS = can_ram_cs;

	/*configure DATA register: can2.0 8 bytes*/
	if (TxMsg->RTR == CAN_DATA_FRAME) {
		for (i = 0; i < 16; i++) {
			CANx->CAN_RAM_DATA_x[i] = TxMsg->Data_32[(16 - i - 1)];
		}
	}

	/* Write frame info in register into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	CANx->CAN_RAM_CMD = can_ram_cmd;
	while (CANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}

/**
  * @brief    Receive auto reply message.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  RxMsg Rx message pointer.
  */
void CAN_RxAutoReply(CAN_TypeDef *CANx, CAN_RxMsgTypeDef *RxMsg)
{
	u32 can_ram_arb;
	u32 can_ram_cmd = 0;
	u32 can_ram_cs;
	u32 can_ram_mask;

	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(IS_CAN_FRAME_TYPE(RxMsg->RTR));
	assert_param(IS_CAN_ID_TYPE(RxMsg->IDE));
	assert_param(RxMsg->MsgBufferIdx <= 15);

	/*configure cmd register, enable access*/
	can_ram_cmd |= (CAN_BIT_RAM_BUFFER_EN | CAN_BIT_RAM_ACC_ARB | CAN_BIT_RAM_ACC_CS | CAN_BIT_RAM_ACC_MASK | \
					CAN_BIT_RAM_ACC_DATA_MASK | CAN_BIT_RAM_DIR);
	can_ram_cmd |= CAN_RAM_ACC_NUM(RxMsg->MsgBufferIdx);

	/*configure frame ARB register*/
	can_ram_arb = CANx->CAN_RAM_ARB;
	can_ram_arb &= (~(CAN_BIT_RAM_RTR | CAN_BIT_RAM_IDE | CAN_MASK_RAM_ID));
	can_ram_arb |= RxMsg->RTR;
	can_ram_arb |= RxMsg->IDE;

	if (RxMsg->IDE == CAN_STANDARD_FRAME) {
		can_ram_arb |= ((RxMsg->StdId & 0x7ff) << 18);
	} else {
		can_ram_arb |= RxMsg->ExtId;
	}
	CANx->CAN_RAM_ARB = can_ram_arb;

	/*configure CS register*/
	can_ram_cs = CANx->CAN_RAM_CS;
	can_ram_cs &= (~(CAN_BIT_RAM_RXTX | CAN_BIT_RAM_AUTOREPLY));
	/*enable auto-reply*/
	can_ram_cs |= CAN_BIT_RAM_AUTOREPLY;

	CANx->CAN_RAM_CS = can_ram_cs;

	/*configure MASK ID register*/
	can_ram_mask = CANx->CAN_RAM_MASK;
	can_ram_mask &= (~CAN_MASK_RAM_ID_MASK | CAN_BIT_RAM_IDE_MASK | CAN_BIT_RAM_RTR_MASK);
	if (RxMsg->IDE == CAN_STANDARD_FRAME) {
		can_ram_mask = CAN_RAM_ID_MASK(RxMsg->ID_MASK << 18);
	} else {
		can_ram_mask = CAN_RAM_ID_MASK(RxMsg->ID_MASK);
	}

	if (RxMsg->IDE_Mask) {
		can_ram_mask |= CAN_BIT_RAM_IDE_MASK;
	}
	if (RxMsg->RTR_Mask) {
		can_ram_mask |= CAN_BIT_RAM_RTR_MASK;
	}
	CANx->CAN_RAM_MASK = can_ram_mask;

	/* Write RX setting into the ram message buffer*/
	can_ram_cmd |= CAN_BIT_RAM_START;
	CANx->CAN_RAM_CMD = can_ram_cmd;
	while (CANx->CAN_RAM_CMD & CAN_BIT_RAM_START);
}
/**
  * @}
  */

/** @defgroup CAN_Interrupt_status_functions CAN Interrupt Status Functions
  * @{
  */

/**
  * @brief  Enables or disables the specified CAN interrupts.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  CAN_IT Specifies the CAN interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg CAN_TX_INT
  *     @arg CAN_RX_INT
  *     @arg CAN_ERR_INT
  *     @arg CAN_WKUP_INT
  *     @arg CAN_BUSOFF_INT
  * @param  NewState New state of the specified CAN interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  */
void CAN_INTConfig(CAN_TypeDef *CANx, u32 CAN_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if (NewState != DISABLE) {
		/* Enable the selected CAN interrupts */
		CANx->CAN_INT_EN |= CAN_IT;
	} else {
		/* Disable the selected CAN interrupts */
		CANx->CAN_INT_EN &= (~CAN_IT);
	}
}

/**
  * @brief  Get CAN interrupt status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return Interrupt status
  */
u32 CAN_GetINTStatus(CAN_TypeDef *CANx)
{
	/*check the parameters*/
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return CANx->CAN_INT_FLAG;
}

/**
  * @brief  Clears the CAN interrupt pending bits.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  CAN_IT Specifies the interrupt to be cleared.
  */
void CAN_ClearINT(CAN_TypeDef *CANx, u32 CAN_IT)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(IS_CAN_CLEAR_IT(CAN_IT));

	/*clear the specified interrupt*/
	CANx->CAN_INT_FLAG = CAN_IT;
}

/**
  * @brief  Clears all CAN interrupt pending bits.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  */
void CAN_ClearAllINT(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	CANx->CAN_INT_FLAG = 0xFFFFFFFF;
}

/**
  * @brief  Get the CAN error status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return Error status
  */
u32 CAN_GetErrStatus(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return (CANx->CAN_ERR_STATUS);
}

/**
  * @brief  Clears the CAN error pending bits.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  ERR_STS Specifies the error status to be cleared.
  */
void CAN_ClearErrStatus(CAN_TypeDef *CANx, u32 ERR_STS)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	/*clear the specified error status*/
	CANx->CAN_ERR_STATUS = ERR_STS;
}

/**
  * @brief  Enables or disables the specified CAN Rx message buffer interrupts.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  BUF_IT Specifies the CAN interrupts sources to be enabled or disabled.
   * @param  NewState New state of the specified CAN interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  */
void CAN_RxMsgBufINTConfig(CAN_TypeDef *CANx, u32 BUF_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if (NewState != DISABLE) {
		/* Enable the selected CAN rx msg buffer interrupts */
		CANx->CAN_MB_RXINT_EN |= BUF_IT;
	} else {
		/* Disable the selected CAN rx msg buffer interrupts */
		CANx->CAN_MB_RXINT_EN &= (~BUF_IT);
	}
}

/**
  * @brief  Enables or disables the specified CAN Tx message buffer interrupts.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  BUF_IT Specifies the CAN interrupts sources to be enabled or disabled.
   * @param  NewState New state of the specified CAN interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  */
void CAN_TxMsgBufINTConfig(CAN_TypeDef *CANx, u32 BUF_IT, u32 NewState)
{
	/*check the parameters*/
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if (NewState != DISABLE) {
		/* Enable the selected CAN tx msg buffer interrupts */
		CANx->CAN_MB_TXINT_EN |= BUF_IT;
	} else {
		/* Disable the selected CAN tx msg buffer interrupts */
		CANx->CAN_MB_TXINT_EN &= (~BUF_IT);
	}
}

/**
  * @brief  Get tx message buffer error status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return Tx message error status
  */
u32 CAN_TxMsgBufErrGet(CAN_TypeDef *CANx)
{
	/*check the parameters*/
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return (CANx->CAN_TX_ERROR_FLAG);
}

/**
  * @brief  Clear tx message buffer error status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  ERR_FLAG The specified error flag.
  */
void CAN_TxMsgBufErrClear(CAN_TypeDef *CANx, u32 ERR_FLAG)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	/*clear the specified msg buffer error*/
	CANx->CAN_TX_ERROR_FLAG = ERR_FLAG;
}

/**
  * @brief  Get message buffer status register value.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgBufIdx Message buffer index.
  * @return Register value
  */
u32 CAN_MsgBufStatusRegGet(CAN_TypeDef *CANx, u32 MsgBufIdx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(MsgBufIdx <= 15);

	return (CANx->CAN_MBx_STS[MsgBufIdx]);
}

/**
  * @brief  Get message buffer status code.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgBufIdx Message buffer index.
  * @return Status, can be the following values:
  *         - CAN_FRAME_PENDING_TX
  *         - CAN_FRAME_FINISH_TX
  *         - CAN_FRAME_OVERWRITE_SEND_TX
  *         - CAN_FRAME_PENDING_RX
  *         - CAN_FRAME_FINISH_RX
  */
u32 CAN_MsgBufStatusGet(CAN_TypeDef *CANx, u32 MsgBufIdx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));
	assert_param(MsgBufIdx <= 15);

	return (CANx->CAN_MBx_STS[MsgBufIdx] & 0xf);
}

/**
  * @brief  Check message buffer available or not.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgBufIdx Message buffer index.
  * @return Status, can be the following values:
  *         - 1: available
  *         - 0: not available
  */
u32 CAN_CheckMsgBufAvailable(CAN_TypeDef *CANx, u32 MsgBufIdx)
{
	u32 Temp;

	assert_param(IS_CAN_ALL_PERIPH(CANx));

	Temp = (CANx->CAN_MBx_STS[MsgBufIdx] & 0xf);

	if ((Temp != CAN_FRAME_PENDING_TX) && (Temp != CAN_FRAME_PENDING_RX)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  Get CAN tx done status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return CAN TX done status bitmask, each bit represents one message buffer TX completion status.
  */
u32 CAN_TxDoneStatusGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return (CANx->CAN_TX_DONE);
}

/**
  * @brief  Get message buffer tx done status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgBufIdx Message buffer index.
  * @return Status, can be the following values:
  *         - 1: tx complete
  *         - 0: tx pending
  */
u32 CAN_MsgBufTxDoneStatusGet(CAN_TypeDef *CANx, u32 MsgBufIdx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if ((CANx->CAN_TX_DONE) & BIT(MsgBufIdx)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  Get CAN rx done status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return CAN RX done status bitmask, each bit represents one message buffer RX completion status.
  */
u32 CAN_RxDoneStatusGet(CAN_TypeDef *CANx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	return (CANx->CAN_RX_DONE);
}
/**
  * @brief  Get message buffer rx done status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgBufIdx Message buffer index.
  * @return Status, can be the following values:
  *         - 1: rx complete
  *         - 0: rx pending
  */
u32 CAN_MsgBufRxDoneStatusGet(CAN_TypeDef *CANx, u32 MsgBufIdx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	if ((CANx->CAN_RX_DONE) & BIT(MsgBufIdx)) {
		return 1;
	} else {
		return 0;
	}
}

/**
  * @brief  Clear CAN tx done status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  Status Clear status.
  */
void CAN_TxDoneStatusClear(CAN_TypeDef *CANx, u32 Status)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	/*clear the specified msg buffer tx done status*/
	CANx->CAN_TX_DONE = Status;
}

/**
  * @brief  Clear CAN tx done message buffer status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgBufIdx Message buffer index.
  */
void CAN_MsgBufTxDoneStatusClear(CAN_TypeDef *CANx, u32 MsgBufIdx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	/*clear the specified msg buffer tx done status*/
	CANx->CAN_TX_DONE = BIT(MsgBufIdx);
}

/**
  * @brief  Clear CAN rx done status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  Status Clear status.
  */
void CAN_RxDoneStatusClear(CAN_TypeDef *CANx, u32 Status)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	/*clear the specified msg buffer rx done status*/
	CANx->CAN_RX_DONE = Status;
}

/**
  * @brief  Clear CAN rx done message buffer status.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgBufIdx Message buffer index.
  */
void CAN_MsgBufRxDoneStatusClear(CAN_TypeDef *CANx, u32 MsgBufIdx)
{
	assert_param(IS_CAN_ALL_PERIPH(CANx));

	/*clear the specified msg buffer rx done status*/
	CANx->CAN_RX_DONE = BIT(MsgBufIdx);
}


/**
  * @brief    CAN TX message trigger ENABLE or DISABLE.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  State New state of the TX trigger. This parameter can be: ENABLE or DISABLE.
  */
void CAN_TxMsgTriggerCmd(CAN_TypeDef *CANx, u32 State)
{
	if (State == DISABLE) {
		CANx->CAN_MB_TRIGGER &= ~CAN_BIT_TX_TRIGGER_EN;
	} else {
		CANx->CAN_MB_TRIGGER |= CAN_BIT_TX_TRIGGER_EN;
	}
}

/**
  * @brief    CAN TX message trigger configure.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  CloseOffset Close offset.
  * @param  Begin Trigger begin.
  */
void CAN_TxMsgTriggerConfig(CAN_TypeDef *CANx, u32 CloseOffset, u32 Begin)
{
	u32 MbTrigger;

	MbTrigger = CANx->CAN_MB_TRIGGER;

	MbTrigger &= (~(CAN_MASK_TX_TRIGGER_CLOSE_OFFSET | CAN_MASK_TX_TRIGGER_BEGIN));

	MbTrigger |= (Begin | CAN_TX_TRIGGER_CLOSE_OFFSET(CloseOffset));

	CANx->CAN_MB_TRIGGER = MbTrigger;
}

/**
  * @brief    CAN control register configure.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgIdx Message buffer index.
  * @param  RxDmaEn RX DMA enable.
  * @param  DmaOffset DMA offset.
  */
void CAN_MsgCtrlRegConfig(CAN_TypeDef *CANx, u32 MsgIdx, u32 RxDmaEn, u32 DmaOffset)
{
	u32 MsgBufferCtrl;

	MsgBufferCtrl = CANx->CAN_MBx_CTRL[MsgIdx];

	MsgBufferCtrl &= (~(CAN_BIT_MSGx_RXDMA_EN | CAN_MASK_MSGx_BA));

	if (RxDmaEn == DISABLE) {
		MsgBufferCtrl &= (~CAN_BIT_MSGx_RXDMA_EN);
	} else {
		MsgBufferCtrl |= CAN_BIT_MSGx_RXDMA_EN;
	}

	MsgBufferCtrl |= (DmaOffset << 10);
	CANx->CAN_MBx_CTRL[MsgIdx] = MsgBufferCtrl;
}

/**
  * @brief  Configure CAN message buffer base address.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  MsgIdx Message buffer index.
  * @param  BaseAddr Message buffer base address.
  */
void CAN_MsgBaseAddrConfig(CAN_TypeDef *CANx, u32 MsgIdx, u32 BaseAddr)
{
	u32 MsgBufferCtrl;

	MsgBufferCtrl = CANx->CAN_MBx_CTRL[MsgIdx];

	MsgBufferCtrl &= ~(CAN_MASK_MSGx_BA);

	MsgBufferCtrl |= CAN_MSGx_BA(BaseAddr);

	CANx->CAN_MBx_CTRL[MsgIdx] = MsgBufferCtrl;
}

/**
  * @brief    CAN message buffer end address.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  EndAddr Message buffer end address.
  */
void CAN_MsgBaseAddrEndReg(CAN_TypeDef *CANx, u32 EndAddr)
{
	CANx->CAN_MB_BA_END &= (~(CAN_MASK_MSG_BA_END));

	CANx->CAN_MB_BA_END |= CAN_MSG_BA_END(EndAddr);
}

/**
  * @brief    Init and Enable CAN RX GDMA.
  * @param  Index 0 or 1.
  * @param  GDMA_InitStruct Pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData GDMA callback data.
  * @param  CallbackFunc GDMA callback function.
  * @param  pRxBuf Pointer to RX buffer.
  * @param  RxCount Number of bytes to receive.
  * @return Initialization result:
  *         - TRUE: DMA channel allocated and initialized successfully.
  *         - FALSE: No available DMA channel.
  */
bool CAN_RXGDMA_Init(
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
	GDMA_InitStruct->GDMA_SrcAddr = CAN_DEV_TABLE[Index].CANx->CAN_RX_DMA_DATA;
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
  * @brief    CAN DMA destination base address configure.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @param  Addr Base address.
  */
void CAN_RxDmaDestBaseAddrConfig(CAN_TypeDef *CANx, u32 Addr)
{
	CANx->CAN_RXDMA_CFG = Addr;
}

/**
  * @brief    CAN get RX DMA data.
  * @param  CANx Where CANx can be CAN0 or CAN1.
  * @return DMA data
  */
u32 CAN_GetRxDmaData(CAN_TypeDef *CANx)
{
	return CANx->CAN_RX_DMA_DATA;
}

/**
  * @brief  Configure the RAM mapping addresses for message buffers.
  *
  * @note   This function assigns physical memory addresses to the CAN message
  *         buffers. The array must contain one extra address for the end register.
  *
  * @param  CANx Pointer to CAN peripheral instance (e.g., CAN0).
  * @param  AddrList Pointer to an array containing buffer addresses.
  *                   Size must be at least (CAN_MESSAGE_BUFFER_SIZE + 1).
  */
void CAN_RamBufferMapConfig(CAN_TypeDef *CANx, uint32_t *AddrList)
{
	uint32_t i;

	/* Configure base address for each individual message buffer */
	for (i = 0; i < CAN_MESSAGE_BUFFER_SIZE; i++) {
		CAN_MsgBaseAddrConfig(CANx, i, AddrList[i]);
	}

	/* Configure the end address register (using the element after the last buffer) */
	CAN_MsgBaseAddrEndReg(CANx, AddrList[CAN_MESSAGE_BUFFER_SIZE]);
}

/**
  * @brief  Initialize the CAN core clock settings.
  * @note   Sets the source to XTAL and disables PLL dividers for direct clocking.
  */
void CAN_CoreClockSet(void)
{
	RCC_PeriphClockSourceSet(CAN, XTAL);
	RCC_PeriphClockDividerFENSet(USB_PLL_CAN, DISABLE);
	RCC_PeriphClockDividerFENSet(SYS_PLL_CAN, DISABLE);
}

/**
  * @brief  Get the current CAN core clock frequency.
  *
  * @param  Rate [Out] Pointer to a variable where the frequency (in Hz) will be stored.
  * @retval RTK_SUCCESS: Clock rate retrieved successfully.
  * @retval RTK_FAIL: Invalid parameter (NULL pointer).
  */
int CAN_CoreClockGet(uint32_t *Rate)
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
static int CAN_CalcTimeSegments(uint32_t TotalTq, uint32_t SamplePoint, CAN_BitTimingTypeDef *Result,
								const CAN_BitTimingTypeDef *Min, const CAN_BitTimingTypeDef *Max)
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
	tseg2 = CAN_CLAMP(tseg2, Min->PhaseSeg2, Max->PhaseSeg2);

	/* ----------------------------------------------------------- */
	/* 2. Calculate Tseg1 (PropSeg + PhaseSeg1)                    */
	/* Formula: TotalTq = SyncSeg(1) + Tseg1 + Tseg2               */
	/* ----------------------------------------------------------- */
	tseg1 = TotalTq - CAN_SYNC_SEG - tseg2;

	/* ----------------------------------------------------------- */
	/* 3. Validate and Adjust Tseg1                                */
	/* If Tseg1 is out of bounds, trade off Tseg2 (Sample Point)   */
	/* to satisfy Tseg1 constraints.                               */
	/* ----------------------------------------------------------- */
	if (tseg1 > tseg1_max) {
		/* Tseg1 is too large; clamp to max and increase Tseg2 */
		tseg1 = tseg1_max;
		tseg2 = TotalTq - CAN_SYNC_SEG - tseg1;

		/* Check if Tseg2 now exceeds its own max limit */
		if (tseg2 > Max->PhaseSeg2) {
			return -RTK_ERR_BADARG;
		}

	} else if (tseg1 < tseg1_min) {
		/* Tseg1 is too small; clamp to min and decrease Tseg2 */
		tseg1 = tseg1_min;
		tseg2 = TotalTq - CAN_SYNC_SEG - tseg1;

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
	Result->PropSeg = CAN_CLAMP(tseg1 / 2, Min->PropSeg, Max->PropSeg);
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
	sample_pnt_res = (CAN_SYNC_SEG + tseg1) * 1000 / TotalTq;

	/* Return the absolute error */
	return (sample_pnt_res > SamplePoint) ?
		   (sample_pnt_res - SamplePoint) : (SamplePoint - sample_pnt_res);
}

/**
 * @brief  [Internal] Get default sample point recommendation for a bitrate.
 *         Based on CiA (CAN in Automation) recommendations.
 *
 * @param  BitRate Target bitrate in bits per second.
 * @return Sample point in permille (e.g., 875 = 87.5%).
 */
static uint16_t CAN_GetDefaultSamplePoint(uint32_t BitRate)
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
 * @brief  Calculate optimal CAN Bit Timing parameters.
 *
 * @details
 *  This function iterates through possible prescaler values to find a configuration
 *  that minimizes the deviation from the desired sample point.
 *  The results (Prescaler, Segments, SJW) are written to the Result structure.
 *
 * @param  BitRate Target bitrate (e.g., 500000 for 500kbps).
 * @param  Result  [Out] Pointer to structure where timing params will be saved.
 *
 * @retval RTK_SUCCESS:      Valid timing configuration found.
 * @retval -RTK_ERR_BADARG:  Calculated parameters are invalid or out of range.
 * @retval RTK_FAIL:         System clock could not be retrieved.
 */
int CAN_CalcBitTiming(uint32_t BitRate, CAN_BitTimingTypeDef *Result)
{
	/* Hardware limits defined in header */
	const CAN_BitTimingTypeDef MinLimits = CAN_TIMING_MIN;
	const CAN_BitTimingTypeDef MaxLimits = CAN_TIMING_MAX;

	/* Max possible Total TQ to determine minimum prescaler */
	uint32_t TotalTq = CAN_SYNC_SEG + MaxLimits.PropSeg + MaxLimits.PhaseSeg1 + MaxLimits.PhaseSeg2;
	CAN_BitTimingTypeDef TempRes = {0};

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

	if (CAN_CoreClockGet(&CoreClock) != RTK_SUCCESS) {
		return RTK_FAIL;
	}

	/* Select default sample point if not provided */
	if (SamplePoint == 0U) {
		SamplePoint = CAN_GetDefaultSamplePoint(BitRate);
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
		Err = CAN_CalcTimeSegments(TotalTq, SamplePoint, &TempRes, &MinLimits, &MaxLimits);

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
		RTK_LOGW(TAG, "CAN BitRate warning: Sample Point Error %d/1000", ErrMin);
	}

	/* ----------------------------------------------------------- */
	/* Post-processing: Calculate SJW (Sync Jump Width)            */
	/* Standard Rule: SJW = min(PhaseSeg1, PhaseSeg2 / 2, 4)       */
	/* ----------------------------------------------------------- */
	Result->SJW = MIN(Result->PhaseSeg1, Result->PhaseSeg2 / 2);
	Result->SJW = CAN_CLAMP(Result->SJW, MinLimits.SJW, MaxLimits.SJW);

	/* Check if a valid solution was found */
	if (ErrMin == INT_MAX) {
		return -RTK_ERR_BADARG;
	}
	/* Optional: Log info if the result is within acceptable margin */
	else if (ErrMin <= CAN_SAMPLE_POINT_MARGIN) {
		RTK_LOGI(TAG, "CAN Config: Pre=%u, SJW=%u, Prop=%u, Ph1=%u, Ph2=%u (Err=%d)\n",
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

/**
  * @}
  */