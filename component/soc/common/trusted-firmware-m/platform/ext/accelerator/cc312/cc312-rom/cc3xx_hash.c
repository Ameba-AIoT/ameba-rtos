/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_hash.h"

#include "cc3xx_dev.h"
#include "cc3xx_error.h"
#include "cc3xx_dma.h"
#include "cc3xx_engine_state.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define SHA256_BLOCK_SIZE  32

static uint8_t cc3xx_hash_partial_block_buf[SHA256_BLOCK_SIZE];
static size_t  cc3xx_hash_partial_block_size = 0;

/* Conveniently, the compiler is smart enough to make this a single-instruction
 * endianness reversal where the architecture has such an instruction
 */
static inline uint32_t bswap_32(uint32_t in) {
    uint32_t out = 0;

    out |= (in & 0xFF000000) >> 24;
    out |= (in & 0x00FF0000) >> 8;
    out |= (in & 0x0000FF00) << 8;
    out |= (in & 0x000000FF) << 24;

    return out;
}

cc3xx_err_t cc3xx_hash_sha256_init(void)
{

    if (cc3xx_engine_in_use) {
        return CC3XX_ERR_ENGINE_IN_USE;
    }
    cc3xx_engine_in_use = true;

    /* Enable the hash engine clock */
    P_CC3XX->misc.hash_clk_enable = 0x1U;

    /* Enable the DMA clock */
    P_CC3XX->misc.dma_clk_enable = 0x1U;

    /* Wait for the crypto engine to be ready */
    while (P_CC3XX->cc_ctl.crypto_busy) {}

    /* Set the crypto engine to the HASH engine */
    P_CC3XX->cc_ctl.crypto_ctl = 0b00111U;

    /* Select HASH mode, not MAC */
    P_CC3XX->hash.hash_sel_aes_mac  = 0x0U;

    /* Configure the hash engine to SHA256 */
    P_CC3XX->hash.hash_control = 0b0010U;

    /* Enable padding */
    P_CC3XX->hash.hash_pad_en = 0x1U;

    /* Disable auto-padding to allow multipart operations */
    P_CC3XX->hash.auto_hw_padding = 0x0U;

    /* Set already processed length to 0 */
    P_CC3XX->hash.hash_cur_len[0] = 0x0U;
    P_CC3XX->hash.hash_cur_len[1] = 0x0U;

    /* Set the registers to the magic initial values of sha256. CryptoCell
     * hardware requires the writes to happen in reverse order
     * (from H7 to H0).
     */
    P_CC3XX->hash.hash_h[7] = 0x5be0cd19U;
    P_CC3XX->hash.hash_h[6] = 0x1f83d9abU;
    P_CC3XX->hash.hash_h[5] = 0x9b05688cU;
    P_CC3XX->hash.hash_h[4] = 0x510e527fU;
    P_CC3XX->hash.hash_h[3] = 0xa54ff53aU;
    P_CC3XX->hash.hash_h[2] = 0x3c6ef372U;
    P_CC3XX->hash.hash_h[1] = 0xbb67ae85U;
    P_CC3XX->hash.hash_h[0] = 0x6a09e667U;

    return CC3XX_ERR_SUCCESS;
}

static void hash_uninit(void)
{
    /* Reset padding registers as required by the hardware */
    P_CC3XX->hash.hash_pad_cfg = 0x0U;
    P_CC3XX->hash.auto_hw_padding = 0x0U;

    /* Set the crypto engine back to the default PASSTHROUGH engine */
    P_CC3XX->cc_ctl.crypto_ctl = 0x0U;

    /* Disable the hash engine clock */
    P_CC3XX->misc.hash_clk_enable = 0x0U;

    /* Disable the DMA clock */
    P_CC3XX->misc.dma_clk_enable = 0x0U;

    cc3xx_engine_in_use = false;
}

cc3xx_err_t cc3xx_hash_sha256_update(const uint8_t *buf,
                                     size_t length)
{
    size_t current_processed_length = 0;
    size_t partial_block_size_free = SHA256_BLOCK_SIZE - cc3xx_hash_partial_block_size;
    size_t partial_block_size_increase = 0;
    size_t dma_input_size;
    cc3xx_err_t err = CC3XX_ERR_SUCCESS;

    /* If we have previously unprocessed data (because it didn't meet the block
     * size), then first fill and dispatch that.
     */
    if (cc3xx_hash_partial_block_size != 0) {
        if (length > partial_block_size_free) {
            partial_block_size_increase = partial_block_size_free;
        } else {
            partial_block_size_increase = length;
        }

        memcpy(cc3xx_hash_partial_block_buf + cc3xx_hash_partial_block_size,
               buf, partial_block_size_increase);
        current_processed_length += partial_block_size_increase;

        cc3xx_hash_partial_block_size += partial_block_size_increase;
        partial_block_size_free = SHA256_BLOCK_SIZE - cc3xx_hash_partial_block_size;
    }

    /* If the unprocessed data buffer is full then write it out */
    if (cc3xx_hash_partial_block_size == SHA256_BLOCK_SIZE) {
        err = cc3xx_dma_input_data(cc3xx_hash_partial_block_buf,
                                   cc3xx_hash_partial_block_size);
        if (err != CC3XX_ERR_SUCCESS) {
            hash_uninit();
            return err;
        }
        cc3xx_hash_partial_block_size = 0;
    }

    /* DMA over all the data that can be done in entire blocks. At least one
     * byte must remain to go into the partial block.
     */
    if (current_processed_length + SHA256_BLOCK_SIZE < length) {
        dma_input_size = ((length - current_processed_length - 1)
                          / SHA256_BLOCK_SIZE) * SHA256_BLOCK_SIZE;
        err = cc3xx_dma_input_data(buf + current_processed_length,
                                   dma_input_size);
        if (err != CC3XX_ERR_SUCCESS) {
            hash_uninit();
            return err;
        }
        current_processed_length += dma_input_size;
    }

    /* Push any remaining data into the unprocessed data block. At this point
     * it's always true that either the current_processed_length is the length
     * (we put all the data into the unprocessed buffer) or that the unprocessed
     * buffer got filled up so that the buffer is empty, so this cannot
     * overflow.
     */
    if (current_processed_length < length) {
        memcpy(cc3xx_hash_partial_block_buf, buf + current_processed_length,
               length - current_processed_length);
        cc3xx_hash_partial_block_size = (length - current_processed_length);
    }

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_hash_sha256_finish(uint8_t *res,
                                     size_t length)
{
    cc3xx_err_t err = CC3XX_ERR_SUCCESS;
    uint32_t tmp_buf[SHA256_OUTPUT_SIZE / sizeof(uint32_t)];
    uint32_t idx;

    if (length < SHA256_OUTPUT_SIZE) {
        hash_uninit();
        return CC3XX_ERR_BUFFER_OVERFLOW;
    }

    /* Process the final block */
    if (cc3xx_hash_partial_block_size > 0) {
        P_CC3XX->hash.auto_hw_padding = 0x1U;
        err = cc3xx_dma_input_data(cc3xx_hash_partial_block_buf,
                                   cc3xx_hash_partial_block_size);
        if (err) {
            hash_uninit();
            return err;
        }
        cc3xx_hash_partial_block_size = 0;
    }

    /* Special-case for hardware padding when the length is 0 */
    if (P_CC3XX->hash.hash_cur_len[0] == 0
        && P_CC3XX->hash.hash_cur_len[1] == 0) {
        P_CC3XX->hash.hash_pad_cfg = 0x4U;
    }

    for (idx = 0; idx < 8; idx++) {
        tmp_buf[idx] = bswap_32(P_CC3XX->hash.hash_h[idx]);
    }

    memcpy(res, tmp_buf, sizeof(tmp_buf));

    hash_uninit();

    return CC3XX_ERR_SUCCESS;
}
