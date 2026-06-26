#include <whc_host_linux.h>

#ifdef CONFIG_WHCH
int whc_host_on_addba_delba(u16 tid, struct sta_recv_priv *sta_recvpriv)
{
	struct recv_reorder_ctrl *preorder_ctrl = &sta_recvpriv->recvreorder_ctrl[tid];

	if (!preorder_ctrl->enable) {
		return -1;
	}

	if (global_idev.wifi_user_config.ap_compatibilty_enabled & BIT(4)) {
		if (preorder_ctrl->bReorderWaiting) {
			del_timer_sync(&preorder_ctrl->reordering_ctrl_timer);
		}
		spin_lock_bh(&preorder_ctrl->pending_recvframe_lock);
		preorder_ctrl->bReorderWaiting &= ~BIT(7);
		while (whc_host_recv_reorder_dequeue(preorder_ctrl, true) == true);
		preorder_ctrl->indicate_seq = 0xffff;
		preorder_ctrl->enable = false;
		spin_unlock_bh(&preorder_ctrl->pending_recvframe_lock);
	} else {
		preorder_ctrl->enable = false;
		preorder_ctrl->indicate_seq = 0xffff;
	}
	whc_host_recv_reorder_free(preorder_ctrl);

	return 0;
}

void whc_host_on_addba_req(u8 iface_type, u16 tid, struct sta_recv_priv *psta_recvpriv)
{
	struct whch_mlme_info	*pmlmeinfo = &global_idev.whchpriv.mlmeinfo[iface_type];
	struct recv_reorder_ctrl *preorder_ctrl;

	if (psta_recvpriv) {
		preorder_ctrl = &psta_recvpriv->recvreorder_ctrl[tid];
		if (!preorder_ctrl->enable) {
			whc_host_recv_reorder_setup(preorder_ctrl, global_idev.wifi_user_config.ap_compatibilty_enabled & BIT(4), iface_type);
		}

		preorder_ctrl->indicate_seq = 0xffff;

		if (global_idev.wifi_user_config.ap_compatibilty_enabled & BIT(4)) {
			spin_lock_bh(&preorder_ctrl->pending_recvframe_lock);
			if (!preorder_ctrl->bReorderWaiting) {
				preorder_ctrl->bReorderWaiting = true | BIT(7);
				mod_timer(&preorder_ctrl->reordering_ctrl_timer, jiffies + msecs_to_jiffies(REORDER_WAIT_TIME));
			} else {
				preorder_ctrl->bReorderWaiting |= BIT(7);
			}
			spin_unlock_bh(&preorder_ctrl->pending_recvframe_lock);

		} else {
			preorder_ctrl->enable = true;
		}
	}
}

int whc_host_on_action_block_ack(struct rtw_event_addba_nego *nego_info)
{
	struct sta_info		*psta = NULL;
	struct sta_recv_priv *psta_recvpriv = NULL;
	u8 iface_type = nego_info->iface_type;

	psta = whc_host_sta_get_stainfo(iface_type, nego_info->mac_addr);

	if (psta == NULL) {
		return 0;
	}

	psta_recvpriv = &psta->sta_recvpriv;

	switch (nego_info->operation) {
	case BIT(0):	//setup
		whc_host_on_addba_req(iface_type, nego_info->tid, psta_recvpriv);
		break;
	case BIT(1):	//del
		whc_host_on_addba_delba(nego_info->tid, psta_recvpriv);
		break;
	default:
		break;
	}

	return 0;
}
#endif