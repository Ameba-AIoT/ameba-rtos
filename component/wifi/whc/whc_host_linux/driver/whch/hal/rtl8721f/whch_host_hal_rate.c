#include <whc_host_linux.h>

#ifdef CONFIG_WHCH

u16 whc_host_hal_mrate_to_hwrate(u8 rate)
{
	u16 ret_rate = DESC_RATE1M;

	if (rate <= RTW_RATE_54M) {
		switch (rate) {
		case RTW_RATE_1M:
			ret_rate = DESC_RATE1M;
			break;
		case RTW_RATE_2M:
			ret_rate = DESC_RATE2M;
			break;
		case RTW_RATE_5_5M:
			ret_rate = DESC_RATE5_5M;
			break;
		case RTW_RATE_11M:
			ret_rate = DESC_RATE11M;
			break;

		case RTW_RATE_6M:
			ret_rate = DESC_RATE6M;
			break;
		case RTW_RATE_9M:
			ret_rate = DESC_RATE9M;
			break;
		case RTW_RATE_12M:
			ret_rate = DESC_RATE12M;
			break;
		case RTW_RATE_18M:
			ret_rate = DESC_RATE18M;
			break;
		case RTW_RATE_24M:
			ret_rate = DESC_RATE24M;
			break;
		case RTW_RATE_36M:
			ret_rate = DESC_RATE36M;
			break;
		case RTW_RATE_48M:
			ret_rate = DESC_RATE48M;
			break;
		case RTW_RATE_54M:
			ret_rate = DESC_RATE54M;
			break;
		default:
			break;
		}
	} else if ((rate >= RTW_RATE_MCS0) && (rate <= RTW_RATE_MCS7)) {
		ret_rate = GEN_HT_RATE_IDX(rate - RTW_RATE_MCS0);
	} else if ((rate >= RTW_RATE_VHT1SS_MCS0) && (rate <= RTW_RATE_VHT1SS_MCS8)) {
		ret_rate = GEN_VHT_RATE_IDX(0, (rate - RTW_RATE_VHT1SS_MCS0));
	} else if ((rate >= RTW_RATE_HE1SS_MCS0) && (rate <= RTW_RATE_HE1SS_MCS9)) {
		ret_rate = GEN_HE_RATE_IDX(0, (rate - RTW_RATE_HE1SS_MCS0));
	}

	return ret_rate;
}

#endif