
#include "../../../ameba_layout.ld"

SECTIONS
{
	.rom_wifi.text : {} > WIFI_ROM
	.rom_wifi.bss : {} > KM4TZ_ROMBSS_RAM_NS

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
