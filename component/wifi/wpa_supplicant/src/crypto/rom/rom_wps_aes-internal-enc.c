/*
 * AES (Rijndael) cipher - encrypt
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

//static void rijndaelEncrypt(const u32 rk[], int Nr, const u8 pt[16], u8 ct[16])
WLAN_ROM_TEXT_SECTION
static void rijndaelEncrypt(const u32 rk[/*44*/], const u8 pt[16], u8 ct[16])
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
	s0 = GETU32(pt) ^ rk[0];
	s1 = GETU32(pt +  4) ^ rk[1];
	s2 = GETU32(pt +  8) ^ rk[2];
	s3 = GETU32(pt + 12) ^ rk[3];

#define ROUND(i,d,s) \
d##0 = WPS_TE0(s##0) ^ WPS_TE1(s##1) ^ WPS_TE2(s##2) ^ WPS_TE3(s##3) ^ rk[4 * i]; \
d##1 = WPS_TE0(s##1) ^ WPS_TE1(s##2) ^ WPS_TE2(s##3) ^ WPS_TE3(s##0) ^ rk[4 * i + 1]; \
d##2 = WPS_TE0(s##2) ^ WPS_TE1(s##3) ^ WPS_TE2(s##0) ^ WPS_TE3(s##1) ^ rk[4 * i + 2]; \
d##3 = WPS_TE0(s##3) ^ WPS_TE1(s##0) ^ WPS_TE2(s##1) ^ WPS_TE3(s##2) ^ rk[4 * i + 3]

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
	s0 = WPS_TE41(t0) ^ WPS_TE42(t1) ^ WPS_TE43(t2) ^ WPS_TE44(t3) ^ rk[0];
	PUTU32(ct, s0);
	s1 = WPS_TE41(t1) ^ WPS_TE42(t2) ^ WPS_TE43(t3) ^ WPS_TE44(t0) ^ rk[1];
	PUTU32(ct +  4, s1);
	s2 = WPS_TE41(t2) ^ WPS_TE42(t3) ^ WPS_TE43(t0) ^ WPS_TE44(t1) ^ rk[2];
	PUTU32(ct +  8, s2);
	s3 = WPS_TE41(t3) ^ WPS_TE42(t0) ^ WPS_TE43(t1) ^ WPS_TE44(t2) ^ rk[3];
	PUTU32(ct + 12, s3);
	/*
		u32 s0, s1, s2, s3, t0, t1, t2, t3;
	#ifndef FULL_UNROLL
		int r;
	#endif // ?FULL_UNROLL

		 //
		 // map byte array block to cipher state
		 // and add initial round key:
		 //
		s0 = GETU32(pt     ) ^ rk[0];
		s1 = GETU32(pt +  4) ^ rk[1];
		s2 = GETU32(pt +  8) ^ rk[2];
		s3 = GETU32(pt + 12) ^ rk[3];

	#define ROUND(i,d,s) \
	d##0 = TE0(s##0) ^ TE1(s##1) ^ TE2(s##2) ^ TE3(s##3) ^ rk[4 * i]; \
	d##1 = TE0(s##1) ^ TE1(s##2) ^ TE2(s##3) ^ TE3(s##0) ^ rk[4 * i + 1]; \
	d##2 = TE0(s##2) ^ TE1(s##3) ^ TE2(s##0) ^ TE3(s##1) ^ rk[4 * i + 2]; \
	d##3 = TE0(s##3) ^ TE1(s##0) ^ TE2(s##1) ^ TE3(s##2) ^ rk[4 * i + 3]

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
		s0 = TE41(t0) ^ TE42(t1) ^ TE43(t2) ^ TE44(t3) ^ rk[0];
		PUTU32(ct     , s0);
		s1 = TE41(t1) ^ TE42(t2) ^ TE43(t3) ^ TE44(t0) ^ rk[1];
		PUTU32(ct +  4, s1);
		s2 = TE41(t2) ^ TE42(t3) ^ TE43(t0) ^ TE44(t1) ^ rk[2];
		PUTU32(ct +  8, s2);
		s3 = TE41(t3) ^ TE42(t0) ^ TE43(t1) ^ TE44(t2) ^ rk[3];
		PUTU32(ct + 12, s3);
	*/
}

//void *aes_encrypt_init(const u8 *key, size_t len)
WLAN_ROM_TEXT_SECTION
void *rom_aes_encrypt_init(unsigned char *mem, const u8 *key, size_t len)
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
	rom_rijndaelKeySetupEnc(rk, key);
	return rk;
	/*
	u32 *rk;
	int res;
	rk = (u32 *)os_malloc(AES_PRIV_SIZE);
	if (rk == NULL)
		return NULL;
	res = rijndaelKeySetupEnc(rk, key, len * 8);
	if (res < 0) {
		os_free(rk,0);
		return NULL;
	}
	rk[WPS_AES_PRIV_NR_POS] = res;
	return rk;
	*/
}

WLAN_ROM_TEXT_SECTION
void rom_aes_internal_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
	rijndaelEncrypt(ctx, plain, crypt);
	/*
	u32 *rk = ctx;
	rijndaelEncrypt(ctx, rk[WPS_AES_PRIV_NR_POS], plain, crypt);
	*/
}

WLAN_ROM_TEXT_SECTION
void rom_aes_encrypt_deinit(void *ctx)
{
	memset(ctx, 0, WPS_AES_PRIV_SIZE);
	//os_free(ctx,0);
}
//#endif
