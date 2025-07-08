/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BOOT_H_
#define _AMEBA_BOOT_H_

extern u8 __image1_validate_code__[];
extern u8 __image1_bss_start__[];
extern u8 __image1_bss_end__[];

extern u8 __embedded_bin_start[];
extern u8 __embedded_bin_end[];
extern u8 __km4tz_pg_loader_ram_start__[];

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
extern u8 __ram_image2_text_end__[];

extern u8 __ipc_table_start__[];
extern u8 __ipc_table_end__[];

extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __rom_bss_start_s__[];
extern u8 __rom_bss_end_s__[];
extern u8 __rom_bss_start_ns__[];
extern u8 __rom_bss_end_ns__[];

extern u8 __ipc_memory_start__[];
extern u8 __km4tz_boot_text_start__[];
extern u8 __km4tz_boot_text_end__[];
extern u8 __km4tz_flash_text_start__[];
extern u8 __km4tz_flash_text_end__[];
extern u8 __km4ns_flash_text_start__[];
extern u8 __km4ns_flash_text_end__[];
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
extern u8 __km4tz_tz_nsc_start__[];
extern u8 __km4tz_tz_entry_start__[];
extern u8 __km4tz_tz_entry_end__[];
extern u8 __km4tz_bd_ram_start__[];

extern u8 __km4tz_psram_tz_nsc_start__[];
extern u8 __km4tz_psram_tz_entry_start__[];
extern u8 __km4tz_psram_tz_entry_end__[];
extern u8 __km4tz_bd_psram_start__[];
extern u8 __non_secure_psram_end__[]; /* if psram is 8MB, than write 0x60800000 will write 0x60000000 */

#ifdef CONFIG_WIFI_HOST_CONTROL
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
extern void BOOT_ROM_InitDebugFlg(void);
extern void BOOT_ROM_OSC131_Enable(void);
extern void BOOT_ROM_LoaderEntry(void);
extern void BOOT_ROM_FromHCI(void);

extern u32 REG_CONFIG_PARSE(u8 index, u32 pwr_cmd_addr);
extern void BOOT_Image1(void);
extern void BOOT_WakeFromPG(void);
extern void BOOT_RccConfig(void);

#define AP_BOOT_INDEX				0x01010101
#define NP_BOOT_INDEX				0x02020202

#define IS_FLASH_ADDR(addr)			((addr >= 0x00200000) && (addr <= 0x1FFFFFFF))
#define RANGE_IS_BOOTRAM(src, len)	(((src) >= BOOT_LOADER_START) && ((len) < BOOT_LOADER_LIMIT - BOOT_LOADER_START) && ((src + len) < BOOT_LOADER_LIMIT))
#define RANGE_IS_FULLMAC(src, len)	((((src) | TZ_IDAU_SEC_OFFSET) >= (FULLMAC_ADDR_START | TZ_IDAU_SEC_OFFSET)) && ((len) < FULLMAC_ADDR_END - FULLMAC_ADDR_START) && \
									(((src + len) | TZ_IDAU_SEC_OFFSET) < (FULLMAC_ADDR_END | TZ_IDAU_SEC_OFFSET)))
#endif   //_AMEBA_BOOT_H_

