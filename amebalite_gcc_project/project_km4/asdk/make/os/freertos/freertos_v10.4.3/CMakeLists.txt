set(DIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER})
set(DIR_PORT ${DIR}/Source/portable/GCC/AmebaLite_KM4/non_secure)
set(HEAP_TRACE_DIR ${BASEDIR}/component/os/freertos/heap_trace)
set(CRASHDIR ${BASEDIR}/component/soc/common/crashdump/)
file(RELATIVE_PATH IMAGE_DIR ${FINAL_IMAGE_DIR} ${IMAGE_TARGET_FOLDER})

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
    ${HEAP_TRACE_DIR}/heap_trace.c
)

set(
    MODULE_IFLAGS
    ${HEAP_TRACE_DIR}
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure
)

target_sources(
    ${CURRENT_LIB_NAME}
    PRIVATE
    ${CSRC}
)

target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE -DSDK_TOOLCHAIN="${SDK_TOOLCHAIN}")
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE -DIMAGE_DIR="${IMAGE_DIR}")
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS} PRIVATE ${CRASHDIR}/include)