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
#include "ir_learn.h"
#include "ir_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define IR_DEBUG  0
#define IR_IRQ_PRIORITY_TEST		6

rtos_sema_t IR_learn_sema = NULL;

IR_InitTypeDef IR_InitStruct;

/*ir learn data packet*/
T_IR_PACKET_TYPEDEF g_ir_learn_packet;

/**
  * @brief  receive and store data to IR_DataStruct
  * @param   No parameter.
  * @return  void
  */
void IR_learning_recv(void)
{
	uint32_t len = 0;
	uint32_t data;

	len = IR_GetRxDataLen(IR_DEV);
	while (len) {
		len--;
		data = IR_ReceiveData(IR_DEV);

		//check loop queue is full or not
		if (((g_ir_learn_packet.write_index + 1) & QUEUE_CAPABILITY) != g_ir_learn_packet.read_index) {
			/*store current timer value*/
			g_ir_learn_packet.queue[g_ir_learn_packet.write_index] = data;
			g_ir_learn_packet.write_index++;
			g_ir_learn_packet.write_index &= QUEUE_CAPABILITY;
		} else {
			/*loop queue overflow*/
			g_ir_learn_packet.over_flow = TRUE;
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
	//portBASE_TYPE taskWoken = pdFALSE;

	IntStatus = IR_GetINTStatus(IR_DEV);
	IntMask = IR_GetIMR(IR_DEV);

	if (IR_InitStruct.IR_Mode == IR_MODE_RX) {
		IR_MaskINTConfig(IR_DEV, IR_RX_INT_ALL_MASK, DISABLE);

		if (IntStatus & IR_BIT_RX_FIFO_FULL_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_FIFO_FULL_INT_CLR);
		}

		//receive by interrupt
		if (IntStatus & IR_BIT_RX_FIFO_LEVEL_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_FIFO_LEVEL_INT_CLR);
			IR_learning_recv();
		}

		if (IntStatus & IR_BIT_RX_CNT_OF_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_CNT_OF_INT_CLR);
		}

		if (IntStatus & IR_BIT_RX_FIFO_OF_INT_STATUS) {
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_FIFO_OF_INT_CLR);
		}

		if (IntStatus & IR_BIT_RX_CNT_THR_INT_STATUS) {
			//taskWoken = pdFALSE;
			IR_ClearINTPendingBit(IR_DEV, IR_BIT_RX_CNT_THR_INT_CLR);
			rtos_sema_give(IR_learn_sema);
			g_ir_learn_packet.data_receive_completed = TRUE;
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
  * @brief  learning a specific ir waveform frequency.
  * @param  p_ir_packet point to ir packet struct.
  * @return The decoded waveform frequency of ir learned waveform.
  * @retval float
  */
uint32_t IR_decode_frequency(T_IR_PACKET_TYPEDEF *p_ir_packet)
{
	uint8_t i = 0;
	uint32_t frequency = 0;
	uint32_t frequency_total = 0;

#if CALCULATE_FREQUENCY
	/* frequency calculation*/
	for (i = 0; i < (p_ir_packet->frequency_num); i++) {
		frequency_total += p_ir_packet->wave_frequency[i];
	}
#else
	for (i = 0; i < (p_ir_packet->frequency_num); i++) {
		frequency_total += (p_ir_packet->high_wave_count[i] * IR_FREQ) / p_ir_packet->high_wave_time[i];
	}
#endif

	frequency = frequency_total / p_ir_packet->frequency_num;

#if CORRECT_FREQ_BY_SOFTWARE
	/* frequency correction by software */
	if (ABS_FREQUENCY(frequency, 38500) <= 1000) {
		frequency = 38000;
	} else if (ABS_FREQUENCY(frequency, 36500) <= 1000) {
		frequency = 36000;
	} else if (ABS_FREQUENCY(frequency, 40500) <= 1000) {
		frequency = 40000;
	} else if (ABS_FREQUENCY(frequency, 56500) <= 1000) {
		frequency = 56000;
	} else {
		printf("rx learn frequency result:\n");
	}
#endif

#if (IR_DEBUG == 1)
	printf("[ir_decode_frequency]frequency = %d Hz \n", (uint32_t)frequency);
#endif
	return frequency;
}


/**
  * @brief  decode the received ir packet
  * @param  p_ir_packet point to ir packet struct.
  * @return result of decoding
  * @retval IR_RET_OK   decode success
  * @retval other   decode failed
  */
uint8_t IR_decode_packet(T_IR_PACKET_TYPEDEF *p_ir_packet)
{
	uint32_t current_data = 0;
	uint32_t current_time = 0;
	uint32_t idx = 0;

	printf("read index:%d, write index:%d\n", p_ir_packet->read_index, p_ir_packet->write_index);

	/* Begin to learn waveform */
	while (p_ir_packet->read_index != p_ir_packet->write_index) {
		current_data = p_ir_packet->queue[p_ir_packet->read_index++];

		printf("data of FIFO:0x%lx\n", current_data);

		/* get carrier period high level count */
		if ((current_data & IR_BIT_RX_LEVEL) == BIT31) {
			p_ir_packet->square_wave_num++;
		}
		current_time = IR_GET_RX_CNT(current_data);

		p_ir_packet->read_index &= QUEUE_CAPABILITY;

		/* Record total time of high wave ,may contain several cycles of carrier frequency*/
		if ((current_time > TIME_LOWEST_VALUE) && (current_time < TIME_HIGHEST_VALUE)) {
			p_ir_packet->detect_time[idx] += current_time;
			p_ir_packet->is_high_level = TRUE;
			//p_ir_packet->square_wave_num++;

			/* Record IR data number */
			p_ir_packet->detect_time_num = idx + 1;
		} else if (current_time > TIME_LOW_STOP_VALUE) {
			/* Wave from high to low */
			if (p_ir_packet->is_high_level == TRUE) {
				/* Calculate IR frequency based on time value of high waveform */
#if CALCULATE_FREQUENCY
				p_ir_packet->wave_frequency[p_ir_packet->frequency_num] = (p_ir_packet->square_wave_num * IR_FREQ) /
						p_ir_packet->detect_time[idx];
#else
				p_ir_packet->high_wave_count[p_ir_packet->frequency_num] = p_ir_packet->square_wave_num;
				p_ir_packet->high_wave_time[p_ir_packet->frequency_num] = p_ir_packet->detect_time[idx];
#endif
				p_ir_packet->frequency_num++;
				p_ir_packet->square_wave_num = 0;

				/* pointer to next buffer to store data */
				idx++;

				/* Record IR data number */
				p_ir_packet->detect_time_num = idx + 1;
				/* Store value of low waveform */
				p_ir_packet->detect_time[idx++] = current_time;

				p_ir_packet->is_high_level = FALSE;
			} else {
				printf("[ir_decode_packet] Pelase consider resetting the trigger threshold!\n");
				return IR_LEARN_ERR;
			}
		}
	}

	printf("idx:%lu\n", idx);
	printf("the number of IR data:%d\n", p_ir_packet->detect_time_num);
	/*to reserve the space of learn data header to be stored in flash*/
	if (idx >= LEARN_WAVE_MAX_SIZE - 4) {
		return IR_LEARN_ERR;
	} else if (idx <= LEARN_WAVE_MIN_SIZE) {
		return IR_LEARN_ERR;
	}

	return IR_RET_OK;
}


/**
  * @brief  capture IR wave and save to flash ,
  *            address is key_index*LEARN_WAVE_MAX_SIZE.
  * @param  key_index   key index for ir learn
  * @return none
  */
void IR_learning_wave_capture(uint32_t *rev_buf)
{
	uint32_t frequency = 0;
	uint32_t index = 0;
	uint16_t return_flag = 0;
	//uint32_t rev_buf[LEARN_WAVE_MAX_SIZE] = {0};
	uint8_t ret = 0;

	/*start capture the IR wave*/
	ret = IR_decode_packet(&g_ir_learn_packet);
	if (IR_LEARN_ERR == ret) {
		printf("[ir_learn_wave_capture]IR wave err, wait wave!!!\n");
		return;
	}

	/* get frequency data */
	frequency = IR_decode_frequency(&g_ir_learn_packet);
	//printf("frequency:%d\n", frequency_float);

	rev_buf[0] = frequency;
	rev_buf[1] = g_ir_learn_packet.detect_time_num;

	/* get IR wave data through g_ir_learn_packet struct */
	for (index = 0; index < g_ir_learn_packet.detect_time_num; index++) {
		if (index < LEARN_WAVE_MAX_SIZE - 3) {
			if (index % 2 == 0) {
				rev_buf[index + 2] = ((uint32_t)(g_ir_learn_packet.detect_time[index] * frequency /
												 IR_FREQ)) | 0x80000000;
#if (IR_DEBUG == 1)
				printf("[ir_learn_wave_capture]data %d = 0x%x\n", index, rev_buf[index + 2]);
#endif
			} else {
				rev_buf[index + 2] = (uint32_t)(g_ir_learn_packet.detect_time[index] * frequency /
												IR_FREQ);
#if (IR_DEBUG == 1)
				printf("[ir_learn_wave_capture]data %d = 0x%x\n", index, rev_buf[index + 2]);
#endif
			}
		}
	}

	/* | -keyIndex- | -Frequency- | -DectTimeNum- | --wave data-- | */
#ifdef AMEBASMART_TODO
	//storage related data to flash

#endif
	printf("[ir_learn_wave_capture]save data flag: %d, 0 success, or failed, frequency: %lu. \n", return_flag, rev_buf[0]);
}

/**
  * @brief  Initialize IR peripheral.
  * @param   No parameter.
  * @return  void
  */
void IR_learning_driver_init(void)
{
	/* Initialize IR */
	IR_StructInit(&IR_InitStruct);
	IR_InitStruct.IR_Clock = IR_CLOCK_HZ;
	IR_InitStruct.IR_Mode = IR_MODE_RX;
	IR_InitStruct.IR_Freq = IR_FREQ;
	IR_InitStruct.IR_RxFIFOThrLevel = 5;
	IR_InitStruct.IR_RxCntThrType = IR_RX_COUNT_LOW_LEVEL;
	IR_InitStruct.IR_RxCntThr = 0xa1644;
	IR_Init(IR_DEV, &IR_InitStruct);

	/* Configure interrrupt if need */
	IR_INTConfig(IR_DEV, IR_RX_INT_ALL_EN, ENABLE);
	InterruptRegister((IRQ_FUN)IR_irq_handler, IR_IRQ, (u32)NULL, IR_IRQ_PRIORITY_TEST);
	InterruptEn(IR_IRQ, IR_IRQ_PRIORITY_TEST);
}


/**
  * @brief  Initializes ir learning data packet struct.
  * @param  p_ir_packet point to ir packet struct.
  * @return result of init
  * @retval true    init success
  * @retval false   init failed
  */
bool IR_learning_data_init(T_IR_PACKET_TYPEDEF *p_ir_packet)
{
	uint8_t i = 0;

	p_ir_packet->read_index = 0;
	p_ir_packet->write_index = 0;

	if (p_ir_packet->queue == NULL) {
		//p_ir_packet->queue  = malloc(LOOP_QUEUE_MAX_SIZE * sizeof(uint32_t));
		p_ir_packet->queue = rtos_mem_malloc(LOOP_QUEUE_MAX_SIZE * sizeof(uint32_t));
	}
	if (p_ir_packet->queue == NULL) {
		printf("[ir_learn_data_init]Memory allocation failed.\n");
		return FALSE;
	}

	p_ir_packet->square_wave_num  = 0;
	p_ir_packet->is_high_level = FALSE;
	p_ir_packet->over_flow = FALSE;
	p_ir_packet->detect_time_num = 0;
	p_ir_packet->frequency_num = 0;

	for (i = 0; i < LEARN_WAVE_MAX_SIZE; i++) {
		p_ir_packet->detect_time[i] = 0;
	}

	for (i = 0; i < LEARN_WAVE_MAX_SIZE / 2 + 1; i++) {
#if CALCULATE_FREQUENCY
		p_ir_packet->wave_frequency[i] = 0;
#else
		p_ir_packet->high_wave_time[i] = 0;
		p_ir_packet->high_wave_count[i] = 0;
#endif
	}

	return TRUE;
}


/**
  * @brief  IR learning
  * @param   No parameter.
  * @return  void
  */
void IR_learning_thread(void)
{
	uint32_t recv_buf[LEARN_WAVE_MAX_SIZE] = {0};
	int i;

	IR_Cmd(IR_DEV, IR_InitStruct.IR_Mode, DISABLE);

	/* add IR Learn Init here*/
	if (IR_learning_data_init(&g_ir_learn_packet) == FALSE) {
		printf("[ir_learn_begin]IR Learn ERR: data init failed");
		return ;
	}
	IR_learning_driver_init();

	IR_Cmd(IR_DEV, IR_MODE_RX, ENABLE);

	do {
		g_ir_learn_packet.read_index = 0;
		g_ir_learn_packet.write_index = 0;
		g_ir_learn_packet.square_wave_num  = 0;
		g_ir_learn_packet.over_flow       = FALSE;
		g_ir_learn_packet.detect_time_num  = 0;
		g_ir_learn_packet.frequency_num   = 0;

		for (i = 0; i < LEARN_WAVE_MAX_SIZE; i++) {
			g_ir_learn_packet.detect_time[i] = 0;
		}

		for (i = 0; i < LEARN_WAVE_MAX_SIZE / 2 + 1; i++) {
#if CALCULATE_FREQUENCY
			g_ir_learn_packet.wave_frequency[i] = 0;
#else
			g_ir_learn_packet.high_wave_time[i] = 0;
			g_ir_learn_packet.high_wave_count[i] = 0;
#endif
		}

		IR_ClearRxFIFO(IR_DEV);

		if (rtos_sema_take(IR_learn_sema, RMC_IR_LEARN_TIMEOUT_MS) == SUCCESS) {
			if (g_ir_learn_packet.data_receive_completed) {
				//TODO: receive complete
				IR_learning_wave_capture((uint32_t *)&recv_buf);
				printf("the number of IR data:0x%lx\n", recv_buf[1]);
			} else {
				printf("exit key pressing IR learning exit \n");
			}
		} else {
			printf("IR learning timeout\n");
		}
	} while (1);

}

int main(void)
{
	Board_IR_Init(PINMUX_S1);

	if (IR_learn_sema == NULL) {
		rtos_sema_create_binary(&IR_learn_sema);
	}

	if (SUCCESS != rtos_task_create(NULL, (const char *const)"IR_learning_THREAD", (rtos_task_t)IR_learning_thread,
									NULL, 3072, 5)) {
		printf("create IR RX thread error\n");

		rtos_sema_delete(IR_learn_sema);
		IR_learn_sema = NULL;
	}

	rtos_sched_start();
	return 0;
}
