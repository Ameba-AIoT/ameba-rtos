##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources 
    nghttp2_buf.c
    nghttp2_callbacks.c
    nghttp2_debug.c
    nghttp2_frame.c
    nghttp2_hd.c
    nghttp2_hd_huffman.c
    nghttp2_hd_huffman_data.c
    nghttp2_helper.c
    nghttp2_http.c
    nghttp2_map.c
    nghttp2_mem.c
    nghttp2_option.c
    nghttp2_outbound_item.c
    nghttp2_pq.c
    nghttp2_priority_spec.c
    nghttp2_queue.c
    nghttp2_rcbuf.c
    nghttp2_session.c
    nghttp2_stream.c
    nghttp2_submit.c
    nghttp2_version.c
)

ameba_list_append(private_includes  
    includes
)

ameba_list_append(private_compile_options  
    -Wno-implicit-fallthrough
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(http2
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)