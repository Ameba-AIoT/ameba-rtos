/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "config_tfm.h"
#include "partition_example.h"

#include "tfm_platform_system.h"
#include "load/partition_defs.h"
#include "psa_manifest/pid.h"

#if !PLATFORM_NV_COUNTER_MODULE_DISABLED
#include "tfm_plat_nv_counters.h"
#endif /* !PLATFORM_NV_COUNTER_MODULE_DISABLED */

#include "psa/client.h"
#include "psa/service.h"
#include "region_defs.h"


#if !PLATFORM_NV_COUNTER_MODULE_DISABLED
#define NV_COUNTER_ID_SIZE  sizeof(enum tfm_nv_counter_t)
#endif /* !PLATFORM_NV_COUNTER_MODULE_DISABLED */

typedef enum tfm_example_err_t (*plat_func_t)(const psa_msg_t *msg);

static psa_status_t example_ioctl_psa_api(const psa_msg_t *msg)
{
	void *input = NULL;
	void *output = NULL;
	psa_invec invec = {0};
	psa_outvec outvec = {0};
	uint8_t input_buffer[PLATFORM_SERVICE_INPUT_BUFFER_SIZE] = {0};
	uint8_t output_buffer[PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE] = {0};
	tfm_example_ioctl_req_t request = 0;
	enum tfm_example_err_t ret = TFM_EXAMPLE_ERR_SYSTEM_ERROR;
	int num = 0;
	uint32_t in_len = PSA_MAX_IOVEC;
	uint32_t out_len = PSA_MAX_IOVEC;
	size_t input_size;
	while ((in_len > 0) && (msg->in_size[in_len - 1] == 0)) {
		in_len--;
	}

	while ((out_len > 0) && (msg->out_size[out_len - 1] == 0)) {
		out_len--;
	}

	if ((in_len < 1) || (in_len > 2) ||
		(out_len > 1)) {
		return TFM_EXAMPLE_ERR_SYSTEM_ERROR;
	}

	num = psa_read(msg->handle, 0, &request, sizeof(request));
	if (num != sizeof(request)) {
		return (enum tfm_example_err_t) PSA_ERROR_PROGRAMMER_ERROR;
	}

	if (in_len > 1) {
		input_size = msg->in_size[1];
		if (input_size > PLATFORM_SERVICE_INPUT_BUFFER_SIZE) {
			return (enum tfm_example_err_t) PSA_ERROR_BUFFER_TOO_SMALL;
		}
		num = psa_read(msg->handle, 1, &input_buffer, msg->in_size[1]);
		if (num != input_size) {
			return (enum tfm_example_err_t) PSA_ERROR_PROGRAMMER_ERROR;
		}
		invec.base = input_buffer;
		invec.len = input_size;
		input = &invec;
	}

	if (out_len > 0) {
		if (msg->out_size[0] > PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE) {
			return (enum tfm_example_err_t) PSA_ERROR_PROGRAMMER_ERROR;
		}
		outvec.base = output_buffer;
		outvec.len = msg->out_size[0];
		output = &outvec;
	}

	// ret = tfm_example_hal_ioctl(request, input, output);
	ret = PSA_SUCCESS;

	if (output != NULL) {
		psa_write(msg->handle, 0, outvec.base, outvec.len);
	}

	return ret;
}

psa_status_t tfm_partition_example_service_sfn(const psa_msg_t *msg)
{
	switch (msg->type) {
	case TFM_EXAMPLE_API_ID_IOCTL:
		return example_ioctl_psa_api(msg);
	default:
		return PSA_ERROR_NOT_SUPPORTED;
	}

	return PSA_ERROR_GENERIC_ERROR;
}

psa_status_t tfm_example_init(void)
{
	/* Initialise the non-volatile counters */

	return PSA_SUCCESS;
}
