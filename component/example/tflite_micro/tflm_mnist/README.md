# MNIST Example

## Description

The *MNIST* database (Modified National Institute of Standards and Technology database) is a large collection of handwritten digits. In this tutorial, MNIST database is used to show a full process from training a model to deploying it and run inference on Ameba SoCs with tflite-micro.

## Supported IC
1. AmebaSmart CA32
2. AmebaLite KM4
3. AmebaLite DSP
4. AmebaDplus KM4

## Tutorial

Step 1-4  are for preparing necessary files on a development machine (server or PC etc.). You can skip them and use prepared files to build the image.

### Step 1. Train a Model

First train and evaluate a classification model for 10 digits of MNIST dataset. You can choose either keras(tensorflow) or pytorch framework by running `python keras_train_eval.py --output keras_mnist_conv` or `python torch_train_eval.py --output torch_mnist_conv`.

A simple convolution based model will be trained for several epochs and then accuracy will be tested.

Due to the limited **computation resources** and **memory** of microcontrollers, we recommend paying attention to **model size** and **operation numbers**.

Use *keras_flops* library under tensorflow/keras framework:

```
from keras_flops import get_flops

model.summary()
flops = get_flops(model, batch_size=1)
```

Use *ptflops* library under pytorch framework:

```
from ptflops import get_model_complexity_info

macs, params = get_model_complexity_info(model, (1,28,28), as_strings=False)
```

After training, keras model is saved in SavedModel format. Pytorch model is saved in .pt format, while a .onnx file is also exported for later conversion stage.

### Step 2. Convert to Tflite

In this stage, **post-training integer quantization** is applied on the trained model and output .tflite format. Float model inference is also supported on Ameba SoCs, however, we recommend using integer quantization which can extremely reduce computation and memory with little performance degradation.

For models trained by keras(tensorflow), run `python convert.py --input-path keras_mnist_conv/saved_model --output-path keras_mnist_conv`.

For models trained by pytorch, run `python convert.py --input-path torch_mnist_conv/model.onnx --output-path torch_mnist_conv`. An additional step will run to convert from .onnx to SavedModel format. 

tf.lite.TFLiteConverter is used to convert SavedModel into int8 .tflite given a representative dataset:

```
converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.representative_dataset = repr_dataset
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
converter.inference_input_type = tf.int8
converter.inference_output_type = tf.int8
tflite_int8_model = converter.convert()
```

Refer to https://ai.google.dev/edge/litert/models/post_training_integer_quant#convert_using_integer-only_quantization for more details.

After conversion, the performance on test set will be validated using int8 .tflite model and two .npy files containing input array and label array of 100 test images are generated for later use on SoC.

In *convert.py*, onnx_tf library (https://github.com/onnx/onnx-tensorflow) is used for converting from onnx to SavedModel. Other convert libraries are available with similar purpose:

onnx2tf https://github.com/PINTO0309/onnx2tf

ai-edge-torch https://github.com/google-ai-edge/ai-edge-torch

nobuco https://github.com/AlexanderLutsenko/nobuco

onnx2tflite https://github.com/MPolaris/onnx2tflite

### Step 3. Optimize Tflite and Convert to C++

Use **tflm_model_transforms** tool from official tflite-micro repository can reduce .tflite size by running some TFLM specific transformations. It also re-align the tflite flatbuffer via the C++ flatbuffer api which can speed up inference on some Ameba platforms. This step is optional, but we strongly recommend running it:

```
git clone https://github.com/tensorflow/tflite-micro.git
cd tflite-micro

bazel build tensorflow/lite/micro/tools:tflm_model_transforms
bazel-bin/tensorflow/lite/micro/tools/tflm_model_transforms --input_model_path=</path/to/my_model.tflite>

# output will be located at: /path/to/my_model_tflm_optimized.tflite
```

Convert .tflite model and .npy test data to .cc and .h files for deployment:

```
python generate_cc_arrays.py models int8_tflm_optimized.tflite
python generate_cc_arrays.py testdata input_int8.npy input_int8.npy label_int8.npy label_int8.npy
```

### Step 4. Inference on SoC with Tflite-Micro

*example_tflm_mnist.cc* shows how to run inference with the trained model on test data, calculate accuracy, profile memory and latency. 

Use https://netron.app/ to visualize the .tflite file and check the operations used by the model. Instantiate operations resolver to register and access the operations. 

```
using MnistOpResolver = tflite::MicroMutableOpResolver<4>;

TfLiteStatus RegisterOps(MnistOpResolver& op_resolver) {
    TF_LITE_ENSURE_STATUS(op_resolver.AddFullyConnected());
    TF_LITE_ENSURE_STATUS(op_resolver.AddConv2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddMaxPool2D());
    TF_LITE_ENSURE_STATUS(op_resolver.AddReshape());
    return kTfLiteOk;
}
```

Refer to https://ai.google.dev/edge/litert/microcontrollers/get_started#run_inference for more details about running inference with tflite-micro.

### Step 5. Build Example
 
- On AmebaDplus

  - **Enable tflite_micro configurations**:

    cd amebadplus_gcc_project and run```./menuconfig.py```

    < MENUCONFIG FOR KM4 CONFIG  --->

    < AI Config  --->
     [*]  Enable TFLITE MICRO

  - **Build and Download**:
    * Refer to the SDK Examples section of the online documentation to generate images.
    * `Download` images to board by Ameba Image Tool.


- On AmebaLite KM4

  - **Enable tflite_micro configurations**:

    cd amebalite_gcc_project and run```./menuconfig.py```

    < MENUCONFIG FOR KM4 CONFIG  --->

    < AI Config  --->
     [*]  Enable TFLITE MICRO

  - **Build and Download**:
    * Refer to the SDK Examples section of the online documentation to generate images.
    * `Download` images to board by Ameba Image Tool.

- On AmebaSmart

  - **Enable tflite_micro configurations**:

    cd amebasmart_gcc_project and run```./menuconfig.py```

    < MENUCONFIG FOR CA32 CONFIG  --->

    < AI Config  --->
     [*]  Enable TFLITE MICRO

  - **Build and Download**:
    * Refer to the SDK Examples section of the online documentation to generate images.
    * `Download` images to board by Ameba Image Tool.

**Expected Results:**

```
[TFLITE-MICRO] ~~~TESTS START~~~
[TFLITE-MICRO] model arena size = 15732
[WLAN-A] IPS in
[TFLITE-MICRO] Accuracy: 100/100

"Unique Tag","Total ticks across all events with that tag."
CONV_2D, 1001689
MAX_POOL_2D, 52380
RESHAPE, 797
FULLY_CONNECTED, 218691
"total number of ticks", 1273557

[RecordingMicroAllocator] Arena allocation total 15732 bytes
[RecordingMicroAllocator] Arena allocation head 13520 bytes
[RecordingMicroAllocator] Arena allocation tail 2212 bytes
[RecordingMicroAllocator] 'TfLiteEvalTensor data' used 204 bytes with alignment overhead (requested 204 bytes for 17 allocations)
[RecordingMicroAllocator] 'Persistent TfLiteTensor data' used 64 bytes with alignment overhead (requested 64 bytes for 2 tensors)
[RecordingMicroAllocator] 'Persistent TfLiteTensor quantization data' used 40 bytes with alignment overhead (requested 40 bytes for 4 allocations)
[RecordingMicroAllocator] 'Persistent buffer data' used 1254 bytes with alignment overhead (requested 1204 bytes for 14 allocations)
[RecordingMicroAllocator] 'NodeAndRegistration struct' used 224 bytes with alignment overhead (requested 224 bytes for 7 NodeAndRegistration structs)

[TFLITE-MICRO] Total Time: 1291.682007 ms

[TFLITE-MICRO] ~~~ALL TESTS PASSED~~~
```

Note: 

1. results of arena size, ProfileMemoryAndLatency are platform dependent.
2. in ameba-aiot, ticks in profile result actually represent as microseconds
