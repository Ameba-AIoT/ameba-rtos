
INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	/* addr fixed */
	.rom.text :
	{
		. = 0x1C00;
		KEEP(*(.vectors_table))
		KEEP(*(.vectors_table_text))
		KEEP(*(.hal.rom.text*))
		KEEP(*(.libc.rom.text*))
		KEEP(*(.apple.rom.text*))
		KEEP(*(.ssl.rom.text*))
		KEEP(*(.FW.rom.text*))
	} > KM4_IROM_NS

	.rom.rodata :
	{
		__rom_entry_ns_start__ = .;
		KEEP(*(.hal.rom.entryns.rodata*))
		__rom_entry_ns_end__ = .;

		KEEP(*(.sim.rom.rodata*))
		KEEP(*(.hal.rom.rodata*))
		KEEP(*(.libc.rom.rodata*))
		KEEP(*(.apple.rom.rodata*))
		KEEP(*(.ssl.rom.rodata*))
		KEEP(*(.FW.rom.rodata*))

		/* to compatiable with IAR compiler, the rom.rodata (especial the text string) must be located at the bottom of ROM */
		KEEP(*(.rom.rodata*))

		KEEP(*(.rodata*))
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
		/* FW BSS */
		KEEP(*(.FW.rom.bss*))

		__rom_bss_end_ns__ = .;

	} >  KM4_ROMBSS_RAM_NS

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

	/* retention RAM entry, this section should in RAM and fixed address for ROM */
	.ram_retention.entry :
	{
		__retention_entry_func__ = ABSOLUTE(.);
	} > RETENTION_RAM
}
