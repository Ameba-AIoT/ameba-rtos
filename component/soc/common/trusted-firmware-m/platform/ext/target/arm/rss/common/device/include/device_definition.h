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
 * \file device_definition.h
 * \brief The structure definitions in this file are exported based on the
 * peripheral definitions from device_cfg.h.
 * This file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#ifndef __DEVICE_DEFINITION_H__
#define __DEVICE_DEFINITION_H__

#include "device_cfg.h"
#include "host_device_definition.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======= Defines peripheral configuration structures ======= */
/* ATU driver structures */
#ifdef ATU_S
#include "atu_rss_drv.h"
extern struct atu_dev_t ATU_DEV_S;
#endif

/* SIC driver structures */
#ifdef SIC_S
#include "sic_drv.h"
extern struct sic_dev_t SIC_DEV_S;
#endif

/* ARM PPC driver structures */
#ifdef PPC_RSS_MAIN0_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_MAIN0_DEV_S;
#endif

#ifdef PPC_RSS_MAIN_EXP0_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_MAIN_EXP0_DEV_S;
#endif

#ifdef PPC_RSS_MAIN_EXP1_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_MAIN_EXP1_DEV_S;
#endif

#ifdef PPC_RSS_MAIN_EXP2_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_MAIN_EXP2_DEV_S;
#endif

#ifdef PPC_RSS_MAIN_EXP3_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_MAIN_EXP3_DEV_S;
#endif

#ifdef PPC_RSS_PERIPH0_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_PERIPH0_DEV_S;
#endif

#ifdef PPC_RSS_PERIPH1_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_PERIPH1_DEV_S;
#endif

#ifdef PPC_RSS_PERIPH_EXP0_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_PERIPH_EXP0_DEV_S;
#endif

#ifdef PPC_RSS_PERIPH_EXP1_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_PERIPH_EXP1_DEV_S;
#endif

#ifdef PPC_RSS_PERIPH_EXP2_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_PERIPH_EXP2_DEV_S;
#endif

#ifdef PPC_RSS_PERIPH_EXP3_S
#include "ppc_rss_drv.h"
extern struct ppc_rss_dev_t PPC_RSS_PERIPH_EXP3_DEV_S;
#endif

/* System counters */
#ifdef SYSCOUNTER_CNTRL_ARMV8_M_S
#include "syscounter_armv8-m_cntrl_drv.h"
extern struct syscounter_armv8_m_cntrl_dev_t SYSCOUNTER_CNTRL_ARMV8_M_DEV_S;
#endif

#ifdef SYSCOUNTER_READ_ARMV8_M_S
#include "syscounter_armv8-m_read_drv.h"
extern struct syscounter_armv8_m_read_dev_t SYSCOUNTER_READ_ARMV8_M_DEV_S;
#endif

/* System timers */
#ifdef SYSTIMER0_ARMV8_M_S
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_S;
#endif
#ifdef SYSTIMER0_ARMV8_M_NS
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_NS;
#endif

#ifdef SYSTIMER1_ARMV8_M_S
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_S;
#endif
#ifdef SYSTIMER1_ARMV8_M_NS
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_NS;
#endif

#ifdef SYSTIMER2_ARMV8_M_S
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER2_ARMV8_M_DEV_S;
#endif
#ifdef SYSTIMER2_ARMV8_M_NS
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER2_ARMV8_M_DEV_NS;
#endif

#ifdef SYSTIMER3_ARMV8_M_S
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER3_ARMV8_M_DEV_S;
#endif
#ifdef SYSTIMER3_ARMV8_M_NS
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER3_ARMV8_M_DEV_NS;
#endif

/* System Watchdogs */
#ifdef SYSWDOG_ARMV8_M_S
#include "syswdog_armv8-m_drv.h"
extern struct syswdog_armv8_m_dev_t SYSWDOG_ARMV8_M_DEV_S;
#endif
#ifdef SYSWDOG_ARMV8_M_NS
#include "syswdog_armv8-m_drv.h"
extern struct syswdog_armv8_m_dev_t SYSWDOG_ARMV8_M_DEV_NS;
#endif

/* ARM MPC SIE 300 driver structures */
#ifdef MPC_VM0_S
#include "mpc_sie_drv.h"
extern struct mpc_sie_dev_t MPC_VM0_DEV_S;
#endif

#ifdef MPC_VM1_S
#include "mpc_sie_drv.h"
extern struct mpc_sie_dev_t MPC_VM1_DEV_S;
#endif

#ifdef MPC_SIC_S
#include "mpc_sie_drv.h"
extern struct mpc_sie_dev_t MPC_SIC_DEV_S;
#endif

/* Message Handling Units (MHU) */
#ifdef MHU_AP_TO_RSS
#include "mhu_v2_x.h"
extern struct mhu_v2_x_dev_t MHU_AP_TO_RSS_DEV;
#endif

#ifdef MHU_RSS_TO_AP
#include "mhu_v2_x.h"
extern struct mhu_v2_x_dev_t MHU_RSS_TO_AP_DEV;
#endif

#ifdef MHU_SCP_TO_RSS
#include "mhu_v2_x.h"
extern struct mhu_v2_x_dev_t MHU_SCP_TO_RSS_DEV;
#endif

#ifdef MHU_RSS_TO_SCP
#include "mhu_v2_x.h"
extern struct mhu_v2_x_dev_t MHU_RSS_TO_SCP_DEV;
#endif

#ifdef KMU_S
#include "kmu_drv.h"
extern struct kmu_dev_t KMU_DEV_S;
#endif

#ifdef LCM_S
#include "lcm_drv.h"
extern struct lcm_dev_t LCM_DEV_S;
#endif

#ifdef DMA350_DMA0_S
#include "dma350_drv.h"
#include "dma350_ch_drv.h"
extern struct dma350_dev_t DMA350_DMA0_DEV_S;
#endif

#ifdef DMA350_DMA0_CH0_S
extern struct dma350_ch_dev_t DMA350_DMA0_CH0_DEV_S;
#endif

#ifdef DMA350_DMA0_CH1_S
extern struct dma350_ch_dev_t DMA350_DMA0_CH1_DEV_S;
#endif

#ifdef DMA350_DMA0_CH2_S
extern struct dma350_ch_dev_t DMA350_DMA0_CH2_DEV_S;
#endif

#ifdef DMA350_DMA0_CH3_S
extern struct dma350_ch_dev_t DMA350_DMA0_CH3_DEV_S;
#endif

#ifdef __cplusplus
}
#endif

#endif  /* __DEVICE_DEFINITION_H__ */
