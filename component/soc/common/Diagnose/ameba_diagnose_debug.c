#include "ameba_diagnose_types.h"
#include "ameba_diagnose.h"
#include "ameba_soc.h"
#include <string.h>

int rtk_diag_req_add_event_demo1(u8 evt_level, const char *data)
{
	struct diag_evt_demo_1 demo = {
		.u8_data = 1,
		.u16_data = 2,
		.f_data = 1.1,
		.ch_data = 'r',
	};
	strncpy(demo.str_data, data, 31);
	return rtk_diag_event_add(evt_level, RTK_EVENT_TYPE_DBG_DEMO_1, (const u8 *)&demo, sizeof(struct diag_evt_demo_1) - (32 - (strlen(data) + 1)));
}