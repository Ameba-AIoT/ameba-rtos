/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_H__
#define __RSS_COMMS_H__

#include "psa/client.h"
#include "cmsis_compiler.h"
#include "rss_comms_atu.h"

#ifdef __cplusplus
extern "C" {
#endif

 /* size suits to get_attest_token() */
#define RSS_COMMS_PAYLOAD_MAX_SIZE (0x40 + 0x800)

/*
 * Allocated for each client request.
 *
 * TODO: Sizing of payload_buf, this should be platform dependent:
 * - sum in_vec size
 * - sum out_vec size
 */
struct client_request_t {
    void *mhu_sender_dev; /* Pointer to MHU sender device to reply on */
    uint8_t protocol_ver;
    uint8_t seq_num;
    uint16_t client_id;
    psa_handle_t handle;
    int32_t type;
    uint32_t in_len;
    uint32_t out_len;
    psa_invec in_vec[PSA_MAX_IOVEC];
    psa_outvec out_vec[PSA_MAX_IOVEC];
    int32_t return_val;
    uint64_t out_vec_host_addr[PSA_MAX_IOVEC];
    uint8_t param_copy_buf[RSS_COMMS_PAYLOAD_MAX_SIZE];
    comms_atu_region_set_t atu_regions;
};

#ifdef __cplusplus
}
#endif

#endif /* __RSS_COMMS_H__ */
