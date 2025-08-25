/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL7005_KEY_MANAGEMENT_HMAC_H_
#define _RL7005_KEY_MANAGEMENT_HMAC_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup KEY_MANAGEMENT_HMAC
  * @brief KEY_MANAGEMENT_HMAC driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup KEY_MANAGEMENT_HMAC_Register_Definitions KEY_MANAGEMENT_HMAC Register Definitions
  * @{
  */

/** @defgroup KM_HMAC_KEY_SEL_NUMBER
  * @brief Key Selection Number Register
  * @{
  */
#define KM_HMAC_BIT_SET_XTS_EN        ((u32)0x00000001 << 8)           /*!< R/W 0x0  Only available when SUPPORT_XTS_MODE = 1. When this bit is set to 1, the trigger will write the selected key to xts_key2. Otherwise, the trigger will write the selected key to key/xts_key1.*/
#define KM_HMAC_MASK_KEY_SEL_NUMBER   ((u32)0x0000003F << 0)           /*!< R/W 0x0  The key number determines which key to use in the key setting. HW keys: 0~31, the number can be used is determined by the number of HW keys configured. Hardwired key: 32, if enabled Secure SW key: 33 Shared SW key: 34~ , the number can be used is determined by the number of SW keys configured.*/
#define KM_HMAC_KEY_SEL_NUMBER(x)     (((u32)((x) & 0x0000003F) << 0))
#define KM_HMAC_GET_KEY_SEL_NUMBER(x) ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup KM_HMAC_KEY_SETUP_TRIGGER
  * @brief Key Setup Process Trigger Register
  * @{
  */
#define KM_HMAC_MASK_ALLOW_NSEC_USE_SEC_KEY            ((u32)0x0000001F << 1)           /*!< Sec: R/W
Nsec: RO 0x0  When setting this field to 1, allow non-secure domain to trigger the secure key. Only secure domain can write this bit. The corresponding bits will be numbered from smallest to largest. For example, the number of the secure domain key is 0, 1, 33. Bits 0, 1, 2 control key0, key1, key33 respectively.*/
#define KM_HMAC_ALLOW_NSEC_USE_SEC_KEY(x)              (((u32)((x) & 0x0000001F) << 1))
#define KM_HMAC_GET_ALLOW_NSEC_USE_SEC_KEY(x)          ((u32)(((x >> 1) & 0x0000001F)))
#define KM_HMAC_BIT_KEY_SETUP_TRIGGER_KEY_MANAGER_IDLE ((u32)0x00000001 << 0)           /*!< R/W1T 0x1  Write 1 to trigger key setup process If the selected key is a hardware key, assert hw_key_req and wait for hw_key_ack. The key setup process will not start until hw_key_ack is received. The key managment unit uses the APB interface to pass the key into the engine according to the selected key. The read data represents whether the engine is idle or not. 0: Key setup process ongoing 1: Key setup is completed After triggering the key setup process, read this bit to confirm completion.*/
/** @} */

/** @defgroup KM_HMAC_SECURE_KEY0_x
  * @brief Secure key0 Register[32*x+31:31*x]
  * @{
  */
#define KM_HMAC_MASK_SECURE_KEY0_x   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Secure key0 register[32*x+31:31*x]*/
#define KM_HMAC_SECURE_KEY0_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define KM_HMAC_GET_SECURE_KEY0_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup KM_HMAC_SHARED_KEY0_x
  * @brief Shared key0 Register[32*x+31:32*x]
  * @{
  */
#define KM_HMAC_MASK_SHARED_KEY0_x   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Shared key0 register[32*x+31:32*x]*/
#define KM_HMAC_SHARED_KEY0_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define KM_HMAC_GET_SHARED_KEY0_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup KEY_MANAGEMENT_HMAC_Exported_Types KEY_MANAGEMENT_HMAC Exported Types
  * @{
  */

/** @brief KEY_MANAGEMENT_HMAC Register Declaration
  */

typedef struct {
	__IO uint32_t KM_HMAC_KEY_SEL_NUMBER   ;  /*!< KEY SELECTION NUMBER REGISTER,  Address offset:0x000 */
	__IO uint32_t KM_HMAC_KEY_SETUP_TRIGGER;  /*!< KEY SETUP PROCESS TRIGGER REGISTER,  Address offset:0x004 */
	__IO uint32_t RSVD0[2]                 ;  /*!< Reserved,  Address offset:0x008-0x00F */
	__IO uint32_t KM_HMAC_SECURE_KEY0_x[8] ;  /*!< SECURE KEY0 REGISTER[32*x+31:31*x] Register,  Address offset:0x010-0x02C */
	__IO uint32_t KM_HMAC_SHARED_KEY0_x[8] ;  /*!< SHARED KEY0 REGISTER[32*x+31:32*x] Register,  Address offset:0x030-0x04C */
} KEY_MANAGEMENT_HMAC_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

/* Exported constants --------------------------------------------------------*/
/** @defgroup KEY_MANAGEMENT_HMAC_Exported_Constants KEY_MANAGEMENT_HMAC Exported Constants
  * @{
  */

/** @defgroup KM_HMAC_KEY
 * @brief HMAC supported key ID
 * @{
 */
#define KM_HMAC_KEY_S_IPSEC_KEY1        ((u8)0x00)  /*!< Secure Hardware Key_1, OPT raw address:0x200, size: 256bit */
#define KM_HMAC_KEY_S_IPSEC_KEY2        ((u8)0x01)  /*!< Secure Hardware Key_2, OPT raw address:0x220, size: 256bit */
#define KM_HMAC_KEY_S_IPSEC_KEY3        ((u8)0x02)  /*!< Secure Hardware Key_3, OPT raw address:0x240, size: 256bit */
#define KM_HMAC_KEY_S_IPSEC_KEY4        ((u8)0x03)  /*!< Secure Hardware Key_4, OPT raw address:0x260, size: 256bit */
#define KM_HMAC_KEY_S_SW                ((u8)0x21)  /*!< Secure Software Key, size: 256bit */
#define KM_HMAC_KEY_NS_SW               ((u8)0x22)  /*!< Non-Secure Software Key, size: 256bit */
/** @} */

/** @defgroup KM_HMAC_Shared_KEY
 * @brief HMAC secure hardware Key shared to non-secure
 * @{
 */
#define KM_HMAC_SHARE_SEC_S_IPSEC_KEY1  ((u32)0x00000001 << 1)  /*!< If 1, secure Hardware Key_0 share to non-secure */
#define KM_HMAC_SHARE_SEC_S_IPSEC_KEY2  ((u32)0x00000001 << 2)  /*!< If 1, secure Hardware Key_1 share to non-secure */
#define KM_HMAC_SHARE_SEC_S_IPSEC_KEY3  ((u32)0x00000001 << 3)  /*!< If 1, secure Hardware Key_0 share to non-secure */
#define KM_HMAC_SHARE_SEC_S_IPSEC_KEY4  ((u32)0x00000001 << 4)  /*!< If 1, secure Hardware Key_1 share to non-secure */
#define KM_HMAC_SHARE_SEC_S_SW          ((u32)0x00000001 << 5)	/*!< If 1, secure Software Key_33 share to non-secure */
/** @} */
/** @} */

/** @} */
/** @} */

/* Other constants --------------------------------------------------------*/
#define KM_HMAC_IS_KEY_SEL_NUMBER(ID)  (((ID) == KM_HMAC_KEY_S_IPSEC_KEY1) || \
                                        ((ID) == KM_HMAC_KEY_S_IPSEC_KEY2) || \
                                        ((ID) == KM_HMAC_KEY_S_IPSEC_KEY3) || \
                                        ((ID) == KM_HMAC_KEY_S_IPSEC_KEY4) || \
                                        ((ID) == KM_HMAC_KEY_S_SW) || \
                                        ((ID) == KM_HMAC_KEY_NS_SW))

#define KM_HMAC_IS_SECURE_KEY_NUM(ID)  (((ID) == KM_HMAC_KEY_S_IPSEC_KEY1) || \
                                        ((ID) == KM_HMAC_KEY_S_IPSEC_KEY2) || \
                                        ((ID) == KM_HMAC_KEY_S_IPSEC_KEY3) || \
                                        ((ID) == KM_HMAC_KEY_S_IPSEC_KEY4) || \
                                        ((ID) == KM_HMAC_KEY_S_SW))

#define KM_HMAC_IS_SW_KEY_NUM(ID)      (((ID) == KM_HMAC_KEY_S_SW) || \
                                        ((ID) == KM_HMAC_KEY_NS_SW))
/* MANUAL_GEN_END */

#endif