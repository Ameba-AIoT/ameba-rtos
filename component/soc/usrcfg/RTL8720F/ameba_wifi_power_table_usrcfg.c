/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "wifi_api.h"

#define CH_NULL 1

/* allow customer to adjust the pwrlmt regu corresponding to domain code, for example*/
volatile struct _pwr_lmt_regu_remap pwrlmt_regu_remapping[1] = {{0}};

volatile u8 array_len_of_pwrlmt_regu_remapping = sizeof(pwrlmt_regu_remapping) / sizeof(struct _pwr_lmt_regu_remap);

/******************************************************************************
 *                             TX_Power Limit
 ******************************************************************************/

// regu_en = {FCC, MKK, ETSI, IC, KCC, ACMA, CHILE, MEXICO, WW, GL, UKRAINE, CN, QATAR, UK, NCC, EXT}
const bool regu_en[16] = {1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0};

u8 regu_en_array_len = sizeof(regu_en) / sizeof(bool);

const s8 tx_pwr_limit_2g_fcc[4][14] = {
	{82, 84, 84, 84, 84, 84, 84, 84, 84, 80, 78, 68, 54, 127}, /*CCK*/
	{64, 68, 72, 76, 80, 80, 80, 76, 72, 66, 62, 54, 44, 127}, /*OFDM*/
	{62, 68, 72, 76, 80, 80, 80, 74, 68, 64, 58, 52, 40, 127}, /*HE_B20*/
	{127, 127, 60, 64, 68, 72, 76, 72, 66, 60, 54, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_fcc[1][4] = {{0, 0, 0, 1}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_etsi[4][14] = {
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 127}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*HE_B20*/
	{127, 127, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_etsi[1][4] = {{0, 0, 0, 1}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_mkk[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 68}, /*CCK*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127}, /*OFDM*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127}, /*HE_B20*/
	{127, 127, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_mkk[1][4] = {{0, 0, 0, 1}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_ic[4][14] = {
	{82, 84, 84, 84, 84, 84, 84, 84, 84, 80, 78, 68, 54, 127}, /*CCK*/
	{64, 68, 72, 76, 80, 80, 80, 76, 72, 66, 62, 54, 44, 127}, /*OFDM*/
	{62, 68, 72, 76, 80, 80, 80, 74, 68, 64, 58, 52, 40, 127}, /*HE_B20*/
	{127, 127, 60, 64, 68, 72, 76, 72, 66, 60, 54, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_ic[1][4] = {{0, 0, 0, 1}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_kcc[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*HE_B20*/
	{127, 127, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_kcc[1][4] = {{0, 0, 0, 1}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_cn[4][14] = {
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 127}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 56, 52, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 54, 50, 127}, /*HE_B20*/
	{127, 127, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_cn[1][4] = {{0, 0, 0, 1}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_ww[4][14] = {
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 68}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 54, 44, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 58, 52, 40, 127}, /*HE_B20*/
	{127, 127, 60, 60, 60, 60, 60, 60, 60, 60, 54, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_2g_acma[][CH_NULL] = {{0}};
const u8 tx_shap_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_chile[][CH_NULL] = {{0}};
const u8 tx_shap_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_mexico[][CH_NULL] = {{0}};
const u8 tx_shap_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_gl[][CH_NULL] = {{0}};
const u8 tx_shap_gl[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ukraine[][CH_NULL] = {{0}};
const u8 tx_shap_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_qatar[][CH_NULL] = {{0}};
const u8 tx_shap_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_uk[][CH_NULL] = {{0}};
const u8 tx_shap_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ncc[][CH_NULL] = {{0}};
const u8 tx_shap_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ext[][CH_NULL] = {{0}};
const u8 tx_shap_ext[][CH_NULL] = {{0}};


/******************************************************************************
 *                           txpwr_lmt_ru
 ******************************************************************************/
const s8 tx_pwr_limit_ru_2g_fcc[3][14] = {
	{40, 48, 48, 48, 52, 72, 60, 54, 54, 48, 44, 40, -8, 127}, /*HE-RU26*/
	{44, 56, 56, 56, 68, 76, 68, 56, 56, 56, 48, 48, 36, 127}, /*HE-RU52*/
	{54, 62, 76, 76, 76, 76, 76, 68, 68, 56, 56, 50, 34, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_fcc_ext[5][14] = {
	{40, 48, 48, 48, 52, 72, 60, 54, 54, 48, 44, 40, -8, 127}, /*HE-RU26 lv0_idx~lv1_idx*/
	{40, 48, 48, 48, 52, 72, 60, 54, 54, 48, 44, 40, -8, 127}, /*HE-RU26 lv1_idx~8*/
	{44, 56, 56, 56, 68, 76, 68, 56, 56, 56, 48, 48, 36, 127}, /*HE-RU52 lv0_idx~lv1_idx*/
	{44, 56, 56, 56, 68, 76, 68, 56, 56, 56, 48, 48, 36, 127}, /*HE-RU52 lv1_idx~3*/
	{54, 62, 76, 76, 76, 76, 76, 68, 68, 56, 56, 50, 34, 127}  /*HE-RU106 1*/
};

const u8 tx_shap_ru_fcc[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106*/
const u8 tx_ru_2g_fcc_ext_loc[1][5] = {{1, 8, 38, 40, 54}};

const s8 tx_pwr_limit_ru_2g_etsi[3][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_etsi_ext[5][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 127}, /*HE-RU26 lv0_idx~lv1_idx*/
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 127}, /*HE-RU26 lv1_idx~8*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 127}, /*HE-RU52 lv0_idx~lv1_idx*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 127}, /*HE-RU52 lv1_idx~3*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 127}  /*HE-RU106 1*/
};

const u8 tx_shap_ru_etsi[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106*/
const u8 tx_ru_2g_etsi_ext_loc[1][5] = {{1, 8, 38, 40, 54}};

const s8 tx_pwr_limit_ru_2g_mkk[3][14] = {
	{36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 127}, /*HE-RU26*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127}, /*HE-RU52*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_mkk_ext[5][14] = {
	{36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 127}, /*HE-RU26 lv0_idx~lv1_idx*/
	{36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 127}, /*HE-RU26 lv1_idx~8*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127}, /*HE-RU52 lv0_idx~lv1_idx*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127}, /*HE-RU52 lv1_idx~3*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}  /*HE-RU106 1*/
};
const u8 tx_shap_ru_mkk[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106*/
const u8 tx_ru_2g_mkk_ext_loc[1][5] = {{1, 8, 38, 40, 54}};

const s8 tx_pwr_limit_ru_2g_ic[3][14] = {
	{40, 48, 48, 48, 52, 72, 60, 54, 54, 48, 44, 40, -8, 127}, /*HE-RU26*/
	{44, 56, 56, 56, 68, 76, 68, 56, 56, 56, 48, 48, 36, 127}, /*HE-RU52*/
	{54, 62, 76, 76, 76, 76, 76, 68, 68, 56, 56, 50, 34, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_ic_ext[5][14] = {
	{40, 48, 48, 48, 52, 72, 60, 54, 54, 48, 44, 40, -8, 127}, /*HE-RU26 lv0_idx~lv1_idx*/
	{40, 48, 48, 48, 52, 72, 60, 54, 54, 48, 44, 40, -8, 127}, /*HE-RU26 lv1_idx~8*/
	{44, 56, 56, 56, 68, 76, 68, 56, 56, 56, 48, 48, 36, 127}, /*HE-RU52 lv0_idx~lv1_idx*/
	{44, 56, 56, 56, 68, 76, 68, 56, 56, 56, 48, 48, 36, 127}, /*HE-RU52 lv1_idx~3*/
	{54, 62, 76, 76, 76, 76, 76, 68, 68, 56, 56, 50, 34, 127}  /*HE-RU106 1*/
};
const u8 tx_shap_ru_ic[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106*/
const u8 tx_ru_2g_ic_ext_loc[1][5] = {{1, 8, 38, 40, 54}};

const s8 tx_pwr_limit_ru_2g_kcc[3][14] = {
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 127}, /*HE-RU26*/
	{44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 127}, /*HE-RU52*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_kcc_ext[5][14] = {
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 127},  /*HE-RU26 lv0_idx~lv1_idx*/
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 127},  /*HE-RU26 lv1_idx~8*/
	{44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 127},  /*HE-RU52 lv0_idx~lv1_idx*/
	{44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 127},  /*HE-RU52 lv1_idx~3*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}   /*HE-RU106 1*/
};

const u8 tx_shap_ru_kcc[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106*/
const u8 tx_ru_2g_kcc_ext_loc[1][5] = {{1, 8, 38, 40, 54}};

const s8 tx_pwr_limit_ru_2g_cn[3][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 18, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_cn_ext[5][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 18, 127},  /*HE-RU26 lv0_idx~lv1_idx*/
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 18, 127},  /*HE-RU26 lv1_idx~8*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127},  /*HE-RU52 lv0_idx~lv1_idx*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127},  /*HE-RU52 lv1_idx~3*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 127}   /*HE-RU106 1*/
};
const u8 tx_shap_ru_cn[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106*/
const u8 tx_ru_2g_cn_ext_loc[1][5] = {{1, 8, 38, 40, 54}};

const s8 tx_pwr_limit_ru_2g_ww[3][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, -8, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 34, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_ww_ext[5][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, -8, 127},  /*HE-RU26 lv0_idx~lv1_idx*/
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, -8, 127},  /*HE-RU26 lv1_idx~8*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127},  /*HE-RU52 lv0_idx~lv1_idx*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127},  /*HE-RU52 lv1_idx~3*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 34, 127}   /*HE-RU106 1*/
};
const u8 tx_shap_ru_ww[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106*/
const u8 tx_ru_2g_ww_ext_loc[1][5] = {{1, 8, 38, 40, 54}};

const s8 tx_pwr_limit_ru_2g_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_acma_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_acma_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_chile_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_chile_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_mexico_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_mexico_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_gl[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_gl_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_gl_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_gl[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ukraine_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_ukraine_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_qatar_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_qatar_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_uk_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_uk_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ncc_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_ncc_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ext[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ext_ext[][CH_NULL] = {{0}};
const u8 tx_ru_2g_ext_ext_loc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ext[][CH_NULL] = {{0}};


/******************************************************************************
 *                           TX_Power byRate
 ******************************************************************************/
const s8 array_mp_txpwr_byrate_2g[] = {
	0x54, 0x54, 0x54, 0x54,  //11M 5.5M 2M 1M is 21dbm 21dbm 21dbm 21dBm
	0x50, 0x50, 0x50, 0x50,  //18M 12M 9M 6M is 20dbm 20dbm 20dbm 20dBm
	0x48, 0x48, 0x4c, 0x4c,  //54M 48M 36M 24M is 18dbm 18dbm 19dbm 19dBm
	0x4c, 0x4c, 0x4c, 0x4c,  // HT_VHT_HE_BW20_MCS3 MCS2 MCS1 MCS0 is 19dbm 19dbm 19dbm 19dBm
	0x44, 0x44, 0x48, 0x4c,  // HT_VHT_HE_BW20_MCS7 MCS6 MCS5 MCS4 is 17dbm 17dbm 18dbm 19dBm
	0x34, 0x38, 0x3c, 0x40,  // VHT_HE_BW20_MCS11 MCS10 MCS9 MCS8 is 13dbm 14dbm 15dbm 16dBm
	0x4c, 0x4c, 0x50, 0x50  // HEDCM_MCS4 MCS3 MCS1 MCS0 is 19dbm 19dbm 20dbm 20dBm
};
u8 array_mp_txpwr_byrate_2g_array_len = sizeof(array_mp_txpwr_byrate_2g) / sizeof(s8);



s8 wifi_get_power_limit_and_limit_ru_by_ch(u8 regulation, u8 chnl, u8 limit_rate,
		u8 band, u8 bandwith, u8 *tx_shape_idx, bool is_lmt_ru)
{
	s8 val = 127;

	switch (regulation) {
	case TXPWR_LMT_FCC:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_fcc[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_fcc[limit_rate][chnl];
			*tx_shape_idx = tx_shap_fcc[band][limit_rate];
		}
		break;
	case TXPWR_LMT_ETSI:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_etsi[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_etsi[limit_rate][chnl];
			*tx_shape_idx = tx_shap_etsi[band][limit_rate];
		}
		break;
	case TXPWR_LMT_MKK:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_mkk[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_mkk[limit_rate][chnl];
			*tx_shape_idx = tx_shap_mkk[band][limit_rate];
		}
		break;
	case TXPWR_LMT_IC:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_ic[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_ic[limit_rate][chnl];
			*tx_shape_idx = tx_shap_ic[band][limit_rate];
		}
		break;
	case TXPWR_LMT_KCC:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_kcc[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_kcc[limit_rate][chnl];
			*tx_shape_idx = tx_shap_kcc[band][limit_rate];
		}
		break;
	case TXPWR_LMT_ACMA:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_acma[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_acma[limit_rate][chnl];
			*tx_shape_idx = tx_shap_acma[band][limit_rate];
		}
		break;
	case TXPWR_LMT_NCC:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_ncc[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_ncc[limit_rate][chnl];
			*tx_shape_idx = tx_shap_ncc[band][limit_rate];
		}
		break;
	case TXPWR_LMT_MEXICO:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_mexico[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_mexico[limit_rate][chnl];
			*tx_shape_idx = tx_shap_mexico[band][limit_rate];
		}
		break;
	case TXPWR_LMT_CHILE:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_chile[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_chile[limit_rate][chnl];
			*tx_shape_idx = tx_shap_chile[band][limit_rate];
		}
		break;
	case TXPWR_LMT_UKRAINE:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_ukraine[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_ukraine[limit_rate][chnl];
			*tx_shape_idx = tx_shap_ukraine[band][limit_rate];
		}
		break;
	case TXPWR_LMT_CN:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_cn[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_cn[limit_rate][chnl];
			*tx_shape_idx = tx_shap_cn[band][limit_rate];
		}
		break;
	case TXPWR_LMT_QATAR:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_qatar[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_qatar[limit_rate][chnl];
			*tx_shape_idx = tx_shap_qatar[band][limit_rate];
		}
		break;
	case TXPWR_LMT_UK:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_uk[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_uk[limit_rate][chnl];
			*tx_shape_idx = tx_shap_uk[band][limit_rate];
		}
		break;
	case TXPWR_LMT_WW:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_ww[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_ww[limit_rate][chnl];
		}
		break;
	case TXPWR_LMT_EXT:
		if (is_lmt_ru) {
			val = tx_pwr_limit_ru_2g_ext[bandwith][chnl];
		} else {
			val = tx_pwr_limit_2g_ext[limit_rate][chnl];
			*tx_shape_idx = tx_shap_ext[band][limit_rate];
		}
		break;
	default:
		break;
	}

	return val;
}

s8 wifi_get_power_limit_and_limit_ru_by_ch_ext(u8 regulation, u8 ru_idx,
		u8 chnl, u8 band, u8 *ru_ext_loc)
{
	s8 val = 127;

	switch (regulation) {
	case TXPWR_LMT_FCC:
		val = tx_pwr_limit_ru_2g_fcc_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_fcc_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_ETSI:
		val = tx_pwr_limit_ru_2g_etsi_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_etsi_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_MKK:
		val = tx_pwr_limit_ru_2g_mkk_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_mkk_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_IC:
		val = tx_pwr_limit_ru_2g_ic_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_ic_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_KCC:
		val = tx_pwr_limit_ru_2g_kcc_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_kcc_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_ACMA:
		val = tx_pwr_limit_ru_2g_acma_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_acma_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_NCC:
		val = tx_pwr_limit_ru_2g_ncc_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_ncc_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_MEXICO:
		val = tx_pwr_limit_ru_2g_mexico_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_mexico_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_CHILE:
		val = tx_pwr_limit_ru_2g_chile_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_chile_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_UKRAINE:
		val = tx_pwr_limit_ru_2g_ukraine_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_ukraine_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_CN:
		val = tx_pwr_limit_ru_2g_cn_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_cn_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_QATAR:
		val = tx_pwr_limit_ru_2g_qatar_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_qatar_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_UK:
		val = tx_pwr_limit_ru_2g_uk_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_uk_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_WW:
		val = tx_pwr_limit_ru_2g_ww_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_ww_ext_loc[band][ru_idx];
		break;
	case TXPWR_LMT_EXT:
		val = tx_pwr_limit_ru_2g_ext_ext[ru_idx][chnl];
		*ru_ext_loc = tx_ru_2g_ext_ext_loc[band][ru_idx];
		break;
	default:
		break;
	}

	return val;
}