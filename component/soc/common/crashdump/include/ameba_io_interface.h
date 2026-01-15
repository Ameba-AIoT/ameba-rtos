/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_IO_INTERFACE_H_
#define _AMEBA_IO_INTERFACE_H_

// String IO Interface Definition
#define bsp_string_output DiagPrintfNano

#define error_printf(fmt, args...) bsp_string_output(fmt, ##args);
#define trace_printf(fmt, args...) bsp_string_output(fmt, ##args);
#define info_printf(fmt, args...) bsp_string_output(fmt, ##args);
#define debug_printf(fmt, args...)

#endif
