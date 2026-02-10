ENTRY(Reset_Handler)

INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	.rom.text : { } > KM0_IROM
	.rom.rodata : { } > KM0_DROM
	.wifi.rom : { } > WIFI_ROM
	.hal.rom.bss : { } > KM0_ROMBSS_RAM

	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_image1_text_start__ = .;
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
		KEEP(*(.image1.export.symb*))
	} > KM0_BOOT_RAM

	/* retention RAM entry, this section should in RAM and fixed address for ROM */
	.ram_retention.entry :
	{
		__retention_entry_func__ = ABSOLUTE(.);
	} > RETENTION_RAM

	/* data & bss should have not any content */
	.ram.data :
	{
		KEEP(*(.data*))
	} > EMPTY_SECTION

 	.ram.bss :
	{
		KEEP(*(.bss*))
	} > EMPTY_SECTION

	/* should not any content in toolchain */
	.rom.text.libgcc :
	{
		KEEP(*(.text*))
		KEEP(*(.rodata*))
	} > EMPTY_SECTION
}
