/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_TRNG_H_
#define _AMEBA_TRNG_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup TRNG
* @brief TRNG driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup TRNG_Register_Definitions TRNG Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup TRNG_IER
 * @brief Interrupt Enable Register
 * @{
 **/
#define TRNG_BIT_BUFFER_FULL_INTERRUPT_ENABLE     ((u32)0x00000001 << 4)          /*!<R/W 0x0  RNG buffer full interrupt enable */
#define TRNG_BIT_APT0_FAIL_INTERRUPT_ENABLE       ((u32)0x00000001 << 3)          /*!<R/W 0x1  Adaptive Proportion Test 0 fail interrupt enable */
#define TRNG_BIT_RCT1_FAIL_INTERRUPT_ENABLE       ((u32)0x00000001 << 2)          /*!<R/W 0x1  Repetition Count Test 1 fail interrupt enable */
#define TRNG_BIT_RCT0_FAIL_INTERRUPT_ENABLE       ((u32)0x00000001 << 1)          /*!<R/W 0x1  Repetition Count Test 0 fail interrupt enable */
#define TRNG_BIT_READ_ERROR_INTERRUPT_ENABLE      ((u32)0x00000001 << 0)          /*!<R/W 0x0  Read error interrupt enable. Trigger interrupt when read RNG data register and RNG valid is not 0x1~0x8. */
/** @} */

/** @defgroup TRNG_ISR
 * @brief Interrupt Status Register
 * @{
 **/
#define TRNG_BIT_BUFFER_FULL_INTERRUPT_ACTIVE     ((u32)0x00000001 << 4)          /*!<RO    */
#define TRNG_BIT_APT0_FAIL_INTERRUPT_ACTIVE       ((u32)0x00000001 << 3)          /*!<RO    */
#define TRNG_BIT_RCT1_FAIL_INTERRUPT_ACTIVE       ((u32)0x00000001 << 2)          /*!<RO    */
#define TRNG_BIT_RCT0_FAIL_INTERRUPT_ACTIVE       ((u32)0x00000001 << 1)          /*!<RO    */
#define TRNG_BIT_READ_ERROR_INTERRUPT_ACTIVE      ((u32)0x00000001 << 0)          /*!<RO    */
/** @} */

/** @defgroup TRNG_ICR
 * @brief Interrupt Clear Register
 * @{
 **/
#define TRNG_BIT_BUFFER_FULL_INTERRUPT_CLEAR      ((u32)0x00000001 << 4)          /*!<WO    */
#define TRNG_BIT_APT0_FAIL_INTERRUPT_CLEAR        ((u32)0x00000001 << 3)          /*!<WO   Clear the this interrupt also clear the error flags of APT0 health tests */
#define TRNG_BIT_RCT1_FAIL_INTERRUPT_CLEAR        ((u32)0x00000001 << 2)          /*!<WO   Clear the this interrupt also clear the error flags of RCT1 health tests */
#define TRNG_BIT_RCT0_FAIL_INTERRUPT_CLEAR        ((u32)0x00000001 << 1)          /*!<WO   Clear the this interrupt also clear the error flags of RCT0 health tests */
#define TRNG_BIT_READ_ERROR_INTERRUPT_CLEAR       ((u32)0x00000001 << 0)          /*!<WO    */
/** @} */

/** @defgroup TRNG_FIFO_STATUS
 * @brief Encoded Status Register
 * @{
 **/
#define TRNG_MASK_VALID                           ((u32)0x0000000F << 0)          /*!<RO   Encoded status register. 0x0: buffer empty or APT is not done. 0x1~0x8: number of word is valid. 0xF: APT test fails. */
#define TRNG_VALID(x)                             (((u32)((x) & 0x0000000F) << 0))
#define TRNG_GET_VALID(x)                         ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup TRNG_DATAx
 * @brief Datax Register
 * @{
 **/
#define TRNG_MASK_RANDOM_NUMBER                   ((u32)0xFFFFFFFF << 0)          /*!<RO -  RNG datax register. Read as 0x0 if RNG valid register is not a ready status. A Read access triggers a buffer pop, so the value is only read once then be discarded. */
#define TRNG_RANDOM_NUMBER(x)                     (((u32)((x) & 0xFFFFFFFF) << 0))
#define TRNG_GET_RANDOM_NUMBER(x)                 ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup TRNG_STATUS
 * @brief Detail Status Register
 * @{
 **/
#define TRNG_BIT_APT0_FAIL                        ((u32)0x00000001 << 26)          /*!<RO   APT0 error detected */
#define TRNG_BIT_RCT1_FAIL                        ((u32)0x00000001 << 25)          /*!<RO   RCT1 error detected */
#define TRNG_BIT_RCT0_FAIL                        ((u32)0x00000001 << 24)          /*!<RO   RCT0 error detected */
#define TRNG_BIT_APT0_DONE                        ((u32)0x00000001 << 16)          /*!<RO   Adaptive Proportion Test done status. (APT test pass count >= APT done threshold setting) */
#define TRNG_BIT_BUFFER_OVERFLOW                  ((u32)0x00000001 << 8)          /*!<RO   Buffer overflow status. It is clear by RNG read out. */
#define TRNG_MASK_BUFFER_ENTRY_LEVEL              ((u32)0x0000000F << 0)          /*!<RO   Number of 32-bit random number are valid. 0: empty 8: full */
#define TRNG_BUFFER_ENTRY_LEVEL(x)                (((u32)((x) & 0x0000000F) << 0))
#define TRNG_GET_BUFFER_ENTRY_LEVEL(x)            ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup TRNG_CTRL
 * @brief Control Register
 * @{
 **/
#define TRNG_MASK_SECURITY_CONTROL                ((u32)0x0000000F << 16)          /*!<R/W 0x0  If this field is equal to 0xA, the TRNG becomes a non-secure slave, and all the registers can be accessed by non-secure transaction. If this field is not equal to 0xA, the non-seucre transaction can only access address 0xC~0x30. This register is available only when NONSEC_RNG_BUFFER config is 1 that supports TrustZone and dual buffer. */
#define TRNG_SECURITY_CONTROL(x)                  (((u32)((x) & 0x0000000F) << 16))
#define TRNG_GET_SECURITY_CONTROL(x)              ((u32)(((x >> 16) & 0x0000000F)))
#define TRNG_MASK_LWC_COND_RATE                   ((u32)0x0000000F << 4)          /*!<- -  Compression rate of lightweight-cipher conditioning component. 0: bypass lightweight-cipher conditioning component. Larger than 0: enable extra TRNG conditioning. This register is available only when LWC_COND config is 1. */
#define TRNG_LWC_COND_RATE(x)                     (((u32)((x) & 0x0000000F) << 4))
#define TRNG_GET_LWC_COND_RATE(x)                 ((u32)(((x >> 4) & 0x0000000F)))
#define TRNG_BIT_LFSR_BYPASS                      ((u32)0x00000001 << 2)          /*!<R/W 0x0  LFSR bypass enable */
#define TRNG_BIT_DEBIAS_CORRECTOR_BYPASS          ((u32)0x00000001 << 1)          /*!<R/W 0x0  Debias corrector bypass enable */
#define TRNG_BIT_NOISE_COLLECTOR_OUTPUT_DISABLE   ((u32)0x00000001 << 0)          /*!<R/W 0x0  Noise collector output disable */
/** @} */

/** @defgroup TRNG_SW_RST
 * @brief Software Reset Register
 * @{
 **/
#define TRNG_BIT_SW_RESET                         ((u32)0x00000001 << 0)          /*!<WO -  Write 1 to reset TRNG. It triggers a reset generator to reset all the componets of TRNG except control register. The register settings remain unchange, but other function blocks incuding noise collector(ROSC sampler), debias modules and health tests will be reset. When SW changes the setting of RCT/APT, it's recommanded to clear error first, and then wirte this register to trigger a reset after apply new setting. */
/** @} */

/** @defgroup TRNG_LFSR_CTRL
 * @brief LFSR Control Register
 * @{
 **/
#define TRNG_BIT_FREE_RUN                         ((u32)0x00000001 << 8)          /*!<R/W 0x1  0x1: continuously run LFSR when no input data 0x0: only shift LFSR when input 1 bit data */
#define TRNG_MASK_INIT_DROP_COUNT                 ((u32)0x000000FF << 0)          /*!<R/W 0x40  Discard number of bit after reset or re-init. Default shift out 64 bits in LFSR. */
#define TRNG_INIT_DROP_COUNT(x)                   (((u32)((x) & 0x000000FF) << 0))
#define TRNG_GET_INIT_DROP_COUNT(x)               ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup TRNG_RCT0
 * @brief Repetition Count Test 0 Control Register
 * @{
 **/
#define TRNG_MASK_RCT0_CUTOFF_VALUE               ((u32)0x000000FF << 8)          /*!<R/W 0x1F  Cutoff value */
#define TRNG_RCT0_CUTOFF_VALUE(x)                 (((u32)((x) & 0x000000FF) << 8))
#define TRNG_GET_RCT0_CUTOFF_VALUE(x)             ((u32)(((x >> 8) & 0x000000FF)))
#define TRNG_MASK_RCT0_SAMPLE_SIZE                ((u32)0x00000007 << 1)          /*!<R/W 0x0  Number of bit per test sample. The sample size is the value add 1. 0: 1-bit 1: 2-bit … 7: 8-bit */
#define TRNG_RCT0_SAMPLE_SIZE(x)                  (((u32)((x) & 0x00000007) << 1))
#define TRNG_GET_RCT0_SAMPLE_SIZE(x)              ((u32)(((x >> 1) & 0x00000007)))
#define TRNG_BIT_RCT0_ENABLE                      ((u32)0x00000001 << 0)          /*!<R/W 0x1  Test enable */
/** @} */

/** @defgroup TRNG_RCT1
 * @brief Repetition Count Test 1 Control Register
 * @{
 **/
#define TRNG_MASK_RCT1_CUTOFF_VALUE               ((u32)0x000000FF << 8)          /*!<R/W 0x5  Cutoff value */
#define TRNG_RCT1_CUTOFF_VALUE(x)                 (((u32)((x) & 0x000000FF) << 8))
#define TRNG_GET_RCT1_CUTOFF_VALUE(x)             ((u32)(((x >> 8) & 0x000000FF)))
#define TRNG_MASK_RCT1_SAMPLE_SIZE                ((u32)0x00000007 << 1)          /*!<R/W 0x7  Number of bit per test sample. The sample size is the value add 1. 0: 1-bit 1: 2-bit … 7: 8-bit */
#define TRNG_RCT1_SAMPLE_SIZE(x)                  (((u32)((x) & 0x00000007) << 1))
#define TRNG_GET_RCT1_SAMPLE_SIZE(x)              ((u32)(((x >> 1) & 0x00000007)))
#define TRNG_BIT_RCT1_ENABLE                      ((u32)0x00000001 << 0)          /*!<R/W 0x1  Test enable */
/** @} */

/** @defgroup TRNG_APT
 * @brief Adaptive Proportion Test Control Register
 * @{
 **/
#define TRNG_MASK_APT0_DONE_THRESHOLD             ((u32)0x0000000F << 28)          /*!<R/W 0x1  Assert test done after how many test pass */
#define TRNG_APT0_DONE_THRESHOLD(x)               (((u32)((x) & 0x0000000F) << 28))
#define TRNG_GET_APT0_DONE_THRESHOLD(x)           ((u32)(((x >> 28) & 0x0000000F)))
#define TRNG_MASK_APT0_CUTOFF_VALUE               ((u32)0x000003FF << 16)          /*!<R/W 0x24D  Cutoff value */
#define TRNG_APT0_CUTOFF_VALUE(x)                 (((u32)((x) & 0x000003FF) << 16))
#define TRNG_GET_APT0_CUTOFF_VALUE(x)             ((u32)(((x >> 16) & 0x000003FF)))
#define TRNG_MASK_APT0_SAMPLE_SIZE                ((u32)0x00000007 << 8)          /*!<R/W 0x0  Number of bit per test sample. The sample size is the value add 1. 0: 1-bit 1: 2-bit … 7: 8-bit */
#define TRNG_APT0_SAMPLE_SIZE(x)                  (((u32)((x) & 0x00000007) << 8))
#define TRNG_GET_APT0_SAMPLE_SIZE(x)              ((u32)(((x >> 8) & 0x00000007)))
#define TRNG_BIT_APT0_WINDOW_SIZE                 ((u32)0x00000001 << 1)          /*!<R/W 0x1  0: 512 bits 1: 1024 bits */
#define TRNG_BIT_APT0_ENABLE                      ((u32)0x00000001 << 0)          /*!<R/W 0x1  Test enable */
/** @} */

/** @defgroup TRNG_EN_CTRL0
 * @brief Ring50[29:0] Enable Register
 * @{
 **/
#define TRNG_MASK_RING50_ENABLE_0                 ((u32)0x3FFFFFFF << 0)          /*!<R/W 0x3FFFFFFF  Enable Ring #29~0 */
#define TRNG_RING50_ENABLE_0(x)                   (((u32)((x) & 0x3FFFFFFF) << 0))
#define TRNG_GET_RING50_ENABLE_0(x)               ((u32)(((x >> 0) & 0x3FFFFFFF)))
/** @} */

/** @defgroup TRNG_EN_CTRL1
 * @brief Ring50[49:30] Enable Register
 * @{
 **/
#define TRNG_BIT_RING50_RNG_ENABLE                ((u32)0x00000001 << 20)          /*!<R/W 0x1  Enable Ring50 noise source */
#define TRNG_MASK_RING50_ENABLE_1                 ((u32)0x000FFFFF << 0)          /*!<R/W 0xFFFFF  Enable Ring #49~30 */
#define TRNG_RING50_ENABLE_1(x)                   (((u32)((x) & 0x000FFFFF) << 0))
#define TRNG_GET_RING50_ENABLE_1(x)               ((u32)(((x >> 0) & 0x000FFFFF)))
/** @} */

/** @defgroup TRNG_Ring_Sample_CLK_CTRL
 * @brief Sample clock control
 * @{
 **/
#define TRNG_MASK_CLOCK_SAMPLE_RATE_CONTROL       ((u32)0x0000000F << 8)          /*!<R/W 0x0  For fine-tune rocs sample clock */
#define TRNG_CLOCK_SAMPLE_RATE_CONTROL(x)         (((u32)((x) & 0x0000000F) << 8))
#define TRNG_GET_CLOCK_SAMPLE_RATE_CONTROL(x)     ((u32)(((x >> 8) & 0x0000000F)))
#define TRNG_MASK_CLOCK_PHASE_CONTROL             ((u32)0x000000FF << 0)          /*!<R/W 0x0  For fine-tune rocs sample clock */
#define TRNG_CLOCK_PHASE_CONTROL(x)               (((u32)((x) & 0x000000FF) << 0))
#define TRNG_GET_CLOCK_PHASE_CONTROL(x)           ((u32)(((x >> 0) & 0x000000FF)))
/** @} */
/** @} */



/* Exported types --------------------------------------------------------*/

/** @defgroup TRNG_Exported_Types TRNG Exported Types
  * @{
*****************************************************************************/


/**
 * @brief TRNG Register Declaration
 *****************************************************************************/
typedef struct {
	__IO uint32_t TRNG_IER;                               /*!< INTERRUPT ENABLE REGISTER,  Address offset: 0X000 */
	__IO uint32_t TRNG_ISR;                               /*!< INTERRUPT STATUS REGISTER,  Address offset: 0X004 */
	__IO uint32_t TRNG_ICR;                               /*!< INTERRUPT CLEAR REGISTER,  Address offset: 0X008 */
	__IO uint32_t TRNG_FIFO_STATUS;                       /*!< ENCODED STATUS REGISTER,  Address offset: 0X00C */
	__I  uint32_t TRNG_DATAx[8];                          /*!< DATAx REGISTER,  Address offset: 0X010-0X02C */
	__IO uint32_t TRNG_STATUS;                            /*!< DETAIL STATUS REGISTER,  Address offset: 0X030 */
	__IO uint32_t TRNG_CTRL;                              /*!< CONTROL REGISTER,  Address offset: 0X034 */
	__IO uint32_t TRNG_SW_RST;                            /*!< SOFTWARE RESET REGISTER,  Address offset: 0X038 */
	__IO uint32_t RSVD0[51];                              /*!<  Reserved,  Address offset:0x3C-0x104 */
	__IO uint32_t TRNG_LFSR_CTRL;                         /*!< LFSR CONTROL REGISTER,  Address offset: 0X108 */
	__IO uint32_t RSVD1;                                  /*!<  Reserved,  Address offset:0x10C */
	__IO uint32_t TRNG_RCT0;                              /*!< REPETITION COUNT TEST 0 CONTROL REGISTER,  Address offset: 0X110 */
	__IO uint32_t TRNG_RCT1;                              /*!< REPETITION COUNT TEST 1 CONTROL REGISTER,  Address offset: 0X114 */
	__IO uint32_t TRNG_APT;                               /*!< ADAPTIVE PROPORTION TEST CONTROL REGISTER,  Address offset: 0X118 */
	__IO uint32_t RSVD2[54];                              /*!<  Reserved,  Address offset:0x11C-0x1F0 */
	__IO uint32_t TRNG_EN_CTRL0;                          /*!< RING50[29:0] ENABLE REGISTER,  Address offset: 0X1F4 */
	__IO uint32_t TRNG_EN_CTRL1;                          /*!< RING50[49:30] ENABLE REGISTER,  Address offset: 0X1F8 */
	__IO uint32_t TRNG_RING_SAMPLE_CLK_CTRL;              /*!< SAMPLE CLOCK CONTROL Register,  Address offset: 0X1FC */
} TRNG_TypeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/* Exported constants --------------------------------------------------------*/
/** @defgroup TRNG_Exported_Constants TRNG Exported Constants
  * @{
  */

/** @defgroup TRNG_Security_Control
  * @{
  */
#define TRNG_NO_SECURITY_EN			0x0A
/**
  * @}
  */

/** @defgroup TRNG_FIFO_Level
  * @{
  */
#define TRNG_FIFO_LEVEL_EMPTY			0x0
#define TRNG_FIFO_LEVEL_INVALID		0xF
/**
  * @}
  */

/** @defgroup TRNG_Polling_Timeout
  * @{
  */
#define TRNG_POLLING_TIMEOUT			0xFFFF
/**
  * @}
  */

/**
  * @}
  */

/** @} */

/** @} */

_LONG_CALL_ extern u32 TRNG_Security_Only(BOOL NewState);
_LONG_CALL_ extern void TRNG_INTConfig(u32 TRNG_IT, u32 NewState);
_LONG_CALL_ extern void TRNG_ClearINT(u32 INTrBit);
_LONG_CALL_ extern u32 TRNG_GetINT(void);
_LONG_CALL_ void TRNG_Init(void);
_LONG_CALL_ void TRNG_Random_Delay(void);

/* MANUAL_GEN_END */

#endif


