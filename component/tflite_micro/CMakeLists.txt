cmake_minimum_required(VERSION 3.13)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(ameba-tflite-micro C CXX)
set(MICROLITE_LIB_NAME "tensorflow-microlite")

set(tflite_dir "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite")
set(signal_dir "${CMAKE_CURRENT_SOURCE_DIR}/signal")
set(download_dir "${CMAKE_CURRENT_LIST_DIR}/downloads_${CMAKE_CHIP_TARGET}")
set(compiler_mlir_dir "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/compiler/mlir/")
set(tflite_micro_dir "${tflite_dir}/micro")
set(tflite_micro_frontend_dir "${tflite_dir}/experimental/microfrontend/lib")
set(tflite_micro_kernels_dir "${tflite_micro_dir}/kernels")

macro(specialize_files list_name high_priority_dir)
  foreach(filepath ${${list_name}})
    get_filename_component(filename ${filepath} NAME)
    if(EXISTS "${high_priority_dir}/${filename}")
      list(REMOVE_ITEM ${list_name} "${filepath}")
    endif()
  endforeach()

  FILE(GLOB high_priority_srcs "${high_priority_dir}/*.cc")
  list(APPEND srcs_kernels ${high_priority_srcs})
endmacro()

file(GLOB srcs_micro
  "${tflite_micro_dir}/debug_log.cc"
  "${tflite_micro_dir}/fake_micro_context.cc"
  "${tflite_micro_dir}/flatbuffer_utils.cc"
  "${tflite_micro_dir}/hexdump.cc"
  "${tflite_micro_dir}/memory_helpers.cc"
  "${tflite_micro_dir}/micro_allocation_info.cc"
  "${tflite_micro_dir}/micro_allocator.cc"
  "${tflite_micro_dir}/micro_context.cc"
  "${tflite_micro_dir}/micro_interpreter_context.cc"
  "${tflite_micro_dir}/micro_interpreter_graph.cc"
  "${tflite_micro_dir}/micro_interpreter.cc"
  "${tflite_micro_dir}/micro_log.cc"
  "${tflite_micro_dir}/micro_op_resolver.cc"
  "${tflite_micro_dir}/micro_profiler.cc"
  "${tflite_micro_dir}/micro_resource_variable.cc"
  "${tflite_micro_dir}/ameba-aiot/micro_time.cc"
  "${tflite_micro_dir}/micro_utils.cc"
  "${tflite_micro_dir}/mock_micro_graph.cc"
  "${tflite_micro_dir}/recording_micro_allocator.cc"
  "${tflite_micro_dir}/system_setup.cc")

file(GLOB src_micro_frontend
  "${tflite_micro_frontend_dir}/*.c"
  "${tflite_micro_frontend_dir}/*.cc")

file(GLOB srcs_tflite_bridge
  "${tflite_micro_dir}/tflite_bridge/*.c"
  "${tflite_micro_dir}/tflite_bridge/*.cc")

file(GLOB srcs_kernels
  "${tflite_micro_kernels_dir}/*.c"
  "${tflite_micro_kernels_dir}/*.cc")

file(GLOB signal_micro_kernels
  "${signal_dir}/micro/kernels/*.c"
  "${signal_dir}/micro/kernels/*.cc")

file(GLOB signal_src
  "${signal_dir}/src/*.c"
  "${signal_dir}/src/*.cc")

set(signal_srcs
  "${signal_micro_kernels}"
  "${signal_src}"
  "${signal_dir}/src/kiss_fft_wrappers/kiss_fft_float.cc"
  "${signal_dir}/src/kiss_fft_wrappers/kiss_fft_int16.cc"
  "${signal_dir}/src/kiss_fft_wrappers/kiss_fft_int32.cc")

# remove sources which will be provided by amebalite or amebasmart
if (CMAKE_CHIP_TARGET MATCHES "amebasmart_ca32*")
  specialize_files(srcs_kernels "${tflite_micro_kernels_dir}/ameba-aiot/amebasmart_ca32")

elseif (CMAKE_CHIP_TARGET MATCHES "amebalite_dsp")
  #download xa_nnlib
  set(xtensa_opt_dir ${download_dir}/xa_nnlib_hifi5)
  set(xtensa_opt_zip ${download_dir}/xa_nnlib_hifi5.zip)
  set(LIBRARY_URL "http://github.com/foss-xtensa/nnlib-hifi5/raw/master/archive/xa_nnlib_hifi5_09_05_2023.zip")
  set(LIBRARY_MD5 "1deb55ef200bf5dbedc70b99b02140c0")

  if(NOT EXISTS ${xtensa_opt_dir})
    file(DOWNLOAD
      "${LIBRARY_URL}"
      ${xtensa_opt_zip}
      EXPECTED_MD5 "${LIBRARY_MD5}")

    execute_process(COMMAND
      unzip -q -o -d ${download_dir} ${xtensa_opt_zip}
      WORKING_DIRECTORY ${download_dir})
  endif()
  specialize_files(srcs_kernels "${tflite_micro_kernels_dir}/xtensa")
  set(rtk_nn_opt_include ${xtensa_opt_dir} ${xtensa_opt_dir}/include)

elseif((CMAKE_CHIP_TARGET MATCHES "amebadplus_km4") OR (CMAKE_CHIP_TARGET MATCHES "amebalite_km4"))
  set(USE_CMSIS_NN TRUE)
  include(${CMAKE_CURRENT_LIST_DIR}/cmsis-nn.cmake)
  specialize_files(srcs_kernels "${tflite_micro_kernels_dir}/cmsis_nn")
  set(rtk_nn_opt_include ${cmsis_nn_dir})
endif()

set(lib_srcs
  "${srcs_micro}"
  "${srcs_kernels}"
  "${srcs_tflite_bridge}"
  "${src_micro_frontend}"
  "${signal_srcs}"
  "${tflite_dir}/kernels/kernel_util.cc"
  "${tflite_dir}/micro/memory_planner/greedy_memory_planner.cc"
  "${tflite_dir}/micro/memory_planner/linear_memory_planner.cc"
  "${tflite_dir}/micro/arena_allocator/non_persistent_arena_buffer_allocator.cc"
  "${tflite_dir}/micro/arena_allocator/persistent_arena_buffer_allocator.cc"
  "${tflite_dir}/micro/arena_allocator/recording_single_arena_buffer_allocator.cc"
  "${tflite_dir}/micro/arena_allocator/single_arena_buffer_allocator.cc"
  "${tflite_dir}/core/c/common.cc"
  "${compiler_mlir_dir}/lite/core/api/error_reporter.cc"
  "${tflite_dir}/core/api/flatbuffer_conversions.cc"
  "${tflite_dir}/core/api/tensor_utils.cc"
  "${tflite_dir}/kernels/internal/common.cc"
  "${tflite_dir}/kernels/internal/quantization_util.cc"
  "${tflite_dir}/kernels/internal/portable_tensor_utils.cc"
  "${tflite_dir}/kernels/internal/tensor_utils.cc"
  "${tflite_dir}/kernels/internal/tensor_ctypes.cc"
  "${tflite_dir}/kernels/internal/reference/portable_tensor_utils.cc"
  "${tflite_dir}/kernels/internal/reference/comparisons.cc"
  "${compiler_mlir_dir}/lite/schema/schema_utils.cc")

add_library(${MICROLITE_LIB_NAME} "${lib_srcs}")
target_include_directories(${MICROLITE_LIB_NAME}
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/
  ${CMAKE_CURRENT_LIST_DIR}/third_party/gemmlowp
  ${CMAKE_CURRENT_LIST_DIR}/third_party/flatbuffers/include
  ${CMAKE_CURRENT_LIST_DIR}/third_party/ruy
  ${CMAKE_CURRENT_LIST_DIR}/third_party/kissfft
  ${CMAKE_CURRENT_LIST_DIR}/signal/micro/kernels
  ${CMAKE_CURRENT_LIST_DIR}/signal/src
  ${CMAKE_CURRENT_LIST_DIR}/signal/src/kiss_fft_wrappers
  ${CMAKE_CURRENT_LIST_DIR}/tensorflow/lite/micro/testing
  ${rtk_nn_opt_include}
)

# remove -fno-strict-aliasing, which will significantly downgrade performance
get_target_property(TARGET_COMPILE_OPTIONS ${MICROLITE_LIB_NAME} COMPILE_OPTIONS)
if (TARGET_COMPILE_OPTIONS)
    list(REMOVE_ITEM TARGET_COMPILE_OPTIONS
      "-fno-strict-aliasing" "-fno-builtin-memset" "-fno-builtin-memcpy")
    set_target_properties(${MICROLITE_LIB_NAME} PROPERTIES COMPILE_OPTIONS "${TARGET_COMPILE_OPTIONS}")
endif()

set(cc_warnings
  -Wsign-compare
  -Wdouble-promotion
  -Wunused-variable
  -Wunused-function
  -Wswitch
  -Wvla
  -Wall
  -Wextra
  -Wmissing-field-initializers
  -Wstrict-aliasing
  -Wno-unused-parameter
  -Wno-unused-variable)

set(common_flags
  -Werror
  -fno-unwind-tables
  -ffunction-sections
  -fdata-sections
  -fmessage-length=0
  -DTF_LITE_STATIC_MEMORY
  -DTF_LITE_DISABLE_X86_NEON
  -DNDEBUG
  -DTF_LITE_MCU_DEBUG_LOG
  ${cc_warnings}
)

set(c_flags ${common_flags} -Wimplicit-function-declaration -std=c17)
set(cxx_flags ${common_flags} -std=c++17 -fno-exceptions -fno-threadsafe-statics -Wnon-virtual-dtor)

target_compile_options(${MICROLITE_LIB_NAME} PRIVATE
  "$<$<COMPILE_LANGUAGE:C>:${c_flags}>"
  "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>"
)
target_compile_options(${MICROLITE_LIB_NAME} INTERFACE -DTF_LITE_STATIC_MEMORY)
target_compile_options(${MICROLITE_LIB_NAME} PRIVATE -Wno-undef)
if(USE_CMSIS_NN)
  target_compile_definitions(${MICROLITE_LIB_NAME} PRIVATE CMSIS_NN)
  target_compile_definitions(${MICROLITE_LIB_NAME} PRIVATE KERNELS_OPTIMIZED_FOR_SPEED) #or KERNELS_OPTIMIZED_FOR_SIZE
endif()
if(BUILD_IN_MCU_SDK)
  target_compile_definitions(${MICROLITE_LIB_NAME} PRIVATE TF_LITE_USE_AIOT_TIME)
else()
  target_compile_definitions(${MICROLITE_LIB_NAME} PRIVATE TF_LITE_USE_CTIME)
endif()

