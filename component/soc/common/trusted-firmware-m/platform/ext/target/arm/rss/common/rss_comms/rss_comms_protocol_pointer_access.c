/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms_protocol_pointer_access.h"

#include "tfm_psa_call_pack.h"
#include "rss_comms_permissions_hal.h"

enum tfm_plat_err_t rss_protocol_pointer_access_deserialize_msg(
        struct client_request_t *req,
        struct rss_pointer_access_msg_t *msg, size_t msg_len)
{
    enum tfm_plat_err_t err;
    uint32_t idx;
    void *mapped_host_ptr;
    uint8_t atu_region;

    if (msg_len != sizeof(*msg)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    req->in_len = PARAM_UNPACK_IN_LEN(msg->ctrl_param);
    req->out_len = PARAM_UNPACK_OUT_LEN(msg->ctrl_param);
    req->type = PARAM_UNPACK_TYPE(msg->ctrl_param);
    req->handle = msg->handle;

    /* Only support 4 iovecs */
    if (req->in_len + req->out_len > PSA_MAX_IOVEC) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    /* Invecs */
    for (idx = 0; idx < req->in_len; idx++) {
        err = comms_permissions_memory_check(msg->host_ptrs[idx],
                                             msg->io_sizes[idx], false);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        err = comms_atu_alloc_region(msg->host_ptrs[idx],
                                     msg->io_sizes[idx],
                                     &atu_region);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        err = comms_atu_add_region_to_set(&req->atu_regions, atu_region);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        err = comms_atu_get_rss_ptr_from_host_addr(atu_region,
                                                   msg->host_ptrs[idx],
                                                   &mapped_host_ptr);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        req->in_vec[idx].base = mapped_host_ptr;
        req->in_vec[idx].len = msg->io_sizes[idx];
    }

    /* Outvecs */
    for (idx = 0; idx < req->out_len; idx++) {
        err = comms_permissions_memory_check(msg->host_ptrs[idx + req->in_len],
                                             msg->io_sizes[idx + req->in_len],
                                             true);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        err = comms_atu_alloc_region(msg->host_ptrs[idx + req->in_len],
                                     msg->io_sizes[idx + req->in_len],
                                     &atu_region);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        err = comms_atu_add_region_to_set(&req->atu_regions, atu_region);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        err = comms_atu_get_rss_ptr_from_host_addr(atu_region,
                msg->host_ptrs[idx + req->in_len], &mapped_host_ptr);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        req->out_vec[idx].base = mapped_host_ptr;
        req->out_vec[idx].len = msg->io_sizes[idx + req->in_len];
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t rss_protocol_pointer_access_serialize_reply(
        struct client_request_t *req,
        struct rss_pointer_access_reply_t *reply, size_t *reply_size)
{
    uint32_t idx;

    reply->return_val = req->return_val;

    /* Outvecs */
    for (idx = 0; idx < req->out_len; idx++) {
        reply->out_size[idx] = req->out_vec[idx].len;
    }

    *reply_size = sizeof(*reply);
    comms_atu_free_regions(req->atu_regions);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t rss_protocol_pointer_access_serialize_error(
        struct client_request_t *req, psa_status_t err,
        struct rss_pointer_access_reply_t *reply,
        size_t *reply_size)
{
    reply->return_val = err;

    /* The out_sizes are already zeroed by the caller. */

    *reply_size = sizeof(*reply);
    if (req != NULL) {
        comms_atu_free_regions(req->atu_regions);
    }

    return TFM_PLAT_ERR_SUCCESS;
}
