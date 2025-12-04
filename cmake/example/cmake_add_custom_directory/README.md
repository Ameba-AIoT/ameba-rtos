# Example Description

This example shows how to add a directory containing several source files to the build system. The code will be compiled and linked to current image.

# HW Configuration

None

# SW configuration

1. Try to replace the source files with your own code, and modify the build config in `CMakeLists.txt`.

2. Reference the symbols defined here in required source files. For example, in `app_example.c`, call `func_foo()` defined by `foo.c` and include `foo.h`.

3. Use `ameba_add_subdirectory*()` to add current directory in the `CMakeLists.txt` of required image or directory.

4. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

The function you defined will be executed after booting.
For example, the following string will be printed as `func_foo()` defined:

```
This is an example of adding a custom directory to Ameba SDK build system!
```

# Note

None

# Supported IC

Common