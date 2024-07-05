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

// vector : AES ECB 256 bit :
static unsigned char aes_test_key[32]  ALIGNMTO(0x4) = {
	0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x00, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11,
	0x39, 0x87, 0x92, 0x66, 0x85, 0x74, 0x65, 0x68, 0x34, 0x92, 0x90, 0x01, 0x83, 0x91, 0x74, 0x98
};

static unsigned char aes_test_ecb_buf[32] = {
	0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
	0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A
};

static unsigned char aes_test_ecb_swkey_enc[32] = {
	0x1C, 0x40, 0x25, 0x93, 0x64, 0x7F, 0x97, 0xB8, 0x46, 0x45, 0x7B, 0x98, 0xC5, 0xAC, 0x74, 0xD6,
	0x1C, 0x40, 0x25, 0x93, 0x64, 0x7F, 0x97, 0xB8, 0x46, 0x45, 0x7B, 0x98, 0xC5, 0xAC, 0x74, 0xD6
};

//default efuse key is full 0xff
static unsigned char aes_test_ecb_hwkey_enc[32] = {
	0x02, 0x4E, 0x9A, 0x6E, 0xE6, 0xE4, 0xAD, 0x25, 0x33, 0x01, 0x7A, 0xBC, 0x56, 0xEB, 0x4C, 0xB7,
	0x02, 0x4E, 0x9A, 0x6E, 0xE6, 0xE4, 0xAD, 0x25, 0x33, 0x01, 0x7A, 0xBC, 0x56, 0xEB, 0x4C, 0xB7
};

u8 cipher_result[128] ALIGNMTO(CACHE_LINE_SIZE);


void dump_buf(char *info, u8 *buf, u32 len)
{
	if (info) {
		printf("%s", info);
	}

	for (u32 i = 0; i < len; i++) {
		printf("%s0x%02X%s", i % 16 == 0 ? "\n     " : " ",
			   buf[i], i == len - 1 ? "\n" : "");
	}
}


int test_aes_ecb(u32 OTPkey)
{
	const u8 *key, *pIv;
	u32 keylen = 0;
	u32 ivlen = 0;
	u8 *message;
	u32 msglen, timeout;
	u8 *pResult;

	key = aes_test_key;
	keylen = 32;
	pIv = NULL;
	ivlen = 0;
	timeout = 0xFFF;
	pResult = cipher_result;
	message = (unsigned char *)aes_test_ecb_buf;
	msglen = sizeof(aes_test_ecb_buf);

	/*take sema to obtain the right to crypto engine*/
	while (IPC_SEMTake(IPC_SEM_CRYPTO, timeout) != _TRUE) {
		printf("ipsec get hw sema fail\n");
	}

	if (OTPkey > 3) {
		CRYPTO_OTPKey_Init(OTPkey, DISABLE);
	} else {
		CRYPTO_OTPKey_Init(OTPkey, ENABLE);
	}

	rtl_crypto_aes_ecb_init(key, keylen);

	rtl_crypto_aes_ecb_encrypt(message, msglen, pIv, ivlen, pResult);

	/*free sema to release the right to crypto engine*/
	IPC_SEMFree(IPC_SEM_CRYPTO);

	if (OTPkey > 3) {

		if (_memcmp(aes_test_ecb_swkey_enc, pResult, msglen) == 0) {
			printf("AES ECB sw key encrypt result success\r\n");
		} else {
			printf("AES ECB sw key encrypt result failed\r\n");
			dump_buf("====encrypt result=====\r\n", pResult, msglen);
		}
	} else {
		if (_memcmp(aes_test_ecb_hwkey_enc, pResult, msglen) == 0) {
			printf("AES ECB otp key encrypt result success\r\n");
		} else {
			printf("AES ECB otp key encrypt result failed\r\n");
			dump_buf("====encrypt result=====\r\n", pResult, msglen);
		}


	}

	message = pResult;

	/*take sema to obtain the right to crypto engine*/
	while (IPC_SEMTake(IPC_SEM_CRYPTO, timeout) != _TRUE) {
		printf("ipsec get hw sema fail\n");
	}

	if (OTPkey > 3) {
		CRYPTO_OTPKey_Init(OTPkey, DISABLE);
	} else {
		CRYPTO_OTPKey_Init(OTPkey, ENABLE);
	}

	rtl_crypto_aes_ecb_decrypt(message, msglen, pIv, ivlen, pResult);

	/*free sema to release the right to crypto engine*/
	IPC_SEMFree(IPC_SEM_CRYPTO);

	if (_memcmp(aes_test_ecb_buf, pResult, msglen) == 0) {
		printf("AES ECB decrypt result success\r\n");
	} else {
		printf("AES ECB decrypt result failed\r\n");
		dump_buf("====decrypt result=====\r\n", pResult, msglen);
	}

	return 0;
}

void aes_test(void)
{
	printf("CRYPTO API Demo...\r\n");

	if (CRYPTO_Init(NULL) != 0) {
		printf("crypto engine init failed\r\n");
	} else {
		printf("init success\n");
	}
	printf("******************************sw key***********************\r\n");
	test_aes_ecb(4);
	printf("******************************OTP key ********************\r\n");
	test_aes_ecb(2);//rsip key 1 0x2c0



	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "aes_test", (rtos_task_t)aes_test, NULL, (3072), (1)) != SUCCESS) {
		printf("Cannot create aes_md5_test demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}


