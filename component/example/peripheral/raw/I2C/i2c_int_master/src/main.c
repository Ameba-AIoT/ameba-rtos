/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>
#include "i2c_ext.h"

#define I2C_ID 0

#define I2C_MTR_ID 			I2C_ID

#define I2C_DATA_LENGTH     127

static const char *TAG = "I2C";
uint8_t	i2cdatasrc[I2C_DATA_LENGTH];
u32 datalength = I2C_DATA_LENGTH;
u8 *pdatabuf;

rtos_sema_t txSemaphore = NULL;
rtos_sema_t rxSemaphore = NULL;

static void i2c_give_sema(u32 IsWrite)
{
	if (IsWrite) {
		rtos_sema_give(txSemaphore);
	} else {
		rtos_sema_give(rxSemaphore);
	}
}

static void i2c_take_sema(u32 IsWrite)
{
	if (IsWrite) {
		rtos_sema_take(txSemaphore, RTOS_MAX_DELAY);
	} else {
		rtos_sema_take(rxSemaphore, RTOS_MAX_DELAY);
	}
}

void i2c_int_task(void)
{
	int  i2clocalcnt;

	I2C_InitTypeDef  i2C_master;
	I2C_IntModeCtrl i2c_intctrl;
	i2c_intctrl.I2Cx = I2C_DEV_TABLE[I2C_MTR_ID].I2Cx;

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

	// prepare for transmission
	_memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	rtos_sema_create_binary(&txSemaphore);
	rtos_sema_create_binary(&rxSemaphore);

	I2C_StructInit(&i2C_master);

	i2C_master.I2CIdx = I2C_MTR_ID;
	i2C_master.I2CAckAddr = 0x23;
	i2C_master.I2CSpdMod = I2C_SS_MODE;
	i2C_master.I2CClk = 100;
	i2C_master.I2CIPClk = I2CCLK_TABLE[I2C_MTR_ID];

	i2C_master.I2CTXTL = I2C_TRX_BUFFER_DEPTH / 3;

	i2c_intctrl.I2CSendSem =  i2c_give_sema;
	i2c_intctrl.I2CWaitSem = i2c_take_sema;

	/* I2C Pin Mux Initialization */
#if defined (CONFIG_AMEBASMART)
	Pinmux_Config(I2C_MTR_SDA, PINMUX_FUNCTION_I2C);
	Pinmux_Config(I2C_MTR_SCL, PINMUX_FUNCTION_I2C);
#else
	if (i2C_master.I2CIdx == 0) {
		Pinmux_Config(I2C_MTR_SDA, PINMUX_FUNCTION_I2C0_SDA);
		Pinmux_Config(I2C_MTR_SCL, PINMUX_FUNCTION_I2C0_SCL);
	} else {
		Pinmux_Config(I2C_MTR_SDA, PINMUX_FUNCTION_I2C1_SDA);
		Pinmux_Config(I2C_MTR_SCL, PINMUX_FUNCTION_I2C1_SCL);
	}
#endif

	PAD_PullCtrl(I2C_MTR_SDA, GPIO_PuPd_UP);
	PAD_PullCtrl(I2C_MTR_SCL, GPIO_PuPd_UP);

	/* I2C HAL Initialization */
	I2C_Init(i2c_intctrl.I2Cx, &i2C_master);

	InterruptRegister((IRQ_FUN)I2C_ISRHandle, I2C_DEV_TABLE[I2C_MTR_ID].IrqNum, (u32)&i2c_intctrl, 5);
	InterruptEn(I2C_DEV_TABLE[I2C_MTR_ID].IrqNum, 5);

	/* I2C Enable Module */
	I2C_Cmd(i2c_intctrl.I2Cx, ENABLE);

	pdatabuf = i2cdatasrc;


	I2C_MasterWriteInt(i2c_intctrl.I2Cx, &i2c_intctrl, pdatabuf, I2C_DATA_LENGTH);
	DelayMs(50);
	I2C_MasterReadInt(i2c_intctrl.I2Cx, &i2c_intctrl, pdatabuf, I2C_DATA_LENGTH);
	DelayMs(50);

	I2C_MasterWriteInt(i2c_intctrl.I2Cx, &i2c_intctrl, pdatabuf, I2C_DATA_LENGTH);
	DelayMs(50);
	I2C_MasterReadInt(i2c_intctrl.I2Cx, &i2c_intctrl, pdatabuf, I2C_DATA_LENGTH);
	DelayMs(50);

	I2C_MasterWriteInt(i2c_intctrl.I2Cx, &i2c_intctrl, pdatabuf, I2C_DATA_LENGTH);
	DelayMs(50);
	I2C_MasterReadInt(i2c_intctrl.I2Cx, &i2c_intctrl, pdatabuf, I2C_DATA_LENGTH);


	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "i2c_task DEMO", (rtos_task_t)i2c_int_task, NULL, (2048), (1)) != SUCCESS) {
		RTK_LOGI(TAG, "Cannot create i2c_int_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}

