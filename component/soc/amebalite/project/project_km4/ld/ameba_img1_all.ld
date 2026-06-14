ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"
#include "project_km4/platform_autoconf.h"
/* define common load memory address */
#ifdef  GENERATE_FLOADER_LD
	#define IMG1_LMA_REGION 			KM4_FLOADER_RAM_S
	#define SRAM_INPUT_TEXT_SECTION 	*(.text*)
	#define	SRAM_INPUT_NON_DRAM			*(.non.dram.text*)
	#define SRAM_INPUT_RODATA			*(.rodata*)
	#define FLASH_INPUT_TEXT_SECTION
	#define	FLASH_INPUT_NON_DRAM
	#define FLASH_INPUT_RODATA
#else
	#define IMG1_LMA_REGION 			KM4_BOOTLOADER_RAM_S
	#ifdef CONFIG_IMG1_FLASH
		#define SRAM_INPUT_TEXT_SECTION
		#define	SRAM_INPUT_NON_DRAM
		#define SRAM_INPUT_RODATA
		#define FLASH_INPUT_TEXT_SECTION *(.text*)
		#define	FLASH_INPUT_NON_DRAM	*(.non.dram.text*)
		#define FLASH_INPUT_RODATA		*(.rodata*)
	#else
		#define SRAM_INPUT_TEXT_SECTION *(.text*)
		#define	SRAM_INPUT_NON_DRAM		*(.non.dram.text*)
		#define SRAM_INPUT_RODATA		*(.rodata*)
		#define FLASH_INPUT_TEXT_SECTION
		#define	FLASH_INPUT_NON_DRAM
		#define FLASH_INPUT_RODATA
	#endif
#endif

/* define trustzone load memory address */
#ifdef CONFIG_IMG3_SRAM
	#define LMA_TZ_REGION KM4_BD_RAM_TZ
#else
	#define LMA_TZ_REGION KM4_BD_PSRAM_TZ
#endif

SECTIONS
{
	/* image1: boot image start */
	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_image1_text_start__ = .;
		__ram_start_table_start__ = .;
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
		FLASH_INPUT_RODATA
	} > KM4_BOOT_XIP

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__image2_entry_func__ = .;

	} > KM4_BOOT_RAM

	/* image3 (TFM/secure) entry table address, image1 jumps here directly */
	.ram_image3.text :
	{
		__ram_image3_start__ = .;
		__ram_image3_end__ = __ram_image3_start__ + LENGTH(LMA_TZ_REGION);
	} > LMA_TZ_REGION
}

__dsp_psram_addr__ = ORIGIN(DSP_BD_PSRAM);
