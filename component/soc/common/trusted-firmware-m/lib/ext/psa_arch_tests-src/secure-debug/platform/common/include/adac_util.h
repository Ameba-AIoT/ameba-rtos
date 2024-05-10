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

#ifndef _PSA_ADAC_UTIL_H_
#define _PSA_ADAC_UTIL_H_
#include <psa_adac.h>

#define PSA_LIFECYCLE_MAJOR_STATE 0xFF00u
/** \brief Token header
 *
 */
typedef struct {
    uint16_t type;
    uint8_t *data;
    size_t size;
} adac_command_frame_t;

typedef enum {
    NULL_TYPE             = 0x0000,
    ADAC_AUTH_VERSION     = 0x0001,
    VENDOR_ID             = 0x0002,
    SOC_CLASS             = 0x0003,
    SOC_ID                = 0x0004,
    TARGET_IDENTITY       = 0x0005,
    HW_PERMISSIONS_FIXED  = 0x0006,
    HW_PERMISSIONS_MASK   = 0x0007,
    PSA_LIFECYCLE         = 0x0008,
    SW_PARTITION_ID       = 0x0009,
    SDA_ID                = 0x000a,
    TOKEN_FORMATS         = 0x0100,
    CERT_FORMATS          = 0x0101,
    CRYPTOSYSTEMS         = 0x0102,
    TOKEN_ADAC            = 0x0200,
    CERT_ADAC             = 0x0201,
    ROT_META              = 0x0202,
    VENDOR_TYPE_ID0       = 0x8000
} adac_type_id_t;

typedef enum {
    UNKNOWN                   = 0x0000,
    ASSEMBLY_AND_TEST         = 0x1000,
    PSA_ROT_AND_PROVISIONING  = 0x2000,
    SECURED                   = 0x3000,
    NON_PSA_ROT_DEBUG         = 0x4000,
    RECOVERABLE_PSA_ROT_DEBUG = 0x5000,
    DECOMMISSIONED            = 0x6000
} psa_lifecycle_major_state_t;

void psa_adac_host_init(void);
psa_status_t psa_adac_load_certificate_chain(const char *chain_file, uint8_t **chain,
                                             size_t *chain_size);
psa_status_t psa_adac_read_extensions(uint32_t *chain, size_t chain_size, psa_tlv_t **extns_list,
                                      size_t *extns_count);
uint8_t detect_cryptosystem(psa_tlv_t **extns_list, size_t extns_count);
uint8_t get_certificate_role(psa_tlv_t **extns_list, size_t extns_count);
psa_status_t psa_adac_get_private_key(const char *key_file, uint8_t *type, psa_key_handle_t *handle,
                                      uint8_t **key_ptr, size_t *size);
psa_status_t psa_adac_issue_command(uint32_t command, request_packet_t *packet,
                                    uint8_t *data, size_t data_size);
response_packet_t *psa_adac_await_response(void);
psa_status_t psa_adac_parse_response(uint32_t command, response_packet_t *packet);

psa_status_t psa_adac_send_certificate(psa_tlv_t **extns_list, size_t extns_count);
psa_status_t psa_adac_construct_token(uint8_t challenge[], size_t challenge_size,
                                      uint8_t sign_type, uint8_t exts[], size_t exts_size,
                                      uint8_t *fragment[], size_t *fragment_size, uint8_t *perm,
                                      psa_key_handle_t handle, uint8_t *key, size_t key_size);
psa_status_t psa_adac_check_cryptosystem_support(response_packet_t *packet, uint8_t key_system);

request_packet_t *request_packet_lock(size_t *max_data_size);

/**
 *   @brief    - Reserve the communication buffer memory for receive packet.
 *   @param    - max_data_size      Valid size of command frame
 *   @return   - Pointer to the command frame to be read
**/
response_packet_t *response_packet_lock(size_t *max_data_size);

/**
 *   @brief    - Release the lock held by transmit packet.
 *   @param    - packet      Most recent command frame sent
 *   @return   - SUCCESS/FAILURE
**/
int request_packet_release(request_packet_t *packet);

/**
 *   @brief    - Release the lock held by receive packet.
 *   @param    - packet      Most recent response packet received
 *   @return   - SUCCESS/FAILURE
**/
int response_packet_release(response_packet_t *packet);

/**
 *   @brief    - Construct the Request packet for the specified ADAC command.
 *   @param    - command      ADAC command
 *               data         Pointer to payload
 *               data_size    Size of the command payload
 *   @return   - Pointer to the command frame to be written
**/
request_packet_t *request_packet_build(uint16_t command, uint8_t *data, size_t data_size);

/**
 *   @brief    - Write the Request packet into the communication buffer for transmit.
 *   @param    - packet       Request packet built for dispatch
 *   @return   - SUCCESS/FAILURE
**/
int request_packet_send(request_packet_t *packet);

/**
 *   @brief    - Read the Response packet from the communication buffer.
 *   @param    - None
 *   @return   - Response packet received from target.
**/
response_packet_t *response_packet_receive();

#endif /* _PSA_ADAC_UTIL_H_ */
