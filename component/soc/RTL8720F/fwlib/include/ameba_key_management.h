/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _KEY_MANAGEMENT_H_
#define _KEY_MANAGEMENT_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup KEY_MANAGEMENT
  * @brief KEY_MANAGEMENT driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup KEY_MANAGEMENT_Register_Definitions KEY_MANAGEMENT Register Definitions
  * @{
  */

/** @defgroup KM_KEY_CONFIG
  * @brief Key Configuration register
  * @{
  */
#define KM_MASK_ENGINE_SEL_NMBER   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Select a Lalu engine (port number) to setup key The port number is 0~7 Note: If SHARE_BY_MULTI_ENGINE=0, this filed is always 0x0. About layout of Lalu engine (port number) It is depend on your platform implementation. Please contact design integrator if you want to know which port number can be selected.*/
#define KM_ENGINE_SEL_NMBER(x)     (((u32)((x) & 0x00000007) << 16))
#define KM_GET_ENGINE_SEL_NMBER(x) ((u32)(((x >> 16) & 0x00000007)))
#define KM_BIT_SET_XTS_EN          ((u32)0x00000001 << 8)            /*!< R/W 0x0  When this bit is set to 0x0 : The trigger will write the selected key to xts_key1/key[255:0]. 0x1 : The trigger will write the selected key to xts_key2/key[511:256]. Note : Only available when SUPPORT_XTS_MODE = 1.*/
#define KM_MASK_KEY_SEL_NUMBER     ((u32)0x0000003F << 0)            /*!< R/W 0x0  The key number determines which key to use in the key setting. HW keys: 0~31, the number can be used is determined by the number of HW keys configured. Hardwired key: 32, if enabled Secure SW key: 33 Shared SW key: 34~ , the number can be used is determined by the number of SW keys configured.*/
#define KM_KEY_SEL_NUMBER(x)       (((u32)((x) & 0x0000003F) << 0))
#define KM_GET_KEY_SEL_NUMBER(x)   ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup KM_KEY_SETUP_TRIGGER
  * @brief Key Setup Process Trigger Register
  * @{
  */
#define KM_BIT_TRIGGER_ERROR                         ((u32)0x00000001 << 31) /*!< RO 0x0  If the user triggers an undefined key number, port number or an error domain key, the key setup process will not be activated. When the above conditions are encountered, this field will read 1. 0x0 : Trigger success 0x1 : Trigger failed*/
#define KM_BIT_KEY_SETUP_TRIGGER_OR_KEY_MANAGER_IDLE ((u32)0x00000001 << 0)  /*!< R/W1T 0x1  Write 1 to trigger key setup process If the selected key is a hardware key, assert hw_key_req and wait for hw_key_ack. The key setup process will not start until hw_key_ack is received. The key managment unit uses the APB interface to pass the key into the engine according to the selected key. The read data represents whether the engine is idle or not. 0x0 : key setup process ongoing 0x1 : key setup is completed After triggering the key setup process, read this bit to confirm completion.*/
/** @} */

/** @defgroup KM_ALLOW_NSEC_USE_SEC_KEY_LO
  * @brief Allow Nsec Use Sec Key Lo Register
  * @{
  */
#define KM_MASK_ALLOW_NSEC_USE_SEC_KEY_LO   ((u32)0x0000007F << 0)           /*!< Sec: R/W Nsec: RO 0x0  Allow_Nsec_Use_Sec_key [31:0] When set this bit to 1, allow non-secure domain to trigger the secure key. Only secure domain can write this bit. x depends on the number of secure domain keys. The corresponding bits will be numbered from smallest to largest. For example, the number of the secure domain key is 0, 1, 33. Bits 0, 1, 2 control key0, key1, key33 respectively.*/
#define KM_ALLOW_NSEC_USE_SEC_KEY_LO(x)     (((u32)((x) & 0x0000007F) << 0))
#define KM_GET_ALLOW_NSEC_USE_SEC_KEY_LO(x) ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup KM_ALLOW_NSEC_USE_SEC_KEY_HI
  * @brief Allow Nsec Use Sec Key Hi Register
  * @{
  */
#define KM_MASK_ALLOW_NSEC_USE_SEC_KEY_HI   ((u32)0x0000007F << 0)           /*!< Sec: R/W Nsec: RO 0x0  Allow_Nsec_Use_Sec_key [63:32] When set this bit to 1, allow non-secure domain to trigger the secure key. Only secure domain can write this bit. x depends on the number of secure domain keys. The corresponding bits will be numbered from smallest to largest. For example, the number of the secure domain key is 0, 1, 33. Bits 0, 1, 2 control key0, key1, key33 respectively.*/
#define KM_ALLOW_NSEC_USE_SEC_KEY_HI(x)     (((u32)((x) & 0x0000007F) << 0))
#define KM_GET_ALLOW_NSEC_USE_SEC_KEY_HI(x) ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup KM_SECURE_KEY0_x
  * @brief Secure key0 Register[32*x+31:31*x]
  * @{
  */
#define KM_MASK_SECURE_KEY0_x   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Secure key0 register[32*x+31:31*x]*/
#define KM_SECURE_KEY0_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define KM_GET_SECURE_KEY0_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup KM_SHARED_KEY0_x
  * @brief Shared key0 Register[32*x+31:32*x]
  * @{
  */
#define KM_MASK_SHARED_KEY0_x   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Shared key0 register[32*x+31:32*x]*/
#define KM_SHARED_KEY0_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define KM_GET_SHARED_KEY0_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup KM_MUTEX_STATUS
  * @brief Mutex Status Register
  * @{
  */
#define KM_MASK_MUTEX_STATUS   ((u32)0x00000003 << 0)           /*!< RS/W1C 0x0  Behavior of read If the read data is  0x0 : the mutex is idle 0x1 : secure domain lock mutex 0x2 : non-secure domain lock mutex Behavior of write 0x1 : unlock mutex Mutex is a mechanism to prevent two domains from R/W to the same engine at the same time. Notice for user : 1. Any domain acess is invalid when the initial status is 0x0. 2. Any domain wants to use the engine needs to read this first. 3. If read 0x0 means the engine is idle, the engine will automatically lock on the corresponding mutex status. 4. Please write 0x1 to unlock the mutex status after using key management.*/
#define KM_MUTEX_STATUS(x)     (((u32)((x) & 0x00000003) << 0))
#define KM_GET_MUTEX_STATUS(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup KM_MUTEX_OVERWRITE
  * @brief Mutex Overwrite register
  * @{
  */
#define KM_BIT_MUTEX_OVERWRITE ((u32)0x00000001 << 0) /*!< R/W 0x0  Allowing secure domain can overwrite current mutex is 0x1 : enable 0x0 : disable Note: The register is available only for secure master*/
/** @} */

/** @defgroup KM_REVISION_NUM
  * @brief Revision number Register
  * @{
  */
#define KM_MASK_YEAR    ((u32)0x000000FF << 16)           /*!< RO -  Year*/
#define KM_YEAR(x)      (((u32)((x) & 0x000000FF) << 16))
#define KM_GET_YEAR(x)  ((u32)(((x >> 16) & 0x000000FF)))
#define KM_MASK_MAJOR   ((u32)0x000000FF << 8)            /*!< RO -  Major version*/
#define KM_MAJOR(x)     (((u32)((x) & 0x000000FF) << 8))
#define KM_GET_MAJOR(x) ((u32)(((x >> 8) & 0x000000FF)))
#define KM_MASK_MINOR   ((u32)0x000000FF << 0)            /*!< RO -  Minor version*/
#define KM_MINOR(x)     (((u32)((x) & 0x000000FF) << 0))
#define KM_GET_MINOR(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup KEY_MANAGEMENT_Exported_Types KEY_MANAGEMENT Exported Types
  * @{
  */

/** @brief KEY_MANAGEMENT Register Declaration
  */

typedef struct {
	__IO uint32_t KM_KEY_CONFIG               ;  /*!< KEY CONFIGURATION REGISTER,  Address offset:0x000 */
	__IO uint32_t KM_KEY_SETUP_TRIGGER        ;  /*!< KEY SETUP PROCESS TRIGGER REGISTER,  Address offset:0x004 */
	__IO uint32_t KM_ALLOW_NSEC_USE_SEC_KEY_LO;  /*!< ALLOW NSEC USE SEC KEY LO REGISTER,  Address offset:0x008 */
	__IO uint32_t KM_ALLOW_NSEC_USE_SEC_KEY_HI;  /*!< ALLOW NSEC USE SEC KEY HI REGISTER,  Address offset:0x00C */
	__IO uint32_t KM_SECURE_KEY0_x[8]         ;  /*!< SECURE KEY0 REGISTER[32*x+31:31*x] Register,  Address offset:0x010-0x02C */
	__IO uint32_t KM_SHARED_KEY0_x[8]         ;  /*!< SHARED KEY0 REGISTER[32*x+31:32*x] Register,  Address offset:0x030-0x04C */
	__IO uint32_t RSVD0[812]                  ;  /*!< Reserved,  Address offset:0x050-0xCFF */
	__IO uint32_t KM_MUTEX_STATUS             ;  /*!< MUTEX STATUS REGISTER,  Address offset:0xD00 */
	__IO uint32_t KM_MUTEX_OVERWRITE          ;  /*!< MUTEX OVERWRITE REGISTER,  Address offset:0xD04 */
	__IO uint32_t RSVD1[186]                  ;  /*!< Reserved,  Address offset:0xD08-0xFEF */
	__I  uint32_t KM_REVISION_NUM             ;  /*!< REVISION NUMBER REGISTER,  Address offset:0xFF0 */
} KEY_MANAGEMENT_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif

/* Exported Contants --------------------------------------------------------*/

/** @defgroup KEY_MANAGEMENT_Exported_Constants KEY_MANAGEMENT Exported Contants
* @{
*/
/** @defgroup KM_KEY
 * @brief supported key ID
 * @{
 */
#define KM_KEY_S_IPSEC_KEY1        ((u8)0x00)  /*!< Secure Hardware Key_1, OPT raw address:0x200, size: 256bit */
#define KM_KEY_S_IPSEC_KEY2        ((u8)0x01)  /*!< Secure Hardware Key_2, OPT raw address:0x220, size: 256bit */
#define KM_KEY_S_IPSEC_KEY3        ((u8)0x02)  /*!< Secure Hardware Key_3, OPT raw address:0x240, size: 256bit */
#define KM_KEY_S_IPSEC_KEY4        ((u8)0x03)  /*!< Secure Hardware Key_4, OPT raw address:0x260, size: 256bit */
#define KM_KEY_S_RSIP_KEY1         ((u8)0x04)  /*!< Secure Hardware Key_5, OPT raw address:0x2C0, size: 256bit */
#define KM_KEY_S_RSIP_KEY2         ((u8)0x05)  /*!< Secure Hardware Key_6, OPT raw address:0x2E0, size: 256bit */
#define KM_KEY_S_SW1               ((u8)0x21)  /*!< Secure Software Key, size: 256bit */
#define KM_KEY_NS_SW1              ((u8)0x22)  /*!< Secure Software Key, size: 256bit */
/** @} */

/** @defgroup KM_Shared_KEY
 * @brief secure hardware Key shared to non-secure
 * @{
 */
#define KM_SHARE_SEC_S_IPSEC_KEY1    ((u32)0x00000001 << 0)  /*!< If 1, secure Hardware Key_1 share to non-secure */
#define KM_SHARE_SEC_S_IPSEC_KEY2    ((u32)0x00000001 << 1)  /*!< If 1, secure Hardware Key_2 share to non-secure */
#define KM_SHARE_SEC_S_IPSEC_KEY3    ((u32)0x00000001 << 2)  /*!< If 1, secure Hardware Key_3 share to non-secure */
#define KM_SHARE_SEC_S_IPSEC_KEY4    ((u32)0x00000001 << 3)  /*!< If 1, secure Hardware Key_4 share to non-secure */
#define KM_SHARE_SEC_S_RSIP_KEY1     ((u32)0x00000001 << 4)  /*!< If 1, secure Hardware Key_5 share to non-secure */
#define KM_SHARE_SEC_S_RSIP_KEY2     ((u32)0x00000001 << 5)  /*!< If 1, secure Hardware Key_6 share to non-secure */
#define KM_SHARE_SEC_S_SW1           ((u32)0x00000001 << 6)	 /*!< If 1, secure Software Key_33 share to non-secure */
/** @} */
/** @} */

/** @} */
/** @} */

/* Other constants --------------------------------------------------------*/
#define KM_AES_ENGINE                    (0)
#define KM_SHA_ENGINE                    (1)
#define KM_MUTEX_TIMEOUT                 ((u32) 1000000)

#define KM_IS_KEY_SEL_NUMBER(ID)  (((ID) == KM_KEY_S_IPSEC_KEY1) || \
                                       ((ID) == KM_KEY_S_IPSEC_KEY2) || \
                                       ((ID) == KM_KEY_S_IPSEC_KEY3) || \
                                       ((ID) == KM_KEY_S_IPSEC_KEY4) || \
                                       ((ID) == KM_KEY_S_RSIP_KEY1) || \
                                       ((ID) == KM_KEY_S_RSIP_KEY2) || \
                                       ((ID) == KM_KEY_S_SW1) || \
                                       ((ID) == KM_KEY_NS_SW1))

#define KM_IS_SECURE_KEY_NUM(ID)  (((ID) == KM_KEY_S_IPSEC_KEY1) || \
                                       ((ID) == KM_KEY_S_IPSEC_KEY2) || \
                                       ((ID) == KM_KEY_S_IPSEC_KEY3) || \
                                       ((ID) == KM_KEY_S_IPSEC_KEY4) || \
                                       ((ID) == KM_KEY_S_RSIP_KEY1) || \
                                       ((ID) == KM_KEY_S_RSIP_KEY2) || \
                                       ((ID) == KM_KEY_S_SW1))

#define KM_IS_SW_KEY_NUM(ID)           (((ID) == KM_KEY_S_SW1) || \
                                       ((ID) == KM_KEY_NS_SW1))



/**
 * @brief Return different key management register addresses according to the state of secure
 * @retval LALU_KEY_REG_BASE or LALU_KEY_REG_BASE_S
 */
_LONG_CALL_ KEY_MANAGEMENT_TypeDef *crypto_km_get_addr(void);

/**
 * @brief  Set software key value
 * @param  key_id: value of KM_KEY in ameba_key_management.h
 * @param  key_len_bits:
 *            @arg KEY_BIT_128
 *            @arg KEY_BIT_192
 *            @arg KEY_BIT_256
 * @param  key_addr: address to software key array
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_km_set_sw_key(u8 key_id, u32 key_len_bits, const u8 *key_addr);

/**
 * @brief  Control whether security keys can be shared with non-secure world.
 *         Note that if sharing is enabled, non-secure code can only trigger the use of the key, but cannot read or write.
 * @param  key_id: value of KM_KEY in ameba_key_management.h
 * @param  is_share: 1: share
 * 			         0: not share (secure only)
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_km_share_secure_key(const u8 key_id, const u8 is_share);

/**
 * @brief  Trigger key load to AES or SHA engine
 * @param  engine_id:
 *            @arg KM_AES_ENGINE
 *            @arg KM_SHA_ENGINE
 * @param  key_id: value of KM_KEY in ameba_key_management.h
 * @param  key_len_bits:
 *            @arg KEY_BIT_128
 *            @arg KEY_BIT_192
 *            @arg KEY_BIT_256
 * @retval 0: success
 * @retval Other: error code. refer to CRYPTO_Process_Status in ameba_crypto_api.h
 */
_LONG_CALL_ int crypto_km_load_key(u8 engine_id, u8 key_id, uint32_t key_len_bits);

/* MbedTLS HW ALT */
#define KM_AES_KEY_S_SW1          KM_KEY_S_SW1
#define KM_AES_KEY_NS_SW1         KM_KEY_NS_SW1
#define KM_AES_KEY_S_SW2          0
#define KM_AES_KEY_NS_SW2         0

__inline__ int crypto_aes_set_sw_key(u8 key_id, u32 key_len_bits, const u8 *key_addr)
{
	return crypto_km_set_sw_key(key_id, key_len_bits, key_addr);
}
#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif