/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_PROTOCOL_POINTER_ACCESS_H__
#define __RSS_COMMS_PROTOCOL_POINTER_ACCESS_H__

#include "psa/client.h"
#include "cmsis_compiler.h"
#include "rss_comms.h"
#include "tfm_platform_system.h"

#ifdef __cplusplus
extern "C" {
#endif

__PACKED_STRUCT rss_pointer_access_msg_t {
    psa_handle_t handle;
    uint32_t ctrl_param;
    uint32_t io_sizes[PSA_MAX_IOVEC];
    uint64_t host_ptrs[PSA_MAX_IOVEC];
};

__PACKED_STRUCT rss_pointer_access_reply_t {
    int32_t return_val;
    uint32_t out_size[PSA_MAX_IOVEC];
};

enum tfm_plat_err_t rss_protocol_pointer_access_deserialize_msg(
        struct client_request_t *req, struct rss_pointer_access_msg_t *msg,
        size_t msg_len);

enum tfm_plat_err_t rss_protocol_pointer_access_serialize_reply(
        struct client_request_t *req,
        struct rss_pointer_access_reply_t *reply,
        size_t *reply_size);

enum tfm_plat_err_t rss_protocol_pointer_access_serialize_error(
        struct client_request_t *req, psa_status_t err,
        struct rss_pointer_access_reply_t *reply,
        size_t *reply_size);


#ifdef __cplusplus
}
#endif

#endif /* __RSS_COMMS_PROTOCOL_POINTER_ACCESS_H__ */
