/******************************************************************************
 *
 * Copyright(c) 2020 - 2021 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include "whc_dev.h"

/**
 * @brief  handle whc_dev_ps_cmd from host
 * @param  ps_cmd: pointer of tickps cmd from host
 * @return none.
 */
void whc_dev_tickps_cmd(struct whc_dev_ps_cmd *ps_cmd)
{
	uint8_t subtype = ps_cmd->type;
	whc_dev_ps_set_tickps_cmd(subtype);

	return;
}

/**
 * @brief  tickps related
 * @param  subtype: tickps subtype, refer to enum whc_tickps_cmd_subtype
 * @return none.
 */
void whc_dev_ps_set_tickps_cmd(uint8_t subtype)
{
	if (subtype == WHC_CMD_TICKPS_R) {
		/* dev enter low power mode after release lock */
		pmu_release_wakelock(PMU_OS);
		pmu_release_wakelock(PMU_WHC_WIFI);
	} else if (subtype == WHC_CMD_TICKPS_A) {
		pmu_acquire_wakelock(PMU_OS);
		pmu_acquire_wakelock(PMU_WHC_WIFI);
	} else if (subtype == WHC_CMD_TICKPS_TYPE_PG) {
		pmu_set_sleep_type(SLEEP_PG);
	} else if (subtype == WHC_CMD_TICKPS_TYPE_CG) {
		pmu_set_sleep_type(SLEEP_CG);
	}
}

/**
 * @brief  callback func for host wake dev
 * @param  none
 * @return none.
 */
void whc_dev_ps_resume_cb(void)
{
	/* acquire wakelock to prevent dev enter low power mode */
	whc_dev_ps_set_tickps_cmd(WHC_CMD_TICKPS_A);
#if defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST && !defined(CONFIG_MP_SHRINK)
	extern void wifi_hal_system_resume_wlan(void);
	/* normal wowlan resume by pkt rx. here by host tx */
	wifi_hal_system_resume_wlan();
#endif
}
