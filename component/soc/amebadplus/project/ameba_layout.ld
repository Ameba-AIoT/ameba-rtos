/* auto gen, please change AmebaDplus_memory_layout.xlsx and exec gen_memory_layout.py */

#include "project_km4/platform_autoconf.h"
/* constant macro */
#define SECURE_ADDR_OFFSET 			(0x10000000)
#define KBYTES(x)  					((x) * 1024)
#define IMAGE_HEADER_LEN			(0x20)
#define KM0_KM4_RAM_SIZE			KBYTES(40)
#define SRAM_BASE					(0x20000000)
#define SRAM_END					(0x20080000)
#define PSRAM_BASE					(0x60000000)
/* PSRAM_END is the address window top = the largest PSRAM this chip supports
 * (amebadplus: 8MB). Real heap top is runtime-measured (ChipInfo_PsramHeapTop),
 * so smaller boards shrink automatically and no customer configuration is needed. */
#define PSRAM_END					(0x60800000)

/* layout configuration */
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

#if defined(CONFIG_IMG1_FLASH)
#define KM4_IMG1_SIZE 				KBYTES(4)
#elif defined(CONFIG_IMG1_PQC_SBOOT)
#define KM4_IMG1_SIZE 				KBYTES(44)
#else
#define KM4_IMG1_SIZE 				KBYTES(32)
#endif

#define RAM_KM0_IMG2_SIZE				KBYTES(96)
/*
                  amebadplus SRAM Layout (512K)
0x20000000 ──────────►┌──────────────────────┐
                      │     KM0/KM4 RAM      │
                      │       (40K)          │
                      │    <Realtek Fixed>   │
                      │                      │
fix addr              │                      │
0x2000A000 ──────────►├──────────────────────┼──┐
                      │  KM4 Bootloader(IMG1)│  ├───────►  KM4_IMG1_SIZE
                      │                      │  │
                   ┌──┼──────────────────────┼──┴─┐
                   │  │  KM4 Trustzone(IMG3) │    │
If Trustzone off,  │  │                      │    │        If Trustzone on,
              ◄────┤  │        RAM_TZ        │    ├─────►  IMG3 = KM4_RAM_TZ_SIZE
IMG3 size is 0.    │  │                      │    │
                   └──┼──────────────────────┼────┴─┐
                      │                      │      │
                      │                      │      │
                      │    KM4 IMG2 RAM      │      │
                      │                      │      ├───►  KM4_IMG2_SIZE = KM0_IMG2_RAM_START - KM4_IMG2_RAM_START
                      │                      │      │
                      │                      │      │
                      ├──────────────────────┼──────┴─┐
                      │                      │        │
                      │    KM0 IMG2 RAM      │        │
                      │                      │        ├─►  RAM_KM0_IMG2_SIZE
                      │                      │        │
                      │                      │        │
                      │                      │        │
 0x20080000──────────►└──────────────────────┴────────┘
*/
/* BOOT(IMG1)/IMG3/IMG2 layout*/												/* Trstzone On			   | Trustzone off			*/
#define KM4_IMG1_RAM_START   		(SRAM_BASE + KM0_KM4_RAM_SIZE)				/* 0x2000A000 ~ 0x20010000 | 0x2000A000 ~ 0x2000B000*/
#define KM4_RAM_TZ_START			(KM4_IMG1_RAM_START + KM4_IMG1_SIZE)			/* 0x20010000 ~ 0x20020000 | unavailable */
#define KM4_IMG2_RAM_START			(KM4_RAM_TZ_START + KM4_RAM_TZ_SIZE)	/* 0x20020000 ~ 0x20068000 | 0x2000B000 ~ 0x20068000*/
#define KM0_IMG2_RAM_START			(SRAM_END - RAM_KM0_IMG2_SIZE) 					/* 0x20068000 ~ 0x20080000 | 0x20068000 ~ 0x20080000*/

/*
                  amebadplus PSRAM Layout (window 8MB, runtime-measured capacity)
PSRAM_BASE      ──────────►┌──────────────────────┐
0x60000000                 │                      │
                        ┌──┤  KM4 BD PSRAM TZ     │
If Trustzone off,       │  │  (IMG3 on PSRAM)     ├─────►  KM4_PSRAM_TZ_SIZE = KBYTES(CONFIG_TZ_S_SIZE)
  TZ_SIZE = 0.     ◄────┤  │                      │        0 when TrustZone off or IMG3 not on PSRAM.
                        └──┤                      │
                           ├──────────────────────┤
                           │                      │
                           │   KM0 BD PSRAM       │
                           │   (NP core image2)   ├─────►  KBYTES(CONFIG_PSRAM_KM0_IMG2_KB)
                           │   Fixed size, right  │        default 876K; Kconfig-configurable.
                           │   after TZ segment   │
                           │                      │
                           ├──────────────────────┤
                           │                      │
                           │   KM4 BD PSRAM       │
                           │   (AP core image2)   ├─────►  PSRAM_END - KM4_IMG2_PSRAM_START
                           │   Fills to PSRAM_END │        (fills to 8MB window top).
                           │   window; heap top   │        Heap top = ChipInfo_PsramHeapTop()
                           │   = runtime-measured │        (runtime-measured capacity, may be
                           │   PSRAM capacity.    │        less than PSRAM_END on small boards).
                           │                      │
PSRAM_END       ──────────►└──────────────────────┘
0x60800000 (8MB)               chip max; real heap top capped to ChipInfo_PsramHeapTop()
*/

#define KM4_PSRAM_TZ_START			(PSRAM_BASE)
#define KM0_IMG2_PSRAM_START		(KM4_PSRAM_TZ_START + KM4_PSRAM_TZ_SIZE)
#define KM4_IMG2_PSRAM_START		(KM0_IMG2_PSRAM_START + KM0_PSRAM_IMG2_SIZE)

/* NP(KM0) gets a segment only when it puts code/data in PSRAM;
 * else it collapses to 0 and all non-secure PSRAM folds into AP(KM4). */
#if defined(CONFIG_IMG2_PSRAM) || defined(CONFIG_DATA_HEAP_PSRAM)
#define KM0_PSRAM_IMG2_SIZE			KBYTES(CONFIG_PSRAM_KM0_IMG2_KB)
#define KM0_BD_PSRAM_ORIGIN			(KM0_IMG2_PSRAM_START + IMAGE_HEADER_LEN)
#define KM0_BD_PSRAM_LENGTH			(KM0_PSRAM_IMG2_SIZE - IMAGE_HEADER_LEN)
#else
#define KM0_PSRAM_IMG2_SIZE			0
#define KM0_BD_PSRAM_ORIGIN			KM0_IMG2_PSRAM_START
#define KM0_BD_PSRAM_LENGTH			0
#endif

MEMORY
{
	EMPTY_SECTION (rwx) :                   ORIGIN = 0x00000000, LENGTH = 0x00000000 - 0x00000000	/* This region shall be empty */

	/* KM4 ROM */
	KM4_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0001E000 - 0x00000000	/* KM4 IROM_S: 120k */
	KM4_IROM_NS (rx) :                      ORIGIN = 0x0001E000, LENGTH = 0x00033000 - 0x0001E000	/* KM4 IROM_NS: 84k */
	KM4_IROM_STDLIB_NS (rx) :               ORIGIN = 0x00033000, LENGTH = 0x00042000 - 0x00033000	/* KM4 STDLIB ROM: 60k */
	KM4_DROM_NS (rx) :                      ORIGIN = 0x00042000, LENGTH = 0x00044800 - 0x00042000	/* KM4 DROM_NS: 10k */
	KM4_DROM (rx) :                         ORIGIN = 0x00044800, LENGTH = 0x00048000 - 0x00044800	/* KM4 DROM_S: 14k */

	/* KM4 Secure RAM, BIT(28)=1 */
	KM4_ROMBSS_RAM_S (rw) :                 ORIGIN = 0x30007000, LENGTH = 0x30008000 - 0x30007000	/* KM4 ROM BSS RAM S: 4k */
	KM4_MSP_RAM_S (rw) :                    ORIGIN = 0x30009000, LENGTH = 0x3000A000 - 0x30009000	/* KM4 MSP_S RAM: 4k */
	KM0_RTOS_STATIC_0_NS(rw):               ORIGIN = 0x20008000, LENGTH = 0x20009000 - 0x20008000	/* KM0 KM0_RTOS_STATIC_0_NS RAM: 4k */
	KM4_BOOTLOADER_RAM_S (rwx) :            ORIGIN = KM4_IMG1_RAM_START + SECURE_ADDR_OFFSET + IMAGE_HEADER_LEN, LENGTH = KM4_IMG1_SIZE  - IMAGE_HEADER_LEN		/* KM4 BOOT Loader RAM: 4K or 24k */
	KM4_FLOADER_RAM_S (rwx) :               ORIGIN = 0x3000A020, LENGTH = 0x30020000 - 0x3000A020	/* KM4 ImgTool Flash Loader RAM: 88k */

	/* KM4 Non-secure RAM */
	KM4_MSP_RAM_NS (rw) :                   ORIGIN = 0x20003000, LENGTH = 0x20004000 - 0x20003000	/* KM4  MSP_NS RAM: 4k */
	KM4_ROMBSS_RAM_COM (rw) :               ORIGIN = 0x20004000, LENGTH = 0x20005000 - 0x20004000	/* KM4  ROM BSS COMMON(S & NS both used) RAM: 4k */
	KM4_ROMBSS_RAM_NS (rw) :                ORIGIN = 0x20005000, LENGTH = 0x20005600 - 0x20005000	/* KM4 ROM BSS NS RAM: 1.5k */
	KM0_RTOS_STATIC_1_NS (rw) :             ORIGIN = 0x20005600, LENGTH = 0x20006000 - 0x20005600	/* KM0 KM0_RTOS_STATIC_1_NS NS RAM: 2.5k */
	KM4_TIMER_IDLE_TASK_STACK (rw) :        ORIGIN = 0x20006000, LENGTH = 0x20007000 - 0x20006000	/* KM4 TIMER and IDLE task stack: 4k */

	/* PMC RAM, fixed in memory layout, do not changed */
	KM0_IMG2_ENTRY (rwx) :                  ORIGIN = 0x20004D20, LENGTH = 0x20004D80 - 0x20004D20	/* KM0 IMG2 Entry(ROM Used): 128B */
	KM4_IMG2_ENTRY (rwx) :                  ORIGIN = 0x20004DA0, LENGTH = 0x20004DC0 - 0x20004DA0	/* KM4 IMG2 Entry: 64B */
	KM0_IPC_RAM (rwx) :                     ORIGIN = 0x20004E00, LENGTH = 0x20005000 - 0x20004E20	/* KM0 IPC RAM: 512B */

	/* IMG2 RAM */
	KM4_BD_RAM_TZ (rwx) :                   ORIGIN = KM4_RAM_TZ_START + IMAGE_HEADER_LEN, 		LENGTH = KM4_RAM_TZ_SIZE - IMAGE_HEADER_LEN						 /* KM4 BD RAM TZ: 64KB */
	KM4_BD_RAM (rwx) :                      ORIGIN = KM4_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = KM0_IMG2_RAM_START - KM4_IMG2_RAM_START - IMAGE_HEADER_LEN	/* KM4 BD RAM: TZ off,372K,0;TZ On,288k */
	KM0_BD_RAM (rwx) :                      ORIGIN = KM0_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = RAM_KM0_IMG2_SIZE - IMAGE_HEADER_LEN	                 /* KM0 BD RAM: TZ off,96K,0;TZ On,96k */

	KM4_BD_PSRAM_TZ (rwx) :                 ORIGIN = KM4_PSRAM_TZ_START + IMAGE_HEADER_LEN,     LENGTH = KM4_PSRAM_TZ_SIZE - IMAGE_HEADER_LEN                    /* KM4 BD PSRAM TZ: 64KB */
	/* KM4 Non-secure PSRAM */
	KM0_BD_PSRAM (rwx) :                    ORIGIN = KM0_BD_PSRAM_ORIGIN,                       LENGTH = KM0_BD_PSRAM_LENGTH                                     /* KM0 BD PSRAM: CONFIG_PSRAM_KM0_IMG2_KB (default 876K), or 0 when NP uses no PSRAM */
	KM4_BD_PSRAM (rwx) :                    ORIGIN = KM4_IMG2_PSRAM_START + IMAGE_HEADER_LEN,   LENGTH = (PSRAM_END - KM4_IMG2_PSRAM_START) - IMAGE_HEADER_LEN   /* KM4 BD PSRAM: last segment, fills to PSRAM_END; heap top capped to measured at runtime */

	/* Retention RAM */
	RETENTION_RAM (rwx) :                   ORIGIN = 0x41008E00, LENGTH = 0x41009000 - 0x41008E00	/* KM0 Retention SRAM: 512B: 0k */

	/* KM0 ROM */
	KM0_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x00006000 - 0x00000000	/* KM0 IROM: 24k */
	KM0_IROM_STDLIB (rx) :                  ORIGIN = 0x00006000, LENGTH = 0x0000A000 - 0x00006000	/* KM0 STDLIB ROM: 16k */
	KM0_DROM (rx) :                         ORIGIN = 0x0000A000, LENGTH = 0x0000B000 - 0x0000A000 	/* KM0 DROM: 4k */
	WIFI_ROM (rx) :                         ORIGIN = 0x0000B000, LENGTH = 0x00028000 - 0x0000B000 	/* KM0 DROM: 116k */

	/* KM0 RAM */
	KM0_ROMBSS_RAM (rw) :                   ORIGIN = 0x20000000, LENGTH = 0x20001000 - 0x20000000	/* KM0 ROM BSS RAM: 4k */
	KM0_MSP_RAM (rw) :                      ORIGIN = 0x20001000, LENGTH = 0x20002000 - 0x20001000	/* KM0 MSP RAM: 4k */
	KM0_TIMER_IDLE_TASK_STACK (rw) :        ORIGIN = 0x20002000, LENGTH = 0x20003000 - 0x20002000	/* KM0 TIMER and IDLE task stack: 4k */

	/* Flash */
	KM4_BOOT_XIP (rx) :                     ORIGIN = 0x0F800020, LENGTH = 0x10000000 - 0x0F800020	/* KM4 Bootloader Virtual address: 8192k */
	KM0_IMG2_XIP (rx) :                     ORIGIN = 0x0C000020, LENGTH = 0x0E000000 - 0x0C000020	/* KM0 IMG2 Virtual address: 32768k */
	KM4_IMG2_XIP (rx) :                     ORIGIN = 0x0E000020, LENGTH = 0x0F800000 - 0x0E000020	/* KM4 IMG2 Virtual address: 24576k */
	BTRACE (rx) :                           ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000	/* BT LOG Virtual address: 8192k */
	CTRACE (rx) :                           ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000	/* COEX LOG Virtual address: 8192k */

	/* mp sram temp  temp use now */
	KM4_FLOADER_RAM_MP (rwx) :               ORIGIN = 0x3000A020, LENGTH = 0x30012000 - 0x3000A020	/* KM4 ImgTool Flash Loader RAM: 32k */

	KM4_BOOTLOADER_ENTRY (rwx) :             ORIGIN = 0x3000A020, LENGTH = 0x3000A400 - 0x3000A020	/* KM4 IMG1 Entry: 1024B aligned for flashloader */
	KM4_BOOTLOADER_RAM_MP (rwx) :            ORIGIN = 0x30013400 + IMAGE_HEADER_LEN, LENGTH = 0x3001A000 - 0x30013400 - IMAGE_HEADER_LEN	/* KM4 BootLoader RAM: 32k */


	KM0_BD_RAM_MP (rwx) :                   ORIGIN = 0x2001A040, LENGTH = 0x2005DF80 - 0x2001A040	/* KM0 BD RAM: 272K, (LENGTH + 0xC0) must be a multiple of 4KB */
	KM0_BD_RAM_MP_END(rwx) :                ORIGIN = 0x2005DF80 - 32, LENGTH = 32                 /* Only for KM0 shrink MP image2 */
	KM4_BD_RAM_MP (rwx) :                   ORIGIN = 0x2005E040, LENGTH = 0x2007FFA0 - 0x2005E040	/* KM4 BD RAM: TZ off, 136k */
}

__km0_image2_entry_func__ = ORIGIN(KM0_BD_RAM);
__km0_ipc_memory_start__ = ORIGIN(KM0_IPC_RAM);
__km0_flash_text_start__ = ORIGIN(KM0_IMG2_XIP);
__km4_flash_text_start__ = ORIGIN(KM4_IMG2_XIP);
__km0_psram_text_start__ = ORIGIN(KM0_BD_PSRAM);
__km4_boot_text_start__ = ORIGIN(KM4_BOOT_XIP);

/*SRAM Layout boundary*/
__km4_bd_ram_start__ = ORIGIN(KM4_BD_RAM) - IMAGE_HEADER_LEN;
__km4_bd_ram_end__ = ORIGIN(KM4_BD_RAM) + LENGTH(KM4_BD_RAM);

/*PSRAM Layout boundary*/
__km4_bd_psram_start__ = ORIGIN(KM4_BD_PSRAM) - IMAGE_HEADER_LEN;
__non_secure_psram_start__ = KM0_IMG2_PSRAM_START;
__non_secure_psram_end__ = PSRAM_END;

/* mp used */
__km4_bd_boot_download_addr__ = 0x30012000;
__km0_bd_ram_mp_start__ = ORIGIN(KM0_BD_RAM_MP);
__km4_bd_ram_mp_start__ = ORIGIN(KM4_BD_RAM_MP);
__km0_image2_entry_func_mp__ = ORIGIN(KM0_IMG2_ENTRY);

/* Region 0: KM4 IROM */
__rom_text_region0_start__ = ORIGIN(KM4_IROM);                                        /* 0x00000000 */
__rom_text_region0_end__   = ORIGIN(KM4_IROM_STDLIB_NS) + LENGTH(KM4_IROM_STDLIB_NS); /* 0x00042000 */
/* Region 1: KM0 IROM + WIFI ROM */
__rom_text_region1_start__ = ORIGIN(KM0_IROM);                                        /* 0x00000000 */
__rom_text_region1_end__   = ORIGIN(WIFI_ROM) + LENGTH(WIFI_ROM);                     /* 0x00028000 */

/* code analyse used */
__rtk_fixed_addr__ = SRAM_BASE + KM0_KM4_RAM_SIZE;
__img1_sram_size__ = KM4_IMG1_SIZE;
__img3_sram_size__ = KM4_RAM_TZ_SIZE;

__km4_msp_ram_ns_limit__ = ORIGIN(KM4_MSP_RAM_NS);
__km0_msp_ram_limit__ = ORIGIN(KM0_MSP_RAM);
