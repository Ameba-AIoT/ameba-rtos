set(DIR ${BASEDIR}/component/network/http2/nghttp2-1.31.0)
set(MODULE_IFLAGS ${BASEDIR}/component/network/http2/nghttp2-1.31.0/includes)
set(MODULE_C_OPTIONS  -Wno-implicit-fallthrough)

set(
    CSRC 
    ${DIR}/nghttp2_buf.c 
    ${DIR}/nghttp2_callbacks.c 
    ${DIR}/nghttp2_debug.c 
    ${DIR}/nghttp2_frame.c 
    ${DIR}/nghttp2_hd.c 
    ${DIR}/nghttp2_hd_huffman.c 
    ${DIR}/nghttp2_hd_huffman_data.c 
    ${DIR}/nghttp2_helper.c 
    ${DIR}/nghttp2_http.c 
    ${DIR}/nghttp2_map.c 
    ${DIR}/nghttp2_mem.c 
    ${DIR}/nghttp2_option.c 
    ${DIR}/nghttp2_outbound_item.c 
    ${DIR}/nghttp2_pq.c 
    ${DIR}/nghttp2_priority_spec.c 
    ${DIR}/nghttp2_queue.c 
    ${DIR}/nghttp2_rcbuf.c 
    ${DIR}/nghttp2_session.c 
    ${DIR}/nghttp2_stream.c 
    ${DIR}/nghttp2_submit.c 
    ${DIR}/nghttp2_version.c 
)

ameba_internal_library(http2)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})

add_custom_command(TARGET ${CURRENT_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy lib_${CURRENT_LIB_NAME}.a ${TARGET_LIBAPP_FOLDER}/lib_http2.a
    COMMAND ${CMAKE_OBJCOPY} -g -D ${TARGET_LIBAPP_FOLDER}/lib_http2.a
)
