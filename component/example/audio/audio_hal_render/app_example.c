/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_audio_hal_render.h"

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE  audio_render_test_cmd_table[] = {
	{
		(const u8 *)"AudioHalRender",  1, CmdAudioHalRenderTest, (const u8 *)"\tAudioHalRender \n"
		"\t\t AudioHalRender [-b] format bits\n"
	},
};