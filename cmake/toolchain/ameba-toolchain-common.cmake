if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
	set(EXE_SUFFIX)
else()
	set(EXE_SUFFIX ".exe")
endif()

set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc${EXE_SUFFIX})
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++${EXE_SUFFIX})
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc${EXE_SUFFIX})
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy${EXE_SUFFIX})
set(CMAKE_OBJDUMP ${CROSS_COMPILE}objdump${EXE_SUFFIX})
set(CMAKE_SIZE ${CROSS_COMPILE}size${EXE_SUFFIX})
set(CMAKE_STRIP ${CROSS_COMPILE}strip${EXE_SUFFIX})
set(CMAKE_AR ${CROSS_COMPILE}ar${EXE_SUFFIX})
set(CMAKE_AS ${CROSS_COMPILE}as${EXE_SUFFIX})
set(CMAKE_NM ${CROSS_COMPILE}nm${EXE_SUFFIX})
set(CMAKE_LD ${CROSS_COMPILE}ld${EXE_SUFFIX})
set(CMAKE_GDB ${CROSS_COMPILE}gdb${EXE_SUFFIX})
set(CMAKE_READELF ${CROSS_COMPILE}readelf${EXE_SUFFIX})

# set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> rs <TARGET> <LINK_FLAGS> <OBJECTS>")
# use default AR option, which is -qc, and the old archive file will be removed before creating.
