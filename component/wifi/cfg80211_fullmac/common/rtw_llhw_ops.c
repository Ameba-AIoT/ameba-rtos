#include <rtw_cfg80211_fullmac.h>

void llhw_send_data(u8 *buf, u32 len, struct sk_buff *pskb)
{
	struct hci_ops_t *intf_ops = global_idev.intf_ops;

	intf_ops->send_data(buf, len, pskb);
}

void llhw_recv_data_process(void *intf_priv)
{
	struct hci_ops_t *intf_ops = global_idev.intf_ops;

	intf_ops->recv_data_process(intf_priv);
}

