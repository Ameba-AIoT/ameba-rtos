ENTRY(Reset_Handler)

INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	/* addr fixed */
	.rom.text :
	{
		KEEP(*(.vectors_table))
		KEEP(*(.vectors_table_text))
		KEEP(*(.hal.rom.text*))
		KEEP(*(.libc.rom.text*))
		KEEP(*(.rom.text*))
	} > KM0_IROM

	.rom.rodata :
	{
		__rom_entry_ns_start__ = .;
		KEEP(*(.hal.rom.entryns.rodata*))
		__rom_entry_ns_end__ = .;

		/*. = (. + 7)&(0xFFFFFFF8);*/
		KEEP(*(.hal.rom.rodata*))
		KEEP(*(.libc.rom.rodata*))

		/* to compatiable with IAR compiler, the rom.rodata (especial the text string) must be located at the bottom of ROM */
		KEEP(*(.rom.rodata*))
	} > KM0_DROM

	.wifi.rom :
	{
		KEEP(*(.wifi.rom.text*))
		KEEP(*(.wifi.rom.rodata*))
	} > WIFI_ROM

	/* addr fixed */
	.hal.rom.bss :
	{
		__rom_bss_start__ = .;
		KEEP(*(.ram_vector_table1))
		KEEP(*(.ram_vector_table2))
		KEEP(*(.ram_vector_table3))

		KEEP(*(.hal.rom.bss*))
		KEEP(*(.hal.rom.bank.bss*))
		KEEP(*(.libc.ram.bss*))

		KEEP(*(.wifi.rom.bss))
		__rom_bss_end__ = .;
	} >  KM0_ROMBSS_RAM

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
		KEEP(*(.wifi.rom.data*))
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
