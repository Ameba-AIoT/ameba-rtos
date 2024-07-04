/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "ir_nec_protocol.h"
#include "ir_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define MAX_TX_SIZE         (NEC_LENGTH - 1)
#define IR_IRQ_PRIORITY_TEST     6

uint32_t buf_index = 0;
uint8_t tx_done = _FALSE;
uint8_t ir_code[2] = {0, 0};

rtos_sema_t IR_sema = NULL;

IR_InitTypeDef IR_InitStruct;
/* struct which stores encoded data */
IR_DataTypeDef IR_DataStruct;

void IR_tx_done_callback(void)
{
	uint8_t data[4];
	buf_index = 0;

	ir_code[0]++;
	if (ir_code[0]++ >= 255) {
		ir_code[1]++;
		if (ir_code[1] >= 255) {
			tx_done = _TRUE;
		}
	}

	printf("TX 0x%02x%02x\n", ir_code[1], ir_code[0]);

	/* Encode by NEC protocol */
	data[0] = ir_code[0];
	data[1] = ~ir_code[0];
	data[2] = ir_code[1];
	data[3] = ~ir_code[1];
	IR_NECEncode(IR_InitStruct.IR_Freq, (uint8_t *)&data, &IR_DataStruct);

	IR_ClearTxFIFO(IR_DEV);

	IR_SendBuf(IR_DEV, IR_DataStruct.irBuf, IR_TX_FIFO_SIZE, FALSE);
	buf_index += IR_TX_FIFO_SIZE;
	IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, ENABLE);

}

u32 IR_irq_handler(void *data)
{
	UNUSED(data);
	u32 IntStatus;
	u32 tx_thr = IR_InitStruct.IR_TxFIFOThrLevel;

	IntStatus = IR_GetINTStatus(IR_DEV);
	//u32 IntMask = IR_GetIMR(IR_DEV);

	if (IR_InitStruct.IR_Mode == IR_MODE_TX) {
		//IR_MaskINTConfig(IR_DEV, IR_TX_INT_ALL_MASK, DISABLE);

		if (IntStatus & IR_BIT_TX_FIFO_LEVEL_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_TX_FIFO_LEVEL_INT_CLR);
			// printf("IR_TX_FIFO_LEVEL_INT %x\n", IntStatus);

			if (buf_index + tx_thr < MAX_TX_SIZE) {
				IR_SendBuf(IR_DEV, IR_DataStruct.irBuf + buf_index, tx_thr, DISABLE);
				buf_index += tx_thr;
			} else {
				IR_SendBuf(IR_DEV, IR_DataStruct.irBuf + buf_index, MAX_TX_SIZE - buf_index, ENABLE);
				buf_index = MAX_TX_SIZE;
			}

			// printf("%d\n", buf_index);

			if (buf_index == MAX_TX_SIZE) {
				/* make sure IR has enough time to send TX FIFO data */
				// DelayMs(50);
				while (IR_FSMRunning(IR_DEV) && !(IR_GetINTStatus(IR_DEV) & IR_BIT_TX_FIFO_EMPTY));
				IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, DISABLE);
				DelayMs(80);
				if (tx_done != _TRUE) {
					IR_tx_done_callback();
				}
			}
		}

		if (IntStatus & IR_BIT_TX_FIFO_EMPTY_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_TX_FIFO_EMPTY_INT_CLR);
			printf("IR_TX_FIFO_EMPTY_INT %lx\n", IntStatus);
		}

		if (IntStatus & IR_BIT_TX_FIFO_OVER_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_TX_FIFO_OVER_INT_CLR);
			printf("IR_TX_FIFO_OVER_INT %lx\n", IntStatus);
		}
	}

	return 0;
}

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param  group: IR pinmux selection.
  * @return  void
  */
void Board_IR_Init(u8 group)
{
	/* enable peripheral clock */
	RCC_PeriphClockCmd(APBPeriph_IRDA, APBPeriph_IRDA_CLOCK, ENABLE);

	/*there are five IR tx and four IR rx of pinmux and pad settings*/
	switch (group) {
	case PINMUX_S0:
		Pinmux_Swdoff();
		Pinmux_Config(IR_PINMUX_S0_TX, PINMUX_FUNCTION_IR_TX);
		Pinmux_Config(IR_PINMUX_S0_RX, PINMUX_FUNCTION_IR_RX);
		//printf("pinmux PA_3 TX; PA_4 RX\n");
		printf("pinmux S0\n");
		break;
	case PINMUX_S1:
		Pinmux_Config(IR_PINMUX_S1_TX, PINMUX_FUNCTION_IR_TX);
		Pinmux_Config(IR_PINMUX_S1_RX, PINMUX_FUNCTION_IR_RX);
		//printf("pinmux PB_11 TX; PB_10 RX\n");
		printf("pinmux S1\n");
		break;
	case PINMUX_S2:
		Pinmux_Config(IR_PINMUX_S2_TX, PINMUX_FUNCTION_IR_TX);
		Pinmux_Config(IR_PINMUX_S2_RX, PINMUX_FUNCTION_IR_RX);
		//printf("pinmux PB_21 TX; PB_22 RX\n");
		printf("pinmux S2\n");
		break;
	case PINMUX_S3:
		Pinmux_Config(IR_PINMUX_S3_TX, PINMUX_FUNCTION_IR_TX);
		Pinmux_Config(IR_PINMUX_S3_RX, PINMUX_FUNCTION_IR_RX);
		//printf("pinmux PB_25 TX; PB_26 RX\n");
		printf("pinmux S3\n");
		break;
	case PINMUX_S4:
		Pinmux_Config(IR_PINMUX_S4_TX, PINMUX_FUNCTION_IR_TX);
		//printf("pinmux PA_17 TX\n");
		printf("pinmux S4\n");
		break;
	default:
		printf("pinmux group value from 0 to 4\n");
	}

}

/**
  * @brief  TX with NEC protocol
  * @param   No parameter.
  * @return  void
  */
void IR_TX_thread(void)
{
	uint8_t data[4]; //Buffer to store address and command
	uint8_t tx_thres = 15;

	/* Register interrupt Callback function */
	InterruptRegister((IRQ_FUN)IR_irq_handler, IR_IRQ, (u32)NULL, IR_IRQ_PRIORITY_TEST);
	InterruptEn(IR_IRQ, IR_IRQ_PRIORITY_TEST);

	/* disable IR device */
	IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, DISABLE);

	/* Initialize IR */
	IR_StructInit(&IR_InitStruct);
	IR_InitStruct.IR_Clock = IR_CLOCK_HZ;
	IR_InitStruct.IR_Mode = IR_MODE_TX;
	IR_InitStruct.IR_Freq = 38000;
	IR_InitStruct.IR_TxFIFOThrLevel = tx_thres;
	IR_Init(IR_DEV, &IR_InitStruct);

	/* Configure interrrupt if need */
	//IR_INTConfig(IR_DEV, IR_TX_INT_ALL_EN, DISABLE);
	IR_INTConfig(IR_DEV, IR_BIT_TX_FIFO_LEVEL_INT_EN, ENABLE);
	IR_MaskINTConfig(IR_DEV, IR_BIT_TX_FIFO_LEVEL_INT_MASK, DISABLE);
	IR_ClearINTPendingBit(IR_DEV, IR_BIT_TX_FIFO_LEVEL_INT_CLR);

	printf("start TX \n 0x%02x%02x\n", ir_code[1], ir_code[0]);

	/* Encode by NEC protocol */
	data[0] = ir_code[0];
	data[1] = ~ir_code[0];
	data[2] = ir_code[1];
	data[3] = ~ir_code[1];
	IR_NECEncode(IR_InitStruct.IR_Freq, (uint8_t *)&data, &IR_DataStruct);
	//printf(">> tx len:%d\n", IR_DataStruct.bufLen);

	buf_index = 0;
	IR_ClearTxFIFO(IR_DEV);
	IR_SendBuf(IR_DEV, IR_DataStruct.irBuf, IR_TX_FIFO_SIZE, FALSE);
	buf_index += IR_TX_FIFO_SIZE;

	/* enable IR device */
	IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, ENABLE);

	rtos_task_delete(NULL);
}

int main(void)
{
	Board_IR_Init(PINMUX_S1);

	if (SUCCESS != rtos_task_create(NULL, (const char *const)"IR_TX_THREAD", (rtos_task_t)IR_TX_thread, NULL, 1024 * 3, 5)) {
		printf("create IR TX thread error\n");
	}

	rtos_sched_start();
	return 0;
}
