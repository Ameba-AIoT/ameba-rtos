/*
 * Copyright (c) 2017-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_app.h"
#include "tfm_log.h"

#ifdef TEST_FRAMEWORK_NS
#include "test_framework_integ_test.h"
#endif

#if PSA_API_TEST_NS
/**
 * \brief This symbol is the entry point provided by the PSA API compliance
 *        test libraries
 */
extern void val_entry(void);
#endif

/**
 * \brief Services test thread
 *
 */
__attribute__((noreturn))
void test_app(void *argument)
{
    UNUSED_VARIABLE(argument);

#ifdef TEST_FRAMEWORK_NS
    tfm_non_secure_client_run_tests();
#endif

#ifdef PSA_API_TEST_NS
    val_entry();
#endif

    /* Output EOT char for test environments like FVP. */
    LOG_MSG("\x04");

    /* End of test */
    for (;;) {
    }
}
