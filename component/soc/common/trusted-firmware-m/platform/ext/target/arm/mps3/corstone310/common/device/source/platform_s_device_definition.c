/*
 * Copyright (c) 2019-2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file platform_ns_device_definition.c
 * \brief This file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#include "platform_s_device_definition.h"
#include "platform_base_address.h"
#include "platform/include/tfm_plat_defs.h"

/* UART CMSDK driver structures */
static const struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_NS = {
    .base = UART0_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t UART0_CMSDK_DEV_NS = {
    &(UART0_CMSDK_DEV_CFG_NS),
    &(UART0_CMSDK_DEV_DATA_NS)
};

/* Corstone-310 PPC driver structures */
static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_MAIN0_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_MAIN0};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_MAIN0_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_MAIN0_DEV_S = {
    &PPC_CORSTONE310_MAIN0_CFG_S,
    &PPC_CORSTONE310_MAIN0_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_MAIN_EXP0_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_MAIN_EXP0};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_MAIN_EXP0_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_MAIN_EXP0_DEV_S = {
    &PPC_CORSTONE310_MAIN_EXP0_CFG_S,
    &PPC_CORSTONE310_MAIN_EXP0_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_MAIN_EXP1_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_MAIN_EXP1};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_MAIN_EXP1_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_MAIN_EXP1_DEV_S = {
    &PPC_CORSTONE310_MAIN_EXP1_CFG_S,
    &PPC_CORSTONE310_MAIN_EXP1_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_MAIN_EXP2_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_MAIN_EXP2};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_MAIN_EXP2_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_MAIN_EXP2_DEV_S = {
    &PPC_CORSTONE310_MAIN_EXP2_CFG_S,
    &PPC_CORSTONE310_MAIN_EXP2_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_MAIN_EXP3_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_MAIN_EXP3};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_MAIN_EXP3_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_MAIN_EXP3_DEV_S = {
    &PPC_CORSTONE310_MAIN_EXP3_CFG_S,
    &PPC_CORSTONE310_MAIN_EXP3_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_PERIPH0_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_PERIPH0};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_PERIPH0_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_PERIPH0_DEV_S = {
    &PPC_CORSTONE310_PERIPH0_CFG_S,
    &PPC_CORSTONE310_PERIPH0_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_PERIPH1_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_PERIPH1};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_PERIPH1_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_PERIPH1_DEV_S = {
    &PPC_CORSTONE310_PERIPH1_CFG_S,
    &PPC_CORSTONE310_PERIPH1_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_PERIPH_EXP0_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_PERIPH_EXP0};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_PERIPH_EXP0_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_PERIPH_EXP0_DEV_S = {
    &PPC_CORSTONE310_PERIPH_EXP0_CFG_S,
    &PPC_CORSTONE310_PERIPH_EXP0_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_PERIPH_EXP1_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_PERIPH_EXP1};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_PERIPH_EXP1_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_PERIPH_EXP1_DEV_S = {
    &PPC_CORSTONE310_PERIPH_EXP1_CFG_S,
    &PPC_CORSTONE310_PERIPH_EXP1_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_PERIPH_EXP2_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_PERIPH_EXP2};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_PERIPH_EXP2_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_PERIPH_EXP2_DEV_S = {
    &PPC_CORSTONE310_PERIPH_EXP2_CFG_S,
    &PPC_CORSTONE310_PERIPH_EXP2_DATA_S };

static struct ppc_corstone310_dev_cfg_t PPC_CORSTONE310_PERIPH_EXP3_CFG_S = {
    .sacfg_base  = CORSTONE310_SACFG_BASE_S,
    .nsacfg_base = CORSTONE310_NSACFG_BASE_NS,
    .ppc_name = PPC_CORSTONE310_PERIPH_EXP3};
static struct ppc_corstone310_dev_data_t PPC_CORSTONE310_PERIPH_EXP3_DATA_S = {
    .sacfg_ns_ppc  = 0,
    .sacfg_sp_ppc  = 0,
    .nsacfg_nsp_ppc = 0,
    .int_bit_mask = 0,
    .is_initialized = false };
struct ppc_corstone310_dev_t PPC_CORSTONE310_PERIPH_EXP3_DEV_S = {
    &PPC_CORSTONE310_PERIPH_EXP3_CFG_S,
    &PPC_CORSTONE310_PERIPH_EXP3_DATA_S };

/* System counters */
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT is invalid.
#endif

static const struct syscounter_armv8_m_cntrl_dev_cfg_t
SYSCOUNTER_CNTRL_ARMV8_M_DEV_CFG_S = {
    .base = SYSCNTR_CNTRL_BASE_S,
    .scale0.integer  = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT,
    .scale0.fixed_point_fraction = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT,
    .scale1.integer  = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT,
    .scale1.fixed_point_fraction = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT
};
static struct syscounter_armv8_m_cntrl_dev_data_t
SYSCOUNTER_CNTRL_ARMV8_M_DEV_DATA_S = {
    .is_initialized = false
};
struct syscounter_armv8_m_cntrl_dev_t SYSCOUNTER_CNTRL_ARMV8_M_DEV_S = {
    &(SYSCOUNTER_CNTRL_ARMV8_M_DEV_CFG_S),
    &(SYSCOUNTER_CNTRL_ARMV8_M_DEV_DATA_S)
};

/* System timers */
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER0_ARMV8_M_DEV_CFG_S
#ifdef TEST_NS_SLIH_IRQ
    TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_SLIH_TEST", "APP-ROT")
#elif defined(TEST_NS_FLIH_IRQ)
    TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_FLIH_TEST", "APP-ROT")
#endif
    = {
    .base = SYSTIMER0_ARMV8_M_BASE_S,
    .default_freq_hz = SYSTIMER0_ARMV8M_DEFAULT_FREQ_HZ
};
static struct systimer_armv8_m_dev_data_t SYSTIMER0_ARMV8_M_DEV_DATA_S
#ifdef TEST_NS_SLIH_IRQ
    TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_SP_SLIH_TEST", "APP-ROT")
#elif defined(TEST_NS_FLIH_IRQ)
    TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_SP_FLIH_TEST", "APP-ROT")
#endif
    = {
    .is_initialized = false
};
struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_S
#ifdef TEST_NS_SLIH_IRQ
    TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_SP_SLIH_TEST", "APP-ROT")
#elif defined(TEST_NS_FLIH_IRQ)
    TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_SP_FLIH_TEST", "APP-ROT")
#endif
    = {
    &(SYSTIMER0_ARMV8_M_DEV_CFG_S),
    &(SYSTIMER0_ARMV8_M_DEV_DATA_S)
};

/* ARM MPC SIE 300 driver structures */
/* Ranges controlled by this SRAM_MPC */
static const struct mpc_sie_memory_range_t MPC_SRAM_RANGE_S = {
    .base         = MPC_SRAM_RANGE_BASE_S,
    .limit        = MPC_SRAM_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_SRAM_RANGE_NS = {
    .base         = MPC_SRAM_RANGE_BASE_NS,
    .limit        = MPC_SRAM_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_SRAM_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_SRAM_RANGE_LIST[MPC_SRAM_RANGE_LIST_LEN] = {
        &MPC_SRAM_RANGE_S,
        &MPC_SRAM_RANGE_NS
    };

static struct mpc_sie_dev_cfg_t MPC_SRAM_DEV_CFG_S = {
    .base = MPC_SRAM_BASE_S,
    .range_list = MPC_SRAM_RANGE_LIST,
    .nbr_of_ranges = MPC_SRAM_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_SRAM_DEV_DATA_S = {
    .is_initialized = false};
struct mpc_sie_dev_t MPC_SRAM_DEV_S = {
    &(MPC_SRAM_DEV_CFG_S),
    &(MPC_SRAM_DEV_DATA_S)};

/* Ranges controlled by this QSPI_MPC */
static const struct mpc_sie_memory_range_t MPC_QSPI_RANGE_S = {
    .base         = MPC_QSPI_RANGE_BASE_S,
    .limit        = MPC_QSPI_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_QSPI_RANGE_NS = {
    .base         = MPC_QSPI_RANGE_BASE_NS,
    .limit        = MPC_QSPI_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_QSPI_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_QSPI_RANGE_LIST[MPC_QSPI_RANGE_LIST_LEN] = {
        &MPC_QSPI_RANGE_S,
        &MPC_QSPI_RANGE_NS
    };

static struct mpc_sie_dev_cfg_t MPC_QSPI_DEV_CFG_S = {
    .base = MPC_QSPI_BASE_S,
    .range_list = MPC_QSPI_RANGE_LIST,
    .nbr_of_ranges = MPC_QSPI_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_QSPI_DEV_DATA_S = {
    .is_initialized = false};
struct mpc_sie_dev_t MPC_QSPI_DEV_S = {
    &(MPC_QSPI_DEV_CFG_S),
    &(MPC_QSPI_DEV_DATA_S)};

/* Ranges controlled by this ISRAM0_MPC */
static const struct mpc_sie_memory_range_t MPC_ISRAM0_RANGE_S = {
    .base         = MPC_ISRAM0_RANGE_BASE_S,
    .limit        = MPC_ISRAM0_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_ISRAM0_RANGE_NS = {
    .base         = MPC_ISRAM0_RANGE_BASE_NS,
    .limit        = MPC_ISRAM0_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM0_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_ISRAM0_RANGE_LIST[MPC_ISRAM0_RANGE_LIST_LEN] = {
        &MPC_ISRAM0_RANGE_S,
        &MPC_ISRAM0_RANGE_NS
    };

static struct mpc_sie_dev_cfg_t MPC_ISRAM0_DEV_CFG_S = {
    .base = MPC_ISRAM0_BASE_S,
    .range_list = MPC_ISRAM0_RANGE_LIST,
    .nbr_of_ranges = MPC_ISRAM0_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_ISRAM0_DEV_DATA_S = {
    .is_initialized = false};
struct mpc_sie_dev_t MPC_ISRAM0_DEV_S = {
    &(MPC_ISRAM0_DEV_CFG_S),
    &(MPC_ISRAM0_DEV_DATA_S)};

/* Ranges controlled by this ISRAM1_MPC */
static const struct mpc_sie_memory_range_t MPC_ISRAM1_RANGE_S = {
    .base         = MPC_ISRAM1_RANGE_BASE_S,
    .limit        = MPC_ISRAM1_RANGE_LIMIT_S,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_ISRAM1_RANGE_NS = {
    .base         = MPC_ISRAM1_RANGE_BASE_NS,
    .limit        = MPC_ISRAM1_RANGE_LIMIT_NS,
    .range_offset = 0,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM1_RANGE_LIST_LEN  2u
static const struct mpc_sie_memory_range_t*
    MPC_ISRAM1_RANGE_LIST[MPC_ISRAM1_RANGE_LIST_LEN] = {
        &MPC_ISRAM1_RANGE_S,
        &MPC_ISRAM1_RANGE_NS
    };

static struct mpc_sie_dev_cfg_t MPC_ISRAM1_DEV_CFG_S = {
    .base = MPC_ISRAM1_BASE_S,
    .range_list = MPC_ISRAM1_RANGE_LIST,
    .nbr_of_ranges = MPC_ISRAM1_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_ISRAM1_DEV_DATA_S = {
    .is_initialized = false};
struct mpc_sie_dev_t MPC_ISRAM1_DEV_S = {
    &(MPC_ISRAM1_DEV_CFG_S),
    &(MPC_ISRAM1_DEV_DATA_S)};

#ifdef CORSTONE310_FVP
/* DMA350 driver structures */
static const struct dma350_dev_cfg_t DMA350_DMA0_DEV_CFG_S = {
    .dma_sec_cfg =   (DMASECCFG_TypeDef*)   (DMA_350_BASE_S + 0x0UL),
    .dma_sec_ctrl =  (DMASECCTRL_TypeDef*)  (DMA_350_BASE_S + 0x100UL),
    .dma_nsec_ctrl = (DMANSECCTRL_TypeDef*) (DMA_350_BASE_S + 0x200UL),
    .dma_info =      (DMAINFO_TypeDef*)     (DMA_350_BASE_S + 0xF00UL)
};
static struct dma350_dev_data_t DMA350_DMA0_DEV_DATA_S = {
    .state = 0
};
struct dma350_dev_t DMA350_DMA0_DEV_S = {
    &(DMA350_DMA0_DEV_CFG_S),
    &(DMA350_DMA0_DEV_DATA_S)
};

#ifdef PLATFORM_SVC_HANDLERS
struct dma350_ch_dev_t DMA350_DMA0_CH0_DEV_S = {
    .cfg = {.ch_base = (DMACH_TypeDef *)(DMA_350_BASE_S + 0x1000UL),
            .channel = 0},
    .data = {0}};

struct dma350_ch_dev_t DMA350_DMA0_CH1_DEV_S = {
    .cfg = {.ch_base = (DMACH_TypeDef *)(DMA_350_BASE_S + 0x1100UL),
            .channel = 1},
    .data = {0}};
#endif /* PLATFORM_SVC_HANDLERS */
#endif /* CORSTONE310_FVP */

/* TGU driver structures */
static const struct tgu_armv8_m_mem_range_t TGU_ITCM_RANGE_S = {
    .base         = ITCM_BASE_S,
    .limit        = ITCM_BASE_S + ITCM_SIZE - 1,
    .range_offset = 0x0,
    .attr         = TGU_SEC_ATTR_SECURE
};

static const struct tgu_armv8_m_mem_range_t TGU_ITCM_RANGE_NS = {
    .base         = ITCM_BASE_NS,
    .limit        = ITCM_BASE_NS + ITCM_SIZE - 1,
    .range_offset = 0x0,
    .attr         = TGU_SEC_ATTR_NONSECURE
};

#define TGU_ITCM_RANGE_LIST_LEN  2u
static const struct tgu_armv8_m_mem_range_t*
    TGU_ITCM_RANGE_LIST[TGU_ITCM_RANGE_LIST_LEN] = {
        &TGU_ITCM_RANGE_S,
        &TGU_ITCM_RANGE_NS
    };

static struct tgu_armv8_m_dev_cfg_t TGU_ARMV8_M_ITCM_DEV_CFG_S = {
    .base = ITGU_CTRL_BASE
};
static struct tgu_armv8_m_dev_data_t TGU_ARMV8_M_ITCM_DEV_DATA_S = {
    .range_list = TGU_ITCM_RANGE_LIST,
    .nbr_of_ranges = TGU_ITCM_RANGE_LIST_LEN,
    .is_initialized = true
};
struct tgu_armv8_m_dev_t TGU_ARMV8_M_ITCM_DEV_S = {
    &(TGU_ARMV8_M_ITCM_DEV_CFG_S),
    &(TGU_ARMV8_M_ITCM_DEV_DATA_S),
};

static const struct tgu_armv8_m_mem_range_t TGU_DTCM_RANGE_S = {
    .base         = DTCM0_BASE_S,
    .limit        = DTCM3_BASE_S + DTCM_BLK_SIZE -1,
    .range_offset = 0x0,
    .attr         = TGU_SEC_ATTR_SECURE
};

static const struct tgu_armv8_m_mem_range_t TGU_DTCM_RANGE_NS = {
    .base         = DTCM0_BASE_NS,
    .limit        = DTCM3_BASE_NS + DTCM_BLK_SIZE -1,
    .range_offset = 0x0,
    .attr         = TGU_SEC_ATTR_NONSECURE
};

#define TGU_DTCM_RANGE_LIST_LEN  2u
static const struct tgu_armv8_m_mem_range_t*
    TGU_DTCM_RANGE_LIST[TGU_DTCM_RANGE_LIST_LEN] = {
        &TGU_DTCM_RANGE_S,
        &TGU_DTCM_RANGE_NS
    };

static struct tgu_armv8_m_dev_cfg_t TGU_ARMV8_M_DTCM_DEV_CFG_S = {
    .base = DTGU_CTRL_BASE
};
static struct tgu_armv8_m_dev_data_t TGU_ARMV8_M_DTCM_DEV_DATA_S = {
    .range_list = TGU_DTCM_RANGE_LIST,
    .nbr_of_ranges = TGU_DTCM_RANGE_LIST_LEN,
    .is_initialized = true,
};
struct tgu_armv8_m_dev_t TGU_ARMV8_M_DTCM_DEV_S = {
    &(TGU_ARMV8_M_DTCM_DEV_CFG_S),
    &(TGU_ARMV8_M_DTCM_DEV_DATA_S),
};

/* Ranges controlled by this DDR4_MPC */
static const struct mpc_sie_memory_range_t MPC_DDR4_BLK0_RANGE_NS = {
    .base         = MPC_DDR4_BLK0_RANGE_BASE_NS,
    .limit        = MPC_DDR4_BLK0_RANGE_LIMIT_NS,
    .range_offset = MPC_DDR4_BLK0_RANGE_OFFSET_NS,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

static const struct mpc_sie_memory_range_t MPC_DDR4_BLK1_RANGE_S = {
    .base         = MPC_DDR4_BLK1_RANGE_BASE_S,
    .limit        = MPC_DDR4_BLK1_RANGE_LIMIT_S,
    .range_offset = MPC_DDR4_BLK1_RANGE_OFFSET_S,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_DDR4_BLK2_RANGE_NS = {
    .base         = MPC_DDR4_BLK2_RANGE_BASE_NS,
    .limit        = MPC_DDR4_BLK2_RANGE_LIMIT_NS,
    .range_offset = MPC_DDR4_BLK2_RANGE_OFFSET_NS,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

static const struct mpc_sie_memory_range_t MPC_DDR4_BLK3_RANGE_S = {
    .base         = MPC_DDR4_BLK3_RANGE_BASE_S,
    .limit        = MPC_DDR4_BLK3_RANGE_LIMIT_S,
    .range_offset = MPC_DDR4_BLK3_RANGE_OFFSET_S,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_DDR4_BLK4_RANGE_NS = {
    .base         = MPC_DDR4_BLK4_RANGE_BASE_NS,
    .limit        = MPC_DDR4_BLK4_RANGE_LIMIT_NS,
    .range_offset = MPC_DDR4_BLK4_RANGE_OFFSET_NS,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

static const struct mpc_sie_memory_range_t MPC_DDR4_BLK5_RANGE_S = {
    .base         = MPC_DDR4_BLK5_RANGE_BASE_S,
    .limit        = MPC_DDR4_BLK5_RANGE_LIMIT_S,
    .range_offset = MPC_DDR4_BLK5_RANGE_OFFSET_S,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

static const struct mpc_sie_memory_range_t MPC_DDR4_BLK6_RANGE_NS = {
    .base         = MPC_DDR4_BLK6_RANGE_BASE_NS,
    .limit        = MPC_DDR4_BLK6_RANGE_LIMIT_NS,
    .range_offset = MPC_DDR4_BLK6_RANGE_OFFSET_NS,
    .attr         = MPC_SIE_SEC_ATTR_NONSECURE
};

static const struct mpc_sie_memory_range_t MPC_DDR4_BLK7_RANGE_S = {
    .base         = MPC_DDR4_BLK7_RANGE_BASE_S,
    .limit        = MPC_DDR4_BLK7_RANGE_LIMIT_S,
    .range_offset = MPC_DDR4_BLK7_RANGE_OFFSET_S,
    .attr         = MPC_SIE_SEC_ATTR_SECURE
};

#define MPC_DDR4_RANGE_LIST_LEN  8u
static const struct mpc_sie_memory_range_t*
    MPC_DDR4_RANGE_LIST[MPC_DDR4_RANGE_LIST_LEN] = {
        &MPC_DDR4_BLK0_RANGE_NS,
        &MPC_DDR4_BLK1_RANGE_S,
        &MPC_DDR4_BLK2_RANGE_NS,
        &MPC_DDR4_BLK3_RANGE_S,
        &MPC_DDR4_BLK4_RANGE_NS,
        &MPC_DDR4_BLK5_RANGE_S,
        &MPC_DDR4_BLK6_RANGE_NS,
        &MPC_DDR4_BLK7_RANGE_S,
    };
static struct mpc_sie_dev_cfg_t MPC_DDR4_DEV_CFG_S = {
    .base = MPC_DDR4_BASE_S,
    .range_list = MPC_DDR4_RANGE_LIST,
    .nbr_of_ranges = MPC_DDR4_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_DDR4_DEV_DATA_S = {
    .is_initialized = false};
struct mpc_sie_dev_t MPC_DDR4_DEV_S = {
    &(MPC_DDR4_DEV_CFG_S),
    &(MPC_DDR4_DEV_DATA_S)};
