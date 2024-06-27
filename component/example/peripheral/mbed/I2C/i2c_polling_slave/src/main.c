/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "PinNames.h"
#include "basic_types.h"
#include "diag.h"
#include "os_wrapper.h"
#include "i2c_ext.h"
#include "i2c_api.h"
#include "pinmap.h"
#include <stdio.h>

//#include "ex_api.h"
#define I2C_0 0
#define I2C_1 1

// RESTART verification
//#define I2C_RESTART_DEMO

#define MBED_I2C_SLAVE_ADDR0    0x23
#define MBED_I2C_BUS_CLK        100000  //hz

#define I2C_DATA_LENGTH         100
uint8_t	i2cdatasrc[I2C_DATA_LENGTH];
uint8_t	i2cdatadst[I2C_DATA_LENGTH];
uint8_t	i2cdatardsrc[I2C_DATA_LENGTH];

i2c_t   i2cslave;

void i2c_slave_rx_check(void)
{
	int i2clocalcnt;
	int result = 0;

	printf("check slave received data>>>\n");
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 2) {
		printf("i2c data: %02x \t %02x\n", i2cdatadst[i2clocalcnt], i2cdatadst[i2clocalcnt + 1]);
	}
	//HalDelayUs(5000);
	// verify result
	result = 1;
#if !defined(I2C_RESTART_DEMO)
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt]) {
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
				printf("idx:%d, src:%x, dst:%x\n", i2clocalcnt, i2cdatasrc[i2clocalcnt], i2cdatadst[i2clocalcnt]);
				for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 2) {
					printf("i2c data: %02x \t %02x\n", i2cdatadst[i2clocalcnt], i2cdatadst[i2clocalcnt + 1]);
				}
				result = 0;
				break;
			}
		}
	} else {
		for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
			if (i2cdatasrc[i2clocalcnt] != i2cdatadst[i2clocalcnt]) {
				printf("idx:%d, src:%x, dst:%x\n", i2clocalcnt, i2cdatasrc[i2clocalcnt], i2cdatadst[i2clocalcnt]);
				result = 0;
				break;
			}
		}
	}
#endif

	printf("\r\nSlave receive: Result is %s\r\n", (result) ? "success" : "fail");
	_memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);
}

void i2c_dual_slave_task(void)
{
	int i2clocalcnt;

	// prepare for transmission
	_memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatardsrc[0], 0x0, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatardsrc[i2clocalcnt] = i2clocalcnt + 1;
	}

	printf("Slave addr=%x\n", MBED_I2C_SLAVE_ADDR0);
	_memset(&i2cslave, 0x00, sizeof(i2c_t));
	i2cslave.i2c_idx = I2C_0;
	i2c_init(&i2cslave, MBED_I2C_SLV_SDA, MBED_I2C_SLV_SCL);
	i2c_frequency(&i2cslave, MBED_I2C_BUS_CLK);
	i2c_slave_address(&i2cslave, 0, MBED_I2C_SLAVE_ADDR0, 0xFF);
	i2c_slave_mode(&i2cslave, 1);

	// Master write - Slave read
	printf("\r\nSlave read>>>\n");
	i2c_slave_read(&i2cslave, (char *)&i2cdatadst[0], I2C_DATA_LENGTH);
	i2c_slave_rx_check();

	// Master read - Slave write
#ifdef I2C_RESTART_DEMO
	i2c_slave_read(&i2cslave, (char *)&i2cdatadst[0], 1);
#endif


	printf("Slave write>>>\n");
	i2c_slave_set_for_rd_req(&i2cslave, 1);
	i2c_slave_write(&i2cslave, (char *)&i2cdatardsrc[0], I2C_DATA_LENGTH);

	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "I2C DULE SLAVE DEMO", (rtos_task_t)i2c_dual_slave_task, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create i2c_dual_slave_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}

