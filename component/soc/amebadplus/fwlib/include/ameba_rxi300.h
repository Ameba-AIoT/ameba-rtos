/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_RXI300_H_
#define _AMEBA_RXI300_H_

/* AUTO_GEN_START */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup RXI300
* @brief RXI300 driver modules
* @{
*/

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup RXI300_Register_Definitions RXI300 Register Definitions
 * @{
 *****************************************************************************/

/** @defgroup NAME
 * @brief RXI300 design name register
 * @{
 **/
#define RXI300_MASK_NAME                            ((u32)0xFFFFFFFF << 0)          /*!<R 00052300h  The design name of RXI-300. */
#define RXI300_NAME(x)                              (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_NAME(x)                          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup VER
 * @brief RXI300 design version register
 * @{
 **/
#define RXI300_MASK_VER                             ((u32)0xFFFFFFFF << 0)          /*!<R 00000300h  The design version of RXI-300. */
#define RXI300_VER(x)                               (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_VER(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup RVE
 * @brief RXI300 design reversion register
 * @{
 **/
#define RXI300_MASK_REV                             ((u32)0xFFFFFFFF << 0)          /*!<R 0h  The design reversion of RXI-300. */
#define RXI300_REV(x)                               (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_REV(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup INST
 * @brief RXI300 design instance number register
 * @{
 **/
#define RXI300_MASK_INST                            ((u32)0xFFFFFFFF << 0)          /*!<R 0h  The design instance number of RXI-300. */
#define RXI300_INST(x)                              (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_INST(x)                          ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IMPL_Y
 * @brief RXI300 implementation year register
 * @{
 **/
#define RXI300_MASK_IMPL_Y                          ((u32)0xFFFFFFFF << 0)          /*!<R 2022h  The implementatin year of RXI-300. Reset value depends on implementation date. */
#define RXI300_IMPL_Y(x)                            (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_IMPL_Y(x)                        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup IMPL_D
 * @brief RXI300 implementation date register
 * @{
 **/
#define RXI300_MASK_IMPL_D                          ((u32)0xFFFFFFFF << 0)          /*!<R 08081200h  The implementatin date of RXI-300. Reset value depends on implementation date. */
#define RXI300_IMPL_D(x)                            (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_IMPL_D(x)                        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup DEV
 * @brief RXI300 developer register
 * @{
 **/
#define RXI300_MASK_DEV                             ((u32)0xFFFFFFFF << 0)          /*!<R 05207583h  The developer of RXI-300. */
#define RXI300_DEV(x)                               (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_DEV(x)                           ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PRO_NUM
 * @brief RXI300 project number register
 * @{
 **/
#define RXI300_MASK_PRO_NUM                         ((u32)0xFFFFFFFF << 0)          /*!<R 00006920h  The project number of RXI-300. */
#define RXI300_PRO_NUM(x)                           (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_PRO_NUM(x)                       ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_PLD0_S
 * @brief Error log payload register
 * @{
 **/
#define RXI300_MASK_ELR_BSTINDEX_S                  ((u32)0x000000FF << 24)          /*!<R 0h  Burst index of the error request. */
#define RXI300_ELR_BSTINDEX_S(x)                    (((u32)((x) & 0x000000FF) << 24))
#define RXI300_GET_ELR_BSTINDEX_S(x)                ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTLEN_S                    ((u32)0x000000FF << 16)          /*!<R 0h  Burst Length of the error request. */
#define RXI300_ELR_BSTLEN_S(x)                      (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_ELR_BSTLEN_S(x)                  ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTTYPE_S                   ((u32)0x00000007 << 11)          /*!<R 0h  BurstSeq of the error request. */
#define RXI300_ELR_BSTTYPE_S(x)                     (((u32)((x) & 0x00000007) << 11))
#define RXI300_GET_ELR_BSTTYPE_S(x)                 ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_MASK_ELR_CMD_S                       ((u32)0x00000007 << 8)          /*!<R 0h  CMD of the error request. 0x1: Write command 0x2: Read command */
#define RXI300_ELR_CMD_S(x)                         (((u32)((x) & 0x00000007) << 8))
#define RXI300_GET_ELR_CMD_S(x)                     ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_MASK_ELR_SRC_S                       ((u32)0x000000FF << 0)          /*!<R 0h  Error source of the error request. 0x10: S0 group address hole default slave 0x11: S1, S2 group address hole default slave */
#define RXI300_ELR_SRC_S(x)                         (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELR_SRC_S(x)                     ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_PLD1_S
 * @brief Error log payload register1
 * @{
 **/
#define RXI300_MASK_ELR_LOCK_S                      ((u32)0x00000003 << 30)          /*!<R 0h  AxLock of the error request. */
#define RXI300_ELR_LOCK_S(x)                        (((u32)((x) & 0x00000003) << 30))
#define RXI300_GET_ELR_LOCK_S(x)                    ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_MASK_ELR_CACHE_S                     ((u32)0x0000000F << 26)          /*!<R 0h  AxCache of the error request. */
#define RXI300_ELR_CACHE_S(x)                       (((u32)((x) & 0x0000000F) << 26))
#define RXI300_GET_ELR_CACHE_S(x)                   ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_MASK_ELR_PROT_S                      ((u32)0x00000007 << 23)          /*!<R 0h  AxProt of the error request. */
#define RXI300_ELR_PROT_S(x)                        (((u32)((x) & 0x00000007) << 23))
#define RXI300_GET_ELR_PROT_S(x)                    ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_MASK_ELR_SIZE_S                      ((u32)0x00000007 << 16)          /*!<R 0h  Size of the error request. */
#define RXI300_ELR_SIZE_S(x)                        (((u32)((x) & 0x00000007) << 16))
#define RXI300_GET_ELR_SIZE_S(x)                    ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_ID_S
 * @brief Error log tagid register
 * @{
 **/
#define RXI300_MASK_ELR_ID_S                        ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Tagid of the error request. 9'b00000????: M0 access error 9'b00010????: M1 access error 9'b0010?????: M2 access error 9'b001100???: M3 access error 9'b01110000?: M4-0 access error 9'b01000000?: M4-1 access error 9'b01010000?: M5 access error 9'b01100000?: M6 access error */
#define RXI300_ELR_ID_S(x)                          (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ID_S(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR0_S
 * @brief Error log addrress register
 * @{
 **/
#define RXI300_MASK_ELR_ADR0_S                      ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Address[31:0] of the error request. */
#define RXI300_ELR_ADR0_S(x)                        (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ADR0_S(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR1_S
 * @brief Error log addrress register1
 * @{
 **/
#define RXI300_MASK_ELR_ADR1_S                      ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Address[63:32] of the error request. */
#define RXI300_ELR_ADR1_S(x)                        (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ADR1_S(x)                    ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_CODE_S
 * @brief Error log error code register
 * @{
 **/
#define RXI300_MASK_ELR_CODE_S                      ((u32)0x000000FF << 0)          /*!<R 0h  The error code of error request, if it's 0, indicates that there is no error request. 8'h00: No error 8'h01: Address hole error */
#define RXI300_ELR_CODE_S(x)                        (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELR_CODE_S(x)                    ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_INTR_CLR_S
 * @brief Error log interrupt clear register
 * @{
 **/
#define RXI300_BIT_ELR_INTR_CLR_S                   ((u32)0x00000001 << 0)          /*!<W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write 1'b1 to this register every time when the error excep-tion occurs. Note that this register is write-only, the return value of reading this register is meaningless. */
/** @} */

/** @defgroup IDAU_BAx
 * @brief IDAU base address registerx
 * @{
 **/
#define RXI300_MASK_IDAU_BAx                        ((u32)0x000FFFFF << 12)          /*!<R/W 0h  It's the base address of region x which can be set as non-secure. */
#define RXI300_IDAU_BAx(x)                          (((u32)((x) & 0x000FFFFF) << 12))
#define RXI300_GET_IDAU_BAx(x)                      ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */

/** @defgroup IDAU_LAx
 * @brief IDAU top address registerx
 * @{
 **/
#define RXI300_MASK_IDAU_LAx                        ((u32)0x000FFFFF << 12)          /*!<R/W FFFFFh  It's the top address of region x which can be set as non-secure. For IDAU_LA0, the default is FFFFF000h; for IDAU_LAx, the default is 00000000h. */
#define RXI300_IDAU_LAx(x)                          (((u32)((x) & 0x000FFFFF) << 12))
#define RXI300_GET_IDAU_LAx(x)                      ((u32)(((x >> 12) & 0x000FFFFF)))
/** @} */

/** @defgroup IDAU_CTRL
 * @brief IDAU enable control register
 * @{
 **/
#define RXI300_MASK_WRITE_CTRL                      ((u32)0x000000FF << 16)          /*!<R/W 0h  Write permission address control register. Bit[23]: controls entry 7. Bit[22]: controls entry 6. Bit[21]: controls entry 5. Bit[20]: controls entry 4. Bit[19]: controls entry 3. Bit[18]: controls entry 2. Bit[17]: controls entry 1. Bit[16]: controls entry 0. When the bit[x] is 1'b1, mask the address region as write forbidden attribution; when it is 1'b0, the address region is read/write attribution. Note: When the LOCK register is 1'b1, you cannot program this register unless system reset. */
#define RXI300_WRITE_CTRL(x)                        (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_WRITE_CTRL(x)                    ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_SECURE_CTRL                     ((u32)0x000000FF << 0)          /*!<R/W 0h  MPC secure address control register. Bit[7]: controls entry 7. Bit[6]: controls entry 6. Bit[5]: controls entry 5. Bit[4]: controls entry 4. Bit[3]: controls entry 3. Bit[2]: controls entry 2. Bit[1]: controls entry 1. Bit[0]: controls entry 0. When the bit[x] is 1'b1, mask the address region as non-secure attribution; when it is 1'b0, the address region is secure attribution. Note: When the LOCK register is 1'b1, you cannot program this register unless system reset. */
#define RXI300_SECURE_CTRL(x)                       (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_SECURE_CTRL(x)                   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup IDAU_LOCK
 * @brief IDAU lock control register
 * @{
 **/
#define RXI300_BIT_IDAU_LOCK                        ((u32)0x00000001 << 0)          /*!<R/W 0h  Lock write transfer for all registers in IDAU (including lock register it self) until system reset if it is ebabled. [0]: lock bit for all registers in IDAU 0: disable lock 1: enable lock */
/** @} */

/** @defgroup PPC1_REG
 * @brief PPC1 control register
 * @{
 **/
#define RXI300_BIT_SPORT1_I2S                       ((u32)0x00000001 << 13)          /*!<R/W 0h  1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_SPORT0_I2S                       ((u32)0x00000001 << 12)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_QSPI                             ((u32)0x00000001 << 11)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_SPIC_USERMODE                    ((u32)0x00000001 << 10)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_PSRAM_SPIC_USERMODE              ((u32)0x00000001 << 9)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_PSRAM_PHY                        ((u32)0x00000001 << 8)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_SPI1                             ((u32)0x00000001 << 7)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_SPI0                             ((u32)0x00000001 << 6)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_SDIO_CFG                         ((u32)0x00000001 << 5)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_PPE_CFG                          ((u32)0x00000001 << 4)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_GDMA0_CFG                        ((u32)0x00000001 << 3)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_SECURE_ENGINE                    ((u32)0x00000001 << 2)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_BT_CFG                           ((u32)0x00000001 << 1)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_WIFI_CFG                         ((u32)0x00000001 << 0)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
/** @} */

/** @defgroup PPC2_REG
 * @brief PPC2 control register
 * @{
 **/
#define RXI300_BIT_TIMER_0_7_BASIC                  ((u32)0x00000001 << 13)          /*!<R/W 0h  1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_PMC_TIMER_0_1                    ((u32)0x00000001 << 12)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_DBG_TIMER                        ((u32)0x00000001 << 11)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_IPC                              ((u32)0x00000001 << 10)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_KEY_SCAN                         ((u32)0x00000001 << 9)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_CAP_TOUCH                        ((u32)0x00000001 << 8)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_ADC_ADC_COMP                     ((u32)0x00000001 << 7)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_GPIO_A_B                         ((u32)0x00000001 << 6)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_UART3_LOG                        ((u32)0x00000001 << 5)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_UART2_BT                         ((u32)0x00000001 << 4)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_UART1                            ((u32)0x00000001 << 3)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_UART0                            ((u32)0x00000001 << 2)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_SYSON                            ((u32)0x00000001 << 1)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_OTPC_CFG                         ((u32)0x00000001 << 0)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
/** @} */

/** @defgroup PPC3_REG
 * @brief PPC3 control register
 * @{
 **/
#define RXI300_BIT_I2C1                             ((u32)0x00000001 << 8)          /*!<R/W 0h  1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_I2C0                             ((u32)0x00000001 << 7)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_IR                               ((u32)0x00000001 << 6)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_PDM                              ((u32)0x00000001 << 5)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_LEDC                             ((u32)0x00000001 << 4)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_RSIP                             ((u32)0x00000001 << 3)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_RXI300                           ((u32)0x00000001 << 2)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_TRNG_POR1_PORT2                  ((u32)0x00000001 << 1)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
#define RXI300_BIT_TIMER_8_9_PULSE_PWM_TIMER10_11   ((u32)0x00000001 << 0)          /*!<R/W   1: Non-secure attribution 0: Secure attribution */
/** @} */

/** @defgroup PPC_LOCK
 * @brief PPC LOCK configure register
 * @{
 **/
#define RXI300_BIT_LOCK                             ((u32)0x00000001 << 0)          /*!<R/W   PPC lock register. When writing 1'b1 to this nit, PPC will be locked. Note: It can not be written to 1'b0 again until system reset to prevent modification. When PPC is locked, you can not program PPC registers unless system reset. */
/** @} */

/** @defgroup SPIC_ARB_CTRL0
 * @brief SPIC arbitration control register
 * @{
 **/
#define RXI300_MASK_SPIC_ARB_CTRL0                  ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  Arbitration control register. The arbitration can flexibly set the priority of input port. The arbitration scheme of slots uses round-robin. User can use more slots to remap input ports request and define the order of slots. If one of requests has low-priority or low-bandwidth , maybe user can allocate less slots. Otherwise, user can allocate more slots. Each slot should be set by a port number. The port number represents a master group. The master group is shown as below: 0x0: KM4 0x1: KM0, GDMA0, WIFI, LX, SDIO 0x2: PPE And the following shows bit field of each slots: [3:0]: slot 0 of arbiter [7:4]: slot 1 of arbiter [11:8]: slot 2 of arbiter [15:12]: slot 3 of arbiter [19:16]: slot 4 of arbiter [23:20]: slot 5 of arbiter [27:24]: slot 6 of arbiter [31:28]: slot 7 of arbiter */
#define RXI300_SPIC_ARB_CTRL0(x)                    (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_SPIC_ARB_CTRL0(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup SPIC_ARB_CTRL1
 * @brief SPIC arbitration control register1
 * @{
 **/
#define RXI300_MASK_SPIC_ARB_CTRL1                  ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  This is extended register of SPIC_ARB_CTRL0. [3:0]: slot 8 of arbiter [7:4]: slot 9 of arbiter [11:8]: slot 10 of arbiter [15:12]: slot 11 of arbiter [19:16]: slot 12 of arbiter [23:20]: slot 13 of arbiter [27:24]: slot 14 of arbiter [31:28]: slot 15 of arbiter */
#define RXI300_SPIC_ARB_CTRL1(x)                    (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_SPIC_ARB_CTRL1(x)                ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup HSRAM_ARB_CTRL0
 * @brief HSRAM arbitration control register
 * @{
 **/
#define RXI300_MASK_HSRAM_ARB_CTRL0                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  Arbitration control register. The arbitration can flexibly set the priority of input port. The arbitration scheme of slots uses round-robin. User can use more slots to remap input ports request and define the order of slots. If one of requests has low-priority or low-bandwidth, maybe user can allocate less slots. Otherwise, user can allocate more slots. Each slot should be set by a port number. The port number represents a master group: 0x0: KM4 0x1: KM0, GDMA0, WIFI, LX, SDIO, SIC, OTPC And the following shows bit field of each slots: [3:0]: slot 0 of arbiter [7:4]: slot 1 of arbiter [11:8]: slot 2 of arbiter [15:12]: slot 3 of arbiter [19:16]: slot 4 of arbiter [23:20]: slot 5 of arbiter [27:24]: slot 6 of arbiter [31:28]: slot 7 of arbiter */
#define RXI300_HSRAM_ARB_CTRL0(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_HSRAM_ARB_CTRL0(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup HSRAM_ARB_CTRL1
 * @brief HSRAM arbitration control register1
 * @{
 **/
#define RXI300_MASK_HSRAM_ARB_CTRL1                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  This is extended register of HSRAM_ARB_CTRL0. [3:0]: slot 8 of arbiter [7:4]: slot 9 of arbiter [11:8]: slot 10 of arbiter [15:12]: slot 11 of arbiter [19:16]: slot 12 of arbiter [23:20]: slot 13 of arbiter [27:24]: slot 14 of arbiter [31:28]: slot 15 of arbiter */
#define RXI300_HSRAM_ARB_CTRL1(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_HSRAM_ARB_CTRL1(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup HPERI_ARB_CTRL0
 * @brief HPERI arbitration control register
 * @{
 **/
#define RXI300_MASK_HPERI_ARB_CTRL0                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  Arbitration control register. The arbitration can flexibly set the priority of input port. The arbitration scheme of slots uses round-robin.User can use more slots to remap input ports request and define the order of slots. If one of requests has low-priority or low-bandwidth, maybe user can allocate less slots. Otherwise, user can allocate more slots. Each slot should be set by a port number. The port number represents a master group: 0x0: KM4 0x1: KM0 0x2: GDMA 0x3: SIC And the following shows bit field of each slots: [3:0]: slot 0 of arbiter [7:4]: slot 1 of arbiter [11:8]: slot 2 of arbiter [15:12]: slot 3 of arbiter [19:16]: slot 4 of arbiter [23:20]: slot 5 of arbiter [27:24]: slot 6 of arbiter [31:28]: slot 7 of arbiter */
#define RXI300_HPERI_ARB_CTRL0(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_HPERI_ARB_CTRL0(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup HPERI_ARB_CTRL1
 * @brief HPERI arbitration control register1
 * @{
 **/
#define RXI300_MASK_HPERI_ARB_CTRL1                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  This is extended register of HPERI_ARB_CTRL0 slot. [3:0]: slot 8 of arbiter [7:4]: slot 9 of arbiter [11:8]: slot 10 of arbiter [15:12]: slot 11 of arbiter [19:16]: slot 12 of arbiter [23:20]: slot 13 of arbiter [27:24]: slot 14 of arbiter [31:28]: slot 15 of arbiter */
#define RXI300_HPERI_ARB_CTRL1(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_HPERI_ARB_CTRL1(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_CTRL0
 * @brief PSRAM arbitration control register0
 * @{
 **/
#define RXI300_MASK_PSRAM_ARB_CTRL0                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  Arbitration control register. The arbitration can flexibly set the priority of input port.The arbitration scheme of slots uses round-robin. User can use more slots to remap input ports request and define the order of slots. If one of requests has low-priority or low-bandwidth, maybe user can allocate less slots. Otherwise, user can allocate more slots. Each slot should be set by a port number. The port number represents a master group: 0x0: KM4 0x1: KM0, GDMA0, WIFI, LX, SDIO, SIC, OTPC 0x2: PPE And the following shows bit field of each slots: [3:0]: slot 0 of arbiter [7:4]: slot 1 of arbiter [11:8]: slot 2 of arbiter [15:12]: slot 3 of arbiter [19:16]: slot 4 of arbiter [23:20]: slot 5 of arbiter [27:24]: slot 6 of arbiter [31:28]: slot 7 of arbiter */
#define RXI300_PSRAM_ARB_CTRL0(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_PSRAM_ARB_CTRL0(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup PSRAM_ARB_CTRL1
 * @brief PSRAM arbitration control register1
 * @{
 **/
#define RXI300_MASK_PSRAM_ARB_CTRL1                 ((u32)0xFFFFFFFF << 0)          /*!<R/W 0h  This is extended register of PSRAM_ARB_CTRL0 slot. [3:0]: slot 8 of arbiter [7:4]: slot 9 of arbiter [11:8]: slot 10 of arbiter [15:12]: slot 11 of arbiter [19:16]: slot 12 of arbiter [23:20]: slot 13 of arbiter [27:24]: slot 14 of arbiter [31:28]: slot 15 of arbiter */
#define RXI300_PSRAM_ARB_CTRL1(x)                   (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_PSRAM_ARB_CTRL1(x)               ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_PLD0
 * @brief Error log payload register
 * @{
 **/
#define RXI300_MASK_ELR_BSTINDEX                    ((u32)0x000000FF << 24)          /*!<R 0h  Burst index of the error request. */
#define RXI300_ELR_BSTINDEX(x)                      (((u32)((x) & 0x000000FF) << 24))
#define RXI300_GET_ELR_BSTINDEX(x)                  ((u32)(((x >> 24) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTLEN                      ((u32)0x000000FF << 16)          /*!<R 0h  Burst Length of the error request. */
#define RXI300_ELR_BSTLEN(x)                        (((u32)((x) & 0x000000FF) << 16))
#define RXI300_GET_ELR_BSTLEN(x)                    ((u32)(((x >> 16) & 0x000000FF)))
#define RXI300_MASK_ELR_BSTTYPE                     ((u32)0x00000007 << 11)          /*!<R 0h  BurstSeq of the error request. */
#define RXI300_ELR_BSTTYPE(x)                       (((u32)((x) & 0x00000007) << 11))
#define RXI300_GET_ELR_BSTTYPE(x)                   ((u32)(((x >> 11) & 0x00000007)))
#define RXI300_MASK_ELR_CMD                         ((u32)0x00000007 << 8)          /*!<R 0h  CMD of the error request. 0x1: Write command 0x2: Read command */
#define RXI300_ELR_CMD(x)                           (((u32)((x) & 0x00000007) << 8))
#define RXI300_GET_ELR_CMD(x)                       ((u32)(((x >> 8) & 0x00000007)))
#define RXI300_MASK_ELRRC                           ((u32)0x000000FF << 0)          /*!<R 0h  Error source of the error request. 0x00: M0 secure default slave 0x01: M1 secure default slave 0x02: M2 secure default slave 0x03: M3 secure default slave 0x04: M4 secure default slave 0x05: M5 secure default slave 0x06: M6 secure default slave 0x10: S0 secure default slave 0x20: S1, S2 secure default slave 0x21: S6 secure default slave 0x30: S3 AHB group secure default slave 0x31: S3 APB group secure default slave 0x32: S3-11 secure default slave 0x33: S3 AXI group secure default slave 0x34: S3-8, S3-9 secure default slave 0x40: S4 secure default slave 0x41: S5 secure default slave */
#define RXI300_ELRRC(x)                             (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELRRC(x)                         ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_PLD1
 * @brief Error log payload register1
 * @{
 **/
#define RXI300_MASK_ELR_LOCK                        ((u32)0x00000003 << 30)          /*!<R 0h  AxLock of the error request. */
#define RXI300_ELR_LOCK(x)                          (((u32)((x) & 0x00000003) << 30))
#define RXI300_GET_ELR_LOCK(x)                      ((u32)(((x >> 30) & 0x00000003)))
#define RXI300_MASK_ELR_CACHE                       ((u32)0x0000000F << 26)          /*!<R 0h  AxCache of the error request. */
#define RXI300_ELR_CACHE(x)                         (((u32)((x) & 0x0000000F) << 26))
#define RXI300_GET_ELR_CACHE(x)                     ((u32)(((x >> 26) & 0x0000000F)))
#define RXI300_MASK_ELR_PROT                        ((u32)0x00000007 << 23)          /*!<R 0h  AxProt of the error request. */
#define RXI300_ELR_PROT(x)                          (((u32)((x) & 0x00000007) << 23))
#define RXI300_GET_ELR_PROT(x)                      ((u32)(((x >> 23) & 0x00000007)))
#define RXI300_MASK_ELRIZE                          ((u32)0x00000007 << 16)          /*!<R 0h  Size of the error request. */
#define RXI300_ELRIZE(x)                            (((u32)((x) & 0x00000007) << 16))
#define RXI300_GET_ELRIZE(x)                        ((u32)(((x >> 16) & 0x00000007)))
/** @} */

/** @defgroup ELR_ID
 * @brief Error log tagid register
 * @{
 **/
#define RXI300_MASK_ELR_ID                          ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Tagid of the error request. 9'b00000????: M0 access error 9'b00010????: M1 access error 9'b0010?????: M2 access error 9'b001100???: M3 access error 9'b01110000?: M4-0 access error 9'b01000000?: M4-1 access error 9'b01010000?: M5 access error 9'b01100000?: M6 access error */
#define RXI300_ELR_ID(x)                            (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ID(x)                        ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR0
 * @brief Error log addrress register
 * @{
 **/
#define RXI300_MASK_ELR_ADR0                        ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Address[31:0] of the error request. */
#define RXI300_ELR_ADR0(x)                          (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ADR0(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_ADR1
 * @brief Error log addrress register1
 * @{
 **/
#define RXI300_MASK_ELR_ADR1                        ((u32)0xFFFFFFFF << 0)          /*!<R 0h  Address[63:32] of the error request. */
#define RXI300_ELR_ADR1(x)                          (((u32)((x) & 0xFFFFFFFF) << 0))
#define RXI300_GET_ELR_ADR1(x)                      ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ELR_CODE
 * @brief Error log error code register
 * @{
 **/
#define RXI300_MASK_ELR_CODE                        ((u32)0x000000FF << 0)          /*!<R 0h  The error code of error request, if it's 0, indicates that there is no error request. 8'h00: No error 8'h02: Secure fault error */
#define RXI300_ELR_CODE(x)                          (((u32)((x) & 0x000000FF) << 0))
#define RXI300_GET_ELR_CODE(x)                      ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ELR_INTR_CLR
 * @brief Error log interrupt clear register
 * @{
 **/
#define RXI300_BIT_ELR_INTR_CLR                     ((u32)0x00000001 << 0)          /*!<W 0h  Error log interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear. It has to write l'b1 to this register every time when the error excep-tion occurs. Note that this register is write-only, the return value of reading this register is meaningless. */
/** @} */
/** @} */

/* Exported types --------------------------------------------------------*/

/** @defgroup RXI300_Exported_Types RXI300 Exported Types
  * @{
*****************************************************************************/

/** @brief RXI300 port
*****************************************************************************/
typedef struct {

	__IO uint32_t IDAU_BAx;                               /*!< IDAU BASE ADDRESS REGISTERx Register */
	__IO uint32_t IDAU_LAx;                               /*!< IDAU TOP ADDRESS REGISTERx Register */
} MPC_EntryTypeDef;





/**
 * @brief RXI300 Register Declaration
 *****************************************************************************/
typedef struct {
	__I  uint32_t NAME;                                   /*!< RXI300 DESIGN NAME REGISTER,  Address offset: 0x000 */
	__I  uint32_t VER;                                    /*!< RXI300 DESIGN VERSION REGISTER,  Address offset: 0x004 */
	__I  uint32_t RVE;                                    /*!< RXI300 DESIGN REVERSION REGISTER,  Address offset: 0x008 */
	__I  uint32_t INST;                                   /*!< RXI300 DESIGN INSTANCE NUMBER REGISTER,  Address offset: 0x00C */
	__I  uint32_t IMPL_Y;                                 /*!< RXI300 IMPLEMENTATION YEAR REGISTER,  Address offset: 0x010 */
	__I  uint32_t IMPL_D;                                 /*!< RXI300 IMPLEMENTATION DATE REGISTER,  Address offset: 0x014 */
	__I  uint32_t DEV;                                    /*!< RXI300 DEVELOPER REGISTER,  Address offset: 0x018 */
	__I  uint32_t PRO_NUM;                                /*!< RXI300 PROJECT NUMBER REGISTER,  Address offset: 0x01C */
	__IO uint32_t RSVD0[56];                              /*!<  Reserved,  Address offset:0x20-0xFC */
	__IO uint32_t ELR_PLD0_S;                             /*!< ERROR LOG PAYLOAD REGISTER,  Address offset: 0x100 */
	__IO uint32_t ELR_PLD1_S;                             /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset: 0x104 */
	__I  uint32_t ELR_ID_S;                               /*!< ERROR LOG TAGID REGISTER,  Address offset: 0x108 */
	__I  uint32_t ELR_ADR0_S;                             /*!< ERROR LOG ADDRRESS REGISTER,  Address offset: 0x10C */
	__I  uint32_t ELR_ADR1_S;                             /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset: 0x110 */
	__IO uint32_t RSVD1[7];                               /*!<  Reserved,  Address offset:0x114-0x12C */
	__IO uint32_t ELR_CODE_S;                             /*!< ERROR LOG ERROR CODE REGISTER,  Address offset: 0x130 */
	__IO uint32_t RSVD2[2];                               /*!<  Reserved,  Address offset:0x134-0x138 */
	__IO uint32_t ELR_INTR_CLR_S;                         /*!< ERROR LOG INTERRUPT CLEAR REGISTER,  Address offset: 0x13C */
	__IO uint32_t RSVD3[48];                              /*!<  Reserved,  Address offset:0x140-0x1FC */
	MPC_EntryTypeDef MPC_Entry[8];                        /*!< RXI300 IP have 8 ports,  Address offset: 0x200-0x23C */
	__IO uint32_t IDAU_CTRL;                              /*!< IDAU ENABLE CONTROL REGISTER,  Address offset: 0x240 */
	__IO uint32_t IDAU_LOCK;                              /*!< IDAU LOCK CONTROL REGISTER,  Address offset: 0x244 */
	__IO uint32_t RSVD4[110];                              /*!<  Reserved,  Address offset:0x248-0x3FC */
	__IO uint32_t PPC1_REG;                               /*!< PPC1 CONTROL REGISTER,  Address offset: 0x400 */
	__IO uint32_t PPC2_REG;                               /*!< PPC2 CONTROL REGISTER,  Address offset: 0x404 */
	__IO uint32_t PPC3_REG;                               /*!< PPC3 CONTROL REGISTER,  Address offset: 0x408 */
	__IO uint32_t PPC_LOCK;                               /*!< PPC LOCK CONFIGURE REGISTER,  Address offset: 0x40C */
	__IO uint32_t RSVD5[764];                              /*!<  Reserved,  Address offset:0x410-0xFFC */
	__IO uint32_t SPIC_ARB_CTRL0;                         /*!< SPIC ARBITRATION CONTROL REGISTER,  Address offset: 0x1000 */
	__IO uint32_t SPIC_ARB_CTRL1;                         /*!< SPIC ARBITRATION CONTROL REGISTER1 Register,  Address offset: 0x1004 */
	__IO uint32_t HSRAM_ARB_CTRL0;                        /*!< HSRAM ARBITRATION CONTROL REGISTER,  Address offset: 0x1008 */
	__IO uint32_t HSRAM_ARB_CTRL1;                        /*!< HSRAM ARBITRATION CONTROL REGISTER1 Register,  Address offset: 0x100C */
	__IO uint32_t HPERI_ARB_CTRL0;                        /*!< HPERI ARBITRATION CONTROL REGISTER,  Address offset: 0x1010 */
	__IO uint32_t HPERI_ARB_CTRL1;                        /*!< HPERI ARBITRATION CONTROL REGISTER1 Register,  Address offset: 0x1014 */
	__IO uint32_t PSRAM_ARB_CTRL0;                        /*!< PSRAM ARBITRATION CONTROL REGISTER0 Register,  Address offset: 0x1018 */
	__IO uint32_t PSRAM_ARB_CTRL1;                        /*!< PSRAM ARBITRATION CONTROL REGISTER1 Register,  Address offset: 0x101C */
	__IO uint32_t RSVD6[56];                              /*!<  Reserved,  Address offset:0x1020-0x10FC */
	__IO uint32_t ELR_PLD0;                               /*!< ERROR LOG PAYLOAD REGISTER,  Address offset: 0x1100 */
	__IO uint32_t ELR_PLD1;                               /*!< ERROR LOG PAYLOAD REGISTER1 Register,  Address offset: 0x1104 */
	__I  uint32_t ELR_ID;                                 /*!< ERROR LOG TAGID REGISTER,  Address offset: 0x1108 */
	__I  uint32_t ELR_ADR0;                               /*!< ERROR LOG ADDRRESS REGISTER,  Address offset: 0x110C */
	__I  uint32_t ELR_ADR1;                               /*!< ERROR LOG ADDRRESS REGISTER1 Register,  Address offset: 0x1110 */
	__IO uint32_t RSVD7[7];                               /*!<  Reserved,  Address offset:0x1114-0x112C */
	__IO uint32_t ELR_CODE;                               /*!< ERROR LOG ERROR CODE REGISTER,  Address offset: 0x1130 */
	__IO uint32_t RSVD8[2];                               /*!<  Reserved,  Address offset:0x1134-0x1138 */
	__O  uint32_t ELR_INTR_CLR;                           /*!< ERROR LOG INTERRUPT CLEAR REGISTER,  Address offset: 0x113C */
} RXI300_TypeDef;
/** @} */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

/** @} */

/** @} */

#endif
