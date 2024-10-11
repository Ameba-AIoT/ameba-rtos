/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     generic_types.h
* @brief    Head file for types used by generic models.
* @details  Data types and external functions declaration.
* @author   bill
* @date     2017-12-22
* @version  v1.0
* *************************************************************************************
*/

/* Define to prevent recursive inclusion */
#ifndef _GENERIC_TYPES_H
#define _GENERIC_TYPES_H

#include "platform_types.h"

BEGIN_DECLS

/**
 * @addtogroup GENERIC_TYPES
 * @{
 */

/**
 * @defgroup GENERIC_TYPES_TYPES Generic Types
 * @brief Error code for models
 * @{
 */
enum
{
    GENERIC_STAT_SUCCESS,
    GENERIC_STAT_CANNOT_SET_RANGE_MIN,
    GENERIC_STAT_CANNOT_SET_RANGE_MAX
} _SHORT_ENUM_;
typedef uint8_t generic_stat_t;

enum
{
    MESH_MSG_STAT_SUCCESS,
    MESH_MSG_STAT_INVALID_ADDR,
    MESH_MSG_STAT_INVALID_MODEL,
    MESH_MSG_STAT_INVALID_APP_KEY_INDEX,
    MESH_MSG_STAT_INVALID_NET_KEY_INDEX,
    MESH_MSG_STAT_INSUFFICIENT_RESOURCES,
    MESH_MSG_STAT_KEY_INDEX_ALREADY_STORED,
    MESH_MSG_STAT_INVALID_PUB_PARAMS,
    MESH_MSG_STAT_NOT_SUB_MODEL, //8, TODO: What is not a sub model?
    MESH_MSG_STAT_STORAGE_FAIL,
    MESH_MSG_STAT_FEATURE_NOT_SUPPORTED,
    MESH_MSG_STAT_CANNOT_UPDATE,
    MESH_MSG_STAT_CANNOT_REMOVE,
    MESH_MSG_STAT_CANNOT_BIND,
    MESH_MSG_STAT_TEMP_UNABLE_CHANGE_STATE,
    MESH_MSG_STAT_CANNOT_SET,
    MESH_MSG_STAT_UNSPECIFIED_ERROR, //16
    MESH_MSG_STAT_INVALID_BINDING,
    MESH_MSG_STAT_INVALID_PATH_ENTRY,
    MESH_MSG_STAT_CANNOT_GET,
    MESH_MSG_STAT_OBSOLETE_INFORMATION,
    MESH_MSG_STAT_INVALID_BEARER,
} _SHORT_ENUM_;
typedef uint8_t mesh_msg_stat_t;


#define MODEL_SUCCESS               0 //!< model data process success
#define MODEL_STOP_TRANSITION       1 //!< stop transition after model data process
/** @} */
/** @} */

END_DECLS

#endif /* _GENERIC_TYPES_H */
