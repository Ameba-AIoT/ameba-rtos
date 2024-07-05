/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

static const char hmac_buf[3][128] = {
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ" },
	{ "abcdefghijklmnopqrstuvwxyz" },
	{ "0123456789" }
};
static unsigned char hamc_hash256_swkey_mac[32] = {
	0xD0, 0x5F, 0xD0, 0x82, 0xC7, 0x46, 0x4E, 0x27, 0x7A, 0x8C, 0x43, 0xB9, 0x14, 0x58, 0x86, 0xEC,
	0xA9, 0x44, 0xD6, 0x53, 0xB0, 0xEB, 0x5D, 0xA3, 0x06, 0x6B, 0xF5, 0x57, 0x5A, 0x70, 0x59, 0x40
};

//default efuse key is full 0xff
static unsigned char hamc_hash256_hwkey_mac[32] = {
	0xBA, 0x13, 0x59, 0xA7, 0x25, 0x1C, 0x5C, 0xD2, 0x2C, 0x41, 0xAE, 0x37, 0x16, 0xCC, 0xC4, 0x3F,
	0x18, 0xF7, 0x24, 0xA5, 0xE4, 0x99, 0xF8, 0x06, 0xB7, 0xCC, 0xAE, 0x36, 0x1E, 0xE4, 0x0F, 0x11
};


u8 mac[128] ALIGNMTO(CACHE_LINE_SIZE);

static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
	printf("%s\n", info);
	for (u32 i = 0; i < len; i++) {
		printf("%s0x%02X%s", i % 16 == 0 ? "\n     " : " ",
			   buf[i], i == len - 1 ? "\n" : "");
	}
}


/*
 *  This test_sha256_hmac function is used to test hardware sha256 hmac functoinality
 */
int test_sha256_hmac(u32 OTPkey)
{
	u32 timeout;
	uint8_t mac[32] ALIGNMTO(CACHE_LINE_SIZE);
	char secret[32] ALIGNMTO(0x4) = "abcdefghijklmnopqrstuvwxyz123456";
	hw_sha_context ctx;
	memset(&ctx, 0, sizeof(hw_sha_context));
	timeout = 0xFFF;

	/*take sema to obtain the right to crypto engine*/
	while (IPC_SEMTake(IPC_SEM_CRYPTO, timeout) != _TRUE) {
		printf("ipsec get hw sema fail\n");
	}

	if (OTPkey > 1) {
		CRYPTO_OTPKey_SHA_Init(OTPkey, DISABLE);
	} else {
		CRYPTO_OTPKey_SHA_Init(OTPkey, ENABLE);
	}

	rtl_crypto_hmac_sha2_init(SHA2_256, (u8 *)secret, strlen(secret), &ctx);
	rtl_crypto_hmac_sha2_update((u8 *)hmac_buf[0], strlen(hmac_buf[0]), &ctx);
	rtl_crypto_hmac_sha2_update((u8 *)hmac_buf[1], strlen(hmac_buf[1]), &ctx);
	rtl_crypto_hmac_sha2_update((u8 *)hmac_buf[2], strlen(hmac_buf[2]), &ctx);
	rtl_crypto_hmac_sha2_final(mac, &ctx);

	IPC_SEMFree(IPC_SEM_CRYPTO);

	if (OTPkey > 1) {
		if (_memcmp(hamc_hash256_swkey_mac, mac, 32) == 0) {
			printf("hamc sha256 sw key digest result success\r\n");
		} else {
			printf("hamc sha256 sw key digest result failed\r\n");
			dump_buf("\n  hw hmac-sha-256 mac:", mac, sizeof(mac));
		}
	} else {
		if (_memcmp(hamc_hash256_hwkey_mac, mac, 32) == 0) {
			printf("hamc sha256 otp key digest result success\r\n");
		} else {
			printf("hamc sha256 otp key digest result failed\r\n");
			dump_buf("\n  hw hmac-sha-256 mac:", mac, sizeof(mac));
		}
	}

	return 0;
}



void sha256_hmac(void)
{
	printf("CRYPTO sha256 hmac API Demo...\r\n");

	if (CRYPTO_SHA_Init(NULL) != 0) {
		printf("crypto engine init failed\r\n");
	} else {
		printf("init success\n");
	}

	test_sha256_hmac(2);//sw key
	test_sha256_hmac(0);//otp key1 0x240
	test_sha256_hmac(1);//otp key2 0x260

	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "sha256_hmac", (rtos_task_t)sha256_hmac, NULL, (3072), (1)) != SUCCESS) {
		printf("Cannot create sha256_hmac task\n\r");
	}

	rtos_sched_start();
	return 0;
}


