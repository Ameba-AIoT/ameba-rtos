if(NOT CMAKE_REFACTOR) # For temporary compatibility
cmake_minimum_required(VERSION 3.20.0)
include(ExternalProject)

#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE ${EXAMPLE} CACHE STRING "Example Name. If EXAMPLE is specified by user,\
the floder named <EXAMPLE> will be searched from components/example.\
e.g., EXAPMLE=ssl_client. Default example name is NULL")

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

set(WORKING_PROJECT_DIR ${CMAKE_BINARY_DIR}/..)
set(MENUCONFIGDIR ${WORKING_PROJECT_DIR}/menuconfig)
set(c_SOC_TYPE amebalite)
set(c_COMPONENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../component)

#EXAMPLE can be a absolute path, relative path where contain a CMakeLists and source files, or can be a specific example name
if (EXAMPLE)
    if(IS_ABSOLUTE EXAMPLE)
        set(EXAMPLEDIR ${EXAMPLE})
    else()
        cmake_path(ABSOLUTE_PATH EXAMPLE BASE_DIRECTORY ${WORKING_PROJECT_DIR}. NORMALIZE OUTPUT_VARIABLE OUTPUT_EXAMPLE)
        message(${OUTPUT_EXAMPLE})
        if (EXISTS ${OUTPUT_EXAMPLE})
            set(EXAMPLEDIR ${OUTPUT_EXAMPLE})
        else()
            file(GLOB_RECURSE EXAMPLEDIR
                ${CMAKE_CURRENT_SOURCE_DIR}/../component/example/example_${EXAMPLE}.c
                ${CMAKE_CURRENT_SOURCE_DIR}/..//component/example/example_${EXAMPLE}.cc)
            cmake_path(REMOVE_FILENAME EXAMPLEDIR)
            if(EXAMPLEDIR)
                message("THE PATH of example_${EXAMPLE}.c is " "${EXAMPLEDIR}")
            else()
                message(SEND_ERROR "example_${EXAMPLE}.c not found. Please check example name!")
            endif()
        endif()
    endif()
endif()

if (EXAMPLEDIR AND EXISTS ${EXAMPLEDIR}/prj.conf)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${EXAMPLEDIR}/prj.conf ${MENUCONFIGDIR}/prj.conf
    )
endif()

execute_process(
    COMMAND python menuconfig.py --check -d ${WORKING_PROJECT_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY
)

set(ASDK_VER 10.3.1)
include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/toolchain/ameba-toolchain-asdk-${ASDK_VER}.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/set_extensions.cmake)
set(CMAKE_PROJECT_INCLUDE_BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/disable_compiler_detection.cmake)

project(amebalite LANGUAGES C CXX ASM)
message ("project : " ${PROJECT_NAME})
set(CMAKE_BUILD_TYPE "")

#use external project because kr4 use different toolchain
ExternalProject_Add(kr4
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/project_kr4
    BINARY_DIR ${CMAKE_BINARY_DIR}/project_kr4
    CMAKE_ARGS -DEXAMPLE=${EXAMPLE} -DUSER_SRC_DIR=${USER_SRC_DIR} -DTOOLCHAIN_DIR=${TOOLCHAIN_DIR} -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR} -DDAILY_BUILD=${DAILY_BUILD} -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS} -DWORKING_PROJECT_DIR=${WORKING_PROJECT_DIR}
    BUILD_ALWAYS TRUE
    INSTALL_COMMAND ""
    USES_TERMINAL_BUILD TRUE
)

add_subdirectory(project_km4)

set_property(TARGET kr4
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${CMAKE_SOURCE_DIR}/project_kr4/vsdk/image
    ${CMAKE_SOURCE_DIR}/project_kr4/vsdk/image_mp
    ${CMAKE_BINARY_DIR}/project_kr4
)

set(DAILY_BUILD 0 CACHE STRING "code analysis argument" FORCE)
unset(EXAMPLE CACHE)
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
#NOTICE: Cache variable EXAMPLE will be deleted at the end.
set(EXAMPLE ${EXAMPLE} CACHE STRING "Example Name. If EXAMPLE is specified by user,\
the floder named <EXAMPLE> will be searched from components/example.\
e.g., EXAPMLE=ssl_client. Default example name is NULL")

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
# set(v_MBEDTLS_VER 3.6.2)
set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_INFO})
set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################
ameba_soc_project_create(amebalite)

#WARNING: project MUST be called after ameba_soc_project_create()
project(${c_SOC_TYPE} LANGUAGES C CXX ASM)

ameba_soc_project_check()
###############################################################################

###############################################################################
#OPTIMIZE: Used in bin-merge process, remove theme if no need
set(KM4_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_km4)
set(KR4_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_kr4)
###############################################################################

###############################################################################
# Add sub projects
add_subdirectory(project_kr4)
add_subdirectory(project_km4)
###############################################################################

###############################################################################
#OPTIMIZE: move these to ameba_soc_project_exit?
set(DAILY_BUILD 0 CACHE STRING "code analysis argument" FORCE)
unset(EXAMPLE CACHE)
unset(USER_SRC_DIR CACHE)
unset(TOOLCHAIN_DIR CACHE)
unset(FINAL_IMAGE_DIR CACHE)

ameba_soc_project_exit()
###############################################################################
endif() #For temporary compatibility
