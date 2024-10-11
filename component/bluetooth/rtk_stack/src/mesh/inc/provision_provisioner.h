/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     provision_provisioner.h
  * @brief    Head file for provisioner.
  * @details  Data structs and external functions declaration.
  * @author   bill
  * @date     2015-11-09
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _PROVISION_PROVISIONER_H_
#define _PROVISION_PROVISIONER_H_

/* Add Includes here */
#include "provision_generic.h"
#include "mesh_api.h"
#include "mesh_provision.h"

BEGIN_DECLS

/** @addtogroup Mesh_Prov_Provisioner
  * @brief
  * @{
  */

/** @defgroup Mesh_Prov_Provisioner_Exported_Types Exported Types
  * @brief
  * @{
  */
/* for app to configure the local network key index */
extern uint16_t assign_net_key_index;
/**
  * for app to configure the provision start address
  * when use the default address assign policy, the stack will choose a address
  * which is equal to or bigger than the start address and bigger than all the provisioned device address.
  */
extern uint16_t assign_addr;
/** @} */

/** @defgroup Mesh_Prov_Provisioner_Exported_Functions Exported Functions
  * @brief
  * @{
  */

/**
  * @brief start the provisioning
  *
  * The function shall be called at the appropriate timing.
  * @param[in] attn_dur: attention duration
  * @return operation result
  */
bool prov_invite(uint8_t attn_dur);

/**
  * @brief assign the idx and addr to the new device
  *
  * The function can be called after @ref prov_invite and before @ref prov_send_prov_data,
  * normally at the @ref PROV_CB_TYPE_PATH_CHOOSE case handler.
  * The default idx assign scheme is to find the unused entry or the least addr entry.
  * The default addr assign scheme is the maximum address of all devices plus 1.
  * @param[in] idx: the entry index, < 0 represents to use the default assign scheme
  * @param[in] addr: the address, 0 represents to use the default assign scheme
  * @return the assign index
  */
uint16_t prov_assign(int16_t idx, uint16_t addr);

/**
  * @brief assign the network key to the new device
  * @param[in] assign_net_key_index: the key index
  * @return operation result
  */
bool prov_assign_net_key_index(uint16_t assign_net_key_index);

/**
  * @brief stop the provisioning
  * @return operation result
  */
bool prov_reject(void);

/**
  * @brief unprov the device in the pb-gatt
  * @deprecated
  * @return operation result
  */
bool prov_unprovisioning(void);

/**
  * @brief choose one path from the eight prov paths
  *
  * public key: no oob public key & oob public key
  * auth data: no oob & input & output & static
  * @param[in] pprov_start: using the start pdu to choose
  * @return operation result
  */
bool prov_path_choose(prov_start_p pprov_start);

/**
  * @brief set the public key of the provisioning device
  *
  * The function shall be called at the appropriate timing.
  * @param[in] public_key: the ecc public key of the device
  * @return operation result
  */
bool prov_device_public_key_set(uint8_t public_key[64]);

/**
  * @brief set the random and confirmation without the knowledge of auth value
  *
  * The app has no need to provide the auth value to the stack.
  * @param[in] random: the random
  * @param[in] conf: the confirmation
  * @return operation result
  */
bool prov_auth_random_conf_set(uint8_t random[16], uint8_t conf[16]);

/**
  * @brief disconnect the prov bearer
  *
  * The spec requires the provisioner to disconnect the bearer after the provision procedure.
  * The mesh stack leaves the app to decide whether to disconnect at the case @ref PROV_CB_TYPE_COMPLETE.
  * @deprecated
  * @param[in] reason: pb-adv bearer need the disconnect reason
  * @return operation result
  */
bool prov_disconnect_prov(pb_adv_link_close_reason_t reason);

/**
  * @brief set the auth random value
  *
  * The function shall be called at the appropriate time.
  * @deprecated
  * @param[in] random: random value
  * @return operation result
  */
bool prov_auth_random_set_prov(uint8_t random[]);

/**
  * @brief change the auth value
  *
  * The function shall be called at the appropriate time. The auth value will changed immediately
  * @param[in] pvalue: auth value
  * @param[in] len: value length
  * @return operation result
  */
bool prov_auth_value_change_prov(uint8_t *pvalue, uint8_t len);

/**
  * @brief set the auth value
  *
  * The function shall be called at the appropriate time. The auth value shall be
  * set with the appropriate length and data format.
  * @param[in] pvalue: auth value
  * @param[in] len: value length
  * @return operation result
  */
bool prov_auth_value_set_prov(uint8_t *pvalue, uint8_t len);

/**
 * @brief provisioner send data
 * @deprecated
 * @param[in] pprov_ctx: provision information
 * @param[in] pdata: provision data to send
 * @param[in] len: provision data length
 * @return send result
 */
bool prov_send_prov(prov_ctx_p pprov_ctx, uint8_t *pdata, uint16_t len);

/**
 * @brief initialize provision resources
 */
void prov_init_prov(void);

///@cond
void prov_assign_free(prov_ctx_p pprov_ctx);
bool prov_auth_value_set_prov_int(prov_ctx_p pprov_ctx, uint8_t *pvalue, uint8_t len);
///@endcond
/** @} */
/** @} */

END_DECLS

#endif  /* _PROVISION_PROVISIONER_H_ */
