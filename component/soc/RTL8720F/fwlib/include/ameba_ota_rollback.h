/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_OTA_ROLLBACK_H_
#define _AMEBA_OTA_ROLLBACK_H_

#include "basic_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * OTA rollback (BOOT + APP) uses the ROM BootCnt, BKUP_REG0[27:24]. Supported only when OTP BOOT_CNT_LMT_EN=1 and
 * not SOLO, otherwise it does nothing.
 *
 * The ROM adds 1 to BootCnt on every boot (k = value after adding) and never clears it. Every reboot goes
 * through the slot selection again, no state is kept:
 *    k   1        2        3        4        5        6        7
 *        new+new  new+old  old+old  old+new  old+old  old+new  ROM stops
 * Call OTA_Rollback_Clear() once the application works. The SDK does it after app_example(); a product that
 * wants to do it after its own self check overrides app_ota_rollback_ok_callback(). If it is never called, the
 * ROM stops at the 7th boot.
 */

/** Bootloader: slot to boot. Returns Prim, or the other slot for this k. Writes nothing. */
u8 OTA_Rollback_SelectApp(u8 Prim, u8 ValidNum);

/** APP: the application works, clear BootCnt. */
void OTA_Rollback_Clear(void);

/*
 * Anti-rollback. BOOT and APP have separate versions, both count the 0 bits of an OTP area.
 * BOOT: SEC_BOOT_VER0/1 (physical 0x378..0x37F), checked by the ROM. APP: the area below, checked by the
 * bootloader. The enable bit is shared (physical 0x368 bit6) because the ROM fixes it.
 * The default APP area is only a default: it is in the user area of the OTP physical map, a product that
 * enables this must plan it itself and keep it away from its own OTP user data.
 * The highest version is OTA_ANTIROLLBACK_APP_OTP_LEN * 8.
 */
#ifndef OTA_ANTIROLLBACK_APP_OTP_ADDR
#define OTA_ANTIROLLBACK_APP_OTP_ADDR			0x4E0
#endif
#ifndef OTA_ANTIROLLBACK_APP_OTP_LEN
#define OTA_ANTIROLLBACK_APP_OTP_LEN			32
#endif
_Static_assert(OTA_ANTIROLLBACK_APP_OTP_ADDR + OTA_ANTIROLLBACK_APP_OTP_LEN <= 0x500, "APP version area must end before the ROM patch area");

/** Current BOOT / APP version in OTP, 0 if anti-rollback is not enabled. An OTP read failure (not expected) returns 0. */
u32 OTA_AntiRollback_GetBootVer(void);
u32 OTA_AntiRollback_GetAppVer(void);

/**
 * Raise the BOOT / APP version in OTP to Major, given by the caller. It is NOT reversible.
 * Returns RTK_SUCCESS, or RTK_FAIL when Major is not above the current version, Major is above the maximum, or
 * the OTP write / read back failed. Nothing else is checked here.
 * It does not need anti-rollback to be enabled: Set the version first, then OTA_AntiRollback_Enable().
 * The caller MUST keep Major not above the Major of the images it wants to keep (both slots): a slot whose
 * Major is below the version is rejected at boot. Call it only after the new image is known to work.
 */
int OTA_AntiRollback_SetBootVer(u32 Major);
int OTA_AntiRollback_SetAppVer(u32 Major);

/**
 * Enable anti-rollback, physical 0x368 bit6. NOT reversible, the ROM then checks SEC_BOOT_VER as well.
 * 0x368 is inside the OTP CRC range (0x200..0x36F). Call it BEFORE the CRC is written in production: it returns
 * RTK_FAIL without writing once a CRC entry exists, because the ROM would then stop at the next boot. If it is
 * needed after the CRC, clear the bit (active low) and then write a new CRC entry as OTP_SetCRC() does.
 */
int OTA_AntiRollback_Enable(void);

#ifdef __cplusplus
}
#endif

#endif
