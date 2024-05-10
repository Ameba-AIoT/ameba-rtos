/** @file
 * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

#include <adac_util.h>
#include <psa_adac_cryptosystems.h>
#include <psa_adac_sdm.h>
#include <psa_adac_debug.h>

void psa_adac_host_init(void)
{
    psa_crypto_init();
}

psa_status_t psa_adac_load_certificate_chain(const char *chain_file, uint8_t **chain,
                                             size_t *chain_size)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;

    if (chain_file == NULL) {
        PSA_ADAC_LOG_ERR("file", "chain file path not found\n");
        r = PSA_ERROR_INVALID_ARGUMENT;
        return r;
    }
    ret_val = load_trust_chain(chain_file, chain, chain_size);
    if (ret_val != 0) {
        PSA_ADAC_LOG_ERR("loader", "Certificate chain cannot be loaded\n");
        r = PSA_ERROR_GENERIC_ERROR;
    }
    return r;
}

psa_status_t psa_adac_read_extensions(uint32_t *chain, size_t chain_size, psa_tlv_t **extns_list,
                                      size_t *extns_count)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;
    size_t count;

    ret_val = split_tlv_static(chain, chain_size, extns_list, MAX_EXTENSIONS, extns_count);
    if (ret_val != 0) {
        PSA_ADAC_LOG_ERR("host", "Error parsing trust chain\n");
        r = PSA_ERROR_GENERIC_ERROR;
        return r;
    }
    count = *extns_count;
    if (count > MAX_EXTENSIONS) {
        PSA_ADAC_LOG_ERR("certificate", "Extension count exceeded maximum allowed\n");
        r = PSA_ERROR_NOT_PERMITTED;
        return r;
    }

    PSA_ADAC_LOG_INFO("host", "Found %zu certificates\n", count);
    return r;
}

uint8_t detect_cryptosystem(psa_tlv_t *extns_list[], size_t extn_count)
{
    size_t i;
    psa_tlv_t *current_extn;
    uint8_t key_type;

    for (i = 0; i < extn_count; i++) {
        current_extn = extns_list[i];
        if ((current_extn)->type_id == CERT_ADAC)
            key_type = ((certificate_header_t *) current_extn->value)->key_type;
    }
    PSA_ADAC_LOG_INFO("host", "Cryptosystem detected: %d\n", key_type);
    return key_type;
}

uint8_t get_certificate_role(psa_tlv_t *extns_list[], size_t extn_count)
{
    size_t i;
    psa_tlv_t *current_extn;
    uint8_t role_type;

    for (i = 0; i < extn_count; i++) {
        current_extn = extns_list[i];
        if ((current_extn)->type_id == CERT_ADAC)
            role_type = ((certificate_header_t *) current_extn->value)->role;
    }
    PSA_ADAC_LOG_INFO("host", "Certificate role: %d\n", role_type);
    return role_type;
}

psa_status_t psa_adac_get_private_key(const char *key_file, uint8_t *type, psa_key_handle_t *handle,
                                      uint8_t **key_ptr, size_t *size)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;
    uint8_t key_type = *type;

    if (key_file == NULL) {
        PSA_ADAC_LOG_ERR("file", "key file path not found\n");
        r = PSA_ERROR_INVALID_ARGUMENT;
        return r;
    }

    switch (key_type) {
    case ECDSA_P256_SHA256:
    case ECDSA_P521_SHA512:
    case RSA_3072_SHA256:
    case RSA_4096_SHA256:
    case ED_25519_SHA512:
    case ED_448_SHAKE256:
    case SM_SM2_SM3:
        ret_val = import_private_key(key_file, type, handle);
        if (ret_val != 0) {
            PSA_ADAC_LOG_ERR("loader", "Could not import private key (%s)\n", key_file);
            r = PSA_ERROR_GENERIC_ERROR;
        } else {
            key_ptr = NULL;
            size = 0;
        }
        break;

    case CMAC_AES:
    case HMAC_SHA256:
        ret_val = load_secret_key(key_file, key_type, key_ptr, size);
        if (ret_val != 0) {
            PSA_ADAC_LOG_ERR("loader", "Could not import secret key (%s)\n", key_file);
            r = PSA_ERROR_GENERIC_ERROR;
        } else {
            handle = NULL;
        }
        break;

    default:
        PSA_ADAC_LOG_INFO("host", "Unsupported key type (0x%x)\n", key_type);
        r = PSA_ERROR_NOT_SUPPORTED;
    }
    return r;
}

psa_status_t psa_adac_issue_command(uint32_t command, request_packet_t *packet,
                               uint8_t *data, size_t data_size)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;

    packet = request_packet_build((uint16_t)command, data, data_size);

    if (packet == NULL) {
        PSA_ADAC_LOG_ERR("host", "Command construction failed\n");
        r = PSA_ERROR_GENERIC_ERROR;
        return r;
    }

    switch (command) {
    case SDP_DISCOVERY_CMD:
        PSA_ADAC_LOG_INFO("host", "Sending discovery request\n");
        break;
    case SDP_AUTH_START_CMD:
        PSA_ADAC_LOG_INFO("host", "Sending challenge request\n");
        break;
    case SDP_AUTH_RESPONSE_CMD:
        PSA_ADAC_LOG_INFO("host", "Sending authentication response\n");
        break;
    case SDP_RESUME_BOOT_CMD:
        PSA_ADAC_LOG_INFO("host", "Sending close session command\n");
        break;
    case SDP_LOCK_DEBUG_CMD:
        PSA_ADAC_LOG_INFO("host", "Sending lock debug request\n");
        break;
    default:
        if (command & 0x8000u)
            PSA_ADAC_LOG_INFO("host", "Vendor specific command provided. ID=(0x%x)\n", command);
        else {
            PSA_ADAC_LOG_INFO("host", "Unrecognized command. ID=(0x%x)\n", command);
            r = PSA_ERROR_NOT_SUPPORTED;
        }
    }
    ret_val = request_packet_send(packet);
    if (ret_val < 0)
        r = PSA_ERROR_GENERIC_ERROR;

    request_packet_release(packet);
    return r;
}

response_packet_t *psa_adac_await_response(void)
{
    return response_packet_receive();
}

psa_status_t psa_adac_parse_response(uint32_t command, response_packet_t *packet)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;
    size_t i;
    psa_tlv_t *tlv;
    psa_auth_challenge_t *challenge;

    if (packet == NULL) {
        PSA_ADAC_LOG_ERR("host", "Target response not obtained\n");
        r = PSA_ERROR_COMMUNICATION_FAILURE;
        return r;
    } else {
        PSA_ADAC_LOG_DEBUG("host", "status = 0x%04x, data_count = %d\n",
                                    packet->status, packet->data_count);
    }

    switch (command) {
    case SDP_DISCOVERY_CMD:
        PSA_ADAC_LOG_INFO("host", "Receiving discovery response...\n");
        for (i = 0; (i + 4) < (packet->data_count * 4);) {
            tlv = (psa_tlv_t *) (((uint8_t *)packet->data) + i);
            //PSA_ADAC_LOG_DEBUG("host", "discovery @+%zu, type_id: 0x%04x, length: %d\n",
            //        i, tlv->type_id, tlv->length_in_bytes);
            i += sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
        }
        break;
    case SDP_AUTH_START_CMD:
        PSA_ADAC_LOG_INFO("host", "Receiving challenge..\n");
        if (packet->data_count * 4 != sizeof(psa_auth_challenge_t)) {
            r = PSA_ERROR_GENERIC_ERROR;
            return r;
        }
        challenge = (psa_auth_challenge_t *) packet->data;
        break;
    case SDP_AUTH_RESPONSE_CMD:
    case SDP_RESUME_BOOT_CMD:
    case SDP_LOCK_DEBUG_CMD:
        break;
    default:
        r = PSA_ERROR_NOT_SUPPORTED;
        PSA_ADAC_LOG_INFO("host", "Unrecognized command. ID=(0x%x)\n", command);
    }
    return r;
}

psa_status_t psa_adac_send_certificate(psa_tlv_t **extns_list, size_t extns_count)
{
    request_packet_t *request;
    response_packet_t *response;
    psa_status_t r;
    uint8_t *payload;
    size_t i, payload_size;
    psa_tlv_t *current_extn;

    for (size_t i = 0; i < extns_count; i++) {
        current_extn = extns_list[i];
        if (current_extn->type_id == 0x0201) {
            payload = (uint8_t *)current_extn;
            payload_size = current_extn->length_in_bytes + sizeof(psa_tlv_t);

            PSA_ADAC_LOG_INFO("host", "Sending Certificate..\n");
            r = psa_adac_issue_command(SDP_AUTH_RESPONSE_CMD, request, payload, payload_size);
            if (r != PSA_SUCCESS)
                return r;

            response = psa_adac_await_response();
            r = psa_adac_parse_response(SDP_AUTH_RESPONSE_CMD, response);
            if (r != PSA_SUCCESS)
                return r;

            if (response->status == SDP_NEED_MORE_DATA)
                response_packet_release(response);
        }
    }
    if (response->status != SDP_NEED_MORE_DATA) {
        PSA_ADAC_LOG_ERR("host", "Unexpected response status %x\n", response->status);
        r = PSA_ERROR_GENERIC_ERROR;
        return r;
    }
    response_packet_release(response);
    return r;
}

psa_status_t psa_adac_construct_token(uint8_t challenge[], size_t challenge_size,
                                      uint8_t sign_type, uint8_t exts[], size_t exts_size,
                                      uint8_t *fragment[], size_t *fragment_size, uint8_t *perm,
                                      psa_key_handle_t handle, uint8_t *key, size_t key_size)
{
    psa_status_t r;

    r = psa_adac_sign_token(challenge, challenge_size, sign_type, exts, exts_size,
                           fragment, fragment_size, perm, handle, key, key_size);
    if (r != PSA_SUCCESS) {
        PSA_ADAC_LOG_ERR("host", "Error signing token\n");
        r = PSA_ERROR_GENERIC_ERROR;
    }
    return r;
}

psa_status_t psa_adac_check_cryptosystem_support(response_packet_t *packet, uint8_t key_system)
{
    int found = 0, j;
    size_t i = 0;
    psa_status_t r;
    psa_tlv_t *tlv;
    uint8_t *key_support_types = NULL;

    while ((i + 4) < (packet->data_count * 4)) {
        tlv = (psa_tlv_t *) (((uint8_t *)packet->data) + i);
        if (tlv->type_id == 0x0102) {
           key_support_types = tlv->value;
           for (j = 0; j < (tlv->length_in_bytes); j++) {
                if (*(key_support_types+j) == key_system) {
                    found = 1;
                    break;
                }
           }
        }
        i += sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
    }

    if (key_support_types == NULL) {
        PSA_ADAC_LOG_ERR("host", "Cryptosystem Type ID not specified\n");
        r = PSA_ERROR_DOES_NOT_EXIST;
    } else if (!found) {
        PSA_ADAC_LOG_ERR("host", "Cryptosystem not supported by target\n");
        r = PSA_ERROR_NOT_SUPPORTED;
    } else {
        PSA_ADAC_LOG_INFO("host", "Cryptosystem supported by target\n");
        r = PSA_SUCCESS;
    }
    return r;
}
