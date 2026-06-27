#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
void whc_host_hal_normal_rx_process(union recv_frame *precvframe, u8 *pphy_info)
{
	struct sk_buff	*skb = precvframe->u.hdr.pkt;
	u8 *paddr1 = NULL ;
	struct sk_buff	*clone_pkt = NULL;

	if (!(global_idev.pndev[WHC_AP_PORT] && rtw_netdev_priv_is_on(global_idev.pndev[WHC_AP_PORT])) &&
		!(global_idev.pndev[WHC_NAN_PORT] && rtw_netdev_priv_is_on(global_idev.pndev[WHC_NAN_PORT]))) {
		goto skip_if2_recv;
	}

#ifdef CONFIG_NAN	/* TODO_NAN */
	if (rtw_is_adapter_up(NAN_WLAN_INDEX) && rtw_is_nan_frame(precvframe)) {
		skb->dev = rtw_get_netdev(NAN_WLAN_INDEX);
		precvframe->u.hdr.iface_type = IFACE_PORT2;
		goto skip_if2_recv;
	}
#endif

#if 0	/* TODO_softap */
	paddr1 = GetAddr1Ptr(precvframe->u.hdr.rx_data);
	if (IS_MCAST(paddr1) == FALSE) {	//unicast packets
		if (rtw_is_adapter_up(IFACE_PORT1) && !memcmp(paddr1, rtw_get_adapter_mac_addr(IFACE_PORT1), ETH_ALEN)) {
			skb->dev = rtw_get_netdev(SOFTAP_WLAN_INDEX);
			precvframe->u.hdr.iface_type = IFACE_PORT1;
		}
	} else {	//broadcast or multicast
		if (GetFrameSubType(precvframe->u.hdr.rx_data) != RTW_PROBEREQ) {
			/* softap only receives broadcast probe_req, others shoule belongs to port0(sta) */
			goto skip_if2_recv;
		}

		if (g_appriv.ap_bcmc_recvframe == NULL) {
			whc_host_recv_free_frame(precvframe, pfree_recv_queue);
			return;
		}

		clone_pkt = skb_copy(precvframe->u.hdr.pkt, GFP_ATOMIC, 0);
		if (wifi_hal_if2_clone_recvframe(precvframe, g_appriv.ap_bcmc_recvframe, clone_pkt, pphy_info) != RTK_SUCCESS) {
			whc_host_recv_free_frame(precvframe, pfree_recv_queue);
			return;
		}

		g_appriv.ap_bcmc_recvframe->u.hdr.attrib.b_ap_bcmc_frame = 1;/*so whc_host_recv_free_frame won't free this recv_frame*/

		if (whc_host_recv_entry(g_appriv.ap_bcmc_recvframe) != RTK_SUCCESS) {
		}
	}
#endif

skip_if2_recv:

	whc_host_recv_entry(precvframe);
}

#endif