/* Copyright 2023 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_LITE_MICRO_KERNELS_TRANSPOSE_CONV_H_
#define TENSORFLOW_LITE_MICRO_KERNELS_TRANSPOSE_CONV_H_

#include <cstdint>
#include <algorithm>

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/micro/micro_common.h"

namespace tflite {

static constexpr int kMaxSmallSize = 5;
struct OpDataTransposeConvCa32 {
  ConvParams params;

  // A scratch buffer is required for optimization implementations.
  int scratch_buffer_index;

  // Index to the converted 64-bit bias buffer from 16-bit bias. This is
  // required to handle 16x8 transpose convolutions where a 16-bit bias is
  // provided, whereas the kernel expects 64-bit biases.
  int bias_converted_buffer_index;

  // Multiplier and shift arrays are required for the int8 implementation.
  int32_t* per_channel_output_multiplier;
  int32_t* per_channel_output_shift;

  // transposed weights (hwoi) for optimization
  int32_t transposed_filter_size = 0;
  int32_t transposed_filter_dims[kMaxSmallSize];
  int transposed_filter_buffer_index;

  // col2im for optimization
  int32_t col2im_size = 0;
  int32_t col2im_dims[kMaxSmallSize];
  int col2im_buffer_index;
};

namespace optimized_ops {
// Returns in 'im_data' (assumes to be zero-initialized) image patch in storage
// order (height, width, depth), constructed from patches in 'col_data', which
// is required to be in storage order (out_height * out_width, filter_height,
// filter_width, in_depth).  Implementation by Yangqing Jia (jiayq).
// Copied from //tensorflow/core/kernels/conv_grad_input_ops.cc
template <typename T>
void Col2im(const T* col_data, const int depth, const int height,
            const int width, const int filter_h, const int filter_w,
            const int pad_t, const int pad_l, const int pad_b, const int pad_r,
            const int stride_h, const int stride_w, T* im_data) {
  //ruy::profiler::ScopeLabel label("Col2im");
  int height_col = (height + pad_t + pad_b - filter_h) / stride_h + 1;
  int width_col = (width + pad_l + pad_r - filter_w) / stride_w + 1;
  int h_pad = -pad_t;
  for (int h = 0; h < height_col; ++h) {
    int w_pad = -pad_l;
    for (int w = 0; w < width_col; ++w) {
      T* im_patch_data = im_data + (h_pad * width + w_pad) * depth;
      for (int ih = h_pad; ih < h_pad + filter_h; ++ih) {
        for (int iw = w_pad; iw < w_pad + filter_w; ++iw) {
          if (ih >= 0 && ih < height && iw >= 0 && iw < width) {
            // TODO(andydavis) Vectorize this loop (if compiler does not).
            for (int i = 0; i < depth; ++i) {
              im_patch_data[i] += col_data[i];
            }
          }
          im_patch_data += depth;
          col_data += depth;
        }
        // Jump over remaining number of depth.
        im_patch_data += depth * (width - filter_w);
      }
      w_pad += stride_w;
    }
    h_pad += stride_h;
  }
}

// TODO(b/188008864) Optimize this function by combining outer loops.
template <typename T>
void BiasAdd(T* im_data, const T* bias_data, const int batch_size,
             const int height, const int width, const int depth) {
  if (bias_data) {
    for (int n = 0; n < batch_size; ++n) {
      for (int h = 0; h < height; ++h) {
        for (int w = 0; w < width; ++w) {
          for (int d = 0; d < depth; ++d) {
            im_data[d] += bias_data[d];
          }
          im_data += depth;
        }
      }
    }
  }
}

} // namespace optimized_ops

// This is the most generic TFLMRegistration. The actual supported types
// may still be target dependent. The only requirement is that every
// implementation (reference or optimized) must define this function.
TFLMRegistration Register_TRANSPOSE_CONV();

#if defined(CMSIS_NN)
// Returns a TFLMRegistration struct for kernel variant that only supports
// int8.
TFLMRegistration Register_TRANSPOSE_CONV_INT8();

#else
// Note that while this block gets used for both reference and optimized kernels
// that do not have any specialized implementations, the only goal here is to
// define fallback implementation that allow reference kernels to still be used
// from applications that call a more specific kernel variant.

inline TFLMRegistration Register_TRANSPOSE_CONV_INT8() {
  return Register_TRANSPOSE_CONV();
}

#endif

}  // namespace tflite

#endif  // TENSORFLOW_LITE_MICRO_KERNELS_TRANSPOSE_CONV_H_
