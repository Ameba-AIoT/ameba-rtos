#include "ameba_soc.h"
#include "hci_platform.h"
#include "osif.h"
#include "bt_inic.h"
#include "bt_debug.h"
#include "hci_if_inic.h"

#define BT_INIC_SDIO_IRQ_PRIO           5
#define BT_INIC_SDIO_DMA_ALIGN_4        4
#define BT_INIC_SDIO_RX_BD_FREE_TH      5

#define BT_INIC_SDIO_TX_BD_NUM          5
#define BT_INIC_SDIO_RX_BD_NUM          20
#define BT_INIC_SDIO_TX_BUFSZ           (1024 + 64) /* it shall be 64 bytes aligned */
#define BT_INIC_SDIO_TX_BUF_SZ_UNIT     64

#define HCI_SDIO_PKT_LEN_FIELD          3
#define HCI_SDIO_PKT_SERVICE_ID_FIELD   1
#define HCI_SDIO_PKT_HDR_LEN            (HCI_SDIO_PKT_LEN_FIELD + HCI_SDIO_PKT_SERVICE_ID_FIELD)

struct bt_inic_sdio_tx_bd {
	u32 address;
};

/* default: GROUP1 */
static const u8 SDIO_DEV_PAD[6] = {
	_PA_18,  // CLK
	_PA_19,  // CMD
	_PA_21,  // D3
	_PA_4,   // D2
	_PA_20,  // D1
	_PA_5    // D0
};

struct bt_inic_sdio_rx_bd {
	u32 data_size: 14; /* bit[13:0], RX Data Size, Maximum 16384-1 */
	u32 ls: 1;         /* bit[14], is the Last Segment ? */
	u32 fs: 1;         /* bit[15], is the First Segment ? */
	u32 rsvd: 16;      /* bit[31:16], Reserved */
	u32 address;       /* The RX buffer physical address, it must be 4-bytes aligned */
};

struct bt_inic_sdio_priv {
	/* SDIO TX(host->device) */
	struct bt_inic_sdio_tx_bd *tx_bd;                     /* TX buffer descriptor */
	u8 *tx_buf[BT_INIC_SDIO_TX_BD_NUM]; /* TX buffer is alloced when sdio initialized, it is static and len is fixed */
	u16 txbd_r_ptr;                /* TX BD read index */
	u16 txbd_w_ptr;                /* TX BD write index */
	u8 is_tx_overflow;             /* is TX data overflowed TX buffer ?  */

	/* SDIO RX(device->host) */
	struct bt_inic_sdio_rx_bd *rx_bd;               /* RX buffer descriptor */
	u8 *rx_buf[BT_INIC_SDIO_RX_BD_NUM]; /* RX buffer is alloced when ready to send rx data, it is dynamic and len is changed with rx data */
	u16 rxbd_r_ptr;                /* RX BD read index */
	u16 rxbd_w_ptr;                /* RX BD write index */
	void *rxbd_mutex;                   /* RX BD mutex, used to protect RXBD when multi-thread operation*/

	/* SDIO irq task */
	void *irq_sem;                      /* SDIO irq task semaphore */
	void *sdio_task_hdl;                /* SDIO irq task handle */
};

static struct bt_inic_sdio_priv g_bt_inic_sdio = {0};

void bt_inic_send_to_host(u8 type, u8 *pdata, u32 len)
{
	u16 rxbd_free_num = 0;
	u16 sdiodata_len = len + HCI_SDIO_PKT_HDR_LEN;

	osif_mutex_take(g_bt_inic_sdio.rxbd_mutex, 0xFFFFFFFF);
	if (g_bt_inic_sdio.rxbd_r_ptr != g_bt_inic_sdio.rxbd_w_ptr) {
		rxbd_free_num = ((g_bt_inic_sdio.rxbd_r_ptr + BT_INIC_SDIO_RX_BD_NUM) - g_bt_inic_sdio.rxbd_w_ptr)
						% BT_INIC_SDIO_RX_BD_NUM;
		if (rxbd_free_num <= 1) {
			BT_LOGE("No available RX BD, r_ptr:%d, w_ptr:%d\r\n",
					g_bt_inic_sdio.rxbd_r_ptr, g_bt_inic_sdio.rxbd_w_ptr);
			osif_mutex_give(g_bt_inic_sdio.rxbd_mutex);
			return;
		}
	}
	// BT_LOGA("%s r_ptr:%d, w_ptr:%d\r\n", __func__, g_bt_inic_sdio.rxbd_r_ptr, g_bt_inic_sdio.rxbd_w_ptr);

	g_bt_inic_sdio.rx_buf[g_bt_inic_sdio.rxbd_w_ptr] =
		osif_mem_aligned_alloc(0, sdiodata_len, BT_INIC_SDIO_DMA_ALIGN_4);
	g_bt_inic_sdio.rx_bd[g_bt_inic_sdio.rxbd_w_ptr].address = (u32)g_bt_inic_sdio.rx_buf[g_bt_inic_sdio.rxbd_w_ptr];
	g_bt_inic_sdio.rx_bd[g_bt_inic_sdio.rxbd_w_ptr].fs = 1;
	g_bt_inic_sdio.rx_bd[g_bt_inic_sdio.rxbd_w_ptr].ls = 1;
	g_bt_inic_sdio.rx_bd[g_bt_inic_sdio.rxbd_w_ptr].data_size = sdiodata_len;

	uint8_t *sdio_rxdata = (uint8_t *)g_bt_inic_sdio.rx_bd[g_bt_inic_sdio.rxbd_w_ptr].address;
	sdio_rxdata[0] = sdiodata_len & 0xff; /* Packet Length */
	sdio_rxdata[1] = (sdiodata_len >> 8) & 0xff;
	sdio_rxdata[2] = 0;
	sdio_rxdata[3] = type; /* Service ID */
	memcpy(&sdio_rxdata[4], pdata, len); /* Interface Data */
	// BT_DUMPA("HOST RX: ", sdio_rxdata, (uint16_t)sdiodata_len);
	DCache_CleanInvalidate(g_bt_inic_sdio.rx_bd[g_bt_inic_sdio.rxbd_w_ptr].address, sdiodata_len);
	DCache_Clean((u32)&g_bt_inic_sdio.rx_bd[g_bt_inic_sdio.rxbd_w_ptr], sizeof(struct bt_inic_sdio_rx_bd));

	g_bt_inic_sdio.rxbd_w_ptr++;
	if (g_bt_inic_sdio.rxbd_w_ptr >= BT_INIC_SDIO_RX_BD_NUM) {
		g_bt_inic_sdio.rxbd_w_ptr = 0;
	}

	SDIO_RXBD_WPTR_Set(SDIO_BT, g_bt_inic_sdio.rxbd_w_ptr);
	SDIO_RxReq(SDIO_BT);
	osif_mutex_give(g_bt_inic_sdio.rxbd_mutex);
}

u32 bt_inic_sdio_irq(void *pdata)
{
	(void) pdata;
	InterruptDis(SDIO_BT_IRQ);
	osif_sem_give(g_bt_inic_sdio.irq_sem);
	return 0;
}

void bt_inic_sdio_rxfifo_send_done_hdl(void)
{
	u16 rxbd_r_ptr_old = g_bt_inic_sdio.rxbd_r_ptr;

	g_bt_inic_sdio.rxbd_r_ptr = SDIO_RXBD_RPTR_Get(SDIO_BT);
	while (rxbd_r_ptr_old != g_bt_inic_sdio.rxbd_r_ptr) {
		osif_mem_aligned_free((void *)g_bt_inic_sdio.rx_bd[rxbd_r_ptr_old].address);
		g_bt_inic_sdio.rx_buf[rxbd_r_ptr_old] = NULL;
		memset(&g_bt_inic_sdio.rx_bd[rxbd_r_ptr_old], 0, sizeof(struct bt_inic_sdio_rx_bd));
		rxbd_r_ptr_old++;
		if (rxbd_r_ptr_old >= BT_INIC_SDIO_RX_BD_NUM) {
			rxbd_r_ptr_old = 0;
		}
	}
}

void bt_inic_sdio_txfifo_data_ready_hdl(void)
{
	u32 reg_val = 0;
	u8 *read_buf = NULL;
	u32 pkt_len = 0;

	g_bt_inic_sdio.txbd_w_ptr = SDIO_TXBD_WPTR_Get(SDIO_BT);
	if (g_bt_inic_sdio.txbd_r_ptr == g_bt_inic_sdio.txbd_w_ptr) {
		if (g_bt_inic_sdio.is_tx_overflow) {
			g_bt_inic_sdio.is_tx_overflow = 0;
			reg_val = SDIO_DMA_CTRL_Get(SDIO_BT);
			BT_LOGE("Error: SDIO TX overflow, Reg DMA_CTRL: 0x%x\r\n", (u8)reg_val);
		} else {
			BT_LOGE("Error: SDIO TX data read false triggered, txbd_w_ptr: %d\r\n",
					g_bt_inic_sdio.txbd_w_ptr);
		}
		return;
	}

	do {
		// BT_LOGA("%s: txbd_w_ptr:%d, txbd_r_ptr:%d \r\n", __func__,
		//      g_bt_inic_sdio.txbd_w_ptr, g_bt_inic_sdio.txbd_r_ptr);
		read_buf = (u8 *)g_bt_inic_sdio.tx_bd[g_bt_inic_sdio.txbd_r_ptr].address;
		DCache_Invalidate((u32)read_buf, BT_INIC_SDIO_TX_BUFSZ);
		pkt_len = *((uint16_t *)read_buf);
		if (pkt_len > BT_INIC_SDIO_TX_BUFSZ) {
			BT_LOGE("Error: SDIO TX packet size is over tx buf size\r\n");
		}
		// BT_DUMPA("HOST TX: ",read_buf, pkt_len);
		bt_inic_recv_from_host(*(read_buf + HCI_SDIO_PKT_LEN_FIELD), read_buf + HCI_SDIO_PKT_HDR_LEN, pkt_len - HCI_SDIO_PKT_HDR_LEN);


		g_bt_inic_sdio.txbd_r_ptr++;
		if (g_bt_inic_sdio.txbd_r_ptr >= BT_INIC_SDIO_TX_BD_NUM) {
			g_bt_inic_sdio.txbd_r_ptr = 0;
		}

		SDIO_TXBD_RPTR_Set(SDIO_BT, g_bt_inic_sdio.txbd_r_ptr);
	} while (g_bt_inic_sdio.txbd_r_ptr != g_bt_inic_sdio.txbd_w_ptr);
}

static void bt_inic_sdio_irq_thread_hdl(void *pdata)
{
	(void) pdata;
	uint16_t int_status = 0;

	while (1) {
		osif_sem_take(g_bt_inic_sdio.irq_sem, 0xFFFFFFFF);

		int_status = SDIO_INTStatus(SDIO_BT);
		SDIO_INTClearAll(SDIO_BT);
		InterruptEn(SDIO_BT_IRQ, BT_INIC_SDIO_IRQ_PRIO);

		/* BT_LOGA("%s: int_status=0x%x \r\n", __func__, int_status); */

		if (int_status & SDIO_BT_BIT_C2H_DMA_OK) {
			// BT_LOGA("######## [IRQ handler] BIT_C2H_DMA_OK #######\r\n");
			SDIO_INTConfig(SDIO_BT, SDIO_BT_BIT_C2H_DMA_OK, DISABLE);
			bt_inic_sdio_rxfifo_send_done_hdl(); /* Process HOST Tx to Device data */
			// BT_LOGA("HOST RX Packet From Device OK\r\n");
			SDIO_INTConfig(SDIO_BT, SDIO_BT_BIT_C2H_DMA_OK, ENABLE);
		}

		if (int_status & SDIO_BT_BIT_H2C_DMA_OK) {
			// BT_LOGA("######## [IRQ handler] BIT_H2C_DMA_OK #######\r\n");
			SDIO_INTConfig(SDIO_BT, SDIO_BT_BIT_H2C_DMA_OK, DISABLE);
			/* printf("###### txfifo data start recv #######\r\n"); */
			bt_inic_sdio_txfifo_data_ready_hdl(); /* Process HOST Tx to Device data */
			// BT_LOGA("HOST TX Packet TO Device OK\r\n");
			SDIO_INTConfig(SDIO_BT, SDIO_BT_BIT_H2C_DMA_OK, ENABLE);
		}

		if (int_status & SDIO_BT_BIT_TXBD_H2C_OVF) {
			// BT_LOGA("######## [IRQ handler] BIT_TXFIFO_H2C_OVF \r\n");
			g_bt_inic_sdio.is_tx_overflow = 1;
		}

		if (int_status & SDIO_BT_BIT_SDIO_CS_RDY) {
			// BT_LOGA("######### [IRQ handler] BIT_SDIO_CS_RDY #########\r\n");
			/* g_sdio_status = SDIO_STATUS_CARD_RECG; */
		}

		if (int_status & SDIO_BT_BIT_SDIO_CS_RST) {
			// BT_LOGA("######### [IRQ handler] BIT_SDIO_CS_RST #########\r\n");
		}

		if (int_status & SDIO_BT_BIT_HOST_WAKE_CPU_INT) {
			// BT_LOGA("######### [IRQ handler] BIT_HOST_WAKE_CPU_INT #########\r\n");
		}

		if (int_status & SDIO_BT_BIT_H2C_SUS_REQ) {
			// BT_LOGA("######### [IRQ handler] BIT_H2C_SUS_REQ #########\r\n");
		}

		if (int_status & SDIO_BT_BIT_CCCR_IOE2_SET) {
			// BT_LOGA("######### [IRQ handler] BIT_CCCR_IOE2_SET #########\r\n");
		}

		if (int_status & SDIO_BT_BIT_CCCR_IOE2_CLR) {
			// BT_LOGA("######### [IRQ handler] BIT_CCCR_IOE2_CLR #########\r\n");
		}
	}
}

static void SPDIO_Board_Init(void)
{
	u8 idx;

	/* Pinmux function and Pad control */
	for (idx = 0; idx < 6; idx++) {
		PAD_PullCtrl(SDIO_DEV_PAD[idx], GPIO_PuPd_UP);
		Pinmux_Config(SDIO_DEV_PAD[idx], PINMUX_FUNCTION_SDIO_SLV);
		// BT_LOGA("SDIO_DEV: P%c%d\n", 'A' + PORT_NUM(SDIO_DEV_PAD[idx]),
		//      PIN_NUM(SDIO_DEV_PAD[idx]));
	}

	/* SDIO function enable and clock enable*/
	RCC_PeriphClockCmd(APBPeriph_SDD, APBPeriph_SDD_CLOCK, ENABLE);
}

static bool bt_inic_get_wakelock_status(void)
{
	uint32_t lock_status = pmu_get_wakelock_status();
	BT_LOGD("[BT_PS] lock_status = 0x%x\r\n", lock_status);

	if (lock_status & ((0x01) << PMU_BT_DEVICE)) {
		return true;    //Already acquire bt wake lock
	} else {
		return false;   //Already release bt wake lock
	}
}

static void bt_inic_acquire_wakelock(void)
{
	if (bt_inic_get_wakelock_status() == false) {
		BT_LOGD("[BT_PS] pmu_acquire_wakelock PMU_BT_DEVICE\r\n");
		pmu_acquire_wakelock(PMU_BT_DEVICE);
	} else {
		BT_LOGD("[BT_PS] already acquire PMU_BT_DEVICE\r\n");
	}
}

static u32 bt_inic_sdio_dev_suspend(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	BT_LOGD("[BT_PS] Enter %s \r\n", __func__);

	hci_platform_force_uart_rts(true);

	SDIO_SetReady(SDIO_BT, DISABLE);

	return TRUE;
}

static u32 bt_inic_sdio_dev_resume(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	BT_LOGD("Enter %s \r\n", __func__);

	bt_inic_acquire_wakelock();
	SDIO_SetReady(SDIO_BT, ENABLE);

	hci_platform_force_uart_rts(false);

	return TRUE;
}

void bt_inic_sdio_init(void)
{
	SDIO_InitTypeDef SDIO_InitStruct;
	u32 i = 0;

	g_bt_inic_sdio.tx_bd = osif_mem_aligned_alloc(0, BT_INIC_SDIO_TX_BD_NUM * sizeof(struct bt_inic_sdio_tx_bd), 4);
	g_bt_inic_sdio.rx_bd = osif_mem_aligned_alloc(0, BT_INIC_SDIO_RX_BD_NUM * sizeof(struct bt_inic_sdio_rx_bd), 8);
	for (i = 0; i < BT_INIC_SDIO_TX_BD_NUM; i++) {
		g_bt_inic_sdio.tx_buf[i] = osif_mem_aligned_alloc(0, BT_INIC_SDIO_TX_BUFSZ, BT_INIC_SDIO_DMA_ALIGN_4);
		g_bt_inic_sdio.tx_bd[i].address = (u32)g_bt_inic_sdio.tx_buf[i];
	}

	DCache_Clean((u32)g_bt_inic_sdio.tx_bd, BT_INIC_SDIO_TX_BD_NUM * sizeof(struct bt_inic_sdio_tx_bd));
	osif_mutex_create(&g_bt_inic_sdio.rxbd_mutex);
	/* enable sdio bt func */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL,
				HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL) | LSYS_BIT_SDIO_BT_EN);

	SPDIO_Board_Init();
	/* config bt func register */
	SDIO_StructInit(&SDIO_InitStruct);
	SDIO_InitStruct.TXBD_BAR = (u32)g_bt_inic_sdio.tx_bd;
	SDIO_InitStruct.TXBD_RING_SIZE = BT_INIC_SDIO_TX_BD_NUM;
	SDIO_InitStruct.TX_BUFFER_SIZE = (((BT_INIC_SDIO_TX_BUFSZ - 1) / BT_INIC_SDIO_TX_BUF_SZ_UNIT) + 1) & 0xff;
	SDIO_InitStruct.RXBD_BAR = (u32)g_bt_inic_sdio.rx_bd;
	SDIO_InitStruct.RXBD_RING_SIZE = BT_INIC_SDIO_RX_BD_NUM;
	SDIO_InitStruct.RXBD_FREE_TH = BT_INIC_SDIO_RX_BD_FREE_TH;
	SDIO_InitStruct.TX_AGG_DISPATCH = 0;
	SDIO_Init(SDIO_BT, &SDIO_InitStruct);

	g_bt_inic_sdio.txbd_w_ptr = SDIO_TXBD_WPTR_Get(SDIO_BT);
	g_bt_inic_sdio.txbd_r_ptr = g_bt_inic_sdio.txbd_w_ptr;
	g_bt_inic_sdio.rxbd_r_ptr = SDIO_RXBD_RPTR_Get(SDIO_BT);
	g_bt_inic_sdio.txbd_w_ptr = g_bt_inic_sdio.txbd_r_ptr;

	osif_sem_create(&g_bt_inic_sdio.irq_sem, 0, 1);
	osif_task_create(&g_bt_inic_sdio.sdio_task_hdl, "bt_inic_sdio_irq_task", bt_inic_sdio_irq_thread_hdl,
					 NULL, 2048, 4);

	InterruptRegister((IRQ_FUN) bt_inic_sdio_irq, SDIO_BT_IRQ, NULL, BT_INIC_SDIO_IRQ_PRIO);
	InterruptEn(SDIO_BT_IRQ, BT_INIC_SDIO_IRQ_PRIO);

	SDIO_INTClearAll(SDIO_BT);
	SDIO_INTConfig(SDIO_BT, 0xffff, ENABLE);

	SDIO_SetReady(SDIO_BT, ENABLE);

	pmu_register_sleep_callback(PMU_BT_DEVICE, (PSM_HOOK_FUN)bt_inic_sdio_dev_suspend, NULL, (PSM_HOOK_FUN)bt_inic_sdio_dev_resume, NULL);

	BT_LOGA("BT SDIO device interface init OK !!\r\n");
}


