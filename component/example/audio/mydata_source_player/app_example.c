/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_mydata_source_player.h"

#ifdef CMD_TEST
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE cutils_test_cmd_table[] = {
	{
		(const u8 *)"mydata_source_player",  1, example_mydata_source_player_test, (const u8 *)"\t mydata_source_player\n"
	},
};
#else
void app_example(void)
{
	example_mydata_source_player();
}
#endif
