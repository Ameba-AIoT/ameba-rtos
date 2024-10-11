/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     app_mesh_flags.h
  * @brief    Head file for mesh configuration.
  * @details  Data types and external functions declaration.
  * @author   bill
  * @date     2015-8-27
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _APP_MESH_FLAGS_H
#define _APP_MESH_FLAGS_H

/* Add Includes here */
#include "mesh_config.h"

BEGIN_DECLS

/* mesh protocol 1.1 */
#define F_BT_MESH_1_1_SUPPORT                   (MESH_1_1_SUPPORT && 1)
#define F_BT_MESH_1_1_EPA_SUPPORT               (F_BT_MESH_1_1_SUPPORT && MESH_EPA && 1)
#define F_BT_MESH_1_1_PRB_SUPPORT               (F_BT_MESH_1_1_SUPPORT && MESH_PRB && 1)
#define F_BT_MESH_1_1_MBT_SUPPORT               (F_BT_MESH_1_1_SUPPORT && MESH_MBT && 1)
#define F_BT_MESH_1_1_DFU_SUPPORT               (F_BT_MESH_1_1_SUPPORT && F_BT_MESH_1_1_MBT_SUPPORT && MESH_DFU && 1)
#define F_BT_MESH_1_1_RPR_SUPPORT               (F_BT_MESH_1_1_SUPPORT && MESH_RPR && 1)
#define F_BT_MESH_1_1_SBR_SUPPORT               (F_BT_MESH_1_1_SUPPORT && MESH_SBR && 1)
#define F_BT_MESH_1_1_DF_SUPPORT                (F_BT_MESH_1_1_SUPPORT && MESH_DF && 1)

END_DECLS

#endif /* _APP_MESH_FLAGS_H */
