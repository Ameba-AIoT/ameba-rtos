if (__INCLUDED_ZLIB_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_ZLIB_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_zlib INTERFACE)

#--------------------------#

ameba_target_set(src_${d_MCU_PROJECT_NAME}_zlib p_SCOPE interface
    p_COMPILE_OPTIONS
        -O3
        -Wno-error
    p_COMPILE_DEFINES
        USE_MMAP
    p_INCLUDES
        ${c_CMPT_UI_DIR}/third_party/zlib/interfaces/include
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src
    p_SOURCES
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/adler32.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/compress.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/crc32.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/deflate.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/gzclose.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/gzlib.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/gzread.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/gzwrite.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/infback.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/inflate.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/inftrees.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/inffast.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/trees.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/uncompr.c
        ${c_CMPT_UI_DIR}/third_party/zlib/zlib/src/zutil.c
)
#--------------------------#