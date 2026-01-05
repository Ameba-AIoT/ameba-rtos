ENTRY(Reset_Handler)

#include "../../../ameba_layout.ld"

SECTIONS
{
	/* image3: security image start */
	.gnu.sgstubs :
	{
		__ram_image3_nsc_start__ = .;
		. = ALIGN(32);
	} > KM4_BD_RAM_NSC

	.ram_image3.entry :
	{
		KEEP(*(.image3.nsc_entry.text*))
		. = ALIGN(32);
	} > KM4_BD_RAM_ENTRY

	.ram_image3.text :
	{
		__ram_image3_text_start__ = .;
		*(.text*)
		*(.sramdram.only.text*)
		*(.rodata*)
		. = ALIGN(32);
	} > KM4_BD_RAM_S

	.ram_image3.data :
	{
		__ram_image3_data_start__ = .;
		*(.data*)
		*(.sramdram.only.data)
		__ram_image3_data_end__ = .;

		. = ALIGN(32);
		__ram_image3_text_end__ = .;
	} > KM4_BD_RAM_S

	.ram_image3.bss :
	{
		__image3_bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__image3_bss_end__ = .;
		end = .;
		. = . + 8192;
	} > KM4_BD_RAM_S

}
