#include "ameba_soc.h"
#include "hci_platform.h"
#include "osif.h"
#include "bt_inic.h"
#include "bt_debug.h"
#include "hci_if_inic.h"
#include "spdio_api.h"

#define BT_INIC_SDIO_TX_BD_NUM          5
#define BT_INIC_SDIO_RX_BD_NUM          20
#define BT_INIC_SDIO_TX_BUFSZ           (1024 + 64) /* it shall be 64 bytes aligned */

#define HCI_SDIO_PKT_LEN_FIELD          3
#define HCI_SDIO_PKT_SERVICE_ID_FIELD   1
#define HCI_SDIO_PKT_HDR_LEN            (HCI_SDIO_PKT_LEN_FIELD + HCI_SDIO_PKT_SERVICE_ID_FIELD)

struct spdio_t spdio_bt_dev = {0};
void *rxbd_mutex = NULL;

void bt_inic_send_to_host(u8 type, u8 *pdata, u32 len)
{
	u16 sdiodata_len = len + HCI_SDIO_PKT_HDR_LEN;
	u8 ret;

	osif_mutex_take(rxbd_mutex, 0xFFFFFFFF);

	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)osif_mem_aligned_alloc(0, sizeof(struct spdio_buf_t), 4);

	if (!tx_buf) {
		BT_LOGA("%s malloc tx_buf fail!\r\n", __func__);
		return;
	}

	memset((u8 *)tx_buf, 0, sizeof(struct spdio_buf_t));

	tx_buf->buf_allocated = tx_buf->buf_addr = (u32)osif_mem_aligned_alloc(0, sdiodata_len, 4);
	if (!tx_buf->buf_allocated) {
		osif_mem_aligned_free((void *)tx_buf);
		BT_LOGA("%s malloc tx_buffer fail!\r\n", __func__);
		return;
	}

	tx_buf->size_allocated = tx_buf->buf_size = sdiodata_len;
	*((u8 *)(tx_buf->buf_addr)) = sdiodata_len & 0xff; /* Packet Length */
	*((u8 *)(tx_buf->buf_addr + 1)) = (sdiodata_len >> 8) & 0xff;
	*((u8 *)(tx_buf->buf_addr + 2)) = 0;
	*((u8 *)(tx_buf->buf_addr + 3)) = type; /* Service ID */

	// copy data
	memcpy((void *)(tx_buf->buf_addr + HCI_SDIO_PKT_HDR_LEN), pdata, len);
	DCache_CleanInvalidate(tx_buf->buf_allocated, sdiodata_len);

	ret = spdio_tx(&spdio_bt_dev, tx_buf);
	if (ret == FALSE) {
		osif_mem_aligned_free((void *)tx_buf->buf_allocated);
		osif_mem_aligned_free((void *)tx_buf);
	}
	osif_mutex_give(rxbd_mutex);
}

char bt_inic_sdio_rxfifo_send_done(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;

	osif_mem_aligned_free((void *)tx_buf->buf_allocated);
	osif_mem_aligned_free((void *)tx_buf);

	return RTK_SUCCESS;
}

char bt_inic_sdio_txfifo_data_ready(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	(void)priv;
	(void)pbuf;
	(void)type;

	// BT_DUMPA("HOST TX: ",read_buf, pkt_len);
	bt_inic_recv_from_host(*(pdata + HCI_SDIO_PKT_LEN_FIELD), pdata + HCI_SDIO_PKT_HDR_LEN, size - HCI_SDIO_PKT_HDR_LEN);

	return RTK_SUCCESS;
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
	u32 i;
	rtk_log_level_set("SPDIO", RTK_LOG_ERROR);

	spdio_bt_dev.host_tx_bd_num = BT_INIC_SDIO_TX_BD_NUM;
	spdio_bt_dev.host_rx_bd_num = BT_INIC_SDIO_RX_BD_NUM;
	spdio_bt_dev.device_rx_bufsz = BT_INIC_SDIO_TX_BUFSZ;

	spdio_bt_dev.rx_buf = (struct spdio_buf_t *)osif_mem_aligned_alloc(0, spdio_bt_dev.host_tx_bd_num * sizeof(struct spdio_buf_t), 4);
	if (!spdio_bt_dev.rx_buf) {
		BT_LOGA("malloc failed for spdio buffer structure!\r\n");
		return;
	}

	for (i = 0; i < spdio_bt_dev.host_tx_bd_num; i++) {
		spdio_bt_dev.rx_buf[i].buf_allocated = spdio_bt_dev.rx_buf[i].buf_addr = (u32)osif_mem_aligned_alloc(0, spdio_bt_dev.device_rx_bufsz, 4);
		if (!spdio_bt_dev.rx_buf[i].buf_allocated) {
			osif_mem_aligned_free((void *)(spdio_bt_dev.rx_buf));
			BT_LOGA("malloc failed for spdio buffer!\r\n");
			return;
		}
		spdio_bt_dev.rx_buf[i].size_allocated = spdio_bt_dev.rx_buf[i].buf_size = spdio_bt_dev.device_rx_bufsz;
	}

	spdio_bt_dev.pSDIO = SDIO_BT;
	spdio_bt_dev.device_rx_done_cb = bt_inic_sdio_txfifo_data_ready;
	spdio_bt_dev.device_tx_done_cb = bt_inic_sdio_rxfifo_send_done;

	/* enable sdio bt func */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL,
				HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL) | LSYS_BIT_SDIO_BT_EN);

	spdio_init(&spdio_bt_dev);

	osif_mutex_create(&rxbd_mutex);

	pmu_register_sleep_callback(PMU_BT_DEVICE, (PSM_HOOK_FUN)bt_inic_sdio_dev_suspend, NULL, (PSM_HOOK_FUN)bt_inic_sdio_dev_resume, NULL);

	BT_LOGA("BT SDIO device interface init OK !!\r\n");
}


