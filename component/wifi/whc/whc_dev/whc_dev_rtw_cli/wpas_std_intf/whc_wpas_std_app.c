#include "whc_dev.h"
#include "os_wrapper.h"
#include "whc_dev_rtw_cli_cmd_define.h"
#include "whc_dev_rtw_cli_cmd_parse.h"

extern int start_wpa_supplicant(char *iface_name);

int whc_wpa_ops_init_wpas_std(u8 *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	start_wpa_supplicant(NULL);

	return 0;
}


int whc_wpa_ops_get_status(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}

int whc_dev_rtw_cli_remove_network(char *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	return 0;
}


int whc_dev_rtw_cli_wpas_test(char *ptr, u8 *buf, int msg_len)
{
	int value = 0;
	(void)buf;

	RTK_LOGE(TAG_WLAN_INIC, "%s,  value: %s\n", __func__, ptr);

	wpas_std_test_cmd(ptr, msg_len);


	return 0;
}

