ameba_global_include(${CMAKE_CURRENT_SOURCE_DIR}/kv)

if(CONFIG_VFS_ENABLED)
    ameba_add_subdirectory_if(CONFIG_VFS_LITTLEFS_INCLUDED littlefs)
    ameba_add_subdirectory_if(CONFIG_VFS_FATFS_INCLUDED fatfs)
    ameba_add_subdirectory_if(CONFIG_VFS_REALFS_INCLUDED realfs)
endif()

# VFS_SUPPORTED but not VFS_ENABLED: VFS disabled in menuconfig, vfs/kv/ftl only build failing stubs
if(CONFIG_VFS_SUPPORTED)
    ameba_add_subdirectory(kv)
    ameba_add_subdirectory(vfs)
    ameba_add_subdirectory_if(CONFIG_BT ftl)
endif()
