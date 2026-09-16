#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include "bt_debug.h"
#include <bt_olt_common.h>
#include <bt_olt_api.h>

extern void hci_platform_set_tx_power_gain_index(uint32_t index);
void olt_bt_inquiry_loop(void)
{
	while (1) {
		BT_LOGA("olt_bt_inquiry_loop\n\r");
		rtk_bt_olt_enable_inquiry();
		osif_delay(20000);
	}
}

void OLT_BTTest(void *ctx)
{
	(void)ctx;
	BT_LOGA("OLT inquiry per 20s\n\r");

	hci_platform_set_tx_power_gain_index(0x5f5f6060);
	rtk_bt_olt_power_on();

	olt_bt_inquiry_loop();

	BT_LOGA("shouldn't come here forever !!!! \n\r");
	osif_task_delete(NULL);
}