// SPDX-License-Identifier: GPL-2.0-only
#include <whc_host_linux.h>

void whc_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb)
{
	struct hci_ops_t *intf_ops = global_idev.intf_ops;

	intf_ops->send_data(buf, len, pskb);
}

void whc_host_recv_data(void *intf_priv)
{
	struct hci_ops_t *intf_ops = global_idev.intf_ops;

	intf_ops->recv_data(intf_priv);
}

#ifdef WHCH_TXAGG
void whc_host_send_xmitbuf(struct whc_xmit_buf *pxmitbuf)
{
	struct hci_ops_t *intf_ops = global_idev.intf_ops;

	intf_ops->send_xmitbuf(pxmitbuf);
}
#endif
