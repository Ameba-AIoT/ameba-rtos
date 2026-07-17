/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This is ROM code section.
 *
 *
 ******************************************************************************/
#ifndef __ROM_RTW_SECURITY_H_
#define __ROM_RTW_SECURITY_H_

#define ROL32( A, n )	( ((A) << (n)) | ( ((A)>>(32-(n)))  & ( (1UL << (n)) - 1 ) ) )
#define ROR32( A, n ) 	ROL32( (A), 32-(n) )

#define WPA_GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))

#define WPA_PUT_LE16(a, val)			\
	do {					\
		(a)[1] = ((u16) (val)) >> 8;	\
		(a)[0] = ((u16) (val)) & 0xff;	\
	} while (0)

#define WPA_PUT_BE32(a, val)					\
	do {							\
		(a)[0] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[3] = (u8) (((u32) (val)) & 0xff);		\
	} while (0)

#define WPA_GET_BE64(a) ((((u64) (a)[0]) << 56) | (((u64) (a)[1]) << 48) | \
			 (((u64) (a)[2]) << 40) | (((u64) (a)[3]) << 32) | \
			 (((u64) (a)[4]) << 24) | (((u64) (a)[5]) << 16) | \
			 (((u64) (a)[6]) << 8) | ((u64) (a)[7]))


#define WPA_PUT_BE64(a, val)				\
	do {						\
		(a)[0] = (u8) (((u64) (val)) >> 56);	\
		(a)[1] = (u8) (((u64) (val)) >> 48);	\
		(a)[2] = (u8) (((u64) (val)) >> 40);	\
		(a)[3] = (u8) (((u64) (val)) >> 32);	\
		(a)[4] = (u8) (((u64) (val)) >> 24);	\
		(a)[5] = (u8) (((u64) (val)) >> 16);	\
		(a)[6] = (u8) (((u64) (val)) >> 8);	\
		(a)[7] = (u8) (((u64) (val)) & 0xff);	\
	} while (0)

#define SHA512_BLOCK_SIZE 128

/* This is based on SHA256 implementation in LibTomCrypt that was released into
 * public domain by Tom St Denis. */

/* Various logical functions */
#define RORc(x, y) \
( ((((unsigned long) (x) & 0xFFFFFFFFUL) >> (unsigned long) ((y) & 31)) | \
   ((unsigned long) (x) << (unsigned long) (32 - ((y) & 31)))) & 0xFFFFFFFFUL)
#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define S(x, n)         RORc((x), (n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define ROR64c(x, y) \
    ( ((((x) & UINT64_C(0xFFFFFFFFFFFFFFFF)) >> ((u64) (y) & UINT64_C(63))) | \
      ((x) << ((u64) (64 - ((y) & UINT64_C(63)))))) & \
      UINT64_C(0xFFFFFFFFFFFFFFFF))

#define S_64(x, n)         ROR64c(x, n)
#define R_64(x, n)         (((x) & UINT64_C(0xFFFFFFFFFFFFFFFF)) >> ((u64) n))
#define Sigma0_64(x)       (S_64(x, 28) ^ S_64(x, 34) ^ S_64(x, 39))
#define Sigma1_64(x)       (S_64(x, 14) ^ S_64(x, 18) ^ S_64(x, 41))
#define Gamma0_64(x)       (S_64(x, 1) ^ S_64(x, 8) ^ R_64(x, 7))
#define Gamma1_64(x)       (S_64(x, 19) ^ S_64(x, 61) ^ R_64(x, 6))


#define GET_TKIP_PN(iv,dot11txpn)\
do{\
	dot11txpn._byte_.TSC0=iv[2];\
	dot11txpn._byte_.TSC1=iv[0];\
	dot11txpn._byte_.TSC2=iv[4];\
	dot11txpn._byte_.TSC3=iv[5];\
	dot11txpn._byte_.TSC4=iv[6];\
	dot11txpn._byte_.TSC5=iv[7];\
}while(0)

struct sha256_state {
	u64 length;
	u32 state[8], curlen;
	u8 buf[64];
};

struct sha384_state {
	u64 length, state[8];
	u32 curlen;
	u8 buf[SHA512_BLOCK_SIZE];
};

struct sha512_state {
	u64 length, state[8];
	u32 curlen;
	u8 buf[SHA512_BLOCK_SIZE];
};

struct mic_data {
	u32  K0, K1;         // Key
	u32  L, R;           // Current state
	u32  M;              // Message accumulator (single word)
	u32     nBytesInM;      // # bytes in M
};

union  u_crc {
	unsigned char	ch[4];
	int i;
};

//===============================
// WEP related
//===============================
void wep_80211_encrypt(
	u8 *pframe, u32 wlan_hdr_len, \
	u32 iv_len, u32 payload_len, \
	u8 *key, u32 key_len);

u8 wep_80211_decrypt(
	u8 *pframe, u32 wlan_hdr_len,
	u32 iv_len, u32 payload_len,
	u8 *key, u32 key_len,
	union u_crc *pcrc\
);

//===============================
// TKIP related
//===============================
void tkip_80211_encrypt(
	u8 *pframe, u32 wlan_hdr_len, \
	u32 iv_len, u32 payload_len, \
	u8 *key, u32 key_len, \
	u8 *ta);

u8 tkip_80211_decrypt(
	u8 *pframe, u32 wlan_hdr_len, \
	u32 iv_len, u32 payload_len, \
	u8 *key, u32 key_len, \
	u8 *ta, union u_crc *pcrc);

void tkip_micappendbyte(struct mic_data *pmicdata, u8 b);
void rtw_secmicsetkey(struct mic_data *pmicdata, u8 *key);
void rtw_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nbytes);
void rtw_secgetmic(struct mic_data *pmicdata, u8 *dst);
void rtw_seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len, u8 *mic_code, u8 pri);
void tkip_phase1(u16 *p1k, const u8 *tk, const u8 *ta, u32 iv32);
void tkip_phase2(u8 *rc4key, const u8 *tk, const u16 *p1k, u16 iv16);

//===============================
// AES related
//===============================
void aes1_encrypt(u8 *key, u8 *data, u8 *ciphertext);
void aesccmp_construct_mic_iv(u8 *mic_iv, sint qc_exists, sint a4_exists, u8 *mpdu, uint payload_length, u8 *pn_vector, uint frtype);
void aesccmp_construct_mic_header1(u8 *mic_header1, sint header_length, u8 *mpdu, uint frtype);
void aesccmp_construct_mic_header2(u8 *mic_header2, u8 *mpdu, sint a4_exists, sint qc_exists);
void aesccmp_construct_ctr_preload(u8 *ctr_preload, sint a4_exists, sint qc_exists, u8 *mpdu, u8 *pn_vector, sint c, uint frtype);
u32 aes_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 frame_type);
u32 aes_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 frame_type);

#endif	//__ROM_RTW_SECURITY_H_

