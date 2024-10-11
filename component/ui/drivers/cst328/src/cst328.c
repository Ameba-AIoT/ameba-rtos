/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "i2c_api.h"
#include "cst328.h"
#include "gpio_api.h"
#include "gpio_irq_api.h"

#define RST_PIN             _PA_10
#define INT_PIN             _PA_9
#define SDA_PIN             _PB_10
#define SCL_PIN             _PB_11

#define CST_TP1_REG         0XD000
#define CST_TP2_REG         0XD007
#define CST_TP3_REG         0XD00C
#define CST_TP4_REG         0XD011
#define CST_TP5_REG         0XD017

#define CST_DEBUG_INFO_MODE 0xD101
#define CST_FW_INFO         0xD208
#define CST_DEVIDE_MODE     0xD109

#define TPD_MAX_FINGERS     5
#define XSIZE               480
#define YSIZE               800
#define I2C_ADDR            0x1A
#define I2C_BUS_CLK         100000

#define CST328_DEBUG               0


static i2c_t g_obj;
static cst328_touch_data_t g_cst328_touch_data;
cst328_touch_data_callback g_cb = NULL;

static void CST328_read_reg(u16 reg, u8 *buf, u8 len)
{
	u16 r;
	r = reg & 0xff;

	reg = (reg >> 8) | (r << 8);
	i2c_write(&g_obj, I2C_ADDR, (char *)&reg, 2, 1);
	i2c_read(&g_obj, I2C_ADDR, (char *)buf, len, 1);
}

static u8 CST328_write_reg(u16 reg, u8 *buf, u8 len)
{
	u8 *temp;
	temp =  malloc(len + 2);
	temp[1] = reg & 0xff;
	temp[0] = (reg >> 8);

	memcpy(temp + 2, buf, len);
	i2c_write(&g_obj, I2C_ADDR, (char *)temp, len + 2, 2);
	free(temp);

	return 1;
}

static int CST328_firmware_info(void)
{
	u8 buf[28];
	u8 temp = 0;
	u32   cst328_ic_version  = 0;
	u16   cst328_ic_checksum = 0;
	CST328_write_reg(CST_DEBUG_INFO_MODE, &temp, 1);
	DelayMs(10);

	CST328_read_reg(CST_FW_INFO, buf, 8);

	cst328_ic_version = buf[3];
	cst328_ic_version <<= 8;
	cst328_ic_version |= buf[2];
	cst328_ic_version <<= 8;
	cst328_ic_version |= buf[1];
	cst328_ic_version <<= 8;
	cst328_ic_version |= buf[0];

	cst328_ic_checksum = buf[7];
	cst328_ic_checksum <<= 8;
	cst328_ic_checksum |= buf[6];
	cst328_ic_checksum <<= 8;
	cst328_ic_checksum |= buf[5];
	cst328_ic_checksum <<= 8;
	cst328_ic_checksum |= buf[4];

	if (CST328_DEBUG) {
		printf("[cst328]the chip ic version:%lu, checksum:%u\n", cst328_ic_version, cst328_ic_checksum);
	}

	if (cst328_ic_version == 0xA5A5A5A5) {
		printf("[cst328][err]the chip ic don't have firmware.\n");
		return -1;
	}

	return 0;
}

static void CST328_touch_report(void)
{
	u8 i = 0;
	u8 i2c_buf[8];
	u8 buf[30] = {0};
	int ret;
	int cnt;
	int idx;
	u8 sw;
	u16 input_x = 0;
	u16 input_y = 0;
	u16 reg;
	int i2c_len, len_1, len_2;

	CST328_read_reg(CST_TP1_REG, buf, 7);
	if (buf[6] != 0xAB) {
		printf("[cst328][err]Scan data is not valid\n");
		i2c_buf[0] = 0xD0;
		i2c_buf[1] = 0x00;
		i2c_buf[2] = 0xAB;
		ret = i2c_write(&g_obj, I2C_ADDR, (char *)i2c_buf, 3, 1);
		if (ret < 0) {
			printf("[cst328][err]Scan send read touch info ending failed\n");
		}
	}

	cnt = buf[5] & 0x7F;

	if (cnt > TPD_MAX_FINGERS) {
		// sync touch data
		printf("[cst328][err]Scan touch exceed max fingers\n");
		i2c_buf[0] = 0xD0;
		i2c_buf[1] = 0x00;
		i2c_buf[2] = 0xAB;
		ret = i2c_write(&g_obj, I2C_ADDR, (char *)i2c_buf, 3, 1);
		if (ret < 0) {
			printf("[cst328][err]Scan send read touch info ending failed\n");
		}
		return;
	} else if (cnt == 0) {
		return;
	}

	if (cnt > 0x01) { // touch cnt greater than one
		printf("[cst328][err]Scan cnt: %d\n", cnt);
		i2c_len = (cnt - 1) * 5 + 1;
		len_1   = i2c_len;
		for (idx = 0; idx < i2c_len; idx += 6) {
			i2c_buf[0] = 0xD0;
			i2c_buf[1] = 0x07 + idx;
			reg = (u16)((i2c_buf[0] << 8) | i2c_buf[1]);
			if (CST328_DEBUG) {
				printf("[cst328][debug]Scan read touch reg: %x\n",  reg);
			}

			if (len_1 >= 6) {
				len_2  = 6;
				len_1 -= 6;
			} else {
				len_2 = len_1;
				len_1 = 0;
			}

			CST328_read_reg(reg, i2c_buf, len_2);
			for (i = 0; i < len_2; i++) {
				buf[5 + idx + i] = i2c_buf[i];
			}
		}
		i2c_len += 5;

		if (buf[i2c_len - 1] != 0xAB) {
			printf("[cst328][err]Scan data error!\n");
			buf[0] = 0xD0;
			buf[1] = 0x00;
			buf[2] = 0xAB;
			ret = i2c_write(&g_obj, I2C_ADDR, (char *)buf, 3, 1);
			if (ret < 0) {
				printf("[cst328][err]Scan send read touch info ending failed\n");
			}
			return;
		}
	}

	// sync touch data
	i2c_buf[0] = 0xD0;
	i2c_buf[1] = 0x00;
	i2c_buf[2] = 0xAB;
	ret = i2c_write(&g_obj, I2C_ADDR, (char *)i2c_buf, 3, 1);
	if (ret < 0) {
		printf("[cst328][err]Scan send read touch info ending failed\n");
	}

	idx = 0;
	for (i = 0; i < cnt; i++) {
		input_x = (u16)((buf[idx + 1] << 4) | ((buf[idx + 3] >> 4) & 0x0F));
		input_y = (u16)((buf[idx + 2] << 4) | (buf[idx + 3] & 0x0F));
		sw = (buf[idx] & 0x0F) >> 1;

		if (CST328_DEBUG) {
			printf("cst328Point x:%d, y:%d, sw:%d.", input_x, input_y, sw);
		}

		if (sw == 0x03) {   //touch pressed
			g_cst328_touch_data.x = XSIZE - input_x;
			g_cst328_touch_data.y = YSIZE - input_y;
			g_cst328_touch_data.state = TOUCH_PRESS;
			if (CST328_DEBUG) {
				printf("CST328_Scan touch data: %d, %d\n",  g_cst328_touch_data.x, g_cst328_touch_data.y);
			}
		} else {
			g_cst328_touch_data.state = TOUCH_RELEASE;
		}

		idx += 5;
	}

}

void gpio_touch_irq_handler(uint32_t id, gpio_irq_event event)
{
	UNUSED(id);
	UNUSED(event);
	CST328_touch_report();
	if (g_cb) {
		g_cb(g_cst328_touch_data);
	}
}

void cst328_init(void)
{
	u8 temp;
	GPIO_InitTypeDef GPIO_InitStructure;

	/*init RST pin*/
	GPIO_InitStructure.GPIO_Pin = RST_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStructure);
	GPIO_WriteBit(RST_PIN, 0);

	DelayMs(100);

	GPIO_WriteBit(RST_PIN, 1);

	/*init INT pin*/
	GPIO_InitStructure.GPIO_Pin = INT_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(&GPIO_InitStructure);

	DelayMs(50);

	/*init I2C*/
	i2c_init(&g_obj, SDA_PIN, SCL_PIN);
	i2c_frequency(&g_obj, I2C_BUS_CLK);

	CST328_firmware_info();

	temp = 0;
	CST328_write_reg(CST_DEVIDE_MODE, &temp, 1);

	gpio_irq_t gpio_btn;
	gpio_irq_init(&gpio_btn, INT_PIN, gpio_touch_irq_handler, (uint32_t)(&gpio_btn));
	gpio_irq_set(&gpio_btn, IRQ_FALL, 1);
	gpio_irq_enable(&gpio_btn);
}

void cst328_register_touch_data_callback(cst328_touch_data_callback cb)
{
	g_cb = cb;
}
