/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms_protocol_embed.h"

#include <string.h>

#include "tfm_psa_call_pack.h"

enum tfm_plat_err_t rss_protocol_embed_deserialize_msg(
        struct client_request_t *req, struct rss_embed_msg_t *msg,
        size_t msg_len)
{
    uint32_t payload_size = 0;
    uint32_t i;

    if (msg_len < (sizeof(*msg) - sizeof(msg->payload))) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    req->in_len = PARAM_UNPACK_IN_LEN(msg->ctrl_param);
    req->out_len = PARAM_UNPACK_OUT_LEN(msg->ctrl_param);
    req->type = PARAM_UNPACK_TYPE(msg->ctrl_param);
    req->handle = msg->handle;

    /* Only support 4 iovecs */
    if (req->in_len + req->out_len > 4) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    /* Invecs */
    for (i = 0; i < req->in_len; ++i) {
        req->in_vec[i].base = req->param_copy_buf + payload_size;
        req->in_vec[i].len = msg->io_size[i];
        payload_size += msg->io_size[i];
    }

    /* Check payload is not too big */
    if (payload_size > sizeof(req->param_copy_buf)
        || payload_size > sizeof(msg->payload)
        || sizeof(*msg) - sizeof(msg->payload) +  payload_size > msg_len ) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    /* Copy payload into the buffer */
    memcpy(req->param_copy_buf, msg->payload, payload_size);

    /* Outvecs */
    for (i = 0; i < req->out_len; ++i) {
        req->out_vec[i].base = req->param_copy_buf + payload_size;
        req->out_vec[i].len = msg->io_size[req->in_len + i];
        payload_size += msg->io_size[req->in_len + i];
    }

    /* Check payload is not too big */
    if (payload_size > sizeof(req->param_copy_buf)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t rss_protocol_embed_serialize_reply(
        struct client_request_t *req, struct rss_embed_reply_t *reply,
        size_t *reply_size)
{
    size_t payload_size = 0;
    size_t len;
    uint32_t i;

    reply->return_val = req->return_val;

    /* Outvecs */
    for (i = 0; i < req->out_len; ++i) {
        len = req->out_vec[i].len;

        if (payload_size + len > sizeof(reply->payload)) {
            return TFM_PLAT_ERR_UNSUPPORTED;
        }

        memcpy(reply->payload + payload_size, req->out_vec[i].base, len);
        reply->out_size[i] = len;
        payload_size += len;
    }

    *reply_size = sizeof(*reply) - sizeof(reply->payload) + payload_size;

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t rss_protocol_embed_serialize_error(
        struct client_request_t *req, psa_status_t err,
        struct rss_embed_reply_t *reply, size_t *reply_size)
{
    reply->return_val = err;

    /* Return the minimum reply size, as the out_sizes are all zeroed */
    *reply_size = sizeof(*reply) - sizeof(reply->payload);

    return TFM_PLAT_ERR_SUCCESS;
}
