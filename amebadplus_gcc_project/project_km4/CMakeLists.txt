cmake_minimum_required(VERSION 3.20.0)

### GLOBAL DIRECTIONS AND VALUES DEFINE ###
set(PROJECTDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRCDIR ${PROJECTDIR}/src)
set(INCDIR ${PROJECTDIR}/inc)
set(BASEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
#remove ../.. in BASEDIR, because BASEDIR is used by so many CMakeLists files
cmake_path(NORMAL_PATH BASEDIR)


set(KM0_BUILDDIR ${PROJECTDIR}/../project_km0)
cmake_path(NORMAL_PATH KM0_BUILDDIR)
set(KM4_BUILDDIR ${PROJECTDIR})

set(TFM_NS_DIR	${PROJECTDIR}/src_tfm)


set(TARGETDIR ${BASEDIR}/component/soc/amebadplus)
set(CMAKE_FILES_DIR ${BASEDIR}/cmake)

set(FREERTOS_VER v10.4.3)
set(FREERTOSDIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER}/Source)
set(OSDIR ${BASEDIR}/component/os)
set(MBEDTLSDIR ${BASEDIR}/component/ssl/mbedtls-2.16.9)
set(WIFIFWDIR ${BASEDIR}/component/wifi/wifi_fw)
set(WIFIMAKEDIR ${BASEDIR}/component/wifi/wifi_make)

set(LDDIR ${CMAKE_CURRENT_SOURCE_DIR}/asdk/ld)

set(UTILITYDIR ${PROJECTDIR}/asdk/gnu_utility)
set(FLASHDLDIR ${PROJECTDIR}/asdk/flashloader)
set(GNU_SCRIPT ${UTILITYDIR}/gnu_script)
set(FLASHDLIMGDIR ${GNU_SCRIPT}/acut_loader)

set(TARGET_LIBSOC_FOLDER ${PROJECTDIR}/asdk/lib/soc)
set(TARGET_LIBAPP_FOLDER ${PROJECTDIR}/asdk/lib/application)

###########################################
if(NOT CMAKE_PROJECT_NAME)
    set(ASDK_VER 10.3.1)
    include (${CMAKE_FILES_DIR}/toolchain/ameba-toolchain-${ASDK_VER}.cmake)
endif()

project(km4)
message ("project : " ${PROJECT_NAME})

set(CMAKE_BUILD_TYPE DEBUG)
# set CMAKE_VERBOSE_MAKEFILE ON to print compile info
set(CMAKE_VERBOSE_MAKEFILE OFF) 

###########################################

#############   include  ##################

include (${CMAKE_FILES_DIR}/ccache.cmake)

include (${CMAKE_FILES_DIR}/extensions.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../menuconfig/.config_km4 ${CMAKE_CURRENT_BINARY_DIR}/.config_km4 COPYONLY)
import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config_km4)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/compileflags.cmake)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/linkflags.cmake)

include(${CMAKE_FILES_DIR}/axf2bin.cmake)

###########################################

if (CONFIG_MP_INCLUDED)
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image_mp)
    set(BUILD_TYPE MFG)
    set(ANALYZE_MP_IMG 1)

else()
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image)
    set(BUILD_TYPE NONE)
    set(ANALYZE_MP_IMG 0)
endif()

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

###ADD SUBDIRECTORIES ###
#they are all in asdk
add_subdirectory(asdk)
#########################
