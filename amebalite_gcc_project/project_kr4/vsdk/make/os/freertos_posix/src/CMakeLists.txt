set(DIR ${BASEDIR}/component/os/freertos/freertos_posix)
set(SOURCE_DIR ${DIR}/FreeRTOS-Plus-POSIX/source)

set(MODULE_C_OPTIONS  -Wno-cast-function-type)

set(
    MODULE_IFLAGS
    ${DIR}/include
	${DIR}/FreeRTOS-Plus-POSIX/include
	${DIR}/FreeRTOS-Plus-POSIX/include/portable/realtek

    ${FREERTOSDIR}/include
	${FREERTOSDIR}/portable/GCC/AmebaLite_KR4

)

set(
    CSRC
    ${SOURCE_DIR}/FreeRTOS_POSIX_clock.c
	${SOURCE_DIR}/FreeRTOS_POSIX_pthread_cond.c
	${SOURCE_DIR}/FreeRTOS_POSIX_timer.c
	${SOURCE_DIR}/FreeRTOS_POSIX_mqueue.c
	${SOURCE_DIR}/FreeRTOS_POSIX_pthread_mutex.c
	${SOURCE_DIR}/FreeRTOS_POSIX_unistd.c
	${SOURCE_DIR}/FreeRTOS_POSIX_pthread_barrier.c
	${SOURCE_DIR}/FreeRTOS_POSIX_sched.c
	${SOURCE_DIR}/FreeRTOS_POSIX_utils.c
	${SOURCE_DIR}/FreeRTOS_POSIX_pthread.c
	${SOURCE_DIR}/FreeRTOS_POSIX_semaphore.c
)



ameba_internal_library(freertos_posix)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
