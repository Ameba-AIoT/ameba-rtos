include(FetchContent)

set(cmsis_nn_dir ${download_dir}/cmsis_nn)
set(cmsis_nn_zip ${download_dir}/cmsis_nn.zip)
set(ZIP_PREFIX_NN "f2cb41ca1450a4eb4307b2779dd5aae9028285a5")
set(CMSIS_NN_MD5 "4d0e623432d6f8d3b201cbcd89218adf")

FetchContent_Declare(
  cmsis_nn
  SOURCE_DIR ${cmsis_nn_dir}
  URL      "http://github.com/ARM-software/CMSIS-NN/archive/${ZIP_PREFIX_NN}.zip"
  URL_HASH MD5=${CMSIS_NN_MD5}
)

FetchContent_MakeAvailable(cmsis_nn)
get_target_property(TARGET_COMPILE_OPTIONS cmsis-nn COMPILE_OPTIONS)
if (TARGET_COMPILE_OPTIONS)
    # The compiler option *'-fno-builtin'* does not utilize optimized implementations of e.g. memcpy and memset, 
    # which are heavily used by CMSIS-NN
    list(REMOVE_ITEM TARGET_COMPILE_OPTIONS "-fno-builtin-memcpy" "-fno-builtin-memset")
    list(APPEND TARGET_COMPILE_OPTIONS "-Wno-undef")
    set_target_properties(cmsis-nn PROPERTIES COMPILE_OPTIONS "${TARGET_COMPILE_OPTIONS}")
endif()
