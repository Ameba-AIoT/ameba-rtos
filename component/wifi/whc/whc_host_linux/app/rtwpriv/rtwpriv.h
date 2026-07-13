/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#ifndef __FEATURE
#define __FEATURE
#include <stdio.h>
#include <stdlib.h>
#include <linux/sockios.h>	/* SIOCDEVPRIVATE */
#include <errno.h>
#include <string.h>		/* for memcpy() et al */
#include <unistd.h>		/* for close() */
#include <sys/socket.h>		/* for "struct sockaddr" et al  */
#include <sys/ioctl.h>		/* for ioctl() */
#include <linux/wireless.h>	/* for "struct iwreq" et al */
#include <math.h>
#include <assert.h>
#include <netinet/in.h>
#include <cctype>
#include <arpa/inet.h>
#include <sys/wait.h>


#define RTWPRIVE_VERSION "V6.1.0_20260706"

#define RTW_IOCTL_MP SIOCDEVPRIVATE
//#define RTWDEBUG
#ifdef RTWDEBUG
#define DBG printf
#else
#define DBG(x,...) do{}while(0)
#endif

typedef unsigned char UCHAR;
typedef unsigned char BOOL, *PBOOL;
typedef unsigned short USHORT;
typedef unsigned int	UINT;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRUE	1
#define FALSE	0

#define DbgPrint	printf
#define PRINT_DATA(_TitleString, _HexData, _HexDataLen)	\
do {	\
	char *szTitle = _TitleString;	\
	u8 *pbtHexData = _HexData;	\
	u32 u4bHexDataLen = _HexDataLen;	\
	u32 __i;	\
	printf("%s", szTitle);	\
	for (__i = 0 ;__i < u4bHexDataLen; __i++)	\
	{	\
		if ((__i & 15) == 0)	\
		{	\
			printf("\n");	\
		}	\
		printf("%02X%s", pbtHexData[__i], ( ((__i&3)==3) ? "  " : " ") ); \
	}	\
	printf("\n");	\
} while (0);

typedef enum _ANTENNA_PATH {
	ANTENNA_NONE 	= 0,
	ANTENNA_D		= 1,
	ANTENNA_C		= 2,
	ANTENNA_CD	= 3,
	ANTENNA_B		= 4,
	ANTENNA_BD	= 5,
	ANTENNA_BC	= 6,
	ANTENNA_BCD	= 7,
	ANTENNA_A		= 8,
	ANTENNA_AD	= 9,
	ANTENNA_AC	= 10,
	ANTENNA_ACD	= 11,
	ANTENNA_AB	= 12,
	ANTENNA_ABD	= 13,
	ANTENNA_ABC	= 14,
	ANTENNA_ABCD	= 15
} ANTENNA_PATH;


typedef enum _TEST_MODE {
	TEST_NONE,
	PACKETS_TX,
	PACKETS_RX,
	CONTINUOUS_TX,
	OFDM_Single_Tone_TX,
	CCK_Carrier_Suppression_TX
} TEST_MODE;

typedef enum _MPT_RATE {
	/* CCK rate. Total: 4*/
	RATE_CCK_1M = 1,
	RATE_CCK_2M,
	RATE_CCK_55M,
	RATE_CCK_11M,

	/* OFDM rate. Total: 8*/
	RATE_OFDM_6M,       //#5
	RATE_OFDM_9M,
	RATE_OFDM_12M,
	RATE_OFDM_18M,
	RATE_OFDM_24M,
	RATE_OFDM_36M,      //#10
	RATE_OFDM_48M,
	RATE_OFDM_54M,

	/* HT rate.  Total: 16*/
	RATE_MCS0,
	RATE_MCS1,
	RATE_MCS2,          //#15
	RATE_MCS3,
	RATE_MCS4,
	RATE_MCS5,
	RATE_MCS6,
	RATE_MCS7,          //#20
	RATE_MCS8,
	RATE_MCS9,
	RATE_MCS10,
	RATE_MCS11,
	RATE_MCS12,         //#25
	RATE_MCS13,
	RATE_MCS14,
	RATE_MCS15,
	RATE_MCS16,
	RATE_MCS17,         //#30
	RATE_MCS18,
	RATE_MCS19,
	RATE_MCS20,
	RATE_MCS21,
	RATE_MCS22,         //#35
	RATE_MCS23,
	RATE_MCS24,
	RATE_MCS25,
	RATE_MCS26,
	RATE_MCS27,         //#40
	RATE_MCS28,
	RATE_MCS29,
	RATE_MCS30,
	RATE_MCS31,


	/* VHT rate. Total: 20*/
	RATE_VHT1SS_MCS0 = 100, // To reserve MCS16~MCS31, the index starts from #100.
	RATE_VHT1SS_MCS1,   //#101
	RATE_VHT1SS_MCS2,
	RATE_VHT1SS_MCS3,
	RATE_VHT1SS_MCS4,
	RATE_VHT1SS_MCS5,
	RATE_VHT1SS_MCS6,   //#106
	RATE_VHT1SS_MCS7,
	RATE_VHT1SS_MCS8,
	RATE_VHT1SS_MCS9,
	RATE_VHT2SS_MCS0,
	RATE_VHT2SS_MCS1,   //#111
	RATE_VHT2SS_MCS2,
	RATE_VHT2SS_MCS3,
	RATE_VHT2SS_MCS4,
	RATE_VHT2SS_MCS5,
	RATE_VHT2SS_MCS6,   //#116
	RATE_VHT2SS_MCS7,
	RATE_VHT2SS_MCS8,
	RATE_VHT2SS_MCS9,
	RATE_VHT3SS_MCS0,
	RATE_VHT3SS_MCS1,	// #121
	RATE_VHT3SS_MCS2,
	RATE_VHT3SS_MCS3,
	RATE_VHT3SS_MCS4,
	RATE_VHT3SS_MCS5,
	RATE_VHT3SS_MCS6,	// #126
	RATE_VHT3SS_MCS7,
	RATE_VHT3SS_MCS8,
	RATE_VHT3SS_MCS9,
	RATE_VHT4SS_MCS0,
	RATE_VHT4SS_MCS1,	// #131
	RATE_VHT4SS_MCS2,
	RATE_VHT4SS_MCS3,
	RATE_VHT4SS_MCS4,
	RATE_VHT4SS_MCS5,
	RATE_VHT4SS_MCS6,	// #136
	RATE_VHT4SS_MCS7,
	RATE_VHT4SS_MCS8,
	RATE_VHT4SS_MCS9,
	RATE_LAST

} MPT_RATE_E, *PMPT_RATE_E;

/* CCK Rates, TxHT = 0 */
#define DESC_RATE1M					0x00
#define DESC_RATE2M					0x01
#define DESC_RATE5_5M				0x02
#define DESC_RATE11M				0x03

/* OFDM Rates, TxHT = 0 */
#define DESC_RATE6M					0x04
#define DESC_RATE9M					0x05
#define DESC_RATE12M				0x06
#define DESC_RATE18M				0x07
#define DESC_RATE24M				0x08
#define DESC_RATE36M				0x09
#define DESC_RATE48M				0x0a
#define DESC_RATE54M				0x0b

/* MCS Rates, TxHT = 1 */
#define DESC_RATEMCS0				0x0c
#define DESC_RATEMCS1				0x0d
#define DESC_RATEMCS2				0x0e
#define DESC_RATEMCS3				0x0f
#define DESC_RATEMCS4				0x10
#define DESC_RATEMCS5				0x11
#define DESC_RATEMCS6				0x12
#define DESC_RATEMCS7				0x13
#define DESC_RATEMCS8				0x14
#define DESC_RATEMCS9				0x15
#define DESC_RATEMCS10				0x16
#define DESC_RATEMCS11				0x17
#define DESC_RATEMCS12				0x18
#define DESC_RATEMCS13				0x19
#define DESC_RATEMCS14				0x1a
#define DESC_RATEMCS15				0x1b
#define DESC_RATEMCS16				0x1C
#define DESC_RATEMCS17				0x1D
#define DESC_RATEMCS18				0x1E
#define DESC_RATEMCS19				0x1F
#define DESC_RATEMCS20				0x20
#define DESC_RATEMCS21				0x21
#define DESC_RATEMCS22				0x22
#define DESC_RATEMCS23				0x23
#define DESC_RATEMCS24				0x24
#define DESC_RATEMCS25				0x25
#define DESC_RATEMCS26				0x26
#define DESC_RATEMCS27				0x27
#define DESC_RATEMCS28				0x28
#define DESC_RATEMCS29				0x29
#define DESC_RATEMCS30				0x2A
#define DESC_RATEMCS31				0x2B
#define DESC_RATEVHTSS1MCS0		0x2C
#define DESC_RATEVHTSS1MCS1		0x2D
#define DESC_RATEVHTSS1MCS2		0x2E
#define DESC_RATEVHTSS1MCS3		0x2F
#define DESC_RATEVHTSS1MCS4		0x30
#define DESC_RATEVHTSS1MCS5		0x31
#define DESC_RATEVHTSS1MCS6		0x32
#define DESC_RATEVHTSS1MCS7		0x33
#define DESC_RATEVHTSS1MCS8		0x34
#define DESC_RATEVHTSS1MCS9		0x35
#define DESC_RATEVHTSS2MCS0		0x36
#define DESC_RATEVHTSS2MCS1		0x37
#define DESC_RATEVHTSS2MCS2		0x38
#define DESC_RATEVHTSS2MCS3		0x39
#define DESC_RATEVHTSS2MCS4		0x3A
#define DESC_RATEVHTSS2MCS5		0x3B
#define DESC_RATEVHTSS2MCS6		0x3C
#define DESC_RATEVHTSS2MCS7		0x3D
#define DESC_RATEVHTSS2MCS8		0x3E
#define DESC_RATEVHTSS2MCS9		0x3F
#define DESC_RATEVHTSS3MCS0		0x40
#define DESC_RATEVHTSS3MCS1		0x41
#define DESC_RATEVHTSS3MCS2		0x42
#define DESC_RATEVHTSS3MCS3		0x43
#define DESC_RATEVHTSS3MCS4		0x44
#define DESC_RATEVHTSS3MCS5		0x45
#define DESC_RATEVHTSS3MCS6		0x46
#define DESC_RATEVHTSS3MCS7		0x47
#define DESC_RATEVHTSS3MCS8		0x48
#define DESC_RATEVHTSS3MCS9		0x49
#define DESC_RATEVHTSS4MCS0		0x4A
#define DESC_RATEVHTSS4MCS1		0x4B
#define DESC_RATEVHTSS4MCS2		0x4C
#define DESC_RATEVHTSS4MCS3		0x4D
#define DESC_RATEVHTSS4MCS4		0x4E
#define DESC_RATEVHTSS4MCS5		0x4F
#define DESC_RATEVHTSS4MCS6		0x50
#define DESC_RATEVHTSS4MCS7		0x51
#define DESC_RATEVHTSS4MCS8		0x52
#define DESC_RATEVHTSS4MCS9		0x53


#define IS_CCK_RATE(_value)     (RATE_CCK_1M <= _value && _value <= RATE_CCK_11M)
#define IS_OFDM_RATE(_value)    (RATE_OFDM_6M <= _value && _value <= RATE_OFDM_54M)
#define IS_HT_RATE(_value)     (RATE_MCS0 <= _value && _value <= RATE_MCS31)
#define IS_HT_1S_RATE(_value)  (RATE_MCS0 <= _value && _value <= RATE_MCS7)
#define IS_HT_2S_RATE(_value)  (RATE_MCS8 <= _value && _value <= RATE_MCS15)
#define IS_HT_3S_RATE(_value)  (RATE_MCS16 <= _value && _value <= RATE_MCS23)
#define IS_HT_4S_RATE(_value)  (RATE_MCS24 <= _value && _value <= RATE_MCS31)

#define IS_VHT_RATE(_value)     (RATE_VHT1SS_MCS0 <= _value && _value <= RATE_VHT4SS_MCS9)
#define IS_VHT_1S_RATE(_value)  (RATE_VHT1SS_MCS0 <= _value && _value <= RATE_VHT1SS_MCS9)
#define IS_VHT_2S_RATE(_value)  (RATE_VHT2SS_MCS0 <= _value && _value <= RATE_VHT2SS_MCS9)
#define IS_VHT_3S_RATE(_value)  (RATE_VHT3SS_MCS0 <= _value && _value <= RATE_VHT3SS_MCS9)
#define IS_VHT_4S_RATE(_value)  (RATE_VHT4SS_MCS0 <= _value && _value <= RATE_VHT4SS_MCS9)

#define IS_2SS_RATE(_rate) ((RATE_MCS8 <= _rate && _rate <= RATE_MCS15) ||\
							(RATE_VHT2SS_MCS0<= _rate && _rate <= RATE_VHT2SS_MCS9))
#define IS_3SS_RATE(_rate) ((RATE_MCS16 <= _rate && _rate <= RATE_MCS23) ||\
							(RATE_VHT3SS_MCS0<= _rate && _rate <= RATE_VHT3SS_MCS9))
#define IS_4SS_RATE(_rate) ((RATE_MCS24 <= _rate && _rate <= RATE_MCS31) ||\
							(RATE_VHT4SS_MCS0<= _rate && _rate <= RATE_VHT4SS_MCS9))

// Add Declarations here.
typedef struct _RT_PMAC_TX_INFO {
	UCHAR			bEnPMacTx: 1;		// 0: Disable PMac 1: Enable PMac
	UCHAR			Mode: 3;				// 0: Packet TX 3:Continuous TX
	UCHAR			Ntx: 4;				// 0-7
	UCHAR			TX_RATE;			// MPT_RATE_E
	UCHAR			TX_RATE_HEX;
	UCHAR			TX_SC;
	UCHAR			bSGI: 1;
	UCHAR			bSPreamble: 1;
	UCHAR			bSTBC: 1;
	UCHAR			bLDPC: 1;
	UCHAR			NDP_sound: 1;
	UCHAR			BandWidth: 3;		// 0: 20 1:40 2:80Mhz
	UCHAR			m_STBC;				// bSTBC + 1
	USHORT			PacketPeriod;
	UINT			PacketCount;
	UINT			PacketLength;
	UCHAR			PacketPattern;
	USHORT			SFD;
	UCHAR			SignalField;
	UCHAR			ServiceField;
	USHORT			LENGTH;
	UCHAR			CRC16[2];
	UCHAR			LSIG[3];
	UCHAR			HT_SIG[6];
	UCHAR			VHT_SIG_A[6];
	UCHAR			VHT_SIG_B[4];
	UCHAR			VHT_SIG_B_CRC;
	UCHAR			VHT_Delimiter[4];
	UCHAR			MacAddress[6];
} RT_PMAC_TX_INFO, *PRT_PMAC_TX_INFO;


typedef struct _RT_PMAC_PKT_INFO {
	UCHAR			MCS;
	UCHAR			Nss;
	UCHAR			Nsts;
	UINT			N_sym;
	UCHAR			SIGA2B3;
} RT_PMAC_PKT_INFO, *PRT_PMAC_PKT_INFO;


//RT_PMAC_TX_INFO	MyPMacTxInfo2;
#define BUF_SIZE 85535



#endif
