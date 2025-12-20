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

// regu_en = {FCC, MKK, ETSI, IC, KCC, ACMA, CHILE, MEXICO, WW, GL, UKRAINE, CN, QATAR, UK, NCC, EXT}
const bool regu_en[16] = {1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0};

u8 regu_en_array_len = sizeof(regu_en) / sizeof(bool);

const s8 tx_pwr_limit_2g_fcc[3][14] = {
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 76, 68, 127}, /*CCK*/
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 66, 56, 127}, /*OFDM*/
	{76, 80, 80, 80, 80, 80, 80, 80, 80, 76, 74, 62, 54, 127}  /*HE_B20*/
};
const u8 tx_shap_fcc[1][3] = {{1, 3, 3}}; /*{2G{CCK, OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_etsi[3][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*CCK*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127}, /*OFDM*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127}  /*HE_B20*/
};
const u8 tx_shap_etsi[1][3] = {{0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_mkk[3][14] = {
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 72}, /*CCK*/
	{76, 78, 78, 78, 78, 78, 76, 78, 78, 78, 78, 78, 78, 127}, /*OFDM*/
	{78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 78, 127}  /*HE_B20*/
};
const u8 tx_shap_mkk[1][3] = {{0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_ic[3][14] = {
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 76, 68, 127}, /*CCK*/
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 66, 56, 127}, /*OFDM*/
	{76, 80, 80, 80, 80, 80, 80, 80, 80, 76, 74, 62, 54, 127}  /*HE_B20*/
};
const u8 tx_shap_ic[1][3] = {{1, 3, 3}}; /*{2G{CCK, OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_kcc[3][14] = {
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 70, 70, 70, 70, 127}, /*CCK*/
	{68, 68, 68, 68, 76, 76, 76, 76, 76, 70, 70, 70, 70, 127}, /*OFDM*/
	{70, 70, 70, 70, 78, 78, 78, 78, 78, 66, 66, 66, 66, 127}  /*HE_B20*/
};
const u8 tx_shap_kcc[1][3] = {{0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_cn[3][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 54, 127}, /*CCK*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 54, 127}, /*OFDM*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 60, 48, 127}  /*HE_B20*/
};
const u8 tx_shap_cn[1][3] = {{1, 3, 3}}; /*{2G{CCK, OFDM, HE_B20}}*/

const s8 tx_pwr_limit_2g_ww[3][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 54, 72}, /*CCK*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 54, 127}, /*OFDM*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 66, 66, 60, 48, 127}  /*HE_B20*/
};
const s8 tx_pwr_limit_2g_acma[][CH_NULL] = {{0}};
const u8 tx_shap_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_chile[][CH_NULL] = {{0}};
const u8 tx_shap_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_mexico[][CH_NULL] = {{0}};
const u8 tx_shap_mexico[][CH_NULL] = {{0}};
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
	{54, 74, 76, 80, 80, 80, 80, 80, 76, 64, 44, 44, 6, 127}, /*HE-RU26*/
	{56, 76, 80, 80, 80, 80, 80, 80, 80, 76, 62, 60, 16, 127}, /*HE-RU52*/
	{74, 76, 80, 80, 80, 80, 80, 80, 80, 78, 66, 66, 30, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_fcc[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}*/

const s8 tx_pwr_limit_ru_2g_etsi[3][14] = {
	{38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 127}, /*HE-RU26*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127}, /*HE-RU52*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_etsi[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}*/

const s8 tx_pwr_limit_ru_2g_mkk[3][14] = {
	{36, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 36, 127}, /*HE-RU26*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 46, 127}, /*HE-RU52*/
	{58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_mkk[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}*/

const s8 tx_pwr_limit_ru_2g_ic[3][14] = {
	{54, 74, 76, 80, 80, 80, 80, 80, 76, 64, 44, 44, 6, 127}, /*HE-RU26*/
	{56, 76, 80, 80, 80, 80, 80, 80, 80, 76, 62, 60, 16, 127}, /*HE-RU52*/
	{74, 76, 80, 80, 80, 80, 80, 80, 80, 78, 66, 66, 30, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_ic[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}*/

const s8 tx_pwr_limit_ru_2g_kcc[3][14] = {
	{42, 42, 42, 42, 44, 44, 44, 44, 44, 42, 42, 42, 42, 127}, /*HE-RU26*/
	{54, 54, 54, 54, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}, /*HE-RU52*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_kcc[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}*/

const s8 tx_pwr_limit_ru_2g_cn[3][14] = {
	{38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 20, 127}, /*HE-RU26*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 30, 127}, /*HE-RU52*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 38, 127}  /*HE-RU106*/
};
const u8 tx_shap_ru_cn[1][3] = {{0, 0, 0}}; /*{2G{HE-RU26, HE-RU52, HE-RU106}*/

const s8 tx_pwr_limit_ru_2g_ww[3][14] = {
	{36, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 6, 127}, /*HE-RU26*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 16, 127}, /*HE-RU52*/
	{58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 30, 127}  /*HE-RU106*/
};
const s8 tx_pwr_limit_ru_2g_acma[][CH_NULL] = {{0}};
const u8 tx_shap_ru_acma[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_chile[][CH_NULL] = {{0}};
const u8 tx_shap_ru_chile[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_mexico[][CH_NULL] = {{0}};
const u8 tx_shap_ru_mexico[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ukraine[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ukraine[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_qatar[][CH_NULL] = {{0}};
const u8 tx_shap_ru_qatar[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_uk[][CH_NULL] = {{0}};
const u8 tx_shap_ru_uk[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ncc[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ncc[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_ru_2g_ext[][CH_NULL] = {{0}};
const u8 tx_shap_ru_ext[][CH_NULL] = {{0}};


/******************************************************************************
 *                           TX_Power byRate
 ******************************************************************************/
const s8 array_mp_txpwr_byrate_2g[] = {
	0x50, 0x50, 0x50, 0x50,  //11M 5.5M 2M 1M is 20dbm 20dbm 20dbm 20dBm
	0x50, 0x50, 0x50, 0x50,  //18M 12M 9M 6M is 20dbm 20dbm 20dbm 20dBm
	0x4c, 0x50, 0x50, 0x50,  //54M 48M 36M 24M is 19dbm 20dbm 20dbm 20dBm
	0x50, 0x50, 0x50, 0x50,  // HT_VHT_HE_MCS3 MCS2 MCS1 MCS0 is 20dbm 20dbm 20dbm 20dBm
	0x48, 0x4c, 0x50, 0x50,  // HT_VHT_HE_MCS7 MCS6 MCS5 MCS4 is 18dbm 19dbm 20dbm 20dBm
	0x38, 0x3c, 0x40, 0x44,  // VHT_HE_MCS11 MCS10 MCS9 MCS8 is 14dbm 15dbm 16dbm 17dBm
	0x50, 0x50, 0x50, 0x50  // HEDCM_MCS4 MCS3 MCS1 MCS0 is 20dbm 20dbm 20dbm 20dBm
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
