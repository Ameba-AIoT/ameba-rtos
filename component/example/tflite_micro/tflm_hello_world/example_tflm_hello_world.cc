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


#include "models/hello_world_int8_model_data.h"
#include "models/hello_world_float_model_data.h"
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

void example_tflm_hello_world(void);
#ifdef __cplusplus
}
#endif

namespace {

tflite::MicroProfiler profiler;
using HelloWorldOpResolver = tflite::MicroMutableOpResolver<1>;

TfLiteStatus RegisterOps(HelloWorldOpResolver& op_resolver) {
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    return kTfLiteOk;
}
}  // namespace

void HandleOutput(float x, float y) {
    // Log the current X and Y values
    MicroPrintf("x: %f, y: %f", static_cast<double>(x), static_cast<double>(y));
}

TfLiteStatus ProfileMemoryAndLatency() {
    HelloWorldOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

    // Arena size just a round number. The exact arena usage can be determined
    // using the RecordingMicroInterpreter.
    constexpr int kTensorArenaSize = 3000;
    uint8_t tensor_arena[kTensorArenaSize];
    constexpr int kNumResourceVariables = 24;

    tflite::RecordingMicroAllocator* allocator(
        tflite::RecordingMicroAllocator::Create(tensor_arena, kTensorArenaSize));
    tflite::RecordingMicroInterpreter interpreter(
        tflite::GetModel(g_hello_world_float_model_data), op_resolver, allocator,
        tflite::MicroResourceVariables::Create(allocator, kNumResourceVariables),
        &profiler);

    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
    TFLITE_CHECK_EQ(interpreter.inputs_size(), 1);
    interpreter.input(0)->data.f[0] = 1.f;
    TF_LITE_ENSURE_STATUS(interpreter.Invoke());

    MicroPrintf("");  // Print an empty new line
    profiler.LogTicksPerTagCsv();

    MicroPrintf("");  // Print an empty new line
    interpreter.GetMicroAllocator().PrintAllocations();

    return kTfLiteOk;
}

TfLiteStatus LoadFloatModelAndPerformInference() {
    const tflite::Model* model =
        ::tflite::GetModel(g_hello_world_float_model_data);
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

    HelloWorldOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

    // Arena size just a round number. The exact arena usage can be determined
    // using the RecordingMicroInterpreter.
    constexpr int kTensorArenaSize = 3000;
    uint8_t tensor_arena[kTensorArenaSize];

    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                       kTensorArenaSize);
    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());

    // Check if the predicted output is within a small range of the
    // expected output
    float epsilon = 0.05f;
    constexpr int kNumTestValues = 4;
    float golden_inputs[kNumTestValues] = {0.f, 1.f, 3.f, 5.f};

    for (int i = 0; i < kNumTestValues; ++i) {
        interpreter.input(0)->data.f[0] = golden_inputs[i];
        TF_LITE_ENSURE_STATUS(interpreter.Invoke());
        float y_pred = interpreter.output(0)->data.f[0];
        HandleOutput(golden_inputs[i], y_pred);
        TFLITE_CHECK_LE(abs(sin(golden_inputs[i]) - y_pred), epsilon);
    }

    return kTfLiteOk;
}


TfLiteStatus LoadQuantModelAndPerformInference() {
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model =
        ::tflite::GetModel(g_hello_world_int8_model_data);
    TFLITE_CHECK_EQ(model->version(), TFLITE_SCHEMA_VERSION);

    HelloWorldOpResolver op_resolver;
    TF_LITE_ENSURE_STATUS(RegisterOps(op_resolver));

    // Arena size just a round number. The exact arena usage can be determined
    // using the RecordingMicroInterpreter.
    constexpr int kTensorArenaSize = 3000;
    uint8_t tensor_arena[kTensorArenaSize];

    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena,
                                         kTensorArenaSize);

    TF_LITE_ENSURE_STATUS(interpreter.AllocateTensors());
    TfLiteTensor* input = interpreter.input(0);
    TFLITE_CHECK_NE(input, nullptr);

    TfLiteTensor* output = interpreter.output(0);
    TFLITE_CHECK_NE(output, nullptr);
    float output_scale = output->params.scale;
    int output_zero_point = output->params.zero_point;

    // Check if the predicted output is within a small range of the
    // expected output
    float epsilon = 0.05;

    constexpr int kNumTestValues = 4;
    float golden_inputs_float[kNumTestValues] = {0.77, 1.57, 2.3, 3.14};

    // The int8 values are calculated using the following formula
    // (golden_inputs_float[i] / input->params.scale + input->params.scale)
    int8_t golden_inputs_int8[kNumTestValues] = {-96, -63, -34, 0};

    for (int i = 0; i < kNumTestValues; ++i) {
        input->data.int8[0] = golden_inputs_int8[i];
        TF_LITE_ENSURE_STATUS(interpreter.Invoke());
        float y_pred = (output->data.int8[0] - output_zero_point) * output_scale;
        HandleOutput(golden_inputs_float[i], y_pred);
        TFLITE_CHECK_LE(abs(sin(golden_inputs_float[i]) - y_pred), epsilon);
    }

    return kTfLiteOk;
}

int tflmicro_hello_world(void) {

    MicroPrintf("\nProfileMemoryAndLatency");
    TF_LITE_ENSURE_STATUS(ProfileMemoryAndLatency());

    MicroPrintf("\nLoadFloatModelAndPerformInference");
    TF_LITE_ENSURE_STATUS(LoadFloatModelAndPerformInference());

    MicroPrintf("\nLoadQuantModelAndPerformInference");
    TF_LITE_ENSURE_STATUS(LoadQuantModelAndPerformInference());

    MicroPrintf("~~~ALL TESTS PASSED~~~\n");
    return kTfLiteOk;
}

static void tflm_hello_world_thread(void *param)
{
    (void)param;
    rtos_time_delay_ms(1000);
    tflmicro_hello_world();
    rtos_task_delete(NULL);
}

void example_tflm_hello_world(void)
{
    if (rtos_task_create(NULL, "example_tflm_hello_world_thread", tflm_hello_world_thread, NULL, 1024 * 6, 1) != RTK_SUCCESS) {
        printf("\n\r%s create example_tflm_hello_world_thread failed", __FUNCTION__);
    }
}
