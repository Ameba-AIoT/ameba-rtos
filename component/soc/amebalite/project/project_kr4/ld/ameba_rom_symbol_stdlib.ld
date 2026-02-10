SECTIONS
{
    PROVIDE(__rom_stdlib_text_start__ = 0x1c000);
    PROVIDE(__rom_stdlib_text_end__ = 0x2b8c0);
    PROVIDE(__rom_stdlib_data_start__ = 0x20002000);
    PROVIDE(__rom_stdlib_data_end__ = 0x200029aa);
    PROVIDE(__rom_stdlib_bss_start__ = 0x200029c0);
    PROVIDE(__rom_stdlib_bss_end__ = 0x20002a24);
    PROVIDE(__rom_stdlib_heap_start__ = 0x20002a40);
}
