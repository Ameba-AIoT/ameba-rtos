/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>
#include "its_s_tests.h"
#include "psa/internal_trusted_storage.h"
#include "test_framework_helpers.h"
#include "tfm_secure_client_2_api.h"
#include "../its_tests_common.h"

/* UID to test partition access control */
#define TEST_UID_ACCESS_CONTROL 42U

/* List of tests */
static void tfm_its_test_1020(struct test_result_t *ret);

static struct test_t psa_its_s_tests[] = {
    {&tfm_its_test_common_001, "TFM_S_ITS_TEST_1001",
     "Set interface"},
    {&tfm_its_test_common_002, "TFM_S_ITS_TEST_1002",
     "Set interface with create flags"},
    {&tfm_its_test_common_003, "TFM_S_ITS_TEST_1003",
     "Set interface with NULL data pointer"},
    {&tfm_its_test_common_004, "TFM_S_ITS_TEST_1004",
     "Set interface with write once UID"},
    {&tfm_its_test_common_005, "TFM_S_ITS_TEST_1005",
     "Get interface with valid data"},
    {&tfm_its_test_common_006, "TFM_S_ITS_TEST_1006",
     "Get interface with zero data length"},
    {&tfm_its_test_common_007, "TFM_S_ITS_TEST_1007",
     "Get interface with invalid UIDs"},
    {&tfm_its_test_common_008, "TFM_S_ITS_TEST_1008",
     "Get interface with data lengths and offsets greater than UID length"},
    {&tfm_its_test_common_009, "TFM_S_ITS_TEST_1009",
     "Get interface with NULL data pointer"},
    {&tfm_its_test_common_010, "TFM_S_ITS_TEST_1010",
     "Get info interface with write once UID"},
    {&tfm_its_test_common_011, "TFM_S_ITS_TEST_1011",
     "Get info interface with valid UID"},
    {&tfm_its_test_common_012, "TFM_S_ITS_TEST_1012",
     "Get info interface with invalid UIDs"},
    {&tfm_its_test_common_013, "TFM_S_ITS_TEST_1013",
     "Remove interface with valid UID"},
    {&tfm_its_test_common_014, "TFM_S_ITS_TEST_1014",
     "Remove interface with write once UID"},
    {&tfm_its_test_common_015, "TFM_S_ITS_TEST_1015",
     "Remove interface with invalid UID"},
    {&tfm_its_test_common_016, "TFM_S_ITS_TEST_1016",
     "Block compaction after remove"},
    {&tfm_its_test_common_017, "TFM_S_ITS_TEST_1017",
     "Multiple partial gets"},
    {&tfm_its_test_common_018, "TFM_S_ITS_TEST_1018",
     "Multiple sets to same UID from same thread"},
    {&tfm_its_test_common_019, "TFM_S_ITS_TEST_1019",
     "Set, get and remove interface with different asset sizes"},
    {&tfm_its_test_1020, "TFM_S_ITS_TEST_1023",
     "Attempt to get a UID set by a different partition"},
};

void register_testsuite_s_psa_its_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(psa_its_s_tests) / sizeof(psa_its_s_tests[0]));

    set_testsuite("PSA internal trusted storage S interface tests "
                  "(TFM_S_ITS_TEST_1XXX)",
                  psa_its_s_tests, list_size, p_test_suite);
}

/**
 * \brief Attempt to get a UID set by a different partition.
 *
 * \param[out] ret  Test result
 */
static void tfm_its_test_1020(struct test_result_t *ret)
{
    psa_status_t status;
    const psa_storage_uid_t uid = TEST_UID_ACCESS_CONTROL;

    /* Set the UID from this partition's context */
    status = psa_its_set(uid, WRITE_DATA_SIZE, WRITE_DATA,
                         PSA_STORAGE_FLAG_NONE);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Set should not fail");
        return;
    }

    /* Attempt to get the UID from the Secure Client 2 partition */
    status = tfm_secure_client_2_call_test(
                                         TFM_SECURE_CLIENT_2_ID_ITS_ACCESS_CTRL,
                                         &uid, sizeof(uid));
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Get should not succeed from a different partition");
        return;
    }

    /* Call remove to clean up storage for the next test */
    status = psa_its_remove(uid);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Remove should not fail with valid UID");
        return;
    }

    ret->val = TEST_PASSED;
}
