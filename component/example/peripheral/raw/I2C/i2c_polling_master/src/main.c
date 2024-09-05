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

u32 restart_enable = 0;
uint16_t i2c_target_addr[2];
u32 master_addr_retry = 1;

#define I2C_ID 0

#define I2C_SLAVE_ADDR0    0x23
#define I2C_BUS_CLK        100000  //hz

#define I2C_DATA_LENGTH         127
uint8_t i2cdatasrc[I2C_DATA_LENGTH];
uint8_t i2cdatadst[I2C_DATA_LENGTH];
uint8_t i2cdatardsrc[I2C_DATA_LENGTH];
uint8_t i2cdatarddst[I2C_DATA_LENGTH];
I2C_InitTypeDef I2CInitData[2];
static const char *TAG = "I2C";


/* RESTART verification */
//#define I2C_RESTART_DEMO

/*LOOP write then read 3 times*/
#define I2C_LOOP_TEST


i2c_t i2cmaster;
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
void i2c_StructInit(i2c_t *obj, uint32_t I2c_index, uint8_t sda, uint8_t scl, uint8_t mode)
{
	uint32_t i2c_idx;

	i2c_idx = I2c_index;
	/* I2C Pin Mux Initialization */

#if defined (CONFIG_AMEBASMART)
	Pinmux_Config(sda, PINMUX_FUNCTION_I2C);
	Pinmux_Config(scl, PINMUX_FUNCTION_I2C);
#else
	if (i2c_idx == 0) {
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
	I2CInitData[obj->i2c_idx].I2CClk        = 100; //KHz
	I2CInitData[obj->i2c_idx].I2CAckAddr    = I2C_SLAVE_ADDR0;
	I2CInitData[obj->i2c_idx].I2CAddrMod    = I2C_ADDR_7BIT;
	I2CInitData[obj->i2c_idx].I2CIPClk      = I2CCLK_TABLE[i2c_idx];

}

/**
  * @brief  Set i2c frequency.
  * @param  obj: i2c object define in application software.
  * @param  hz: i2c clock(unit is Hz).
  * @retval none
  */
void i2c_Frequency(i2c_t *obj, int hz)
{
	uint16_t i2c_default_clk    = (uint16_t) I2CInitData[obj->i2c_idx].I2CClk;
	uint16_t i2c_user_clk       = (uint16_t)(hz / 1000);

	if (i2c_default_clk != i2c_user_clk) {
		/* Deinit I2C first */
		I2C_Cmd(obj->I2Cx, DISABLE);
		if (i2c_user_clk <= 100) {
			I2CInitData[obj->i2c_idx].I2CSpdMod = I2C_SS_MODE;
		} else if ((i2c_user_clk > 100) && (i2c_user_clk <= 400)) {
			I2CInitData[obj->i2c_idx].I2CSpdMod = I2C_FS_MODE;
		} else if (i2c_user_clk > 400) {
			I2CInitData[obj->i2c_idx].I2CSpdMod = I2C_HS_MODE;
		}

		/* Load the user defined I2C clock */
		I2CInitData[obj->i2c_idx].I2CClk = i2c_user_clk;

		/* Init I2C now */
		I2C_Init(obj->I2Cx, &I2CInitData[obj->i2c_idx]);
		I2C_Cmd(obj->I2Cx, ENABLE);
	}
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

/**
  * @brief  I2C master restart after all bytes are sent.
  * @param  I2Cx: where I2Cx can be I2C0_DEV or I2C1_DEV to select the I2C peripheral.
  * @param  pBuf: point to the data to be sent.
  * @param  len: the length of data that to be sent.
  * @param  restart: specifies whether a RESTART is issued after all the bytes are sent.
  * @retval none
  */
void i2c_Send_restart(I2C_TypeDef *I2Cx, u8 *pBuf, u8 len, u8 restart)
{
	u8 cnt = 0;

	/* Write in the DR register the data to be sent */
	for (cnt = 0; cnt < len; cnt++) {
		while ((I2C_CheckFlagState(I2Cx, I2C_BIT_TFNF)) == 0);

		if (cnt >= len - 1) {
			/*generate restart signal*/
			I2Cx->IC_DATA_CMD = (*pBuf++) | (restart << 10);
		} else {
			I2Cx->IC_DATA_CMD = (*pBuf++);
		}
	}

	while ((I2C_CheckFlagState(I2Cx, I2C_BIT_TFE)) == 0);
}

/**
  * @brief  I2C master write in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  address: slave address which will be transmitted.
  * @param  data: point to the data to be sent.
  * @param  length: the length of data that to be sent.
  * @param  stop: specifies whether a STOP is issued after all the bytes are sent.
  * @retval the length of data send.
  */
int i2c_Write(i2c_t *obj, int address, const char *data, int length, int stop)
{
	if (i2c_target_addr[obj->i2c_idx] != address) {
		/* Deinit I2C first */
		I2C_Cmd(obj->I2Cx, DISABLE);

		/* Load the user defined I2C target slave address */
		i2c_target_addr[obj->i2c_idx] = address;
		I2CInitData[obj->i2c_idx].I2CAckAddr = address;

		/* Init I2C now */
		I2C_Init(obj->I2Cx, &I2CInitData[obj->i2c_idx]);
		I2C_Cmd(obj->I2Cx, ENABLE);
	}

	if ((!restart_enable) || (1 == stop)) {
		return (I2C_MasterWrite(obj->I2Cx, (unsigned char *)data, length));
	} else {
		i2c_Send_restart(obj->I2Cx, (unsigned char *)data, length, 1);
	}
	return length;
}

/**
  * @brief  I2C master read in poll mode.
  * @param  obj: i2c object define in application software.
  * @param  address: slave address which will be transmitted.
  * @param  data: point to the buffer to hold the received data.
  * @param  length: the length of data that to be received.
  * @param  stop: specifies whether a STOP is issued after all the bytes are received.
  * @retval the length of data received.
  */
int i2c_Read(i2c_t *obj, int address, char *data, int length, int stop)
{
	/* To avoid gcc warnings */
	(void) stop;
	if (i2c_target_addr[obj->i2c_idx] != address) {
		/* Deinit I2C first */
		I2C_Cmd(obj->I2Cx, DISABLE);

		/* Load the user defined I2C target slave address */
		i2c_target_addr[obj->i2c_idx] = address;
		I2CInitData[obj->i2c_idx].I2CAckAddr = address;

		/* Init I2C now */
		I2C_Init(obj->I2Cx, &I2CInitData[obj->i2c_idx]);
		I2C_Cmd(obj->I2Cx, ENABLE);
	}

	if (!master_addr_retry) {
		I2C_MasterRead(obj->I2Cx, (unsigned char *)data, length);
	} else {
		while (0 == I2C_MasterRead(obj->I2Cx, (unsigned char *)data, length)) {
			/* Wait for i2c enter trap state from trap_stop state*/
			DelayUs(100);

			/* Deinit I2C first */
			I2C_Cmd(obj->I2Cx, DISABLE);

			/* Load the user defined I2C target slave address */
			i2c_target_addr[obj->i2c_idx] = address;
			I2CInitData[obj->i2c_idx].I2CAckAddr = address;

			/* Init I2C now */
			I2C_Init(obj->I2Cx, &I2CInitData[obj->i2c_idx]);
			I2C_Cmd(obj->I2Cx, ENABLE);
		}
	}

	return length;
}
void i2c_master_rx_check(void)
{

	int     i2clocalcnt;
	int     result = 0;

	RTK_LOGI(TAG, "check master received data>>>\n");
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 2) {
//		RTK_LOGI(TAG,"i2c data: %02x \t %02x\n",i2cdatarddst[i2clocalcnt],i2cdatarddst[i2clocalcnt+1]);
	}

	// verify result
	result = 1;
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatarddst[i2clocalcnt] != i2cdatardsrc[i2clocalcnt]) {
			result = 0;
			break;
		}
	}
	RTK_LOGI(TAG, "Master receive: Result is %s\r\n", (result) ? "success" : "fail");
}

void i2c_dual_master_task(void)
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
	_memset(&i2cdatarddst[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatardsrc[i2clocalcnt] = i2clocalcnt + 1;
	}


	RTK_LOGI(TAG, "Slave addr=%x\n", I2C_SLAVE_ADDR0);
	_memset(&i2cmaster, 0x00, sizeof(i2c_t));
	i2c_StructInit(&i2cmaster, I2C_ID, I2C_MTR_SDA, I2C_MTR_SCL, I2C_MASTER_MODE);

	/* I2C HAL Initialization */
	I2C_Init(i2cmaster.I2Cx, &I2CInitData[i2cmaster.i2c_idx]);

	I2C_Cmd(i2cmaster.I2Cx, ENABLE);

	// Master write - Slave read
	RTK_LOGI(TAG, "\r\nMaster polling write>>>\n");
#ifdef I2C_RESTART_DEMO
	i2c_Write(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], 1, 0);
	i2c_Write(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[1], (I2C_DATA_LENGTH - 1), 1);
#else
	i2c_Write(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], I2C_DATA_LENGTH, 1);
#endif
	DelayMs(50);


	// Master read - Slave write
	RTK_LOGI(TAG, "Master polling read>>>\n");
#ifdef I2C_RESTART_DEMO
	i2c_Write(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], 1, 0);
#endif
	i2c_Read(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatarddst[0], I2C_DATA_LENGTH, 1);
	DelayMs(50);
	i2c_master_rx_check();

#ifdef I2C_LOOP_TEST

	RTK_LOGI(TAG, "\r\nMaster polling write2>>>\n");
	i2c_Write(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], I2C_DATA_LENGTH, 1);
	DelayMs(50);
	RTK_LOGI(TAG, "Master polling read2>>>\n");
	i2c_Read(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatarddst[0], I2C_DATA_LENGTH, 1);
	DelayMs(50);
	i2c_master_rx_check();

	RTK_LOGI(TAG, "\r\nMaster polling write3>>>\n");
	i2c_Write(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], I2C_DATA_LENGTH, 1);
	DelayMs(50);
	RTK_LOGI(TAG, "Master polling read3>>>\n");
	i2c_Read(&i2cmaster, I2C_SLAVE_ADDR0, (char *)&i2cdatarddst[0], I2C_DATA_LENGTH, 1);
	DelayMs(50);

	i2c_master_rx_check();
#endif

	while (1);
}


int main(void)
{
	if (rtos_task_create(NULL, "I2C DULE MASTER DEMO", (rtos_task_t)i2c_dual_master_task, NULL, (3072), (1)) != SUCCESS) {
		RTK_LOGI(TAG, "Cannot create i2c_dual_master_task demo task\n\r");
	}

	rtos_sched_start();

	return 0;
}

