set(DIR ${TARGETDIR}/bootloader)
set(CFGDIR ${TARGETDIR}/usrcfg)
set(DRVDIR ${TARGETDIR}/fwlib/ram_km4)
set(DRVCOM ${TARGETDIR}/fwlib/ram_common)
set(LOGDIR ${TARGETDIR}/swlib)

add_executable(target_loader_${PROJECT_NAME})

add_custom_target(
    boot
    COMMENT "build bootloader only"
    DEPENDS target_loader_${PROJECT_NAME}
)

if (CONFIG_MP_SHRINK)
    target_sources(target_loader_${PROJECT_NAME} PRIVATE ${DIR}/bootloader_km4_mp.c)
else()
    target_sources(target_loader_${PROJECT_NAME} PRIVATE ${DIR}/bootloader_km4.c)
endif()

target_sources(
    target_loader_${PROJECT_NAME} 
    PRIVATE 
    ${DIR}/boot_ota_km4.c 
	${DIR}/boot_security_km4.c 
	${CFGDIR}/ameba_boot_trustzonecfg.c 
	${CFGDIR}/ameba_bootcfg.c 
	${CFGDIR}/ameba_flashcfg.c 
	${DRVDIR}/ameba_flashclk.c 
	${DRVCOM}/ameba_psram.c
	${DRVCOM}/ameba_clk.c 
	${DRVCOM}/ameba_ldo.c 
	${DRVCOM}/ameba_reset.c 
	${DRVCOM}/ameba_pll.c 
	${DRVCOM}/ameba_fault_handle.c 
    ${LOGDIR}/log.c
)

target_include_directories(target_loader_${PROJECT_NAME} PRIVATE ${LOGDIR} ${TARGETDIR}/lib/bootloader ${BASEDIR}/component/soc/common/crashdump/include)

list(APPEND 
    LINK_LOADER_LIB 
    ${TARGET_LIBSOC_FOLDER}/lib_bootloader.a  
    ${TARGET_LIBSOC_FOLDER}/lib_crashdump.a
    ) 


target_link_libraries(
    target_loader_${PROJECT_NAME} 
    PRIVATE ameba_interface_${PROJECT_NAME} ameba_wrap_interface_${PROJECT_NAME}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map 
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld 
    ${LINK_ROM_LIB} 
    ${LINK_LOADER_LIB}
)

if(CONFIG_MP_SHRINK)
    set(IMG1_LD ${LDDIR}/ameba_img1_all_mp.ld)
else()
    set(IMG1_LD ${LDDIR}/ameba_img1_all.ld)
endif()

add_custom_command(
  TARGET target_loader_${PROJECT_NAME} PRE_LINK 
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG1_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build bootloader linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${IMG1_LD}
    ${PROJECTDIR}/../amebaDplus_layout.ld
)

set_target_properties(target_loader_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


add_custom_command(TARGET target_loader_${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E copy text.map ${IMAGE_TARGET_FOLDER}/text_loader.map
    COMMAND ${CMAKE_COMMAND} -E copy target_loader_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_loader.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_loader.axf | sort > ${IMAGE_TARGET_FOLDER}/target_loader.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_loader.axf > ${IMAGE_TARGET_FOLDER}/target_loader.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_loader.axf ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_boot_all.bin"

    COMMAND ${CMAKE_COMMAND} -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}  
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
    -DBUILD_TYPE=${BUILD_TYPE}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/boot_postbuild.cmake
)

set_property(TARGET target_loader_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${IMAGE_TARGET_FOLDER}/../../../km4_boot_all.bin 
)
