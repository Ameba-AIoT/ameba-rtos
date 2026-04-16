/*
 *  Bluetooth support for Realtek devices
 *
 *  Copyright (C) 2015 Endless Mobile, Inc.
 *  Copyright (C) 2018 Realtek Semiconductor Corp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

struct btsdio_data {
	struct hci_dev   *hdev;
	struct sdio_func *func;

	struct work_struct work;
	struct sk_buff_head txq;
	int int_count;

	struct task_struct *thread;
	wait_queue_head_t wq;

	spinlock_t lock;

};

#define VERSION "0.12.40fa29d.20230313-112552"
#define ALIGN_UP(a, size)                   ((a+size-1) & (~(size-1)))

// #define BT_OP(ogf, ocf)                     ((ocf) | ((ogf) << 10))
// #define BT_OGF_VS                           0x3F

// #define BT_HCI_CMD_VS_BT_ON                 BT_OP(BT_OGF_VS, 0x0201)
// #define BT_HCI_CMD_VS_BT_OFF                BT_OP(BT_OGF_VS, 0x0202)
// #define BT_HCI_CMD_VS_BT_HOST_SLEEP         BT_OP(BT_OGF_VS, 0x0203)
// #define BT_HCI_CMD_VS_BT_HOST_RESUME        BT_OP(BT_OGF_VS, 0x0204)

#define BTSDIO_DMA_ALIGN                    8
#define HCI_SDIO_PKT_HDR_LEN                4

#define REG_RDAT                            0xE000      /* Receiver Data FIFO */
#define REG_TDAT                            0x8000      /* Transmitter Data FIFO */
#define REG_HISR                            0x18        /* Host Interrupt Status Register */
#define HCI_HISR_RX_REQUEST                 BIT(0)      /* HISR BIT(0) : RX_REQUEST INTRRUPT */
#define REG_FREE_TXBD_NUM                   0x20        /* FREE_TXBD_NUM */
#define REG_BT_CTRL                         0x40        /* BT Control register */
#define REG_CPU_IND                         0x87        /* CPU Indication */
