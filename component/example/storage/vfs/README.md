# Example Description

This example is for virtual file system module which wraps littlefs/fatfs.

# HW Configuration

None

# SW configuration

1. Modify the interface function `app_filesystem_init()` located in `amebaXXX_gcc_project/project_<core>/src/main.c` to config your expected test mode.
Test Mode: (`VFS_LITTLEFS`/`VFS_FATFS`, `VFS_INF_FLASH`/`VFS_INF_SD`)
   - For `VFS_LITTLEFS` with `VFS_INF_FLASH`:
		```C
		ret = vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
		```

   - For `VFS_FATFS` with `VFS_INF_FLASH`:
		```C
		ret = vfs_user_register(VFS_PREFIX, VFS_FATFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
		```

2. Type command `./menuconfig.py` and choose `CONFIG VFS`
   - For `VFS_LITTLEFS`:
		```
		[*] Enable VFS LITTLEFS
		[ ] Enable VFS FATFS
		```

   - For `VFS_FATFS` with `VFS_INF_FLASH`:
		```
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		FATFS Memory Type (FlASH)
		```

3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = "vfs_file_1", content = "VFS module works normally !!!") succeeded

2. Read vfs file succeeded

3. Delete vfs file succeeded

# Note

None

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
