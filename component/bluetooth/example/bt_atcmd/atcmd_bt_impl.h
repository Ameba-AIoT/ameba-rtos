/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __ATCMD_BT_IMPL_H__
#define __ATCMD_BT_IMPL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <basic_types.h>
#include <bt_debug.h>

typedef int (*cmd_func_t)(int argc, char *argv[]);

typedef struct {
	const char *cmd_name;
	cmd_func_t cmd_func;
	uint16_t argc_min;
	uint16_t argc_max;
} cmd_table_t;

int atcmd_bt_excute(int argc, char *argv[], const cmd_table_t *cmd_table, const char *tag);

/* bt example cmd implementation */
void fBTDEMO(u16 argc, char *argv[]);

/* bt vendor cmd impl */
void fBTVENDOR(u16 argc, char *argv[]);

/* le gap cmd impl */
void fBLEGAP(u16 argc, char *argv[]);

/* mesh stack cmd impl */
void fBLEMESHSTACK(u16 argc, char *argv[]);

/* mesh config client model cmd impl */
void fBLEMESHCONFIG(u16 argc, char *argv[]);

/* mesh generic on off model cmd impl */
void fBLEMESHGOO(u16 argc, char *argv[]);

/* mesh datatrans model cmd impl */
void fBLEMESHDATA(u16 argc, char *argv[]);

/* mesh device firmware update relate models cmd impl */
void fBLEMESHDFU(u16 argc, char *argv[]);

/* mesh remote prov client model cmd impl */
void fBLEMESHRMT(u16 argc, char *argv[]);

/* mesh light lightness model cmd impl */
void fBLEMESHLL(u16 argc, char *argv[]);

/* mesh light ctl model cmd impl */
void fBLEMESHLCTL(u16 argc, char *argv[]);

/* mesh light hsl model cmd impl */
void fBLEMESHLHSL(u16 argc, char *argv[]);

/* mesh light xyl model cmd impl */
void fBLEMESHLXYL(u16 argc, char *argv[]);

/* mesh light lc model cmd impl */
void fBLEMESHLLC(u16 argc, char *argv[]);

/* mesh time model cmd impl */
void fBLEMESHTIME(u16 argc, char *argv[]);

/* mesh scheduler model cmd impl */
void fBLEMESHSCHEDULER(u16 argc, char *argv[]);

/* mesh scene model cmd impl */
void fBLEMESHSCENE(u16 argc, char *argv[]);

/* mesh generic default transition time model cmd impl */
void fBLEMESHGDTT(u16 argc, char *argv[]);

/* mesh generic level model cmd impl */
void fBLEMESHGLE(u16 argc, char *argv[]);

/* mesh generic power on off model cmd impl */
void fBLEMESHGPOO(u16 argc, char *argv[]);

/* mesh generic power level model cmd impl */
void fBLEMESHGPL(u16 argc, char *argv[]);

/* mesh generic battery model cmd impl */
void fBLEMESHGB(u16 argc, char *argv[]);

/* mesh generic location model cmd impl */
void fBLEMESHGLO(u16 argc, char *argv[]);

/* mesh generic property model cmd impl */
void fBLEMESHGP(u16 argc, char *argv[]);

/* mesh sensor model cmd impl */
void fBLEMESHSENSOR(u16 argc, char *argv[]);

/* mesh health model cmd impl */
void fBLEMESHHEALTH(u16 argc, char *argv[]);

/* mesh directed forwarding cmd impl */
void fBLEMESHDF(u16 argc, char *argv[]);

/* mesh directed forwarding cmd impl */
void fBLEMESHSBR(u16 argc, char *argv[]);

/* mesh private beacon cmd impl */
void fBLEMESHPRB(u16 argc, char *argv[]);

/* br gap cmd impl */
void fBRGAP(u16 argc, char *argv[]);

/* common gap cmd impl */
void fBTGAP(u16 argc, char *argv[]);

/* gatts cmd impl */
void fBLEGATTS(u16 argc, char *argv[]);

/* gattc cmd impl */
void fBLEGATTC(u16 argc, char *argv[]);

/* sdp cmd impl */
void fBTSDP(u16 argc, char *argv[]);

/* a2dp cmd impl */
void fBTA2DP(u16 argc, char *argv[]);

/* avrcp cmd impl */
void fBTAVRCP(u16 argc, char *argv[]);

/* spp cmd impl */
void fBTSPP(u16 argc, char *argv[]);

/* rfc cmd impl */
void fBTRFC(u16 argc, char *argv[]);

/* hid cmd impl */
void fBTHID(u16 argc, char *argv[]);

/* hfp cmd impl */
void fBTHFP(u16 argc, char *argv[]);

/* pbap cmd impl */
void fBTPBAP(u16 argc, char *argv[]);

/* le audio bap */
void fBLEBAP(u16 argc, char *argv[]);

/* le audio iso */
void fBLEISO(u16 argc, char *argv[]);

/* le audio cap */
void fBLECAP(u16 argc, char *argv[]);

/* le audio tmap */
void fBLETMAP(u16 argc, char *argv[]);

/* le audio gmap */
void fBLEGMAP(u16 argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* __ATCMD_BT_IMPL_H__ */