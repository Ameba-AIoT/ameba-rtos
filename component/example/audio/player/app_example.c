/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_player.h"

#ifdef CMD_TEST
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE cutils_test_cmd_table[] = {
	{
		(const u8 *)"player",  1, example_player_test, (const u8 *)"\t player\n"
	},
};
#else
void app_example(void)
{
	example_player();
}
#endif
