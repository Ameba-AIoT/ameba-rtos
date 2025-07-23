ENTRY(Reset_Handler)

#include "../../../ameba_layout.ld"

SECTIONS
{
	/* addr fixed */
	.rom_wifi.text :
	{
		AA__wifi_share_rom_start__ = .;
		KEEP(*(.wifi.rom.text*))
		KEEP(*(.wifi.rom.sramdram.only.text*))
		KEEP(*(.wifi.rom.rodata*))
		KEEP(*(.wifi.rom.hal.rom.rodata*))
		AA__wifi_share_rom_end__ = .;
	} > WIFI_ROM

	.rom_wifi.bss :
	{
		. = 0x4C0;
		AA__wifi_share_bss_start__ = .;
		KEEP(*(.wifi.rom.bss))
		KEEP(*(.wifi.rom.hal.rom.bss*))
		AA__wifi_share_bss_end__ = . + 1;
	} > KM4TZ_ROMBSS_RAM_NS

	/* data & bss should have not any content */
	.ram.data :
	{
		KEEP(*(.data*))
		KEEP(*(.wifi.rom.data*))
		KEEP(*(.bss*))
	} > EMPTY_SECTION

	/* should not any content in toolchain */
	.rom.text.libgcc :
	{
		KEEP(*(.text*))
		KEEP(*(.rodata*))
	} > EMPTY_SECTION
}

ASSERT(AA__wifi_share_bss_end__ <= (AA__wifi_share_bss_start__ + 0x830), "assert: bcut wifi rom bss is out of range.")

