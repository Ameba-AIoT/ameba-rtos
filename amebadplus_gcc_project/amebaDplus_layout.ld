/* auto gen, please change AmebaDplus_memory_layout.xlsx and exec gen_memory_layout.py */

#include "project_km4/inc/platform_autoconf.h"
/* constant macro */
#define SECURE_ADDR_OFFSET 			(0x10000000)
#define KBYTES(x)  					((x) * 1024)
#define IMAGE_HEADER_LEN			(0x20)
#define KM0_KM4_RAM_SIZE			KBYTES(40)
#define SRAM_BASE					(0x20000000)
#define SRAM_END					(0x20080000)
#define PSRAM_BASE					(0x60000000)
#define PSRAM_KR4_END				(0x60400000)

/* layout configuration */
#define TZ_NSC_SIZE 	 			(4)
#define TZ_ENTRY_SIZE 				(16)
#define TZ_S_SIZE 					(44)

#if defined(CONFIG_IMG3_SRAM)
#define RAM_TZ_NSC_SIZE 	 		KBYTES(TZ_NSC_SIZE)
#define RAM_TZ_ENTRY_SIZE 			KBYTES(TZ_ENTRY_SIZE)
#define RAM_KM4_IMG3_SIZE 			KBYTES(TZ_S_SIZE)
#define PSRAM_TZ_NSC_SIZE 	 		KBYTES(0)
#define PSRAM_TZ_ENTRY_SIZE 		KBYTES(0)
#define PSRAM_KM4_IMG3_SIZE 		KBYTES(0)
#elif defined(CONFIG_IMG3_PSRAM)
#define RAM_TZ_NSC_SIZE 	 		KBYTES(0)
#define RAM_TZ_ENTRY_SIZE 			KBYTES(0)
#define RAM_KM4_IMG3_SIZE 			KBYTES(0)
#define PSRAM_TZ_NSC_SIZE 	 		KBYTES(TZ_NSC_SIZE)
#define PSRAM_TZ_ENTRY_SIZE 		KBYTES(TZ_ENTRY_SIZE)
#define PSRAM_KM4_IMG3_SIZE 		KBYTES(TZ_S_SIZE)
#else /* undefine trustzone */
#define RAM_TZ_NSC_SIZE 	 		KBYTES(0)
#define RAM_TZ_ENTRY_SIZE 			KBYTES(0)
#define RAM_KM4_IMG3_SIZE 			KBYTES(0)
#define PSRAM_TZ_NSC_SIZE 	 		KBYTES(0)
#define PSRAM_TZ_ENTRY_SIZE 		KBYTES(0)
#define PSRAM_KM4_IMG3_SIZE 		KBYTES(0)
#endif

#if defined(CONFIG_IMG1_FLASH)
#define KM4_IMG1_SIZE 				KBYTES(4)
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
If Trustzone off,  │  │      TZ_NSC(4K)      │    │        If Trustzone on,
              ◄────┤  │     TZ_ENTRY(16K)    │    ├─────►  IMG3 = RAM_TZ_NSC_SIZE + RAM_TZ_ENTRY_SIZE + RAM_KM4_IMG3_SIZE
IMG3 size is 0.    │  │     KM4_IMG3(44K)    │    │
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
#define KM4_IMG1_RAM_SATRT   		(SRAM_BASE + KM0_KM4_RAM_SIZE)				/* 0x2000A000 ~ 0x20010000 | 0x2000A000 ~ 0x2000B000*/
#define KM4_RAM_TZ_NSC_START		(KM4_IMG1_RAM_SATRT + KM4_IMG1_SIZE )		/* 0x20010000 ~ 0x20011000 | unavailable */
#define KM4_RAM_TZ_ENTRY_START		(KM4_RAM_TZ_NSC_START + RAM_TZ_NSC_SIZE )		/* 0x20011000 ~ 0x20015000 | unavailable */
#define KM4_RAM_TZ_SECURE_START		(KM4_RAM_TZ_ENTRY_START + RAM_TZ_ENTRY_SIZE )	/* 0x20015000 ~ 0x20020000 | unavailable */
#define KM4_IMG2_RAM_START			(KM4_RAM_TZ_SECURE_START + RAM_KM4_IMG3_SIZE)	/* 0x20020000 ~ 0x20068000 | 0x2000B000 ~ 0x20068000*/
#define KM0_IMG2_RAM_START			(SRAM_END - RAM_KM0_IMG2_SIZE) 					/* 0x20068000 ~ 0x20080000 | 0x20068000 ~ 0x20080000*/

/* IMG3 On PSRAM */
#define KM4_PSRAM_TZ_NSC_START		(PSRAM_BASE)									/* 0x60000000 ~ 0x60001000 | unavailable */
#define KM4_PSRAM_TZ_ENTRY_START	(KM4_PSRAM_TZ_NSC_START + PSRAM_TZ_NSC_SIZE )	/* 0x60001000 ~ 0x60005000 | unavailable */
#define KM4_PSRAM_TZ_SECURE_START	(KM4_PSRAM_TZ_ENTRY_START + PSRAM_TZ_ENTRY_SIZE)/* 0x60005000 ~ 0x60010000 | unavailable */

#define PSRAM_KM4_IMG2_SIZE			(KBYTES(3220) - (PSRAM_TZ_NSC_SIZE + PSRAM_TZ_ENTRY_SIZE + PSRAM_KM4_IMG3_SIZE))
#define KM4_IMG2_PSRAM_START		(KM4_PSRAM_TZ_SECURE_START + PSRAM_KM4_IMG3_SIZE)	/* 0x60010000 ~ 0x60325000 | 0x60000000 ~ 0x60325000*/
#define KM0_IMG2_PSRAM_START		(KM4_IMG2_PSRAM_START + PSRAM_KM4_IMG2_SIZE) 	 	/* 0x60325000 ~ 0x60400000 | 0x60325000 ~ 0x60400000*/
#define PSRAM_KM0_IMG2_SIZE			(PSRAM_KR4_END - KM0_IMG2_PSRAM_START)

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
	KM4_BOOTLOADER_RAM_S (rwx) :            ORIGIN = KM4_IMG1_RAM_SATRT + SECURE_ADDR_OFFSET + IMAGE_HEADER_LEN, LENGTH = KM4_IMG1_SIZE  - IMAGE_HEADER_LEN		/* KM4 BOOT Loader RAM: 4K or 24k */
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
	KM4_BD_RAM_TZ_NSC (rwx) :               ORIGIN = KM4_RAM_TZ_NSC_START + IMAGE_HEADER_LEN, 	LENGTH = RAM_TZ_NSC_SIZE - IMAGE_HEADER_LEN		/* KM4 BD RAM NSC: TZ Off,0;TZ On, 4K*/
	KM4_BD_RAM_TZ_ENTRY (rwx) :             ORIGIN = KM4_RAM_TZ_ENTRY_START, 				  	LENGTH = RAM_TZ_ENTRY_SIZE						/* KM4 BD RAM ENTRY: TZ Off,0;TZ On,16k */
	KM4_BD_RAM_TZ_S (rwx) :                 ORIGIN = KM4_RAM_TZ_SECURE_START + SECURE_ADDR_OFFSET  + IMAGE_HEADER_LEN, LENGTH = RAM_KM4_IMG3_SIZE - IMAGE_HEADER_LEN/* KM4 BD RAM S: TZ Off,0;TZ On,44k*/
	KM4_BD_RAM (rwx) :                      ORIGIN = KM4_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = KM0_IMG2_RAM_START - KM4_IMG2_RAM_START - IMAGE_HEADER_LEN	/* KM4 BD RAM: TZ off,372K,0;TZ On,288k */
	KM0_BD_RAM (rwx) :                      ORIGIN = KM0_IMG2_RAM_START + IMAGE_HEADER_LEN,   	LENGTH = RAM_KM0_IMG2_SIZE - IMAGE_HEADER_LEN	/* KM0 BD RAM: TZ off,96K,0;TZ On,96k */

	KM4_BD_PSRAM_TZ_NSC (rwx) :             ORIGIN = KM4_PSRAM_TZ_NSC_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_TZ_NSC_SIZE - IMAGE_HEADER_LEN	/* KM4 BD PSRAM NSC: Tz off, 0; TZ On, 4k */
	KM4_BD_PSRAM_TZ_ENTRY (rwx) :           ORIGIN = KM4_PSRAM_TZ_ENTRY_START, 					LENGTH = PSRAM_TZ_ENTRY_SIZE					/* KM4 BD PSRAM ENTRY: Tz off, 0; TZ On, 16k */
	KM4_BD_PSRAM_TZ_S (rwx) :               ORIGIN = KM4_PSRAM_TZ_SECURE_START + SECURE_ADDR_OFFSET  + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4_IMG3_SIZE - IMAGE_HEADER_LEN	/* KM4 BD PSRAM S: Tz off, 0; TZ On, 44k */
	/* KM4 Non-secure PSRAM */
	KM4_BD_PSRAM (rwx) :                    ORIGIN = KM4_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM4_IMG2_SIZE - IMAGE_HEADER_LEN	/* KM4 BD PSRAM : Tz off,3220K;TZ on, 3156K */
	KM0_BD_PSRAM (rwx) :                    ORIGIN = KM0_IMG2_PSRAM_START + IMAGE_HEADER_LEN, LENGTH = PSRAM_KM0_IMG2_SIZE - IMAGE_HEADER_LEN	/* KM0 BD PSRAM: 876K */

	KM4_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KM4 PSRAM HEAP EXT */
	KM0_PSRAM_HEAP_EXT (rwx) :              ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KM0 PSRAM HEAP EXT */

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


	KM4_BD_RAM_MP (rwx) :                   ORIGIN = 0x2001A000 + IMAGE_HEADER_LEN,   	LENGTH = 0x2003F800 - 0x2001A000 - IMAGE_HEADER_LEN	/* KM4 BD RAM: TZ off,150K*/
	KM0_BD_RAM_MP (rwx) :                   ORIGIN = 0x2003F800 + IMAGE_HEADER_LEN,   	LENGTH = 0x20080000 - 0x2003F800 - IMAGE_HEADER_LEN	/* KM0 BD RAM: 270k */

}

__km0_image2_entry_func__ = ORIGIN(KM0_BD_RAM);
__km0_ipc_memory_start__ = ORIGIN(KM0_IPC_RAM);
__km0_flash_text_start__ = ORIGIN(KM0_IMG2_XIP);
__km4_flash_text_start__ = ORIGIN(KM4_IMG2_XIP);
__km0_psram_text_start__ = ORIGIN(KM0_BD_PSRAM);
__km4_boot_text_start__ = ORIGIN(KM4_BOOT_XIP);

/*SRAM Layout boundary*/
__km4_tz_nsc_start__ = ORIGIN(KM4_BD_RAM_TZ_NSC) - IMAGE_HEADER_LEN;
__km4_tz_entry_start__ = ORIGIN(KM4_BD_RAM_TZ_ENTRY);
__km4_tz_entry_end__ = ORIGIN(KM4_BD_RAM_TZ_ENTRY) + LENGTH(KM4_BD_RAM_TZ_ENTRY);
__km4_bd_ram_start__ = ORIGIN(KM4_BD_RAM) - IMAGE_HEADER_LEN;

/*PSRAM Layout boundary*/
__km4_psram_tz_nsc_start__ = ORIGIN(KM4_BD_PSRAM_TZ_NSC) - IMAGE_HEADER_LEN;
__km4_psram_tz_entry_start__ = ORIGIN(KM4_BD_PSRAM_TZ_ENTRY) ;
__km4_psram_tz_entry_end__ = ORIGIN(KM4_BD_PSRAM_TZ_ENTRY) + LENGTH(KM4_BD_PSRAM_TZ_ENTRY);
__km4_bd_psram_start__ = ORIGIN(KM4_BD_PSRAM) - IMAGE_HEADER_LEN;

/* mp used */
__km4_bd_boot_download_addr__ = 0x30012000;
__km0_bd_ram_mp_start__ = ORIGIN(KM0_BD_RAM_MP);
__km0_image2_entry_func_mp__ = ORIGIN(KM0_IMG2_ENTRY);