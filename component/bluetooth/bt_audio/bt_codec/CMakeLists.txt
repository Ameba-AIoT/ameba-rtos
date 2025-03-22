##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin
ameba_list_append(public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_btcodec.a
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(p_NO_WHOLE_ARCHIVE ${public_libraries}) #default: whole-archived

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
	sbc/decoder/srce/alloc.c
	sbc/decoder/srce/bitalloc.c
	sbc/decoder/srce/bitalloc-sbc.c
	sbc/decoder/srce/bitstream-decode.c
	sbc/decoder/srce/decoder-oina.c
	sbc/decoder/srce/decoder-private.c
	sbc/decoder/srce/decoder-sbc.c
	sbc/decoder/srce/dequant.c
	sbc/decoder/srce/framing.c
	sbc/decoder/srce/framing-sbc.c
	sbc/decoder/srce/oi_codec_version.c
	sbc/decoder/srce/synthesis-8-generated.c
	sbc/decoder/srce/synthesis-dct8.c
	sbc/decoder/srce/synthesis-sbc.c
	sbc/encoder/srce/sbc_analysis.c
	sbc/encoder/srce/sbc_dct.c
	sbc/encoder/srce/sbc_dct_coeffs.c
	sbc/encoder/srce/sbc_enc_bit_alloc_mono.c
	sbc/encoder/srce/sbc_enc_bit_alloc_ste.c
	sbc/encoder/srce/sbc_enc_coeffs.c
	sbc/encoder/srce/sbc_encoder.c
	sbc/encoder/srce/sbc_packing.c
	lc3/src/attdet.c
	lc3/src/bits.c
	lc3/src/bwdet.c
	lc3/src/energy.c
	lc3/src/lc3.c
	lc3/src/ltpf.c
	lc3/src/mdct.c
	lc3/src/plc.c
	lc3/src/sns.c
	lc3/src/spec.c
	lc3/src/tables.c
	lc3/src/tns.c
)

ameba_list_append(private_includes
	${c_CMPT_AUDIO_DIR}/interfaces
	${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
	sbc/decoder/include
	sbc/encoder/include
	lc3/include
	lc3/src
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_external_app_library(btcodec #target name, will be part of output library name: lib_btcodec.a
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)

##########################################################################################
