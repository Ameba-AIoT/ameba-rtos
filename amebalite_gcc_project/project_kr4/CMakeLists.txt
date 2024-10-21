cmake_minimum_required(VERSION 3.20.0)

### GLOBAL DIRECTIONS AND VALUES DEFINE ###
set(PROJECTDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRCDIR ${PROJECTDIR}/src)
set(INCDIR ${PROJECTDIR}/inc)
set(BASEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
#remove ../.. in BASEDIR, because BASEDIR is used by so many CMakeLists files
cmake_path(NORMAL_PATH BASEDIR)


set(KM4_BUILDDIR ${PROJECTDIR}/../project_km4)
cmake_path(NORMAL_PATH KM4_BUILDDIR)
set(KR4_BUILDDIR ${PROJECTDIR})


set(TARGETDIR ${BASEDIR}/component/soc/amebalite)
set(CMAKE_FILES_DIR ${BASEDIR}/cmake)

set(FREERTOS_VER v10.4.3)
set(MBEDTLS_VER 2.16.9)
set(LITTLEFS_VER r2.50)
set(FATFS_VER r0.14b)
set(LWIP_VER v2.1.2)

set(FREERTOSDIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER}/Source)
set(OSDIR ${BASEDIR}/component/os)
set(MBEDTLSDIR ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER})
set(WIFIFWDIR ${BASEDIR}/component/wifi/wifi_fw)
set(WIFIMAKEDIR ${BASEDIR}/component/wifi/wifi_make)

set(LDDIR ${CMAKE_CURRENT_SOURCE_DIR}/vsdk/ld)

set(UTILITYDIR ${PROJECTDIR}/vsdk/gnu_utility)
set(GNU_SCRIPT ${UTILITYDIR}/gnu_script)
set(FLASHDLIMGDIR ${GNU_SCRIPT}/acut_loader)
set(FLOADER_BIN_DIR ${UTILITYDIR}/image_tool_flashloader/amebalite_acut)

set(TARGET_LIBSOC_FOLDER ${PROJECTDIR}/vsdk/lib/soc)
set(TARGET_LIBAPP_FOLDER ${PROJECTDIR}/vsdk/lib/application)

###########################################
if(NOT VSDK_VER)
    set(VSDK_VER 10.3.1)
    include (${CMAKE_FILES_DIR}/toolchain/ameba-toolchain-vsdk-${VSDK_VER}.cmake)
    set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_FILES_DIR}/set_extensions.cmake)
    set(CMAKE_PROJECT_INCLUDE_BEFORE ${CMAKE_FILES_DIR}/disable_compiler_detection.cmake)
endif()

project(kr4 LANGUAGES C CXX ASM)
message ("project : " ${PROJECT_NAME})

set(CMAKE_BUILD_TYPE "")
# set CMAKE_VERBOSE_MAKEFILE ON to print compile info
set(CMAKE_VERBOSE_MAKEFILE OFF)

###########################################

#############   include  ##################

include (${CMAKE_FILES_DIR}/ccache.cmake)

include (${CMAKE_FILES_DIR}/extensions.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../menuconfig/.config_kr4 ${CMAKE_CURRENT_BINARY_DIR}/.config_kr4 COPYONLY)
import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config_kr4)
include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/compileflags.cmake)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/linkflags.cmake)

include(${CMAKE_FILES_DIR}/axf2bin.cmake)

include(${CMAKE_FILES_DIR}/git/GetGitRevisionDescription.cmake)
git_describe(GIT_VER)
###########################################

if(CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
    set(AP_IMAGEDIR ${KM4_BUILDDIR}/vsdk/image)
else()
    set(AP_IMAGEDIR ${KR4_BUILDDIR}/vsdk/image)
endif()

if (CONFIG_MP_INCLUDED)
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/vsdk/image_mp)
    set(BUILD_TYPE MFG)
    set(ANALYZE_MP_IMG 1)

else()
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/vsdk/image)
    set(BUILD_TYPE NONE)
    set(ANALYZE_MP_IMG 0)
endif()

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

message("CACHE EXAMPLE: " ${EXAMPLE})

###ADD SUBDIRECTORIES ###
#they are all in vsdk
add_subdirectory(vsdk)
#########################
