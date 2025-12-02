# Example Description

This example shows how to link prebuilt static libraries. The existing library file `lib_foo.a` will be linked to current image.

# HW Configuration

None

# SW configuration

1. Try to replace the static library files in `lib/` with your own. Make sure the libraries are built using the same toolchain and platform as current MCU project. An example of generating a static library `lib_foo_prebuilt.a` is provided in `foo_build/`.

2. In `CMakeLists.txt`, modify the build config public part, especially setting `public_libraries` to the path of the prebuilt library file and setting `public_includes` to the include directory. In private part, an example of building `lib_foo_prebuilt.a` from `foo_build/` is provided, you can delete the part if not needed.

3. Use `ameba_add_subdirectory*()` to add current directory in the `CMakeLists.txt` of required image or directory.

4. Reference the symbols defined by the library in required source files. For example, in `app_example.c`, call `func_foo_prebuilt()` defined by `foo_build/foo_prebuilt.c` and include `foo_prebuilt.h`.

5. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

The function you defined will be executed after booting.
For example, the following string will be printed as `func_foo_prebuilt()` defined:

```
This is an example of linking prebuilt library files to Ameba SDK build system!
```

# Note

The toolchain and platform for building the library files must be consistent with the current project. Otherwise, the linking may fail or unpredictable errors may occur during execution.

# Supported IC

Common