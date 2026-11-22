set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# LLVM/clang based ASDK. ASDK_VER is "llvm22", so ToolChainVerMajor is "asdk-llvm22".
# Package: asdk-llvm22-multilib-EL-rtos-n4.2-a32C-<linux-x86_64|darwin-arm64>-<build>.tar.bz2, installed to <TOOLCHAINDIR>/asdk-llvm22-<build>
set(ToolChainVerMajor asdk-${ASDK_VER})
set(ToolChainPkgName asdk-llvm22-multilib-EL-rtos-n4.2-a32C)
# "clang --version" prints "Realtek LLVM-22 Build xxxx", it does not contain the package build number
set(TOOLCHAIN_VERSION_KEY "LLVM-22")

if(USE_SECOND_SOURCE OR "$ENV{USE_SECOND_SOURCE}")
set(TOOLCHAINURL https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/llvm22_v1/)
else()
set(TOOLCHAINURL https://aiot.realmcu.com/download/toolchain)
endif()

set(TOOLCHAINDIR)

if(DEFINED ENV{RTK_TOOLCHAIN_DIR})
    set(TOOLCHAINDIR $ENV{RTK_TOOLCHAIN_DIR})
	message("env toolchain path: ${TOOLCHAINDIR}")
endif()

if(TOOLCHAIN_DIR)
	set(TOOLCHAINDIR ${TOOLCHAIN_DIR})
	message("User defined toolchain path: ${TOOLCHAINDIR}")
endif()

file(TO_CMAKE_PATH "${TOOLCHAINDIR}" TOOLCHAINDIR)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux OR ${CMAKE_HOST_SYSTEM_NAME} STREQUAL Darwin)
	if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
		set(ToolChainVerMinor 261104)
		set(ToolChainHost linux-x86_64)
	else()
		set(ToolChainVerMinor 261105)
		set(ToolChainHost darwin-arm64)
	endif()
	if(NOT TOOLCHAINDIR)
		set(TOOLCHAINDIR "$ENV{HOME}/rtk-toolchain")
		message("Default toolchain path: ${TOOLCHAINDIR}")
	endif()
	set(SDK_TOOLCHAIN ${TOOLCHAINDIR}/${ToolChainVerMajor}-${ToolChainVerMinor})
	set(TOOLCHAINNAME ${ToolChainPkgName}-${ToolChainHost}-${ToolChainVerMinor}.tar.bz2)
else()
	message(FATAL_ERROR "The LLVM toolchain (${ToolChainVerMajor}) is only available for Linux and macOS hosts so far")
endif()

set(CROSS_COMPILE ${SDK_TOOLCHAIN}/bin/arm-none-eabi-)

# Auto download is disabled until the package is published: stop here if neither the installed directory nor the package exists.
# (ameba-toolchain-llvm-check.cmake would try to download ${TOOLCHAINURL}/${TOOLCHAINNAME} otherwise)
if(NOT EXISTS ${SDK_TOOLCHAIN} AND NOT EXISTS ${TOOLCHAINDIR}/${TOOLCHAINNAME})
	message(FATAL_ERROR "the llvm toolchain is not published yet, put it into ${TOOLCHAINDIR} manually.")
endif()

# Options for every use of the compiler driver (compile, assembly, link, and the commands that run ${CMAKE_C_COMPILER} directly with ${TOOLCHAIN_DRIVER_OPTS},
# e.g. --version check and linker script preprocessing; empty for GNU toolchains). The wrapper (bin/*-clang) appends -multi-lib-config and link only options
# (--rtlib=compiler-rt -fuse-ld=lld) to every command, which makes the driver warn on commands that do not need them:
#  - -Wno-missing-multilib: the target of the command (e.g. ca32 -mthumb, km4 .S, commands without -march) matches none of the multilib variants. The multilib
#    is only used to find libraries at link time, a real mismatch still fails at link (cannot open libclang_rt.builtins.a)
#  - -Wno-unused-command-line-argument: the link only options above are unused in compile/preprocess commands
set(TOOLCHAIN_DRIVER_OPTS -Wno-missing-multilib -Wno-unused-command-line-argument)
string(REPLACE ";" " " TOOLCHAIN_DRIVER_FLAGS "${TOOLCHAIN_DRIVER_OPTS}")
string(APPEND CMAKE_C_FLAGS " ${TOOLCHAIN_DRIVER_FLAGS}")
string(APPEND CMAKE_CXX_FLAGS " ${TOOLCHAIN_DRIVER_FLAGS}")
string(APPEND CMAKE_ASM_FLAGS " ${TOOLCHAIN_DRIVER_FLAGS}")

include(${CMAKE_CURRENT_LIST_DIR}/ameba-toolchain-llvm-check.cmake)

# clang has no spec file support (gcc: "-specs nosys.specs" => "--start-group %G -lc -lnosys --end-group" after all inputs), so append the newlib stubs to the link rule.
# They must come after all SDK archives and libc must come before libnosys:
#  - lld resolves a symbol from the first archive in command line order, libc lock.o would win over swlib/locks.c (duplicate symbols);
#  - libc reent.o and libnosys errno.o both define 'errno' (duplicate symbols if libnosys is first and reent.o is pulled by _reclaim_reent).
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <FLAGS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> -Wl,--start-group -lc -lnosys -Wl,--end-group")
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> -Wl,--start-group -lc -lnosys -Wl,--end-group")
