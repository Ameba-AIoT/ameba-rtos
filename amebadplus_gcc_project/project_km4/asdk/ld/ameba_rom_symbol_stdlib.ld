SECTIONS
{
    PROVIDE(__rom_stdlib_text_start__ = 0x33000);
    PROVIDE(stdlib_loguart_relocation = 0x337bd);
    PROVIDE(__rom_stdlib_text_end__ = 0x40a40);
    PROVIDE(__rom_stdlib_data_start__ = 0x20006000);
    PROVIDE(__rom_stdlib_data_end__ = 0x200069ae);
    PROVIDE(__rom_stdlib_bss_start__ = 0x200069c0);
    PROVIDE(rom_customer_log = 0x200069c0);
    PROVIDE(__rom_stdlib_bss_end__ = 0x20006a14);
    PROVIDE(__rom_stdlib_heap_start__ = 0x20006a20);
}
