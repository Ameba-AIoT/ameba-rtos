#include "whc_dev.h"

/**
 * @brief  to initialize the inic device.
 * @param  none.
 * @return none.
 */
void whc_fullmac_spi_dev_init(void)
{
	rtk_log_level_set(TAG_WLAN_INIC, RTK_LOG_DEBUG);

	wifi_set_user_config();
	init_skb_pool(wifi_user_config.skb_num_np, wifi_user_config.skb_buf_size ? wifi_user_config.skb_buf_size : MAX_SKB_BUF_SIZE, SKB_CACHE_SZ);

	whc_spi_dev_init();

	/* initialize the dev priv */
	whc_dev_init_priv();
#ifdef CONFIG_WHC_WIFI_API_PATH
	whc_dev_api_init();
#endif
}