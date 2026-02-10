ENTRY(Reset_Handler)

#include "../../ameba_layout.ld"

SECTIONS
{
	/* addr fixed */
	.rom.text :
	{
		KEEP(*(.vectors_table))
		KEEP(*(.vectors_table_text))

		. = ALIGN (32);
		__rom_xo_start__ = .;
		KEEP(*(.apple.rom.text*))

		KEEP(*ameba_crypto_aes.o(.hal.rom.text*))
		KEEP(*ameba_crypto_sha.o(.hal.rom.text*))
		KEEP(*ameba_crypto.o(.hal.rom.text*))
		KEEP(*ameba_pke_ecc.o(.hal.rom.text*))
		KEEP(*ameba_pke_rsa.o(.hal.rom.text*))
		KEEP(*ameba_pke_common.o(.hal.rom.text*))
		KEEP(*ameba_crypto_rsip.o(.hal.rom.text*))
		KEEP(*ameba_rsip_rom.o(.hal.rom.text*))

		KEEP(*ameba_secure_boot_s.o(.hal.rom.text*))
		/*separate boot code with dirver*/
		KEEP(*ameba_boot_hci_s.o(.hal.rom.text*))
		KEEP(*ameba_boot_rom_s.o(.hal.rom.text*))
		. = 0x7800;
		__rom_xo_end__ = .;

		KEEP(*(.share.rom.text*))
		KEEP(*(.libc.rom.text*))
		KEEP(*(.hal.rom.text*))
		KEEP(*(.ssl.rom.text*))
	} > KM4TZ_IROM

	/* NS func entry, this section should in NS_ROM and fixed address for S_ROM */
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
	} > SHARE_DROM

	.rom.rodata :
	{
		KEEP(*(.rom_vector_table1))
		KEEP(*(.hal.rom.rodata*))
		KEEP(*(.hal.rom.bank.rodata*))

		KEEP(*(.apple.rom.rodata*))
		KEEP(*(.ssl.rom.rodata*))

		/* to compatible with IAR compiler, the rom.rodata (especial the text string) must be located at the bottom of ROM */
		KEEP(*(.rom.rodata*))
	} > KM4TZ_DROM

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
	} >  KM4TZ_ROMBSS_RAM_NS

	/* addr fixed */
	.hal.s_rom.bss :
	{
		__rom_bss_start_s__ = .;

		KEEP(*(.hal.rom.bank.bss*))
		__rom_bss_end_s__ = .;

	} >  KM4TZ_ROMBSS_RAM_S

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

ASSERT(__rom_bss_end__ == __rom_bss_chk__, "assert: [__rom_bss_chk__, __rom_bss_end__] shall empty.")
