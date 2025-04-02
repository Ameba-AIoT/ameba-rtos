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

#include <math.h>

#include "models/model_int8_tflm_optimized_model_data.h"
#include "testdata/input_int8_test_data.h"
#include "testdata/label_int8_test_data.h"
#include "tensorflow/lite/core/c/common.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/recording_micro_interpreter.h"


#ifdef __cplusplus
extern "C" {
#endif
#include "ameba_soc.h"
#include "os_wrapper.h"

void example_tflm_mnist(void);
#ifdef __cplusplus
}
#endif

namespace {

constexpr int kTensorArenaSize = 30000;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

tflite::MicroProfiler profiler;

using MnistOpResolver = tflite::MicroMutableOpResolver<4>;

TfLiteStatus RegisterOps(MnistOpResolver& op_resolver) {
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddMaxPool2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
    return kTfLiteOk;
}
}  // namespace


TfLiteStatus LoadQuantModelAndPerformInference() {
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model =
        ::tflite::GetModel(g_model_int8_tflm_optimized_model_data);
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

    MnistOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

    constexpr int kNumResourceVariables = 24;

    tflite::RecordingMicroAllocator* allocator(
        tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
    tflite::RecordingMicroInterpreter interpreter(
        model, op_resolver, allocator,
        tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables),
        &profiler);
    
    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
    MicroPrintf("[TFLITE-MICRO] model arena size = %u",
                interpreter.arena_used_bytes());

    TfLiteTensor* input = interpreter.input(0);
    TFLITE_CHECK_NE(input, nullptr);

    TfLiteTensor* output = interpreter.output(0);
    TFLITE_CHECK_NE(output, nullptr);

    int correct = 0;
    const int kCategoryCount = 10;
    const int input_size = g_input_int8_test_data_size / g_label_int8_test_data_size;
    float total_time_ms = 0;
    int t1 = DTimestamp_Get();
    int prediction_index = 0;
    for (unsigned int i = 0; i < g_label_int8_test_data_size; i++) {
        for (int k=0; k<input_size; k++){
            input->data.int8[k] = g_input_int8_test_data[i*input_size+k];
        }
        TF_LITE_ENSURE_STATUS(interpreter.Invoke());
        int max_value = -128;
        for (int j = 0; j < kCategoryCount; ++j) {
            if (output->data.int8[j] > max_value) {
                max_value = output->data.int8[j];
                prediction_index = j;
            }
        }
        if (prediction_index == g_label_int8_test_data[i]) {
            correct += 1;
        }
    }
    MicroPrintf("[TFLITE-MICRO] Accuracy: %d/%d\n", correct, g_label_int8_test_data_size);
    profiler.LogTicksPerTagCsv();
    MicroPrintf("");
    interpreter.GetMicroAllocator().PrintAllocations();
    MicroPrintf("");

    int t2 = DTimestamp_Get();
    total_time_ms = (float) (t2-t1) / 1000;
    MicroPrintf("[TFLITE-MICRO] Total Time: %f ms\n", total_time_ms);

    return kTfLiteOk;

}

int tflmicro_mnist(void) {
    MicroPrintf("[TFLITE-MICRO] ~~~TESTS START~~~");
    TF_LITE_ENSURE_STATUS(LoadQuantModelAndPerformInference());
    MicroPrintf("[TFLITE-MICRO] ~~~ALL TESTS PASSED~~~");
    return kTfLiteOk;
}

static void tflm_mnist_thread(void *param)
{
    (void)param;
    rtos_time_delay_ms(1000);
    tflmicro_mnist();
    rtos_task_delete(NULL);
}

void example_tflm_mnist(void)
{
    if (rtos_task_create(NULL, "example_tflm_mnist_thread", tflm_mnist_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
        printf("\n\r%s create example_tflm_mnist_thread failed", __FUNCTION__);
    }
}
