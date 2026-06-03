#ifndef __WHCH_HOST_80211SPEC_MACRO_H__
#define __WHCH_HOST_80211SPEC_MACRO_H__

#define _TO_DS_		BIT(8)
#define _FROM_DS_	BIT(9)
#define _RETRY_		BIT(11)
#define _PWRMGT_	BIT(12)
#define _MORE_DATA_	BIT(13)
#define _PRIVACY_	BIT(14)
#define _ORDER_		BIT(15)

#define SetToDs(pbuf)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16(_TO_DS_); \
	} while(0)

#define SetFrDs(pbuf)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16(_FROM_DS_); \
	} while(0)

#define SetMData(pbuf)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16(_MORE_DATA_); \
	} while(0)

#define SetPrivacy(pbuf)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16(_PRIVACY_); \
	} while(0)

#define SetOrder(pbuf)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16(_ORDER_); \
	} while (0)

#define SetSeqNum(pbuf, num) \
	do {    \
		*(unsigned short *)(void*)((size_t)(pbuf) + 22) = \
			((*(unsigned short *)(void*)((size_t)(pbuf) + 22)) & le16_to_cpu((unsigned short)~0xfff0)) | \
			le16_to_cpu((unsigned short)(0xfff0 & (num << 4))); \
	} while(0)

#define SetPriority(pbuf, tid)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) &= ~0x000f; \
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16(tid & 0xf); \
	} while(0)

#define SetEOSP(pbuf, eosp)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16( (eosp & 1) << 4); \
	} while(0)

#define SetAckpolicy(pbuf, ack)	\
	do	{	\
		*(unsigned short *)(void*)(pbuf) |= cpu_to_le16( (ack & 3) << 5); \
	} while(0)

#define GetOrder(pbuf)	(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_ORDER_)) != 0)
#define GetPriority(pbuf)	((le16_to_cpu(*(unsigned short *)(void*)(pbuf))) & 0xf)

#define GetAddr1Ptr(pbuf)	((unsigned char *)((size_t)(pbuf) + 4))

#define get_frame_sub_type(pbuf)	(cpu_to_le16(*(unsigned short *)(void*)(pbuf)) & (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))


#define is_qos(pframe)	(get_frame_sub_type(pframe) & BIT(7))


static inline unsigned char *get_ra(unsigned char *pframe)
{
	unsigned char 	*ra;
	ra = GetAddr1Ptr(pframe);
	return ra;
}

struct addba_request {
	unsigned char	dialog_token;
	unsigned short	BA_para_set;
	unsigned short	BA_timeout_value;
	unsigned short	BA_starting_seqctrl;
} __packed;

/* Frame control bits */
#define _MORE_FRAG_	BIT(10)

/* Frame control accessors */
#define GetToDs(pbuf)		(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_TO_DS_)) != 0)
#define GetFrDs(pbuf)		(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_FROM_DS_)) != 0)
#define GetMFrag(pbuf)		(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_MORE_FRAG_)) != 0)
#define GetMData(pbuf)		(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_MORE_DATA_)) != 0)
#define GetPrivacy(pbuf)	(((*(unsigned short *)(void*)(pbuf)) & le16_to_cpu(_PRIVACY_)) != 0)
#define GetFrameType(pbuf)	(le16_to_cpu(*(unsigned short *)(void*)(pbuf)) & (BIT(3) | BIT(2)))
#define GetFrameSubType(pbuf)	(cpu_to_le16(*(unsigned short *)(void*)(pbuf)) & (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))
#define GetSequence(pbuf)	(cpu_to_le16(*(unsigned short *)(void*)((size_t)(pbuf) + 22)) >> 4)
#define GetFragNum(pbuf)	(cpu_to_le16(*(unsigned short *)(void*)((size_t)(pbuf) + 22)) & 0x0f)
#define get_tofr_ds(pframe)	((GetToDs(pframe) << 1) | GetFrDs(pframe))
#define GetAMsdu(pbuf)		(((le16_to_cpu(*(unsigned short *)(void*)pbuf)) >> 7) & 0x1)

/* Address field accessors */
#define GetAddr2Ptr(pbuf)	((unsigned char *)((size_t)(pbuf) + 10))
#define GetAddr3Ptr(pbuf)	((unsigned char *)((size_t)(pbuf) + 16))
#define GetAddr4Ptr(pbuf)	((unsigned char *)((size_t)(pbuf) + 24))

static inline unsigned char *get_da(unsigned char *pframe)
{
	unsigned int to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);
	switch (to_fr_ds) {
	case 0x02:
	case 0x03:
		return GetAddr3Ptr(pframe);
	default:
		return GetAddr1Ptr(pframe);
	}
}

static inline unsigned char *get_sa(unsigned char *pframe)
{
	unsigned int to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);
	switch (to_fr_ds) {
	case 0x01:
		return GetAddr3Ptr(pframe);
	case 0x03:
		return GetAddr4Ptr(pframe);
	default:
		return GetAddr2Ptr(pframe);
	}
}

static inline unsigned char *get_hdr_bssid(unsigned char *pframe)
{
	unsigned int to_fr_ds = (GetToDs(pframe) << 1) | GetFrDs(pframe);
	switch (to_fr_ds) {
	case 0x00:
		return GetAddr3Ptr(pframe);
	case 0x01:
		return GetAddr2Ptr(pframe);
	default:
		return GetAddr1Ptr(pframe);
	}
}

/* Security macros */
#define GET_ENCRY_ALGO(psecuritypriv, psta_security, encry_algo, bmcst) \
do { \
	if ((psecuritypriv)->dot11_wpa_mode) { \
		if (bmcst) \
			encry_algo = (u8)(psecuritypriv)->dot118021XGrpPrivacy; \
		else \
			encry_algo = (u8)(psta_security)->dot11_security_privacy; \
		break; \
	} else { \
		encry_algo = (u8)(psecuritypriv)->dot11PrivacyAlgrthm; \
	} \
} while (0)

#define SET_ICE_IV_LEN(iv_len, icv_len, encrypt) \
do { \
	switch (encrypt) { \
	case _WEP40_: case _WEP104_: iv_len = 4; icv_len = 4; break; \
	case _TKIP_:                 iv_len = 8; icv_len = 4; break; \
	case _AES_:                  iv_len = 8; icv_len = 8; break; \
	case _GCMP_256_:             iv_len = 8; icv_len = 16; break; \
	case _SMS4_:                 iv_len = 18; icv_len = 16; break; \
	default:                     iv_len = 0; icv_len = 0; break; \
	} \
} while (0)

#endif