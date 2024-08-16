/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CMSIS_RIDR_H__
#define __CMSIS_RIDR_H__

/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_RIDR     RTK Implementation Defined Registers (RIDR)
  \brief    Type definitions for the RTK Implementation Defined Registers (RIDR)
  @{
 */

/**
  \brief  Structure type to access the RTK Implementation Defined Registers (RIDR).
 */
typedef struct {
	__IM  uint32_t VNUM;					/*!< Offset: 0x000 (R/)  Revision number register */
	__IM  uint32_t CPUNUM;					/*!< Offset: 0x004 (R/)  CPU number register */
	__IOM uint32_t ACTLR1;					/*!< Offset: 0x008 (R/W) Reserved for future extension */
	__IOM uint32_t ACTLR2;					/*!< Offset: 0x00C (R/W) Reserved for future extension */
	__IOM uint32_t CNTCTRL0;				/*!< Offset: 0x010 (R/W) Legacy cp3 control regsiter 0 */
	__IOM uint32_t CNTCTRL1;				/*!< Offset: 0x014 (R/W) Legacy cp3 control regsiter 1 */
	__IOM uint32_t CNTCTRL2;				/*!< Offset: 0x018 (R/W) Legacy cp3 control regsiter 2 */
	__IOM uint32_t CNTDATA;					/*!< Offset: 0x01C (R/W) cp3 read/write data regsiter */
	uint32_t RESERVED0[8U];					/*!< Offset: 0x020 ~ 0x03C */
	__IOM uint32_t LMUARBTSLT;				/*!< Offset: 0x040 (R/W)  LMU arbiter time slot */
#if defined (ARM_CORE_CM4)
	uint32_t RESERVED1[3U];					/*!< Offset: 0x044 ~ 0x04C */
	__IOM uint32_t ITCM0_BASE;				/*!< Offset: 0x050 (R/W)  ITCM0 Range Base register */
	__IOM uint32_t ITCM0_TOP;				/*!< Offset: 0x054 (R/W)  ITCM0 Range Top register */
	__IOM uint32_t ITCM1_BASE;				/*!< Offset: 0x058 (R/W)  ITCM1 Range Base register */
	__IOM uint32_t ITCM1_TOP;				/*!< Offset: 0x05C (R/W)  ITCM1 Range Top register */
	__IOM uint32_t DTCM0_BASE;				/*!< Offset: 0x060 (R/W)  DTCM0 Range Base register */
	__IOM uint32_t DTCM0_TOP;				/*!< Offset: 0x064 (R/W)  DTCM0 Range Top register */
	__IOM uint32_t DTCM1_BASE;				/*!< Offset: 0x068 (R/W)  DTCM1 Range Base register */
	__IOM uint32_t DTCM1_TOP;				/*!< Offset: 0x06C (R/W)  DTCM1 Range Top register */
#else /*ARM_CORE_CM0*/
	uint32_t RESERVED1[11U];				/*!< Offset: 0x044 ~ 0x06C */
#endif
	__IM  uint32_t XO_RANGE_EN; 			/*!< Offset: 0x070 (R/W)  execution only range enable register */
	__IM  uint32_t XO_BASE;					/*!< Offset: 0x074 (R/W)  execution only base register */
	__IM  uint32_t XO_TOP;					/*!< Offset: 0x078 (R/W)  execution only top register */
	uint32_t RESERVED2;						/*!< Offset: 0x07C */
	__IM  uint32_t EXEC_PRI;				/*!< Offset: 0x080 (R/W)  execution Priority register */
	uint32_t RESERVED3[3U];					/*!< Offset: 0x084 ~ 0x8C */
	__IOM uint32_t DBGACTLR;				/*!< Offset: 0x090 (R/W)  Debug Auxiliary Control register */
	uint32_t RESERVED4[3U];				  	/*!< Offset: 0x094 ~ 0x9C */
	__IOM uint32_t ICWRR_BASE;				/*!< Offset: 0x0A0 (R/W)  Instruction Cache Way Restricted Range Base register */
	__IOM uint32_t ICWRR_TOP;				/*!< Offset: 0x0A4 (R/W)  Instruction Cache Way Restricted Range Top register */
	__IOM uint32_t DCWRR_BASE;				/*!< Offset: 0x0A8 (R/W)  Data Cache Way Restricted Range Base register */
	__IOM uint32_t DCWRR_TOP;				/*!< Offset: 0x0AC (R/W)  Data Cache Way Restricted Range Top register */
	uint32_t RESERVED5[3U];			  		/*!< Offset: 0x0B0 ~ 0xB8 */
	__IOM uint32_t PBUS_WRESP;				/*!< Offset: 0x0BC (R/W)  Peripheral Bus Wait Response register */
	__IOM uint32_t PBUS_RANGE;				/*!< Offset: 0x0C0 (R/W)  Peripheral Bus Range register */
	uint32_t RESERVED6[7U];					/*!< Offset: 0x0C4 ~ 0x0DC */
	__IOM uint32_t ZWF_RANGE;				/*!< Offset: 0x0E0 (R/W)  ZWF Range register */
} RIDR_Type;

/* RIDR Legacy CP3 Control Register 0 Definitions */
#define RIDR_CNTCTRL0_CNT3_LO_Pos						24U                                     /*!< RIDR CNTCTRL0: Counter3 Low Position */
#define RIDR_CNTCTRL0_CNT3_LO_Msk						(0xFFUL << RIDR_CNTCTRL0_CNT3_LO_Pos)    /*!< RIDR CNTCTRL0: Counter3 Low Mask */

#define RIDR_CNTCTRL0_CNT2_LO_Pos						16U                                     /*!< RIDR CNTCTRL0: Counter2 Low Position */
#define RIDR_CNTCTRL0_CNT2_LO_Msk						(0xFFUL << RIDR_CNTCTRL0_CNT2_LO_Pos)    /*!< RIDR CNTCTRL0: Counter2 Low Mask */

#define RIDR_CNTCTRL0_CNT1_LO_Pos						8U                                      /*!< RIDR CNTCTRL0: Counter1 Low Position */
#define RIDR_CNTCTRL0_CNT1_LO_Msk						(0xFFUL << RIDR_CNTCTRL0_CNT1_LO_Pos)    /*!< RIDR CNTCTRL0: Counter1 Low Mask */

#define RIDR_CNTCTRL0_CNT0_LO_Pos						0U                                          /*!< RIDR CNTCTRL0: Counter0 Low Position */
#define RIDR_CNTCTRL0_CNT0_LO_Msk           			(0xFFUL /*<< RIDR_CNTCTRL0_CNT0_LO_Pos*/)    /*!< RIDR CNTCTRL0: Counter0 Low Mask */

/* RIDR Legacy CP3 Control Register 1 Definitions */
#define RIDR_CNTCTRL1_DUAL_MODE_CNT3_HI_Pos              24U                                               /*!< RIDR CNTCTRL1: Dual-mode Counter3 High Position */
#define RIDR_CNTCTRL1_DUAL_MODE_CNT3_HI_Msk             (0xFFUL << RIDR_CNTCTRL1_DUAL_MODE_CNT3_HI_Pos)    /*!< RIDR CNTCTRL1: Dual-mode Counter3 High Mask */

#define RIDR_CNTCTRL1_DUAL_MODE_CNT2_HI_Pos              16U                                               /*!< RIDR CNTCTRL1: Dual-mode Counter2 High Position */
#define RIDR_CNTCTRL1_DUAL_MODE_CNT2_HI_Msk             (0xFFUL << RIDR_CNTCTRL1_DUAL_MODE_CNT2_HI_Pos)    /*!< RIDR CNTCTRL1: Dual-mode Counter2 High Mask */

#define RIDR_CNTCTRL1_DUAL_MODE_CNT1_HI_Pos              8U                                                /*!< RIDR CNTCTRL1: Dual-mode Counter1 High Position */
#define RIDR_CNTCTRL1_DUAL_MODE_CNT1_HI_Msk             (0xFFUL << RIDR_CNTCTRL1_DUAL_MODE_CNT1_HI_Pos)    /*!< RIDR CNTCTRL1: Dual-mode Counter1 High Mask */

#define RIDR_CNTCTRL1_DUAL_MODE_CNT0_HI_Pos              0U                                                /*!< RIDR CNTCTRL1: Dual-mode Counter0 High Position */
#define RIDR_CNTCTRL1_DUAL_MODE_CNT0_HI_Msk             (0xFFUL /*<< RIDR_CNTCTRL1_DUAL_MODE_CNT0_HI_Pos*/)/*!< RIDR CNTCTRL1: Dual-mode Counter0 High Mask */

/* RIDR Legacy CP3 Control Register 2 Definitions */
#define RIDR_CNTCTRL2_CNT_EN_Pos						16U                                              	/*!< RIDR CNTCTRL2: Counter Enable Position */
#define RIDR_CNTCTRL2_CNT_EN_Msk						(0x1UL << RIDR_CNTCTRL2_CNT_EN_Pos)    				/*!< RIDR CNTCTRL2: Counter Enable Mask */

#define RIDR_CNTCTRL2_CNT_IDX_Pos						6U                                              	/*!< RIDR CNTCTRL2: Counter Index Position */
#define RIDR_CNTCTRL2_CNT_IDX_Msk						(0xFUL << RIDR_CNTCTRL2_CNT_IDX_Pos)    			/*!< RIDR CNTCTRL2: Counter Index Mask */

#define RIDR_CNTCTRL2_DUAL_MODE_CNT3_SET_Pos			3U                                              	/*!< RIDR CNTCTRL2: Dual-mode Counter3 Set Position */
#define RIDR_CNTCTRL2_DUAL_MODE_CNT3_SET_Msk			(0x1UL << RIDR_CNTCTRL2_DUAL_MODE_CNT3_SET_Pos)		/*!< RIDR CNTCTRL2: Dual-mode Counter3 Set Mask */

#define RIDR_CNTCTRL2_DUAL_MODE_CNT2_SET_Pos			2U                                              	/*!< RIDR CNTCTRL2: Dual-mode Counter2 Set Position */
#define RIDR_CNTCTRL2_DUAL_MODE_CNT2_SET_Msk			(0x1UL << RIDR_CNTCTRL2_DUAL_MODE_CNT2_SET_Pos)		/*!< RIDR CNTCTRL2: Dual-mode Counter2 Set Mask */

#define RIDR_CNTCTRL2_DUAL_MODE_CNT1_SET_Pos			1U                                              	/*!< RIDR CNTCTRL2: Dual-mode Counter1 Set Position */
#define RIDR_CNTCTRL2_DUAL_MODE_CNT1_SET_Msk			(0x1UL << RIDR_CNTCTRL2_DUAL_MODE_CNT1_SET_Pos)		/*!< RIDR CNTCTRL2: Dual-mode Counter1 Set Mask */

#define RIDR_CNTCTRL2_DUAL_MODE_CNT0_SET_Pos			0U                                                	/*!< RIDR CNTCTRL2: Dual-mode Counter0 Set Position */
#define RIDR_CNTCTRL2_DUAL_MODE_CNT0_SET_Msk			(0x1UL /*<< RIDR_CNTCTRL2_DUAL_MODE_CNT0_SET_Pos*/)	/*!< RIDR CNTCTRL2: Dual-mode Counter0 Set Mask */

/*@}*/ /* end of group CMSIS_RIDR */

#define RIDR_BASE           (TPI_BASE +  0x2000UL)                     /*!< RTK Implementation Defined Registers Base Address*/
#define RIDR                ((RIDR_Type      *)     RIDR_BASE        ) /*!< RTK Implementation Defined struct */

#endif /* __CMSIS_RIDR_H__ */
