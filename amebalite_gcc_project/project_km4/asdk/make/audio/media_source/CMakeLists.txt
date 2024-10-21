set(DIR ${BASEDIR}/component/audio/media_source)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/ssl/mbedtls-2.16.9/include
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/media_utils/include
    ${BASEDIR}/component/audio/media_common/include
    ${BASEDIR}/component/audio/configs/include
    ${DIR}/include
)

set(MODULE_CPP_DEFINES __RTOS__ CONFIG_MEDIA_DATASOURCE_BUFFER CONFIG_MEDIA_DATASOURCE_HTTP)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter)

set(
    CPPSRC 
    ${DIR}/src/data_source_base.cpp
    ${DIR}/src/data_source_factory.cpp
    ${DIR}/src/file_source.cpp
    ${DIR}/src/buffer_source.cpp
    ${DIR}/src/cdata_source.cpp
    ${DIR}/src/http/http_source.cpp
    ${DIR}/src/http/http_tls.cpp
)

set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
