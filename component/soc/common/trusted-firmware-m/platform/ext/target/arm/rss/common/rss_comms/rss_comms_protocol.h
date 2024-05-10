/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_PROTOCOL_H__
#define __RSS_COMMS_PROTOCOL_H__

#include "psa/client.h"
#include "cmsis_compiler.h"
#include "rss_comms.h"
#include "tfm_platform_system.h"

#ifdef RSS_COMMS_PROTOCOL_EMBED_ENABLED
#include "rss_comms_protocol_embed.h"
#endif /* RSS_COMMS_PROTOCOL_EMBED_ENABLED */

#ifdef RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
#include "rss_comms_protocol_pointer_access.h"
#endif /* RSS_MHU_PROTOCOL_V0_ENABLED */

#ifdef __cplusplus
extern "C" {
#endif

enum rss_comms_protocol_version_t {
#ifdef RSS_COMMS_PROTOCOL_EMBED_ENABLED
    RSS_COMMS_PROTOCOL_EMBED = 0,
#endif /* RSS_COMMS_PROTOCOL_EMBED_ENABLED */
#ifdef RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
    RSS_COMMS_PROTOCOL_POINTER_ACCESS = 1,
#endif /* RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED */
};


__PACKED_STRUCT serialized_rss_comms_header_t {
    uint8_t protocol_ver;
    uint8_t seq_num;
    uint16_t client_id;
};

/* MHU message passed from NSPE to SPE to deliver a PSA client call */
__PACKED_STRUCT serialized_psa_msg_t {
    struct serialized_rss_comms_header_t header;
    __PACKED_UNION {
#ifdef RSS_COMMS_PROTOCOL_EMBED_ENABLED
        struct rss_embed_msg_t embed;
#endif /* RSS_COMMS_PROTOCOL_EMBED_ENABLED */
#ifdef RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
        struct rss_pointer_access_msg_t pointer_access;
#endif /* RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED */
    } msg;
};

/* MHU reply message to hold the PSA client call return result from SPE */
__PACKED_STRUCT serialized_psa_reply_t {
    struct serialized_rss_comms_header_t header;
    __PACKED_UNION {
#ifdef RSS_COMMS_PROTOCOL_EMBED_ENABLED
        struct rss_embed_reply_t embed;
#endif /* RSS_COMMS_PROTOCOL_EMBED_ENABLED */
#ifdef RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED
        struct rss_pointer_access_reply_t pointer_access;
#endif /* RSS_COMMS_PROTOCOL_POINTER_ACCESS_ENABLED */
    } reply;
};

/**
 * \brief Convert a serialized message to a client_request_t.
 *
 * \param[out] req               The client_request_t to fill.
 * \param[in]  msg               The serialized message to extract data from.
 * \param[in]  msg_len           The size of the message.
 *
 * \retval TFM_PLAT_ERR_SUCCESS  Operation succeeded.
 * \retval Other return code     Operation failed with an error code.
 */
enum tfm_plat_err_t rss_protocol_deserialize_msg(struct client_request_t *req,
        struct serialized_psa_msg_t *msg, size_t msg_len);

/**
 * \brief Convert a a client_request_t to a serialized reply.
 *
 * \param[in]  req               The client_request_t to serialize data from.
 * \param[out] reply             The reply to fill.
 * \param[out] reply_size        The size of the reply that was filled.
 *
 * \retval TFM_PLAT_ERR_SUCCESS  Operation succeeded.
 * \retval Other return code     Operation failed with an error code.
 */
enum tfm_plat_err_t rss_protocol_serialize_reply(struct client_request_t *req,
        struct serialized_psa_reply_t *reply, size_t *reply_size);

/**
 * \brief Create a serialised error reply from a header and an error code.
 *        Intended to for the RSS to notify the AP of errors during the message
 *        deserialization phase.
 *
 * \param[in]  req               The client_request_t to serialize data from. If
 *                               the error occured in allocation this pointer
 *                               may be NULL. This may not contain message
 *                               header information if the message
 *                               deserialize failed.
 * \param[in]  header            The header of the received
 *                               serialized_psa_msg_t whose deserialization
 *                               caused the error.
 * \param[in]  error             The error code to be transmitted to the AP.
 * \param[out] reply             The reply to fill.
 * \param[out] reply_size        The size of the reply that was filled.
 *
 * \retval TFM_PLAT_ERR_SUCCESS  Operation succeeded.
 * \retval Other return code     Operation failed with an error code.
 */
enum tfm_plat_err_t rss_protocol_serialize_error(
        struct client_request_t *req,
        struct serialized_rss_comms_header_t *header, psa_status_t error,
        struct serialized_psa_reply_t *reply, size_t *reply_size);


#ifdef __cplusplus
}
#endif

#endif /* __RSS_COMMS_PROTOCOL_H__ */
