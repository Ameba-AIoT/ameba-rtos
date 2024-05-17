/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms_hal.h"

#include "rss_comms.h"
#include "rss_comms_queue.h"
#include "mhu.h"
#include "device_definition.h"
#include "tfm_spm_log.h"
#include "tfm_pools.h"
#include "rss_comms_protocol.h"
#include <string.h>

/* Declared statically to avoid using huge amounts of stack space. Maybe revisit
 * if functions not being reentrant becomes a problem.
 */
static __ALIGNED(4) struct serialized_psa_msg_t msg;
static __ALIGNED(4) struct serialized_psa_reply_t reply;

TFM_POOL_DECLARE(req_pool, sizeof(struct client_request_t),
                 RSS_COMMS_MAX_CONCURRENT_REQ);

static enum tfm_plat_err_t initialize_mhu(void)
{
    enum mhu_error_t err;

    err = mhu_init_sender(&MHU_RSS_TO_AP_DEV);
    if (err != MHU_ERR_NONE) {
        SPMLOG_ERRMSGVAL("[COMMS] RSS to AP MHU driver init failed: ", err);
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = mhu_init_receiver(&MHU_AP_TO_RSS_DEV);
    if (err != MHU_ERR_NONE) {
        SPMLOG_ERRMSGVAL("[COMMS] AP to RSS MHU driver init failed: ", err);
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    SPMLOG_DBGMSG("[COMMS] MHU driver initialized successfully.\r\n");
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_multi_core_hal_receive(void *mhu_receiver_dev,
                                               void *mhu_sender_dev)
{
    enum mhu_error_t mhu_err;
    enum tfm_plat_err_t err;
    size_t msg_len = sizeof(msg);
    size_t reply_size;

    memset(&msg, 0, sizeof(msg));
    memset(&reply, 0, sizeof(reply));

    /* Receive complete message */
    mhu_err = mhu_receive_data(mhu_receiver_dev, (uint8_t *)&msg, &msg_len);
    if (mhu_err != MHU_ERR_NONE) {
        SPMLOG_DBGMSGVAL("[COMMS] MHU receive failed: ", mhu_err);
        /* Can't respond, since we don't know anything about the message */
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    SPMLOG_DBGMSG("[COMMS] Received message\r\n");
    SPMLOG_DBGMSGVAL("[COMMS] size=", msg_len);
    SPMLOG_DBGMSGVAL("[COMMS] seq_num=", msg.header.seq_num);

    struct client_request_t *req = tfm_pool_alloc(req_pool);
    if (!req) {
        /* No free capacity, drop message */
        err = TFM_PLAT_ERR_SYSTEM_ERR;
        goto out_return_err;
    }

    err = rss_protocol_deserialize_msg(req, &msg, msg_len);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        /* Deserialisation failed, drop message */
        SPMLOG_DBGMSGVAL("[COMMS] Deserialize message failed: ", err);
        goto out_free_req;
    }

    /* Record the MHU sender device to be used for the reply */
    req->mhu_sender_dev = mhu_sender_dev;

    if (queue_enqueue(req) != 0) {
        /* No queue capacity, drop message */
        err = TFM_PLAT_ERR_SYSTEM_ERR;
        goto out_free_req;
    }

    /* Message successfully received */
    return TFM_PLAT_ERR_SUCCESS;

out_free_req:
    tfm_pool_free(req_pool, req);

out_return_err:
    /* Attempt to respond with a failure message */
    if (rss_protocol_serialize_error(req, &msg.header,
                                     PSA_ERROR_CONNECTION_BUSY,
                                     &reply, &reply_size)
        == TFM_PLAT_ERR_SUCCESS) {
        mhu_send_data(mhu_sender_dev, (uint8_t *)&reply, reply_size);
    }

    return err;
}

enum tfm_plat_err_t tfm_multi_core_hal_reply(struct client_request_t *req)
{
    enum tfm_plat_err_t err;
    enum mhu_error_t mhu_err;
    size_t reply_size;

    if (!is_valid_chunk_data_in_pool(req_pool, (uint8_t *)req)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* FIXME: Queue replies to avoid blocking execution */

    err = rss_protocol_serialize_reply(req, &reply, &reply_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        SPMLOG_DBGMSGVAL("[COMMS] Serialize reply failed: ", err);
        goto out;
    }

    mhu_err = mhu_send_data(req->mhu_sender_dev, (uint8_t *)&reply, reply_size);
    if (mhu_err != MHU_ERR_NONE) {
        SPMLOG_DBGMSGVAL("[COMMS] MHU send failed: ", mhu_err);
        err = TFM_PLAT_ERR_SYSTEM_ERR;
        goto out;
    }

    SPMLOG_DBGMSG("[COMMS] Sent reply\r\n");

out:
    tfm_pool_free(req_pool, req);
    return err;
}

enum tfm_plat_err_t tfm_multi_core_hal_init(void)
{
    int32_t spm_err;

    spm_err = tfm_pool_init(req_pool, POOL_BUFFER_SIZE(req_pool),
                            sizeof(struct client_request_t),
                            RSS_COMMS_MAX_CONCURRENT_REQ);
    if (spm_err) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return initialize_mhu();
}
