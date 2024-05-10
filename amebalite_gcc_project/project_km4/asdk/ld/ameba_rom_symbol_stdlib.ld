SECTIONS
{
    PROVIDE(__rom_stdlib_text_start__ = 0x34000);
    PROVIDE(__rom_stdlib_text_end__ = 0x40280);
    PROVIDE(__rom_stdlib_data_start__ = 0x20006000);
    PROVIDE(__rom_stdlib_data_end__ = 0x200069ac);
    PROVIDE(__rom_stdlib_bss_start__ = 0x200069c0);
    PROVIDE(__rom_stdlib_bss_end__ = 0x20006a0c);
    PROVIDE(__rom_stdlib_heap_start__ = 0x20006a20);
}
