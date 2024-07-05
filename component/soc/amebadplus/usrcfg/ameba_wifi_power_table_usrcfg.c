/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "wifi_conf.h"

#define CH_NULL 1

/* allow customer to adjust the pwrlmt regu corresponding to domain code, for example*/
volatile pwr_lmt_regu_remap pwrlmt_regu_remapping[1] = {{0}};

volatile u8 array_len_of_pwrlmt_regu_remapping = sizeof(pwrlmt_regu_remapping) / sizeof(pwr_lmt_regu_remap);

/******************************************************************************
 *                             TX_Power Limit
 ******************************************************************************/

// regu_en = {FCC, MKK, ETSI, IC, KCC, ACMA, CHILE, MEXICO, WW, GL, UKRAINE, CN, QATAR, UK, NCC, EXT}
const bool regu_en[16] = {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0};

u8 regu_en_array_len = sizeof(regu_en) / sizeof(bool);

const s8 tx_pwr_limit_2g_fcc[4][14] = {
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 68, 62, 127}, /*CCK*/
	{66, 68, 72, 76, 76, 76, 76, 76, 72, 68, 62, 52, 46, 127}, /*OFDM*/
	{64, 68, 72, 76, 76, 76, 76, 76, 72, 68, 60, 48, 44, 127}, /*HT_B20*/
	{127, 127, 56, 60, 68, 68, 64, 60, 56, 52, 46, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_fcc[3][28] = {
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 70, 72, 72, 72, 72, 72, 68, 68, 68, 68}, /*OFDM*/
	{72, 72, 72, 72, 72, 72, 72, 72, 66, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 68, 68, 68, 68}, /*HT_B20*/
	{70, 72, 72, 72, 58, 72, 72, 72, 72, 72, 72, 72, 68, 68}  /*HT_B40*/
};
const u8 tx_shap_fcc[2][4] = {{0, 1, 1, 1}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_etsi[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*CCK*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127}, /*OFDM*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127}, /*HT_B20*/
	{127, 127, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_etsi[3][28] = {
	{68, 68, 68, 68, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127, 38, 38, 38, 38, 38, 38, 38, 127}, /*OFDM*/
	{68, 68, 68, 68, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127, 38, 38, 38, 38, 38, 38, 38, 127}, /*HT_B20*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 127, 38, 38, 38, 127}  /*HT_B40*/
};
const u8 tx_shap_etsi[2][4] = {{0, 0, 0, 0}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_mkk[4][14] = {
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68}, /*CCK*/
	{76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 127}, /*OFDM*/
	{76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 127}, /*HT_B20*/
	{127, 127, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_mkk[3][28] = {
	{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 127, 127, 127, 127, 127, 127, 127, 127}, /*OFDM*/
	{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 127, 127, 127, 127, 127, 127, 127, 127}, /*HT_B20*/
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_mkk[2][4] = {{0, 0, 0, 0}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_ic[4][14] = {
	{80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 68, 62, 127}, /*CCK*/
	{66, 68, 72, 76, 76, 76, 76, 76, 72, 68, 62, 52, 46, 127}, /*OFDM*/
	{64, 68, 72, 76, 76, 76, 76, 76, 72, 68, 60, 48, 44, 127}, /*HT_B20*/
	{127, 127, 56, 60, 68, 68, 64, 60, 56, 52, 46, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_ic[3][28] = {
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127, 72, 72, 66, 72, 72, 72, 72, 72, 72, 127, 127, 127}, /*OFDM*/
	{72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72, 127, 127, 127, 72, 72, 68, 72, 72, 72, 72, 72, 72, 127, 127, 127}, /*HT_B20*/
	{72, 72, 72, 62, 58, 72, 127, 127, 72, 72, 72, 72, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_ic[2][4] = {{0, 1, 1, 1}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_kcc[4][14] = {
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127}, /*CCK*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127}, /*OFDM*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 127}, /*HT_B20*/
	{127, 127, 64, 64, 64, 64, 64, 64, 64, 64, 64, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_kcc[3][28] = {
	{62, 62, 62, 52, 70, 70, 66, 66, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 62, 62, 62, 62, 62, 127, 127, 127}, /*OFDM*/
	{62, 62, 62, 52, 70, 70, 66, 66, 6, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 62, 62, 62, 62, 62, 127, 127, 127}, /*HT_B20*/
	{54, 66, 66, 52, 56, 56, 56, 66, 66, 66, 66, 66, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_kcc[2][4] = {{0, 0, 0, 0}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_cn[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*CCK*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 60, 127}, /*OFDM*/
	{68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 58, 127}, /*HT_B20*/
	{127, 127, 68, 68, 68, 68, 68, 64, 62, 60, 60, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_cn[3][28] = {
	{68, 68, 68, 68, 68, 68, 68, 68, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 72, 72, 72, 72, 72, 127, 127, 127}, /*OFDM*/
	{68, 68, 68, 68, 68, 68, 68, 68, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 72, 72, 72, 72, 72, 127, 127, 127}, /*HT_B20*/
	{68, 68, 68, 68, 127, 127, 127, 127, 127, 127, 72, 72, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_cn[2][4] = {{0, 1, 1, 1}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_ww[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 68}, /*CCK*/
	{66, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 52, 46, 127}, /*OFDM*/
	{64, 68, 68, 68, 68, 68, 68, 68, 68, 68, 60, 48, 44, 127}, /*HT_B20*/
	{127, 127, 56, 60, 64, 64, 64, 60, 56, 52, 46, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_ww[3][28] = {
	{62, 62, 62, 52, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 64, 38, 38, 38, 38, 38, 38, 38, 68}, /*OFDM*/
	{62, 62, 62, 52, 60, 60, 60, 60, 6, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 64, 38, 38, 38, 38, 38, 38, 38, 68}, /*HT_B20*/
	{54, 60, 60, 52, 56, 56, 56, 60, 60, 60, 38, 38, 38, 68}  /*HT_B40*/
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
 *                           TX_Power byRate
 ******************************************************************************/
const s8 array_mp_txpwr_byrate_2g[] = {
	0x50, 0x50, 0x50, 0x50,  //11M 5.5M 2M 1M is 20dbm 20dbm 20dbm 20dBm
	0x4c, 0x4c, 0x4c, 0x4c,  //18M 12M 9M 6M is 19dbm 19dbm 19dbm 19dBm
	0x48, 0x48, 0x4c, 0x4c,  //54M 48M 36M 24M is 18dbm 18dbm 19dbm 19dBm
	0x4c, 0x4c, 0x4c, 0x4c,  // HT_MCS3 MCS2 MCS1 MCS0 is 19dbm 19dbm 19dbm 19dBm
	0x44, 0x44, 0x48, 0x4c  // HT_MCS7 MCS6 MCS5 MCS4 is 17dbm 17dbm 18dbm 19dBm
};
u8 array_mp_txpwr_byrate_2g_array_len = sizeof(array_mp_txpwr_byrate_2g) / sizeof(s8);
const s8 array_mp_txpwr_byrate_5g[] = {
	0x48, 0x48, 0x48, 0x48,  //18M 12M 9M 6M is 18dbm 18dbm 18dbm 18dBm
	0x40, 0x40, 0x44, 0x48,  //54M 48M 36M 24M is 16dbm 16dbm 17dbm 18dBm
	0x48, 0x48, 0x48, 0x48,  // HT_MCS3 MCS2 MCS1 MCS0 is 18dbm 18dbm 18dbm 18dBm
	0x3c, 0x3c, 0x40, 0x44  // HT_MCS7 MCS6 MCS5 MCS4 is 15dbm 15dbm 16dbm 17dBm
};
u8 array_mp_txpwr_byrate_5g_array_len = sizeof(array_mp_txpwr_byrate_5g) / sizeof(s8);

s8 wifi_hal_phy_get_power_limit_value(u8 regulation, u8 band, u8 limit_rate, u8 chnl_idx, boolean is_shape)
{
	s8 power_limit = 127;
	s8 tx_shape_idx = -1;

	switch (regulation) {
	case TXPWR_LMT_FCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_fcc[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_fcc[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_fcc[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_ETSI:
		if (is_shape) {
			tx_shape_idx = tx_shap_etsi[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_etsi[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_etsi[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_MKK:
		if (is_shape) {
			tx_shape_idx = tx_shap_mkk[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_mkk[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_mkk[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_IC:
		if (is_shape) {
			tx_shape_idx = tx_shap_ic[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ic[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ic[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_KCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_kcc[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_kcc[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_kcc[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_ACMA:
		if (is_shape) {
			tx_shape_idx = tx_shap_acma[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_acma[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_acma[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_NCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_ncc[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ncc[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ncc[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_MEXICO:
		if (is_shape) {
			tx_shape_idx = tx_shap_mexico[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_mexico[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_mexico[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_CHILE:
		if (is_shape) {
			tx_shape_idx = tx_shap_chile[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_chile[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_chile[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_UKRAINE:
		if (is_shape) {
			tx_shape_idx = tx_shap_ukraine[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ukraine[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ukraine[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_CN:
		if (is_shape) {
			tx_shape_idx = tx_shap_cn[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_cn[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_cn[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_QATAR:
		if (is_shape) {
			tx_shape_idx = tx_shap_qatar[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_qatar[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_qatar[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_UK:
		if (is_shape) {
			tx_shape_idx = tx_shap_uk[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_uk[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_uk[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_WW:
		if (is_shape) {
			tx_shape_idx = -1;
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ww[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ww[limit_rate][chnl_idx];
			}
		}
		break;
	case TXPWR_LMT_EXT:
		if (is_shape) {
			tx_shape_idx = tx_shap_ext[band][limit_rate];
		} else {
			if (band == BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ext[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ext[limit_rate][chnl_idx];
			}
		}
		break;
	default:
		break;
	}

	if (is_shape) {
		return tx_shape_idx;
	} else {
		return power_limit;
	}
}
