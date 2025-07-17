/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef RTW_COEX_API_EXT_H
#define RTW_COEX_API_EXT_H

#include <stdlib.h>
#include <string.h>
#include "PortNames.h"
#include "PinNames.h"
#include "rtw_coex_host_api.h"

void rtk_coex_extc_ntfy_init(struct extchip_para_t *p_extchip_para);
s32 rtk_coex_extc_get_init_params(struct extchip_para_t *p_extchip_para);

extern PinName port_pin(PortName port, int pin_n);

static inline uint16_t uart_pinname_to_pin(char *pin_name)
{
	uint16_t port, pin;
	char *underline;

	if (strstr(pin_name, "PA_") != NULL) {
		port = 0; //PORT_A
	} else if (strstr(pin_name, "PB_") != NULL) {
		port = 1; //PORT_B
	} else if (strstr(pin_name, "PC_") != NULL) {
		port = 2; //PORT_C
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[COEX][EXT] !!!!!!!!!! error pin name, SHOULD be like PA_3 !!!!!!!!!!!");
		return 0;
	}
	underline = strchr(pin_name, '_');
	pin = strtoul(underline + 1, (char **)NULL, 10);

	if (pin >= 32) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[COEX][EXT] !!!!!!!!!! error pin name, SHOULD less than 32 !!!!!!!!!!!");
		return 0;
	}
	pin = (uint16_t)port_pin((PortName)port, (int)pin);
	return pin;
}

//////////////////////////////////////////////////////////////////
/////////// called by main.c, for init parameters from menuconfig
//////////////////////////////////////////////////////////////////
static inline void coex_extc_paras_config(struct extchip_para_t *p_extchip_para)
{
// pta mode
#if defined(CONFIG_EXT_PTA_MODE_SIMPLE)
	p_extchip_para->pta_index = EXT_PTA1;
#else
	p_extchip_para->pta_index = EXT_PTA2;
#endif

// protocol
#if defined(CONFIG_EXT_PROTOCOL_WPAN)
	p_extchip_para->active_protocol = EXT_PTA_PROTOCOL_WPAN;
#elif defined(CONFIG_EXT_PROTOCOL_BT)
	p_extchip_para->active_protocol = EXT_PTA_PROTOCOL_BT;
#elif defined(CONFIG_EXT_PROTOCOL_BT_WPAN)
	p_extchip_para->active_protocol = EXT_PROTOCOL_BT_WPAN;
#else
	p_extchip_para->active_protocol = EXT_PTA_PROTOCOL_UNDEF;
#endif

// pri mode
#if defined(CONFIG_EXT_PRIORITY_DIRECTIONAL)
	p_extchip_para->pri_mode = 0;
#else
	p_extchip_para->pri_mode = 1;
#endif

// req polar
#if defined(CONFIG_EXT_REQUEST_POLAR_HIGH)
	p_extchip_para->req_polar = 1;
#else
	p_extchip_para->req_polar = 0;
#endif

// gnt polar
#if defined(CONFIG_EXT_GNT_POLAR_LOW)
	p_extchip_para->gnt_polar = 0;
#else
	p_extchip_para->gnt_polar = 1;
#endif

// pri_det_time
#if defined(CONFIG_EXT_PRI_DET_TIME)
	p_extchip_para->pri_det_time = CONFIG_EXT_PRI_DET_TIME;
#endif

// trx det time
#if defined(CONFIG_EXT_TRX_DET_TIME)
	p_extchip_para->trx_det_time = CONFIG_EXT_TRX_DET_TIME;
#endif

// pta pinmux pin set
#if defined(CONFIG_EXT_PTA_PIN_REQ)
	p_extchip_para->pta_pad_req = uart_pinname_to_pin(CONFIG_EXT_PTA_PIN_REQ);
#endif
#if defined(CONFIG_EXT_PTA_PIN_PRI)
	p_extchip_para->pta_pad_pri = uart_pinname_to_pin(CONFIG_EXT_PTA_PIN_PRI);
#endif
#if defined(CONFIG_EXT_PTA_PIN_GNT)
	p_extchip_para->pta_pad_gnt = uart_pinname_to_pin(CONFIG_EXT_PTA_PIN_GNT);
#endif

	p_extchip_para->valid = 1;
}





#endif /*#ifndef RTW_COEX_API_EXT_H*/