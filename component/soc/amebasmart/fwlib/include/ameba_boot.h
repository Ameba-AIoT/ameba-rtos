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

extern u8 __bdram_heap_buffer_start__[];
extern u8 __bdram_heap_buffer_size__[];

extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __rom_bss_start_s__[];
extern u8 __rom_bss_end_s__[];
extern u8 __rom_bss_start_ns__[];
extern u8 __rom_bss_end_ns__[];
extern u8 __ram_image3_start__[];
extern u8 __ram_image3_end__[];
extern u8 __psram_image3_start__[];
extern u8 __psram_image3_end__[];

extern u8 __flash_text_start__[];
extern u8 __flash_img2_end__[];
extern u8 __flash_sec_text_start__[];
extern u8 __ram_start_table_start__[];
extern u8 __rom_top_4k_start_[];
extern u8 __km4_boot_text_start__[];

extern u8  __rom_entry_ns_start__[];

extern u8 __retention_entry_func__[];

extern u8 __km0_flash_text_start__[];
extern u8 __km0_ipc_memory_start__[];
extern u8 __km4_flash_text_start__[];
extern u8 __km0_image2_entry_func__[];
extern u8 __km4_image2_entry_func__[];
extern u8 __km4_audio_buf_start__[];
extern u8 __ca32_fip_dram_start__[];
extern u8 __ca32_flash_text_start__[];
extern u8 __ca32_bl1_dram_start__[];

extern u8 __psram_heap_buffer_start__[];
extern u8 __psram_heap_buffer_size__[];
extern u8 __psram_heap_extend_start__[];
extern u8 __psram_heap_extend_size__[];
extern u8 __non_secure_psram_end__[]; /* if psram is 8MB, than write 0x60800000 will write 0x60000000 */

/* sym for stdlib rom */
extern u8 __rom_stdlib_bss_start__[];
extern u8 __rom_stdlib_bss_end__[];
extern u8 __rom_stdlib_data_start__[];
extern u8 __rom_stdlib_data_end__[];
extern u8 __rom_stdlib_text_start__[];
extern u8 __rom_stdlib_text_end__[];

extern u8 __git_ver_table_start__[];
extern u8 __git_ver_table_end__[];

enum _BOOT_TYPE_ {
	BOOT_FROM_FLASH = 0,
	BOOT_FROM_SDIO  = 1,
	BOOT_FROM_USB   = 2,
	BOOT_FROM_UART0   = 3,
	BOOT_FROM_UART1   = 4,
	BOOT_FROM_SPI   = 5,
	BOOT_FROM_RSVD  = 6,
};


/* stdlib export to normal rom */
typedef struct {
	void (*stdlib_init)(void);
	void (*stdlib_test)(void);
} STDLIB_ENTRY_TABLE;


/* security boot */
typedef struct {

	void (*clear_ns_rom_bss)(void);

	//export to stdlib rom
	void (*loguart_putchar)(u8 c);
	u8(*loguart_getchar)(bool PullMode);
	u32(*diagprintf)(const char *fmt, ...);
} ROM_SECURE_CALL_NS_ENTRY;

typedef struct {
	u32 address; /*!< Specifies the register going to config.
			0xFFFFFFFF means end of the register config sequence */

	u32 cmd;/*!<  Bit0~ Bit7 Specifies the read/write/polling/delay/end command.
			Bit8~Bit12 specifies the config sequence index range 0x0~0x1E, 0x1F means all allowed.
			BIT13 means whether need skip this cmd, Delay CMD won't care this bits.
			BIT14~ BIT31 rsvd*/

	u32 bitsc; /* bit mask clear or polling target */
	u32 bitss; /* bit mask set or polling mask */
} REG_CONFIG_SEQ;

enum ROM_PATCH_CMD {
	ROM_PATCH_CMD_INVALID = 0x00,
	ROM_PATCH_CMD_READ = 0xFA,
	ROM_PATCH_CMD_WRITE  = 0xFB,
	ROM_PATCH_CMD_POLLING   = 0xFC,
	ROM_PATCH_CMD_DELAY   = 0xFD,
	ROM_PATCH_CMD_LOGE   = 0xFE,
	ROM_PATCH_CMD_END   = 0xFF,
};

typedef struct {
	u32 address; /*!< Specifies the register going to config.
			0xFFFFFFFF means end of the register config sequence */

	u8 rsvd; /* reserved for further use */
	u8 cmd;/*specifies the cmd */
	u16 index; /* specifies the config sequence index  */

	u32 bitsc; /* bit mask clear or polling target */
	u32 bitss; /* bit mask set or polling mask */
} _PACKED_ ROM_PATCH_TABLE;

typedef struct {
	u32 VAddrStart;
	u32 VAddrEnd;
	u32 PAddrStart;
	u32 PAddrEnd;
} MMU_ConfDef;

typedef struct {
	u32 func;
	u32 clk;
	u32 state;
} RCC_ConfDef;

typedef struct {
	u32 rst;
	u32 state;
} GRST_ConfDef;

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
	u32	NPPLL_CLK;
	u32	APPLL_CLK;
	u32	Vol_Type;
	u32	KM4_CPU_CKD;
	u32	AP_CPU_CKD;
} SocClk_Info_TypeDef;

#define TCM_SIZE_0KB			((u32)0x00000000)
#define TCM_SIZE_16KB			((u32)0x00000001)
#define TCM_SIZE_32KB			((u32)0x00000002)
#define TCM_SIZE_48KB			((u32)0x00000003)
#define TCM_SIZE_64KB			((u32)0x00000004)
#define TCM_SIZE_80KB			((u32)0x00000005)
#define TCM_SIZE_96KB			((u32)0x00000006)

typedef struct {
	u32 TCM_Size;
	u32 ICACHE_EN;
	u32 DCACHE_EN;
	u32 IWRR;
	u32 DWRR;
	u32 TCM_BASE;
	u32 TCM_TOP;
} CACHETCM_TypeDef;

typedef struct {
	u32 WRR_EN;
	u32 WRR;
	u32 WRR_BASE;
	u32 WRR_TOP;
} CACHEWRR_TypeDef;

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

#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define NS_ENTRY __attribute__((cmse_nonsecure_entry))
typedef void __attribute__((cmse_nonsecure_call)) nsfunc(void);
#endif

typedef u8(*FuncPtr)(void);

extern void SysTick_Handler(void);
extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void BOOT_ROM_InitDebugFlg(void);
extern void BOOT_ROM_OSC131_Enable(void);

extern u32 REG_CONFIG_PARSE(u8 index, u32 pwr_cmd_addr);
extern void BOOT_Image1(void);
extern void BOOT_WakeFromPG(void);
extern void BOOT_RccConfig(void);
extern PRAM_START_FUNCTION BOOT_SectionInit(void);

extern RAM_START_FUNCTION Img2EntryFun0; //RamWakeupFun

extern u32 OTA_Region[3][2];
extern u32 HUK_Derive_En;

#define DDR_AUTOGATING		ENABLE

#define BOOT_FROM_OTA1		0
#define BOOT_FROM_OTA2		1

#define VOL_09					1
#define VOL_10					3

#define KM4_09V_CLK_LIMIT		250
#define KM4_1P0V_CLK_LIMIT		333

#define AP_09V_CLK_LIMIT		920
#define AP_1P0V_CLK_LIMIT_PSRAM		1200
#define AP_1P0V_CLK_LIMIT_DDR		1320

#define SPIC_CLK_LIMIT			(208 * MHZ_TICK_CNT)	/* For Flash run up to 104MHz */

#define IS_FLASH_ADDR(addr)			((addr >= SPI_FLASH_BASE) && (addr <= 0x0FFFFFFF))
#define IS_BOOT_ADDR(addr)			((addr >= HS_BOOT_ADDR_START) && (addr <= HS_BOOT_ADDR_END))
#define IS_HS_SRAM_S_ADDR(addr)		((addr >= HS_SRAM_S_ADDR_START) && (addr <= HS_SRAM_S_ADDR_END))
#define IS_LS_SRAM_ADDR(addr)		((addr >= LS_SRAM_ADDR_START) && (addr <= LS_SRAM_ADDR_END))

#endif   //_AMEBA_BOOT_H_
