#define _EXAMPLE_PLAYER_H_
#ifdef _EXAMPLE_PLAYER_H_

#ifdef AEC_TEST
#include "platform_stdlib.h"
u32 example_player_test(u16 argc, u8 *argv[]);
u32 player_pause(u16 argc, u8 *argv[]);
u32 player_stop(u16 argc, u8 *argv[]);
u32 player_resume(u16 argc, u8 *argv[]);
u32 player_set_volume(u16 argc, u8 *argv[]);
u32 player_set_loop(u16 argc, u8 *argv[]);
#endif
#endif


