#include "whc_dev.h"
#include "os_wrapper.h"
#include "whc_wpa_ops_cmd_define.h"
#include "whc_wpa_ops_cmd_parse.h"

extern int start_wpa_supplicant(char *iface_name);

int whc_wpa_ops_init_wpas_std(u8 *ptr, u8 *buf)
{
	(void)ptr;
	(void)buf;

	start_wpa_supplicant(NULL);

	return 0;
}
