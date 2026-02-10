ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"

SECTIONS
{
	.rom.text : { } > KM4TZ_IROM
	.share.rodata : {} > SHARE_DROM
	.rom.rodata : { } > KM4TZ_DROM
	.hal.rom.bss : { } > KM4TZ_ROMBSS_RAM_NS
	.hal.s_rom.bss : { } > KM4TZ_ROMBSS_RAM_S

	/* data & bss should have not any content */
	.ram.data :
	{
		KEEP(*(.data*))
		KEEP(*(.bss*))
	} > EMPTY_SECTION

	/* should not any content in toolchain */
	.rom.text.libgcc :
	{
		KEEP(*(.text*))
		KEEP(*(.rodata*))
	} > EMPTY_SECTION
}
