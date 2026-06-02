/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOT_H_
#define _AMEBA_BOOT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern u8 __image1_validate_code__[];
extern u8 __image1_bss_start__[];
extern u8 __image1_bss_end__[];

extern u8 __image2_entry_func__[];
extern u8 __bss_start__[];
extern u8 __bss_end__[];

extern u8 __cmd_table_start__[];
extern u8 __cmd_table_end__[];
extern u8 __psram_bss_start__[];
extern u8 __psram_bss_end__[];
extern u8 __ram_nocache_start__[];
extern u8 __ram_nocache_end__[];
extern u8 __image3_bss_start__[];
extern u8 __image3_bss_end__[];
extern u8 __image3_heap_start__[];
extern u8 __image3_heap_size__[];
extern u8 __ram_image2_text_end__[];

extern u8 __sram_image2_start__[];
extern u8 __sram_image2_end__[];
extern u8 __ipc_table_start__[];
extern u8 __ipc_table_end__[];
extern u8 __global_bss_start__[];
extern u8 __global_bss_end__[];
extern u8 __km4tz_pqc_rom_msp_top__[];

extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __rom_bss_start_s__[];
extern u8 __rom_bss_end_s__[];
extern u8 __rom_bss_start_ns__[];
extern u8 __rom_bss_end_ns__[];

extern u8 __km4tz_ns_rom_start__[];
extern u8 __km4tz_ns_rom_end__[];
extern u8 __km4tz_rombss_ram_ns_start__[];
extern u8 __km4tz_bootloader_ram_s_start__[];

extern u8 __ipc_memory_start__[];
extern u8 __km4tz_boot_text_start__[];
extern u8 __km4tz_boot_text_end__[];
extern u8 __km4tz_flash_text_start__[];
extern u8 __km4tz_flash_text_end__[];
extern u8 __km4ns_flash_text_start__[];
extern u8 __km4ns_flash_text_end__[];
extern u8 __km4ns_app_flash_text_start__[];
extern u8 __km4ns_app_flash_text_end__[];
extern u8 __km4tz_img3_text_start__[];
extern u8 __km4tz_img3_text_end__[];

extern u8 __bdram_heap_buffer_start__[];
extern u8 __bdram_heap_buffer_size__[];
extern u8 __psram_heap_buffer_start__[];
extern u8 __psram_heap_buffer_size__[];
extern u8 __psram_heap_extend_start__[];
extern u8 __psram_heap_extend_size__[];
extern u8 __retrieve_ap_buffer_start__[];
extern u8 __retrieve_ap_buffer_size__[];

/* for trustzone config */
extern u8 __image3_entry_func__[];
extern u8 __image3_ram_start__[];
extern u8 __image3_ram_end__[];
extern u8 __ram_image3_nsc_start__[];
extern u8 __ram_image3_nsc_end__[];
extern u8 __km4tz_bd_ram_start__[];
extern u8 __km4tz_bd_ram_end__[];
extern u8 __km4ns_bd_ram_end__[];

extern u8 __km4tz_bd_psram_start__[];
extern u8 __non_secure_psram_end__[]; /* if psram is 8MB, than write 0x60800000 will write 0x60000000 */

extern u8 __km4tz_msp_ram_ns_limit__[];
extern u8 __km4tz_msp_ram_ns__[];

/* backtrace addr */
extern u8 __rom_text_region0_start__[];
extern u8 __rom_text_region0_end__[];
extern u8 __rom_text_region1_start__[];
extern u8 __rom_text_region1_end__[];
extern u8 __image2_backtrace_start__[];
extern u8 __image2_backtrace_end__[];
extern u8 __flash_text_start__[];
extern u8 __flash_text_end__[];
extern u8 __sram_image2_start__[];
extern u8 __sram_image2_end__[];
extern u8 __psram_image2_start__[];
extern u8 __psram_image2_end__[];

#if defined (CONFIG_UNITY_TEST) && CONFIG_UNITY_TEST
extern u8 __unity_table_start__[];
extern u8 __unity_table_end__[];
#endif

#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
extern u8 __tcm_heap_buffer_start__[];
extern u8 __tcm_heap_buffer_size__[];
#endif

extern u8 __git_ver_table_start__[];
extern u8 __git_ver_table_end__[];

enum _BOOT_TYPE_ {
	BOOT_FROM_FLASH = 0,
	BOOT_FROM_USB  = 1,
	BOOT_FROM_SDIO   = 2,
	BOOT_FROM_FLASH1 = 3,
	BOOT_FROM_UART0   = 4,
	BOOT_FROM_UART1   = 5,
	BOOT_FROM_SPI   = 6,
	BOOT_FROM_RSVD  = 7,
};

typedef struct {
	u32 VAddrStart;
	u32 VAddrEnd;
	u32 PAddrStart;
	u32 PAddrEnd;
} MMU_ConfDef;


#define OTA_ID1				0
#define OTA_ID2				1
#define OTA_CNT				2

#define BOOT_FROM_OTA1		OTA_ID1
#define BOOT_FROM_OTA2		OTA_ID2

#define OTA_INDEX_1			0
#define OTA_INDEX_2			1

typedef struct {
	u32 BaseAddr; /*start address for RSIP , should be 4KB aligned*/
	u32 EndAddr; /*end address for RSIP , should be 4KB aligned , EndAddr = BaseAddr + len - 4K*/
	u32 Type; /* OTF type: ctr or mix */
	u32 KeySize; /* OTF KeySize */
	u32 IVNum; /* OTF KeySize */
} RSIP_OTFDef;


typedef struct {
	u32	USBPLL_CLK;
	u32	SYSPLL_CLK;
	u8	Vol_Type;
	u8	CPU_CKD;/* Cock of CPU0/CPU1/SRAM/ROM are same.*/
} SocClk_Info_TypeDef;

typedef struct {
	void (*rdp_decrypt_func)(u32 addr, u8 *key, u8  cnt, u8 *buf);
	u32 psram_s_start_addr;
} BOOT_EXPORT_SYMB_TABLE;

typedef struct {
	void (*RamStartFun)(void);
	void (*RamWakeupFun)(void);
	u32 VectorNS;
} RAM_START_FUNCTION, *PRAM_START_FUNCTION;

typedef struct _RAM_FUNCTION_START_TABLE_ {
	void (*RamStartFun)(void);
	void (*RamWakeupFun)(void);
	void (*RamPatchFun0)(void);
	void (*RamPatchFun1)(void);
	void (*RamPatchFun2)(void);
	void (*FlashStartFun)(void);
	u32 Img1ValidCode;
	BOOT_EXPORT_SYMB_TABLE *ExportTable;
} RAM_FUNCTION_START_TABLE, *PRAM_FUNCTION_START_TABLE;

typedef struct _DSLP_RETENTION_FUNC_TABLE_ {
	void (*DSLPPatchFun0)(void);
	u32	PatchLen;
} DSLP_RETENTION_FUNC_TABLE, *PDSLP_RETENTION_FUNC_TABLE;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define NS_ENTRY    __attribute__((cmse_nonsecure_entry))
typedef void __attribute__((cmse_nonsecure_call)) nsfunc(void);
#endif

typedef u8(*FuncPtr)(void);

extern void SysTick_Handler(void);
extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void BOOT_ROM_OSC131_Enable(void);
extern void BOOT_ROM_LoaderEntry(void);
extern void BOOT_ROM_FromHCI(void);

extern u32 REG_CONFIG_PARSE(u8 index, u32 pwr_cmd_addr);
extern void BOOT_Image1(void);
extern void BOOT_RccConfig(void);

#define AP_BOOT_INDEX				0x01010101
#define NP_BOOT_INDEX				0x02020202

#define IS_FLASH_ADDR(addr)			(((addr) >= 0x00200000) && ((addr) <= 0x1FFFFFFF))

_Static_assert(sizeof(void *) == 4, "Cortex-M build requires 32-bit pointers");
_Static_assert(sizeof(size_t) == 4, "Cortex-M build assumes 32-bit size_t");
_Static_assert(BOOT_LOADER_LIMIT > BOOT_LOADER_START, "BOOT range must be valid");
_Static_assert(FULLMAC_ADDR_END > FULLMAC_ADDR_START, "FULLMAC range must be valid");

/**
 * @brief   Validates if a memory buffer is strictly contained within a specified region.
 *
 * @details This function performs a secure check to ensure that the buffer starting
 *          at 'addr' with length 'len' resides completely within the memory region
 *          defined by [region_start, region_limit).
 *
 *          The check is implemented to be immune to integer overflow attacks.
 *          Instead of calculating (addr + len), which may wrap around, it calculates
 *          the remaining space (limit - addr) and compares it with 'len'.
 *
 * @param[in] addr          The starting address of the buffer to check.
 * @param[in] len           The length of the buffer in bytes.
 * @param[in] region_start  The valid starting address of the memory region (inclusive).
 * @param[in] region_limit  The end address of the memory region (exclusive).
 *
 * @return  true   The buffer is valid and strictly inside the region.
 * @return  false  The buffer overlaps the boundary, is outside the region,
 *                 or the parameters caused a logic error (e.g., addr >= limit).
 */
__STATIC_FORCEINLINE bool is_memory_range_valid(u32 addr, size_t len,
		u32 region_start, u32 region_limit)
{
	size_t remaining_space = region_limit - addr;

	/* Check if the address starts before the region allows. */
	if ((addr < region_start) || (addr >= region_limit)) {
		return false;
	}

	/* Secure Length Check:
	 *    Calculate the remaining valid space available from 'addr' to 'region_limit'.
	 *    Since we verified (addr < region_limit) in step 2, (region_limit - addr)
	 *    is guaranteed not to underflow and will result in a positive size.
	 *
	 * If the requested length is greater than the remaining space, it's an overflow.
	 * zero len is not allowed*/
	if ((len == 0) || (len > remaining_space)) {
		return false;
	}

	return true;
}

/* ========================================================================== */
/*                               MACROS                                       */
/* ========================================================================== */

/**
 * @brief Checks if the source buffer is within the BOOT RAM region.
 *
 * @param src  Source address (cast to u32 automatically).
 * @param len  Length of the buffer (cast to size_t automatically).
 * @return true if valid, false otherwise.
 */
#define RANGE_IS_BOOTRAM(src, len) \
    is_memory_range_valid((u32)(src), (size_t)(len), \
                          (u32)BOOT_LOADER_START, (u32)BOOT_LOADER_LIMIT)

/**
 * @brief Checks if the source buffer is within the FULLMAC region.
 *
 * @note  This macro applies the TZ_IDAU_SEC_OFFSET to the source address
 *        before performing the range check, consistent with TrustZone logic.
 *
 * @param src  Source address.
 * @param len  Length of the buffer.
 * @return true if valid, false otherwise.
 */
#define RANGE_IS_FULLMAC(src, len) \
    is_memory_range_valid(((u32)(src) | (u32)TZ_IDAU_SEC_OFFSET), (size_t)(len), \
                          (u32)FULLMAC_ADDR_START, (u32)FULLMAC_ADDR_END)

/**
 * @brief Checks if the source buffer is within the Flash Loader region.
 *
 * @note  This macro applies the TZ_IDAU_SEC_OFFSET to the source address
 *        before performing the range check, consistent with TrustZone logic.
 *
 * @param src  Source address.
 * @param len  Length of the buffer.
 * @return true if valid, false otherwise.
 */
#define RANGE_IS_FLOADER(src, len) \
    is_memory_range_valid(((u32)(src) | (u32)TZ_IDAU_SEC_OFFSET), (size_t)(len), \
                          (u32)LOADER_DOWNLOAD_ADDR, (u32)SRAM_END_ADDRESS_S)

#ifdef __cplusplus
}
#endif

#endif   //_AMEBA_BOOT_H_
