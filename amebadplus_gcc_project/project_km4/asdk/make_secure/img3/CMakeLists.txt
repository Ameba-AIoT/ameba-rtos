set(DIR ${TARGETDIR}/img3)
set(FWDIR ${TARGETDIR}/fwlib)

set(MODULE_IFLAGS ${DIR}./)

set(MODULE_C_OPTIONS -O0)

set(
    CSRC 
    ${DIR}/boot_img3.c 
    ${DIR}/secure_src.c
)

set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

target_sources(target_img3 PRIVATE ${CSRC})