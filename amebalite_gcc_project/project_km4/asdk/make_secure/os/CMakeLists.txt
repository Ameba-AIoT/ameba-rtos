set(DIR ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure)

set(
    MODULE_IFLAGS
    ${DIR}./
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KM4/secure
)

set(MODULE_C_OPTIONS -O0)
set(
    CSRC
    ${DIR}/secure_context.c
	${DIR}/secure_context_port.c
	${DIR}/secure_heap.c
	${DIR}/secure_init.c
)

set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

target_sources(target_img3 PRIVATE ${CSRC})
