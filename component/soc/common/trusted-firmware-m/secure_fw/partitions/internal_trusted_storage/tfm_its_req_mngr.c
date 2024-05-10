/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "cmsis_compiler.h"
#include "config_tfm.h"
#include "psa/storage_common.h"
#include "tfm_internal_trusted_storage.h"

#include "psa/framework_feature.h"
#include "psa/service.h"
#include "psa_manifest/tfm_internal_trusted_storage.h"
#include "tfm_its_defs.h"

#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
static uint8_t *p_data;
#else
static psa_handle_t handle;
#endif

static psa_status_t tfm_its_set_req(const psa_msg_t *msg)
{
    psa_storage_uid_t uid;
    psa_storage_create_flags_t create_flags;
    size_t num;
    size_t data_length;

    if (msg->in_size[0] != sizeof(uid) ||
        msg->in_size[2] != sizeof(create_flags)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 2, &create_flags, sizeof(create_flags));
    if (num != sizeof(create_flags)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    data_length = msg->in_size[1];
#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
    if (data_length) {
        p_data = (uint8_t *)psa_map_invec(msg->handle, 1);
    } else {
        p_data = NULL;
    }
#else
    handle = msg->handle;
#endif
    return tfm_its_set(msg->client_id, uid, data_length, create_flags);
}

static psa_status_t tfm_its_get_req(const psa_msg_t *msg)
{
    psa_status_t status;
    psa_storage_uid_t uid;
    size_t data_size;
    size_t data_length;
    size_t data_offset;
    size_t num;

    if (msg->in_size[0] != sizeof(uid) ||
        msg->in_size[1] != sizeof(data_offset)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 1, &data_offset, sizeof(data_offset));
    if (num != sizeof(data_offset)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    data_size = msg->out_size[0];
#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
    if (data_size) {
        p_data = (uint8_t *)psa_map_outvec(msg->handle, 0);
    } else {
        p_data = NULL;
    }
#else
    handle = msg->handle;
#endif
    status = tfm_its_get(msg->client_id, uid, data_offset, data_size, &data_length);
#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
    if ((status == PSA_SUCCESS) && (data_size != 0)) {
        psa_unmap_outvec(msg->handle, 0, data_length);
    }
#endif
    return status;
}

static psa_status_t tfm_its_get_info_req(const psa_msg_t *msg)
{
    psa_status_t status;
    psa_storage_uid_t uid;
    struct psa_storage_info_t info;
    size_t num;

    if (msg->in_size[0] != sizeof(uid) ||
        msg->out_size[0] != sizeof(info)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    status = tfm_its_get_info(msg->client_id, uid, &info);
    if (status == PSA_SUCCESS) {
        psa_write(msg->handle, 0, &info, sizeof(info));
    }

    return status;
}

static psa_status_t tfm_its_remove_req(const psa_msg_t *msg)
{
    psa_storage_uid_t uid;
    size_t num;

    if (msg->in_size[0] != sizeof(uid)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg->handle, 0, &uid, sizeof(uid));
    if (num != sizeof(uid)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_its_remove(msg->client_id, uid);
}

psa_status_t tfm_its_entry(void)
{
    return tfm_its_init();
}

psa_status_t tfm_internal_trusted_storage_service_sfn(const psa_msg_t *msg)
{
    switch (msg->type) {
    case TFM_ITS_SET:
        return tfm_its_set_req(msg);
    case TFM_ITS_GET:
        return tfm_its_get_req(msg);
    case TFM_ITS_GET_INFO:
        return tfm_its_get_info_req(msg);
    case TFM_ITS_REMOVE:
        return tfm_its_remove_req(msg);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return PSA_ERROR_GENERIC_ERROR;
}

#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
static uint8_t *p_data;
uint8_t *its_req_mngr_get_vec_base(void)
{
    return p_data;
}
#else
size_t its_req_mngr_read(uint8_t *buf, size_t num_bytes)
{
    return psa_read(handle, 1, buf, num_bytes);
}

void its_req_mngr_write(const uint8_t *buf, size_t num_bytes)
{
    psa_write(handle, 0, buf, num_bytes);
}
#endif

