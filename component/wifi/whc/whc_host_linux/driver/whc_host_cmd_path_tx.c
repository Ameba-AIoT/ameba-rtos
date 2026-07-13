#include <whc_host_linux.h>

/* SIZE_TX_DESC not included in len  */
/* pbuf needed to be freed by user if need */
void whc_host_cmd_data_send_to_dev(u8 *pbuf, u32 len, u32 with_txdesc)
{
	struct whc_cmd_path_hdr *hdr = NULL;
	u8 *buf;
	u32 buf_len = 0;

	if (!with_txdesc) {
		buf_len = SIZE_TX_DESC;
	}
	buf_len += sizeof(struct whc_cmd_path_hdr) + len;

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (buf) {
		/* add header and copy data */
		hdr = (struct whc_cmd_path_hdr *)(buf + SIZE_TX_DESC);

		hdr->event = WHC_WIFI_EVT_CMD;
		hdr->len = buf_len;

		if (!with_txdesc) {
			memcpy(hdr + 1, pbuf, len);
		} else {
			memcpy(hdr + 1, pbuf + SIZE_TX_DESC, len - SIZE_TX_DESC);
		}

		/* send cmd */
		whc_host_send_data(buf, buf_len, NULL);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
		kfree(buf);
#endif
	} else {
		dev_err(global_idev.pwhc_dev, "%s can't alloc buffer!\n", __func__);
	}
}
