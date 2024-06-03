// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include <rtw_cfg80211_fullmac.h>

#define CH_PLAN_VERSION "R67"
/* sync to WS-240223-Willis-Efuse_Channel_Plan_new_define-R67.xlsx */
#define RTL_RR_2G_01 REG_RULE(2412-10, 2462+10, 40, 0, 20, 0), REG_RULE(2467-10, 2472+10, 40, 0, 20, NL80211_RRF_NO_IR)
#define RTL_NRR_2G_01 2
#define RTL_RR_2G_02 REG_RULE(2412-10, 2472+10, 40, 0, 20, 0)
#define RTL_NRR_2G_02 1
#define RTL_RR_2G_03 REG_RULE(2412-10, 2462+10, 40, 0, 20, 0)
#define RTL_NRR_2G_03 1
#define RTL_RR_2G_04 REG_RULE(2412-10, 2484+10, 40, 0, 20, 0)
#define RTL_NRR_2G_04 1
#define RTL_RR_2G_05 REG_RULE(2457-10, 2472+10, 40, 0, 20, 0)
#define RTL_NRR_2G_05 1
#define RTL_RR_2G_06 REG_RULE(2412-10, 2462+10, 40, 0, 20, 0), REG_RULE(2467-10, 2484+10, 40, 0, 20, NL80211_RRF_NO_IR)
#define RTL_NRR_2G_06 1

#define RTL_RR_5G_01 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_01 5
#define RTL_RR_5G_02 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_02 3
#define RTL_RR_5G_03 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_03 4
#define RTL_RR_5G_04 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5660+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_04 4
#define RTL_RR_5G_05 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5620+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5805+10, 40, 0, 30, 0)
#define RTL_NRR_5G_05 4
#define RTL_RR_5G_06 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_06 2
#define RTL_RR_5G_07 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_07 3
#define RTL_RR_5G_08 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5805+10, 40, 0, 30, 0)
#define RTL_NRR_5G_08 3
#define RTL_RR_5G_09 REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_09 1
#define RTL_RR_5G_10 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_10 2
#define RTL_RR_5G_11 REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_11 1
#define RTL_RR_5G_12 REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_12 4
#define RTL_RR_5G_13 REG_RULE(5280-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_13 2
#define RTL_RR_5G_14 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0)
#define RTL_NRR_5G_14 1
#define RTL_RR_5G_15 REG_RULE(5745-10, 5805+10, 40, 0, 30, 0)
#define RTL_NRR_5G_15 1
#define RTL_RR_5G_16 REG_RULE(5500-10, 5700+10, 40, 0, 30, 0)
#define RTL_NRR_5G_16 1
#define RTL_RR_5G_17 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_17 4
#define RTL_RR_5G_18 REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, 0),\
		       REG_RULE(5745-10, 5805+10, 40, 0, 30, 0)
#define RTL_NRR_5G_18 4
#define RTL_RR_5G_19 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_19 4
#define RTL_RR_5G_20 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5805+10, 40, 0, 30, 0)
#define RTL_NRR_5G_20 5
#define RTL_RR_5G_21 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_21 3
#define RTL_RR_5G_22 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5720+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_22 4
#define RTL_RR_5G_23 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5640+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_23 4
#define RTL_RR_5G_24 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_24 4
#define RTL_RR_5G_25 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_25 4
#define RTL_RR_5G_26 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_26 4
#define RTL_RR_5G_27 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_27 5
#define RTL_RR_5G_28 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_28 3
#define RTL_RR_5G_29 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_29 3
#define RTL_RR_5G_30 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_30 2
#define RTL_RR_5G_31 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_31 4
#define RTL_RR_5G_32 REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_32 2
#define RTL_RR_5G_33 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5720+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_33 5
#define RTL_RR_5G_34 REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_34 3
#define RTL_RR_5G_35 REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5680-10, 5680+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5700-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_35 3
#define RTL_RR_5G_36 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5720+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_36 4
#define RTL_RR_5G_37 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_37 2
#define RTL_RR_5G_38 REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5550-10, 5720+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_38 2
#define RTL_RR_5G_39 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS)
#define RTL_NRR_5G_39 4
#define RTL_RR_5G_40 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_40 4
#define RTL_RR_5G_41 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_41 4
#define RTL_RR_5G_42 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_42 4
#define RTL_RR_5G_43 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_43 4
#define RTL_RR_5G_44 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5600-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_44 5
#define RTL_RR_5G_45 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_45 3
#define RTL_RR_5G_46 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_46 3
#define RTL_RR_5G_47 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5805+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_47 3
#define RTL_RR_5G_48 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5720+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_48 3
#define RTL_RR_5G_49 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5660+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_49 2
#define RTL_RR_5G_50 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_50 4
#define RTL_RR_5G_51 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5470-10, 5600+10, 40, 0, 30, NL80211_RRF_DFS)
#define RTL_NRR_5G_51 3
#define RTL_RR_5G_52 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5720+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5885+10, 40, 0, 30, 0)
#define RTL_NRR_5G_52 4
#define RTL_RR_5G_53 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5650+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_53 4
#define RTL_RR_5G_54 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5745-10, 5805+10, 40, 0, 30, 0)
#define RTL_NRR_5G_54 2
#define RTL_RR_5G_55 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		      REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		      REG_RULE(5500-10, 5720+10, 40, 0, 30, NL80211_RRF_NO_IR | NL80211_RRF_DFS),\
		      REG_RULE(5745-10, 5885+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_55 4
#define RTL_RR_5G_56 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5580+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5600-10, 5720+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5885+10, 40, 0, 30, 0)
#define RTL_NRR_5G_56 5
#define RTL_RR_5G_57 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5500-10, 5720+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_57 4
#define RTL_RR_5G_58 REG_RULE(5180-10, 5240+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5660-10, 5700+10, 40, 0, 30, NL80211_RRF_NO_IR),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, NL80211_RRF_NO_IR)
#define RTL_NRR_5G_58 4
#define RTL_RR_5G_59 REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5825+10, 40, 0, 30, 0)
#define RTL_NRR_5G_59 2
#define RTL_RR_5G_60 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5720+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5865+10, 40, 0, 30, 0)
#define RTL_NRR_5G_60 4
#define RTL_RR_5G_61 REG_RULE(5180-10, 5240+10, 40, 0, 30, 0),\
		       REG_RULE(5260-10, 5320+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5500-10, 5640+10, 40, 0, 30, NL80211_RRF_DFS),\
		       REG_RULE(5745-10, 5865+10, 40, 0, 30, 0)
#define RTL_NRR_5G_61 4

static struct ieee80211_regdomain rtl_regd_20 = {
	.n_reg_rules = RTL_NRR_2G_01,
	.alpha2 = "20",
	.reg_rules = {
		RTL_RR_2G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_21 = {
	.n_reg_rules = RTL_NRR_2G_02,
	.alpha2 = "21",
	.reg_rules = {
		RTL_RR_2G_02,
	}
};

static struct ieee80211_regdomain rtl_regd_22 = {
	.n_reg_rules = RTL_NRR_2G_03,
	.alpha2 = "22",
	.reg_rules = {
		RTL_RR_2G_03,
	}
};

static struct ieee80211_regdomain rtl_regd_23 = {
	.n_reg_rules = RTL_NRR_2G_04,
	.alpha2 = "23",
	.reg_rules = {
		RTL_RR_2G_04,
	}
};

static struct ieee80211_regdomain rtl_regd_24 = {
	.n_reg_rules = RTL_NRR_2G_05,
	.alpha2 = "24",
	.reg_rules = {
		RTL_RR_2G_05,
	}
};

static struct ieee80211_regdomain rtl_regd_25 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_03,
	.alpha2 = "25",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_03,
	}
};

static struct ieee80211_regdomain rtl_regd_26 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_02,
	.alpha2 = "26",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_02,
	}
};

static struct ieee80211_regdomain rtl_regd_27 = {
	.n_reg_rules = RTL_NRR_2G_04 + RTL_NRR_5G_02,
	.alpha2 = "27",
	.reg_rules = {
		RTL_RR_2G_04, RTL_RR_5G_02,
	}
};

static struct ieee80211_regdomain rtl_regd_28 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_05,
	.alpha2 = "28",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_05,
	}
};

static struct ieee80211_regdomain rtl_regd_29 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_06,
	.alpha2 = "29",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_06,
	}
};

static struct ieee80211_regdomain rtl_regd_30 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_07,
	.alpha2 = "30",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_07,
	}
};

static struct ieee80211_regdomain rtl_regd_31 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_08,
	.alpha2 = "31",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_08,
	}
};

static struct ieee80211_regdomain rtl_regd_32 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_09,
	.alpha2 = "32",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_09,
	}
};

static struct ieee80211_regdomain rtl_regd_33 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_10,
	.alpha2 = "33",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_10,
	}
};

static struct ieee80211_regdomain rtl_regd_34 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_01,
	.alpha2 = "34",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_35 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_03,
	.alpha2 = "35",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_03,
	}
};

static struct ieee80211_regdomain rtl_regd_36 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_04,
	.alpha2 = "36",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_04,
	}
};

static struct ieee80211_regdomain rtl_regd_37 = {
	.n_reg_rules = RTL_NRR_2G_04 + RTL_NRR_5G_10,
	.alpha2 = "37",
	.reg_rules = {
		RTL_RR_2G_04, RTL_RR_5G_10,
	}
};

static struct ieee80211_regdomain rtl_regd_38 = {
	.n_reg_rules = RTL_NRR_2G_04 + RTL_NRR_5G_11,
	.alpha2 = "38",
	.reg_rules = {
		RTL_RR_2G_04, RTL_RR_5G_11,
	}
};

static struct ieee80211_regdomain rtl_regd_39 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_12,
	.alpha2 = "39",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_12,
	}
};

static struct ieee80211_regdomain rtl_regd_40 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_13,
	.alpha2 = "40",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_13,
	}
};

static struct ieee80211_regdomain rtl_regd_41 = {
	.n_reg_rules = RTL_NRR_2G_06,
	.alpha2 = "41",
	.reg_rules = {
		RTL_RR_2G_06,
	}
};

static struct ieee80211_regdomain rtl_regd_42 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_14,
	.alpha2 = "42",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_14,
	}
};

static struct ieee80211_regdomain rtl_regd_43 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_06,
	.alpha2 = "43",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_06,
	}
};

static struct ieee80211_regdomain rtl_regd_44 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_09,
	.alpha2 = "44",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_09,
	}
};

static struct ieee80211_regdomain rtl_regd_45 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_01,
	.alpha2 = "45",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_46 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_15,
	.alpha2 = "46",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_15,
	}
};

static struct ieee80211_regdomain rtl_regd_47 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_10,
	.alpha2 = "47",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_10,
	}
};

static struct ieee80211_regdomain rtl_regd_48 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_07,
	.alpha2 = "48",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_07,
	}
};

static struct ieee80211_regdomain rtl_regd_49 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_06,
	.alpha2 = "49",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_06,
	}
};

static struct ieee80211_regdomain rtl_regd_50 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_16,
	.alpha2 = "50",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_16,
	}
};

static struct ieee80211_regdomain rtl_regd_51 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_09,
	.alpha2 = "51",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_09,
	}
};

static struct ieee80211_regdomain rtl_regd_52 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_17,
	.alpha2 = "52",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_17,
	}
};

static struct ieee80211_regdomain rtl_regd_53 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_18,
	.alpha2 = "53",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_18,
	}
};

static struct ieee80211_regdomain rtl_regd_54 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_15,
	.alpha2 = "54",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_15,
	}
};

static struct ieee80211_regdomain rtl_regd_55 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_01,
	.alpha2 = "55",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_56 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_19,
	.alpha2 = "56",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_19,
	}
};

static struct ieee80211_regdomain rtl_regd_57 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_20,
	.alpha2 = "57",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_20,
	}
};

static struct ieee80211_regdomain rtl_regd_58 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_14,
	.alpha2 = "58",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_14,
	}
};

static struct ieee80211_regdomain rtl_regd_59 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_21,
	.alpha2 = "59",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_21,
	}
};

static struct ieee80211_regdomain rtl_regd_2A = {
	.n_reg_rules = RTL_NRR_2G_02,
	.alpha2 = "2A",
	.reg_rules = {
		RTL_RR_2G_02,
	}
};

static struct ieee80211_regdomain rtl_regd_60 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_09,
	.alpha2 = "60",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_09,
	}
};

static struct ieee80211_regdomain rtl_regd_61 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_01,
	.alpha2 = "61",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_62 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_03,
	.alpha2 = "62",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_03,
	}
};

static struct ieee80211_regdomain rtl_regd_63 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_23,
	.alpha2 = "63",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_23,
	}
};

static struct ieee80211_regdomain rtl_regd_64 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_24,
	.alpha2 = "64",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_24,
	}
};

static struct ieee80211_regdomain rtl_regd_65 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_24,
	.alpha2 = "65",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_24,
	}
};

static struct ieee80211_regdomain rtl_regd_66 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_27,
	.alpha2 = "66",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_27,
	}
};

static struct ieee80211_regdomain rtl_regd_67 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_25,
	.alpha2 = "67",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_25,
	}
};

static struct ieee80211_regdomain rtl_regd_68 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_27,
	.alpha2 = "68",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_27,
	}
};

static struct ieee80211_regdomain rtl_regd_69 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_25,
	.alpha2 = "69",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_25,
	}
};

static struct ieee80211_regdomain rtl_regd_6A = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_25,
	.alpha2 = "6A",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_25,
	}
};

static struct ieee80211_regdomain rtl_regd_6B = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_29,
	.alpha2 = "6B",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_29,
	}
};

static struct ieee80211_regdomain rtl_regd_6C = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_26,
	.alpha2 = "6C",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_26,
	}
};

static struct ieee80211_regdomain rtl_regd_6D = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_28,
	.alpha2 = "6D",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_28,
	}
};

static struct ieee80211_regdomain rtl_regd_6E = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_25,
	.alpha2 = "6E",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_25,
	}
};

static struct ieee80211_regdomain rtl_regd_6F = {
	.n_reg_rules = RTL_NRR_5G_06,
	.alpha2 = "6F",
	.reg_rules = {
		RTL_RR_5G_06,
	}
};

static struct ieee80211_regdomain rtl_regd_70 = {
	.n_reg_rules = RTL_NRR_5G_30,
	.alpha2 = "70",
	.reg_rules = {
		RTL_RR_5G_30,
	}
};

static struct ieee80211_regdomain rtl_regd_71 = {
	.n_reg_rules = RTL_NRR_5G_25,
	.alpha2 = "71",
	.reg_rules = {
		RTL_RR_5G_25,
	}
};

static struct ieee80211_regdomain rtl_regd_72 = {
	.n_reg_rules = RTL_NRR_5G_31,
	.alpha2 = "72",
	.reg_rules = {
		RTL_RR_5G_31,
	}
};

static struct ieee80211_regdomain rtl_regd_73 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_01,
	.alpha2 = "73",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_74 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_19,
	.alpha2 = "74",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_19,
	}
};

static struct ieee80211_regdomain rtl_regd_75 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_32,
	.alpha2 = "75",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_32,
	}
};

static struct ieee80211_regdomain rtl_regd_2B = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_33,
	.alpha2 = "2B",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_33,
	}
};

static struct ieee80211_regdomain rtl_regd_76 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_22,
	.alpha2 = "76",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_22,
	}
};

static struct ieee80211_regdomain rtl_regd_77 = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_34,
	.alpha2 = "77",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_34,
	}
};

static struct ieee80211_regdomain rtl_regd_78 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_35,
	.alpha2 = "78",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_35,
	}
};

static struct ieee80211_regdomain rtl_regd_79 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_02,
	.alpha2 = "79",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_02,
	}
};

static struct ieee80211_regdomain rtl_regd_2C = {
	.n_reg_rules = RTL_NRR_2G_02,
	.alpha2 = "2C",
	.reg_rules = {
		RTL_RR_2G_02,
	}
};

static struct ieee80211_regdomain rtl_regd_2D = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_22,
	.alpha2 = "2D",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_22,
	}
};

static struct ieee80211_regdomain rtl_regd_2E = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_37,
	.alpha2 = "2E",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_37,
	}
};

static struct ieee80211_regdomain rtl_regd_2F = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_38,
	.alpha2 = "2F",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_38,
	}
};

static struct ieee80211_regdomain rtl_regd_3A = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_02,
	.alpha2 = "3A",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_02,
	}
};

static struct ieee80211_regdomain rtl_regd_3B = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_01,
	.alpha2 = "3B",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_3C = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_10,
	.alpha2 = "3C",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_10,
	}
};

static struct ieee80211_regdomain rtl_regd_3D = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_15,
	.alpha2 = "3D",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_15,
	}
};

static struct ieee80211_regdomain rtl_regd_3E = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_03,
	.alpha2 = "3E",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_03,
	}
};

static struct ieee80211_regdomain rtl_regd_3F = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_22,
	.alpha2 = "3F",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_22,
	}
};

static struct ieee80211_regdomain rtl_regd_4A = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_33,
	.alpha2 = "4A",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_33,
	}
};

static struct ieee80211_regdomain rtl_regd_4B = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_22,
	.alpha2 = "4B",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_22,
	}
};

static struct ieee80211_regdomain rtl_regd_4C = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_28,
	.alpha2 = "4C",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_28,
	}
};

static struct ieee80211_regdomain rtl_regd_4D = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_01,
	.alpha2 = "4D",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_01,
	}
};

static struct ieee80211_regdomain rtl_regd_4E = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_42,
	.alpha2 = "4E",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_42,
	}
};

static struct ieee80211_regdomain rtl_regd_4F = {
	.n_reg_rules = RTL_NRR_5G_43,
	.alpha2 = "4F",
	.reg_rules = {
		RTL_RR_5G_43,
	}
};

static struct ieee80211_regdomain rtl_regd_5A = {
	.n_reg_rules = RTL_NRR_5G_44,
	.alpha2 = "5A",
	.reg_rules = {
		RTL_RR_5G_44,
	}
};

static struct ieee80211_regdomain rtl_regd_5B = {
	.n_reg_rules = RTL_NRR_5G_45,
	.alpha2 = "5B",
	.reg_rules = {
		RTL_RR_5G_45,
	}
};

static struct ieee80211_regdomain rtl_regd_5C = {
	.n_reg_rules = RTL_NRR_5G_43,
	.alpha2 = "5C",
	.reg_rules = {
		RTL_RR_5G_43,
	}
};

static struct ieee80211_regdomain rtl_regd_5D = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_08,
	.alpha2 = "5D",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_08,
	}
};

static struct ieee80211_regdomain rtl_regd_5E = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_03,
	.alpha2 = "5E",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_03,
	}
};

static struct ieee80211_regdomain rtl_regd_5F = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_47,
	.alpha2 = "5F",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_47,
	}
};

static struct ieee80211_regdomain rtl_regd_7A = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_28,
	.alpha2 = "7A",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_28,
	}
};

static struct ieee80211_regdomain rtl_regd_7B = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_46,
	.alpha2 = "7B",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_46,
	}
};

static struct ieee80211_regdomain rtl_regd_7C = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_47,
	.alpha2 = "7C",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_47,
	}
};

static struct ieee80211_regdomain rtl_regd_7D = {
	.n_reg_rules = RTL_NRR_2G_04 + RTL_NRR_5G_48,
	.alpha2 = "7D",
	.reg_rules = {
		RTL_RR_2G_04, RTL_RR_5G_48,
	}
};

static struct ieee80211_regdomain rtl_regd_7E = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_48,
	.alpha2 = "7E",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_48,
	}
};

static struct ieee80211_regdomain rtl_regd_00 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_49,
	.alpha2 = "00",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_49,
	}
};

static struct ieee80211_regdomain rtl_regd_01 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_50,
	.alpha2 = "01",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_50,
	}
};

static struct ieee80211_regdomain rtl_regd_02 = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_07,
	.alpha2 = "02",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_07,
	}
};

static struct ieee80211_regdomain rtl_regd_03 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_33,
	.alpha2 = "03",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_33,
	}
};

static struct ieee80211_regdomain rtl_regd_04 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_51,
	.alpha2 = "04",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_51,
	}
};

static struct ieee80211_regdomain rtl_regd_05 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_06,
	.alpha2 = "05",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_06,
	}
};

static struct ieee80211_regdomain rtl_regd_06 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_07,
	.alpha2 = "06",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_07,
	}
};

static struct ieee80211_regdomain rtl_regd_07 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_23,
	.alpha2 = "07",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_23,
	}
};

static struct ieee80211_regdomain rtl_regd_08 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_21,
	.alpha2 = "08",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_21,
	}
};

static struct ieee80211_regdomain rtl_regd_09 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_17,
	.alpha2 = "09",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_17,
	}
};

static struct ieee80211_regdomain rtl_regd_1B = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_52,
	.alpha2 = "1B",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_52,
	}
};

static struct ieee80211_regdomain rtl_regd_1C = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_53,
	.alpha2 = "1C",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_53,
	}
};

static struct ieee80211_regdomain rtl_regd_0B = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_22,
	.alpha2 = "0B",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_22,
	}
};

static struct ieee80211_regdomain rtl_regd_0C = {
	.n_reg_rules = RTL_NRR_2G_03 + RTL_NRR_5G_54,
	.alpha2 = "0C",
	.reg_rules = {
		RTL_RR_2G_03, RTL_RR_5G_54,
	}
};

static struct ieee80211_regdomain rtl_regd_0D = {
	.n_reg_rules = RTL_NRR_2G_04 + RTL_NRR_5G_14,
	.alpha2 = "0D",
	.reg_rules = {
		RTL_RR_2G_04, RTL_RR_5G_14,
	}
};

static struct ieee80211_regdomain rtl_regd_0E = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_57,
	.alpha2 = "0E",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_57,
	}
};

static struct ieee80211_regdomain rtl_regd_0F = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_58,
	.alpha2 = "0F",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_58,
	}
};

static struct ieee80211_regdomain rtl_regd_10 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_56,
	.alpha2 = "10",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_56,
	}
};

static struct ieee80211_regdomain rtl_regd_11 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_59,
	.alpha2 = "11",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_59,
	}
};

static struct ieee80211_regdomain rtl_regd_12 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_60,
	.alpha2 = "12",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_60,
	}
};

static struct ieee80211_regdomain rtl_regd_13 = {
	.n_reg_rules = RTL_NRR_2G_02 + RTL_NRR_5G_61,
	.alpha2 = "13",
	.reg_rules = {
		RTL_RR_2G_02, RTL_RR_5G_61,
	}
};

static struct ieee80211_regdomain rtl_regd_7F = {
	.n_reg_rules = RTL_NRR_2G_01 + RTL_NRR_5G_55,
	.alpha2 = "7F",
	.reg_rules = {
		RTL_RR_2G_01, RTL_RR_5G_55,
	}
};

static struct chplan_regd chplan_regd_map[] = {
	CHPLAN_REGD_ENT(0x20, &rtl_regd_20),
	CHPLAN_REGD_ENT(0x21, &rtl_regd_21),
	CHPLAN_REGD_ENT(0x22, &rtl_regd_22),
	CHPLAN_REGD_ENT(0x23, &rtl_regd_23),
	CHPLAN_REGD_ENT(0x24, &rtl_regd_24),
	CHPLAN_REGD_ENT(0x25, &rtl_regd_25),
	CHPLAN_REGD_ENT(0x26, &rtl_regd_26),
	CHPLAN_REGD_ENT(0x27, &rtl_regd_27),
	CHPLAN_REGD_ENT(0x28, &rtl_regd_28),
	CHPLAN_REGD_ENT(0x29, &rtl_regd_29),
	CHPLAN_REGD_ENT(0x30, &rtl_regd_30),
	CHPLAN_REGD_ENT(0x31, &rtl_regd_31),
	CHPLAN_REGD_ENT(0x32, &rtl_regd_32),
	CHPLAN_REGD_ENT(0x33, &rtl_regd_33),
	CHPLAN_REGD_ENT(0x34, &rtl_regd_34),
	CHPLAN_REGD_ENT(0x35, &rtl_regd_35),
	CHPLAN_REGD_ENT(0x36, &rtl_regd_36),
	CHPLAN_REGD_ENT(0x37, &rtl_regd_37),
	CHPLAN_REGD_ENT(0x38, &rtl_regd_38),
	CHPLAN_REGD_ENT(0x39, &rtl_regd_39),
	CHPLAN_REGD_ENT(0x40, &rtl_regd_40),
	CHPLAN_REGD_ENT(0x41, &rtl_regd_41),
	CHPLAN_REGD_ENT(0x42, &rtl_regd_42),
	CHPLAN_REGD_ENT(0x43, &rtl_regd_43),
	CHPLAN_REGD_ENT(0x44, &rtl_regd_44),
	CHPLAN_REGD_ENT(0x45, &rtl_regd_45),
	CHPLAN_REGD_ENT(0x46, &rtl_regd_46),
	CHPLAN_REGD_ENT(0x47, &rtl_regd_47),
	CHPLAN_REGD_ENT(0x48, &rtl_regd_48),
	CHPLAN_REGD_ENT(0x49, &rtl_regd_49),
	CHPLAN_REGD_ENT(0x50, &rtl_regd_50),
	CHPLAN_REGD_ENT(0x51, &rtl_regd_51),
	CHPLAN_REGD_ENT(0x52, &rtl_regd_52),
	CHPLAN_REGD_ENT(0x53, &rtl_regd_53),
	CHPLAN_REGD_ENT(0x54, &rtl_regd_54),
	CHPLAN_REGD_ENT(0x55, &rtl_regd_55),
	CHPLAN_REGD_ENT(0x56, &rtl_regd_56),
	CHPLAN_REGD_ENT(0x57, &rtl_regd_57),
	CHPLAN_REGD_ENT(0x58, &rtl_regd_58),
	CHPLAN_REGD_ENT(0x59, &rtl_regd_59),
	CHPLAN_REGD_ENT(0x2A, &rtl_regd_2A),
	CHPLAN_REGD_ENT(0x60, &rtl_regd_60),
	CHPLAN_REGD_ENT(0x61, &rtl_regd_61),
	CHPLAN_REGD_ENT(0x62, &rtl_regd_62),
	CHPLAN_REGD_ENT(0x63, &rtl_regd_63),
	CHPLAN_REGD_ENT(0x64, &rtl_regd_64),
	CHPLAN_REGD_ENT(0x65, &rtl_regd_65),
	CHPLAN_REGD_ENT(0x66, &rtl_regd_66),
	CHPLAN_REGD_ENT(0x67, &rtl_regd_67),
	CHPLAN_REGD_ENT(0x68, &rtl_regd_68),
	CHPLAN_REGD_ENT(0x69, &rtl_regd_69),
	CHPLAN_REGD_ENT(0x6A, &rtl_regd_6A),
	CHPLAN_REGD_ENT(0x6B, &rtl_regd_6B),
	CHPLAN_REGD_ENT(0x6C, &rtl_regd_6C),
	CHPLAN_REGD_ENT(0x6D, &rtl_regd_6D),
	CHPLAN_REGD_ENT(0x6E, &rtl_regd_6E),
	CHPLAN_REGD_ENT(0x6F, &rtl_regd_6F),
	CHPLAN_REGD_ENT(0x70, &rtl_regd_70),
	CHPLAN_REGD_ENT(0x71, &rtl_regd_71),
	CHPLAN_REGD_ENT(0x72, &rtl_regd_72),
	CHPLAN_REGD_ENT(0x73, &rtl_regd_73),
	CHPLAN_REGD_ENT(0x74, &rtl_regd_74),
	CHPLAN_REGD_ENT(0x75, &rtl_regd_75),
	CHPLAN_REGD_ENT(0x2B, &rtl_regd_2B),
	CHPLAN_REGD_ENT(0x76, &rtl_regd_76),
	CHPLAN_REGD_ENT(0x77, &rtl_regd_77),
	CHPLAN_REGD_ENT(0x78, &rtl_regd_78),
	CHPLAN_REGD_ENT(0x79, &rtl_regd_79),
	CHPLAN_REGD_ENT(0x2C, &rtl_regd_2C),
	CHPLAN_REGD_ENT(0x2D, &rtl_regd_2D),
	CHPLAN_REGD_ENT(0x2E, &rtl_regd_2E),
	CHPLAN_REGD_ENT(0x2F, &rtl_regd_2F),
	CHPLAN_REGD_ENT(0x3A, &rtl_regd_3A),
	CHPLAN_REGD_ENT(0x3B, &rtl_regd_3B),
	CHPLAN_REGD_ENT(0x3C, &rtl_regd_3C),
	CHPLAN_REGD_ENT(0x3D, &rtl_regd_3D),
	CHPLAN_REGD_ENT(0x3E, &rtl_regd_3E),
	CHPLAN_REGD_ENT(0x3F, &rtl_regd_3F),
	CHPLAN_REGD_ENT(0x4A, &rtl_regd_4A),
	CHPLAN_REGD_ENT(0x4B, &rtl_regd_4B),
	CHPLAN_REGD_ENT(0x4C, &rtl_regd_4C),
	CHPLAN_REGD_ENT(0x4D, &rtl_regd_4D),
	CHPLAN_REGD_ENT(0x4E, &rtl_regd_4E),
	CHPLAN_REGD_ENT(0x4F, &rtl_regd_4F),
	CHPLAN_REGD_ENT(0x5A, &rtl_regd_5A),
	CHPLAN_REGD_ENT(0x5B, &rtl_regd_5B),
	CHPLAN_REGD_ENT(0x5C, &rtl_regd_5C),
	CHPLAN_REGD_ENT(0x5D, &rtl_regd_5D),
	CHPLAN_REGD_ENT(0x5E, &rtl_regd_5E),
	CHPLAN_REGD_ENT(0x5F, &rtl_regd_5F),
	CHPLAN_REGD_ENT(0x7A, &rtl_regd_7A),
	CHPLAN_REGD_ENT(0x7B, &rtl_regd_7B),
	CHPLAN_REGD_ENT(0x7C, &rtl_regd_7C),
	CHPLAN_REGD_ENT(0x7D, &rtl_regd_7D),
	CHPLAN_REGD_ENT(0x7E, &rtl_regd_7E),
	CHPLAN_REGD_ENT(0x00, &rtl_regd_00),
	CHPLAN_REGD_ENT(0x01, &rtl_regd_01),
	CHPLAN_REGD_ENT(0x02, &rtl_regd_02),
	CHPLAN_REGD_ENT(0x03, &rtl_regd_03),
	CHPLAN_REGD_ENT(0x04, &rtl_regd_04),
	CHPLAN_REGD_ENT(0x05, &rtl_regd_05),
	CHPLAN_REGD_ENT(0x06, &rtl_regd_06),
	CHPLAN_REGD_ENT(0x07, &rtl_regd_07),
	CHPLAN_REGD_ENT(0x08, &rtl_regd_08),
	CHPLAN_REGD_ENT(0x09, &rtl_regd_09),
	CHPLAN_REGD_ENT(0x1B, &rtl_regd_1B),
	CHPLAN_REGD_ENT(0x1C, &rtl_regd_1C),
	CHPLAN_REGD_ENT(0x0B, &rtl_regd_0B),
	CHPLAN_REGD_ENT(0x0C, &rtl_regd_0C),
	CHPLAN_REGD_ENT(0x0D, &rtl_regd_0D),
	CHPLAN_REGD_ENT(0x0E, &rtl_regd_0E),
	CHPLAN_REGD_ENT(0x0F, &rtl_regd_0F),
	CHPLAN_REGD_ENT(0x10, &rtl_regd_10),
	CHPLAN_REGD_ENT(0x11, &rtl_regd_11),
	CHPLAN_REGD_ENT(0x12, &rtl_regd_12),
	CHPLAN_REGD_ENT(0x13, &rtl_regd_13),
	CHPLAN_REGD_ENT(0x7F, &rtl_regd_7F)     /* Word wide */
};

static struct ieee80211_regdomain *_rtl_reg_get_regd(u8 chplan)
{
	struct chplan_regd *map = NULL;
	u16 map_sz = 0;
	int i = 0;

	map_sz = sizeof(chplan_regd_map) / sizeof(struct chplan_regd);
	for (i = 0; i < map_sz; i++) {
		if (chplan == chplan_regd_map[i].chplan) {
			map = (struct chplan_regd *)&chplan_regd_map[i];
			break;
		}
	}

	if (map) {
		return (struct ieee80211_regdomain *)map->regd;
	} else {
		return &rtl_regd_7F;
	}
}

static void _rtl_reg_set_country_code(struct wiphy *wiphy, u8 *country)
{
	int ret = 0;
	struct country_code_table_t table;
	const struct ieee80211_regdomain *regd = NULL;

	ret = llhw_wifi_set_country_code(country);
	if (ret == 0) {
		llhw_wifi_get_country_code(&table);
		regd = _rtl_reg_get_regd(table.channel_plan);
		memcpy((void *)&regd->alpha2[0], &table.char2[0], 2);
		wiphy_apply_custom_regulatory(wiphy, regd);
		wiphy->regd = regd;
	} else {
		dev_err(global_idev.fullmac_dev, "%s set country %s failed\n", __func__, country);
	}
}

void rtw_reg_notifier(struct wiphy *wiphy, struct regulatory_request *request)
{
	switch (request->initiator) {
	case NL80211_REGDOM_SET_BY_USER:
	case NL80211_REGDOM_SET_BY_DRIVER:
	case NL80211_REGDOM_SET_BY_CORE:
	case NL80211_REGDOM_SET_BY_COUNTRY_IE:
		_rtl_reg_set_country_code(wiphy, request->alpha2);
		break;
	default:
		/* do nothing */
		break;
	}

	return;
}

void rtw_regd_deinit(void)
{
	struct wiphy *wiphy = global_idev.pwiphy_global;

	wiphy->regd = NULL;
}

int rtw_regd_init(void)
{
	struct wiphy *wiphy = global_idev.pwiphy_global;
	struct country_code_table_t table;
	char ww_char2[2] = {'0', '0'};
	const struct ieee80211_regdomain *regd = NULL;
	int ret = 0;

	ret = llhw_wifi_get_country_code(&table);
	if (ret < 0) {
		dev_err(global_idev.fullmac_dev, "%s get country code from firmware failed (%d)\n", __func__, ret);
		return ret;
	}

	regd = _rtl_reg_get_regd(table.channel_plan);
	if ((table.char2[0] == '0') && (table.char2[1] == '0')) {
		memcpy((void *)&regd->alpha2[0], &ww_char2[0], 2);
		dev_info(global_idev.fullmac_dev,
				 "Willis-Efuse_Channel_Plan Version: %s, world wide.\n",
				 CH_PLAN_VERSION);
	} else {
		memcpy((void *)&regd->alpha2[0], &table.char2[0], 2);
		dev_info(global_idev.fullmac_dev,
				 "Willis-Efuse_Channel_Plan Version: %s, country: %c%c.\n",
				 CH_PLAN_VERSION, table.char2[0], table.char2[1]);
	}

	wiphy->regulatory_flags |= REGULATORY_CUSTOM_REG;
	wiphy->regulatory_flags |= REGULATORY_WIPHY_SELF_MANAGED;
	wiphy->regulatory_flags &= ~REGULATORY_STRICT_REG;
	wiphy->regulatory_flags &= ~REGULATORY_DISABLE_BEACON_HINTS;

	wiphy_apply_custom_regulatory(wiphy, regd);
	wiphy->regd = regd;

	/* add reg_notifier to set channel plan by user.
	because _rtl_reg_set_country_code must be called after wifi on, rtw_regd_init assignement should be moved to rtw_regd_init */
	wiphy->reg_notifier = rtw_reg_notifier;

	return 0;
}
