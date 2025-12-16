#include "ameba_soc.h"
#include "hci_platform.h"
#include "osif.h"
#include "bt_inic.h"
#include "bt_debug.h"
#include "spdio_api.h"

#if defined (CONFIG_SDN) && CONFIG_SDN
#include "hci_if_inic_sdn.h"
#else
#include "hci_if_inic.h"
#endif

#define BT_INIC_SDIO_HOST_TX_BD_NUM          5
#define BT_INIC_SDIO_HOST_RX_BD_NUM          20
#define BT_INIC_SDIO_DEV_RX_BUFSZ            (1024 + 64) /* it shall be 64 bytes aligned */
#define BT_INIC_SDIO_DEV_TX_BUFSZ            292   /* shall be 4 byte aligned */

#define HCI_SDIO_PKT_LEN_FIELD          3
#define HCI_SDIO_PKT_SERVICE_ID_FIELD   1
#define HCI_SDIO_PKT_HDR_LEN            (HCI_SDIO_PKT_LEN_FIELD + HCI_SDIO_PKT_SERVICE_ID_FIELD)

struct spdio_t spdio_bt_dev = {0};
void *rxbd_mutex = NULL;
void *tx_bd_free = NULL;

static bool sdio_tx_buf_alloc(struct spdio_buf_t **buf)
{
	return osif_msg_recv(tx_bd_free, buf, BT_TIMEOUT_NONE);
}

static bool sdio_tx_buf_free(struct spdio_buf_t *buf)
{
	if (!osif_msg_send(tx_bd_free, &buf, BT_TIMEOUT_NONE)) {
		return false;
	}
	return true;
}

static bool sdio_tx_transport_open(void)
{
	int i;
	struct spdio_buf_t *buf;

	if (false == osif_msg_queue_create(&tx_bd_free, BT_INIC_SDIO_HOST_RX_BD_NUM, sizeof(struct spdio_buf_t *))) {
		return false;
	}

	for (i = 0; i < BT_INIC_SDIO_HOST_RX_BD_NUM; i++) {
		struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)osif_mem_aligned_alloc(0, sizeof(struct spdio_buf_t), 4);
		if (tx_buf == NULL) {
			goto failed;
		}
		memset((u8 *)tx_buf, 0, sizeof(struct spdio_buf_t));

		tx_buf->buf_allocated = tx_buf->buf_addr = (u32)osif_mem_aligned_alloc(0, BT_INIC_SDIO_DEV_TX_BUFSZ, 4);
		if (!tx_buf->buf_allocated) {
			osif_mem_aligned_free((void *)tx_buf);
			goto failed;
		}

		if (false == osif_msg_send(tx_bd_free, &tx_buf, BT_TIMEOUT_NONE)) {
			osif_mem_aligned_free((void *)tx_buf->buf_allocated);
			osif_mem_aligned_free((void *)tx_buf);
			goto failed;
		}
	}

	return true;

failed:
	if (tx_bd_free) {
		while (osif_msg_recv(tx_bd_free, &buf, BT_TIMEOUT_NONE)) {
			if (buf) {
				osif_mem_aligned_free((void *)buf->buf_allocated);
				osif_mem_aligned_free((void *)buf);
			}
		}
		osif_msg_queue_delete(tx_bd_free);
		tx_bd_free = NULL;
	}

	return false;
}

void bt_inic_send_to_host(u8 type, u8 *pdata, u32 len)
{
	u16 sdiodata_len = len + HCI_SDIO_PKT_HDR_LEN;
	u8 ret;

	struct spdio_buf_t *tx_buf = NULL;
	osif_mutex_take(rxbd_mutex, 0xFFFFFFFF);

	if (sdio_tx_buf_alloc(&tx_buf)) {
		if (!tx_buf || !tx_buf->buf_allocated) {
			BT_LOGA("Failed to allocate memory for tx_buf\r\n");
			if (tx_buf) {
				sdio_tx_buf_free(tx_buf);
			}
			goto exit;
		}

		tx_buf->size_allocated = tx_buf->buf_size = sdiodata_len;
		*((u8 *)(tx_buf->buf_addr)) = sdiodata_len & 0xff; /* Packet Length */
		*((u8 *)(tx_buf->buf_addr + 1)) = (sdiodata_len >> 8) & 0xff;
		*((u8 *)(tx_buf->buf_addr + 2)) = 0;
		*((u8 *)(tx_buf->buf_addr + 3)) = type; /* Service ID */

		// copy data
		memcpy((void *)(tx_buf->buf_addr + HCI_SDIO_PKT_HDR_LEN), pdata, len);

		BT_DUMPA("HOST RX: ", (void *)tx_buf->buf_addr, sdiodata_len);

		DCache_CleanInvalidate(tx_buf->buf_allocated, sdiodata_len);

		ret = spdio_tx(&spdio_bt_dev, tx_buf);
		if (ret == FALSE) {
			sdio_tx_buf_free(tx_buf);
		}

	} else {
		BT_LOGA("Failed to allocate memory for tx_buf\r\n");
		goto exit;
	}

exit:
	osif_mutex_give(rxbd_mutex);
	return;
}

char bt_inic_sdio_rxfifo_send_done(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;
	sdio_tx_buf_free(tx_buf);

	return RTK_SUCCESS;
}

char bt_inic_sdio_txfifo_data_ready(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	(void)priv;
	(void)pbuf;
	(void)type;

	// BT_DUMPA("HOST TX: ",pdata, size);
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

	spdio_bt_dev.host_tx_bd_num = BT_INIC_SDIO_HOST_TX_BD_NUM;
	spdio_bt_dev.host_rx_bd_num = BT_INIC_SDIO_HOST_RX_BD_NUM;
	spdio_bt_dev.device_rx_bufsz = BT_INIC_SDIO_DEV_RX_BUFSZ;

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

	sdio_tx_transport_open();

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


