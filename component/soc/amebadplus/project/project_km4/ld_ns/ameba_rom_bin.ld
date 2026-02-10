
INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	.rom.text : { } > KM4_IROM_NS
	.rom.rodata : { } > KM4_DROM_NS
	.hal.rom.bss : { } > KM4_ROMBSS_RAM_COM
	.hal.ns_rom.bss : { } > KM4_ROMBSS_RAM_NS

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
