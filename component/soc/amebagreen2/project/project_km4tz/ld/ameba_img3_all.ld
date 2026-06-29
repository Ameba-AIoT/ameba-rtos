ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4tz/platform_autoconf.h"

/* ----------------------------------------------------------------------- *
 * image3 is split into three sub-images (xip / sram / psram), like image2.
 * Which physical bin carries the secure code is decided by the mutually
 * exclusive CONFIG_IMG3_{FLASH,SRAM,PSRAM} choice (bin packaging is done in
 * make/image3/CMakeLists.txt):
 *
 *   entry + NSC gateway (.gnu.sgstubs) + bulk .text/.rodata
 *       -> the region selected by CONFIG_IMG3_* (FLASH: xip / SRAM / PSRAM).
 *          __image3_entry_func__ stays at that region's ORIGIN, matching the
 *          symbol image1 defines, so image1's ld and the SAU config are left
 *          untouched.
 *   .sramdram.only.text / .sram.only.text/.data / .non.dram.text / .data
 *       -> always SRAM-TZ (the sram sub-image). This is where low-power code
 *          that must run after flash deep-power-down / psram off will live.
 *
 * The xip sub-image header always carries a flash virtual address
 * (__image3_flash_text_start__); it must never be an SRAM/PSRAM address.
 * Empty sub-images are emitted as header-only bins by the post-build step.
 * ----------------------------------------------------------------------- */

#if defined(CONFIG_IMG3_PSRAM)
#define LMA_TZ_S_TEXT_RO   KM4TZ_BD_PSRAM_TZ
#elif defined(CONFIG_IMG3_FLASH)
#define LMA_TZ_S_TEXT_RO   KM4TZ_IMG3_XIP
#else
#define LMA_TZ_S_TEXT_RO   KM4TZ_BD_RAM_TZ
#endif

SECTIONS
{
	/* -------- entry + NSC gateway + bulk text (region per CONFIG_IMG3_*) -- */
	.ram_image3.entry :
	{
		__image3_entry_func__ = .;
		KEEP(*(.image3.entry.data))
		KEEP(*(.image3.nsc_entry.text*))
		__ram_tz_end__ = .;
	} > LMA_TZ_S_TEXT_RO

	.gnu.sgstubs :
	{
		. = ALIGN(32);
		__ram_image3_nsc_start__ = .;
		*(.gnu.sgstubs*)
		. = ALIGN(32);
	} > LMA_TZ_S_TEXT_RO

	__ram_image3_nsc_end__ = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(.text*)
		*(.rodata*)
		__ram_image3_text_end__ = .;
	} > LMA_TZ_S_TEXT_RO

	/* -------- SRAM-resident code/data (always the sram sub-image) -------- */
	.sram_image3.text.data : ALIGN(32)
	{
		*(.non.dram.text*)        /* may be flash/sram; kept in SRAM (safe) */
		*(.sramdram.only.text*)   /* must not stay in flash */
		*(.sram.only.text*)       /* must run from SRAM (low-power path) */
		*(.sram.only.data*)
		. = ALIGN(32);
		__ram_image3_data_start__ = .;
		*(.data*)
		__ram_image3_data_end__ = .;
	} > KM4TZ_BD_RAM_TZ

	/* ------------------------------- BSS --------------------------------- */
	.ram_image3.bss (NOLOAD):
	{
		__image3_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		__image3_heap_start__ = .;
	} > KM4TZ_BD_RAM_TZ

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__image2_entry_func__ = .;
	} > KM4TZ_BD_RAM

	/* 0-byte anchor: source for header-only (empty) image3 sub-images */
	.img3_empty :
	{
		__image3_empty__ = .;
	} > KM4TZ_BD_RAM
}

/* Each image3 sub-image is loaded to its region origin, so its header address
 * is fixed regardless of which region the secure code is placed in. The xip
 * sub-image header is therefore always a flash virtual address. */
__image3_xip_start__   = ORIGIN(KM4TZ_IMG3_XIP);
__image3_sram_start__  = ORIGIN(KM4TZ_BD_RAM_TZ);
__image3_psram_start__ = ORIGIN(KM4TZ_BD_PSRAM_TZ);

__image3_ram_start__ = ORIGIN(KM4TZ_BD_RAM_TZ) - IMAGE_HEADER_LEN;
__image3_ram_end__ = __image3_ram_start__ + LENGTH(KM4TZ_BD_RAM_TZ) + IMAGE_HEADER_LEN;
__image3_heap_size__ = __image3_ram_end__ - __image3_heap_start__;

/* Secure PSRAM-TZ region bounds (used by the SAU config in IMG3_PSRAM mode). */
__image3_psram_ram_start__ = ORIGIN(KM4TZ_BD_PSRAM_TZ) - IMAGE_HEADER_LEN;
__image3_psram_ram_end__ = __image3_psram_ram_start__ + LENGTH(KM4TZ_BD_PSRAM_TZ) + IMAGE_HEADER_LEN;
