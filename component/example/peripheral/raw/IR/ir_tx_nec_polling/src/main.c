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

IR_InitTypeDef IR_InitStruct;
#define IR_LOOP_TEST	1
#define IR_TEST_LEARNNING	0
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
	/* struct which stores encoded data */
	IR_DataTypeDef IR_DataStruct;
	/*Buffer which stores address and command*/
#if defined(IR_LOOP_TEST) && (IR_LOOP_TEST == 1)
	uint8_t ir_code[2] = {0, 0};
#else
	uint8_t ir_code[2] = {0x12, 0x34};
#endif
	uint8_t data[4];

	uint32_t tx_count;
	uint8_t tx_thres = 15;

	IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, DISABLE);

	/* Initialize IR */
	IR_StructInit(&IR_InitStruct);
	IR_InitStruct.IR_Clock = IR_CLOCK_HZ;
	IR_InitStruct.IR_Mode = IR_MODE_TX;
	IR_InitStruct.IR_Freq = 38000;
	IR_Init(IR_DEV, &IR_InitStruct);

	/* Configure interrrupt if need */

	do {
		tx_count = 0;
#if defined(IR_LOOP_TEST) && (IR_LOOP_TEST == 1)
		if (ir_code[0]++ >= 255) {
			ir_code[1]++;
		}
#endif
		printf("TX BYTE1%2x BYTE0%2x\n", ir_code[1], ir_code[0]);

		/* Encode by NEC protocol */
		data[0] = ir_code[0];
		data[1] = ~ir_code[0];
		data[2] = ir_code[1];
		data[3] = ~ir_code[1];
		IR_NECEncode(IR_InitStruct.IR_Freq, (uint8_t *)&data, &IR_DataStruct);

		IR_SendBuf(IR_DEV, IR_DataStruct.irBuf, IR_TX_FIFO_SIZE, FALSE);
		IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, ENABLE);

		tx_count += IR_TX_FIFO_SIZE;
		while (IR_DataStruct.bufLen - tx_count > 0) {
			while (IR_GetTxFIFOFreeLen(IR_DEV) < tx_thres) {
				rtos_task_yield();
			}

			if (IR_DataStruct.bufLen - tx_count > tx_thres) {
				IR_SendBuf(IR_DEV, IR_DataStruct.irBuf + tx_count, tx_thres, FALSE);
				tx_count += tx_thres;
			} else {
				IR_SendBuf(IR_DEV, IR_DataStruct.irBuf + tx_count, IR_DataStruct.bufLen - tx_count, TRUE);
				tx_count = IR_DataStruct.bufLen;
			}
		}

		rtos_time_delay_ms(1000); //delay 1s
		IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, DISABLE);

#if defined(IR_TEST_LEARNNING) && (IR_TEST_LEARNNING == 1)
		break;
#endif

	} while (1);

	rtos_task_delete(NULL);
}

int main(void)
{
	Board_IR_Init(PINMUX_S1);

	if (SUCCESS != rtos_task_create(NULL, (const char *const)"IR_TX_THREAD", (rtos_task_t)IR_TX_thread, NULL, 1024 * 3, 5)) {
		printf("creat IR TX thread error\n");
	}

	rtos_sched_start();
	return 0;
}
