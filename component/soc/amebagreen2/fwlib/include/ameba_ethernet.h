/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL7005_ETHERNET_H_
#define _RL7005_ETHERNET_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup ETHERNET
  * @brief ETHERNET driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup ETHERNET_Register_Definitions ETHERNET Register Definitions
  * @{
  */

/** @defgroup ETH_IDR0
  * @brief
  * @{
  */
#define MASK_IDR0   ((u32)0x000000FF << 24)           /*!< R/W 0x0  The ID register0-5 are only permitted to write by 4-byte access. Read access can be byte, word, or double word access.*/
#define IDR0(x)     (((u32)((x) & 0x000000FF) << 24))
#define GET_IDR0(x) ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_IDR1   ((u32)0x000000FF << 16)           /*!< R/W 0x0  */
#define IDR1(x)     (((u32)((x) & 0x000000FF) << 16))
#define GET_IDR1(x) ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_IDR2   ((u32)0x000000FF << 8)            /*!< R/W 0x0  */
#define IDR2(x)     (((u32)((x) & 0x000000FF) << 8))
#define GET_IDR2(x) ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_IDR3   ((u32)0x000000FF << 0)            /*!< R/W 0x0  */
#define IDR3(x)     (((u32)((x) & 0x000000FF) << 0))
#define GET_IDR3(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_IDR4
  * @brief
  * @{
  */
#define MASK_IDR4   ((u32)0x000000FF << 24)           /*!< R/W 0x0  */
#define IDR4(x)     (((u32)((x) & 0x000000FF) << 24))
#define GET_IDR4(x) ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_IDR5   ((u32)0x000000FF << 16)           /*!< R/W 0x0  */
#define IDR5(x)     (((u32)((x) & 0x000000FF) << 16))
#define GET_IDR5(x) ((u32)(((x >> 16) & 0x000000FF)))
/** @} */

/** @defgroup ETH_MAR0
  * @brief
  * @{
  */
#define MASK_MAR0   ((u32)0x000000FF << 24)           /*!< R/W 0x0  Multicast Register 0, The MAR register0-7 are only permitted to write by 4-byte access. Read access can be byte, word, or double word access. Driver is responsible for initializing these registers. The MAR7-0 define 64 bits which is a bit wise index of the multicast function of multicast addresses. The hash function of multicast address is the upper 6 MSB's of the CRC32 of the address (destination). The index then is the numerical representation of those 6 bits in hex format.*/
#define MAR0(x)     (((u32)((x) & 0x000000FF) << 24))
#define GET_MAR0(x) ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_MAR1   ((u32)0x000000FF << 16)           /*!< R/W 0x0  Multicast Register 1*/
#define MAR1(x)     (((u32)((x) & 0x000000FF) << 16))
#define GET_MAR1(x) ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_MAR2   ((u32)0x000000FF << 8)            /*!< R/W 0x0  Multicast Register 2*/
#define MAR2(x)     (((u32)((x) & 0x000000FF) << 8))
#define GET_MAR2(x) ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_MAR3   ((u32)0x000000FF << 0)            /*!< R/W 0x0  Multicast Register 3*/
#define MAR3(x)     (((u32)((x) & 0x000000FF) << 0))
#define GET_MAR3(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_MAR4
  * @brief
  * @{
  */
#define MASK_MAR4   ((u32)0x000000FF << 24)           /*!< R/W 0x0  Multicast Register 4*/
#define MAR4(x)     (((u32)((x) & 0x000000FF) << 24))
#define GET_MAR4(x) ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_MAR5   ((u32)0x000000FF << 16)           /*!< R/W 0x0  Multicast Register 5*/
#define MAR5(x)     (((u32)((x) & 0x000000FF) << 16))
#define GET_MAR5(x) ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_MAR6   ((u32)0x000000FF << 8)            /*!< R/W 0x0  */
#define MAR6(x)     (((u32)((x) & 0x000000FF) << 8))
#define GET_MAR6(x) ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_MAR7   ((u32)0x000000FF << 0)            /*!< R/W 0x0  Multicast Register 7*/
#define MAR7(x)     (((u32)((x) & 0x000000FF) << 0))
#define GET_MAR7(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_TXOKCNT
  * @brief
  * @{
  */
#define MASK_TXOK   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of  Tx Ok packets.  Rolls over automatically.  Write to clear.*/
#define TXOK(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_TXOK(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_RXOKCNT
  * @brief
  * @{
  */
#define MASK_RXOK   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of Rx Ok packets.  Rolls over automatically.  Write to clear.*/
#define RXOK(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_RXOK(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_TXERR
  * @brief
  * @{
  */
#define MASK_TXERR   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of TxErr packets.  Rolls over automatically.  Write  to clear. The source of TxErr are txunder, txabt(in half-duplex mode) or txlinkerr.*/
#define TXERR(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_TXERR(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_RXERR
  * @brief
  * @{
  */
#define MASK_RXERR   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of RxErr packets. Rolls over automatically. Write to clear.  The source of RxErr are GMAC meets the following condition when rx fifo is not full.:  1.  RCR.accept runt = 0, but the ingress pkt length < 64B.  2.  RCR.accepterr = 0, but the ingress pkt is a CRC error pkt.*/
#define RXERR(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_RXERR(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_MISSPKT
  * @brief
  * @{
  */
#define MASK_MISSPKT   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of Missed packets.  Rolls over automatically.  Write to clear.*/
#define MISSPKT(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_MISSPKT(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_FAE
  * @brief
  * @{
  */
#define MASK_FAE   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of FAE (Frame Alignment Error) packets.  Rolls over automatically.  Write to clear. RCR.AER should be high.  When FAE occurs in 10/100mode, the following thins happens:  *Rx status des.CRC error does not assert if the ingress(exclude alignment error nibble) if correct.  *RxErr_reg increased by 1.  *FAE_reg increased by 1  *Rx status des.RCDF does not asserts.  data length does not include alignment error nibble.*/
#define FAE(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_FAE(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_TX1COL
  * @brief
  * @{
  */
#define MASK_TX1COL   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of  TxCol packets.  Rolls over automatically.  Write to clear.  This only record which have entered just one collision before Tx OK*/
#define TX1COL(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_TX1COL(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_TXMCOL
  * @brief
  * @{
  */
#define MASK_TXMCOL   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of TxCol packets.  Rolls over automatically.  Write to clear.  This keeps track of those packets with less than 16 collisions (or the configured retry count) before Tx Ok*/
#define TXMCOL(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_TXMCOL(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_RXOKPHY
  * @brief
  * @{
  */
#define MASK_RXOKPHY   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of Rx Ok packets with physical address matching destination address.  Rolls over automatically.  Write to clear.*/
#define RXOKPHY(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_RXOKPHY(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_RXOKBRD
  * @brief
  * @{
  */
#define MASK_RXOKBRD   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of Rx Ok packets with broadcast destination address..   Rolls over automatically.   Write to clear.*/
#define RXOKBRD(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_RXOKBRD(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_RXOKMU1
  * @brief
  * @{
  */
#define MASK_RXOKMUL   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of Rx Ok packets with multicast destination address.   Rolls over automatically.   Write to clear.*/
#define RXOKMUL(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_RXOKMUL(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_TXABT
  * @brief
  * @{
  */
#define MASK_TXABT   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of Tx aborted packets.  Rolls over automatically.   Write to clear.   This accounts for over collision, underrun, LNK failure conditions.*/
#define TXABT(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_TXABT(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_TXUNDERC
  * @brief
  * @{
  */
#define MASK_TXUNDERC   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of Tx Underrun packets. Rolls over automatically.  Write to clear.  gmac only.*/
#define TXUNDERC(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_TXUNDERC(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_MISSPKT_RDU
  * @brief
  * @{
  */
#define MASK_RDU_MISSPKTC   ((u16)0x0000FFFF << 0)           /*!< RWC 0x0  16-bit counter of miss packets due to rx descriptor is unavailable. Rolls over automatically.  Write to clear.  gmac only.*/
#define RDU_MISSPKTC(x)     (((u16)((x) & 0x0000FFFF) << 0))
#define GET_RDU_MISSPKTC(x) ((u16)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_TRSR
  * @brief
  * @{
  */
#define BIT_TOK            ((u32)0x00000001 << 3)           /*!< R   Transmit OK: Set to 1 indicates that the transmission of a packet was completed successfully and no transmit underrun occurs.*/
#define TOK(x)             ((u32)(((x) & 0x00000001) << 3))
#define GET_TOK(x)         ((u32)(((x >> 3) & 0x00000001)))
#define BIT_TXUNDER        ((u32)0x00000001 << 2)           /*!< R   Transmit FIFO Underrun: Set to 1 if the Tx FIFO was exhausted during the transmission of a packet.*/
#define TXUNDER(x)         ((u32)(((x) & 0x00000001) << 2))
#define GET_TXUNDER(x)     ((u32)(((x >> 2) & 0x00000001)))
#define BIT_RXFIFOEMPTY    ((u32)0x00000001 << 1)           /*!< R   Rx Fifo is Empty.*/
#define RXFIFOEMPTY(x)     ((u32)(((x) & 0x00000001) << 1))
#define GET_RXFIFOEMPTY(x) ((u32)(((x >> 1) & 0x00000001)))
#define BIT_RBU            ((u32)0x00000001 << 0)           /*!< R   Rx buff is un-available, rx full*/
#define RBU(x)             ((u32)(((x) & 0x00000001) << 0))
#define GET_RBU(x)         ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_CR
  * @brief
  * @{
  */
#define BIT_TDSC_DEBUG     ((u32)0x00000001 << 5)           /*!< R/W 0x0  Tx descriptor debug*/
#define TDSC_DEBUG(x)      ((u32)(((x) & 0x00000001) << 5))
#define GET_TDSC_DEBUG(x)  ((u32)(((x >> 5) & 0x00000001)))
#define BIT_DIS_LSO_ERR    ((u32)0x00000001 << 4)           /*!< R/W 0x0  Disable LSO error*/
#define DIS_LSO_ERR(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_DIS_LSO_ERR(x) ((u32)(((x >> 4) & 0x00000001)))
#define BIT_RXJUMBO        ((u32)0x00000001 << 3)           /*!< R/W 0x0  1: support jumbo packet receiving.    The maximum packet size is 16384B(0x4000).  0: Does not support jumbo packet receiving.     When rxjumbo sets to 1, the minium rx descriptor ring size is 16.*/
#define RXJUMBO(x)         ((u32)(((x) & 0x00000001) << 3))
#define GET_RXJUMBO(x)     ((u32)(((x >> 3) & 0x00000001)))
#define BIT_RXVLAN         ((u32)0x00000001 << 2)           /*!< R/W 0x0  Receive VLAN de-tagging enable.1: Enable. 0: Disable.  1: detag vlan and set rx status dsc.tava = 1 when ingress packet has vlantag.  0: disable detag vlan and rx status dsc.tava = 0.*/
#define RXVLAN(x)          ((u32)(((x) & 0x00000001) << 2))
#define GET_RXVLAN(x)      ((u32)(((x >> 2) & 0x00000001)))
#define BIT_RXCHKSUM       ((u32)0x00000001 << 1)           /*!< R/W 0x0  Receive checksum offload enable. 1: Enable. 0: Disable.*/
#define RXCHKSUM(x)        ((u32)(((x) & 0x00000001) << 1))
#define GET_RXCHKSUM(x)    ((u32)(((x >> 1) & 0x00000001)))
#define BIT_RST            ((u32)0x00000001 << 0)           /*!< W 0x0  Reset: Setting to 1 forces the Ethernet module to a software reset state which disables the transmitter and receiver, reinitializes the FIFOs, triggers interrupt Swint for RISC to reset the system buffer pointer to the initial value Tx/Rx FDP. The values of IDR0-5 and MAR0-7 will have no changes. This bit is 1 during the reset operation, and is cleared to 0 by the Ethernet Module when the reset operation is complete.*/
#define RST(x)             ((u32)(((x) & 0x00000001) << 0))
#define GET_RST(x)         ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_ISR_AND_IMR
  * @brief
  * @{
  */
#define BIT_IMR_RDU6        ((u32)0x00000001 << 31)           /*!< R/W   */
#define IMR_RDU6(x)         ((u32)(((x) & 0x00000001) << 31))
#define GET_IMR_RDU6(x)     ((u32)(((x >> 31) & 0x00000001)))
#define BIT_IMR_RDU5        ((u32)0x00000001 << 30)           /*!< R/W   */
#define IMR_RDU5(x)         ((u32)(((x) & 0x00000001) << 30))
#define GET_IMR_RDU5(x)     ((u32)(((x >> 30) & 0x00000001)))
#define BIT_IMR_RDU4        ((u32)0x00000001 << 29)           /*!< R/W   */
#define IMR_RDU4(x)         ((u32)(((x) & 0x00000001) << 29))
#define GET_IMR_RDU4(x)     ((u32)(((x >> 29) & 0x00000001)))
#define BIT_IMR_RDU3        ((u32)0x00000001 << 28)           /*!< R/W   */
#define IMR_RDU3(x)         ((u32)(((x) & 0x00000001) << 28))
#define GET_IMR_RDU3(x)     ((u32)(((x >> 28) & 0x00000001)))
#define BIT_IMR_RDU2        ((u32)0x00000001 << 27)           /*!< R/W   */
#define IMR_RDU2(x)         ((u32)(((x) & 0x00000001) << 27))
#define GET_IMR_RDU2(x)     ((u32)(((x >> 27) & 0x00000001)))
#define BIT_IMR_SWINT       ((u32)0x00000001 << 26)           /*!< R/W   */
#define IMR_SWINT(x)        ((u32)(((x) & 0x00000001) << 26))
#define GET_IMR_SWINT(x)    ((u32)(((x >> 26) & 0x00000001)))
#define BIT_IMR_TDU         ((u32)0x00000001 << 25)           /*!< R/W   */
#define IMR_TDU(x)          ((u32)(((x) & 0x00000001) << 25))
#define GET_IMR_TDU(x)      ((u32)(((x >> 25) & 0x00000001)))
#define BIT_IMR_LINKCHG     ((u32)0x00000001 << 24)           /*!< R/W   */
#define IMR_LINKCHG(x)      ((u32)(((x) & 0x00000001) << 24))
#define GET_IMR_LINKCHG(x)  ((u32)(((x >> 24) & 0x00000001)))
#define BIT_IMR_TER         ((u32)0x00000001 << 23)           /*!< R/W   */
#define IMR_TER(x)          ((u32)(((x) & 0x00000001) << 23))
#define GET_IMR_TER(x)      ((u32)(((x >> 23) & 0x00000001)))
#define BIT_IMR_TOK_TI      ((u32)0x00000001 << 22)           /*!< R/W   */
#define IMR_TOK_TI(x)       ((u32)(((x) & 0x00000001) << 22))
#define GET_IMR_TOK_TI(x)   ((u32)(((x >> 22) & 0x00000001)))
#define BIT_IMR_RDU1        ((u32)0x00000001 << 21)           /*!< R/W   */
#define IMR_RDU1(x)         ((u32)(((x) & 0x00000001) << 21))
#define GET_IMR_RDU1(x)     ((u32)(((x >> 21) & 0x00000001)))
#define BIT_IMR_RER_OVF     ((u32)0x00000001 << 20)           /*!< R/W   */
#define IMR_RER_OVF(x)      ((u32)(((x) & 0x00000001) << 20))
#define GET_IMR_RER_OVF(x)  ((u32)(((x >> 20) & 0x00000001)))
#define BIT_IMR_RER_RUNT    ((u32)0x00000001 << 18)           /*!< R/W   */
#define IMR_RER_RUNT(x)     ((u32)(((x) & 0x00000001) << 18))
#define GET_IMR_RER_RUNT(x) ((u32)(((x >> 18) & 0x00000001)))
#define BIT_IMR_CNT_WRAP    ((u32)0x00000001 << 17)           /*!< R/W   */
#define IMR_CNT_WRAP(x)     ((u32)(((x) & 0x00000001) << 17))
#define GET_IMR_CNT_WRAP(x) ((u32)(((x >> 17) & 0x00000001)))
#define BIT_IMR_ROK         ((u32)0x00000001 << 16)           /*!< R/W   */
#define IMR_ROK(x)          ((u32)(((x) & 0x00000001) << 16))
#define GET_IMR_ROK(x)      ((u32)(((x >> 16) & 0x00000001)))
#define BIT_ISR_RDU6        ((u32)0x00000001 << 15)           /*!< RWC   Rx Descriptor Unavailable for RING6:  When set, indicates Rx descriptor is unavailable.  Write 1 to clear.*/
#define ISR_RDU6(x)         ((u32)(((x) & 0x00000001) << 15))
#define GET_ISR_RDU6(x)     ((u32)(((x >> 15) & 0x00000001)))
#define BIT_ISR_RDU5        ((u32)0x00000001 << 14)           /*!< RWC   Rx Descriptor Unavailable for RING5:  When set, indicates Rx descriptor is unavailable.  Write 1 to clear.*/
#define ISR_RDU5(x)         ((u32)(((x) & 0x00000001) << 14))
#define GET_ISR_RDU5(x)     ((u32)(((x >> 14) & 0x00000001)))
#define BIT_ISR_RDU4        ((u32)0x00000001 << 13)           /*!< RWC   Rx Descriptor Unavailable for RING4:  When set, indicates Rx descriptor is unavailable.  Write 1 to clear.*/
#define ISR_RDU4(x)         ((u32)(((x) & 0x00000001) << 13))
#define GET_ISR_RDU4(x)     ((u32)(((x >> 13) & 0x00000001)))
#define BIT_ISR_RDU3        ((u32)0x00000001 << 12)           /*!< RWC   Rx Descriptor Unavailable for RING3:  When set, indicates Rx descriptor is unavailable.  Write 1 to clear.*/
#define ISR_RDU3(x)         ((u32)(((x) & 0x00000001) << 12))
#define GET_ISR_RDU3(x)     ((u32)(((x >> 12) & 0x00000001)))
#define BIT_ISR_RDU2        ((u32)0x00000001 << 11)           /*!< RWC   Rx Descriptor Unavailable for RING2:  When set, indicates Rx descriptor is unavailable.  Write 1 to clear.*/
#define ISR_RDU2(x)         ((u32)(((x) & 0x00000001) << 11))
#define GET_ISR_RDU2(x)     ((u32)(((x >> 11) & 0x00000001)))
#define BIT_ISR_SWINT       ((u32)0x00000001 << 10)           /*!< RWC   Software Interrupt pending:  When set to 1 indicates a software interrupt was forced.   Write 1 to clear.*/
#define ISR_SWINT(x)        ((u32)(((x) & 0x00000001) << 10))
#define GET_ISR_SWINT(x)    ((u32)(((x >> 10) & 0x00000001)))
#define BIT_ISR_TDU         ((u32)0x00000001 << 9)            /*!< RWC   Tx Descriptor Unavailable:  When set, indicates Tx descriptor is unavailable.  Write 1 to clear.*/
#define ISR_TDU(x)          ((u32)(((x) & 0x00000001) << 9))
#define GET_ISR_TDU(x)      ((u32)(((x >> 9) & 0x00000001)))
#define BIT_ISR_LINKCHG     ((u32)0x00000001 << 8)            /*!< RWC   Link Change:   Write 1 to clear.*/
#define ISR_LINKCHG(x)      ((u32)(((x) & 0x00000001) << 8))
#define GET_ISR_LINKCHG(x)  ((u32)(((x >> 8) & 0x00000001)))
#define BIT_ISR_TER         ((u32)0x00000001 << 7)            /*!< RWC   Transmit (Tx) Error:  Indicates that a packet transmission was aborted, due to excessive collisions, according to the TXRR's setting.   Write 1 to clear.*/
#define ISR_TER(x)          ((u32)(((x) & 0x00000001) << 7))
#define GET_ISR_TER(x)      ((u32)(((x >> 7) & 0x00000001)))
#define BIT_ISR_TOK_TI      ((u32)0x00000001 << 6)            /*!< RWC   Transmit Interrupt:   Indicates that the DMA of the last descriptor of TxIntMitigation number of Tx packet has completed and the last descriptor has been closed.  Write 1 to clear.*/
#define ISR_TOK_TI(x)       ((u32)(((x) & 0x00000001) << 6))
#define GET_ISR_TOK_TI(x)   ((u32)(((x >> 6) & 0x00000001)))
#define BIT_ISR_RDU1        ((u32)0x00000001 << 5)            /*!< RWC   Rx Descriptor Unavailable for RING1:  When set, indicates Rx descriptor is unavailable.  Write 1 to clear.*/
#define ISR_RDU1(x)         ((u32)(((x) & 0x00000001) << 5))
#define GET_ISR_RDU1(x)     ((u32)(((x >> 5) & 0x00000001)))
#define BIT_ISR_RER_OVF     ((u32)0x00000001 << 4)            /*!< RWC   Rx FIFO Overflow, caused by RBO/RDU, poor system bus (Lexra bus) performance, or overloaded Lexra bus traffic.  Write 1 to clear.*/
#define ISR_RER_OVF(x)      ((u32)(((x) & 0x00000001) << 4))
#define GET_ISR_RER_OVF(x)  ((u32)(((x >> 4) & 0x00000001)))
#define BIT_ISR_RER_RUNT    ((u32)0x00000001 << 2)            /*!< RWC   Rx error caused by runt error characterized by the frame length in bytes being less than 64 bytes. Write 1 to clear.*/
#define ISR_RER_RUNT(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_ISR_RER_RUNT(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_ISR_CNT_WRAP    ((u32)0x00000001 << 1)            /*!< RWC   Tx/rx counter wrap interrupt.*/
#define ISR_CNT_WRAP(x)     ((u32)(((x) & 0x00000001) << 1))
#define GET_ISR_CNT_WRAP(x) ((u32)(((x >> 1) & 0x00000001)))
#define BIT_ISR_ROK         ((u32)0x00000001 << 0)            /*!< RWC   Receive (Rx) OK:  This interrupt is set either when RxIntMitigation packet is met or RxPktTimer expires. Write 1 to clear.*/
#define ISR_ROK(x)          ((u32)(((x) & 0x00000001) << 0))
#define GET_ISR_ROK(x)      ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_TCR
  * @brief
  * @{
  */
#define MASK_IFG2_0   ((u32)0x00000007 << 10)           /*!< R/W 0x0  InterFrameGap Time: This field allows the user to adjust the interframe gap time longer than the standard: 9.6 us for 10Mbps, 960 ns for 100Mbps. The time can be programmed from 9.6 us to 14.4 us (10Mbps) and 960ns to 1440ns (100Mbps).  The formula for the inter frame gap is:  IFG[2:0]    IFGx1000MHz (nS)   IFGx100MHz (nS)  IFGx10MHz (uS)  0  1  1        96                           960                         9.6  1  0  0        96 +8 * 1                 960 +8 * 10              9.6 +8 * 0.1  1  0  1        96 + 16 * 1               960 + 16 * 10           9.6 + 16 * 0.1  1  1  0        96 + 24 * 1               960 + 24 * 10           9.6 + 24 * 0.1  1  1  1        96 + 32 * 1               960 + 32 * 10           9.6 + 32 * 0.1  0  0  0        96 + 40 * 1               960 + 40 * 10           9.6 + 40 * 0.1  0  0  1        96 + 48 * 1               960 + 48 * 10           9.6 + 48 * 0.1  0  1  0        96 + 96 * 1               960 + 96 * 01           9.6 + 96 * 0.1*/
#define IFG2_0(x)     (((u32)((x) & 0x00000007) << 10))
#define GET_IFG2_0(x) ((u32)(((x >> 10) & 0x00000007)))
#define MASK_LBK      ((u32)0x00000003 << 8)            /*!< R/W 0x0  The loopback function must be independent of the link state.  2'b00 : normal operation  2'b01 : Loopback mode_R2T(lbkmode_r2t) :      Lbk methld: no SW requirement.                1. receive ingress data.                2. assign rxd to txd (txd).                3. egress pkt. b10 : Reserved  2'b11 : Loopback mode(lbk_mode).        Lbk method: 1. rx fet descriptor.                            2. tx fet descriptor and data                           3. put data in txfifo                           4. assign txd to rxd.(lbk)                           5. gmac mv data to lxb.  In this mode(lbk_mode), the recommended value of RC_REG(0x44) is 0xf.*/
#define LBK(x)        (((u32)((x) & 0x00000003) << 8))
#define GET_LBK(x)    ((u32)(((x >> 8) & 0x00000003)))
/** @} */

/** @defgroup ETH_RCR
  * @brief
  * @{
  */
#define BIT_HOMEPNA    ((u32)0x00000001 << 7)           /*!< R/W 1'b0  Home PNA mode*/
#define HOMEPNA(x)     ((u32)(((x) & 0x00000001) << 7))
#define GET_HOMEPNA(x) ((u32)(((x >> 7) & 0x00000001)))
#define BIT_AFLOW      ((u32)0x00000001 << 6)           /*!< R/W 1'b0  Accept flow control :  When set to 1, flow control packet will also be received & DMA to rx buffer for debug. Default is 0.*/
#define AFLOW(x)       ((u32)(((x) & 0x00000001) << 6))
#define GET_AFLOW(x)   ((u32)(((x >> 6) & 0x00000001)))
#define BIT_AER        ((u32)0x00000001 << 5)           /*!< R/W 1'b0  Accept Error Packet:   When set to 1, all packets with CRC error, alignment error, and/or collided fragments will be accepted.       Rxerrc keeps,       Rxokc +1,       Rx status dsc.CRC =1,       Rx status dsc.RCDF =0.  When set to 0, all packets with CRC error, alignment error, and/or collided fragments will be rejected.       Rxerrc +1,       Rxokc keeps,       Rx status dsc.CRC =0,       Rx status dsc.RCDF =1.*/
#define AER(x)         ((u32)(((x) & 0x00000001) << 5))
#define GET_AER(x)     ((u32)(((x >> 5) & 0x00000001)))
#define BIT_AR         ((u32)0x00000001 << 4)           /*!< R/W 1'b0  Accept Runt: This bit allows the receiver to accept packets that are smaller than 64 bytes. The packet must be at least 8 bytes long to be accepted as a runt. Set to 1 to accept runt packets.*/
#define AR(x)          ((u32)(((x) & 0x00000001) << 4))
#define GET_AR(x)      ((u32)(((x >> 4) & 0x00000001)))
#define BIT_AB         ((u32)0x00000001 << 3)           /*!< R/W 1'b0  Set to 1 to accept broadcast packets, 0 to reject.*/
#define AB(x)          ((u32)(((x) & 0x00000001) << 3))
#define GET_AB(x)      ((u32)(((x >> 3) & 0x00000001)))
#define BIT_AM         ((u32)0x00000001 << 2)           /*!< R/W 1'b0  Set to 1 to accept multicast packets, 0 to reject. RC_REG.AB and AM should be enabled simulatanously.*/
#define AM(x)          ((u32)(((x) & 0x00000001) << 2))
#define GET_AM(x)      ((u32)(((x >> 2) & 0x00000001)))
#define BIT_APM        ((u32)0x00000001 << 1)           /*!< R/W 1'b0  Set to 1 to accept physical match packets, 0 to reject.*/
#define APM(x)         ((u32)(((x) & 0x00000001) << 1))
#define GET_APM(x)     ((u32)(((x >> 1) & 0x00000001)))
#define BIT_AAP        ((u32)0x00000001 << 0)           /*!< R/W 1'b0  Set to 1 to accept all packets with physical destination address, 0 to reject.*/
#define AAP(x)         ((u32)(((x) & 0x00000001) << 0))
#define GET_AAP(x)     ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_CPUTAG
  * @brief
  * @{
  */
#define BIT_CTEN_RX         ((u32)0x00000001 << 31)           /*!< R/W 0x0  Enable parsing ingress packet with cputag format.*/
#define CTEN_RX(x)          ((u32)(((x) & 0x00000001) << 31))
#define GET_CTEN_RX(x)      ((u32)(((x >> 31) & 0x00000001)))
#define MASK_CT_TSIZE       ((u32)0x0000000F << 27)           /*!< R/W 0x0  Cputag size in egress pkt.  CTT_size: 4'h0: 0 Byte                4'h1: 4 Byte                4'h2: 8Byte                4'h3: 12 Byte*/
#define CT_TSIZE(x)         (((u32)((x) & 0x0000000F) << 27))
#define GET_CT_TSIZE(x)     ((u32)(((x >> 27) & 0x0000000F)))
#define MASK_CT_RSIZE_3_2   ((u32)0x00000003 << 25)           /*!< R/W 0x0  After rl8681 (including), this field is only for ingress pkt.  CTR_size:  4'h0: 0 Byte                  4'h1: 4 Byte                  4'h2: 8Byte F or Apollo cputag with PTP timestamp, set CT_RSIZE to h2.*/
#define CT_RSIZE_3_2(x)     (((u32)((x) & 0x00000003) << 25))
#define GET_CT_RSIZE_3_2(x) ((u32)(((x >> 25) & 0x00000003)))
#define BIT_CT_DSLRN        ((u32)0x00000001 << 24)           /*!< R/W 0x1  In rtl8370s, Setting of Disable Learning field in tx cputag.  Setting of Disable Learning field in RTL8368 or DSLRN field in RTL8307h tx cputag when short_dsc_format = 0.*/
#define CT_DSLRN(x)         ((u32)(((x) & 0x00000001) << 24))
#define GET_CT_DSLRN(x)     ((u32)(((x >> 24) & 0x00000001)))
#define BIT_CT_NORMK        ((u32)0x00000001 << 23)           /*!< R/W 0x0  Setting of NORMK field in RTL8307h tx cputag when short_dsc_format = 0.*/
#define CT_NORMK(x)         ((u32)(((x) & 0x00000001) << 23))
#define GET_CT_NORMK(x)     ((u32)(((x >> 23) & 0x00000001)))
#define BIT_CT_ASPRI        ((u32)0x00000001 << 22)           /*!< R/W 0x1  In RTL8307h, setting ASPRI field in tx cputag when short_dsc_format = 0.  In RTL8370S, setting priority select field in tx cputag when short_dsc_format =0..*/
#define CT_ASPRI(x)         ((u32)(((x) & 0x00000001) << 22))
#define GET_CT_ASPRI(x)     ((u32)(((x >> 22) & 0x00000001)))
#define MASK_CT_SWITCH      ((u32)0x0000000F << 18)           /*!< R/W 0x0  Support cputag format of switch  0: no cputag support  1: 8368  2: 8306  3: 8307  4: 8370  5: gmac in 8681.  6: gmac in Apollo,  rle0371, rl6266 and   rtl9601(rle0639)*/
#define CT_SWITCH(x)        (((u32)((x) & 0x0000000F) << 18))
#define GET_CT_SWITCH(x)    ((u32)(((x >> 18) & 0x0000000F)))
#define MASK_CT_RSIZE_1_0   ((u32)0x00000003 << 16)           /*!< R/W 0x0  */
#define CT_RSIZE_1_0(x)     (((u32)((x) & 0x00000003) << 16))
#define GET_CT_RSIZE_1_0(x) ((u32)(((x >> 16) & 0x00000003)))
#define MASK_CTPM           ((u32)0x000000FF << 8)            /*!< R/W 0x0  CPU tag protocol mask.  8306:0xf0  8368:0xe0  8370:0xff  8307h:0xff*/
#define CTPM(x)             (((u32)((x) & 0x000000FF) << 8))
#define GET_CTPM(x)         ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_CTPV           ((u32)0x000000FF << 0)            /*!< R/W 0x0  CPU tag protocol value.  8306:0x90  8368:0xa0 or 0xb0  8370:0x04  8307h:0x04*/
#define CTPV(x)             (((u32)((x) & 0x000000FF) << 0))
#define GET_CTPV(x)         ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_CONFIG
  * @brief
  * @{
  */
#define MASK_RFF_SIZE_SEL      ((u32)0x00000003 << 28)           /*!< R/W 2'b10  Set gmac_rxfifo size.  2'b00: 1KB  2'b01: 1664B  2'b10: 2KB*/
#define RFF_SIZE_SEL(x)        (((u32)((x) & 0x00000003) << 28))
#define GET_RFF_SIZE_SEL(x)    ((u32)(((x >> 28) & 0x00000003)))
#define BIT_R_TSO_ID_SEL       ((u32)0x00000001 << 27)           /*!< R/W 1'b0  0: each ID of IP header is same as original ID.  1: increment 1 from original ID. Default value is 0.*/
#define R_TSO_ID_SEL(x)        ((u32)(((x) & 0x00000001) << 27))
#define GET_R_TSO_ID_SEL(x)    ((u32)(((x >> 27) & 0x00000001)))
#define BIT_R_MDSLV_OUTEDGE    ((u32)0x00000001 << 26)           /*!< R/W 0x0  this bit works only  in phy_mode 0: change mdio output data in falling edge  1: change mdio output data in rising edge*/
#define R_MDSLV_OUTEDGE(x)     ((u32)(((x) & 0x00000001) << 26))
#define GET_R_MDSLV_OUTEDGE(x) ((u32)(((x >> 26) & 0x00000001)))
#define BIT_EN_INT_ROUTE       ((u32)0x00000001 << 25)           /*!< R/W 0x0  0: disable interrupt routing, IMR0_reg, IMR1_reg.  1: enable interrupt routing, IMR0_reg, IMR1_reg.*/
#define EN_INT_ROUTE(x)        ((u32)(((x) & 0x00000001) << 25))
#define GET_EN_INT_ROUTE(x)    ((u32)(((x >> 25) & 0x00000001)))
#define BIT_EN_INT_SPLIT       ((u32)0x00000001 << 24)           /*!< R/W 0x0  When, en_int_route = 0, 0: only int.tdu, tok and rok present.  1: IMR0_reg, IMR1_reg and ISR1_reg present.    When en_int_route = 1, this field is 1.*/
#define EN_INT_SPLIT(x)        ((u32)(((x) & 0x00000001) << 24))
#define GET_EN_INT_SPLIT(x)    ((u32)(((x >> 24) & 0x00000001)))
/** @} */

/** @defgroup ETH_CPUTAG1
  * @brief
  * @{
  */
#define MASK_SPA_DSL   ((u32)0x00000007 << 4)           /*!< R/W 0x0  */
#define SPA_DSL(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_SPA_DSL(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_SPA_PON   ((u32)0x00000007 << 0)           /*!< R/W 0x0  The ingress cputag.SPA=PON field. Used in Apollo cputag.*/
#define SPA_PON(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_SPA_PON(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_MSR
  * @brief
  * @{
  */
#define BIT_FORCE_TRXFCE           ((u32)0x00000001 << 31)           /*!< R/W 1'b0  Force Tx/RX Flow Control:   1 = enabled Flow control in the absence of NWAY.  0 = disables Flow control in the absence of NWAY.  Version effect: rl6166(ECO) and after.*/
#define FORCE_TRXFCE(x)            ((u32)(((x) & 0x00000001) << 31))
#define GET_FORCE_TRXFCE(x)        ((u32)(((x >> 31) & 0x00000001)))
#define BIT_RXFCE                  ((u32)0x00000001 << 30)           /*!< R/W 1'b0  RX Flow control Enable: The flow control is enabled in full-duplex mode only.  Packets are dropped if buffer is exhausted.  Default is 0.    1 = Rx Flow Control Enabled.  0 = Rx Flow Control Disabled.*/
#define RXFCE(x)                   ((u32)(((x) & 0x00000001) << 30))
#define GET_RXFCE(x)               ((u32)(((x >> 30) & 0x00000001)))
#define BIT_TXFCE                  ((u32)0x00000001 << 29)           /*!< R/W 1'b0  Tx Flow Control Enable:  1 = tx flow control enabled.  0 = tx flow control disabled.  ACCEPT ERROR MUST NOT BE ENABLED*/
#define TXFCE(x)                   ((u32)(((x) & 0x00000001) << 29))
#define GET_TXFCE(x)               ((u32)(((x >> 29) & 0x00000001)))
#define MASK_SPEED                 ((u32)0x00000003 << 27)           /*!< R   {SPEED_1000, SPEED_10}  00: 100Mbps,    01: 10Mbps 10: 1000Mbps,   11: not allowed*/
#define SPEED(x)                   (((u32)((x) & 0x00000003) << 27))
#define GET_SPEED(x)               ((u32)(((x >> 27) & 0x00000003)))
#define BIT_LINKB                  ((u32)0x00000001 << 26)           /*!< R   Inverse of Link status.  0 = Link OK.  1 = Link Fail.*/
#define LINKB(x)                   ((u32)(((x) & 0x00000001) << 26))
#define GET_LINKB(x)               ((u32)(((x >> 26) & 0x00000001)))
#define BIT_TXPF                   ((u32)0x00000001 << 25)           /*!< R   Tx Pause frame:   1 = Ethernet module has sent a pause packet.   0 = the Ethernet module has sent a timer done packet.*/
#define TXPF(x)                    ((u32)(((x) & 0x00000001) << 25))
#define GET_TXPF(x)                ((u32)(((x >> 25) & 0x00000001)))
#define BIT_RXPF                   ((u32)0x00000001 << 24)           /*!< R   Pause Flag:   1 =  Ethernet module is in backoff state because a pause packet received.   0 = pause state is clear.*/
#define RXPF(x)                    ((u32)(((x) & 0x00000001) << 24))
#define GET_RXPF(x)                ((u32)(((x >> 24) & 0x00000001)))
#define BIT_SEL_RGMII              ((u32)0x00000001 << 23)           /*!< R/W 0x0  gmac_sel_rgmii*/
#define SEL_RGMII(x)               ((u32)(((x) & 0x00000001) << 23))
#define GET_SEL_RGMII(x)           ((u32)(((x >> 23) & 0x00000001)))
#define BIT_FULLDUPREG             ((u32)0x00000001 << 22)           /*!< R 0x0  Indicates Full duplex mode from mdio results.*/
#define FULLDUPREG(x)              ((u32)(((x) & 0x00000001) << 22))
#define GET_FULLDUPREG(x)          ((u32)(((x >> 22) & 0x00000001)))
#define BIT_NWCOMPLETE             ((u32)0x00000001 << 21)           /*!< R 0x0  Nway complete*/
#define NWCOMPLETE(x)              ((u32)(((x) & 0x00000001) << 21))
#define GET_NWCOMPLETE(x)          ((u32)(((x >> 21) & 0x00000001)))
#define BIT_SEL_MII                ((u32)0x00000001 << 20)           /*!< R 0x0  indicates in mii mode.*/
#define SEL_MII(x)                 ((u32)(((x) & 0x00000001) << 20))
#define GET_SEL_MII(x)             ((u32)(((x >> 20) & 0x00000001)))
#define BIT_FORCEDFULLDUP          ((u32)0x00000001 << 19)           /*!< R/W 0x0  force gmac operates at full duplex mode.   This bit is Write only in RLE0315.  This bit is R/W in RLE0390 (原RL6166).  1'b1: force gmac in full duplex mode.  1'b0: duplex status is from MDIO auto-polling.  Not means gmac is in half duplex mode.*/
#define FORCEDFULLDUP(x)           ((u32)(((x) & 0x00000001) << 19))
#define GET_FORCEDFULLDUP(x)       ((u32)(((x >> 19) & 0x00000001)))
#define BIT_FORCELINK              ((u32)0x00000001 << 18)           /*!< R/W 0x0  force gmac in link ok mode.   This bit is Write only in RLE0315.  This bit is R/W in RLE0390 (RL6166).  1'b1: force gmac in linkok.  1'b0: link status is from MDIO auto-polling.*/
#define FORCELINK(x)               ((u32)(((x) & 0x00000001) << 18))
#define GET_FORCELINK(x)           ((u32)(((x >> 18) & 0x00000001)))
#define MASK_FORCE_SPD             ((u32)0x00000003 << 16)           /*!< R/W 2'b11  Force gmac in 10/100/GIGA mode.  2'b00: 100M   2'b01: 10M  2'b10: GIGA  2'b11: not force mode.  These bits are Write only in RLE0390 (RL6166). The current speed status of gmac can be checked in MSR_reg.{speed1000, speed_10}.   In force mode, SW can set gmac operates in , FD and Link ok(for example) and SW can not set gmac operates in , HD and Link ok. When FORCEDFULLDUP is b0, the duplex status is from MDIO auto-polling, therefore, it is not controlled by SW.*/
#define FORCE_SPD(x)               (((u32)((x) & 0x00000003) << 16))
#define GET_FORCE_SPD(x)           ((u32)(((x >> 16) & 0x00000003)))
#define BIT_GMAC_SEL_PHYIF_0       ((u32)0x00000001 << 15)           /*!< R 0x1  1: phy interface 0.works.  0: phy interface 1 works.*/
#define GMAC_SEL_PHYIF_0(x)        ((u32)(((x) & 0x00000001) << 15))
#define GET_GMAC_SEL_PHYIF_0(x)    ((u32)(((x >> 15) & 0x00000001)))
#define BIT_GMAC_PHY_MODE          ((u32)0x00000001 << 13)           /*!< R/W 0x0  1: in phy mode. (bit[6] will be read as '0')  0: not in phy mode. (bit[6] will be read as '1')*/
#define GMAC_PHY_MODE(x)           ((u32)(((x) & 0x00000001) << 13))
#define GET_GMAC_PHY_MODE(x)       ((u32)(((x >> 13) & 0x00000001)))
#define BIT_RGMII_RX_DECODE_STS    ((u32)0x00000001 << 12)           /*!< R/W 0x0  0: Does not support rgmii in band status(link status, speed and duplex mode of the PHY) by decoding rxd.  1: Supports rgmii in band status(link status, speed and duplex mode of the PHY) by decoding rxd.*/
#define RGMII_RX_DECODE_STS(x)     ((u32)(((x) & 0x00000001) << 12))
#define GET_RGMII_RX_DECODE_STS(x) ((u32)(((x >> 12) & 0x00000001)))
#define BIT_RGMII_TX_ENCODE_STS    ((u32)0x00000001 << 11)           /*!< R/W 0x0  This field is valid only in phy mode.   0: Does not support rgmii in band status(link status, speed and duplex mode of the PHY) by encoding txd.  1: Supports rgmii in band status(link status, speed and duplex mode of the PHY) by encoding txd.*/
#define RGMII_TX_ENCODE_STS(x)     ((u32)(((x) & 0x00000001) << 11))
#define GET_RGMII_TX_ENCODE_STS(x) ((u32)(((x >> 11) & 0x00000001)))
#define BIT_FORCE_SPD_MODE         ((u32)0x00000001 << 10)           /*!< R/W 0x0  1: gmac is in force speed mode. The real speed in force mode is set in MS_REG.FORCE_SPD.  0: gmac speed status is from md operation.*/
#define FORCE_SPD_MODE(x)          ((u32)(((x) & 0x00000001) << 10))
#define GET_FORCE_SPD_MODE(x)      ((u32)(((x >> 10) & 0x00000001)))
#define MASK_EEE_ABY               ((u32)0x00000003 << 8)            /*!< R 0x0  Indicates eee ability of PCS.  Eee_aby[0]: eee ability in 100M  Eee_aby[1]: eee ability in giga mode.*/
#define EEE_ABY(x)                 (((u32)((x) & 0x00000003) << 8))
#define GET_EEE_ABY(x)             ((u32)(((x >> 8) & 0x00000003)))
#define BIT_REG_RMII2MII_EN        ((u32)0x00000001 << 7)            /*!< R/W 1'b0  1: transfer MII signals to RMII signals  This function is only valid in MII mode.  (default 0: for AmebaI//AmebaPro/AmebaPro2)*/
#define REG_RMII2MII_EN(x)         ((u32)(((x) & 0x00000001) << 7))
#define GET_REG_RMII2MII_EN(x)     ((u32)(((x >> 7) & 0x00000001)))
#define BIT_GMAC_MD_MST            ((u32)0x00000001 << 6)            /*!< R 0x1  1: GMAC is used to be MDIO master. (When bit[13] is setting as '0')  0: GMAC is used to be MDIO slave. (When bit[13] is setting as '1')*/
#define GMAC_MD_MST(x)             ((u32)(((x) & 0x00000001) << 6))
#define GET_GMAC_MD_MST(x)         ((u32)(((x >> 6) & 0x00000001)))
#define BIT_REFCLK_PHASE           ((u32)0x00000001 << 1)            /*!< R/W 0x0  1: for Tx transaction, TXEN & TXD signals are changed on the rising edge of REF_CLK; for Rx transaction, RXD signals are sampled on the rising edge of REF_CLK  0: for Tx transaction, TXEN & TXD signals are changed on the falling edge of REF_CLK; for Rx transaction, RXD signals are sampled on the falling edge of REF_CLK*/
#define REFCLK_PHASE(x)            ((u32)(((x) & 0x00000001) << 1))
#define GET_REFCLK_PHASE(x)        ((u32)(((x >> 1) & 0x00000001)))
#define BIT_REFCLK_ON              ((u32)0x00000001 << 0)            /*!< R/W 0x1  1: REF_CLK on  0: REF_CLK off*/
#define REFCLK_ON(x)               ((u32)(((x) & 0x00000001) << 0))
#define GET_REFCLK_ON(x)           ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_MIIAR
  * @brief
  * @{
  */
#define BIT_FLAG                    ((u32)0x00000001 << 31)           /*!< R/W 0x0  Flag bit, used to identify access to MII register:  1:  Write data to MII register.  Turns to 0 automatically upon completion of MAC writing to the specified MII register.  0:  Read data from MII register.  Turns to 1 automatically upon completion of MAC reading the specified MII register.    Read write turn around time I s about 64 us.*/
#define FLAG(x)                     ((u32)(((x) & 0x00000001) << 31))
#define GET_FLAG(x)                 ((u32)(((x >> 31) & 0x00000001)))
#define MASK_PHYADDRESS             ((u32)0x0000001F << 26)           /*!< R/W 0x0  Defines the Phy address for the MII.*/
#define PHYADDRESS(x)               (((u32)((x) & 0x0000001F) << 26))
#define GET_PHYADDRESS(x)           ((u32)(((x >> 26) & 0x0000001F)))
#define BIT_MDIO_BUSY               ((u32)0x00000001 << 25)           /*!< R   MDIO is busy*/
#define MDIO_BUSY(x)                ((u32)(((x) & 0x00000001) << 25))
#define GET_MDIO_BUSY(x)            ((u32)(((x >> 25) & 0x00000001)))
#define BIT_EN_EEEP_AUTO_WRITE      ((u32)0x00000001 << 23)           /*!< R/W 0x0  HW auto writes PCS.page4.reg20 and reg27 to adjust analog parameters based on eeep_lpi state.   SW should change page to page4 then set MIIA_REG.en_eeep_auto_write.  HW will write eeep_abig_on/off and eeep_ldvbias_on/off (in EEEP_CR6_REG and EEEP_CR7_REG) to PCS.page4.reg20 and reg27, respectively, based on eeep_lpi state.     If SW wants to access another page of PCS, SW should disable MIIA_REG.en_eeep_auto_write.*/
#define EN_EEEP_AUTO_WRITE(x)       ((u32)(((x) & 0x00000001) << 23))
#define GET_EN_EEEP_AUTO_WRITE(x)   ((u32)(((x >> 23) & 0x00000001)))
#define BIT_DISABLE_AUTO_POLLING    ((u32)0x00000001 << 22)           /*!< R/W 0x0  Disable auto polling feature of mdio operation.  0: HW auto polling PCS status.   1: HW does not auto polling PCS status.*/
#define DISABLE_AUTO_POLLING(x)     ((u32)(((x) & 0x00000001) << 22))
#define GET_DISABLE_AUTO_POLLING(x) ((u32)(((x >> 22) & 0x00000001)))
#define BIT_POLLING_EEE_REG         ((u32)0x00000001 << 21)           /*!< R/W 0x0  polling PCS EEE advertisement register (7.60). In rl6166(原rle0315) and before: this bit is not supported.  In rle0390: In in-direct method, gmac had to access PCS register by multiples R/W of reg13 and 14. It is OK for PCS to be aborted in the middle of indirect method.  In rle0371: HW will complete current indirect access to PCS then stop in-direct method.  Please refer to EEE_CR1_reg.*/
#define POLLING_EEE_REG(x)          ((u32)(((x) & 0x00000001) << 21))
#define GET_POLLING_EEE_REG(x)      ((u32)(((x >> 21) & 0x00000001)))
#define MASK_REGADDR4_0             ((u32)0x0000001F << 16)           /*!< R/W 0x0  Defines 5-bit MII register address.*/
#define REGADDR4_0(x)               (((u32)((x) & 0x0000001F) << 16))
#define GET_REGADDR4_0(x)           ((u32)(((x >> 16) & 0x0000001F)))
#define MASK_DATA15_0               ((u32)0x0000FFFF << 0)            /*!< R/W 0x0  Defines 16 bit MII resgister data.*/
#define DATA15_0(x)                 (((u32)((x) & 0x0000FFFF) << 0))
#define GET_DATA15_0(x)             ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_SWINT
  * @brief
  * @{
  */
#define BIT_SWINT    ((u32)0x00000001 << 24)           /*!< WA0   Write 1 will force ISR register bit 10 set to 1*/
#define SWINT(x)     ((u32)(((x) & 0x00000001) << 24))
#define GET_SWINT(x) ((u32)(((x >> 24) & 0x00000001)))
/** @} */

/** @defgroup ETH_VLAN_REG
  * @brief
  * @{
  */
#define MASK_STAG_PID         ((u32)0x0000FFFF << 16)           /*!< R/W 0x8  Set the s-tag protocol identifier. This field is valid only when COM_REG.TDSC_VLAN_TYPE is high.*/
#define STAG_PID(x)           (((u32)((x) & 0x0000FFFF) << 16))
#define GET_STAG_PID(x)       ((u32)(((x >> 16) & 0x0000FFFF)))
#define BIT_TDSC_VLAN_TYPE    ((u32)0x00000001 << 15)           /*!< R/W 0x0  1: tx command descriptor.tx_vlan_action is for s-tag.  0: tx command descriptor.tx_vlan_action is for c-tag.*/
#define TDSC_VLAN_TYPE(x)     ((u32)(((x) & 0x00000001) << 15))
#define GET_TDSC_VLAN_TYPE(x) ((u32)(((x >> 15) & 0x00000001)))
/** @} */

/** @defgroup ETH_LED_CTRL
  * @brief
  * @{
  */
#define BIT_EEE_EN_LED    ((u32)0x00000001 << 19)           /*!< R/W 0x0  Enable/disable lpi led.*/
#define EEE_EN_LED(x)     ((u32)(((x) & 0x00000001) << 19))
#define GET_EEE_EN_LED(x) ((u32)(((x >> 19) & 0x00000001)))
#define BIT_CUSTOM_LED    ((u32)0x00000001 << 18)           /*!< R/W 0x0  Custom led mode. 1: using custom led matrix table. 0: using Non-custom led table.*/
#define CUSTOM_LED(x)     ((u32)(((x) & 0x00000001) << 18))
#define GET_CUSTOM_LED(x) ((u32)(((x >> 18) & 0x00000001)))
#define MASK_LEDSEL       ((u32)0x00000003 << 16)           /*!< R/W 0x0  Non custom led select.  Following EEE spec, combine the Link signal and LPI signal in the same LED pin. While entering LPI state, LED will blink with  2784ms cycle, of which the first  2349ms LED light is turned off, after  435ms is turned on.   NO EEE in RL6166.*/
#define LEDSEL(x)         (((u32)((x) & 0x00000003) << 16))
#define GET_LEDSEL(x)     ((u32)(((x >> 16) & 0x00000003)))
#define MASK_LEDSEL3      ((u32)0x0000000F << 12)           /*!< R/W 0x0  Custom led3 select.*/
#define LEDSEL3(x)        (((u32)((x) & 0x0000000F) << 12))
#define GET_LEDSEL3(x)    ((u32)(((x >> 12) & 0x0000000F)))
#define MASK_LEDSEL2      ((u32)0x0000000F << 8)            /*!< R/W 0x0  Custom led2 select.*/
#define LEDSEL2(x)        (((u32)((x) & 0x0000000F) << 8))
#define GET_LEDSEL2(x)    ((u32)(((x >> 8) & 0x0000000F)))
#define MASK_LEDSEL1      ((u32)0x0000000F << 4)            /*!< R/W 0x0  Custom led1 select.*/
#define LEDSEL1(x)        (((u32)((x) & 0x0000000F) << 4))
#define GET_LEDSEL1(x)    ((u32)(((x >> 4) & 0x0000000F)))
#define MASK_LEDSEL0      ((u32)0x0000000F << 0)            /*!< R/W 0x0  Custom led0 select.*/
#define LEDSEL0(x)        (((u32)((x) & 0x0000000F) << 0))
#define GET_LEDSEL0(x)    ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ETH_BIST
  * @brief
  * @{
  */
#define BIT_CSR_RFF_TMCK    ((u32)0x00000001 << 14)           /*!< R/W 1'b0  Set TMCK for RXFIFO*/
#define CSR_RFF_TMCK(x)     ((u32)(((x) & 0x00000001) << 14))
#define GET_CSR_RFF_TMCK(x) ((u32)(((x >> 14) & 0x00000001)))
#define BIT_CSR_RFF_NAP     ((u32)0x00000001 << 13)           /*!< R/W 1'b0  Set NAP for RXFIFO*/
#define CSR_RFF_NAP(x)      ((u32)(((x) & 0x00000001) << 13))
#define GET_CSR_RFF_NAP(x)  ((u32)(((x >> 13) & 0x00000001)))
#define BIT_CSR_RFF_DVSE    ((u32)0x00000001 << 12)           /*!< R/W 1'b0  Set DVSE for RXFIFO*/
#define CSR_RFF_DVSE(x)     ((u32)(((x) & 0x00000001) << 12))
#define GET_CSR_RFF_DVSE(x) ((u32)(((x >> 12) & 0x00000001)))
#define MASK_CSR_RFF_DVS    ((u32)0x0000000F << 8)            /*!< R/W 4'b0000  Set DVS for RXFIFO*/
#define CSR_RFF_DVS(x)      (((u32)((x) & 0x0000000F) << 8))
#define GET_CSR_RFF_DVS(x)  ((u32)(((x >> 8) & 0x0000000F)))
#define BIT_CSR_TFF_TMCK    ((u32)0x00000001 << 6)            /*!< R/W 1'b0  Set TMCK for TXFIFO*/
#define CSR_TFF_TMCK(x)     ((u32)(((x) & 0x00000001) << 6))
#define GET_CSR_TFF_TMCK(x) ((u32)(((x >> 6) & 0x00000001)))
#define BIT_CSR_TFF_NAP     ((u32)0x00000001 << 5)            /*!< R/W 1'b0  Set NAP for TXFIFO*/
#define CSR_TFF_NAP(x)      ((u32)(((x) & 0x00000001) << 5))
#define GET_CSR_TFF_NAP(x)  ((u32)(((x >> 5) & 0x00000001)))
#define BIT_CSR_TFF_DVSE    ((u32)0x00000001 << 4)            /*!< R/W 1'b0  Set DVSE for TXFIFO*/
#define CSR_TFF_DVSE(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_CSR_TFF_DVSE(x) ((u32)(((x >> 4) & 0x00000001)))
#define MASK_CSR_TFF_DVS    ((u32)0x0000000F << 0)            /*!< R/W 4'b0000  Set DVS for TXFIFO*/
#define CSR_TFF_DVS(x)      (((u32)((x) & 0x0000000F) << 0))
#define GET_CSR_TFF_DVS(x)  ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ETH_PGLBK
  * @brief
  * @{
  */
#define BIT_EN_PGLBK                ((u32)0x00000001 << 15)           /*!< R/W 0x0  Enable packet generator mode*/
#define EN_PGLBK(x)                 ((u32)(((x) & 0x00000001) << 15))
#define GET_EN_PGLBK(x)             ((u32)(((x >> 15) & 0x00000001)))
#define BIT_PGLBK_DATA_PAT_SEL      ((u32)0x00000001 << 14)           /*!< R/W 0x0  1: fixed pattern, 0: pseudo random*/
#define PGLBK_DATA_PAT_SEL(x)       ((u32)(((x) & 0x00000001) << 14))
#define GET_PGLBK_DATA_PAT_SEL(x)   ((u32)(((x >> 14) & 0x00000001)))
#define BIT_PGLBK_DONE              ((u32)0x00000001 << 13)           /*!< R 0x0  packet generator in wire lbk mode status. 1: done*/
#define PGLBK_DONE(x)               ((u32)(((x) & 0x00000001) << 13))
#define GET_PGLBK_DONE(x)           ((u32)(((x >> 13) & 0x00000001)))
#define BIT_PGLBK_FAIL              ((u32)0x00000001 << 12)           /*!< R 0x0  packet generator in wire lbk mode status. 1: fail*/
#define PGLBK_FAIL(x)               ((u32)(((x) & 0x00000001) << 12))
#define GET_PGLBK_FAIL(x)           ((u32)(((x >> 12) & 0x00000001)))
#define BIT_PBLBK_PKT_LENGTH_SEL    ((u32)0x00000001 << 11)           /*!< R/W 0x0  0: random (range: 64-1504B), 1: 64B*/
#define PBLBK_PKT_LENGTH_SEL(x)     ((u32)(((x) & 0x00000001) << 11))
#define GET_PBLBK_PKT_LENGTH_SEL(x) ((u32)(((x >> 11) & 0x00000001)))
#define MASK_PGLBK_PKT_NUM_SEL      ((u32)0x00000003 << 9)            /*!< R/W 0x0  2'b00: 100 pkts,   2'b10: 5 pkts.  2'b01: 1 pkt  2'b11: reserved.*/
#define PGLBK_PKT_NUM_SEL(x)        (((u32)((x) & 0x00000003) << 9))
#define GET_PGLBK_PKT_NUM_SEL(x)    ((u32)(((x >> 9) & 0x00000003)))
#define BIT_R_DISABLE_TXCRC         ((u32)0x00000001 << 8)            /*!< R/W 0x0  1: not append CRC for egress packet.  0: append CRC for egress packet.*/
#define R_DISABLE_TXCRC(x)          ((u32)(((x) & 0x00000001) << 8))
#define GET_R_DISABLE_TXCRC(x)      ((u32)(((x >> 8) & 0x00000001)))
#define MASK_PGLBK_DATA_PAT         ((u32)0x000000FF << 0)            /*!< R/W 0x0  Pattern inside tx packet when pglbk_data_pat_sel is high. For RLE0516, only pglbk_data_pat[3:0] are used.*/
#define PGLBK_DATA_PAT(x)           (((u32)((x) & 0x000000FF) << 0))
#define GET_PGLBK_DATA_PAT(x)       ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_EEE_CR1
  * @brief
  * @{
  */
#define BIT_EN_FRC_EEE               ((u32)0x00000001 << 31)           /*!< R/W 0x0  Enable EEE force mode  0: Disable EEE FORCE Mode  1: Enable EEE FORCE Mode If EN_FRC_EEE=1, both FRC_EEE_Giga and FRC_EEE_100 are used to force MAC operation ability.*/
#define EN_FRC_EEE(x)                ((u32)(((x) & 0x00000001) << 31))
#define GET_EN_FRC_EEE(x)            ((u32)(((x >> 31) & 0x00000001)))
#define BIT_FRC_EEE_GIGA             ((u32)0x00000001 << 30)           /*!< R/W 0x0  If EN_FRC_EEE=1, FRC_EEE_Giga:   0: Disable MAC EEE ability for 1000BASE-T.  1: Enable MAC EEE ability for 1000BASE-T. If EN_FRC_EEE=0, FRC_EEE_Giga means EEE advertisement register 7.60 bit[2]*/
#define FRC_EEE_GIGA(x)              ((u32)(((x) & 0x00000001) << 30))
#define GET_FRC_EEE_GIGA(x)          ((u32)(((x >> 30) & 0x00000001)))
#define BIT_FRC_EEE                  ((u32)0x00000001 << 29)           /*!< R/W 0x0  If EN_FRC_EEE=1, FRC_EEE_100  0: Disable MAC EEE ability for 100BASE-TX.  1: Enable MAC EEE ability for 100BASE-TX. If EN_FRC_EEE=0, FRC_EEE_100 means EEE advertisement register 7.60 bit[1]*/
#define FRC_EEE(x)                   ((u32)(((x) & 0x00000001) << 29))
#define GET_FRC_EEE(x)               ((u32)(((x >> 29) & 0x00000001)))
#define BIT_EN_EEE                   ((u32)0x00000001 << 28)           /*!< R/W 0x0  Set same as PHY setting, for LED.*/
#define EN_EEE(x)                    ((u32)(((x) & 0x00000001) << 28))
#define GET_EN_EEE(x)                ((u32)(((x >> 28) & 0x00000001)))
#define BIT_EN_EEE_TX                ((u32)0x00000001 << 27)           /*!< R/W 0x0  Enable MAC TX EEE ability  0: Disable EEE ability for TX.  1: Enable EEE ability for TX.*/
#define EN_EEE_TX(x)                 ((u32)(((x) & 0x00000001) << 27))
#define GET_EN_EEE_TX(x)             ((u32)(((x >> 27) & 0x00000001)))
#define BIT_EN_EEE_RX                ((u32)0x00000001 << 26)           /*!< R/W 0x0  Enable MAC RX EEE ability  0: Disable EEE ability for RX.   1: Enable EEE ability for RX. Application note : The following methods could be used to disable EEE function.    a) n[1:0] = 00 -> completely disable EEE function, or    b) n[3:2] = 00 -> disable /1000Mbps EEE  capability (excludes 10Mbps ).*/
#define EN_EEE_RX(x)                 ((u32)(((x) & 0x00000001) << 26))
#define GET_EN_EEE_RX(x)             ((u32)(((x >> 26) & 0x00000001)))
#define BIT_EEE_OUTQ_LOWQ_OVER       ((u32)0x00000001 << 23)           /*!< R/W 0x0  SW has many packets to be tranmitted, then set this bit to tell HW leave lpi state in condition2.*/
#define EEE_OUTQ_LOWQ_OVER(x)        ((u32)(((x) & 0x00000001) << 23))
#define GET_EEE_OUTQ_LOWQ_OVER(x)    ((u32)(((x >> 23) & 0x00000001)))
#define BIT_EEE_STS                  ((u32)0x00000001 << 19)           /*!< R 0x0  Indicate EEE status, both TX and RX.  1: Both TX and RX in LPI mode simultaneously.  0: not both TX and RX in LPI mode simultaneously*/
#define EEE_STS(x)                   ((u32)(((x) & 0x00000001) << 19))
#define GET_EEE_STS(x)               ((u32)(((x >> 19) & 0x00000001)))
#define BIT_EEE_TX_STS               ((u32)0x00000001 << 18)           /*!< R 0x0  Indicate EEE TX status  1: TX is in LPI mode.   0: TX is in normal mode.*/
#define EEE_TX_STS(x)                ((u32)(((x) & 0x00000001) << 18))
#define GET_EEE_TX_STS(x)            ((u32)(((x >> 18) & 0x00000001)))
#define BIT_EEE_RX_STS               ((u32)0x00000001 << 17)           /*!< R 0x0  Indicate Port 0 EEE RX status   1: RX is in LPI mode.   0: RX is in normal mode.*/
#define EEE_RX_STS(x)                ((u32)(((x) & 0x00000001) << 17))
#define GET_EEE_RX_STS(x)            ((u32)(((x >> 17) & 0x00000001)))
#define BIT_EEE_PAUSEFLAG            ((u32)0x00000001 << 16)           /*!< R 0x0  Set indication for LPI request due to RX pause on frame.  1: indicate RX pause on frame; 0: no indicate.*/
#define EEE_PAUSEFLAG(x)             ((u32)(((x) & 0x00000001) << 16))
#define GET_EEE_PAUSEFLAG(x)         ((u32)(((x >> 16) & 0x00000001)))
#define BIT_EEE_WAKE_SET1            ((u32)0x00000001 << 14)           /*!< R/W 0x0  Enable LPI TX high priority pkt Wakeup decision maker for EEE.  1: enable; 0: disable. Gmac will leave lpi mode when EEE_CR1_reg.txq_th is met or one of the egress is larger than 512B.*/
#define EEE_WAKE_SET1(x)             ((u32)(((x) & 0x00000001) << 14))
#define GET_EEE_WAKE_SET1(x)         ((u32)(((x >> 14) & 0x00000001)))
#define BIT_EEE_WAKE_SET0            ((u32)0x00000001 << 13)           /*!< R/W 0x0  Enable LPI TX transmit wakeup decision maker for EEE.  1: enable; 0: disable.*/
#define EEE_WAKE_SET0(x)             ((u32)(((x) & 0x00000001) << 13))
#define GET_EEE_WAKE_SET0(x)         ((u32)(((x >> 13) & 0x00000001)))
#define BIT_EEE_REQ_SET2             ((u32)0x00000001 << 12)           /*!< R/W 0x0  Enable LPI TX request RX pause on frame decision maker for EEE.  1: enable; 0: disable.*/
#define EEE_REQ_SET2(x)              ((u32)(((x) & 0x00000001) << 12))
#define GET_EEE_REQ_SET2(x)          ((u32)(((x >> 12) & 0x00000001)))
#define BIT_EEE_REQ_SET1             ((u32)0x00000001 << 11)           /*!< R/W 0x0  Enable LPI TX request queue empty decision maker for EEE.  1: enable; 0: disable. For RLE0390(原RL6166), only for tx is single descriptor operation. For next version, this bit can support multi-descriptor operation.*/
#define EEE_REQ_SET1(x)              ((u32)(((x) & 0x00000001) << 11))
#define GET_EEE_REQ_SET1(x)          ((u32)(((x >> 11) & 0x00000001)))
#define BIT_EEE_REQ_SET0             ((u32)0x00000001 << 10)           /*!< R/W 0x0  Enable LPI TX request Rate decision maker for EEE.  1: enable; 0: disable.*/
#define EEE_REQ_SET0(x)              ((u32)(((x) & 0x00000001) << 10))
#define GET_EEE_REQ_SET0(x)          ((u32)(((x >> 10) & 0x00000001)))
#define BIT_EEE_RXTIMER_EN           ((u32)0x00000001 << 9)            /*!< R/W 0x0  Enable RX meter to count RX LPI status time*/
#define EEE_RXTIMER_EN(x)            ((u32)(((x) & 0x00000001) << 9))
#define GET_EEE_RXTIMER_EN(x)        ((u32)(((x >> 9) & 0x00000001)))
#define BIT_EEE_TXTIMER_EN           ((u32)0x00000001 << 8)            /*!< R/W 0x0  Enable TX meter to count TX LPI status time*/
#define EEE_TXTIMER_EN(x)            ((u32)(((x) & 0x00000001) << 8))
#define GET_EEE_TXTIMER_EN(x)        ((u32)(((x >> 8) & 0x00000001)))
#define MASK_EEE_TIMER_UNIT_GIGA_1   ((u32)0x00000003 << 6)            /*!< R/W 0x0  Set the unit for timer T_wakeup, T_tx_rate, and T_pause in EEE function.  00: 1uS;  01: 16 uS;  10: 128 uS;  11: 1024uS.*/
#define EEE_TIMER_UNIT_GIGA_1(x)     (((u32)((x) & 0x00000003) << 6))
#define GET_EEE_TIMER_UNIT_GIGA_1(x) ((u32)(((x >> 6) & 0x00000003)))
#define MASK_EEE_TIMER_UNIT_GIGA_2   ((u32)0x00000003 << 4)            /*!< R/W 0x0  Set the unit for timer T_tx_delay in EEE function.  00: 8uS;  01: 64uS;  10: 256uS;  11: 1024uS.*/
#define EEE_TIMER_UNIT_GIGA_2(x)     (((u32)((x) & 0x00000003) << 4))
#define GET_EEE_TIMER_UNIT_GIGA_2(x) ((u32)(((x >> 4) & 0x00000003)))
#define MASK_EEE_TIMER_UNIT_100_1    ((u32)0x00000003 << 2)            /*!< R/W 0x0  Set the unit for timer T_wakeup, T_tx_rate, and T_pause in EEE function.  00: 1uS;  01: 16uS;  10: 128uS;  11: 1024uS.*/
#define EEE_TIMER_UNIT_100_1(x)      (((u32)((x) & 0x00000003) << 2))
#define GET_EEE_TIMER_UNIT_100_1(x)  ((u32)(((x >> 2) & 0x00000003)))
#define MASK_EEE_TIMER_UNIT_100_2    ((u32)0x00000003 << 0)            /*!< R/W 0x0  Set the unit for timer T_tx_delay in EEE function.  00: 8uS;  01: 64uS;  10: 512uS;  11: 1024uS.*/
#define EEE_TIMER_UNIT_100_2(x)      (((u32)((x) & 0x00000003) << 0))
#define GET_EEE_TIMER_UNIT_100_2(x)  ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup ETH_EEE_CR2
  * @brief
  * @{
  */
#define MASK_EEE_TIMER_TW_GIGA   ((u32)0x000000FF << 24)           /*!< R/W 0x10  Timer value in EEE function for T_wakeup. Value from 0~256. Unit: EEE_Timer_Unit_Giga_1. Default value is 16 us.*/
#define EEE_TIMER_TW_GIGA(x)     (((u32)((x) & 0x000000FF) << 24))
#define GET_EEE_TIMER_TW_GIGA(x) ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_EEE_TIMER_TR_GIGA   ((u32)0x000000FF << 16)           /*!< R/W 0x0a  Timer value in EEE function for T_tx_rate. Unit: EEE_Timer_Unit_Giga_1. Default value is 10 us.*/
#define EEE_TIMER_TR_GIGA(x)     (((u32)((x) & 0x000000FF) << 16))
#define GET_EEE_TIMER_TR_GIGA(x) ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_EEE_TIMER_TD_GIGA   ((u32)0x000000FF << 8)            /*!< R/W 0xc4  Timer value in EEE function for QoS based LPI decision maker T_tx_delay. Unit: EEE_Timer_Unit_Giga_2*4. Default value is 200  ms.*/
#define EEE_TIMER_TD_GIGA(x)     (((u32)((x) & 0x000000FF) << 8))
#define GET_EEE_TIMER_TD_GIGA(x) ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_EEE_TIMER_TP_GIGA   ((u32)0x000000FF << 0)            /*!< R/W 0x0a  Timer value in EEE function for TX decision maker T_pause. Unit: EEE_Timer_Unit_Giga_1. Default value is 10 us.*/
#define EEE_TIMER_TP_GIGA(x)     (((u32)((x) & 0x000000FF) << 0))
#define GET_EEE_TIMER_TP_GIGA(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_EEE_CR3
  * @brief
  * @{
  */
#define MASK_EEE_TIMER_TW_100   ((u32)0x000000FF << 24)           /*!< R/W 0x1e  Timer value in EEE function for T_wakeup. Value from 0~256. Unit: EEE_Timer_Unit_100_1 Default value is 30 us.*/
#define EEE_TIMER_TW_100(x)     (((u32)((x) & 0x000000FF) << 24))
#define GET_EEE_TIMER_TW_100(x) ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_EEE_TIMER_TR_100   ((u32)0x000000FF << 16)           /*!< R/W 0x0a  Timer value in EEE function for T_tx_rate. Unit: EEE_Timer_Unit_100_1. Default value is 10 us.*/
#define EEE_TIMER_TR_100(x)     (((u32)((x) & 0x000000FF) << 16))
#define GET_EEE_TIMER_TR_100(x) ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_EEE_TIMER_TD_100   ((u32)0x000000FF << 8)            /*!< R/W 0xf4  Timer value in EEE function for QoS based LPI decision maker T_tx_delay. Unit: EEE_Timer_Unit_100_2*4. Default value is 1s.*/
#define EEE_TIMER_TD_100(x)     (((u32)((x) & 0x000000FF) << 8))
#define GET_EEE_TIMER_TD_100(x) ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_EEE_TIMER_TP_100   ((u32)0x000000FF << 0)            /*!< R/W 0x0a  Timer value in EEE function for TX decision maker T_pause. Unit: EEE_Timer_Unit_100_1. Default value is 10 us.*/
#define EEE_TIMER_TP_100(x)     (((u32)((x) & 0x000000FF) << 0))
#define GET_EEE_TIMER_TP_100(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_EEE_CR4
  * @brief
  * @{
  */
#define MASK_EEE_TX_THR_GIGA   ((u32)0x0000FFFF << 16)           /*!< R/W 0x0271  Set the TX threshold in EEE function. Unit is Byte. EEE_TX_THR=Round [(T_tx_rate*Rate) / 8 ]. If you set T_tx_rate=10us, Rate=1000Mbps, so set EEE_TX_THR =  0x4E2 (1250bytes). Default value is 0x271(~half rate).*/
#define EEE_TX_THR_GIGA(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define GET_EEE_TX_THR_GIGA(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define MASK_EEE_TX_THR_100    ((u32)0x0000FFFF << 0)            /*!< R/W 0x003e  Set the TX threshold in EEE function. Unit is Byte. EEE_TX_THR=Round [(T_tx_rate*Rate) / 8 ]. If you set T_tx_rate=10us, Rate=100Mbps, so set EEE_TX_THR = 0x7D (125bytes). Default value is 0x3E (~half rate).*/
#define EEE_TX_THR_100(x)      (((u32)((x) & 0x0000FFFF) << 0))
#define GET_EEE_TX_THR_100(x)  ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_EEE_LPI_TMO_REG
  * @brief
  * @{
  */
#define MASK_EEE_RXLPI_TIME   ((u32)0x003FFFFF << 0)           /*!< RC 0x0  RX in LPI time. (unit: 1us) (accumulation)Valid as (PORT_MASK[0]==1&&RXTimer_EN==1)*/
#define EEE_RXLPI_TIME(x)     (((u32)((x) & 0x003FFFFF) << 0))
#define GET_EEE_RXLPI_TIME(x) ((u32)(((x >> 0) & 0x003FFFFF)))
/** @} */

/** @defgroup ETH_EEE_LPI_TM1_REG
  * @brief
  * @{
  */
#define BIT_EEE_STOP_TXC        ((u32)0x00000001 << 23)           /*!< R/W 0x0  This bit works in rgmii_1000 and rgmii_100 modes only.  1: gmac halts gmac_rgmii_txc_out when gmac enters lpi.  0: gmac does not halt gmac_rgmii_txc_out when gmac enters lpi. This bit should always be b0 for RLE0390(原RL6166). Gmac can halt gmac_rgmii_txc_out in the next version.*/
#define EEE_STOP_TXC(x)         ((u32)(((x) & 0x00000001) << 23))
#define GET_EEE_STOP_TXC(x)     ((u32)(((x >> 23) & 0x00000001)))
#define BIT_EEE_TRXCTL_LEVEL    ((u32)0x00000001 << 22)           /*!< R/W 0x0  The gmac_rgmii_txe and gmac_rgmii_rxdv level when gmac is in lpi.   1: tx_ctl and rx_ctl is high when in lpi.  0: reserved.*/
#define EEE_TRXCTL_LEVEL(x)     ((u32)(((x) & 0x00000001) << 22))
#define GET_EEE_TRXCTL_LEVEL(x) ((u32)(((x >> 22) & 0x00000001)))
#define MASK_EEE_TXLPI_TIME     ((u32)0x003FFFFF << 0)            /*!< RC 0x0  TX in LPI time. (unit: 1us ) (accumulation)Valid as (PORT_MASK[0]==1&&TXTimer_EN==1)*/
#define EEE_TXLPI_TIME(x)       (((u32)((x) & 0x003FFFFF) << 0))
#define GET_EEE_TXLPI_TIME(x)   ((u32)(((x >> 0) & 0x003FFFFF)))
/** @} */

/** @defgroup ETH_EEEP_CR1
  * @brief
  * @{
  */
#define BIT_EN_EEEP                 ((u32)0x00000001 << 31)           /*!< R/W 0x0  Enable EEEP function 0b0: Disable EEEP function.  0b1: Enable EEEP function.*/
#define EN_EEEP(x)                  ((u32)(((x) & 0x00000001) << 31))
#define GET_EN_EEEP(x)              ((u32)(((x >> 31) & 0x00000001)))
#define BIT_CFG_EN_EEEP_RX          ((u32)0x00000001 << 30)           /*!< R/W 0x0  1: Enable EEEP RX. 0: Disable EEEP RX.*/
#define CFG_EN_EEEP_RX(x)           ((u32)(((x) & 0x00000001) << 30))
#define GET_CFG_EN_EEEP_RX(x)       ((u32)(((x >> 30) & 0x00000001)))
#define BIT_CFG_EN_EEEP_TX          ((u32)0x00000001 << 29)           /*!< R/W 0x0  1: Enable EEEP TX. 0: Disable EEEP TX.*/
#define CFG_EN_EEEP_TX(x)           ((u32)(((x) & 0x00000001) << 29))
#define GET_CFG_EN_EEEP_TX(x)       ((u32)(((x >> 29) & 0x00000001)))
#define BIT_CFG_EN_EEEP_GIGA        ((u32)0x00000001 << 28)           /*!< R/W 0x0  1: Enable EEEP in giga mode. 0: Disable EEEP in giga mode.*/
#define CFG_EN_EEEP_GIGA(x)         ((u32)(((x) & 0x00000001) << 28))
#define GET_CFG_EN_EEEP_GIGA(x)     ((u32)(((x >> 28) & 0x00000001)))
#define BIT_CFG_EN_EEEP             ((u32)0x00000001 << 27)           /*!< R/W 0x0  1: Enable EEEP in  mode. 0: Disable EEEP in  mode.*/
#define CFG_EN_EEEP(x)              ((u32)(((x) & 0x00000001) << 27))
#define GET_CFG_EN_EEEP(x)          ((u32)(((x >> 27) & 0x00000001)))
#define BIT_CFG_EEEP_DEFER_TXLPI    ((u32)0x00000001 << 26)           /*!< R/W 0x0  ???*/
#define CFG_EEEP_DEFER_TXLPI(x)     ((u32)(((x) & 0x00000001) << 26))
#define GET_CFG_EEEP_DEFER_TXLPI(x) ((u32)(((x >> 26) & 0x00000001)))
#define BIT_PHY_MST_MODE            ((u32)0x00000001 << 24)           /*!< R/W 0x0  When PMA is in master clock role, the EEEP_RX can work. SW needs to configure PCS reg9.bit12(Master-slave manual config enable) and reg9.bit11(Master-slave config value). Then setting EEEP_CR1_REG.PHY_MST_MODE.*/
#define PHY_MST_MODE(x)             ((u32)(((x) & 0x00000001) << 24))
#define GET_PHY_MST_MODE(x)         ((u32)(((x >> 24) & 0x00000001)))
#define MASK_EEEP_DBGO_SEL          ((u32)0x0000007F << 16)           /*!< R/W 0x0  eeep module debug signals selection.*/
#define EEEP_DBGO_SEL(x)            (((u32)((x) & 0x0000007F) << 16))
#define GET_EEEP_DBGO_SEL(x)        ((u32)(((x >> 16) & 0x0000007F)))
/** @} */

/** @defgroup ETH_EEEP_CR2
  * @brief
  * @{
  */
#define MASK_EEEP_SLEEP_STEP   ((u32)0x000000FF << 0)           /*!< R/W 0x1  Value from 1 to 255, indicate the rx sleep step in EEEP function. Value 0 is the same as 1. EEPROM and register configurable.//???*/
#define EEEP_SLEEP_STEP(x)     (((u32)((x) & 0x000000FF) << 0))
#define GET_EEEP_SLEEP_STEP(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_EEEP_CR3
  * @brief
  * @{
  */
#define MASK_EEEP_TX_THR   ((u32)0x0000FFFF << 16)           /*!< R/W 0xF0  Set the TX threshold in EEEP function. Unit: Byte; EEEP_TX_THR=Round [(T_rx_rate*Rate) / 8 ].   If you set T_rx_rate=192us,Rate=10Mbps, so set EEEP_TX_THR= 0xF0 (240 bytes). EEPROM and register configurable.//HYH???*/
#define EEEP_TX_THR(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define GET_EEEP_TX_THR(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define MASK_EEEP_RX_THR   ((u32)0x0000FFFF << 0)            /*!< R/W 0xF0  Set the RX threshold in EEEP function. Unit: Byte; EEEP_RX_THR=Round [(T_rx_rate*Rate) / 8 ].   If you set T_rx_rate=192us,Rate=10Mbps, so set EEEP_TX_THR= 0xF0 (240 bytes). EEPROM and register configurable. //HYH???*/
#define EEEP_RX_THR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GET_EEEP_RX_THR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_EEEP_CR4
  * @brief
  * @{
  */
#define MASK_EEEP_TIMER_RX_RATE   ((u32)0x000000FF << 24)           /*!< R/W 0xC  Timer value in EEEP function for T_rx_rate. Unit: microsecond. Default value is 192 us. EEPROM and register configurable.*/
#define EEEP_TIMER_RX_RATE(x)     (((u32)((x) & 0x000000FF) << 24))
#define GET_EEEP_TIMER_RX_RATE(x) ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_EEEP_TIMER_RX_WAKE   ((u32)0x000000FF << 16)           /*!< R/W 0x2  Timer value in EEEP function for T_rx_wake. Unit: microsecond. Default value is 32 us. EEPROM and register configurable.*/
#define EEEP_TIMER_RX_WAKE(x)     (((u32)((x) & 0x000000FF) << 16))
#define GET_EEEP_TIMER_RX_WAKE(x) ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_EEEP_TIMER_TX_WAIT   ((u32)0x000000FF << 8)            /*!< R/W 0x1  Timer value in EEEP function for T_tx_wait. Unit: microsecond. Default value is 16 us. EEPROM and register configurable.*/
#define EEEP_TIMER_TX_WAIT(x)     (((u32)((x) & 0x000000FF) << 8))
#define GET_EEEP_TIMER_TX_WAIT(x) ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_EEEP_TIMER_PAUSE     ((u32)0x000000FF << 0)            /*!< R/W 0x1  Timer value in EEEP function for T_pause_on. Unit:refer to EEEP_Timer_Unit. Default value is 16 us. EEPROM and register configurable.*/
#define EEEP_TIMER_PAUSE(x)       (((u32)((x) & 0x000000FF) << 0))
#define GET_EEEP_TIMER_PAUSE(x)   ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_EEEP_CR5
  * @brief
  * @{
  */
#define MASK_EEEP_TX_RATE          ((u32)0x000000FF << 16)           /*!< R/W 0x0  */
#define EEEP_TX_RATE(x)            (((u32)((x) & 0x000000FF) << 16))
#define GET_EEEP_TX_RATE(x)        ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_EEEP_TIMER_UNIT       ((u32)0x00000003 << 8)            /*!< R/W 0x1  Set the unit for timer in EEEP function. 00: 1us;  01: 16us;  10: 128us;  11: 1024us. EEPROM and register configurable.*/
#define EEEP_TIMER_UNIT(x)         (((u32)((x) & 0x00000003) << 8))
#define GET_EEEP_TIMER_UNIT(x)     ((u32)(((x >> 8) & 0x00000003)))
#define MASK_EEEP_TIMER_RX_SLEEP   ((u32)0x000000FF << 0)            /*!< R/W 0xC  value of Timer in EEEP function for T_rx_sleep. Unit: microsecond. Default value is 192 us. EEPROM and register configurable.*/
#define EEEP_TIMER_RX_SLEEP(x)     (((u32)((x) & 0x000000FF) << 0))
#define GET_EEEP_TIMER_RX_SLEEP(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup ETH_EEEP_CR6
  * @brief
  * @{
  */
#define MASK_EEEP_ABIG_ON    ((u32)0x0000FFFF << 16)           /*!< R/W 0xb80a  */
#define EEEP_ABIG_ON(x)      (((u32)((x) & 0x0000FFFF) << 16))
#define GET_EEEP_ABIG_ON(x)  ((u32)(((x >> 16) & 0x0000FFFF)))
#define MASK_EEEP_ABIG_OFF   ((u32)0x0000FFFF << 0)            /*!< R/W 0x800a  */
#define EEEP_ABIG_OFF(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GET_EEEP_ABIG_OFF(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_EEEP_CR7
  * @brief
  * @{
  */
#define MASK_EEEP_LDVBIAS_ON    ((u32)0x0000FFFF << 16)           /*!< R/W 0x7550  */
#define EEEP_LDVBIAS_ON(x)      (((u32)((x) & 0x0000FFFF) << 16))
#define GET_EEEP_LDVBIAS_ON(x)  ((u32)(((x >> 16) & 0x0000FFFF)))
#define MASK_EEEP_LDVBIAS_OFF   ((u32)0x0000FFFF << 0)            /*!< R/W 0x0550  */
#define EEEP_LDVBIAS_OFF(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GET_EEEP_LDVBIAS_OFF(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ETH_IMR0_REG
  * @brief
  * @{
  */
#define BIT_IMR0_TDU5    ((u32)0x00000001 << 28)           /*!< R/W 0x0  */
#define IMR0_TDU5(x)     ((u32)(((x) & 0x00000001) << 28))
#define GET_IMR0_TDU5(x) ((u32)(((x >> 28) & 0x00000001)))
#define BIT_IMR0_TDU4    ((u32)0x00000001 << 27)           /*!< R/W 0x0  */
#define IMR0_TDU4(x)     ((u32)(((x) & 0x00000001) << 27))
#define GET_IMR0_TDU4(x) ((u32)(((x >> 27) & 0x00000001)))
#define BIT_IMR0_TDU3    ((u32)0x00000001 << 26)           /*!< R/W 0x0  */
#define IMR0_TDU3(x)     ((u32)(((x) & 0x00000001) << 26))
#define GET_IMR0_TDU3(x) ((u32)(((x >> 26) & 0x00000001)))
#define BIT_IMR0_TDU2    ((u32)0x00000001 << 25)           /*!< R/W 0x0  */
#define IMR0_TDU2(x)     ((u32)(((x) & 0x00000001) << 25))
#define GET_IMR0_TDU2(x) ((u32)(((x >> 25) & 0x00000001)))
#define BIT_IMR0_TDU1    ((u32)0x00000001 << 24)           /*!< RO 0x0  This bit is IMR.tdu*/
#define IMR0_TDU1(x)     ((u32)(((x) & 0x00000001) << 24))
#define GET_IMR0_TDU1(x) ((u32)(((x >> 24) & 0x00000001)))
#define BIT_IMR0_TOK5    ((u32)0x00000001 << 20)           /*!< R/W 0x0  */
#define IMR0_TOK5(x)     ((u32)(((x) & 0x00000001) << 20))
#define GET_IMR0_TOK5(x) ((u32)(((x >> 20) & 0x00000001)))
#define BIT_IMR0_TOK4    ((u32)0x00000001 << 19)           /*!< R/W 0x0  */
#define IMR0_TOK4(x)     ((u32)(((x) & 0x00000001) << 19))
#define GET_IMR0_TOK4(x) ((u32)(((x >> 19) & 0x00000001)))
#define BIT_IMR0_TOK3    ((u32)0x00000001 << 18)           /*!< R/W 0x0  */
#define IMR0_TOK3(x)     ((u32)(((x) & 0x00000001) << 18))
#define GET_IMR0_TOK3(x) ((u32)(((x >> 18) & 0x00000001)))
#define BIT_IMR0_TOK2    ((u32)0x00000001 << 17)           /*!< R/W 0x0  */
#define IMR0_TOK2(x)     ((u32)(((x) & 0x00000001) << 17))
#define GET_IMR0_TOK2(x) ((u32)(((x >> 17) & 0x00000001)))
#define BIT_IMR0_TOK1    ((u32)0x00000001 << 16)           /*!< RO 0x0  This bit is IMR.tok*/
#define IMR0_TOK1(x)     ((u32)(((x) & 0x00000001) << 16))
#define GET_IMR0_TOK1(x) ((u32)(((x >> 16) & 0x00000001)))
#define BIT_IMR0_ROK6    ((u32)0x00000001 << 5)            /*!< R/W 0x0  */
#define IMR0_ROK6(x)     ((u32)(((x) & 0x00000001) << 5))
#define GET_IMR0_ROK6(x) ((u32)(((x >> 5) & 0x00000001)))
#define BIT_IMR0_ROK5    ((u32)0x00000001 << 4)            /*!< R/W 0x0  */
#define IMR0_ROK5(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_IMR0_ROK5(x) ((u32)(((x >> 4) & 0x00000001)))
#define BIT_IMR0_ROK4    ((u32)0x00000001 << 3)            /*!< R/W 0x0  */
#define IMR0_ROK4(x)     ((u32)(((x) & 0x00000001) << 3))
#define GET_IMR0_ROK4(x) ((u32)(((x >> 3) & 0x00000001)))
#define BIT_IMR0_ROK3    ((u32)0x00000001 << 2)            /*!< R/W 0x0  */
#define IMR0_ROK3(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_IMR0_ROK3(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_IMR0_ROK2    ((u32)0x00000001 << 1)            /*!< R/W 0x0  */
#define IMR0_ROK2(x)     ((u32)(((x) & 0x00000001) << 1))
#define GET_IMR0_ROK2(x) ((u32)(((x >> 1) & 0x00000001)))
#define BIT_IMR0_ROK1    ((u32)0x00000001 << 0)            /*!< RO 0x0  This bit is IMR.rok*/
#define IMR0_ROK1(x)     ((u32)(((x) & 0x00000001) << 0))
#define GET_IMR0_ROK1(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_IMR1_REG
  * @brief
  * @{
  */
#define BIT_IMR1_TDU5    ((u32)0x00000001 << 28)           /*!< R/W 0x0  */
#define IMR1_TDU5(x)     ((u32)(((x) & 0x00000001) << 28))
#define GET_IMR1_TDU5(x) ((u32)(((x >> 28) & 0x00000001)))
#define BIT_IMR1_TDU4    ((u32)0x00000001 << 27)           /*!< R/W 0x0  */
#define IMR1_TDU4(x)     ((u32)(((x) & 0x00000001) << 27))
#define GET_IMR1_TDU4(x) ((u32)(((x >> 27) & 0x00000001)))
#define BIT_IMR1_TDU3    ((u32)0x00000001 << 26)           /*!< R/W 0x0  */
#define IMR1_TDU3(x)     ((u32)(((x) & 0x00000001) << 26))
#define GET_IMR1_TDU3(x) ((u32)(((x >> 26) & 0x00000001)))
#define BIT_IMR1_TDU2    ((u32)0x00000001 << 25)           /*!< R/W 0x0  */
#define IMR1_TDU2(x)     ((u32)(((x) & 0x00000001) << 25))
#define GET_IMR1_TDU2(x) ((u32)(((x >> 25) & 0x00000001)))
#define BIT_IMR1_TDU1    ((u32)0x00000001 << 24)           /*!< R/W 0x0  */
#define IMR1_TDU1(x)     ((u32)(((x) & 0x00000001) << 24))
#define GET_IMR1_TDU1(x) ((u32)(((x >> 24) & 0x00000001)))
#define BIT_IMR1_TOK5    ((u32)0x00000001 << 20)           /*!< R/W 0x0  */
#define IMR1_TOK5(x)     ((u32)(((x) & 0x00000001) << 20))
#define GET_IMR1_TOK5(x) ((u32)(((x >> 20) & 0x00000001)))
#define BIT_IMR1_TOK4    ((u32)0x00000001 << 19)           /*!< R/W 0x0  */
#define IMR1_TOK4(x)     ((u32)(((x) & 0x00000001) << 19))
#define GET_IMR1_TOK4(x) ((u32)(((x >> 19) & 0x00000001)))
#define BIT_IMR1_TOK3    ((u32)0x00000001 << 18)           /*!< R/W 0x0  */
#define IMR1_TOK3(x)     ((u32)(((x) & 0x00000001) << 18))
#define GET_IMR1_TOK3(x) ((u32)(((x >> 18) & 0x00000001)))
#define BIT_IMR1_TOK2    ((u32)0x00000001 << 17)           /*!< R/W 0x0  */
#define IMR1_TOK2(x)     ((u32)(((x) & 0x00000001) << 17))
#define GET_IMR1_TOK2(x) ((u32)(((x >> 17) & 0x00000001)))
#define BIT_IMR1_TOK1    ((u32)0x00000001 << 16)           /*!< R/W 0x0  */
#define IMR1_TOK1(x)     ((u32)(((x) & 0x00000001) << 16))
#define GET_IMR1_TOK1(x) ((u32)(((x >> 16) & 0x00000001)))
#define BIT_IMR1_RDU6    ((u32)0x00000001 << 13)           /*!< R/W 0x0  */
#define IMR1_RDU6(x)     ((u32)(((x) & 0x00000001) << 13))
#define GET_IMR1_RDU6(x) ((u32)(((x >> 13) & 0x00000001)))
#define BIT_IMR1_RDU5    ((u32)0x00000001 << 12)           /*!< R/W 0x0  */
#define IMR1_RDU5(x)     ((u32)(((x) & 0x00000001) << 12))
#define GET_IMR1_RDU5(x) ((u32)(((x >> 12) & 0x00000001)))
#define BIT_IMR1_RDU4    ((u32)0x00000001 << 11)           /*!< R/W 0x0  */
#define IMR1_RDU4(x)     ((u32)(((x) & 0x00000001) << 11))
#define GET_IMR1_RDU4(x) ((u32)(((x >> 11) & 0x00000001)))
#define BIT_IMR1_RDU3    ((u32)0x00000001 << 10)           /*!< R/W 0x0  */
#define IMR1_RDU3(x)     ((u32)(((x) & 0x00000001) << 10))
#define GET_IMR1_RDU3(x) ((u32)(((x >> 10) & 0x00000001)))
#define BIT_IMR1_RDU2    ((u32)0x00000001 << 9)            /*!< R/W 0x0  */
#define IMR1_RDU2(x)     ((u32)(((x) & 0x00000001) << 9))
#define GET_IMR1_RDU2(x) ((u32)(((x >> 9) & 0x00000001)))
#define BIT_IMR1_RDU1    ((u32)0x00000001 << 8)            /*!< R/W 0x0  */
#define IMR1_RDU1(x)     ((u32)(((x) & 0x00000001) << 8))
#define GET_IMR1_RDU1(x) ((u32)(((x >> 8) & 0x00000001)))
#define BIT_IMR1_ROK6    ((u32)0x00000001 << 5)            /*!< R/W 0x0  */
#define IMR1_ROK6(x)     ((u32)(((x) & 0x00000001) << 5))
#define GET_IMR1_ROK6(x) ((u32)(((x >> 5) & 0x00000001)))
#define BIT_IMR1_ROK5    ((u32)0x00000001 << 4)            /*!< R/W 0x0  */
#define IMR1_ROK5(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_IMR1_ROK5(x) ((u32)(((x >> 4) & 0x00000001)))
#define BIT_IMR1_ROK4    ((u32)0x00000001 << 3)            /*!< R/W 0x0  */
#define IMR1_ROK4(x)     ((u32)(((x) & 0x00000001) << 3))
#define GET_IMR1_ROK4(x) ((u32)(((x >> 3) & 0x00000001)))
#define BIT_IMR1_ROK3    ((u32)0x00000001 << 2)            /*!< R/W 0x0  */
#define IMR1_ROK3(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_IMR1_ROK3(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_IMR1_ROK2    ((u32)0x00000001 << 1)            /*!< R/W 0x0  */
#define IMR1_ROK2(x)     ((u32)(((x) & 0x00000001) << 1))
#define GET_IMR1_ROK2(x) ((u32)(((x >> 1) & 0x00000001)))
#define BIT_IMR1_ROK1    ((u32)0x00000001 << 0)            /*!< R/W 0x0  */
#define IMR1_ROK1(x)     ((u32)(((x) & 0x00000001) << 0))
#define GET_IMR1_ROK1(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_ISR1_REG
  * @brief
  * @{
  */
#define BIT_ISR_TDU5    ((u32)0x00000001 << 28)           /*!< R/W 0x0  Tx Descriptor Ring5 Unavailable: When set, indicates Tx descriptor Ring5 is unavailable.  Write 1 to clear.*/
#define ISR_TDU5(x)     ((u32)(((x) & 0x00000001) << 28))
#define GET_ISR_TDU5(x) ((u32)(((x >> 28) & 0x00000001)))
#define BIT_ISR_TDU4    ((u32)0x00000001 << 27)           /*!< R/W 0x0  Tx Descriptor Ring4 Unavailable: When set, indicates Tx descriptor Ring5 is unavailable.  Write 1 to clear.*/
#define ISR_TDU4(x)     ((u32)(((x) & 0x00000001) << 27))
#define GET_ISR_TDU4(x) ((u32)(((x >> 27) & 0x00000001)))
#define BIT_ISR_TDU3    ((u32)0x00000001 << 26)           /*!< R/W 0x0  Tx Descriptor Ring3 Unavailable: When set, indicates Tx descriptor Ring5 is unavailable.  Write 1 to clear.*/
#define ISR_TDU3(x)     ((u32)(((x) & 0x00000001) << 26))
#define GET_ISR_TDU3(x) ((u32)(((x >> 26) & 0x00000001)))
#define BIT_ISR_TDU2    ((u32)0x00000001 << 25)           /*!< R/W 0x0  Tx Descriptor Ring2 Unavailable: When set, indicates Tx descriptor Ring5 is unavailable.  Write 1 to clear.*/
#define ISR_TDU2(x)     ((u32)(((x) & 0x00000001) << 25))
#define GET_ISR_TDU2(x) ((u32)(((x >> 25) & 0x00000001)))
#define BIT_ISR_TDU1    ((u32)0x00000001 << 24)           /*!< RO 0x0  This bit is ISR.tdu*/
#define ISR_TDU1(x)     ((u32)(((x) & 0x00000001) << 24))
#define GET_ISR_TDU1(x) ((u32)(((x >> 24) & 0x00000001)))
#define BIT_ISR_TOK5    ((u32)0x00000001 << 20)           /*!< R/W 0x0  Transmit Interrupt:   Indicates that the DMA of the last Ring5 descriptor of TxIntMitigation number of Tx packet has completed and the last descriptor has been closed.   Write 1 to clear.*/
#define ISR_TOK5(x)     ((u32)(((x) & 0x00000001) << 20))
#define GET_ISR_TOK5(x) ((u32)(((x >> 20) & 0x00000001)))
#define BIT_ISR_TOK4    ((u32)0x00000001 << 19)           /*!< R/W 0x0  Transmit Interrupt:   Indicates that the DMA of the last Ring4 descriptor of TxIntMitigation number of Tx packet has completed and the last descriptor has been closed.   Write 1 to clear.*/
#define ISR_TOK4(x)     ((u32)(((x) & 0x00000001) << 19))
#define GET_ISR_TOK4(x) ((u32)(((x >> 19) & 0x00000001)))
#define BIT_ISR_TOK3    ((u32)0x00000001 << 18)           /*!< R/W 0x0  Transmit Interrupt:   Indicates that the DMA of the last Ring3 descriptor of TxIntMitigation number of Tx packet has completed and the last descriptor has been closed.   Write 1 to clear.*/
#define ISR_TOK3(x)     ((u32)(((x) & 0x00000001) << 18))
#define GET_ISR_TOK3(x) ((u32)(((x >> 18) & 0x00000001)))
#define BIT_ISR_TOK2    ((u32)0x00000001 << 17)           /*!< R/W 0x0  Transmit Interrupt:   Indicates that the DMA of the last Ring2 descriptor of TxIntMitigation number of Tx packet has completed and the last descriptor has been closed.   Write 1 to clear.*/
#define ISR_TOK2(x)     ((u32)(((x) & 0x00000001) << 17))
#define GET_ISR_TOK2(x) ((u32)(((x >> 17) & 0x00000001)))
#define BIT_ISR_TOK1    ((u32)0x00000001 << 16)           /*!< RO 0x0  This bit is ISR.tok*/
#define ISR_TOK1(x)     ((u32)(((x) & 0x00000001) << 16))
#define GET_ISR_TOK1(x) ((u32)(((x >> 16) & 0x00000001)))
#define BIT_ISR_ROK6    ((u32)0x00000001 << 5)            /*!< R/W 0x0  Receive (Rx) OK:   This interrupt is set either when RxIntMitigation packet is met or RxPktTimer expires in Ring6.   Write 1 to clear.*/
#define ISR_ROK6(x)     ((u32)(((x) & 0x00000001) << 5))
#define GET_ISR_ROK6(x) ((u32)(((x >> 5) & 0x00000001)))
#define BIT_ISR_ROK5    ((u32)0x00000001 << 4)            /*!< R/W 0x0  Receive (Rx) OK:   This interrupt is set either when RxIntMitigation packet is met or RxPktTimer expires in Ring5.   Write 1 to clear.*/
#define ISR_ROK5(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_ISR_ROK5(x) ((u32)(((x >> 4) & 0x00000001)))
#define BIT_ISR_ROK4    ((u32)0x00000001 << 3)            /*!< R/W 0x0  Receive (Rx) OK:   This interrupt is set either when RxIntMitigation packet is met or RxPktTimer expires in Ring4.   Write 1 to clear.*/
#define ISR_ROK4(x)     ((u32)(((x) & 0x00000001) << 3))
#define GET_ISR_ROK4(x) ((u32)(((x >> 3) & 0x00000001)))
#define BIT_ISR_ROK3    ((u32)0x00000001 << 2)            /*!< R/W 0x0  Receive (Rx) OK:   This interrupt is set either when RxIntMitigation packet is met or RxPktTimer expires in Ring3.   Write 1 to clear.*/
#define ISR_ROK3(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_ISR_ROK3(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_ISR_ROK2    ((u32)0x00000001 << 1)            /*!< R/W 0x0  Receive (Rx) OK:   This interrupt is set either when RxIntMitigation packet is met or RxPktTimer expires in Ring2.   Write 1 to clear.*/
#define ISR_ROK2(x)     ((u32)(((x) & 0x00000001) << 1))
#define GET_ISR_ROK2(x) ((u32)(((x >> 1) & 0x00000001)))
#define BIT_ISR_ROK1    ((u32)0x00000001 << 0)            /*!< RO 0x0  This bit is ISR.rok*/
#define ISR_ROK1(x)     ((u32)(((x) & 0x00000001) << 0))
#define GET_ISR_ROK1(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_INT_R_REG
  * @brief
  * @{
  */
#define BIT_TR5_INT_ROUTING    ((u32)0x00000001 << 24)           /*!< R/W 0x0  */
#define TR5_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 24))
#define GET_TR5_INT_ROUTING(x) ((u32)(((x >> 24) & 0x00000001)))
#define BIT_TR4_INT_ROUTING    ((u32)0x00000001 << 22)           /*!< R/W 0x0  */
#define TR4_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 22))
#define GET_TR4_INT_ROUTING(x) ((u32)(((x >> 22) & 0x00000001)))
#define BIT_TR3_INT_ROUTING    ((u32)0x00000001 << 20)           /*!< R/W 0x0  */
#define TR3_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 20))
#define GET_TR3_INT_ROUTING(x) ((u32)(((x >> 20) & 0x00000001)))
#define BIT_TR2_INT_ROUTING    ((u32)0x00000001 << 18)           /*!< R/W 0x0  */
#define TR2_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 18))
#define GET_TR2_INT_ROUTING(x) ((u32)(((x >> 18) & 0x00000001)))
#define BIT_TR1_INT_ROUTING    ((u32)0x00000001 << 16)           /*!< R/W 0x0  */
#define TR1_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 16))
#define GET_TR1_INT_ROUTING(x) ((u32)(((x >> 16) & 0x00000001)))
#define BIT_RR6_INT_ROUTING    ((u32)0x00000001 << 10)           /*!< R/W 0x0  */
#define RR6_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 10))
#define GET_RR6_INT_ROUTING(x) ((u32)(((x >> 10) & 0x00000001)))
#define BIT_RR5_INT_ROUTING    ((u32)0x00000001 << 8)            /*!< R/W 0x0  */
#define RR5_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 8))
#define GET_RR5_INT_ROUTING(x) ((u32)(((x >> 8) & 0x00000001)))
#define BIT_RR4_INT_ROUTING    ((u32)0x00000001 << 6)            /*!< R/W 0x0  */
#define RR4_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 6))
#define GET_RR4_INT_ROUTING(x) ((u32)(((x >> 6) & 0x00000001)))
#define BIT_RR3_INT_ROUTING    ((u32)0x00000001 << 4)            /*!< R/W 0x0  */
#define RR3_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_RR3_INT_ROUTING(x) ((u32)(((x >> 4) & 0x00000001)))
#define BIT_RR2_INT_ROUTING    ((u32)0x00000001 << 2)            /*!< R/W 0x0  */
#define RR2_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_RR2_INT_ROUTING(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_RR1_INT_ROUTING    ((u32)0x00000001 << 0)            /*!< R/W 0x0  */
#define RR1_INT_ROUTING(x)     ((u32)(((x) & 0x00000001) << 0))
#define GET_RR1_INT_ROUTING(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_RST_PWR_DOWN_OPERATION_CTRL_REG
  * @brief
  * @{
  */
#define BIT_AUTORXCLKDIS    ((u32)0x00000001 << 3)           /*!< R/W 0x0  1: Auto Disable Rx Clock when AUTOISOU=1*/
#define AUTORXCLKDIS(x)     ((u32)(((x) & 0x00000001) << 3))
#define GET_AUTORXCLKDIS(x) ((u32)(((x >> 3) & 0x00000001)))
#define BIT_AUTOTXCLKDIS    ((u32)0x00000001 << 2)           /*!< R/W 0x0  1: Auto Disable Tx Clock when AUTOISOU=1*/
#define AUTOTXCLKDIS(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_AUTOTXCLKDIS(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_AUTOISOU        ((u32)0x00000001 << 1)           /*!< R/W 0x0  Auto Isolate Update*/
#define AUTOISOU(x)         ((u32)(((x) & 0x00000001) << 1))
#define GET_AUTOISOU(x)     ((u32)(((x >> 1) & 0x00000001)))
#define BIT_AUTOLSU         ((u32)0x00000001 << 0)           /*!< R/W 0x0  Auto Link Status Update*/
#define AUTOLSU(x)          ((u32)(((x) & 0x00000001) << 0))
#define GET_AUTOLSU(x)      ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_CLK_CTRL_REG
  * @brief
  * @{
  */
#define BIT_REV_00E4_7       ((u32)0x00000001 << 7)           /*!< R/W 0x0  Reserved bit*/
#define REV_00E4_7(x)        ((u32)(((x) & 0x00000001) << 7))
#define GET_REV_00E4_7(x)    ((u32)(((x >> 7) & 0x00000001)))
#define BIT_REV_00E4_6       ((u32)0x00000001 << 6)           /*!< R/W 0x0  Reserved bit*/
#define REV_00E4_6(x)        ((u32)(((x) & 0x00000001) << 6))
#define GET_REV_00E4_6(x)    ((u32)(((x >> 6) & 0x00000001)))
#define BIT_REV_00E4_5       ((u32)0x00000001 << 5)           /*!< R/W 0x0  Reserved bit*/
#define REV_00E4_5(x)        ((u32)(((x) & 0x00000001) << 5))
#define GET_REV_00E4_5(x)    ((u32)(((x >> 5) & 0x00000001)))
#define BIT_REV_00E4_4       ((u32)0x00000001 << 4)           /*!< R/W 0x1  Reserved bit*/
#define REV_00E4_4(x)        ((u32)(((x) & 0x00000001) << 4))
#define GET_REV_00E4_4(x)    ((u32)(((x >> 4) & 0x00000001)))
#define BIT_REV_00E4_3       ((u32)0x00000001 << 3)           /*!< R/W 0x1  Reserved bit*/
#define REV_00E4_3(x)        ((u32)(((x) & 0x00000001) << 3))
#define GET_REV_00E4_3(x)    ((u32)(((x >> 3) & 0x00000001)))
#define BIT_DMA_DATA_SWAP    ((u32)0x00000001 << 2)           /*!< R/W 0x1  1: Swap data order for dma_mst*/
#define DMA_DATA_SWAP(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_DMA_DATA_SWAP(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_RXCLK_EN         ((u32)0x00000001 << 1)           /*!< R/W 0x1  0: Rx Clock Gated (txc_out)*/
#define RXCLK_EN(x)          ((u32)(((x) & 0x00000001) << 1))
#define GET_RXCLK_EN(x)      ((u32)(((x >> 1) & 0x00000001)))
#define BIT_TXCLK_EN         ((u32)0x00000001 << 0)           /*!< R/W 0x1  0: Tx Clock Gated (rxc_out)*/
#define TXCLK_EN(x)          ((u32)(((x) & 0x00000001) << 0))
#define GET_TXCLK_EN(x)      ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_INTERUPT_MASK_REG
  * @brief
  * @{
  */
#define BIT_LX_MST_TO_INTEN    ((u32)0x00000001 << 5)           /*!< R/W 0x0  Interrupt enable of lexra master timeout*/
#define LX_MST_TO_INTEN(x)     ((u32)(((x) & 0x00000001) << 5))
#define GET_LX_MST_TO_INTEN(x) ((u32)(((x >> 5) & 0x00000001)))
#define BIT_LX_SLV_TO_INTEN    ((u32)0x00000001 << 4)           /*!< R/W 0x0  Interrupt enable of lexra slave timeout*/
#define LX_SLV_TO_INTEN(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_LX_SLV_TO_INTEN(x) ((u32)(((x >> 4) & 0x00000001)))
#define BIT_ISON               ((u32)0x00000001 << 3)           /*!< R/W 0x0  Isolate Notify*/
#define ISON(x)                ((u32)(((x) & 0x00000001) << 3))
#define GET_ISON(x)            ((u32)(((x >> 3) & 0x00000001)))
#define BIT_PWRDWNN            ((u32)0x00000001 << 2)           /*!< R/W 0x0  Power Down Notify*/
#define PWRDWNN(x)             ((u32)(((x) & 0x00000001) << 2))
#define GET_PWRDWNN(x)         ((u32)(((x >> 2) & 0x00000001)))
#define BIT_LPBKN              ((u32)0x00000001 << 1)           /*!< R/W 0x0  Loopback Notify*/
#define LPBKN(x)               ((u32)(((x) & 0x00000001) << 1))
#define GET_LPBKN(x)           ((u32)(((x >> 1) & 0x00000001)))
#define BIT_RSTN               ((u32)0x00000001 << 0)           /*!< R/W 0x0  Reset Notify*/
#define RSTN(x)                ((u32)(((x) & 0x00000001) << 0))
#define GET_RSTN(x)            ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_INTERUPT_STATUS_REG
  * @brief
  * @{
  */
#define BIT_LX_MST_TO_INT    ((u32)0x00000001 << 5)           /*!< R/W 0x0  Interrupt status of lexra master timeout*/
#define LX_MST_TO_INT(x)     ((u32)(((x) & 0x00000001) << 5))
#define GET_LX_MST_TO_INT(x) ((u32)(((x >> 5) & 0x00000001)))
#define BIT_LX_SLV_TO_INT    ((u32)0x00000001 << 4)           /*!< R/W 0x0  Interrupt status of lexra slave timeout*/
#define LX_SLV_TO_INT(x)     ((u32)(((x) & 0x00000001) << 4))
#define GET_LX_SLV_TO_INT(x) ((u32)(((x >> 4) & 0x00000001)))
#define BIT_ISONS            ((u32)0x00000001 << 3)           /*!< RWC 0x0  Isolate Notify When set, indicate Isolate is detected.  Write 1 to clear.*/
#define ISONS(x)             ((u32)(((x) & 0x00000001) << 3))
#define GET_ISONS(x)         ((u32)(((x >> 3) & 0x00000001)))
#define BIT_PWRDWNNS         ((u32)0x00000001 << 2)           /*!< RWC 0x0  Power Down Notify When set, indicate Isolate is detected.  Write 1 to clear.*/
#define PWRDWNNS(x)          ((u32)(((x) & 0x00000001) << 2))
#define GET_PWRDWNNS(x)      ((u32)(((x >> 2) & 0x00000001)))
#define BIT_LPBKNS           ((u32)0x00000001 << 1)           /*!< RWC 0x0  Loopback Notify When set, indicate Isolate is detected.  Write 1 to clear.*/
#define LPBKNS(x)            ((u32)(((x) & 0x00000001) << 1))
#define GET_LPBKNS(x)        ((u32)(((x >> 1) & 0x00000001)))
#define BIT_RSTNS            ((u32)0x00000001 << 0)           /*!< RWC 0x0  Reset Notify When set, indicate Isolate is detected.  Write 1 to clear.*/
#define RSTNS(x)             ((u32)(((x) & 0x00000001) << 0))
#define GET_RSTNS(x)         ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_TXFDPx
  * @brief
  * @{
  */
#define MASK_TXFDPx   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Tx xst priority Descriptor Pointer to the Tx Ring*/
#define TXFDPx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_TXFDPx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_TXCDOx
  * @brief
  * @{
  */
#define MASK_TXCDOx   ((u32)0x00000FFF << 0)           /*!< R   Tx xst priority Current Descriptor Offset: FDP+CDO = current descriptor pointer.  CDO increments by 16 bytes each time.*/
#define TXCDOx(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_TXCDOx(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_TXFDP1
  * @brief
  * @{
  */
#define MASK_TXFDP1   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Tx 1st priority Descriptor Pointer to the Tx Ring*/
#define TXFDP1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_TXFDP1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_TXCDO1
  * @brief
  * @{
  */
#define MASK_TXCDO1   ((u32)0x00000FFF << 0)           /*!< R   Tx 1st priority Current Descriptor Offset: FDP+CDO = current descriptor pointer.  CDO increments by 16 bytes each time.*/
#define TXCDO1(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_TXCDO1(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_TXFDP2
  * @brief
  * @{
  */
#define MASK_TXFDP2   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Tx 2st priority Descriptor Pointer to the Tx Ring*/
#define TXFDP2(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_TXFDP2(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_TXCDO2
  * @brief
  * @{
  */
#define MASK_TXCDO2   ((u32)0x00000FFF << 0)           /*!< R   Tx 2st priority Current Descriptor Offset: FDP+CDO = current descriptor pointer.  CDO increments by 16 bytes each time.*/
#define TXCDO2(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_TXCDO2(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_TXFDP3
  * @brief
  * @{
  */
#define MASK_TXFDP3   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Tx 3st priority Descriptor Pointer to the Tx Ring*/
#define TXFDP3(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_TXFDP3(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_TXCDO3
  * @brief
  * @{
  */
#define MASK_TXCDO3   ((u32)0x00000FFF << 0)           /*!< R   Tx 3st priority Current Descriptor Offset: FDP+CDO = current descriptor pointer.  CDO increments by 16 bytes each time.*/
#define TXCDO3(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_TXCDO3(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_TXFDP4
  * @brief
  * @{
  */
#define MASK_TXFDP4   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Tx 4st priority Descriptor Pointer to the Tx Ring*/
#define TXFDP4(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_TXFDP4(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_TXCDO4
  * @brief
  * @{
  */
#define MASK_TXCDO4   ((u32)0x00000FFF << 0)           /*!< R   Tx 4st priority Current Descriptor Offset: FDP+CDO = current descriptor pointer.  CDO increments by 16 bytes each time.*/
#define TXCDO4(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_TXCDO4(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_TXFDP5
  * @brief
  * @{
  */
#define MASK_TXFDP5   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Tx 5st priority Descriptor Pointer to the Tx Ring*/
#define TXFDP5(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_TXFDP5(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_TXCDO5
  * @brief
  * @{
  */
#define MASK_TXCDO5   ((u32)0x00000FFF << 0)           /*!< R   Tx 5st priority Current Descriptor Offset: FDP+CDO = current descriptor pointer.  CDO increments by 16 bytes each time.*/
#define TXCDO5(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_TXCDO5(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RRING_MAPx
  * @brief
  * @{
  */
#define MASK_EXTx_PRI7_ROUTE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  Cputag.priority = 7 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI7_ROUTE(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_EXTx_PRI7_ROUTE(x) ((u32)(((x >> 28) & 0x00000007)))
#define MASK_EXTx_PRI6_ROUTE   ((u32)0x00000007 << 24)           /*!< R/W 0x0  Cputag.priority = 6 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI6_ROUTE(x)     (((u32)((x) & 0x00000007) << 24))
#define GET_EXTx_PRI6_ROUTE(x) ((u32)(((x >> 24) & 0x00000007)))
#define MASK_EXTx_PRI5_ROUTE   ((u32)0x00000007 << 20)           /*!< R/W 0x0  Cputag.priority = 5 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI5_ROUTE(x)     (((u32)((x) & 0x00000007) << 20))
#define GET_EXTx_PRI5_ROUTE(x) ((u32)(((x >> 20) & 0x00000007)))
#define MASK_EXTx_PRI4_ROUTE   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Cputag.priority = 4 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI4_ROUTE(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_EXTx_PRI4_ROUTE(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_EXTx_PRI3_ROUTE   ((u32)0x00000007 << 12)           /*!< R/W 0x0  Cputag.priority = 3 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI3_ROUTE(x)     (((u32)((x) & 0x00000007) << 12))
#define GET_EXTx_PRI3_ROUTE(x) ((u32)(((x >> 12) & 0x00000007)))
#define MASK_EXTx_PRI2_ROUTE   ((u32)0x00000007 << 8)            /*!< R/W 0x0  Cputag.priority = 2 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI2_ROUTE(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_EXTx_PRI2_ROUTE(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_EXTx_PRI1_ROUTE   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Cputag.priority = 1 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI1_ROUTE(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_EXTx_PRI1_ROUTE(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_EXTx_PRI0_ROUTE   ((u32)0x00000007 << 0)            /*!< R/W 0x0  Cputag.priority = 0 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXTx_PRI0_ROUTE(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_EXTx_PRI0_ROUTE(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_RRING_MAP1
  * @brief
  * @{
  */
#define MASK_EXT1_PRI7_ROUTE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  Cputag.priority = 7 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI7_ROUTE(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_EXT1_PRI7_ROUTE(x) ((u32)(((x >> 28) & 0x00000007)))
#define MASK_EXT1_PRI6_ROUTE   ((u32)0x00000007 << 24)           /*!< R/W 0x0  Cputag.priority = 6 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI6_ROUTE(x)     (((u32)((x) & 0x00000007) << 24))
#define GET_EXT1_PRI6_ROUTE(x) ((u32)(((x >> 24) & 0x00000007)))
#define MASK_EXT1_PRI5_ROUTE   ((u32)0x00000007 << 20)           /*!< R/W 0x0  Cputag.priority = 5 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI5_ROUTE(x)     (((u32)((x) & 0x00000007) << 20))
#define GET_EXT1_PRI5_ROUTE(x) ((u32)(((x >> 20) & 0x00000007)))
#define MASK_EXT1_PRI4_ROUTE   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Cputag.priority = 4 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI4_ROUTE(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_EXT1_PRI4_ROUTE(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_EXT1_PRI3_ROUTE   ((u32)0x00000007 << 12)           /*!< R/W 0x0  Cputag.priority = 3 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI3_ROUTE(x)     (((u32)((x) & 0x00000007) << 12))
#define GET_EXT1_PRI3_ROUTE(x) ((u32)(((x >> 12) & 0x00000007)))
#define MASK_EXT1_PRI2_ROUTE   ((u32)0x00000007 << 8)            /*!< R/W 0x0  Cputag.priority = 2 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI2_ROUTE(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_EXT1_PRI2_ROUTE(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_EXT1_PRI1_ROUTE   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Cputag.priority = 1 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI1_ROUTE(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_EXT1_PRI1_ROUTE(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_EXT1_PRI0_ROUTE   ((u32)0x00000007 << 0)            /*!< R/W 0x0  Cputag.priority = 0 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT1_PRI0_ROUTE(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_EXT1_PRI0_ROUTE(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_RRING_MAP2
  * @brief
  * @{
  */
#define MASK_EXT2_PRI7_ROUTE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  Cputag.priority = 7 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI7_ROUTE(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_EXT2_PRI7_ROUTE(x) ((u32)(((x >> 28) & 0x00000007)))
#define MASK_EXT2_PRI6_ROUTE   ((u32)0x00000007 << 24)           /*!< R/W 0x0  Cputag.priority = 6 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI6_ROUTE(x)     (((u32)((x) & 0x00000007) << 24))
#define GET_EXT2_PRI6_ROUTE(x) ((u32)(((x >> 24) & 0x00000007)))
#define MASK_EXT2_PRI5_ROUTE   ((u32)0x00000007 << 20)           /*!< R/W 0x0  Cputag.priority = 5 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI5_ROUTE(x)     (((u32)((x) & 0x00000007) << 20))
#define GET_EXT2_PRI5_ROUTE(x) ((u32)(((x >> 20) & 0x00000007)))
#define MASK_EXT2_PRI4_ROUTE   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Cputag.priority = 4 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI4_ROUTE(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_EXT2_PRI4_ROUTE(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_EXT2_PRI3_ROUTE   ((u32)0x00000007 << 12)           /*!< R/W 0x0  Cputag.priority = 3 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI3_ROUTE(x)     (((u32)((x) & 0x00000007) << 12))
#define GET_EXT2_PRI3_ROUTE(x) ((u32)(((x >> 12) & 0x00000007)))
#define MASK_EXT2_PRI2_ROUTE   ((u32)0x00000007 << 8)            /*!< R/W 0x0  Cputag.priority = 2 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI2_ROUTE(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_EXT2_PRI2_ROUTE(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_EXT2_PRI1_ROUTE   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Cputag.priority = 1 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI1_ROUTE(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_EXT2_PRI1_ROUTE(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_EXT2_PRI0_ROUTE   ((u32)0x00000007 << 0)            /*!< R/W 0x0  Cputag.priority = 0 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT2_PRI0_ROUTE(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_EXT2_PRI0_ROUTE(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_RRING_MAP3
  * @brief
  * @{
  */
#define MASK_EXT3_PRI7_ROUTE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  Cputag.priority = 7 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI7_ROUTE(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_EXT3_PRI7_ROUTE(x) ((u32)(((x >> 28) & 0x00000007)))
#define MASK_EXT3_PRI6_ROUTE   ((u32)0x00000007 << 24)           /*!< R/W 0x0  Cputag.priority = 6 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI6_ROUTE(x)     (((u32)((x) & 0x00000007) << 24))
#define GET_EXT3_PRI6_ROUTE(x) ((u32)(((x >> 24) & 0x00000007)))
#define MASK_EXT3_PRI5_ROUTE   ((u32)0x00000007 << 20)           /*!< R/W 0x0  Cputag.priority = 5 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI5_ROUTE(x)     (((u32)((x) & 0x00000007) << 20))
#define GET_EXT3_PRI5_ROUTE(x) ((u32)(((x >> 20) & 0x00000007)))
#define MASK_EXT3_PRI4_ROUTE   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Cputag.priority = 4 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI4_ROUTE(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_EXT3_PRI4_ROUTE(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_EXT3_PRI3_ROUTE   ((u32)0x00000007 << 12)           /*!< R/W 0x0  Cputag.priority = 3 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI3_ROUTE(x)     (((u32)((x) & 0x00000007) << 12))
#define GET_EXT3_PRI3_ROUTE(x) ((u32)(((x >> 12) & 0x00000007)))
#define MASK_EXT3_PRI2_ROUTE   ((u32)0x00000007 << 8)            /*!< R/W 0x0  Cputag.priority = 2 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI2_ROUTE(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_EXT3_PRI2_ROUTE(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_EXT3_PRI1_ROUTE   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Cputag.priority = 1 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI1_ROUTE(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_EXT3_PRI1_ROUTE(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_EXT3_PRI0_ROUTE   ((u32)0x00000007 << 0)            /*!< R/W 0x0  Cputag.priority = 0 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT3_PRI0_ROUTE(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_EXT3_PRI0_ROUTE(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_RRING_MAP4
  * @brief
  * @{
  */
#define MASK_EXT4_PRI7_ROUTE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  Cputag.priority = 7 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI7_ROUTE(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_EXT4_PRI7_ROUTE(x) ((u32)(((x >> 28) & 0x00000007)))
#define MASK_EXT4_PRI6_ROUTE   ((u32)0x00000007 << 24)           /*!< R/W 0x0  Cputag.priority = 6 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI6_ROUTE(x)     (((u32)((x) & 0x00000007) << 24))
#define GET_EXT4_PRI6_ROUTE(x) ((u32)(((x >> 24) & 0x00000007)))
#define MASK_EXT4_PRI5_ROUTE   ((u32)0x00000007 << 20)           /*!< R/W 0x0  Cputag.priority = 5 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI5_ROUTE(x)     (((u32)((x) & 0x00000007) << 20))
#define GET_EXT4_PRI5_ROUTE(x) ((u32)(((x >> 20) & 0x00000007)))
#define MASK_EXT4_PRI4_ROUTE   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Cputag.priority = 4 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI4_ROUTE(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_EXT4_PRI4_ROUTE(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_EXT4_PRI3_ROUTE   ((u32)0x00000007 << 12)           /*!< R/W 0x0  Cputag.priority = 3 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI3_ROUTE(x)     (((u32)((x) & 0x00000007) << 12))
#define GET_EXT4_PRI3_ROUTE(x) ((u32)(((x >> 12) & 0x00000007)))
#define MASK_EXT4_PRI2_ROUTE   ((u32)0x00000007 << 8)            /*!< R/W 0x0  Cputag.priority = 2 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI2_ROUTE(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_EXT4_PRI2_ROUTE(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_EXT4_PRI1_ROUTE   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Cputag.priority = 1 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI1_ROUTE(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_EXT4_PRI1_ROUTE(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_EXT4_PRI0_ROUTE   ((u32)0x00000007 << 0)            /*!< R/W 0x0  Cputag.priority = 0 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT4_PRI0_ROUTE(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_EXT4_PRI0_ROUTE(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_RRING_MAP5
  * @brief
  * @{
  */
#define MASK_EXT5_PRI7_ROUTE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  Cputag.priority = 7 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI7_ROUTE(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_EXT5_PRI7_ROUTE(x) ((u32)(((x >> 28) & 0x00000007)))
#define MASK_EXT5_PRI6_ROUTE   ((u32)0x00000007 << 24)           /*!< R/W 0x0  Cputag.priority = 6 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI6_ROUTE(x)     (((u32)((x) & 0x00000007) << 24))
#define GET_EXT5_PRI6_ROUTE(x) ((u32)(((x >> 24) & 0x00000007)))
#define MASK_EXT5_PRI5_ROUTE   ((u32)0x00000007 << 20)           /*!< R/W 0x0  Cputag.priority = 5 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI5_ROUTE(x)     (((u32)((x) & 0x00000007) << 20))
#define GET_EXT5_PRI5_ROUTE(x) ((u32)(((x >> 20) & 0x00000007)))
#define MASK_EXT5_PRI4_ROUTE   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Cputag.priority = 4 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI4_ROUTE(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_EXT5_PRI4_ROUTE(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_EXT5_PRI3_ROUTE   ((u32)0x00000007 << 12)           /*!< R/W 0x0  Cputag.priority = 3 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI3_ROUTE(x)     (((u32)((x) & 0x00000007) << 12))
#define GET_EXT5_PRI3_ROUTE(x) ((u32)(((x >> 12) & 0x00000007)))
#define MASK_EXT5_PRI2_ROUTE   ((u32)0x00000007 << 8)            /*!< R/W 0x0  Cputag.priority = 2 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI2_ROUTE(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_EXT5_PRI2_ROUTE(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_EXT5_PRI1_ROUTE   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Cputag.priority = 1 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI1_ROUTE(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_EXT5_PRI1_ROUTE(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_EXT5_PRI0_ROUTE   ((u32)0x00000007 << 0)            /*!< R/W 0x0  Cputag.priority = 0 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT5_PRI0_ROUTE(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_EXT5_PRI0_ROUTE(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_RRING_MAP6
  * @brief
  * @{
  */
#define MASK_EXT6_PRI7_ROUTE   ((u32)0x00000007 << 28)           /*!< R/W 0x0  Cputag.priority = 7 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI7_ROUTE(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_EXT6_PRI7_ROUTE(x) ((u32)(((x >> 28) & 0x00000007)))
#define MASK_EXT6_PRI6_ROUTE   ((u32)0x00000007 << 24)           /*!< R/W 0x0  Cputag.priority = 6 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI6_ROUTE(x)     (((u32)((x) & 0x00000007) << 24))
#define GET_EXT6_PRI6_ROUTE(x) ((u32)(((x >> 24) & 0x00000007)))
#define MASK_EXT6_PRI5_ROUTE   ((u32)0x00000007 << 20)           /*!< R/W 0x0  Cputag.priority = 5 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI5_ROUTE(x)     (((u32)((x) & 0x00000007) << 20))
#define GET_EXT6_PRI5_ROUTE(x) ((u32)(((x >> 20) & 0x00000007)))
#define MASK_EXT6_PRI4_ROUTE   ((u32)0x00000007 << 16)           /*!< R/W 0x0  Cputag.priority = 4 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI4_ROUTE(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_EXT6_PRI4_ROUTE(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_EXT6_PRI3_ROUTE   ((u32)0x00000007 << 12)           /*!< R/W 0x0  Cputag.priority = 3 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI3_ROUTE(x)     (((u32)((x) & 0x00000007) << 12))
#define GET_EXT6_PRI3_ROUTE(x) ((u32)(((x >> 12) & 0x00000007)))
#define MASK_EXT6_PRI2_ROUTE   ((u32)0x00000007 << 8)            /*!< R/W 0x0  Cputag.priority = 2 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI2_ROUTE(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_EXT6_PRI2_ROUTE(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_EXT6_PRI1_ROUTE   ((u32)0x00000007 << 4)            /*!< R/W 0x0  Cputag.priority = 1 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI1_ROUTE(x)     (((u32)((x) & 0x00000007) << 4))
#define GET_EXT6_PRI1_ROUTE(x) ((u32)(((x >> 4) & 0x00000007)))
#define MASK_EXT6_PRI0_ROUTE   ((u32)0x00000007 << 0)            /*!< R/W 0x0  Cputag.priority = 0 route select. {0,1,7} ring1, {2} ring2, {3} ring3,...*/
#define EXT6_PRI0_ROUTE(x)     (((u32)((x) & 0x00000007) << 0))
#define GET_EXT6_PRI0_ROUTE(x) ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_RXFDPx
  * @brief
  * @{
  */
#define MASK_RXFDPx   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Rx  queue Descriptor Pointer to the Rx Ring*/
#define RXFDPx(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_RXFDPx(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_RXCDOx
  * @brief
  * @{
  */
#define MASK_RXCDOx   ((u16)0x00000FFF << 0)           /*!< R 0x0  Rx Current Descriptor Offset of  queue: RxFDP+RxCDO = current descriptor pointer.  CDO increments by 16 each time (each increment is one byte)*/
#define RXCDOx(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXCDOx(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXRINGSIZEx
  * @brief
  * @{
  */
#define MASK_RXRINGSIZEx   ((u16)0x00000FFF << 0)           /*!< R/W 0x0  The total number of descriptors in the Rx descriptor rings of  queue.  Act as bit mask, eg. RxRingSize [11:0]:  0000_0000_1111:  16 descriptors  0000_0001_1111:  32 descriptors  0000_0011_1111:  64 descriptors  0000_0111_1111:  128 descriptors  0000_1111_1111:  256 descriptors  0001_1111_1111:  512 descriptors  0011_1111_1111:  1024 descriptors  0111_1111_1111:  2048 descriptors  1111_1111_1111:  4096 descriptors  Any other value in this register yields undefined results.*/
#define RXRINGSIZEx(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXRINGSIZEx(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_CPU_DESNx
  * @brief
  * @{
  */
#define MASK_ETHRNTRXCPU_DES_NUMx   ((u32)0x00000FFF << 0)           /*!< R/W 0x0  Indicate the number of descriptor of  queue, which has been finished Rx process and returned to IO by CPU. After ending Rx process, CPU needs to update this field.*/
#define ETHRNTRXCPU_DES_NUMx(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_ETHRNTRXCPU_DES_NUMx(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_DES_THRESx
  * @brief
  * @{
  */
#define MASK_RX_PSE_DES_THRES_ONx    ((u32)0x00000FFF << 16)           /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be assert.*/
#define RX_PSE_DES_THRES_ONx(x)      (((u32)((x) & 0x00000FFF) << 16))
#define GET_RX_PSE_DES_THRES_ONx(x)  ((u32)(((x >> 16) & 0x00000FFF)))
#define MASK_RX_PSE_DES_THRES_OFFx   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be de-assert.  Rx_Pse_Des_Thress_off_ should be less than (EthrntRxCPU_DES_NUM_ - X). X is the maximum rx descriptors will be used for single ingress packet.*/
#define RX_PSE_DES_THRES_OFFx(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_RX_PSE_DES_THRES_OFFx(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXFDP2
  * @brief
  * @{
  */
#define MASK_RXFDP2   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Rx  queue Descriptor Pointer to the Rx Ring*/
#define RXFDP2(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_RXFDP2(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_RXCDO2
  * @brief
  * @{
  */
#define MASK_RXCDO2   ((u16)0x00000FFF << 0)           /*!< R 0x0  Rx Current Descriptor Offset of  queue: RxFDP+RxCDO = current descriptor pointer.  CDO increments by 16 each time (each increment is one byte)*/
#define RXCDO2(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXCDO2(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXRINGSIZE2
  * @brief
  * @{
  */
#define MASK_RXRINGSIZE2   ((u16)0x00000FFF << 0)           /*!< R/W 0x0  The total number of descriptors in the Rx descriptor rings of  queue.  Act as bit mask, eg. RxRingSize [11:0]:  0000_0000_1111:  16 descriptors  0000_0001_1111:  32 descriptors  0000_0011_1111:  64 descriptors  0000_0111_1111:  128 descriptors  0000_1111_1111:  256 descriptors  0001_1111_1111:  512 descriptors  0011_1111_1111:  1024 descriptors  0111_1111_1111:  2048 descriptors  1111_1111_1111:  4096 descriptors  Any other value in this register yields undefined results.*/
#define RXRINGSIZE2(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXRINGSIZE2(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_CPU_DESN2
  * @brief
  * @{
  */
#define MASK_ETHRNTRXCPU_DES_NUM2   ((u32)0x00000FFF << 0)           /*!< R/W 0x0  Indicate the number of descriptor of  queue, which has been finished Rx process and returned to IO by CPU. After ending Rx process, CPU needs to update this field.*/
#define ETHRNTRXCPU_DES_NUM2(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_ETHRNTRXCPU_DES_NUM2(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_DES_THRES2
  * @brief
  * @{
  */
#define MASK_RX_PSE_DES_THRES_ON2    ((u32)0x00000FFF << 16)           /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be assert.*/
#define RX_PSE_DES_THRES_ON2(x)      (((u32)((x) & 0x00000FFF) << 16))
#define GET_RX_PSE_DES_THRES_ON2(x)  ((u32)(((x >> 16) & 0x00000FFF)))
#define MASK_RX_PSE_DES_THRES_OFF2   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be de-assert.  Rx_Pse_Des_Thress_off_ should be less than (EthrntRxCPU_DES_NUM_ - X). X is the maximum rx descriptors will be used for single ingress packet.*/
#define RX_PSE_DES_THRES_OFF2(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_RX_PSE_DES_THRES_OFF2(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXFDP3
  * @brief
  * @{
  */
#define MASK_RXFDP3   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Rx  queue Descriptor Pointer to the Rx Ring*/
#define RXFDP3(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_RXFDP3(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_RXCDO3
  * @brief
  * @{
  */
#define MASK_RXCDO3   ((u16)0x00000FFF << 0)           /*!< R 0x0  Rx Current Descriptor Offset of  queue: RxFDP+RxCDO = current descriptor pointer.  CDO increments by 16 each time (each increment is one byte)*/
#define RXCDO3(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXCDO3(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXRINGSIZE3
  * @brief
  * @{
  */
#define MASK_RXRINGSIZE3   ((u16)0x00000FFF << 0)           /*!< R/W 0x0  The total number of descriptors in the Rx descriptor rings of  queue.  Act as bit mask, eg. RxRingSize [11:0]:  0000_0000_1111:  16 descriptors  0000_0001_1111:  32 descriptors  0000_0011_1111:  64 descriptors  0000_0111_1111:  128 descriptors  0000_1111_1111:  256 descriptors  0001_1111_1111:  512 descriptors  0011_1111_1111:  1024 descriptors  0111_1111_1111:  2048 descriptors  1111_1111_1111:  4096 descriptors  Any other value in this register yields undefined results.*/
#define RXRINGSIZE3(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXRINGSIZE3(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_CPU_DESN3
  * @brief
  * @{
  */
#define MASK_ETHRNTRXCPU_DES_NUM3   ((u32)0x00000FFF << 0)           /*!< R/W 0x0  Indicate the number of descriptor of  queue, which has been finished Rx process and returned to IO by CPU. After ending Rx process, CPU needs to update this field.*/
#define ETHRNTRXCPU_DES_NUM3(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_ETHRNTRXCPU_DES_NUM3(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_DES_THRES3
  * @brief
  * @{
  */
#define MASK_RX_PSE_DES_THRES_ON3    ((u32)0x00000FFF << 16)           /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be assert.*/
#define RX_PSE_DES_THRES_ON3(x)      (((u32)((x) & 0x00000FFF) << 16))
#define GET_RX_PSE_DES_THRES_ON3(x)  ((u32)(((x >> 16) & 0x00000FFF)))
#define MASK_RX_PSE_DES_THRES_OFF3   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be de-assert.  Rx_Pse_Des_Thress_off_ should be less than (EthrntRxCPU_DES_NUM_ - X). X is the maximum rx descriptors will be used for single ingress packet.*/
#define RX_PSE_DES_THRES_OFF3(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_RX_PSE_DES_THRES_OFF3(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXFDP4
  * @brief
  * @{
  */
#define MASK_RXFDP4   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Rx  queue Descriptor Pointer to the Rx Ring*/
#define RXFDP4(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_RXFDP4(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_RXCDO4
  * @brief
  * @{
  */
#define MASK_RXCDO4   ((u16)0x00000FFF << 0)           /*!< R 0x0  Rx Current Descriptor Offset of  queue: RxFDP+RxCDO = current descriptor pointer.  CDO increments by 16 each time (each increment is one byte)*/
#define RXCDO4(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXCDO4(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXRINGSIZE4
  * @brief
  * @{
  */
#define MASK_RXRINGSIZE4   ((u16)0x00000FFF << 0)           /*!< R/W 0x0  The total number of descriptors in the Rx descriptor rings of  queue.  Act as bit mask, eg. RxRingSize [11:0]:  0000_0000_1111:  16 descriptors  0000_0001_1111:  32 descriptors  0000_0011_1111:  64 descriptors  0000_0111_1111:  128 descriptors  0000_1111_1111:  256 descriptors  0001_1111_1111:  512 descriptors  0011_1111_1111:  1024 descriptors  0111_1111_1111:  2048 descriptors  1111_1111_1111:  4096 descriptors  Any other value in this register yields undefined results.*/
#define RXRINGSIZE4(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXRINGSIZE4(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_CPU_DESN4
  * @brief
  * @{
  */
#define MASK_ETHRNTRXCPU_DES_NUM4   ((u32)0x00000FFF << 0)           /*!< R/W 0x0  Indicate the number of descriptor of  queue, which has been finished Rx process and returned to IO by CPU. After ending Rx process, CPU needs to update this field.*/
#define ETHRNTRXCPU_DES_NUM4(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_ETHRNTRXCPU_DES_NUM4(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_DES_THRES4
  * @brief
  * @{
  */
#define MASK_RX_PSE_DES_THRES_ON4    ((u32)0x00000FFF << 16)           /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be assert.*/
#define RX_PSE_DES_THRES_ON4(x)      (((u32)((x) & 0x00000FFF) << 16))
#define GET_RX_PSE_DES_THRES_ON4(x)  ((u32)(((x >> 16) & 0x00000FFF)))
#define MASK_RX_PSE_DES_THRES_OFF4   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be de-assert.  Rx_Pse_Des_Thress_off_ should be less than (EthrntRxCPU_DES_NUM_ - X). X is the maximum rx descriptors will be used for single ingress packet.*/
#define RX_PSE_DES_THRES_OFF4(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_RX_PSE_DES_THRES_OFF4(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXFDP5
  * @brief
  * @{
  */
#define MASK_RXFDP5   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Rx  queue Descriptor Pointer to the Rx Ring*/
#define RXFDP5(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_RXFDP5(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_RXCDO5
  * @brief
  * @{
  */
#define MASK_RXCDO5   ((u16)0x00000FFF << 0)           /*!< R 0x0  Rx Current Descriptor Offset of  queue: RxFDP+RxCDO = current descriptor pointer.  CDO increments by 16 each time (each increment is one byte)*/
#define RXCDO5(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXCDO5(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXRINGSIZE5
  * @brief
  * @{
  */
#define MASK_RXRINGSIZE5   ((u16)0x00000FFF << 0)           /*!< R/W 0x0  The total number of descriptors in the Rx descriptor rings of  queue.  Act as bit mask, eg. RxRingSize [11:0]:  0000_0000_1111:  16 descriptors  0000_0001_1111:  32 descriptors  0000_0011_1111:  64 descriptors  0000_0111_1111:  128 descriptors  0000_1111_1111:  256 descriptors  0001_1111_1111:  512 descriptors  0011_1111_1111:  1024 descriptors  0111_1111_1111:  2048 descriptors  1111_1111_1111:  4096 descriptors  Any other value in this register yields undefined results.*/
#define RXRINGSIZE5(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXRINGSIZE5(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_CPU_DESN5
  * @brief
  * @{
  */
#define MASK_ETHRNTRXCPU_DES_NUM5   ((u32)0x00000FFF << 0)           /*!< R/W 0x0  Indicate the number of descriptor of  queue, which has been finished Rx process and returned to IO by CPU. After ending Rx process, CPU needs to update this field.*/
#define ETHRNTRXCPU_DES_NUM5(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_ETHRNTRXCPU_DES_NUM5(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_DES_THRES5
  * @brief
  * @{
  */
#define MASK_RX_PSE_DES_THRES_ON5    ((u32)0x00000FFF << 16)           /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be assert.*/
#define RX_PSE_DES_THRES_ON5(x)      (((u32)((x) & 0x00000FFF) << 16))
#define GET_RX_PSE_DES_THRES_ON5(x)  ((u32)(((x >> 16) & 0x00000FFF)))
#define MASK_RX_PSE_DES_THRES_OFF5   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be de-assert.  Rx_Pse_Des_Thress_off_ should be less than (EthrntRxCPU_DES_NUM_ - X). X is the maximum rx descriptors will be used for single ingress packet.*/
#define RX_PSE_DES_THRES_OFF5(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_RX_PSE_DES_THRES_OFF5(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXFDP6
  * @brief
  * @{
  */
#define MASK_RXFDP6   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Rx  queue Descriptor Pointer to the Rx Ring*/
#define RXFDP6(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_RXFDP6(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_RXCDO6
  * @brief
  * @{
  */
#define MASK_RXCDO6   ((u16)0x00000FFF << 0)           /*!< R 0x0  Rx Current Descriptor Offset of  queue: RxFDP+RxCDO = current descriptor pointer.  CDO increments by 16 each time (each increment is one byte)*/
#define RXCDO6(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXCDO6(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RXRINGSIZE6
  * @brief
  * @{
  */
#define MASK_RXRINGSIZE6   ((u16)0x00000FFF << 0)           /*!< R/W 0x0  The total number of descriptors in the Rx descriptor rings of  queue.  Act as bit mask, eg. RxRingSize [11:0]:  0000_0000_1111:  16 descriptors  0000_0001_1111:  32 descriptors  0000_0011_1111:  64 descriptors  0000_0111_1111:  128 descriptors  0000_1111_1111:  256 descriptors  0001_1111_1111:  512 descriptors  0011_1111_1111:  1024 descriptors  0111_1111_1111:  2048 descriptors  1111_1111_1111:  4096 descriptors  Any other value in this register yields undefined results.*/
#define RXRINGSIZE6(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXRINGSIZE6(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_CPU_DESN6
  * @brief
  * @{
  */
#define MASK_ETHRNTRXCPU_DES_NUM6   ((u32)0x00000FFF << 0)           /*!< R/W 0x0  Indicate the number of descriptor of  queue, which has been finished Rx process and returned to IO by CPU. After ending Rx process, CPU needs to update this field.*/
#define ETHRNTRXCPU_DES_NUM6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_ETHRNTRXCPU_DES_NUM6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_DES_THRES6
  * @brief
  * @{
  */
#define MASK_RX_PSE_DES_THRES_ON6    ((u32)0x00000FFF << 16)           /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be assert.*/
#define RX_PSE_DES_THRES_ON6(x)      (((u32)((x) & 0x00000FFF) << 16))
#define GET_RX_PSE_DES_THRES_ON6(x)  ((u32)(((x >> 16) & 0x00000FFF)))
#define MASK_RX_PSE_DES_THRES_OFF6   ((u32)0x00000FFF << 0)            /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num2 and the descriptor   of  queue currently in use by Ethernet Module in which flow control will be de-assert.  Rx_Pse_Des_Thress_off_ should be less than (EthrntRxCPU_DES_NUM_ - X). X is the maximum rx descriptors will be used for single ingress packet.*/
#define RX_PSE_DES_THRES_OFF6(x)     (((u32)((x) & 0x00000FFF) << 0))
#define GET_RX_PSE_DES_THRES_OFF6(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_FDP1
  * @brief
  * @{
  */
#define MASK_RXFDP1   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  Rx 1st queue Descriptor Pointer to the Rx Ring*/
#define RXFDP1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_RXFDP1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_RX_CDO1
  * @brief
  * @{
  */
#define MASK_RXCDO1   ((u16)0x00000FFF << 0)           /*!< R 0x0  Rx Current Descriptor Offset of 1st queue: RxFDP+RxCDO = current descriptor pointer.  CDO increments by 16 each time (each increment is one byte).*/
#define RXCDO1(x)     (((u16)((x) & 0x00000FFF) << 0))
#define GET_RXCDO1(x) ((u16)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ETH_RX_RINGSIZE1
  * @brief
  * @{
  */
#define MASK_RXRINGSIZE_1_LOW    ((u16)0x000000FF << 8)           /*!< R/W 0x0  This is the total number of descriptors in the Rx descriptor rings of 1st queue.  Act as bit mask, eg. RxRingSize [11:0]: 0000_0000_1111:  16 descriptors 0000_0001_1111:  32 descriptors 0000_0011_1111:  64 descriptors 0000_0111_1111:  128 descriptors 0000_1111_1111:  256 descriptors 0001_1111_1111:  512 descriptors 0011_1111_1111:  1024 descriptors 0111_1111_1111:  2048 descriptors  1111_1111_1111:  4096 descriptors   Any other value in this register yields undefined results.*/
#define RXRINGSIZE_1_LOW(x)      (((u16)((x) & 0x000000FF) << 8))
#define GET_RXRINGSIZE_1_LOW(x)  ((u16)(((x >> 8) & 0x000000FF)))
#define MASK_RXRINGSIZE_1_HIGH   ((u16)0x0000000F << 0)           /*!< R/W 0x0  This is the bits11-8 of RxRingSize1.*/
#define RXRINGSIZE_1_HIGH(x)     (((u16)((x) & 0x0000000F) << 0))
#define GET_RXRINGSIZE_1_HIGH(x) ((u16)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ETH_SMSA
  * @brief
  * @{
  */
#define MASK_SMSA   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  SRAM mapping start address for header mapping to sram*/
#define SMSA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GET_SMSA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup ETH_PROBE_SELEXCT
  * @brief
  * @{
  */
#define MASK_PROBE_SIGNAL_SELECT   ((u32)0x0000000F << 0)           /*!< R/W 0x0  For debug port signals use Value = 4: Tx/Rx related signals*/
#define PROBE_SIGNAL_SELECT(x)     (((u32)((x) & 0x0000000F) << 0))
#define GET_PROBE_SIGNAL_SELECT(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ETH_DIAG1
  * @brief
  * @{
  */
#define MASK_RXMRING_STATE   ((u32)0x00000007 << 3)           /*!< R   */
#define RXMRING_STATE(x)     (((u32)((x) & 0x00000007) << 3))
#define GET_RXMRING_STATE(x) ((u32)(((x >> 3) & 0x00000007)))
#define MASK_LSO_STATS       ((u32)0x00000007 << 0)           /*!< R   Reserved(3bits)*/
#define LSO_STATS(x)         (((u32)((x) & 0x00000007) << 0))
#define GET_LSO_STATS(x)     ((u32)(((x >> 0) & 0x00000007)))
/** @} */

/** @defgroup ETH_CONFIG_CLK_AND_RX_PSE_DES_THRES_H
  * @brief
  * @{
  */
#define BIT_SET_D_TXC                 ((u32)0x00000001 << 21)           /*!< R/W 0x0  Add delay to gmii_txc_out. Used in clk.  Rgmii_1000: add 2nS delay.  Rgmii_100:  add 4nS delay.  Rgmii_10:   add 4nS delay.*/
#define SET_D_TXC(x)                  ((u32)(((x) & 0x00000001) << 21))
#define GET_SET_D_TXC(x)              ((u32)(((x >> 21) & 0x00000001)))
#define MASK_RXC_DLY_SEL              ((u32)0x0000001F << 16)           /*!< R/W 0x0  32-tap delay chain for rxc. This field is valid when CONFIG_CLK_REG.dis_dly_mode is 0.*/
#define RXC_DLY_SEL(x)                (((u32)((x) & 0x0000001F) << 16))
#define GET_RXC_DLY_SEL(x)            ((u32)(((x >> 16) & 0x0000001F)))
#define MASK_RX_PSE_DES_THRES_OFF_1   ((u32)0x0000000F << 0)            /*!< R/W 0x0  This is the Rx_Pse_Des_Thres_off1[11:8].*/
#define RX_PSE_DES_THRES_OFF_1(x)     (((u32)((x) & 0x0000000F) << 0))
#define GET_RX_PSE_DES_THRES_OFF_1(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ETH_ETHRNTRXCPU_DES_NUM1
  * @brief
  * @{
  */
#define MASK_ETHRNTRXCPU_DES_NUM_1_7_0    ((u32)0x000000FF << 24)           /*!< R/W 0x0  Indicate the number of descriptor of 1st queue, which has been finished Rx process and returned to IO by CPU. After ending Rx process, CPU needs to update this field.*/
#define ETHRNTRXCPU_DES_NUM_1_7_0(x)      (((u32)((x) & 0x000000FF) << 24))
#define GET_ETHRNTRXCPU_DES_NUM_1_7_0(x)  ((u32)(((x >> 24) & 0x000000FF)))
#define MASK_RX_PSE_DES_THRES_ON_1_7_0    ((u32)0x000000FF << 16)           /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num1 and the descriptor   of 1st queue currently in use by Ethernet Module in which flow control will be assert.*/
#define RX_PSE_DES_THRES_ON_1_7_0(x)      (((u32)((x) & 0x000000FF) << 16))
#define GET_RX_PSE_DES_THRES_ON_1_7_0(x)  ((u32)(((x >> 16) & 0x000000FF)))
#define MASK_RX_PSE_DES_THRES_OFF_1_7_0   ((u32)0x000000FF << 8)            /*!< R/W 0x0  Specifies the difference between EthrnetRxCPU_Des_Num1 and the descriptor   of 1st queue currently in use by Ethernet Module in which flow control will be de-assert.   Rx_Pse_Des_Thress_off_1 should be less than (EthrntRxCPU_DES_NUM_1 - X). X is the maximum rx descriptors will be used for single ingress packet.*/
#define RX_PSE_DES_THRES_OFF_1_7_0(x)     (((u32)((x) & 0x000000FF) << 8))
#define GET_RX_PSE_DES_THRES_OFF_1_7_0(x) ((u32)(((x >> 8) & 0x000000FF)))
#define MASK_ETHRNTRXCPU_DES_NUM_1_11_8   ((u32)0x0000000F << 4)            /*!< R/W 0x0  This is the EthrntRxCPU_Des_Num1[11:8].*/
#define ETHRNTRXCPU_DES_NUM_1_11_8(x)     (((u32)((x) & 0x0000000F) << 4))
#define GET_ETHRNTRXCPU_DES_NUM_1_11_8(x) ((u32)(((x >> 4) & 0x0000000F)))
#define MASK_RX_PSE_DES_THRES_ON_1_11_8   ((u32)0x0000000F << 0)            /*!< R/W 0x0  This is the Rx_Pse_Des_Thres_on1[11:8].*/
#define RX_PSE_DES_THRES_ON_1_11_8(x)     (((u32)((x) & 0x0000000F) << 0))
#define GET_RX_PSE_DES_THRES_ON_1_11_8(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ETH_ETHER_IO_CMD
  * @brief
  * @{
  */
#define BIT_MAX_DMA_SEL_0          ((u32)0x00000001 << 31)           /*!< R/W 0x0  Select the dma burst size on bus(memory controller should assert continuous btrdy).  00:16 DW(1DW=4B)  01:32 DW 10:64 DW 11:128DW.  AmebaI/AmebaPro/AmebaPro2 only support DMA burst length = 16DW*/
#define MAX_DMA_SEL_0(x)           ((u32)(((x) & 0x00000001) << 31))
#define GET_MAX_DMA_SEL_0(x)       ((u32)(((x >> 31) & 0x00000001)))
#define BIT_SHORTDESFORMAT         ((u32)0x00000001 << 30)           /*!< R/W 0x0  Short descriptor format. Set 1 tx/rx descriptor will use old format  - 4x32bit each item, set 0 use new format to support sram mapping AmebaI/AmebaPro/AmebaPro2 only support old format (1b'1)*/
#define SHORTDESFORMAT(x)          ((u32)(((x) & 0x00000001) << 30))
#define GET_SHORTDESFORMAT(x)      ((u32)(((x >> 30) & 0x00000001)))
#define BIT_MAX_DMA_SEL_1          ((u32)0x00000001 << 29)           /*!< R/W 0x0  see Ethernet_IO_CMD.bit31 AmebaI/AmebaPro/AmebaPro2 only support DMA burst length = 16DW*/
#define MAX_DMA_SEL_1(x)           ((u32)(((x) & 0x00000001) << 29))
#define GET_MAX_DMA_SEL_1(x)       ((u32)(((x >> 29) & 0x00000001)))
#define BIT_EN_EARLY_TX            ((u32)0x00000001 << 28)           /*!< R/W 0x0  default 0,  0: disable,  1: enable.   Disable early tx by GAMC while tx command descriptor.IPCS, UDPCS or TCPCS are set to high. For multiple tx descriptor application, this field should be 0. For low lxbus throughput environment, this field should be 0. For slow clk_gmac application, this field should be 0.*/
#define EN_EARLY_TX(x)             ((u32)(((x) & 0x00000001) << 28))
#define GET_EN_EARLY_TX(x)         ((u32)(((x >> 28) & 0x00000001)))
#define MASK_TXPKTTIMER            ((u32)0x0000000F << 24)           /*!< R/W 0x0  Timer to trigger TxOK interrupt before receipt of TxIntMitigation pkts. 0000 - no timer set 0001 ~ 1111 : the timer interval defining a multiple of TU  ex:  0011 = timer interval set to 3 x4 =12TU  Once TxOK is asserted the timer  mechanism is reinitialized.*/
#define TXPKTTIMER(x)              (((u32)((x) & 0x0000000F) << 24))
#define GET_TXPKTTIMER(x)          ((u32)(((x >> 24) & 0x0000000F)))
#define BIT_TXINTMITIGATION_3      ((u32)0x00000001 << 23)           /*!< R/W 0x0  see Ethernet_IO_CMD.bit18-16*/
#define TXINTMITIGATION_3(x)       ((u32)(((x) & 0x00000001) << 23))
#define GET_TXINTMITIGATION_3(x)   ((u32)(((x >> 23) & 0x00000001)))
#define BIT_RXPKTTIMER_3           ((u32)0x00000001 << 22)           /*!< R/W 0x0  see Ethernet_IO_CMD.bit15-13*/
#define RXPKTTIMER_3(x)            ((u32)(((x) & 0x00000001) << 22))
#define GET_RXPKTTIMER_3(x)        ((u32)(((x >> 22) & 0x00000001)))
#define BIT_RXINTMITIGATION_3      ((u32)0x00000001 << 21)           /*!< R/W 0x0  see Ethernet_IO_CMD.bit10-8*/
#define RXINTMITIGATION_3(x)       ((u32)(((x) & 0x00000001) << 21))
#define GET_RXINTMITIGATION_3(x)   ((u32)(((x >> 21) & 0x00000001)))
#define MASK_TSH                   ((u32)0x00000003 << 19)           /*!< R/W 0x0  Tx Threshold: Specifies the threshold level in the Tx FIFO to begin the transmission. When the byte count of the data in the Tx FIFO reaches this level, (or the FIFO contains at least one complete packet or the end of a packet) the Ethernet module will transmit this packet.  00:128B.  01:256B.  10:512B.  11:1024B.     For poor lxbus bandwidth, tsh should be set b11.       In rle0516, this field should be setted 0 or 1 (128B).*/
#define TSH(x)                     (((u32)((x) & 0x00000003) << 19))
#define GET_TSH(x)                 ((u32)(((x >> 19) & 0x00000003)))
#define MASK_TXINTMITIGATION_2_0   ((u32)0x00000007 << 16)           /*!< R/W 0x0  This sets the number of packets received before TxOK interrupt is triggered.  0000- 1  pkt  0001- 4  pkts  0010- 8  pkts  0011- 12 pkts  0100- 16 pkts  0101- 20 pkts  0110- 24 pkts  0111- 28 pkts ...*/
#define TXINTMITIGATION_2_0(x)     (((u32)((x) & 0x00000007) << 16))
#define GET_TXINTMITIGATION_2_0(x) ((u32)(((x >> 16) & 0x00000007)))
#define MASK_RXPKTTIMER_2_0        ((u32)0x00000007 << 13)           /*!< R/W 0x0  Timer to trigger RxOK interrupt before receipt of RxIntMitigation pkts.  0000 - no timer set  0001 ~ 1111 : the timer interval defining a multiple of TU  ex:  011 = timer interval set to 3 x4 =12TU  This only applies to packets of size larger than 128 bytes.   Once RxOK is asserted the timer  mechanism is reinitialized.  For ingress pkt which is short than 128B, RxOK interrupt asserts after DMA completes.*/
#define RXPKTTIMER_2_0(x)          (((u32)((x) & 0x00000007) << 13))
#define GET_RXPKTTIMER_2_0(x)      ((u32)(((x >> 13) & 0x00000007)))
#define MASK_RXFTH                 ((u32)0x00000003 << 11)           /*!< R/W 0x0  Rx Threshold: Specifies the threshold level in the Rx FIFO to begin the transmission. When the byte count of the data in the Rx FIFO reaches this level, (or the FIFO contains at least one complete packet or the end of a packet) the Ethernet module will transmit this packet.  00: 1024B  01: 128B  10: 256B  11: 512B  The rx threshold should be larger than maximum dma burst size. The combination of max_dma_sel and rxfth are listed as followed: max_dma_sel            rxfth                                                                                        b00(64B)               b00(1024B), b01 (128B), b10(256B), b11(512B).                                                b01(128B)              b00(1024B), b10(256B), b11(512B).                                                            b10(256B)              b00(1024B), b11(512B),                                                                     In rle0516:  SW should not set rxfth to 0b00 due to the rx fifo size is only 512B. The recommend value of rle0516 is 0b11 for performance concerns.  00 = no rx threshold. The Ethernet Module begins the transfer of data after having received a whole packet in the FIFO.  01 = 32 bytes  10 = 64 bytes  11 = 128 bytes   max_dma_sel            rxfth                                      b0(64B)                b10(64B), b11(128B).                       b1(128B)               b11(128B).*/
#define RXFTH(x)                   (((u32)((x) & 0x00000003) << 11))
#define GET_RXFTH(x)               ((u32)(((x >> 11) & 0x00000003)))
#define MASK_RXINTMITIGATION_2_0   ((u32)0x00000007 << 8)            /*!< R/W 0x0  This sets the number of packets received before RxOK interrupt is triggered.  0000- 1 pkt  0001- 4 pkts  0010- 8 pkts  0011- 12pkts  0100- 16 pkts  0101- 20 pkts  0110- 24 pkts  0111- 28 pkts ...*/
#define RXINTMITIGATION_2_0(x)     (((u32)((x) & 0x00000007) << 8))
#define GET_RXINTMITIGATION_2_0(x) ((u32)(((x >> 8) & 0x00000007)))
#define MASK_REG_INT_TIMER_SEL     ((u32)0x00000003 << 6)            /*!< R/W 0x0  RXPktTimer, TXPktTimer Unit. (TU)          speed      TU               00:      10M      25.6uS 00:      100M      2.56uS 00:      1000M      256nS 01:      10M      102.4uS 01:      100M      10.24uS 01:      1000M      1.024uS 10:      10M      409.6uS 10:      100M      40.96uS 10:      1000M      4.096uS 11:      10M      1.6mS 11:      100M      0.16mS 11:      1000M      16uS*/
#define REG_INT_TIMER_SEL(x)       (((u32)((x) & 0x00000003) << 6))
#define GET_REG_INT_TIMER_SEL(x)   ((u32)(((x >> 6) & 0x00000003)))
#define BIT_RE                     ((u32)0x00000001 << 5)            /*!< R/W 0x0  MII Rx Enable*/
#define RE(x)                      ((u32)(((x) & 0x00000001) << 5))
#define GET_RE(x)                  ((u32)(((x >> 5) & 0x00000001)))
#define BIT_TE                     ((u32)0x00000001 << 4)            /*!< R/W 0x0  MII Tx Enable*/
#define TE(x)                      ((u32)(((x) & 0x00000001) << 4))
#define GET_TE(x)                  ((u32)(((x >> 4) & 0x00000001)))
#define BIT_TXFN4TH                ((u32)0x00000001 << 3)            /*!< W 0x0  4th Priority DMA-Ethernet Transmit enable. 1: Enable.  0: Disable.*/
#define TXFN4TH(x)                 ((u32)(((x) & 0x00000001) << 3))
#define GET_TXFN4TH(x)             ((u32)(((x >> 3) & 0x00000001)))
#define BIT_TXFN3RD                ((u32)0x00000001 << 2)            /*!< W 0x0  3rd Priority DMA-Ethernet Transmit enable. 1: Enable. 0: Disable.*/
#define TXFN3RD(x)                 ((u32)(((x) & 0x00000001) << 2))
#define GET_TXFN3RD(x)             ((u32)(((x >> 2) & 0x00000001)))
#define BIT_TXFN2ND                ((u32)0x00000001 << 1)            /*!< W 0x0  2nd Priority DMA-Ethernet Transmit enable. 1: Enable. 0: Disable.*/
#define TXFN2ND(x)                 ((u32)(((x) & 0x00000001) << 1))
#define GET_TXFN2ND(x)             ((u32)(((x >> 1) & 0x00000001)))
#define BIT_TXFN1ST                ((u32)0x00000001 << 0)            /*!< W 0x0  1st Priority DMA-Ethernet Transmit enable. 1: Enable. 0: Disable.*/
#define TXFN1ST(x)                 ((u32)(((x) & 0x00000001) << 0))
#define GET_TXFN1ST(x)             ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_IO_CMD1
  * @brief
  * @{
  */
#define BIT_CF_ORGCOL_SEL       ((u32)0x00000001 << 31)           /*!< R/W 0x1  0: using GMAC internal generated collision signal  1: using MII interface collision signal( (default 1: AmebaPro2)*/
#define CF_ORGCOL_SEL(x)        ((u32)(((x) & 0x00000001) << 31))
#define GET_CF_ORGCOL_SEL(x)    ((u32)(((x >> 31) & 0x00000001)))
#define MASK_DSC_FORMAT_EXTRA   ((u32)0x00000007 << 28)           /*!< R/W 3'b011  Extra descriptor format. In rle0437, bit28 is is write only. SW should set this field before set IO_CMD.TE or RE to 1. AmebaI/AmebaPro/AmebaPro 2 only support the fixed value 3b'011*/
#define DSC_FORMAT_EXTRA(x)     (((u32)((x) & 0x00000007) << 28))
#define GET_DSC_FORMAT_EXTRA(x) ((u32)(((x >> 28) & 0x00000007)))
#define BIT_EN_PRECISE_DMA      ((u32)0x00000001 << 27)           /*!< R/W 0x0  0: disable precise DMA transfer.  1: enable precise DMA transfer.  AmebaI/AmebaPro/AmebaPro2 only support 1b'0 (disable precise DMA transfer)*/
#define EN_PRECISE_DMA(x)       ((u32)(((x) & 0x00000001) << 27))
#define GET_EN_PRECISE_DMA(x)   ((u32)(((x >> 27) & 0x00000001)))
#define BIT_RXOKINT_MSK_128B    ((u32)0x00000001 << 26)           /*!< R/W 0x1  1: For ingress pkt which is short than 128B, RxOK interrupt asserts after DMA completes    (compatible issue). 0: For ingress pkt which is short than 128B, RxOK interrupt does not assert after DMA completes.*/
#define RXOKINT_MSK_128B(x)     ((u32)(((x) & 0x00000001) << 26))
#define GET_RXOKINT_MSK_128B(x) ((u32)(((x >> 26) & 0x00000001)))
#define BIT_EN_RX_MRING         ((u32)0x00000001 << 25)           /*!< R/W 0x0  Enable rx multiple rings.  1: rx using multiple rings.    max: 6rings (ring1 to ring6).  0 rx using single ring (ring1 only).   SW should set this field before set IO_CMD.TE or RE to 1 AmebaI/AmebaPro/AmebaPro2 only support single rx ring*/
#define EN_RX_MRING(x)          ((u32)(((x) & 0x00000001) << 25))
#define GET_EN_RX_MRING(x)      ((u32)(((x >> 25) & 0x00000001)))
#define BIT_EN_4GB              ((u32)0x00000001 << 24)           /*!< R/W 0x1  1: support 4GB addressing in lx master bus.    For gmac used in rl0371 and after.  0: no support. For project used in rle0390 and before.   SW should set this field before set IO_CMD.TE or RE to 1 AmebaPro2 support 4GB addressing AmebaI/AmebaPro only support 1GB addressing*/
#define EN_4GB(x)               ((u32)(((x) & 0x00000001) << 24))
#define GET_EN_4GB(x)           ((u32)(((x >> 24) & 0x00000001)))
#define BIT_RXRING6             ((u32)0x00000001 << 21)           /*!< W 0x0  Ethernet-DMA Receive Ring6 enable.  1: Enable.   0: Disable.  If RXRING6 is not used, SW should put one dummy rx command descriptor (own = 0) at RXFDP6_reg.*/
#define RXRING6(x)              ((u32)(((x) & 0x00000001) << 21))
#define GET_RXRING6(x)          ((u32)(((x >> 21) & 0x00000001)))
#define BIT_RXRING5             ((u32)0x00000001 << 20)           /*!< W 0x0  Ethernet-DMA Receive Ring5 enable.  1: Enable.   0: Disable.  If RXRING5 is not used, SW should put one dummy rx command descriptor (own = 0) at RXFDP5_reg.*/
#define RXRING5(x)              ((u32)(((x) & 0x00000001) << 20))
#define GET_RXRING5(x)          ((u32)(((x >> 20) & 0x00000001)))
#define BIT_RXRING4             ((u32)0x00000001 << 19)           /*!< W 0x0  Ethernet-DMA Receive Ring4 enable.  1: Enable.   0: Disable.  If RXRING4 is not used, SW should put one dummy rx command descriptor (own = 0) at RXFDP4_reg.*/
#define RXRING4(x)              ((u32)(((x) & 0x00000001) << 19))
#define GET_RXRING4(x)          ((u32)(((x >> 19) & 0x00000001)))
#define BIT_RXRING3             ((u32)0x00000001 << 18)           /*!< W 0x0  Ethernet-DMA Receive Ring3 enable.  1: Enable.   0: Disable.  If RXRING3 is not used, SW should put one dummy rx command descriptor (own = 0) at RXFDP3_reg.*/
#define RXRING3(x)              ((u32)(((x) & 0x00000001) << 18))
#define GET_RXRING3(x)          ((u32)(((x >> 18) & 0x00000001)))
#define BIT_RXRING2             ((u32)0x00000001 << 17)           /*!< W 0x0  Ethernet-DMA Receive Ring2 enable.  1: Enable.   0: Disable.  If RXRING2 is not used, SW should put one dummy rx command descriptor (own = 0) at RXFDP2_reg.*/
#define RXRING2(x)              ((u32)(((x) & 0x00000001) << 17))
#define GET_RXRING2(x)          ((u32)(((x >> 17) & 0x00000001)))
#define BIT_RXRING1             ((u32)0x00000001 << 16)           /*!< W 0x0  Ethernet-DMA Receive Ring1 enable.  1: Enable.  0: Disable.  If RXRING1 is not used, SW should put one dummy rx command descriptor (own = 0) at RXFDP1_reg.*/
#define RXRING1(x)              ((u32)(((x) & 0x00000001) << 16))
#define GET_RXRING1(x)          ((u32)(((x >> 16) & 0x00000001)))
#define MASK_TX_HL_PRI_SEL      ((u32)0x00000003 << 14)           /*!< R/W 0x0  b00: TX ring uses strict priority.  b01: TX ring uses high and low queue priority.     Inside high queue, tx ring is round robin.     Inside low queue, tx ring is round robin.    Strict priority is used for high and low queue selection. b10 and b11: reserved.  In RL6266, when tx_hl_pri is 1, txq1_h and txq2_h are 1 and txq3_h, txq4_h and txq5_h are 0.   SW should set this field before set IO_CMD.TE to 1.  AmebaI/AmebaPro/AmebaPro2 only support the fixed value 2b'00*/
#define TX_HL_PRI_SEL(x)        (((u32)((x) & 0x00000003) << 14))
#define GET_TX_HL_PRI_SEL(x)    ((u32)(((x >> 14) & 0x00000003)))
#define BIT_TXFN5TH             ((u32)0x00000001 << 8)            /*!< W 0x0  5th Priority DMA-Ethernet Transmit enable.  1: Enable.   0: Disable.  After IO_CMD.TE is set high, TxFN5th is writable.*/
#define TXFN5TH(x)              ((u32)(((x) & 0x00000001) << 8))
#define GET_TXFN5TH(x)          ((u32)(((x >> 8) & 0x00000001)))
#define BIT_TXQ5_H              ((u32)0x00000001 << 4)            /*!< R/W 0x0  1: TxFN5th is a high queue.  0: TxFN5th is a low queue.  This field is only used in eee.*/
#define TXQ5_H(x)               ((u32)(((x) & 0x00000001) << 4))
#define GET_TXQ5_H(x)           ((u32)(((x >> 4) & 0x00000001)))
#define BIT_TXQ4_H              ((u32)0x00000001 << 3)            /*!< R/W 0x0  1: TxFN4th is a high queue.  0: TxFN4th is a low queue.  This field is only used in eee.*/
#define TXQ4_H(x)               ((u32)(((x) & 0x00000001) << 3))
#define GET_TXQ4_H(x)           ((u32)(((x >> 3) & 0x00000001)))
#define BIT_TXQ3_H              ((u32)0x00000001 << 2)            /*!< R/W 0x0  1: TxFN3rd is a high queue.  0: TxFN3rd is a low queue.  This field is only used in eee.*/
#define TXQ3_H(x)               ((u32)(((x) & 0x00000001) << 2))
#define GET_TXQ3_H(x)           ((u32)(((x >> 2) & 0x00000001)))
#define BIT_TXQ2_H              ((u32)0x00000001 << 1)            /*!< R/W 0x0  1: TxFN2nd is a high queue.  0: TxFN2nd is a low queue.  This field is only used in eee.*/
#define TXQ2_H(x)               ((u32)(((x) & 0x00000001) << 1))
#define GET_TXQ2_H(x)           ((u32)(((x >> 1) & 0x00000001)))
#define BIT_TXQ1_H              ((u32)0x00000001 << 0)            /*!< R/W 0x0  1: TxFN1st is a high queue.  0: TxFN1st is a low queue.  This field is only used in eee.*/
#define TXQ1_H(x)               ((u32)(((x) & 0x00000001) << 0))
#define GET_TXQ1_H(x)           ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @defgroup ETH_WOL
  * @brief
  * @{
  */
#define BIT_WOL_IMR    ((u32)0x00000001 << 3)           /*!< R/W 0x0  Interrupt mask of wol_pme.*/
#define WOL_IMR(x)     ((u32)(((x) & 0x00000001) << 3))
#define GET_WOL_IMR(x) ((u32)(((x >> 3) & 0x00000001)))
#define BIT_WOL_PME    ((u32)0x00000001 << 2)           /*!< R 0x0  HW asserts pme when   a.HW entered wol idle state and   b.received magic ingress pkt.  HW asserts pme to wake up system.   Wol_pme indicates:  0: No magic pkt receives.  1: HW had received one magic pkt and system should wake up.*/
#define WOL_PME(x)     ((u32)(((x) & 0x00000001) << 2))
#define GET_WOL_PME(x) ((u32)(((x >> 2) & 0x00000001)))
#define BIT_WOL_STS    ((u32)0x00000001 << 1)           /*!< R 0x0  HW enters wol idle state when   a. SW issues wol_cmd state and   b. HW tx/rx function enter idle state.   Wol_sts indicates:  0: HW is not in wol idle state.  1: HW is in wol idle state.*/
#define WOL_STS(x)     ((u32)(((x) & 0x00000001) << 1))
#define GET_WOL_STS(x) ((u32)(((x >> 1) & 0x00000001)))
#define BIT_WOL_CMD    ((u32)0x00000001 << 0)           /*!< R/W 0x0  Issue wol command by SW*/
#define WOL_CMD(x)     ((u32)(((x) & 0x00000001) << 0))
#define GET_WOL_CMD(x) ((u32)(((x >> 0) & 0x00000001)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup ETHERNET_Exported_Types ETHERNET Exported Types
  * @{
  */

/** @brief ETHERNET Register Declaration
  */

typedef struct {
	__IO uint32_t ETH_IDR0                             ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t ETH_IDR4                             ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t ETH_MAR0                             ;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t ETH_MAR4                             ;  /*!< Register,  Address offset:0x00C */
	__IO uint16_t ETH_TXOKCNT                          ;  /*!< Register,  Address offset:0x010 */
	__IO uint16_t ETH_RXOKCNT                          ;  /*!< Register,  Address offset:0x012 */
	__IO uint16_t ETH_TXERR                            ;  /*!< Register,  Address offset:0x014 */
	__IO uint16_t ETH_RXERR                            ;  /*!< Register,  Address offset:0x016 */
	__IO uint16_t ETH_MISSPKT                          ;  /*!< Register,  Address offset:0x018 */
	__IO uint16_t ETH_FAE                              ;  /*!< Register,  Address offset:0x01A */
	__IO uint16_t ETH_TX1COL                           ;  /*!< Register,  Address offset:0x01C */
	__IO uint16_t ETH_TXMCOL                           ;  /*!< Register,  Address offset:0x01E */
	__IO uint16_t ETH_RXOKPHY                          ;  /*!< Register,  Address offset:0x020 */
	__IO uint16_t ETH_RXOKBRD                          ;  /*!< Register,  Address offset:0x022 */
	__IO uint16_t ETH_RXOKMU1                          ;  /*!< Register,  Address offset:0x024 */
	__IO uint16_t ETH_TXABT                            ;  /*!< Register,  Address offset:0x026 */
	__IO uint16_t ETH_TXUNDERC                         ;  /*!< Register,  Address offset:0x028 */
	__IO uint16_t ETH_MISSPKT_RDU                      ;  /*!< Register,  Address offset:0x02A */
	__IO uint32_t RSVD0[2]                             ;  /*!< Reserved,  Address offset:0x02C-0x033 */
	__I  uint32_t ETH_TRSR                             ;  /*!< Register,  Address offset:0x034 */
	__IO uint32_t ETH_CR                               ;  /*!< Register,  Address offset:0x038 */
	__IO uint32_t ETH_ISR_AND_IMR                      ;  /*!< Register,  Address offset:0x03C */
	__IO uint32_t ETH_TCR                              ;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t ETH_RCR                              ;  /*!< Register,  Address offset:0x044 */
	__IO uint32_t ETH_CPUTAG                           ;  /*!< Register,  Address offset:0x048 */
	__IO uint32_t ETH_CONFIG                           ;  /*!< Register,  Address offset:0x04C */
	__IO uint32_t ETH_CPUTAG1                          ;  /*!< Register,  Address offset:0x050 */
	__IO uint32_t RSVD1                                ;  /*!< Reserved,  Address offset:0x054-0x057 */
	__IO uint32_t ETH_MSR                              ;  /*!< Register,  Address offset:0x058 */
	__IO uint32_t ETH_MIIAR                            ;  /*!< Register,  Address offset:0x05C */
	__O  uint32_t ETH_SWINT                            ;  /*!< Register,  Address offset:0x060 */
	__IO uint32_t ETH_VLAN_REG                         ;  /*!< Register,  Address offset:0x064 */
	__IO uint32_t RSVD2[2]                             ;  /*!< Reserved,  Address offset:0x068-0x06F */
	__IO uint32_t ETH_LED_CTRL                         ;  /*!< Register,  Address offset:0x070 */
	__IO uint32_t RSVD3[3]                             ;  /*!< Reserved,  Address offset:0x074-0x07F */
	__IO uint32_t ETH_BIST                             ;  /*!< Register,  Address offset:0x080 */
	__IO uint32_t ETH_PGLBK                            ;  /*!< Register,  Address offset:0x084 */
	__IO uint32_t ETH_EEE_CR1                          ;  /*!< Register,  Address offset:0x088 */
	__IO uint32_t ETH_EEE_CR2                          ;  /*!< Register,  Address offset:0x08C */
	__IO uint32_t ETH_EEE_CR3                          ;  /*!< Register,  Address offset:0x090 */
	__IO uint32_t ETH_EEE_CR4                          ;  /*!< Register,  Address offset:0x094 */
	__I  uint32_t ETH_EEE_LPI_TMO_REG                  ;  /*!< Register,  Address offset:0x098 */
	__IO uint32_t ETH_EEE_LPI_TM1_REG                  ;  /*!< Register,  Address offset:0x09C */
	__IO uint32_t RSVD4[4]                             ;  /*!< Reserved,  Address offset:0x0A0-0x0AF */
	__IO uint32_t ETH_EEEP_CR1                         ;  /*!< Register,  Address offset:0x0B0 */
	__IO uint32_t ETH_EEEP_CR2                         ;  /*!< Register,  Address offset:0x0B4 */
	__IO uint32_t ETH_EEEP_CR3                         ;  /*!< Register,  Address offset:0x0B8 */
	__IO uint32_t ETH_EEEP_CR4                         ;  /*!< Register,  Address offset:0x0BC */
	__IO uint32_t ETH_EEEP_CR5                         ;  /*!< Register,  Address offset:0x0C0 */
	__IO uint32_t ETH_EEEP_CR6                         ;  /*!< Register,  Address offset:0x0C4 */
	__IO uint32_t ETH_EEEP_CR7                         ;  /*!< Register,  Address offset:0x0C8 */
	__IO uint32_t RSVD5                                ;  /*!< Reserved,  Address offset:0x0CC-0x0CF */
	__IO uint32_t ETH_IMR0_REG                         ;  /*!< Register,  Address offset:0x0D0 */
	__IO uint32_t ETH_IMR1_REG                         ;  /*!< Register,  Address offset:0x0D4 */
	__IO uint32_t ETH_ISR1_REG                         ;  /*!< Register,  Address offset:0x0D8 */
	__IO uint32_t ETH_INT_R_REG                        ;  /*!< Register,  Address offset:0x0DC */
	__IO uint32_t ETH_RST_PWR_DOWN_OPERATION_CTRL_REG  ;  /*!< Register,  Address offset:0x0E0 */
	__IO uint32_t ETH_CLK_CTRL_REG                     ;  /*!< Register,  Address offset:0x0E4 */
	__IO uint32_t RSVD6[2]                             ;  /*!< Reserved,  Address offset:0x0E8-0x0EF */
	__IO uint32_t ETH_INTERUPT_MASK_REG                ;  /*!< Register,  Address offset:0x0F0 */
	__IO uint32_t ETH_INTERUPT_STATUS_REG              ;  /*!< Register,  Address offset:0x0F4 */
	__IO uint32_t RSVD7[1154]                          ;  /*!< Reserved,  Address offset:0x0F8-0x12FF */
	__IO uint32_t ETH_TXFDP1                           ;  /*!< Register,  Address offset:0x1300 */
	__I  uint32_t ETH_TXCDO1                           ;  /*!< Register,  Address offset:0x1304 */
	__IO uint32_t RSVD8[2]                             ;  /*!< Reserved,  Address offset:0x1308-0x130F */
	__IO uint32_t ETH_TXFDP2                           ;  /*!< Register,  Address offset:0x1310 */
	__I  uint32_t ETH_TXCDO2                           ;  /*!< Register,  Address offset:0x1314 */
	__IO uint32_t RSVD9[2]                             ;  /*!< Reserved,  Address offset:0x1318-0x131F */
	__IO uint32_t ETH_TXFDP3                           ;  /*!< Register,  Address offset:0x1320 */
	__I  uint32_t ETH_TXCDO3                           ;  /*!< Register,  Address offset:0x1324 */
	__IO uint32_t RSVD10[2]                            ;  /*!< Reserved,  Address offset:0x1328-0x132F */
	__IO uint32_t ETH_TXFDP4                           ;  /*!< Register,  Address offset:0x1330 */
	__I  uint32_t ETH_TXCDO4                           ;  /*!< Register,  Address offset:0x1334 */
	__IO uint32_t RSVD11[2]                            ;  /*!< Reserved,  Address offset:0x1338-0x133F */
	__IO uint32_t ETH_TXFDP5                           ;  /*!< Register,  Address offset:0x1340 */
	__I  uint32_t ETH_TXCDO5                           ;  /*!< Register,  Address offset:0x1344 */
	__IO uint32_t RSVD12[10]                           ;  /*!< Reserved,  Address offset:0x1348-0x136F */
	__IO uint32_t ETH_RRING_MAP1                       ;  /*!< Register,  Address offset:0x1370 */
	__IO uint32_t ETH_RRING_MAP2                       ;  /*!< Register,  Address offset:0x1374 */
	__IO uint32_t ETH_RRING_MAP3                       ;  /*!< Register,  Address offset:0x1378 */
	__IO uint32_t ETH_RRING_MAP4                       ;  /*!< Register,  Address offset:0x137C */
	__IO uint32_t ETH_RRING_MAP5                       ;  /*!< Register,  Address offset:0x1380 */
	__IO uint32_t ETH_RRING_MAP6                       ;  /*!< Register,  Address offset:0x1384 */
	__IO uint32_t RSVD13[2]                            ;  /*!< Reserved,  Address offset:0x1388-0x138F */
	__IO uint32_t ETH_RXFDP2                           ;  /*!< Register,  Address offset:0x1390 */
	__I  uint16_t ETH_RXCDO2                           ;  /*!< Register,  Address offset:0x1394 */
	__IO uint16_t ETH_RXRINGSIZE2                      ;  /*!< Register,  Address offset:0x1396 */
	__IO uint32_t ETH_RX_CPU_DESN2                     ;  /*!< Register,  Address offset:0x1398 */
	__IO uint32_t ETH_RX_DES_THRES2                    ;  /*!< Register,  Address offset:0x139C */
	__IO uint32_t ETH_RXFDP3                           ;  /*!< Register,  Address offset:0x13A0 */
	__I  uint16_t ETH_RXCDO3                           ;  /*!< Register,  Address offset:0x13A4 */
	__IO uint16_t ETH_RXRINGSIZE3                      ;  /*!< Register,  Address offset:0x13A6 */
	__IO uint32_t ETH_RX_CPU_DESN3                     ;  /*!< Register,  Address offset:0x13A8 */
	__IO uint32_t ETH_RX_DES_THRES3                    ;  /*!< Register,  Address offset:0x13AC */
	__IO uint32_t ETH_RXFDP4                           ;  /*!< Register,  Address offset:0x13B0 */
	__I  uint16_t ETH_RXCDO4                           ;  /*!< Register,  Address offset:0x13B4 */
	__IO uint16_t ETH_RXRINGSIZE4                      ;  /*!< Register,  Address offset:0x13B6 */
	__IO uint32_t ETH_RX_CPU_DESN4                     ;  /*!< Register,  Address offset:0x13B8 */
	__IO uint32_t ETH_RX_DES_THRES4                    ;  /*!< Register,  Address offset:0x13BC */
	__IO uint32_t ETH_RXFDP5                           ;  /*!< Register,  Address offset:0x13C0 */
	__I  uint16_t ETH_RXCDO5                           ;  /*!< Register,  Address offset:0x13C4 */
	__IO uint16_t ETH_RXRINGSIZE5                      ;  /*!< Register,  Address offset:0x13C6 */
	__IO uint32_t ETH_RX_CPU_DESN5                     ;  /*!< Register,  Address offset:0x13C8 */
	__IO uint32_t ETH_RX_DES_THRES5                    ;  /*!< Register,  Address offset:0x13CC */
	__IO uint32_t ETH_RXFDP6                           ;  /*!< Register,  Address offset:0x13D0 */
	__I  uint16_t ETH_RXCDO6                           ;  /*!< Register,  Address offset:0x13D4 */
	__IO uint16_t ETH_RXRINGSIZE6                      ;  /*!< Register,  Address offset:0x13D6 */
	__IO uint32_t ETH_RX_CPU_DESN6                     ;  /*!< Register,  Address offset:0x13D8 */
	__IO uint32_t ETH_RX_DES_THRES6                    ;  /*!< Register,  Address offset:0x13DC */
	__IO uint32_t RSVD14[4]                            ;  /*!< Reserved,  Address offset:0x13E0-0x13EF */
	__IO uint32_t ETH_RX_FDP1                          ;  /*!< Register,  Address offset:0x13F0 */
	__I  uint16_t ETH_RX_CDO1                          ;  /*!< Register,  Address offset:0x13F4 */
	__IO uint16_t ETH_RX_RINGSIZE1                     ;  /*!< Register,  Address offset:0x13F6 */
	__IO uint32_t RSVD15                               ;  /*!< Reserved,  Address offset:0x13F8-0x13FB */
	__IO uint32_t ETH_SMSA                             ;  /*!< Register,  Address offset:0x13FC */
	__IO uint32_t ETH_PROBE_SELEXCT                    ;  /*!< Register,  Address offset:0x1400 */
	__I  uint32_t ETH_DIAG1                            ;  /*!< Register,  Address offset:0x1404 */
	__IO uint32_t RSVD16[9]                            ;  /*!< Reserved,  Address offset:0x1408-0x142B */
	__IO uint32_t ETH_CONFIG_CLK_AND_RX_PSE_DES_THRES_H;  /*!< Register,  Address offset:0x142C */
	__IO uint32_t ETH_ETHRNTRXCPU_DES_NUM1             ;  /*!< Register,  Address offset:0x1430 */
	__IO uint32_t ETH_ETHER_IO_CMD                     ;  /*!< Register,  Address offset:0x1434 */
	__IO uint32_t ETH_IO_CMD1                          ;  /*!< Register,  Address offset:0x1438 */
	__IO uint32_t ETH_WOL                              ;  /*!< Register,  Address offset:0x143C */
} ETHERNET_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

// Please add your definitions here
/// Defines the packet buffer size
#define ETH_PKT_BUFF_SZ                 1600 //1514?
/// Defines the MAC address length
#define ETH_MAC_ADDR_LEN                6
/// Defines the delay period when checking the own bit of the Tx/Rx descriptors
#define ETH_OWN_BIT_UPDATE_PERIOD       10
/// Defines the max. timeout value when checking the flag of MDIO operations
#define ETH_TIMEOUT_CNT_MAX             1000000
/// Defines the header of vlantag (ctag)
#define ETH_C_VLAN_HDR                  0x8100279F
/// Defines the header of vlantag (stag)
#define ETH_S_VLAN_HDR                  0x88A8279F
/// Defines the size (unit: Bytes) of each Tx descriptor
#define ETH_TX_DESC_SIZE	20  // 20 Bytes
/// Defines the size (unit: Bytes) of each Rx descriptor
#define ETH_RX_DESC_SIZE	16  // 16 Bytes
/**
  \brief  Defines PHY  register 0
*/

#define PHY_RESET 			BIT(15)
#define EN_LOOPBACK 		BIT(14)
#define SPEED_SELECT 		BIT(13)
#define EN_AUTO_GEN 		BIT(12)
#define POWER_DOWN 			BIT(11)
#define ISOLATION			BIT(10)
#define RESTART_AUTO_GEN 	BIT(9)
#define DUPLEX_ON 			BIT(8)
#define COLLISION_TEST 		BIT(7)



/**
  \brief  Defines PHY  register (0x5C)
*/
#define FEMAC_MIIA_SHIFT_FLAG           31
#define FEMAC_MIIA_BIT_FLAG             ((u32)0x00000001 << 31)
#define FEMAC_MIIA_SHIFT_PHY_ADDR       26
#define FEMAC_MIIA_MASK_PHY_ADDR        ((u32)0x0000001F << 26)
#define FEMAC_MIIA_SHIFT_MDIO_BUSY      25
#define FEMAC_MIIA_BIT_MDIO_BUSY        ((u32)0x00000001 << 25)
#define FEMAC_MIIA_SHIFT_REG_ADDR       16
#define FEMAC_MIIA_MASK_REG_ADDR        ((u32)0x0000001F << 16)
#define FEMAC_MIIA_SHIFT_DATA           0
#define FEMAC_MIIA_MASK_DATA            ((u32)0x0000FFFF << 0)

/**
  \brief  Defines FEMAC IDR0 register (0x0)
*/
#define FEMAC_IDR0_SHIFT_IDR0           24
#define FEMAC_IDR0_BIT_IDR0             ((u32)0x000000FF << 24)
#define FEMAC_IDR0_SHIFT_IDR1           16
#define FEMAC_IDR0_BIT_IDR1             ((u32)0x000000FF << 16)
#define FEMAC_IDR0_SHIFT_IDR2           8
#define FEMAC_IDR0_BIT_IDR2             ((u32)0x000000FF << 8)
#define FEMAC_IDR0_SHIFT_IDR3           0
#define FEMAC_IDR0_BIT_IDR3             ((u32)0x000000FF << 0)

/**
  \brief  Defines FEMAC IDR4 register (0x4)
*/
#define FEMAC_IDR4_SHIFT_IDR4           24
#define FEMAC_IDR4_BIT_IDR4             ((u32)0x000000FF << 24)
#define FEMAC_IDR4_SHIFT_IDR5           16
#define FEMAC_IDR4_BIT_IDR5             ((u32)0x000000FF << 16)

/**
  \brief  Defines FEMAC Interrupt Status/Mask register (0x3C)
*/
#define FEMAC_IMR_SHIFT_LINKCHG         24
#define FEMAC_IMR_BIT_LINKCHG           ((u32)0x00000001 << 24)
#define FEMAC_IMR_SHIFT_TOK             22
#define FEMAC_IMR_BIT_TOK               ((u32)0x00000001 << 22)
#define FEMAC_IMR_SHIFT_RER_OVF         20
#define FEMAC_IMR_BIT_RER_OVF           ((u32)0x00000001 << 20)
#define FEMAC_IMR_SHIFT_ROK             16
#define FEMAC_IMR_BIT_ROK               ((u32)0x00000001 << 16)
#define FEMAC_ISR_SHIFT_RER_OVF         4
#define FEMAC_ISR_BIT_RER_OVF           ((u32)0x00000001 << 4)

/**
  \brief  Defines FEMAC Receive Configuration register (0x44)
*/
#define FEMAC_RC_SHIFT_AER              5
#define FEMAC_RC_MASK_AER               ((u32)0x00000001 << 5)
#define FEMAC_RC_SHIFT_AR               4
#define FEMAC_RC_MASK_AR                ((u32)0x00000001 << 4)
#define FEMAC_RC_SHIFT_AB               3
#define FEMAC_RC_MASK_AB                ((u32)0x00000001 << 3)
#define FEMAC_RC_SHIFT_AM               2
#define FEMAC_RC_MASK_AM                ((u32)0x00000001 << 2)
#define FEMAC_RC_SHIFT_APM              1
#define FEMAC_RC_MASK_APM               ((u32)0x00000001 << 1)
#define FEMAC_RC_SHIFT_AAP              0
#define FEMAC_RC_MASK_AAP               ((u32)0x00000001 << 0)

/**
  \brief  Defines FEMAC Media Status register (0x58)
*/
#define FEMAC_MS_SHIFT_SPEED            27
#define FEMAC_MS_BIT_SPEED              ((u32)0x00000003 << 27)
#define FEMAC_MS_SHIFT_LINKB            26
#define FEMAC_MS_BIT_LINKB              ((u32)0x00000001 << 26)
#define FEMAC_MS_SHIFT_FULLDUP          22
#define FEMAC_MS_BIT_FULLDUP            ((u32)0x00000003 << 22)
#define FEMAC_MS_SHIFT_NWCOMPLETE       21
#define FEMAC_MS_BIT_NWCOMPLETE         ((u32)0x00000003 << 21)

/**
  \brief  Defines FEMAC MII access register (0x5C)
*/
#define FEMAC_MIIA_SHIFT_FLAG           31
#define FEMAC_MIIA_BIT_FLAG             ((u32)0x00000001 << 31)
#define FEMAC_MIIA_SHIFT_PHY_ADDR       26
#define FEMAC_MIIA_MASK_PHY_ADDR        ((u32)0x0000001F << 26)
#define FEMAC_MIIA_SHIFT_MDIO_BUSY      25
#define FEMAC_MIIA_BIT_MDIO_BUSY        ((u32)0x00000001 << 25)
#define FEMAC_MIIA_SHIFT_REG_ADDR       16
#define FEMAC_MIIA_MASK_REG_ADDR        ((u32)0x0000001F << 16)
#define FEMAC_MIIA_SHIFT_DATA           0
#define FEMAC_MIIA_MASK_DATA            ((u32)0x0000FFFF << 0)

/**
  \brief  Defines FEMAC Tx command descriptor
*/
#define FEMAC_TX_DSC_SHIFT_OWN          31
#define FEMAC_TX_DSC_BIT_OWN            ((u32)0x00000001 << 31)
#define FEMAC_TX_DSC_SHIFT_EOR          30
#define FEMAC_TX_DSC_BIT_EOR            ((u32)0x00000001 << 30)
#define FEMAC_TX_DSC_SHIFT_FS           29
#define FEMAC_TX_DSC_BIT_FS             ((u32)0x00000001 << 29)
#define FEMAC_TX_DSC_SHIFT_LS           28
#define FEMAC_TX_DSC_BIT_LS             ((u32)0x00000001 << 28)
#define FEMAC_TX_DSC_SHIFT_CRC          23
#define FEMAC_TX_DSC_BIT_CRC            ((u32)0x00000001 << 23)

/**
  \brief  Defines FEMAC Rx command descriptor
*/
#define FEMAC_RX_DSC_SHIFT_OWN          31
#define FEMAC_RX_DSC_BIT_OWN            ((u32)0x00000001 << 31)
#define FEMAC_RX_DSC_SHIFT_EOR          30
#define FEMAC_RX_DSC_BIT_EOR            ((u32)0x00000001 << 30)

/**
  \brief  Defines FEPHY register page number
*/

#define FEPHY_REG_PAGE_0            0x0
#define FEPHY_REG_PAGE_1            0x1
#define FEPHY_REG_PAGE_2            0x2
#define FEPHY_REG_PAGE_3            0x3
#define FEPHY_REG_PAGE_4            0x4
#define FEPHY_REG_PAGE_5            0x5
#define FEPHY_REG_PAGE_6            0x6
#define FEPHY_REG_PAGE_7            0x7
#define FEPHY_REG_PAGE_8            0x8
#define FEPHY_REG_PAGE_9            0x9
#define FEPHY_REG_PAGE_A            0xA
#define FEPHY_REG_PAGE_B            0xB

/**
  \brief  Defines FEPHY Page 0xA46 Reg 20
*/
#define FEPHY_REG_SHIFT_FUSE_RDY        0
#define FEPHY_REG_MASK_FUSE_RDY         ((u32)0x00000001 << 0)

#define FEPHY_IDAC_DEFAULT              0x6

/** @defgroup Ethernet_Wait_Type
  * @{
  */
#define WAIT_SMI_WRITE_DONE		0
#define WAIT_SMI_READ_DONE		1
#define WAIT_RMII_LINKUP		2
/**
  * @}
  */

/**
  \brief  Defines the FEPHY register address
*/
enum fephy_reg_addr {
	FEPHY_REG_ADDR_0        = 0,
	FEPHY_REG_ADDR_1        = 1,
	FEPHY_REG_ADDR_2        = 2,
	FEPHY_REG_ADDR_3        = 3,
	FEPHY_REG_ADDR_4        = 4,
	FEPHY_REG_ADDR_5        = 5,
	FEPHY_REG_ADDR_6        = 6,
	FEPHY_REG_ADDR_7        = 7,
	FEPHY_REG_ADDR_8        = 8,
	FEPHY_REG_ADDR_9        = 9,
	FEPHY_REG_ADDR_10       = 10,
	FEPHY_REG_ADDR_11       = 11,
	FEPHY_REG_ADDR_12       = 12,
	FEPHY_REG_ADDR_13       = 13,
	FEPHY_REG_ADDR_14       = 14,
	FEPHY_REG_ADDR_15       = 15,
	FEPHY_REG_ADDR_16       = 16,
	FEPHY_REG_ADDR_17       = 17,
	FEPHY_REG_ADDR_18       = 18,
	FEPHY_REG_ADDR_19       = 19,
	FEPHY_REG_ADDR_20       = 20,
	FEPHY_REG_ADDR_21       = 21,
	FEPHY_REG_ADDR_22       = 22,
	FEPHY_REG_ADDR_23       = 23,
	FEPHY_REG_ADDR_24       = 24,
	FEPHY_REG_ADDR_25       = 25,
	FEPHY_REG_ADDR_26       = 26,
	FEPHY_REG_ADDR_27       = 27,
	FEPHY_REG_ADDR_28       = 28,
	FEPHY_REG_ADDR_29       = 29,
	FEPHY_REG_ADDR_30       = 30,
	FEPHY_REG_ADDR_31       = 31,
};

/**
  \brief  Defines Ethernet interrupt event.
*/
enum  eth_int_event_e {
	EthTxDone       = 0,
	EthRxDone       = 1,
	EthLinkUp       = 2,
	EthLinkDown     = 3,
	EthRx_RDU       = 4,
};

/**
  \brief  Defines the InterFrameGap time.
*/
enum  eth_ifg_time_e {
	eth_ifg_time_0     = 0,
	eth_ifg_time_1     = 1,
	eth_ifg_time_2     = 2,
	eth_ifg_time_3     = 3,  // 9.6 us for 10 Mbps, 960 ns for 100 Mbps
	eth_ifg_time_4     = 4,
	eth_ifg_time_5     = 5,
	eth_ifg_time_6     = 6,
	eth_ifg_time_7     = 7,
};

/**
  \brief  Defines the type of loopback function.
*/
enum  eth_lbk_fun_e {
	eth_normal_op       = 0,
	eth_r2t_lbk         = 1,
	eth_t2r_lbk         = 3,
};

/**
  \brief  Defines the link speed.
*/
enum  eth_link_speed_e {
	eth_speed_100       = 0,
	eth_speed_10        = 1,
};

/**
  \brief  Defines the link status.
*/
enum  eth_link_status_e {
	eth_link_up         = 0,
	eth_link_down       = 1,
};

/**
  \brief  Defines the duplex mode.
*/
enum  eth_duplex_mode_e {
	eth_half_duplex     = 0,
	eth_full_duplex     = 1,
};

/**
  \brief  Defines the n-way status.
*/
enum  eth_nway_status_e {
	eth_nway_incompleted    = 0,
	eth_nway_completed      = 1,
};

/**
  \brief  Defines the operation for PHY's register.
*/
enum  eth_phy_reg_op_e {
	eth_phy_reg_read    = 0,
	eth_phy_reg_write   = 1,
};

/**
  \brief  Defines Tx VLAN action.
*/
enum  eth_vlan_action_e {
	eth_vlan_hdr_intact     = 0,
	eth_vlan_hdr_insert     = 1,
	eth_vlan_hdr_remove     = 2,
	eth_vlan_hdr_remark_vid = 3,
};

/**
  \brief  Defines the type of ethernet packet.
*/
enum  eth_packet_type_e {
	eth_pkt_ethernet        = 0,
	eth_pkt_ipv4            = 1,
	eth_pkt_ipv4_pptp       = 2,
	eth_pkt_ipv4_icmp       = 3,
	eth_pkt_ipv4_igmp       = 4,
	eth_pkt_ipv4_tcp        = 5,
	eth_pkt_ipv4_udp        = 6,
	eth_pkt_ipv6            = 7,
	eth_pkt_icmpv6          = 8,
	eth_pkt_ipv6_tcp        = 9,
	eth_pkt_ipv6_udp        = 10,
};

enum  eth_sys_hw_ctrl_e {
	eth_sys_hw_fephy_ip,
	eth_sys_hw_femac_ip,
	eth_sys_hw_uabg_en,
	eth_sys_hw_uahv_en,
	eth_sys_hw_fephy_en,
	eth_sys_hw_lx_en,
};


/**
  \brief  Ethernet callback function for interrupt event.
*/
typedef void (*eth_callback_t)(u32 event, u32 data);
/**
  \brief  Callback function to make OS do a context-switch while waiting.
*/
typedef void (*eth_task_yield)(void);


/**
  \brief  The structure of Tx descriptor.
*/
typedef struct {
	uint32_t dw1;    // offset 0
	uint32_t addr;   // offset 4
	uint32_t dw2;    // offset 8
	uint32_t dw3;    // offset 12
	uint32_t dw4;    // offset 16
} ETH_TxDescTypeDef;

/**
  \brief  The structure of Rx descriptor.
*/
typedef struct {
	uint32_t dw1;    // offset 0
	uint32_t addr;   // offset 4
	uint32_t dw2;    // offset 8
	uint32_t dw3;   // offset 12
} ETH_RxDescTypeDef;


enum eth_refclk_phase {
	ETH_SAMPLED_ON_RISING_EDGE = 0x00,
	ETH_SAMPLED_ON_FALLING_EDGE = 0x01,/*Falling edge is not a common usage*/
};

enum eth_refclk_on {
	ETH_REFCLK_OFF = 0x00,
	ETH_REFCLK_ON = 0x01,
};

enum eth_phy_lpb {
	ETH_PHY_NORMAL_MODE = 0x0,
	ETH_PHY_LOOPBACK_MODE = 0x1,
};

enum eth_mac_lpb {
	ETH_NORMAL_MODE = 0,
	ETH_LOOPBACK_R2T_MODE = 1,
	ETH_LOOPBACK_T2R_MODE = 3,
};


enum eth_ifg_time {
	ETH_IFG_0 = 0,
	ETH_IFG_1 = 1,
	ETH_IFG_2 = 2,
	ETH_IFG_3 = 3,// 9.6 us for 10 Mbps, 960 ns for 100 Mbps
	ETH_IFG_4 = 4,
	ETH_IFG_5 = 5,
	ETH_IFG_6 = 6,
	ETH_IFG_7 = 7,
};

enum eth_rx_jumbo_cfg {
	ETH_RX_JUMBO_DISABLE = 0x0,
	ETH_RX_JUMBO_ENABLE = 0x1,
};

enum eth_tx_threshold {
	ETH_TX_THRESHOLD_128B = 0,
	ETH_TX_THRESHOLD_256B = 1,
	ETH_TX_THRESHOLD_512B = 2,
	ETH_TX_THRESHOLD_1024B = 3,
};

enum eth_rx_threshold {
	ETH_RX_THRESHOLD_1024B = 0,
	ETH_RX_THRESHOLD_128B = 1,
	ETH_RX_THRESHOLD_256B = 2,
	ETH_RX_THRESHOLD_512B = 3,
};

enum eth_trigger_level {
	ETH_TRIGGER_LEVEL_1_PKT = 0,
	ETH_TRIGGER_LEVEL_4_PKTS = 1,
	ETH_TRIGGER_LEVEL_8_PKTS = 2,
	ETH_TRIGGER_LEVEL_12_PKTS = 3,
	ETH_TRIGGER_LEVEL_16_PKTS = 4,
	ETH_TRIGGER_LEVEL_20_PKTS = 5,
	ETH_TRIGGER_LEVEL_24_PKTS = 6,
	ETH_TRIGGER_LEVEL_28_PKTS = 7,
};

//RTL8201FR TX Setup time
enum eth_phy_tx_setup {
	ETH_PHY_TX_SETUP_TIME_6NS = 0x6,
	ETH_PHY_TX_SETUP_TIME_8NS = 0x5,
	ETH_PHY_TX_SETUP_TIME_10NS = 0x4, //default 10ns_0x1111
	ETH_PHY_TX_SETUP_TIME_12NS = 0x3,
	ETH_PHY_TX_SETUP_TIME_14NS = 0x2,
	ETH_PHY_TX_SETUP_TIME_16NS = 0x1,
	ETH_PHY_TX_SETUP_TIME_18NS = 0x0,
};

//RTL8201FR RX Setup time
enum eth_phy_rx_setup {
	ETH_PHY_RX_SETUP_TIME_8NS = 0x8,
	ETH_PHY_RX_SETUP_TIME_10NS = 0x9, //default 10ns_0x1111
	ETH_PHY_RX_SETUP_TIME_12NS = 0x6,
	ETH_PHY_RX_SETUP_TIME_14NS = 0x7,
	ETH_PHY_RX_SETUP_TIME_16NS = 0x4,
	ETH_PHY_RX_SETUP_TIME_18NS = 0x5,
};



/**
 * \brief       ETH init structure definition.
 *
 * \ingroup     ETH_Exported_Types
 */
typedef struct {
	enum eth_phy_type 		ETH_Phy_Type;
	enum eee_mode 			ETH_Phy_EEE_mode;
	enum eth_refclk_phase 	ETH_RefClkPhase;
	enum eth_refclk_on 		ETH_RefClkDirec;
	enum eth_mac_lpb 		ETH_Mac_LoopBackMode;
	enum eth_phy_lpb 		ETH_Phy_LooPBackMode;
	enum eth_ifg_time 		ETH_IFGTime;
	enum eth_rx_jumbo_cfg 	ETH_RxJumbo;
	enum eth_tx_threshold 	ETH_TxThreshold;
	enum eth_rx_threshold 	ETH_RxThreshold;
	enum eth_trigger_level 	ETH_TxTriggerLevel;
	enum eth_trigger_level 	ETH_RxTriggerLevel;
	enum eth_phy_tx_setup	ETH_PhyTxSetupTime;
	enum eth_phy_rx_setup 	ETH_PhyRxSetupTime;
	u8 		ETH_MacAddr[6];
	u32 	ETH_RCR; //offset 0x44 RCR ReceiveConfigReg
	u8 		ETH_TxDescNum;
	u8 		ETH_RxDescNum;
	u32 	ETH_IntMaskAndStatus;
	ETH_TxDescTypeDef *ETH_TxDesc;
	ETH_RxDescTypeDef *ETH_RxDesc;
	u8 *ETH_TxPktBuf;
	u8 *ETH_RxPktBuf;
	u8 		ETH_TxDescCurrentNum;
	u8 		ETH_RxDescCurrentNum;
	u8 		ETH_RxFrameStartDescIdx;
	u32 	ETH_RxFrameLen;
	u32		ETH_TxFrameLen;
	u32 	ETH_RxSegmentCount;
	u16 	ETH_TxAllocBufSize;
	u16 	ETH_RxAllocBufSize;
	u16 	ETH_TxBufSize;
	u16 	ETH_RxBufSize;
	u32 	ETH_PHY_EEE_EN;
	eth_callback_t callback;
	eth_task_yield task_yield;
} ETH_InitTypeDef, *PETH_InitTypeDef;

/**
 * @addtogroup hs_hal_ethernet_rom_func ETHERNET HAL ROM APIs.
 * @ingroup hs_hal_ethernet
 * @{
 */
u32 Ethernet_GetINT(void);
u32 Ethernet_ClearINT(u32 INTrBit);
u32 RMII_IRQHandler(ETH_InitTypeDef *ETH_InitStruct);
void Ethernet_AutoPolling(u32 opt);
void Ethernet_ResetPHY(void);
void Ethernet_SetDescAddr(ETH_InitTypeDef *ETH_InitStruct);
void Ethernet_SetMacAddr(u8 *ETH_MacAddr);
void Ethernet_StructInit(ETH_InitTypeDef *ETH_InitStruct);
u32 Ethernet_init(ETH_InitTypeDef *ETH_InitStruct);
u16 Ethernet_Read_PhyReg(u16 page, u16 reg_addr);
u32 Ethernet_Write_PhyReg(u16 page, u16 reg_addr, u16 data);
u32 Ethernet_GetLinkStatus(void);
void Ethernet_SetRefclkDirec(u32 refclk_mode);
void Ethernet_OutputClk2Phy(u32 pin);
void Ethernet_UseExtClk(u32 pin);
u8 *Ethernet_GetRXPktInfo(ETH_InitTypeDef *ETH_InitStruct, u32 *rx_len);
void Ethernet_UpdateRXDESC(ETH_InitTypeDef *ETH_InitStruct);
u8 *Ethernet_GetTXPktInfo(ETH_InitTypeDef *ETH_InitStruct);
void Ethernet_UpdateTXDESCAndSend(ETH_InitTypeDef *ETH_InitStruct, u32 size);

extern void ethernet_mii_init(void);
extern int link_is_up;

//#define  ENABLE_EEE_FUNCTION

#define  PHY_TYPE_SELECT RTL_8721F

/* MANUAL_GEN_END */

/** @} */

/** @} */


#endif