if (__INCLUDED_CMSIS_DSP_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_CMSIS_DSP_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_cmsis_dsp INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_cmsis_dsp p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/BasicMathFunctions/BasicMathFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/BayesFunctions/BayesFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/CommonTables/CommonTables.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/ComplexMathFunctions/ComplexMathFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/ControllerFunctions/ControllerFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/DistanceFunctions/DistanceFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/FastMathFunctions/FastMathFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/FilteringFunctions/FilteringFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/MatrixFunctions/MatrixFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/StatisticsFunctions/StatisticsFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/SupportFunctions/SupportFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/SVMFunctions/SVMFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/TransformFunctions/TransformFunctions.c
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Source/TransformFunctions/arm_bitreversal2.S
)
#--------------------------#