
set(LINK_OPTIONS)



list(
    APPEND LINK_OPTIONS
    -L${VSDK_TOOLCHAIN}/riscv32-none-elf/lib/rv32imafc/ilp32f
    -L${VSDK_TOOLCHAIN}/lib/gcc/riscv32-none-elf/${VSDK_VER}/rv32imafc/ilp32f
    -L${PROJECTDIR}/vsdk/lib/
    -Os
    -march=rv32imafc -mabi=ilp32f
    -nostartfiles
    -specs nosys.specs

    -Wl,--cref
    -Wl,--build-id=none
    -save-temps
    -Wl,--warn-common
    -msafe-relax
    -Wl,--start-group -lc -lgcc  -Wl,--end-group
)

if(CONFIG_CPU_HAS_FPU)
    list(
        APPEND LINK_OPTIONS
        -mfloat-abi=hard
        -mfpu=fpv5-sp-d16
    )
endif()

set(
    LINK_OPTIONS_WRAP
    "SHELL:-Wl,-wrap,memchr"
    "SHELL:-Wl,-wrap,memcmp"
    "SHELL:-Wl,-wrap,memcpy"
    "SHELL:-Wl,-wrap,memmove"
    "SHELL:-Wl,-wrap,memset"
    "SHELL:-Wl,-wrap,strcmp"
    "SHELL:-Wl,-wrap,strlen"
    "SHELL:-Wl,-wrap,strtoul"
    "SHELL:-Wl,-wrap,strupr"
    "SHELL:-Wl,-wrap,stricmp"
    "SHELL:-Wl,-wrap,printf"
    "SHELL:-Wl,-wrap,malloc"
    "SHELL:-Wl,-wrap,realloc"
    "SHELL:-Wl,-wrap,free"
    "SHELL:-Wl,-wrap,calloc"
    "SHELL:-Wl,-wrap,_malloc_r"
    "SHELL:-Wl,-wrap,_realloc_r"
    "SHELL:-Wl,-wrap,_free_r"
    "SHELL:-Wl,-wrap,_calloc_r"
    "SHELL:-Wl,-wrap,rand"
    "SHELL:-Wl,-wrap,fopen"
    "SHELL:-Wl,-wrap,fclose"
    "SHELL:-Wl,-wrap,fread"
    "SHELL:-Wl,-wrap,fwrite"
    "SHELL:-Wl,-wrap,fseek"
    "SHELL:-Wl,-wrap,fsetpos"
    "SHELL:-Wl,-wrap,fgetpos"
    "SHELL:-Wl,-wrap,rewind"
    "SHELL:-Wl,-wrap,fflush"
    "SHELL:-Wl,-wrap,remove"
    "SHELL:-Wl,-wrap,rename"
    "SHELL:-Wl,-wrap,feof"
    "SHELL:-Wl,-wrap,ferror"
    "SHELL:-Wl,-wrap,ftell"
    "SHELL:-Wl,-wrap,ftruncate"
    "SHELL:-Wl,-wrap,fputc"
    "SHELL:-Wl,-wrap,fputs"
    "SHELL:-Wl,-wrap,fgets"
    "SHELL:-Wl,-wrap,stat"
    "SHELL:-Wl,-wrap,mkdir"
    "SHELL:-Wl,-wrap,scandir"
    "SHELL:-Wl,-wrap,readdir"
    "SHELL:-Wl,-wrap,opendir"
    "SHELL:-Wl,-wrap,access"
    "SHELL:-Wl,-wrap,rmdir"
    "SHELL:-Wl,-wrap,closedir"
)