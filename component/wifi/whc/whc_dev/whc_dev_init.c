#include "whc_dev.h"

/**
 * @brief  to initialize the whc device (non-IPC).
 * @param  none.
 * @return none.
 */
void whc_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);

#ifdef CONFIG_WHC_CMD_PATH
	whc_dev_init_cmd_path();
#endif

	wifi_set_user_config();

	/* init skb pool early(before wifi_on) to ensure the intf path is ready, because whc uses skb as rxbuf */
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size, SKB_CACHE_SZ);

	whc_dev_intf_init();

#ifdef CONFIG_WHC_DEV_TCPIP_KEEPALIVE
	whc_dev_pktfilter_init();
#endif

	/* initialize the dev priv */
	whc_dev_xmit_init();

#ifdef CONFIG_WHC_WIFI_API_PATH
	whc_dev_api_init();
#endif
}
