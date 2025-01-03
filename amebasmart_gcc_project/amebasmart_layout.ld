/* auto gen, please change AmebaSmart_memory_layout.xlsx and exec gen_memory_layout.py */

#include "project_hp/inc/platform_autoconf.h"
/* constant macro */
#define SECURE_ADDR_OFFSET 			(0x10000000)
#define KBYTES(x)  					((x) * 1024)
#define IMAGE_HEADER_LEN			(0x20)

MEMORY
{
	/* ROM */
	KM4_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0001E000 - 0x00000000	/* KM4 IROM_S: 120k */
	KM4_IROM_NS (rx) :                      ORIGIN = 0x0001E000, LENGTH = 0x00033000 - 0x0001E000	/* KM4 IROM_NS: 84k */
	KM4_IROM_STDLIB_NS (rx) :               ORIGIN = 0x00033000, LENGTH = 0x00046000 - 0x00033000	/* KM4 STDLIB ROM: 76k */
	KM4_DROM_NS (rx) :                      ORIGIN = 0x00046000, LENGTH = 0x00054000 - 0x00046000	/* KM4 DROM_NS: 56k */
	KM4_DROM (rx) :                         ORIGIN = 0x00054000, LENGTH = 0x00058000 - 0x00054000	/* KM4 DROM_S: 16k */

	KM0_IROM (rx) :                         ORIGIN = 0x00000000, LENGTH = 0x0000C000 - 0x00000000	/* KM0 IROM: 48k */
	KM0_DROM (rx) :                         ORIGIN = 0x0000C000, LENGTH = 0x00010000 - 0x0000C000	/* KM0 DROM: 16k */

	CA32_IROM (rx) :                        ORIGIN = 0x00000000, LENGTH = 0x00000400 - 0x00000000	/* CA32 IROM: 1k */

	/* KM4 SRAM, BIT(28)=1 */
	KM4_ROMBSS_RAM_S (rw) :                 ORIGIN = 0x30000000, LENGTH = 0x30001000 - 0x30000000	/* KM4 ROM BSS RAM S: 4k */
	KM4_MSP_RAM_S (rw) :                    ORIGIN = 0x30001000, LENGTH = 0x30003000 - 0x30001000	/* KM4 MSP_S RAM: 8k */
	KM4_FLOADER_RAM_S (rwx) :               ORIGIN = 0x30003020, LENGTH = 0x3001B000 - 0x30003020	/* KM4 ImgTool Flash Loader RAM: 95k */
	KM4_BOOTLOADER_RAM_S (rwx) :            ORIGIN = 0x30003020, LENGTH = 0x30014000 - 0x30003020	/* KM4 BOOT Loader RAM: 68k */
	KM4_BD_RAM (rwx) :                      ORIGIN = 0x20014020, LENGTH = 0x2001B000 - 0x20014020	/* KM4 MAIN SRAM: 28k, used when XIP only */
	KM4_MSP_RAM_NS (rw) :                   ORIGIN = 0x2001B000, LENGTH = 0x2001C000 - 0x2001B000	/* KM4 MSP_NS RAM: 4k */
	KM4_ROMBSS_RAM_COM (rw) :               ORIGIN = 0x2001C000, LENGTH = 0x2001D000 - 0x2001C000	/* KM4 ROM BSS COMMON(S & NS both used) RAM: 4k */
	KM4_ROMBSS_RAM_NS (rw) :                ORIGIN = 0x2001D000, LENGTH = 0x2001E000 - 0x2001D000	/* KM4 ROM BSS NS RAM: 4k */
	KM4_STDLIB_HEAP_RAM_NS (rw) :           ORIGIN = 0x2001E000, LENGTH = 0x2001F000 - 0x2001E000	/* KM4 ROM STDLIB HEAP: 4k */
	/* 0x3001F000 ~ 0x3001FE00 used for atf share ram when XIP only */
	CA32_BL1_SRAM_S (rwx) :                 ORIGIN = 0x3001FE00, LENGTH = 0x30020000 - 0x3001FE00	/* CA32 BL1 SRAM: 512B */
	KM4_AUDIO_BUFFER (rw) :                 ORIGIN = 0x20020000, LENGTH = 0x20040000 - 0x20020000	/* KM4 AUDIO BUFFER: 128k */

	/* KM4 EXTENTION SRAM */
	EXTENTION_SRAM (rwx) :                  ORIGIN = 0x22000000, LENGTH = 0x23000000 - 0x22000000	/* KM4 EXTENTION SRAM: 16384k (Wi-Fi 40KB + Bluetooth 256KB) */

	/* KM0 SRAM */
	KM0_ROMBSS_RAM (rw) :                   ORIGIN = 0x23000000, LENGTH = 0x23001000 - 0x23000000	/* KM0 ROM BSS RAM: 4k */
	KM0_MSP_RAM (rw) :                      ORIGIN = 0x23001000, LENGTH = 0x23002000 - 0x23001000	/* KM0 MSP RAM: 4k */
	KM0_BD_RAM (rwx) :                      ORIGIN = 0x23002020, LENGTH = 0x2301FD00 - 0x23002020	/* KM0 MAIN RAM: 119k */
	KM0_IPC_RAM (rwx) :                     ORIGIN = 0x2301FD00, LENGTH = 0x23020000 - 0x2301FD00	/* KM0 IPC RAM: 768B */
	/* Retention RAM */
	RETENTION_RAM (rwx) :                   ORIGIN = 0x23020000, LENGTH = 0x23020200 - 0x23020000	/* KM0 Retention SRAM: 512B */

	/* Flash */
	KM4_BOOT_XIP (rx) :                     ORIGIN = 0x0A000020, LENGTH = 0x0C000000 - 0x0A000020	/* KM4 Bootloader Virtual address: 32767k */
	KM0_IMG2_XIP (rx) :                     ORIGIN = 0x0C000020, LENGTH = 0x0D000000 - 0x0C000020	/* KM0 IMG2 Virtual address: 32767k */
	KM4_IMG2_XIP (rx) :                     ORIGIN = 0x0D000020, LENGTH = 0x0E000000 - 0x0D000020	/* KM4 IMG2 Virtual address: 32767k */
	CA32_IMG2_XIP (rx) :                    ORIGIN = 0x0E000020, LENGTH = 0x10000000 - 0x0E000020	/* CA32 IMG2 Virtual address: 32767k */

	BTRACE (rx) :                           ORIGIN = 0xC8800000, LENGTH = 0xC9000000 - 0xC8800000	/* BT LOG Virtual address: 8192k */
	CTRACE (rx) :                           ORIGIN = 0xCA000000, LENGTH = 0xCA7FFFFF - 0xCA000000	/* COEX LOG Virtual address: 8192k */

	/* DRAM */
#if CONFIG_XIP_FLASH
	KM4_BD_DRAM (rwx) :                     ORIGIN = 0x60000020, LENGTH = 0x60060000 - 0x60000020	/* KM4 MAIN DRAM: 384K */

	CA32_BL1_DRAM_S (rwx) :                 ORIGIN = 0x70060020, LENGTH = 0x70070000 - 0x70060020	/* CA32 BL1 DRAM S: 64k */
	CA32_BL3_DRAM_NS (rwx) :                ORIGIN = 0x60094000, LENGTH = 0x60800000 - 0x60094000	/* CA32 BL3 DRAM NS: 7600K */
	CA32_DYNAMIC_APP_LOAD_DRAM (rwx) :      ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* CA32 CA32 DYNAMIC APP LOAD DRAM: 1MB */
	KM4_DRAM_HEAP_EXT (rwx) :               ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KM4 PSRAM HEAP EXT: 0 */
#else
	KM4_BD_DRAM (rwx) :                     ORIGIN = 0x60000020, LENGTH = 0x6015B000 - 0x60000020	/* KM4 MAIN DRAM: 1387k */
	KM4_BD_RAM_NSC (rwx) :                  ORIGIN = 0x6015B020, LENGTH = 0x6015C000 - 0x6015B020	/* KM4 BD RAM NSC: 3k */
	KM4_BD_RAM_ENTRY (rwx) :                ORIGIN = 0x6015C000, LENGTH = 0x60160000 - 0x6015C000	/* KM4 BD RAM ENTRY: 16k */
	KM4_BD_RAM_S (rwx) :                    ORIGIN = 0x70160020, LENGTH = 0x70180000 - 0x70160020	/* KM4 BD RAM S: 127k */

	CA32_BL1_DRAM_S (rwx) :                 ORIGIN = 0x70180020, LENGTH = 0x701A0000 - 0x70180020	/* CA32 BL1 DRAM S: 127k */
	CA32_BL3_DRAM_NS (rwx) :                ORIGIN = 0x60300000, LENGTH = 0x60700000 - 0x60300000	/* CA32 BL3 DRAM NS: 4MB */
	CA32_DYNAMIC_APP_LOAD_DRAM (rwx) :      ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* CA32 CA32 DYNAMIC APP LOAD DRAM: 1MB */
	KM4_DRAM_HEAP_EXT (rwx) :               ORIGIN = 0x60700000, LENGTH = 0x60800000 - 0x60700000	/* KM4 PSRAM HEAP EXT: 1MB, (PSRAM Die is 8MB) */
#endif

	/* By Default, KM4 let KM4_BD_DRAM and [0x60600000, 0x6FFFFFFF) be non-secure, ATF Will Let [CA32_BL3_DRAM_NS, 0x60600000) Non-Secure after image load done */
	CA32_FIP_DRAM_S (rwx) :                 ORIGIN = 0x70400000, LENGTH = 0x70600000 - 0x70400000	/* CA32 FIP(BL2+BL32+BL33) IMG: 2MB */

	KM0_BD_DRAM (rwx) :                     ORIGIN = 0x6FFFFFFF, LENGTH = 0x6FFFFFFF - 0x6FFFFFFF	/* KM0 BD DRAM */
}

__rom_stdlib_text_start__ = ORIGIN(KM4_IROM_STDLIB_NS);
__km0_ipc_memory_start__ = ORIGIN(KM0_IPC_RAM);
__km4_boot_text_start__ = ORIGIN(KM4_BOOT_XIP);
__km0_flash_text_start__ = ORIGIN(KM0_IMG2_XIP);
__km4_flash_text_start__ = ORIGIN(KM4_IMG2_XIP);
__ca32_flash_text_start__ = ORIGIN(CA32_IMG2_XIP);

__ca32_bl1_sram_start__ = ORIGIN(CA32_BL1_SRAM_S);
__ca32_bl1_dram_start__ = ORIGIN(CA32_BL1_DRAM_S);
__ca32_fip_dram_start__ = ORIGIN(CA32_FIP_DRAM_S);
__dram_dynamic_app_text_start__ = ORIGIN(CA32_DYNAMIC_APP_LOAD_DRAM);
__dram_dynamic_app_text_end__ = ORIGIN(CA32_DYNAMIC_APP_LOAD_DRAM) + LENGTH(CA32_DYNAMIC_APP_LOAD_DRAM);
__dram_dynamic_app_text_size__ = LENGTH(CA32_DYNAMIC_APP_LOAD_DRAM);
