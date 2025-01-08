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

//inline void Quantize(const int32_t* multiplier, const int32_t* shift,
//                     int32_t channel_size, int32_t total_size,
//                     int32_t output_zp, int32_t output_min, int32_t output_max,
//                     int32_t* scratch, int8_t* output) {
//  //ruy::profiler::ScopeLabel label("Quantize/int8_t");
//
//  // Here we're trying to quantize the raw accumulators:
//  //        output_channels
//  //       data data data data data
//  // rows  data data data data data
//  //       data data data data data
//  //          ....
//  //
//  // In order to minimize the reload of the multipliers & shifts, once we load
//  // the multipliers & shifts, we load & quantize the raw accumulators for every
//  // row.
//#ifdef USE_NEON
//  const int32x4_t output_offset_vec = vdupq_n_s32(output_zp);
//  const int32x4_t output_activation_min_vec = vdupq_n_s32(output_min);
//  const int32x4_t output_activation_max_vec = vdupq_n_s32(output_max);
//  const int32x4_t minus_ones = vdupq_n_s32(-1);
//#endif
//
//  TFLITE_DCHECK_EQ(total_size % channel_size, 0);
//  const int32_t rows = total_size / channel_size;
//
//  int c = 0;
//
//#ifdef USE_NEON
//  for (; c <= channel_size - 8; c += 8) {
//    int32x4_t out_shift_1 = vld1q_s32(shift + c);
//    int32x4_t out_shift_2 = vld1q_s32(shift + c + 4);
//
//    int32x4_t right_shift_1 = vminq_s32(out_shift_1, minus_ones);
//    int32x4_t right_shift_2 = vminq_s32(out_shift_2, minus_ones);
//
//    int32x4_t left_shift_1 = vsubq_s32(out_shift_1, right_shift_1);
//    int32x4_t left_shift_2 = vsubq_s32(out_shift_2, right_shift_2);
//
//    int32x4_t out_mul_1 = vld1q_s32(multiplier + c);
//    int32x4_t out_mul_2 = vld1q_s32(multiplier + c + 4);
//    for (int n = 0; n < rows; ++n) {
//      int loc = n * channel_size + c;
//      int32x4_t acc_1 = vld1q_s32(scratch + loc);
//      int32x4_t acc_2 = vld1q_s32(scratch + loc + 4);
//
//      // Saturating Doubling High Mul.
//      acc_1 = vshlq_s32(acc_1, left_shift_1);
//      acc_1 = vqdmulhq_s32(acc_1, out_mul_1);
//      acc_2 = vshlq_s32(acc_2, left_shift_2);
//      acc_2 = vqdmulhq_s32(acc_2, out_mul_2);
//
//      // Rounding Dividing By POT.
//      acc_1 = vrshlq_s32(acc_1, right_shift_1);
//      acc_2 = vrshlq_s32(acc_2, right_shift_2);
//
//      // Add the output offset.
//      acc_1 = vaddq_s32(acc_1, output_offset_vec);
//      acc_2 = vaddq_s32(acc_2, output_offset_vec);
//
//      // Apply the activation function.
//      acc_1 = vmaxq_s32(acc_1, output_activation_min_vec);
//      acc_1 = vminq_s32(acc_1, output_activation_max_vec);
//      acc_2 = vmaxq_s32(acc_2, output_activation_min_vec);
//      acc_2 = vminq_s32(acc_2, output_activation_max_vec);
//
//      // Saturating cast to int8_t and store to destination.
//      const int16x4_t acc_s16_1 = vqmovn_s32(acc_1);
//      const int16x4_t acc_s16_2 = vqmovn_s32(acc_2);
//      const int16x8_t res_s16 = vcombine_s16(acc_s16_1, acc_s16_2);
//      const int8x8_t res_s8 = vqmovn_s16(res_s16);
//      vst1_s8(output + loc, res_s8);
//    }
//  }
//
//#endif  // USE_NEON
//  // Handle leftover values, one by one. This is very slow.
//  for (; c < channel_size; c++) {
//    for (int n = 0; n < rows; ++n) {
//      int loc = n * channel_size + c;
//      int32_t acc = scratch[loc];
//      acc = MultiplyByQuantizedMultiplier(acc, multiplier[c], shift[c]);
//      acc += output_zp;
//      acc = std::max(acc, output_min);
//      acc = std::min(acc, output_max);
//      output[loc] = static_cast<int8_t>(acc);
//    }
//  }
//}


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
