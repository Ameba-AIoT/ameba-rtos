include(${CMAKE_FILES_DIR}/axf2bin.cmake)

if (CONFIG_MP_SHRINK)
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .ram_image1.entry -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf ${IMAGE_TARGET_FOLDER}/entry_1.bin
        COMMAND ${CMAKE_OBJCOPY} -j .ram_image1.text -j .ram_image1.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf ${IMAGE_TARGET_FOLDER}/ram_1.bin
    )
else()
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .ram_image1.entry  -j .ram_image1.text -j .ram_image1.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_loader.axf ${IMAGE_TARGET_FOLDER}/ram_1.bin
    )
endif()

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

if (CONFIG_MP_SHRINK)
    execute_process(
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/entry_1.bin 992
    )
else()
    execute_process(
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/ram_1.bin 32
    )
    execute_process(
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/xip_boot.bin 32
    )
endif()


if (CONFIG_MP_SHRINK)
    execute_process(
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/entry_1.bin  __ram_start_table_start__  ${IMAGE_TARGET_FOLDER}/target_loader.map
    )
    execute_process( 
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_1.bin  __ram_image1_text_start__  ${IMAGE_TARGET_FOLDER}/target_loader.map
    )
    execute_process(    
        COMMAND ${CMAKE_COMMAND} -E  cat ${IMAGE_TARGET_FOLDER}/entry_1_prepend.bin ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin
        OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/km4_boot.bin
    )
    execute_process(    
        COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/km4_boot.bin ${BUILD_TYPE}
    )
else()
    execute_process( 
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_1.bin  __ram_start_table_start__  ${IMAGE_TARGET_FOLDER}/target_loader.map
    )
    execute_process( 
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/xip_boot.bin  __km4_boot_text_start__  ${IMAGE_TARGET_FOLDER}/target_loader.map
    )
    execute_process( 
        COMMAND ${CMAKE_COMMAND} -E  cat ${IMAGE_TARGET_FOLDER}/xip_boot_prepend.bin ${IMAGE_TARGET_FOLDER}/ram_1_prepend.bin
        OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/km4_boot.bin
    )
    execute_process( 
        COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/km4_boot.bin ${BUILD_TYPE}
    )
endif()

message( "========== Image manipulating end ==========")

set(KM4_BOOT_ALL ${IMAGE_TARGET_FOLDER}/km4_boot_all.bin)


if(EXISTS ${KM4_BOOT_ALL})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BOOT_ALL} ${IMAGE_TARGET_FOLDER}/../../..
    )
endif()
