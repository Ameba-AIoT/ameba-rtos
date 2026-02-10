
#include "../../ameba_layout.ld"

SECTIONS
{
	.ap_only.rom : {} > KM4TZ_ONLY_ROM
	.share.text : { } > SHARE_IROM
	.share.rodata : { } > SHARE_DROM
	.wifi.rom : { } > WIFI_ROM
	.os.rom : { } > OS_ROM
	.delay.rom : { } > DELAY_ROM
	.hal.rom.bss : { } > KM4TZ_ROMBSS_RAM_NS

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
