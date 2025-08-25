/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RAND_H_
#define _RAND_H_

_LONG_CALL_ extern u32 _rand(void);
_LONG_CALL_ extern int RandBytes_Get(void *buf, u32 len);

#endif //_RAND_H_