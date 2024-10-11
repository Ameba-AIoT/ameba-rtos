/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cmsis.h"
#include "mpu_config.h"

static const char *TAG = "MPU";
u8 mpu_entry_register[MPU_MAX_REGION];

/**
  * @brief  init mpu memory attribute to typical value.
  * @param  NA.
  * @note   typical value:
  *              - typical value if defined in @ref mpu_mem_attri_typical_define.
  * @retval   None
  */
void mpu_init(void)
{
	ARM_MPU_Disable();

	/* Configure memory types */
	MPU->MAIR0 = ((MPU_MEM_ATTR0) << MPU_MAIR0_Attr0_Pos) | \
				 ((MPU_MEM_ATTR1) << MPU_MAIR0_Attr1_Pos) | \
				 ((MPU_MEM_ATTR2) << MPU_MAIR0_Attr2_Pos) | \
				 ((MPU_MEM_ATTR3) << MPU_MAIR0_Attr3_Pos);

	MPU->MAIR1 = ((MPU_MEM_ATTR4) << MPU_MAIR1_Attr4_Pos) | \
				 ((MPU_MEM_ATTR5) << MPU_MAIR1_Attr5_Pos) | \
				 ((MPU_MEM_ATTR6) << MPU_MAIR1_Attr6_Pos) | \
				 ((MPU_MEM_ATTR7) << MPU_MAIR1_Attr7_Pos);

	/* PRIVDEFENA Privileged background region enable:
	 *    <0=> All accesses to unmapped addresses result in faults.
	 *    <1=> Enables the default memory map for privilege code when the address accessed does not map into any MPU region.
	 * HFNMIENA MPU Enable for HardFault and NMI (Non-Maskable Interrupt):
	 *    <0=> HardFault and NMI handlers bypass MPU configuration as if MPU is disabled.
	 *    <1=> MPU access rules apply to HardFault and NMI handlers.
	 */
	ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk & ~MPU_CTRL_HFNMIENA_Msk);
}

/**
  * @brief  change mpu memory attribute.
  * @param  attr_idx: where x can be 0~7.
  * @param  mem_attr: memory attribute. @ref mpu_region_memory_attribute_define.
  * @note   NA:
  *              - NA.
  * @retval   None
  */
void mpu_set_mem_attr(uint8_t attr_idx, uint8_t mem_attr)
{
	assert_param(attr_idx < MPU_MAX_REGION);

	ARM_MPU_Disable();
	ARM_MPU_SetMemAttr(attr_idx, mem_attr);
	ARM_MPU_Enable(MPU->CTRL);
}

/**
  * @brief  config mpu region memory attribute.
  * @param  region_num: where x can be 0~3. (KM0 & KM4 both)
  * @param  pmpu_cfg: pointer to an mpu_region_config structure which has been configured.
  * @note    NA:
  *              - NA.
  * @retval   None
  */
void mpu_region_cfg(uint8_t region_num, mpu_region_config *pmpu_cfg)
{
	MPU_Type *mpu = MPU;
	u32 region_limit = pmpu_cfg->region_base + pmpu_cfg->region_size - 1;

	assert_param(region_num < MPU_MAX_REGION);
	assert_param((pmpu_cfg->region_base & ~MPU_RBAR_BASE_Msk) == 0);
	assert_param((pmpu_cfg->region_size & ~MPU_RLAR_LIMIT_Msk) == 0);

	ARM_MPU_Disable();

	mpu->RNR = region_num;
	mpu->RBAR = (pmpu_cfg->region_base & MPU_RBAR_BASE_Msk) | \
				pmpu_cfg->sh | \
				pmpu_cfg->ap | \
				pmpu_cfg->xn;
	mpu->RLAR = ARM_MPU_RLAR(region_limit, pmpu_cfg->attr_idx);

	ARM_MPU_Enable(mpu->CTRL);

	RTK_LOGD(TAG, "mpu_region[%d] RBAR:0x%lx RLAR:0x%lx attr_idx:%d\n", mpu->RNR, mpu->RBAR, mpu->RLAR, pmpu_cfg->attr_idx);
	RTK_LOGD(TAG, "MAIR0:0x%lx MAIR1:0x%lx CTRL:0x%lx TYPE:0x%lx\n", mpu->MAIR0, mpu->MAIR1, mpu->CTRL, mpu->TYPE);
}

/**
  * @brief  free mpu entry.
  * @param  entry_index: KM0: 0~3, KM4_NS: 0~7; KM4_S: 0~3.
  * @retval   None
  */
void mpu_entry_free(u32 entry_index)
{
	mpu_entry_register[entry_index] = 0;
}

/**
  * @brief  alloc a free mpu entry.
  * @param  NA.
  * @retval value: mpu entry index KM0: 0~3, KM4_NS: 0~7; KM4_S: 0~3, -1 is fail.
  */
char mpu_entry_alloc(void)
{
	u32 entry_index = 0;

	for (entry_index = 0; entry_index < MPU_MAX_REGION; entry_index++) {
		if (mpu_entry_register[entry_index] == 0) {
			mpu_entry_register[entry_index] = 1;
			return entry_index;
		}
	}

	return -1;
}
