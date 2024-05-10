/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_audio_hal_capture.h"

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE  audio_capture_test_cmd_table[] = {
	{
		(const u8 *)"AudioHwCapture",  1, CmdAudioHalCaptureTest, (const u8 *)"\tAudioHwCapture \n"
		"\t\t AudioHwCapture [-r] rate [-c] channels [-b] capture bytes one time\n"
	},
};