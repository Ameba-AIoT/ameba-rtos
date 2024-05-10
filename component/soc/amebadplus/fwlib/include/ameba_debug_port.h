/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DBG_PORT_H_
#define _AMEBA_DBG_PORT_H_

/** @addtogroup Ameba_Platform
  * @{
  */

/** @defgroup DBG_PORT
  * @brief DBG_PORT driver modules
  * @{
  */

/** @addtogroup DBG_PORT
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-46 debug port groups.
  *		-Debug select by chiptop or local.
  *
  *****************************************************************************************
  * How to use DBG_PORT
  *****************************************************************************************
  *		1. Enable IP debug port using the follwoing function:
  *			DBG_Port_xxx(void)
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/* AUTO_GEN_START */
/** @defgroup DBG_PORT_Exported_Constants DBG_PORT Exported Constants
 * @{
 */
#define DBG_REGU_PMC                                 0x6E
#define DBG_RTC                                      0x6F
#define DBG_SDM32K                                   0x70
#define DBG_IWDG                                     0x71
#define DBG_LS_SYSON_ENTRY1                          0x00
#define DBG_LS_SYSON_ENTRY2                          0x01
#define DBG_LS_SYSON_ENTRY3                          0x02
#define DBG_LS_SYSON_ENTRY4                          0x03
#define DBG_LS_SYSON_ENTRY5                          0x04
#define DBG_LS_SYSON_ENTRY6                          0x05
#define DBG_LS_SYSON_ENTRY7                          0x06
#define DBG_LS_SYSON_ENTRY8                          0x07
#define DBG_LS_SYSON_ENTRY9                          0x08
#define DBG_LS_SYSON_ENTRY10                         0x09
#define DBG_LS_SYSON_ENTRY11                         0x0A
#define DBG_LS_SYSON_ENTRY12                         0x0B
#define DBG_LS_SYSON_ENTRY13                         0x0C
#define DBG_LS_SYSON_ENTRY14                         0x0D
#define DBG_LS_SYSON_ENTRY15                         0x0E
#define DBG_LS_SYSON_ENTRY16                         0x0F
#define DBG_LS_SYSON_ENTRY17                         0x10
#define DBG_LS_SYSON_ENTRY18                         0x11
#define DBG_LS_SYSON_ENTRY19                         0x12
#define DBG_LS_SYSON_ENTRY20                         0x13
#define DBG_LS_SYSON_ENTRY21                         0x14
#define DBG_LS_SYSON_ENTRY22                         0x15
#define DBG_LS_SYSON_ENTRY23                         0x16
#define DBG_LS_SYSON_ENTRY24                         0x17
#define DBG_LS_SYSON_ENTRY25                         0x18
#define DBG_LS_SYSON_ENTRY26                         0x19
#define DBG_LS_SYSON_ENTRY27                         0x1A
#define DBG_LS_SYSON_ENTRY28                         0x1B
#define DBG_LS_SYSON_ENTRY29                         0x1C
#define DBG_LS_SYSON_ENTRY30                         0x1D
#define DBG_LS_SYSON_ENTRY31                         0x1E
#define DBG_LS_SYSON_ENTRY32                         0x1F
#define DBG_ADC                                      0x72
#define DBG_ADC_COMP                                 0x73
#define DBG_CAPTOUCH                                 0x74
#define DBG_AIP_ARB                                  0x75
#define DBG_WLAFE_CTRL_ENTRY1                        0x68
#define DBG_WLAFE_CTRL_ENTRY2                        0x69
#define DBG_XTAL_CTRL_ENTRY1                         0x60
#define DBG_XTAL_CTRL_ENTRY2                         0x61
#define DBG_XTAL_CTRL_ENTRY3                         0x62
#define DBG_XTAL_CTRL_ENTRY4                         0x63
#define DBG_KEYSCAN_ENTRY1                           0x50
#define DBG_KEYSCAN_ENTRY2                           0x51
#define DBG_KEYSCAN_ENTRY3                           0x52
#define DBG_KEYSCAN_ENTRY4                           0x53
#define DBG_KEYSCAN_ENTRY5                           0x54
#define DBG_KEYSCAN_ENTRY6                           0x55
#define DBG_KEYSCAN_ENTRY7                           0x56
#define DBG_KEYSCAN_ENTRY8                           0x57
#define DBG_WATCHDOG0                                0x76
#define DBG_WATCHDOG1                                0x77
#define DBG_WATCHDOG2                                0x78
#define DBG_OTP_ENTRY1                               0x64
#define DBG_OTP_ENTRY2                               0x65
#define DBG_OTP_ENTRY3                               0x66
#define DBG_OTP_ENTRY4                               0x67
#define DBG_SIC_ENTRY1                               0x58
#define DBG_SIC_ENTRY2                               0x59
#define DBG_SIC_ENTRY3                               0x5A
#define DBG_SIC_ENTRY4                               0x5B
#define DBG_SIC_ENTRY5                               0x5C
#define DBG_SIC_ENTRY6                               0x5D
#define DBG_SIC_ENTRY7                               0x5E
#define DBG_SIC_ENTRY8                               0x5F
#define DBG_LOGUART                                  0x79
#define DBG_UART0                                    0x7A
#define DBG_UART1                                    0x7B
#define DBG_UART2                                    0x7C
#define DBG_RCC                                      0x7D
#define DBG_AUDIO_CODEC                              0x7E
#define DBG_SPORT0                                   0x7F
#define DBG_SPORT1                                   0x80
#define DBG_BT_ENTRY1                                0x6A
#define DBG_BT_ENTRY2                                0x6B
#define DBG_SPI_FLASH                                0x81
#define DBG_I2C0                                     0x82
#define DBG_I2C1                                     0x83
#define DBG_IRDA                                     0x84
#define DBG_LEDC                                     0x85
#define DBG_LX_ARB                                   0x86
#define DBG_IPSEC_AES                                0x87
#define DBG_IPSEC_HASH                               0x88
#define DBG_PSRAM_SPIC_ENTRY1                        0x6C
#define DBG_PSRAM_SPIC_ENTRY2                        0x6D
#define DBG_PSRAM_PHY                                0x89
#define DBG_SDDON_ENTRY1                             0x40
#define DBG_SDDON_ENTRY2                             0x41
#define DBG_SDDON_ENTRY3                             0x42
#define DBG_SDDON_ENTRY4                             0x43
#define DBG_SDDON_ENTRY5                             0x44
#define DBG_SDDON_ENTRY6                             0x45
#define DBG_SDDON_ENTRY7                             0x46
#define DBG_SDDON_ENTRY8                             0x47
#define DBG_SDDON_ENTRY9                             0x48
#define DBG_SDDON_ENTRY10                            0x49
#define DBG_SDDON_ENTRY11                            0x4A
#define DBG_SDDON_ENTRY12                            0x4B
#define DBG_SDDON_ENTRY13                            0x4C
#define DBG_SDDON_ENTRY14                            0x4D
#define DBG_SDDON_ENTRY15                            0x4E
#define DBG_SDDON_ENTRY16                            0x4F
#define DBG_SDDOFF_ENTRY1                            0x20
#define DBG_SDDOFF_ENTRY2                            0x21
#define DBG_SDDOFF_ENTRY3                            0x22
#define DBG_SDDOFF_ENTRY4                            0x23
#define DBG_SDDOFF_ENTRY5                            0x24
#define DBG_SDDOFF_ENTRY6                            0x25
#define DBG_SDDOFF_ENTRY7                            0x26
#define DBG_SDDOFF_ENTRY8                            0x27
#define DBG_SDDOFF_ENTRY9                            0x28
#define DBG_SDDOFF_ENTRY10                           0x29
#define DBG_SDDOFF_ENTRY11                           0x2A
#define DBG_SDDOFF_ENTRY12                           0x2B
#define DBG_SDDOFF_ENTRY13                           0x2C
#define DBG_SDDOFF_ENTRY14                           0x2D
#define DBG_SDDOFF_ENTRY15                           0x2E
#define DBG_SDDOFF_ENTRY16                           0x2F
#define DBG_SDDOFF_ENTRY17                           0x30
#define DBG_SDDOFF_ENTRY18                           0x31
#define DBG_SDDOFF_ENTRY19                           0x32
#define DBG_SDDOFF_ENTRY20                           0x33
#define DBG_SDDOFF_ENTRY21                           0x34
#define DBG_SDDOFF_ENTRY22                           0x35
#define DBG_SDDOFF_ENTRY23                           0x36
#define DBG_SDDOFF_ENTRY24                           0x37
#define DBG_SDDOFF_ENTRY25                           0x38
#define DBG_SDDOFF_ENTRY26                           0x39
#define DBG_SDDOFF_ENTRY27                           0x3A
#define DBG_SDDOFF_ENTRY28                           0x3B
#define DBG_SDDOFF_ENTRY29                           0x3C
#define DBG_SDDOFF_ENTRY30                           0x3D
#define DBG_SDDOFF_ENTRY31                           0x3E
#define DBG_SDDOFF_ENTRY32                           0x3F
#define DBG_SPI0                                     0x8A
#define DBG_SPI1                                     0x8B
#define DBG_TRNG                                     0x8C
#define DBG_USB                                      0x8D
#define DBG_WLMAC                                    0x8E
#define DBG_KM4                                      0x8F
#define DBG_KM0                                      0x90
/**
  * @}
  */
/* AUTO_GEN_END */

/* MANUAL_GEN_START */

//Please add your defination here

/** @defgroup ADC_DEBUG_GROUP_SELECT_Definitions
  * @{
  */
#define ADC_LOCAL_DBG_S0						((u8)0x00)
#define ADC_LOCAL_DBG_S1						((u8)0x01)
#define ADC_LOCAL_DBG_S2						((u8)0x02)
#define ADC_LOCAL_DBG_S3						((u8)0x03)
#define IS_ADC_LOCAL_DBG_GROUP(sel)			(((sel) == ADC_LOCAL_DBG_S0) || \
												((sel) == ADC_LOCAL_DBG_S1) || \
												((sel) == ADC_LOCAL_DBG_S2) || \
												((sel) == ADC_LOCAL_DBG_S3))
/**
  * @}
  */

/** @defgroup CTC_DEBUG_GROUP_SELECT_Definitions
  * @{
  */
#define CTC_LOCAL_DBG_CLKRST						((u8)0x00)
#define CTC_LOCAL_DBG_APBSLV						((u8)0x01)
#define CTC_LOCAL_DBG_CTRL						((u8)0x02)
#define CTC_LOCAL_DBG_DECISION						((u8)0x03)
#define CTC_LOCAL_DBG_ADC_ARB						((u8)0x04)
#define IS_CTC_LOCAL_DBG_GROUP(sel)			(((sel) == CTC_LOCAL_DBG_CLKRST) || \
												((sel) == CTC_LOCAL_DBG_APBSLV) || \
												((sel) == CTC_LOCAL_DBG_CTRL) || \
												((sel) == CTC_LOCAL_DBG_DECISION) || \
												((sel) == CTC_LOCAL_DBG_ADC_ARB))
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif
