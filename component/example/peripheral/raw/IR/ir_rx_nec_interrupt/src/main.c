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

rtos_sema_t IR_Recv_end_sema = NULL;
IR_InitTypeDef IR_InitStruct;
/* Buffer which store encoded data */
IR_DataTypeDef IR_DataStruct;

/* the maximum number of carrier
 * if the counter is larger than MAX_CARRIER, we consider it is a non-carrier
 * this value must be larger enough but less than log0/1 non-carrier time */
#define MAX_CARRIER 2000
#define IR_IRQ_PRIORITY_TEST		6

/**
  * @brief  receive and store data to IR_DataStruct
  * @param   No parameter.
  * @return  void
  */
void IR_RX_recv(void)
{
	uint16_t len = 0;
	static u8 continue_filter = 0;
	u32 data;
	u32 duration;

	len = IR_GetRxDataLen(IR_DEV);
	while (len) {
		len--;
		data = IR_ReceiveData(IR_DEV);
		duration = data & 0x7fffffff;

		if (IR_DataStruct.bufLen > 67) {//67
			printf("the waveform is inverse, you should set: \n");
			printf("IR_InitStruct.IR_RxCntThrType = IR_RX_COUNT_HIGH_LEVEL \n");
			printf("#define INVERSE_DATA in Ir_nec_protocol.h \n\n");
			break;
		}

		if ((IR_DataStruct.bufLen == 0) && (IR_DataStruct.irBuf[0] == 0)) {
			// printf("data0x%x, duration0x%x \r\n", data, duration);
			IR_DataStruct.irBuf[0] = data;
		} else {
			if (duration > MAX_CARRIER) {
				IR_DataStruct.irBuf[++IR_DataStruct.bufLen] = data;
			} else if (continue_filter == 0) {
				IR_DataStruct.irBuf[++IR_DataStruct.bufLen] = data;
			} else {
				IR_DataStruct.irBuf[IR_DataStruct.bufLen] += duration;
			}
		}

		if (duration > MAX_CARRIER) {
			continue_filter = 0;
		} else {
			continue_filter = 1;
		}

		len = IR_GetRxDataLen(IR_DEV);
	}

}

/**
  * @brief  interrupt handler function
  * @param   No parameter.
  * @return  void
  */
void IR_irq_handler(void)
{
	u32 IntStatus, IntMask;

	IntStatus = IR_GetINTStatus(IR_DEV);
	IntMask = IR_GetIMR(IR_DEV);

	if (IR_InitStruct.IR_Mode == IR_MODE_RX) {
		IR_MaskINTConfig(IR_DEV, IR_RX_INT_ALL_MASK, DISABLE);

		if (IntStatus & IR_BIT_RX_FIFO_FULL_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_FIFO_FULL_INT_CLR);
		}

		if (IntStatus & IR_BIT_RX_FIFO_LEVEL_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_FIFO_LEVEL_INT_CLR);
			IR_RX_recv();
		}

		if (IntStatus & IR_BIT_RX_CNT_OF_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_CNT_OF_INT_CLR);
		}

		if (IntStatus & IR_BIT_RX_FIFO_OF_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_FIFO_OF_INT_CLR);
		}

		if (IntStatus & IR_BIT_RX_CNT_THR_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_CNT_THR_INT_CLR);
			rtos_sema_give(IR_Recv_end_sema);
		}

		if (IntStatus & IR_BIT_RX_FIFO_ERROR_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_FIFO_ERROR_INT_CLR);
		}

	}

	IR_MaskINTConfig(IR_DEV, IntMask, ENABLE);
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
  * @brief  Initialize IR peripheral.
  * @param   No parameter.
  * @return  void
  */
void Driver_IR_Init(void)
{
	/* Initialize IR */
	IR_StructInit(&IR_InitStruct);
	IR_InitStruct.IR_Clock = IR_CLOCK_HZ;
	IR_InitStruct.IR_Mode = IR_MODE_RX;
	IR_InitStruct.IR_Freq = 10000000; //sample frequency :Hz
	IR_InitStruct.IR_RxFIFOThrLevel = 5;
	IR_InitStruct.IR_RxCntThrType = IR_RX_COUNT_LOW_LEVEL;
	IR_InitStruct.IR_RxCntThr = 0xa1644;//66ms
	IR_Init(IR_DEV, &IR_InitStruct);

	/* Configure interrrupt if need */
	IR_INTConfig(IR_DEV, IR_RX_INT_ALL_EN, ENABLE);
	//register interrupt callback function
	InterruptRegister((IRQ_FUN) IR_irq_handler, IR_IRQ, (u32)NULL, IR_IRQ_PRIORITY_TEST);
	InterruptEn(IR_IRQ, IR_IRQ_PRIORITY_TEST);

}

/**
  * @brief  RX with NEC protocol
  * @param   No parameter.
  * @return  void
  */
void IR_RX_thread(void)
{
	IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, DISABLE);
	Driver_IR_Init();
	IR_Cmd(IR_DEV, IR_MODE_RX, ENABLE);
	uint8_t ir_code[2] = {0, 0};
	uint8_t data[4] = {0, 0, 0, 0};

	do {
		IR_DataStruct.bufLen = 0;
		IR_DataStruct.codeLen = 0;
		IR_DataStruct.irBuf[0] = 0;
		IR_ClearRxFIFO(IR_DEV);
		if (rtos_sema_take(IR_Recv_end_sema, 10 * 1000) == SUCCESS) {
			//rx end
			u32 result;
			result = IR_NECDecode(IR_InitStruct.IR_Freq, (uint8_t *)&data, &IR_DataStruct);
			ir_code[0] = data[0];
			ir_code[1] = data[2];
			printf("result %lu RX 0x%02x%02x\n", result, ir_code[1], ir_code[0]);
		} else {
			printf("rx timeout\n");
		}
	} while (1);

}

int main(void)
{
	Board_IR_Init(PINMUX_S1);

	if (IR_Recv_end_sema == NULL) {
		rtos_sema_create_binary(&IR_Recv_end_sema);
	}

	if (SUCCESS != rtos_task_create(NULL, (const char *const)"IR_RX_THREAD", (rtos_task_t)IR_RX_thread, NULL, 256 * 16, 5)) {
		printf("create IR RX thread error\n");
	}

	rtos_sched_start();
	return 0;
}
