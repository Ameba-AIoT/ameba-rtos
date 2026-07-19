# RTL8720F km4ns prebuilt-library hooks.
#
# The common ameba_add_internal_library() (cmake/utility.cmake) stays exactly as the
# base SDK: it merely calls ameba_soc_internal_library_hook() when a SoC defines it,
# before its default build-from-source path, and returns early if the hook handled the
# library. All the km4ns-specific knowledge (p_NO_SEAL semantics, MP suffix,
# seal-to-lib/internal, link-prebuilt) lives here, so the common function stays generic.
#
# Scope safety: this file is included only by the km4ns project (see
# project_km4ns/CMakeLists.txt, after km4tz is already configured), so it never affects
# km4tz or other SoCs. And both branches below are gated by config that is only ever set
# in the km4ns seal/prebuilt context, so the hook is inert (declines, out_handled=FALSE)
# everywhere else regardless.
#
# Contract:
#   ameba_soc_internal_library_hook(name original_args out_handled)
#     - parses its own flags (p_NO_SEAL / p_NO_WHOLE_ARCHIVE) from original_args
#     - reads ambient project vars (c_CURRENT_IMAGE, c_MCU_PROJ_CONFIG, c_MCU_SDK_DIR)
#       just like the common function does
#     - if it fully handled the library it registers the lib into the current image's
#       link lists, sets c_CURRENT_TARGET_NAME/FILE (PARENT_SCOPE) and out_handled=TRUE
#     - otherwise it leaves out_handled=FALSE and the common default path runs

function(ameba_soc_internal_library_hook name original_args out_handled)
    set(${out_handled} FALSE PARENT_SCOPE)

    set(options p_NO_WHOLE_ARCHIVE p_NO_SEAL)
    cmake_parse_arguments(HOOK "${options}" "" "" ${original_args})

    # User-configurable components opt out of sealing: decline so the common function
    # compiles them from source (their source ships with the SDK).
    if(HOOK_p_NO_SEAL)
        return()
    endif()

    set(_internal_dir "${c_MCU_SDK_DIR}/lib/internal")

    # MP suffix derived from menuconfig, mirroring the external-lib convention
    # (e.g. lib_wifi_whc_np -> lib_wifi_whc_np_mp / lib_wifi_whc_np_mp_shrink),
    # so each conf variant is stored/selected separately.
    set(_psc_mp_suffix "")
    if(CONFIG_MP_INCLUDED)
        if(CONFIG_MP_SHRINK)
            set(_psc_mp_suffix "_mp_shrink")
        else()
            set(_psc_mp_suffix "_mp")
        endif()
    endif()

    # Forward args to the module builder without the standalone flag it doesn't take.
    set(_fwd_args ${original_args})
    if(HOOK_p_NO_WHOLE_ARCHIVE)
        list(REMOVE_ITEM _fwd_args p_NO_WHOLE_ARCHIVE)
    endif()

    # Path 1: seal build - compile and copy .a to lib/internal/.
    # Single-condition if() so the release strip step removes this whole block
    # cleanly from the released SDK.

    # Path 2: released prebuilt SDK - register the exact prebuilt lib from lib/internal/.
    # MP suffix selects the correct variant strictly from menuconfig, no fallback.
    # EXISTS guard: some libs have p_DROP_IF_NO_SOURCES and produce no .a for this
    # config (e.g. lib_misc has no km4ns sources); skip those silently.
    if(CONFIG_KM4NS_IMG2_PREBUILT)
        set(_psc_lib "${_internal_dir}/lib_${name}${_psc_mp_suffix}.a")
        if(EXISTS "${_psc_lib}" AND c_CURRENT_IMAGE)
            if(HOOK_p_NO_WHOLE_ARCHIVE)
                set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY
                    ${c_CURRENT_IMAGE}_no_whole_archive_libs "${_psc_lib}")
            else()
                set_property(TARGET ${c_MCU_PROJ_CONFIG} APPEND PROPERTY
                    ${c_CURRENT_IMAGE}_whole_archive_libs "${_psc_lib}")
            endif()
        endif()
        set(c_CURRENT_TARGET_NAME "" PARENT_SCOPE)
        set(c_CURRENT_TARGET_FILE "" PARENT_SCOPE)
        set(${out_handled} TRUE PARENT_SCOPE)
        return()
    endif()

    # Neither seal nor prebuilt applies -> decline; the common default path builds from source.
endfunction()
