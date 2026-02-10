
INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	/* addr fixed */
	.rom.text :
	{
		. = 0xC00;
		KEEP(*(.vectors_table))
		KEEP(*(.vectors_table_text))
		KEEP(*(.hal.rom.text*))
		KEEP(*(.libc.rom.text*))
		KEEP(*(.ssl.rom.text*))

		KEEP(*(.rom.text*))
	} > KM4_IROM_NS

	.rom.rodata :
	{
		__rom_entry_ns_start__ = .;
		KEEP(*(.hal.rom.entryns.rodata*))
		__rom_entry_ns_end__ = .;

		KEEP(*(.hal.rom.rodata*))
		KEEP(*(.libc.rom.rodata*))
		KEEP(*(.ssl.rom.rodata*))

		/* to compatiable with IAR compiler, the rom.rodata (especial the text string) must be located at the bottom of ROM */
		KEEP(*(.rom.rodata*))
	} > KM4_DROM_NS

	/* addr fixed */
	.hal.rom.bss :
	{
		__rom_bss_start__ = .;
		KEEP(*diag.o(.hal.rom.bss*))
		KEEP(*ameba_flash_rom.o(.hal.rom.bss*))
		KEEP(*shell_rom.o(.hal.rom.bss*))
		KEEP(*ameba_crypto_rom.o(.hal.rom.bss*))
		KEEP(*mbedtls_rom_ecp.o(.hal.rom.bss*))
		KEEP(*mbedtls_rom_ecp_curves.o(.hal.rom.bss*))
		KEEP(*ameba_delay_rom.o(.hal.rom.bss*))
		KEEP(*ameba_gpio_rom.o(.hal.rom.bss*))

		__rom_bss_chk__ = .;
		KEEP(*(.hal.rom.bss*))
		__rom_bss_end__ = .;
	} >  KM4_ROMBSS_RAM_COM

	/* addr fixed */
	.hal.ns_rom.bss :
	{
		__rom_bss_start_ns__ = .;
		KEEP(*(.ram_vector_table1))
		KEEP(*(.ram_vector_table2))
		KEEP(*(.ram_vector_table3))
		KEEP(*(.hal.rom.bank.bss*))

		KEEP(*(.ssl_ram_map*))
		__rom_bss_end_ns__ = .;
	} >  KM4_ROMBSS_RAM_NS

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

	/* retention RAM entry, this section should in RAM and fixed address for ROM */
	.ram_retention.entry :
	{
		__retention_entry_func__ = ABSOLUTE(.);
	} > RETENTION_RAM
}

ASSERT(__rom_bss_end__ == __rom_bss_chk__, "KM4_ROMBSS_RAM_COM is changed.")