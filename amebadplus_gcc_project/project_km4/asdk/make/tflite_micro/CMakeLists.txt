set(TFMICRO_DIR ${BASEDIR}/component/tflite_micro)
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)

set(BUILD_IN_MCU_SDK TRUE)
set(CMAKE_CHIP_TARGET amebadplus_km4)
add_subdirectory(${TFMICRO_DIR} ${BUILD_DIR})

# location of output archives
set(TFLMICRO_ARCHIVE ${BUILD_DIR}/libtensorflow-microlite.a)
set(CMSIS_NN_ARCHIVE ${CMAKE_BINARY_DIR}/_deps/cmsis_nn-build/libcmsis-nn.a)

add_custom_command(
    OUTPUT ${TARGET_LIBAPP_FOLDER}/lib_tflite_micro.a ${TARGET_LIBAPP_FOLDER}/lib_cmsis_nn.a
    COMMAND ${CMAKE_OBJCOPY} -g -D ${TFLMICRO_ARCHIVE}
    COMMAND ${CMAKE_COMMAND} -E copy ${TFLMICRO_ARCHIVE} ${TARGET_LIBAPP_FOLDER}/lib_tflite_micro.a
    COMMAND ${CMAKE_OBJCOPY} -g -D ${CMSIS_NN_ARCHIVE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMSIS_NN_ARCHIVE} ${TARGET_LIBAPP_FOLDER}/lib_cmsis_nn.a
    DEPENDS ${TFLMICRO_ARCHIVE} ${CMSIS_NN_ARCHIVE}
)

add_custom_target(
    tflite_micro_postbuild
    DEPENDS ${TARGET_LIBAPP_FOLDER}/lib_tflite_micro.a ${TARGET_LIBAPP_FOLDER}/lib_cmsis_nn.a
)

add_dependencies(target_img2_${PROJECT_NAME} tflite_micro_postbuild)

