/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_SECURE_H__
#define __ATCMD_SECURE_H__

#include "platform_autoconf.h"

#ifdef CONFIG_ATCMD_SECURE
/* ===== OTP Physical Addresses (Key area) ===== */

/* RSIP Key Group 0: ECB key (0x2C0) + CTR key (0x2E0), each 32B */
#define OTP_RSIP_KEY1_ADDR		0x2C0	/* ECB key, 32B */
#define OTP_RSIP_KEY2_ADDR		0x2E0	/* CTR key, 32B */
#define OTP_RSIP_KEY1_LEN		32
#define OTP_RSIP_KEY2_LEN		32

/* RSIP Key Group 1 (Default group only, not CONFIG_RSIP_SINGLE_KEY) */
#define OTP_RSIP_KEY3_ADDR		0x200	/* ECB key, 32B */
#define OTP_RSIP_KEY4_ADDR		0x220	/* CTR key, 32B */
#define OTP_RSIP_KEY3_LEN		32
#define OTP_RSIP_KEY4_LEN		32

/* RDP Key: Legacy group uses IPSEC-S independent key at 0x200 (same as RSIP Key3) */
#define OTP_RDP_KEY_ADDR		0x200
#define OTP_RDP_KEY_LEN			32

/* Secure Boot public key hash: SHA-256 of ECC/PQC public key, each 32B */
#define OTP_SBOOT_PK1_ADDR		0x320	/* PK1 hash, 32B */
#define OTP_SBOOT_PK2_ADDR		0x340	/* PK2 hash, 32B */
#define OTP_SBOOT_PK_LEN		32

/* SBoot HMAC key (shared with RSIP Key4/S_IPSEC_Key2 at 0x220), 32B */
#define OTP_SBOOT_HMAC_ADDR		0x220
#define OTP_SBOOT_HMAC_LEN		32

/* HUK: 128-bit hardware unique key seed */
#define OTP_HUK_ADDR			0x310	/* HUK seed, 16B */
#define OTP_HUK_LEN			16

/* ===== OTP Control/Status Registers (Physical) ===== */

/* HUK write protect, Physical 0x364
 * [7] HUK_W_Forbidden: 0=lock write, default 1 */
#define OTP_HUK_CTRL			0x364

/* Key/PK protection, Physical 0x365
 * [1] PK1_W_Forbidden: 0=lock, default 1
 * [2] PK2_W_Forbidden: 0=lock, default 1
 * [3] RSIP Key3 R / S_IPSEC_Key1 R (Legacy: RDP key R), 0=protect, default 1
 * [4] RSIP Key3 W / S_IPSEC_Key1 W (Legacy: RDP key W), 0=protect, default 1
 * [5] RSIP Key4 R / S_IPSEC_Key2 R (Legacy: HMAC key R), 0=protect, default 1
 * [6] RSIP Key4 W / S_IPSEC_Key2 W (Legacy: HMAC key W), 0=protect, default 1 */
#define OTP_KEY_PROT			0x365

/* RSIP Key1 R protect, Physical 0x366
 * [7] RSIP_KEY1_R_Protection_EN, 0=protect, default 1 */
#define OTP_RSIP_KEY1_R_CTRL		0x366

/* RSIP Key1 W / Key2 RW / Mode lock, Physical 0x367
 * [0] RSIP_KEY1_W_Forbidden, 0=lock, default 1
 * [1] RSIP_KEY2_R_Protection, 0=protect, default 1
 * [2] RSIP_KEY2_W_Forbidden, 0=lock, default 1
 * [3] RSIP_MODE_W_Forbidden, 0=lock, default 1 */
#define OTP_RSIP_KEY_CTRL		0x367

/* Security enable, Physical 0x368
 * [1] PQC_EN, 0=enable, default 1 (RTL8720F only)
 * [2] RSIP_EN, 0=enable, default 1
 * [3] SECURE_BOOT_EN, 0=enable, default 1
 * [4] Legacy: HW_DIS / Default: BOOT_RAND_DELAY_EN, 0=enable, default 1
 * [5] RDP_EN, 0=enable, default 1 */
#define OTP_SEC_EN			0x368

/* RSIP mode + HUK deriv, Physical 0x369
 * [1:0] RSIP_MODE: 00/11=XTS, 01=CTR, 10=GCM
 * [2] HUK_DERIV_EN, 0=enable, default 1 */
#define OTP_RSIP_MODE_CTRL		0x369

/* Secure Boot algorithm, Physical 0x36B
 * [3:0] SBOOT_AUTH_ALG (ed25519=0, secp256r1=3, ml_dsa_65=0xF1, ...)
 * [7:4] SBOOT_HASH_ALG (sha256=0, sha384=1, hmac256=3, ...) */
#define OTP_SBOOT_ALG			0x36B

/* Logical enable byte: logical 0x3
 * [2] SECURE_BOOT_EN, 1=enable, default 0
 * [3] RSIP_EN, 1=enable, default 0
 * [4] RDP_EN, 1=enable, default 0
 * [5] PQC_EN, 1=enable, default 0 (RTL8720F only) */
#define OTP_LOG_EN_ADDR			0x03

/* ===== Bit Positions ===== */

/* RSIP_EN: logical 0x3[3], physical 0x368[2] */
#define RSIP_EN_LOG_BIT			3
#define RSIP_EN_PHY_BIT			2

/* RDP_EN: logical 0x3[4], physical 0x368[5] */
#define RDP_EN_LOG_BIT			4
#define RDP_EN_PHY_BIT			5

/* SECURE_BOOT_EN: logical 0x3[2], physical 0x368[3] */
#define SBOOT_EN_LOG_BIT		2
#define SBOOT_EN_PHY_BIT		3

/* PQC_EN: logical 0x3[5], physical 0x368[1] (RTL8720F only) */
#define PQC_EN_LOG_BIT			5
#define PQC_EN_PHY_BIT			1

/* RSIP_MODE: physical 0x369[1:0] */
#define RSIP_MODE_BIT_MASK		0x03
#define RSIP_MODE_W_LOCK_BIT		3	/* 0x367[3] */

/* RSIP Key Protection: grp0 */
#define RSIP_KEY1_R_PROT_BIT		7	/* 0x366[7] */
#define RSIP_KEY1_W_PROT_BIT		0	/* 0x367[0] */
#define RSIP_KEY2_R_PROT_BIT		1	/* 0x367[1] */
#define RSIP_KEY2_W_PROT_BIT		2	/* 0x367[2] */

/* RSIP Key Protection: grp1 (Default group only) */
#define RSIP_KEY3_R_PROT_BIT		3	/* 0x365[3] */
#define RSIP_KEY3_W_PROT_BIT		4	/* 0x365[4] */
#define RSIP_KEY4_R_PROT_BIT		5	/* 0x365[5] */
#define RSIP_KEY4_W_PROT_BIT		6	/* 0x365[6] */

/* RDP Key Protection (Legacy group): same addr 0x365 as Key3 prot bits */
#define RDP_KEY_R_PROT_BIT		3	/* 0x365[3] */
#define RDP_KEY_W_PROT_BIT		4	/* 0x365[4] */

/* SBoot PK write protect */
#define PK1_W_PROT_BIT			1	/* 0x365[1]: PK1 write protect, 0=lock */
#define PK2_W_PROT_BIT			2	/* 0x365[2]: PK2 write protect, 0=lock */

/* HUK control bits */
#define HUK_LOCK_BIT			7	/* 0x364[7]: HUK write protect, 0=lock */
#define HUK_DERIV_EN_BIT		2	/* 0x369[2]: HUK derive enable, 0=enable */

/* SBoot ALG register masks */
#define SBOOT_AUTH_MASK			0x0F	/* 0x36B[3:0] */
#define SBOOT_HASH_SHIFT		4	/* 0x36B[7:4] */

/* ===== RSIP Mode Values ===== */
#define RSIP_MODE_XTS			0
#define RSIP_MODE_CTR			1
#define RSIP_MODE_GCM			2

/* ===== Helper functions ===== */

void at_secure(u16 argc, char **argv);

#endif /* CONFIG_ATCMD_SECURE */
#endif /* __ATCMD_SECURE_H__ */
