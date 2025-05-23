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

#ifndef TENSORFLOW_LITE_KERNELS_CPU_BACKEND_GEMM_CUSTOM_H_
#define TENSORFLOW_LITE_KERNELS_CPU_BACKEND_GEMM_CUSTOM_H_

#include <cstdint>

#include "ruy/profiler/instrumentation.h"  // from @ruy
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_custom_gemv.h"
#include "tensorflow/lite/micro/kernels/ameba-aiot/amebasmart_ca32/cpu_backend_gemm_params.h"


namespace tflite {

namespace cpu_backend_gemm {
// This is an ameba-tflm custom implementation of the gemm function that uses
// the custom gemv function to perform the matrix multiplication.
namespace detail {

  template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
            typename DstScalar, QuantizationFlavor quantization_flavor>
  struct GemmImplCustom {
    static bool Run(const MatrixParams<LhsScalar>& lhs_params, const LhsScalar* lhs_data,
                    const MatrixParams<RhsScalar>& rhs_params, const RhsScalar* rhs_data,
                    const MatrixParams<DstScalar>& dst_params, DstScalar* dst_data,
                    const GemmParams<AccumScalar, DstScalar, quantization_flavor>& params) {
      bool result = true;
      for (int r_c = 0; r_c < rhs_params.cols; ++r_c) {
        result &= tflite::cpu_backend_gemm::detail::CustomGemv(
            lhs_params, lhs_data, rhs_params, rhs_data + rhs_params.rows * r_c,
            dst_params, dst_data + dst_params.rows * r_c, params);
        if (!result) {
          return false;
        }
      }
      return result; 
    }
  };
  
}  // namespace detail

// The main entry point for CpuBackendGemm::Gemm.

/* Public entry point */

template <typename LhsScalar, typename RhsScalar, typename AccumScalar,
          typename DstScalar, QuantizationFlavor quantization_flavor>
bool Gemm(const MatrixParams<LhsScalar>& lhs_params, const LhsScalar* lhs_data,
          const MatrixParams<RhsScalar>& rhs_params, const RhsScalar* rhs_data,
          const MatrixParams<DstScalar>& dst_params, DstScalar* dst_data,
          const GemmParams<AccumScalar, DstScalar, quantization_flavor>& params) {
  ruy::profiler::ScopeLabel label("cpu_backend_gemm::Gemm");
  ValidateParams(lhs_params, rhs_params, dst_params, params);
  if (!IsValidGemm(lhs_params, rhs_params, dst_params)) {
    // For now, assert in debug mode, return in opt.
    // TODO(b/183099395) Eliminate debug/release discrepancy by plumbing in
    // TFLiteStatus so we can return an error here.
    TFLITE_DCHECK(false);
    return false;
  }
  // If we did not choose to force usage of ruy above, then we may now consider
  // using custom GEMV code for the matrix*vector cases.
  const bool try_custom_gemv = (dst_params.cols == 1);
  if (try_custom_gemv) {
    // GEMV case: try a custom fast GEMV path. It will return true if it
    // actually handled it.
    return detail::CustomGemv(lhs_params, lhs_data, rhs_params, rhs_data,
                           dst_params, dst_data, params);
  }
  // Generic case: dispatch to any backend as a general GEMM.
  return detail::GemmImplCustom<LhsScalar, RhsScalar, AccumScalar, DstScalar,
           quantization_flavor>::Run(lhs_params, lhs_data, rhs_params, rhs_data,
                                     dst_params, dst_data, params);
}

}  // namespace cpu_backend_gemm

}  // namespace tflite

#endif  // TENSORFLOW_LITE_KERNELS_CPU_BACKEND_GEMM_CUSTOM_H_
