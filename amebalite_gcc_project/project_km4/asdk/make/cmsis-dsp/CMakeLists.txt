set(DIR ${TARGETDIR}/cmsis-dsp/Source)

set( 
	CSRC 
	${DIR}/BasicMathFunctions/BasicMathFunctions.c
	${DIR}/BayesFunctions/BayesFunctions.c
	${DIR}/CommonTables/CommonTables.c
	${DIR}/ComplexMathFunctions/ComplexMathFunctions.c
	${DIR}/ControllerFunctions/ControllerFunctions.c
	${DIR}/DistanceFunctions/DistanceFunctions.c
	${DIR}/FastMathFunctions/FastMathFunctions.c
	${DIR}/FilteringFunctions/FilteringFunctions.c
	${DIR}/MatrixFunctions/MatrixFunctions.c
	${DIR}/StatisticsFunctions/StatisticsFunctions.c
	${DIR}/SupportFunctions/SupportFunctions.c
	${DIR}/SVMFunctions/SVMFunctions.c
	${DIR}/TransformFunctions/TransformFunctions.c
    ${DIR}/TransformFunctions/arm_bitreversal2.S
)

ameba_internal_library(cmsis_dsp)

target_sources(
    ${CURRENT_LIB_NAME}
    PRIVATE
    ${CSRC}
)

add_custom_command(TARGET ${CURRENT_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy lib_${CURRENT_LIB_NAME}.a ${TARGET_LIBAPP_FOLDER}/lib_cmsis_dsp.a
    COMMAND ${CMAKE_OBJCOPY} -g -D ${TARGET_LIBAPP_FOLDER}/lib_cmsis_dsp.a
)