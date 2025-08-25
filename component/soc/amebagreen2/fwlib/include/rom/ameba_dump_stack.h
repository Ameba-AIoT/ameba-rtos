/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DUMP_STACK_H_
#define _AMEBA_DUMP_STACK_H_

#define bsp_string_output DiagPrintf

#define trace_printf(fmt, args...) bsp_string_output(fmt, ##args);

#define MIN_DUMP_DEPTH  0x80
#define MAX_DUMP_DEPTH  0x4000
#define DUMP_STACK_MIN_DEPTH(depth)     ((depth >= MIN_DUMP_DEPTH) && (depth <= MAX_DUMP_DEPTH) ? depth:MIN_DUMP_DEPTH)

typedef uint32_t (*crashdump_Patch)(uint32_t *sp, uint32_t *ex_pc);

// extern uint32_t crash_dump_depth;
/***
*  @brief	Gets the exception stack content
*
*  @param	sp, stack pointer while crash
*
*  @param	depth, exception stack depth while crash
*
*  @return	None
*
***/
void crash_GetExStackContent(uint32_t *sp, uint32_t depth);
/***
*  @brief	Get a exception stack depth that can be dumped
*
*  @return	Exception stack depth
*
***/
uint32_t crash_GetExStackDepth(void);

/***
*  @brief	Sets the depth at which the exception stack can be dumped
*
*  @param	depth, exception stack depth
*
*  @return	none
*
***/
void crash_SetExStackDepth(uint32_t depth);

#endif
