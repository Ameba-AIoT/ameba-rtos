#include <whc_host_linux.h>

#ifdef CONFIG_WHCH

//===============================
// WEP related
//===============================
void wep_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len, u8 *key, u32 key_len)
{
	union  {
		unsigned char	ch[4];
		int i;
	} u_crc;
	struct arc4context	 rc4_ctx;
	u8	*payload, *iv, wepkey[16];

	// copy iv (pframe + wlan_hdr_len) to first three bytes of wepkey
	iv = pframe + wlan_hdr_len;

	memcpy(&wepkey[0], iv, 3);
	memcpy(&wepkey[3], key, key_len);

	payload = pframe + wlan_hdr_len + iv_len ;

	// For MCU project, tx fragmentation is not supported
	u_crc.i = cpu_to_le32(crc32_get(payload, payload_len));
	rt_arc4_init(&rc4_ctx, wepkey, 3 + key_len);
	rt_arc4_crypt(&rc4_ctx, payload, payload, payload_len);
	rt_arc4_crypt(&rc4_ctx, payload + payload_len, u_crc.ch, 4);


}

u8 wep_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len, u8 *key, u32 key_len, union u_crc *pcrc)
{
	struct arc4context	 rc4_ctx;
	u8	*payload, *iv, wepkey[16];

	iv = pframe + wlan_hdr_len;

	memcpy(&wepkey[0], iv, 3);
	memcpy(&wepkey[3], key, key_len);

	payload = pframe + wlan_hdr_len + iv_len ;

	//decrypt payload include icv
	rt_arc4_init(&rc4_ctx, wepkey, 3 + key_len);
	rt_arc4_crypt(&rc4_ctx, payload, payload,  payload_len);
	if (payload_len < 4) {
		return 0;
	}

	//calculate icv and compare the icv
	pcrc->i = le32_to_cpu(crc32_get(payload, payload_len - 4));

	if (pcrc->ch[3] != payload[payload_len - 1] || pcrc->ch[2] != payload[payload_len - 2] || \
		pcrc->ch[1] != payload[payload_len - 3] || pcrc->ch[0] != payload[payload_len - 4]) {
		return 0;
	}

	return 1;
}

//===============================
// TKIP related
//===============================

// ------------------------------
// Part 1: TKIP MIC
// ------------------------------
static u32 tkip_secmicgetuint32(u8 *p)
// Convert from Byte[] to Us4Byte32 in a portable way
{
	s32 i;
	u32 res = 0;

	for (i = 0; i < 4; i++) {
		res |= ((u32)(*p++)) << (8 * i);
	}

	return res;
}

static void tkip_micputuint32(u8 *p, u32 val)
// Convert from Us4Byte32 to Byte[] in a portable way
{
	long i;

	for (i = 0; i < 4; i++) {
		*p++ = (u8)(val & 0xff);
		val >>= 8;
	}

}

static void tkip_micclear(struct mic_data *pmicdata)
{
// Reset the state to the empty message.

	pmicdata->L = pmicdata->K0;
	pmicdata->R = pmicdata->K1;
	pmicdata->nBytesInM = 0;
	pmicdata->M = 0;

}

void tkip_micappendbyte(struct mic_data *pmicdata, u8 b)
{
	// Append the byte to our word-sized buffer
	pmicdata->M |= ((unsigned long)b) << (8 * pmicdata->nBytesInM);
	pmicdata->nBytesInM++;
	// Process the word if it is full.
	if (pmicdata->nBytesInM >= 4) {
		pmicdata->L ^= pmicdata->M;
		pmicdata->R ^= ROL32(pmicdata->L, 17);
		pmicdata->L += pmicdata->R;
		pmicdata->R ^= ((pmicdata->L & 0xff00ff00) >> 8) | ((pmicdata->L & 0x00ff00ff) << 8);
		pmicdata->L += pmicdata->R;
		pmicdata->R ^= ROL32(pmicdata->L, 3);
		pmicdata->L += pmicdata->R;
		pmicdata->R ^= ROR32(pmicdata->L, 2);
		pmicdata->L += pmicdata->R;
		// Clear the buffer
		pmicdata->M = 0;
		pmicdata->nBytesInM = 0;
	}
}

void whc_host_secmicsetkey(struct mic_data *pmicdata, u8 *key)
{
	// Set the key

	pmicdata->K0 = tkip_secmicgetuint32(key);
	pmicdata->K1 = tkip_secmicgetuint32(key + 4);
	// and reset the message
	tkip_micclear(pmicdata);
}

void whc_host_secmicappend(struct mic_data *pmicdata, u8 *src, u32 nbytes)
{

	// This is simple
	while (nbytes > 0) {
		tkip_micappendbyte(pmicdata, *src++);
		nbytes--;
	}

}

void whc_host_secgetmic(struct mic_data *pmicdata, u8 *dst)
{
	// Append the minimum padding
	tkip_micappendbyte(pmicdata, 0x5a);
	tkip_micappendbyte(pmicdata, 0);
	tkip_micappendbyte(pmicdata, 0);
	tkip_micappendbyte(pmicdata, 0);
	tkip_micappendbyte(pmicdata, 0);
	// and then zeroes until the length is a multiple of 4
	while (pmicdata->nBytesInM != 0) {
		tkip_micappendbyte(pmicdata, 0);
	}
	// The appendByte function has already computed the result.
	tkip_micputuint32(dst, pmicdata->L);
	tkip_micputuint32(dst + 4, pmicdata->R);
	// Reset to the empty message.
	tkip_micclear(pmicdata);
}

/* API for receive frame MIC check */
void whc_host_seccalctkipmic(u8 *key, u8 *header, u8 *data, u32 data_len, u8 *mic_code, u8 pri)
{

	struct mic_data	micdata;
	u8 priority[4] = {0x0, 0x0, 0x0, 0x0};

	whc_host_secmicsetkey(&micdata, key);
	priority[0] = pri;

	/* Michael MIC pseudo header: DA, SA, 3 x 0, Priority */
	if (header[1] & 1) { //ToDS==1
		whc_host_secmicappend(&micdata, &header[16], 6);  //DA
		if (header[1] & 2) { //From Ds==1
			whc_host_secmicappend(&micdata, &header[24], 6);
		} else {
			whc_host_secmicappend(&micdata, &header[10], 6);
		}
	} else {	//ToDS==0
		whc_host_secmicappend(&micdata, &header[4], 6);   //DA
		if (header[1] & 2) { //From Ds==1
			whc_host_secmicappend(&micdata, &header[16], 6);
		} else {
			whc_host_secmicappend(&micdata, &header[10], 6);
		}

	}
	whc_host_secmicappend(&micdata, &priority[0], 4);


	whc_host_secmicappend(&micdata, data, data_len);

	whc_host_secgetmic(&micdata, mic_code);

}

// ------------------------------
// Part 2: TKIP encryption/decryption
// ------------------------------


/* macros for extraction/creation of unsigned char/unsigned short values  */
#define RotR1(v16)   ((((v16) >> 1) & 0x7FFF) ^ (((v16) & 1) << 15))
#define   Lo8(v16)   ((u8)( (v16)       & 0x00FF))
#define   Hi8(v16)   ((u8)(((v16) >> 8) & 0x00FF))
#define  Lo16(v32)   ((u16)( (v32)       & 0xFFFF))
#define  Hi16(v32)   ((u16)(((v32) >>16) & 0xFFFF))
#define  Mk16(hi,lo) ((lo) ^ (((u16)(hi)) << 8))

/* select the Nth 16-bit word of the temporal key unsigned char array TK[]   */
#define  TK16(N)     Mk16(tk[2*(N)+1],tk[2*(N)])

/* S-box lookup: 16 bits --> 16 bits */
#define _S_(v16)     (TKIP_Sbox1[0][Lo8(v16)] ^ TKIP_Sbox1[1][Hi8(v16)])

/* fixed algorithm "parameters" */
#define PHASE1_LOOP_CNT   8    /* this needs to be "big enough"     */
#define TA_SIZE           6    /*  48-bit transmitter address       */
#define TK_SIZE          16    /* 128-bit temporal key              */
#define P1K_SIZE         10    /*  80-bit Phase1 key                */
#define RC4_KEY_SIZE     16    /* 128-bit RC4KEY (104 bits unknown) */


/* 2-unsigned char by 2-unsigned char subset of the full AES S-box table */
static const unsigned short TKIP_Sbox1[2][256] =      /* Sbox for hash (can be in ROM)     */
{ {
		0xC6A5, 0xF884, 0xEE99, 0xF68D, 0xFF0D, 0xD6BD, 0xDEB1, 0x9154,
		0x6050, 0x0203, 0xCEA9, 0x567D, 0xE719, 0xB562, 0x4DE6, 0xEC9A,
		0x8F45, 0x1F9D, 0x8940, 0xFA87, 0xEF15, 0xB2EB, 0x8EC9, 0xFB0B,
		0x41EC, 0xB367, 0x5FFD, 0x45EA, 0x23BF, 0x53F7, 0xE496, 0x9B5B,
		0x75C2, 0xE11C, 0x3DAE, 0x4C6A, 0x6C5A, 0x7E41, 0xF502, 0x834F,
		0x685C, 0x51F4, 0xD134, 0xF908, 0xE293, 0xAB73, 0x6253, 0x2A3F,
		0x080C, 0x9552, 0x4665, 0x9D5E, 0x3028, 0x37A1, 0x0A0F, 0x2FB5,
		0x0E09, 0x2436, 0x1B9B, 0xDF3D, 0xCD26, 0x4E69, 0x7FCD, 0xEA9F,
		0x121B, 0x1D9E, 0x5874, 0x342E, 0x362D, 0xDCB2, 0xB4EE, 0x5BFB,
		0xA4F6, 0x764D, 0xB761, 0x7DCE, 0x527B, 0xDD3E, 0x5E71, 0x1397,
		0xA6F5, 0xB968, 0x0000, 0xC12C, 0x4060, 0xE31F, 0x79C8, 0xB6ED,
		0xD4BE, 0x8D46, 0x67D9, 0x724B, 0x94DE, 0x98D4, 0xB0E8, 0x854A,
		0xBB6B, 0xC52A, 0x4FE5, 0xED16, 0x86C5, 0x9AD7, 0x6655, 0x1194,
		0x8ACF, 0xE910, 0x0406, 0xFE81, 0xA0F0, 0x7844, 0x25BA, 0x4BE3,
		0xA2F3, 0x5DFE, 0x80C0, 0x058A, 0x3FAD, 0x21BC, 0x7048, 0xF104,
		0x63DF, 0x77C1, 0xAF75, 0x4263, 0x2030, 0xE51A, 0xFD0E, 0xBF6D,
		0x814C, 0x1814, 0x2635, 0xC32F, 0xBEE1, 0x35A2, 0x88CC, 0x2E39,
		0x9357, 0x55F2, 0xFC82, 0x7A47, 0xC8AC, 0xBAE7, 0x322B, 0xE695,
		0xC0A0, 0x1998, 0x9ED1, 0xA37F, 0x4466, 0x547E, 0x3BAB, 0x0B83,
		0x8CCA, 0xC729, 0x6BD3, 0x283C, 0xA779, 0xBCE2, 0x161D, 0xAD76,
		0xDB3B, 0x6456, 0x744E, 0x141E, 0x92DB, 0x0C0A, 0x486C, 0xB8E4,
		0x9F5D, 0xBD6E, 0x43EF, 0xC4A6, 0x39A8, 0x31A4, 0xD337, 0xF28B,
		0xD532, 0x8B43, 0x6E59, 0xDAB7, 0x018C, 0xB164, 0x9CD2, 0x49E0,
		0xD8B4, 0xACFA, 0xF307, 0xCF25, 0xCAAF, 0xF48E, 0x47E9, 0x1018,
		0x6FD5, 0xF088, 0x4A6F, 0x5C72, 0x3824, 0x57F1, 0x73C7, 0x9751,
		0xCB23, 0xA17C, 0xE89C, 0x3E21, 0x96DD, 0x61DC, 0x0D86, 0x0F85,
		0xE090, 0x7C42, 0x71C4, 0xCCAA, 0x90D8, 0x0605, 0xF701, 0x1C12,
		0xC2A3, 0x6A5F, 0xAEF9, 0x69D0, 0x1791, 0x9958, 0x3A27, 0x27B9,
		0xD938, 0xEB13, 0x2BB3, 0x2233, 0xD2BB, 0xA970, 0x0789, 0x33A7,
		0x2DB6, 0x3C22, 0x1592, 0xC920, 0x8749, 0xAAFF, 0x5078, 0xA57A,
		0x038F, 0x59F8, 0x0980, 0x1A17, 0x65DA, 0xD731, 0x84C6, 0xD0B8,
		0x82C3, 0x29B0, 0x5A77, 0x1E11, 0x7BCB, 0xA8FC, 0x6DD6, 0x2C3A,
	},


	{  /* second half of table is unsigned char-reversed version of first! */
		0xA5C6, 0x84F8, 0x99EE, 0x8DF6, 0x0DFF, 0xBDD6, 0xB1DE, 0x5491,
		0x5060, 0x0302, 0xA9CE, 0x7D56, 0x19E7, 0x62B5, 0xE64D, 0x9AEC,
		0x458F, 0x9D1F, 0x4089, 0x87FA, 0x15EF, 0xEBB2, 0xC98E, 0x0BFB,
		0xEC41, 0x67B3, 0xFD5F, 0xEA45, 0xBF23, 0xF753, 0x96E4, 0x5B9B,
		0xC275, 0x1CE1, 0xAE3D, 0x6A4C, 0x5A6C, 0x417E, 0x02F5, 0x4F83,
		0x5C68, 0xF451, 0x34D1, 0x08F9, 0x93E2, 0x73AB, 0x5362, 0x3F2A,
		0x0C08, 0x5295, 0x6546, 0x5E9D, 0x2830, 0xA137, 0x0F0A, 0xB52F,
		0x090E, 0x3624, 0x9B1B, 0x3DDF, 0x26CD, 0x694E, 0xCD7F, 0x9FEA,
		0x1B12, 0x9E1D, 0x7458, 0x2E34, 0x2D36, 0xB2DC, 0xEEB4, 0xFB5B,
		0xF6A4, 0x4D76, 0x61B7, 0xCE7D, 0x7B52, 0x3EDD, 0x715E, 0x9713,
		0xF5A6, 0x68B9, 0x0000, 0x2CC1, 0x6040, 0x1FE3, 0xC879, 0xEDB6,
		0xBED4, 0x468D, 0xD967, 0x4B72, 0xDE94, 0xD498, 0xE8B0, 0x4A85,
		0x6BBB, 0x2AC5, 0xE54F, 0x16ED, 0xC586, 0xD79A, 0x5566, 0x9411,
		0xCF8A, 0x10E9, 0x0604, 0x81FE, 0xF0A0, 0x4478, 0xBA25, 0xE34B,
		0xF3A2, 0xFE5D, 0xC080, 0x8A05, 0xAD3F, 0xBC21, 0x4870, 0x04F1,
		0xDF63, 0xC177, 0x75AF, 0x6342, 0x3020, 0x1AE5, 0x0EFD, 0x6DBF,
		0x4C81, 0x1418, 0x3526, 0x2FC3, 0xE1BE, 0xA235, 0xCC88, 0x392E,
		0x5793, 0xF255, 0x82FC, 0x477A, 0xACC8, 0xE7BA, 0x2B32, 0x95E6,
		0xA0C0, 0x9819, 0xD19E, 0x7FA3, 0x6644, 0x7E54, 0xAB3B, 0x830B,
		0xCA8C, 0x29C7, 0xD36B, 0x3C28, 0x79A7, 0xE2BC, 0x1D16, 0x76AD,
		0x3BDB, 0x5664, 0x4E74, 0x1E14, 0xDB92, 0x0A0C, 0x6C48, 0xE4B8,
		0x5D9F, 0x6EBD, 0xEF43, 0xA6C4, 0xA839, 0xA431, 0x37D3, 0x8BF2,
		0x32D5, 0x438B, 0x596E, 0xB7DA, 0x8C01, 0x64B1, 0xD29C, 0xE049,
		0xB4D8, 0xFAAC, 0x07F3, 0x25CF, 0xAFCA, 0x8EF4, 0xE947, 0x1810,
		0xD56F, 0x88F0, 0x6F4A, 0x725C, 0x2438, 0xF157, 0xC773, 0x5197,
		0x23CB, 0x7CA1, 0x9CE8, 0x213E, 0xDD96, 0xDC61, 0x860D, 0x850F,
		0x90E0, 0x427C, 0xC471, 0xAACC, 0xD890, 0x0506, 0x01F7, 0x121C,
		0xA3C2, 0x5F6A, 0xF9AE, 0xD069, 0x9117, 0x5899, 0x273A, 0xB927,
		0x38D9, 0x13EB, 0xB32B, 0x3322, 0xBBD2, 0x70A9, 0x8907, 0xA733,
		0xB62D, 0x223C, 0x9215, 0x20C9, 0x4987, 0xFFAA, 0x7850, 0x7AA5,
		0x8F03, 0xF859, 0x8009, 0x171A, 0xDA65, 0x31D7, 0xC684, 0xB8D0,
		0xC382, 0xB029, 0x775A, 0x111E, 0xCB7B, 0xFCA8, 0xD66D, 0x3A2C,
	}
};

/*
**********************************************************************
* Routine: Phase 1 -- generate P1K, given TA, TK, IV32
*
* Inputs:
*     tk[]      = temporal key                         [128 bits]
*     ta[]      = transmitter's MAC address            [ 48 bits]
*     iv32      = upper 32 bits of IV                  [ 32 bits]
* Output:
*     p1k[]     = Phase 1 key                          [ 80 bits]
*
* Note:
*     This function only needs to be called every 2**16 packets,
*     although in theory it could be called every packet.
*
**********************************************************************
*/
void tkip_phase1(u16 *p1k, const u8 *tk, const u8 *ta, u32 iv32)
{
	signed int  i;
	/* Initialize the 80 bits of P1K[] from IV32 and TA[0..5]     */
	p1k[0]      = Lo16(iv32);
	p1k[1]      = Hi16(iv32);
	p1k[2]      = Mk16(ta[1], ta[0]); /* use TA[] as little-endian */
	p1k[3]      = Mk16(ta[3], ta[2]);
	p1k[4]      = Mk16(ta[5], ta[4]);

	/* Now compute an unbalanced Feistel cipher with 80-bit block */
	/* size on the 80-bit block P1K[], using the 128-bit key TK[] */
	for (i = 0; i < PHASE1_LOOP_CNT ; i++) {
		/* Each add operation here is mod 2**16 */
		p1k[0] += _S_(p1k[4] ^ TK16((i & 1) + 0));
		p1k[1] += _S_(p1k[0] ^ TK16((i & 1) + 2));
		p1k[2] += _S_(p1k[1] ^ TK16((i & 1) + 4));
		p1k[3] += _S_(p1k[2] ^ TK16((i & 1) + 6));
		p1k[4] += _S_(p1k[3] ^ TK16((i & 1) + 0));
		p1k[4] += (unsigned short)i;                     /* avoid "slide attacks" */
	}
}


/*
**********************************************************************
* Routine: Phase 2 -- generate RC4KEY, given TK, P1K, IV16
*
* Inputs:
*     tk[]      = Temporal key                         [128 bits]
*     p1k[]     = Phase 1 output key                   [ 80 bits]
*     iv16      = low 16 bits of IV counter            [ 16 bits]
* Output:
*     rc4key[]  = the key used to encrypt the packet   [128 bits]
*
* Note:
*     The value {TA,IV32,IV16} for Phase1/Phase2 must be unique
*     across all packets using the same key TK value. Then, for a
*     given value of TK[], this TKIP48 construction guarantees that
*     the final RC4KEY value is unique across all packets.
*
* Suggested implementation optimization: if PPK[] is "overlaid"
*     appropriately on RC4KEY[], there is no need for the final
*     for loop below that copies the PPK[] result into RC4KEY[].
*
**********************************************************************
*/
void tkip_phase2(u8 *rc4key, const u8 *tk, const u16 *p1k, u16 iv16)
{
	signed int  i;
	u16 PPK[6];                          /* temporary key for mixing    */
	/* Note: all adds in the PPK[] equations below are mod 2**16         */
	for (i = 0; i < 5; i++) {
		PPK[i] = p1k[i];    /* first, copy P1K to PPK      */
	}
	PPK[5]  =  p1k[4] + iv16;            /* next,  add in IV16          */

	/* Bijective non-linear mixing of the 96 bits of PPK[0..5]           */
	PPK[0] +=    _S_(PPK[5] ^ TK16(0));   /* Mix key in each "round"     */
	PPK[1] +=    _S_(PPK[0] ^ TK16(1));
	PPK[2] +=    _S_(PPK[1] ^ TK16(2));
	PPK[3] +=    _S_(PPK[2] ^ TK16(3));
	PPK[4] +=    _S_(PPK[3] ^ TK16(4));
	PPK[5] +=    _S_(PPK[4] ^ TK16(5));   /* Total # S-box lookups == 6  */

	/* Final sweep: bijective, "linear". Rotates kill LSB correlations   */
	PPK[0] +=  RotR1(PPK[5] ^ TK16(6));
	PPK[1] +=  RotR1(PPK[0] ^ TK16(7));   /* Use all of TK[] in Phase2   */
	PPK[2] +=  RotR1(PPK[1]);
	PPK[3] +=  RotR1(PPK[2]);
	PPK[4] +=  RotR1(PPK[3]);
	PPK[5] +=  RotR1(PPK[4]);
	/* Note: At this point, for a given key TK[0..15], the 96-bit output */
	/*       value PPK[0..5] is guaranteed to be unique, as a function   */
	/*       of the 96-bit "input" value   {TA,IV32,IV16}. That is, P1K  */
	/*       is now a keyed permutation of {TA,IV32,IV16}.               */

	/* Set RC4KEY[0..3], which includes "cleartext" portion of RC4 key   */
	rc4key[0] = Hi8(iv16);                /* RC4KEY[0..2] is the WEP IV  */
	rc4key[1] = (Hi8(iv16) | 0x20) & 0x7F; /* Help avoid weak (FMS) keys  */
	rc4key[2] = Lo8(iv16);
	rc4key[3] = Lo8((PPK[5] ^ TK16(0)) >> 1);


	/* Copy 96 bits of PPK[0..5] to RC4KEY[4..15]  (little-endian)       */
	for (i = 0; i < 6; i++) {
		rc4key[4 + 2 * i] = Lo8(PPK[i]);
		rc4key[5 + 2 * i] = Hi8(PPK[i]);
	}
}

void tkip_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len, u8 *key, u32 key_len, u8 *ta)
{
	u16	pnl;
	u32	pnh;
	u8	rc4key[16];
	u8   ttkey[16];
	union  {
		unsigned char	ch[4];
		int i;
	} u_crc;
	struct arc4context rc4_ctx;
	u8 *payload, *iv; //,*prwskey;
	union pn48 dot11txpn;

	iv = pframe + wlan_hdr_len;
	payload = pframe + wlan_hdr_len + iv_len;

	GET_TKIP_PN(iv, dot11txpn);

	pnl = (u16)(dot11txpn.val);
	pnh = (u32)(dot11txpn.val >> 16);

	tkip_phase1((u16 *)(void *)&ttkey[0], key, ta, pnh);
	tkip_phase2(&rc4key[0], key, (u16 *)(void *)&ttkey[0], pnl);

	// For MCU project, tx fragmentation not supported

	u_crc.i = cpu_to_le32(crc32_get(payload, payload_len));/* modified by Amy*/
	rt_arc4_init(&rc4_ctx, rc4key, key_len);
	rt_arc4_crypt(&rc4_ctx, payload, payload, payload_len);
	rt_arc4_crypt(&rc4_ctx, payload + payload_len, u_crc.ch, 4);
}

u8 tkip_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 iv_len, u32 payload_len, u8 *key, u32 key_len, u8 *ta, union u_crc *pcrc)
{
	/* To avoid gcc warnings */
	(void) key_len;

	u16 pnl;
	u32 pnh;
	union pn48 dot11txpn;
	u8	*payload, *iv;
	u8   ttkey[16], rc4key[16];
	struct arc4context rc4_ctx;

	// generate tkip trasmit per-packet key
	iv = pframe + wlan_hdr_len;
	payload = pframe + wlan_hdr_len + iv_len;
	GET_TKIP_PN(iv, dot11txpn);
	pnl = (u16)(dot11txpn.val);
	pnh = (u32)(dot11txpn.val >> 16);
	tkip_phase1((u16 *)(void *)&ttkey[0], key, ta, pnh);
	tkip_phase2(&rc4key[0], key, (unsigned short *)(void *)&ttkey[0], pnl);

	//decrypt payload include icv
	rt_arc4_init(&rc4_ctx, rc4key, 16);
	rt_arc4_crypt(&rc4_ctx, payload, payload, payload_len);
	if (payload_len < 4) {
		return 0;
	}
	pcrc->i = le32_to_cpu(crc32_get(payload, payload_len - 4));

	if (pcrc->ch[3] != payload[payload_len - 1] || pcrc->ch[2] != payload[payload_len - 2] || \
		pcrc->ch[1] != payload[payload_len - 3] || pcrc->ch[0] != payload[payload_len - 4]) {
		return 0;
	}
	return 1;

}

//===============================
// AES related
//===============================

//----------------------------------------
// AES implementation mode 1
//----------------------------------------

static const u8 aes1_sbox_table[256] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};


static const u8 rcon_table[12] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
	0x1b, 0x36, 0x36, 0x36
};


static void aes1_xor_128(u8 *a, u8 *b, u8 *out)
{
	signed int i;
	for (i = 0; i < 16; i++) {
		out[i] = a[i] ^ b[i];
	}
}


static void aes1_xor_32(u8 *a, u8 *b, u8 *out)
{
	signed int i;
	for (i = 0; i < 4; i++) {
		out[i] = a[i] ^ b[i];
	}
}


static u8 aes1_sbox(u8 a)
{
	return aes1_sbox_table[(signed int)a];
}


static void aes1_next_key(u8 *key, signed int round)
{
	u8 rcon;
	u8 sbox_key[4];

	sbox_key[0] = aes1_sbox(key[13]);
	sbox_key[1] = aes1_sbox(key[14]);
	sbox_key[2] = aes1_sbox(key[15]);
	sbox_key[3] = aes1_sbox(key[12]);

	rcon = rcon_table[round];

	aes1_xor_32(&key[0], sbox_key, &key[0]);
	key[0] = key[0] ^ rcon;

	aes1_xor_32(&key[4], &key[0], &key[4]);
	aes1_xor_32(&key[8], &key[4], &key[8]);
	aes1_xor_32(&key[12], &key[8], &key[12]);
}


static void aes1_byte_sub(u8 *in, u8 *out)
{
	signed int i;

	for (i = 0; i < 16; i++) {
		out[i] = aes1_sbox(in[i]);
	}
}


static void aes1_shift_row(u8 *in, u8 *out)
{
	out[0] =  in[0];
	out[1] =  in[5];
	out[2] =  in[10];
	out[3] =  in[15];
	out[4] =  in[4];
	out[5] =  in[9];
	out[6] =  in[14];
	out[7] =  in[3];
	out[8] =  in[8];
	out[9] =  in[13];
	out[10] = in[2];
	out[11] = in[7];
	out[12] = in[12];
	out[13] = in[1];
	out[14] = in[6];
	out[15] = in[11];
}


static void aes1_mix_column(u8 *in, u8 *out)
{
	signed int i;
	u8 add1b[4];
	u8 add1bf7[4];
	u8 rotl[4];
	u8 swap_halfs[4];
	u8 andf7[4];
	u8 rotr[4];
	u8 temp[4];
	u8 tempb[4];

	for (i = 0 ; i < 4; i++) {
		if ((in[i] & 0x80) == 0x80) {
			add1b[i] = 0x1b;
		} else {
			add1b[i] = 0x00;
		}
	}

	swap_halfs[0] = in[2];    /* Swap halfs */
	swap_halfs[1] = in[3];
	swap_halfs[2] = in[0];
	swap_halfs[3] = in[1];

	rotl[0] = in[3];        /* Rotate left 8 bits */
	rotl[1] = in[0];
	rotl[2] = in[1];
	rotl[3] = in[2];

	andf7[0] = in[0] & 0x7f;
	andf7[1] = in[1] & 0x7f;
	andf7[2] = in[2] & 0x7f;
	andf7[3] = in[3] & 0x7f;

	for (i = 3; i > 0; i--) { /* logical shift left 1 bit */
		andf7[i] = andf7[i] << 1;
		if ((andf7[i - 1] & 0x80) == 0x80) {
			andf7[i] = (andf7[i] | 0x01);
		}
	}
	andf7[0] = andf7[0] << 1;
	andf7[0] = andf7[0] & 0xfe;

	aes1_xor_32(add1b, andf7, add1bf7);
	aes1_xor_32(in, add1bf7, rotr);

	temp[0] = rotr[0];         /* Rotate right 8 bits */
	rotr[0] = rotr[1];
	rotr[1] = rotr[2];
	rotr[2] = rotr[3];
	rotr[3] = temp[0];

	aes1_xor_32(add1bf7, rotr, temp);
	aes1_xor_32(swap_halfs, rotl, tempb);
	aes1_xor_32(temp, tempb, out);
}


void aes1_encrypt(u8 *key, u8 *data, u8 *ciphertext)
{
	signed int round;
	signed int i;
	u8 intermediatea[16];
	u8 intermediateb[16];
	u8 round_key[16];

	for (i = 0; i < 16; i++) {
		round_key[i] = key[i];
	}

	for (round = 0; round < 11; round++) {
		if (round == 0) {
			aes1_xor_128(round_key, data, ciphertext);
			aes1_next_key(round_key, round);
		} else if (round == 10) {
			aes1_byte_sub(ciphertext, intermediatea);
			aes1_shift_row(intermediatea, intermediateb);
			aes1_xor_128(intermediateb, round_key, ciphertext);
		} else { /* 1 - 9 */
			aes1_byte_sub(ciphertext, intermediatea);
			aes1_shift_row(intermediatea, intermediateb);
			aes1_mix_column(&intermediateb[0], &intermediatea[0]);
			aes1_mix_column(&intermediateb[4], &intermediatea[4]);
			aes1_mix_column(&intermediateb[8], &intermediatea[8]);
			aes1_mix_column(&intermediateb[12], &intermediatea[12]);
			aes1_xor_128(intermediatea, round_key, ciphertext);
			aes1_next_key(round_key, round);
		}
	}
}

/************************************************/
/* aesccmp_construct_mic_iv()                           */
/* Builds the MIC IV from header fields and PN  */
/************************************************/

void aesccmp_construct_mic_iv(u8 *mic_iv, signed int qc_exists, signed int a4_exists, u8 *mpdu, uint payload_length, u8 *pn_vector, uint frtype)
{
	signed int i;

	mic_iv[0] = 0x59;
	if (qc_exists && a4_exists) {
		mic_iv[1] = mpdu[30] & 0x0f;    /* QoS_TC           */
	}
	if (qc_exists && !a4_exists) {
		mic_iv[1] = mpdu[24] & 0x0f;    /* mute bits 7-4    */
	}
	if (!qc_exists) {
		mic_iv[1] = 0x00;
	}
	/* 802.11w management frame should set management bit(4) */
	if (frtype == RTW_MGT_TYPE) {
		mic_iv[1] |= BIT(4);
	}
	for (i = 2; i < 8; i++) {
		mic_iv[i] = mpdu[i + 8];    /* mic_iv[2:7] = A2[0:5] = mpdu[10:15] */
	}

	for (i = 8; i < 14; i++) {
		mic_iv[i] = pn_vector[13 - i];    /* mic_iv[8:13] = PN[5:0] */
	}

	mic_iv[14] = (unsigned char)(payload_length / 256);
	mic_iv[15] = (unsigned char)(payload_length % 256);
}


/************************************************/
/* aesccmp_construct_mic_header1()                      */
/* Builds the first MIC header block from       */
/* header fields.                               */
/************************************************/

void aesccmp_construct_mic_header1(u8 *mic_header1, signed int header_length, u8 *mpdu, uint frtype)
{
	mic_header1[0] = (u8)((header_length - 2) / 256);
	mic_header1[1] = (u8)((header_length - 2) % 256);
	/* 802.11w management frame don't AND subtype bits 4,5,6 of frame control field */
	if (frtype == RTW_MGT_TYPE) {
		mic_header1[2] = mpdu[0];
	} else {
		mic_header1[2] = mpdu[0] & 0xcf;    /* Mute CF poll & CF ack bits */
	}

	mic_header1[3] = mpdu[1] & 0x47;    /* Mute retry, more data and pwr mgt bits and +htc/ordr bit */
	mic_header1[4] = mpdu[4];       /* A1 */
	mic_header1[5] = mpdu[5];
	mic_header1[6] = mpdu[6];
	mic_header1[7] = mpdu[7];
	mic_header1[8] = mpdu[8];
	mic_header1[9] = mpdu[9];
	mic_header1[10] = mpdu[10];     /* A2 */
	mic_header1[11] = mpdu[11];
	mic_header1[12] = mpdu[12];
	mic_header1[13] = mpdu[13];
	mic_header1[14] = mpdu[14];
	mic_header1[15] = mpdu[15];
}

/************************************************/
/* aesccmp_construct_mic_header2()                      */
/* Builds the last MIC header block from        */
/* header fields.                               */
/************************************************/

void aesccmp_construct_mic_header2(u8 *mic_header2, u8 *mpdu, signed int a4_exists, signed int qc_exists)
{
	signed int i;

	for (i = 0; i < 16; i++) {
		mic_header2[i] = 0x00;
	}

	mic_header2[0] = mpdu[16];    /* A3 */
	mic_header2[1] = mpdu[17];
	mic_header2[2] = mpdu[18];
	mic_header2[3] = mpdu[19];
	mic_header2[4] = mpdu[20];
	mic_header2[5] = mpdu[21];

	//mic_header2[6] = mpdu[22] & 0xf0;   /* SC */
	mic_header2[6] = 0x00;
	mic_header2[7] = 0x00; /* mpdu[23]; */


	if (!qc_exists && a4_exists) {
		for (i = 0; i < 6; i++) {
			mic_header2[8 + i] = mpdu[24 + i];    /* A4 */
		}

	}

	if (qc_exists && !a4_exists) {
		mic_header2[8] = mpdu[24] & 0x0f; /* mute bits 15 - 4 */
		mic_header2[9] = mpdu[25] & 0x00;
	}

	if (qc_exists && a4_exists) {
		for (i = 0; i < 6; i++) {
			mic_header2[8 + i] = mpdu[24 + i];    /* A4 */
		}

		mic_header2[14] = mpdu[30] & 0x0f;
		mic_header2[15] = mpdu[31] & 0x00;
	}
}

/************************************************/
/* aesccmp_construct_mic_header2()                      */
/* Builds the last MIC header block from        */
/* header fields.                               */
/************************************************/

void aesccmp_construct_ctr_preload(u8 *ctr_preload, signed int a4_exists, signed int qc_exists, u8 *mpdu, u8 *pn_vector, signed int c, uint frtype)
{
	signed int i = 0;

	for (i = 0; i < 16; i++) {
		ctr_preload[i] = 0x00;
	}
	i = 0;

	ctr_preload[0] = 0x01;                                  /* flag */
	if (qc_exists && a4_exists) {
		ctr_preload[1] = mpdu[30] & 0x0f;    /* QoC_Control */
	}
	if (qc_exists && !a4_exists) {
		ctr_preload[1] = mpdu[24] & 0x0f;
	}
	/* 802.11w management frame should set management bit(4) */
	if (frtype == RTW_MGT_TYPE) {
		ctr_preload[1] |= BIT(4);
	}

	for (i = 2; i < 8; i++) {
		ctr_preload[i] = mpdu[i + 8];    /* ctr_preload[2:7] = A2[0:5] = mpdu[10:15] */
	}

	for (i = 8; i < 14; i++) {
		ctr_preload[i] =    pn_vector[13 - i];    /* ctr_preload[8:13] = PN[5:0] */
	}

	ctr_preload[14] = (unsigned char)(c / 256);   /* Ctr */
	ctr_preload[15] = (unsigned char)(c % 256);
}

/* A 128 bit, bitwise exclusive or  		*/

static void aesccmp_bitwise_xor(u8 *ina, u8 *inb, u8 *out)
{
	signed int i;

	for (i = 0; i < 16; i++) {
		out[i] = ina[i] ^ inb[i];
	}
}


u32 aes_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 frame_type)
{
	uint qc_exists, a4_exists, htc_exists, i, j, payload_remainder,
		 num_blocks, payload_index;

	u8 pn_vector[6];
	u8 mic_iv[16];
	u8 mic_header1[16];
	u8 mic_header2[16];
	u8 ctr_preload[16];

	/* Intermediate Buffers */
	u8 chain_buffer[16];
	u8 aes_out[16];
	u8 padded_buffer[16];

	u32 frtype = frame_type & 0xC;

	_memset((void *)mic_iv, 0, 16);
	_memset((void *)mic_header1, 0, 16);
	_memset((void *)mic_header2, 0, 16);
	_memset((void *)ctr_preload, 0, 16);
	_memset((void *)chain_buffer, 0, 16);
	_memset((void *)aes_out, 0, 16);
	_memset((void *)padded_buffer, 0, 16);

	if ((wlan_hdr_len == WLAN_HDR_A3_LEN) || (wlan_hdr_len ==  WLAN_HDR_A3_QOS_LEN) || (wlan_hdr_len ==  WLAN_HDR_A3_QOS_HTC_LEN)) {
		a4_exists = 0;
	} else {
		a4_exists = 1;
	}

	// Check whether QoC control exist or not.
	if (       // RTW_DATA_TYPE =	(BIT(3))
		(frame_type == 0x18) ||	//RTW_DATA_CFACK (BIT(4) | RTW_DATA_TYPE)
		(frame_type == 0x28) ||	//RTW_DATA_CFPOLL (BIT(5) | RTW_DATA_TYPE)
		(frame_type == 0x38)) {	//RTW_DATA_CFACKPOLL (BIT(5) | BIT(4) | RTW_DATA_TYPE)
		qc_exists = 1;
	}
	/* add for CONFIG_IEEE80211W, none 11w also can use */
	else if (
		(frame_type == 0x88) ||	//QoS Data
		(frame_type == 0x98) ||	//QoS Data + CF-Ack
		(frame_type == 0xa8) ||	//QoS Data + CF-Poll
		(frame_type == 0xb8)) {	//QoS Data + CF-Ack + CF-Poll
		if ((wlan_hdr_len !=  WLAN_HDR_A3_QOS_LEN) && (wlan_hdr_len != WLAN_HDR_A3_QOS_HTC_LEN)) {
			wlan_hdr_len += 2;
		}

		qc_exists = 1;
	} else {
		qc_exists = 0;
	}

	pn_vector[0] = pframe[wlan_hdr_len];
	pn_vector[1] = pframe[wlan_hdr_len + 1];
	pn_vector[2] = pframe[wlan_hdr_len + 4];
	pn_vector[3] = pframe[wlan_hdr_len + 5];
	pn_vector[4] = pframe[wlan_hdr_len + 6];
	pn_vector[5] = pframe[wlan_hdr_len + 7];

	aesccmp_construct_mic_iv(mic_iv, qc_exists, a4_exists, pframe, payload_len, pn_vector, frtype);

	htc_exists = (pframe[1] & 0x80) ? 1 : 0;
	aesccmp_construct_mic_header1(mic_header1, (wlan_hdr_len - 4 * htc_exists)/* AAD len not including htc len */, pframe, frtype);
	aesccmp_construct_mic_header2(mic_header2, pframe, a4_exists, qc_exists);

	payload_remainder = payload_len % 16;
	num_blocks = payload_len / 16;

	/* Find start of payload */
	payload_index = (wlan_hdr_len + 8);

	/* Calculate MIC */
	aes1_encrypt(key, mic_iv, aes_out);
	aesccmp_bitwise_xor(aes_out, mic_header1, chain_buffer);
	aes1_encrypt(key, chain_buffer, aes_out);
	aesccmp_bitwise_xor(aes_out, mic_header2, chain_buffer);
	aes1_encrypt(key, chain_buffer, aes_out);

	for (i = 0; i < num_blocks; i++) {
		aesccmp_bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);//bitwise_xor(aes_out, &message[payload_index], chain_buffer);

		payload_index += 16;
		aes1_encrypt(key, chain_buffer, aes_out);
	}

	/* Add on the final payload block if it needs padding */
	if (payload_remainder > 0) {
		for (j = 0; j < 16; j++) {
			padded_buffer[j] = 0x00;
		}
		for (j = 0; j < payload_remainder; j++) {
			padded_buffer[j] = pframe[payload_index++];//padded_buffer[j] = message[payload_index++];
		}
		aesccmp_bitwise_xor(aes_out, padded_buffer, chain_buffer);
		aes1_encrypt(key, chain_buffer, aes_out);

	}

	/* Insert MIC into payload */
	for (j = 0; j < 8; j++) {
		pframe[payload_index + j] = aes_out[j];   // use last block's result first 8B for MIC
	}

	payload_index = wlan_hdr_len + 8;
	for (i = 0; i < num_blocks; i++) {
		aesccmp_construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe, pn_vector, i + 1, frtype);
		aes1_encrypt(key, ctr_preload, aes_out);
		aesccmp_bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);//bitwise_xor(aes_out, &message[payload_index], chain_buffer);
		for (j = 0; j < 16; j++) {
			pframe[payload_index++] = chain_buffer[j];    //for (j=0; j<16;j++) message[payload_index++] = chain_buffer[j];
		}
	}

	if (payload_remainder > 0) {        /* If there is a short final block, then pad it,*/
		/* encrypt it and copy the unpadded part back   */
		aesccmp_construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe, pn_vector, num_blocks + 1, frtype);

		for (j = 0; j < 16; j++) {
			padded_buffer[j] = 0x00;
		}
		for (j = 0; j < payload_remainder; j++) {
			padded_buffer[j] = pframe[payload_index + j]; //padded_buffer[j] = message[payload_index+j];
		}
		aes1_encrypt(key, ctr_preload, aes_out);
		aesccmp_bitwise_xor(aes_out, padded_buffer, chain_buffer);
		for (j = 0; j < payload_remainder; j++) {
			pframe[payload_index++] = chain_buffer[j];    //for (j=0; j<payload_remainder;j++) message[payload_index++] = chain_buffer[j];
		}
	}

	/* Encrypt the MIC */
	aesccmp_construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe, pn_vector, 0, frtype);

	for (j = 0; j < 16; j++) {
		padded_buffer[j] = 0x00;
	}
	for (j = 0; j < 8; j++) {
		padded_buffer[j] = pframe[j + wlan_hdr_len + 8 + payload_len]; //padded_buffer[j] = message[j+wlan_hdr_len+8+payload_len];
	}

	aes1_encrypt(key, ctr_preload, aes_out);
	aesccmp_bitwise_xor(aes_out, padded_buffer, chain_buffer);
	for (j = 0; j < 8; j++) {
		pframe[payload_index++] = chain_buffer[j];    //for (j=0; j<8;j++) message[payload_index++] = chain_buffer[j];
	}

	return 1;
}

u32 aes_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 frame_type)
{
	uint	qc_exists, a4_exists, i, j, payload_remainder,
			num_blocks, payload_index;
	signed int res = 1;
	u8 pn_vector[6];
	u8 mic_iv[16];
	u8 mic_header1[16];
	u8 mic_header2[16];
	u8 ctr_preload[16];

	/* Intermediate Buffers */
	u8 chain_buffer[16];
	u8 aes_out[16];
	u8 padded_buffer[16];
	u8 mic_dec[8];

	u32 frtype = frame_type & 0xC;

	_memset((void *)mic_iv, 0, 16);
	_memset((void *)mic_header1, 0, 16);
	_memset((void *)mic_header2, 0, 16);
	_memset((void *)ctr_preload, 0, 16);
	_memset((void *)chain_buffer, 0, 16);
	_memset((void *)aes_out, 0, 16);
	_memset((void *)padded_buffer, 0, 16);

	//start to decrypt the payload

	num_blocks = (payload_len - 8) / 16; //(payload_len including llc, payload_length and mic )

	payload_remainder = (payload_len - 8) % 16;

	pn_vector[0]  = pframe[wlan_hdr_len];
	pn_vector[1]  = pframe[wlan_hdr_len + 1];
	pn_vector[2]  = pframe[wlan_hdr_len + 4];
	pn_vector[3]  = pframe[wlan_hdr_len + 5];
	pn_vector[4]  = pframe[wlan_hdr_len + 6];
	pn_vector[5]  = pframe[wlan_hdr_len + 7];

	if ((wlan_hdr_len == WLAN_HDR_A3_LEN) || (wlan_hdr_len ==  WLAN_HDR_A3_QOS_LEN)) {
		a4_exists = 0;
	} else {
		a4_exists = 1;
	}

	// Check whether QoC control exist or not.
	// Do not use wifi.h which is
	if (       // RTW_DATA_TYPE =	(BIT(3))
		(frame_type == 0x18) ||	//RTW_DATA_CFACK (BIT(4) | RTW_DATA_TYPE)
		(frame_type == 0x28) ||	//RTW_DATA_CFPOLL (BIT(5) | RTW_DATA_TYPE)
		(frame_type == 0x38)) {	//RTW_DATA_CFACKPOLL (BIT(5) | BIT(4) | RTW_DATA_TYPE)
		if (wlan_hdr_len !=  WLAN_HDR_A3_QOS_LEN) {
			wlan_hdr_len += 2;
		}

		qc_exists = 1;
	} else if ( /* only for data packet . add for CONFIG_IEEE80211W, none 11w also can use */
		(frame_type == 0x88) ||	//QoS Data
		(frame_type == 0x98) ||	//QoS Data + CF-Ack
		(frame_type == 0xa8) ||	//QoS Data + CF-Poll
		(frame_type == 0xb8)) {	//QoS Data + CF-Ack + CF-Poll
		if (wlan_hdr_len !=  WLAN_HDR_A3_QOS_LEN) {
			wlan_hdr_len += 2;
		}

		qc_exists = 1;
	} else {
		qc_exists = 0;
	}

	// now, decrypt pframe with wlan_hdr_len offset and payload_len long

	payload_index = wlan_hdr_len + 8; // 8 is for extiv

	for (i = 0; i < num_blocks; i++) {
		aesccmp_construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe, pn_vector, i + 1, frtype);

		aes1_encrypt(key, ctr_preload, aes_out);
		aesccmp_bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);

		for (j = 0; j < 16; j++) {
			pframe[payload_index++] = chain_buffer[j];
		}
	}

	if (payload_remainder > 0) {
		/* If there is a short final block, then pad it,*/
		/* encrypt it and copy the unpadded part back   */

		aesccmp_construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe, pn_vector, num_blocks + 1, frtype);

		for (j = 0; j < 16; j++) {
			padded_buffer[j] = 0x00;
		}
		for (j = 0; j < payload_remainder; j++) {
			padded_buffer[j] = pframe[payload_index + j];
		}
		aes1_encrypt(key, ctr_preload, aes_out);
		aesccmp_bitwise_xor(aes_out, padded_buffer, chain_buffer);
		for (j = 0; j < payload_remainder; j++) {
			pframe[payload_index++] = chain_buffer[j];
		}
	}

	/* Decrypt the MIC */
	aesccmp_construct_ctr_preload(ctr_preload, a4_exists, qc_exists, pframe, pn_vector, 0, frtype);

	for (j = 0; j < 16; j++) {
		padded_buffer[j] = 0x00;
	}
	for (j = 0; j < 8; j++) {
		padded_buffer[j] = pframe[j + wlan_hdr_len + 8 + payload_len - 8];
	}
	aes1_encrypt(key, ctr_preload, aes_out);
	aesccmp_bitwise_xor(aes_out, padded_buffer, chain_buffer);
	for (j = 0; j < 8; j++) {
		mic_dec[j] = chain_buffer[j];
	}

	//start to calculate the mic

	pn_vector[0] = pframe[wlan_hdr_len];
	pn_vector[1] = pframe[wlan_hdr_len + 1];
	pn_vector[2] = pframe[wlan_hdr_len + 4];
	pn_vector[3] = pframe[wlan_hdr_len + 5];
	pn_vector[4] = pframe[wlan_hdr_len + 6];
	pn_vector[5] = pframe[wlan_hdr_len + 7];

	aesccmp_construct_mic_iv(mic_iv, qc_exists, a4_exists, pframe, payload_len - 8, pn_vector, frtype);

	aesccmp_construct_mic_header1(mic_header1, wlan_hdr_len, pframe, frtype);
	aesccmp_construct_mic_header2(mic_header2, pframe, a4_exists, qc_exists);

	payload_remainder = (payload_len - 8) % 16;
	num_blocks = (payload_len - 8) / 16;

	/* Find start of payload */
	payload_index = (wlan_hdr_len + 8);

	/* Calculate MIC */
	aes1_encrypt(key, mic_iv, aes_out);
	aesccmp_bitwise_xor(aes_out, mic_header1, chain_buffer);
	aes1_encrypt(key, chain_buffer, aes_out);
	aesccmp_bitwise_xor(aes_out, mic_header2, chain_buffer);
	aes1_encrypt(key, chain_buffer, aes_out);

	for (i = 0; i < num_blocks; i++) {
		aesccmp_bitwise_xor(aes_out, &pframe[payload_index], chain_buffer);

		payload_index += 16;
		aes1_encrypt(key, chain_buffer, aes_out);
	}

	/* Add on the final payload block if it needs padding */
	if (payload_remainder > 0) {
		for (j = 0; j < 16; j++) {
			padded_buffer[j] = 0x00;
		}
		for (j = 0; j < payload_remainder; j++) {
			padded_buffer[j] = pframe[payload_index++];
		}
		aesccmp_bitwise_xor(aes_out, padded_buffer, chain_buffer);
		aes1_encrypt(key, chain_buffer, aes_out);
	}

	//compare the mic
	for (i = 0; i < 8; i++) {
		if (mic_dec[i] != aes_out[i]) {
			res = 0;
		}
	}
	return res;
}


#define AES_BLOCK_SIZE 16

static const u32 Te0[256] = {
	0xc66363a5U, 0xf87c7c84U, 0xee777799U, 0xf67b7b8dU,
	0xfff2f20dU, 0xd66b6bbdU, 0xde6f6fb1U, 0x91c5c554U,
	0x60303050U, 0x02010103U, 0xce6767a9U, 0x562b2b7dU,
	0xe7fefe19U, 0xb5d7d762U, 0x4dababe6U, 0xec76769aU,
	0x8fcaca45U, 0x1f82829dU, 0x89c9c940U, 0xfa7d7d87U,
	0xeffafa15U, 0xb25959ebU, 0x8e4747c9U, 0xfbf0f00bU,
	0x41adadecU, 0xb3d4d467U, 0x5fa2a2fdU, 0x45afafeaU,
	0x239c9cbfU, 0x53a4a4f7U, 0xe4727296U, 0x9bc0c05bU,
	0x75b7b7c2U, 0xe1fdfd1cU, 0x3d9393aeU, 0x4c26266aU,
	0x6c36365aU, 0x7e3f3f41U, 0xf5f7f702U, 0x83cccc4fU,
	0x6834345cU, 0x51a5a5f4U, 0xd1e5e534U, 0xf9f1f108U,
	0xe2717193U, 0xabd8d873U, 0x62313153U, 0x2a15153fU,
	0x0804040cU, 0x95c7c752U, 0x46232365U, 0x9dc3c35eU,
	0x30181828U, 0x379696a1U, 0x0a05050fU, 0x2f9a9ab5U,
	0x0e070709U, 0x24121236U, 0x1b80809bU, 0xdfe2e23dU,
	0xcdebeb26U, 0x4e272769U, 0x7fb2b2cdU, 0xea75759fU,
	0x1209091bU, 0x1d83839eU, 0x582c2c74U, 0x341a1a2eU,
	0x361b1b2dU, 0xdc6e6eb2U, 0xb45a5aeeU, 0x5ba0a0fbU,
	0xa45252f6U, 0x763b3b4dU, 0xb7d6d661U, 0x7db3b3ceU,
	0x5229297bU, 0xdde3e33eU, 0x5e2f2f71U, 0x13848497U,
	0xa65353f5U, 0xb9d1d168U, 0x00000000U, 0xc1eded2cU,
	0x40202060U, 0xe3fcfc1fU, 0x79b1b1c8U, 0xb65b5bedU,
	0xd46a6abeU, 0x8dcbcb46U, 0x67bebed9U, 0x7239394bU,
	0x944a4adeU, 0x984c4cd4U, 0xb05858e8U, 0x85cfcf4aU,
	0xbbd0d06bU, 0xc5efef2aU, 0x4faaaae5U, 0xedfbfb16U,
	0x864343c5U, 0x9a4d4dd7U, 0x66333355U, 0x11858594U,
	0x8a4545cfU, 0xe9f9f910U, 0x04020206U, 0xfe7f7f81U,
	0xa05050f0U, 0x783c3c44U, 0x259f9fbaU, 0x4ba8a8e3U,
	0xa25151f3U, 0x5da3a3feU, 0x804040c0U, 0x058f8f8aU,
	0x3f9292adU, 0x219d9dbcU, 0x70383848U, 0xf1f5f504U,
	0x63bcbcdfU, 0x77b6b6c1U, 0xafdada75U, 0x42212163U,
	0x20101030U, 0xe5ffff1aU, 0xfdf3f30eU, 0xbfd2d26dU,
	0x81cdcd4cU, 0x180c0c14U, 0x26131335U, 0xc3ecec2fU,
	0xbe5f5fe1U, 0x359797a2U, 0x884444ccU, 0x2e171739U,
	0x93c4c457U, 0x55a7a7f2U, 0xfc7e7e82U, 0x7a3d3d47U,
	0xc86464acU, 0xba5d5de7U, 0x3219192bU, 0xe6737395U,
	0xc06060a0U, 0x19818198U, 0x9e4f4fd1U, 0xa3dcdc7fU,
	0x44222266U, 0x542a2a7eU, 0x3b9090abU, 0x0b888883U,
	0x8c4646caU, 0xc7eeee29U, 0x6bb8b8d3U, 0x2814143cU,
	0xa7dede79U, 0xbc5e5ee2U, 0x160b0b1dU, 0xaddbdb76U,
	0xdbe0e03bU, 0x64323256U, 0x743a3a4eU, 0x140a0a1eU,
	0x924949dbU, 0x0c06060aU, 0x4824246cU, 0xb85c5ce4U,
	0x9fc2c25dU, 0xbdd3d36eU, 0x43acacefU, 0xc46262a6U,
	0x399191a8U, 0x319595a4U, 0xd3e4e437U, 0xf279798bU,
	0xd5e7e732U, 0x8bc8c843U, 0x6e373759U, 0xda6d6db7U,
	0x018d8d8cU, 0xb1d5d564U, 0x9c4e4ed2U, 0x49a9a9e0U,
	0xd86c6cb4U, 0xac5656faU, 0xf3f4f407U, 0xcfeaea25U,
	0xca6565afU, 0xf47a7a8eU, 0x47aeaee9U, 0x10080818U,
	0x6fbabad5U, 0xf0787888U, 0x4a25256fU, 0x5c2e2e72U,
	0x381c1c24U, 0x57a6a6f1U, 0x73b4b4c7U, 0x97c6c651U,
	0xcbe8e823U, 0xa1dddd7cU, 0xe874749cU, 0x3e1f1f21U,
	0x964b4bddU, 0x61bdbddcU, 0x0d8b8b86U, 0x0f8a8a85U,
	0xe0707090U, 0x7c3e3e42U, 0x71b5b5c4U, 0xcc6666aaU,
	0x904848d8U, 0x06030305U, 0xf7f6f601U, 0x1c0e0e12U,
	0xc26161a3U, 0x6a35355fU, 0xae5757f9U, 0x69b9b9d0U,
	0x17868691U, 0x99c1c158U, 0x3a1d1d27U, 0x279e9eb9U,
	0xd9e1e138U, 0xebf8f813U, 0x2b9898b3U, 0x22111133U,
	0xd26969bbU, 0xa9d9d970U, 0x078e8e89U, 0x339494a7U,
	0x2d9b9bb6U, 0x3c1e1e22U, 0x15878792U, 0xc9e9e920U,
	0x87cece49U, 0xaa5555ffU, 0x50282878U, 0xa5dfdf7aU,
	0x038c8c8fU, 0x59a1a1f8U, 0x09898980U, 0x1a0d0d17U,
	0x65bfbfdaU, 0xd7e6e631U, 0x844242c6U, 0xd06868b8U,
	0x824141c3U, 0x299999b0U, 0x5a2d2d77U, 0x1e0f0f11U,
	0x7bb0b0cbU, 0xa85454fcU, 0x6dbbbbd6U, 0x2c16163aU,
};


const u8 rcons[] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
	/* for 128-bit blocks, Rijndael never uses more than 10 rcon values */
};


static inline u32 rotr(u32 val, int bits)
{
	return (val >> bits) | (val << (32 - bits));
}

#define RCON(i) ((u32) rcons[(i)] << 24)

#define TE0(i) Te0[((i) >> 24) & 0xff]
#define TE1(i) rotr(Te0[((i) >> 16) & 0xff], 8)
#define TE2(i) rotr(Te0[((i) >> 8) & 0xff], 16)
#define TE3(i) rotr(Te0[(i) & 0xff], 24)

#define TE41(i) ((Te0[((i) >> 24) & 0xff] << 8) & 0xff000000)
#define TE42(i) (Te0[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE43(i) (Te0[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE44(i) ((Te0[(i) & 0xff] >> 8) & 0x000000ff)

#define TE421(i) ((Te0[((i) >> 16) & 0xff] << 8) & 0xff000000)
#define TE432(i) (Te0[((i) >> 8) & 0xff] & 0x00ff0000)
#define TE443(i) (Te0[(i) & 0xff] & 0x0000ff00)
#define TE414(i) ((Te0[((i) >> 24) & 0xff] >> 8) & 0x000000ff)
#define TE411(i) ((Te0[((i) >> 24) & 0xff] << 8) & 0xff000000)
#define TE422(i) (Te0[((i) >> 16) & 0xff] & 0x00ff0000)
#define TE433(i) (Te0[((i) >> 8) & 0xff] & 0x0000ff00)
#define TE444(i) ((Te0[(i) & 0xff] >> 8) & 0x000000ff)

#ifdef _MSC_VER
#define SWAP(x) (_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00)
#define GETU32(p) SWAP(*((u32 *)(p)))
#define PUTU32(ct, st) { *((u32 *)(ct)) = SWAP((st)); }
#else
#define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ \
					((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
#define PUTU32(ct, st) { \
					(ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); \
					(ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }
#endif

#define AES_PRIV_SIZE (4 * 4 * 15 + 4)
#define AES_PRIV_NR_POS (4 * 15)

static int os_memcmp_const(const void *a, const void *b, size_t len)
{
	const u8 *aa = a;
	const u8 *bb = b;
	size_t i;
	u8 res;

	for (res = 0, i = 0; i < len; i++) {
		res |= aa[i] ^ bb[i];
	}

	return res;
}

/**
 * Expand the cipher key into the encryption key schedule.
 *
 * @return	the number of rounds for the given cipher key size.
 */
static int rijndaelKeySetupEnc(u32 rk[], const u8 cipherKey[], int keyBits)
{
	int i;
	u32 temp;

	rk[0] = GETU32(cipherKey);
	rk[1] = GETU32(cipherKey +  4);
	rk[2] = GETU32(cipherKey +  8);
	rk[3] = GETU32(cipherKey + 12);

	if (keyBits == 128) {
		for (i = 0; i < 10; i++) {
			temp  = rk[3];
			rk[4] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
					TE443(temp) ^ TE414(temp) ^ RCON(i);
			rk[5] = rk[1] ^ rk[4];
			rk[6] = rk[2] ^ rk[5];
			rk[7] = rk[3] ^ rk[6];
			rk += 4;
		}
		return 10;
	}

	rk[4] = GETU32(cipherKey + 16);
	rk[5] = GETU32(cipherKey + 20);

	if (keyBits == 192) {
		for (i = 0; i < 8; i++) {
			temp  = rk[5];
			rk[6] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
					TE443(temp) ^ TE414(temp) ^ RCON(i);
			rk[7] = rk[1] ^ rk[6];
			rk[8] = rk[2] ^ rk[7];
			rk[9] = rk[3] ^ rk[8];
			if (i == 7) {
				return 12;
			}
			rk[10] = rk[4] ^ rk[9];
			rk[11] = rk[5] ^ rk[10];
			rk += 6;
		}
	}

	rk[6] = GETU32(cipherKey + 24);
	rk[7] = GETU32(cipherKey + 28);

	if (keyBits == 256) {
		for (i = 0; i < 7; i++) {
			temp  = rk[7];
			rk[8] = rk[0] ^ TE421(temp) ^ TE432(temp) ^
					TE443(temp) ^ TE414(temp) ^ RCON(i);
			rk[9] = rk[1] ^ rk[8];
			rk[10] = rk[2] ^ rk[9];
			rk[11] = rk[3] ^ rk[10];
			if (i == 6) {
				return 14;
			}
			temp  = rk[11];
			rk[12] = rk[4] ^ TE411(temp) ^ TE422(temp) ^
					 TE433(temp) ^ TE444(temp);
			rk[13] = rk[5] ^ rk[12];
			rk[14] = rk[6] ^ rk[13];
			rk[15] = rk[7] ^ rk[14];
			rk += 8;
		}
	}

	return -1;
}

static void rijndaelEncrypt(const u32 rk[], int Nr, const u8 pt[16], u8 ct[16])
{
	u32 s0, s1, s2, s3, t0, t1, t2, t3;
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
d##0 = TE0(s##0) ^ TE1(s##1) ^ TE2(s##2) ^ TE3(s##3) ^ rk[4 * i]; \
d##1 = TE0(s##1) ^ TE1(s##2) ^ TE2(s##3) ^ TE3(s##0) ^ rk[4 * i + 1]; \
d##2 = TE0(s##2) ^ TE1(s##3) ^ TE2(s##0) ^ TE3(s##1) ^ rk[4 * i + 2]; \
d##3 = TE0(s##3) ^ TE1(s##0) ^ TE2(s##1) ^ TE3(s##2) ^ rk[4 * i + 3]

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
	if (Nr > 10) {
		ROUND(10, s, t);
		ROUND(11, t, s);
		if (Nr > 12) {
			ROUND(12, s, t);
			ROUND(13, t, s);
		}
	}

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
	s0 = TE41(t0) ^ TE42(t1) ^ TE43(t2) ^ TE44(t3) ^ rk[0];
	PUTU32(ct, s0);
	s1 = TE41(t1) ^ TE42(t2) ^ TE43(t3) ^ TE44(t0) ^ rk[1];
	PUTU32(ct +  4, s1);
	s2 = TE41(t2) ^ TE42(t3) ^ TE43(t0) ^ TE44(t1) ^ rk[2];
	PUTU32(ct +  8, s2);
	s3 = TE41(t3) ^ TE42(t0) ^ TE43(t1) ^ TE44(t2) ^ rk[3];
	PUTU32(ct + 12, s3);
}

static void *aes_encrypt_init(const u8 *key, size_t len)
{
	u32 *rk;
	int res;

	rk = (u32 *)kzalloc(AES_PRIV_SIZE, GFP_ATOMIC);
	if (rk == NULL) {
		return NULL;
	}
	res = rijndaelKeySetupEnc(rk, key, len * 8);
	if (res < 0) {
		kfree((u8 *)rk);
		return NULL;
	}
	rk[AES_PRIV_NR_POS] = res;
	return rk;
}

static int aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
	u32 *rk = ctx;
	rijndaelEncrypt(ctx, rk[AES_PRIV_NR_POS], plain, crypt);
	return 0;
}

static void aes_encrypt_deinit(void *ctx)
{
	_memset(ctx, 0, AES_PRIV_SIZE);
	kfree(ctx);
}

static void inc32(u8 *block)
{
	u32 val;
	val = WPA_GET_BE32(block + AES_BLOCK_SIZE - 4);
	val++;
	WPA_PUT_BE32(block + AES_BLOCK_SIZE - 4, val);
}

static void xor_block(u8 *dst, const u8 *src)
{
	int i;
	for (i = 0; i < 16; i++) {
		dst[i] ^= src[i];
	}
}

static void shift_right_block(u8 *v)
{
	u32 val;

	val = WPA_GET_BE32(v + 12);
	val >>= 1;
	if (v[11] & 0x01) {
		val |= 0x80000000;
	}
	WPA_PUT_BE32(v + 12, val);

	val = WPA_GET_BE32(v + 8);
	val >>= 1;
	if (v[7] & 0x01) {
		val |= 0x80000000;
	}
	WPA_PUT_BE32(v + 8, val);

	val = WPA_GET_BE32(v + 4);
	val >>= 1;
	if (v[3] & 0x01) {
		val |= 0x80000000;
	}
	WPA_PUT_BE32(v + 4, val);

	val = WPA_GET_BE32(v);
	val >>= 1;
	WPA_PUT_BE32(v, val);
}


/* Multiplication in GF(2^128) */
static void gf_mult(const u8 *x, const u8 *y, u8 *z)
{
	u8 v[16];
	int i, j;

	_memset(z, 0, 16); /* Z_0 = 0^128 */
	memcpy(v, (u8 *) y, 16); /* V_0 = Y */

	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			if (x[i] & BIT(7 - j)) {
				/* Z_(i + 1) = Z_i XOR V_i */
				xor_block(z, v);
			} else {
				/* Z_(i + 1) = Z_i */
			}

			if (v[15] & 0x01) {
				/* V_(i + 1) = (V_i >> 1) XOR R */
				shift_right_block(v);
				/* R = 11100001 || 0^120 */
				v[0] ^= 0xe1;
			} else {
				/* V_(i + 1) = V_i >> 1 */
				shift_right_block(v);
			}
		}
	}
}

static void ghash_start(u8 *y)
{
	/* Y_0 = 0^128 */
	_memset(y, 0, 16);
}

static void ghash(const u8 *h, const u8 *x, size_t xlen, u8 *y)
{
	size_t m, i;
	const u8 *xpos = x;
	u8 tmp[16];

	m = xlen / 16;

	for (i = 0; i < m; i++) {
		/* Y_i = (Y^(i-1) XOR X_i) dot H */
		xor_block(y, xpos);
		xpos += 16;

		/* dot operation:
		 * multiplication operation for binary Galois (finite) field of
		 * 2^128 elements */
		gf_mult(y, h, tmp);
		memcpy(y, tmp, 16);
	}

	if (x + xlen > xpos) {
		/* Add zero padded last block */
		size_t last = x + xlen - xpos;
		memcpy(tmp, (u8 *) xpos, last);
		_memset(tmp + last, 0, sizeof(tmp) - last);

		/* Y_i = (Y^(i-1) XOR X_i) dot H */
		xor_block(y, tmp);

		/* dot operation:
		 * multiplication operation for binary Galois (finite) field of
		 * 2^128 elements */
		gf_mult(y, h, tmp);
		memcpy(y, tmp, 16);
	}

	/* Return Y_m */
}

static void aes_gctr(void *aes, const u8 *icb, const u8 *x, size_t xlen, u8 *y)
{
	size_t i, n, last;
	u8 cb[AES_BLOCK_SIZE], tmp[AES_BLOCK_SIZE];
	const u8 *xpos = x;
	u8 *ypos = y;

	if (xlen == 0) {
		return;
	}

	n = xlen / 16;
	memcpy(cb, (u8 *) icb, AES_BLOCK_SIZE);

	/* Full blocks */
	for (i = 0; i < n; i++) {
		aes_encrypt(aes, cb, ypos);
		xor_block(ypos, xpos);
		xpos += AES_BLOCK_SIZE;
		ypos += AES_BLOCK_SIZE;

		inc32(cb);
	}

	last = x + xlen - xpos;

	if (last) {
		/* Last, partial block */
		aes_encrypt(aes, cb, tmp);
		for (i = 0; i < last; i++) {
			*ypos++ = *xpos++ ^ tmp[i];
		}
	}
}

static void *aes_gcm_init_hash_subkey(const u8 *key, size_t key_len, u8 *H)
{
	void *aes;

	aes = aes_encrypt_init(key, key_len);
	if (aes == NULL) {
		return NULL;
	}

	/* Generate hash subkey H = AES_K(0^128) */
	_memset(H, 0, AES_BLOCK_SIZE);
	aes_encrypt(aes, H, H);

	return aes;
}

static void aes_gcm_prepare_j0(const u8 *iv, size_t iv_len, const u8 *H, u8 *J0)
{
	u8 len_buf[16];

	if (iv_len == 12) {
		/* Prepare block J_0 = IV || 0^31 || 1 [len(IV) = 96] */
		memcpy(J0, (u8 *) iv, iv_len);
		_memset(J0 + iv_len, 0, AES_BLOCK_SIZE - iv_len);
		J0[AES_BLOCK_SIZE - 1] = 0x01;
	} else {
		/*
		 * s = 128 * ceil(len(IV)/128) - len(IV)
		 * J_0 = GHASH_H(IV || 0^(s+64) || [len(IV)]_64)
		 */
		ghash_start(J0);
		ghash(H, iv, iv_len, J0);
		WPA_PUT_BE64(len_buf, 0);
		WPA_PUT_BE64(len_buf + 8, iv_len * 8);
		ghash(H, len_buf, sizeof(len_buf), J0);
	}
}

static void aes_gcm_gctr(void *aes, const u8 *J0, const u8 *in, size_t len,
						 u8 *out)
{
	u8 J0inc[AES_BLOCK_SIZE];

	if (len == 0) {
		return;
	}

	memcpy(J0inc, (u8 *) J0, AES_BLOCK_SIZE);
	inc32(J0inc);
	aes_gctr(aes, J0inc, in, len, out);
}

static void aes_gcm_ghash(const u8 *H, const u8 *aad, size_t aad_len,
						  const u8 *crypt, size_t crypt_len, u8 *S)
{
	u8 len_buf[16];

	/*
	 * u = 128 * ceil[len(C)/128] - len(C)
	 * v = 128 * ceil[len(A)/128] - len(A)
	 * S = GHASH_H(A || 0^v || C || 0^u || [len(A)]64 || [len(C)]64)
	 * (i.e., zero padded to block size A || C and lengths of each in bits)
	 */
	ghash_start(S);
	ghash(H, aad, aad_len, S);
	ghash(H, crypt, crypt_len, S);
	WPA_PUT_BE64(len_buf, aad_len * 8);
	WPA_PUT_BE64(len_buf + 8, crypt_len * 8);
	ghash(H, len_buf, sizeof(len_buf), S);
}

/**
 * aes_gcm_ae - GCM-AE_K(IV, P, A)
 */
static int aes_gcm_ae(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
					  const u8 *plain, size_t plain_len,
					  const u8 *aad, size_t aad_len, u8 *crypt, u8 *tag)
{
	u8 H[AES_BLOCK_SIZE];
	u8 J0[AES_BLOCK_SIZE];
	u8 S[16];
	void *aes;

	aes = aes_gcm_init_hash_subkey(key, key_len, H);
	if (aes == NULL) {
		return -1;
	}

	aes_gcm_prepare_j0(iv, iv_len, H, J0);

	/* C = GCTR_K(inc_32(J_0), P) */
	aes_gcm_gctr(aes, J0, plain, plain_len, crypt);

	aes_gcm_ghash(H, aad, aad_len, crypt, plain_len, S);

	/* T = MSB_t(GCTR_K(J_0, S)) */
	aes_gctr(aes, J0, S, sizeof(S), tag);

	/* Return (C, T) */
	aes_encrypt_deinit(aes);

	return 0;
}

/**
 * aes_gcm_ad - GCM-AD_K(IV, C, A, T)
 */
static int aes_gcm_ad(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
					  const u8 *crypt, size_t crypt_len,
					  const u8 *aad, size_t aad_len, const u8 *tag, u8 *plain)
{
	u8 H[AES_BLOCK_SIZE];
	u8 J0[AES_BLOCK_SIZE];
	u8 S[16], T[16];
	void *aes;

	aes = aes_gcm_init_hash_subkey(key, key_len, H);
	if (aes == NULL) {
		return -1;
	}

	aes_gcm_prepare_j0(iv, iv_len, H, J0);

	/* P = GCTR_K(inc_32(J_0), C) */
	aes_gcm_gctr(aes, J0, crypt, crypt_len, plain);

	aes_gcm_ghash(H, aad, aad_len, crypt, crypt_len, S);

	/* T' = MSB_t(GCTR_K(J_0, S)) */
	aes_gctr(aes, J0, S, sizeof(S), T);

	aes_encrypt_deinit(aes);

	if (os_memcmp_const(tag, T, 16) != 0) {
		dev_err(global_idev.pwhc_dev, "GCM: Tag mismatch\n");
		return -1;
	}

	return 0;
}

static int aes_gmac(const u8 *key, size_t key_len, const u8 *iv, size_t iv_len,
					const u8 *aad, size_t aad_len, u8 *tag)
{
	return aes_gcm_ae(key, key_len, iv, iv_len, NULL, 0, aad, aad_len, NULL,
					  tag);
}

static void gcmp_aad_nonce(const struct rtw_ieee80211_hdr *hdr, const u8 *data,
						   u8 *aad, size_t *aad_len, u8 *nonce)
{
	u16 fc, subtype, seq;
	int qos = 0, addr4 = 0;
	u8 *pos;

	fc = le16_to_cpu(hdr->frame_ctl);
	subtype = GetFrameSubType(hdr);
	if ((fc & (_TO_DS_ | _FROM_DS_)) == (_TO_DS_ | _FROM_DS_)) {
		addr4 = 1;
	}

	if (GetFrameType(hdr) == RTW_DATA_TYPE) {
		fc &= ~0x0070; /* Mask subtype bits */
		if (subtype == RTW_QOS_DATA_TYPE) {
			const u8 *qc;
			qos = 1;
			fc &= ~_ORDER_;
			qc = (const u8 *)hdr + 24;
			if (addr4) {
				qc += ETH_ALEN;
			}
		}
	}

	fc &= ~(_RETRY_ | _PWRMGT_ | _MORE_DATA_);
	WPA_PUT_LE16(aad, fc);
	pos = (u8 *)(aad + 2);
	memcpy(pos, (void *)hdr->addr1, 3 * ETH_ALEN);
	memcpy(pos + ETH_ALEN, (void *)hdr->addr2, ETH_ALEN);
	memcpy(pos + 2 * ETH_ALEN, (void *)hdr->addr3, ETH_ALEN);
	pos += 3 * ETH_ALEN;
	seq = le16_to_cpu(hdr->seq_ctl);
	seq &= ~0xfff0; /* Mask Seq#; do not modify Frag# */
	WPA_PUT_LE16(pos, seq);
	pos += 2;

	memcpy(pos, (u8 *)hdr + 24, addr4 * ETH_ALEN + qos * 2);
	pos += addr4 * ETH_ALEN;
	if (qos) {
		pos[0] &= ~0x70;
		if (1 /* FIX: either device has SPP A-MSDU Capab = 0 */) {
			pos[0] &= ~0x80;
		}
		pos++;
		*pos++ = 0x00;
	}

	*aad_len = pos - aad;

	memcpy(nonce, (void *)hdr->addr2, ETH_ALEN);
	nonce[6] = data[7]; /* PN5 */
	nonce[7] = data[6]; /* PN4 */
	nonce[8] = data[5]; /* PN3 */
	nonce[9] = data[4]; /* PN2 */
	nonce[10] = data[1]; /* PN1 */
	nonce[11] = data[0]; /* PN0 */
}

/**
 * gcmp_decrypt -
 * @tk: the temporal key
 * @tk_len: length of @tk
 * @hdr: the mac header
 * @data: payload after mac header (PN + enc_data + MIC)
 * @data_len: length of @data (PN + enc_data + MIC)
 * @decrypted_len: length of the data decrypted
 */
static u8 *gcmp_decrypt(const u8 *tk, size_t tk_len, const struct rtw_ieee80211_hdr *hdr,
						const u8 *data, size_t data_len, size_t *decrypted_len, size_t hdrlen)
{
	(void) hdrlen;
	u8 aad[30], nonce[12], *plain;
	size_t aad_len, mlen;
	const u8 *m;

	if (data_len < 8 + 16) {
		return NULL;
	}

	plain = kzalloc(data_len + AES_BLOCK_SIZE, GFP_ATOMIC);
	if (plain == NULL) {
		return NULL;
	}

	m = data + 8;
	mlen = data_len - 8 - 16;

	_memset(aad, 0, sizeof(aad));
	gcmp_aad_nonce(hdr, data, aad, &aad_len, nonce);

	if (aes_gcm_ad(tk, tk_len, nonce, sizeof(nonce), m, mlen, aad, aad_len,
				   m + mlen, plain) < 0) {
		kfree(plain);
		return NULL;
	}

	*decrypted_len = mlen;
	return plain;
}

/**
 * gcmp_encrypt -
 * @tk: the temporal key
 * @tk_len: length of @tk
 * @frame: the point to mac header, the frame including mac header and payload,
 *         if @pn is NULL, then the frame including pn
 * @len: length of @frame
 *         length = mac header + payload
 * @hdrlen: length of the mac header
 * @qos: pointer to the QOS field of the frame
 * @pn: packet number
 * @keyid: key id
 * @encrypted_len: length of the encrypted frame
 *                 including mac header, pn, payload and MIC
 */
static u8 *gcmp_encrypt(const u8 *tk, size_t tk_len, const u8 *frame, size_t len,
						size_t hdrlen, const u8 *qos,
						const u8 *pn, int keyid, size_t *encrypted_len)
{
	(void) qos;
	u8 aad[30], nonce[12], *crypt, *pos;
	const u8 *pdata;
	size_t aad_len, plen;
	struct rtw_ieee80211_hdr *hdr;

	if (len < hdrlen || hdrlen < 24) {
		return NULL;
	}
	plen = len - hdrlen;

	crypt = kzalloc(hdrlen + 8 + plen + 16 + AES_BLOCK_SIZE, GFP_ATOMIC);
	if (crypt == NULL) {
		return NULL;
	}

	if (pn == NULL) {
		memcpy(crypt, (void *)frame, hdrlen + 8);
		hdr = (struct rtw_ieee80211_hdr *)crypt;
		pos = crypt + hdrlen + 8;
		pdata = frame + hdrlen + 8;
	} else {
		memcpy(crypt, (void *)frame, hdrlen);
		hdr = (struct rtw_ieee80211_hdr *)crypt;
		pos = crypt + hdrlen;

		*pos++ = pn[5]; /* PN0 */
		*pos++ = pn[4]; /* PN1 */
		*pos++ = 0x00; /* Rsvd */
		*pos++ = 0x20 | (keyid << 6);
		*pos++ = pn[3]; /* PN2 */
		*pos++ = pn[2]; /* PN3 */
		*pos++ = pn[1]; /* PN4 */
		*pos++ = pn[0]; /* PN5 */
		pdata = frame + hdrlen;
	}

	_memset(aad, 0, sizeof(aad));
	gcmp_aad_nonce(hdr, crypt + hdrlen, aad, &aad_len, nonce);

	if (aes_gcm_ae(tk, tk_len, nonce, sizeof(nonce), pdata, plen,
				   aad, aad_len, pos, pos + plen) < 0) { // nonce as iv
		kfree(crypt);
		return NULL;
	}

	*encrypted_len = hdrlen + 8 + plen + 16;

	return crypt;
}

/**
 * gcmp_80211_encrypt -
 * @key: the temporal key
 * @wlan_hdr_len: mac header length
 * @pframe: the frame including the mac header, pn and payload
 * @payload_len: payload length, i.e., length of the plain text, without PN and MIC
 * @key_len: the length of key
 */
u32 gcmp_80211_encrypt(u8 *pframe, u32 wlan_hdr_len, u32 payload_len, u8 *key, u32 key_len)
{
	u8 *enc = NULL;
	size_t enc_len = 0;

	enc = gcmp_encrypt(key, key_len,
					   (const u8 *)pframe,
					   wlan_hdr_len + payload_len,
					   wlan_hdr_len,
					   (wlan_hdr_len == 26) ? (pframe + wlan_hdr_len - 2) : NULL,
					   NULL, 0, &enc_len);
	if (enc == NULL) {
		dev_err(global_idev.pwhc_dev, "Failed to encrypt GCMP frame\n");
		return 0;
	}

	/* Copy @enc back to @frame and free @enc */
	memcpy(pframe, enc, enc_len);

	kfree(enc);

	return 1;
}

/**
 * gcmp_80211_decrypt -
 * @key: the temporal key
 * @wlan_hdr_len: length of the mac header
 * @pframe: the raw frame (@hdrlen + PN + enc_data + MIC)
 * @frame_len: length of the frame (@hdrlen + PN + enc_data + MIC)
 * @key_len: the length of key
 */
u32 gcmp_80211_decrypt(u8 *pframe, u32 wlan_hdr_len, u32 frame_len, u8 *key, u32 key_len)
{
	u8 *plain = NULL;
	size_t plain_len = 0;
	const struct rtw_ieee80211_hdr *hdr = NULL;

	hdr = (const struct rtw_ieee80211_hdr *)pframe;

	plain = gcmp_decrypt(key, key_len,
						 hdr,
						 pframe + wlan_hdr_len, /* PN + enc_data + MIC */
						 frame_len - wlan_hdr_len, /* PN + enc_data + MIC */
						 &plain_len, wlan_hdr_len);

	if (plain == NULL) {
		dev_err(global_idev.pwhc_dev, "Failed to decrypt GCMP(%u) frame\n", key_len);
		return 0;
	}

	/* Copy @plain back to @frame and free @plain */
	memcpy(pframe + wlan_hdr_len + 8, plain, plain_len); // +8? llc header
	kfree(plain);

	return 1;
}

#endif
