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

// RESTART verification
//#define I2C_RESTART_DEMO

#define MBED_I2C_SLAVE_ADDR0    0x23
#define MBED_I2C_BUS_CLK        100000  //hz

#define I2C_0 0

#define I2C_DATA_LENGTH         100
char i2cdatasrc[I2C_DATA_LENGTH];
char i2cdatardsrc[I2C_DATA_LENGTH];
char i2cdatarddst[I2C_DATA_LENGTH];

i2c_t   i2cmaster;

void i2c_master_rx_check(void)
{

	int i2clocalcnt;
	int result = 0;

	printf("check master received data>>>\n");
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt += 2) {
//		printf("i2c data: %02x \t %02x\n",i2cdatarddst[i2clocalcnt],i2cdatarddst[i2clocalcnt+1]);
	}

	// verify result
	result = 1;
	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		if (i2cdatarddst[i2clocalcnt] != i2cdatardsrc[i2clocalcnt]) {
			result = 0;
			break;
		}
	}
	printf("\r\nMaster receive: Result is %s\r\n", (result) ? "success" : "fail");

}

void i2c_dual_master_task(void)
{
	int i2clocalcnt;

	// prepare for transmission
	_memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatardsrc[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatarddst[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	for (i2clocalcnt = 0; i2clocalcnt < I2C_DATA_LENGTH; i2clocalcnt++) {
		i2cdatardsrc[i2clocalcnt] = i2clocalcnt + 1;
	}

	printf("Slave addr=%x\n", MBED_I2C_SLAVE_ADDR0);
	_memset(&i2cmaster, 0x00, sizeof(i2c_t));
	i2cmaster.i2c_idx = I2C_0;
	i2c_init(&i2cmaster, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	i2c_frequency(&i2cmaster, MBED_I2C_BUS_CLK);
#ifdef I2C_RESTART_DEMO
	i2c_restart_enable(&i2cmaster);
#endif

	// Master write - Slave read
	printf("\r\nMaster write>>>\n");
#ifdef I2C_RESTART_DEMO
	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], 1, 0);
	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[1], (I2C_DATA_LENGTH - 1), 1);
#else
	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], I2C_DATA_LENGTH, 1);
#endif

	// Master read - Slave write
	printf("Master read>>>\n");
#ifdef I2C_RESTART_DEMO
	i2c_write(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char *)&i2cdatasrc[0], 1, 0);
#endif
	i2c_read(&i2cmaster, MBED_I2C_SLAVE_ADDR0, (char *)&i2cdatarddst[0], I2C_DATA_LENGTH, 1);

	i2c_master_rx_check();

	while (1) {;}
}

int main(void)
{
	if (rtos_task_create(NULL, "I2C DULE MASTER DEMO", (rtos_task_t)i2c_dual_master_task, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create i2c_dual_master_task demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}

