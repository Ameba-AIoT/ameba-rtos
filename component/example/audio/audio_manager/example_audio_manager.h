#ifndef _EXAMPLE_AUDIO_MANAGER_H_
#define _EXAMPLE_AUDIO_MANAGER_H_

#define USING_CMD 0

#if USING_CMD
u32 example_manager_test(u16 argc, u8 *argv[]);
#else
void example_manager(void);
#endif

#endif