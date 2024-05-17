/*
 * Copyright(c) 2020, Realtek Semiconductor Corp. All rights reserved.
 *
 * Author: CTC PSP Software
 *
 */

#ifndef _AMEBA_IO_INTERFACE_H_
#define _AMEBA_IO_INTERFACE_H_

// String IO Interface Definition
#define bsp_string_output DiagPrintf


#ifdef CONFIG_DEBUG_TINY_CRASH_DUMP
#define error_printf(fmt, args...) bsp_string_output(fmt, ##args);
#define trace_printf(fmt, args...) bsp_string_output(fmt, ##args);
#define info_printf(fmt, args...) bsp_string_output(fmt, ##args);

#else
#define error_printf(fmt, args...) bsp_string_output("\033[1;33m[ERROR] [%s::%d] " fmt "\033[0m", __FUNCTION__, __LINE__, ##args);

#define trace_printf(fmt, args...) bsp_string_output("\033[1;33m[TRACE] [%s::%d] " fmt "\033[0m", __FUNCTION__, __LINE__, ##args);

#define info_printf(fmt, args...) bsp_string_output("\033[1;33m[INFO] [%s::%d] " fmt "\033[0m", __FUNCTION__, __LINE__, ##args);

#endif /* CONFIG_DEBUG_TINY_CRASH_DUMP */

#ifdef BACKTRACE_DEBUG
#define debug_printf(fmt, args...) bsp_string_output(fmt, ##args);
#else
#define debug_printf(fmt, args...)

#endif

#endif
