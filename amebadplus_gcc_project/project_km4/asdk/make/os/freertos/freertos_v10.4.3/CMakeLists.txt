set(DIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER})
set(DIR_PORT ${DIR}/Source/portable/GCC/AmebaDplus_KM4/non_secure)

set( 
    CSRC 
    ${DIR}/Source/tasks.c
    ${DIR}/Source/list.c
    ${DIR}/Source/croutine.c
    ${DIR}/Source/queue.c
    ${DIR}/Source/timers.c
    ${DIR}/Source/event_groups.c
    ${DIR}/Source/stream_buffer.c
    ${DIR_PORT}/port.c
    ${DIR_PORT}/portasm.c
    ${DIR}/Source/portable/MemMang/heap_5.c
)

set( 
    MODULE_IFLAGS
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure
)



if(CONFIG_INTERNAL_LIB)

    target_sources(
        ${CURRENT_LIB_NAME}
        PRIVATE 
        ${CSRC}
    )

else()

    foreach(src ${CSRC})
        set_property(GLOBAL APPEND PROPERTY RENAME_CMD COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.image2.net.ram.text ${CMAKE_CURRENT_BINARY_DIR}/../../../CMakeFiles/target_img2_${PROJECT_NAME}.dir${src}.o ${CMAKE_CURRENT_BINARY_DIR}/../../../CMakeFiles/target_img2_${PROJECT_NAME}.dir${src}.o)
    endforeach()

    set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

    target_sources(
    target_img2_${PROJECT_NAME}
    PRIVATE 
    ${CSRC}
    )

endif()