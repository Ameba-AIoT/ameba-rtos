set(c_GLOBAL_COMMON_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_COMMON_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_COMMON_LINK_OPTIONS
    -O2
    -nostartfiles
    "SHELL:-specs nosys.specs"
    # Add -Wl,--gc-sections in actual target for not rom compile.
    -Wl,--cref
    -Wl,--build-id=none
    -save-temps
    -Wl,--warn-common
)

# if no SHELL: -Wl,-wrap,memchr and -Wl,-wrap,memcmp may be merged as -Wl,-wrap,memchr,memcmp
# wrap function are defined in strwrap.c
set(
    LINK_OPTIONS_ROM_WRAP
    "SHELL:-Wl,-wrap,memchr"
    "SHELL:-Wl,-wrap,memcmp"
    "SHELL:-Wl,-wrap,memcpy"
    "SHELL:-Wl,-wrap,memmove"
    "SHELL:-Wl,-wrap,memset"
    "SHELL:-Wl,-wrap,rand"
    "SHELL:-Wl,-wrap,strcmp"
    "SHELL:-Wl,-wrap,stricmp"
    "SHELL:-Wl,-wrap,strlen"
    "SHELL:-Wl,-wrap,strtoul"
    "SHELL:-Wl,-wrap,strupr"
    "SHELL:-Wl,-wrap,strcpy"
    "SHELL:-Wl,-wrap,strncpy"
    "SHELL:-Wl,-wrap,strtol"
    # "SHELL:-Wl,-wrap,sprintf" # only allowed for ROM code
    # "SHELL:-Wl,-wrap,printf"
)
ameba_list_append(c_GLOBAL_COMMON_LINK_OPTIONS ${LINK_OPTIONS_ROM_WRAP})

# wrap function are defined in gcc_wrap.c
ameba_list_append(c_GLOBAL_COMMON_LINK_OPTIONS
    "SHELL:-Wl,-wrap,calloc"
    "SHELL:-Wl,-wrap,free"
    "SHELL:-Wl,-wrap,malloc"
    "SHELL:-Wl,-wrap,realloc"
    "SHELL:-Wl,-wrap,_calloc_r"
    "SHELL:-Wl,-wrap,_free_r"
    "SHELL:-Wl,-wrap,_malloc_r"
    "SHELL:-Wl,-wrap,_realloc_r"
    "SHELL:-Wl,-wrap,printf"
)

# wrap function are defined in vfs_wrap.c
if (CONFIG_WHC_HOST OR CONFIG_WHC_NONE)
ameba_list_append(c_GLOBAL_COMMON_LINK_OPTIONS
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
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
