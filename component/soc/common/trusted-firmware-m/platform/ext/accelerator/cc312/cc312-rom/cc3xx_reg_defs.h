/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_REG_DEFS_H
#define CC3XX_REG_DEFS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __IM
#define __IM volatile const /*! Defines 'read only' structure member permissions */
#endif /* __IM */

#ifndef __OM
#define __OM volatile /*! Defines 'write only' structure member permissions */
#endif /* __OM */

#ifndef __IOM
#define __IOM volatile /*! Defines 'read / write' structure member permissions */
#endif /* __IOM */

#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT struct __attribute__((packed, aligned(1)))
#endif /* __PACKED_STRUCT */

#ifndef __PACKED_UNION
#define __PACKED_UNION union __attribute__((packed, aligned(1)))
#endif /* __PACKED_UNION */

#ifndef __ALIGNED
#define __ALIGNED(x) __attribute__((aligned(x)))
#endif /* __ALIGNED */

__PACKED_STRUCT __ALIGNED(4) _cc3xx_reg_map_t {
    /* PKA block */
    __PACKED_STRUCT {
        __IOM uint32_t memory_map[32];
                    /*!< Offset: 0x0 (RW) */
        __IOM uint32_t opcode;
                    /*!< Offset: 0x80 (RW) */
        __IOM uint32_t n_np_t0_t1_addr;
                    /*!< Offset: 0x84 (RW) */
        __IM  uint32_t pka_status;
                    /*!< Offset: 0x88 (RO) */
        __OM  uint32_t pka_sw_reset;
                    /*!< Offset: 0x8C (WO) */
        __IOM uint32_t pka_l[8];
                    /*!< Offset: 0x90 (RW) */
        __IM  uint32_t pka_pipe_rdy;
                    /*!< Offset: 0xB0 (RO) */
        __IM  uint32_t pka_done;
                    /*!< Offset: 0xB4 (RO) */
        __IOM uint32_t pka_mon_select;
                    /*!< Offset: 0xB8 (RW) */
        __IOM uint32_t pka_mongo_sel;
                    /*!< Offset: 0xBC (RW) */
        __IOM uint32_t pka_ram_enc;
                    /*!< Offset: 0xC0 (RW) */
        __IM  uint32_t pka_version;
                    /*!< Offset: 0xC4 (RO) */
        uint8_t _reserved_1[8];
                    /*!< Offset: 0xC8-0xCC Reserved */
        __IM  uint32_t pka_mon_read;
                    /*!< Offset: 0xD0 (RO) */
        __OM  uint32_t pka_sram_addr;
                    /*!< Offset: 0xD4 (WO) */
        __OM  uint32_t pka_sram_wdata;
                    /*!< Offset: 0xD8 (WO) */
        __IOM uint32_t pka_sram_rdata;
                    /*!< Offset: 0xDC (RW) */
        __OM  uint32_t pka_sram_wr_clr;
                    /*!< Offset: 0xE0 (WO) */
        __OM  uint32_t pka_sram_raddr;
                    /*!< Offset: 0xE4 (WO) */
        uint8_t _reserved_2[8];
                    /*!< Offset: 0xE8-0xEC Reserved */
        __OM  uint32_t pka_word_access;
                    /*!< Offset: 0xF0 (WO) */
        uint8_t _reserved_3[4];
                    /*!< Offset: 0xF4 Reserved */
        __OM  uint32_t pka_buff_addr;
                    /*!< Offset: 0xF8 (WO) */
        uint8_t _reserved_4[4];
                    /*!< Offset: 0xFC Reserved */
    } pka;

    /* RNG block */
    __PACKED_STRUCT {
        __IOM uint32_t rng_imr;
                    /*!< Offset: 0x100 (RW) */
        __IM  uint32_t rng_isr;
                    /*!< Offset: 0x104 (RO) */
        __OM  uint32_t rng_icr;
                    /*!< Offset: 0x108 (WO) */
        __IOM uint32_t trng_config;
                    /*!< Offset: 0x10C (RW) */
        __IM  uint32_t trng_valid;
                    /*!< Offset: 0x110 (RO) */
        __IM  uint32_t ehr_data[6];
                    /*!< Offset: 0x114 (RO) */
        __IOM uint32_t rnd_source_enable;
                    /*!< Offset: 0x12C (RW) */
        __IOM uint32_t sample_cnt1;
                    /*!< Offset: 0x130 (RW) */
        __IOM uint32_t autocorr_statistic;
                    /*!< Offset: 0x134 (RW) */
        __IOM uint32_t trng_debug_control;
                    /*!< Offset: 0x138 (RW) */
        uint8_t _reserved_0[4];
                    /*!< Offset: 0x13C Reserved */
        __IOM uint32_t rng_sw_reset;
                    /*!< Offset: 0x140 (RW) */
        uint8_t _reserved_1[112];
                    /*!< Offset: 0x144-0x1B0 Reserved */
        __IM  uint32_t rng_debug_en_input;
                    /*!< Offset: 0x1B4 (RO) */
        __IM  uint32_t rng_busy;
                    /*!< Offset: 0x1B8 (RO) */
        __OM  uint32_t rst_bits_counter;
                    /*!< Offset: 0x1BC (WO) */
        __IM  uint32_t rng_version;
                    /*!< Offset: 0x1C0 (RO) */
        __OM  uint32_t rng_clk_enable;
                    /*!< Offset: 0x1C4 (WO) */
        __IOM uint32_t rng_dma_enable;
                    /*!< Offset: 0x1C8 (RW) */
        __IOM uint32_t rng_dma_src_mask;
                    /*!< Offset: 0x1CC (RW) */
        __IOM uint32_t rng_dma_sram_addr;
                    /*!< Offset: 0x1D0 (RW) */
        __IOM uint32_t rng_dma_samples_num;
                    /*!< Offset: 0x1D4 (RW) */
        __IOM uint32_t rng_watchdog_val;
                    /*!< Offset: 0x1D8 (RW) */
        __IM  uint32_t rng_dma_status;
                /*!< Offset: 0x1DC (RO) */
        uint8_t _reserved_2[416];
                    /*!< Offset: 0x1E0-0x380 Reserved */
    } rng;

    /* ChaCha block */
    __PACKED_STRUCT {
        __IOM uint32_t chacha_control_reg;
                    /*!< offset: 0x380 (RW) */
        __IM  uint32_t chacha_version;
                    /*!< offset: 0x384 (RO) */
        __OM  uint32_t chacha_key[8];
                    /*!< offset: 0x388 (WO) */
        __IOM uint32_t chacha_iv[2];
                    /*!< offset: 0x3a8 (RW) */
        __IM  uint32_t chacha_busy;
                    /*!< offset: 0x3b0 (RO) */
        __IM  uint32_t chacha_hw_flags;
                    /*!< offset: 0x3b4 (RO) */
        __IOM uint32_t chacha_block_cnt_lsb;
                    /*!< offset: 0x3b8 (RW) */
        __IOM uint32_t chacha_block_cnt_msb;
                    /*!< offset: 0x3bc (RW) */
        __OM  uint32_t chacha_sw_reset;
                    /*!< offset: 0x3c0 (WO) */
        __IM  uint32_t chacha_for_poly_key[8];
                    /*!< offset: 0x3c4 (RO) */
        __IOM uint32_t chacha_byte_word_order_cntl_reg;
                    /*!< Offset: 0x3E4 (RW) */
        __IM  uint32_t chacha_debug_reg;
                    /*!< offset: 0x3E8 (RO) */
        uint8_t _reserved_0[20];
                    /*!< Offset: 0x3EC-0x3FC Reserved */
    } chacha;

    /* AES block */
    __PACKED_STRUCT {
        __OM uint32_t aes_key_0[8];
                    /*!< Offset: 0x400 (WO) */
        __OM uint32_t aes_key_1[8];
                    /*!< Offset: 0x420 (WO) */
        __IOM uint32_t aes_iv_0[4];
                    /*!< Offset: 0x440 (RW) */
        __IOM uint32_t aes_iv_1[4];
                    /*!< Offset: 0x450 (RW) */
        __IOM uint32_t aes_ctr_0[4];
                    /*!< Offset: 0x460 (RW) */
        __IM uint32_t aes_busy;
                    /*!< Offset: 0x470 (RO) */
        uint8_t _reserved_0[4];
                    /*!< Offset: 0x474 Reserved */
        __OM uint32_t aes_sk;
                    /*!< Offset: 0x478 (WO) */
        __OM uint32_t aes_cmac_init;
                    /*!< Offset: 0x47C (WO) */
        uint8_t _reserved_1[52];
                    /*!< Offset: 0x480-0x4B0 Reserved */
        __OM uint32_t aes_sk1;
                    /*!< Offset: 0x4B4 (WO) */
        uint8_t _reserved_2[4];
                    /*!< Offset: 0x4B8 Reserved */
        __IOM uint32_t aes_remaining_bytes;
                    /*!< Offset: 0x4BC (RW) */
        __IOM uint32_t aes_control;
                    /*!< Offset: 0x4C0 (RW) */
        uint8_t _reserved_3[4];
                    /*!< Offset: 0x4C4 Reserved */
        __IM uint32_t aes_hw_flags;
                    /*!< Offset: 0x4C8 (RO) */
        uint8_t _reserved_4[12];
                    /*!< Offset: 0x4CC-0x4D4 Reserved */
        __PACKED_UNION {
            __IOM uint32_t aes_rbg_seed;
                        /*!< Offset: 0x4D8 (RW) */
            __IOM uint32_t aes_ctr_no_increment;
                        /*!< Offset: 0x4D8 (RW) */
        };
        uint8_t _reserved_5[20];
                    /*!< Offset: 0x4DC-0x4EC Reserved */
        __IOM uint32_t aes_dfa_is_on;
                    /*!< Offset: 0x4F0 (RW) */
        uint8_t _reserved_6[4];
                    /*!< Offset: 0x4F4 Reserved */
        __IM uint32_t aes_dfa_err_status;
                    /*!< Offset: 0x4F8 (RO) */
        __IM uint32_t aes_rbg_seeding_rdy;
                    /*!< Offset: 0x4FC (RO) */
        uint8_t _reserved_7[36];
                    /*!< Offset: 0x500-0x520 Reserved */
        __OM uint32_t aes_cmac_size0_kick;
                    /*!< Offset: 0x524 (WO) */
        uint8_t _reserved_8[28];
                    /*!< Offset: 0x528-0x540 Reserved */
        __IOM uint32_t aes_dummy_rounds_enable;
                    /*!< Offset: 0x544 (RW) */
        uint8_t _reserved_9[248];
                    /*!< Offset: 0x548-0x63C Reserved */
    } aes;

    /* Hash block */
    __PACKED_STRUCT {
        __IOM uint32_t hash_h[9];
                    /*!< Offset: 0x640 (RW) */
        uint8_t _reserved_0[32];
                    /*!< Offset: 0x664-0x680 Reserved */
        __OM uint32_t auto_hw_padding;
                    /*!< Offset: 0x684 (WO) */
        __IOM uint32_t hash_xor_din;
                    /*!< Offset: 0x688 (RW) */
        uint8_t _reserved_1[8];
                    /*!< Offset: 0x68C-0x690 Reserved */
        __OM uint32_t load_init_state;
                    /*!< Offset: 0x694 (WO) */
        uint8_t _reserved_2[12];
                    /*!< Offset: 0x698-0x6A0 Reserved */
        __OM uint32_t hash_sel_aes_mac;
                    /*!< Offset: 0x6A4 (WO) */
        uint8_t _reserved_3[264];
                    /*!< Offset: 0x6A8-0x6FC Reserved */
        __IM uint32_t hash_version;
                    /*!< Offset: 0x7B0 (RO) */
        uint8_t _reserved_4[12];
                    /*!< Offset: 0x7B4-0x7BC Reserved */
        __IOM uint32_t hash_control;
                    /*!< Offset: 0x7C0 (RW) */
        __IOM uint32_t hash_pad_en;
                    /*!< Offset: 0x7C4 (RW) */
        __IOM uint32_t hash_pad_cfg;
                    /*!< Offset: 0x7C8 (RW) */
        __IOM uint32_t hash_cur_len[2];
                    /*!< Offset: 0x7CC (RW) */
        uint8_t _reserved_5[8];
                    /*!< Offset: 0x7D0-0x7D8 Reserved */
        __IM uint32_t hash_param;
                    /*!< Offset: 0x7DC (RO) */
        uint8_t _reserved_6[4];
                    /*!< Offset: 0x7E0 Reserved */
        __OM uint32_t hash_aes_sw_reset;
                    /*!< Offset: 0x7E4 (WO) */
        __IOM uint32_t hash_endianess;
                    /*!< Offset: 0x7E8 (RW) */
        uint8_t _reserved_7[36];
                    /*!< Offset: 0x7E4-0x80C Reserved */
    } hash;

    /* Misc Block */
    __PACKED_STRUCT {
        __IOM uint32_t aes_clk_enable;
                    /*!< Offset: 0x810 (RW) */
        uint8_t _reserved_0[4];
                    /*!< Offset: 0x814 Reserved */
        __IOM uint32_t hash_clk_enable;
                    /*!< Offset: 0x818 (RW) */
        __IOM uint32_t pka_clk_enable;
                    /*!< Offset: 0x81C (RW) */
        __IOM uint32_t dma_clk_enable;
                    /*!< Offset: 0x820 (RW) */
        __IM uint32_t clk_status;
                    /*!< Offset: 0x824 (RO) */
        uint8_t _reserved_1[48];
                    /*!< Offset: 0x828-0x854 Reserved */
        __OM uint32_t chacha_clk_enable;
                    /*!< Offset: 0x858 (WO) */
        uint8_t _reserved_2[164];
                    /*!< Offset: 0x85C-0x8FC Reserved */
    } misc;

    /* CC_CTL Block */
    __PACKED_STRUCT {
        __OM uint32_t crypto_ctl;
                    /*!< Offset: 0x900 (WO) */
        uint8_t _reserved_0[12];
                    /*!< Offset: 0x904-0x90C Reserved */
        __IM uint32_t crypto_busy;
                    /*!< Offset: 0x910 (RO) */
        uint8_t _reserved_1[8];
                    /*!< Offset: 0x914-0x918 Reserved */
        __IM uint32_t hash_busy;
                    /*!< Offset: 0x91C (RO) */
        uint8_t _reserved_2[16];
                    /*!< Offset: 0x920-0x92C Reserved */
        __IOM uint32_t context_id;
                    /*!< Offset: 0x930 (RW) */
        uint8_t _reserved_3[44];
                    /*!< Offset: 0x85C-0x8FC Reserved */
    } cc_ctl;

    /* GHash block */
    __PACKED_STRUCT {
        __OM uint32_t ghash_subkey_0[4];
                    /*!< Offset: 0x960 (WO) */
        __IOM uint32_t ghash_iv_0[4];
                    /*!< Offset: 0x970 (RW) */
        __IM uint32_t ghash_busy;
                    /*!< Offset: 0x980 (RO) */
        __OM uint32_t ghash_init;
                    /*!< Offset: 0x984 (WO) */
        uint8_t _reserved_0[120];
                    /*!< Offset: 0x988-0x9FC Reserved */
    } ghash;

    /* Host_RGF block */
    __PACKED_STRUCT {
        __IM  uint32_t host_rgf_irr;
                    /*!< Offset: 0xA00 (RO) */
        __IOM uint32_t host_rgf_imr;
                    /*!< Offset: 0xA04 (RW) */
        __OM uint32_t host_rgf_icr;
                    /*!< Offset: 0xA08 (WO) */
        __IOM uint32_t host_rgf_endian;
                    /*!< Offset: 0xA0C (RW) */
        uint8_t _reserved_0[20];
                    /*!< Offset: 0xA10-0xA20 Reserved */
        __IM uint32_t host_rgf_signature;
                    /*!< Offset: 0xA24 (RO) */
        __IM uint32_t host_boot;
                    /*!< Offset: 0xA28 (RO) */
        uint8_t _reserved_1[12];
                    /*!< Offset: 0xA2C-0xA34 Reserved */
        __IOM uint32_t host_cryptokey_sel;
                    /*!< Offset: 0xA38 (RW) */
        uint8_t _reserved_2[60];
                    /*!< Offset: 0xA3C-0xA78 Reserved */
        __IOM uint32_t host_core_clk_gating_enable;
                    /*!< Offset: 0xA78 (RW) */
        __IM uint32_t host_cc_is_idle;
                    /*!< Offset: 0xA7C (RO) */
        __IOM uint32_t host_powerdown;
                    /*!< Offset: 0xA80 (RW) */
        __IM uint32_t host_remove_ghash_engine;
                    /*!< Offset: 0xA84 (RO) */
        __IM uint32_t host_remove_chacha_engine;
                    /*!< Offset: 0xA88 (RO) */
        __IOM uint32_t host_sf_dynamic_cntl;
                    /*!< Offset: 0xA8C (RW) */
        __IM uint32_t host_sf_ready;
                    /*!< Offset: 0xA90 (RO) */
        uint8_t _reserved_3[108];
                    /*!< Offset: 0xA94-0xAFC Reserved */
    } host_rgf;

    /* AHB block */
    __PACKED_STRUCT {
        __IOM uint32_t ahbm_singles;
                    /*!< Offset: 0xB00 (RW) */
        __IOM uint32_t ahbm_hprot;
                    /*!< Offset: 0xB04 (RW) */
        __IOM uint32_t ahbm_hmastlock;
                    /*!< Offset: 0xB08 (RW) */
        __IOM uint32_t ahbm_hnonsec;
                    /*!< Offset: 0xB0C (RW) */
        uint8_t _reserved_0[240];
                    /*!< Offset: 0xB10-0xBF4 Reserved */
    } ahb;

    /* DIN block */
    __PACKED_STRUCT {
        __OM uint32_t din_buffer;
                    /*!< Offset: 0xC00 (WO) */
        uint8_t _reserved_0[28];
                    /*!< Offset: 0xC04-0xC1C Reserved */
        __IM uint32_t din_mem_dma_busy;
                    /*!< Offset: 0xC20 (RO) */
        uint8_t _reserved_1[4];
                    /*!< Offset: 0xC24 Reserved */
        __OM uint32_t src_lli_word0;
                    /*!< Offset: 0xC28 (WO) */
        __OM uint32_t src_lli_word1;
                    /*!< Offset: 0xC2C (WO) */
        __IOM uint32_t sram_src_addr;
                    /*!< Offset: 0xC30 (RW) */
        __IOM uint32_t din_sram_bytes_len;
                    /*!< Offset: 0xC34 (RW) */
        __IM uint32_t din_sram_dma_busy;
                    /*!< Offset: 0xC38 (RO) */
        uint8_t _reserved_2[12];
                    /*!< Offset: 0xC3C-0xC44 Reserved */
        __OM uint32_t din_cpu_data_size;
                    /*!< Offset: 0xC48 (WO) */
        uint8_t _reserved_3[4];
                    /*!< Offset: 0xC4C Reserved */
        __IM uint32_t fifo_in_empty;
                    /*!< Offset: 0xC50 (RO) */
        uint8_t _reserved_4[4];
                    /*!< Offset: 0xC54 Reserved */
        __OM uint32_t din_fifo_rst_pntr;
                    /*!< Offset: 0xC58 (WO) */
        uint8_t _reserved_5[164];
                    /*!< Offset: 0xC5C-0xCFC Reserved */
    } din;

    /* DOUT block */
    __PACKED_STRUCT {
        __IM uint32_t dout_buffer;
                    /*!< Offset: 0xD00 (RO) */
        uint8_t _reserved_0[28];
                    /*!< Offset: 0xD04-0xD1C Reserved */
        __IM uint32_t dout_mem_dma_busy;
                    /*!< Offset: 0xD20 (RO) */
        uint8_t _reserved_1[4];
                    /*!< Offset: 0xD24 Reserved */
        __OM uint32_t dst_lli_word0;
                    /*!< Offset: 0xD28 (WO) */
        __IOM uint32_t dst_lli_word1;
                    /*!< Offset: 0xD2C (RW) */
        __IOM uint32_t sram_dest_addr;
                    /*!< Offset: 0xD30 (RW) */
        __IOM uint32_t dout_sram_bytes_len;
                    /*!< Offset: 0xD34 (RW) */
        __IM uint32_t dout_sram_dma_busy;
                    /*!< Offset: 0xD38 (RO) */
        uint8_t _reserved_2[8];
                    /*!< Offset: 0xD3C-0xD40 Reserved */
        __OM uint32_t read_align_last;
                    /*!< Offset: 0xD44 (WO) */
        uint8_t _reserved_3[8];
                    /*!< Offset: 0xD48-0xD4C Reserved */
        __IM uint32_t dout_fifo_empty;
                    /*!< Offset: 0xD50 (RO) */
        uint8_t _reserved_4[428];
                    /*!< Offset: 0xD54-0xEFC Reserved */
    } dout;

    /* Host_SRAM block */
    __PACKED_STRUCT {
        __IOM uint32_t sram_data;
                    /*!< Offset: 0x0F00 (RW) */
        __OM  uint32_t sram_addr;
                    /*!< Offset: 0x0F04 (WO) */
        __IM  uint32_t sram_data_ready;
                    /*!< Offset: 0x0F08 (RO) */
        uint8_t _reserved_0[196];
                    /*!< Offset: 0xF08-0xFCC Reserved */
    } host_sram;

    /* ID_Registers block */
    __PACKED_STRUCT {
        __IM uint32_t peripheral_id_4;
                    /*!< Offset: 0xFD0 (RO) */
        __IM uint32_t pidreserved0;
                    /*!< Offset: 0xFD4 (RO) */
        __IM uint32_t pidreserved1;
                    /*!< Offset: 0xFD8 (RO) */
        __IM uint32_t pidreserved2;
                    /*!< Offset: 0xFDC (RO) */
        __IM uint32_t peripheral_id_0;
                    /*!< Offset: 0xFE0 (RO) */
        __IM uint32_t peripheral_id_1;
                    /*!< Offset: 0xFE4 (RO) */
        __IM uint32_t peripheral_id_2;
                    /*!< Offset: 0xFE8 (RO) */
        __IM uint32_t peripheral_id_3;
                    /*!< Offset: 0xFEC (RO) */
        __IM uint32_t component_id_0;
                    /*!< Offset: 0xFF0 (RO) */
        __IM uint32_t component_id_1;
                    /*!< Offset: 0xFF4 (RO) */
        uint8_t _reserved_0[3592];
                    /*!< Offset: 0xFF8-0x1DFC Reserved */
    } id;

    /* AO block */
    __PACKED_STRUCT {
        __IOM uint32_t host_dcu_en[4];
                    /*!< Offset: 0x1E00 (RW) */
        __IOM uint32_t host_dcu_lock[4];
                    /*!< Offset: 0x1E10 (RW) */
        __IM uint32_t ao_icv_dcu_restriction_mask[4];
                    /*!< Offset: 0x1E20 (RO) */
        __IM uint32_t ao_cc_sec_debug_reset;
                    /*!< Offset: 0x1E30 (RO) */
        __IOM uint32_t host_ao_lock_bits;
                    /*!< Offset: 0x1E34 (RW) */
        __IOM uint32_t ao_apb_filtering;
                    /*!< Offset: 0x1E38 (RW) */
        __IM uint32_t ao_cc_gppc;
                    /*!< Offset: 0x1E3C (RO) */
        __OM uint32_t host_rgf_cc_sw_rst;
                    /*!< Offset: 0x1E40 (WO) */
        __IM uint32_t dcu_debug_bits[4];
                    /*!< Offset: 0x1E44 (RO) */
        __IM uint32_t ao_permanent_disable_mask[4];
                    /*!< Offset: 0x1E54 (RO) */
        uint8_t _reserved_0[160];
                    /*!< Offset: 0x1E58-0x1EFC Reserved */
    } ao;

    /* NVM block */
    __PACKED_STRUCT {
        __IM uint32_t aib_fuse_prog_completed;
                    /*!< Offset: 0x1F04 (RO) */
        __IM uint32_t nvm_debug_status;
                    /*!< Offset: 0x1F08 (RO) */
        __IM uint32_t lcs_is_valid;
                    /*!< Offset: 0x1F0C (RO) */
        __IM uint32_t nvm_is_idle;
                    /*!< Offset: 0x1F10 (RO) */
        __IM uint32_t lcs_reg;
                    /*!< Offset: 0x1F14 (RO) */
        __OM uint32_t host_shadow_kdr_reg;
                    /*!< Offset: 0x1F18 (WO) */
        __OM uint32_t host_shadow_kcp_reg;
                    /*!< Offset: 0x1F1C (WO) */
        __OM uint32_t host_shadow_kce_reg;
                    /*!< Offset: 0x1F20 (WO) */
        __OM uint32_t host_shadow_kpicv_reg;
                    /*!< Offset: 0x1F24 (WO) */
        __OM uint32_t host_shadow_kceicv_reg;
                    /*!< Offset: 0x1F28 (WO) */
        __IM uint32_t otp_addr_width_def;
                    /*!< Offset: 0x1F2C (RO) */
        __IM uint32_t gp_param;
                    /*!< Offset: 0x1F30 (RO) */
        uint8_t _reserved_0[204];
                    /*!< Offset: 0x1F34-0x1FFC Reserved */
    } nvm;

    __PACKED_STRUCT {
        __IOM uint32_t huk[8];
        __IOM uint32_t icv_provisioning_key[4];
        __IOM uint32_t icv_code_encryption_key[4];
        __IOM uint32_t icv_programmed_flags[1];
        __PACKED_UNION {
            __IOM uint32_t rot_public_key[8];
            __PACKED_STRUCT {
                __IOM uint32_t icv_rot_public_key[4];
                __IOM uint32_t oem_rot_public_key[4];
            };
        };
        __IOM uint32_t oem_provisioning_secret[4];
        __IOM uint32_t oem_code_encryption_key[4];
        __IOM uint32_t oem_programmed_flags[1];
        __PACKED_UNION {
            __IOM uint32_t nv_counter[5];
            __PACKED_STRUCT {
                __IOM uint32_t icv_nv_counter[2];
                __IOM uint32_t oem_nv_counter[3];
            };
        };
        __IOM uint32_t general_purpose_configuration_flags[1];
        __IOM uint32_t dcu_debug_lock_mask[4];
        __IOM uint32_t general_purpose_code_data[];
    } otp;
};

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_REG_DEFS_H */
