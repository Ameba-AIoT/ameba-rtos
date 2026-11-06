/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_ota_rollback.h"

/*
 * k is the ROM BootCnt after the ROM added this boot. The ROM never clears it, it is cleared only by
 * OTA_Rollback_Clear(). With both slots valid:
 *   k : 1        2        3        4        5        6        7
 *       new+new  new+old  old+old  old+new  old+old  old+new  ROM stops
 * The ROM switches BOOT at k=3 and stops at k=7. The other APP slot is used when bit k of this mask is set.
 */
#define OTA_ROLLBACK_APP_SWITCH_MASK			(BIT(2) | BIT(3) | BIT(5))

static u8 OTA_Rollback_Support(void)
{
	return SYSCFG_OTP_BOOT_CNT_LMT_EN() ? TRUE : FALSE;
}

u8 OTA_Rollback_SelectApp(u8 Prim, u8 ValidNum)
{
	u32 k;

	if (!OTA_Rollback_Support() || (ValidNum != 2)) {
		return Prim;
	}

	k = BKUP_GET_BOOT_CNT(BKUP_Read(BKUP_REG0));
	if ((k <= BOOT_CNT_TOTAL_LIMIT) && (OTA_ROLLBACK_APP_SWITCH_MASK & BIT(k))) {
		return (Prim + 1) % 2;
	}

	return Prim;
}

void OTA_Rollback_Clear(void)
{
	if (OTA_Rollback_Support()) {
		BKUP_Clear(BKUP_REG0, BKUP_MASK_BOOT_CNT);
	}
}

/* ------------------------------------ anti-rollback ------------------------------------ */

/* Number of 0 bits in OTP physical [Addr, Addr + Len), the version. Returns 0 if a byte can not be read. */
static u32 OTA_AntiRollback_OtpCountZero(u32 Addr, u32 Len)
{
	u32 Ver = 0;
	u32 i, b;
	u8 Data;

	for (i = 0; i < Len; i++) {
		if (OTP_Read8(Addr + i, &Data) != RTK_SUCCESS) {
			return 0;
		}
		for (b = 0; b < 8; b++) {
			if (!(Data & BIT(b))) {
				Ver++;
			}
		}
	}

	return Ver;
}

#define OTA_ANTIROLLBACK_BOOT_OTP_ADDR		SEC_BOOT_IMGVER0
#define OTA_ANTIROLLBACK_BOOT_OTP_LEN			2

u32 OTA_AntiRollback_GetBootVer(void)
{
	return SYSCFG_OTP_AntiRollback() ? OTA_AntiRollback_OtpCountZero(OTA_ANTIROLLBACK_BOOT_OTP_ADDR, OTA_ANTIROLLBACK_BOOT_OTP_LEN) : 0;
}

u32 OTA_AntiRollback_GetAppVer(void)
{
	return SYSCFG_OTP_AntiRollback() ? OTA_AntiRollback_OtpCountZero(OTA_ANTIROLLBACK_APP_OTP_ADDR, OTA_ANTIROLLBACK_APP_OTP_LEN) : 0;
}

/* Write the version up to Major, 0 bits only. The OTP is read back to check it. */
static int OTA_AntiRollback_OtpSetZero(u32 Addr, u32 Len, u32 Major)
{
	u32 Cur = OTA_AntiRollback_OtpCountZero(Addr, Len);
	u32 i;
	u8 Data;

	for (i = 0; (i < Len) && (Cur < Major); i++) {
		if (OTP_Read8(Addr + i, &Data) != RTK_SUCCESS) {
			return RTK_FAIL;
		}
		while ((Data != 0) && (Cur < Major)) {
			u8 Bit = 0;
			while (!(Data & BIT(Bit))) {
				Bit++;
			}
			Data &= ~BIT(Bit);
			Cur++;
		}
		if (OTP_Write8(Addr + i, Data) != RTK_SUCCESS) {
			return RTK_FAIL;
		}
	}

	return (OTA_AntiRollback_OtpCountZero(Addr, Len) == Major) ? RTK_SUCCESS : RTK_FAIL;
}

static int OTA_AntiRollback_SetVer(u32 Addr, u32 Len, u32 Major)
{
	if ((Major <= OTA_AntiRollback_OtpCountZero(Addr, Len)) || (Major > Len * 8)) {
		return RTK_FAIL;
	}

	return OTA_AntiRollback_OtpSetZero(Addr, Len, Major);
}

int OTA_AntiRollback_SetBootVer(u32 Major)
{
	/* SEC_BOOT_IMGVER0 is inside the OTP CRC range 0x200..0x36F, same as SEC_CFG2 in OTA_AntiRollback_Enable() */
	if (OTPC_GET_CRC_ENTRY_EN(OTPC_DEV->OTPC_OTP_CRC_RESULT) != 0) {
		return RTK_FAIL;
	}

	return OTA_AntiRollback_SetVer(OTA_ANTIROLLBACK_BOOT_OTP_ADDR, OTA_ANTIROLLBACK_BOOT_OTP_LEN, Major);
}

int OTA_AntiRollback_SetAppVer(u32 Major)
{
	return OTA_AntiRollback_SetVer(OTA_ANTIROLLBACK_APP_OTP_ADDR, OTA_ANTIROLLBACK_APP_OTP_LEN, Major);
}

int OTA_AntiRollback_Enable(void)
{
	u8 Data;

	/* SEC_CFG2 is inside the OTP CRC range 0x200..0x36F: once a CRC entry is written, changing it would fail the
	 * ROM CRC check at the next boot and the device would not start. */
	if (OTPC_GET_CRC_ENTRY_EN(OTPC_DEV->OTPC_OTP_CRC_RESULT) != 0) {
		return RTK_FAIL;
	}

	if (OTP_Read8(SEC_CFG2, &Data) != RTK_SUCCESS) {
		return RTK_FAIL;
	}

	return OTP_Write8(SEC_CFG2, Data & ~(SEC_BIT_ANTI_ROLLBACK_EN & 0xFF));
}
