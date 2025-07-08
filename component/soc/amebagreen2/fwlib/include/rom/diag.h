/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DIAG_H_
#define _DIAG_H_

#include "platform_autoconf.h"
#include "basic_types.h"
#include <stdarg.h>

typedef void (*out_fct_type)(char character, char *buffer, size_t idx, size_t maxlen);

int DiagPrintf(const char *fmt, ...);
int DiagVSNprintf(char *buf, size_t maxlen, const char *fmt,  va_list ap);
int DiagVSprintf(char *buf, const char *fmt, va_list ap);
int DiagVprintf(const char *fmt, va_list ap);
int DiagSPrintf(u8 *buf, const char *fmt, ...);
int DiagSnPrintf(char *buf, size_t size, const char *fmt, ...);
int DiagPrintfNano(const char *fmt, ...);
int DiagVprintfNano(const char *fmt, va_list ap);
int DiagVSNprintfNano(char *buf, size_t maxlen, const char *fmt,  va_list ap);

/** @brief Log Module Definition */
typedef enum {
	MODULE_USB_OTG, /**< usb otg */
	MODULE_USB_CLASS, /**< usb class */
	MODULE_NUMs		/**< Module Number */
} MODULE_DEFINE;

/** @brief Log Level Definition */
typedef enum {
	LEVEL_ERROR	= 0, /**< Error */
	LEVEL_WARN	= 1, /**< Warning */
	LEVEL_INFO		= 2, /**< Information */
	LEVEL_TRACE	= 3, /**< Trace Data */
	LEVEL_NUMs		= 4  /**< Level Number */
} LEVEL_DEFINE;

#define DBG_PRINTF(MODULE, LEVEL, pFormat, ...)     do {\
   if ((LEVEL < LEVEL_NUMs) && (MODULE < MODULE_NUMs) )\
        DiagPrintf("["#MODULE"-"#LEVEL"]:"pFormat, ##__VA_ARGS__);\
}while(0)

#endif //_DIAG_H_
