/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"

static const char *const TAG = "ECDSA";

static u8 pri[32] = {0xB0, 0x03, 0x6A, 0xBD, 0x9B, 0x98, 0x45, 0x8E, 0x15, 0x8B, 0x0A, 0xB5, 0x43, 0x6F, 0x2A, 0x0A,
					 0x41, 0x7E, 0xEA, 0x11, 0xB1, 0x36, 0x6B, 0xAB, 0x48, 0xEB, 0x19, 0x97, 0x0A, 0xAE, 0x93, 0xCF
					};
static u8 randk[32] = {0x4, 0x64, 0xD, 0x22, 0x26, 0x87, 0x70, 0x36, 0xD6, 0xC2, 0xFD, 0xA2, 0x16, 0xD4, 0x2C, 0x8A,
					   0xE, 0xB6, 0x1E, 0x5E, 0x53, 0xA6, 0xC6, 0x5D, 0xC2, 0x5C, 0x13, 0xF0, 0xED, 0x67, 0xC4, 0x3D
					  };
static u8 hash[32] = {0xAC, 0x32, 0xF8, 0x19, 0x9E, 0x4C, 0xB5, 0xED, 0x01, 0x11, 0x6B, 0xAD, 0xE6, 0x6B, 0x61, 0x8E,
					  0x2B, 0x53, 0xC6, 0x73, 0xE2, 0xCC, 0x29, 0x7C, 0x17, 0xCF, 0x97, 0xEA, 0x4E, 0xA8, 0x94, 0xDF
					 };

u32 curve = ECDSA_ECP_SECP256K1;


void ecdsa_test(void)
{
	u8 Q[64];
	u8 r[32] = {0};
	u8 s[32] = {0};
	u32 otp_key, ret;

	otp_key = 0;

	RTK_LOGI(TAG, "ECDSA API Demo...\r\n");

	RCC_PeriphClockCmd(APBPeriph_ECDSA, APBPeriph_ECDSA_CLOCK, ENABLE);

	ECDSA_TypeDef *ECDSA = (ECDSA_TypeDef *)ECDSA_REG_BASE;
	ECDSA_EngRst(ECDSA);

	ret = ECDSA_Sign(ECDSA, curve, pri, randk, hash, 32, Q, Q + 32, r, s, otp_key);

	if (ret != 0) {
		RTK_LOGI(TAG, "ecdsa sign failed\r\n");
	} else {
		RTK_LOGI(TAG, "ecdsa sign success\n");
	}

	ret = ECDSA_Verify(ECDSA, curve, hash, 32, Q,  Q + 32, r, s);

	if (ret != 0) {
		RTK_LOGI(TAG, "ecdsa veri failed\r\n");
	} else {
		RTK_LOGI(TAG, "ecdsa veri success\n");
	}

	while (1);
}

int example_raw_ecdsa_test(void)
{
	if (rtos_task_create(NULL, "ecdsa_test", (rtos_task_t)ecdsa_test, NULL, (3072), (1)) != SUCCESS) {
		RTK_LOGI(TAG, "Cannot create ecdsa_test demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}


