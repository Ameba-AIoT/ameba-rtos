/*
 * Copyright(c) 2020, Realtek Semiconductor Corp. All rights reserved.
 *
 * Author: CTC PSP Software
 *
 */

#ifndef _AMEBA_IO_INTERFACE_H_
#define _AMEBA_IO_INTERFACE_H_

// String IO Interface Definition
#define bsp_string_output DiagPrintfNano

#define error_printf(fmt, args...) bsp_string_output(fmt, ##args);
#define trace_printf(fmt, args...) bsp_string_output(fmt, ##args);
#define info_printf(fmt, args...) bsp_string_output(fmt, ##args);

#ifndef CONFIG_BACK_TRACE_DEPTH_LIMIT
#define CONFIG_BACK_TRACE_DEPTH_LIMIT 10
#endif

#ifdef BACKTRACE_DEBUG
#define debug_printf(fmt, args...) bsp_string_output(fmt, ##args);
#else
#define debug_printf(fmt, args...)

#endif

#endif
