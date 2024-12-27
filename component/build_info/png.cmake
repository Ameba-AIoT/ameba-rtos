if (__INCLUDED_PNG_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_PNG_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_png INTERFACE)

#--------------------------#

ameba_target_set(src_${d_MCU_PROJECT_NAME}_png p_SCOPE interface
    p_COMPILE_OPTIONS
        --std=gnu89
        -Wall
        -Wno-unused-parameter
        -Wno-error
    p_INCLUDES
        ${c_CMPT_UI_DIR}/third_party/libpng/interfaces/include
        ${c_CMPT_UI_DIR}/third_party/zlib/interfaces/include
    p_SOURCES
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/png.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngerror.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngget.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngmem.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngpread.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngread.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngrio.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngrtran.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngrutil.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngset.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngtrans.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngwio.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngwrite.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngwtran.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/pngwutil.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/arm/arm_init.c
        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/arm/filter_neon_intrinsics.c

        ${c_CMPT_UI_DIR}/third_party/libpng/libpng/arm/filter_neon.S
)
#--------------------------#