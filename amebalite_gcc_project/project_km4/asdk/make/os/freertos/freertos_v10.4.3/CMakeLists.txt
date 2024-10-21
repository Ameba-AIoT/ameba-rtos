set(DIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER})
set(DIR_PORT ${DIR}/Source/portable/GCC/AmebaLite_KM4/non_secure)

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
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure
)

target_sources(
    ${CURRENT_LIB_NAME}
    PRIVATE
    ${CSRC}
)

