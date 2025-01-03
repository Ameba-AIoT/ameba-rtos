#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${CMAKE_FILES_DIR}/axf2bin.cmake)
include(${CMAKE_FILES_DIR}/utility.cmake)
include(${CMAKE_FILES_DIR}/global_define.cmake)
set(t_USER_CUSTOM_LOG_PREFIX "HP_IMG3_POSTBUILD")

execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .ram_image3.text -j .ram_image3.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf ${IMAGE_TARGET_FOLDER}/ram_3_s.bin
)
execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .gnu.sgstubs -j .ram_image3.entry -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf ${IMAGE_TARGET_FOLDER}/ram_3_nsc.bin
)

message( "========== Image Info HEX ==========")

execute_process(
    COMMAND ${CMAKE_SIZE} -A --radix=16 ${IMAGE_TARGET_FOLDER}/target_img3.axf
)
execute_process(
    COMMAND ${CMAKE_SIZE} -t --radix=16 ${IMAGE_TARGET_FOLDER}/target_img3.axf
)

message("========== Image Info HEX ==========")


message( "========== Image Info DEC ==========")

execute_process(
    COMMAND ${CMAKE_SIZE} -A --radix=10 ${IMAGE_TARGET_FOLDER}/target_img3.axf
)
execute_process(
    COMMAND ${CMAKE_SIZE} -t --radix=10 ${IMAGE_TARGET_FOLDER}/target_img3.axf
)

message( "========== Image Info DEC ==========")


message( "========== Image manipulating start ==========")
execute_process(
	COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_s.bin 32
)
execute_process(
	COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_nsc.bin 32
)


execute_process(
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_s.bin  __ram_image3_text_start__  ${IMAGE_TARGET_FOLDER}/target_img3.map
)
execute_process(
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_nsc.bin  __ram_image3_nsc_start__  ${IMAGE_TARGET_FOLDER}/target_img3.map
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/ram_3_s_prepend.bin ${IMAGE_TARGET_FOLDER}/ram_3_nsc_prepend.bin
    OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/km4_image3_all.bin
)
execute_process(
    COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/km4_image3_all.bin ${BUILD_TYPE}
    WORKING_DIRECTORY ${PROJECTDIR}/..
)
