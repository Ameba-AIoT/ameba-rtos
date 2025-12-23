/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "wifi_api.h"

#define CH_NULL 1

/* allow customer to adjust the pwrlmt regu corresponding to domain code, for example*/
volatile struct _pwr_lmt_regu_remap pwrlmt_regu_remapping_1v8[1] = {{0}};

volatile u8 array_len_of_pwrlmt_regu_remapping_1v8 = sizeof(pwrlmt_regu_remapping_1v8) / sizeof(struct _pwr_lmt_regu_remap);

/******************************************************************************
 *                             TX_Power Limit
 ******************************************************************************/

// regu_en_1v8 = {FCC, MKK, ETSI, IC, KCC, ACMA, CHILE, MEXICO, WW, GL, UKRAINE, CN, QATAR, UK, NCC, EXT}
const bool regu_en_1v8[16] = {1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0};

u8 regu_en_array_len_1v8 = sizeof(regu_en_1v8) / sizeof(bool);

const s8 tx_pwr_limit_2g_fcc_1v8[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 46, 36, 127}, /*CCK*/
	{48, 56, 56, 56, 56, 56, 56, 56, 56, 48, 44, 30, 24, 127}, /*OFDM*/
	{44, 48, 56, 56, 56, 56, 56, 56, 56, 56, 40, 30, 24, 127}, /*HT_B20*/
	{127, 127, 36, 44, 48, 56, 48, 40, 34, 30, 28, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_fcc_1v8[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48}, /*OFDM*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48}, /*HT_B20*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48}  /*HT_B40*/
};
const u8 tx_shap_fcc_1v8[2][4] = {{0, 1, 1, 1}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_etsi_1v8[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*CCK*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}, /*OFDM*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}, /*HT_B20*/
	{127, 127, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_etsi_1v8[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 38, 38, 38, 38, 38, 38, 38, 127}, /*OFDM*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 38, 38, 38, 38, 38, 38, 38, 127}, /*HT_B20*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 38, 38, 38, 127}  /*HT_B40*/
};
const u8 tx_shap_etsi_1v8[2][4] = {{0, 0, 0, 0}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_mkk_1v8[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60}, /*CCK*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}, /*OFDM*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}, /*HT_B20*/
	{127, 127, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_mkk_1v8[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 127, 127, 127, 127, 127}, /*OFDM*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 127, 127, 127, 127, 127}, /*HT_B20*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_mkk_1v8[2][4] = {{0, 0, 0, 0}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_ic_1v8[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 46, 36, 127}, /*CCK*/
	{48, 56, 56, 56, 56, 56, 56, 56, 56, 48, 44, 30, 24, 127}, /*OFDM*/
	{44, 48, 56, 56, 56, 56, 56, 56, 56, 56, 40, 30, 24, 127}, /*HT_B20*/
	{127, 127, 36, 44, 48, 56, 48, 40, 34, 30, 28, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_ic_1v8[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127}, /*OFDM*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127}, /*HT_B20*/
	{48, 48, 48, 48, 48, 48, 127, 127, 48, 48, 48, 48, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_ic_1v8[2][4] = {{0, 1, 1, 1}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_kcc_1v8[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 127}, /*CCK*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}, /*OFDM*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127}, /*HT_B20*/
	{127, 127, 56, 56, 56, 56, 56, 56, 56, 56, 56, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_kcc_1v8[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127}, /*OFDM*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127}, /*HT_B20*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_kcc_1v8[2][4] = {{0, 0, 0, 0}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_cn_1v8[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 52, 127}, /*CCK*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 48, 44, 40, 127}, /*OFDM*/
	{56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 48, 42, 40, 127}, /*HT_B20*/
	{127, 127, 52, 56, 56, 56, 52, 46, 44, 44, 44, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_cn_1v8[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 48, 48, 48, 48, 48, 127, 127, 127}, /*OFDM*/
	{48, 48, 48, 48, 48, 48, 48, 48, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 48, 48, 48, 48, 48, 127, 127, 127}, /*HT_B20*/
	{48, 48, 48, 48, 127, 127, 127, 127, 127, 127, 48, 48, 127, 127}  /*HT_B40*/
};
const u8 tx_shap_cn_1v8[2][4] = {{0, 1, 1, 1}, {0, 0, 0}}; /*{2G{CCK, OFDM, HE_B20, HE_B40}, 5G{OFDM, HE_B20, HE_B40}}*/

const s8 tx_pwr_limit_2g_ww_1v8[4][14] = {
	{60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 46, 36, 60}, /*CCK*/
	{48, 56, 56, 56, 56, 56, 56, 56, 56, 48, 44, 30, 24, 127}, /*OFDM*/
	{44, 48, 56, 56, 56, 56, 56, 56, 56, 56, 40, 30, 24, 127}, /*HT_B20*/
	{127, 127, 36, 44, 48, 56, 48, 40, 34, 30, 28, 127, 127, 127}  /*HT_B40*/
};
const s8 tx_pwr_limit_5g_ww_1v8[3][28] = {
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 38, 38, 38, 38, 38, 38, 38, 48}, /*OFDM*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 38, 38, 38, 38, 38, 38, 38, 48}, /*HT_B20*/
	{48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 38, 38, 38, 48}  /*HT_B40*/
};
const s8 tx_pwr_limit_2g_acma_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_acma_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_acma_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_chile_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_chile_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_chile_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_mexico_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_mexico_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_mexico_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ukraine_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ukraine_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_ukraine_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_qatar_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_qatar_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_qatar_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_uk_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_uk_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_uk_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ncc_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ncc_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_ncc_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ext_1v8[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ext_1v8[][CH_NULL] = {{0}};
const u8 tx_shap_ext_1v8[][CH_NULL] = {{0}};


/******************************************************************************
 *                           TX_Power byRate
 ******************************************************************************/
const s8 array_mp_txpwr_byrate_2g_1v8[] = {
	0x3c, 0x3c, 0x3c, 0x3c,  //11M 5.5M 2M 1M is 15dbm 15dbm 15dbm 15dBm
	0x38, 0x38, 0x38, 0x38,  //18M 12M 9M 6M is 14dbm 14dbm 14dbm 14dBm
	0x30, 0x30, 0x34, 0x38,  //54M 48M 36M 24M is 12dbm 12dbm 13dbm 14dBm
	0x38, 0x38, 0x38, 0x38,  // HT_MCS3 MCS2 MCS1 MCS0 is 14dbm 14dbm 14dbm 14dBm
	0x2c, 0x2c, 0x30, 0x34  // HT_MCS7 MCS6 MCS5 MCS4 is 11dbm 11dbm 12dbm 13dBm
};
u8 array_mp_txpwr_byrate_2g_array_len_1v8 = sizeof(array_mp_txpwr_byrate_2g_1v8) / sizeof(s8);
const s8 array_mp_txpwr_byrate_5g_1v8[] = {
	0x30, 0x30, 0x30, 0x30,  //18M 12M 9M 6M is 12dbm 12dbm 12dbm 12dBm
	0x24, 0x24, 0x28, 0x2c,  //54M 48M 36M 24M is 9dbm 9dbm 10dbm 11dBm
	0x2c, 0x30, 0x30, 0x30,  // HT_MCS3 MCS2 MCS1 MCS0 is 11dbm 12dbm 12dbm 12dBm
	0x20, 0x20, 0x24, 0x28  // HT_MCS7 MCS6 MCS5 MCS4 is 8dbm 8dbm 9dbm 10dBm
};
u8 array_mp_txpwr_byrate_5g_array_len_1v8 = sizeof(array_mp_txpwr_byrate_5g_1v8) / sizeof(s8);

s8 wifi_hal_phy_get_power_limit_value_1v8(u8 regulation, u8 band, u8 limit_rate, u8 chnl_idx, bool is_shape)
{
	s8 power_limit = 127;
	s8 tx_shape_idx = -1;

	switch (regulation) {
	case TXPWR_LMT_FCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_fcc_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_fcc_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_fcc_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_ETSI:
		if (is_shape) {
			tx_shape_idx = tx_shap_etsi_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_etsi_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_etsi_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_MKK:
		if (is_shape) {
			tx_shape_idx = tx_shap_mkk_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_mkk_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_mkk_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_IC:
		if (is_shape) {
			tx_shape_idx = tx_shap_ic_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ic_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ic_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_KCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_kcc_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_kcc_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_kcc_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_ACMA:
		if (is_shape) {
			tx_shape_idx = tx_shap_acma_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_acma_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_acma_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_NCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_ncc_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ncc_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ncc_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_MEXICO:
		if (is_shape) {
			tx_shape_idx = tx_shap_mexico_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_mexico_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_mexico_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_CHILE:
		if (is_shape) {
			tx_shape_idx = tx_shap_chile_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_chile_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_chile_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_UKRAINE:
		if (is_shape) {
			tx_shape_idx = tx_shap_ukraine_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ukraine_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ukraine_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_CN:
		if (is_shape) {
			tx_shape_idx = tx_shap_cn_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_cn_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_cn_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_QATAR:
		if (is_shape) {
			tx_shape_idx = tx_shap_qatar_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_qatar_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_qatar_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_UK:
		if (is_shape) {
			tx_shape_idx = tx_shap_uk_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_uk_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_uk_1v8[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_WW:
		if (is_shape) {
			tx_shape_idx = -1;
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ww_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ww_1v8[limit_rate][chnl_idx];
			}
		}
		break;
	case TXPWR_LMT_EXT:
		if (is_shape) {
			tx_shape_idx = tx_shap_ext_1v8[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ext_1v8[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ext_1v8[limit_rate][chnl_idx];
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
