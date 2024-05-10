/*
 * Copyright (c) 2017-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_TEST_APP_H__
#define __TFM_TEST_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Simple macro to mark UNUSED variables
 *
 */
#define UNUSED_VARIABLE(X) ((void)(X))

/**
 * \brief Main test application for the RTX-TFM core
 *        integration tests
 *
 */
void test_app(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_TEST_APP_H__ */
