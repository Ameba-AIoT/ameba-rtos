/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     provision_generic.h
  * @brief    Head file for provision generic layer.
  * @details
  * @author   bill
  * @date     2016-2-24
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _PROVISION_GENERIC_H_
#define _PROVISION_GENERIC_H_

/* Add Includes here */
#include "platform_types.h"
#include "gap_scheduler.h"

BEGIN_DECLS

/** @addtogroup Mesh_Prov_Generic
  * @{
  */

/** @defgroup Mesh_Prov_Generic_Exported_Types Exported Types
  * @{
  */
typedef enum
{
    PB_GENERIC_TRANSACTION_START,
    PB_GENERIC_TRANSACTION_ACK,
    PB_GENERIC_TRANSACTION_CONTINUE,
    PB_GENERIC_BEARER_CTRL
} prov_generic_ctrl_format_t;

typedef struct
{
    uint8_t gpcf: 2; //!< Generic Provisioning Control Format @ref prov_generic_ctrl_format_t
    uint8_t info: 6;
} _PACKED4_ prov_generic_ctrl_t;

typedef enum
{
    PB_GENERIC_CB_LINK_OPENED,
    PB_GENERIC_CB_LINK_OPEN_FAILED,
    PB_GENERIC_CB_LINK_CLOSED,
    PB_GENERIC_CB_MSG_ACKED,
    PB_GENERIC_CB_MSG,
    //todo: PB_GENERIC_CB_MSG_TRANSMITTED maybe useful calc ecdh secret after send public key to save time
} prov_generic_cb_type_t;

typedef enum
{
    PROV_BEARER_TYPE_INVALID,
    PROV_BEARER_TYPE_ADV,
    PROV_BEARER_TYPE_GATT,
    PROV_BEARER_TYPE_RPR
} prov_bearer_type_t;

typedef struct
{
    prov_bearer_type_t type;
    union
    {
        gap_sched_link_t link;
    };
} prov_bearer_t;

typedef void (*prov_bearer_cb_pf)(prov_bearer_t bearer, prov_generic_cb_type_t type,
                                  uint8_t *pbuffer,
                                  uint16_t len);
/** @} */
/** @} */

END_DECLS

#endif /* _PROVISION_GENERIC_H_ */
