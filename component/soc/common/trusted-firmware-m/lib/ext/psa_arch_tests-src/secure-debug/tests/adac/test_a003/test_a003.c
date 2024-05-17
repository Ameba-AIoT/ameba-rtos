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

#include "test_a003.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 3)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_discovery_tlv_response\n"
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
    psa_tlv_t *tlv = NULL, *current_tlv = NULL, **tlv_seq = NULL;
    uint16_t *type_id_list;
    size_t type_id_size, i, j;

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

    // Variable to collect the reference to each TLV in the response sequence.
    tlv_seq = (psa_tlv_t **)malloc((response->data_count) * sizeof(psa_tlv_t *));
    if (tlv_seq == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_INSUFFICIENT_SIZE);
        goto test_end;
    }

    // Variable to collect the type ID from each TLV entry.
    type_id_list = (uint16_t *)malloc((response->data_count) * sizeof(uint16_t));
    if (type_id_list == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(4), VAL_STATUS_INSUFFICIENT_SIZE);
        goto test_end;
    }

    // Parse the TLV sequence.
    for (i = 0, j = 0; (i + 4) < (response->data_count * 4); j++) {
        tlv = (psa_tlv_t *) (((uint8_t *)response->data) + i);
        tlv_seq[j] = tlv;
        i += sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
    }

    j = 0;
    while (tlv_seq[j] != tlv)
    {
        current_tlv = tlv_seq[j];
        type_id_list[j] = current_tlv->type_id;

        if (type_id_list[j] == PSA_LIFECYCLE)
            status = check_lifecycle_major_state(current_tlv->value, current_tlv->length_in_bytes);
        else if (type_id_list[j] == TOKEN_FORMATS)
            status = check_adac_token_support(current_tlv->value, current_tlv->length_in_bytes);
        else if (type_id_list[j] == CERT_FORMATS)
            status = check_adac_cert_support(current_tlv->value, current_tlv->length_in_bytes);
        else
            status = VAL_STATUS_SUCCESS;

        j++;
    }

    if (status != VAL_STATUS_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(5), status);
        goto test_end;
    }

    response_packet_release(response);

test_end:
    if (tlv_seq != NULL)
        free(tlv_seq);

    if (type_id_list != NULL)
        free(type_id_list);

    val->test_exit();
}

static val_status_t check_lifecycle_major_state(uint8_t *value, size_t size)
{
    uint16_t state;

    // Device cannot have multiple lifecycle states.
    if (size != 0x2)
        return VAL_STATUS_INVALID_SIZE;

    state = (*(uint16_t *)value) & PSA_LIFECYCLE_MAJOR_STATE;

    switch (state) {
    case UNKNOWN:
    case ASSEMBLY_AND_TEST:
    case PSA_ROT_AND_PROVISIONING:
    case SECURED:
    case NON_PSA_ROT_DEBUG:
    case RECOVERABLE_PSA_ROT_DEBUG:
    case DECOMMISSIONED:
        return VAL_STATUS_SUCCESS;
    default:
        return VAL_STATUS_DATA_MISMATCH;
        break;
    }
}

static val_status_t check_adac_token_support(uint8_t *value, size_t size)
{
    uint16_t payload;
    uint8_t payload_bytes = (uint8_t)size;
    uint8_t i;

    for (i = 0; i < payload_bytes; i += 2)
    {
        payload = (*(uint16_t *)(value + i));
        if (payload == TOKEN_ADAC)
            return VAL_STATUS_SUCCESS;
    }

    return VAL_STATUS_DATA_MISMATCH;
}

static val_status_t check_adac_cert_support(uint8_t *value, size_t size)
{
    uint16_t payload;
    uint8_t payload_bytes = (uint8_t)size;
    uint8_t i;

    for (i = 0; i < payload_bytes; i += 2)
    {
        payload = (*(uint16_t *)(value + i));
        if (payload == CERT_ADAC)
            return VAL_STATUS_SUCCESS;
    }

    return VAL_STATUS_DATA_MISMATCH;
}
