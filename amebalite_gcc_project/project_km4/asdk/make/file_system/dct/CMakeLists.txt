set(DIR ${BASEDIR}/component/file_system/dct)

ameba_app_library(dct)

target_sources(
    ${CURRENT_LIB_NAME} 
    PRIVATE 
    ${DIR}/crc32.c
	${DIR}/dct.c
	${DIR}/dct_util.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} 
    PRIVATE 
    ${DIR}
)
