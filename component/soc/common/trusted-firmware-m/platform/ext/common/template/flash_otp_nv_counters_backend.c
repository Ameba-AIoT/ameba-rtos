/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* NOTE: For the security of the protected storage system, the bootloader
 * rollback protection, and the protection of cryptographic material it is
 * CRITICAL to use an internal (in-die) persistent memory for the implementation
 * of the OTP_NV_COUNTERS flash area.
 */

#include "flash_otp_nv_counters_backend.h"

#include "tfm_plat_defs.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

#include <string.h>

#include "cmsis.h"
#include "core_cache.h"
#include "ameba_cache.h"
#include "ameba_crypto_api.h"
#include "rand.h"
#include "sysreg_sec.h"
#include "ameba_syscfg.h"

static enum tfm_plat_err_t create_or_restore_layout(void);

#ifdef OTP_NV_COUNTERS_RAM_EMULATION

static struct flash_otp_nv_counters_region_t otp_nv_ram_buf = {0};

enum tfm_plat_err_t read_otp_nv_counters_flash(uint32_t offset, void *data, uint32_t cnt)
{
    memcpy(data, ((void*)&otp_nv_ram_buf) + offset, cnt);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t init_otp_nv_counters_flash(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;

    if (otp_nv_ram_buf.init_value != OTP_NV_COUNTERS_INITIALIZED) {
#if defined(OTP_WRITEABLE)
        err = create_or_restore_layout();
#else
        err = TFM_PLAT_ERR_SYSTEM_ERR;
#endif
    }
    return err;
}

#if defined(OTP_WRITEABLE)
static enum tfm_plat_err_t create_or_restore_layout(void)
{
    memset(&otp_nv_ram_buf, 0, sizeof(otp_nv_ram_buf));
    otp_nv_ram_buf.init_value = OTP_NV_COUNTERS_INITIALIZED;
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t write_otp_nv_counters_flash(uint32_t offset, const void *data, uint32_t cnt)
{
    memcpy(((void*)&otp_nv_ram_buf) + offset, data, cnt);

    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* defined(OTP_WRITEABLE)*/

#else /* OTP_NV_COUNTERS_RAM_EMULATION */
/* Valid entries for data item width */
static const uint32_t data_width_byte[] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
};

#if defined(OTP_WRITEABLE)
static enum tfm_plat_err_t make_backup(void);
#endif

/* Compilation time checks to be sure the defines are well defined */
#ifndef TFM_OTP_NV_COUNTERS_AREA_ADDR
#error "TFM_OTP_NV_COUNTERS_AREA_ADDR must be defined in flash_layout.h"
#endif

#if defined(OTP_WRITEABLE)
    #ifndef TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR
    #error "TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR must be defined in flash_layout.h"
    #endif
#endif

#ifndef TFM_OTP_NV_COUNTERS_AREA_SIZE
#error "TFM_OTP_NV_COUNTERS_AREA_SIZE must be defined in flash_layout.h"
#endif

#if defined(OTP_WRITEABLE)
    #ifndef TFM_OTP_NV_COUNTERS_SECTOR_SIZE
    #error "TFM_OTP_NV_COUNTERS_SECTOR_SIZE must be defined in flash_layout.h"
    #endif
#endif
#ifndef OTP_NV_COUNTERS_FLASH_DEV
    #ifndef TFM_HAL_ITS_FLASH_DRIVER
    #error "OTP_NV_COUNTERS_FLASH_DEV or TFM_HAL_ITS_FLASH_DRIVER must be defined in flash_layout.h"
    #else
    #define OTP_NV_COUNTERS_FLASH_DEV TFM_HAL_ITS_FLASH_DRIVER
    #endif
#endif

/* Default flash block size is either the PROGRAM_UNIT if it is > 128, or if the
 * program unit is < 128, then it is the closest multiple of the PROGRAM_UNIT to
 * 128. The aim of this is to keep the block size reasonable, to avoid large
 * amounts of read/write calls, while also keeping sensible alignment.
 */
#ifndef OTP_NV_COUNTERS_WRITE_BLOCK_SIZE
#define OTP_NV_COUNTERS_WRITE_BLOCK_SIZE (TFM_HAL_ITS_PROGRAM_UNIT > 256 ? \
                                          TFM_HAL_ITS_PROGRAM_UNIT : \
                                          (256 / TFM_HAL_ITS_PROGRAM_UNIT) * TFM_HAL_ITS_PROGRAM_UNIT)
#endif /* OTP_NV_COUNTERS_WRITE_BLOCK_SIZE */

#if (TFM_OTP_NV_COUNTERS_SECTOR_SIZE % OTP_NV_COUNTERS_WRITE_BLOCK_SIZE != 0) || \
    (OTP_NV_COUNTERS_WRITE_BLOCK_SIZE % TFM_HAL_ITS_PROGRAM_UNIT != 0)
#error "OTP_NV_COUNTERS_WRITE_BLOCK_SIZE has wrong alignment"
#endif
/* End of compilation time checks to be sure the defines are well defined */

static uint8_t block[OTP_NV_COUNTERS_WRITE_BLOCK_SIZE];

/* Import the CMSIS flash device driver */
extern ARM_DRIVER_FLASH OTP_NV_COUNTERS_FLASH_DEV;
#define GCM_TAG_SIZE		16

/*flash_otp_nv_counters_region_t is encrypto except init_value, hash and swap_count*/
static enum tfm_plat_err_t read_otp_nv_counters_flash_crypto(uint32_t offset, void *data, uint32_t cnt)
{
	enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
	u8 Key[32];
	u32 enc_start_offset = sizeof(((struct flash_otp_nv_counters_region_t*)0)->init_value);
	u32 enc_end_offset = offsetof(struct flash_otp_nv_counters_region_t, hash);
	u32 region_offset;
	u32 swap_count_start_offset = offsetof(struct flash_otp_nv_counters_region_t, swap_count);
    uint32_t swap_count;

	ALIGNMTO(CACHE_LINE_SIZE) u8 dec_tag[CACHE_LINE_ALIGMENT(GCM_TAG_SIZE)];
	ALIGNMTO(CACHE_LINE_SIZE) u8 dec_buf[CACHE_LINE_ALIGMENT(OTP_NV_COUNTERS_WRITE_BLOCK_SIZE)];

    if (cnt == 0) {
	    return TFM_PLAT_ERR_SUCCESS;
	}

	if(offset < FLASH_OTP_NV_COUNTERS_AREA_OFFSET || offset >= (FLASH_OTP_NV_COUNTERS_AREA_OFFSET + FLASH_OTP_NV_COUNTERS_AREA_SIZE)) {
		return TFM_PLAT_ERR_SYSTEM_ERR;
	}

	if(offset < TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR) {
		region_offset = offset - FLASH_OTP_NV_COUNTERS_AREA_OFFSET;
	} else {
		region_offset = offset - TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR;
	}

	if(((region_offset == 0) && (cnt == enc_start_offset)) || region_offset >= swap_count_start_offset) {
		err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.ReadData(offset, data, cnt);
		return (err < 0) ? TFM_PLAT_ERR_SYSTEM_ERR : err;
	}

	if(cnt > swap_count_start_offset) {
		return TFM_PLAT_ERR_SYSTEM_ERR;
	}

	/* Read all encrypto data for hash check */
	if(offset < TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR) {
		err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(FLASH_OTP_NV_COUNTERS_AREA_OFFSET, block, sizeof(struct flash_otp_nv_counters_region_t));
	} else {
		err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR, block, sizeof(struct flash_otp_nv_counters_region_t));
	}
	if (err < 0) {
		return TFM_PLAT_ERR_SYSTEM_ERR;
	}

	memcpy(&swap_count, (const void *)&block[swap_count_start_offset], sizeof(swap_count));
	/*Invalid state, do not decrypto*/
	if (swap_count == 0 || swap_count == UINT32_MAX) {
		memcpy(data, (const void *)&block[region_offset], cnt);
		return TFM_PLAT_ERR_SUCCESS;
	}

	/* Initialize AES engine */
	CRYPTO_Init(NULL);
	/* Load RDP key to IPSEC */
	CRYPTO_OTPKey_Init(0, ENABLE);

	do {
		/* Key array not used, pass in for API restrict */
		err = rtl_crypto_aes_gcm_init(Key, sizeof(Key));
		if (err != TFM_PLAT_ERR_SUCCESS) {
			break;
		}

		err = rtl_crypto_aes_gcm_decrypt(&block[enc_start_offset], enc_end_offset - enc_start_offset, DerivedKey_Bkup,
										DerivedKey_Bkup, sizeof(DerivedKey_Bkup), dec_buf, dec_tag);
		if (err != TFM_PLAT_ERR_SUCCESS) {
			break;
		}

		err = memcmp(&block[enc_end_offset], dec_tag, GCM_TAG_SIZE);
		if (err != TFM_PLAT_ERR_SUCCESS) {
			break;
		}

		memcpy(&block[enc_start_offset], dec_buf, enc_end_offset - enc_start_offset);
		memcpy(data, (const void *)&block[region_offset], cnt);
	} while (0);

	CRYPTO_OTPKey_Init(0, DISABLE);
	return (err != TFM_PLAT_ERR_SUCCESS) ? TFM_PLAT_ERR_SYSTEM_ERR : err;
}

static enum tfm_plat_err_t write_otp_nv_counters_flash_crypto(uint32_t offset, const void *data, uint32_t cnt)
{
	enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
	u8 Key[32];
	u32 enc_start_offset = sizeof(((struct flash_otp_nv_counters_region_t*)0)->init_value);
	u32 enc_end_offset = offsetof(struct flash_otp_nv_counters_region_t, hash);
	u32 swap_count_start_offset = offsetof(struct flash_otp_nv_counters_region_t, swap_count);
	u8 *pbuf = (u8 *)data;

	ALIGNMTO(CACHE_LINE_SIZE) u8 enc_tag[CACHE_LINE_ALIGMENT(GCM_TAG_SIZE)];
	ALIGNMTO(CACHE_LINE_SIZE) u8 enc_buf[CACHE_LINE_ALIGMENT(OTP_NV_COUNTERS_WRITE_BLOCK_SIZE)];

	if (cnt == 0) {
		return TFM_PLAT_ERR_SUCCESS;
	} else if (cnt != swap_count_start_offset || offset != FLASH_OTP_NV_COUNTERS_AREA_OFFSET) {
		return TFM_PLAT_ERR_SYSTEM_ERR;
	}

	/* Initialize AES engine */
	CRYPTO_Init(NULL);
	/* Load RDP key to IPSEC */
	CRYPTO_OTPKey_Init(0, ENABLE);

	do {
		/* Key array not used, pass in for API restrict */
		err = rtl_crypto_aes_gcm_init(Key, sizeof(Key));
		if (err != TFM_PLAT_ERR_SUCCESS) {
			err = TFM_PLAT_ERR_SYSTEM_ERR;
			break;
		}

		err = rtl_crypto_aes_gcm_encrypt(&pbuf[enc_start_offset], enc_end_offset - enc_start_offset, DerivedKey_Bkup,
										DerivedKey_Bkup, sizeof(DerivedKey_Bkup), enc_buf, enc_tag);
		if (err != TFM_PLAT_ERR_SUCCESS) {
			err = TFM_PLAT_ERR_SYSTEM_ERR;
			break;
		}

		memcpy((void *)&pbuf[enc_start_offset], enc_buf, enc_end_offset - enc_start_offset);
		memcpy((void *)&pbuf[enc_end_offset], enc_tag, GCM_TAG_SIZE);
        err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.ProgramData(offset, pbuf, swap_count_start_offset);
        if (err < 0) {
			err = TFM_PLAT_ERR_SYSTEM_ERR;
			break;
        }
	} while (0);

	CRYPTO_OTPKey_Init(0, DISABLE);
	return err;
}

enum tfm_plat_err_t read_otp_nv_counters_flash(uint32_t offset, void *data, uint32_t cnt)
{
    int32_t err;
    ARM_FLASH_CAPABILITIES DriverCapabilities;
    uint8_t data_width;
    uint32_t remaining_cnt, read_cnt;

    DriverCapabilities = OTP_NV_COUNTERS_FLASH_DEV.GetCapabilities();
    data_width = data_width_byte[DriverCapabilities.data_width];

    if (offset % data_width) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }
    remaining_cnt = cnt;
    read_cnt = 0;
    if (remaining_cnt) {
		err = read_otp_nv_counters_flash_crypto(
                                        TFM_OTP_NV_COUNTERS_AREA_ADDR + offset,
                                        data,
                                        cnt / data_width);
        if (err == TFM_PLAT_ERR_SYSTEM_ERR) {
            return err;
        }
    }

#ifdef AMEBA_NONEED
    uint8_t temp_buffer[sizeof(uint32_t)];

    read_cnt += (cnt / data_width) * data_width;
    remaining_cnt -= read_cnt;
    if (remaining_cnt) {
        err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(
                            TFM_OTP_NV_COUNTERS_AREA_ADDR + offset + read_cnt,
                            temp_buffer,
                            1);
        if (err < 0) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
        memcpy((uint8_t *)data + read_cnt, temp_buffer, remaining_cnt);
    }
#endif
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t init_otp_nv_counters_flash(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    uint32_t init_value;
    uint32_t swap_count;
    uint32_t backup_swap_count;

    if ((TFM_OTP_NV_COUNTERS_AREA_SIZE) < sizeof(struct flash_otp_nv_counters_region_t)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

	if (sizeof(struct flash_otp_nv_counters_region_t) > sizeof(block)) {
		return TFM_PLAT_ERR_SYSTEM_ERR;
	}

    err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.Initialize(NULL);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, init_value),
            &init_value, sizeof(init_value));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, swap_count),
            &swap_count, sizeof(swap_count));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    if (init_value != OTP_NV_COUNTERS_INITIALIZED ||
        (swap_count == 0 || swap_count == UINT32_MAX)) {
#if defined(OTP_WRITEABLE)
        err = create_or_restore_layout();
    }
    else
    {
        err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, swap_count)
                + TFM_OTP_NV_COUNTERS_AREA_SIZE,
                &backup_swap_count, sizeof(backup_swap_count));
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        if (backup_swap_count == 0 || backup_swap_count == UINT32_MAX ||
            backup_swap_count < swap_count ||
            (backup_swap_count == UINT32_MAX - 1 && swap_count == 1)) {
            err = make_backup();
            if (err != TFM_PLAT_ERR_SUCCESS) {
                return err;
            }
        }
#else
        err = TFM_PLAT_ERR_SYSTEM_ERR;
#endif
    }

    return err;
}

#if defined(OTP_WRITEABLE)
static inline uint32_t round_down(uint32_t num, uint32_t boundary)
{
    return num - (num % boundary);
}

static inline uint32_t round_up(uint32_t num, uint32_t boundary)
{
    return (num + boundary - 1) - ((num + boundary - 1) % boundary);
}

static enum tfm_plat_err_t erase_flash_region(size_t start, size_t size)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    size_t idx;

    if ((start % TFM_OTP_NV_COUNTERS_SECTOR_SIZE) != 0) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    for (idx = round_down(start, TFM_OTP_NV_COUNTERS_SECTOR_SIZE);
         idx < start + size;
         idx += TFM_OTP_NV_COUNTERS_SECTOR_SIZE) {
        err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.EraseSector(idx);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return err;
}

static enum tfm_plat_err_t copy_flash_region(size_t from, size_t to, size_t size)
{
    int32_t err;
    size_t copy_size;
    size_t idx;
    size_t end;

    end = size;
    ARM_FLASH_CAPABILITIES DriverCapabilities;
    uint8_t data_width;

    DriverCapabilities = OTP_NV_COUNTERS_FLASH_DEV.GetCapabilities();
    data_width = data_width_byte[DriverCapabilities.data_width];

    for(idx = 0; idx < end; idx += copy_size) {
        copy_size = (idx + sizeof(block)) <= end ? sizeof(block) : end - idx;

        err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(from + idx, block, copy_size / data_width);
        if (err < 0) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        err = OTP_NV_COUNTERS_FLASH_DEV.ProgramData(to + idx, block, copy_size / data_width);
        if (err < 0) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }
    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t make_backup(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;

    err = erase_flash_region(TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR,
                             TFM_OTP_NV_COUNTERS_AREA_SIZE);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = copy_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_AREA_SIZE);

    return err;
}

static enum tfm_plat_err_t copy_data_into_block(uint32_t data_offset,
                                                size_t data_size,
                                                const uint8_t *data,
                                                uint32_t block_offset,
                                                size_t block_size,
                                                uint8_t *block)
{
    uint32_t copy_start_offset;
    uint32_t copy_end_offset;

    /* Check if any of our data needs to be copied into this buf */
    if (data_offset + data_size > block_offset &&
        data_offset < block_offset + block_size) {
        if (data_offset > block_offset) {
            copy_start_offset = data_offset;
        } else {
            copy_start_offset = block_offset;
        }

        if (data_offset + data_size > block_offset + block_size) {
            copy_end_offset = block_offset + block_size;
        } else {
            copy_end_offset = data_offset + data_size;
        }

        memcpy(block + (copy_start_offset - block_offset),
               data + (copy_start_offset - data_offset),
               copy_end_offset - copy_start_offset);
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t write_otp_nv_counters_flash(uint32_t offset, const void *data, uint32_t cnt)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    size_t copy_size;
    size_t erase_start_offset;
    size_t erase_end_offset;
    size_t swap_count_erase_start_offset;
    size_t swap_count_program_block_start_offset;
    size_t idx;
    uint32_t swap_count;
    uint32_t swap_count_buf_size = TFM_HAL_ITS_PROGRAM_UNIT > sizeof(swap_count) ?
        TFM_HAL_ITS_PROGRAM_UNIT : sizeof(swap_count);

    erase_start_offset = round_down(offset, TFM_OTP_NV_COUNTERS_SECTOR_SIZE);
    erase_end_offset = round_up(offset + cnt, TFM_OTP_NV_COUNTERS_SECTOR_SIZE);

    swap_count_erase_start_offset =
        round_down(offsetof(struct flash_otp_nv_counters_region_t, swap_count),
                   TFM_OTP_NV_COUNTERS_SECTOR_SIZE);

    swap_count_program_block_start_offset =
        round_down(offsetof(struct flash_otp_nv_counters_region_t, swap_count),
                   TFM_HAL_ITS_PROGRAM_UNIT);

    if (erase_end_offset > TFM_OTP_NV_COUNTERS_AREA_SIZE) {
        /* Erase is beyond the TFM_OTP_NV_COUNTERS_AREA */
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    ARM_FLASH_CAPABILITIES DriverCapabilities;
    uint8_t data_width;

    DriverCapabilities = OTP_NV_COUNTERS_FLASH_DEV.GetCapabilities();
    data_width = data_width_byte[DriverCapabilities.data_width];

    /* read the swap_count now, to make life easier when writing it later */
    err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.ReadData(
            TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR +
            offsetof(struct flash_otp_nv_counters_region_t, swap_count),
            &swap_count, sizeof(swap_count) / data_width);
    if (err < 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    swap_count += 1;
    if (swap_count == UINT32_MAX) {
        swap_count = 1;
    }

    /* If it's not part of the sectors that are being erased, first erase the
     * sector with the swap_count flag.
     */
    if (erase_end_offset <= swap_count_erase_start_offset) {
        err = erase_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR +
                                 swap_count_erase_start_offset,
                                 TFM_OTP_NV_COUNTERS_SECTOR_SIZE);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    /* Erase the region we want to write */
    err = erase_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR + erase_start_offset,
                             erase_end_offset - erase_start_offset);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Don't write the swap count, as this is done at the end */
    if (erase_end_offset > swap_count_program_block_start_offset) {
        erase_end_offset = swap_count_program_block_start_offset;
    }

    for (idx = erase_start_offset; idx < erase_end_offset; idx += copy_size) {
        if ((idx + sizeof(block)) <= erase_end_offset) {
            copy_size = sizeof(block);
        } else {
             copy_size = erase_end_offset - idx;
        }

		err = read_otp_nv_counters_flash_crypto(
                                        TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR + idx,
                                        block,
                                        copy_size / data_width);
        if (err == TFM_PLAT_ERR_SYSTEM_ERR) {
            return err;
        }

        err = copy_data_into_block(offset, cnt, data, idx, copy_size, block);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        uint32_t num_items = copy_size / data_width;

		err = write_otp_nv_counters_flash_crypto(TFM_OTP_NV_COUNTERS_AREA_ADDR + idx, block, num_items);
        if (err == TFM_PLAT_ERR_SYSTEM_ERR) {
            return err;
        }

        /* When err is positive it contains the number of data items
         * successfully programmed. Check that every byte of
         * programming succeeded.
         */
        if (err > 0 && err != num_items) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    /* If we've not already restored most of the last sector (except the swap
     * count), do it now. */
    if (erase_end_offset < swap_count_program_block_start_offset) {
        err = copy_flash_region(
                TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR + swap_count_erase_start_offset,
                TFM_OTP_NV_COUNTERS_AREA_ADDR + swap_count_erase_start_offset,
                swap_count_program_block_start_offset - swap_count_erase_start_offset);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    /* Copy the last program-unit. This _may_ contain the data that we needed to
     * write on platforms that have large program-units, so attempt to write
     * into the buffer still (and let copy_data_into_block() check if it needs
     * to actually copy).
     */
    err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.ReadData(
            TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR + swap_count_program_block_start_offset,
            block, swap_count_buf_size / data_width);
    if (err < 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = copy_data_into_block(offset, cnt, data,
                               swap_count_program_block_start_offset,
                               swap_count_buf_size, block);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = copy_data_into_block(
            offsetof(struct flash_otp_nv_counters_region_t, swap_count),
            sizeof(swap_count), (uint8_t *)&swap_count,
            swap_count_program_block_start_offset, swap_count_buf_size, block);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    uint32_t num_items = swap_count_buf_size / data_width;

    err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.ProgramData(
            TFM_OTP_NV_COUNTERS_AREA_ADDR + swap_count_program_block_start_offset,
            block, num_items);
    if (err < 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* When err is positive it contains the number of data items
     * successfully programmed. Check that every byte of
     * programming succeeded.
     */
    if (err > 0 && err != num_items) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = make_backup();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    return err;
}

static enum tfm_plat_err_t restore_backup(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;

    err = erase_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                             TFM_OTP_NV_COUNTERS_AREA_SIZE);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = copy_flash_region(TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_AREA_SIZE);

    return err;
}

static enum tfm_plat_err_t create_or_restore_layout(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    uint32_t init_value;
    uint32_t swap_count;
    uint32_t backup_init_value;
    uint32_t backup_swap_count;
    size_t idx;
    size_t end;
    size_t copy_size;
    ARM_FLASH_CAPABILITIES DriverCapabilities;
    uint8_t data_width;

    DriverCapabilities = OTP_NV_COUNTERS_FLASH_DEV.GetCapabilities();
    data_width = data_width_byte[DriverCapabilities.data_width];

    err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, init_value)
            + TFM_OTP_NV_COUNTERS_AREA_SIZE,
            &backup_init_value, sizeof(init_value));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, swap_count)
            + TFM_OTP_NV_COUNTERS_AREA_SIZE,
            &backup_swap_count, sizeof(swap_count));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    if (backup_init_value == OTP_NV_COUNTERS_INITIALIZED &&
        /* valid backup, restore */
        backup_swap_count != 0 && backup_swap_count != UINT32_MAX)  {
        err = restore_backup();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    } else {
        /* No valid layouts, create from scratch */
        err = erase_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                TFM_OTP_NV_COUNTERS_AREA_SIZE);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        memset(block, 0, sizeof(block));
        end = TFM_OTP_NV_COUNTERS_AREA_SIZE;
        for(idx = 0; idx < end; idx += copy_size) {
            copy_size = (idx + sizeof(block)) <= end ? sizeof(block) : end - idx;

            err = (enum tfm_plat_err_t)OTP_NV_COUNTERS_FLASH_DEV.ProgramData(TFM_OTP_NV_COUNTERS_AREA_ADDR + idx,
                    block, copy_size / data_width);
            if (err < 0) {
                return TFM_PLAT_ERR_SYSTEM_ERR;
            }
        }

        err = make_backup();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        init_value = OTP_NV_COUNTERS_INITIALIZED;
        err = write_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, init_value),
                &init_value, sizeof(init_value));
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        swap_count = 1;
        err = write_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, swap_count),
                &swap_count, sizeof(swap_count));
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }
    return err;
}
#endif /*  OTP_WRITEABLE */

#endif /* OTP_NV_COUNTERS_RAM_EMULATION */
