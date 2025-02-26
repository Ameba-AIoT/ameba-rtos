#include <whc_bridge_host_intf.h>
#include <whc_dev_struct.h>
#include <whc_dev.h>

void whc_bridge_host_send_data(u8 *buf, u32 len)
{
	struct whc_bridge_hdr *hdr = NULL;
	u32 event = *(u32 *)buf;
	u8 *txbuf = NULL;
	u32 txsize = len + sizeof(struct whc_bridge_hdr);

	if (event != WHC_WIFI_EVT_XIMT_PKTS) {
		txbuf = kzalloc(txsize, GFP_KERNEL);
		if (txbuf) {
			hdr = (struct whc_bridge_hdr *)txbuf;
			hdr->event = WHC_WIFI_EVT_BRIDGE;
			hdr->len = len;
			/* copy data */
			memcpy(txbuf + sizeof(struct whc_bridge_hdr), buf, len);
			whc_host_send_data(txbuf, txsize, NULL);
			kfree(txbuf);
		} else {
			dev_err(global_idev.fullmac_dev, "%s can't alloc spi buffer!\n", __func__);
		}
	} else {
		whc_host_send_data(buf, len, NULL);
	}
}

int whc_bridge_host_pkt_rx_to_user(struct sk_buff *pskb)
{
	int ret = 0;
	u32 event = *(u32 *)(pskb->data + SIZE_RX_DESC);
	u32 size = 0;
	struct whc_bridge_hdr *hdr = NULL;
	u8 *rxbuf = NULL;


	switch (event) {
	case WHC_WIFI_EVT_BRIDGE:
		hdr = (struct whc_bridge_hdr *)(pskb->data + SIZE_RX_DESC);
		size = hdr->len;
		rxbuf = (u8 *)pskb->data + SIZE_RX_DESC + sizeof(struct whc_bridge_hdr);
		whc_bridge_host_buf_rx_to_user(rxbuf, size);
		break;
	default:
		break;
	}

	return ret;
}