/*
 * Copyright (c) 2022 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file ppc_corstone310_reg_map.h
 * \brief Register map for PPC.
 *
 */

#ifndef __PPC_CORSTONE310_REG_MAP_H__
#define __PPC_CORSTONE310_REG_MAP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Secure Access Configuration Register Block */
struct corstone310_sacfg_block_t {
    volatile uint32_t reserved0[8];
    volatile uint32_t secppcintstat;  /* 0x020 (R/ ) Secure PPC IRQ Status */
    volatile uint32_t secppcintclr;   /* 0x024 (R/W) Secure PPC IRQ Clear */
    volatile uint32_t secppcinten;    /* 0x028 (R/W) Secure PPC IRQ Enable */
    volatile uint32_t reserved1[9];
    volatile uint32_t mainnsppc0;     /* 0x050 (R/W) Non-secure Access
                                       *             Peripheral Protection
                                       *             Control 0 on the Main
                                       *             Interconnect */
    volatile uint32_t reserved2[3];
    volatile uint32_t mainnsppcexp0;  /* 0x060 (R/W) Expansion 0 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on the
                                       *             Main Interconnect */
    volatile uint32_t mainnsppcexp1;  /* 0x064 (R/W) Expansion 1 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on the
                                       *             Main Interconnect */
    volatile uint32_t mainnsppcexp2;  /* 0x068 (R/W) Expansion 2 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on the
                                       *             Main Interconnect */
    volatile uint32_t mainnsppcexp3;  /* 0x06C (R/W) Expansion 3 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on the
                                       *             Main Interconnect */
    volatile uint32_t periphnsppc0;   /* 0x070 (R/W) Non-secure Access
                                       *             Peripheral Protection
                                       *             Control 0 on the Peripheral
                                       *             Interconnect */
    volatile uint32_t periphnsppc1;   /* 0x074 (R/W) Non-secure Access
                                       *             Peripheral Protection
                                       *             Control 1 on the Peripheral
                                       *             Interconnect */
    volatile uint32_t reserved3[2];
    volatile uint32_t periphnsppcexp0;/* 0x080 (R/W) Expansion 0 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on
                                       *             Peripheral Bus */
    volatile uint32_t periphnsppcexp1;/* 0x084 (R/W) Expansion 1 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on
                                       *             Peripheral Bus */
    volatile uint32_t periphnsppcexp2;/* 0x088 (R/W) Expansion 2 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on
                                       *             Peripheral Bus */
    volatile uint32_t periphnsppcexp3;/* 0x08C (R/W) Expansion 3 Non-secure
                                       *             Access Peripheral
                                       *             Protection Control on
                                       *             Peripheral Bus */
    volatile uint32_t mainspppc0;     /* 0x090 (R/W) Secure Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control 0 on Main
                                       *             Interconnect */
    volatile uint32_t reserved4[3];
    volatile uint32_t mainspppcexp0;  /* 0x0A0 (R/W) Expansion 0 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Main
                                       *             Interconnect */
    volatile uint32_t mainspppcexp1;  /* 0x0A4 (R/W) Expansion 1 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Main
                                       *             Interconnect */
    volatile uint32_t mainspppcexp2;  /* 0x0A8 (R/W) Expansion 2 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Main
                                       *             Interconnect */
    volatile uint32_t mainspppcexp3;  /* 0x0AC (R/W) Expansion 3 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Main
                                       *             Interconnect */
    volatile uint32_t periphspppc0;   /* 0x0B0 (R/W) Secure Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control 0 on
                                       *             Peripheral Interconnect */
    volatile uint32_t periphspppc1;   /* 0x0B4 (R/W) Secure Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control 1 on
                                       *             Peripheral Interconnect */
    volatile uint32_t reserved5[2];
    volatile uint32_t periphspppcexp0;/* 0x0C0 (R/W) Expansion 0 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Peripheral
                                       *             Interconnect */
    volatile uint32_t periphspppcexp1;/* 0x0C4 (R/W) Expansion 1 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Peripheral
                                       *             Interconnect */
    volatile uint32_t periphspppcexp2;/* 0x0C8 (R/W) Expansion 2 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Peripheral
                                       *             Interconnect */
    volatile uint32_t periphspppcexp3;/* 0x0CC (R/W) Expansion 3 Secure
                                       *             Unprivileged Access
                                       *             Peripheral Protection
                                       *             Control on Peripheral
                                       *             Interconnect */
    volatile uint32_t reserved6[960];
    volatile uint32_t pidr4;          /* 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved7[3];
    volatile uint32_t pidr0;          /* 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;          /* 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;          /* 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;          /* 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;          /* 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;          /* 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;          /* 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;          /* 0xFFC (R/ ) Component ID 3 */
};

#ifdef __cplusplus
}
#endif

#endif /* __PPC_CORSTONE310_REG_MAP_H__ */
