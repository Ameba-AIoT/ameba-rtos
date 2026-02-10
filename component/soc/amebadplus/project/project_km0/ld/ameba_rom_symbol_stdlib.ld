SECTIONS
{
    PROVIDE(__rom_stdlib_text_start__ = 0x6000);
    PROVIDE(stdlib_loguart_relocation = 0x6815);
    PROVIDE(__rom_stdlib_text_end__ = 0x97e0);
    PROVIDE(__rom_stdlib_data_start__ = 0x20002000);
    PROVIDE(__rom_stdlib_data_end__ = 0x20002064);
    PROVIDE(__rom_stdlib_bss_start__ = 0x20002080);
    PROVIDE(rom_customer_log = 0x20002080);
    PROVIDE(__rom_stdlib_bss_end__ = 0x200020a4);
    PROVIDE(__rom_stdlib_heap_start__ = 0x200020c0);
}
