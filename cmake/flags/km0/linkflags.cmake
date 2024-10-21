
set(
    LINK_OPTIONS
    -L${ASDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.base
    -L${ASDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.base
    -L${PROJECTDIR}/asdk/lib/

    -march=armv8-m.base
    -mthumb
    -save-temps

    -nostartfiles
    "SHELL:-specs nosys.specs"
    "SHELL:-specs nano.specs"

    -Wl,--cref
    -Wl,--build-id=none

    -Wl,--no-enum-size-warning
    -Wl,--warn-common
)

if(CONFIG_AMEBAD)
    list(APPEND LINK_OPTIONS -Os)
else()
    list(APPEND LINK_OPTIONS -O2)
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
)

list(
    APPEND LINK_OPTIONS
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