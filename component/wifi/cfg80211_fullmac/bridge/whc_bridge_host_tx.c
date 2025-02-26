#include <whc_host_linux.h>
#include <whc_bridge_host_intf.h>

/* SIZE_TX_DESC not included in len  */
/* pbuf needed to be freed by user if need */
void whc_bridge_host_send_data_to_dev(u8 *pbuf, u32 len, u32 with_txdesc)
{
	u8 *buf;
	u32 buf_len = len;

	if (with_txdesc) {
		whc_bridge_host_send_data(pbuf, buf_len);
	} else {
		/* SIZE_TX_DESC is needed by dev hw */
		buf_len += SIZE_TX_DESC;
		buf = kzalloc(buf_len, GFP_KERNEL);
		if (buf) {
			/* copy data */
			memcpy((u8 *)(buf + SIZE_TX_DESC), pbuf, len);

			/* send */
			whc_bridge_host_send_data(buf, buf_len);
#ifndef CONFIG_INIC_USB_ASYNC_SEND
			kfree(buf);
#endif
		} else {
			dev_err(global_idev.fullmac_dev, "%s can't alloc buffer!\n", __func__);
		}
	}

}