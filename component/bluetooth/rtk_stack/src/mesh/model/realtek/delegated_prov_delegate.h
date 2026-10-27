/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _DELEGATED_PROV_DELEGATE_H_
#define _DELEGATED_PROV_DELEGATE_H_

#include "mesh_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DELEGATED_PROV_DELEGATE
 * @brief Delegate role - fission orchestration on top of the DPROV Server + Client
 *        models (see delegated_prov.h for the models and wire format).
 *
 * A Delegate that provisions further devices carves its address range in half and
 * grants the tail to each node it provisions, turning that node into a Delegate too.
 * This layer registers both models, owns their data callbacks, and drives the split:
 * REPORT_CONFIRMED triggers address splitting + sub-grant, and the child's dev_key is
 * kept (deferred deletion) so the sub-grant GRANT can be retransmitted until the
 * child's GRANT_STATUS confirms it. The models themselves know nothing of fission -
 * all of it lives in delegated_prov_delegate.c.
 * @{
 */

/**
 * @brief register a full Delegate (Server + Client) and enable fission on it
 *
 * A Delegate is Server + Client: the Server runs the granted provisioning flow, the
 * Client sends fission sub-grants to the devices it provisions. This registers both
 * models on the given element and owns their data callbacks, so REPORT_CONFIRMED
 * drives address splitting + sub-grant, the sub-grant GRANT is retransmitted until
 * confirmed, and the child's GRANT_STATUS drives the deferred key deletion. The app
 * just calls this once; no model objects to manage.
 *
 * @param[in] element_index: element the two models register to
 * @return TRUE if both models registered
 */
bool dprov_delegate_init(uint8_t element_index);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _DELEGATED_PROV_DELEGATE_H_ */
