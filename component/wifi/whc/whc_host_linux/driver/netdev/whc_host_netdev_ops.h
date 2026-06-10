#ifndef __RTW_NETDEV_OPS_H__
#define __RTW_NETDEV_OPS_H__

#define WIFI_FULLMAC_LABEL (0x8730E)
#define WIFI_MP_MSG_BUF_SIZE (4096)

#define DRIVERVERSION "v1.15.12-27-g7f6d5a49a.20220627"

#define RTW_PRIV_DGB_CMD (SIOCDEVPRIVATE)
#define RTW_PRIV_MP_CMD (SIOCDEVPRIVATE + 1)

struct netdev_priv_t {
	u32 label;
	u32 wlan_idx;
	bool priv_is_on;
	unsigned int flags;
};

struct rtw_priv_ioctl {
	unsigned char __user *data;
	unsigned short len;
};

#define rtw_netdev_priv_is_on(netdev) (((struct netdev_priv_t *)netdev_priv(netdev))->priv_is_on)
#define rtw_netdev_idx(netdev) (((struct netdev_priv_t *)netdev_priv(netdev))->wlan_idx)
#define rtw_netdev_label(netdev) (((struct netdev_priv_t *)netdev_priv(netdev))->label)
#define rtw_netdev_flags(netdev) (((struct netdev_priv_t *)netdev_priv(netdev))->flags)
#define wdev_to_ndev(w) ((w)->netdev)
#define ndev_to_wdev(n) ((n)->ieee80211_ptr)

int rtw_ndev_set_mac_address(struct net_device *pnetdev, void *p);
struct net_device_stats *rtw_ndev_get_stats(struct net_device *pnetdev);
u16 rtw_ndev_select_queue(struct net_device *pnetdev, struct sk_buff *skb, struct net_device *sb_dev);
int rtw_ndev_init(struct net_device *pnetdev);
void rtw_ndev_uninit(struct net_device *pnetdev);
enum netdev_tx rtw_xmit_entry(struct sk_buff *skb, struct net_device *pnetdev);

void rtw_inetaddr_notifier_register(void);
void rtw_inetaddr_notifier_unregister(void);

int rtw_netdev_probe(struct device *pdev);
int rtw_netdev_remove(struct device *pdev);

#endif //__RTW_NETDEV_OPS_H__
