cmake_minimum_required(VERSION 3.20.0)

### GLOBAL DIRECTIONS AND VALUES DEFINE ###

set(PROJECTDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRCDIR ${PROJECTDIR}/src)
set(INCDIR ${PROJECTDIR}/inc)
set(BASEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
#remove ../.. in BASEDIR, because BASEDIR is used by so many CMakeLists files
cmake_path(NORMAL_PATH BASEDIR)

set(KM0_BUILDDIR ${PROJECTDIR})
set(KM4_BUILDDIR ${PROJECTDIR}/../project_km4)
cmake_path(NORMAL_PATH KM4_BUILDDIR)

set(TARGETDIR ${BASEDIR}/component/soc/amebadplus)
set(BOOTLOADER ${BASEDIR}/component/soc/amebadplus/bootloader)
set(CMAKE_FILES_DIR ${BASEDIR}/cmake)

set(FREERTOS_VER v10.4.3)
set(FREERTOSDIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER}/Source)
set(OSDIR ${BASEDIR}/component/os)
set(MBEDTLSDIR ${BASEDIR}/component/ssl/mbedtls-2.16.9)
set(WIFIFWDIR ${BASEDIR}/component/wifi/wifi_fw)
set(WIFIMAKEDIR ${BASEDIR}/component/wifi/wifi_make)

set(LDDIR ${CMAKE_CURRENT_SOURCE_DIR}/asdk/ld)

set(IMAGESCRIPTDIR  ${BASEDIR}/../tools/image_scripts)

set(PREPENDTOOL python ${IMAGESCRIPTDIR}/axf2bin.py prepend_header)
set(OTAPREPENDTOOL python ${IMAGESCRIPTDIR}/axf2bin.py ota_prepend_header )
set(PADTOOL python ${IMAGESCRIPTDIR}/axf2bin.py pad)
set(IMAGETOOL python ${IMAGESCRIPTDIR}/axf2bin.py imagetool)


set(TARGET_LIBSOC_FOLDER ${PROJECTDIR}/asdk/lib/soc)
set(TARGET_LIBAPP_FOLDER ${PROJECTDIR}/asdk/lib/application)

###########################################
if(NOT CMAKE_PROJECT_NAME)
    set(ASDK_VER 10.3.1)
    include (${CMAKE_FILES_DIR}/toolchain/ameba-toolchain-${ASDK_VER}.cmake)
endif()

project(km0)
message ("project : " ${PROJECT_NAME})

set(CMAKE_BUILD_TYPE DEBUG)
# set CMAKE_VERBOSE_MAKEFILE ON to print compile info
set(CMAKE_VERBOSE_MAKEFILE OFF) 

###########################################

#############   include  ##################
include (${CMAKE_FILES_DIR}/ccache.cmake)

include (${CMAKE_FILES_DIR}/extensions.cmake)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../menuconfig/.config_km0 ${CMAKE_CURRENT_BINARY_DIR}/.config_km0 COPYONLY)
import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config_km0)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/compileflags.cmake)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/linkflags.cmake)

include(${CMAKE_FILES_DIR}/axf2bin.cmake)

###########################################

if (CONFIG_MP_INCLUDED)
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image_mp)
    set(BUILD_TYPE MFG)
    set(ANALYZE_MP_IMG  1)

else()
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image)
    set(BUILD_TYPE NONE)
    set(ANALYZE_MP_IMG  0)
endif()

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
###ADD SUBDIRECTORIES ###
#they are all in asdk
add_subdirectory(asdk)
#########################

