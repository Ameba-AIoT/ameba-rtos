#include "ameba_soc.h"
#include "ameba_inic.h"
#include "spdio_api.h"
#include "platform_autoconf.h"
#include "osif.h"
#include "hci_if_inic.h"
#include "bt_debug.h"
#include "bt_inic_defs.h"
#include <string.h>

#if !(defined(CONFIG_WHC_INTF_SDIO) && CONFIG_WHC_INTF_SDIO)
struct bt_inic_sdio_priv_t {
	struct spdio_t dev;
	void *tx_lock;
	void *rxbd_release_sema;
};

#define BT_INIC_SDIO_TX_BD_NUM          5
#define BT_INIC_SDIO_RX_BD_NUM          20
#define BT_INIC_SDIO_TX_BUFSZ           (1024 + 64) /* it shall be 64 bytes aligned */
#endif

struct bt_inic_sdio_hdr {
	uint32_t    event;
	uint16_t    len;
	uint8_t     type;
} __attribute__((packed));

#define DEV_DMA_ALIGN                   4

#define HCI_SDIO_PKT_EVENT_FIELD        4
#define HCI_SDIO_PKT_LEN_FIELD          2
#define HCI_SDIO_PKT_SERVICE_ID_FIELD   1
#define HCI_SDIO_PKT_HDR_LEN            (HCI_SDIO_PKT_EVENT_FIELD + HCI_SDIO_PKT_LEN_FIELD + HCI_SDIO_PKT_SERVICE_ID_FIELD)

#if defined(CONFIG_WHC_INTF_SDIO) && CONFIG_WHC_INTF_SDIO
// extern void whc_sdio_dev_send_data(u8 *data, u32 len);
extern void whc_bridge_dev_api_send_to_host(u8 *data, u32 len);
extern void (*bt_inic_sdio_recv_ptr)(uint8_t *buffer, uint16_t len);
void bt_inic_sdio_recv_from_host(uint8_t *pdata, uint16_t len);

void bt_inic_sdio_init(void)
{
	bt_inic_sdio_recv_ptr = bt_inic_sdio_recv_from_host;
}

void bt_inic_send_to_host(u8 type, u8 *pbuf, u32 len)
{
	u8 *buf = NULL;
	struct bt_inic_sdio_hdr *hdr;
	u16 sdiolen = len + sizeof(struct bt_inic_sdio_hdr);

	buf = osif_mem_alloc(RAM_TYPE_DATA_ON, sdiolen + DEV_DMA_ALIGN);

	if (!(buf)) {
		BT_LOGE("ERROR NULL\n");
		return;
	}

	memset(buf, 0, sdiolen + DEV_DMA_ALIGN);
	hdr = (struct bt_inic_sdio_hdr *)N_BYTE_ALIGMENT((u32)buf, DEV_DMA_ALIGN);

	hdr->event = INIC_BT_HOST_RX;
	hdr->len = sdiolen;
	hdr->type = type;

	memcpy((void *)(hdr + 1), pbuf, len);//BLE Packet data

	// BT_LOGD("BT dev send type %d len %d\n", type, sdiolen);
	// BT_DUMPA("Share TX:\r\n", buf, sdiolen);
	// whc_sdio_dev_send_data((u8 *)hdr, sdiolen);
	whc_bridge_dev_api_send_to_host((u8 *)hdr, sdiolen);
	osif_mem_free(buf);

	return;
}

/*spdio rx done callback (HOST->Device), manage your package and buffer*/
void bt_inic_sdio_recv_from_host(uint8_t *pdata, uint16_t len)
{
	struct bt_inic_sdio_hdr *hdr = (struct bt_inic_sdio_hdr *)pdata;

	uint8_t *rx_buf = NULL;

	if (hdr->event != INIC_BT_HOST_TX) {
		return;
	}

	if (SIZE_TX_DESC + hdr->len > len) {
		BT_LOGE("Error: SDIO RX packet size is over rx buf size\r\n");
		return;
	}

	rx_buf = osif_mem_alloc(RAM_TYPE_DATA_ON, hdr->len - HCI_SDIO_PKT_HDR_LEN);
	if (rx_buf == NULL) {
		RTK_LOGE("BT_INIC", "%s, can't alloc buffer!!\n", __func__);
		return;
	}

	memcpy(rx_buf, (void *)(hdr + 1), hdr->len - HCI_SDIO_PKT_HDR_LEN);

	// BT_LOGA("BT Host send type %d len %d\n", hdr->type, hdr->len);
	// BT_DUMPA("Share RX:\r\n", rx_buf, hdr->len - HCI_SDIO_PKT_HDR_LEN);

	bt_inic_recv_from_host(hdr->type, rx_buf, hdr->len - HCI_SDIO_PKT_HDR_LEN);

	if (rx_buf) {
		osif_mem_free(rx_buf);
	}

	return;
}

#else /* #if defined(CONFIG_WHC_INTF_SDIO) && CONFIG_WHC_INTF_SDIO */
struct bt_inic_sdio_priv_t sdio_priv = {0};
static char inic_sdio_dev_rpwm_cb(void *priv, u16 value)
{
	UNUSED(priv);

	if (value & RPWM2_CG_BIT) {
		SDIO_SetReady(SDIO_WIFI, DISABLE);
		pmu_release_wakelock(PMU_WLAN_DEVICE);
	}

	if (value & RPWM2_ACT_BIT) {
		pmu_acquire_wakelock(PMU_WLAN_DEVICE);
		SDIO_SetReady(SDIO_WIFI, ENABLE);
	}

	return 0;
}

/*spdio tx done callback(Device->HOST), manage buffer*/
// this API will be called after package have been moved to HOST
static char inic_sdio_dev_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;

	osif_mem_free((void *)tx_buf);
	osif_sem_give(sdio_priv.rxbd_release_sema);

	return RTK_SUCCESS;
}

/*spdio rx done callback (HOST->Device), manage your package and buffer*/
static char inic_sdio_dev_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	(void) priv;
	(void) pbuf;
	(void) size;
	(void) type;
	uint32_t pkt_len;

	struct bt_inic_sdio_hdr *hdr = (struct bt_inic_sdio_hdr *)pdata;
	u8 *rx_buf = NULL;

	pkt_len = hdr->len - HCI_SDIO_PKT_HDR_LEN;

	if (hdr->event != INIC_BT_HOST_TX) {
		return RTK_FAIL;
	}

	if (SIZE_TX_DESC + hdr->len > BT_INIC_SDIO_TX_BUFSZ) {
		BT_LOGE("Error: SDIO RX packet size is over rx buf size\r\n");
		return RTK_FAIL;
	}

	rx_buf = osif_mem_alloc(RAM_TYPE_DATA_ON, pkt_len);
	if (rx_buf == NULL) {
		RTK_LOGE("BT_INIC", "%s, can't alloc buffer!!\n", __func__);
		return RTK_FAIL;
	}

	memcpy(rx_buf, (void *)(hdr + 1), pkt_len);

	// BT_LOGA("BT Host send type %d len %d\n", hdr->type, hdr->len);
	// BT_DUMPA("Share RX:\r\n", rx_buf, pkt_len);

	bt_inic_recv_from_host(hdr->type, rx_buf, pkt_len);

	return RTK_SUCCESS;
}

static void inic_dev_send(struct spdio_buf_t *pbuf)
{
	/* Call this function when receive pkt, call API or send API return value.
	Use locks to ensure exclusive execution. */
	osif_mutex_take(sdio_priv.tx_lock, BT_TIMEOUT_FOREVER);
	while (spdio_tx(&sdio_priv.dev, pbuf) == FALSE) {
		/* wait for RXBD release */
		osif_sem_take(sdio_priv.rxbd_release_sema, BT_TIMEOUT_FOREVER);
	}

	osif_mutex_give(sdio_priv.tx_lock);

	return;
}

void bt_inic_send_to_host(u8 type, u8 *pbuf, u32 len)
{
	u8 *tx_buf = NULL;
	struct bt_inic_sdio_hdr *hdr;
	struct spdio_buf_t *spdio_txbuf = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct spdio_buf_t));

	u16 sdiolen = len + sizeof(struct bt_inic_sdio_hdr);

	tx_buf = osif_mem_alloc(RAM_TYPE_DATA_ON, sdiolen + DEV_DMA_ALIGN);

	if (!(tx_buf)) {
		BT_LOGE("ERROR NULL\n");
		return;
	}

	memset(tx_buf, 0, sdiolen + DEV_DMA_ALIGN);
	hdr = (struct bt_inic_sdio_hdr *)N_BYTE_ALIGMENT((u32)tx_buf, DEV_DMA_ALIGN);

	hdr->event = INIC_BT_HOST_RX;
	hdr->len = sdiolen;
	hdr->type = type;

	memcpy((void *)(hdr + 1), pbuf, len);//BLE Packet data

	// BT_LOGD("BT dev send type %d len %d\n", type, sdiolen);
	// BT_DUMPA("Share TX:\r\n", tx_buf, sdiolen);

	spdio_txbuf->priv = NULL;
	spdio_txbuf->buf_allocated = spdio_txbuf->buf_addr = (u32)tx_buf;
	spdio_txbuf->size_allocated = spdio_txbuf->buf_size = sdiolen;

	inic_dev_send(spdio_txbuf);
}

void bt_inic_sdio_init(void)
{
	u32 i;
	u8 *buf = NULL;
	struct spdio_t *dev = &sdio_priv.dev;

	dev->priv = NULL;
	dev->rx_bd_num = BT_INIC_SDIO_TX_BD_NUM;
	dev->tx_bd_num = BT_INIC_SDIO_RX_BD_NUM;
	dev->rx_bd_bufsz = BT_INIC_SDIO_TX_BUFSZ;
	dev->rx_buf = (struct spdio_buf_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, dev->rx_bd_num * sizeof(struct spdio_buf_t));

	if (!dev->rx_buf) {
		RTK_LOGE("BT_INIC", "malloc failed for spdio buffer structure!\n");
		return;
	}

	for (i = 0; i < dev->rx_bd_num; i++) {
		buf = osif_mem_aligned_alloc(RAM_TYPE_DATA_ON, BT_INIC_SDIO_TX_BUFSZ, DEV_DMA_ALIGN);

		dev->rx_buf[i].buf_allocated = dev->rx_buf[i].buf_addr = (u32) buf;
		dev->rx_buf[i].size_allocated = dev->rx_buf[i].buf_size = dev->rx_bd_bufsz;
		dev->rx_buf[i].priv = buf;

		// this buffer must be 4 byte alignment
		if (dev->rx_buf[i].buf_addr % 4) {
			RTK_LOGE("BT_INIC", "rx buffer address not 4-byte aligned!\n");
			return;
		}
	}

	dev->rx_done_cb = inic_sdio_dev_rx_done_cb;
	dev->tx_done_cb = inic_sdio_dev_tx_done_cb;
	dev->rpwm_cb = inic_sdio_dev_rpwm_cb;

	osif_mutex_create(&sdio_priv.tx_lock);
	osif_sem_create(&sdio_priv.rxbd_release_sema, 0, BT_TIMEOUT_FOREVER);

	spdio_init(dev);

	RTK_LOGI("BT_INIC", "SDIO device init done!\n");

}

#endif /* #if defined(CONFIG_WHC_INTF_SDIO) && CONFIG_WHC_INTF_SDIO */
