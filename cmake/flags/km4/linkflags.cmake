
set(LINK_OPTIONS)


if(CONFIG_CPU_HAS_FPU)
    list(
        APPEND LINK_OPTIONS
        -L${ASDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.main+fp/fpu
        -L${ASDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu
    )
endif()


if(CONFIG_AMEBAD)
    list(APPEND LINK_OPTIONS -march=armv8-m.main+dsp)
elseif(CONFIG_AMEBAGREEN2)
    list(APPEND LINK_OPTIONS -march=armv8.1-m.main)
elseif(CONFIG_AMEBAL2)
    list(APPEND LINK_OPTIONS -march=armv8.1-m.main)
else()
    list(APPEND LINK_OPTIONS -march=armv8.1-m.main+dsp)
endif()

if(CONFIG_AMEBALITE OR CONFIG_AMEBAD)
    list(APPEND LINK_OPTIONS -Os)
else()
    list(APPEND LINK_OPTIONS -O2)
endif()

list(
    APPEND LINK_OPTIONS
    -L${PROJECTDIR}/asdk/lib/

    -mthumb
    -mcmse
    -nostartfiles
    -specs nosys.specs

    -Wl,--cref
    -Wl,--build-id=none
    -save-temps
    -Wl,--no-enum-size-warning
    -Wl,--warn-common
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

)

if(CONFIG_AMEBAGREEN2)
    list(
        APPEND LINK_OPTIONS_WRAP
        "SHELL:-Wl,-wrap,strcpy"
        "SHELL:-Wl,-wrap,strncpy"
        "SHELL:-Wl,-wrap,strtol"
        "SHELL:-Wl,-wrap,sprintf"
        "SHELL:-Wl,-wrap,readdir"
        "SHELL:-Wl,-wrap,opendir"
        "SHELL:-Wl,-wrap,closedir"
        "SHELL:-Wl,-wrap,mkdir"
    )
elseif(CONFIG_AMEBAL2)
    list(
        APPEND LINK_OPTIONS_WRAP
        "SHELL:-Wl,-wrap,strcpy"
        "SHELL:-Wl,-wrap,strncpy"
        "SHELL:-Wl,-wrap,strtol"
        "SHELL:-Wl,-wrap,sprintf"
        "SHELL:-Wl,-wrap,readdir"
        "SHELL:-Wl,-wrap,opendir"
        "SHELL:-Wl,-wrap,closedir"
    )
else()
    list(
        APPEND LINK_OPTIONS_WRAP
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
endif()


# This is rom wrap for amebagreen2
set(
    LINK_OPTIONS_ROM_WRAP
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
    "SHELL:-Wl,-wrap,rand"
    "SHELL:-Wl,-wrap,strcpy"
    "SHELL:-Wl,-wrap,strncpy"
    "SHELL:-Wl,-wrap,strtol"
    "SHELL:-Wl,-wrap,sprintf"
)