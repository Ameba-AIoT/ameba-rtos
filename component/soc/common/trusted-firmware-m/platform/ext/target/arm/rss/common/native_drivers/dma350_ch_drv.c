/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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

#include "dma350_ch_drv.h"

#include <stddef.h>
#include <stdint.h>

enum dma350_ch_error_t dma350_ch_init(struct dma350_ch_dev_t *dev)
{
    dev->data.state = DMA350_CH_INITIALIZED;

    return DMA350_CH_ERR_NONE;
}

uint32_t *dma350_cmdlink_generate(struct dma350_cmdlink_gencfg_t *cmdlink_cfg,
                                  uint32_t *buffer, uint32_t *buffer_end)
{
    uint32_t *cfg;
    uint32_t header_sel;
    /* Check if cmdlink fits inside the buffer. */
    /* Header bit 0 and 1 have no associated registers. +1 is For the header */
    if (&buffer[__builtin_popcount((cmdlink_cfg->header) & 0xFFFFFFFCUL) + 1] >=
        buffer_end) {
        return NULL;
    }
    cfg = (uint32_t *)&cmdlink_cfg->cfg;
    *(buffer++) = cmdlink_cfg->header;

    /* Note: REGCLEAR (Bit 0) has no associated field and Bit 1 is reserved,
     *       cfg starts from Bit 2 */
    for (header_sel = (0x1UL << 2); header_sel; header_sel <<= 1) {
        if (cmdlink_cfg->header & header_sel) {
            *(buffer++) = *(cfg++);
        } else {
            cfg++;
        }
    }
    return buffer;
}

void dma350_cmdlink_init(struct dma350_cmdlink_gencfg_t *cmdlink_cfg)
{
    static const struct dma350_cmdlink_gencfg_t default_cmdlink = {
        .header = 0,
        .cfg = {.intren = DMA350_CH_INTREN_RESET_VALUE,
                .ctrl = DMA350_CH_CTRL_RESET_VALUE,
                .srcaddr = 0,
                .srcaddrhi = 0,
                .desaddr = 0,
                .desaddrhi = 0,
                .xsize = 0,
                .xsizehi = 0,
                .srctranscfg = DMA350_CH_SRCTRANSCFG_RESET_VALUE,
                .destranscfg = DMA350_CH_DESTRANSCFG_RESET_VALUE,
                .xaddrinc = 0,
                .yaddrstride = 0,
                .fillval = 0,
                .ysize = 0,
                .tmpltcfg = 0,
                .srctmplt = 0,
                .destmplt = 0,
                .srctrigincfg = 0,
                .destrigincfg = 0,
                .trigoutcfg = 0,
                .gpoen0 = 0,
                .reserved0 = 0,
                .gpoval0 = 0,
                .reserved1 = 0,
                .streamintcfg = 0,
                .reserved2 = 0,
                .linkattr = 0,
                .autocfg = DMA350_CH_AUTOCFG_RESET_VALUE,
                .linkaddr = DMA350_CH_LINKADDR_RESET_VALUE,
                .linkaddrhi = 0}};
    *cmdlink_cfg = default_cmdlink;
}

union dma350_ch_status_t dma350_ch_wait_status(struct dma350_ch_dev_t *dev)
{
    /* Reference implementation with busy wait */
    while(dma350_ch_is_busy(dev));

    return dma350_ch_get_status(dev);
}
