/* Copyright 2022 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/micro/kernels/fully_connected.h"

#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/portable_tensor_utils.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/fully_connected.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/fully_connected.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_custom.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_custom_gemv.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"

// #define OPTIMIZE_FULLY_CONNECTED_FLOAT

namespace tflite {
namespace {
typedef std::int32_t int32;
typedef std::int8_t int8;

inline void FullyConnected_ca32(
    const FullyConnectedParams& params, const RuntimeShape& input_shape,
    const int8_t* input_data, const RuntimeShape& filter_shape,
    const int8_t* filter_data, const RuntimeShape& bias_shape,
    const int32_t* bias_data, const RuntimeShape& output_shape,
    int8_t* output_data) {
  const int32 input_offset = params.input_offset;
  const int32 filter_offset = params.weights_offset;
  const int32 output_offset = params.output_offset;
  const int32 output_multiplier = params.output_multiplier;
  const int output_shift = params.output_shift;
  const int32 output_activation_min = params.quantized_activation_min;
  const int32 output_activation_max = params.quantized_activation_max;
  TFLITE_DCHECK_GE(filter_shape.DimensionsCount(), 2);
  TFLITE_DCHECK_GE(output_shape.DimensionsCount(), 1);
  // TODO(benoitjacob): This really should be: const int batches = ArraySize(output_dims, 1);
  // but the current --variable_batch hack consists in overwriting the 3rd
  // dimension with the runtime batch size, as we don't keep track for each
  // array of which dimension is the batch dimension in it.
  const int output_dim_count = output_shape.DimensionsCount();
  const int filter_dim_count = filter_shape.DimensionsCount();
  const int batches = FlatSizeSkipDim(output_shape, output_dim_count - 1);
  const int filter_rows = filter_shape.Dims(filter_dim_count - 2);
  const int filter_cols = filter_shape.Dims(filter_dim_count - 1);
  TFLITE_DCHECK_EQ(filter_shape.FlatSize(), filter_rows * filter_cols);
  const int output_rows = output_shape.Dims(output_dim_count - 1);
  TFLITE_DCHECK_EQ(output_rows, filter_rows);
  if (bias_data) {
    TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_rows);
  }

  cpu_backend_gemm::MatrixParams<int8> lhs_params;
  lhs_params.rows = filter_rows;
  lhs_params.cols = filter_cols;
  lhs_params.order = cpu_backend_gemm::Order::kRowMajor;
  lhs_params.zero_point = -filter_offset;
  cpu_backend_gemm::MatrixParams<int8> rhs_params;
  rhs_params.rows = filter_cols;
  rhs_params.cols = batches;
  rhs_params.order = cpu_backend_gemm::Order::kColMajor;
  rhs_params.zero_point = -input_offset;
  cpu_backend_gemm::MatrixParams<int8> dst_params;
  dst_params.rows = filter_rows;
  dst_params.cols = batches;
  dst_params.order = cpu_backend_gemm::Order::kColMajor;
  dst_params.zero_point = output_offset;
  cpu_backend_gemm::GemmParams<int32, int8> gemm_params;
  gemm_params.bias = bias_data;
  gemm_params.clamp_min = output_activation_min;
  gemm_params.clamp_max = output_activation_max;
  gemm_params.multiplier_fixedpoint = output_multiplier;
  gemm_params.multiplier_exponent = output_shift;

  if(!(cpu_backend_gemm::Gemm(lhs_params, filter_data, rhs_params, input_data,
                              dst_params, output_data, gemm_params))) {
    tflite::reference_integer_ops::FullyConnected(
        params, input_shape, input_data, filter_shape, filter_data, bias_shape,
        bias_data, output_shape, output_data);
  }
}

#ifdef OPTIMIZE_FULLY_CONNECTED_FLOAT
inline void FullyConnectedFloat(
    const FullyConnectedParams& params, const RuntimeShape& input_shape,
    const float* input_data, const RuntimeShape& weights_shape,
    const float* weights_data, const RuntimeShape& bias_shape,
    const float* optional_bias_data, const RuntimeShape& output_shape,
    float* output_data /*, CpuBackendContext* cpu_backend_context*/) {

  const int dims_count = weights_shape.DimensionsCount();
  const int input_rows = weights_shape.Dims(dims_count - 1);
  const int lhs_params_rows = FlatSizeSkipDim(weights_shape, dims_count - 1);

  cpu_backend_gemm::MatrixParams<float> rhs_params;
  rhs_params.order = cpu_backend_gemm::Order::kColMajor;
  rhs_params.rows = input_rows;
  rhs_params.cols = input_shape.FlatSize() / input_rows;
  rhs_params.cache_policy =
      cpu_backend_gemm::DefaultCachePolicy(params.rhs_cacheable);
  TFLITE_DCHECK_EQ(input_shape.FlatSize(), rhs_params.rows * rhs_params.cols);
  cpu_backend_gemm::MatrixParams<float> lhs_params;
  lhs_params.order = cpu_backend_gemm::Order::kRowMajor;
  lhs_params.cols = weights_shape.Dims(dims_count - 1);
  lhs_params.rows =
      lhs_params_rows;  // FlatSizeSkipDim(weights_shape, dims_count - 1);
  lhs_params.cache_policy =
      cpu_backend_gemm::DefaultCachePolicy(params.lhs_cacheable);
  cpu_backend_gemm::MatrixParams<float> dst_params;
  dst_params.order = cpu_backend_gemm::Order::kColMajor;
  dst_params.rows = output_shape.Dims(output_shape.DimensionsCount() - 1);
  dst_params.cols =
      FlatSizeSkipDim(output_shape, output_shape.DimensionsCount() - 1);
  cpu_backend_gemm::GemmParams<float, float> gemm_params;
  gemm_params.bias = optional_bias_data;
  gemm_params.clamp_min = params.float_activation_min;
  gemm_params.clamp_max = params.float_activation_max;

  if(!(cpu_backend_gemm::Gemm(lhs_params, filter_data, rhs_params, input_data,
                              dst_params, output_data, gemm_params))) {
    tflite::reference_ops::FullyConnected(
        params, input_shape, input_data, weights_shape, weights_data,
        bias_shape, optional_bias_data, output_shape, output_data);
  }
}
#endif

void* Init(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context,
                                           sizeof(OpDataFullyConnected));
}

TfLiteStatus Prepare(TfLiteContext* context, TfLiteNode* node) {
  MicroContext* micro_context = GetMicroContext(context);

  TFLITE_DCHECK(node->user_data != nullptr);
  TFLITE_DCHECK(node->builtin_data != nullptr);

  auto* data = static_cast<OpDataFullyConnected*>(node->user_data);
  const auto params =
      static_cast<const TfLiteFullyConnectedParams*>(node->builtin_data);

  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kFullyConnectedInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* filter = micro_context->AllocateTempInputTensor(
      node, kFullyConnectedWeightsTensor);
  TF_LITE_ENSURE(context, filter != nullptr);
  TfLiteTensor* bias =
      micro_context->AllocateTempInputTensor(node, kFullyConnectedBiasTensor);
  TfLiteTensor* output = micro_context->AllocateTempOutputTensor(
      node, kFullyConnectedOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);
  TF_LITE_ENSURE_TYPES_EQ(context, input->type, output->type);

  if ((input->type == kTfLiteFloat32 && filter->type != kTfLiteFloat32) ||
      (input->type == kTfLiteInt8 &&
       (filter->type != kTfLiteInt8 && filter->type != kTfLiteInt4)) ||
      (input->type == kTfLiteInt16 && filter->type != kTfLiteInt8)) {
    MicroPrintf("Input type: %s with filter type : %s not supported.",
                TfLiteTypeGetName(input->type),
                TfLiteTypeGetName(filter->type));
    return kTfLiteError;
  }

  if (filter->type == kTfLiteInt4) {
    int filter_size =
        RuntimeShape(filter->dims->size,
                     reinterpret_cast<const int32_t*>(filter->dims->data))
            .FlatSize();
    context->RequestScratchBufferInArena(context, filter_size,
                                         &data->filter_buffer_index);
  }

  TF_LITE_ENSURE_OK(context, CalculateOpDataFullyConnected(
                                 context, params->activation, input->type,
                                 input, filter, bias, output, data));

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(filter);
  if (bias != nullptr) {
    micro_context->DeallocateTempTfLiteTensor(bias);
  }
  micro_context->DeallocateTempTfLiteTensor(output);
  return kTfLiteOk;
}

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->builtin_data != nullptr);
  const auto* params =
      static_cast<const TfLiteFullyConnectedParams*>(node->builtin_data);

  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kFullyConnectedInputTensor);
  const TfLiteEvalTensor* filter =
      tflite::micro::GetEvalInput(context, node, kFullyConnectedWeightsTensor);
  const TfLiteEvalTensor* bias =
      tflite::micro::GetEvalInput(context, node, kFullyConnectedBiasTensor);
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kFullyConnectedOutputTensor);

  TFLITE_DCHECK(node->user_data != nullptr);
  const auto& data =
      *(static_cast<const OpDataFullyConnected*>(node->user_data));

  // Checks in Prepare ensure input, output and filter types are all the same.
  switch (input->type) {
    case kTfLiteFloat32: {
      const float* bias_data =
          nullptr != bias ? tflite::micro::GetTensorData<float>(bias) : nullptr;

#ifdef OPTIMIZE_FULLY_CONNECTED_FLOAT
      tflite::FullyConnectedFloat(FullyConnectedParamsFloat(params->activation),
                                  tflite::micro::GetTensorShape(input),
                                  tflite::micro::GetTensorData<float>(input),
                                  tflite::micro::GetTensorShape(filter),
                                  tflite::micro::GetTensorData<float>(filter),
                                  tflite::micro::GetTensorShape(bias),
                                  bias_data,
                                  tflite::micro::GetTensorShape(output),
                                  tflite::micro::GetTensorData<float>(output));
#else
      tflite::reference_ops::FullyConnected(
          FullyConnectedParamsFloat(params->activation),
          tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorShape(filter),
          tflite::micro::GetTensorData<float>(filter),
          tflite::micro::GetTensorShape(bias), bias_data,
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<float>(output));
#endif
      break;
    }

    case kTfLiteInt8: {
      switch (filter->type) {
        case kTfLiteInt4: {
          int8_t* unpacked_filter_data = static_cast<int8_t*>(
              context->GetScratchBuffer(context, data.filter_buffer_index));
          tflite::tensor_utils::UnpackDenseInt4IntoInt8(
              tflite::micro::GetTensorData<int8_t>(filter),
              tflite::micro::GetTensorShape(filter).FlatSize(),
              unpacked_filter_data);
          tflite::reference_integer_ops::FullyConnected(
              FullyConnectedParamsQuantized(data),
              tflite::micro::GetTensorShape(input),
              tflite::micro::GetTensorData<int8_t>(input),
              tflite::micro::GetTensorShape(filter), unpacked_filter_data,
              tflite::micro::GetTensorShape(bias),
              tflite::micro::GetOptionalTensorData<int32_t>(bias),
              tflite::micro::GetTensorShape(output),
              tflite::micro::GetTensorData<int8_t>(output));
          break;
        }
        case kTfLiteInt8: {
          if (data.is_per_channel) {
            tflite::reference_integer_ops::FullyConnectedPerChannel(
                FullyConnectedParamsQuantized(data),
                data.per_channel_output_multiplier,
                reinterpret_cast<const int*>(data.per_channel_output_shift),
                tflite::micro::GetTensorShape(input),
                tflite::micro::GetTensorData<int8_t>(input),
                tflite::micro::GetTensorShape(filter),
                tflite::micro::GetTensorData<int8_t>(filter),
                tflite::micro::GetTensorShape(bias),
                tflite::micro::GetOptionalTensorData<int32_t>(bias),
                tflite::micro::GetTensorShape(output),
                tflite::micro::GetTensorData<int8_t>(output));
          } else {
            const int32_t* bias_data =
            nullptr != bias ? tflite::micro::GetTensorData<int32_t>(bias)
                            : nullptr;
            tflite::FullyConnected_ca32(FullyConnectedParamsQuantized(data),
                tflite::micro::GetTensorShape(input),
                tflite::micro::GetTensorData<int8_t>(input),
                tflite::micro::GetTensorShape(filter),
                tflite::micro::GetTensorData<int8_t>(filter),
                tflite::micro::GetTensorShape(bias),
                bias_data,
                tflite::micro::GetTensorShape(output),
                tflite::micro::GetTensorData<int8_t>(output));
          }
          break;
        }
        default: {
          MicroPrintf("Filter type %s (%d) not supported.",
                      TfLiteTypeGetName(filter->type), input->type);
          return kTfLiteError;
        }
      }
      break;
    }

    case kTfLiteInt16: {
      switch (filter->type) {
        case kTfLiteInt8: {
          if (data.is_per_channel) {
            tflite::reference_integer_ops::FullyConnectedPerChannel(
                FullyConnectedParamsQuantized(data),
                data.per_channel_output_multiplier,
                reinterpret_cast<const int*>(
                    data.per_channel_output_shift),
                tflite::micro::GetTensorShape(input),
                tflite::micro::GetTensorData<int16_t>(input),
                tflite::micro::GetTensorShape(filter),
                tflite::micro::GetTensorData<int8_t>(filter),
                tflite::micro::GetTensorShape(bias),
                tflite::micro::GetOptionalTensorData<int64_t>(bias),
                tflite::micro::GetTensorShape(output),
                tflite::micro::GetTensorData<int16_t>(output));
          } else {
            tflite::reference_integer_ops::FullyConnected(
                FullyConnectedParamsQuantized(data),
                tflite::micro::GetTensorShape(input),
                tflite::micro::GetTensorData<int16_t>(input),
                tflite::micro::GetTensorShape(filter),
                tflite::micro::GetTensorData<int8_t>(filter),
                tflite::micro::GetTensorShape(bias),
                tflite::micro::GetOptionalTensorData<int64_t>(bias),
                tflite::micro::GetTensorShape(output),
                tflite::micro::GetTensorData<int16_t>(output));
          }
          break;
        }
        default: {
          MicroPrintf("Filter type %s (%d) not supported.",
                      TfLiteTypeGetName(filter->type), input->type);
          return kTfLiteError;
        }
      }
      break;
    }

    default: {
      TF_LITE_KERNEL_LOG(context, "Type %s (%d) not supported.",
                         TfLiteTypeGetName(input->type), input->type);
      return kTfLiteError;
    }
  }
  return kTfLiteOk;
}

}  // namespace

TFLMRegistration Register_FULLY_CONNECTED() {
  return tflite::micro::RegisterOp(Init, Prepare, Eval);
}

}  // namespace tflite
