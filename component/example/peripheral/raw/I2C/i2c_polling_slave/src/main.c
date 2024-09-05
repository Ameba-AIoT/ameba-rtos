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
struct i2c_m {
	uint32_t i2c_idx;
	I2C_TypeDef *I2Cx;
};
typedef struct i2c_m i2c_t;

/* RESTART verification */
//#define I2C_RESTART_DEMO

/*LOOP read then write 3 times*/
#define I2C_LOOP_TEST

#define I2C_ID 0

#define I2C_SLAVE_ADDR0    0x23

#define I2C_DATA_LENGTH         127
uint8_t i2cdatasrc[I2C_DATA_LENGTH];
uint8_t i2cdatadst[I2C_DATA_LENGTH];
uint8_t i2cdatardsrc[I2C_DATA_LENGTH];
I2C_InitTypeDef I2CInitData[2];

u32 restart_enable = 0;
uint16_t i2c_target_addr[2];
u32 master_addr_retry = 1;
static const char *TAG = "I2C";

i2c_t i2cslave;

/**
  * @brief  Initializes the I2C device, include clock/function/I2C registers.
  * @param  I2c_index: I2C_0/I2C_1.
  * @param  obj: i2c object define in application software.
  * @param  sda: SDA PinName according to pinmux spec.
  * @param  scl: SCL PinName according to pinmux spec.
  * @param  mode I2C_SLAVE_MODE/I2C_MASTER_MODE.
  * @retval none
  */
static void i2c_StructInit(i2c_t *obj, uint32_t I2c_index, uint8_t sda, uint8_t scl, uint8_t mode)
{
	uint32_t i2c_idx = 0;
	i2c_idx = I2c_index;
	/* I2C Pin Mux Initialization */


#if defined (CONFIG_AMEBASMART)
	Pinmux_Config(sda, PINMUX_FUNCTION_I2C);
	Pinmux_Config(scl, PINMUX_FUNCTION_I2C);
#else
	if (I2c_index == 0) {
		Pinmux_Config(sda, PINMUX_FUNCTION_I2C0_SDA);
		Pinmux_Config(scl, PINMUX_FUNCTION_I2C0_SCL);
	} else {
		Pinmux_Config(sda, PINMUX_FUNCTION_I2C1_SDA);
		Pinmux_Config(scl, PINMUX_FUNCTION_I2C1_SCL);
	}
#endif
	PAD_PullCtrl(sda, GPIO_PuPd_UP);
	PAD_PullCtrl(scl, GPIO_PuPd_UP);


	RTK_LOGI(TAG, "i2c_idx:%x\n", i2c_idx);

	obj->i2c_idx = i2c_idx;
	obj->I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;

	/* Set I2C Device Number */
	I2CInitData[obj->i2c_idx].I2CIdx = i2c_idx;

	/* Load I2C default value */
	I2C_StructInit(&I2CInitData[obj->i2c_idx]);

	/* Assign I2C Pin Mux */
	I2CInitData[obj->i2c_idx].I2CMaster     = mode;
	I2CInitData[obj->i2c_idx].I2CSpdMod     = I2C_SS_MODE;
	I2CInitData[obj->i2c_idx].I2CClk        = 100;
	I2CInitData[obj->i2c_idx].I2CAckAddr    = I2C_SLAVE_ADDR0;
	I2CInitData[obj->i2c_idx].I2CAddrMod    = I2C_ADDR_7BIT;

}

void i2c_Restart_enable(i2c_t *obj)
{
	uint32_t i2cen = 0;

	if (obj->I2Cx->IC_ENABLE & I2C_BIT_ENABLE) {
		I2C_Cmd(obj->I2Cx, DISABLE);
		i2cen = 1;
	}

	obj->I2Cx->IC_CON |= I2C_BIT_IC_RESTATRT_EN;

	if (i2cen) {
		I2C_Cmd(obj->I2Cx, ENABLE);
	}

	restart_enable = 1;
}

static void i2c_slave_rx_check(void)
{
	int     i2clocalcnt;
	int     result = 0;

	RTK_LOGI(TAG, "check slave received data>>>\n");
	//for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 1) {
	//RTK_LOGI(TAG,"i2c data: %02x\n", i2cdatadst[i2clocalcnt]);
	//}
	//HalDelayUs(5000);

	// verify result
	result = 1;
#if !defined(I2C_RESTART_DEMO)
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt]) {
			RTK_LOGI(TAG, "i2c data: %02x \t %02x\n", i2cdatasrc[i2clocalcnt], i2cdatadst[i2clocalcnt]);
			result = 0;
			break;
		}
	}
#else
	if (i2cdatasrc[0] == i2cdatadst[0]) {
		if (i2cdatasrc[1] != i2cdatadst[1]) {
			result = 0;
		}
	} else if (i2cdatasrc[1] == i2cdatadst[0]) {
		for (i2clocalcnt = 1; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
			if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt - 1]) {
				RTK_LOGI(TAG, "idx:%d, src:%x, dst:%x\n", i2clocalcnt, i2cdatasrc[i2clocalcnt], i2cdatadst[i2clocalcnt]);
				for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt ++) {
					RTK_LOGI(TAG, "i2c data: %02x\n", i2cdatadst[i2clocalcnt]);
				}
				result = 0;
				break;
			}
		}
	} else {
		for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
			if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt]) {
				RTK_LOGI(TAG, "idx:%d, src:%x, dst:%x\n", i2clocalcnt, i2cdatasrc[i2clocalcnt], i2cdatadst[i2clocalcnt]);
				result = 0;
				break;
			}
		}
	}
#endif

	RTK_LOGI(TAG, "Slave receive: Result is %s\r\n", (result) ? "success" : "fail");
	_memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);
}

void i2c_dual_slave_task(void)
{
	int i2clocalcnt;
	/* enable I2C functions */
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
	_memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatardsrc[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatardsrc[i2clocalcnt] = i2clocalcnt + 1;
	}

	RTK_LOGI(TAG, "Slave addr=%x\n", I2C_SLAVE_ADDR0);
	_memset(&i2cslave, 0x00, sizeof(i2c_t));
	i2c_StructInit(&i2cslave, I2C_ID, I2C_SLV_SDA, I2C_SLV_SCL, I2C_SLAVE_MODE);

	I2C_Init(i2cslave.I2Cx, &I2CInitData[i2cslave.i2c_idx]);
	I2C_Cmd(i2cslave.I2Cx, ENABLE);

	// Master write - Slave read
	RTK_LOGI(TAG, "Slave read 1>>>\n");
	I2C_SlaveRead(i2cslave.I2Cx, &i2cdatadst[0], I2C_DATA_LENGTH);
	i2c_slave_rx_check();

	// Master read - Slave write
#ifdef I2C_RESTART_DEMO
	I2C_SlaveRead(i2cslave.I2Cx, &i2cdatadst[0], 1);
#endif

	RTK_LOGI(TAG, "Slave write 1>>>\n");
	I2C_SlaveWrite(i2cslave.I2Cx, &i2cdatardsrc[0], I2C_DATA_LENGTH);
	DelayMs(1);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_RX_DONE);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_RD_REQ);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_TX_ABRT);
#ifdef I2C_LOOP_TEST
	RTK_LOGI(TAG, "Slave read  2>>>\n");
	I2C_SlaveRead(i2cslave.I2Cx, &i2cdatadst[0], I2C_DATA_LENGTH);
	i2c_slave_rx_check();

	RTK_LOGI(TAG, "Slave write  2>>>\n");
	I2C_SlaveWrite(i2cslave.I2Cx, &i2cdatardsrc[0], I2C_DATA_LENGTH);
	DelayMs(1);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_RX_DONE);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_RD_REQ);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_TX_ABRT);


	RTK_LOGI(TAG, "Slave read 3>>>\n");
	I2C_SlaveRead(i2cslave.I2Cx, &i2cdatadst[0], I2C_DATA_LENGTH);
	i2c_slave_rx_check();

	RTK_LOGI(TAG, "Slave write 3>>>\n");
	I2C_SlaveWrite(i2cslave.I2Cx, &i2cdatardsrc[0], I2C_DATA_LENGTH);
	DelayMs(1);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_RX_DONE);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_RD_REQ);
	I2C_ClearINT(i2cslave.I2Cx, I2C_BIT_R_TX_ABRT);
#endif
	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "I2C DULE SLAVE DEMO", (rtos_task_t)i2c_dual_slave_task, NULL, (3072), (1)) != SUCCESS) {
		RTK_LOGI(TAG, "Cannot create i2c_dual_slave_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}

