/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "wifi_api.h"

#define CH_NULL 1

/******************************************************************************
 *                             TX_Power Limit
 ******************************************************************************/

// regu_en_3v3_1v25 = {FCC, MKK, ETSI, IC, KCC, ACMA, CHILE, MEXICO, WW, GL, UKRAINE, CN, QATAR, UK, NCC, EXT}
const bool regu_en_3v3_1v25[16] = {0};

u8 regu_en_array_len_3v3_1v25 = sizeof(regu_en_3v3_1v25) / sizeof(bool);

const s8 tx_pwr_limit_2g_fcc_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_fcc_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_fcc_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_etsi_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_etsi_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_etsi_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_mkk_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_mkk_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_mkk_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_ic_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ic_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_ic_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_kcc_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_kcc_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_kcc_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_cn_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_cn_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_cn_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_ww_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ww_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_acma_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_acma_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_acma_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_chile_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_chile_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_chile_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_mexico_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_mexico_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_mexico_3v3_1v25[][CH_NULL] = {{0}};

const s8 tx_pwr_limit_2g_ukraine_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ukraine_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_ukraine_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_qatar_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_qatar_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_qatar_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_uk_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_uk_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_uk_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ncc_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ncc_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_ncc_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_2g_ext_3v3_1v25[][CH_NULL] = {{0}};
const s8 tx_pwr_limit_5g_ext_3v3_1v25[][CH_NULL] = {{0}};
const u8 tx_shap_ext_3v3_1v25[][CH_NULL] = {{0}};


/******************************************************************************
 *                           TX_Power byRate
 ******************************************************************************/
const s8 array_mp_txpwr_byrate_2g_3v3_1v25[] = {0};
u8 array_mp_txpwr_byrate_2g_array_len_3v3_1v25 = sizeof(array_mp_txpwr_byrate_2g_3v3_1v25) / sizeof(s8);
const s8 array_mp_txpwr_byrate_5g_3v3_1v25[] = {0};
u8 array_mp_txpwr_byrate_5g_array_len_3v3_1v25 = sizeof(array_mp_txpwr_byrate_5g_3v3_1v25) / sizeof(s8);

s8 wifi_hal_phy_get_power_limit_value_3v3_1v25(u8 regulation, u8 band, u8 limit_rate, u8 chnl_idx, bool is_shape)
{
	s8 power_limit = 127;
	s8 tx_shape_idx = -1;

	switch (regulation) {
	case TXPWR_LMT_FCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_fcc_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_fcc_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_fcc_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_ETSI:
		if (is_shape) {
			tx_shape_idx = tx_shap_etsi_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_etsi_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_etsi_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_MKK:
		if (is_shape) {
			tx_shape_idx = tx_shap_mkk_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_mkk_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_mkk_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_IC:
		if (is_shape) {
			tx_shape_idx = tx_shap_ic_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ic_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ic_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_KCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_kcc_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_kcc_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_kcc_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_ACMA:
		if (is_shape) {
			tx_shape_idx = tx_shap_acma_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_acma_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_acma_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_NCC:
		if (is_shape) {
			tx_shape_idx = tx_shap_ncc_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ncc_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ncc_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_MEXICO:
		if (is_shape) {
			tx_shape_idx = tx_shap_mexico_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_mexico_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_mexico_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_CHILE:
		if (is_shape) {
			tx_shape_idx = tx_shap_chile_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_chile_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_chile_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_UKRAINE:
		if (is_shape) {
			tx_shape_idx = tx_shap_ukraine_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ukraine_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ukraine_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_CN:
		if (is_shape) {
			tx_shape_idx = tx_shap_cn_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_cn_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_cn_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_QATAR:
		if (is_shape) {
			tx_shape_idx = tx_shap_qatar_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_qatar_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_qatar_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_UK:
		if (is_shape) {
			tx_shape_idx = tx_shap_uk_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_uk_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_uk_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;

	case TXPWR_LMT_WW:
		if (is_shape) {
			tx_shape_idx = -1;
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ww_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ww_3v3_1v25[limit_rate][chnl_idx];
			}
		}
		break;
	case TXPWR_LMT_EXT:
		if (is_shape) {
			tx_shape_idx = tx_shap_ext_3v3_1v25[band][limit_rate];
		} else {
			if (band == RTW_BAND_ON_24G) {
				power_limit = tx_pwr_limit_2g_ext_3v3_1v25[limit_rate][chnl_idx];
			} else {
				power_limit = tx_pwr_limit_5g_ext_3v3_1v25[limit_rate][chnl_idx];
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
