#include <whc_bridge_host_intf.h>
#include <whc_dev_struct.h>
#include <whc_dev.h>

void whc_bridge_host_send_data(u8 *buf, u32 len)
{
	whc_host_send_data(buf, len, NULL);
}

int whc_bridge_host_pkt_rx_to_user(struct sk_buff *pskb)
{
	int ret = 0;
	u16 size;

	/* size after padding for align */
	size = *(u16 *)pskb->data;
	ret = whc_bridge_host_buf_rx_to_user((u8 *)pskb->data + SIZE_RX_DESC, size);

	return ret;
}