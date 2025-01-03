set(TFMICRO_DIR ${c_COMPONENT_DIR}/tflite_micro)
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)

set(BUILD_IN_MCU_SDK TRUE)
set(CMAKE_CHIP_TARGET amebasmart_ca32_freertos)
add_subdirectory(${TFMICRO_DIR} ${BUILD_DIR})

# location of output archives
set(TFLMICRO_ARCHIVE ${BUILD_DIR}/libtensorflow-microlite.a)

add_custom_command(
    OUTPUT ${d_SDK_LIB_APPLICATION_DIR}/lib_tflite_micro.a
    COMMAND ${CMAKE_OBJCOPY} -g -D ${TFLMICRO_ARCHIVE}
    COMMAND ${CMAKE_COMMAND} -E copy ${TFLMICRO_ARCHIVE} ${d_SDK_LIB_APPLICATION_DIR}/lib_tflite_micro.a
    DEPENDS ${TFLMICRO_ARCHIVE}
)

add_custom_target(
    tflite_micro_postbuild
    DEPENDS ${d_SDK_LIB_APPLICATION_DIR}/lib_tflite_micro.a
)

add_dependencies(dep_${d_MCU_PROJECT_NAME}_tflite_micro tflite_micro_postbuild)
