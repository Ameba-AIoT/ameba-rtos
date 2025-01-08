/**
  ******************************************************************************
  * @file    inic_host_api.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include "inic_host.h"


/* -------------------------------- spi --------------------------------- */
int rtk_coex_ipc_h2c_info_handler(u16 type, u8 *pdata, u16 data_len)
{
	int ret = 0;
	u8 *param_buf;
	u8 *ptr = NULL;
	u8 buf_len;

	if (pdata == NULL || data_len == 0) {
		buf_len = 12;
		param_buf = rtos_mem_zmalloc(buf_len);
		param_buf[0] = (u32)type;
		memcpy(param_buf, &type, sizeof(u16));

		inic_api_host_message_send(INIC_API_COEX_H2C_INFO, param_buf, buf_len, (u8 *)&ret, sizeof(ret));

		rtos_mem_free(param_buf);
		return ret;
	}

	buf_len = 8 + data_len;

	param_buf = (u8 *)rtos_mem_zmalloc(buf_len);
	if (param_buf == NULL) {
		return -1;
	}
	ptr = param_buf;
	memcpy(ptr, &type, sizeof(u16));
	ptr += 4;
	memcpy(ptr, &data_len, sizeof(u16));
	ptr += 4;
	memcpy(ptr, pdata, data_len);

	inic_api_host_message_send(INIC_API_COEX_H2C_INFO, param_buf, buf_len, (u8 *)&ret, sizeof(ret));

	rtos_mem_free(param_buf);

	return ret;
}
