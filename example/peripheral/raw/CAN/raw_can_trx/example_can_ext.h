/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CAN_EXT_H_
#define _CAN_EXT_H_

#define CAN0_TX  _PA_25
#define CAN0_RX  _PA_26
#define CAN0_STB _PA_3

#define CAN1_TX  _PB_17
#define CAN1_RX  _PB_18
#define CAN1_STB _PB_19

int example_raw_can_trx(void);
#endif