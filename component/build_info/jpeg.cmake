if (__INCLUDED_JPEG_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_JPEG_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_jpeg INTERFACE)

#--------------------------#

ameba_target_set(src_${d_MCU_PROJECT_NAME}_jpeg p_SCOPE interface
    p_COMPILE_OPTIONS
        -O3
        -fstrict-aliasing
        -Wno-sign-compare
        -Wno-unused-parameter
        -Wno-error
    p_INCLUDES
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/interfaces/include
    p_SOURCES
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcapimin.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcapistd.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jaricom.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcarith.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jccoefct.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jccolor.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcdctmgr.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jchuff.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcinit.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcmainct.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcmarker.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcmaster.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcomapi.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcparam.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcphuff.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcprepct.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jcsample.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jctrans.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdapimin.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdapistd.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdarith.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdatadst.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdatasrc.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdcoefct.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdcolor.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jddctmgr.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdhuff.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdinput.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdmainct.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdmarker.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdmaster.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdmerge.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdphuff.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdpostct.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdsample.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jdtrans.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jerror.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jfdctflt.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jfdctfst.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jfdctint.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jidctflt.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jidctfst.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jidctint.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jidctred.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jmemmgr.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jmemnobs.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jquant1.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jquant2.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/jutils.c
        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/simd/jsimd_arm.c

        ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/libjpeg-turbo/simd/jsimd_arm_neon.S
)
#--------------------------#