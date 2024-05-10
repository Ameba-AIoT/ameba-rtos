/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_Flash_SST26VF064B.h"
#include "RTE_Device.h"
#include "flash_device_definition.h"

#include "cmsis.h"
#include "mpu_armv8.h"
#include "assert.h"

#if (RTE_FLASH0)

#define MPU_REGION_NUMBER   8

struct mpu_state_save {
    uint32_t mpu;
    uint32_t shcsr;
    uint32_t mair[2];
    ARM_MPU_Region_t mpu_table[MPU_REGION_NUMBER];
};

static struct mpu_state_save mpu_s;
static struct mpu_state_save mpu_ns;
static uint32_t irq_state = 0;

__STATIC_INLINE uint32_t __save_disable_irq(void)
{
    uint32_t result = 0;

    __ASM volatile ("mrs %0, primask \n cpsid i" : "=r" (result) :: "memory");
    return result;
}

__STATIC_INLINE void __restore_irq(uint32_t status)
{
    __ASM volatile ("msr primask, %0" :: "r" (status) : "memory");
}

static void mpu_save(struct arm_flash_sst26vf064b_flash_dev_t* flash_dev,
                     MPU_Type* mpu,
                     SCB_Type* scb,
                     struct mpu_state_save* mpu_state)
{
    static const uint8_t mpu_attr_num = 0;
    const uint32_t qspi_flash_size =
        flash_dev->dev->total_sector_cnt * flash_dev->dev->sector_size;
    uint32_t memory_base;

    mpu_state->shcsr = scb->SHCSR;
    mpu_state->mpu = mpu->CTRL;

    if(mpu == MPU) {
        ARM_MPU_Disable();
    } else {
        ARM_MPU_Disable_NS();
    }

    for(uint8_t i = 0; i < MPU_REGION_NUMBER; i++) {
        mpu->RNR = i;
        mpu_state->mpu_table[i].RBAR = mpu->RBAR;
        mpu_state->mpu_table[i].RLAR = mpu->RLAR;
        mpu->RBAR = 0;
        mpu->RLAR = 0;
    }
    mpu_state->mair[0] = mpu->MAIR[0];
    mpu_state->mair[1] = mpu->MAIR[1];

    mpu->MAIR[0] = 0;
    mpu->MAIR[1] = 0;

    /* Attr0 : Device memory, nGnRE */
    if(mpu == MPU) {
        ARM_MPU_SetMemAttr(mpu_attr_num,
                           ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE,
                                        ARM_MPU_ATTR_DEVICE_nGnRE));
        memory_base = flash_dev->memory_base_s;
    } else {
        ARM_MPU_SetMemAttr_NS(mpu_attr_num,
                              ARM_MPU_ATTR(ARM_MPU_ATTR_DEVICE,
                                           ARM_MPU_ATTR_DEVICE_nGnRE));
        memory_base = flash_dev->memory_base_ns;
    }

    mpu->RNR = 0;
    mpu->RBAR = ARM_MPU_RBAR(memory_base,
                             ARM_MPU_SH_NON,
                             1,
                             0,
                             1);
    #ifdef TFM_PXN_ENABLE
    mpu->RLAR = ARM_MPU_RLAR_PXN((memory_base + qspi_flash_size) - 1, 1, mpu_attr_num);
    #else
    mpu->RLAR = ARM_MPU_RLAR((memory_base + qspi_flash_size) - 1, mpu_attr_num);
    #endif

    if(mpu == MPU) {
        ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
    } else {
        ARM_MPU_Enable_NS(MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk);
    }
}

static void mpu_restore(MPU_Type* mpu,
                        SCB_Type* scb,
                        struct mpu_state_save* mpu_state)
{
    if(mpu == MPU) {
        ARM_MPU_Disable();
    } else {
        ARM_MPU_Disable_NS();
    }

    for(uint8_t i = 0; i < MPU_REGION_NUMBER; i++) {
        mpu->RNR = i;
        mpu->RBAR = mpu_state->mpu_table[i].RBAR;
        mpu->RLAR = mpu_state->mpu_table[i].RLAR;
    }

    mpu->MAIR[0] = mpu_state->mair[0];
    mpu->MAIR[1] = mpu_state->mair[1];

    __DMB();
    mpu->CTRL = mpu_state->mpu;
#ifdef SCB_SHCSR_MEMFAULTENA_Msk
    scb->SHCSR = mpu_state->shcsr;
#endif
    __DSB();
    __ISB();
}

static int8_t setup(struct arm_flash_sst26vf064b_flash_dev_t* flash_dev) {
    irq_state = __save_disable_irq();
    mpu_save(flash_dev, MPU, SCB, &mpu_s);
    mpu_save(flash_dev, MPU_NS, SCB_NS, &mpu_ns);
    if(flash_dev->dev->is_initialized &&
       (select_qspi_mode(flash_dev->dev->controller) != AXI_QSPI_ERR_NONE)) {
            return -1;
    }
    return 0;
}

static int8_t release(struct arm_flash_sst26vf064b_flash_dev_t* flash_dev) {
    enum axi_qspi_error_t ret = select_xip_mode(flash_dev->dev->controller);
    mpu_restore(MPU, SCB, &mpu_s);
    mpu_restore(MPU_NS, SCB_NS, &mpu_ns);
    __restore_irq(irq_state);

    if(ret != AXI_QSPI_ERR_NONE) {
        return -1;
    } else {
        return 0;
    }
}
static struct arm_flash_sst26vf064b_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &SPI_SST26VF064B_DEV,
    .data   = &SST26VF064B_DEV_DATA,
    .setup_qspi = setup,
    .release_qspi = release,
    .memory_base_s = QSPI_SRAM_BASE_S,
    .memory_base_ns = QSPI_SRAM_BASE_NS,
};

ARM_FLASH_SST26VF064B(ARM_FLASH0_DEV, Driver_FLASH0);

#endif /* RTE_FLASH0 */
