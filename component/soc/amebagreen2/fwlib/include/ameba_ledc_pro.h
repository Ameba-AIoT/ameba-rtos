/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBAGREEN2_LEDC_PRO_H_
#define _AMEBAGREEN2_LEDC_PRO_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup LEDC_PRO
  * @brief LEDC_PRO driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup LEDC_PRO_Register_Definitions LEDC_PRO Register Definitions
  * @{
  */

/** @defgroup LEDC_PRO_LED_PALY_CTL0
  * @brief
  * @{
  */
#define LEDC_PRO_BIT_R_LED_ROW_DISP_INV       ((u32)0x00000001 << 23)          /*!< R/WE 0x0  1: inverse row output logic of the current display mode; 0: default row output logic of the current display mode*/
#define LEDC_PRO_BIT_R_LED_COL_DISP_INV       ((u32)0x00000001 << 22)          /*!< R/WE 0x0  1: inverse column output logic of the current display mode; 0: default column output logic of the current display mode*/
#define LEDC_PRO_BIT_R_LED_DISPLAY_MODE       ((u32)0x00000001 << 21)          /*!< R/WE 0x0  1: show rows in sequence 0: show columns in sequence*/
#define LEDC_PRO_BIT_R_SEQ_PASS_FLAG          ((u32)0x00000001 << 20)          /*!< RC 0x0  1: indicate 0x0c[31:0] sequence check pass, clear 0 when read*/
#define LEDC_PRO_BIT_R_LED_ROW_IDLE_STATUS    ((u32)0x00000001 << 19)          /*!< R/WE 0x1  1: output 0: floating*/
#define LEDC_PRO_BIT_R_LED_COLUMN_IDLE_STATUS ((u32)0x00000001 << 18)          /*!< R/WE 0x1  1: output 0: floating*/
#define LEDC_PRO_BIT_R_LED_BUF_CLEAR          ((u32)0x00000001 << 17)          /*!< WA0 0x0  write 1 generate one pulse. Clear and reset ram data*/
#define LEDC_PRO_MASK_R_OVERLAP_TIME          ((u32)0x000000FF << 9)           /*!< R/WE 0x3  time is r_overlap_time +1, delay beweent stage, unit 4M clock*/
#define LEDC_PRO_R_OVERLAP_TIME(x)            (((u32)((x) & 0x000000FF) << 9))
#define LEDC_PRO_GET_R_OVERLAP_TIME(x)        ((u32)(((x >> 9) & 0x000000FF)))
#define LEDC_PRO_MASK_R_LED_ROW               ((u32)0x00000007 << 6)           /*!< R/WE 0x0  led row number - 1*/
#define LEDC_PRO_R_LED_ROW(x)                 (((u32)((x) & 0x00000007) << 6))
#define LEDC_PRO_GET_R_LED_ROW(x)             ((u32)(((x >> 6) & 0x00000007)))
#define LEDC_PRO_MASK_R_LED_COLUMN            ((u32)0x00000007 << 3)           /*!< R/WE 0x0  led column number - 1*/
#define LEDC_PRO_R_LED_COLUMN(x)              (((u32)((x) & 0x00000007) << 3))
#define LEDC_PRO_GET_R_LED_COLUMN(x)          ((u32)(((x >> 3) & 0x00000007)))
#define LEDC_PRO_BIT_LEDBUF_SWITCH_ON         ((u32)0x00000001 << 2)           /*!< R 0x0  1: sw trigger hw switch ram; 0: hw switch finish*/
#define LEDC_PRO_BIT_LEDBUF_FLAG              ((u32)0x00000001 << 1)           /*!< R 0x1  1: display data of ram1; 0: display data of ram0. HW check in when display enable and sw trigger hw switch ram*/
#define LEDC_PRO_BIT_R_LED_DISPLAY_EN         ((u32)0x00000001 << 0)           /*!< R/W 0x0  1: enable led driver to display;0:disable led driver to display*/
/** @} */

/** @defgroup LEDC_PRO_LED_PALY_CTL1
  * @brief
  * @{
  */
#define LEDC_PRO_MASK_R_LED_BLANK_TIME   ((u32)0x000000FF << 16)           /*!< R/WE 0x3B  time is r_led_blank_time +1 ,  unit 4M clock*/
#define LEDC_PRO_R_LED_BLANK_TIME(x)     (((u32)((x) & 0x000000FF) << 16))
#define LEDC_PRO_GET_R_LED_BLANK_TIME(x) ((u32)(((x >> 16) & 0x000000FF)))
#define LEDC_PRO_MASK_R_LED_PWM_TIME     ((u32)0x00001FFF << 0)            /*!< R/WE 0x5  one stage time of a frame, unit 255*250ns*/
#define LEDC_PRO_R_LED_PWM_TIME(x)       (((u32)((x) & 0x00001FFF) << 0))
#define LEDC_PRO_GET_R_LED_PWM_TIME(x)   ((u32)(((x >> 0) & 0x00001FFF)))
/** @} */

/** @defgroup LEDC_PRO_PSDO_I2C_CTL
  * @brief
  * @{
  */
#define LEDC_PRO_MASK_R_I2C_DATA_NUM   ((u32)0x0000007F << 8)           /*!< R/WE 0x0  psdo i2c data num, unit: byte*/
#define LEDC_PRO_R_I2C_DATA_NUM(x)     (((u32)((x) & 0x0000007F) << 8))
#define LEDC_PRO_GET_R_I2C_DATA_NUM(x) ((u32)(((x >> 8) & 0x0000007F)))
#define LEDC_PRO_BIT_R_PSEDO_I2C_EN    ((u32)0x00000001 << 7)           /*!< R/W 0x0  indicate psedo i2c in working, only when this bit=1 can access psedo i2c ram*/
#define LEDC_PRO_BIT_R_I2C_TX_STATUS   ((u32)0x00000001 << 6)           /*!< R 0x1  1: psdo i2c is idle, 0: psdo i2c is working*/
#define LEDC_PRO_MASK_R_I2C_CLK_DIV    ((u32)0x0000003F << 0)           /*!< R/WE 0x0  clock division factor = r_i2c_clk_div * 2, minimum value is 2*/
#define LEDC_PRO_R_I2C_CLK_DIV(x)      (((u32)((x) & 0x0000003F) << 0))
#define LEDC_PRO_GET_R_I2C_CLK_DIV(x)  ((u32)(((x >> 0) & 0x0000003F)))
/** @} */

/** @defgroup LEDC_PRO_SEQUENCE_CHECK0
  * @brief
  * @{
  */
#define LEDC_PRO_MASK_R_SEQ_CHK0   ((u32)0xFFFFFFFF << 0)           /*!< R/WPD 0x0  sequence check, when this field first write 0x69696969, then write 0x96969696, tirgger led/i2c work*/
#define LEDC_PRO_R_SEQ_CHK0(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LEDC_PRO_GET_R_SEQ_CHK0(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup LEDC_PRO_SEQUENCE_CHECK1
  * @brief
  * @{
  */
#define LEDC_PRO_MASK_R_SEQ_CHK1   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  register protect sequence check, when this field = 0xa5a5a5a5, enable access protect register*/
#define LEDC_PRO_R_SEQ_CHK1(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LEDC_PRO_GET_R_SEQ_CHK1(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup LEDC_PRO_LED_RAM
  * @brief
  * @{
  */
#define LEDC_PRO_MASK_R_LED_RAM_DATA   ((u32)0xFFFFFFFF << 0)           /*!< R/W 0x0  4100_9280~4100_92FF 128bytes 4100_9280~4100_92BF,64bytes used for led display 4100_9280~4100_92C0,65bytes used for psdo i2c*/
#define LEDC_PRO_R_LED_RAM_DATA(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define LEDC_PRO_GET_R_LED_RAM_DATA(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup LEDC_PRO_Exported_Types LEDC_PRO Exported Types
  * @{
  */

/** @brief LEDC_PRO Register Declaration
  */

typedef struct {
	__IO uint32_t LEDC_PRO_LED_PALY_CTL0  ;  /*!< Register,  Address offset:0x000 */
	__IO uint32_t LEDC_PRO_LED_PALY_CTL1  ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t LEDC_PRO_PSDO_I2C_CTL   ;  /*!< Register,  Address offset:0x008 */
	__IO uint32_t LEDC_PRO_SEQUENCE_CHECK0;  /*!< Register,  Address offset:0x00C */
	__IO uint32_t LEDC_PRO_SEQUENCE_CHECK1;  /*!< Register,  Address offset:0x010 */
	__IO uint32_t RSVD0[27]               ;  /*!< Reserved,  Address offset:0x014-0x07F */
	__IO uint32_t LEDC_PRO_LED_RAM        ;  /*!< Register,  Address offset:0x080 */
} LEDC_PRO_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

/* Exported Constants --------------------------------------------------------*/
/** @defgroup LEDC_PRO_Exported_Constants LEDC_PRO Exported Constants
* @{
*/

#define TRIGGER_SEQUENCE1  0x69696969
#define TRIGGER_SEQUENCE2  0x96969696

#define REG_PROTECT_DISABLE  0xa5a5a5a5
#define REG_PROTECT_ENABLE  0x0

#define IS_VALID_NUM(num) (num <= 127)
#define IS_VALID_DIV(div) (div >= 2)
#define IS_VALID_ROWCOL(row, col) (col <= 7 && row <=7)

#define IDLE_STATUS_OUTPUT		0x1
#define IDLE_STATUS_FLOATING	0x0
#define RAM_CMD_OFFSET			0x80
#define RAM_DATA_OFFSET			0x81

#define RAM_TOTAL_LEN			0x80
#define LED_MATRIX_8X8			0x7
/** @} */

/** @} */

/** @} */

_LONG_CALL_ void pseudo_i2c_init(u32 div);
_LONG_CALL_ void pseudo_i2c_write_ram(u8 cmd, u8 *data, int len);
_LONG_CALL_ void pseudo_i2c_reset(void);
_LONG_CALL_ void pseudo_i2c_protect(u32 sequence);
_LONG_CALL_ void pseudo_i2c_set_clkdiv(u32 div);
_LONG_CALL_ void pseudo_i2c_set_len(u32 len);

/* MANUAL_GEN_END */

#endif