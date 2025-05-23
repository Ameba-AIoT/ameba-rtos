/* Copyright 2021 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/transpose_conv.h"

#include <cstddef>
#include <cstdint>

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/transpose.h"
#include "tensorflow/lite/kernels/internal/reference/transpose_conv.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/transpose_conv.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/internal/types.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/padding.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_custom.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_custom_gemv.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"
#include "tensorflow/lite/micro/micro_log.h"

namespace tflite {
namespace {
using int8 = std::int8_t;
using uint8 = std::uint8_t;
using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;

// For the TfLite transpose_conv implementation, input tensor 0 corresponds to
// the OutputShapeTensor. However, since TFLM does not support dynamic tensors,
// the TFLM implementation ignores input tensor 0 and the only inputs we care
// about are kFilterTensor, kInputTensor and kBiasTensor.
constexpr int kFilterTensor = 1;
constexpr int kInputTensor = 2;
constexpr int kBiasTensor = 3;
constexpr int kOutputTensor = 0;

// Conv is quantized along dimension 0:
// https://www.tensorflow.org/lite/performance/quantization_spec
constexpr int kConvQuantizedDimension = 0;

inline PaddingType RuntimePaddingType(TfLitePadding padding) {
  switch (padding) {
    case TfLitePadding::kTfLitePaddingSame:
      return PaddingType::kSame;
    case TfLitePadding::kTfLitePaddingValid:
      return PaddingType::kValid;
    case TfLitePadding::kTfLitePaddingUnknown:
    default:
      return PaddingType::kNone;
  }
}

TfLiteStatus CalculateOpData(TfLiteContext* context, TfLiteNode* node,
                             const TfLiteTransposeConvParams* params, int width,
                             int height, int filter_width, int filter_height,
                             const TfLiteType data_type, OpDataTransposeConvCa32* data) {
  bool has_bias = node->inputs->size == 4;
  // Check number of inputs/outputs
  TF_LITE_ENSURE(context, has_bias || node->inputs->size == 3);
  TF_LITE_ENSURE_EQ(context, node->outputs->size, 1);

  // Matching GetWindowedOutputSize in TensorFlow.
  auto padding = params->padding;
  int unused_output_width;
  int unused_output_height;
  TfLitePaddingValues padding_values = ComputePaddingHeightWidth(
      params->stride_height, params->stride_width, 1,
      1,  // Dilation height and width are always 1 for transpose_conv.
      height, width, filter_height, filter_width, padding,
      &unused_output_height, &unused_output_width);

  data->params.padding_type = RuntimePaddingType(padding);
  data->params.padding_values.width = padding_values.width;
  data->params.padding_values.height = padding_values.height;
  data->params.padding_values.width_offset = padding_values.width_offset;
  data->params.padding_values.height_offset = padding_values.height_offset;

  MicroContext* micro_context = GetMicroContext(context);

  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* filter =
      micro_context->AllocateTempInputTensor(node, kFilterTensor);
  TF_LITE_ENSURE(context, filter != nullptr);
  TfLiteTensor* bias =
      micro_context->AllocateTempInputTensor(node, kBiasTensor);
  TfLiteTensor* output =
      micro_context->AllocateTempOutputTensor(node, kOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);
  
  // Note that quantized inference requires that all tensors have their
  // parameters set. This is usually done during quantized training.
  if (data_type != kTfLiteFloat32) {
    int output_channels = filter->dims->data[kConvQuantizedDimension];

    TF_LITE_ENSURE_STATUS(tflite::PopulateConvolutionQuantizationParams(
        context, input, filter, bias, output, kTfLiteActNone,
        &data->params.output_multiplier, &data->params.output_shift,
        &data->params.quantized_activation_min,
        &data->params.quantized_activation_max,
        data->per_channel_output_multiplier, data->per_channel_output_shift,
        output_channels));
  }
  // TODO(b/192090531): Remove this once all 8x16 transpose conv models use
  // 64-bit biases.
  if (input->type == kTfLiteInt16) {
    TFLITE_DCHECK(filter->type == kTfLiteInt8);
    TFLITE_DCHECK(output->type == kTfLiteInt16);
    // Handle the case where the bias is 16 bits for 16x8 transpose
    // convolution where the kernel actually expects 64-bit biases.
    if (bias != nullptr && bias->type == kTfLiteInt16) {
      TFLITE_DCHECK(
          context->RequestScratchBufferInArena(
              context, GetTensorShape(bias).FlatSize() * sizeof(std::int64_t),
              &(data->bias_converted_buffer_index)) == kTfLiteOk);
    }
  }

  // only support Float32 optimization
  if (input->type == kTfLiteFloat32) {
    // col2im
    data->col2im_size = 2;
    data->col2im_dims[0] = input->dims->data[1] * input->dims->data[2];
    data->col2im_dims[1] = filter->dims->data[0] * filter->dims->data[1] * filter->dims->data[2];
    int required_scratch = data->col2im_dims[0] * data->col2im_dims[1];
    if (input->type == kTfLiteFloat32) {
      required_scratch *= sizeof(float);
    }
    TF_LITE_ENSURE_OK(
        context, context->RequestScratchBufferInArena(
                     context, required_scratch, &data->col2im_buffer_index));
    // Transpose filter (HWOI order)
    data->transposed_filter_size = 4;
    int required_filter_size = SizeOfDimension(filter, 0)
                          * filter_height * filter_width
                          *SizeOfDimension(filter,3);
    data->transposed_filter_dims[0] = SizeOfDimension(filter, 1);
    data->transposed_filter_dims[1] = SizeOfDimension(filter, 2);
    data->transposed_filter_dims[2] = SizeOfDimension(filter, 0);
    data->transposed_filter_dims[3] = SizeOfDimension(filter, 3);
    if (input->type == kTfLiteFloat32) {
      required_filter_size *= sizeof(float);
    }
    TF_LITE_ENSURE_OK(
      context, context->RequestScratchBufferInArena(
                   context, required_filter_size, &data->transposed_filter_buffer_index));
  }

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(filter);
  micro_context->DeallocateTempTfLiteTensor(output);
  if (bias != nullptr) {
    micro_context->DeallocateTempTfLiteTensor(bias);
  }

  return kTfLiteOk;
}

void* TransposeConvInit(TfLiteContext* context, const char* buffer,
                        size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpDataTransposeConvCa32));
}

TfLiteStatus TransposeConvPrepare(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);
  TFLITE_DCHECK(node->builtin_data != nullptr);

  OpDataTransposeConvCa32* data = static_cast<OpDataTransposeConvCa32*>(node->user_data);
  const auto params =
      static_cast<const TfLiteTransposeConvParams*>(node->builtin_data);

  MicroContext* micro_context = GetMicroContext(context);

  TfLiteTensor* output =
      micro_context->AllocateTempOutputTensor(node, kOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);
  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* filter =
      micro_context->AllocateTempInputTensor(node, kFilterTensor);
  TF_LITE_ENSURE(context, filter != nullptr);

  TF_LITE_ENSURE_MSG(
      context,
      input->type == filter->type ||
          (input->type == kTfLiteInt16 && filter->type == kTfLiteInt8),
      "Hybrid models are not supported on TFLite Micro.");

  // Get height and width of the output.
  const int width = SizeOfDimension(output, 2);
  const int height = SizeOfDimension(output, 1);
  const int filter_width = SizeOfDimension(filter, 2);
  const int filter_height = SizeOfDimension(filter, 1);

  // Dynamically allocate per-channel quantization parameters.
  const int num_channels = filter->dims->data[kConvQuantizedDimension];
  data->per_channel_output_multiplier =
      static_cast<int32_t*>(context->AllocatePersistentBuffer(
          context, num_channels * sizeof(int32_t)));
  data->per_channel_output_shift =
      static_cast<int32_t*>(context->AllocatePersistentBuffer(
          context, num_channels * sizeof(int32_t)));

  // Quantized kernels use an int32 scratch buffer.
  if (input->type == kTfLiteInt8) {
    TFLITE_DCHECK(context->RequestScratchBufferInArena != nullptr);
    TFLITE_DCHECK(context->RequestScratchBufferInArena(
                      context,
                      GetTensorShape(output).FlatSize() * sizeof(int32_t),
                      &(data->scratch_buffer_index)) == kTfLiteOk);
  }

  // Quantized 16x8 kernels use an int64 scratch buffer.
  if (input->type == kTfLiteInt16) {
    TFLITE_DCHECK(context->RequestScratchBufferInArena != nullptr);
    TFLITE_DCHECK(context->RequestScratchBufferInArena(
                      context,
                      GetTensorShape(output).FlatSize() * sizeof(std::int64_t),
                      &(data->scratch_buffer_index)) == kTfLiteOk);
  }

  // All per-channel quantized tensors need valid zero point and scale arrays.
  if (input->type == kTfLiteInt8 || input->type == kTfLiteInt16) {
    TF_LITE_ENSURE_EQ(context, filter->quantization.type,
                      kTfLiteAffineQuantization);

    const auto* affine_quantization =
        static_cast<TfLiteAffineQuantization*>(filter->quantization.params);
    TF_LITE_ENSURE(context, affine_quantization);
    TF_LITE_ENSURE(context, affine_quantization->scale);
    TF_LITE_ENSURE(context, affine_quantization->zero_point);

    TF_LITE_ENSURE(context,
                   affine_quantization->scale->size == 1 ||
                       affine_quantization->scale->size ==
                           filter->dims->data[kConvQuantizedDimension]);
    TF_LITE_ENSURE_EQ(context, affine_quantization->scale->size,
                      affine_quantization->zero_point->size);
  }

  TF_LITE_ENSURE_STATUS(CalculateOpData(context, node, params, width, height,
                                        filter_width, filter_height,
                                        input->type, data));

  // Offsets (zero points)
  data->params.input_offset = -input->params.zero_point;
  data->params.weights_offset = -filter->params.zero_point;
  data->params.output_offset = output->params.zero_point;

  // Stride
  data->params.stride_width = params->stride_width;
  data->params.stride_height = params->stride_height;

  micro_context->DeallocateTempTfLiteTensor(output);
  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(filter);
  return kTfLiteOk;
}

// TransposeConv Float32 optimization
// TransposeConvV2 expect the weights in HWOI order.
void TransposeConvV2(
    const ConvParams& params, const RuntimeShape& input_shape,
    const float* input_data, const RuntimeShape& hwoi_ordered_filter_shape,
    const float* hwoi_ordered_filter_data, const RuntimeShape& bias_shape,
    const float* bias_data, const RuntimeShape& output_shape,
    float* const output_data, const RuntimeShape& col2im_shape,
    float* col2im_data/*, CpuBackendContext* cpu_backend_context*/) {
  //ruy::profiler::ScopeLabel label("TransposeConvV2/float");
  TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(hwoi_ordered_filter_shape.DimensionsCount(), 4);
  TFLITE_DCHECK(col2im_data);
  TFLITE_DCHECK(hwoi_ordered_filter_data);

  const int batch_size = MatchingDim(input_shape, 0, output_shape, 0);
  const int input_image_size = input_shape.Dims(1) * input_shape.Dims(2);
  const int output_height = output_shape.Dims(1);
  const int output_width = output_shape.Dims(2);
  const int output_image_size = output_height * output_width;
  const int input_depth =
      MatchingDim(input_shape, 3, hwoi_ordered_filter_shape, 3);
  const int output_depth =
      MatchingDim(output_shape, 3, hwoi_ordered_filter_shape, 2);
  const int input_offset = input_image_size * input_depth;
  const int output_offset = output_image_size * output_depth;

  const int filter_height = hwoi_ordered_filter_shape.Dims(0);
  const int filter_width = hwoi_ordered_filter_shape.Dims(1);
  const int padding_top = params.padding_values.height;
  const int padding_bottom =
      params.padding_values.height + params.padding_values.height_offset;
  const int padding_left = params.padding_values.width;
  const int padding_right =
      params.padding_values.width + params.padding_values.width_offset;
  const int stride_height = params.stride_height;
  const int stride_width = params.stride_width;

  const float output_activation_min = params.float_activation_min;
  const float output_activation_max = params.float_activation_max;

  const int hwoi_ordered_filter_total_size =
      filter_height * filter_width * output_depth;

  cpu_backend_gemm::MatrixParams<float> lhs_params;
  lhs_params.order = cpu_backend_gemm::Order::kRowMajor;
  lhs_params.rows = hwoi_ordered_filter_total_size;
  lhs_params.cols = input_depth;
  float* output_data_p = output_data;
  std::fill_n(output_data, output_offset * batch_size, 0.0f);
  for (int i = 0; i < batch_size; ++i) {
    cpu_backend_gemm::MatrixParams<float> rhs_params;
    rhs_params.order = cpu_backend_gemm::Order::kColMajor;
    rhs_params.rows = input_depth;
    rhs_params.cols = input_image_size;
    cpu_backend_gemm::MatrixParams<float> dst_params;
    dst_params.order = cpu_backend_gemm::Order::kColMajor;
    dst_params.rows = hwoi_ordered_filter_total_size;
    dst_params.cols = input_image_size;
    cpu_backend_gemm::GemmParams<float, float> gemm_params;

    // lhs_params.rows >=4 and lhs_params.cols >=8 have been assured
    // by the caller. So we can use the optimized gemm.
    cpu_backend_gemm::Gemm(lhs_params, hwoi_ordered_filter_data, rhs_params,
                           input_data + input_offset * i, dst_params,
                           col2im_data, gemm_params);

    optimized_ops::Col2im(col2im_data, output_depth, output_height, output_width,
           filter_height, filter_width, padding_top, padding_left,
           padding_bottom, padding_right, stride_height, stride_width,
           output_data_p);
    output_data_p += output_offset;
  }
  output_data_p = output_data;
  optimized_ops::BiasAdd(output_data_p, bias_data, batch_size, output_height, output_width,
          output_depth);

  for (int i = 0; i < output_offset * batch_size; ++i) {
    output_data[i] = std::min(std::max(output_data[i], output_activation_min),
                              output_activation_max);
  }
}

TfLiteStatus TransposeConvEval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kInputTensor);
  const TfLiteEvalTensor* filter =
      tflite::micro::GetEvalInput(context, node, kFilterTensor);
  const TfLiteEvalTensor* bias =
      (NumInputs(node) == 4)
          ? tflite::micro::GetEvalInput(context, node, kBiasTensor)
          : nullptr;
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kOutputTensor);

  TFLITE_DCHECK(node->user_data != nullptr);
  const OpDataTransposeConvCa32& data = *(static_cast<const OpDataTransposeConvCa32*>(node->user_data));

  TF_LITE_ENSURE_EQ(context, input->type, output->type);

  switch (input->type) {  // Already know in/out types are same.
    case kTfLiteFloat32: {
      const auto& params =
          *(reinterpret_cast<TfLiteConvParams*>(node->builtin_data));
      ConvParams op_params = data.params;
      CalculateActivationRange(params.activation,
                               &op_params.float_activation_min,
                               &op_params.float_activation_max);
      // Transposed filter
      const RuntimeShape& filter_shape = tflite::micro::GetTensorShape(filter);
      const RuntimeShape& input_shape = tflite::micro::GetTensorShape(input);
      
      // check lhs_cols and lhs_rows here to avoid unnecessary calculation of transposed filter and col2im
      const int lhs_cols = input_shape.Dims(3);
      const int lhs_rows = filter_shape.Dims(0) * filter_shape.Dims(1) * filter_shape.Dims(2);
      if(lhs_cols < 8 || lhs_rows < 4) {
        reference_ops::TransposeConv(
          op_params, tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorShape(filter),
          tflite::micro::GetTensorData<float>(filter),
          tflite::micro::GetTensorShape(bias),
          tflite::micro::GetOptionalTensorData<float>(bias),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<float>(output),
          tflite::micro::GetTensorShape(nullptr), nullptr);
        return kTfLiteOk;
      }

      float* transposed_filter = static_cast<float*>(
          context->GetScratchBuffer(context, data.transposed_filter_buffer_index));
      RuntimeShape hwoi_ordered_filter_shape(data.transposed_filter_size, data.transposed_filter_dims);
      TransposeParams transpose_params;
      transpose_params.perm_count = 4;
      transpose_params.perm[0] = 1;
      transpose_params.perm[1] = 2;
      transpose_params.perm[2] = 0;
      transpose_params.perm[3] = 3;
      reference_ops::Transpose(transpose_params, filter_shape,
                                 tflite::micro::GetTensorData<float>(filter),
                                 hwoi_ordered_filter_shape,
                                 transposed_filter);
      // col2im
      float* col2im = NULL;
      RuntimeShape col2im_shape(data.col2im_size, data.col2im_dims);
      col2im = static_cast<float*>(
          context->GetScratchBuffer(context, data.col2im_buffer_index));

      // Eval float
      TransposeConvV2(
          op_params, tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<float>(input),
          hwoi_ordered_filter_shape,
          transposed_filter,
          tflite::micro::GetTensorShape(bias),
          tflite::micro::GetOptionalTensorData<float>(bias),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<float>(output),
          col2im_shape, col2im);
      break;
    }
    case kTfLiteInt8: {
      int32_t* scratch_buffer = static_cast<int32_t*>(
          context->GetScratchBuffer(context, data.scratch_buffer_index));
      reference_integer_ops::TransposeConv(
          data.params, data.per_channel_output_multiplier,
          data.per_channel_output_shift, tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<int8_t>(input),
          tflite::micro::GetTensorShape(filter),
          tflite::micro::GetTensorData<int8_t>(filter),
          tflite::micro::GetTensorShape(bias),
          tflite::micro::GetOptionalTensorData<int32_t>(bias),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<int8_t>(output),
          tflite::micro::GetTensorShape(nullptr), nullptr, scratch_buffer);
      break;
    }
    case kTfLiteInt16: {
      auto* scratch_buffer = static_cast<int64_t*>(
          context->GetScratchBuffer(context, data.scratch_buffer_index));
      if (bias != nullptr && bias->type == kTfLiteInt16) {
        auto* bias_converted_buffer =
            static_cast<int64_t*>(context->GetScratchBuffer(
                context, data.bias_converted_buffer_index));
        for (int i = 0; i < tflite::micro::GetTensorShape(bias).FlatSize();
             i++) {
          bias_converted_buffer[i] = bias->data.i16[i];
        }
        reference_integer_ops::TransposeConv(
            data.params, data.per_channel_output_multiplier,
            data.per_channel_output_shift, tflite::micro::GetTensorShape(input),
            tflite::micro::GetTensorData<int16_t>(input),
            tflite::micro::GetTensorShape(filter),
            tflite::micro::GetTensorData<int8_t>(filter),
            tflite::micro::GetTensorShape(bias), bias_converted_buffer,
            tflite::micro::GetTensorShape(output),
            tflite::micro::GetTensorData<int16_t>(output),
            tflite::micro::GetTensorShape(nullptr), nullptr, scratch_buffer);
      } else {
        reference_integer_ops::TransposeConv(
            data.params, data.per_channel_output_multiplier,
            data.per_channel_output_shift, tflite::micro::GetTensorShape(input),
            tflite::micro::GetTensorData<int16_t>(input),
            tflite::micro::GetTensorShape(filter),
            tflite::micro::GetTensorData<int8_t>(filter),
            tflite::micro::GetTensorShape(bias),
            tflite::micro::GetOptionalTensorData<std::int64_t>(bias),
            tflite::micro::GetTensorShape(output),
            tflite::micro::GetTensorData<int16_t>(output),
            tflite::micro::GetTensorShape(nullptr), nullptr, scratch_buffer);
      }
      break;
    }
    default:
      MicroPrintf("Type %s (%d) not supported.", TfLiteTypeGetName(input->type),
                  input->type);
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace

TFLMRegistration Register_TRANSPOSE_CONV() {
  return tflite::micro::RegisterOp(TransposeConvInit, TransposeConvPrepare,
                                   TransposeConvEval);
}

}  // namespace tflite
