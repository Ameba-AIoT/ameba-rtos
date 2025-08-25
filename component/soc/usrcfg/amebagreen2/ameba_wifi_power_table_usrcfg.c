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
 *                           TX_Power Limit
 ******************************************************************************/

// regu_en = {FCC, MKK, ETSI, IC, KCC, ACMA, CHILE, MEXICO, WW, GL, UKRAINE, CN, QATAR, UK, NCC,  EXT}
const bool regu_en[16] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0};
u8 regu_en_array_len = sizeof(regu_en) / sizeof(bool);

const s8 tx_pwr_limit_2g_fcc[3][14] = {
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 76, 62, 127}, /*CCK*/
	{66, 72, 76, 76, 76, 76, 76, 76, 76, 72, 62, 58, 50, 127}, /*OFDM*/
	{64, 72, 76, 76, 76, 76, 76, 76, 76, 72, 62, 58, 46, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_5g_fcc[2][28] = {
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72}, /*OFDM*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72}  /*HE_B20*/
};
const u8 tx_shap_fcc[2][3] = {{0, 3, 3}, {3, 3}}; /*{2G{CCK, OFDM, HE_B20}, 5G{OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_etsi[3][14] = {
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 127}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_5g_etsi[2][28] = {
	{64, 64, 64, 64, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 36, 36, 36, 36, 36, 36, 127}, /*OFDM*/
	{64, 64, 64, 64, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 36, 36, 36, 36, 36, 36, 127}  /*HE_B20*/
};
const u8 tx_shap_etsi[2][3] = {{0, 0, 0}, {0, 0}}; /*{2G{CCK, OFDM, HE_B20}, 5G{OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_mkk[3][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 68}, /*CCK*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127}, /*OFDM*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_5g_mkk[2][28] = {
	{64, 64, 64, 64, 52, 52, 52, 52, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127, 127, 127, 127, 127, 127}, /*OFDM*/
	{64, 64, 64, 64, 52, 52, 52, 52, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127, 127, 127, 127, 127, 127}  /*HE_B20*/
};
const u8 tx_shap_mkk[2][3] = {{0, 0, 0}, {0, 0}}; /*{2G{CCK, OFDM, HE_B20}, 5G{OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_ic[3][14] = {
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 76, 62, 127}, /*CCK*/
	{66, 72, 76, 76, 76, 76, 76, 76, 76, 72, 62, 58, 50, 127}, /*OFDM*/
	{64, 72, 76, 76, 76, 76, 76, 76, 76, 72, 62, 58, 46, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_5g_ic[2][28] = {
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127}, /*OFDM*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127}  /*HE_B20*/
};
const u8 tx_shap_ic[2][3] = {{0, 3, 3}, {3, 3}}; /*{2G{CCK, OFDM, HE_B20}, 5G{OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_kcc[3][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_5g_kcc[2][28] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 32, 32, 32, 32, 32, 127, 127, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 72, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 32, 32, 127, 127, 127}  /*HE_B20*/
};
const u8 tx_shap_kcc[2][3] = {{0, 0, 0}, {0, 0}}; /*{2G{CCK, OFDM, HE_B20}, 5G{OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_cn[3][14] = {
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 127}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 58, 48, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 58, 48, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_5g_cn[2][28] = {
	{64, 64, 64, 64, 64, 64, 64, 64, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 72, 72, 72, 72, 72, 127, 127, 127}, /*OFDM*/
	{64, 64, 64, 64, 64, 64, 64, 64, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 72, 72, 72, 72, 72, 127, 127, 127}  /*HE_B20*/
};
const u8 tx_shap_cn[2][3] = {{0, 3, 3}, {0, 0}}; /*{2G{CCK, OFDM, HE_B20}, 5G{OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_ww[3][14] = {
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 68}, /*CCK*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 58, 48, 127}, /*OFDM*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 58, 46, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_5g_ww[2][28] = {
	{60, 60, 60, 60, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 32, 32, 32, 32, 32, 36, 36, 72}, /*OFDM*/
	{60, 60, 60, 60, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 36, 36, 32, 32, 36, 36, 72}  /*HE_B20*/
};
const s8 tx_pwr_limit_2g_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_acma[][CH_NULL] = {{0}};
const u8 tx_shap_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_chile[][CH_NULL] = {{0}};
const u8 tx_shap_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_mexico[][CH_NULL] = {{0}};
const u8 tx_shap_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ukraine[][CH_NULL] = {{0}};
const u8 tx_shap_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_qatar[][CH_NULL] = {{0}};
const u8 tx_shap_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_uk[][CH_NULL] = {{0}};
const u8 tx_shap_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ncc[][CH_NULL] = {{0}};
const u8 tx_shap_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ext[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ext[][CH_NULL] = {{0}};
const u8 tx_shap_ext[][CH_NULL] = {{0}};


/******************************************************************************
 *                           txpwr_lmt_ru
 ******************************************************************************/
const s8 tx_pwr_limit_ru_2g_fcc[3][14] = {
	{40, 48, 68, 76, 76, 76, 76, 76, 76, 56, 40, 40, -8, 127}, /*HE-RU26*/
	{44, 58, 64, 76, 76, 76, 76, 76, 76, 56, 42, 44, 36, 127}, /*HE-RU52*/
	{54, 62, 68, 76, 76, 76, 76, 76, 76, 68, 56, 50, 34, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_5g_fcc[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 32, 48, 72, 72, 72, 72, 72, 36, 28, 0}, /*HE-RU26*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 44, 60, 72, 72, 72, 72, 72, 36, 24, 16}, /*HE-RU52*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 62, 72, 72, 72, 72, 72, 72, 36, 24, 16}  /*HE-RU106*/
};
const u8 tx_shap_ru_fcc[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}, 5G{HE-RU26, HE-RU52, HE-RU106}}*/

const s8 tx_pwr_limit_ru_2g_etsi[3][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_5g_etsi[3][28] = {
	{28, 28, 28, 28, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 36, 36, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 36, 36, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 36, 36, 36, 36, 36, 36, 36, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_etsi[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}, 5G{HE-RU26, HE-RU52, HE-RU106}}*/

const s8 tx_pwr_limit_ru_2g_mkk[3][14] = {
	{36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 127}, /*HE-RU26*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127}, /*HE-RU52*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_5g_mkk[3][28] = {
	{28, 28, 28, 28, 16, 16, 16, 16, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 127, 127, 127, 127, 127, 127, 127, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 28, 28, 28, 28, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127, 127, 127, 127, 127, 127, 127, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 40, 40, 40, 40, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127, 127, 127, 127, 127, 127, 127, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_mkk[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}, 5G{HE-RU26, HE-RU52, HE-RU106}}*/

const s8 tx_pwr_limit_ru_2g_ic[3][14] = {
	{40, 48, 68, 76, 76, 76, 76, 76, 76, 56, 40, 40, -8, 127}, /*HE-RU26*/
	{44, 58, 64, 76, 76, 76, 76, 76, 76, 56, 42, 44, 36, 127}, /*HE-RU52*/
	{54, 62, 68, 76, 76, 76, 76, 76, 76, 68, 56, 50, 34, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_5g_ic[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 48, 48, 32, 48, 72, 72, 72, 72, 72, 127, 127, 127}, /*HE-RU26*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127, 127, 127, 60, 60, 44, 60, 72, 72, 72, 72, 72, 127, 127, 127}, /*HE-RU52*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127, 72, 72, 62, 72, 72, 72, 72, 72, 72, 127, 127, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_ic[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}, 5G{HE-RU26, HE-RU52, HE-RU106}}*/

const s8 tx_pwr_limit_ru_2g_kcc[3][14] = {
	{32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 127}, /*HE-RU26*/
	{44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 127}, /*HE-RU52*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_5g_kcc[3][28] = {
	{24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 28, 28, 28, 28, 28, 127, 127, 127}, /*HE-RU26*/
	{36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 28, 28, 28, 28, 28, 127, 127, 127}, /*HE-RU52*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 28, 28, 28, 28, 28, 127, 127, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_kcc[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}, 5G{HE-RU26, HE-RU52, HE-RU106}}*/

const s8 tx_pwr_limit_ru_2g_cn[3][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 18, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_5g_cn[3][28] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 64, 64, 64, 64, 64, 127, 127, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 40, 40, 40, 40, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 72, 72, 72, 72, 72, 127, 127, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 52, 52, 52, 52, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 72, 72, 72, 72, 72, 127, 127, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_cn[2][3] = {{0, 0, 0}, {0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}, 5G{HE-RU26, HE-RU52, HE-RU106}}*/

const s8 tx_pwr_limit_ru_2g_ww[3][14] = {
	{28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, -8, 127}, /*HE-RU26*/
	{40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 34, 127}, /*HE-RU52*/
	{52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 34, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_5g_ww[3][28] = {
	{24, 24, 24, 24, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 36, 28, 0}, /*HE-RU26*/
	{36, 36, 36, 36, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 36, 24, 16}, /*HE-RU52*/
	{48, 48, 48, 48, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 28, 28, 28, 28, 28, 36, 24, 16}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_acma[][CH_NULL] = {{0}};
const u8 tx_shap_ru_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_chile[][CH_NULL] = {{0}};
const u8 tx_shap_ru_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_mexico[][CH_NULL] = {{0}};
const u8 tx_shap_ru_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_ukraine[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_qatar[][CH_NULL] = {{0}};
const u8 tx_shap_ru_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_uk[][CH_NULL] = {{0}};
const u8 tx_shap_ru_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_ncc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ext[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_5g_ext[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ext[][CH_NULL] = {{0}};


/******************************************************************************
 *                           TX_Power byRate
 ******************************************************************************/
const s8 array_mp_txpwr_byrate_2g[] = {
	0x50, 0x50, 0x50, 0x50,  //11M 5.5M 2M 1M is 20dbm 20dbm 20dbm 20dBm
	0x4c, 0x4c, 0x4c, 0x4c,  //18M 12M 9M 6M is 19dbm 19dbm 19dbm 19dBm
	0x48, 0x48, 0x4c, 0x48,  //54M 48M 36M 24M is 18dbm 18dbm 19dbm 18dBm
	0x4c, 0x4c, 0x4c, 0x4c,  // HT_VHT_HE_MCS3 MCS2 MCS1 MCS0 is 19dbm 19dbm 19dbm 19dBm
	0x44, 0x44, 0x48, 0x4c,  // HT_VHT_HE_MCS7 MCS6 MCS5 MCS4 is 17dbm 17dbm 18dbm 19dBm
	0x34, 0x38, 0x3c, 0x40,  // VHT_HE_MCS11 MCS10 MCS9 MCS8 is 13dbm 14dbm 15dbm 16dBm
	0x4c, 0x4c, 0x4c, 0x4c  // HEDCM_MCS4 MCS3 MCS1 MCS0 is 19dbm 19dbm 19dbm 19dBm
};
u8 array_mp_txpwr_byrate_2g_array_len = sizeof(array_mp_txpwr_byrate_2g) / sizeof(s8);
const s8 array_mp_txpwr_byrate_5g[] = {
	0x48, 0x48, 0x48, 0x48,  //18M 12M 9M 6M is 18dbm 18dbm 18dbm 18dBm
	0x44, 0x44, 0x44, 0x48,  //54M 48M 36M 24M is 17dbm 17dbm 17dbm 18dBm
	0x48, 0x48, 0x48, 0x48,  // HT_VHT_HE_MCS3 MCS2 MCS1 MCS0 is 18dbm 18dbm 18dbm 18dBm
	0x40, 0x40, 0x40, 0x44,  // HT_VHT_HE_MCS7 MCS6 MCS5 MCS4 is 16dbm 16dbm 16dbm 17dBm
	0x30, 0x34, 0x38, 0x3c,  // VHT_HE_MCS11 MCS10 MCS9 MCS8 is 12dbm 13dbm 14dbm 15dBm
	0x44, 0x48, 0x48, 0x48  // HEDCM_MCS4 MCS3 MCS1 MCS0 is 17dbm 18dbm 18dbm 18dBm
};
u8 array_mp_txpwr_byrate_5g_array_len = sizeof(array_mp_txpwr_byrate_5g) / sizeof(s8);



s8 wifi_get_power_limit_and_limit_ru_by_ch(u8 regulation, u8 chnl, u8 limit_rate,
		u8 band, u8 bandwith, u8 *tx_shape_idx, bool is_lmt_ru)
{
	s8 val = 127;

	switch (regulation) {
	case TXPWR_LMT_FCC:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_fcc[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_fcc[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_fcc[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_fcc[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_fcc[band][limit_rate];
		}
		break;
	case TXPWR_LMT_ETSI:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_etsi[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_etsi[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_etsi[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_etsi[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_etsi[band][limit_rate];
		}
		break;
	case TXPWR_LMT_MKK:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_mkk[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_mkk[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_mkk[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_mkk[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_mkk[band][limit_rate];
		}
		break;
	case TXPWR_LMT_IC:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_ic[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_ic[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_ic[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_ic[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_ic[band][limit_rate];
		}
		break;
	case TXPWR_LMT_KCC:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_kcc[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_kcc[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_kcc[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_kcc[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_kcc[band][limit_rate];
		}
		break;
	case TXPWR_LMT_ACMA:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_acma[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_acma[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_acma[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_acma[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_acma[band][limit_rate];
		}
		break;
	case TXPWR_LMT_NCC:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_ncc[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_ncc[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_ncc[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_ncc[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_ncc[band][limit_rate];
		}
		break;
	case TXPWR_LMT_MEXICO:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_mexico[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_mexico[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_mexico[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_mexico[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_mexico[band][limit_rate];
		}
		break;
	case TXPWR_LMT_CHILE:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_chile[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_chile[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_chile[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_chile[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_chile[band][limit_rate];
		}
		break;
	case TXPWR_LMT_UKRAINE:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_ukraine[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_ukraine[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_ukraine[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_ukraine[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_ukraine[band][limit_rate];
		}
		break;
	case TXPWR_LMT_CN:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_cn[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_cn[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_cn[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_cn[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_cn[band][limit_rate];
		}
		break;
	case TXPWR_LMT_QATAR:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_qatar[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_qatar[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_qatar[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_qatar[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_qatar[band][limit_rate];
		}
		break;
	case TXPWR_LMT_UK:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_uk[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_uk[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_uk[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_uk[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_uk[band][limit_rate];
		}
		break;
	case TXPWR_LMT_WW:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_ww[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_ww[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_ww[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_ww[limit_rate][chnl];
			}
		}
		break;
	case TXPWR_LMT_EXT:
		if (is_lmt_ru) {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_ru_2g_ext[bandwith][chnl];
			} else {
				val = tx_pwr_limit_ru_5g_ext[bandwith][chnl];
			}
		} else {
			if (band == RTW_BAND_ON_24G) {
				val = tx_pwr_limit_2g_ext[limit_rate][chnl];
			} else {
				val = tx_pwr_limit_5g_ext[limit_rate][chnl];
			}
			*tx_shape_idx = tx_shap_ext[band][limit_rate];
		}
		break;
	default:
		break;
	}

	return val;
}
