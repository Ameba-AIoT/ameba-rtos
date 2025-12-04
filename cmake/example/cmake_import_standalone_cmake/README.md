# Example Description

This example shows how to add a third-party project with a standalone `CMakeLists.txt` to the build system. The code will be compiled and linked to current image.

# HW Configuration

None

# SW configuration

1. Try to replace the files in `third_party_project` with your own.

2. In `cmake_wrap/CMakeLists.txt`, modify the build config public part. In private part, replace the directory and target name with your own.

3. Use `ameba_add_subdirectory*()` to add the directory `cmake_wrap` in the `CMakeLists.txt` of required image or directory.

4. Reference the symbols defined here in required source files. For example, in `app_example.c`, call `func_foo_cmake()` defined by `third_party_project/foo_cmake.c` and include `foo_cmake.h`.

5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

The function you defined will be executed after booting.
For example, the following string will be printed as `func_foo_cmake()` defined:

```
This is an example of adding a third-party project with standalone CMakeLists.txt to Ameba SDK build system!
```

# Note

The toolchain and platform used in the `third_party_project/CMakeLists.txt` must be consistent with the current project. Otherwise, the building may fail or unpredictable errors may occur during execution. In this example, `third_party_project/CMakeLists.txt` uses the flags that automatically inherited from current MCU project.

# Supported IC

Common