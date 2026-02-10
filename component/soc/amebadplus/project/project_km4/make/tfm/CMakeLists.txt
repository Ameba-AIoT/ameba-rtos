execute_process(
    COMMAND ${CMAKE_COMMAND} -DTFM_PROFILE=profile_medium -DTFM_PLATFORM=realtek/amebadplus -DBL2=OFF ${c_COMPONENT_DIR}/soc/common/trusted-firmware-m
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/trusted-firmware-m
)

add_custom_target(
    target_tfm
    ALL
    COMMAND make install
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/trusted-firmware-m
)

add_custom_command(
    TARGET target_tfm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/trusted-firmware-m/install/outputs ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/trusted-firmware-m/install/interface/lib/s_veneers.o ${c_SDK_IMAGE_TARGET_DIR}
)