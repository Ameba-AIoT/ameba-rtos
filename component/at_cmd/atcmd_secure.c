/**
 * @file atcmd_secure.c
 * @brief AT+SEC command implementation for secure OTP programming.
 *
 * Provides high-level AT commands for burning and managing four security
 * features stored in OTP:
 *   - **RSIP** — Flash firmware encryption (AES-256 XTS/GCM/CTR)
 *   - **RDP**  — TrustZone secure firmware protection
 *   - **Secure Boot** — Firmware signature verification (ECC/PQC)
 *   - **HUK**  — Hardware Unique Key (128-bit device seed)
 *
 * ICs are divided into two groups:
 *   - **Legacy** (CONFIG_RSIP_SINGLE_KEY): single RSIP key group, IPSEC-S RDP
 *   - **Default**: dual RSIP key groups, RSIP-based RDP (aligned to RTL8720F)
 *
 * Command prefix: AT+SEC
 *
 * @see otp_feature_analysis.md for OTP layout, address mapping, and IC differences
 */

/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#ifndef CONFIG_AMEBAD
#ifdef CONFIG_ATCMD_SECURE

#include "atcmd_service.h"
#include "atcmd_secure.h"

static const char *TAG = "ATCMD_SEC";

/* RSIP XTS two-step key programming state (RAM only, lost on reset).
 * XTS mode requires two 32B keys but AT-CMD cannot carry both in one
 * string. Step 1 buffers key1; step 2 writes both keys + mode to OTP.
 * While pending, only RSIP_KEY2 sub-command is accepted.
 */
static u8 rsip_key1_buf[OTP_RSIP_KEY1_LEN]; /* Buffered ECB key for XTS */
static u8 rsip_pending = FALSE;                       /* 0=idle, 1=awaiting RSIP_KEY2 */
static int rsip_pending_grp;                  /* Stored group (0 or 1) */

/**
 * @brief Generic security feature enable helper.
 *
 * Supports two enable modes:
 *   - **temp** (temporary): set logical OTP bit (reversible, for development)
 *   - **perm** (permanent): clear physical OTP bit (irreversible, for production)
 *
 * @param log_bit Bit position in logical OTP 0x3.
 * @param phy_bit Bit position in physical OTP 0x368.
 * @param arg     "temp" (logical set), "temp_off" (logical clear), or "perm" (physical clear).
 * @return RTK_SUCCESS on success, RTK_FAIL on error.
 */
static int sec_enable(u8 log_bit, u8 phy_bit, const char *arg)
{
	if (_strcmp(arg, "temp") == 0) {
		return OTP_SetLogBit(OTP_LOG_EN_ADDR, log_bit, 1);
	}
	if (_strcmp(arg, "temp_off") == 0) {
		return OTP_SetLogBit(OTP_LOG_EN_ADDR, log_bit, 0);
	}
	if (_strcmp(arg, "perm") == 0) {
		return OTP_ClearPhyBit(OTP_SEC_EN, phy_bit);
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "param err\n");
	return RTK_FAIL;
}

/**
 * @brief Convert a hex string to a byte buffer.
 * @param str     Input hex string (must have even length).
 * @param buf     Output byte buffer.
 * @param max_len Maximum number of bytes to write.
 * @return Number of converted bytes, 0 on error (odd length).
 */
static u32 hex2buf(const char *str, u8 *buf, u32 max_len)
{
	u32 slen, i;

	slen = _strlen((char *)str);
	if (slen & 1) {
		return 0;
	}
	slen >>= 1;
	if (slen > max_len) {
		slen = max_len;
	}
	for (i = 0; i < slen; i++) {
		buf[i] = _2char2hex(str[i * 2], str[i * 2 + 1]);
	}
	return slen;
}

/* ===== Parameter parsing ===== */

/**
 * @brief Parse RSIP key group string.
 * @param s Input string: "grp0" or "grp1".
 * @return 0 for grp0, 1 for grp1, -1 on error.
 * @note grp1 is only valid for Default group (not CONFIG_RSIP_SINGLE_KEY).
 */
static int parse_grp(const char *s)
{
	if (_strcmp(s, "grp0") == 0) {
		return 0;
	}
	if (_strcmp(s, "grp1") == 0) {
		return 1;
	}
	return -1;
}

/**
 * @brief Parse RSIP encryption mode string.
 * @param s Input string: "xts", "ctr", or "gcm".
 * @return RSIP_MODE_XTS (0), RSIP_MODE_CTR (1), or RSIP_MODE_GCM (2), -1 on error.
 * @note GCM mode is only supported on Default group (RTL8721F, RTL8720F).
 */
static int parse_mode(const char *s)
{
	if (_strcmp(s, "xts") == 0) {
		return RSIP_MODE_XTS;
	}
	if (_strcmp(s, "ctr") == 0) {
		return RSIP_MODE_CTR;
	}
	if (_strcmp(s, "gcm") == 0) {
		return RSIP_MODE_GCM;
	}
	return -1;
}

/**
 * @brief Parse key protection type string.
 * @param s Input string: "r", "w", or "rw".
 * @return Bitmask: 1=read-protect, 2=write-protect, 3=both, 0 on error.
 */
static int parse_prot(const char *s)
{
	if (_strcmp(s, "r") == 0) {
		return 1;
	}
	if (_strcmp(s, "w") == 0) {
		return 2;
	}
	if (_strcmp(s, "rw") == 0) {
		return 3;
	}
	return 0;
}

/**
 * @brief Parse Secure Boot public key slot string.
 * @param s Input string: "pk1" or "pk2".
 * @return 0 for pk1 (OTP 0x320), 1 for pk2 (OTP 0x340), -1 on error.
 */
static int parse_pk(const char *s)
{
	if (_strcmp(s, "pk1") == 0) {
		return 0;
	}
	if (_strcmp(s, "pk2") == 0) {
		return 1;
	}
	return -1;
}

/* ===== SBoot algorithm tables ===== */

/**
 * @brief Maximum auth algorithm support level for current IC.
 *
 * Determines which entries in sboot_auth_tbl[] are usable:
 *   - 0: ed25519 only (RTL8721Dx, CONFIG_AMEBADPLUS)
 *   - 1: ed25519 + 7 ECDSA curves (!AMEBADPLUS Legacy or AMEBAGREEN2)
 *   - 2: all algorithms including PQC (RTL8720F, default path)
 */
#if defined(CONFIG_AMEBADPLUS)
#define SBOOT_AUTH_MAX_LEVEL 0
#elif defined(CONFIG_RSIP_SINGLE_KEY) || defined(CONFIG_AMEBAGREEN2)
#define SBOOT_AUTH_MAX_LEVEL 1
#else
#define SBOOT_AUTH_MAX_LEVEL 2
#endif

/**
 * @brief Secure Boot authentication algorithm table entry.
 */
struct auth_entry {
	const char *name;	/**< Algorithm name string (e.g. "ed25519", "secp256r1") */
	u8 val;			/**< OTP value for SECURE_BOOT_AUTH_ALG at 0x36B[3:0] */
	u8 level;		/**< Minimum support level required (see SBOOT_AUTH_MAX_LEVEL) */
};

/**
 * @brief Secure Boot hash algorithm table entry.
 */
struct hash_entry {
	const char *name;	/**< Hash algorithm name string (e.g. "sha256", "hmac384") */
	u8 val;			/**< OTP value for SECURE_BOOT_HASH_ALG at 0x36B[7:4] */
};

/**
 * @brief Supported authentication algorithms for Secure Boot.
 *
 * Maps algorithm name strings to OTP values and support levels.
 * Availability depends on IC group (see otp_feature_analysis.md Section 5.1).
 */
static const struct auth_entry sboot_auth_tbl[] = {
	{"ed25519",    0x00, 0},
	{"secp192r1",  0x01, 1},
	{"secp224r1",  0x02, 1},
	{"secp256r1",  0x03, 1},
	{"secp384r1",  0x04, 2},
	{"bp256r1",    0x06, 1},
	{"bp384r1",    0x07, 2},
	{"bp512r1",    0x08, 2},
	{"curve25519", 0x09, 2},
	{"secp192k1",  0x0A, 1},
	{"secp224k1",  0x0B, 1},
	{"secp256k1",  0x0C, 1},
	{"ml_dsa_65",  0xF1, 2},
};

/**
 * @brief Supported hash algorithms for Secure Boot.
 *
 * All ICs support the same set of hash algorithms.
 * OTP values are written to 0x36B[7:4].
 */
static const struct hash_entry sboot_hash_tbl[] = {
	{"sha256",  0x00},
	{"sha384",  0x01},
	{"sha512",  0x02},
	{"hmac256", 0x03},
	{"hmac384", 0x04},
	{"hmac512", 0x05},
};

#ifndef ARRAY_SIZE
/** @brief Calculate number of elements in a statically-sized array */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/**
 * @brief Parse Secure Boot authentication algorithm string.
 *
 * Looks up the algorithm name in sboot_auth_tbl[] and checks whether
 * the current IC supports it (level <= SBOOT_AUTH_MAX_LEVEL).
 *
 * @param s Algorithm name string.
 * @return OTP value for the algorithm, -1 if not found or not supported.
 */
static int parse_auth(const char *s)
{
	u32 i;

	for (i = 0; i < ARRAY_SIZE(sboot_auth_tbl); i++) {
		if (_strcmp(s, sboot_auth_tbl[i].name) == 0) {
			if (sboot_auth_tbl[i].level > SBOOT_AUTH_MAX_LEVEL) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "auth not sup\n");
				return -1;
			}
			return sboot_auth_tbl[i].val;
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "auth err\n");
	return -1;
}

/**
 * @brief Parse Secure Boot hash algorithm string.
 * @param s Hash algorithm name string.
 * @return OTP value for the hash algorithm, -1 if not found.
 */
static int parse_hash(const char *s)
{
	u32 i;

	for (i = 0; i < ARRAY_SIZE(sboot_hash_tbl); i++) {
		if (_strcmp(s, sboot_hash_tbl[i].name) == 0) {
			return sboot_hash_tbl[i].val;
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "hash err\n");
	return -1;
}

/* ===== RSIP helpers ===== */

/**
 * @brief Get RSIP key OTP addresses and expected data length for a group/mode pair.
 *
 * Address mapping:
 *   - grp0 XTS: addr1=0x2C0 (ECB 32B) + addr2=0x2E0 (CTR 32B), total 64B
 *   - grp0 CTR/GCM: addr2=0x2E0, 32B
 *   - grp1 XTS: addr1=0x200 (ECB 32B) + addr2=0x220 (CTR 32B), total 64B
 *   - grp1 CTR/GCM: addr2=0x220, 32B
 *
 * @param grp   Key group (0 or 1).
 * @param mode  RSIP_MODE_XTS, RSIP_MODE_CTR, or RSIP_MODE_GCM.
 * @param addr1 Output: first key address (ECB key for XTS, 0 for CTR/GCM).
 * @param addr2 Output: second key address (CTR key).
 * @return Expected key data length in bytes, 0 on error (invalid group for Legacy ICs).
 * @note grp1 is only available on Default group (not CONFIG_RSIP_SINGLE_KEY).
 */
static u32 rsip_get_key_addrs(int grp, int mode, u32 *addr1, u32 *addr2)
{
#ifdef CONFIG_RSIP_SINGLE_KEY
	if (grp != 0) {
		return 0;
	}
#endif
	if (grp == 0) {
		*addr1 = OTP_RSIP_KEY1_ADDR;
		*addr2 = OTP_RSIP_KEY2_ADDR;
	} else {
		*addr1 = OTP_RSIP_KEY3_ADDR;
		*addr2 = OTP_RSIP_KEY4_ADDR;
	}

	if (mode == RSIP_MODE_XTS) {
		return OTP_RSIP_KEY1_LEN * 2;
	}
	*addr1 = 0;
	return OTP_RSIP_KEY2_LEN;
}

/**
 * @brief Write RSIP_MODE to physical OTP 0x369[1:0] with verification.
 *
 * OTP AND semantics mean bits can only be cleared (1->0). If the current
 * mode already has bits set that the new mode does not, the operation fails
 * (mode is locked).
 *
 * @param mode_val RSIP mode value: RSIP_MODE_XTS(0), RSIP_MODE_CTR(1), RSIP_MODE_GCM(2).
 * @return RTK_SUCCESS on success, RTK_FAIL if mode is locked or write/verify fails.
 */
static int rsip_write_mode(u8 mode_val)
{
	u8 old, cur, write_val;

	if (OTP_Read8(OTP_RSIP_MODE_CTRL, &old) != RTK_SUCCESS) {
		return RTK_FAIL;
	}

	cur = old & RSIP_MODE_BIT_MASK;
	/* Check: can only go 1->0, mode_val must be subset of cur */
	if ((mode_val & ~cur) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "mode locked:%x\n", cur);
		return RTK_FAIL;
	}
	if (cur == mode_val) {
		return RTK_SUCCESS;
	}

	/* Write: set bits[1:0] to mode_val, preserve bits[7:2]
	 * 0xFC | mode_val -> AND with old preserves upper bits, sets [1:0] */
	write_val = 0xFC | mode_val;
	if (OTP_Write8(OTP_RSIP_MODE_CTRL, write_val) != RTK_SUCCESS) {
		return RTK_FAIL;
	}
	return RTK_SUCCESS;
}

/* ===== RSIP sub-commands ===== */

/**
 * @brief Handle AT+SEC=RSIP_KEY command — burn RSIP encryption key and set mode.
 *
 * For CTR/GCM mode: writes the single key and RSIP_MODE to OTP immediately.
 * For XTS mode: buffers the first key (ECB) and enters pending state.
 *               Send AT+SEC=RSIP_KEY2 to provide the second key (CTR) and
 *               commit both keys + mode to OTP.
 *
 * @par Usage
 *   AT+SEC=RSIP_KEY,<grp>,<mode>,<hex>
 *
 * @par Parameters
 *   - grp: "grp0" (0x2C0+0x2E0) or "grp1" (0x200+0x220, Default group only)
 *   - mode: "xts", "ctr", or "gcm" (Default group only)
 *   - hex: key hex string (32B = 64 chars)
 *
 * @param argc Argument count (minimum 5).
 * @param argv Argument vector: argv[2]=grp, argv[3]=mode, argv[4]=hex_key.
 */
static void at_sec_rsip_key(u16 argc, char **argv)
{
	int grp, mode;
	u8 buf[OTP_RSIP_KEY1_LEN];

	if (argc < 5) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	grp = parse_grp(argv[2]);
	mode = parse_mode(argv[3]);

#ifdef CONFIG_RSIP_SINGLE_KEY
	if (grp != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "grp1 not sup\n");
		return;
	}
	if (mode == RSIP_MODE_GCM) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "gcm not sup\n");
		return;
	}
#endif

	if (grp < 0 || mode < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "param err\n");
		return;
	}

	if (mode == RSIP_MODE_XTS) {
		/* XTS step 1: buffer first key (ECB), wait for RSIP_KEY2 */
		if (hex2buf(argv[4], rsip_key1_buf, sizeof(rsip_key1_buf)) != OTP_RSIP_KEY1_LEN) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ecb len err\n");
			return;
		}
		rsip_pending = TRUE;
		rsip_pending_grp = grp;
		RTK_LOGS(TAG, RTK_LOG_INFO, "KEY1 OK\n");
	} else {
		/* CTR/GCM: single key, write immediately */
		u32 addr1, addr2;

		rsip_get_key_addrs(grp, mode, &addr1, &addr2);
		if (rsip_write_mode((u8)mode) != RTK_SUCCESS) {
			return;
		}
		if (hex2buf(argv[4], buf, sizeof(buf)) != OTP_RSIP_KEY2_LEN) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "key len err\n");
			return;
		}
		if (OTP_WritePhyBuf(addr2, buf, OTP_RSIP_KEY2_LEN) != RTK_SUCCESS) {
			return;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=RSIP_KEY2 command — provide second key for XTS mode.
 *
 * Completes the two-step XTS key programming sequence:
 *   1. AT+SEC=RSIP_KEY buffers the ECB key (step 1)
 *   2. AT+SEC=RSIP_KEY2 provides the CTR key and commits both keys + mode to OTP (step 2)
 *
 * On success or failure, the pending state is cleared and the key buffer zeroed.
 * If power is lost between steps, no OTP is written (state is RAM-only).
 *
 * @par Usage
 *   AT+SEC=RSIP_KEY2,<hex>  (32B = 64 chars)
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]=hex key string (64 chars = 32 bytes).
 */
static void at_sec_rsip_key2(u16 argc, char **argv)
{
	u32 addr1, addr2;
	u8 buf[OTP_RSIP_KEY2_LEN];

	if (!rsip_pending) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "no pending\n");
		return;
	}

	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	if (hex2buf(argv[2], buf, sizeof(buf)) != OTP_RSIP_KEY2_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ctr len err\n");
		return;
	}

	rsip_get_key_addrs(rsip_pending_grp, RSIP_MODE_XTS, &addr1, &addr2);

	/* From here, OTP writes are attempted — clear pending regardless */
	rsip_pending = FALSE;

	if (rsip_write_mode(RSIP_MODE_XTS) != RTK_SUCCESS) {
		goto clear;
	}
	if (OTP_WritePhyBuf(addr1, rsip_key1_buf, OTP_RSIP_KEY1_LEN) != RTK_SUCCESS) {
		goto clear;
	}
	if (OTP_WritePhyBuf(addr2, buf, OTP_RSIP_KEY2_LEN) != RTK_SUCCESS) {
		goto clear;
	}

	/* Secure clear key buffer */
	_memset(rsip_key1_buf, 0, sizeof(rsip_key1_buf));

	RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	return;

clear:
	_memset(rsip_key1_buf, 0, sizeof(rsip_key1_buf));
}

/**
 * @brief Handle AT+SEC=RSIP_EN command — enable RSIP.
 *
 * @par Usage
 *   AT+SEC=RSIP_EN,<perm|temp|temp_off>
 *
 * - temp: set logical 0x3[3] = 1 (reversible, for development)
 * - temp_off: clear logical 0x3[3] = 0 (reverse temp enable)
 * - perm: clear physical 0x368[2] = 0 (irreversible, for production)
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]="perm", "temp", or "temp_off".
 */
static void at_sec_rsip_en(u16 argc, char **argv)
{
	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}
	if (sec_enable(RSIP_EN_LOG_BIT, RSIP_EN_PHY_BIT, argv[2]) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=RSIP_LOCK command — lock RSIP_MODE (irreversible).
 *
 * Clears RSIP_MODE_W_Forbidden bit at physical 0x367[3] to 0,
 * preventing any further changes to RSIP_MODE. This operation is permanent.
 */
static void at_sec_rsip_lock(void)
{
	if (OTP_ClearPhyBit(OTP_RSIP_KEY_CTRL, RSIP_MODE_W_LOCK_BIT) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=RSIP_PROT command — set RSIP key read/write protection.
 *
 * Protection bit mapping:
 *   - grp0: 0x366[7](KEY1_R), 0x367[0](KEY1_W), 0x367[1](KEY2_R), 0x367[2](KEY2_W)
 *   - grp1: 0x365[3](KEY3_R), 0x365[4](KEY3_W), 0x365[5](KEY4_R), 0x365[6](KEY4_W)
 *
 * @par Usage
 *   AT+SEC=RSIP_PROT,<grp>,<r|w|rw>
 *
 * @param argc Argument count (minimum 4).
 * @param argv Argument vector: argv[2]=grp, argv[3]=protection type.
 * @note grp1 protection is only available on Default group (not CONFIG_RSIP_SINGLE_KEY).
 */
static void at_sec_rsip_prot(u16 argc, char **argv)
{
	int grp, prot;

	if (argc < 4) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	grp = parse_grp(argv[2]);
	prot = parse_prot(argv[3]);

#ifdef CONFIG_RSIP_SINGLE_KEY
	if (grp != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "grp1 not sup\n");
		return;
	}
#endif

	if (grp < 0 || prot == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "param err\n");
		return;
	}

	if (grp == 0) {
		/* grp0: KEY1(ECB) + KEY2(CTR) protection */
		if (prot & 1) { /* read protect */
			if (OTP_ClearPhyBit(OTP_RSIP_KEY1_R_CTRL,
								RSIP_KEY1_R_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
			if (OTP_ClearPhyBit(OTP_RSIP_KEY_CTRL,
								RSIP_KEY2_R_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
		}
		if (prot & 2) { /* write protect */
			if (OTP_ClearPhyBit(OTP_RSIP_KEY_CTRL,
								RSIP_KEY1_W_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
			if (OTP_ClearPhyBit(OTP_RSIP_KEY_CTRL,
								RSIP_KEY2_W_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
		}
	} else {
		/* grp1: KEY3 + KEY4 protection, all in OTP_KEY_PROT (0x365) */
		if (prot & 1) {
			if (OTP_ClearPhyBit(OTP_KEY_PROT,
								RSIP_KEY3_R_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
			if (OTP_ClearPhyBit(OTP_KEY_PROT,
								RSIP_KEY4_R_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
		}
		if (prot & 2) {
			if (OTP_ClearPhyBit(OTP_KEY_PROT,
								RSIP_KEY3_W_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
			if (OTP_ClearPhyBit(OTP_KEY_PROT,
								RSIP_KEY4_W_PROT_BIT) != RTK_SUCCESS) {
				return;
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
}

/* ===== RDP sub-commands ===== */

/**
 * @brief Handle AT+SEC=RDP_EN command — enable RDP (common to all ICs).
 *
 * @par Usage
 *   AT+SEC=RDP_EN,<perm|temp|temp_off>
 *
 * - temp: set logical 0x3[4] = 1 (reversible)
 * - temp_off: clear logical 0x3[4] = 0 (reverse temp enable)
 * - perm: clear physical 0x368[5] = 0 (irreversible)
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]="perm", "temp", or "temp_off".
 */
static void at_sec_rdp_en(u16 argc, char **argv)
{
	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}
	if (sec_enable(RDP_EN_LOG_BIT, RDP_EN_PHY_BIT, argv[2]) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/* RDP_KEY/RDP_PROT: Legacy group only (IPSEC-S independent key at 0x200) */
#ifdef CONFIG_RSIP_SINGLE_KEY

/**
 * @brief Handle AT+SEC=RDP_KEY command — burn RDP key (Legacy group only).
 *
 * Writes 32-byte IPSEC-S RDP key to OTP 0x200 (S_IPSEC_Key1).
 * Performs automatic read-back verification after writing.
 *
 * @par Usage
 *   AT+SEC=RDP_KEY,<hex>  (32B = 64 hex chars)
 *
 * @note Only available when CONFIG_RSIP_SINGLE_KEY is defined (Legacy group).
 *       Default group ICs should use AT+SEC=RSIP_KEY for RDP (RDP reuses RSIP keys).
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]=hex key string (64 chars = 32 bytes).
 */
static void at_sec_rdp_key(u16 argc, char **argv)
{
	u8 buf[OTP_RDP_KEY_LEN];

	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	if (hex2buf(argv[2], buf, sizeof(buf)) != OTP_RDP_KEY_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "len err:32B\n");
		return;
	}

	if (OTP_WritePhyBuf(OTP_RDP_KEY_ADDR, buf, OTP_RDP_KEY_LEN) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=RDP_PROT command — set RDP key protection (Legacy group only).
 *
 * Protection bit mapping (Legacy group):
 *   - 0x365[3]: S_IPSEC_Key1 read protect (RDP key)
 *   - 0x365[4]: S_IPSEC_Key1 write protect (RDP key)
 *
 * @par Usage
 *   AT+SEC=RDP_PROT,<r|w|rw>
 *
 * @note Only available when CONFIG_RSIP_SINGLE_KEY is defined (Legacy group).
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]=protection type ("r", "w", or "rw").
 */
static void at_sec_rdp_prot(u16 argc, char **argv)
{
	int prot;

	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	prot = parse_prot(argv[2]);
	if (prot == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "param err\n");
		return;
	}

	if (prot & 1) {
		if (OTP_ClearPhyBit(OTP_KEY_PROT, RDP_KEY_R_PROT_BIT) != RTK_SUCCESS) {
			return;
		}
	}
	if (prot & 2) {
		if (OTP_ClearPhyBit(OTP_KEY_PROT, RDP_KEY_W_PROT_BIT) != RTK_SUCCESS) {
			return;
		}
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
}
#endif /* CONFIG_RSIP_SINGLE_KEY */

/* ===== SBoot helpers ===== */

/**
 * @brief Write Secure Boot algorithm byte to physical OTP 0x36B with verification.
 *
 * Encodes auth and hash into a single byte:
 *   - Standard auth (val <= 0x0F): byte = (hash << 4) | auth
 *   - PQC auth (val > 0x0F, e.g. ml_dsa_65=0xF1): byte = auth_val directly
 *
 * OTP AND semantics: can only clear bits. If current value has bits set
 * that the desired value does not, the operation fails (algorithm is locked).
 *
 * @param auth OTP auth algorithm value (from sboot_auth_tbl).
 * @param hash OTP hash algorithm value (from sboot_hash_tbl).
 * @return RTK_SUCCESS on success, RTK_FAIL if locked or write/verify fails.
 */
static int sboot_write_alg(u8 auth, u8 hash)
{
	u8 cur, desired, to_clear;

	/* PQC (auth > 0x0F) uses full byte; otherwise combine hash<<4 | auth */
	if (auth > SBOOT_AUTH_MASK) {
		desired = auth;
	} else {
		desired = (hash << SBOOT_HASH_SHIFT) | auth;
	}

	if (OTP_Read8(OTP_SBOOT_ALG, &cur) != RTK_SUCCESS) {
		return RTK_FAIL;
	}

	/* Can only clear bits (1->0) */
	if ((desired & ~cur) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "alg locked:%x\n", cur);
		return RTK_FAIL;
	}

	to_clear = cur & ~desired;
	if (to_clear == 0) {
		return RTK_SUCCESS;    /* already correct */
	}

	if (OTP_Write8(OTP_SBOOT_ALG, desired) != RTK_SUCCESS) {
		return RTK_FAIL;
	}

	return RTK_SUCCESS;
}

/* ===== SBoot sub-commands ===== */

/**
 * @brief Handle AT+SEC=SBOOT_PK command — burn Secure Boot public key hash.
 *
 * Writes 32-byte SHA-256 hash of the public key to OTP PK1 (0x320) or PK2 (0x340).
 * Performs automatic read-back verification after writing.
 *
 * @par Usage
 *   AT+SEC=SBOOT_PK,<pk>,<hex>  (32B = 64 hex chars)
 *
 * @par Parameters
 *   - pk: "pk1" (0x320, ECC key hash) or "pk2" (0x340, Cert/PQC key hash)
 *   - hex: SHA-256 hash hex string (64 chars)
 *
 * @param argc Argument count (minimum 4).
 * @param argv Argument vector: argv[2]=pk slot, argv[3]=hex hash string.
 */
static void at_sec_sboot_pk(u16 argc, char **argv)
{
	int pk;
	u32 addr;
	u8 buf[OTP_SBOOT_PK_LEN];

	if (argc < 4) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	pk = parse_pk(argv[2]);
	if (pk < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "pk err\n");
		return;
	}

	addr = pk ? OTP_SBOOT_PK2_ADDR : OTP_SBOOT_PK1_ADDR;

	if (hex2buf(argv[3], buf, OTP_SBOOT_PK_LEN) != OTP_SBOOT_PK_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "len err:32B\n");
		return;
	}

	if (OTP_WritePhyBuf(addr, buf, OTP_SBOOT_PK_LEN) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=SBOOT_EN command — enable Secure Boot.
 *
 * @par Usage
 *   AT+SEC=SBOOT_EN,<perm|temp|temp_off>
 *
 * - temp: set logical 0x3[2] = 1 (reversible, for development)
 * - temp_off: clear logical 0x3[2] = 0 (reverse temp enable)
 * - perm: clear physical 0x368[3] = 0 (irreversible, for production)
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]="perm", "temp", or "temp_off".
 */
static void at_sec_sboot_en(u16 argc, char **argv)
{
	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}
	if (sec_enable(SBOOT_EN_LOG_BIT, SBOOT_EN_PHY_BIT, argv[2]) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=SBOOT_ALG command — set Secure Boot signature and hash algorithms.
 *
 * Writes the algorithm byte to physical OTP 0x36B:
 *   byte = (hash_val << 4) | auth_val
 *
 * Performs automatic read-back verification after writing.
 *
 * @par Usage
 *   AT+SEC=SBOOT_ALG,<auth>,<hash>
 *
 * @par Parameters
 *   - auth: algorithm name (e.g. "ed25519", "secp256r1", "ml_dsa_65")
 *   - hash: hash name (e.g. "sha256", "sha384", "hmac256")
 *
 * @param argc Argument count (minimum 4).
 * @param argv Argument vector: argv[2]=auth algorithm, argv[3]=hash algorithm.
 * @see sboot_auth_tbl for supported auth algorithms per IC.
 * @see sboot_hash_tbl for supported hash algorithms.
 */
static void at_sec_sboot_alg(u16 argc, char **argv)
{
	int auth, hash;

	if (argc < 4) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	auth = parse_auth(argv[2]);
	hash = parse_hash(argv[3]);
	if (auth < 0 || hash < 0) {
		return;
	}

	if (sboot_write_alg((u8)auth, (u8)hash) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=SBOOT_PQC command — enable PQC Secure Boot.
 *
 * Only available on RTL8720F (default path, not SINGLE_KEY, not AMEBAGREEN2).
 * Other ICs will return "not sup" error.
 *
 * @par Usage
 *   AT+SEC=SBOOT_PQC,<perm|temp|temp_off>  (RTL8720F only)
 *
 * - temp: set logical 0x3[5] = 1 (reversible)
 * - temp_off: clear logical 0x3[5] = 0 (reverse temp enable)
 * - perm: clear physical 0x368[1] = 0 (irreversible)
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]="perm", "temp", or "temp_off".
 */
static void at_sec_sboot_pqc(u16 argc, char **argv)
{
#if !defined(CONFIG_RSIP_SINGLE_KEY) && !defined(CONFIG_AMEBAGREEN2)
	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}
	if (sec_enable(PQC_EN_LOG_BIT, PQC_EN_PHY_BIT, argv[2]) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
#else
	(void)argc;
	(void)argv;
	RTK_LOGS(TAG, RTK_LOG_ERROR, "not sup\n");
#endif
}

/**
 * @brief Handle AT+SEC=SBOOT_HMAC command — burn Secure Boot HMAC key.
 *
 * Writes 32-byte HMAC key to OTP 0x220 (shared with RSIP Key4 / S_IPSEC_Key2).
 * Performs automatic read-back verification after writing.
 *
 * @par Usage
 *   AT+SEC=SBOOT_HMAC,<hex>  (32B = 64 hex chars)
 *
 * @warning Address 0x220 is shared with RSIP Key4 (Default group) or
 *          S_IPSEC_Key2 (Legacy group). Ensure no conflict before use.
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]=hex key string (64 chars = 32 bytes).
 */
static void at_sec_sboot_hmac(u16 argc, char **argv)
{
	u8 buf[OTP_SBOOT_HMAC_LEN];

	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	if (hex2buf(argv[2], buf, OTP_SBOOT_HMAC_LEN) != OTP_SBOOT_HMAC_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "len err:32B\n");
		return;
	}

	if (OTP_WritePhyBuf(OTP_SBOOT_HMAC_ADDR, buf, OTP_SBOOT_HMAC_LEN) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=SBOOT_PROT command — write-protect Secure Boot public key (irreversible).
 *
 * Protection bit mapping:
 *   - pk1: 0x365[1] = 0 (PK1 write protect)
 *   - pk2: 0x365[2] = 0 (PK2 write protect)
 *
 * @par Usage
 *   AT+SEC=SBOOT_PROT,<pk>
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]="pk1" or "pk2".
 */
static void at_sec_sboot_prot(u16 argc, char **argv)
{
	int pk;

	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	pk = parse_pk(argv[2]);
	if (pk < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "pk err\n");
		return;
	}

	if (OTP_ClearPhyBit(OTP_KEY_PROT, pk ? PK2_W_PROT_BIT : PK1_W_PROT_BIT) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/* ===== HUK sub-commands ===== */

/**
 * @brief Handle AT+SEC=HUK_KEY command — burn Hardware Unique Key seed.
 *
 * Writes 16-byte (128-bit) random seed to OTP 0x310.
 * Performs automatic read-back verification after writing.
 *
 * @par Usage
 *   AT+SEC=HUK_KEY,<hex>  (16B = 32 hex chars)
 *
 * @param argc Argument count (minimum 3).
 * @param argv Argument vector: argv[2]=hex seed string (32 chars = 16 bytes).
 */
static void at_sec_huk_key(u16 argc, char **argv)
{
	u8 buf[OTP_HUK_LEN];

	if (argc < 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "arg err\n");
		return;
	}

	if (hex2buf(argv[2], buf, OTP_HUK_LEN) != OTP_HUK_LEN) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "len err:16B\n");
		return;
	}

	if (OTP_WritePhyBuf(OTP_HUK_ADDR, buf, OTP_HUK_LEN) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=HUK_EN command — enable HUK derivation (irreversible).
 *
 * Clears HUK_DERIV_EN bit at physical 0x369[2] to 0.
 * After enabling, HUK is used by Boot ROM to derive device-unique keys via KDF.
 */
static void at_sec_huk_en(void)
{
	if (OTP_ClearPhyBit(OTP_RSIP_MODE_CTRL, HUK_DERIV_EN_BIT) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/**
 * @brief Handle AT+SEC=HUK_LOCK command — write-protect HUK (irreversible).
 *
 * Clears HUK_W_Forbidden_EN bit at physical 0x364[7] to 0,
 * permanently preventing any further writes to the HUK OTP area.
 */
static void at_sec_huk_lock(void)
{
	if (OTP_ClearPhyBit(OTP_HUK_CTRL, HUK_LOCK_BIT) == RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OK\n");
	}
}

/* ===== Help ===== */

/**
 * @brief Print AT+SEC command usage help.
 *
 * Output varies by IC group:
 *   - Legacy group: includes RDP_KEY, RDP_PROT
 *   - Default group: shows "RDP:use RSIP" hint
 *   - RTL8720F: includes SBOOT_PQC
 */
static void at_sec_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "RSIP_KEY:<grp>,<mode>,<hex>\n"
			 "  xts:2step(KEY1->RSIP_KEY2) ctr/gcm:1key(32B)\n"
			 "RSIP_KEY2:<hex> (xts key2, 32B)\n"
			 "RSIP_EN:<perm|temp|temp_off>\n"
			 "RSIP_LOCK\n"
			 "RSIP_PROT:<grp>,<r|w|rw>\n");
#ifdef CONFIG_RSIP_SINGLE_KEY
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "RDP_KEY:<hex> (32B)\n"
			 "RDP_EN:<perm|temp|temp_off>\n"
			 "RDP_PROT:<r|w|rw>\n");
#else
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "RDP:use RSIP\n");
#endif
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "SBOOT_PK:<pk>,<hex> (32B)\n"
			 "  pk:pk1,pk2\n"
			 "SBOOT_EN:<perm|temp|temp_off>\n"
			 "SBOOT_ALG:<auth>,<hash>\n"
			 "  auth:ed25519,secp256r1,...\n"
			 "  hash:sha256,sha384,...\n");
#if !defined(CONFIG_RSIP_SINGLE_KEY) && !defined(CONFIG_AMEBAGREEN2)
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "SBOOT_PQC:<perm|temp|temp_off>\n");
#endif
	RTK_LOGS(NOTAG, RTK_LOG_INFO,
			 "SBOOT_HMAC:<hex> (32B)\n"
			 "SBOOT_PROT:<pk>\n"
			 "HUK_KEY:<hex> (16B)\n"
			 "HUK_EN\n"
			 "HUK_LOCK\n");
}

/* ===== Main dispatch ===== */

/**
 * @brief AT+SEC main command dispatcher.
 *
 * Parses the sub-command string and dispatches to the appropriate handler.
 *
 * Supported sub-commands:
 *   - RSIP_KEY, RSIP_KEY2, RSIP_EN, RSIP_LOCK, RSIP_PROT — RSIP flash encryption
 *   - RDP_KEY, RDP_EN, RDP_PROT — RDP TrustZone protection
 *   - SBOOT_PK, SBOOT_EN, SBOOT_ALG, SBOOT_PQC, SBOOT_HMAC, SBOOT_PROT — Secure Boot
 *   - HUK_KEY, HUK_EN, HUK_LOCK — Hardware Unique Key
 *
 * @param argc Argument count.
 * @param argv Argument vector: argv[0]="AT", argv[1]="+SEC", argv[2]=sub-command.
 */
void at_secure(u16 argc, char **argv)
{
	if (argc < 2) {
		at_sec_help();
		return;
	}

	/* RSIP_KEY2: always handled (checks pending state internally) */
	if (_strcmp((const char *)argv[1], "RSIP_KEY2") == 0) {
		at_sec_rsip_key2(argc, argv);
		return;
	}

	/* When XTS key1 is pending, only RSIP_KEY2 is accepted */
	if (rsip_pending) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "pending key2\n");
		return;
	}

	if (_strcmp((const char *)argv[1], "RSIP_KEY") == 0) {
		at_sec_rsip_key(argc, argv);
	} else if (_strcmp((const char *)argv[1], "RSIP_EN") == 0) {
		at_sec_rsip_en(argc, argv);
	} else if (_strcmp((const char *)argv[1], "RSIP_LOCK") == 0) {
		at_sec_rsip_lock();
	} else if (_strcmp((const char *)argv[1], "RSIP_PROT") == 0) {
		at_sec_rsip_prot(argc, argv);
	} else if (_strcmp((const char *)argv[1], "RDP_KEY") == 0) {
#ifdef CONFIG_RSIP_SINGLE_KEY
		at_sec_rdp_key(argc, argv);
#else
		RTK_LOGS(TAG, RTK_LOG_INFO, "Use RSIP\n");
#endif
	} else if (_strcmp((const char *)argv[1], "RDP_EN") == 0) {
		at_sec_rdp_en(argc, argv);
	} else if (_strcmp((const char *)argv[1], "RDP_PROT") == 0) {
#ifdef CONFIG_RSIP_SINGLE_KEY
		at_sec_rdp_prot(argc, argv);
#else
		RTK_LOGS(TAG, RTK_LOG_INFO, "Use RSIP\n");
#endif
	} else if (_strcmp((const char *)argv[1], "SBOOT_PK") == 0) {
		at_sec_sboot_pk(argc, argv);
	} else if (_strcmp((const char *)argv[1], "SBOOT_EN") == 0) {
		at_sec_sboot_en(argc, argv);
	} else if (_strcmp((const char *)argv[1], "SBOOT_ALG") == 0) {
		at_sec_sboot_alg(argc, argv);
	} else if (_strcmp((const char *)argv[1], "SBOOT_PQC") == 0) {
		at_sec_sboot_pqc(argc, argv);
	} else if (_strcmp((const char *)argv[1], "SBOOT_HMAC") == 0) {
		at_sec_sboot_hmac(argc, argv);
	} else if (_strcmp((const char *)argv[1], "SBOOT_PROT") == 0) {
		at_sec_sboot_prot(argc, argv);
	} else if (_strcmp((const char *)argv[1], "HUK_KEY") == 0) {
		at_sec_huk_key(argc, argv);
	} else if (_strcmp((const char *)argv[1], "HUK_EN") == 0) {
		at_sec_huk_en();
	} else if (_strcmp((const char *)argv[1], "HUK_LOCK") == 0) {
		at_sec_huk_lock();
	} else {
		at_sec_help();
	}
}

/** @brief AT+SEC command registration table (AP-only) */
ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_sec_items[] = {
	{"+SEC", at_secure},
};
#endif /* CONFIG_ATCMD_SECURE */
#endif /* CONFIG_AMEBAD */
