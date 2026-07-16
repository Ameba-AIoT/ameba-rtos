/*
 * AES (Rijndael) cipher - decrypt
 *
 * Modifications to public domain implementation:
 * - cleanup
 * - use C pre-processor to make it easier to change S table access
 * - added option (AES_SMALL_TABLES) for reducing code size by about 8 kB at
 *   cost of reduced throughput (quite small difference on Pentium 4,
 *   10-25% when using -O1 or -O2 optimization)
 *
 * Copyright (c) 2003-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

//#ifdef CONFIG_WPS
#include "utils/includes.h"
#include "utils/common.h"
#include "crypto/crypto.h"
#include "rom_wps_aes_i.h"

/**
 * Expand the cipher key into the decryption key schedule.
 *
 * @return	the number of rounds for the given cipher key size.
 */
//static int rijndaelKeySetupDec(u32 rk[], const u8 cipherKey[], int keyBits)
WLAN_ROM_TEXT_SECTION
static void rijndaelKeySetupDec(u32 rk[/*44*/], const u8 cipherKey[])
{

	int Nr = 10, i, j;
	u32 temp;

	/* expand the cipher key: */
	rom_rijndaelKeySetupEnc(rk, cipherKey);
	/* invert the order of the round keys: */
	for (i = 0, j = 4 * Nr; i < j; i += 4, j -= 4) {
		temp = rk[i    ];
		rk[i    ] = rk[j    ];
		rk[j    ] = temp;
		temp = rk[i + 1];
		rk[i + 1] = rk[j + 1];
		rk[j + 1] = temp;
		temp = rk[i + 2];
		rk[i + 2] = rk[j + 2];
		rk[j + 2] = temp;
		temp = rk[i + 3];
		rk[i + 3] = rk[j + 3];
		rk[j + 3] = temp;
	}
	/* apply the inverse MixColumn transform to all round keys but the
	 * first and the last: */
	for (i = 1; i < Nr; i++) {
		rk += 4;
		for (j = 0; j < 4; j++) {
			rk[j] = WPS_TD0_(WPS_TE4((rk[j] >> 24))) ^
					WPS_TD1_(WPS_TE4((rk[j] >> 16) & 0xff)) ^
					WPS_TD2_(WPS_TE4((rk[j] >>  8) & 0xff)) ^
					WPS_TD3_(WPS_TE4((rk[j]) & 0xff));
		}
	}
	/*
	int Nr, i, j;
	u32 temp;

	// expand the cipher key:
	Nr = rijndaelKeySetupEnc(rk, cipherKey, keyBits);
	if (Nr < 0)
		return Nr;
	// invert the order of the round keys:
	for (i = 0, j = 4*Nr; i < j; i += 4, j -= 4) {
		temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
		temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
		temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
		temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
	}
	// apply the inverse MixColumn transform to all round keys but the
	// first and the last:
	for (i = 1; i < Nr; i++) {
		rk += 4;
		for (j = 0; j < 4; j++) {
			rk[j] = TD0_(TE4((rk[j] >> 24)       )) ^
				TD1_(TE4((rk[j] >> 16) & 0xff)) ^
				TD2_(TE4((rk[j] >>  8) & 0xff)) ^
				TD3_(TE4((rk[j]      ) & 0xff));
		}
	}

	return Nr;
	*/
}

//void *aes_decrypt_init(const u8 *key, size_t len)
WLAN_ROM_TEXT_SECTION
void *rom_aes_decrypt_init(unsigned char *mem, const u8 *key, size_t len)
{

	u32 *rk;
	if (len != 16) {
		return NULL;
	}
	//rk = (u32 *)os_malloc(WPS_AES_PRIV_SIZE);
	rk = (u32 *) mem;

	if (rk == NULL) {
		return NULL;
	}
	rijndaelKeySetupDec(rk, key);
	return rk;
	/*
	u32 *rk;
	int res;
	rk = (u32 *)os_malloc(AES_PRIV_SIZE);//244
	if (rk == NULL)
		return NULL;
	res = rijndaelKeySetupDec(rk, key, len * 8);//128
	if (res < 0) {
		os_free(rk,0);
		return NULL;
	}
	rk[WPS_AES_PRIV_NR_POS] = res;
	return rk;
	*/
}

//static void rijndaelDecrypt(const u32 rk[/*44*/], int Nr, const u8 ct[16],u8 pt[16])
WLAN_ROM_TEXT_SECTION
static void rijndaelDecrypt(const u32 rk[/*44*/], const u8 ct[16], u8 pt[16])
{

	u32 s0, s1, s2, s3, t0, t1, t2, t3;
	const int Nr = 10;
#ifndef FULL_UNROLL
	int r;
#endif /* ?FULL_UNROLL */

	/*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	s0 = GETU32(ct) ^ rk[0];
	s1 = GETU32(ct +  4) ^ rk[1];
	s2 = GETU32(ct +  8) ^ rk[2];
	s3 = GETU32(ct + 12) ^ rk[3];

#define ROUND(i,d,s) \
d##0 = WPS_TD0(s##0) ^ WPS_TD1(s##3) ^ WPS_TD2(s##2) ^ WPS_TD3(s##1) ^ rk[4 * i]; \
d##1 = WPS_TD0(s##1) ^ WPS_TD1(s##0) ^ WPS_TD2(s##3) ^ WPS_TD3(s##2) ^ rk[4 * i + 1]; \
d##2 = WPS_TD0(s##2) ^ WPS_TD1(s##1) ^ WPS_TD2(s##0) ^ WPS_TD3(s##3) ^ rk[4 * i + 2]; \
d##3 = WPS_TD0(s##3) ^ WPS_TD1(s##2) ^ WPS_TD2(s##1) ^ WPS_TD3(s##0) ^ rk[4 * i + 3]

#ifdef FULL_UNROLL

	ROUND(1, t, s);
	ROUND(2, s, t);
	ROUND(3, t, s);
	ROUND(4, s, t);
	ROUND(5, t, s);
	ROUND(6, s, t);
	ROUND(7, t, s);
	ROUND(8, s, t);
	ROUND(9, t, s);

	rk += Nr << 2;

#else  /* !FULL_UNROLL */

	/* Nr - 1 full rounds: */
	r = Nr >> 1;
	for (;;) {
		ROUND(1, t, s);
		rk += 8;
		if (--r == 0) {
			break;
		}
		ROUND(0, s, t);
	}

#endif /* ?FULL_UNROLL */

#undef ROUND

	/*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
	s0 = WPS_TD41(t0) ^ WPS_TD42(t3) ^ WPS_TD43(t2) ^ WPS_TD44(t1) ^ rk[0];
	PUTU32(pt, s0);
	s1 = WPS_TD41(t1) ^ WPS_TD42(t0) ^ WPS_TD43(t3) ^ WPS_TD44(t2) ^ rk[1];
	PUTU32(pt +  4, s1);
	s2 = WPS_TD41(t2) ^ WPS_TD42(t1) ^ WPS_TD43(t0) ^ WPS_TD44(t3) ^ rk[2];
	PUTU32(pt +  8, s2);
	s3 = WPS_TD41(t3) ^ WPS_TD42(t2) ^ WPS_TD43(t1) ^ WPS_TD44(t0) ^ rk[3];
	PUTU32(pt + 12, s3);

	/*
		u32 s0, s1, s2, s3, t0, t1, t2, t3;
	#ifndef FULL_UNROLL
		int r;
	#endif // ?FULL_UNROLL

		 //
		 // map byte array block to cipher state
		 // and add initial round key:
		 //
		s0 = GETU32(ct     ) ^ rk[0];
		s1 = GETU32(ct +  4) ^ rk[1];
		s2 = GETU32(ct +  8) ^ rk[2];
		s3 = GETU32(ct + 12) ^ rk[3];

	#define ROUND(i,d,s) \
	d##0 = TD0(s##0) ^ TD1(s##3) ^ TD2(s##2) ^ TD3(s##1) ^ rk[4 * i]; \
	d##1 = TD0(s##1) ^ TD1(s##0) ^ TD2(s##3) ^ TD3(s##2) ^ rk[4 * i + 1]; \
	d##2 = TD0(s##2) ^ TD1(s##1) ^ TD2(s##0) ^ TD3(s##3) ^ rk[4 * i + 2]; \
	d##3 = TD0(s##3) ^ TD1(s##2) ^ TD2(s##1) ^ TD3(s##0) ^ rk[4 * i + 3]

	#ifdef FULL_UNROLL

		ROUND(1,t,s);
		ROUND(2,s,t);
		ROUND(3,t,s);
		ROUND(4,s,t);
		ROUND(5,t,s);
		ROUND(6,s,t);
		ROUND(7,t,s);
		ROUND(8,s,t);
		ROUND(9,t,s);
		if (Nr > 10) {
			ROUND(10,s,t);
			ROUND(11,t,s);
			if (Nr > 12) {
				ROUND(12,s,t);
				ROUND(13,t,s);
			}
		}

		rk += Nr << 2;

	#else  // !FULL_UNROLL

		// Nr - 1 full rounds:
		r = Nr >> 1;
		for (;;) {
			ROUND(1,t,s);
			rk += 8;
			if (--r == 0)
				break;
			ROUND(0,s,t);
		}

	#endif // ?FULL_UNROLL

	#undef ROUND

		 //
		 // apply last round and
		 // map cipher state to byte array block:
		 //
		s0 = TD41(t0) ^ TD42(t3) ^ TD43(t2) ^ TD44(t1) ^ rk[0];
		PUTU32(pt     , s0);
		s1 = TD41(t1) ^ TD42(t0) ^ TD43(t3) ^ TD44(t2) ^ rk[1];
		PUTU32(pt +  4, s1);
		s2 = TD41(t2) ^ TD42(t1) ^ TD43(t0) ^ TD44(t3) ^ rk[2];
		PUTU32(pt +  8, s2);
		s3 = TD41(t3) ^ TD42(t2) ^ TD43(t1) ^ TD44(t0) ^ rk[3];
		PUTU32(pt + 12, s3);
	*/
}

WLAN_ROM_TEXT_SECTION
void rom_aes_internal_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
	rijndaelDecrypt(ctx, crypt, plain);
	/*
	u32 *rk = ctx;
	rijndaelDecrypt(ctx, rk[WPS_AES_PRIV_NR_POS], crypt, plain);
	*/
}

WLAN_ROM_TEXT_SECTION
void rom_aes_decrypt_deinit(void *ctx)
{
	memset(ctx, 0, WPS_AES_PRIV_SIZE);
	//os_free(ctx,0);
}
//#endif
