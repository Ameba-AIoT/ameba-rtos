#ifndef __RTW_NETDEV_OPS_H__
#define __RTW_NETDEV_OPS_H__

#define WIFI_FULLMAC_LABEL (0x8730E)
#define WIFI_MP_MSG_BUF_SIZE (4096)

struct netdev_priv_t {
	u32 label;
	u32 wlan_idx;
	bool priv_is_on;
};

#define rtw_netdev_priv_is_on(netdev) (((struct netdev_priv_t *)netdev_priv(netdev))->priv_is_on)
#define rtw_netdev_idx(netdev) (((struct netdev_priv_t *)netdev_priv(netdev))->wlan_idx)
#define rtw_netdev_label(netdev) (((struct netdev_priv_t *)netdev_priv(netdev))->label)
#define wdev_to_ndev(w) ((w)->netdev)
#define ndev_to_wdev(n) ((n)->ieee80211_ptr)

#ifdef CONFIG_P2P
int rtw_ndev_init(struct net_device *pnetdev);
void rtw_ndev_uninit(struct net_device *pnetdev);
enum netdev_tx rtw_xmit_entry(struct sk_buff *skb, struct net_device *pnetdev);
u16 rtw_ndev_select_queue(struct net_device *pnetdev, struct sk_buff *skb, struct net_device *sb_dev);
int rtw_ndev_set_mac_address(struct net_device *pnetdev, void *p);
struct net_device_stats *rtw_ndev_get_stats(struct net_device *pnetdev);
int rtw_ndev_ioctl(struct net_device *ndev, struct ifreq *rq, int cmd_id);
#endif
#endif //__RTW_NETDEV_OPS_H__
