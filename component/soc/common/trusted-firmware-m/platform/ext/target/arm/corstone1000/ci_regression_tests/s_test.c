/*
 * Copyright (c) 2021-22, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "extra_s_tests.h"
#include "platform_base_address.h"
#include "firewall.h"
#include "tfm_sp_log.h"
#include "s_io_storage_test.h"

/* TODO: if needed each test function can be made as a separate test case, in
 * such case EXTRA_TEST_XX definitions can be removed */
#define EXTRA_TEST_SUCCESS 0
#define EXTRA_TEST_FAILED -1

#define DISABLED_TEST 0

int test_io_storage_multiple_flash_simultaneous(void);

enum host_firewall_host_comp_id_t {
  HOST_FCTRL = (0x00u),
  COMP_SYSPERIPH,
  COMP_DBGPERIPH,
  COMP_AONPERIPH,
  COMP_XNVM,
  COMP_CVM,
  COMP_HOSTCPU,
  COMP_EXTSYS0,
  COMP_EXTSYS1,
  COMP_EXPSLV0,
  COMP_EXPSLV1,
  COMP_EXPMST0,
  COMP_EXPMST1,
  COMP_OCVM,
  COMP_DEBUG,
};

static int test_host_firewall_status(void)
{
    enum fw_lockdown_status_t status;
    uint32_t any_component_id = 2;

    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, any_component_id);
    status = fw_get_lockdown_status();
    if (status != FW_LOCKED) {
        LOG_INFFMT("FAIL: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

static int test_host_firewall_external_flash_configurations(void)
{
    enum rgn_mpl_t mpl_rights = 0;
    enum rgn_mpl_t expected_rights = 0;

#if !(PLATFORM_IS_FVP)
    /* External flash */
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_EXPMST0);
    fc_select_region(3);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK | RGN_MPL_SECURE_READ_MASK |
                                              RGN_MPL_SECURE_WRITE_MASK);
    if (mpl_rights != expected_rights) {
        LOG_INFFMT("FAIL1: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
    /* XIP Permissions */
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_XNVM);
    fc_select_region(1);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK |
                              RGN_MPL_SECURE_READ_MASK |
                              RGN_MPL_NONSECURE_READ_MASK);
    if (mpl_rights != expected_rights) {
        LOG_INFFMT("FAIL2: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
#else
    /* Enable the below test when FVP Host Firewall is configured. */
    /*
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_XNVM);
    fc_select_region(1);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    tfm_log_printf("mpl rights = %d\n\r", mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK |
                  RGN_MPL_SECURE_READ_MASK |
                  RGN_MPL_SECURE_WRITE_MASK |
                  RGN_MPL_NONSECURE_READ_MASK |
                  RGN_MPL_NONSECURE_WRITE_MASK);
    if (mpl_rights != expected_rights) {
        tfm_log_printf("FAIL1: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
    */
#endif

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

static int test_host_firewall_secure_flash_configurations(void)
{
    enum rgn_mpl_t mpl_rights = 0;
    enum rgn_mpl_t expected_rights = 0;

#if !(PLATFORM_IS_FVP)
    /* External flash */
    fc_select((void *)CORSTONE1000_HOST_FIREWALL_BASE, COMP_EXPMST1);
    fc_select_region(1);
    fc_read_mpl(RGN_MPE0, &mpl_rights);
    expected_rights = (RGN_MPL_ANY_MST_MASK | RGN_MPL_SECURE_READ_MASK |
                                              RGN_MPL_SECURE_WRITE_MASK);
    if (mpl_rights != expected_rights) {
        LOG_INFFMT("FAIL: %s.\n\r", __func__);
        return EXTRA_TEST_FAILED;
    }
#endif

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

static int test_bir_programming(void)
{
    /* BIR is expected to bhaive like write once register */

    volatile uint32_t *bir_base = (uint32_t *)CORSTONE1000_HOST_BIR_BASE;

    bir_base[0] = 0x1;
    bir_base[0] = 0x2;
    if (bir_base[0] != 0x1) {
        LOG_INFFMT("FAIL: %s : (%u)\n\r", __func__, bir_base[0]);
        return EXTRA_TEST_FAILED;
    }

    LOG_INFFMT("PASS: %s\n\r", __func__);
    return EXTRA_TEST_SUCCESS;
}

void s_test(struct test_result_t *ret)
{
    int status;
    int failures = 0;

#if (DISABLED_TEST == 1)
    status = test_host_firewall_status();
    if (status) {
        failures++;
    }
#endif

    status = test_host_firewall_secure_flash_configurations();
    if (status) {
        failures++;
    }

    status = test_host_firewall_external_flash_configurations();
    if (status) {
        failures++;
    }

#if (DISABLED_TEST == 1)
    status = test_bir_programming();
    if (status) {
        failures++;
    }
#endif

    if (failures) {
        LOG_INFFMT("Not all platform test could pass: failures=%d\n\r", failures);
        ret->val = TEST_FAILED;
        return;
    }

    LOG_INFFMT("ALL_PASS: corstone1000 platform test cases passed.\n\r");
    ret->val = TEST_PASSED;
}

static struct test_t plat_s_t[] = {
    {&s_test, "TFM_S_EXTRA_TEST_1001",
     "Extra Secure test"},
    {&s_test_io_storage_multiple_flash_simultaneous, "TFM_S_EXTRA_TEST_1002",
     "Extra Secure test: io storage access multiple flash simultaneous"},
};

void register_testsuite_extra_s_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(plat_s_t) /
                 sizeof(plat_s_t[0]));

    set_testsuite("Extra Secure interface tests"
                  "(TFM_S_EXTRA_TEST_1XXX)",
                  plat_s_t, list_size, p_test_suite);
}
