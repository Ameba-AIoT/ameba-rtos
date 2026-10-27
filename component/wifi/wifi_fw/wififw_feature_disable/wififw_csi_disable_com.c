/**
 ******************************************************************************
* @file    wififw_csi_disable_com.c
* @author
* @version
* @date
******************************************************************************
* @attention
*
* This module is a confidential and proprietary property of RealTek and
* possession or use of this module requires written permission of RealTek.
*
* Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
******************************************************************************
*/
#define __WIFIFW_CSI_DISABLE_COM_C__

#include "basic_types.h"

void wififw_h2c_csi(void *pbuf)
{
	UNUSED(pbuf);
}

void wififw_csi_check_retry(u8 a, u8 b)
{
	UNUSED(a);
	UNUSED(b);
}

bool wififw_csi_check_done(void)
{
	return TRUE;
}

void wififw_csi_handle(void)
{
}
