if (__INCLUDED_SWLIB_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_SWLIB_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_swlib INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_swlib_wrap INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_swlib p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/swlib/diag.c
	${d_SOC_PLATFORM_DIR}/swlib/memset.c
	${d_SOC_PLATFORM_DIR}/swlib/rand.c
	${d_SOC_PLATFORM_DIR}/swlib/memcpy.c
	${d_SOC_PLATFORM_DIR}/swlib/memcmp.c
	${d_SOC_PLATFORM_DIR}/swlib/memchr.c
	${d_SOC_PLATFORM_DIR}/swlib/memmove.c
	${d_SOC_PLATFORM_DIR}/swlib/strcmp.c
	${d_SOC_PLATFORM_DIR}/swlib/strlen.c
	${d_SOC_PLATFORM_DIR}/swlib/stricmp.c
	${d_SOC_PLATFORM_DIR}/swlib/strupr.c
	${d_SOC_PLATFORM_DIR}/swlib/strtoul.c
	${d_SOC_PLATFORM_DIR}/swlib/char2num.c
	${d_SOC_PLATFORM_DIR}/swlib/strwrap.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_swlib_wrap p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/swlib/gcc_wrap.c
)
#--------------------------#