
INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	.rom.text : { } > KM4_IROM_NS
	.rom.rodata : { } > KM4_DROM_NS
	.hal.rom.bss : { } > KM4_ROMBSS_RAM_COM
	.hal.ns_rom.bss : { } > KM4_ROMBSS_RAM_NS

	/* secure ROM BSS, this section should in secure RAM, add this to link
	 * symbols to avoid compile error */
	.hal.s_rom.bss :
	{
		__rom_bss_start_s__ = ABSOLUTE(.);
		__rom_bss_end_s__ = __rom_bss_start_s__ + LENGTH(KM4_ROMBSS_RAM_S);
	} >  KM4_ROMBSS_RAM_S

	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
	} > KM4_BOOTLOADER_RAM_S

	/* data & bss should have not any content */
	.ram.data :
	{
		KEEP(*(.data*))
	} > KM4_BD_RAM
	.ram.bss :
	{
		KEEP(*(.bss*))
	} > KM4_BD_RAM
	.rom.text.libgcc :
	{
		KEEP(*(.text*))
	} > KM4_IROM_NS
}
