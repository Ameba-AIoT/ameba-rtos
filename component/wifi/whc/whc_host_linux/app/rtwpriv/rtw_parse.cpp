/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 *****************************************************************************/

#include "rtwpriv.h"
#include "rtw_api.h"
const char *HW_rateindex_Array[] = { "1M", "2M", "5.5M", "11M", "6M", "9M", "12M", "18M", "24M", "36M", "48M", "54M",
									 "HTMCS0", "HTMCS1", "HTMCS2", "HTMCS3", "HTMCS4", "HTMCS5", "HTMCS6", "HTMCS7",
									 "HTMCS8", "HTMCS9", "HTMCS10", "HTMCS11", "HTMCS12", "HTMCS13", "HTMCS14", "HTMCS15",
									 "HTMCS16", "HTMCS17", "HTMCS18", "HTMCS19", "HTMCS20", "HTMCS21", "HTMCS22", "HTMCS23",
									 "HTMCS24", "HTMCS25", "HTMCS26", "HTMCS27", "HTMCS28", "HTMCS29", "HTMCS30", "HTMCS31",
									 "VHT1MCS0", "VHT1MCS1", "VHT1MCS2", "VHT1MCS3", "VHT1MCS4", "VHT1MCS5", "VHT1MCS6", "VHT1MCS7", "VHT1MCS8", "VHT1MCS9",
									 "VHT2MCS0", "VHT2MCS1", "VHT2MCS2", "VHT2MCS3", "VHT2MCS4", "VHT2MCS5", "VHT2MCS6", "VHT2MCS7", "VHT2MCS8", "VHT2MCS9",
									 "VHT3MCS0", "VHT3MCS1", "VHT3MCS2", "VHT3MCS3", "VHT3MCS4", "VHT3MCS5", "VHT3MCS6", "VHT3MCS7", "VHT3MCS8", "VHT3MCS9",
									 "VHT4MCS0", "VHT4MCS1", "VHT4MCS2", "VHT4MCS3", "VHT4MCS4", "VHT4MCS5", "VHT4MCS6", "VHT4MCS7", "VHT4MCS8", "VHT4MCS9",
									 "HE1MCS0", "HE1MCS1", "HE1MCS2", "HE1MCS3", "HE1MCS4", "HE1MCS5",
									 "HE1MCS6", "HE1MCS7", "HE1MCS8", "HE1MCS9", "HE1MCS10", "HE1MCS11",
									 "HE2MCS0", "HE2MCS1", "HE2MCS2", "HE2MCS3", "HE2MCS4", "HE2MCS5",
									 "HE2MCS6", "HE2MCS7", "HE2MCS8", "HE2MCS9", "HE2MCS10", "HE2MCS11",
									 "HE3MCS0", "HE3MCS1", "HE3MCS2", "HE3MCS3", "HE3MCS4", "HE3MCS5",
									 "HE3MCS6", "HE3MCS7", "HE3MCS8", "HE3MCS9", "HE3MCS10", "HE3MCS11",
									 "HE4MCS0", "HE4MCS1", "HE4MCS2", "HE4MCS3", "HE4MCS4", "HE4MCS5",
									 "HE4MCS6", "HE4MCS7", "HE4MCS8", "HE4MCS9", "HE4MCS10", "HE4MCS11"
								   };
u8 key_char2num(u8 ch)
{
	if ((ch >= '0') && (ch <= '9')) {
		return ch - '0';
	} else if ((ch >= 'a') && (ch <= 'f')) {
		return ch - 'a' + 10;
	} else if ((ch >= 'A') && (ch <= 'F')) {
		return ch - 'A' + 10;
	} else {
		return 0xff;
	}
}

u8 key_2char2num(u8 hch, u8 lch)
{
	return ((key_char2num(hch) << 4) | key_char2num(lch));
}

void macstr2num(u8 *dst, u8 *src)
{
	int	jj, kk;
	for (jj = 0, kk = 0; jj < 6; jj++, kk += 3) {
		dst[jj] = key_2char2num(src[kk], src[kk + 1]);
	}
}

void dump_buf(char *buf, int len)
{
	int i;
	printf("-----------------Len %d----------------\n", len);
	for (i = 0; i < len; i++) {
		printf("%2.2x-", *(buf + i));
	}
	printf("\n");
}

int randInRange(int min, int max)
{
	double scale = 1.0 / ((double)RAND_MAX + 1);
	double range = max - min + 1;
	return min + (int)(rand() * scale * range);
}

UCHAR randomByte()
{
	return (UCHAR) randInRange(0, 255);
}

void split(char **arr, char *str, char *del)
{
	char *s = strtok(str, del);
	while (s != NULL) {
		*arr++ = s;
		s = strtok(NULL, del);
	}
}

int wifirate2_ratetbl_inx(int rate)
{
	int	inx = 0;
	rate = rate & 0x7f;
	switch (rate) {
	case 54*2:
		inx = 11;
		break;
	case 48*2:
		inx = 10;
		break;
	case 36*2:
		inx = 9;
		break;
	case 24*2:
		inx = 8;
		break;
	case 18*2:
		inx = 7;
		break;
	case 12*2:
		inx = 6;
		break;
	case 9*2:
		inx = 5;
		break;
	case 6*2:
		inx = 4;
		break;
	case 11*2:
		inx = 3;
		break;
	case 11:
		inx = 2;
		break;
	case 2*2:
		inx = 1;
		break;
	case 1*2:
		inx = 0;
		break;
	}
	return inx;
}

u8 HwRateToMPTRate(u8 rate)
{
	u8	ret_rate = RATE_CCK_1M;
	switch (rate) {
	case DESC_RATE1M:
		ret_rate = RATE_CCK_1M;
		break;
	case DESC_RATE2M:
		ret_rate = RATE_CCK_2M;
		break;
	case DESC_RATE5_5M:
		ret_rate = RATE_CCK_55M;
		break;
	case DESC_RATE11M:
		ret_rate = RATE_CCK_11M;
		break;
	case DESC_RATE6M:
		ret_rate = RATE_OFDM_6M;
		break;
	case DESC_RATE9M:
		ret_rate = RATE_OFDM_9M;
		break;
	case DESC_RATE12M:
		ret_rate = RATE_OFDM_12M;
		break;
	case DESC_RATE18M:
		ret_rate = RATE_OFDM_18M;
		break;
	case DESC_RATE24M:
		ret_rate = RATE_OFDM_24M;
		break;
	case DESC_RATE36M:
		ret_rate = RATE_OFDM_36M;
		break;
	case DESC_RATE48M:
		ret_rate = RATE_OFDM_48M;
		break;
	case DESC_RATE54M:
		ret_rate = RATE_OFDM_54M;
		break;
	case DESC_RATEMCS0:
		ret_rate = RATE_MCS0;
		break;
	case DESC_RATEMCS1:
		ret_rate = RATE_MCS1;
		break;
	case DESC_RATEMCS2:
		ret_rate = RATE_MCS2;
		break;
	case DESC_RATEMCS3:
		ret_rate = RATE_MCS3;
		break;
	case DESC_RATEMCS4:
		ret_rate = RATE_MCS4;
		break;
	case DESC_RATEMCS5:
		ret_rate = RATE_MCS5;
		break;
	case DESC_RATEMCS6:
		ret_rate = RATE_MCS6;
		break;
	case DESC_RATEMCS7:
		ret_rate = RATE_MCS7;
		break;
	case DESC_RATEMCS8:
		ret_rate = RATE_MCS8;
		break;
	case DESC_RATEMCS9:
		ret_rate = RATE_MCS9;
		break;
	case DESC_RATEMCS10:
		ret_rate = RATE_MCS10;
		break;
	case DESC_RATEMCS11:
		ret_rate = RATE_MCS11;
		break;
	case DESC_RATEMCS12:
		ret_rate = RATE_MCS12;
		break;
	case DESC_RATEMCS13:
		ret_rate = RATE_MCS13;
		break;
	case DESC_RATEMCS14:
		ret_rate = RATE_MCS14;
		break;
	case DESC_RATEMCS15:
		ret_rate = RATE_MCS15;
		break;
	case DESC_RATEMCS16:
		ret_rate = RATE_MCS16;
		break;
	case DESC_RATEMCS17:
		ret_rate = RATE_MCS17;
		break;
	case DESC_RATEMCS18:
		ret_rate = RATE_MCS18;
		break;
	case DESC_RATEMCS19:
		ret_rate = RATE_MCS19;
		break;
	case DESC_RATEMCS20:
		ret_rate = RATE_MCS20;
		break;
	case DESC_RATEMCS21:
		ret_rate = RATE_MCS21;
		break;
	case DESC_RATEMCS22:
		ret_rate = RATE_MCS22;
		break;
	case DESC_RATEMCS23:
		ret_rate = RATE_MCS23;
		break;
	case DESC_RATEMCS24:
		ret_rate = RATE_MCS24;
		break;
	case DESC_RATEMCS25:
		ret_rate = RATE_MCS25;
		break;
	case DESC_RATEMCS26:
		ret_rate = RATE_MCS26;
		break;
	case DESC_RATEMCS27:
		ret_rate = RATE_MCS27;
		break;
	case DESC_RATEMCS28:
		ret_rate = RATE_MCS28;
		break;
	case DESC_RATEMCS29:
		ret_rate = RATE_MCS29;
		break;
	case DESC_RATEMCS30:
		ret_rate = RATE_MCS30;
		break;
	case DESC_RATEMCS31:
		ret_rate = RATE_MCS31;
		break;
	case DESC_RATEVHTSS1MCS0:
		ret_rate = RATE_VHT1SS_MCS0;
		break;
	case DESC_RATEVHTSS1MCS1:
		ret_rate = RATE_VHT1SS_MCS1;
		break;
	case DESC_RATEVHTSS1MCS2:
		ret_rate = RATE_VHT1SS_MCS2;
		break;
	case DESC_RATEVHTSS1MCS3:
		ret_rate = RATE_VHT1SS_MCS3;
		break;
	case DESC_RATEVHTSS1MCS4:
		ret_rate = RATE_VHT1SS_MCS4;
		break;
	case DESC_RATEVHTSS1MCS5:
		ret_rate = RATE_VHT1SS_MCS5;
		break;
	case DESC_RATEVHTSS1MCS6:
		ret_rate = RATE_VHT1SS_MCS6;
		break;
	case DESC_RATEVHTSS1MCS7:
		ret_rate = RATE_VHT1SS_MCS7;
		break;
	case DESC_RATEVHTSS1MCS8:
		ret_rate = RATE_VHT1SS_MCS8;
		break;
	case DESC_RATEVHTSS1MCS9:
		ret_rate = RATE_VHT1SS_MCS9;
		break;
	case DESC_RATEVHTSS2MCS0:
		ret_rate = RATE_VHT2SS_MCS0;
		break;
	case DESC_RATEVHTSS2MCS1:
		ret_rate = RATE_VHT2SS_MCS1;
		break;
	case DESC_RATEVHTSS2MCS2:
		ret_rate = RATE_VHT2SS_MCS2;
		break;
	case DESC_RATEVHTSS2MCS3:
		ret_rate = RATE_VHT2SS_MCS3;
		break;
	case DESC_RATEVHTSS2MCS4:
		ret_rate = RATE_VHT2SS_MCS4;
		break;
	case DESC_RATEVHTSS2MCS5:
		ret_rate = RATE_VHT2SS_MCS5;
		break;
	case DESC_RATEVHTSS2MCS6:
		ret_rate = RATE_VHT2SS_MCS6;
		break;
	case DESC_RATEVHTSS2MCS7:
		ret_rate = RATE_VHT2SS_MCS7;
		break;
	case DESC_RATEVHTSS2MCS8:
		ret_rate = RATE_VHT2SS_MCS8;
		break;
	case DESC_RATEVHTSS2MCS9:
		ret_rate = RATE_VHT2SS_MCS9;
		break;
	case DESC_RATEVHTSS3MCS0:
		ret_rate = RATE_VHT3SS_MCS0;
		break;
	case DESC_RATEVHTSS3MCS1:
		ret_rate = RATE_VHT3SS_MCS1;
		break;
	case DESC_RATEVHTSS3MCS2:
		ret_rate = RATE_VHT3SS_MCS2;
		break;
	case DESC_RATEVHTSS3MCS3:
		ret_rate = RATE_VHT3SS_MCS3;
		break;
	case DESC_RATEVHTSS3MCS4:
		ret_rate = RATE_VHT3SS_MCS4;
		break;
	case DESC_RATEVHTSS3MCS5:
		ret_rate = RATE_VHT3SS_MCS5;
		break;
	case DESC_RATEVHTSS3MCS6:
		ret_rate = RATE_VHT3SS_MCS6;
		break;
	case DESC_RATEVHTSS3MCS7:
		ret_rate = RATE_VHT3SS_MCS7;
		break;
	case DESC_RATEVHTSS3MCS8:
		ret_rate = RATE_VHT3SS_MCS8;
		break;
	case DESC_RATEVHTSS3MCS9:
		ret_rate = RATE_VHT3SS_MCS9;
		break;
	case DESC_RATEVHTSS4MCS0:
		ret_rate = RATE_VHT4SS_MCS0;
		break;
	case DESC_RATEVHTSS4MCS1:
		ret_rate = RATE_VHT4SS_MCS1;
		break;
	case DESC_RATEVHTSS4MCS2:
		ret_rate = RATE_VHT4SS_MCS2;
		break;
	case DESC_RATEVHTSS4MCS3:
		ret_rate = RATE_VHT4SS_MCS3;
		break;
	case DESC_RATEVHTSS4MCS4:
		ret_rate = RATE_VHT4SS_MCS4;
		break;
	case DESC_RATEVHTSS4MCS5:
		ret_rate = RATE_VHT4SS_MCS5;
		break;
	case DESC_RATEVHTSS4MCS6:
		ret_rate = RATE_VHT4SS_MCS6;
		break;
	case DESC_RATEVHTSS4MCS7:
		ret_rate = RATE_VHT4SS_MCS7;
		break;
	case DESC_RATEVHTSS4MCS8:
		ret_rate = RATE_VHT4SS_MCS8;
		break;
	case DESC_RATEVHTSS4MCS9:
		ret_rate = RATE_VHT4SS_MCS9;
		break;
	default:
		printf("hw_rate_to_m_rate(): Non supported Rate [%x]!!!\n", rate);
		break;
	}
	return ret_rate;
}


int rtw_mpRateParseFunc(char *targetStr)
{
	int i = 0;
	int mptRate = 108;
	int rateidx = 0;
	for (i = 0; i <= 83; i++) {
		if (strcmp(targetStr, HW_rateindex_Array[i]) == 0) {
			DBG("index = %d \n", i);
			rateidx = i;
			return rateidx;
		}
	}
	if (rateidx == 0 && strcmp(targetStr, "1M") != 0) {
		mptRate = atoi(targetStr);
		if (mptRate <= 127) {
			rateidx = wifirate2_ratetbl_inx(mptRate);
		} else if (mptRate < 200) {
			rateidx = (mptRate - 128 + 12);
		}
		return rateidx;
	}
	printf("\n	#### Please input a Data RATE String as:######\n");
	for (i = 0; i <= 83; i++) {
		printf("%s ", HW_rateindex_Array[i]);
		if ((i % 10 == 0) && (i != 0)) {
			printf("\n");
		}
	}
	printf("\n Can't find rate index, Press any key to exit & input again...");
	getchar();
	exit(0);
}

u16 rtw_ant_strpars(char *targetStr)
{
	u16 CurrentAntenna;
	if (strcmp(targetStr, "d") == 0) {
		CurrentAntenna = ANTENNA_D;
	} else if (strcmp(targetStr, "c") == 0) {
		CurrentAntenna = ANTENNA_C;
	} else if (strcmp(targetStr, "cd") == 0) {
		CurrentAntenna = ANTENNA_CD;
	} else if (strcmp(targetStr, "b") == 0) {
		CurrentAntenna = ANTENNA_B;
	} else if (strcmp(targetStr, "bd") == 0) {
		CurrentAntenna = ANTENNA_BD;
	} else if (strcmp(targetStr, "bc") == 0) {
		CurrentAntenna = ANTENNA_BC;
	} else if (strcmp(targetStr, "bcd") == 0) {
		CurrentAntenna = ANTENNA_BCD;
	} else if (strcmp(targetStr, "a") == 0) {
		CurrentAntenna = ANTENNA_A;
	} else if (strcmp(targetStr, "ad") == 0) {
		CurrentAntenna = ANTENNA_AD;
	} else if (strcmp(targetStr, "ac") == 0) {
		CurrentAntenna = ANTENNA_AC;
	} else if (strcmp(targetStr, "acd") == 0) {
		CurrentAntenna = ANTENNA_ACD;
	} else if (strcmp(targetStr, "ab") == 0) {
		CurrentAntenna = ANTENNA_AB;
	} else if (strcmp(targetStr, "abd") == 0) {
		CurrentAntenna = ANTENNA_ABD;
	} else if (strcmp(targetStr, "abc") == 0) {
		CurrentAntenna = ANTENNA_ABC;
	} else if (strcmp(targetStr, "abcd") == 0) {
		CurrentAntenna = ANTENNA_ABCD;
	} else {
		CurrentAntenna = ANTENNA_A;
	}
	DBG("%s :Config CurrentAntenna %d\n", __func__, CurrentAntenna);
	return CurrentAntenna;
}


int Read_Parsing_file(PRT_PMAC_TX_INFO pPMacTxInfo2, u16 *Antenna, u8 *UnicastDID)
{
	int pktLength = 1000, pktInterval = 100, pktPattern = 0x00, stbc = 0, ldpc = 0, ndp_sound = 0, SPreambleSGI = 0;
	char line[1024];
	char FileName[] = "rtw_Config.txt";
	FILE *fp = fopen(FileName, "r");
	if (!fp) {
		return 0;
	} else {
		printf("Try to Open %s file , parsing config file !!!\n", FileName);
		while (fgets(line, 1024, fp)) {
			DBG("read Str %s\n", line);
			if (strncmp(line, "ConfigSetting=off", 18) == 0) {
				printf("off parsing config!!!\n");
				return 0;
			} else if (strncmp(line, "PacketLength=", 13) == 0) {
				if (sscanf(line, "	PacketLength=%d", &pktLength) > 0) {
					printf("PacketLength= %d\n", pktLength);
					pPMacTxInfo2->PacketLength = pktLength;
				} else {
					pPMacTxInfo2->PacketLength = 1000;
				}
			} else if (strncmp(line, "PacketPeriod=", 13) == 0) {
				if (sscanf(line, "PacketPeriod=%d", &pktInterval) > 0) {
					printf("PacketPeriod= %d\n", pktInterval);
					pPMacTxInfo2->PacketPeriod = pktInterval;
				} else {
					pPMacTxInfo2->PacketPeriod = 100;
				}
			} else if (strncmp(line, "PacketPattern=", 13) == 0) {
				if (strncmp(line, "PacketPattern=rand", 18) == 0) {
					printf("PacketPattern= rand\n");
					pPMacTxInfo2->PacketPattern = randomByte();
				} else if (sscanf(line, "PacketPattern=%x", &pktPattern) > 0) {
					printf("PacketPattern= 0x%x\n", pktPattern);
					pPMacTxInfo2->PacketPattern = pktPattern;
				} else {
					printf("Unkonw PacketPattern,Defualt Random Pattern\n");
					pPMacTxInfo2->PacketPattern = randomByte();
				}
			} else if (strncmp(line, "LDPC=", 5) == 0) {
				if (sscanf(line, "LDPC=%d", &ldpc) > 0) {
					printf("LDPC= %d\n", ldpc);
					pPMacTxInfo2->bLDPC = ldpc;
				}
			} else if (strncmp(line, "STBC=", 5) == 0) {
				if (sscanf(line, "STBC=%d", &stbc) > 0) {
					printf("STBC= %d\n", stbc);
					pPMacTxInfo2->bSTBC = stbc;
				}
			} else if (strncmp(line, "SPreambleSGI=", 13) == 0) {
				if (sscanf(line, "SPreambleSGI=%d", &SPreambleSGI) > 0) {
					printf("SPreamble & SGI= %d\n", SPreambleSGI);
					pPMacTxInfo2->bSPreamble = SPreambleSGI;
					pPMacTxInfo2->bSGI = SPreambleSGI;
				}
			} else if (strncmp(line, "NDP_sound=", 10) == 0) {
				if (sscanf(line, "NDP_sound=%d", &ndp_sound) > 0) {
					printf("NDP_sound= %d\n", ndp_sound);
					pPMacTxInfo2->NDP_sound = ndp_sound;
				}
			} else if (strncmp(line, "TxAnt=", 6) == 0) {
				const char *delim = "=";
				char *pch;
				u8 i;
				u16 antenna = 0;
				pch = strtok(line, delim);
				pch = strtok(NULL, delim);
				for (i = 0; i < strlen(pch); i++) {
					switch (pch[i]) {
					case 'A':
					case 'a':
						antenna |= ANTENNA_A;
						break;
					case 'B':
					case 'b':
						antenna |= ANTENNA_B;
						break;
					case 'C':
					case 'c':
						antenna |= ANTENNA_C;
						break;
					case 'D':
					case 'd':
						antenna |= ANTENNA_D;
						break;
					}
				}
				*Antenna = antenna;
				printf("%s :Config Current Antenna %d\n", __func__, *Antenna);
			} else if (strncmp(line, "	destaddr=", 10) == 0) {
				const char *delim = "=";
				char *pch;
				//printf ("Splitting string \"%s\" into tokens:\n",line);
				pch = strtok(line, delim);
				pch = strtok(NULL, delim);
				//printf ("%s\n",pch);
				macstr2num(UnicastDID, (u8 *) pch);
			}
		}
		fclose(fp);
		return 1;
	}
	fclose(fp);
	return 0;
}

void rtw_help(void)
{
	printf("#####[ 1. SW TX mode ]#####\n");
	printf("\t Input the parameters like this: rtwpriv wlan0 mp_XXX XXX\n");
	printf("	1. mp_start				## enter to MP Mode\n \
	2. mp_bandwidth 40M=[Num]		## input [Num], 0=20M, 1=40M, 2=80M\n \
	3. mp_channel [Num]			## input [Num] = 1~167 channel number\n \
	4. mp_rate [Rate Id Num] or [RateID]\n");
	printf("	5. mp_ant_tx [Path]			## input Antenna [PARH] = a,b,c,d,ab,ac,ad...\n\
	6. mp_txpower patha=[index],pathb=[index],pathc=[index],pathd=[index] 	## input power index range [index] = 1~63\n\
	7. mp_get_txpower [PATH Num]		## Input Antenna PATH Num 0~3 , A = 0, B = 1 , C = 2 , D = 3\n\
	8. mp_ctx [Tx Mode]			##input [ Tx Mode ]\n\
				[a].Continuous Packet Tx = 'background,pkt'\n\
				[b].Continuous Tx = 'background'\n\
				[c].Count Packet Tx = 'count=[num],pkt'\n\
				[d].Carrier suppression testing = 'background,cs'\n\
				[e].Single Tone TX testing = 'background,stone'\n\
	   mp_ctx stop				## Stop Tx Mode\n\
	9. mp_query				## Get Tx/Rx Packet Counter.\n\n");
	printf("##### [ 2. HW TX mode ] VHT/HE IC Suppport Only #####\n");
	printf("\tInput the parameters like this:\n\trtwpriv wlan0 [Channel] [Bandwidth] [ANT_PAH] [RateID] [TxMode] [Packet Interval] [PacketLength] [Packet Count] [Packet Pattern]\n\
	[Channel]: 1~177\n\
	[BW]: 0 = 20M, 1 = 40M, 2 = 80M\n \
	[ANT_PAH]: a: PATH A, b: PATH B, c: PATH C, d: PATH D, ab : PATH AB 2x2 ....\n\
	[RateID]:\n\t");
	{
		int bg_idx = 12, n_idx = 32, vht_idx = 40;
		int i;

		for (i = 0; i <= 131; i++) {
			if (strncmp(HW_rateindex_Array[i], "HE1MCS0", 7) == 0) {
				printf("\n\n\t[ 80211AX Support Rate Lists]:\n\t");
			} else if (strncmp(HW_rateindex_Array[i], "VHT1MCS0", 8) == 0) {
				printf("\n\t[ 80211AC Support Rate Lists]:\n\t");
			}
			printf("%s ", HW_rateindex_Array[i]);
			if (i == 11) {
				printf("\n\t");
			} else if (i > 11 && i <= 43) {
				int n_cur = i - bg_idx;

				if (n_cur % 16 == 0 && n_cur != 0) {
					printf("\n\t");
				} else if (n_cur == 31) {
					printf("\n\t");
				}
			} else if (i > 43 && i <= 83) {
				int vht_cur = i - (bg_idx + n_idx);

				if (vht_cur % 9 == 0 && vht_cur != 0) {
					printf("\n\t");
				}
			} else if (i > 83) {
				int he_cur = i - (bg_idx + vht_idx + n_idx);
				if (he_cur % 11 == 0 && he_cur != 0) {
					printf("\n\t");
				}
			}
		}
	}
	printf("\n\n	[ TxMode ]: 1: PACKET Tx 2:CONTINUOUS TX 3:OFDM Single Tone TX\n");
	printf("\t[ PacketLength] (Option): Packet of payload data length, default 1500\n");
	printf("\t[ Packet Count] (Option): count the number of packet to Tx , set 0 for CONTINUOUS Packet TX ,default 0\n");
	printf("\t[ Packet Interval] (Option): 1~65535 us,default 2000\n");
	printf("\t[ Packet Pattern] (Option): 00~ff(hex) ,default random hex\n\n");

	printf("\n##### [ 3. (80211AX) HW TX mode ] AX IC Suppport Only #####\n\n");
	printf("\tInput the parameters like this(CMD format):\n\t\
	rtwpriv wlan0 [Channel] [Bandwidth] [ANT_PAH] [RateID] [TxMode] [ru_tone=(int) [ppdu=(int) ru_alloc=(int)]\n\n\
	RU Tone support list(Refer Coding:LDPC):\n\
	RU Tone CMD: [ mp_plcp_data ru_tone=(int) ]\n\
	0 : [26-Tone]\n\
	1 : [52-Tone]\n\
	2 : [106-Tone]\n\
	3 : [242-Tone]\n\
	4 : [484-Tone]\n\
	5 : [966-Tone]\n\n\
	Conifg PPDU Type: 0 = CCK , 1 = LEGACY, 2 = HT_MF, 3 = HT_GF, 4 = VHT, 5 = HE_SU, 6 = HE_ER_SU, 7 = HE_MU_OFDMA, 8 = HE_TB\n\
	Coding CMD:	[mp_plcp_user coding=(int)] ( 0:BCC , 1:LDPC )\n\
	RU Alloc CMD:	[ mp_plcp_user ru_alloc=(int) ]\n\n\
	If support 6G band:\n\
		rtwpriv wlan0 mp_band=[Num]		## input [Num], 0=2G, 1=5G, 2=6G\n");

	printf("\n#####[ 4. RX mode ] #####\n");
	printf("	1. mp_start				## enter to MP Mode\n \
	2. mp_bandwidth 40M=[Num]		## input [Num], 0=20M, 1=40M, 2=80M\n \
	3. mp_channel [Num]			## input [Num] = 1~167 channel number\n \
	5. mp_ant_rx [Path]			## input Antenna [PARH] = a,b,c,d,ab,ac,ad...\n\
	6. mp_arx start				## Enter Rx packet mode\n \
	7. mp_arx phy				## Query Rx Phy Packet Count\n\
	8. mp_reset_stats			## reset all Count\n\n");
	printf("\n#####[ 5. Read/Write RF reg ] #####\n");
	printf("	1. rfr [RF_Path] [Reg_Addr]  ,return hex value, # RF Path= 0/1/2/3 , Reg Addr hex = 0xXX.\n \
	2. rfw [RF_Path] [Reg_Addr] [Value] ,# RF Path= 0/1/2/3 , Reg Addr hex = 0xXX , Value Hex = 0x00000 .\n \
	3. read_rf [RF_Path],[reg_Addr] ,return decimal value ,# RF Path= 0/1/2/3 , Reg Addr hex = 0xXX .\n \
	4. write_rf [RF_Path],[Reg_Addr],[Value] ,# RF Path= 0/1/2/3 , Reg Addr hex = 0xXX , Value Hex = 0x00000 .\n\n");
	printf("\n#####[ 6. Read/Write mac/bb reg ] #####\n");
	printf("	1. read [data width],[Reg_Addr]  ,return hex value, # Data Width= 1/2/4 , Reg Addr hex = 0xXX .\n \
	2. write [data width],[Reg_Add],[Value] ,# Data Width= 1/2/4 , Reg Addr hex = 0xXX , Value Hex = 0x00000 .\n");
}
