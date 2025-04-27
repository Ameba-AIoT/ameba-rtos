/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_BT_MP_H__
#define __ATCMD_BT_MP_H__

#include "ameba_soc.h"

#if defined(CONFIG_BT) && CONFIG_BT
#if defined(CONFIG_MP_INCLUDED) && CONFIG_MP_INCLUDED

/* Exported types --------------------------------------------------------*/
typedef struct _at_command_mp_ext_item_ {
	const char	*mp_ext_cmd;
	int	(*mp_ext_fun)(void **argv, int argc);
	const char	*mp_ext_usage;
} at_mp_ext_item_t;

/* Exported constants --------------------------------------------------------*/
#define MP_EXT2_PREFIX		"[ATM2]: "
#define MP_EXT2_PRINTF(...) \
		do{ \
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, MP_EXT2_PREFIX); \
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, __VA_ARGS__); \
		}while(0)

#define UART_BRIDGE_USAGE		"ATM2=bridge\n"
#define BT_POWER_USAGE			"ATM2=bt_power,ACT <ACT: on/off>\n"
#define GNT_BT_USAGE			"ATM2=gnt_bt,TARGET <TARGET: wifi/bt>\n"
#define SELECTION_BT_ANTENNA	"ATM2=ant,TARGET <TARGET: s0/s1>\n"

/* Exported functions --------------------------------------------------------*/
extern void rtk_bt_mp_power_on(void);
extern void rtk_bt_mp_power_off(void);
extern void rtk_bt_set_bt_antenna(uint8_t ant);
extern void rtk_coex_btc_set_bt_ant(u8 bt_ant);
extern void rtk_coex_btc_set_pta(u8 type, u8 role, u8 process);

void print_bt_mp_at(void);
void at_mp_init(void);

#endif /* #if CONFIG_ATCMD_MP */
#endif /* #if CONFIG_BT */
#endif /* #ifndef __ATCMD_BT_MP_H__ */
