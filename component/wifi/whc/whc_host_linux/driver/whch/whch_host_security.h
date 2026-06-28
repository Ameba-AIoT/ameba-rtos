#ifndef __WHC_HOST_SECURITY_H__
#define __WHC_HOST_SECURITY_H__

#define ROL32(A, n)	(((A) << (n)) | (((A)>>(32-(n)))  & ((1UL << (n)) - 1)))
#define ROR32(A, n)	ROL32((A), 32-(n))

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

#define CCMPH_2_PN(ch)	(((ch) & 0x000000000000ffff) \
			| (((ch) & 0xffffffff00000000) >> 16))
#define PN_LESS_CHK(a, b)	(((a-b) & 0x800000000000) != 0)
#define VALID_PN_CHK(new, old)	(((old) == 0) || PN_LESS_CHK(old, new))
#define CCMPH_2_KEYID(ch)	(((ch) & 0x00000000c0000000) >> 30)

struct mic_data {
	u32  K0, K1;         // Key
	u32  L, R;           // Current state
	u32  M;              // Message accumulator (single word)
	u32  nBytesInM;      // # bytes in M
};

union u_crc {
	unsigned char	ch[4];
	int		i;
};

struct arc4context {
	u32 x;
	u32 y;
	u8 state[256];
};

#define GET_TKIP_PN(iv, dot11txpn)			\
	do {						\
		dot11txpn._byte_.TSC0 = iv[2];		\
		dot11txpn._byte_.TSC1 = iv[0];		\
		dot11txpn._byte_.TSC2 = iv[4];		\
		dot11txpn._byte_.TSC3 = iv[5];		\
		dot11txpn._byte_.TSC4 = iv[6];		\
		dot11txpn._byte_.TSC5 = iv[7];		\
	} while (0)

u32 crc32_get(u8 *buf, int len);
void rt_arc4_init(struct arc4context *parc4ctx, u8 *key, u32 key_len);
void rt_arc4_crypt(struct arc4context *parc4ctx, u8 *dest, u8 *src, u32 len);

struct whch_security_priv {
	union Keytype_32	dot11_wpa_grpkey[1];	// 802.1x Group Key, for inx0 and inx1
	union Keytype	dot11_tkip_grpmickey_tx[1];
	union Keytype	dot11_tkip_grpmickey_rx[1];

	u32             dot11_wpa_mode;	// NDIS_802_11_AUTHENTICATION_MODE
	u32             dot11PrivacyAlgrthm;	// This specify the privacy for shared auth. algorithm.
	u32             dot118021XGrpPrivacy;	// This specify the privacy algthm. used for Grp key

	u8              dot11_wep_key_index;	// this is only valid for legendary wep, 0~3 for key id. (tx key index)
	u8              dot11_wpa_grpkey_index; 	// key id used for Grp Key ( tx key index)

	u8              b_usetkipkey: 1;
	u8              b_sw_encrypt: 1;
	u8              b_installGrpkey: 1;
	u8              b_hw_decrypted: 1;
	union Keytype   dot11_wep_key[4];
	u8              dot11_wep_keylen[4];

	u8				iv_seq[4][8];
};

void whc_host_secmicsetkey(struct mic_data *pmicdata, u8 *key);
void whc_host_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nbytes);
void whc_host_secgetmic(struct mic_data *pmicdata, u8 *dst);
void whc_host_seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len, u8 *mic_code, u8 pri);

u8 wep_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len,
					 u8 *key, u32 key_len, union u_crc *pcrc);
u8 tkip_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len,
					  u8 *key, u32 key_len, u8 *ta, union u_crc *pcrc);
u32 aes_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 frame_type);
u32 gcmp_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 frame_len, u8 *key, u32 key_len);

void wep_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len, u8 *key, u32 key_len);
void tkip_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len, u8 *key, u32 key_len, u8 *ta);
u32 aes_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 frame_type);
u32 gcmp_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 key_len);
#endif /* __WHC_HOST_SECURITY_H__ */
