/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     provision_service.h
  * @brief    Head file for smart mesh provision service.
  * @details
  * @author   bill
  * @date     2015-11-09
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _PROVISION_SERVICE_H_
#define _PROVISION_SERVICE_H_

/* Add Includes here */
#include "provision_generic.h"
#include "proxy_protocol.h"

BEGIN_DECLS

/** @addtogroup Provision_Service
  * @{
  */

/** @defgroup Provision_Service_Exported_Constants Exported Constants
  * @{
  */
#define MESH_PB_GATT_PKT_OFFSET                         (3) //!< opcode + handle
#define MESH_PB_GATT_PKT_HEADER_SIZE                    (1) //!< control(2 bits) + info(6 bits)
#define MESH_PB_GATT_START_HEADER_SIZE                  (3) //!< length(2) + fcs(1)

///@cond
/** @brief  Index of each characteristic in service database. */
#define MESH_PROV_SERVICE_UUID                          0x1827
#define MESH_PROV_DATA_IN_UUID                          0x2ADB
#define MESH_PROV_DATA_OUT_UUID                         0x2ADC
///@endcond

/** @} */

/** @defgroup Provision_Service_Exported_Types Exported Types
  * @brief
  * @{
  */
extern uint8_t prov_proxy_ctx_id;
extern prov_bearer_cb_pf pf_pb_gatt_cb;
/** @} */

/** @defgroup Provision_Service_Exported_Functions Exported Functions
  * @brief
  * @{
  */

/**
  * @brief allocate proxy resource for provision on this link
  *
  * The resource will be released automatically when disconnected.
  * @param[in] link: the link for provisioning
  *
  * @return the proxy resource index
  * @retval MESH_PROXY_PROTOCOL_RSVD_CTX_ID: allocate failed
  */
uint8_t prov_service_alloc_proxy_ctx(gap_sched_link_t link);

/**
  * @brief allocate proxy resources for provision on this link
  *
  * Use this api to recycle resource if the link is still kept.
  * @param[in] link: the link for provisioning
  * @return operation result
  */
bool prov_service_free_proxy_ctx(gap_sched_link_t link);

///@cond
uint8_t prov_service_get_active_count(void);
bool prov_service_send(gap_sched_link_t link, uint8_t *pvalue, uint16_t len);
bool prov_service_receive(uint8_t ctx_id, proxy_pdu_type_t type, uint8_t *pvalue, uint16_t len);
void prov_service_handle_tx_done(gap_sched_link_t link);
void prov_service_handle_disc(gap_sched_link_t link);
///@endcond
/** @} */
/** @} */

END_DECLS

#endif /* _PROVISION_SERVICE_H_ */
