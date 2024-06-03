ENTRY(Reset_Handler)

#include "../../../amebalite_layout.ld"
#include "../../inc/platform_autoconf.h"
/* define common load memory address */

#define IMG1_LMA_REGION 			KM4_BOOTLOADER_RAM_MP

#define SRAM_INPUT_TEXT_SECTION *(.text*)
#define	SRAM_INPUT_NON_DRAM		*(.non.dram.text*)
#define SRAM_INPUT_RODATA		*(.rodata*)
#define FLASH_INPUT_TEXT_SECTION 
#define	FLASH_INPUT_NON_DRAM
#define FLASH_INPUT_RODATA		

SECTIONS
{
	/* image1: boot image start */
	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
		*(.image1.export.symb)
	} > KM4_BOOTLOADER_ENTRY

	/*  Add . to assign the start address of the section */
	/*  to prevent the change of the start address by ld doing section alignment */

	.ram_image1.text :
	{
		__ram_image1_text_start__ = .;
		/* image1 text */

		SRAM_INPUT_TEXT_SECTION 
		SRAM_INPUT_NON_DRAM		
		SRAM_INPUT_RODATA
		*(.sramdram.only.text*)
		. = ALIGN (4);
	} > IMG1_LMA_REGION

	.ram_image1.data :
	{
		__ram_image1_data_start__ = .;
		*(.data*)
		. = ALIGN(16);
		__ram_image1_data_end__ = .;

		__ram_image1_text_end__ = .;
	} > IMG1_LMA_REGION

	.ram_image1.bss :
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
	} > IMG1_LMA_REGION
	
	/* image2: normal image start */
	.ram_image2.entry :
	{
		__ram_image2_text_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

	} > KM4_BD_RAM_MP
}

__dsp_psram_addr__ = ORIGIN(DSP_BD_PSRAM);
