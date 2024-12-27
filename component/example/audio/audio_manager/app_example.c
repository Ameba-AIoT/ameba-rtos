/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_audio_manager.h"

#if USING_CMD
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE manager_test_cmd_table[] = {
	{
		(const u8 *)"audio_manager",  1, example_manager_test, (const u8 *)"\t audio_manager\n"
		"\t\t test cmd: ~audio_manager [-r] rate [-c] channels [-f] format  \n"
		"\t\t default params: [-r] 16000 [-c] 2 [-f] format 16 \n"
		"\t\t test demo: audio_manager -r 48000 -c 2 -f 16\n"
	},
};
#else
void app_example(void)
{
	example_manager();
}
#endif