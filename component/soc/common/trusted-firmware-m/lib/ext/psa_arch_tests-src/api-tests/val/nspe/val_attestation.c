/** @file
 * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
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

#include "val_target.h"
#include "pal_interfaces_ns.h"
#include "val_framework.h"
#include "val_client_defs.h"
#include "val_attestation.h"

#ifdef INITIAL_ATTESTATION

uint32_t    mandatory_claims = 0;
uint32_t    mandaroty_sw_components = 0;
bool_t      sw_component_present = 0;

static int get_items_in_map(QCBORDecodeContext *decode_context,
                            struct items_to_get_t *item_list)
{
    int                     item_index;
    QCBORItem               item;
    struct items_to_get_t  *item_ptr = item_list;

    /* initialize the data type of all items in the list */
    while (item_ptr->label != 0)
    {
        item_ptr->item.uDataType = QCBOR_TYPE_NONE;
        item_ptr++;
    }

    QCBORDecode_GetNext(decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_MAP)
    {
        return VAL_ATTEST_ERROR;
    }

    for (item_index = item.val.uCount; item_index != 0; item_index--)
    {
        if (QCBORDecode_GetNext(decode_context, &item) != QCBOR_SUCCESS)
        {
            return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
        }
        if (item.uLabelType != QCBOR_TYPE_INT64)
        {
            continue;
        }

        item_ptr = item_list;
        while (item_ptr->label != 0)
        {
            if (item.label.int64 == item_ptr->label)
            {
                item_ptr->item = item;
            }
            item_ptr++;
        }
    }

    return VAL_ATTEST_SUCCESS;
}

static int get_item_in_map(QCBORDecodeContext *decode_context,
                           int32_t label,
                           QCBORItem *item)
{
    struct items_to_get_t   item_list[2];

    item_list[0].label = label;
    item_list[1].label = 0;

    if (get_items_in_map(decode_context, item_list))
    {
        return VAL_ATTEST_ERROR;
    }

    if (item_list[0].item.uDataType == QCBOR_TYPE_NONE)
    {
        return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
    }

    *item = item_list[0].item;

    return VAL_ATTEST_SUCCESS;
}

static int parse_unprotected_headers(QCBORDecodeContext *decode_context,
                                     struct q_useful_buf_c *child __UNUSED)
{
    struct items_to_get_t   item_list[3];

    item_list[0].label = COSE_HEADER_PARAM_KID;
    item_list[1].label = 0;

    if (get_items_in_map(decode_context, item_list))
    {
        return VAL_ATTEST_ERR_CBOR_STRUCTURE;
    }

    return VAL_ATTEST_SUCCESS;
}

static int parse_protected_headers(struct q_useful_buf_c protected_headers,
                                   int32_t *alg_id)
{
    QCBORDecodeContext  decode_context;
    QCBORItem           item;

    QCBORDecode_Init(&decode_context, protected_headers, 0);

    if (get_item_in_map(&decode_context, COSE_HEADER_PARAM_ALG, &item))
    {
        return VAL_ATTEST_ERROR;
    }

    if (QCBORDecode_Finish(&decode_context))
    {
        return VAL_ATTEST_ERROR;
    }

    if ((item.uDataType != QCBOR_TYPE_INT64) || (item.val.int64 > INT32_MAX))
    {
        return VAL_ATTEST_ERROR;
    }

    *alg_id = (int32_t)item.val.int64;

    return VAL_ATTEST_SUCCESS;
}

/**
    @brief    - This API will verify the claims
    @param    - decode_context      : The buffer containing the challenge
                item                : context for decoding the data items
                completed_challenge : Buffer containing the challenge
    @return   - error status
**/
static int parse_claims(QCBORDecodeContext *decode_context, QCBORItem item,
                                   struct q_useful_buf_c completed_challenge)
{
    int i, count = 0, sw_comp_count = 0, index = 0;
    int status = VAL_ATTEST_SUCCESS;

    /* Parse each claim and validate their data type */
    while (status == VAL_ATTEST_SUCCESS)
    {
        status = QCBORDecode_GetNext(decode_context, &item);
        if (status != VAL_ATTEST_SUCCESS)
            break;

        mandatory_claims |= 1 << (EAT_CBOR_ARM_RANGE_BASE - item.label.int64);
        if (item.uLabelType == QCBOR_TYPE_INT64)
        {
            if (item.label.int64 == EAT_CBOR_ARM_LABEL_NONCE)
            {
                if (item.uDataType == QCBOR_TYPE_BYTE_STRING)
                {
                    /* Given challenge vs challenge in token */
                    if (UsefulBuf_Compare(item.val.string, completed_challenge))
                        return VAL_ATTEST_TOKEN_CHALLENGE_MISMATCH;
                }
                else
                    return VAL_ATTEST_TOKEN_NOT_SUPPORTED;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_BOOT_SEED ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_UEID)
            {
                if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
                    return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_ORIGINATION ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_PROFILE_DEFINITION ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_HW_VERSION)
            {
                if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                    return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_CLIENT_ID ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE)
            {
                if (item.uDataType != QCBOR_TYPE_INT64)
                    return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_SW_COMPONENTS)
            {
                if (item.uDataType != QCBOR_TYPE_ARRAY)
                    return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

                sw_comp_count = item.val.uCount;
                sw_component_present = 1;
                for (index = 0; index < sw_comp_count; index++)
                {
                    status = QCBORDecode_GetNext(decode_context, &item);
                    if (status != VAL_ATTEST_SUCCESS)
                       continue;

                    count = item.val.uCount;
                    for (i = 0; i <= count; i++)
                    {
                        //mandaroty_sw_components |= 1 << item.label.int64;
                        if (item.label.int64 == EAT_CBOR_SW_COMPONENT_MEASUREMENT)
                        {
                           if (index == 0)
                        	   mandaroty_sw_components |= 1 << item.label.int64;
                           if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
                              return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                        }
                        else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_MEASUREMENT_DESC)
                        {
                            if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                                return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                        }
                        else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_VERSION)
                        {
                            if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                               return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                        }
                        else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_SIGNER_ID)
                        {
                            if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
                               return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                        }
                        else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_EPOCH)
                        {
                             if (item.uDataType != QCBOR_TYPE_INT64)
                               return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                        }
                        else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_TYPE)
                        {
                            if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                               return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                        }

                        if (i < count)
                        {
                            status = QCBORDecode_GetNext(decode_context, &item);
                            if (status != VAL_ATTEST_SUCCESS)
                               return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                        }
                    } /*for (i = 0; i <= count; i++)*/
                } /*for (index = 0; index<sw_comp_count; index++)*/

            }
        }
        else
        {
            /* For other claim types */
        }
    }

    if (status == QCBOR_ERR_HIT_END || status == QCBOR_ERR_NO_MORE_ITEMS)
        return VAL_ATTEST_SUCCESS;
    else
        return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
}

/**
    @brief    - This API will verify the attestation token
    @param    - challenge       : The buffer containing the challenge
                challenge_size  : Size of the challenge buffer
                token           : The buffer containing the attestation token
                token_size      : Size of the token buffer
    @return   - error status
**/
int32_t val_initial_attest_verify_token(uint8_t *challenge, size_t challenge_size,
                                        uint8_t *token, size_t token_size)
{
    int32_t               status = VAL_ATTEST_SUCCESS;
    int32_t               cose_algorithm_id;
    QCBORItem             item;
    QCBORDecodeContext    decode_context;
    struct q_useful_buf_c completed_challenge;
    struct q_useful_buf_c completed_token;
    struct q_useful_buf_c payload;
    struct q_useful_buf_c signature;
    struct q_useful_buf_c protected_headers;
    struct q_useful_buf_c kid;
    struct q_useful_buf_c token_hash;

    USEFUL_BUF_MAKE_STACK_UB(buffer_for_encoded_key, MAX_ENCODED_COSE_KEY_SIZE);
    USEFUL_BUF_MAKE_STACK_UB(buffer_for_token_hash, T_COSE_CRYPTO_SHA256_SIZE);

    kid.ptr = buffer_for_encoded_key.ptr;

    /* Construct the token buffer for validation */
    completed_token.ptr = token;
    completed_token.len = token_size;

    /* Construct the challenge buffer for validation */
    completed_challenge.ptr = challenge;
    completed_challenge.len = challenge_size;

/*
    -------------------------
    |  CBOR Array Type      |
    -------------------------
    |  Protected Headers    |
    -------------------------
    |  Unprotected Headers  |
    -------------------------
    |  Payload              |
    -------------------------
    |  Signature            |
    -------------------------
*/

    /* Initialize the decorder */
    QCBORDecode_Init(&decode_context, completed_token, QCBOR_DECODE_MODE_NORMAL);

    /* Get the Header */
    QCBORDecode_GetNext(&decode_context, &item);

    /* Check the CBOR Array type. Check if the count is 4.
     * Only COSE_SIGN1 is supported now.
     */
    if (item.uDataType != QCBOR_TYPE_ARRAY || item.val.uCount != 4 ||
       !QCBORDecode_IsTagged(&decode_context, &item, CBOR_TAG_COSE_SIGN1))
        return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    /* Get the next headers */
    QCBORDecode_GetNext(&decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
        return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    protected_headers = item.val.string;

    /* Parse the protected headers and check the data type and value*/
    status = parse_protected_headers(protected_headers, &cose_algorithm_id);
    if (status != VAL_ATTEST_SUCCESS)
        return status;

    /* Parse the unprotected headers and check the data type and value */
    status = parse_unprotected_headers(&decode_context, &kid);
    if (status != VAL_ATTEST_SUCCESS)
        return status;

    /* Get the payload */
    QCBORDecode_GetNext(&decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
        return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    payload = item.val.string;

    /* Get the digital signature */
    QCBORDecode_GetNext(&decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
        return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    signature = item.val.string;

    /* Compute the hash from the token */
    status = val_attestation_function(VAL_INITIAL_ATTEST_COMPUTE_HASH, cose_algorithm_id,
                                      buffer_for_token_hash, &token_hash,
                                      protected_headers, payload);
    if (status != VAL_ATTEST_SUCCESS)
        return status;

    /* Verify the signature */
    status = val_attestation_function(VAL_INITIAL_ATTEST_VERIFY_WITH_PK, cose_algorithm_id,
                                      token_hash, signature);
    if (status != VAL_ATTEST_SUCCESS)
        return status;

    /* Initialize the Decoder and validate the payload format */
    QCBORDecode_Init(&decode_context, payload, QCBOR_DECODE_MODE_NORMAL);
    status = QCBORDecode_GetNext(&decode_context, &item);
    if (status != VAL_ATTEST_SUCCESS)
        return status;

    if (item.uDataType != QCBOR_TYPE_MAP)
        return VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    /* Parse the payload and check the data type of each claim */
    status = parse_claims(&decode_context, item, completed_challenge);
    if (status != VAL_ATTEST_SUCCESS)
        return status;

    if ((mandatory_claims & MANDATORY_CLAIM_WITH_SW_COMP) == MANDATORY_CLAIM_WITH_SW_COMP)
    {
        if ((mandaroty_sw_components & MANDATORY_SW_COMP) != MANDATORY_SW_COMP)
            return VAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS;
        mandaroty_sw_components = 0;
    }
    else if ((mandatory_claims & MANDATORY_CLAIM_NO_SW_COMP) != MANDATORY_CLAIM_NO_SW_COMP)
    {
        return VAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS;
    }

    return VAL_ATTEST_SUCCESS;
}
#endif /* INITIAL_ATTESTATION */

/**
    @brief    - This API will call the requested attestation function
    @param    - type : function code
                ...  : variable number of arguments
    @return   - Error status
**/
int32_t val_attestation_function(int type, ...)
{
#ifdef INITIAL_ATTESTATION
    va_list      valist;
    val_status_t status;
    uint8_t      *challenge, *token;
    size_t       challenge_size, verify_token_size;

    va_start(valist, type);
    switch (type)
    {
        case VAL_INITIAL_ATTEST_VERIFY_TOKEN:
            challenge = va_arg(valist, uint8_t*);
            challenge_size = va_arg(valist, size_t);
            token = va_arg(valist, uint8_t*);
            verify_token_size = va_arg(valist, size_t);
            status = val_initial_attest_verify_token(challenge, challenge_size,
                                                   token, verify_token_size);
            break;
        default:
            status = pal_attestation_function(type, valist);
            break;
    }

    va_end(valist);
    return status;
#else
    (void)type;
    return VAL_STATUS_ERROR;
#endif /* INITIAL_ATTESTATION */
}
