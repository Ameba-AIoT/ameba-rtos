/* auto gen, please change AmebaLite_memory_layout.xlsx and exec gen_memory_layout.py */
#include "project_km4/platform_autoconf.h"
/* constant macro */
#define SECURE_ADDR_OFFSET 			(0x10000000)
#define KBYTES(x)  					((x) * 1024)
#define IMAGE_HEADER_LEN			(0x20)
#define KR4_KM4_RAM_SIZE			KBYTES(40)
#define SRAM_BASE					(0x20000000)
#define SRAM_END					(0x20080000)
#define PSRAM_BASE					(0x60000000)
#define PSRAM_END					(0x60800000)

/* layout configuration */
/* TZ region sized by CONFIG_TZ_S_SIZE, default 64K. */
#if defined(CONFIG_IMG3_SRAM)
#define KM4_RAM_TZ_SIZE				KBYTES(CONFIG_TZ_S_SIZE)
#define KM4_PSRAM_TZ_SIZE			KBYTES(0)
#elif defined(CONFIG_IMG3_PSRAM)
#define KM4_RAM_TZ_SIZE				KBYTES(0)
#define KM4_PSRAM_TZ_SIZE			KBYTES(CONFIG_TZ_S_SIZE)
#else /* undefine trustzone */
#define KM4_RAM_TZ_SIZE				KBYTES(0)
#define KM4_PSRAM_TZ_SIZE			KBYTES(0)
#endif

/* IMG1 Size */
#if defined(CONFIG_IMG1_FLASH)
#define KM4_IMG1_SIZE 				KBYTES(4)
#else
#define KM4_IMG1_SIZE 				KBYTES(28)
#endif
/*
                   AmebaLite SRAM Layout(512KB)
0x20000000 ───────────┬──────────────────────┐
                      │    KR4/KM4 RAM       │
                      │      (40K)           │
                      │  <RealTek  FIXED>    │
                      │                      │
0x20004C00 ──────────►├────────────────────┐ │
                      │KR4 BootEntry backup│ │
0x20004C40 ──────────►├────────────────────┘ │
                      │                      │
0x2000A000 ──────────►├──────────────────────┼─┐
                      │ KM4 Bootloader(IMG1) │ ├─────►  KM4_IMG1_SIZE
                      │                      │ │
                  ┌───┼──────────────────────┼─┴┐
If Trustzone off, │   │ KM4 Trustzone(IMG3)  │  │     If Trustzone on,
                  │   │                      │  │
           ◄──────┤   │      TZ Region       │  ├─────► IMG3 SIZE = KM4_RAM_TZ_SIZE = KBYTES(CONFIG_TZ_S_SIZE)
IMG3 size is 0.   │   │       (64K)          │  │
                  └───┼──────────────────────┼──┴─┐
                      │                      │    │
                      │    KM4 IMG2 RAM      │    │
                      │                      │    │
0x20040020 ──────────►├──────────────────────┤    ├────► RAM_KM4_IMG2_SIZE
                      │   KR4 Boot Entry     ├────┼────┐
0x20040040 ──────────►├──────────────────────┤    │    │
                      │                      │    │    └─► Fix addr in rom of KR4
                      ├──────────────────────┼────┴┐
                      │                      │     │
                      │    KR4 IMG2 RAM      │     │
                      │                      │     ├───► RAM_KR4_IMG2_SIZE
                      │                      │     │
                      │                      │     │
0x20080000 ──────────►└──────────────────────┴─────┘
 */
/* BOOT(IMG1)/IMG3/IMG2 layout*/													/* Trustzone on			   | Trustzone off	*/
#define KM4_IMG1_RAM_SATRT   		(SRAM_BASE + KR4_KM4_RAM_SIZE)					/* IMG1 On SRAM: 0x2000A000 ~ 0x20011400 | IMG1 On FLASH: 0x2000A000 ~ 0x2000D000*/
#define KM4_RAM_TZ_START			(KM4_IMG1_RAM_SATRT + KM4_IMG1_SIZE )			/* TZ region start: 0x20011400 ~ 0x20021400 | unavailable */

/* IMG2 Size*/
#if defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)
#define RAM_KR4_IMG2_SIZE			KBYTES(96)
#define KM4_IMG2_RAM_START			(KM4_RAM_TZ_START + KM4_RAM_TZ_SIZE)			/* 0x20021400 ~ 0x20068000 | 0x2000D000 ~ 0x20068000*/
#define KR4_IMG2_RAM_START			(SRAM_END - RAM_KR4_IMG2_SIZE) 					/* 0x20068000 ~ 0x20078000 | 0x20068000 ~ 0x20078000*/
#define RAM_KM4_IMG2_SIZE			(KR4_IMG2_RAM_START - KM4_IMG2_RAM_START)
#else
#define RAM_KM4_IMG2_SIZE			KBYTES(96)
#define KM4_IMG2_RAM_START			(KM4_RAM_TZ_START + KM4_RAM_TZ_SIZE)			/* 0x20021400 ~ 0x20039400 | 0x2000D000 ~ 0x20025000*/
#define KR4_IMG2_RAM_START			(KM4_IMG2_RAM_START + RAM_KM4_IMG2_SIZE) 		/* 0x20039400 ~ 0x20078000 | 0x20025000 ~ 0x20078000*/
#define RAM_KR4_IMG2_SIZE			(SRAM_END - KR4_IMG2_RAM_START)
#endif

/* IMG3 On PSRAM */
#define KM4_PSRAM_TZ_START			(PSRAM_BASE)									/* TZ region start: 0x60000000 ~ 0x60010000 | unavailable */
#define PSRAM_DSP_START				(0x60300000)

#define PSRAM_KM4_IMG2_SIZE			(KBYTES(1536) - KM4_PSRAM_TZ_SIZE)
#define KM4_IMG2_PSRAM_START		(KM4_PSRAM_TZ_START + KM4_PSRAM_TZ_SIZE)		/* 0x60010000 ~ 0x60180000 | 0x60000000 ~ 0x60180000*/
#define KR4_IMG2_PSRAM_START		(KM4_IMG2_PSRAM_START + PSRAM_KM4_IMG2_SIZE) 	 	/* 0x60180000 ~ 0x60300000 | 0x60180000 ~ 0x60300000*/
#define PSRAM_KR4_IMG2_SIZE			(PSRAM_DSP_START - KR4_IMG2_PSRAM_START)

MEMORY
{
	/* KM4 ROM */
	KM4_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0001E000 - 0x00000000	/* KM4 IROM_S: 120k */
	KM4_IROM_NS (rx) :                      ORIGIN = 0x0001E000, LENGTH = 0x00034000 - 0x0001E000	/* KM4 IROM_NS: 88k */
	KM4_IROM_STDLIB_NS (rx) :               ORIGIN = 0x00034000, LENGTH = 0x00041000 - 0x00034000	/* KM4 STDLIB ROM: 52k */
	KM4_DROM_NS (rx) :                      ORIGIN = 0x00041000, LENGTH = 0x00045000 - 0x00041000	/* KM4 DROM_NS: 16k */
	KM4_DROM (rx) :                         ORIGIN = 0x00045000, LENGTH = 0x00048000 - 0x00045000	/* KM4 DROM_S: 12k */

	/* KM4 Secure RAM, BIT(28)=1 */
	KM4_ROMBSS_RAM_S (rw) :                 ORIGIN = 0x30007000, LENGTH = 0x30008000 - 0x30007000	/* KM4 ROM BSS RAM S: 4k */
	KM4_MSP_RAM_S (rw) :                    ORIGIN = 0x30008000, LENGTH = 0x3000A000 - 0x30008000	/* KM4 MSP_S RAM: 8k */
	KM4_BOOTLOADER_RAM_S (rwx) :            ORIGIN = KM4_IMG1_RAM_SATRT + SECURE_ADDR_OFFSET + IMAGE_HEADER_LEN, LENGTH = KM4_IMG1_SIZE  - IMAGE_HEADER_LEN		/* KM4 BOOT Loader RAM: 4K or 24k */
	KM4_FLOADER_RAM_S (rwx) :               ORIGIN = 0x3000A020, LENGTH = 0x30020000 - 0x3000A020	/* KM4 ImgTool Flash Loader RAM: 88k */

	/* KM4 Non-secure RAM */
	KM4_MSP_RAM_NS (rw) :                   ORIGIN = 0x20003000, LENGTH = 0x20004000 - 0x20003000	/* KM4  MSP_NS RAM: 4k */
	KM4_ROMBSS_RAM_COM (rw) :               ORIGIN = 0x20004000, LENGTH = 0x20005000 - 0x20004000	/* KM4  ROM BSS COMMON(S & NS both used) RAM: 4k */
	KM4_ROMBSS_RAM_NS (rw) :                ORIGIN = 0x20005000, LENGTH = 0x20005FBF - 0x20005000	/* KM4 ROM BSS NS RAM: 4k */
	KM4_BOOT_RAM (rwx) :                    ORIGIN = 0x20005FE0, LENGTH = 0x20006000 - 0x20005FE0	/* KM4 BOOT RAM: 64B */
	KM4_TIMER_IDLE_TASK_STACK (rw) :        ORIGIN = 0x20006000, LENGTH = 0x20007000 - 0x20006000	/* KM4 TIMER and IDLE task stack: 4k */

	/* PMC RAM, fixed in memory layout, do not changed */
	KR4_IPC_RAM (rwx) :                     ORIGIN = 0x20004900, LENGTH = 0x20004C00 - 0x20004900	/* KR4 IPC RAM: 768B */
	KR4_BOOT_RAM_BACKUP (rwx) :             ORIGIN = 0x20004C20, LENGTH = 0x20004C40 - 0x20004C20	/* KR4 BOOT RAM: 64B, Backup KR4 BootEntry.bin(32B Header+32B Entry)*/
	KR4_RAM_ONLY (rwx) :                    ORIGIN = 0x20005520, LENGTH = 0x20005900 - 0x20005520	/* KR4 PMC RAM: 1K */
	KM4_RAM_ONLY (rwx) :                    ORIGIN = 0x20005920, LENGTH = 0x20005D20 - 0x20005920	/* KM4 PMC RAM: 1K */
	KR4_BOOT_RAM (rwx) :                    ORIGIN = 0x20040020, LENGTH = 0x20040040 - 0x20040020	/* KR4 BOOT RAM: 32B, Can be ignore When DSP use this addr */

	/* IMG2 RAM */
	KM4_BD_RAM_TZ (rwx) :                   ORIGIN = KM4_RAM_TZ_START + IMAGE_HEADER_LEN, 		LENGTH = KM4_RAM_TZ_SIZE - IMAGE_HEADER_LEN		/* KM4 BD RAM TZ: TZ Off,0;TZ On,64K (NS-alias, SAU marks secure) */

	KM4_BD_RAM (rwx) :                      ORIGIN = KM4_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = RAM_KM4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KM4 BD RAM */
	KR4_BD_RAM (rwx) :                      ORIGIN = KR4_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = RAM_KR4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KR4 BD RAM */

	/* KM4 Non-secure PSRAM */
	KM4_BD_PSRAM_TZ (rwx) :                 ORIGIN = KM4_PSRAM_TZ_START + IMAGE_HEADER_LEN, 	LENGTH = KM4_PSRAM_TZ_SIZE - IMAGE_HEADER_LEN	/* KM4 BD PSRAM TZ: 64k (NS-alias, SAU marks secure) */

	KM4_BD_PSRAM (rwx) :                    ORIGIN = KM4_IMG2_PSRAM_START + IMAGE_HEADER_LEN, 	LENGTH = PSRAM_KM4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KM4 MAIN RAM NS : TZ On,1472K;TZ Off, 1536K */
	KR4_BD_PSRAM (rwx) :                    ORIGIN = KR4_IMG2_PSRAM_START + IMAGE_HEADER_LEN, 	LENGTH = PSRAM_KR4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KR4 BD PSRAM: 1536k */
	DSP_BD_PSRAM (rwx) :                    ORIGIN = PSRAM_DSP_START + IMAGE_HEADER_LEN, 	  	LENGTH = PSRAM_END - PSRAM_DSP_START - IMAGE_HEADER_LEN	/* DSP BD PSRAM: 5120k */
	KM4_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KM4 PSRAM HEAP EXT */
	KR4_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KR4 PSRAM HEAP EXT */

	/* Retention RAM */
	RETENTION_RAM (rwx) :                   ORIGIN = 0x4100CE00, LENGTH = 0x4100D000 - 0x4100CE00	/* KR4 Retention SRAM: 512B: 0k */

	/* KR4 ROM */
	KR4_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0001C000 - 0x00000000	/* KR4 IROM: 112k */
	KR4_IROM_STDLIB (rx) :                  ORIGIN = 0x0001C000, LENGTH = 0x0002D000 - 0x0001C000	/* KR4 STDLIB ROM: 68k */
	KR4_DROM (rx) :                         ORIGIN = 0x0002D000 , LENGTH = 0x00030000 - 0x0002D000 	/* KR4 DROM: 12k */

	/* KR4 RAM */
	KR4_ROMBSS_RAM (rw) :                   ORIGIN = 0x20000000, LENGTH = 0x20001000 - 0x20000000	/* KR4 ROM BSS RAM: 4k */
	KR4_MSP_RAM (rw) :                      ORIGIN = 0x20001000, LENGTH = 0x20002000 - 0x20001000	/* KR4 MSP RAM: 4k */
	KR4_TIMER_IDLE_TASK_STACK (rw) :        ORIGIN = 0x20002000, LENGTH = 0x20003000 - 0x20002000	/* KR4 TIMER and IDLE task stack: 4k */

	/* Flash */
	KR4_IMG2_XIP (rx) :                     ORIGIN = 0x0C000020, LENGTH = 0x0D000000 - 0x0C000020	/* KR4 IMG2 Virtual address: 32768k */
	DSP_IMG2_XIP (rx) :                     ORIGIN = 0x0D000020, LENGTH = 0x0E000000 - 0x0D000020	/* DSP IMG2 Virtual address: 24576k */
	KM4_IMG2_XIP (rx) :                     ORIGIN = 0x0E000020, LENGTH = 0x0F800000 - 0x0E000020	/* KM4 IMG2 Virtual address: 24576k */
	KM4_BOOT_XIP (rx) :                     ORIGIN = 0x0F800020, LENGTH = 0x10000000 - 0x0F800020	/* KM4 Bootloader Virtual address: 8192k */
	BTRACE (rx) :                           ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000	/* BT LOG Virtual address: 8192k */
	CTRACE (rx) :                           ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000	/* COEX LOG Virtual address: 8192k */

	/* DSP ROM */
	DSP_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x00000400 - 0x00000000	/* KM0 IROM: 1k */

	/* mp use */
	KM4_FLOADER_RAM_MP (rwx) :               ORIGIN = 0x3000A020, LENGTH = 0x30012000 - 0x3000A020	/* KM4 ImgTool Flash Loader RAM: 32k */

	KM4_BOOTLOADER_ENTRY (rwx) :             ORIGIN = 0x3000A020, LENGTH = 0x3000A400 - 0x3000A020	/* KM4 IMG1 Entry: 1024B aligned for flashloader */
	KM4_BOOTLOADER_RAM_MP (rwx) :            ORIGIN = 0x30013400 + IMAGE_HEADER_LEN, LENGTH = 0x3001A000 - 0x30013400 - IMAGE_HEADER_LEN	/* KM4 BootLoader RAM: 32k */

#if defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)
	KR4_BD_RAM_MP (rwx) :                   ORIGIN = 0x2001A060, LENGTH = 0x20061F80 - 0x2001A060 /* KR4 BD RAM: 288k */
	KR4_BD_RAM_MP_END (rwx) :               ORIGIN = 0x20061F80 - 32, LENGTH = 32                 /* Only for KR4 shrink MP image2 */
	KM4_BD_RAM_MP (rwx) :                   ORIGIN = 0x20062060, LENGTH = 0x2007FFA0 - 0x20062060 /* KM4 BD RAM: TZ off,120k */
#else
	KR4_BD_RAM_MP (rwx) :                   ORIGIN = 0x2001A060, LENGTH = 0x20041F80 - 0x2001A060 /* KR4 BD RAM: TZ off,160K */
	KM4_BD_RAM_MP (rwx) :                   ORIGIN = 0x20042060, LENGTH = 0x2007FFA0 - 0x20042060 /* KM4 BD RAM: 248k */
#endif
}

__kr4_image2_entry_func__ = ORIGIN(KR4_BD_RAM);
__kr4_ipc_memory_start__ = ORIGIN(KR4_IPC_RAM);
__kr4_flash_text_start__ = ORIGIN(KR4_IMG2_XIP);
__km4_flash_text_start__ = ORIGIN(KM4_IMG2_XIP);
__dsp_flash_text_start__ = ORIGIN(DSP_IMG2_XIP);
__kr4_psram_text_start__ = ORIGIN(KR4_BD_PSRAM);
__km4_boot_text_start__ = ORIGIN(KM4_BOOT_XIP);

__kr4_start_table_backup_addr__ = ORIGIN(KR4_BOOT_RAM_BACKUP);
__kr4_boot_entry_addr__ = ORIGIN(KR4_BOOT_RAM);

/*SRAM Layout boundary*/
/* TZ region symbols (__image3_ram_start__/end, __ram_image3_nsc_start__/end) are defined in img3 link script */
__km4_bd_ram_start__ = ORIGIN(KM4_BD_RAM) - IMAGE_HEADER_LEN;

/*PSRAM Layout boundary*/
__km4_bd_psram_start__ = ORIGIN(KM4_BD_PSRAM) - IMAGE_HEADER_LEN;
__non_secure_psram_end__ = PSRAM_END;

__ap_sram_heap_start = SRAM_END;
__ap_sram_heap_size = KBYTES(256);

/* Region 0: KM4 IROM*/
__rom_text_region0_start__ = ORIGIN(KM4_IROM);
__rom_text_region0_end__   = ORIGIN(KM4_IROM_STDLIB_NS) + LENGTH(KM4_IROM_STDLIB_NS);

/* Region 1: KR4 IROM*/
__rom_text_region1_start__ = ORIGIN(KR4_IROM);
__rom_text_region1_end__   = ORIGIN(KR4_IROM_STDLIB) + LENGTH(KR4_IROM_STDLIB);


/* mp used */
__km4_bd_boot_download_addr__ = 0x30012000;
__kr4_bd_ram_mp_start__ = ORIGIN(KR4_BD_RAM_MP);
__km4_bd_ram_mp_start__ = ORIGIN(KM4_BD_RAM_MP);
__kr4_boot_entry2_addr__ = ORIGIN(KR4_RAM_ONLY);

/* code analyse used */
__rtk_fixed_addr__ = SRAM_BASE + KR4_KM4_RAM_SIZE;
__img1_sram_size__ = KM4_IMG1_SIZE;
__img3_sram_size__ = KM4_RAM_TZ_SIZE;

/* for sleep backup */
__km4_msp_ram_ns_limit__ = ORIGIN(KM4_MSP_RAM_NS);
