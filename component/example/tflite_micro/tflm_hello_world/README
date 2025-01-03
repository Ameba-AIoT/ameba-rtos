# Hello World Example

## Description:

This example is designed to demonstrate the absolute basics of using [TensorFlow
Lite for Microcontrollers](https://www.tensorflow.org/lite/microcontrollers).
It includes a pre-trained model, and converting it for use with TensorFlow Lite
for Microcontrollers for running inference on a microcontroller.

## Supported IC <a name = "supported-ic"></a>
1. AmebaSmart
2. AmebaLite
3. AmebaDplus

## Sw Configuration:
* On AmebaDplus
1. **Enable tflite_micro configurations**:
```./menuconfig.py```
    < MENUCONFIG FOR KM4 CONFIG  --->
    < Audio Config  --->
    [*]  Enable TFLITE MICRO

2. **GCC:** use CMD ```./build.py -a tflm_hello_world``` to compile example


## Expected Result
**tflm_hello_world**
```
ProfileMemoryAndLatency
"Unique Tag","Total ticks across all events with that tag."
FULLY_CONNECTED, 103
"total number of ticks", 103

[RecordingMicroAllocator] Arena allocation total 1400 bytes
[RecordingMicroAllocator] Arena allocation head 136 bytes
[RecordingMicroAllocator] Arena allocation tail 1264 bytes
[RecordingMicroAllocator] 'TfLiteEvalTensor data' used 120 bytes with alignment overhead (requested 120 bytes for 10 allocations)
[RecordingMicroAllocator] 'Persistent TfLiteTensor data' used 64 bytes with alignment overhead (requested 64 bytes for 2 tensors)
[RecordingMicroAllocator] 'Persistent buffer data' used 733 bytes with alignment overhead (requested 692 bytes for 7 allocations)
[RecordingMicroAllocator] 'NodeAndRegistration struct' used 96 bytes with alignment overhead (requested 96 bytes for 3 NodeAndRegistration structs)

LoadFloatModelAndPerformInference
x: 0.000000, y: 0.026405
x: 1.000000, y: 0.863044
x: 3.000000, y: 0.127646
x: 5.000000, y: -0.956519

LoadQuantModelAndPerformInference
x: 0.770000, y: 0.696440
x: 1.570000, y: 0.994915
x: 2.300000, y: 0.729604
x: 3.140000, y: -0.008291

~~~ALL TESTS PASSED~~~
```
Note: 1. results of ProfileMemoryAndLatency are platform dependent.
      2. in ameba-aiot, ticks in profile result actually represent as microseconds
