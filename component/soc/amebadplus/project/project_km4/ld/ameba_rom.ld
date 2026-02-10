ENTRY(Reset_Handler)

INCLUDE "../../ameba_layout.ld"

SECTIONS
{
	/* addr fixed */
	.rom.text :
	{
		KEEP(*(.vectors_table))

		. = 0x140;
		__rom_xo_start__ = .;
		KEEP(*ameba_crypto_rom.o(.hal.rom.text*))
		KEEP(*ameba_secure_boot.o(.hal.rom.text*))
		/* KEEP(*ameba_ecdsa.o(.hal.rom.text*)) */
		/* KEEP(*ameba_rsa.o(.hal.rom.text*)) */
		KEEP(*ameba_aes.o(.hal.rom.text*))
		KEEP(*ameba_sha.o(.hal.rom.text*))
		KEEP(*ameba_md5.o(.hal.rom.text*))
		KEEP(*ameba_otf_rom.o(.hal.rom.text*))
		KEEP(*(.apple.rom.text*))
		/*separate boot code with dirver*/
		KEEP(*ameba_boot_rom.o(.hal.rom.text*))
		__rom_xo_end__ = .;

		. = 0x9000;
		/* a fixed addr for fpga download */
		KEEP(*(.vectors_table_text))
		KEEP(*(.hal.rom.text*))
		KEEP(*(.libc.rom.text*))
		KEEP(*(.ssl.rom.text*))
		KEEP(*(.rom.text*))
	} > KM4_IROM

	.rom.rodata :
	{
		KEEP(*(.hal.rom.rodata*))
		KEEP(*(.libc.rom.rodata*))
		KEEP(*(.apple.rom.rodata*))
		KEEP(*(.ssl.rom.rodata*))

		/* to compatiable with IAR compiler, the rom.rodata (especial the text string) must be located at the bottom of ROM */
		KEEP(*(.rom.rodata*))
	} > KM4_DROM

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
	.hal.s_rom.bss :
	{
		__rom_bss_start_s__ = .;
		KEEP(*(.ram_vector_table1))
		KEEP(*(.ram_vector_table2))
		KEEP(*(.ram_vector_table3))
		KEEP(*(.hal.rom.bank.bss*))

		KEEP(*(.ssl_ram_map*))
		__rom_bss_end_s__ = .;

	} >  KM4_ROMBSS_RAM_S

	/* image1 entry, this section should in RAM and fixed address for ROM */
	.ram_image1.entry :
	{
		__ram_start_table_start__ = .;
		KEEP(*(SORT(.image1.entry.data*)))
		KEEP(*(.image1.validate.rodata*))
	} > KM4_BOOTLOADER_RAM_S

	/* NS func entry, this section should in NS_ROM and fixed address for S_ROM */
	.ns_rom.rodata :
	{
		__rom_entry_ns_start__ = ABSOLUTE(.);
	} > KM4_DROM_NS

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