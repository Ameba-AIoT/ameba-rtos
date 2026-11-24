/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "rtwpriv.h"
#include "rtw_api.h"
#include "rtw_transport.h"
#include "rtw_hwtx.h"

static char cmdbuf[BUF_SIZE];
static char tmpcmdstr[BUF_SIZE];

static int cmdstr2buf_mp(rtw_xport_ctx_t *ctx, char *orgbuf, char *newcmdstr, bool bfreebuf)
{
	int err;

	snprintf(orgbuf, strlen(newcmdstr) + 1, "%s", newcmdstr);
	DBG("cmdbuf= \"%s\"\n", orgbuf);
	err = rtw_mp_send(ctx, orgbuf, BUF_SIZE, 1);

	memset(newcmdstr, 0, strlen(newcmdstr));
	if (bfreebuf) {
		memset(orgbuf, 0, BUF_SIZE);
	}
	if (err < 0) {
		return err;
	}

	return err;
}

int rtw_hwtx_wants_run(int argc, char *argv[])
{
	if (argc < 3) {
		return 0;
	}

	if (strncmp(argv[2], "stop", 4) == 0) {
		return 1;
	}

	if (argc >= 7) {
		return 1;
	}

	return 0;
}

int rtw_hwtx_run(rtw_xport_ctx_t *ctx, int argc, char *argv[])
{
	PRT_PMAC_TX_INFO pPMacTxInfo;
	RT_PMAC_TX_INFO	MyPMacTxInfo;
	u8 UnicastDID[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 CurrentAntenna;
	u8 PktPattern = 0x00;
	char *ifname;
	int inRate = 11, inBW = 0, inCH = 1, txmode = 1, pktlen = 1500, pktcount = 0, pktPeriod = 0;
	int inBand = 0;
	int err = 0;
	char rate_str[10];
	char ant_str[5];
	char input[BUF_SIZE];
	char copy_input[BUF_SIZE];
	char setmpchar[] = {"mp_hxtx "};
	char setmp_rate[32] = {"mp_rate"};
	char setmp_ch[32] = {"mp_channel"};
	char setmp_bw[32] = {"mp_bandwidth 40M="};
	char setmp_ant[32] = {"mp_ant_tx "};
	char setmp_get_he[10] = {"mp_get_he"};
	char setmp_set_plcp_data_ru_tone[22] = {"mp_plcp_data ru_tone="};
	char setmp_set_plcp_data_ppdu[19] = {"mp_plcp_data ppdu="};
	char setmp_set_plcp_data_ru_alloc[23] = {"mp_plcp_data ru_alloc="};
	UCHAR tmpU1B, Idx;
	int i = 0;
	int ru_tone = -1, ppdu = -1, ru_alloc = -1;

	if (!ctx || ctx->type != RTW_XPORT_IOCTL) {
		fprintf(stderr, "HW TX requires ioctl transport\n");
		return -EINVAL;
	}

	pPMacTxInfo = &MyPMacTxInfo;
	memset(pPMacTxInfo, 0, sizeof(RT_PMAC_TX_INFO));

	pPMacTxInfo->bEnPMacTx = TRUE;

	memset(cmdbuf, '\0', sizeof(cmdbuf));
	memset(tmpcmdstr, '\0', sizeof(tmpcmdstr));

	if (strncmp(argv[2], "stop", 4) == 0) {
		DBG("#####Stop HW TX mode!!!#####\n");
		pPMacTxInfo->bEnPMacTx = FALSE;
	} else if (!isdigit(argv[2][0])) {
		int argc_idx;

		printf("(CMD format):\n\
rtwpriv wlan0 band=(int) ch=(int) bw=(int) ant=[a~d] rate=[str] tx=(int) period=(int) pktlen=(int) pattern=(hex)\n\
\t      ru_tone=(int) ppdu=(int) ru_alloc=(int)\n\n");

		for (argc_idx = 2; argc_idx < argc; argc_idx++) {
			if ((strncmp(argv[argc_idx], "band", 4) == 0)) {
				sscanf(argv[argc_idx], "band=%d", &inBand);
				printf("config band = %d\n", inBand);

			} else if ((strncmp(argv[argc_idx], "ch", 2) == 0)) {
				sscanf(argv[argc_idx], "ch=%d", &inCH);
				printf("config CH = %d\n", inCH);

			} else if ((strncmp(argv[argc_idx], "bw", 2) == 0)) {
				sscanf(argv[argc_idx], "bw=%d", &inBW);
				printf("config BW = %d\n", inBW);

			} else if ((strncmp(argv[argc_idx], "ant", 3) == 0)) {

				sscanf(argv[argc_idx], "ant=%s", ant_str);
				CurrentAntenna = rtw_ant_strpars(ant_str);
				printf("config ANT %s= %d\n", ant_str, CurrentAntenna);

			} else if ((strncmp(argv[argc_idx], "rate", 4) == 0)) {

				sscanf(argv[argc_idx], "rate=%s", rate_str);
				inRate = rtw_mpRateParseFunc(rate_str);
				printf("config Rate %s = %d\n", rate_str, inRate);

			} else if ((strncmp(argv[argc_idx], "tx", 2) == 0)) {
				sscanf(argv[argc_idx], "tx=%d", &txmode);
				printf("config Tx Mode = %d\n", txmode);

			} else if ((strncmp(argv[argc_idx], "period", 6) == 0)) {
				sscanf(argv[argc_idx], "period=%d", &pktPeriod);
				printf("config Tx period = %d\n", pktPeriod);

			} else if ((strncmp(argv[argc_idx], "pktlen", 6) == 0)) {
				sscanf(argv[argc_idx], "pktlen=%d", &pktlen);
				printf("config Tx pktlen = %d\n", pktlen);

			} else if ((strncmp(argv[argc_idx], "pattern", 8) == 0)) {
				char Pattern_str[9];
				sscanf(argv[argc_idx], "pattern=%s", Pattern_str);

				PktPattern  = strtol(Pattern_str, NULL, 16);
				if (PktPattern <= 255) {
					printf("config Pkt Pattern  = %x\n", PktPattern);
				} else {
					PktPattern = 0x00;
					printf("Error not < 255 Pkt Pattern  = %d\n", PktPattern);
				}
			} else if ((strncmp(argv[argc_idx], "ru_tone", 7) == 0)) {
				sscanf(argv[argc_idx], "ru_tone=%d", &ru_tone);
				printf("config ru_tone  = %d\n", ru_tone);
			} else if ((strncmp(argv[argc_idx], "ppdu", 4) == 0)) {
				sscanf(argv[argc_idx], "ppdu=%d", &ppdu);
				printf("config ppdu  = %d\n", ppdu);
			} else if ((strncmp(argv[argc_idx], "ru_alloc", 8) == 0)) {
				sscanf(argv[argc_idx], "ru_alloc=%d", &ru_alloc);
				printf("config ru_alloc  = %d\n", ru_alloc);
			}
		}
	} else {

		if (argc > 2) {
			inCH  = atoi(argv[2]);
		}

		if (argc > 3) {
			inBW  = atoi(argv[3]);
		}
		if (inBW < 0) {
			rtw_help();
			fprintf(stderr, "Interface doesn't accept private ioctl...BW parameter Error, bandwidth\n");
			return -EINVAL;
		}
		if (argc > 4) {
			strncpy(ant_str, argv[4], strlen(argv[4]));
			CurrentAntenna = rtw_ant_strpars(ant_str);
		}
		if (argc > 5) {
			strncpy(rate_str, argv[5], strlen(argv[5]));
			inRate = rtw_mpRateParseFunc(argv[5]);
		}
		if (argc > 6) {
			txmode = atoi(argv[6]);
		} else {
			fprintf(stderr, "Interface doesn't accept private ioctl...No txmode parameter Error\n");
			return -EINVAL;
		}

		if (argc > 7) {
			if ((strncmp(argv[7], "ru_tone", 7) == 0)) {
				sscanf(argv[7], "ru_tone=%d", &ru_tone);
				printf("config ru_tone  = %d\n", ru_tone);
			} else {
				pktPeriod = atoi(argv[7]);
				printf("config packet Period = %d\n", pktPeriod);
			}
		}
		if (argc > 8) {
			if ((strncmp(argv[8], "ppdu", 4) == 0)) {
				sscanf(argv[8], "ppdu=%d", &ppdu);
				printf("config ppdu  = %d\n", ppdu);
			} else {
				pktlen = atoi(argv[8]);
				printf("[config packet length =%d]\n", pktlen);

			}
		}
		if (argc > 9) {
			if ((strncmp(argv[9], "ru_alloc", 8) == 0)) {
				sscanf(argv[9], "ru_alloc=%d", &ru_alloc);
				printf("config ru_alloc  = %d\n", ru_alloc);
			} else {
				pktcount = atoi(argv[9]);
				printf("config packet count =%d\n", pktcount);
			}
		}
		if (argc > 10) {
			int val = atoi(argv[10]);
			if (val <= 255) {
				PktPattern  = strtol(argv[10], NULL, 16);
			} else {
				rtw_help();
				fprintf(stderr, "Interface doesn't accept private ioctl...parameter Error\n");
				return -EINVAL;
			}
		}
	}

	if (txmode >= 4 || txmode <= 0) {
		rtw_help();
		fprintf(stderr, "Interface doesn't accept private ioctl...txmode parameter Error\n");
		return -EINVAL;
	}

	ifname = argv[1];

	if (Read_Parsing_file(&MyPMacTxInfo, &CurrentAntenna, UnicastDID) != 1) {
		u16 umac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		DBG("No rtw_Config.txt File, Use Default Setting for PMACT \n");
		pPMacTxInfo->TX_SC = 0;
		pPMacTxInfo->bSTBC = FALSE;
		pPMacTxInfo->bLDPC = FALSE;
		pPMacTxInfo->bSPreamble = FALSE;
		pPMacTxInfo->bSGI = FALSE;
		pPMacTxInfo->NDP_sound = FALSE;
		pPMacTxInfo->PacketPeriod = pktPeriod;
		if (PktPattern != 0x00) {
			pPMacTxInfo->PacketPattern = PktPattern;
		} else {
			pPMacTxInfo->PacketPattern = randomByte();
		}
		DBG("PacketPattern = %x\n", pPMacTxInfo->PacketPattern);
		pPMacTxInfo->PacketCount = pktcount;
		pPMacTxInfo->PacketLength = pktlen;
		memcpy(pPMacTxInfo->MacAddress, umac, 6);
	}

	for (i = 0; i < argc; i++) {
		DBG("argv[%d] = %s\n", i, argv[i]);
	}

	if (pPMacTxInfo->bEnPMacTx != FALSE) {
		if (PktPattern != 0x00) {
			pPMacTxInfo->PacketPattern = PktPattern;
			printf("Packet Pattern = %x\n", pPMacTxInfo->PacketPattern);
		} else {
			pPMacTxInfo->PacketPattern = randomByte();
			printf("Packet Pattern = random Byte\n");
		}
		pPMacTxInfo->TX_RATE = HwRateToMPTRate(inRate);
		if (inBW > 4 || inBW < 0) {
			pPMacTxInfo->BandWidth = 0;
		} else {
			pPMacTxInfo->BandWidth = inBW;
		}

		if (txmode == 1) {
			pPMacTxInfo->Mode = PACKETS_TX;
		} else if (txmode == 2) {
			pPMacTxInfo->Mode = CONTINUOUS_TX;
		} else if (txmode == 3) {
			pPMacTxInfo->Mode = OFDM_Single_Tone_TX;
		} else {
			pPMacTxInfo->Mode = PACKETS_TX;
		}

		snprintf(tmpcmdstr, sizeof(tmpcmdstr), "%s %d", setmp_ch, inCH);
		DBG("tmpcmdstr  = %s\n", tmpcmdstr);
		err = cmdstr2buf_mp(ctx, cmdbuf, tmpcmdstr, TRUE);
		if (err < 0) {
			return err;
		}

		snprintf(tmpcmdstr, sizeof(tmpcmdstr), "%s%d", setmp_bw, inBW);
		DBG("tmpcmdstr  = %s\n", tmpcmdstr);
		err = cmdstr2buf_mp(ctx, cmdbuf, tmpcmdstr, FALSE);
		if (err < 0) {
			return err;
		}
		DBG("ret buf	= %s\n", cmdbuf);
		{
			int oldbw, newbw;
			if (sscanf(cmdbuf, "Change BW %d to BW %d", &oldbw, &newbw) > 1) {
				if (inBW != newbw) {
					pPMacTxInfo->BandWidth = newbw;
					printf("ret BW config = %s\n", cmdbuf);
				}
			}

			snprintf(tmpcmdstr, sizeof(tmpcmdstr), "%s %d", setmp_ch, inCH);
			DBG("tmpcmdstr  = %s\n", tmpcmdstr);
			err = cmdstr2buf_mp(ctx, cmdbuf, tmpcmdstr, TRUE);
			if (err < 0) {
				return err;
			}

			snprintf(tmpcmdstr, sizeof(tmpcmdstr), "%s%d", setmp_bw, inBW);
			DBG("tmpcmdstr	= %s\n", tmpcmdstr);
			err = cmdstr2buf_mp(ctx, cmdbuf, tmpcmdstr, FALSE);
			if (err < 0) {
				return err;
			}
		}

		snprintf(tmpcmdstr, sizeof(tmpcmdstr), "%s ", setmp_rate);
		strcat(tmpcmdstr, argv[5]);
		DBG("tmpcmdstr  = \"%s\" , strlen=%d\n", tmpcmdstr, strlen(tmpcmdstr));
		err = cmdstr2buf_mp(ctx, cmdbuf, tmpcmdstr, TRUE);
		if (err < 0) {
			return err;
		}

		snprintf(tmpcmdstr, sizeof(tmpcmdstr), "%s%s", setmp_ant, ant_str);
		DBG("tmpcmdstr  = %s\n", tmpcmdstr);
		err = cmdstr2buf_mp(ctx, cmdbuf, tmpcmdstr, TRUE);
		if (err < 0) {
			return err;
		}

		memset(cmdbuf, 0, BUF_SIZE);

		memcpy(pPMacTxInfo->MacAddress, UnicastDID, 6);
		pPMacTxInfo->Ntx = 0;
		for (Idx = 16; Idx < 20; Idx++) {
			tmpU1B = (CurrentAntenna >> Idx) & 1;
			if (tmpU1B) {
				pPMacTxInfo->Ntx++;
			}
		}
		DBG("pPMacTxInfo->Ntx =%d\n", pPMacTxInfo->Ntx);
		memcpy(input, &MyPMacTxInfo, sizeof(RT_PMAC_TX_INFO));
		DBG("SGI %d bSPreamble %d bSTBC %d bLDPC %d NDP_sound %d\n", pPMacTxInfo->bSGI, pPMacTxInfo->bSPreamble, pPMacTxInfo->bSTBC, pPMacTxInfo->bLDPC,
			pPMacTxInfo->NDP_sound);
		DBG("TXSC %d BandWidth %d PacketPeriod %d PacketCount %d PacketLength %d PacketPattern %d\n", pPMacTxInfo->TX_SC, pPMacTxInfo->BandWidth,
			pPMacTxInfo->PacketPeriod, pPMacTxInfo->PacketCount, pPMacTxInfo->PacketLength, pPMacTxInfo->PacketPattern);
#ifdef RTWDEBUG
		PRINT_DATA((char *)"LSIG ", pPMacTxInfo->LSIG, 3);
		PRINT_DATA((char *)"HT_SIG", pPMacTxInfo->HT_SIG, 6);
		PRINT_DATA((char *)"VHT_SIG_A", pPMacTxInfo->VHT_SIG_A, 6);
		PRINT_DATA((char *)"VHT_SIG_B", pPMacTxInfo->VHT_SIG_B, 4);
		DBG("VHT_SIG_B_CRC %x\n", pPMacTxInfo->VHT_SIG_B_CRC);
		PRINT_DATA((char *)"VHT_Delimiter", pPMacTxInfo->VHT_Delimiter, 4);
		PRINT_DATA((char *)"Dest Address", pPMacTxInfo->MacAddress, 6);
#endif
	}

	memcpy(copy_input, setmpchar, strlen(setmpchar));
	memcpy(copy_input + strlen(setmpchar), (void *)&MyPMacTxInfo, sizeof(RT_PMAC_TX_INFO));

	memset(cmdbuf, 0, sizeof(cmdbuf));
	snprintf(cmdbuf, sizeof(cmdbuf), "%s", setmp_get_he);
	DBG("cmdbuf= %s %ld\n", cmdbuf, strlen(cmdbuf));
	rtw_io_ctrl_nochk(ifname, cmdbuf);

	if (strncmp(cmdbuf, "true", 4) == 0) {
		printf("HE HW Tx\n");
		memset(cmdbuf, 0, sizeof(cmdbuf));
		if (pPMacTxInfo->bEnPMacTx == FALSE) {
			snprintf(cmdbuf, sizeof(cmdbuf), "%s", "mp_ctx stop");
			DBG("cmdbuf= %s %ld\n", cmdbuf, strlen(cmdbuf));
		} else {
			if (pktPeriod != 0) {
				snprintf(cmdbuf, sizeof(cmdbuf), "mp_ctx pktinterval=%d", pktPeriod);
				err = rtw_mp_send(ctx, cmdbuf, BUF_SIZE, 0);
				if (err < 0) {
					return -1;
				}
			}
			if (pktlen != 1500) {
				memset(cmdbuf, 0, sizeof(cmdbuf));
				sprintf(cmdbuf, "mp_ctx pktlen=%d", pktlen);
				err = rtw_mp_send(ctx, cmdbuf, BUF_SIZE, 0);
				if (err < 0) {
					return -1;
				}
			}
			if (PktPattern != 0x00) {
				memset(cmdbuf, 0, sizeof(cmdbuf));
				sprintf(cmdbuf, "mp_ctx payload=%d", PktPattern);
				err = rtw_mp_send(ctx, cmdbuf, BUF_SIZE, 0);
				if (err < 0) {
					return -1;
				}
			}
			if (ru_tone != -1) {
				memset(cmdbuf, 0, sizeof(cmdbuf));
				sprintf(cmdbuf, "%s%d", setmp_set_plcp_data_ru_tone, ru_tone);
				DBG("cmdbuf= %s %ld\n", cmdbuf, strlen(cmdbuf));
				err = rtw_mp_send(ctx, cmdbuf, BUF_SIZE, 0);
				if (err < 0) {
					return -1;
				}
			}
			if (ppdu != -1) {
				memset(cmdbuf, 0, sizeof(cmdbuf));
				sprintf(cmdbuf, "%s%d", setmp_set_plcp_data_ppdu, ppdu);
				DBG("cmdbuf= %s %ld\n", cmdbuf, strlen(cmdbuf));
				err = rtw_mp_send(ctx, cmdbuf, BUF_SIZE, 0);
				if (err < 0) {
					return -1;
				}
			}
			if (ru_alloc != -1) {
				memset(cmdbuf, 0, sizeof(cmdbuf));
				sprintf(cmdbuf, "%s%d", setmp_set_plcp_data_ru_alloc, ru_alloc);
				DBG("cmdbuf= %s %ld\n", cmdbuf, strlen(cmdbuf));
				err = rtw_mp_send(ctx, cmdbuf, BUF_SIZE, 0);
				if (err < 0) {
					return -1;
				}
			}
			memset(cmdbuf, 0, sizeof(cmdbuf));
			if (txmode == 1) {
				if (pktcount != 0) {
					sprintf(cmdbuf, "mp_ctx count=%d,pkt", pktcount);
				} else {
					sprintf(cmdbuf, "%s", "mp_ctx background,pkt");
				}
			} else if (txmode == 2) {
				sprintf(cmdbuf, "%s", "mp_ctx background");
			} else if (txmode == 3) {
				sprintf(cmdbuf, "%s", "mp_ctx background,stone");
			}
			DBG("cmdbuf= %s %ld\n", cmdbuf, strlen(cmdbuf));
		}
		err = rtw_mp_send(ctx, cmdbuf, BUF_SIZE, 0);
		if (err < 0) {
			return -1;
		}
	} else {
		err = rtw_mp_send_binary(ctx, copy_input, sizeof(RT_PMAC_TX_INFO) + 9);
		if (err < 0) {
			fprintf(stderr, "Interface doesn't accept private ioctl...\n");
			fprintf(stderr, "%s: %s\n", argv[2], strerror(errno));
		} else {
			if (strlen(copy_input) != 0) {
				printf("%-8.16s %s:%s\n", ifname, argv[2], copy_input);
			}
		}
	}

	return err;
}
