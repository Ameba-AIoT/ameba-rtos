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

#include "test_a010.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 10)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_sw_partition\n"
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

    psa_tlv_t *exts[MAX_EXTENSIONS], *tk_exts[MAX_EXTENSIONS];
    size_t exts_count = 0, tk_exts_count = 0;
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

    psa_key_handle_t handle;
    uint8_t *key = NULL;
    size_t key_size = 0;

    ret = psa_adac_get_private_key(key_file, &key_type, &handle, &key, &key_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(5), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    ret = psa_adac_issue_command(SDP_AUTH_START_CMD, request, NULL, 0);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(6), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    ret = psa_adac_parse_response(SDP_AUTH_START_CMD, response);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(7), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    psa_auth_challenge_t *challenge = (psa_auth_challenge_t *) response->data;

    uint8_t *token = NULL;
    size_t token_size = 0;

    set_sw_partitions((uint8_t *)tk_exts, &tk_exts_count, 2);

    ret = psa_adac_construct_token(challenge->challenge_vector, sizeof(challenge->challenge_vector),
                                   key_type, (uint8_t *)tk_exts, tk_exts_count, &token, &token_size,
                                   NULL, handle, key, key_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(8), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }
    response_packet_release(response);

    ret = psa_adac_send_certificate(exts, exts_count);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(9), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

	val->print(PRINT_INFO, "Sending token\n", 0);
    ret = psa_adac_issue_command(SDP_AUTH_RESPONSE_CMD, request, (uint8_t *)token, token_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(10), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

	val->print(PRINT_INFO, "Receiving token_authentication response\n", 0);
    response = psa_adac_await_response();
    ret = psa_adac_parse_response(SDP_AUTH_RESPONSE_CMD, response);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(11), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    if (response->status == SDP_SUCCESS)
	    val->print(PRINT_INFO, "Target unlocked successfully\n", 0);
    else
        val->err_check_set(TEST_CHECKPOINT_NUM(12), VAL_STATUS_ERROR);

    response_packet_release(response);

test_end:
    val->test_exit();
}

static void set_sw_partitions(uint8_t *token_exts, size_t *token_exts_len, size_t num_tlv_entries)
{
    psa_tlv_t *tlv = NULL;
    size_t i = 0;
    uint8_t j, k, tlv_value_size;

    for (j = 0; j < num_tlv_entries; j++) {
        tlv = (psa_tlv_t *)(token_exts + i);
        tlv->type_id = SW_PARTITION_ID;
        tlv->length_in_bytes = 0x6;
        tlv_value_size = ROUND_TO_WORD(tlv->length_in_bytes);

        // Arbitrary scheme of software partitions IDs
        for (k = 0; k < tlv->length_in_bytes; k++)
            (tlv->value)[k] = j*4 + k;

        // Pad with 0x0 for a 32 bit aligned TLV
        while ((k % 4 != 0) && (k < tlv_value_size)) {
            (tlv->value)[k] = 0x0;
            k++;
        }

        i += sizeof(psa_tlv_t) + tlv_value_size;
    }
    *token_exts_len = i;
}
