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

#include "test_a011.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 11)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_tlv_reserved\n"
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

    request_packet_t *request;
    response_packet_t *response;
    psa_tlv_t *exts[MAX_EXTENSIONS], *tlv = NULL;
    uint8_t *chain = NULL;
    size_t chain_size = 0, exts_count = 0, i, err_count;

    // Send discovery packet with no requested type ID.
    request = request_packet_build(SDP_DISCOVERY_CMD, NULL, 0);
    if (request == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(1), VAL_STATUS_CONNECTION_FAILED);
        goto test_end;
    }
    request_packet_send(request);
    request_packet_release(request);

    response = psa_adac_await_response();
    if (response == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(2), VAL_STATUS_CONNECTION_FAILED);
        goto test_end;
    }

    // Parse the TLV sequence in the discovery response.
    err_count = 0;
    for (i = 0; (i + 4) < (response->data_count * 4);) {
        tlv = (psa_tlv_t *) (((uint8_t *)response->data) + i);
        err_count += (tlv->_reserved != 0x0);
        i += sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
    }
    response_packet_release(response);

    if (err_count != 0x0) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_INVALID);
        goto test_end;
    }

    ret = psa_adac_load_certificate_chain(chain_file, &chain, &chain_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(4), VAL_STATUS_LOAD_ERROR);
        goto test_end;
    }

    ret = psa_adac_read_extensions((uint32_t *)chain, chain_size, exts, &exts_count);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(5), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    err_count = 0;
    for (i = 0; i < exts_count; i++) {
        tlv = exts[i];
        err_count += (tlv->_reserved != 0x0);
    }

    if (err_count != 0x0) {
        val->err_check_set(TEST_CHECKPOINT_NUM(6), VAL_STATUS_INVALID);
        goto test_end;
    }

test_end:
    val->test_exit();
}
