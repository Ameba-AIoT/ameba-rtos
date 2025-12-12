ENTRY(Reset_Handler)

INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	.rom.text : { } > KM0_IROM
	.rom.rodata : { } > KM0_DROM
	.hal.rom.bss : { } > KM0_ROMBSS_RAM

	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_image1_text_start__ = .;
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
		KEEP(*(.image1.export.symb*))
	} > KM0_BD_RAM

	/* addr not fixed, addr will change when link with RAM code */
	.ram_image1.text :
	{
		KEEP(*(.hal.ram.data*))
	} > KM0_BD_RAM

	/* retention RAM entry, this section should in RAM and fixed address for ROM */
	.ram_retention.entry :
	{
		__retention_entry_func__ = ABSOLUTE(.);
	} > RETENTION_RAM

	/* data & bss should have not any content */
	.ram.data :
	{
		KEEP(*(.data*))
	} > KM0_BD_RAM
 	.ram.bss :
	{
    		KEEP(*(.bss*))
	} > KM0_BD_RAM
	.rom.text.libgcc :
	{
		KEEP(*(.text*))
	} > KM0_IROM
}
