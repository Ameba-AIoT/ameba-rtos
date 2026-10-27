ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4/platform_autoconf.h"
#ifdef CONFIG_IMG3_PSRAM
	#define LMA_TZ_NSC 		KM4_BD_PSRAM_TZ_NSC
	#define LMA_TZ			KM4_BD_PSRAM_TZ
	#define TZ_AREA_START	__km4_psram_tz_start__
	#define TZ_AREA_END		__km4_psram_tz_end__
#else
	#define LMA_TZ_NSC 		KM4_BD_RAM_TZ_NSC
	#define LMA_TZ			KM4_BD_RAM_TZ
	#define TZ_AREA_START	__km4_tz_start__
	#define TZ_AREA_END		__km4_tz_end__
#endif

/*
 * TZ area (one block, linked at the non-secure alias, SAU/MPC mark it secure):
 *   TZ_START          : NSC sub-image header (32B), then veneers (.gnu.sgstubs), up to TZ_START + TZ_NSC_WINDOW
 *   TZ_START + 0x400  : image3 sub-image header (32B)
 *   TZ_START + 0x420  : ENTRY, text, data, bss, secure heap, up to TZ_END
 */
SECTIONS
{
	/* image3: security image start */
	.gnu.sgstubs :
	{
		__ram_image3_nsc_start__ = .;
		. = ALIGN(32);
	} > LMA_TZ_NSC

	__ram_image3_nsc_end__ = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

	.ram_image3.entry :
	{
		__ram_image3_text_start__ = .;
		KEEP(*(.image3.nsc_entry.text*))
		. = ALIGN(32);
	} > LMA_TZ

	.ram_image3.text :
	{
		*(.text*)
		*(.rodata*)
		. = ALIGN(32);
	} > LMA_TZ

	.ram_image3.data :
	{
		__ram_image3_data_start__ = .;
		*(.data*)
		__ram_image3_data_end__ = .;

		. = ALIGN(32);
		__ram_image3_text_end__ = .;
	} > LMA_TZ

	.ram_image3.bss :
	{
		__image3_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		. = ALIGN(32);
		__image3_heap_start__ = .;
	} > LMA_TZ

}

/* Calculate remaining space for secure heap */
__image3_heap_size__ = ORIGIN(LMA_TZ) + LENGTH(LMA_TZ) - __image3_heap_start__;

ASSERT((TZ_AREA_START & (KBYTES(4) - 1)) == 0, "TZ area start must be 4K aligned (MPC granularity).")
ASSERT((TZ_AREA_END & (KBYTES(4) - 1)) == 0, "CONFIG_TZ_S_SIZE must be a multiple of 4 (KB): TZ area end must be 4K aligned (MPC granularity).")
ASSERT(__ram_image3_nsc_end__ <= TZ_AREA_START + TZ_NSC_WINDOW, "NSC veneers overflow the NSC window.")
ASSERT(ADDR(.ram_image3.entry) == TZ_AREA_START + TZ_NSC_WINDOW + IMAGE_HEADER_LEN, "image3 sub-image must start right after the NSC window.")
ASSERT(__image3_heap_start__ <= ORIGIN(LMA_TZ) + LENGTH(LMA_TZ), "TZ area too small for image3 code/data/bss, increase CONFIG_TZ_S_SIZE.")
