/******************************************************************************
*
* Copyright(c) 2007 - 2018 Realtek Corporation. All rights reserved.
*
******************************************************************************/
#include "ameba_soc.h"
#include "example_audio_track.h"

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE track_test_cmd_table[] = {
	{
		(const u8 *)"audio_track",  1, example_track_test, (const u8 *)"\t audio_track\n"
		"\t\t test cmd: ~audio_track [-r] rate [-b] write_frames_one_time [-c] track_channels [-f] format  \n"
		"\t\t default params: [-r] 16000 [-p] 1024 [-c] 2 [-f] format 16 \n"
		"\t\t test demo: audio_track -r 48000 -c 1 \n"
		"\t\t careful: if you set SINE_GEN_EVERY_TIME as 0, please remember to set -b as [integer * rate * 1 / g_freq]\n"
	},
};