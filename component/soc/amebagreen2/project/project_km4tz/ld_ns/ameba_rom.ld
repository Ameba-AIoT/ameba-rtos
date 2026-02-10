
#include "../../ameba_layout.ld"

SECTIONS
{
	/* addr fixed */
	.ap_only.rom :
	{
		KEEP(*ameba_vector_rom.o(.hal.rom.text*))
		KEEP(*ameba_rsip_rom.o(.hal.rom.text*))
		KEEP(*ameba_crypto_rsip.o(.hal.rom.text*))
	} > KM4TZ_ONLY_ROM

	/* addr fixed */
	.delay.rom :
	{
		KEEP(*ameba_delay_rom.o(.hal.rom.text*))
	} > DELAY_ROM

	/* addr fixed */
	.share.text :
	{
		KEEP(*(.share.rom.text*))
		KEEP(*(.libc.rom.text*))
		KEEP(*(.hal.rom.text*))
	} > SHARE_IROM

	.share.rodata :
	{
		__drom_common_start__ = .;
		KEEP(*ameba_vector_rom.o(*.rom.rodata*))
		KEEP(*ameba_crypto_aes.o(*.rom.rodata*))
		KEEP(*ameba_crypto_sha.o(*.rom.rodata*))
		KEEP(*ameba_crypto.o(*.rom.rodata*))
		KEEP(*ameba_pke_ecc.o(*.rom.rodata*))
		KEEP(*ameba_pke_rsa.o(*.rom.rodata*))
		KEEP(*ameba_pke_common.o(*.rom.rodata*))
		KEEP(*(.libc.rom.rodata*))
		KEEP(*(.com.rom.rodata*))
		__drom_common_end__ = .;

		KEEP(*(.hal.rom.rodata*))

		__drom_bank_start__ = .;
		KEEP(*(.hal.rom.bank.rodata*))
		KEEP(*(.rom.rodata*))
		KEEP(*(.rom_vector_table1))
	} > SHARE_DROM

	.wifi.rom :
	{
		AA__wifi_share_rom_start__ = .;
		KEEP(*(.wifi.rom.text*))
		KEEP(*(.wifi.rom.rodata*))
		KEEP(*(.wifi.rom.hal.rom.rodata*))
		AA__wifi_share_rom_end__ = .;
	} > WIFI_ROM

	.os.rom :
	{
		AA__os_share_rom_start__ = .;
		KEEP(*(.os.rom.text*))
		KEEP(*(.os.rom.rodata*))
		AA__os_share_rom_end__ = .;
	} > OS_ROM

	/* addr fixed */
	.hal.rom.bss :
	{
		__rom_bss_start__ = .;
		KEEP(*(.ram_vector_table2))
		KEEP(*(.ram_vector_table3))

		KEEP(*ameba_flash_rom.o(.hal.rom.bss*))
		KEEP(*shell_rom.o(.hal.rom.bss*))
		KEEP(*ameba_gpio_rom.o(.hal.rom.bss*))
		KEEP(*log.o(.hal.rom.bss*))
		KEEP(*diag.o(.hal.rom.bss*))
		KEEP(*ameba_chipinfo_rom.o(.hal.rom.bss*))
		KEEP(*ameba_delay_rom.o(.hal.rom.bss*))
		KEEP(*ameba_dump_stack.o(.hal.rom.bss*))
		KEEP(*ameba_vector_rom.o(.hal.rom.bss*))
		__rom_bss_chk__ = .;
		KEEP(*(.hal.rom.bss*))
		__rom_bss_end__ = .;

		__rom_bss_start_ns__ = .;
		KEEP(*(.hal.rom.bank.bss*))

		AA__wifi_share_bss_start__ = .;
		KEEP(*(.wifi.rom.bss))
		KEEP(*(.wifi.rom.hal.rom.bss*))
		AA__wifi_share_bss_end__ = .;

		AA__os_share_bss_start__ = . + 1;
		KEEP(*(.os.rom.bss*))
		AA__os_share_bss_end__ = .;

		__retrieve_ap_buffer_start__ = (AA__os_share_bss_end__ + 31) & (0xFFFFFFE0);

		__rom_bss_end_ns__ = ORIGIN(KM4TZ_ROMBSS_RAM_NS) + LENGTH(KM4TZ_ROMBSS_RAM_NS);
	} >  KM4TZ_ROMBSS_RAM_NS

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

ASSERT(__rom_bss_end__ == __rom_bss_chk__, "assert: [__rom_bss_chk__, __rom_bss_end__] shall empty.")
ASSERT(__drom_bank_start__ == __drom_common_end__, "assert: [__drom_common_end__, __drom_bank_start__] shall empty.")
__retrieve_ap_buffer_size__ =ORIGIN(KM4TZ_MSP_RAM_NS) + LENGTH(KM4TZ_MSP_RAM_NS) - __retrieve_ap_buffer_start__;
