#include <string.h>
#include "bt_debug.h"
#include "bt_olt_common.h"
#include "hci_common.h"

bool rtk_bt_olt_power_on(void)
{
	BT_LOGA("rtk_bt_olt_power_on\n\r");
	hci_olt_open();

	return true;
}

bool rtk_bt_olt_enable_inquiry(void)
{
	uint8_t buf[8 + 3] = {0};

	buf[2] = 5;
	buf[3] = 0x33;
	buf[4] = 0x8b;
	buf[5] = 0x9e;
	buf[6] = 0x0f;
	buf[7] = 0x00;

	if (true != hci_if_olt_send_status(0x0401, buf, buf[2] + 3)) {
		BT_LOGE("hci_if_olt_send_status fail\n\r");
		return false;
	}
	return true;
}