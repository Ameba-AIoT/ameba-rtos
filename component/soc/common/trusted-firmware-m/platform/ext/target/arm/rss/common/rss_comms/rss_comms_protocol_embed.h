/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_PROTOCOL_EMBED_H__
#define __RSS_COMMS_PROTOCOL_EMBED_H__

#include "psa/client.h"
#include "cmsis_compiler.h"
#include "rss_comms.h"
#include "tfm_platform_system.h"

#ifdef __cplusplus
extern "C" {
#endif

__PACKED_STRUCT rss_embed_msg_t {
    psa_handle_t handle;
    uint32_t ctrl_param; /* type, in_len, out_len */
    uint16_t io_size[PSA_MAX_IOVEC];
    uint8_t payload[RSS_COMMS_PAYLOAD_MAX_SIZE];
};

__PACKED_STRUCT rss_embed_reply_t {
    int32_t return_val;
    uint16_t out_size[PSA_MAX_IOVEC];
    uint8_t payload[RSS_COMMS_PAYLOAD_MAX_SIZE];
};

enum tfm_plat_err_t rss_protocol_embed_deserialize_msg(
        struct client_request_t *req, struct rss_embed_msg_t *msg,
        size_t msg_len);

enum tfm_plat_err_t rss_protocol_embed_serialize_reply(
        struct client_request_t *req, struct rss_embed_reply_t *reply,
        size_t *reply_size);

enum tfm_plat_err_t rss_protocol_embed_serialize_error(
        struct client_request_t *req, psa_status_t err,
        struct rss_embed_reply_t *reply, size_t *reply_size);


#ifdef __cplusplus
}
#endif

#endif /* __RSS_COMMS_PROTOCOL_EMBED_H__ */
