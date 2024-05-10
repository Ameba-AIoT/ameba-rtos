/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2017 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"

void test_app(void *argument);

#ifdef CONFIG_TFM_TEST_PSA_API
/*PSA API need auto test*/
void app_example(void)
{
	if (pdTRUE != xTaskCreate(test_app, "test_app", 1024,
							  NULL, tskIDLE_PRIORITY + 5, NULL)) {
		DiagPrintf("Create test_app Err!!\n");
	}
}
#endif

u32 CmdTfmTest(IN u16 argc, IN u8  *argv[])
{
	(void) argc;	/* To avoid gcc warnings */
	(void) argv;	/* To avoid gcc warnings */

	if (pdTRUE != xTaskCreate(test_app, "test_app", 1024,
							  NULL, tskIDLE_PRIORITY + 5, NULL)) {
		DiagPrintf("Create test_app Err!!\n");
	}

	return _TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   tfm_test_cmd_table[] = {
	{
		(const u8 *)"TFM", 2, CmdTfmTest, (const u8 *)"\tTFM \n"
		"\t\t <VeriItem, strings> <sub-command to refer to the tfm_test_cmd.c, Dec>\n"
		"\t\t VeriItem: \n"
	},
};

