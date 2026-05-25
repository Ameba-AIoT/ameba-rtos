ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4tz/platform_autoconf.h"
/* img1's layout of Floader or Fullmac is same */
#if defined(GENERATE_FLOADER_LD) || defined (CONFIG_WHC_DEV_SDIO_BOOT)
	#define IMG1_LMA_REGION 			KM4TZ_FLOADER_RAM_S
#else
	#define	IMG1_LMA_REGION				KM4TZ_BOOTLOADER_RAM_S
#endif

#define IMG2_DATA_LMA_REGION		KM4TZ_BD_RAM

#if defined(GENERATE_FLOADER_LD) || defined (CONFIG_WHC_DEV_SDIO_BOOT) || !defined(CONFIG_IMG1_FLASH)
	#define SRAM_INPUT_TEXT_SECTION *(.text*)
	#define	SRAM_INPUT_NON_DRAM		*(.non.dram.text*)
	#define SRAM_INPUT_BOOT_RODATA	*(.boot.rodata*)
	#define SRAM_INPUT_RODATA		*(.rodata*)
	#define FLASH_INPUT_TEXT_SECTION
	#define	FLASH_INPUT_NON_DRAM
	#define FLASH_INPUT_BOOT_RODATA
	#define FLASH_INPUT_RODATA
#else
	#define SRAM_INPUT_TEXT_SECTION
	#define	SRAM_INPUT_NON_DRAM
	#define SRAM_INPUT_BOOT_RODATA
	#define SRAM_INPUT_RODATA
	#define FLASH_INPUT_TEXT_SECTION *(.text*)
	#define	FLASH_INPUT_NON_DRAM	*(.non.dram.text*)
	#define FLASH_INPUT_BOOT_RODATA	*(.boot.rodata*)
	#define FLASH_INPUT_RODATA		*(.rodata*)
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

	/*  Add . to assign the start address of the section */
	/*  to prevent the change of the start address by ld doing section alignment */
	.ram_image1.text . :
	{
		/* image1 text */
		SRAM_INPUT_TEXT_SECTION
		SRAM_INPUT_NON_DRAM
		SRAM_INPUT_BOOT_RODATA
		SRAM_INPUT_RODATA
		*(.sramdram.only.text*)
	} > IMG1_LMA_REGION

	.ram_image1.data . :
	{
		. = ALIGN(32);
		__ram_image1_data_start__ = .;
		*(.data*)
		__ram_image1_data_end__ = .;

		__ram_image1_text_end__ = .;
	} > IMG1_LMA_REGION

	.ram_image1.bss . :
	{
		__image1_bss_start__ = .;
		*(.bss*)
		__image1_bss_end__ = .;
	} > IMG1_LMA_REGION

	.xip_image1.text : {
		/* image1 text */
		FLASH_INPUT_TEXT_SECTION
		FLASH_INPUT_NON_DRAM
		FLASH_INPUT_BOOT_RODATA
		FLASH_INPUT_RODATA
	} > KM4TZ_BOOT_XIP

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__image2_entry_func__ = .;
	} > IMG2_DATA_LMA_REGION
}
