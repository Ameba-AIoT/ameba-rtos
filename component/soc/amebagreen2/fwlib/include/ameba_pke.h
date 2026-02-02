/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PKE_H_
#define _AMEBA_PKE_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PKE
  * @brief PKE driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PKE_Register_Definitions PKE Register Definitions
  * @{
  */

/** @defgroup PKE_CONTROL
  * @brief PKE Control Register
  * @{
  */
#define PKE_BIT_CONTROL_HASH_READY        ((u32)0x00000001 << 31)          /*!< R/W 0x0  Write 1 to inform PKE that hash value used in SIMPLIFY ECDSA/EDDSA are ready*/
#define PKE_BIT_CONTROL_START             ((u32)0x00000001 << 30)          /*!< R/WAC 0x0  Write 1 to start PKE (cleared automatically by PKE)*/
#define PKE_BIT_CONTROL_OTHER_FUNCTION    ((u32)0x00000001 << 27)          /*!< R/W 0x0  Other functions (not RSA/ECC) enable bit 1: enabled 0: disabled (RSA: modular exponentiation; ECC: scalar multiplication)*/
#define PKE_BIT_CONTROL_WORD_SWAP         ((u32)0x00000001 << 26)          /*!< R/W 0x0  Endian word swap enable (only supported in MMEM)*/
#define PKE_BIT_CONTROL_BYTE_SWAP         ((u32)0x00000001 << 25)          /*!< R/W 0x0  Endian byte swap enable (only supported in MMEM)*/
#define PKE_BIT_CONTROL_PRECOMPUTATION    ((u32)0x00000001 << 24)          /*!< R/W 0x0  R^2 mod N ready (1 means not need to calculate the R^2 mod N, already stored by user) For RSA , R^2 mod N ready and DPA defense can't be enabled at the same time. Valid only when bit[27] == 0*/
#define PKE_MASK_CONTROL_PRECISE_LENGTH   ((u32)0x0000FFFF << 8)           /*!< R/W 0x0  Precise RSA/ECC length in bits Caution: when preforming ecc point operation under binary field, there are situation which width of modulo is larger than ecc length. For example, width of f(x) of sect163r2 is 164, larger than ECC length 163. In this situation, just fill in Precise ECC length 163.*/
#define PKE_CONTROL_PRECISE_LENGTH(x)     (((u32)((x) & 0x0000FFFF) << 8))
#define PKE_GET_CONTROL_PRECISE_LENGTH(x) ((u32)(((x >> 8) & 0x0000FFFF)))
#define PKE_MASK_CONTROL_MODE_MAIN        ((u32)0x00000003 << 6)           /*!< R/W 0x0  2'b00: ECC Prime field 2'b01: ECC Binary field 2'b10: RSA 2'b11: Edwards or Montgomery Curves*/
#define PKE_CONTROL_MODE_MAIN(x)          (((u32)((x) & 0x00000003) << 6))
#define PKE_GET_CONTROL_MODE_MAIN(x)      ((u32)(((x >> 6) & 0x00000003)))
#define PKE_BIT_CONTROL_MODE_CURVE        ((u32)0x00000001 << 5)           /*!< R/W 0x0  1'b0: Edward Curves 1'b1: Montgomery Curves*/
#define PKE_BIT_CONTROL_USE_LFSR          ((u32)0x00000001 << 4)           /*!< R/W 0x0  1: use internal LFSR0, and do not use internal LFSR. If user does not use internal LFSR, user needs to initial a 32-bit random number (can't be 0) to MMEM offset 0x600 (RSA3072), or MMEM offser 0x800 (RSA4096) or MMEM offset 0x380 (ECC). And if configured to ECC, please also clear the upper MMEM offset 0x381 to 0x3bf to 0.*/
#define PKE_BIT_CONTROL_TIMING_ATTACK     ((u32)0x00000001 << 3)           /*!< R/W 0x0  1: Timing attack defense enable 0: Timing attack defense disable*/
#define PKE_BIT_CONTROL_DPA_DEFENSE       ((u32)0x00000001 << 2)           /*!< R/W 0x0  1: DPA defense enable 0: DPA defense disable*/
#define PKE_BIT_CONTROL_IMEM_READ         ((u32)0x00000001 << 1)           /*!< R/W 0x0  1: Enable IMEM read (or else, the IMEM can only be read when bit[30] = 1) 0: IMEM can't be read*/
#define PKE_BIT_CONTROL_CAL_PRECOMPUTE    ((u32)0x00000001 << 0)           /*!< R/W 0x0  When using RSA/ECC (bit 27 == 0), if this bit is set, PKE will calculate R ^ 2 mod N, n[63:0] ^ (-1) mod r only*/
/** @} */

/** @defgroup PKE_STATUS
  * @brief PKE Status Register
  * @{
  */
#define PKE_BIT_STATUS_HASH_NEED ((u32)0x00000001 << 9)           /*!< RO 0x0  SIMPLIFY_ECDSA/EdDSA needs hash value*/
#define PKE_BIT_STATUS_IDLE      ((u32)0x00000001 << 8)           /*!< RO 0x1  PKE idle*/
#define PKE_MASK_STATUS          ((u32)0x0000007F << 1)           /*!< R/W0C 0x0  7'h1: prime check error(for modular number) 7'h2: R^2 mod N failed 7'h4: ECC odd point (Y = 0) (Montgomery curve will not check the Y value) 7'h5: RSA not supported 7'h6: check version error 7'h7: no such function 7'h8: ECC Z = 0 7'h10: Modular Inverse failed 7'h20: N < input message for RSA 7'h40: No valid exponent*/
#define PKE_STATUS(x)            (((u32)((x) & 0x0000007F) << 1))
#define PKE_GET_STATUS(x)        ((u32)(((x >> 1) & 0x0000007F)))
#define PKE_BIT_STATUS_FINISH    ((u32)0x00000001 << 0)           /*!< R/W0C 0x0  PKE operation finish*/
/** @} */

/** @defgroup PKE_INTERRUPT
  * @brief PKE Interrupt Mask Register
  * @{
  */
#define PKE_BIT_INTERRUPT_SIMPLIFY_HASH_MASK ((u32)0x00000001 << 8)           /*!< R/W 0x0  Wait for SIMPLIFY_ECDSA_EDDSA hash value 0: interrupt masked 1: interrupt not masked*/
#define PKE_MASK_INTERRUPT_STATUS_MASK       ((u32)0x000000FF << 0)           /*!< R/W 0x0  Interrupt mask for interrupt in PKE status register bit[7:0] 8'h00: interrupt masked 8'hff: interrupt not masked Caution: valid value of this field is only 8'h00 or 8'hff*/
#define PKE_INTERRUPT_STATUS_MASK(x)         (((u32)((x) & 0x000000FF) << 0))
#define PKE_GET_INTERRUPT_STATUS_MASK(x)     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup PKE_SWAP
  * @brief Base addess of word swap Register
  * @{
  */
#define PKE_MASK_KEY_NUM     ((u32)0x00000007 << 17)           /*!< R/W 0x0  Use which key (binary encoding), key0 ~ key7*/
#define PKE_KEY_NUM(x)       (((u32)((x) & 0x00000007) << 17))
#define PKE_GET_KEY_NUM(x)   ((u32)(((x >> 17) & 0x00000007)))
#define PKE_BIT_KEY_PORT_EN  ((u32)0x00000001 << 16)           /*!< R/W 0x0  1: Enable hardware key port 0: Do not enable hardware key port*/
#define PKE_MASK_SWAP_BASE   ((u32)0x000001FF << 2)            /*!< R/W 0x0  Word swap base address in word (32 bits)*/
#define PKE_SWAP_BASE(x)     (((u32)((x) & 0x000001FF) << 2))
#define PKE_GET_SWAP_BASE(x) ((u32)(((x >> 2) & 0x000001FF)))
/** @} */

/** @defgroup PKE_VERSION
  * @brief Version Number Register
  * @{
  */
#define PKE_MASK_VERSION   ((u32)0xFFFFFFFF << 0)           /*!< RO Depends on Project  32-bit code update date or version number , such as 32'h20211015*/
#define PKE_VERSION(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PKE_GET_VERSION(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PKE_LFSR_SEED
  * @brief PKESeeding Register
  * @{
  */
#define PKE_MASK_SEED   ((u32)0xFFFFFFFF << 0)           /*!< WO   Seed of internal LFSRs*/
#define PKE_SEED(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define PKE_GET_SEED(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PKE_MUTEX
  * @brief PKE Mutex Register
  * @{
  */
#define PKE_MASK_MUTEX_STATUS   ((u32)0x00000003 << 0)           /*!< R/W0C 0x0  If read 0, lock mutex. If not 0, the engine is being used. 2'b01: secure domain lock mutex 2'b10: non-secure domain lock mutex Write 0 to unlock mutex*/
#define PKE_MUTEX_STATUS(x)     (((u32)((x) & 0x00000003) << 0))
#define PKE_GET_MUTEX_STATUS(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup PKE_MUTEX_OVERWRITE
  * @brief PKE Mutex Overwrite Register
  * @{
  */
#define PKE_BIT_MUTEX_OVERWRITE ((u32)0x00000001 << 0) /*!< R/W 0x0  Allows secure domain overwrite mutex 1: Enable 0: Disable*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup PKE_Exported_Types PKE Exported Types
  * @{
  */

/** @brief PKE Register Declaration
  */

typedef struct {
	__IO uint32_t PKE_CONTROL        ;  /*!< PKE CONTROL REGISTER,  Address offset:0x000 */
	__IO uint32_t RSVD0              ;  /*!< Reserved,  Address offset:0x004-0x007 */
	__IO uint32_t PKE_STATUS         ;  /*!< PKE STATUS REGISTER,  Address offset:0x008 */
	__IO uint32_t PKE_INTERRUPT      ;  /*!< PKE INTERRUPT MASK REGISTER,  Address offset:0x00C */
	__IO uint32_t RSVD1              ;  /*!< Reserved,  Address offset:0x010-0x013 */
	__IO uint32_t PKE_SWAP           ;  /*!< BASE ADDESS OF WORD SWAP REGISTER,  Address offset:0x014 */
	__IO uint32_t RSVD2              ;  /*!< Reserved,  Address offset:0x018-0x01B */
	__I  uint32_t PKE_VERSION        ;  /*!< VERSION NUMBER REGISTER,  Address offset:0x01C */
	__IO uint32_t RSVD3[3]           ;  /*!< Reserved,  Address offset:0x020-0x02B */
	__O  uint32_t PKE_LFSR_SEED      ;  /*!< PKESEEDING REGISTER,  Address offset:0x02C */
	__IO uint32_t RSVD4              ;  /*!< Reserved,  Address offset:0x030-0x033 */
	__IO uint32_t PKE_MUTEX          ;  /*!< PKE MUTEX REGISTER,  Address offset:0x034 */
	__IO uint32_t PKE_MUTEX_OVERWRITE;  /*!< PKE MUTEX OVERWRITE REGISTER,  Address offset:0x038 */
} PKE_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your definition here

typedef enum {
	PKE_ECP_CURVE_NONE = 0,       /*!< Curve not defined. */
	PKE_ECP_CURVE_SECP192R1,      /*!< Domain parameters for the 192-bit curve defined by FIPS 186-4 and SEC1. */
	PKE_ECP_CURVE_SECP224R1,      /*!< Domain parameters for the 224-bit curve defined by FIPS 186-4 and SEC1. */
	PKE_ECP_CURVE_SECP256R1,      /*!< Domain parameters for the 256-bit curve defined by FIPS 186-4 and SEC1. */
	PKE_ECP_CURVE_SECP384R1,      /*!< Domain parameters for the 384-bit curve defined by FIPS 186-4 and SEC1. */
	PKE_ECP_CURVE_NULL,           /*!< 521-bit curve not support */
	PKE_ECP_CURVE_BP256R1,        /*!< Domain parameters for 256-bit Brainpool curve. */
	PKE_ECP_CURVE_BP384R1,        /*!< Domain parameters for 384-bit Brainpool curve. */
	PKE_ECP_CURVE_BP512R1,        /*!< Domain parameters for 512-bit Brainpool curve. */
	PKE_ECP_CURVE_CURVE25519,     /*!< Domain parameters for Curve25519. */
	PKE_ECP_CURVE_SECP192K1,      /*!< Domain parameters for 192-bit "Koblitz" curve. */
	PKE_ECP_CURVE_SECP224K1,      /*!< Domain parameters for 224-bit "Koblitz" curve. */
	PKE_ECP_CURVE_SECP256K1,      /*!< Domain parameters for 256-bit "Koblitz" curve. */
	PKE_ECP_CURVE_CURVE448,       /*!< Domain parameters for Curve448. */
	/* The curve id above part is consistent with mbedtls */
	PKE_ECP_CURVE_ED25519,        /*!< Supported by default rom driver */
	/* The curve parameter above part is in the rom */
	PKE_ECP_CURVE_ED448,          /*!< Not supported by default, user can add support by themselves */
} pke_ecp_curve_id;

/**
* @brief PKE ECP Group
*/

typedef struct pke_ecp_group {
	pke_ecp_curve_id curve_id;    /*!< value in pke_ecp_curve_id */
	uint32_t precise_bits;        /*!< ecc precise in bits */
	uint8_t curve_field;          /*!< PKE_MODE_MAIN_ECC_PRIME, PKE_MODE_MAIN_ECC_BINARY or PKE_MODE_MAIN_EDWARDS */
	uint8_t curve_is_Montgomery;  /*!< 1 for Montgomery curve (PKE_MASK_CONTROL_MODE_MAIN must 2b'11), others is 0 */
	uint8_t A_size;               /*!< Size of parameter A in byte */
	uint8_t B_size;               /*!< Size of parameter B in byte */
	uint8_t P_size;               /*!< Size of parameter P in byte */
	uint8_t N_size;               /*!< Size of parameter N in byte */
	uint8_t Gx_size;              /*!< Size of parameter Gx in byte */
	uint8_t Gy_size;              /*!< Size of parameter Gy in byte */
	uint8_t Gz_size;              /*!< Size of parameter Gy in byte */
	uint8_t *A_p;                 /*!< Pointer to parameter A */
	uint8_t *B_p;                 /*!< Pointer to parameter B */
	uint8_t *P_p;                 /*!< Pointer to parameter P */
	uint8_t *N_p;                 /*!< Pointer to parameter N */
	uint8_t *Gx_p;                /*!< Pointer to parameter Gx */
	uint8_t *Gy_p;                /*!< Pointer to parameter Gy */
	uint8_t *Gz_p;                /*!< Pointer to parameter Gy */
} pke_ecp_group;

/**
* @brief PKE ECP Point
*/

typedef struct pke_ecp_point {
	uint8_t is_inited;       /*!< The value set to 1 in pke_ecp_point_init. Other value set to 0 */
	uint8_t *X_p;             /*!< The X coordinate of the ECP point. */
	uint8_t X_size;          /*!< The X coordinate size in byte */
	uint8_t *Y_p;             /*!< The X coordinate of the ECP point. */
	uint8_t Y_size;          /*!< The X coordinate size in byte */
	uint8_t *Z_p;             /*!< The X coordinate of the ECP point. */
	uint8_t Z_size;          /*!< The X coordinate size in byte */
} pke_ecp_point;

/** @} */

/* Exported Constants --------------------------------------------------------*/

/** @defgroup PKE_Exported_Constants PKE Exported Constants
* @{
*/

/** @defgroup PKE_Timeout
  * @{
  */
#define PKE_MUTEX_TIMEOUT                          ((u32) 1000000)
#define PKE_TIMEOUT_CNT                            (0x100000)
/** @} */

/** @defgroup PKE_ECDSA_Private_Key_Selection
  * @{
  */

/*
 * otp raw addr 0x280 -> hw_key1(256bit)
 * otp raw addr 0x2a0 -> hw_key2(256bit)
 * pke_key_port0 = {hw_key2 , hw_key1 } (512bit)
 * pke_key_port1 = {256bit 0 , hw_key2 } (512bit)
 */
#define PKE_ECDSA_PRIV_KEY_HW_0                    (0)
#define PKE_ECDSA_PRIV_KEY_HW_1                    (1)
#define PKE_ECDSA_PRIV_KEY_SW                      (2)
#define IS_PKE_ECDSA_PRIV_KEY(ID) (((ID) == PKE_ECDSA_PRIV_KEY_HW_0) || \
                                   ((ID) == PKE_ECDSA_PRIV_KEY_HW_1) || \
                                   ((ID) == PKE_ECDSA_PRIV_KEY_SW))

#define IS_PKE_ECDSA_SECURE_HW_KEY(ID) (((ID) == PKE_ECDSA_PRIV_KEY_HW_0) || \
                                        ((ID) == PKE_ECDSA_PRIV_KEY_HW_1))
/** @} */

/* PKE ECC uncommon curve support */

/** @defgroup PKE_Error_Code

  * @{
  */

/* PKE error code: status register bit[7:1] */
#define PKE_STATUS_PRIME_CHK_ERR                   (0x1)
#define PKE_STATUS_R2MOD_N_FAILED              	   (0x2)
#define PKE_STATUS_ECC_ODD_POINT                   (0x4)
#define PKE_STATUS_INVALID_FUNCTION                (0x7)
#define PKE_STATUS_ECC_Z                           (0x8)
#define PKE_STATUS_EDDSA_FAIL                      (0x9)
#define PKE_STATUS_MOD_INV_FALIED                  (0x10)
#define PKE_STATUS_RSA_INVALID_MESSAGE             (0x20)
#define PKE_STATUS_NO_VALID_EXP                    (0x40)
/** @} */

/** @defgroup PKE_Mode
  * @{
  */

/* PKE mode (PKE_Control bit[7:6]) */
#define PKE_MODE_MAIN_ECC_PRIME                    (0x0)
#define PKE_MODE_MAIN_ECC_BINARY                   (0x1)
#define PKE_MODE_MAIN_RSA                          (0x2)
#define PKE_MODE_MAIN_EDWARDS_MONTGOMERY           (0x3)
/** @} */

/** @defgroup PKE_Engine_Memory
  * @{
  */
#define PKE_MMEM_BASE                              ((int)pke_get_pke_addr() + 0x10000)
#define PKE_TMEM_BASE                              ((int)pke_get_pke_addr() + 0x20000)
/** @} */

/** @defgroup PKE_RSA
  * @{
  */
#define PKE_RSA_PARAMETER_SIZE                     (0x200)                 /*!< Max: RSA-4096 */
#define PKE_RSA_PARAMETER_ADDR_N                   ((void *)(PKE_MMEM_BASE + 0x0))   /*!< RSA Modulus */
#define PKE_RSA_PARAMETER_ADDR_E                   ((void *)(PKE_MMEM_BASE + 0x200)) /*!< RSA Exponent */
#define PKE_RSA_PARAMETER_ADDR_A                   ((void *)(PKE_MMEM_BASE + 0x400)) /*!< RSA Message */
#define PKE_RSA_PARAMETER_ADDR_RES                 ((void *)(PKE_TMEM_BASE)) /*!< RSA Result */

#define PKE_RSA_WITNESS_N                          ((void *)(PKE_MMEM_BASE + 0x0))
#define PKE_RSA_WITNESS_A                          ((void *)(PKE_MMEM_BASE + 0x400))
#define PKE_RSA_WITNESS_RES                        ((void *)(PKE_TMEM_BASE))

#define PKE_RSA_PRIME_MUL_P                        ((void *)(PKE_MMEM_BASE + 0x0))
#define PKE_RSA_PRIME_MUL_Q                        ((void *)(PKE_MMEM_BASE + 0x180))
#define PKE_RSA_PRIME_MUL_RES                      ((void *)(PKE_TMEM_BASE))
/** @} */

/** @defgroup PKE_ECC
  * @{
  */
#define PKE_ECC_PARAMETER_SIZE                     (0x40)                  /*!< Max: ECC 512 bits */
#define PKE_ECC_PARAMETER_ADDR_P                   ((void *)(PKE_MMEM_BASE + 0x0))   /*!< ECC Curve parameter Modulus */
#define PKE_ECC_PARAMETER_ADDR_E                   ((void *)(PKE_MMEM_BASE + 0x40))  /*!< ECC Exponent */
#define PKE_ECC_PARAMETER_ADDR_A                   ((void *)(PKE_MMEM_BASE + 0x80))  /*!< ECC Curve parameter a */
#define PKE_ECC_PARAMETER_ADDR_B                   ((void *)(PKE_MMEM_BASE + 0x100)) /*!< ECC Curve parameter b / d */
#define PKE_ECC_PARAMETER_ADDR_G_x                 ((void *)(PKE_MMEM_BASE + 0x140)) /*!< ECC Curve base point x */
#define PKE_ECC_PARAMETER_ADDR_G_y                 ((void *)(PKE_MMEM_BASE + 0x180)) /*!< ECC Curve base point y */
#define PKE_ECC_PARAMETER_ADDR_G_z                 ((void *)(PKE_MMEM_BASE + 0x1C0)) /*!< ECC Curve base point z, set to 1 */
#define PKE_ECC_PARAMETER_ADDR_RES_x               ((void *)(PKE_TMEM_BASE))         /*!< ECC Result x */
#define PKE_ECC_PARAMETER_ADDR_RES_y               ((void *)(PKE_TMEM_BASE + 0x40))  /*!< ECC Result y */
#define PKE_ECC_PARAMETER_ADDR_N                   ((void *)(PKE_MMEM_BASE + 0x400)) /*!< ECC Curve parameter order */
/** @} */

/** @defgroup PKE_Simplified_ECDSA_EdDSA
  * @{
  */
#define PKE_ECC_TMEM_FUNCTION_ENTRY                ((void *)(PKE_TMEM_BASE + 0x0))   /*!< Function entries */
#define PKE_ECC_TMEM_ADDR_P                        ((void *)(PKE_TMEM_BASE + 0x80))  /*!< Simplified ECDSA/EdDSA curve modulus */
#define PKE_ECC_TMEM_ADDR_E                        ((void *)(PKE_TMEM_BASE + 0xC0))  /*!< Simplified ECDSA/EdDSA private key */
#define PKE_ECC_TMEM_ADDR_A                        ((void *)(PKE_TMEM_BASE + 0x100)) /*!< Simplified ECDSA/EdDSA curve parameter a */
#define PKE_ECC_TMEM_ADDR_B                        ((void *)(PKE_TMEM_BASE + 0x140)) /*!< Simplified ECDSA curve parameter b / d */
#define PKE_ECC_TMEM_ADDR_G_x                      ((void *)(PKE_TMEM_BASE + 0x180)) /*!< Simplified ECDSA/EdDSA curve base point x */
#define PKE_ECC_TMEM_ADDR_G_y                      ((void *)(PKE_TMEM_BASE + 0x1C0)) /*!< Simplified ECDSA/EdDSA curve base point y */
#define PKE_ECC_TMEM_ADDR_G_z                      ((void *)(PKE_TMEM_BASE + 0x200)) /*!< Simplified ECDSA/EdDSA curve point z, set to 1 */
#define PKE_ECC_TMEM_ADDR_N                        ((void *)(PKE_TMEM_BASE + 0x300)) /*!< Simplified ECDSA/EdDSA curve order */
#define PKE_ECC_TMEM_ADDR_HASH                     ((void *)(PKE_TMEM_BASE + 0x380)) /*!< Simplified ECDSA: hash message, EdDSA hashed private key */
/** @} */

/** @defgroup PKE_Simplified_ECDSA
  * @{
  */
#define PKE_ECC_TMEM_ADDR_ECDSA_HASH_LEN           ((void *)(PKE_TMEM_BASE + 0x7C4)) /*!< Simplified ECDSA message hash length in bit */
#define PKE_ECC_TMEM_ADDR_ECDSA_RAND               ((void *)(PKE_TMEM_BASE + 0x2C0)) /*!< Simplified ECDSA rand num, for signing */
#define PKE_ECC_TMEM_ADDR_ECDSA_SIGN_R             ((void *)(PKE_TMEM_BASE + 0x340)) /*!< Simplified ECDSA signing result R */
#define PKE_ECC_TMEM_ADDR_ECDSA_SIGN_S             ((void *)(PKE_TMEM_BASE + 0x3C0)) /*!< Simplified ECDSA signing result S */
#define PKE_ECC_TMEM_ADDR_ECDSA_PUBKEY_x           ((void *)(PKE_TMEM_BASE + 0x240)) /*!< Simplified ECDSA public key x */
#define PKE_ECC_TMEM_ADDR_ECDSA_PUBKEY_y           ((void *)(PKE_TMEM_BASE + 0x280)) /*!< Simplified ECDSA public key y */
#define PKE_ECC_TMEM_ADDR_ECDSA_VERIFY_RQ          ((void *)(PKE_TMEM_BASE + 0x540)) /*!< Simplified ECDSA rQ for verify */
/** @} */

/** @defgroup PKE_Simplified_EdDSA
  * @{
  */
#define PKE_ECC_TMEM_ADDR_EdDSA_PUBKEY             ((void *)(PKE_TMEM_BASE + 0x540)) /*!< Simplified EdDSA encoded public key */
#define PKE_ECC_TMEM_ADDR_EdDSA_SIGN_MSG           ((void *)(PKE_TMEM_BASE + 0x580)) /*!< Simplified EdDSA sign message */
#define PKE_ECC_TMEM_ADDR_EdDSA_SIGN_R             ((void *)(PKE_TMEM_BASE + 0x640)) /*!< Simplified EdDSA signing result R */
#define PKE_ECC_TMEM_ADDR_EdDSA_SIGN_S             ((void *)(PKE_TMEM_BASE + 0x6C0)) /*!< Simplified EdDSA signing result S */
#define PKE_ECC_TMEM_ADDR_EdDSA_VERIFY_MSG         ((void *)(PKE_TMEM_BASE + 0x340)) /*!< Simplified EdDSA verify message */
#define PKE_ECC_TMEM_ADDR_EdDSA_VERIFY_B_x         ((void *)(PKE_TMEM_BASE + 0x440)) /*!< Simplified EdDSA verify B_x value */
#define PKE_ECC_TMEM_ADDR_EdDSA_VERIFY_B_y         ((void *)(PKE_TMEM_BASE + 0x480)) /*!< Simplified EdDSA verify B_y value */
#define PKE_ECC_TMEM_ADDR_EdDSA_VERIFY_A_x         ((void *)(PKE_TMEM_BASE + 0x740)) /*!< Simplified EdDSA verify A_x value, if A == B, verify success */
#define PKE_ECC_TMEM_ADDR_EdDSA_VERIFY_A_y         ((void *)(PKE_TMEM_BASE + 0x780)) /*!< Simplified EdDSA verify A_y value, if A == B, verify success */
/** @} */

/** @defgroup PKE_Other_Function_ID
  * @{
  */
#define PKE_FUNCTION_ENTRY_CLEAR                   (0x0)
#define PKE_FUNCTION_ENTRY_PUBKEY_MOD_MUL          (0x2)
#define PKE_FUNCTION_ENTRY_PUBKEY_MOD_ADD          (0x3)
#define PKE_FUNCTION_ENTRY_ECDSA_MUL_PRIVATE       (0x4)
#define PKE_FUNCTION_ENTRY_PUBKEY_R_SQAR           (0x5)
#define PKE_FUNCTION_ENTRY_PUBKEY_N_INV            (0x6)
#define PKE_FUNCTION_ENTRY_PUBKEY_K_INV            (0x7)
#define PKE_FUNCTION_ENTRY_PUBKEY_ECC_ADD_POINT    (0x8)
#define PKE_FUNCTION_ENTRY_PUBKEY_SET_A_FROM_P     (0xA)
#define PKE_FUNCTION_ENTRY_PUBKEY_MOD_MUL_BIN      (0xB)
#define PKE_FUNCTION_ENTRY_PUBKEY_MOD_XOR          (0xC)
#define PKE_FUNCTION_ENTRY_PUBKEY_X_MOD_N          (0xE)
#define PKE_FUNCTION_ENTRY_PUBKEY_MOD_SUB          (0xF)
#define PKE_FUNCTION_ENTRY_WITNESS                 (0x10)
#define PKE_FUNCTION_ENTRY_BIG_NUM_MUL             (0x12)
#define PKE_FUNCTION_ENTRY_PQ_MOD                  (0x13)
#define PKE_FUNCTION_ENTRY_SIMPLIFY_ECDSA_GEN      (0x18)
#define PKE_FUNCTION_ENTRY_SIMPLIFY_ECDSA_SIGN     (0x19)
#define PKE_FUNCTION_ENTRY_SIMPLIFY_ECDSA_VERIFY   (0x1A)
#define PKE_FUNCTION_ENTRY_SIMPLIFY_EDDSA_GEN      (0x1B)
#define PKE_FUNCTION_ENTRY_SIMPLIFY_EDDSA_SIGN_R   (0x1C)
#define PKE_FUNCTION_ENTRY_SIMPLIFY_EDDSA_SIGN_S   (0x1D)
#define PKE_FUNCTION_ENTRY_SIMPLIFY_EDDSA_VERIFY   (0x1E)

#define PKE_ECC_IS_MONTGOMERY_EDWARDS_CURVE(CURVE_ID) (((CURVE_ID) == PKE_ECP_CURVE_CURVE25519) || \
                                                       ((CURVE_ID) == PKE_ECP_CURVE_CURVE448) || \
                                                       ((CURVE_ID) == PKE_ECP_CURVE_ED25519) || \
                                                       ((CURVE_ID) == PKE_ECP_CURVE_ED448))
/** @} */
/** @} */

/* Exported Functions --------------------------------------------------------*/

/** @defgroup PKE_Exported_Functions PKE Exported Functions
* @{
*/

/* PKE common API */
_LONG_CALL_ PKE_TypeDef *pke_get_pke_addr(void);
_LONG_CALL_ void pke_engine_init(void);
_LONG_CALL_ void pke_lock_mutex(void);
_LONG_CALL_ void pke_unlock_mutex(void);
_LONG_CALL_ int pke_engine_check_status(uint32_t check_bit);
_LONG_CALL_ int pke_engine_write(void *dst, const void *src, size_t len, size_t op_size);
_LONG_CALL_ void pke_read_string(uint32_t *dst, const char *src, size_t len);
_LONG_CALL_ void pke_reverse_array(uint8_t *arr, size_t len);
_LONG_CALL_ int pke_function_set_entry(uint8_t function_id);

/* RSA */
_LONG_CALL_ int pke_rsa_exp_mod(uint8_t *output, uint32_t outlen, uint8_t *A, uint32_t ALen,
								uint8_t *Key, uint32_t KLen, uint8_t *N, uint32_t NLen);
_LONG_CALL_ int pke_rsa_witness(uint8_t *a_input, size_t a_bits, uint8_t *n_input, size_t n_bits);

/* ECC */
_LONG_CALL_ int pke_ecp_group_init_in_rom(pke_ecp_group *grp, pke_ecp_curve_id curve_id);
_LONG_CALL_ void pke_ecp_point_init(pke_ecp_point *grp);
_LONG_CALL_ void pke_ecp_point_init_base_point(pke_ecp_group *grp, pke_ecp_point *P);
_LONG_CALL_ int pke_ecp_mul(pke_ecp_group *grp, pke_ecp_point *R, uint8_t *m_p, uint8_t m_size, pke_ecp_point *P);

/* ECDSA */
_LONG_CALL_ int pke_ecdsa_share_hw_key(int8_t key_id, int8_t is_share);
_LONG_CALL_ int pke_ecdsa_genkey(pke_ecp_group *grp, int8_t key_id, uint8_t *pri_key_p, uint8_t pri_key_size, uint8_t *pub_key_x, uint8_t *pub_key_y);
_LONG_CALL_ void pke_ecdsa_lalu_hash_process(uint8_t *buf_out, uint8_t *hash_in, uint32_t hash_len_byte, uint32_t precise_bit);
_LONG_CALL_ int pke_ecdsa_write_signature(pke_ecp_group *grp, int8_t key_id, uint8_t *rand_p, uint8_t rand_size,
		uint8_t *pri_key_p, uint8_t pri_key_size, uint8_t *HASH_p, uint8_t HASH_size, uint8_t *sign_r, uint8_t *sign_s);
_LONG_CALL_ int pke_ecdsa_read_signature(pke_ecp_group *grp, uint8_t *pub_key_x_p, uint8_t *pub_key_y_p,
		uint8_t *HASH_p, uint8_t HASH_size, uint8_t *sign_r, uint8_t *sign_s);

/* EdDSA */
_LONG_CALL_ int pke_eddsa_genkey(pke_ecp_group *grp, uint8_t *hashed_key_p, uint8_t *pub_key_encode_p);
_LONG_CALL_ int pke_eddsa_read_signature(pke_ecp_group *grp, uint8_t *pub_key_p,
		uint8_t *k_buf, uint8_t k_size, uint8_t *sign_r, uint8_t *sign_s);

/* EdDSA: Ed25519 */
_LONG_CALL_ int pke_eddsa_ed25519_genkey(uint8_t *priv_key_le, uint8_t *pub_key_le);
_LONG_CALL_ int pke_eddsa_ed25519_write_signature(uint8_t *message, size_t msg_len, uint8_t *priv_key_le, uint8_t *sign_r, uint8_t *sign_s);
_LONG_CALL_ int pke_eddsa_ed25519_read_signature(uint8_t *message, size_t msg_len, uint8_t *pub_key_le, uint8_t *sign_r, uint8_t *sign_s);

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */
/** @} */

/** @} */

/** @} */

#endif