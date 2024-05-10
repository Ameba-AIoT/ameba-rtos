/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "fwu_tests_common.h"
#include "region_defs.h"

/* Size of image header (bytes). It takes two bytes in the image header. */
#define IMAGE_HEADER_SIZE        BL2_HEADER_SIZE
#define IMAGE_HEADER_SIZE_LOW    (uint8_t)(IMAGE_HEADER_SIZE & 0x000000FF)
#define IMAGE_HEADER_SIZE_HIGH  \
            (uint8_t)((IMAGE_HEADER_SIZE & 0x0000FF00) >> 8)

/* Size of the secure image itself. Does not include header. It takes four bytes
 * in the image header.
 */
#define SECURE_IMAGE_SIZE         IMAGE_S_CODE_SIZE
#define SECURE_IMAGE_SIZE_BYTE_0  \
            (uint8_t)(SECURE_IMAGE_SIZE & 0x000000FF)
#define SECURE_IMAGE_SIZE_BYTE_1  \
            (uint8_t)((SECURE_IMAGE_SIZE & 0x0000FF00) >> 8)
#define SECURE_IMAGE_SIZE_BYTE_2  \
            (uint8_t)((SECURE_IMAGE_SIZE & 0x00FF0000) >> 16)
#define SECURE_IMAGE_SIZE_BYTE_3  \
            (uint8_t)((SECURE_IMAGE_SIZE & 0xFF000000) >> 24)

/* The _off value of the protected TLV area in secure image. */
#define PROTECTED_TLV_OFF_SECURE (IMAGE_HEADER_SIZE + SECURE_IMAGE_SIZE)

/* Size of the nonsecure image itself. Does not include header. It takes four
 * bytes in the image header.
 */
#define NONSECURE_IMAGE_SIZE         IMAGE_NS_CODE_SIZE
#define NONSECURE_IMAGE_SIZE_BYTE_0  \
            (uint8_t)(NONSECURE_IMAGE_SIZE & 0x000000FF)
#define NONSECURE_IMAGE_SIZE_BYTE_1  \
            (uint8_t)((NONSECURE_IMAGE_SIZE & 0x0000FF00) >> 8)
#define NONSECURE_IMAGE_SIZE_BYTE_2  \
            (uint8_t)((NONSECURE_IMAGE_SIZE & 0x00FF0000) >> 16)
#define NONSECURE_IMAGE_SIZE_BYTE_3  \
            (uint8_t)((NONSECURE_IMAGE_SIZE & 0xFF000000) >> 24)

/* The _off value of the protected TLV area in nonsecure image. */
#define PROTECTED_TLV_OFF_NONSECURE (IMAGE_HEADER_SIZE + NONSECURE_IMAGE_SIZE)

#if (MCUBOOT_IMAGE_NUMBER > 1)
#define dependency_component FWU_COMPONENT_ID_NONSECURE
/* The off of the first byte of depdency version in the TLV area of secure
 * image.
 */
#define TLV_DEPENDENCY_VERSION_OFF_SECURE           115

/* The off of the first byte of depdency version in the TLV area of nonsecure
 * image.
 */
#define TLV_DEPENDENCY_VERSION_OFF_NONSECURE        116

/* The off of the image version in the image header. */
#define HEADER_IMAGE_VERSION_OFF                    20
#endif

/* Version: Major */
#define   IMAGE_VERSION_FOR_TEST_MAJOR              0x09

/* Version: Minor */
#define   IMAGE_VERSION_FOR_TEST_MINOR              0x09

/* Version: Low byte o f revision */
#define   IMAGE_VERSION_FOR_TEST_REVISION_LOW       0x09

/* Version: High byte o f revision */
#define   IMAGE_VERSION_FOR_TEST_REVISION_HIGH      0x00

#if MCUBOOT_IMAGE_NUMBER > 1
static psa_fwu_component_t test_component = FWU_COMPONENT_ID_SECURE;
#else
static psa_fwu_component_t test_component = FWU_COMPONENT_ID_FULL;
#endif

/* The secure image header(from image header magic to image version). The image
 * version is 0.0.0 + 0.
 */
static const uint8_t header_test_image_version_zero[28] = \
                        { 0x3d, 0xb8, 0xf3, 0x96, /* IMAGE_MAGIC */ \
                          0x00, 0x00, 0x00, 0x00,  /* LoadAddr */  \
                          /* Image Header size */
                          IMAGE_HEADER_SIZE_LOW, IMAGE_HEADER_SIZE_HIGH, \
                          0x7B, 0x00, /* Protected TLV size */  \
                          /* Image size */
                          SECURE_IMAGE_SIZE_BYTE_0, SECURE_IMAGE_SIZE_BYTE_1, \
                          SECURE_IMAGE_SIZE_BYTE_2, SECURE_IMAGE_SIZE_BYTE_3, \
                          0x00, 0x00, 0x00, 0x00, /* Flags */  \
                          0x00, /* Version: Major */  \
                          0x00, /* Version: Minor */  \
                          0x00, 0x00, /* Version: Revision */  \
                          0x00, 0x00, 0x00, 0x00, /* Version: Build number */  \
                        };

/* The protected TLV data + part of the TLV data of the secure image.
 * The dependency nonsecure image and MIN_VER:0.0.0+0.
 */
static const uint8_t tlv_dep_version_zero[128] = \
                        { 0x08, 0x69, /* IMAGE_TLV_PROT_INFO_MAGIC */  \
                          0x7B, 0x00, /* The size of protected TLV area */
                          /* Secure count: */
                          0x50, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x01,  \
                          /* Boot record: */
                          0x60, 0x00, 0x5b, 0x00, 0xa5, 0x01, 0x63, 0x53,  \
                          0x50, 0x45, 0x04, 0x65, 0x31, 0x2e, 0x33, 0x2e,  \
                          0x30, 0x05, 0x58, 0x20, 0xfc, 0x57, 0x01, 0xdc,  \
                          0x61, 0x35, 0xe1, 0x32, 0x38, 0x47, 0xbd, 0xc4,  \
                          0x0f, 0x04, 0xd2, 0xe5, 0xbe, 0xe5, 0x83, 0x3b,  \
                          0x23, 0xc2, 0x9f, 0x93, 0x59, 0x3d, 0x00, 0x01,  \
                          0x8c, 0xfa, 0x99, 0x94, 0x06, 0x66, 0x53, 0x48,  \
                          0x41, 0x32, 0x35, 0x36, 0x02, 0x58, 0x20, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,        \
                          /* DEPENDENCY: nonsecure, MIN_VER:0.0.0+0*/
                          0x40, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          /* custom_tlvs: None */
                          0x07, 0x69, /* IMAGE_TLV_INFO_MAGIC */  \
                          0x50, 0x01, 0x10 /* Part of the SHA256 */  \
                        };

/* Return 0 if the test suite is setup successfully. */
static int8_t test_setup(uint8_t test_suite_number, struct test_result_t *ret)
{
    psa_status_t status;
    psa_fwu_component_info_t info;

    /* Query the state of the component. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_LOG("Test: %d setup failed.", test_suite_number);
        TEST_FAIL("Query should succeed");
        return -1;
    }

    /* If the component isn't in READY state, skip running the FWU tests. */
    if (info.state != PSA_FWU_READY) {
        TEST_LOG("The component isn't in READY state: the device is not ready to run FWU tests, skip FWU test: %d.",
                 test_suite_number);
        ret->val = TEST_PASSED;
        return 1;
    }
    return 0;
}

static void test_tear_down(uint8_t test_suite_number, struct test_result_t *ret)
{
    psa_status_t status;
    psa_fwu_component_info_t info;

    /* Cancel a FWU process when a component is in WRITTING or CANDIDATE state. */
    status = psa_fwu_cancel(test_component);
    if (status != PSA_SUCCESS) {
        TEST_LOG("Test: %d tear down failed.", test_suite_number);
        TEST_FAIL("psa_fwu_cancel should succeed");
        return;
    }
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_LOG("Test: %d tear down failed.", test_suite_number);
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_FAILED) {
        TEST_LOG("The component is expected to be in PSA_FWU_FAILED after psa_fwu_cancel(), actual state is %d",
                  info.state);
        TEST_LOG("Test: %d tear down failed.", test_suite_number);
        TEST_FAIL("Wrong component state is returned.");
    }

    /* Clean the component to make the component back to READY state. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_SUCCESS) {
        TEST_LOG("Test: %d tear down failed.", test_suite_number);
        TEST_FAIL("psa_fwu_clean should succeed");
        return;
    }

    /* Query the staging image. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_LOG("Test: %d tear down failed.", test_suite_number);
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_READY) {
        TEST_LOG("The component is expected to be in PSA_FWU_READY after psa_fwu_clean(), actual state is %d",
                  info.state);
        TEST_LOG("Test: %d tear down failed.", test_suite_number);
        TEST_FAIL("Wrong component state is returned.");
    }
}

void tfm_fwu_test_common_001(struct test_result_t *ret)
{
    psa_status_t status;
    psa_fwu_component_info_t info;
    uint8_t test_suite_number = 1;

    if (test_setup(test_suite_number, ret) != 0) {
        /* Test result(ret) is set within test_setup(). */
        return;
    }

    /* Trigger a FWU process. Image manifest is bundled within the image. */
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start fails.");
        return;
    }

    /* Query the component. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query fails");
        return;
    }

    /* The component should be in PSA_FWU_WRITING after psa_fwu_start(). */
    if (info.state != PSA_FWU_WRITING) {
        TEST_LOG("The component is expected to be in PSA_FWU_WRITING after psa_fwu_start(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
        return;
    }

    /* The image header(from header magic to image version) is downloaded
     * into the target device by this psa_fwu_write operation.
     */
    status = psa_fwu_write(test_component,
                           0,
                           header_test_image_version_zero,
                           sizeof(header_test_image_version_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the image header data.");
        return;
    }

    /* Query the staging image. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query fails");
        return;
    }

    /* The component should be in PSA_FWU_WRITING after psa_fwu_write(). */
    if (info.state != PSA_FWU_WRITING) {
        TEST_LOG("The component is expected to be in PSA_FWU_WRITING after psa_fwu_write(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
        return;
    }

    /* Write the protected TLV + IMAGE_TLV_INFO_MAGIC + part of the SHA256.
     * The image dependency information is downloaded into the target device
     * by this psa_fwu_write operation.
     */
    status = psa_fwu_write(test_component,
                           (size_t)PROTECTED_TLV_OFF_SECURE,
                           tlv_dep_version_zero,
                           sizeof(tlv_dep_version_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the protected TLV data.");
        return;
    }

    status = psa_fwu_finish(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_finish should not fail.");
        return;
    }

    /* Query the staging image. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query fails");
        return;
    }

    /* The component should be in PSA_FWU_WRITING after psa_fwu_write(). */
    if (info.state != PSA_FWU_CANDIDATE) {
        TEST_LOG("The component is expected to be in PSA_FWU_CANDIDATE after psa_fwu_finish(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
        return;
    }

    status = psa_fwu_install();

    /* Dependency check in multiple images:
     * dependency image ID: nonsecure image.
     * dependency image version: 0.0.0 + 0.
     * Currently running nonsecure image version: 0.0.0 + 0
     * So the dependency check should pass.
     * In the current implementation, image verification is deferred to
     * reboot, so PSA_SUCCESS_REBOOT is returned when success.
     */
    if (status != PSA_SUCCESS_REBOOT) {
        TEST_FAIL("Install should not fail when the image dependency check should pass.");
        return;
    }

    /* Query the staging image. */
    if (psa_fwu_query(test_component, &info) != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_STAGED) {
        TEST_LOG("The component is expected to be in PSA_FWU_STAGED after psa_fwu_install(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
        return;
    }

    /* The state of the component should change to FAILED after reject and PSA_SUCCESS should be returned. */
    status = psa_fwu_reject(PSA_ERROR_GENERIC_ERROR);
    if (status != PSA_SUCCESS) {
        TEST_LOG("PSA_SUCCESS is expected to be return by psa_fwu_reject when component is in STAGED state, \
                  actual return status is %d",
                  status);
        TEST_FAIL("Wrong error code is returned.");
        return;
    }

    /* Query the staging image. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_FAILED) {
        TEST_LOG("The component is expected to be in PSA_FWU_FAILED after psa_fwu_reject(PSA_ERROR_NOT_PERMITTED), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
        return;
    }

    /* Check the error code. */
    if (info.error != PSA_ERROR_GENERIC_ERROR) {
        TEST_LOG("The error is expected to be PSA_ERROR_GENERIC_ERROR, actual error is %x",
                  info.error);
        TEST_FAIL("Wrong info.error is returned.");
        return;
    }

    /* Clean the component to make the component back to READY state. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_clean should succeed");
        return;
    }

    /* Query the staging image. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_READY) {
        TEST_LOG("The component is expected to be in PSA_FWU_READY after psa_fwu_clean(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
        return;
    }

    /* Trigger a FWU process again after the end of the former FWU process. */
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start fails.");
        return;
    }

    /* Query the staging image. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* The component should be in PSA_FWU_WRITING after psa_fwu_start(). */
    if (info.state != PSA_FWU_WRITING) {
        TEST_LOG("The component is expected to be in PSA_FWU_WRITING after psa_fwu_start(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
        return;
    }

    /* Test tear down. */
    test_tear_down(test_suite_number, ret);
    if (ret->val == TEST_FAILED) {
        /* Test result has been set within test_tear_down() if it fails. */
        return;
    }

    /* End of the test. The candidate is back to PSA_FWU_READY state now. */
    ret->val = TEST_PASSED;
}

/* Write with NULL block. Query with NULL info. */
void tfm_fwu_test_common_002(struct test_result_t *ret)
{
    psa_status_t status;
    uint8_t test_suite_number = 2;

    if (test_setup(test_suite_number, ret) != 0) {
        /* Test result(ret) is set within test_setup(). */
        return;
    }

    /* Trigger a FWU process. Image manifest is bundled within the image. */
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start fails.");
        return;
    }

#ifdef TFM_FWU_TEST_WRITE_WITH_NULL
    /* Write the data block NULL. */
    status = psa_fwu_write(test_component,
                           0,
                           NULL,
                           sizeof(header_test_image_version_zero));
    if (status == PSA_SUCCESS) {
        TEST_FAIL("Write should fail with data block NULL");
        return;
    }
#endif

    /* Write the data block with block_offset + len overflow. */
    status = psa_fwu_write(test_component,
                           (size_t)0xFFFFFFFF,
                           header_test_image_version_zero,
                           0x10);
    if (status == PSA_SUCCESS) {
        TEST_FAIL("Write should fail with len + image_offset overflow");
        return;
    }

#ifdef TFM_FWU_TEST_QUERY_WITH_NULL
    /* Query the component. */
    status = psa_fwu_query(test_component, NULL);
    if (status == PSA_SUCCESS) {
        TEST_FAIL("Query should fail with NULL info");
        return;
    }
#endif

    /* Test tear down. */
    test_tear_down(test_suite_number, ret);
    if (ret->val == TEST_FAILED) {
        /* Test result has been set within test_tear_down() if it fails. */
        return;
    }

    /* End of the test. The candidate is back to PSA_FWU_READY state now. */
    ret->val = TEST_PASSED;
}

/* Start a FWU process with a nonexistent component.
 * Write image data with a nonexistent component.
 * Cancel a FWU process with a nonexistent component.
 * Finish a FWU process with a nonexistent component.
 * Clean a FWU process with a nonexistent component.
 * Query a nonexistent component.
 */
void tfm_fwu_test_common_003(struct test_result_t *ret)
{
    psa_status_t status;
    uint8_t test_suite_number = 3;
    psa_fwu_component_info_t info;

    if (test_setup(test_suite_number, ret) != 0) {
        /* Test result(ret) is set within test_setup(). */
        return;
    }

    /* Trigger a FWU process with nonexistent component. */
    status = psa_fwu_start(FWU_COMPONENT_NUMBER, NULL, 0);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("psa_fwu_start with nonexistent component should fail.");
        return;
    }

    /* Trigger a FWU process. */
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start with nonexistent component should fail.");
        return;
    }

    /* Write image data with a nonexistent component. */
    status = psa_fwu_write(FWU_COMPONENT_NUMBER,
                           (size_t)0x0,
                           header_test_image_version_zero,
                           sizeof(header_test_image_version_zero));
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Write with nonexistent component should fail.");
        return;
    }

    /* Cancel a FWU process with a nonexistent component. */
    status = psa_fwu_cancel(FWU_COMPONENT_NUMBER);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Cancel with nonexistent component should fail.");
        return;
    }

    /* Write image data preparing for finish. */
    status = psa_fwu_write(test_component,
                           (size_t)0x0,
                           header_test_image_version_zero,
                           sizeof(header_test_image_version_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail.");
        return;
    }

    /* Finish a FWU process with a nonexistent component. */
    status = psa_fwu_finish(FWU_COMPONENT_NUMBER);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Finish with nonexistent component should fail.");
        return;
    }

    /* Cancel the FWU process preparing for clean. */
    status = psa_fwu_cancel(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_cancel should not fail.");
        return;
    }

    /* Clean a FWU process with a nonexistent component. */
    status = psa_fwu_clean(FWU_COMPONENT_NUMBER);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Clean with nonexistent component should fail.");
        return;
    }

    /* Clean the component to make the component back to READY state. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_clean should succeed");
        return;
    }

    /* Query a FWU process with a nonexistent component. */
    status = psa_fwu_query(FWU_COMPONENT_NUMBER, &info);
    if (status != PSA_ERROR_DOES_NOT_EXIST) {
        TEST_FAIL("Query should succeed");
        return;
    }

    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_READY) {
        TEST_LOG("The component is expected to be in PSA_FWU_READY after psa_fwu_clean(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
    }

    /* End of the test. The candidate is already back to PSA_FWU_READY state now.
     * No need to call test_tear_down().
     */
    ret->val = TEST_PASSED;
}

/* Test start, cancel, write, finish, install, accept, clean and reject when the
 * component is in bad state:
 * Start after write, after finish and after install;
 * Cancel before start, after start and after install;
 * Write before start, after install, finish after install, install after install;
 * Finish before start, after finish, after install;
 * Install before start, before write, before finish;
 * Accept before start, after start, after write, after finish and after install;
 * Clean before start, after start, after write, after finish and after install;
 * Reject before start, after start, after write, after finish.
 */
void tfm_fwu_test_common_004(struct test_result_t *ret)
{
    psa_status_t status;
    uint8_t test_suite_number = 4;

    if (test_setup(test_suite_number, ret) != 0) {
        /* Test result(ret) is set within test_setup(). */
        return;
    }

    /* Write before start. */
    status = psa_fwu_write(test_component,
                           (size_t)0x0,
                           header_test_image_version_zero,
                           sizeof(header_test_image_version_zero));
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_write should fail before start.");
        return;
    }

    /* Finish before start. */
    status = psa_fwu_finish(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_finish should fail before start.");
        return;
    }

    /* Cancel before start. */
    status = psa_fwu_cancel(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_cancel should fail.");
        return;
    }

    /* Install before start. */
    status = psa_fwu_install();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_install should fail before start.");
        return;
    }

#ifdef FWU_SUPPORT_TRIAL_STATE
    /* Accept before start. The component is in READY state. */
    status = psa_fwu_accept();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_accept should fail before start.");
        return;
    }
#endif

    /* Clean before start. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_clean should fail before start.");
        return;
    }

    /* Reject before start. */
    status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_reject should fail before start.");
        return;
    }

    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start should not fail.");
        return;
    }

    /* Install after start. */
    status = psa_fwu_install();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_install should fail after start.");
        return;
    }

#ifdef FWU_SUPPORT_TRIAL_STATE
    /* Accept after start. */
    status = psa_fwu_accept();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_accept should fail after start.");
        return;
    }
#endif
    /* Clean after start. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_clean should fail after start.");
        return;
    }

    /* Reject after start. */
    status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_reject should fail after start.");
        return;
    }

    status = psa_fwu_write(test_component,
                           (size_t)0x0,
                           header_test_image_version_zero,
                           sizeof(header_test_image_version_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail.");
        return;
    }

    /* Start after write. */
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_start should fail after psa_fwu_write().");
        return;
    }

    /* Install after write. */
    status = psa_fwu_install();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_install should fail after write.");
        return;
    }

#ifdef FWU_SUPPORT_TRIAL_STATE
    /* Accept after write. */
    status = psa_fwu_accept();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_accept should fail after write.");
        return;
    }
#endif
    /* Clean after write. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_clean should fail after write.");
        return;
    }

    /* Reject after write. */
    status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_reject should fail after write.");
        return;
    }

    /* Write the protected TLV + IMAGE_TLV_INFO_MAGIC + part of the SHA256.
     * The image dependency information is downloaded into the target device
     * by this psa_fwu_write operation.
     */
    status = psa_fwu_write(test_component,
                           (size_t)PROTECTED_TLV_OFF_SECURE,
                           tlv_dep_version_zero,
                           sizeof(tlv_dep_version_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the protected TLV data.");
        return;
    }

    status = psa_fwu_finish(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_finish should not fail.");
        return;
    }

    /* Start after finish. */
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_start should fail after psa_fwu_finish().");
        return;
    }

    /* Finish after finish. */
    status = psa_fwu_finish(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_finish should fail after finish.");
        return;
    }

#ifdef FWU_SUPPORT_TRIAL_STATE
    /* Accept after finish. */
    status = psa_fwu_accept();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_accept should fail after finish.");
        return;
    }
#endif
    /* Clean after finish. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_clean should fail after finish.");
        return;
    }

    /* Reject after finish. */
    status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_reject should fail after finish.");
        return;
    }

    status = psa_fwu_install();
    if (status != PSA_SUCCESS_REBOOT) {
        TEST_FAIL("psa_fwu_install should not fail.");
        return;
    }

    /* Cancel after install. */
    status = psa_fwu_cancel(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_cancel should fail after psa_fwu_install().");
        return;
    }

    /* Start after install. */
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_start should fail after psa_fwu_install().");
        return;
    }

    /* Write after install. */
    status = psa_fwu_write(test_component,
                           (size_t)0x0,
                           header_test_image_version_zero,
                           sizeof(header_test_image_version_zero));
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_write should fail after psa_fwu_install().");
        return;
    }

    /* Finish after install. */
    status = psa_fwu_finish(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_finish should fail after psa_fwu_install().");
        return;
    }

    /* Install after install. */
    status = psa_fwu_install();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_install should fail after psa_fwu_install().");
        return;
    }

#ifdef FWU_SUPPORT_TRIAL_STATE
    /* Accept after install(before reboot). */
    status = psa_fwu_accept();
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_accept should fail after install.");
        return;
    }
#endif
    /* Clean after install(before reboot). */
    status = psa_fwu_clean(test_component);
    if (status != PSA_ERROR_BAD_STATE) {
        TEST_FAIL("psa_fwu_clean should fail after install.");
        return;
    }

    /* Reject after install. */
    status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_reject failed after install.");
        return;
    }

    /* Clean after install(before reboot). */
    status = psa_fwu_clean(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_clean should not fail after reject.");
        return;
    }

    /* End of the test. The candidate is back to PSA_FWU_READY state now. */
    ret->val = TEST_PASSED;
}

#if (MCUBOOT_IMAGE_NUMBER > 1)
void tfm_fwu_test_common_005(struct test_result_t *ret)
{
    psa_status_t status;
    uint8_t test_suite_number = 5;
    const uint8_t header_test_image_version_not_zero[28] = \
                        { 0x3d, 0xb8, 0xf3, 0x96, /* IMAGE_MAGIC */ \
                          0x00, 0x00, 0x00, 0x00,  /* LoadAddr */  \
                          /* Image Header size */  \
                          IMAGE_HEADER_SIZE_LOW, IMAGE_HEADER_SIZE_HIGH,  \
                          0x7B, 0x00, /* Protected TLV size */  \
                          /* Image size */
                          SECURE_IMAGE_SIZE_BYTE_0,  \
                          SECURE_IMAGE_SIZE_BYTE_1,  \
                          SECURE_IMAGE_SIZE_BYTE_2,  \
                          SECURE_IMAGE_SIZE_BYTE_3,  \
                          0x00, 0x00, 0x00, 0x00, /* Flags */  \
                          IMAGE_VERSION_FOR_TEST_MAJOR, /* Version: Major */  \
                          IMAGE_VERSION_FOR_TEST_MINOR, /* Version: Minor */  \
                          /* Version: Low byte of revision */
                          IMAGE_VERSION_FOR_TEST_REVISION_LOW, \
                          /* Version: High byte of revision */
                          IMAGE_VERSION_FOR_TEST_REVISION_HIGH,  \
                          0x00, 0x00, 0x00, 0x00, /* Version: Build number */  \
                        };

    /* The protected TLV data + part of the TLV data of the secure image. */
    const uint8_t tlv_data_secure_image[128] = \
                        { 0x08, 0x69, /* IMAGE_TLV_PROT_INFO_MAGIC */  \
                          0x7B, 0x00, /* The size of protected TLV area */
                          /* Secure count: */
                          0x50, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x01,  \
                          /* Boot record: */
                          0x60, 0x00, 0x5b, 0x00, 0xa5, 0x01, 0x63, 0x53,  \
                          0x50, 0x45, 0x04, 0x65, 0x31, 0x2e, 0x33, 0x2e,  \
                          0x30, 0x05, 0x58, 0x20, 0xfc, 0x57, 0x01, 0xdc,  \
                          0x61, 0x35, 0xe1, 0x32, 0x38, 0x47, 0xbd, 0xc4,  \
                          0x0f, 0x04, 0xd2, 0xe5, 0xbe, 0xe5, 0x83, 0x3b,  \
                          0x23, 0xc2, 0x9f, 0x93, 0x59, 0x3d, 0x00, 0x01,  \
                          0x8c, 0xfa, 0x99, 0x94, 0x06, 0x66, 0x53, 0x48,  \
                          0x41, 0x32, 0x35, 0x36, 0x02, 0x58, 0x20, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          /* DEPENDENCY: nonsecure, MIN_VER:0.0.0+0 */
                          0x40, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x00, 0x00,  \
                          0x00, /* Version: Major */  \
                          0x00, /* Version: Minor */  \
                          /* Version: Low byte of revision */
                          0x00, \
                          /* Version: High byte of revision */
                          0x00,  \
                          0x00, 0x00, 0x00, 0x00, /* Version: Build number */  \
                          /* custom_tlvs: None */
                          0x07, 0x69, /* IMAGE_TLV_INFO_MAGIC */  \
                          0x50, 0x01, 0x10 /* Part of the SHA256 */  \
                        };

    if (test_setup(test_suite_number, ret) != 0) {
        /* Test result(ret) is set within test_setup(). */
        return;
    }
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start should not fail.");
        return;
    }

    /* The image header(from header magic to image version) is downloaded
     * into the target device by this psa_fwu_write operation.
     */
    status = psa_fwu_write(test_component,
                           0,
                           header_test_image_version_not_zero,
                           sizeof(header_test_image_version_not_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the image header data.");
        return;
    }

    /* Write the protected TLV + IMAGE_TLV_INFO_MAGIC + part of the SHA256.
     * The image dependency information is downloaded into the target device
     * by this psa_fwu_write operation.
     */
    /* The required min_version of dependency is 0.0.0. */
    status = psa_fwu_write(test_component,
                           (size_t)PROTECTED_TLV_OFF_SECURE,
                           tlv_data_secure_image,
                           sizeof(tlv_data_secure_image));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the protected TLV data.");
        return;
    }

    status = psa_fwu_finish(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_finish should not fail.");
        return;
    }
    status = psa_fwu_install();

    /* Dependency check in multiple images:
     * dependency image ID: nonsecure image.
     * Currently running nonsecure image version should always equal or greater
     * than the required 0.0.0.
     * So the dependency check should pass.
     * PSA_SUCCESS_REBOOT should be returned.
     */
    if (status != PSA_SUCCESS_REBOOT) {
        TEST_FAIL("Install PSA_SUCCESS_REBOOT should be returned when the image dependency check pass.");
        return;
    }

    /* Reject after install. */
    status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_reject failed after install.");
        return;
    }

    /* Clean after reject. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_clean should not fail after reject.");
        return;
    }

    /* End of the test. The candidate is back to PSA_FWU_READY state now. */
    ret->val = TEST_PASSED;
}

#if (FWU_CONCURRENTLY_UPDATE_COMPONENTS > 1)
void tfm_fwu_test_common_006(struct test_result_t *ret)
{
    psa_status_t status;
    psa_fwu_image_version_t dep_version_in_tlv = {0, 0, 0, 0};
    uint8_t test_suite_number = 6;
    psa_fwu_component_info_t info;
    const uint8_t header_test_image_version_not_zero[28] = \
                        { 0x3d, 0xb8, 0xf3, 0x96, /* IMAGE_MAGIC */ \
                          0x00, 0x00, 0x00, 0x00,  /* LoadAddr */  \
                          /* Image Header size */  \
                          IMAGE_HEADER_SIZE_LOW, IMAGE_HEADER_SIZE_HIGH,  \
                          0x7B, 0x00, /* Protected TLV size */  \
                          /* Image size */
                          SECURE_IMAGE_SIZE_BYTE_0,  \
                          SECURE_IMAGE_SIZE_BYTE_1,  \
                          SECURE_IMAGE_SIZE_BYTE_2,  \
                          SECURE_IMAGE_SIZE_BYTE_3,  \
                          0x00, 0x00, 0x00, 0x00, /* Flags */  \
                          IMAGE_VERSION_FOR_TEST_MAJOR, /* Version: Major */  \
                          IMAGE_VERSION_FOR_TEST_MINOR, /* Version: Minor */  \
                          /* Version: Low byte of revision */
                          IMAGE_VERSION_FOR_TEST_REVISION_LOW, \
                          /* Version: High byte of revision */
                          IMAGE_VERSION_FOR_TEST_REVISION_HIGH,  \
                          0x00, 0x00, 0x00, 0x00, /* Version: Build number */  \
                        };

    /* The nonsecure image header(from image header magic to image version). */
    uint8_t header_dep_image_version_not_zero[28] = \
                        { 0x3d, 0xb8, 0xf3, 0x96, /* IMAGE_MAGIC */ \
                          0x00, 0x00, 0x00, 0x00,  /* LoadAddr */  \
                          /* Image Header size */  \
                          IMAGE_HEADER_SIZE_LOW, IMAGE_HEADER_SIZE_HIGH,  \
                          0x7C, 0x00, /* Protected TLV size */  \
                          /* Image size */
                          NONSECURE_IMAGE_SIZE_BYTE_0,  \
                          NONSECURE_IMAGE_SIZE_BYTE_1,  \
                          NONSECURE_IMAGE_SIZE_BYTE_2,  \
                          NONSECURE_IMAGE_SIZE_BYTE_3,  \
                          0x00, 0x00, 0x00, 0x00, /* Flags */  \
                          0x00, /* Version: Major */  \
                          0x00, /* Version: Minor */  \
                          /* Version: Low byte of revision */
                          0x00, \
                          /* Version: High byte of revision */
                          0x00,  \
                          0x00, 0x00, 0x00, 0x00, /* Version: Build number */  \
                        };

    /* The protected TLV data + part of the TLV data of the secure image. */
    uint8_t tlv_data_nonsecure_image[128] = \
                        { 0x08, 0x69,  /* IMAGE_TLV_PROT_INFO_MAGIC */  \
                          0x7c, 0x00,  /* The size of protected TLV area */  \
                          /* Secure count: */
                          0x50, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          /* Boot record: */
                          0x60, 0x00, 0x5c, 0x00, 0xa5, 0x01, 0x64, 0x4e,  \
                          0x53, 0x50, 0x45, 0x04, 0x65, 0x30, 0x2e, 0x30,  \
                          0x2e, 0x30, 0x05, 0x58, 0x20, 0xe1, 0x80, 0x15,  \
                          0x99, 0x3d, 0x6d, 0x27, 0x60, 0xb4, 0x99, 0x27,  \
                          0x4b, 0xae, 0xf2, 0x64, 0xb8, 0x3a, 0xf2, 0x29,  \
                          0xe9, 0xa7, 0x85, 0xf3, 0xd5, 0xbf, 0x00, 0xb9,  \
                          0xd3, 0x2c, 0x1f, 0x03, 0x96, 0x06, 0x66, 0x53,  \
                          0x48, 0x41, 0x32, 0x35, 0x36, 0x02, 0x58, 0x20,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          /* DEPENDENCY: secure, MIN_VER: */
                          0x40, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          IMAGE_VERSION_FOR_TEST_MAJOR, /* Version: Major */  \
                          IMAGE_VERSION_FOR_TEST_MINOR, /* Version: Minor */  \
                          /* Version: Low byte of revision */
                          IMAGE_VERSION_FOR_TEST_REVISION_LOW, \
                          /* Version: High byte of revision */
                          IMAGE_VERSION_FOR_TEST_REVISION_HIGH,  \
                          0x00, 0x00, 0x00, 0x00, /* Version: Build number */  \
                          /* custom_tlvs: None */
                          0x07, 0x69, /* IMAGE_TLV_INFO_MAGIC */  \
                          0x50, 0x01  /* Part of the SHA256 */  \
                        };

    if (test_setup(test_suite_number, ret) != 0) {
        /* Test result(ret) is set within test_setup(). */
        return;
    }
    status = psa_fwu_start(test_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start should not fail.");
        return;
    }

    /* The image header(from header magic to image version) is downloaded
     * into the target device by this psa_fwu_write operation.
     */
    status = psa_fwu_write(test_component,
                           0,
                           header_test_image_version_not_zero,
                           sizeof(header_test_image_version_not_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the image header data.");
        return;
    }

    /* Write the protected TLV + IMAGE_TLV_INFO_MAGIC + part of the SHA256.
     * The image dependency information is downloaded into the target device
     * by this psa_fwu_write operation.
     */
    /* The protected TLV data + part of the TLV data of the secure image.
     */
    uint8_t tlv_data_secure_image[128] = \
                        { 0x08, 0x69, /* IMAGE_TLV_PROT_INFO_MAGIC */  \
                          0x7B, 0x00, /* The size of protected TLV area */
                          /* Secure count: */
                          0x50, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x01,  \
                          /* Boot record: */
                          0x60, 0x00, 0x5b, 0x00, 0xa5, 0x01, 0x63, 0x53,  \
                          0x50, 0x45, 0x04, 0x65, 0x31, 0x2e, 0x33, 0x2e,  \
                          0x30, 0x05, 0x58, 0x20, 0xfc, 0x57, 0x01, 0xdc,  \
                          0x61, 0x35, 0xe1, 0x32, 0x38, 0x47, 0xbd, 0xc4,  \
                          0x0f, 0x04, 0xd2, 0xe5, 0xbe, 0xe5, 0x83, 0x3b,  \
                          0x23, 0xc2, 0x9f, 0x93, 0x59, 0x3d, 0x00, 0x01,  \
                          0x8c, 0xfa, 0x99, 0x94, 0x06, 0x66, 0x53, 0x48,  \
                          0x41, 0x32, 0x35, 0x36, 0x02, 0x58, 0x20, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  \
                          /* DEPENDENCY: nonsecure, MIN_VER:0.0.0+0 */
                          0x40, 0x00, 0x0c, 0x00, 0x01, 0x00, 0x00, 0x00,  \
                          0x00, /* Version: Major */  \
                          0x00, /* Version: Minor */  \
                          /* Version: Low byte of revision */
                          0x00, \
                          /* Version: High byte of revision */
                          0x00,  \
                          0x00, 0x00, 0x00, 0x00, /* Version: Build number */  \
                          /* custom_tlvs: None */
                          0x07, 0x69, /* IMAGE_TLV_INFO_MAGIC */  \
                          0x50, 0x01, 0x10 /* Part of the SHA256 */  \
                        };

    /* Query the active dependency image. */
    if (psa_fwu_query(dependency_component, &info) != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* The required image version is greater than the running. */
    dep_version_in_tlv.major = info.version.major + 1;

    /* Update the version bytes of the TLV data. */
    memcpy(tlv_data_secure_image + TLV_DEPENDENCY_VERSION_OFF_SECURE,
           &dep_version_in_tlv,
           sizeof(dep_version_in_tlv));
    status = psa_fwu_write(test_component,
                           (size_t)PROTECTED_TLV_OFF_SECURE,
                           tlv_data_secure_image,
                           sizeof(tlv_data_secure_image));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the protected TLV data.");
        return;
    }
    status = psa_fwu_finish(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_finish should not fail.");
        return;
    }

    status = psa_fwu_install();
    /* Dependency check in multiple images:
     * dependency image ID: nonsecure image.
     * Currently running nonsecure image version: info.version <
     * dep_version_in_tlv.
     * So the dependency check should fail.
     * PSA_ERROR_DEPENDENCY_NEEDED should be returned.
     */
    if (status != PSA_ERROR_DEPENDENCY_NEEDED) {
        TEST_FAIL("Install PSA_ERROR_DEPENDENCY_NEEDED should be returned when the image dependency check fails.");
        return;
    }

    /* Query the CANDIDATE image. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the component state: the component states in CANDIDATE state. */
    if (info.state != PSA_FWU_CANDIDATE) {
        TEST_FAIL("Image should be in CANDIDATE state after install returns PSA_ERROR_DEPENDENCY_NEEDED.");
        return;
    }

    /* ******************************************************************
     * Start the Firmware Update of the dependency image(nonsecure image)
     * ******************************************************************
     */
    /* Update the image version to meet the required min_version. */
    memcpy(header_dep_image_version_not_zero + HEADER_IMAGE_VERSION_OFF,
           &dep_version_in_tlv,
           sizeof(dep_version_in_tlv));

    status = psa_fwu_start(dependency_component, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_start should not fail.");
        return;
    }

    /* The image header(from header magic to image version) is downloaded
     * into the target device by this psa_fwu_write operation.
     */
    status = psa_fwu_write(dependency_component,
                           0,
                           header_dep_image_version_not_zero,
                           sizeof(header_dep_image_version_not_zero));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the image header data.");
        return;
    }

    /* Install again after writing the image header of the dependency image. At
     * this point, the dependency image has not been installed. When checking
     * the dependency, the dependency image in the stage slot cannot to used. So
     * PSA_ERROR_DEPENDENCY_NEEDED should still be returned by the install.
     */
    status = psa_fwu_install();
    if (status != PSA_ERROR_DEPENDENCY_NEEDED) {
        TEST_FAIL("Install should fail when the boot magic of the dependency image is not good.");
        return;
    }

    /* Write the protected TLV + IMAGE_TLV_INFO_MAGIC + part of the SHA256.
     * The image dependency information is downloaded into the target device
     * by this psa_fwu_write operation.
     */
    /* The required image version(0,0,0) is less than or equal to the running.
     */
    status = psa_fwu_write(dependency_component,
                           (size_t)PROTECTED_TLV_OFF_NONSECURE,
                           tlv_data_nonsecure_image,
                           sizeof(tlv_data_nonsecure_image));
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_write should not fail when writing the protected TLV data.");
        return;
    }
    status = psa_fwu_finish(dependency_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_finish should not fail.");
        return;
    }

    /* Install the dependency image(nonsecure image).
     * Dependency check:
     * dependency image ID: secure image which has been once installed.
     * dependency image version:IMAGE_VERSION_FOR_TEST_MAJOR,
     *                          IMAGE_VERSION_FOR_TEST_MINOR,
     *                          IMAGE_VERSION_FOR_TEST_REVISION_HIGH << 8 |
     *                          IMAGE_VERSION_FOR_TEST_REVISION_LOW,
     * which equals to the image version of the secure image in the secondary
     * slot. So no matter what's the version of the running secure image. The
     * dependency check should pass.
     * In the current implementation, image verification is deferred to
     * reboot, so PSA_SUCCESS_REBOOT is returned when success.
     */
    status = psa_fwu_install();
    if (status != PSA_SUCCESS_REBOOT) {
        TEST_FAIL("Install should fail after the dependency image successfully being installed.");
        return;
    }

    /* Query the test component. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_STAGED) {
        TEST_LOG("The component is expected to be in PSA_FWU_STAGED after \
                  psa_fwu_install(), actual state is %d",
                 info.state);
        TEST_FAIL("Bad state is returned.");
        return;
    }

    /* Query the dependency component. */
    status = psa_fwu_query(dependency_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_STAGED) {
        TEST_LOG("The dependency component is expected to be in PSA_FWU_STAGED \
                  after psa_fwu_install(), actual state is %d",
                 info.state);
        TEST_FAIL("Bad state is returned.");
        return;
    }

    status = psa_fwu_reject(PSA_ERROR_NOT_PERMITTED);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_reject should succeed");
        return;
    }

    /* Query the test component. */
    status = psa_fwu_query(test_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_FAILED) {
        TEST_LOG("The component is expected to be in PSA_FWU_FAILED after \
                  psa_fwu_install(), actual state is %d",
                 info.state);
        TEST_FAIL("Bad state is returned.");
        return;
    }

    status = psa_fwu_query(dependency_component, &info);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("Query should succeed");
        return;
    }

    /* Check the image state. */
    if (info.state != PSA_FWU_FAILED) {
        TEST_LOG("The component is expected to be in PSA_FWU_FAILED after psa_fwu_cancel(), actual state is %d",
                  info.state);
        TEST_FAIL("Wrong component state is returned.");
    }


    /* Clean the component to make the component back to READY state. */
    status = psa_fwu_clean(test_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_clean should succeed");
        return;
    }
    status = psa_fwu_clean(dependency_component);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("psa_fwu_clean should succeed");
        return;
    }

    /* End of the test. The candidate is back to PSA_FWU_READY state now. */
    ret->val = TEST_PASSED;
}
#endif /* FWU_CONCURRENTLY_UPDATE_COMPONENTS > 1 */
#endif /* MCUBOOT_IMAGE_NUMBER > 1 */

#ifdef TFM_FWU_TEST_REQUEST_REBOOT
void tfm_fwu_test_common_007(struct test_result_t *ret)
{
    /* Request reboot. */
    psa_fwu_request_reboot();
    TEST_FAIL("Request reboot should not fail");

    ret->val = TEST_FAILED;
}
#endif
