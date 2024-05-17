/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms.h"

#include <stddef.h>
#include <stdint.h>

#include "rss_comms_hal.h"
#include "rss_comms_queue.h"
#include "tfm_rpc.h"
#include "tfm_multi_core.h"
#include "tfm_psa_call_pack.h"
#include "tfm_spm_log.h"
#include "rss_comms_permissions_hal.h"

static struct client_request_t *req_to_process;

static psa_status_t message_dispatch(struct client_request_t *req)
{
    enum tfm_plat_err_t plat_err;

    /* Create the call parameters */
    struct client_call_params_t spm_params = {
        .handle = req->handle,
        .type = req->type,
        .in_vec = req->in_vec,
        .in_len = req->in_len,
        .out_vec = req->out_vec,
        .out_len = req->out_len,
    };

    SPMLOG_DBGMSG("[RSS-COMMS] Dispatching message\r\n");
    SPMLOG_DBGMSGVAL("handle=", spm_params.handle);
    SPMLOG_DBGMSGVAL("type=", spm_params.type);
    SPMLOG_DBGMSGVAL("in_len=", spm_params.in_len);
    SPMLOG_DBGMSGVAL("out_len=", spm_params.out_len);
    if (spm_params.in_len > 0) {
        SPMLOG_DBGMSGVAL("in_vec[0].len=", spm_params.in_vec[0].len);
    }
    if (spm_params.in_len > 1) {
        SPMLOG_DBGMSGVAL("in_vec[1].len=", spm_params.in_vec[1].len);
    }
    if (spm_params.in_len > 2) {
        SPMLOG_DBGMSGVAL("in_vec[2].len=", spm_params.in_vec[2].len);
    }
    if (spm_params.in_len > 3) {
        SPMLOG_DBGMSGVAL("in_vec[3].len=", spm_params.in_vec[3].len);
    }
    if (spm_params.out_len > 0) {
        SPMLOG_DBGMSGVAL("out_vec[0].len=", spm_params.out_vec[0].len);
    }
    if (spm_params.out_len > 1) {
        SPMLOG_DBGMSGVAL("out_vec[1].len=", spm_params.out_vec[1].len);
    }
    if (spm_params.out_len > 2) {
        SPMLOG_DBGMSGVAL("out_vec[2].len=", spm_params.out_vec[2].len);
    }
    if (spm_params.out_len > 3) {
        SPMLOG_DBGMSGVAL("out_vec[3].len=", spm_params.out_vec[3].len);
    }

    plat_err = comms_permissions_service_check(spm_params.handle,
                                               spm_params.in_vec,
                                               spm_params.in_len,
                                               spm_params.type);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        SPMLOG_ERRMSG("[RSS-COMMS] Call not permitted\r\n");
        return PSA_ERROR_NOT_PERMITTED;
    }

    return tfm_rpc_psa_call(&spm_params);
}

static void rss_comms_reply(const void *owner, int32_t ret)
{
    struct client_request_t *req = (struct client_request_t *)owner;

    req->return_val = ret;

    SPMLOG_DBGMSG("[RSS-COMMS] Sending reply\r\n");
    SPMLOG_DBGMSGVAL("protocol_ver=", req->protocol_ver);
    SPMLOG_DBGMSGVAL("seq_num=", req->seq_num);
    SPMLOG_DBGMSGVAL("client_id=", req->client_id);
    SPMLOG_DBGMSGVAL("return_val=", req->return_val);
    SPMLOG_DBGMSGVAL("out_vec[0].len=", req->out_vec[0].len);
    SPMLOG_DBGMSGVAL("out_vec[1].len=", req->out_vec[1].len);
    SPMLOG_DBGMSGVAL("out_vec[2].len=", req->out_vec[2].len);
    SPMLOG_DBGMSGVAL("out_vec[3].len=", req->out_vec[3].len);

    if (tfm_multi_core_hal_reply(req) != TFM_PLAT_ERR_SUCCESS) {
        SPMLOG_DBGMSG("[RSS-COMMS] Sending reply failed!\r\n");
    }
}

static void rss_comms_handle_req(void)
{
    psa_status_t status;
    void *queue_entry;

    /* FIXME: consider memory limitations that may prevent dispatching all
     * messages in one go.
     */
    while (queue_dequeue(&queue_entry) == 0) {
        /* Deliver PSA Client call request to handler in SPM. */
        req_to_process = queue_entry;
        status = message_dispatch(req_to_process);
        if (status != PSA_SUCCESS) {
            SPMLOG_DBGMSGVAL("[RSS-COMMS] Message dispatch failed: ", status);
            rss_comms_reply(req_to_process, status);
        }
    }
}

static const void *rss_comms_get_caller_data(int32_t client_id)
{
    (void)client_id;

    return req_to_process;
}

static struct tfm_rpc_ops_t rpc_ops = {
    .handle_req = rss_comms_handle_req,
    .reply = rss_comms_reply,
    .get_caller_data = rss_comms_get_caller_data,
};

int32_t tfm_inter_core_comm_init(void)
{
    int32_t ret;

    /* Register RPC callbacks */
    ret = tfm_rpc_register_ops(&rpc_ops);
    if (ret != TFM_RPC_SUCCESS) {
        return ret;
    }

    /* Platform specific initialization */
    ret = tfm_multi_core_hal_init();
    if (ret != TFM_PLAT_ERR_SUCCESS) {
        tfm_rpc_unregister_ops();
        return ret;
    }

    return TFM_RPC_SUCCESS;
}
