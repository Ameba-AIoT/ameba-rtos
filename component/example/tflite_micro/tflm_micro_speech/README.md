# Micro Speech Example

## Description:

This example shows how to run inference using TensorFlow Lite Micro (TFLM) on two models for wake-word recognition. The first model is an audio preprocessor that generates spectrogram data from raw audio samples. The second is the Micro Speech model, a less than 20 kB model that can recognize 2 keywords, "yes" and "no", from speech data. The Micro Speech model takes the spectrogram data as input and produces category probabilities.
For more details, please refer to [TensorFlow Lite for Microcontrollers](https://github.com/tensorflow/tflite-micro/blob/main/tensorflow/lite/micro/examples/micro_speech/README.md)

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

2. **GCC:** use CMD ```./build.py -a tflm_micro_speech``` to compile example


## Expected Result
**tflm_micro_speech**
```
Testing NoFeatureTest
AudioPreprocessor model arena size = 8440
Testing YesFeatureTest
AudioPreprocessor model arena size = 8440
Testing NoTest
AudioPreprocessor model arena size = 8440
MicroSpeech model arena size = 6812
MicroSpeech category predictions for <no>
  0.0000 silence
  0.0547 unknown
  0.0000 yes
  0.9453 no
Testing YesTest
AudioPreprocessor model arena size = 8440
MicroSpeech model arena size = 6812
MicroSpeech category predictions for <yes>
  0.0000 silence
  0.0000 unknown
  0.9961 yes
  0.0000 no
Testing SilenceTest
AudioPreprocessor model arena size = 8440
MicroSpeech model arena size = 6812
MicroSpeech category predictions for <silence>
  0.3359 silence
  0.2344 unknown
  0.2344 yes
  0.1953 no
Testing NoiseTest
AudioPreprocessor model arena size = 8440
MicroSpeech model arena size = 6812
MicroSpeech category predictions for <silence>
  0.9688 silence
  0.0117 unknown
  0.0078 yes
  0.0117 no
6/6 tests passed
~~~ALL TESTS PASSED~~~
```