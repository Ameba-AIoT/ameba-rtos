/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _STRPROC_PRIVATE_H_
#define _STRPROC_PRIVATE_H_

#ifndef isprint
#define in_range(c, lo, up)  ((u8)c >= lo && (u8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
//#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == ',')

#define isupper(c)	(((c)>='A')&&((c)<='Z'))
#define islower(c)	(((c)>='a')&&((c)<='z'))
#define isalpha(c)	(isupper(c) || islower(c))
#endif

/*
 * Fast implementation of tolower() for internal usage. Do not use in your
 * code.
 */
#ifndef _tolower
__attribute__((always_inline)) static inline char _tolower(const char c)
{
	return c | 0x20;
}
#endif //_tolower

/* Fast check for octal digit */
__attribute__((always_inline)) static inline int isodigit(const char c)
{
	return c >= '0' && c <= '7';
}

#endif // _STRPROC_PRIVATE_H_
