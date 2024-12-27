/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

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

// Reference implementation of timer functions.  Platforms are not required to
// implement these timer methods, but they are required to enable profiling.

// On platforms that have a POSIX stack or C library, it can be written using
// methods from <sys/time.h> or clock() from <time.h>.

// To add an equivalent function for your own platform, create your own
// implementation file, and place it in a subfolder with named after the OS
// you're targeting. For example, see the Cortex M bare metal version in
// tensorflow/lite/micro/bluepill/micro_time.cc

#include "tensorflow/lite/micro/micro_time.h"

#if defined(TF_LITE_USE_CTIME)
#include <ctime>
#elif defined(TF_LITE_USE_AIOT_TIME)
extern "C" uint64_t rtos_time_get_current_system_time_us(void);
#endif

namespace tflite {

#if defined(HIFI5)
uint32_t ticks_per_second() {
  // In ameba-aiot, ticks here actually represent microseconds
  return 1000000;
}

uint32_t GetCurrentTimeTicks() {
  return (*((volatile uint32_t*)(0x4011E000 + 0x004)));
}

#elif defined(TF_LITE_USE_AIOT_TIME)
uint32_t ticks_per_second() {
  // In ameba-aiot, ticks here actually represent microseconds
  return 1000000;
}

uint32_t GetCurrentTimeTicks() {
  return (uint32_t)rtos_time_get_current_system_time_us();
}

#elif defined(TF_LITE_USE_CTIME)

// For platforms that support ctime, we implment the micro_time interface in
// this central location.
uint32_t ticks_per_second() { return CLOCKS_PER_SEC; }

uint32_t GetCurrentTimeTicks() { return clock(); }

#else
// This returns 0 by default because timing is an optional feature that builds
// without errors on platforms that do not need it.
uint32_t ticks_per_second() { return 0; }

uint32_t GetCurrentTimeTicks() { return 0; }
#endif

}  // namespace tflite
