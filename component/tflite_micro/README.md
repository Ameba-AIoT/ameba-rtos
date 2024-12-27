# TensorFlow Lite Micro for Ameba SoCs

## Overview

TensorFlow Lite for Microcontrollers is a port of TensorFlow Lite designed to run machine learning models on DSPs, microcontrollers and other devices with limited memory.

This repository is a version of the [TensorFlow Lite Micro library](https://www.tensorflow.org/lite/microcontrollers) for Realtek Ameba SoCs. 

### Supported SoCs

| Chip       | OS    | Processor |         master         |
| :--------- | ----- | --------- | :--------------------: |
| AmebaSmart | RTOS  | CA32      | ![alt text][supported] |
| AmebaLite  | RTOS  | HiFi5 DSP | ![alt text][supported] |
| AmebaLite  | RTOS  | KM4       | ![alt text][supported] |
| AmebaDplus | RTOS  | KM4       | ![alt text][supported] |

[supported]: https://img.shields.io/badge/-supported-green "supported"

## Getting Started

### Build Tensorflow Lite Micro Library

To build Tensorflow Lite Micro Library, enable tflite_micro configurations by menuconfig.py in gcc_project directory.

### Build Examples

Please refer to the examples in component/example/tflite_micro directory.

## Version Sync

This repository has been automatically generated from the master TensorFlow Lite for Microcontrollers repository at https://github.com/tensorflow/tflite-micro.

To sync to the latest tflite-micro and create a ameba SoCs compatible project, you can run the script:

```
sync/sync_from_tflite_micro.sh
```

## License

This repository is provided under Apache 2.0 license, see [LICENSE](LICENSE) file for details.

TensorFlow library source code and third_party code contain their own licenses specified under respective repos.
