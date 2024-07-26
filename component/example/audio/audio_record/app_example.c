/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_audio_record.h"

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE cutils_test_cmd_table[] = {
	{
		(const u8 *)"audio_record",  1, example_record_test, (const u8 *)"\t audio_record\n"
		"\t\t test cmd: audio_record [-r] rate [-b] record_bytes_one_time [-c] record_channels [-m] record_mode [-f] format  \n"
		"\t\t           [-or] 1:only do record, 0:record then play [-noirq] 1:irq mode, 0:no irq mode [-ref] 1:test ref 0: not test ref\n"
		"\t\t           [-pres] 1: pressure test, 0: record fixed time [-cxs] mic source for channelx, exp, -c0s: mic src for channel0 \n"
		"\t\t default params: [-r] 16000 [-b] 8192 [-c] 2 [-m] 0 [-f] format 16 [-or] 0 [-noirq] 0 [-ref] 0\n"
		"\t\t record_mode: 0:no_afe_pure_data; 1:no_afe_all_data \n"
		"\t\t test demo: audio_record -r 16000 -b 8192 \n"
		"\t\t test noirq, -b should be 8ms bytes: audio_record -c 1 -b 256 -noirq 1 -r 16000;\n"
	},
};