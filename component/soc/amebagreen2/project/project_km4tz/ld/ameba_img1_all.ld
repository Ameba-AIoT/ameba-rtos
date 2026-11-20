ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4tz/platform_autoconf.h"
/* img1's layout of Floader or Fullmac is same */
#if defined(GENERATE_FLOADER_LD) || \
	(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	#define IMG1_LMA_REGION 		 KM4TZ_FLOADER_RAM_S
#else
	#define	IMG1_LMA_REGION			 KM4TZ_BOOTLOADER_RAM_S
#endif

#define IMG2_DATA_LMA_REGION	 KM4TZ_BD_RAM

#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
#if defined (CONFIG_IMG3_PSRAM)
	#define IMG3_TZ_REGION		KM4TZ_BD_PSRAM_TZ
#elif defined (CONFIG_IMG3_FLASH)
	#define IMG3_TZ_REGION		KM4TZ_IMG3_XIP
#else
	#define IMG3_TZ_REGION		KM4TZ_BD_RAM_TZ
#endif
#endif

#if defined(GENERATE_FLOADER_LD) || \
	(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV)) || \
	!defined(CONFIG_IMG1_FLASH)
	#define OUT_TEXT_SECTION			.ram_image1.text
	#define IMG1_TEXT_LMA_REGION		IMG1_LMA_REGION
	#define SRAM_INPUT_TEXT_SECTION		*(.sramdram.only.text*)
#else
	#define OUT_TEXT_SECTION			.xip_image1.text
	#define IMG1_TEXT_LMA_REGION		KM4TZ_BOOT_XIP
	#define SRAM_INPUT_TEXT_SECTION
#endif

SECTIONS
{
	/* image1: boot image start */
	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_image1_text_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
		*(.image1.export.symb)
	} > IMG1_LMA_REGION

	/* text section */
	OUT_TEXT_SECTION :
	{
		/* image1 text */
		*(.text*)
		*(.non.dram.text*)
		*(.boot.rodata*)
		*(.rodata*)
		/* NOR flash device tables (CONFIG_NOR_FLASH_NEW_STRATEGY). KEEP() is required:
		 * nothing references the entries by symbol, so --gc-sections would drop them
		 * and silently unmatch every device. */
		. = ALIGN(4);
		__nor_flash_table_start__ = .;
		KEEP(*(.nor.flash.table.data*))
		__nor_flash_table_end__ = .;
		. = ALIGN(4);
		SRAM_INPUT_TEXT_SECTION
	} > IMG1_TEXT_LMA_REGION

	.ram_image1.text :
	{
		/* If there is any code that must be placed in RAM, please use .sramdram.only.text section */
		*(.sramdram.only.text*)
	} > IMG1_LMA_REGION

	.ram_image1.data :
	{
		. = ALIGN(32);
		__ram_image1_data_start__ = .;
		*(.data*)
		__ram_image1_data_end__ = .;

		. = ALIGN(4);
		PROVIDE_HIDDEN(__embedded_bin_start = .);
		KEEP(*(.embedded_bin))
		. = ALIGN(4);
		PROVIDE_HIDDEN(__embedded_bin_end = .);

		__ram_image1_text_end__ = .;
	} > IMG1_LMA_REGION

	/*  Add . to assign the start address of the section */
	/*  to prevent the change of the start address by ld doing section alignment */
	.ram_image1.bss . :
	{
		__image1_bss_start__ = .;
		*(.bss*)
		__image1_bss_end__ = .;
	} > IMG1_LMA_REGION

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__image2_entry_func__ = .;
	} > IMG2_DATA_LMA_REGION

#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	/* image3 TZ RAM: define entry symbol for image1->image3 jump */
	.ram_image3.text :
	{
		__image3_entry_func__ = .;
	} > IMG3_TZ_REGION
#endif
}

#ifdef CONFIG_FULLMAC_PG_LOADER
ASSERT(__image1_bss_start__ == __image1_bss_end__, "FULLMAC PG Loader can not contain bss.")
#endif
__km4tz_pg_loader_ram_start__ = ORIGIN(FULLMAC_PG_LOADER_RAM);

