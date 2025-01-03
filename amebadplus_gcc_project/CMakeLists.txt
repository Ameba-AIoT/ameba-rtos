cmake_minimum_required(VERSION 3.20.0)

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

set(ASDK_VER 10.3.1)
include (${CMAKE_CURRENT_SOURCE_DIR}/../cmake/toolchain/ameba-toolchain-asdk-${ASDK_VER}.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/set_extensions.cmake)
set(CMAKE_PROJECT_INCLUDE_BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/disable_compiler_detection.cmake)

project(amebadplus LANGUAGES C CXX ASM)
message ("project : " ${PROJECT_NAME})
set(CMAKE_BUILD_TYPE "")

add_subdirectory(project_km4)
add_subdirectory(project_km0)

set(DAILY_BUILD 0 CACHE STRING "code analysis argument" FORCE)
unset(EXAMPLE CACHE)
unset(USER_SRC_DIR CACHE)
unset(TOOLCHAIN_DIR CACHE)
unset(FINAL_IMAGE_DIR CACHE)
