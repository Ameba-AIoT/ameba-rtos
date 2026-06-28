/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2020 Arm Limited
 */

#ifndef __FAULT_INJECTION_HARDENING_H__
#define __FAULT_INJECTION_HARDENING_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int fih_rc;

#define FIH_SUCCESS              0
#define FIH_FAILURE              1

#define FIH_DECLARE(name, val)
#define FIH_CALL(func, ret, ...) func(__VA_ARGS__)
#define FIH_NOT_EQ(a, b)         0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __FAULT_INJECTION_HARDENING_H__ */
