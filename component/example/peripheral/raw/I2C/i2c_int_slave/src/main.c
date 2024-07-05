/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "i2c_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

typedef struct i2c_m {
	uint32_t i2c_idx;
	I2C_TypeDef *I2Cx;
} i2c_t;

typedef struct {
	i2c_t I2Cint;
	u8 *pbuf;
	u32 datalength;
} i2c_ts;


#define I2C_ID 0
static const char *TAG = "I2C";

#define I2C_SLAVE_ADDR0    0x23

#define I2C_DATA_LENGTH         127
uint8_t	i2cdatasrc[I2C_DATA_LENGTH];
uint8_t	i2cdatadst[I2C_DATA_LENGTH];
I2C_InitTypeDef I2CInitData[2];

i2c_ts i2cslave;
u32 length = 127;
u32 rx_done;
u32 tx_done;
void i2c_rx_check(void)
{
	int     i2clocalcnt;
	int     result = 0;

	RTK_LOGI(TAG, "check slave source data>>>\n");
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 1) {
		RTK_LOGI(TAG, "%s%02X%s", i2clocalcnt % 16 == 0 ? "\n     " : " ",
				 i2cdatasrc[i2clocalcnt], i2clocalcnt == I2C_DATA_LENGTH - 1 ? "\n" : "");
	}

	RTK_LOGI(TAG, "check slave received data>>>\n");
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH - 1; i2clocalcnt += 1) {

		RTK_LOGI(TAG, "%s%02X%s", i2clocalcnt % 16 == 0 ? "\n	  " : " ",
				 i2cdatadst[i2clocalcnt], i2clocalcnt == I2C_DATA_LENGTH - 1 ? "\n" : "");
	}

	// verify result
	result = 1;
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatadst[i2clocalcnt] != i2cdatasrc[i2clocalcnt]) {
			result = 0;
			break;
		}
	}
	RTK_LOGI(TAG, "SLAVE receive: Result is %s\r\n", (result) ? "success" : "fail");
}

static void I2CISRHandleTxEmpty(IN i2c_ts *obj)
{
	u8 I2CStop = 0;

	/* To check I2C master TX data length. If all the data are transmitted,
	mask all the interrupts and invoke the user callback */
	if (!obj->datalength) {
		while (0 == I2C_CheckFlagState(obj->I2Cint.I2Cx, I2C_BIT_TFE));

		/* I2C Disable TX Related Interrupts */
		I2C_INTConfig(obj->I2Cint.I2Cx, (I2C_BIT_M_TX_ABRT | I2C_BIT_M_TX_EMPTY | I2C_BIT_M_TX_OVER), DISABLE);

		/* Clear all I2C pending interrupts */
		I2C_ClearAllINT(obj->I2Cint.I2Cx);
	}

	if (obj->datalength > 0) {
		int cnt = 16;
		/* Check I2C TX FIFO status. If it's not full, one byte data will be written into it. */
		while (cnt > 0 && I2C_CheckFlagState(obj->I2Cint.I2Cx, I2C_BIT_TFNF) && obj->datalength > 0) {
			if (obj->datalength == 1)	{
				I2CStop = 1;
			}
			I2C_MasterSend(obj->I2Cint.I2Cx, obj->pbuf, 0, I2CStop, 0);
			obj->pbuf++;
			obj->datalength--;
			cnt--;
		}
	}
}

static void I2CISRHandleRdReq(IN i2c_ts *obj)
{
	if (!obj->datalength) {
		while (0 == I2C_CheckFlagState(obj->I2Cint.I2Cx, I2C_BIT_TFE));

		/* Disable I2C TX Related Interrupts */
		I2C_INTConfig(obj->I2Cint.I2Cx, (I2C_BIT_M_TX_ABRT | I2C_BIT_M_TX_OVER | I2C_BIT_M_RX_DONE | I2C_BIT_M_RD_REQ), DISABLE);

		I2C_ClearAllINT(obj->I2Cint.I2Cx);
	} else {
		/* I2C Slave transmits data to Master. If the TX FIFO is NOT full,
		write one byte from slave TX buffer to TX FIFO. */
		if (I2C_CheckFlagState(obj->I2Cint.I2Cx, I2C_BIT_TFNF)) {
			I2C_SlaveSend(obj->I2Cint.I2Cx, *obj->pbuf);
			obj->pbuf++;
			obj->datalength--;
		}
	}
}

static void I2CISRHandleRxFull(IN i2c_ts *obj)
{

	/* To check I2C master RX data length. If all the data are received,
	mask all the interrupts and invoke the user callback.
	Otherwise, if there is data in the RX FIFO and move the data from RX
	FIFO to user data buffer*/
	/* Receive data till the RX buffer data length is zero */

	if (obj->datalength > 0) {
		if (I2C_CheckFlagState(obj->I2Cint.I2Cx, (I2C_BIT_RFNE | I2C_BIT_RFF))) {
			*(obj->pbuf) = I2C_ReceiveData(obj->I2Cint.I2Cx);
			obj->pbuf++;
			obj->datalength--;
		}
	}

	/* All data are received. Mask all related interrupts. */
	if (!obj->datalength) {
		/*I2C Disable RX Related Interrupts*/
		I2C_INTConfig(obj->I2Cint.I2Cx, (I2C_BIT_M_RX_FULL | I2C_BIT_M_RX_OVER | I2C_BIT_M_RX_UNDER), DISABLE);
	}
}


static u32 I2CISRHandle(IN void *Data)
{
	i2c_ts *obj = (i2c_ts *) Data;

	obj->I2Cint.I2Cx = I2C_DEV_TABLE[obj->I2Cint.i2c_idx].I2Cx;

	u32 intr_status = I2C_GetINT(obj->I2Cint.I2Cx);

	/* I2C ADDR MATCH Intr*/
	if (intr_status & I2C_BIT_M_LP_WAKE_1) {
		/* Clear I2C interrupt */
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_M_LP_WAKE_1);
	}
	if (intr_status & I2C_BIT_M_LP_WAKE_2) {
		/* Clear I2C interrupt */
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_M_LP_WAKE_2);
	}

	/* I2C STOP DET Intr */
	if (intr_status & I2C_BIT_R_STOP_DET) {
		rx_done = 1;
		/* Clear I2C interrupt */
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_R_STOP_DET);
	}

	/* I2C Activity Intr */
	if (intr_status & I2C_BIT_R_ACTIVITY) {
		/* Clear I2C interrupt */
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_R_ACTIVITY);
	}

	/* I2C RX Done Intr */
	if (intr_status & I2C_BIT_R_RX_DONE) {
		tx_done = 1;
		//slave-transmitter and master not ACK it, This occurs on the last byte of
		//the transmission, indicating that the transmission is done.
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_R_RX_DONE);

	}

	/* I2C TX Abort Intr */
	if (intr_status & I2C_BIT_R_TX_ABRT) {
		RTK_LOGI(TAG, "I2C_BIT_R_TX_ABRT\n");
		I2C_ClearAllINT(obj->I2Cint.I2Cx);
		I2C_Cmd(obj->I2Cint.I2Cx, DISABLE);
		uint32_t temp = obj->I2Cint.I2Cx->IC_CON;
		temp &= ~I2C_BIT_MASTER_MODE;
		obj->I2Cint.I2Cx->IC_CON = temp;
		I2C_Cmd(obj->I2Cint.I2Cx, ENABLE);
		DelayUs(10);
		I2C_Cmd(obj->I2Cint.I2Cx, DISABLE);
		temp = obj->I2Cint.I2Cx->IC_CON;
		temp |= I2C_BIT_MASTER_MODE;
		obj->I2Cint.I2Cx->IC_CON = temp;
		I2C_Cmd(obj->I2Cint.I2Cx, ENABLE);
		obj->pbuf = i2cdatasrc;
		obj->datalength = length;

	}

	/* I2C TX Empty Intr */
	if (intr_status & I2C_BIT_R_TX_EMPTY) {
		I2CISRHandleTxEmpty(obj);
	}

	if (intr_status & I2C_BIT_R_RD_REQ) {
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_R_RD_REQ);
		I2CISRHandleRdReq(obj);
	}

	/* I2C TX Over Run Intr */
	if (intr_status & I2C_BIT_R_TX_OVER) {
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_R_TX_OVER);
	}

	/* I2C RX Full Intr */
	if ((intr_status & I2C_BIT_R_RX_FULL) || (intr_status & I2C_BIT_R_RX_OVER)) {

		/*I2C RX Over Run Intr*/
		if (intr_status & I2C_BIT_R_RX_OVER) {
			I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_R_RX_OVER);
		}
		I2CISRHandleRxFull(obj);
	}

	/*I2C RX Under Run Intr*/
	if (intr_status & I2C_BIT_R_RX_UNDER) {
		I2C_ClearINT(obj->I2Cint.I2Cx, I2C_BIT_R_RX_UNDER);
	}

	return 0;
}

static void RtkI2CInit(i2c_ts *obj1, uint8_t sda, uint8_t scl)
{
	i2c_ts *obj = obj1;
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;

	obj->I2Cint.I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;

	/* Set I2C Device Number */
	I2CInitData[i2c_idx].I2CIdx = i2c_idx;

	I2CInitData[i2c_idx].I2CAckAddr	= I2C_SLAVE_ADDR0;

	/* I2C Pin Mux Initialization */

	switch (I2C_ID) {
	case 0:
		RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
		break;
	case 1:
		RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, ENABLE);
		break;
#if defined (CONFIG_AMEBASMART)
	case 2:
		RCC_PeriphClockCmd(APBPeriph_I2C2, APBPeriph_I2C2_CLOCK, ENABLE);
		break;
#endif
	default:
		RTK_LOGI(TAG, "I2C id error\r\n");
		break;
	}

#if defined (CONFIG_AMEBASMART)
	Pinmux_Config(I2C_SLV_SDA, PINMUX_FUNCTION_I2C);
	Pinmux_Config(I2C_SLV_SCL, PINMUX_FUNCTION_I2C);
#else
	if (i2c_idx == 0) {
		Pinmux_Config(I2C_SLV_SDA, PINMUX_FUNCTION_I2C0_SDA);
		Pinmux_Config(I2C_SLV_SCL, PINMUX_FUNCTION_I2C0_SCL);
	} else {
		Pinmux_Config(I2C_SLV_SDA, PINMUX_FUNCTION_I2C1_SDA);
		Pinmux_Config(I2C_SLV_SCL, PINMUX_FUNCTION_I2C1_SCL);
	}
#endif


	PAD_PullCtrl(sda, GPIO_PuPd_UP);
	PAD_PullCtrl(scl, GPIO_PuPd_UP);


	InterruptRegister((IRQ_FUN)I2CISRHandle, I2C_DEV_TABLE[i2c_idx].IrqNum, (u32)(obj), 5);
	InterruptEn(I2C_DEV_TABLE[i2c_idx].IrqNum, 5);

	/* I2C HAL Initialization */
	I2C_Init(obj->I2Cint.I2Cx, &I2CInitData[i2c_idx]);

	/* I2C Enable Module */
	I2C_Cmd(obj->I2Cint.I2Cx, ENABLE);
}


_OPTIMIZE_NONE_
void i2c_interrupt_mode_task(void)
{
	int i2clocalcnt;

	// prepare for transmission
	_memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	RTK_LOGI(TAG, "Slave addr=%x\n", I2C_SLAVE_ADDR0);
	_memset(&i2cslave, 0x00, sizeof(i2c_ts));

	i2cslave.I2Cint.i2c_idx = I2C_ID;
	I2C_StructInit(&I2CInitData[i2cslave.I2Cint.i2c_idx]);
	I2CInitData[i2cslave.I2Cint.i2c_idx].I2CMaster = I2C_SLAVE_MODE;
	RtkI2CInit(&i2cslave, I2C_SLV_SDA, I2C_SLV_SCL);

	// Master write - Slave read
	i2cslave.pbuf = i2cdatadst;
	i2cslave.datalength = length;
	rx_done = 0;
	I2C_INTConfig(i2cslave.I2Cint.I2Cx, (I2C_BIT_M_RX_FULL | I2C_BIT_M_RX_OVER | I2C_BIT_M_RX_UNDER | I2C_BIT_R_STOP_DET), ENABLE);
	while (rx_done == 0);

	i2c_rx_check();

	// Master read - Slave write
	i2cslave.pbuf = i2cdatasrc;
	i2cslave.datalength = length;
	tx_done = 0;
	I2C_INTConfig(i2cslave.I2Cint.I2Cx, (I2C_BIT_M_TX_ABRT | I2C_BIT_M_TX_OVER | I2C_BIT_M_RX_DONE | I2C_BIT_M_RD_REQ), ENABLE);
	while (tx_done == 0);

	// Master write - Slave read
	i2cslave.pbuf = i2cdatadst;
	i2cslave.datalength = length;
	rx_done = 0;
	I2C_INTConfig(i2cslave.I2Cint.I2Cx, (I2C_BIT_M_RX_FULL | I2C_BIT_M_RX_OVER | I2C_BIT_M_RX_UNDER | I2C_BIT_R_STOP_DET), ENABLE);
	while (rx_done == 0);

	// Master read - Slave write
	i2cslave.pbuf = i2cdatasrc;
	i2cslave.datalength = length;
	rx_done = 0;
	I2C_INTConfig(i2cslave.I2Cint.I2Cx, (I2C_BIT_M_TX_ABRT | I2C_BIT_M_TX_OVER | I2C_BIT_M_RX_DONE | I2C_BIT_M_RD_REQ), ENABLE);
	while (rx_done == 0);

	// Master write - Slave read
	i2cslave.pbuf = i2cdatadst;
	i2cslave.datalength = length;
	rx_done = 0;
	I2C_INTConfig(i2cslave.I2Cint.I2Cx, (I2C_BIT_M_RX_FULL | I2C_BIT_M_RX_OVER | I2C_BIT_M_RX_UNDER | I2C_BIT_R_STOP_DET), ENABLE);
	while (rx_done == 0);

	// Master read - Slave write
	i2cslave.pbuf = i2cdatasrc;
	i2cslave.datalength = length;
	rx_done = 0;
	I2C_INTConfig(i2cslave.I2Cint.I2Cx, (I2C_BIT_M_TX_ABRT | I2C_BIT_M_TX_OVER | I2C_BIT_M_RX_DONE | I2C_BIT_M_RD_REQ), ENABLE);
	while (rx_done == 0);

	while (1);
}


int main(void)
{
	if (rtos_task_create(NULL, "I2C INTERRUPT DEMO", (rtos_task_t)i2c_interrupt_mode_task, NULL, (3072), 1) != SUCCESS) {
		RTK_LOGI(TAG, "Cannot create i2c_interrupt_mode_task demo task\n\r");
	}

	rtos_sched_start();

	return 0;
}

