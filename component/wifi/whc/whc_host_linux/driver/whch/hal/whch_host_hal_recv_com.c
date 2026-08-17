#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
void whc_host_hal_deseg_priv_reset(struct rx_deseg_priv *pdeseg)
{
	if (pdeseg->deseg_skb) {
		dev_kfree_skb_any(pdeseg->deseg_skb);
	}
	memset(pdeseg, 0, sizeof(struct rx_deseg_priv));
}

void whc_host_hal_normal_rx_process(union recv_frame *precvframe, u8 *pphy_info)
{
	struct sk_buff	*skb = precvframe->u.hdr.pkt;
	u8 *paddr1 = NULL;

	if (!(global_idev.pndev[WHC_AP_PORT] && rtw_netdev_priv_is_on(global_idev.pndev[WHC_AP_PORT]))
#ifdef CONFIG_NAN
		&& !(global_idev.pndev[WHC_NAN_PORT] && rtw_netdev_priv_is_on(global_idev.pndev[WHC_NAN_PORT]))
#endif
	   ) {
		goto skip_if2_recv;
	}

#ifdef CONFIG_NAN
	if (global_idev.pndev[WHC_NAN_PORT] && rtw_netdev_priv_is_on(global_idev.pndev[WHC_NAN_PORT])
		&& whch_host_nan_is_nan_frame(precvframe)) {
		skb->dev = global_idev.pndev[WHC_NAN_PORT];
		precvframe->u.hdr.iface_type = WHC_NAN_PORT;
		goto skip_if2_recv;
	}
#endif

	/* Route unicast data frames addressed to the softap MAC onto the AP port. */
	paddr1 = GetAddr1Ptr(precvframe->u.hdr.rx_data);
	if (IS_MCAST(paddr1) == false) {	//unicast packets
		if (global_idev.pndev[WHC_AP_PORT] && rtw_netdev_priv_is_on(global_idev.pndev[WHC_AP_PORT]) &&
			!memcmp(paddr1, global_idev.pndev[WHC_AP_PORT]->dev_addr, ETH_ALEN)) {
			skb->dev = global_idev.pndev[WHC_AP_PORT];
			precvframe->u.hdr.iface_type = WHC_AP_PORT;
		}
	}

skip_if2_recv:

	whc_host_recv_entry(precvframe);
}

#endif