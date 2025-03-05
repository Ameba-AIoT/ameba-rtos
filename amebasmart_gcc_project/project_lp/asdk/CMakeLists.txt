if(NOT CMAKE_REFACTOR) # For temporary compatibility.
set(CMAKE_EXECUTABLE_SUFFIX .axf)
ameba_set_if(d_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON p_ELSE OFF)

add_compile_options(-fdata-sections -ffunction-sections)

ameba_executable(target_img2_${PROJECT_NAME} ${c_EMPTY_C_FILE})

############################make_subdirs####################################
ameba_add_subdirectories(
    make/bootloader
    make/os
    make/monitor
    make/target
    # make/cmsis
    make/project
    make/application
    make/utils
)
# ameba_add_subdirectories_if(CONFIG_APP_DEMO
#     make/example
# )
############################################################################

#######################make_subdirs_lib_img2################################
if(NOT d_AMEBA_RELEASE)
    ameba_add_subdirectories(
        make_lib/chipinfo_lib
        make_lib/crashdump_lib
        make_lib/pmc_lib
    )
    add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_fw/amebasmart ${CMAKE_CURRENT_BINARY_DIR}/wlan)
endif()
############################################################################


get_property(IMG2_LIBS_PROPERTY GLOBAL PROPERTY g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS)

ameba_info("g_${PROJECT_NAME}_IMAGE2_LIBS: ${IMG2_LIBS_PROPERTY}")
ameba_info("d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES: ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES}")

target_link_libraries(
    target_img2_${PROJECT_NAME}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
    d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    ${IMG2_LIBS_PROPERTY}
    ${ASDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.base/libm.a
    -Wl,--no-whole-archive

    #NOTE: DONOT add -Wl,--whole-archive for this, for they are added in component/build_info/dependency_*.cmake
    ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_LIBRARIES}

    -lm -lstdc++
)

foreach(dep IN LISTS d_LIST_${d_MCU_PROJECT_NAME_UPPER}_NO_LOADER_DEPS)
    ameba_target_depend(target_img2_${PROJECT_NAME} ${dep})
endforeach()

add_custom_command(
    OUTPUT ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2_ns.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    # COMMAND ${CMAKE_OBJDUMP} -h -S --disassemble-all ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.txt
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_pure_img2.map

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km0_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -Dd_PLATFORM_PROJECT_DIR=${d_PLATFORM_PROJECT_DIR}
    -Dd_MCU_PROJECT_DIR=${d_MCU_PROJECT_DIR}
    -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DCONFIG_LINUX_FW_EN=${CONFIG_LINUX_FW_EN}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DKM4_PROJECT_DIR=${KM4_PROJECT_DIR} -DPROJECTDIR=${PROJECTDIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_BT=${CONFIG_BT} -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG} -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake

    DEPENDS target_img2_${PROJECT_NAME}
    COMMENT "target_img2_${PROJECT_NAME} post build"
)

add_custom_target(
    target_img2_${PROJECT_NAME}_postbuild
    ALL
    DEPENDS ${IMAGE_TARGET_FOLDER}/target_img2.axf
)

add_custom_command(
  TARGET target_img2_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${d_SDK_LD_DIR}/ameba_img2_sram.ld -o ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${d_SDK_LD_DIR}/ameba_img2_sram.ld
    ${d_PLATFORM_PROJECT_DIR}/${d_PLATFORM_TYPE}_layout.ld
)

set_target_properties(target_img2_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


set_property(TARGET target_img2_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
)

##########################  CUSTOM TARGET  ###################################

add_custom_target(
    ${PROJECT_NAME}
    DEPENDS target_img2_${PROJECT_NAME}
)
else() # For temporary compatibility.NOTE:  script below is new version cmake
set(CMAKE_EXECUTABLE_SUFFIX .axf)

#REVIEW: move this to ameba_mcu_project_create?
ameba_global_include(${c_GLOBAL_MCU_INCLUDE_DIRECTORIES})
add_compile_options(-fdata-sections -ffunction-sections)

if(CONFIG_AMEBASMART_A_CUT)
    ameba_set(ROM_LIB_DIR ${c_SDK_LIB_DIR}/${c_SOC_TYPE}_rom_acut)
    # ameba_set(d_FLOADER_BIN_DIR ${d_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${d_PLATFORM_TYPE}_acut)
    ameba_set(FLOADER_BIN_DIR ${c_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${c_SOC_TYPE}_acut)
else()
    ameba_fatal("CONFIG_AMEBASMART_A_CUT not defined as TRUE")
endif()

if(CONFIG_LINK_ROM_SYMB)
    ameba_unset(LINK_ROM_LIB)
    ameba_set(LINK_ROM_SYMBOL    ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
else()
    ameba_set(LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
    ameba_set(LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.a)
endif()

ameba_add_subdirectory(image2)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)
endif()# For temporary compatibility.