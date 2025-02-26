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
extern u8 __sbss_start__[];
extern u8 __sbss_end__[];
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
extern u8 __km4_image3_flash_start__[];
extern u8 __km4_audio_buf_start__[];

extern u8 __bdram_heap_buffer_start__[];
extern u8 __bdram_heap_buffer_size__[];
extern u8 __psram_heap_buffer_start__[];
extern u8 __psram_heap_buffer_size__[];
extern u8 __psram_heap_extend_start__[];
extern u8 __psram_heap_extend_size__[];

/* sym for stdlib rom */
extern u8 __rom_stdlib_bss_start__[];
extern u8 __rom_stdlib_bss_end__[];
extern u8 __rom_stdlib_data_start__[];
extern u8 __rom_stdlib_data_end__[];
extern u8 __rom_stdlib_text_start__[];
extern u8 __rom_stdlib_text_end__[];


/* sym for trustzone config */
extern u8 __km4_tz_nsc_start__[];
extern u8 __km4_tz_entry_start__[];
extern u8 __km4_tz_entry_end__[];
extern u8 __km4_bd_ram_start__[];

extern u8 __km4_psram_tz_nsc_start__[];
extern u8 __km4_psram_tz_entry_start__[];
extern u8 __km4_psram_tz_entry_end__[];
extern u8 __km4_bd_psram_start__[];
extern u8 __non_secure_psram_end__[]; /* if psram is 8MB, than write 0x60800000 will write 0x60000000 */

extern u8 __git_ver_table_start__[];
extern u8 __git_ver_table_end__[];

/* mp used */
extern u8 __km0_bd_ram_mp_start__[];
extern u8 __km0_image2_entry_func_mp__[];

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

/* according to customer's need */
typedef struct {
	//export to stdlib rom
	int(*loguart_putchar)(char *ptr, int len);
	int(*loguart_getchar)(char *ptr, int len);
} ROM_CUSTOMER_LOGUART;

enum _REG_CONFIG_SEQ_ {
	/*use these command means do not care the index,
		use these cmd if do not care excute location*/
	CPU_PWRSEQ_CMD_READ = 0xFFFFFF00,
	CPU_PWRSEQ_CMD_WRITE  = 0xFFFFFF01,
	CPU_PWRSEQ_CMD_POLLING   = 0xFFFFFF02,
	CPU_PWRSEQ_CMD_DELAY   = 0xFFFFFF03,
	CPU_PWRSEQ_CMD_LOGE   = 0xFFFFFF08,
	CPU_PWRSEQ_CMD_END   = 0xFFFFFFFF,

	/*newly add command,use if care the seq excute location*/
	REG_CONFIG_CMD_READ = 0x00,
	REG_CONFIG_CMD_WRITE  = 0x01,
	REG_CONFIG_CMD_POLLING   = 0x02,
	REG_CONFIG_CMD_DELAY   = 0x03,
	REG_CONFIG_CMD_LOGE   = 0x08,
	REG_CONFIG_CMD_END   = 0xFF,
};

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
	u32	PLL_CLK;
	u8	Vol_Type;
	u8	KM4_CKD;/* KM4/SRAM is the same clock */
	u8	KM0_CKD;
	u8	PSRAMC_CKD;
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

#define IS_FLASH_ADDR(addr)			((addr >= SPI_FLASH_BASE) && (addr <= 0x0FFFFFFF))
#define IS_BOOT_ADDR(addr)			((addr >= HS_BOOT_ADDR_START) && (addr <= HS_BOOT_ADDR_END))
#define IS_HS_SRAM_S_ADDR(addr)		((addr >= HS_SRAM_S_ADDR_START) && (addr <= HS_SRAM_S_ADDR_END))
#define IS_LS_SRAM_ADDR(addr)		((addr >= LS_SRAM_ADDR_START) && (addr <= LS_SRAM_ADDR_END))

#endif   //_AMEBA_BOOT_H_
