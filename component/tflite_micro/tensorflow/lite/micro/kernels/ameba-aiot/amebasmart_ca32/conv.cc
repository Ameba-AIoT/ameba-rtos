/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/lite/micro/kernels/conv.h"
#include "tensorflow/lite/c/builtin_op_data.h"
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/kernels/internal/portable_tensor_utils.h"
#include "tensorflow/lite/kernels/internal/quantization_util.h"
#include "tensorflow/lite/kernels/internal/reference/conv.h"
#include "tensorflow/lite/kernels/internal/reference/integer_ops/conv.h"
#include "tensorflow/lite/kernels/internal/tensor_ctypes.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/padding.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/conv.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_custom_gemv.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_custom.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/im2col_utils.h"
#include "tensorflow/lite/micro/kernels/kernel_util.h"

namespace tflite {
namespace {
using int8 = std::int8_t;
using uint8 = std::uint8_t;
using int16 = std::int16_t;
using uint16 = std::uint16_t;
using int32 = std::int32_t;
using uint32 = std::uint32_t;

const int kConvInputTensor = 0;
const int kConvWeightsTensor = 1;
const int kConvBiasTensor = 2;
const int kConvOutputTensor = 0;

// Conv is quantized along dimension 0:
// https://www.tensorflow.org/lite/performance/quantization_spec
const int kConvQuantizedDimension = 0;

bool IsIm2ColRequired(const TfLiteConvParams* params,
                      const TfLiteTensor* filter) {
  // segregate based on dilated conv & non-dialated conv
  const bool need_dilated_im2col =
      params->dilation_width_factor != 1 || params->dilation_height_factor != 1;

  if (need_dilated_im2col) {
    MicroPrintf("need_dilated_im2col optimization is not supported currently!!");
    return false;
  }
  const bool need_non_dilated_im2col =
      params->stride_width != 1 || params->stride_height != 1 ||
      filter->dims->data[2] != 1 || filter->dims->data[1] != 1;

  const bool need_im2col = need_dilated_im2col || need_non_dilated_im2col;

  return need_im2col;
}

inline void ConvFloat(const ConvParams& params, const RuntimeShape& input_shape,
                 const float* input_data, const RuntimeShape& filter_shape,
                 const float* filter_data, const RuntimeShape& bias_shape,
                 const float* bias_data, const RuntimeShape& output_shape,
                 float* output_data, const RuntimeShape& im2col_shape,
                 float* im2col_data/*, CpuBackendContext* cpu_backend_context*/) {
  const int stride_width = params.stride_width;
  const int stride_height = params.stride_height;
  const int dilation_width_factor = params.dilation_width_factor;
  const int dilation_height_factor = params.dilation_height_factor;
  const float output_activation_min = params.float_activation_min;
  const float output_activation_max = params.float_activation_max;
  TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(filter_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(im2col_shape.DimensionsCount(), 4);

  //ruy::profiler::ScopeLabel label("Conv");

  // NB: the float 0.0f value is represented by all zero bytes.
  const uint8_t float_zero_byte = 0x00;
  const float* gemm_input_data = nullptr;
  const RuntimeShape* gemm_input_shape = nullptr;
  const int filter_width = filter_shape.Dims(2);
  const int filter_height = filter_shape.Dims(1);

  const bool need_dilated_im2col =
      dilation_width_factor != 1 || dilation_height_factor != 1;
  const bool need_im2col = stride_width != 1 || stride_height != 1 ||
                           filter_width != 1 || filter_height != 1;
  if (need_dilated_im2col) {
    MicroPrintf("need_dilated_im2col optimization is not supported currently!!");
    tflite::reference_ops::Conv(
      params, input_shape,
      input_data,
      filter_shape,
      filter_data,
      bias_shape,
      bias_data,
      output_shape,
      output_data,
      tflite::micro::GetTensorShape(nullptr), nullptr);
    return;
  } else if (need_im2col) {
    TFLITE_DCHECK(im2col_data);
    tflite::optimized_ops::Im2col(params, filter_height, filter_width,
                                  float_zero_byte, input_shape, input_data,
                                  im2col_shape, im2col_data);
    gemm_input_data = im2col_data;
    gemm_input_shape = &im2col_shape;
  } else {
    //TFLITE_DCHECK(!im2col_data);
    //gemm_input_data = input_data;
    //gemm_input_shape = &input_shape;
    tflite::reference_ops::Conv(
        params, input_shape,
        input_data,
        filter_shape,
        filter_data,
        bias_shape,
        bias_data,
        output_shape,
        output_data,
        tflite::micro::GetTensorShape(nullptr), nullptr);
    return;
  }

  const int gemm_input_rows = gemm_input_shape->Dims(3);
  const int gemm_input_cols = FlatSizeSkipDim(*gemm_input_shape, 3);
  const int filter_rows = filter_shape.Dims(0);
  const int filter_cols = FlatSizeSkipDim(filter_shape, 0);
  const int output_rows = output_shape.Dims(3);
  const int output_cols =
      output_shape.Dims(0) * output_shape.Dims(1) * output_shape.Dims(2);

  TFLITE_DCHECK_EQ(output_rows, filter_rows);
  TFLITE_DCHECK_EQ(output_cols, gemm_input_cols);  //
  TFLITE_DCHECK_EQ(filter_cols, gemm_input_rows);  //
  TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_rows);

  int m = FlatSizeSkipDim(*gemm_input_shape, 3);
  int n = output_shape.Dims(3);
  int k = gemm_input_shape->Dims(3);

  cpu_backend_gemm::MatrixParams<float> lhs_params;
  lhs_params.order = cpu_backend_gemm::Order::kRowMajor;
  lhs_params.rows = n;
  lhs_params.cols = k;
  cpu_backend_gemm::MatrixParams<float> rhs_params;
  rhs_params.order = cpu_backend_gemm::Order::kColMajor;
  rhs_params.rows = k;
  rhs_params.cols = m;
  cpu_backend_gemm::MatrixParams<float> dst_params;
  dst_params.order = cpu_backend_gemm::Order::kColMajor;
  dst_params.rows = n;
  dst_params.cols = m;
  cpu_backend_gemm::GemmParams<float, float> gemm_params;
  gemm_params.bias = bias_data;
  gemm_params.clamp_min = output_activation_min;
  gemm_params.clamp_max = output_activation_max;

  if(!(cpu_backend_gemm::Gemm(lhs_params, filter_data, rhs_params, gemm_input_data,
    dst_params, output_data, gemm_params))) {
    tflite::reference_ops::Conv(
      params, input_shape, input_data,
      filter_shape, filter_data,
      bias_shape, bias_data,
      output_shape, output_data,
      tflite::micro::GetTensorShape(nullptr), nullptr);
  }
}

ConvParams ConvParamsFloatCa32(const TfLiteConvParams& params,
                           const OpDataConvCa32& data) {
  ConvParams op_params;
  CalculateActivationRange(params.activation, &op_params.float_activation_min,
                           &op_params.float_activation_max);
  op_params.padding_type = tflite::micro::RuntimePaddingType(params.padding);
  op_params.padding_values.width = data.padding.width;
  op_params.padding_values.height = data.padding.height;
  op_params.stride_width = params.stride_width;
  op_params.stride_height = params.stride_height;
  op_params.dilation_width_factor = params.dilation_width_factor;
  op_params.dilation_height_factor = params.dilation_height_factor;
  return op_params;
}

// Returns a ConvParams struct with all the parameters needed for a
// quantized computation.
ConvParams ConvParamsQuantizedCa32(const TfLiteConvParams& params,
                                   const OpDataConvCa32& data) {
  ConvParams op_params;
  op_params.input_offset = -data.input_zero_point;
  op_params.weights_offset = -data.filter_zero_point;
  op_params.output_offset = data.output_zero_point;
  op_params.output_multiplier = data.output_multiplier;
  op_params.output_shift = -data.output_shift;
  op_params.padding_type = tflite::micro::RuntimePaddingType(params.padding);
  op_params.padding_values.height = data.padding.height;
  op_params.padding_values.width = data.padding.width;
  op_params.stride_height = params.stride_height;
  op_params.stride_width = params.stride_width;
  op_params.dilation_height_factor = params.dilation_height_factor;
  op_params.dilation_width_factor = params.dilation_width_factor;
  op_params.quantized_activation_min = data.output_activation_min;
  op_params.quantized_activation_max = data.output_activation_max;
  return op_params;
}

inline void ConvPerChannelCa32(
    const ConvParams& params, const int32* output_multiplier,
    const int32* output_shift, const RuntimeShape& input_shape,
    const int8* input_data, const RuntimeShape& filter_shape,
    const int8* filter_data, const RuntimeShape& bias_shape,
    const int32* bias_data, const RuntimeShape& output_shape, int8* output_data,
    const RuntimeShape& im2col_shape, int8* im2col_data) {
  const int stride_width = params.stride_width;
  const int stride_height = params.stride_height;
  const int dilation_width_factor = params.dilation_width_factor;
  const int dilation_height_factor = params.dilation_height_factor;
  const int32 input_offset = params.input_offset;
  const int32 output_offset = params.output_offset;
  // Set min and max value of the output.
  const int32 output_activation_min = params.quantized_activation_min;
  const int32 output_activation_max = params.quantized_activation_max;
  TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(filter_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);

  const int8* gemm_input_data = nullptr;
  const RuntimeShape* gemm_input_shape = nullptr;
  const int filter_width = filter_shape.Dims(2);
  const int filter_height = filter_shape.Dims(1);
  const bool need_dilated_im2col =
      dilation_width_factor != 1 || dilation_height_factor != 1;
  const bool need_im2col = stride_width != 1 || stride_height != 1 ||
                           filter_width != 1 || filter_height != 1;
  const int8 input_zero_point = -input_offset;
  const uint8 zero_point_byte =
      *reinterpret_cast<const uint8*>(&input_zero_point);


  if (need_dilated_im2col) {
    MicroPrintf("need_dilated_im2col optimization is not supported currently!!");
    tflite::reference_integer_ops::ConvPerChannel(
      params, output_multiplier,
      output_shift, input_shape,
      input_data, filter_shape,
      filter_data, bias_shape,
      bias_data, output_shape, output_data);
    return;
  } else if (need_im2col) {
    TFLITE_DCHECK(im2col_data);
    tflite::optimized_ops::Im2col(params, filter_height, filter_width,
                                  zero_point_byte, input_shape, input_data,
                                  im2col_shape, im2col_data);
    gemm_input_data = im2col_data;
    gemm_input_shape = &im2col_shape;
  } else {
    //TFLITE_DCHECK(!im2col_data);
    //gemm_input_data = input_data;
    //gemm_input_shape = &input_shape;
    tflite::reference_integer_ops::ConvPerChannel(
        params, output_multiplier,
        output_shift, input_shape,
        input_data, filter_shape,
        filter_data, bias_shape,
        bias_data, output_shape, output_data);
    return;
  }

  const int gemm_input_rows = gemm_input_shape->Dims(3);
  const int gemm_input_cols = FlatSizeSkipDim(*gemm_input_shape, 3);
  const int filter_rows = filter_shape.Dims(0);
  const int filter_cols = FlatSizeSkipDim(filter_shape, 0);
  const int output_rows = output_shape.Dims(3);
  // See b/79927784.
  // const int output_cols = FlatSizeSkipDim(output_shape, 3);
  const int output_cols =
      output_shape.Dims(0) * output_shape.Dims(1) * output_shape.Dims(2);

  TFLITE_DCHECK_EQ(output_rows, filter_rows);
  TFLITE_DCHECK_EQ(output_cols, gemm_input_cols);  //
  TFLITE_DCHECK_EQ(filter_cols, gemm_input_rows);  //
  TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_rows);
  
  cpu_backend_gemm::MatrixParams<int8> lhs_params;
  lhs_params.rows = filter_rows;
  lhs_params.cols = filter_cols;
  lhs_params.order = cpu_backend_gemm::Order::kRowMajor;
  lhs_params.zero_point = 0;  // filter is symmetric-quantized
  cpu_backend_gemm::MatrixParams<int8> rhs_params;
  rhs_params.rows = gemm_input_rows;
  rhs_params.cols = gemm_input_cols;
  rhs_params.order = cpu_backend_gemm::Order::kColMajor;
  rhs_params.zero_point = -input_offset;
  cpu_backend_gemm::MatrixParams<int8> dst_params;
  dst_params.rows = output_rows;
  dst_params.cols = output_cols;
  dst_params.order = cpu_backend_gemm::Order::kColMajor;
  dst_params.zero_point = output_offset;
  cpu_backend_gemm::GemmParams<
      int32, int8,
      cpu_backend_gemm::QuantizationFlavor::kIntegerWithPerRowMultiplier>
      gemm_params;
  gemm_params.bias = bias_data;
  gemm_params.clamp_min = output_activation_min;
  gemm_params.clamp_max = output_activation_max;
  gemm_params.multiplier_fixedpoint_perchannel = output_multiplier;
  gemm_params.multiplier_exponent_perchannel = output_shift;

  if(!(cpu_backend_gemm::Gemm(lhs_params, filter_data, rhs_params, gemm_input_data,
      dst_params, output_data, gemm_params))) {
      tflite::reference_integer_ops::ConvPerChannel(
        params, output_multiplier,
        output_shift, input_shape,
        input_data, filter_shape,
        filter_data, bias_shape,
        bias_data, output_shape, output_data);
  }
  return;
}

TfLiteStatus CalculateOpDataConvCa32(TfLiteContext* context, TfLiteNode* node,
                                     const TfLiteConvParams& params, int width,
                                     int height, int filter_width,
                                     int filter_height, int out_width,
                                     int out_height, const TfLiteType data_type,
                                     OpDataConvCa32* data) {
  bool has_bias = node->inputs->size == 3;
  // Check number of inputs/outputs
  TF_LITE_ENSURE(context, has_bias || node->inputs->size == 2);
  TF_LITE_ENSURE_EQ(context, node->outputs->size, 1);

  // Matching GetWindowedOutputSize in TensorFlow.
  auto padding = params.padding;
  data->padding = ComputePaddingHeightWidth(
      params.stride_height, params.stride_width, params.dilation_height_factor,
      params.dilation_width_factor, height, width, filter_height, filter_width,
      padding, &out_height, &out_width);

  MicroContext* micro_context = GetMicroContext(context);

  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kConvInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* filter =
      micro_context->AllocateTempInputTensor(node, kConvWeightsTensor);
  TF_LITE_ENSURE(context, filter != nullptr);
  TfLiteTensor* bias =
      micro_context->AllocateTempInputTensor(node, kConvBiasTensor);
  TfLiteTensor* output =
      micro_context->AllocateTempOutputTensor(node, kConvOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);

  // Note that quantized inference requires that all tensors have their
  // parameters set. This is usually done during quantized training.
  if (data_type != kTfLiteFloat32) {
    int output_channels = filter->dims->data[kConvQuantizedDimension];

    TF_LITE_ENSURE_STATUS(tflite::PopulateConvolutionQuantizationParams(
        context, input, filter, bias, output, params.activation,
        &data->output_multiplier, &data->output_shift,
        &data->output_activation_min, &data->output_activation_max,
        data->per_channel_output_multiplier, data->per_channel_output_shift,
        output_channels));
  }

  data->input_zero_point = input->params.zero_point;
  data->filter_zero_point = filter->params.zero_point;
  data->output_zero_point = output->params.zero_point;

  // im2col tensor allocate
  // only for optimized kernel (float32 or int8)
  if(input->type == kTfLiteFloat32 || input->type == kTfLiteInt8) {
    data->need_im2col = IsIm2ColRequired(&params, filter);
    if (data->need_im2col) {
      int input_depth = input->dims->data[3];
      int batches = input->dims->data[0];
      int required_scratch = batches * out_height * out_width * input_depth *
                             filter_height * filter_width;
      if (input->type == kTfLiteFloat32) {
        required_scratch *= sizeof(float);
      }
      TF_LITE_ENSURE_OK(
          context, context->RequestScratchBufferInArena(
                       context, required_scratch, &data->scratch_tensor_index));

      data->im2col_size = 4;
      data->im2col_dims[0] = batches;
      data->im2col_dims[1] = out_height;
      data->im2col_dims[2] = out_width;
      data->im2col_dims[3] = input_depth * filter_height * filter_width;
    }
  }

  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(filter);
  micro_context->DeallocateTempTfLiteTensor(output);
  if (bias != nullptr) {
    micro_context->DeallocateTempTfLiteTensor(bias);
  }

  return kTfLiteOk;
}

void* Init(TfLiteContext* context, const char* buffer, size_t length) {
  TFLITE_DCHECK(context->AllocatePersistentBuffer != nullptr);
  return context->AllocatePersistentBuffer(context, sizeof(OpDataConvCa32));
}

TfLiteStatus PrepareConvCa32(TfLiteContext* context, TfLiteNode* node) {
  TFLITE_DCHECK(node->user_data != nullptr);
  TFLITE_DCHECK(node->builtin_data != nullptr);

  OpDataConvCa32* data = static_cast<OpDataConvCa32*>(node->user_data);
  const auto& params =
      *(static_cast<const TfLiteConvParams*>(node->builtin_data));
  MicroContext* micro_context = GetMicroContext(context);

  TfLiteTensor* output =
      micro_context->AllocateTempOutputTensor(node, kConvOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);
  TfLiteTensor* input =
      micro_context->AllocateTempInputTensor(node, kConvInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor* filter =
      micro_context->AllocateTempInputTensor(node, kConvWeightsTensor);
  TF_LITE_ENSURE(context, filter != nullptr);

  TF_LITE_ENSURE_EQ(context, input->type, output->type);
  TF_LITE_ENSURE_MSG(
      context,
      (input->type == kTfLiteFloat32 && filter->type == kTfLiteFloat32) ||
          (input->type == kTfLiteInt16 && filter->type == kTfLiteInt8) ||
          (input->type == kTfLiteInt8 &&
           (filter->type == kTfLiteInt4 || filter->type == kTfLiteInt8)),
      "Hybrid models are not supported on TFLite Micro.");

  const int input_width = input->dims->data[2];
  const int input_height = input->dims->data[1];
  const int filter_width = filter->dims->data[2];
  const int filter_height = filter->dims->data[1];
  const int output_width = output->dims->data[2];
  const int output_height = output->dims->data[1];

  // Dynamically allocate per-channel quantization parameters.
  const int num_channels = filter->dims->data[kConvQuantizedDimension];
  data->per_channel_output_multiplier =
      static_cast<int32_t*>(context->AllocatePersistentBuffer(
          context, num_channels * sizeof(int32_t)));
  data->per_channel_output_shift =
      static_cast<int32_t*>(context->AllocatePersistentBuffer(
          context, num_channels * sizeof(int32_t)));

  // All per-channel quantized tensors need valid zero point and scale arrays.
  if (input->type == kTfLiteInt8 || input->type == kTfLiteInt16) {
    TF_LITE_ENSURE_EQ(context, filter->quantization.type,
                      kTfLiteAffineQuantization);

    const auto* affine_quantization =
        static_cast<TfLiteAffineQuantization*>(filter->quantization.params);
    TFLITE_DCHECK(affine_quantization != nullptr);
    TFLITE_DCHECK(affine_quantization->scale != nullptr);
    TFLITE_DCHECK(affine_quantization->zero_point != nullptr);

    TF_LITE_ENSURE(context,
                   affine_quantization->scale->size == 1 ||
                       affine_quantization->scale->size ==
                           filter->dims->data[kConvQuantizedDimension]);
  }

  TF_LITE_ENSURE_STATUS(CalculateOpDataConvCa32(
      context, node, params, input_width, input_height, filter_width,
      filter_height, output_width, output_height, input->type, data));
  
  if (filter->type == kTfLiteInt4) {
    int filter_size =
        RuntimeShape(filter->dims->size,
                     reinterpret_cast<const int32_t*>(filter->dims->data))
            .FlatSize();
    context->RequestScratchBufferInArena(context, filter_size,
                                         &data->filter_buffer_index);
  }

  micro_context->DeallocateTempTfLiteTensor(filter);
  micro_context->DeallocateTempTfLiteTensor(input);
  micro_context->DeallocateTempTfLiteTensor(output);
  return kTfLiteOk;
}

TfLiteStatus Eval(TfLiteContext* context, TfLiteNode* node) {
  const TfLiteEvalTensor* input =
      tflite::micro::GetEvalInput(context, node, kConvInputTensor);
  const TfLiteEvalTensor* filter =
      tflite::micro::GetEvalInput(context, node, kConvWeightsTensor);
  const TfLiteEvalTensor* bias =
      (NumInputs(node) == 3)
          ? tflite::micro::GetEvalInput(context, node, kConvBiasTensor)
          : nullptr;
  TfLiteEvalTensor* output =
      tflite::micro::GetEvalOutput(context, node, kConvOutputTensor);

  TFLITE_DCHECK(node->builtin_data != nullptr);
  const auto& params =
      *(reinterpret_cast<TfLiteConvParams*>(node->builtin_data));
  TFLITE_DCHECK(node->user_data != nullptr);
  const auto& data = *(static_cast<const OpDataConvCa32*>(node->user_data));

  switch (input->type) {  // Already know in/out types are same.
    case kTfLiteInt8: {
      switch (filter->type) {
        case kTfLiteInt4: {
          int8_t* unpacked_filter_data = static_cast<int8_t*>(
              context->GetScratchBuffer(context, data.filter_buffer_index));
          tflite::tensor_utils::UnpackDenseInt4IntoInt8(
              tflite::micro::GetTensorData<int8_t>(filter),
              tflite::micro::GetTensorShape(filter).FlatSize(),
              unpacked_filter_data);
          reference_integer_ops::ConvPerChannel(
              ConvParamsQuantizedCa32(params, data),
              data.per_channel_output_multiplier, data.per_channel_output_shift,
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
          int8_t* im2col = NULL;
          tflite::RuntimeShape im2col_shape(data.im2col_size, data.im2col_dims);
          if (data.need_im2col) {
            im2col = static_cast<int8_t*>(
                context->GetScratchBuffer(context, data.scratch_tensor_index));
          }
        
          ConvPerChannelCa32(
              ConvParamsQuantizedCa32(params, data),
              data.per_channel_output_multiplier, data.per_channel_output_shift,
              tflite::micro::GetTensorShape(input),
              tflite::micro::GetTensorData<int8_t>(input),
              tflite::micro::GetTensorShape(filter),
              tflite::micro::GetTensorData<int8_t>(filter),
              tflite::micro::GetTensorShape(bias),
              tflite::micro::GetOptionalTensorData<int32_t>(bias),
              tflite::micro::GetTensorShape(output),
              tflite::micro::GetTensorData<int8_t>(output), im2col_shape, im2col);
          break;
        }
        default:
          MicroPrintf("Weight type %s (%d) not supported.",
                    TfLiteTypeGetName(filter->type), filter->type);
          return kTfLiteError;
      }
      break;
    }
    case kTfLiteFloat32: {
      float* im2col = NULL;
      tflite::RuntimeShape im2col_shape(data.im2col_size, data.im2col_dims);
      if (data.need_im2col) {
        im2col = static_cast<float*>(
            context->GetScratchBuffer(context, data.scratch_tensor_index));
      }
      ConvFloat(
          ConvParamsFloatCa32(params, data), tflite::micro::GetTensorShape(input),
          tflite::micro::GetTensorData<float>(input),
          tflite::micro::GetTensorShape(filter),
          tflite::micro::GetTensorData<float>(filter),
          tflite::micro::GetTensorShape(bias),
          tflite::micro::GetOptionalTensorData<float>(bias),
          tflite::micro::GetTensorShape(output),
          tflite::micro::GetTensorData<float>(output),
          im2col_shape, im2col);
      //const auto& data_non_ca32 = *(static_cast<const OpDataConv*>(node->user_data));
      //tflite::reference_ops::Conv(
      //    ConvParamsFloatCa32(params, data), tflite::micro::GetTensorShape(input),
      //    tflite::micro::GetTensorData<float>(input),
      //    tflite::micro::GetTensorShape(filter),
      //    tflite::micro::GetTensorData<float>(filter),
      //    tflite::micro::GetTensorShape(bias),
      //    tflite::micro::GetOptionalTensorData<float>(bias),
      //    tflite::micro::GetTensorShape(output),
      //    tflite::micro::GetTensorData<float>(output),
      //    tflite::micro::GetTensorShape(nullptr), nullptr);
        break;
    }
    case kTfLiteInt16: {
      if (bias == nullptr || bias->type == kTfLiteInt32) {
        reference_integer_ops::ConvPerChannel(
            ConvParamsQuantizedCa32(params, data),
            data.per_channel_output_multiplier, data.per_channel_output_shift,
            tflite::micro::GetTensorShape(input),
            tflite::micro::GetTensorData<int16_t>(input),
            tflite::micro::GetTensorShape(filter),
            tflite::micro::GetTensorData<int8_t>(filter),
            tflite::micro::GetTensorShape(bias),
            tflite::micro::GetOptionalTensorData<std::int32_t>(bias),
            tflite::micro::GetTensorShape(output),
            tflite::micro::GetTensorData<int16_t>(output));
      } else if (bias->type == kTfLiteInt64) {
        reference_integer_ops::ConvPerChannel(
            ConvParamsQuantizedCa32(params, data),
            data.per_channel_output_multiplier, data.per_channel_output_shift,
            tflite::micro::GetTensorShape(input),
            tflite::micro::GetTensorData<int16_t>(input),
            tflite::micro::GetTensorShape(filter),
            tflite::micro::GetTensorData<int8_t>(filter),
            tflite::micro::GetTensorShape(bias),
            tflite::micro::GetOptionalTensorData<std::int64_t>(bias),
            tflite::micro::GetTensorShape(output),
            tflite::micro::GetTensorData<int16_t>(output));
      } else {
        MicroPrintf("Bias type %s (%d) not supported.",
                    TfLiteTypeGetName(bias->type), bias->type);
        return kTfLiteError;
      }
      break;
    } 
    default:
      TF_LITE_KERNEL_LOG(context, "Type %s (%d) not supported.",
                         TfLiteTypeGetName(input->type), input->type);
      return kTfLiteError;
  }
  return kTfLiteOk;
}

}  // namespace

TFLMRegistration Register_CONV_2D() {
  return tflite::micro::RegisterOp(Init, PrepareConvCa32, Eval);
}
}  // namespace tflite
