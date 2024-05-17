/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include <psa_adac.h>
#include <adac_util.h>
#include "val_interfaces.h"

#include "test_a009.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 9)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_scope_limit\n"
TEST_PUBLISH(TEST_NUM, test_entry);

void test_entry(val_api_t *val_api)
{
    psa_status_t ret;
    val_api_t *val = val_api;

    /* test init */
    val->test_init(TEST_NUM, TEST_DESC);
    if (!IS_TEST_START(val->get_status()))
    {
        goto test_end;
    }
    psa_adac_host_init();

    uint8_t *chain = NULL;
    size_t chain_size = 0;
    request_packet_t *request;
    response_packet_t *response;

    ret = psa_adac_load_certificate_chain(chain_file, &chain, &chain_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(1), VAL_STATUS_LOAD_ERROR);
        goto test_end;
    }

    psa_tlv_t *exts[MAX_EXTENSIONS];
    size_t exts_count = 0;
    uint8_t key_type;

    ret = psa_adac_read_extensions((uint32_t *)chain, chain_size, exts, &exts_count);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(2), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    key_type = detect_cryptosystem(exts, exts_count);

    ret = psa_adac_issue_command(SDP_DISCOVERY_CMD, request, NULL, 0);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    ret = psa_adac_parse_response(SDP_DISCOVERY_CMD, response);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(4), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    ret = psa_adac_check_cryptosystem_support(response, key_type);
    if (ret == PSA_ERROR_NOT_SUPPORTED) {
        val->set_status(RESULT_SKIP(key_type));
        goto test_end;
    }

    response_packet_release(response);

    ret = psa_adac_issue_command(SDP_AUTH_START_CMD, request, NULL, 0);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(5), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    ret = psa_adac_parse_response(SDP_AUTH_START_CMD, response);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(6), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    response_packet_release(response);

    uint8_t *payload = NULL;
    size_t payload_size, i;
    psa_tlv_t *current_extn;
    certificate_header_t *header = NULL;
    uint8_t *leaf_cert = NULL;

    // Modify scope limit constraints at host-side for soc_class
    for (i = 0; i < exts_count; i++) {
        current_extn = exts[i];
        if (current_extn->type_id == CERT_ADAC) {
            payload = (uint8_t *)current_extn;
            payload_size = current_extn->length_in_bytes + sizeof(psa_tlv_t);

            header = (certificate_header_t *) (current_extn->value);
            if (header->role != SDP_CRT_ROLE_ROOT) {
                leaf_cert = current_extn->value;
                modify_soc_class_neutral(header, i);
            }

            ret = psa_adac_issue_command(SDP_AUTH_RESPONSE_CMD, request, payload, payload_size);
            if (ret != PSA_SUCCESS) {
                val->err_check_set(TEST_CHECKPOINT_NUM(7), VAL_STATUS_WRITE_FAILED);
                goto test_end;
            }

            // Restore the tampered certificate value
            if (header->role == SDP_CRT_ROLE_ROOT)
                modify_soc_class_neutral(header, i);

            response = psa_adac_await_response();
            ret = psa_adac_parse_response(SDP_AUTH_RESPONSE_CMD, response);
            if (ret != PSA_SUCCESS) {
                val->err_check_set(TEST_CHECKPOINT_NUM(8), VAL_STATUS_READ_FAILED);
                goto test_end;
            }

            if (response->status == SDP_NEED_MORE_DATA) {
                response_packet_release(response);
            } else if (response->status == SDP_FAILURE) {
                val->print(PRINT_INFO, "Inconsistent certificate chain rejected by target\n", 0);
                break;
            } else {
                val->err_check_set(TEST_CHECKPOINT_NUM(9), VAL_STATUS_INVALID);
                break;
            }
        }
    }

    if (leaf_cert == NULL) {
	    val->print(PRINT_ERROR, "Leaf certificate not found\n", 0);
        val->err_check_set(TEST_CHECKPOINT_NUM(10), VAL_STATUS_NOT_FOUND);
        goto test_end;
    }

    if (response->status != SDP_FAILURE)
        val->err_check_set(TEST_CHECKPOINT_NUM(11), VAL_STATUS_ERROR);

    response_packet_release(response);

test_end:
    val->test_exit();
}

static void modify_soc_class_neutral(certificate_header_t *h, size_t chain_count)
{
    // Change the values to create non-neutral fields for soc_class for non-root certificates.
    h->soc_class = h->soc_class ^ (uint32_t)(~chain_count);
}
