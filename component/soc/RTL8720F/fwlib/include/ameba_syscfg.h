/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SYSCFG_H_
#define _AMEBA_SYSCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Platform
  * @{
  */

/** @defgroup SYSCFG
  * @brief SYSCFG driver modules
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup SYSCFG_Exported_Constants SYSCFG Exported Constants
  * @{
  */

/** @defgroup SYSCFG_Bounding_Option_definitions
  * @{
  */
#define SYSCFG_BD_QFN32						((u32)0x000000000)
#define SYSCFG_BD_QFN48_MCM_8MBFlash			((u32)0x000000001)
#define SYSCFG_BD_QFN48						((u32)0x000000002)
#define SYSCFG_BD_QFN48_NEW					((u32)0x000000000)
#define SYSCFG_BD_QFN68						((u32)0x000000007)
#define SYSCFG_BD_QFN68_NEW					((u32)0x000000005)
#define SYSCFG_BD_TFBGA_MCM_8761A			((u32)0x000000004)
/**
  * @}
  */

/** @defgroup SYSCFG_CUT_Version_definitions
  * @{
  */
#define SYSCFG_CUT_VERSION_A				0
#define SYSCFG_CUT_VERSION_B				1
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SYSCFG_Exported_Functions SYSCFG Exported Functions
  * @{
  */
_LONG_CALL_ u32 SYSCFG_TRP_UARTImage(void);
_LONG_CALL_ u32 SYSCFG_TRP_ICFG(void);
_LONG_CALL_ u32 SYSCFG_TRP_TestMode(void);
_LONG_CALL_ u32 SYSCFG_TRP_OTPBYP(void);
_LONG_CALL_ u32 SYSCFG_TRP_Boot_Int(void);
_LONG_CALL_ u32 SYSCFG_CHIPType_Get(void);
_LONG_CALL_ u32 SYSCFG_RLVersion(void);
_LONG_CALL_ u32 SYSCFG_GetRLNum(void);
_LONG_CALL_ u32 SYSCFG_OTP_BOOTSEL(void);
_LONG_CALL_ u32 SYSCFG_OTP_BTFUNC(void);
_LONG_CALL_ u32 SYSCFG_OTP_SDIO_PINMUX(void);
_LONG_CALL_ u32 SYSCFG_OTP_BootMCMFlash(void);
_LONG_CALL_ u32 SYSCFG_OTP_FlashDSleepEn(void);
_LONG_CALL_ u32 SYSCFG_OTP_SPICAddr4ByteEn(void);
_LONG_CALL_ u32 SYSCFG_OTP_SPICBootSpeedUpDis(void);
_LONG_CALL_ u32 SYSCFG_OTP_BootFlashBaud(void);
_LONG_CALL_ u32 SYSCFG_OTP_BOOT_CNT_LMT_EN(void);
_LONG_CALL_ u32 SYSCFG_OTP_DisBootLog(void);
_LONG_CALL_ u32 SYSCFG_OTP_LowBaudLog(void);
_LONG_CALL_ u32 SYSCFG_OTP_SBootEn(void);
_LONG_CALL_ u32 SYSCFG_OTP_SBootEn_PQC(void);
_LONG_CALL_ u32 SYSCFG_OTP_RDPEn(void);
_LONG_CALL_ u32 SYSCFG_OTP_RSIPEn(void);
_LONG_CALL_ u32 SYSCFG_OTP_RandDelayEn(void);
_LONG_CALL_ u32 SYSCFG_OTP_AntiRollback(void);
_LONG_CALL_ u32 SYSCFG_OTP_FaultLogDis(void);
_LONG_CALL_ u32 SYSCFG_OTP_RSIPMode(void);
_LONG_CALL_ u32 SYSCFG_OTP_HUKDerivEn(void);
_LONG_CALL_ u32 SYSCFG_OTP_PKID2(void);
_LONG_CALL_ u32 SYSCFG_HWTrigDownload(void);
_LONG_CALL_ u32 SYSCFG_SWTrigDownload(void);
_LONG_CALL_ u32 SYSCFG_OTP_AuthAlg(void);
_LONG_CALL_ u32 SYSCFG_OTP_HashAlg(void);
_LONG_CALL_ u32 SYSCFG_OTP_OTAAddr(void);
_LONG_CALL_ u32 SYSCFG_OTP_BootVer(void);
_LONG_CALL_ u32 SYSCFG_OTP_RMAMode(void);
_LONG_CALL_ u32 SYSCFG_OTP_RomPatchEn(void);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/

/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other definations --------------------------------------------------------*/
/*******************  Macro definition for TRP_ICFG ********************/
#define SYSCFG_OTP_BOOT_FROM_INT					0
#define SYSCFG_OTP_BOOT_FROM_EXT					1

/*******************  Macro definition for Lifecycle ********************/
#define RTK_PSA_LIFECYCLE_RMA				        	1
#define RTK_PSA_LIFECYCLE_DECOMMISSIONED			2

/*******************  Macro definition for RSIP MODE ********************/
#define RSIP_XTS_MODE                     0
#define RSIP_CTR_MODE                     1
#define RSIP_GCM_MODE                     2
#define RSIP_INVALID_MODE                 0xff

#ifdef __cplusplus
}
#endif

#endif //_AMEBA_SYSCFG_H_
