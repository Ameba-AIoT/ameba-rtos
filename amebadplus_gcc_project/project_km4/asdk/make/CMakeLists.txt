add_executable(target_img2_${PROJECT_NAME} "")
target_compile_definitions(target_img2_${PROJECT_NAME} PRIVATE IMAGE2_BUILD)

add_custom_target(
    image2_${PROJECT_NAME} 
    COMMENT "build img2 only"
    DEPENDS target_img2_${PROJECT_NAME}
)


set(CONFIG_INTERNAL_LIB y)

if(CONFIG_INTERNAL_LIB)
    define_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME}
    BRIEF_DOCS "internal library lists which will be linked to img2"
    FULL_DOCS "internal library lists which will be linked to img2"
    )
    set_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME} "")
else()
    define_property(GLOBAL PROPERTY RENAME_CMD
    BRIEF_DOCS "rename section cmds that some objs need"
    FULL_DOCS "rename section cmds that some objs need"
    )
    set_property(GLOBAL PROPERTY RENAME_CMD "")
endif()


add_subdirectory(at_cmd)
add_subdirectory(cmsis)
add_subdirectory(cmsis-dsp)
add_subdirectory(audio)
add_subdirectory(file_system)
add_subdirectory(libnosys)
add_subdirectory(os)
add_subdirectory(project)
add_subdirectory(target)
add_subdirectory(test)
add_subdirectory(utils)

if(CONFIG_BT)
    add_subdirectory(bluetooth)
endif()

if(CONFIG_802154_EN)
    add_subdirectory(wpan)
endif()

if(CONFIG_WLAN)
    add_subdirectory(${WIFIMAKEDIR} ${CMAKE_CURRENT_BINARY_DIR}/wlan)
    if(CONFIG_AS_INIC_AP)
        add_subdirectory(application)
        if(NOT CONFIG_MP_SHRINK)
            add_subdirectory(network)
        endif()
        add_subdirectory(utilities)
        add_subdirectory(utilities_example)
    elseif(CONFIG_AS_INIC_NP)

    elseif(CONFIG_SINGLE_CORE_WIFI)
        add_subdirectory(application)
        if(NOT CONFIG_MP_SHRINK)
            add_subdirectory(network)
        endif()
        add_subdirectory(utilities)
        add_subdirectory(utilities_example)
    endif()
endif()

if(CONFIG_USB_DEVICE_EN)
    add_subdirectory(usb_otg)
endif()

if(CONFIG_MBED_TLS_ENABLED)
    add_subdirectory(mbedtls)
endif()

if(CONFIG_GUI_EN)
    add_subdirectory(ui)
endif()

if(CONFIG_LINKKIT_AWSS)
    add_subdirectory(linkkit)
endif()

set(LINK_APP_LIB)
set(LINK_THIRD_APP_LIB)

include(../libs.cmake)

include(${WIFIMAKEDIR}/wifilib.cmake)

if(CONFIG_INTERNAL_LIB)
    get_property(IMG2_LIBS_PROPERTY GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME})
endif()

target_link_libraries(
    target_img2_${PROJECT_NAME} 
    PRIVATE ameba_interface_${PROJECT_NAME} ameba_wrap_interface_${PROJECT_NAME}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map 
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    -Wl,--whole-archive
    ${IMG2_LIBS_PROPERTY}
    ${LINK_APP_LIB}
    -Wl,--no-whole-archive
    ${LINK_ROM_LIB_NS}
    ${LINK_THIRD_APP_LIB}
    ${ASDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu/crtbegin.o
    ${ASDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu/crtend.o
    -lm -lstdc++
)


add_custom_command(
    # TARGET target_img2_${PROJECT_NAME} POST_BUILD
    OUTPUT ${KM0_BUILDDIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2_ns.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.asm ${KM0_BUILDDIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_image2_all.bin"

    COMMAND ${CMAKE_COMMAND} -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}  
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
    -DBUILD_TYPE=${BUILD_TYPE}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake


    COMMAND ${CMAKE_COMMAND} -E echo "========== Image analyze start =========="
    COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
    COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
    COMMAND ${CMAKE_COMMAND} -E echo "========== Image analyze end =========="
    WORKING_DIRECTORY ${PROJECTDIR}/asdk
    DEPENDS target_img2_${PROJECT_NAME}
)

add_custom_target(
    target_wifi_nouse 
    ALL 
    DEPENDS ${KM0_BUILDDIR}/asdk/image/target_img2_otrcore.asm
)

if(CONFIG_MP_SHRINK)
    set(IMG2_LD ${LDDIR}/ameba_img2_all_mp.ld)
else()
    set(IMG2_LD ${LDDIR}/ameba_img2_all.ld)
endif()

if(CONFIG_TRUSTZONE)
    set(LINK_ROM_SYMBOL ${LINK_ROM_SYMBOL_NS})
else()
    set(LINK_ROM_SYMBOL ${LINK_ROM_SYMBOL_S})
endif()


add_custom_command(
  TARGET target_img2_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_STDLIB} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${IMG2_LD}
    ${PROJECTDIR}/../amebaDplus_layout.ld
)

set_target_properties(target_img2_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


if(NOT CONFIG_INTERNAL_LIB)
    get_property(RENAME_CMD_PROPERTY GLOBAL PROPERTY RENAME_CMD)
    add_custom_command(
        TARGET target_img2_${PROJECT_NAME} PRE_LINK 
        COMMAND ${RENAME_CMD_PROPERTY}
        COMMENT "rename img2 objects"
    )
endif()

set_property(TARGET target_img2_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${IMAGE_TARGET_FOLDER}/../../../km0_km4_app.bin 
    ${IMAGE_TARGET_FOLDER}/../../../km0_km4_app_mp.bin 
    ${IMAGE_TARGET_FOLDER}/../../../ota_all.bin 
)
