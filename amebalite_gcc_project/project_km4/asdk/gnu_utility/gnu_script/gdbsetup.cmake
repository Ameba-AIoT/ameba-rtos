if (NOT EXISTS ${IMAGE_TARGET_FOLDER})
    message(FATAL_ERROR "${IMAGE_TARGET_FOLDER} not exist")
endif()

file(GLOB BINFILES ${GNU_SCRIPT}/*.bin)

if(BINFILES)
    file(REMOVE ${BINFILES})
endif()

if(EXISTS ${PROJECTDIR}/asdk/lib/rom_acut)
    message("${PROJECTDIR}/asdk/lib/rom_acut exist")
else()
    file(MAKE_DIRECTORY ${PROJECTDIR}/asdk/lib/rom_acut)
endif()

file(GLOB ACUTFILES ${GNU_SCRIPT}/acut_loader/*)

file(COPY ${ACUTFILES} DESTINATION ${GNU_SCRIPT})

file(READ ${GNU_SCRIPT}/rtl_gdb_flash_write.txt content)

if(EXISTS ${IMAGE_TARGET_FOLDER}/kr4_km4_app.bin)
    file(SIZE ${IMAGE_TARGET_FOLDER}/kr4_km4_app.bin FLASH_FILE_SIZE)
    string(REGEX REPLACE "set \\$FlashFileSize =[^\r\n\t]*" "set \$FlashFileSize =${FLASH_FILE_SIZE}" content ${content})
endif()

if(EXISTS ${IMAGE_TARGET_FOLDER}/km4_boot_all.bin)
    file(SIZE ${IMAGE_TARGET_FOLDER}/km4_boot_all.bin XIP_BOOT_SIZE)
    string(REGEX REPLACE "set \\$XIPBootSize =[^\r\n\t]*" "set \$XIPBootSize =${XIP_BOOT_SIZE}" content ${content})
endif()

if(EXISTS ${IMAGE_TARGET_FOLDER}/km4_image3_all.bin)
    file(SIZE ${IMAGE_TARGET_FOLDER}/km4_image3_all.bin IMG3_FILE_SIZE)
    string(REGEX REPLACE "set \\$Img3FileSize =[^\r\n\t]*" "set \$Img3FileSize =${IMG3_FILE_SIZE}" content ${content})
endif()

if(EXISTS ${IMAGE_TARGET_FOLDER}/dsp.bin)
    file(SIZE ${IMAGE_TARGET_FOLDER}/dsp.bin DSP_FILE_SIZE)
    string(REGEX REPLACE "set \\$DSPFlashSize =[^\r\n\t]*" "set \$DSPFlashSize =${DSP_FILE_SIZE}" content ${content})
endif()

file(WRITE ${GNU_SCRIPT}/rtl_gdb_flash_write.txt ${content})
