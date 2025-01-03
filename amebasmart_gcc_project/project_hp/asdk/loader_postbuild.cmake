#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${CMAKE_FILES_DIR}/axf2bin.cmake)
include(${CMAKE_FILES_DIR}/utility.cmake)
include(${CMAKE_FILES_DIR}/global_define.cmake)
set(t_USER_CUSTOM_LOG_PREFIX "HP_POSTBUILD")

execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .ram_image1.entry  -j .ram_image1.text -j .ram_image1.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf ${IMAGE_TARGET_FOLDER}/ram_1.bin
)


execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .xip_image1.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf ${IMAGE_TARGET_FOLDER}/xip_boot.bin
)

message( "========== Image Info HEX ==========")
execute_process(
    COMMAND ${CMAKE_SIZE} -A --radix=16 ${IMAGE_TARGET_FOLDER}/target_loader.axf
)
execute_process(
    COMMAND ${CMAKE_SIZE} -t --radix=16 ${IMAGE_TARGET_FOLDER}/target_loader.axf
)
message("========== Image Info HEX ==========")

message( "========== Image Info DEC ==========")
execute_process(
    COMMAND ${CMAKE_SIZE} -A --radix=10 ${IMAGE_TARGET_FOLDER}/target_loader.axf
)
execute_process(
    COMMAND ${CMAKE_SIZE} -t --radix=10 ${IMAGE_TARGET_FOLDER}/target_loader.axf
)
message( "========== Image Info DEC ==========")


message( "========== Image manipulating start ==========")


execute_process(
    COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/ram_1.bin 32
)

execute_process(
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_1.bin  __ram_start_table_start__  ${IMAGE_TARGET_FOLDER}/target_loader.map
)
if(IMGTOOL_LOADER)
    execute_process(
        COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin ${BUILD_TYPE}
        WORKING_DIRECTORY ${PROJECTDIR}/..
    )
else()
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E  cat ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin
        OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/km4_boot_all.bin
    )
    execute_process(
        COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/km4_boot_all.bin ${BUILD_TYPE}
        WORKING_DIRECTORY ${PROJECTDIR}/..
    )
endif()


message( "========== Image manipulating end ==========")

if (NOT IMGTOOL_LOADER)
    set(KM4_BOOT_ALL ${IMAGE_TARGET_FOLDER}/km4_boot_all.bin)
    if(EXISTS ${KM4_BOOT_ALL})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BOOT_ALL} ${FINAL_IMAGE_DIR}
        )
    endif()
endif()
