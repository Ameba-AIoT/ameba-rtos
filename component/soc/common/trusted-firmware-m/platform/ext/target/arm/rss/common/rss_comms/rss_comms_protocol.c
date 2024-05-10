/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms_protocol.h"

#include "tfm_spm_log.h"
#include <string.h>

enum tfm_plat_err_t rss_protocol_deserialize_msg(
        struct client_request_t *req, struct serialized_psa_msg_t *msg,
        size_t msg_len)
{
    memset(req, 0, sizeof(struct client_request_t));

    if (msg_len < sizeof(msg->header)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    req->protocol_ver = msg->header.protocol_ver;
    req->seq_num = msg->header.seq_num;
    req->client_id = msg->header.client_id;

    switch (msg->header.protocol_ver) {
#ifdef RSS_COMMS_PROTOCOL_EMBED_ENABLED
    case RSS_COMMS_PROTOCOL_EMBED:
        SPMLOG_DBGMSG("[COMMS] Deserializing as embed message\r\n");
        return rss_protocol_embed_deserialize_msg(req, &msg->msg.embed,
            msg_len - sizeof(struct serialized_rss_comms_header_t));
#endif /* RSS_COMMS_PROTOCOL_EMBED_ENABLED */
#ifdef RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
    case RSS_COMMS_PROTOCOL_POINTER_ACCESS:
        SPMLOG_DBGMSG("[COMMS] Deserializing as pointer_access message\r\n");
        return rss_protocol_pointer_access_deserialize_msg(req, &msg->msg.pointer_access,
                                               msg_len - sizeof(struct serialized_rss_comms_header_t));
#endif
    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }
}

enum tfm_plat_err_t rss_protocol_serialize_reply(struct client_request_t *req,
        struct serialized_psa_reply_t *reply, size_t *reply_size)
{
    enum tfm_plat_err_t err;

    memset(reply, 0, sizeof(struct serialized_psa_reply_t));

    reply->header.protocol_ver = req->protocol_ver;
    reply->header.seq_num = req->seq_num;
    reply->header.client_id = req->client_id;

    switch (reply->header.protocol_ver) {
#ifdef RSS_COMMS_PROTOCOL_EMBED_ENABLED
    case RSS_COMMS_PROTOCOL_EMBED:
        err = rss_protocol_embed_serialize_reply(req, &reply->reply.embed,
                                                 reply_size);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
        break;
#endif /* RSS_COMMS_PROTOCOL_EMBED_ENABLED */
#ifdef RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
    case RSS_COMMS_PROTOCOL_POINTER_ACCESS:
        err = rss_protocol_pointer_access_serialize_reply(req,
                &reply->reply.pointer_access, reply_size);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
        break;
#endif
    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    *reply_size += sizeof(struct serialized_rss_comms_header_t);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t rss_protocol_serialize_error(
        struct client_request_t *req,
        struct serialized_rss_comms_header_t *header, psa_status_t error,
        struct serialized_psa_reply_t *reply, size_t *reply_size)
{
    enum tfm_plat_err_t err;

    memset(reply, 0, sizeof(struct serialized_psa_reply_t));
    memcpy(&reply->header, header,
           sizeof(struct serialized_rss_comms_header_t));

    switch (reply->header.protocol_ver) {
#ifdef RSS_COMMS_PROTOCOL_EMBED_ENABLED
    case RSS_COMMS_PROTOCOL_EMBED:
        err = rss_protocol_embed_serialize_error(req, error,
                                                 &reply->reply.embed,
                                                 reply_size);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
        break;
#endif /* RSS_COMMS_PROTOCOL_EMBED_ENABLED */
#ifdef RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
    case RSS_COMMS_PROTOCOL_POINTER_ACCESS:
        err = rss_protocol_pointer_access_serialize_error(req, error,
                &reply->reply.pointer_access, reply_size);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
        break;
#endif
    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    *reply_size += sizeof(struct serialized_rss_comms_header_t);

    return TFM_PLAT_ERR_SUCCESS;
}
