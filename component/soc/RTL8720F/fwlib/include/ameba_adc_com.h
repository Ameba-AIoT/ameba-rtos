/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _RL6955_ADC_COM_H_
#define _RL6955_ADC_COM_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup ADC_COM
  * @brief ADC_COM driver modules
  * @{
  */


/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup ADC_COM_Register_Definitions ADC_COM Register Definitions
  * @{
  */

/** @defgroup ADC_COM_ADC_CTRL0
  * @brief
  * @{
  */
#define ADC_COM_BIT_EN_THERMAL         ((u32)0x00000001 << 14)           /*!< R/W 1'b0  Thermal sensor enable  0: disable thermal sensor(default) 1:enable thermal sensor*/
#define ADC_COM_MASK_REG_ASY_DELAY     ((u32)0x00000003 << 12)           /*!< R/W 2'b01  Asy SAR ADC compare time. Control the delay of the comparator. If the comparator fails to produce a result for a long time, it will force the output of the result. 00:Fast; 01: 10: 11:Slow:*/
#define ADC_COM_REG_ASY_DELAY(x)       (((u32)((x) & 0x00000003) << 12))
#define ADC_COM_GET_REG_ASY_DELAY(x)   ((u32)(((x >> 12) & 0x00000003)))
#define ADC_COM_MASK_SEL_LDO2P5_VOL    ((u32)0x00000003 << 9)            /*!< R/W 'b01  00:2.125V 01:2.25V 10:2.375V 11:2.50V AIF default 01:2.25V*/
#define ADC_COM_SEL_LDO2P5_VOL(x)      (((u32)((x) & 0x00000003) << 9))
#define ADC_COM_GET_SEL_LDO2P5_VOL(x)  ((u32)(((x >> 9) & 0x00000003)))
#define ADC_COM_MASK_SEL_LDOAV09_VOL   ((u32)0x00000003 << 7)            /*!< R/W 'b10  AUX ADC 0.9V LDO voltage: 00:0.85V 01:0.9V 10:0.95V (default) 11:1.0V AIF default 10:0.95V;*/
#define ADC_COM_SEL_LDOAV09_VOL(x)     (((u32)((x) & 0x00000003) << 7))
#define ADC_COM_GET_SEL_LDOAV09_VOL(x) ((u32)(((x >> 7) & 0x00000003)))
#define ADC_COM_BIT_EN_ADC_CKS         ((u32)0x00000001 << 4)            /*!< R/W 'b0  EN_ADC_CKS = 0,ADC_CKS=0 or 1,ADC always Stop to Sample State; EN_ADC_CKS = 1,ADC_CKS=1,ADC Sample State; EN_ADC_CKS = 1,ADC_CKS=0,ADC Sample Finish,ADC compare start; AIF default 1;*/
#define ADC_COM_BIT_EN_VREFP_LPF       ((u32)0x00000001 << 3)            /*!< R/W 'b0  When 0.AUX ADC Reference LPF disable; When 1,AUX ADC Reference LPF enable; AIF default 1;*/
#define ADC_COM_BIT_EN_META            ((u32)0x00000001 << 2)            /*!< R/W 'b1  EN_META = 1, When ADC compare not finish,force comparator to 0; EN_META = 0, Do nothing; AIF default 1;*/
#define ADC_COM_MASK_META_DELAY        ((u32)0x00000003 << 0)            /*!< R/W 'b01  EN_META=1,When ADC compare not finish,force comparator to 0; META_DELAY select the delay to force comparator to 0; AIF default 01;*/
#define ADC_COM_META_DELAY(x)          (((u32)((x) & 0x00000003) << 0))
#define ADC_COM_GET_META_DELAY(x)      ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup ADC_COM_ADC_CTRL1
  * @brief
  * @{
  */
#define ADC_COM_MASK_SEL_VREFP_VOL   ((u32)0x0000000F << 0)           /*!< R/W 4'b0100  AUX ADC Vrefp voltage: 0000:1.45V 0001:1.50V 0010:1.55V 0011:1.60V 0100:1.65V 0101:1.70V 0110:1.75V 0111:1.80V 1000:1.85V 1001:1.875V 1010:1.90V 1011:1.95V 1100:2.00V 1101:2.05V 1110:2.10V 1111:2.15V  Cs * VIN = 8C * Vrefp 3C * 5V = 8C * 1.875V 4C * 3.3V = 8C * 1.65V 8C * 1.8V = 8C * 1.8V AIF default 0100: 1.65V;*/
#define ADC_COM_SEL_VREFP_VOL(x)     (((u32)((x) & 0x0000000F) << 0))
#define ADC_COM_GET_SEL_VREFP_VOL(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ADC_COM_POW_CTRL
  * @brief
  * @{
  */
#define ADC_COM_BIT_POW_ADC          ((u32)0x00000001 << 9) /*!< R/W 'b0  POW_ADC =0.AUX ADC off; POW_ADC =1,AUX ADC on; AIF default 1;*/
#define ADC_COM_BIT_POW_AUXADC_MBIAS ((u32)0x00000001 << 8) /*!< R/W 'b0  POW_ADC_MBIAS=0,AUX ADC and Touch bias current all off; POW_ADC_MBIAS=1,AUX ADC and Touch bias current on; AIF default 1;*/
#define ADC_COM_BIT_POW_LDO2P5       ((u32)0x00000001 << 6) /*!< R/W 'b0  1:AUX_ADC 2.5V LDO on; 0:AUX_ADC 2.5V LDO off; AIF default1;*/
#define ADC_COM_BIT_POW_LDOAV09      ((u32)0x00000001 << 5) /*!< R/W 'b0  1:AUX_ADC 0.9V LDO on; 0:AUX_ADC 0.9V LDO off; AIF default 1;*/
#define ADC_COM_BIT_POW_REFPGEN      ((u32)0x00000001 << 4) /*!< R/W 'b0  1:AUX_ADC Reference on; 0:AUX_ADC Reference off; AIF default1;*/
#define ADC_COM_BIT_VCCKOFF33V_B     ((u32)0x00000001 << 1) /*!< R/W 1'b0  When 0,Internal DVDD power off, All ADC and Touch off;*/
#define ADC_COM_BIT_VCCKOFFB_DV      ((u32)0x00000001 << 0) /*!< R/W 1'b1  When 0,Internal DVDD power off, All ADC and Touch off; 1: enable auxadc power cut 0: shutdown auxadc all power cut for power saving when scan mode , it will tie0,  also ISO signal must tie 1*/
/** @} */

/** @defgroup ADC_COM_DRV_CFG
  * @brief
  * @{
  */
#define ADC_COM_BIT_FORCE_ADC_POW_EN ((u32)0x00000001 << 1) /*!< R/W 0x0  Enable ADC decoupling mode, the ADC power signal will be controlled by software*/
/** @} */

/** @defgroup ADC_COM_POW_CNT0
  * @brief
  * @{
  */
#define ADC_COM_MASK_PWR_EN_LPF_CNT   ((u32)0x00000FFF << 0)           /*!< R/W 12'h18f  The delay time between  pow_mbias and en_vrefp_lpf*/
#define ADC_COM_PWR_EN_LPF_CNT(x)     (((u32)((x) & 0x00000FFF) << 0))
#define ADC_COM_GET_PWR_EN_LPF_CNT(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ADC_COM_POW_CNT1
  * @brief
  * @{
  */
#define ADC_COM_MASK_PWR_ADC_CNT        ((u32)0x00000FFF << 12)           /*!< R/W 12'h31f  The delay time between  pow_adc and ck_ad*/
#define ADC_COM_PWR_ADC_CNT(x)          (((u32)((x) & 0x00000FFF) << 12))
#define ADC_COM_GET_PWR_ADC_CNT(x)      ((u32)(((x >> 12) & 0x00000FFF)))
#define ADC_COM_MASK_PWR_TO_ENADC_CKS   ((u32)0x00000FFF << 0)            /*!< R/W 12'h18f  The delay time between  pow_adc and en_adc_cks*/
#define ADC_COM_PWR_TO_ENADC_CKS(x)     (((u32)((x) & 0x00000FFF) << 0))
#define ADC_COM_GET_PWR_TO_ENADC_CKS(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ADC_COM_ADC_CLK_HCNTx_DIV
  * @brief ADC Channel Clock Divider Register
  * @{
  */
#define ADC_COM_MASK_ADC_CLK_HCNTx   ((u32)0x00000FFF << 0)           /*!< R/W 12'd19  This field defines clock driver high level of  chn in ADC module.  high level =( bit_adc_clk_hcntx+1) * ip_clk period .  hcnt11-13 is rsvd.*/
#define ADC_COM_ADC_CLK_HCNTx(x)     (((u32)((x) & 0x00000FFF) << 0))
#define ADC_COM_GET_ADC_CLK_HCNTx(x) ((u32)(((x >> 0) & 0x00000FFF)))
/** @} */

/** @defgroup ADC_COM_ADC_CLK_COV
  * @brief
  * @{
  */
#define ADC_COM_BIT_ADC_COV_MODE   ((u32)0x00000001 << 31)           /*!< R/W 1'b0  0: convert time is 500ns 1: convert time is 700ns*/
#define ADC_COM_MASK_ADC_ISO_CNT   ((u32)0x0000000F << 26)           /*!< R/W 4'd0  Control the time of iso ch. if adc_iso_cnt is 0, iso_ch will disable*/
#define ADC_COM_ADC_ISO_CNT(x)     (((u32)((x) & 0x0000000F) << 26))
#define ADC_COM_GET_ADC_ISO_CNT(x) ((u32)(((x >> 26) & 0x0000000F)))
/** @} */

/** @defgroup ADC_COM_ADC_VOL_LIST_0
  * @brief
  * @{
  */
#define ADC_COM_MASK_ADC_SEL_VREFP_VOL_0   ((u32)0x0000000F << 0)           /*!< R/W 4'b0100  AUX ADC Vrefp voltage(all chn): 0000:1.45V 0001:1.50V 0010:1.55V 0011:1.60V 0100:1.65V 0101:1.70V 0110:1.75V 0111:1.80V 1000:1.85V 1001:1.875V 1010:1.90V 1011:1.95V 1100:2.00V 1101:2.05V 1110:2.10V 1111:2.15V  Cs * VIN = 8C * Vrefp 3C * 5V = 8C * 1.875V 4C * 3.3V = 8C * 1.65V 8C * 1.8V = 8C * 1.8V AIF default 0100: 1.65V;*/
#define ADC_COM_ADC_SEL_VREFP_VOL_0(x)     (((u32)((x) & 0x0000000F) << 0))
#define ADC_COM_GET_ADC_SEL_VREFP_VOL_0(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @defgroup ADC_COM_ADC_PAD_TEST
  * @brief
  * @{
  */
#define ADC_COM_BIT_LP_ADC_FORCE     ((u32)0x00000001 << 4)           /*!< R/W 0h  force sw select instead of pinmux input when adc test mode 1:using [3:0] for adc channel 0:using io input for adc channel*/
#define ADC_COM_MASK_LP_ADC_CH_NUM   ((u32)0x0000000F << 0)           /*!< R/W 0h  sw select adc test channel*/
#define ADC_COM_LP_ADC_CH_NUM(x)     (((u32)((x) & 0x0000000F) << 0))
#define ADC_COM_GET_LP_ADC_CH_NUM(x) ((u32)(((x >> 0) & 0x0000000F)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup ADC_COM_Exported_Types ADC_COM Exported Types
  * @{
  */

/** @brief ADC_COM Register Declaration
  */

typedef struct {
	__IO uint32_t ADC_COM_ADC_CTRL0            ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t ADC_COM_ADC_CTRL1            ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t RSVD0                        ;  /*!< Reserved,  Address offset:0x008-0x00B */
	__IO uint32_t ADC_COM_POW_CTRL             ;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t RSVD1[3]                     ;  /*!< Reserved,  Address offset:0x010-0x01B */
	__IO uint32_t ADC_COM_DRV_CFG              ;  /*!< Register,  Address offset:0x01C */
	__IO uint32_t RSVD2                        ;  /*!< Reserved,  Address offset:0x020-0x023 */
	__IO uint32_t ADC_COM_POW_CNT0             ;  /*!< Register,  Address offset:0x024 */
	__IO uint32_t ADC_COM_POW_CNT1             ;  /*!< Register,  Address offset:0x028 */
	__IO uint32_t RSVD3                        ;  /*!< Reserved,  Address offset:0x02C-0x02F */
	__IO uint32_t ADC_COM_ADC_CLK_HCNTx_DIV[16];  /*!< ADC CHANNEL CLOCK DIVIDER REGISTER,  Address offset:0x030-0x06C */
	__IO uint32_t ADC_COM_ADC_CLK_COV          ;  /*!< Register,  Address offset:0x070 */
	__IO uint32_t ADC_COM_ADC_VOL_LIST_0       ;  /*!< Register,  Address offset:0x074 */
	__IO uint32_t RSVD4[2]                     ;  /*!< Reserved,  Address offset:0x078-0x07F */
	__IO uint32_t ADC_COM_ADC_PAD_TEST         ;  /*!< Register,  Address offset:0x080 */
	__IO uint32_t ADC_COM_DUMMY                ;  /*!< DUMMY REGISTER,  Address offset:0x084 */
} ADC_COM_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


// Please add your definitions here

/* Exported Types ------------------------------------------------------------*/

/* Exported Constants --------------------------------------------------------*/
#define ADC_COM_BASE    CMP_REG_BASE
#define ADC_COM					((ADC_COM_TypeDef *)ADC_COM_BASE)

/** @defgroup ADC_Convert_Mode
  * @{
  */
#define ADC_CONV_500NS				((u8)0x0)
#define ADC_CONV_700NS				((u8)0x1)

#define IS_ADC_CONV_MODE(x)			((x) == ADC_CONV_500NS || \
																(x) == ADC_CONV_700NS)
/**
  * @}
  */

/* Exported Functions --------------------------------------------------------*/


/** @} */

/** @} */

// If other definitions do not need to be extracted to CHM document, add them here.
/* Other Definitions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif