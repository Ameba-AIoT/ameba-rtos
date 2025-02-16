/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     gmas_def.h
  * @brief    Header file for Gaming Audio Profile.
  * @details  This file defines Gaming Audio Profile related definitions.
  * @author
  * @date
  * @version
  * *************************************************************************************
  */

#ifndef _GMAS_DEF_H_
#define _GMAS_DEF_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>

/**
 * \defgroup    LEA_USE_CASE_GMAP_Def Gaming Audio Profile Definition
 *
 * \brief   Gaming Audio Profile related definitions.
 */

/**
 * \defgroup GMAP_Def_Exported_Macros Gaming Audio Profile Definition Exported Macros
 *
 * \ingroup LEA_USE_CASE_GMAP_Def
 * @{
 */

/**
 * gmas_def.h
 *
 * \defgroup  GMAS_UUID Gaming Audio Service UUID
 *
 * \brief  Define Gaming Audio Service UUID.
 *
 * \ingroup GMAP_Def_Exported_Macros
 * @{
 */
#define GATT_UUID_GMAS                        0x1858    /**< Gaming Audio Service. */

#define GMAS_UUID_CHAR_GMAP_ROLE              0x2C00    /**< GMAP Role. */
#define GMAS_UUID_CHAR_UGG_FEATURES           0x2C01    /**< UGG Features. */
#define GMAS_UUID_CHAR_UGT_FEATURES           0x2C02    /**< UGT Features. */
#define GMAS_UUID_CHAR_BGS_FEATURES           0x2C03    /**< BGS Features. */
#define GMAS_UUID_CHAR_BGR_FEATURES           0x2C04    /**< BGR Features. */
/**
 * End of GMAS_UUID
 * @}
 */

/**
 * gmas_def.h
 *
 * \defgroup  GMAP_ROLE_BIT_VALUE GMAP Role Bit Value
 *
 * \brief  Define GMAP Role characteristic bit value.
 *
 * \ingroup GMAP_Def_Exported_Macros
 * @{
 */
#define GMAP_ROLE_CHAR_BIT_VALUE_UGG    0x01   /**< Bit 0: Unicast Game Gateway (UGG). */
#define GMAP_ROLE_CHAR_BIT_VALUE_UGT    0x02   /**< Bit 1: Unicast Game Terminal (UGT). */
#define GMAP_ROLE_CHAR_BIT_VALUE_BGS    0x04   /**< Bit 2: Broadcast Game Sender (BGS). */
#define GMAP_ROLE_CHAR_BIT_VALUE_BGR    0x08   /**< Bit 3: Broadcast Game Receiver (BGR). */
/**
 * End of GMAP_ROLE_BIT_VALUE
 * @}
 */

/**
 * gmas_def.h
 *
 * \defgroup  UGG_FEATURES_BIT_VALUE UGG Features Bit Value
 *
 * \brief  Define UGG Features characteristic bit value.
 *
 * \ingroup GMAP_Def_Exported_Macros
 * @{
 */
#define UGG_FEATURES_CHAR_BIT_VALUE_UGG_MULTIPLEX_FEATURE_SUP      0x01    /**< Bit 0: UGG Multiplex feature support. */
#define UGG_FEATURES_CHAR_BIT_VALUE_UGG_96K_SOURCE_FEATURE_SUP     0x02    /**< Bit 1: UGG 96 kbps Source feature support. */
#define UGG_FEATURES_CHAR_BIT_VALUE_UGG_MULTISINK_FEATURE_SUP      0x04    /**< Bit 2: UGG Multisink feature support. */
/**
 * End of UGG_FEATURES_BIT_VALUE
 * @}
 */

/**
 * gmas_def.h
 *
 * \defgroup  UGT_FEATURES_BIT_VALUE UGT Features Bit Value
 *
 * \brief  Define UGT Features characteristic bit value.
 *
 * \ingroup GMAP_Def_Exported_Macros
 * @{
 */
#define UGT_FEATURES_CHAR_BIT_VALUE_UGT_SOURCE_FEATURE_SUP         0x01    /**< Bit 0: UGT Source feature support. */
#define UGT_FEATURES_CHAR_BIT_VALUE_UGT_80K_SOURCE_FEATURE_SUP     0x02    /**< Bit 1: UGT 80 kbps Source feature support. */
#define UGT_FEATURES_CHAR_BIT_VALUE_UGT_SINK_FEATURE_SUP           0x04    /**< Bit 2: UGT Sink feature support. */
#define UGT_FEATURES_CHAR_BIT_VALUE_UGT_64K_SOURCE_FEATURE_SUP     0x08    /**< Bit 3: UGT 64 kbps Sink feature support. */
#define UGT_FEATURES_CHAR_BIT_VALUE_UGT_MULTIPLEX_FEATURE_SUP      0x10    /**< Bit 4: UGT Multiplex feature support. */
#define UGT_FEATURES_CHAR_BIT_VALUE_UGT_MULTISINK_FEATURE_SUP      0x20    /**< Bit 5: UGT Multisink feature support. */
#define UGT_FEATURES_CHAR_BIT_VALUE_UGT_MULTISOURCE_FEATURE_SUP    0x40    /**< Bit 6: UGT Multisource feature support. */
/**
 * End of UGT_FEATURES_BIT_VALUE
 * @}
 */

/**
 * gmas_def.h
 *
 * \defgroup  BGS_FEATURES_BIT_VALUE BGS Features Bit Value
 *
 * \brief  Define BGS Features characteristic bit value.
 *
 * \ingroup GMAP_Def_Exported_Macros
 * @{
 */
#define BGS_FEATURES_CHAR_BIT_VALUE_BGS_96K_FEATURE_SUP            0x01    /**< Bit 0: BGS 96 kbps feature support. */
/**
 * End of BGS_FEATURES_BIT_VALUE
 * @}
 */

/**
 * gmas_def.h
 *
 * \defgroup  BGR_FEATURES_BIT_VALUE BGR Features Bit Value
 *
 * \brief  Define BGR Features characteristic bit value.
 *
 * \ingroup GMAP_Def_Exported_Macros
 * @{
 */
#define BGR_FEATURES_CHAR_BIT_VALUE_BGR_MULTISINK_FEATURE_SUP      0x01    /**< Bit 0: BGR Multisink feature support. */
#define BGR_FEATURES_CHAR_BIT_VALUE_BGR_MULTIPLEX_FEATURE_SUP      0x02    /**< Bit 1: BGR Multiplex feature support. */
/**
 * End of BGR_FEATURES_BIT_VALUE
 * @}
 */

/**
 * End of GMAP_Def_Exported_Macros
 * @}
 */

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
