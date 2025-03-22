if(NOT CMAKE_REFACTOR) # For temporary compatibility
if (WIN32)
    set(CMAKE_SYSTEM_NAME Generic)
endif()
cmake_minimum_required(VERSION 3.20.0)

## vscode extension support

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE ${EXAMPLE} CACHE STRING "Example Name. If EXAMPLE is specified by user,\
the floder named <EXAMPLE> will be searched from components/example.\
e.g., EXAPMLE=ssl_client. Default example name is NULL")

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE_FOR_CA32 ${EXAMPLE_FOR_CA32} CACHE STRING "Example Name for ca32. If EXAMPLE_FOR_CA32 is specified by user,\
the floder named <EXAMPLE_FOR_CA32> will be searched from components/example.\
e.g., EXAMPLE_FOR_CA32=ssl_client. Default example name is NULL")

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE_FOR_KM4 ${EXAMPLE_FOR_KM4} CACHE STRING "EXAMPLE Name for km4. If EXAMPLE_FOR_KM4 is specified by user,\
the floder named <EXAMPLE_FOR_KM4> will be searched from components/example.\
e.g., EXAMPLE_FOR_KM4=ssl_client. Default example name is NULL")

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE_FOR_KM0 ${EXAMPLE_FOR_KM0} CACHE STRING "Example Name for km0. If EXAMPLE_FOR_KM0 is specified by user,\
the floder named <EXAMPLE_FOR_KM0> will be searched from components/example.\
e.g., EXAMPLE_FOR_KM0=ssl_client. Default example name is NULL")

#NOTICE: Cache variable USER_SRC_DIR will be deleted at the end.
set(USER_SRC_DIR ${USER_SRC_DIR} CACHE PATH "User source code floader containing main.c\
and CMakeLists.txt. If USER_SRC_DIR is specified by user, the project will add this floader\
as a subdirectory. USER_SRC_DIR can be a absolute path or a path relative to 'BASEDIR', \
which is parent floder of component. e.g., USER_SRC_DIR=/home/user/src, USER_SRC_DIR=component/example/xxx/src.\
Default source code floader is amebadeplus_gcc_project/project/project_km4/src")

#NOTICE: Cache variable TOOLCHAIN_DIR will be deleted at the end.
set(TOOLCHAIN_DIR ${TOOLCHAIN_DIR} CACHE PATH "Toolchain installation path. If TOOLCHAIN_DIR is specified\
by user, cross-compile toolchain will be searched in <TOOLCHAIN_DIR>. Default toolchain installation path is\
'/opt/rtk-toolchain' in linux and 'C:/rtk-toolchain' in windows.")

#NOTICE: Cache variable FINAL_IMAGE_DIR will be deleted at the end.
set(FINAL_IMAGE_DIR ${FINAL_IMAGE_DIR} CACHE PATH "Final images path, i.e., xxx_boot_all.bin, ota_all.bin \
and xxx_app.bin final path. If FINAL_IMAGE_DIR is specified by user, final images will be copied to \
<FINAL_IMAGE_DIR> after building. Default final image directory is project root path, \
where the current CMakeLists.txt is located.")

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ameba.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/platform_project_config.cmake)

set(c_SOC_TYPE amebasmart)

set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_INFO})
ameba_reset_global_define()

ameba_platform_project_create()
ameba_platform_project_init()

ameba_platform_project_config() #ameba_platform_project_custom_config is called internally and automatically
git_describe(GIT_VER)

set(CMAKE_BUILD_TYPE "")

ameba_set(ASDK_VER ${v_ASDK_VER})
# must include toolchain before project(xx LANGUAGES C CXX ASM)
include (${c_CMAKE_FILES_DIR}/toolchain/ameba-toolchain-asdk-${v_ASDK_VER}.cmake)

project(amebasmart LANGUAGES C CXX ASM)

ameba_info("top project : ${PROJECT_NAME}, platform_type: ${d_PLATFORM_TYPE}, gitver: ${GIT_VER}")

add_subdirectory(project_ap)
add_subdirectory(project_hp)
add_subdirectory(project_lp)

set(DAILY_BUILD 0 CACHE STRING "code analysis argument" FORCE)
unset(EXAMPLE CACHE)
unset(EXAMPLE_FOR_CA32 CACHE)
unset(EXAMPLE_FOR_KM4 CACHE)
unset(EXAMPLE_FOR_KM0 CACHE)
unset(USER_SRC_DIR CACHE)
unset(TOOLCHAIN_DIR CACHE)
unset(FINAL_IMAGE_DIR CACHE)
else() #For temporary compatibility.NOTE:  script below is new version cmake
if (WIN32)
    set(CMAKE_SYSTEM_NAME Generic)
endif()
cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake2/ameba.cmake)

###############################################################################
## vscode extension support
#OPTIMIZE: move these to ameba_soc_project_create?
#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE ${EXAMPLE} CACHE STRING "Example Name. If EXAMPLE is specified by user,\
the floder named <EXAMPLE> will be searched from components/example.\
e.g., EXAPMLE=ssl_client. Default example name is NULL")

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE_FOR_CA32 ${EXAMPLE_FOR_CA32} CACHE STRING "Example Name for ca32. If EXAMPLE_FOR_CA32 is specified by user,\
the floder named <EXAMPLE_FOR_CA32> will be searched from components/example.\
e.g., EXAMPLE_FOR_CA32=ssl_client. Default example name is NULL")

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE_FOR_KM4 ${EXAMPLE_FOR_KM4} CACHE STRING "EXAMPLE Name for km4. If EXAMPLE_FOR_KM4 is specified by user,\
the floder named <EXAMPLE_FOR_KM4> will be searched from components/example.\
e.g., EXAMPLE_FOR_KM4=ssl_client. Default example name is NULL")

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE_FOR_KM0 ${EXAMPLE_FOR_KM0} CACHE STRING "Example Name for km0. If EXAMPLE_FOR_KM0 is specified by user,\
the floder named <EXAMPLE_FOR_KM0> will be searched from components/example.\
e.g., EXAMPLE_FOR_KM0=ssl_client. Default example name is NULL")

#NOTICE: Cache variable USER_SRC_DIR will be deleted at the end.
set(USER_SRC_DIR ${USER_SRC_DIR} CACHE PATH "User source code floader containing main.c\
and CMakeLists.txt. If USER_SRC_DIR is specified by user, the project will add this floader\
as a subdirectory. USER_SRC_DIR can be a absolute path or a path relative to 'BASEDIR', \
which is parent floder of component. e.g., USER_SRC_DIR=/home/user/src, USER_SRC_DIR=component/example/xxx/src.\
Default source code floader is amebadeplus_gcc_project/project/project_km4/src")

#NOTICE: Cache variable TOOLCHAIN_DIR will be deleted at the end.
set(TOOLCHAIN_DIR ${TOOLCHAIN_DIR} CACHE PATH "Toolchain installation path. If TOOLCHAIN_DIR is specified\
by user, cross-compile toolchain will be searched in <TOOLCHAIN_DIR>. Default toolchain installation path is\
'/opt/rtk-toolchain' in linux and 'C:/rtk-toolchain' in windows.")

#NOTICE: Cache variable FINAL_IMAGE_DIR will be deleted at the end.
set(FINAL_IMAGE_DIR ${FINAL_IMAGE_DIR} CACHE PATH "Final images path, i.e., xxx_boot_all.bin, ota_all.bin \
and xxx_app.bin final path. If FINAL_IMAGE_DIR is specified by user, final images will be copied to \
<FINAL_IMAGE_DIR> after building. Default final image directory is project root path, \
where the current CMakeLists.txt is located.")
###############################################################################

###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
# set(v_FREERTOS_VER v10.4.3)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)
set(v_MBEDTLS_VER 3.6.2)
set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_INFO})
set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_soc_project_create(amebasmart)

#WARNING: project MUST be called after ameba_soc_project_create()
project(${c_SOC_TYPE} LANGUAGES C CXX ASM)

ameba_soc_project_check()
###############################################################################
#OPTIMIZE: Used in bin-merge process, remove theme if no need
set(AP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_ap)
set(HP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_hp)
set(LP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_lp)

if (CONFIG_MP_INCLUDED)
    ameba_set_if(CONFIG_LINUX_FW_EN d_IMAGE_APP_ALL
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image_mp/km0_km4_app_mp.bin
        p_ELSE
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image_mp/km0_km4_ca32_app_mp.bin
    )
else()
    ameba_set_if(CONFIG_LINUX_FW_EN d_IMAGE_APP_ALL
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image/km0_km4_app.bin
        p_ELSE
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image/km0_km4_ca32_app.bin
    )
endif()
set(d_IMAGE_OTA_ALL ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image/ota_all.bin)
###############################################################################

###############################################################################
# Add sub projects
ameba_add_subdirectory(project_ap)
ameba_add_subdirectory(project_hp)
ameba_add_subdirectory(project_lp)
###############################################################################


###############################################################################
#OPTIMIZE: move these to ameba_soc_project_exit?
set(DAILY_BUILD 0 CACHE STRING "code analysis argument" FORCE)
unset(EXAMPLE CACHE)
unset(EXAMPLE_FOR_CA32 CACHE)
unset(EXAMPLE_FOR_KM4 CACHE)
unset(EXAMPLE_FOR_KM0 CACHE)
unset(USER_SRC_DIR CACHE)
unset(TOOLCHAIN_DIR CACHE)
unset(FINAL_IMAGE_DIR CACHE)

ameba_soc_project_exit()
###############################################################################
endif() #For temporary compatibility
