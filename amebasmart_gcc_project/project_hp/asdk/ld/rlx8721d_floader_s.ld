ENTRY(Reset_Handler)

INCLUDE "../../amebasmart_layout.ld"

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
	} > KM4_FLOADER_RAM_S

	/*  Add . to assign the start address of the section */
	/*  to prevent the change of the start address by ld doing section alignment */
	.ram_image1.text . :
	{
		/* image1 text */
		*(.text*)
		*(.rodata*)
		. = ALIGN (4);
	} > KM4_FLOADER_RAM_S

	.ram_image1.data . :
	{
		__ram_image1_data_start__ = .;
		*(.data*)
		. = ALIGN(16);
		__ram_image1_data_end__ = .;

		__ram_image1_text_end__ = .;
	} > KM4_FLOADER_RAM_S

	.ram_image1.bss . :
	{
		__image1_bss_start__ = .;
		*(.bss*)
		__image1_bss_end__ = .;
	} > KM4_FLOADER_RAM_S

	/* image2: normal image start */
	.ram_image2.entry :
	{
		__ram_image2_text_start__ = .;
		__image2_entry_func__ = .;
		KEEP(*(SORT(.image2.entry.data*)))

	} > KM4_BD_DRAM
}
