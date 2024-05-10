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

#include "test_a004.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 4)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_discovery_tlv_request\n"
TEST_PUBLISH(TEST_NUM, test_entry);

void test_entry(val_api_t *val_api)
{
    psa_status_t ret;
    val_status_t status;
    val_api_t *val = NULL;
    val = val_api;

    /* test init */
    val->test_init(TEST_NUM, TEST_DESC);
    if (!IS_TEST_START(val->get_status()))
    {
        goto test_end;
    }
    psa_adac_host_init();

    request_packet_t *request;
    response_packet_t *response;
    psa_tlv_t *tlv = NULL;
    uint16_t *type_id_list, *type_id_list_rev;
    uint8_t *type_id_ptr;
    size_t type_id_size, i, j, tlv_entries;

    // Send discovery packet with no requested type ID.
    type_id_ptr = NULL;
    type_id_size = 0;
    ret = psa_adac_issue_command(SDP_DISCOVERY_CMD, request, type_id_ptr, type_id_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(1), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    if (response == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(2), VAL_STATUS_CONNECTION_FAILED);
        goto test_end;
    }

    // Variable to collect the type ID from each TLV entry.
    type_id_list = (uint16_t *)malloc((response->data_count) * sizeof(uint16_t));
    type_id_list_rev = (uint16_t *)malloc((response->data_count) * sizeof(uint16_t));
    if ((type_id_list == NULL) || (type_id_list_rev == NULL)) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_INSUFFICIENT_SIZE);
        goto test_end;
    }

    // Parse the TLV sequence.
    for (i = 0, j = 0; (i + 4) < (response->data_count * 4); j++) {
        tlv = (psa_tlv_t *) (((uint8_t *)response->data) + i);
        type_id_list[j] = tlv->type_id;
        i += sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
    }
    tlv_entries = j;
    response_packet_release(response);

    // Create a list of type ID in decreasing ID value.
    j = 0;
    while (j < tlv_entries) {
        type_id_list_rev[j] = type_id_list[tlv_entries-1-j];
        j++;
    }

    // Send discovery request packet with a subset of requested type ID.
    type_id_ptr = (uint8_t *)type_id_list;
    type_id_size = (tlv_entries/2) * sizeof(uint16_t);
    ret = psa_adac_issue_command(SDP_DISCOVERY_CMD, request, type_id_ptr, type_id_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(4), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    if (response == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(5), VAL_STATUS_CONNECTION_FAILED);
        goto test_end;
    }

    if (response->status == SDP_SUCCESS) {
        for (i = 0, j = 0; (i + 4) < (response->data_count * 4); j++) {
            tlv = (psa_tlv_t *) (((uint8_t *)response->data) + i);
            i += sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
        }

        // Response must contain at least as many type IDs as requested.
        if (j < tlv_entries/2) {
            val->err_check_set(TEST_CHECKPOINT_NUM(6), VAL_STATUS_ERROR);
            goto test_end;
        }
    } else {
        val->err_check_set(TEST_CHECKPOINT_NUM(7), VAL_STATUS_ERROR);
        goto test_end;
    }
    response_packet_release(response);

    // Send discovery packet with a reverse order of requested type ID.
    type_id_ptr = (uint8_t *)type_id_list_rev;
    type_id_size = tlv_entries * sizeof(uint16_t);
    ret = psa_adac_issue_command(SDP_DISCOVERY_CMD, request, type_id_ptr, type_id_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(8), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    if (response == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(9), VAL_STATUS_CONNECTION_FAILED);
        goto test_end;
    }

    if (response->status != SDP_SUCCESS)
        val->err_check_set(TEST_CHECKPOINT_NUM(10), VAL_STATUS_ERROR);

    response_packet_release(response);

test_end:
    if ((type_id_list != NULL) || (type_id_list_rev != NULL)) {
        free(type_id_list);
        free(type_id_list_rev);
    }
    val->test_exit();
}
