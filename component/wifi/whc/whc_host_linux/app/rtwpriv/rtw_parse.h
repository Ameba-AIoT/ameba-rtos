/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 *****************************************************************************/

#ifndef __RTW_PARSE_H__
#define __RTW_PARSE_H__

#include "rtwpriv.h"

extern const char *HW_rateindex_Array[];

u8 key_char2num(u8 ch);
u8 key_2char2num(u8 hch, u8 lch);
void macstr2num(u8 *dst, u8 *src);
void dump_buf(char *buf, int len);
int randInRange(int min, int max);
UCHAR randomByte(void);
void split(char **arr, char *str, char *del);
int wifirate2_ratetbl_inx(int rate);
u8 HwRateToMPTRate(u8 rate);
int rtw_mpRateParseFunc(char *targetStr);
u16 rtw_ant_strpars(char *targetStr);
int Read_Parsing_file(PRT_PMAC_TX_INFO pPMacTxInfo2, u16 *Antenna, u8 *UnicastDID);
void rtw_help(void);

#endif /* __RTW_PARSE_H__ */
