#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig

ameba_set_if(CONFIG_MP_INCLUDED c_SDK_IMAGE_FOLDER_NAME image_mp p_ELSE image)
ameba_set_if(CONFIG_WHC_HOST_LOC_KM4_MENU AP_IMAGEDIR ${CMAKE_BINARY_DIR}/project_km4/${c_SDK_IMAGE_FOLDER_NAME} p_ELSE ${CMAKE_BINARY_DIR}/project_kr4/${c_SDK_IMAGE_FOLDER_NAME})

message( "========== Image app generate start ==========")

set(app_full_path ${c_IMAGE_OUTPUT_DIR}/${c_APP_BINARY_NAME})
ameba_modify_file_path(${app_full_path} app_ns_full_path p_SUFFIX _ns)
ameba_modify_file_path(${app_full_path} app_tmp_full_path p_SUFFIX _tmp)
ameba_modify_file_path(${app_ns_full_path} app_ns_tmp_full_path p_SUFFIX _tmp)
ameba_modify_file_path(${app_full_path} app_compress p_SUFFIX _compress)
ameba_modify_file_path(${app_ns_full_path} app_compress_ns p_SUFFIX _compress)

# Reject a DSP image that does not match the current MCU layout (e.g. a stale bin
# built for a different DSP base). A DSP image is a chain of sub-images, each a 32B
# header (8B ASCII magic "81958711" + 4B size LE + 4B load LE + 16B rsvd) plus
# <size> payload bytes; dsp_all.bin also has a leading 4K boot manifest (size field
# == 0xffffffff), skipped here. At least one non-zero sub-image must load at
# expect_hex (the DSP entry-table address __dsp_img_load_addr__, no -0x20).
#
# sram_lo_hex/sram_hi_hex additionally bound any SRAM-targeting sub-image to the DSP's
# own SRAM slice (__dsp_sram_start__ + IMAGE_HEADER_LEN .. __dsp_sram_end__). The PSRAM
# base above is TZ- and IMG1-invariant, so it alone cannot catch a bin whose SRAM window
# went stale -- and that direction is the dangerous one: move the window up (enable
# CONFIG_IMG3_SRAM, switch IMG1_FLASH -> IMG1_SRAM) without regenerating the DSP LSP and
# the DSP writes below the mpc1/mpc2 non-secure base, into KM4 IMG1/IMG3 secure memory.
# Pass empty strings to skip this part.
#
# Caveat: the DSP packer (dsp_combine.py) derives the SRAM sub-image load address from
# the lowest PT_LOAD vaddr inside SRAM, and falls back to SRAM_BASE when the DSP places
# nothing there. So an empty sram_dsp yields load == SRAM_BASE, size 0 -- benign, and the
# bin then carries no window information to check at all. The check bites exactly when it
# matters: as soon as something really is placed in SRAM.
function(ameba_check_dsp_image bin_file expect_hex sram_lo_hex sram_hi_hex)
    math(EXPR c_want "0x${expect_hex}")
    set(c_sram_base 0x20000000)         # SRAM_BASE, the packer's "no SRAM payload" fallback
    set(c_sram_top  0x20100000)         # end of the SRAM alias window; above it is not SRAM
    set(c_sram_lo 0)
    set(c_sram_hi 0)
    if(sram_lo_hex AND sram_hi_hex)
        # +0x20: the loader copies the payload after the sub-image header, so a segment
        # placed in the DSP SRAM slice starts at DSP_BD_SRAM_ORIGIN, not _START.
        math(EXPR c_sram_lo "0x${sram_lo_hex} + 32")
        math(EXPR c_sram_hi "0x${sram_hi_hex}")
    endif()
    file(READ ${bin_file} c_hex HEX)
    string(LENGTH "${c_hex}" c_len)
    set(c_off 0)
    if(c_len GREATER_EQUAL 24)
        string(SUBSTRING "${c_hex}" 16 8 c_first_sz)
        if(c_first_sz STREQUAL "ffffffff")
            set(c_off 8192)     # skip 4K boot manifest (0x1000 bytes = 8192 hex chars)
        endif()
    endif()
    set(c_match FALSE)
    while(c_off LESS c_len)
        math(EXPR c_remain "${c_len} - ${c_off}")
        if(c_remain LESS 64)
            break()
        endif()
        string(SUBSTRING "${c_hex}" ${c_off} 16 c_magic)
        if(NOT c_magic STREQUAL "3831393538373131")
            message(FATAL_ERROR "${bin_file}: bad DSP sub-image magic at hex ${c_off} (${c_magic})")
        endif()
        math(EXPR c_so "${c_off} + 16")
        string(SUBSTRING "${c_hex}" ${c_so} 8 c_sz)
        math(EXPR c_lo "${c_off} + 24")
        string(SUBSTRING "${c_hex}" ${c_lo} 8 c_ld)
        # decode little-endian u32 (hex bytes b0 b1 b2 b3 -> value 0xb3b2b1b0)
        string(SUBSTRING "${c_sz}" 0 2 s0)
        string(SUBSTRING "${c_sz}" 2 2 s1)
        string(SUBSTRING "${c_sz}" 4 2 s2)
        string(SUBSTRING "${c_sz}" 6 2 s3)
        math(EXPR c_size "0x${s3}${s2}${s1}${s0}")
        string(SUBSTRING "${c_ld}" 0 2 d0)
        string(SUBSTRING "${c_ld}" 2 2 d1)
        string(SUBSTRING "${c_ld}" 4 2 d2)
        string(SUBSTRING "${c_ld}" 6 2 d3)
        math(EXPR c_load "0x${d3}${d2}${d1}${d0}")
        if(c_size GREATER 0 AND c_load EQUAL c_want)
            set(c_match TRUE)
        endif()
        # Bound any sub-image that targets SRAM to the DSP's own slice.
        if(c_sram_lo GREATER 0 AND c_load GREATER_EQUAL ${c_sram_base} AND c_load LESS ${c_sram_top})
            math(EXPR c_end "${c_load} + ${c_size}")
            math(EXPR c_load_hx "${c_load}" OUTPUT_FORMAT HEXADECIMAL)
            if(c_load EQUAL ${c_sram_base})
                # Packer fallback: the DSP placed nothing in SRAM. Only a zero-size
                # sub-image is benign here -- a payload would land on the RTK fixed
                # 40K region (ROM BSS / MSP / IPC / PMC).
                if(c_size GREATER 0)
                    message(FATAL_ERROR "${bin_file}: SRAM sub-image carries ${c_size} bytes at the "
                                        "SRAM_BASE fallback ${c_load_hx}, which is the RTK fixed region. "
                                        "The DSP packer could not resolve an SRAM segment; check the DSP LSP.")
                endif()
                set(c_sram_note " (no SRAM payload)")
            elseif(c_load LESS ${c_sram_lo} OR c_end GREATER ${c_sram_hi})
                math(EXPR c_lo_hx "${c_sram_lo}" OUTPUT_FORMAT HEXADECIMAL)
                math(EXPR c_hi_hx "${c_sram_hi}" OUTPUT_FORMAT HEXADECIMAL)
                message(FATAL_ERROR "${bin_file}: SRAM sub-image [${c_load_hx}, +${c_size}) is outside the "
                                    "DSP SRAM slice [${c_lo_hx}, ${c_hi_hx}) of this MCU layout -- that memory "
                                    "belongs to KM4/KR4 (and below the mpc1/mpc2 non-secure base it is secure, "
                                    "so the DSP would take a bus error). Regenerate the DSP LSP for this layout: "
                                    "tools/scripts/dsp_layout_sync.py, then rebuild the DSP image.")
            else()
                set(c_sram_note " (SRAM payload at ${c_load_hx}, ${c_size} bytes)")
            endif()
        endif()
        math(EXPR c_off "${c_off} + (32 + ${c_size}) * 2")
    endwhile()
    if(NOT c_match)
        message(FATAL_ERROR "${bin_file} does not match this MCU layout: no non-zero sub-image loads at 0x${expect_hex}. Rebuild the DSP image for this layout (likely stale).")
    endif()
    message(STATUS "DSP image OK: ${bin_file} loads at 0x${expect_hex}${c_sram_note}")
endfunction()

# The DSP image check only applies when DSP is enabled. Without CONFIG_DSP_EN there
# is no DSP bin to validate (and no DSP-related Kconfig, e.g. CONFIG_DSP_SDK_IMAGE_DIR,
# is set), so skip the whole block. Note __dsp_img_load_addr__ is defined in the img1
# loader ld regardless of DSP_EN, so gating on it is not enough -- gate on DSP_EN here.
if(CONFIG_DSP_EN)
    # Resolve the expected DSP load address from the km4 loader map once.
    set(c_DSP_LOADER_MAP ${CMAKE_BINARY_DIR}/project_km4/${c_SDK_IMAGE_FOLDER_NAME}/target_loader.map)
    set(c_DSP_ADDR_HEX "")
    # DSP SRAM slice bounds, for the SRAM part of the check. Both stay empty on a layout
    # where the DSP owns no SRAM (DSP_BD_SRAM length 0 -> _start == _end == SRAM_END),
    # which skips that part instead of rejecting every bin.
    set(c_DSP_SRAM_LO_HEX "")
    set(c_DSP_SRAM_HI_HEX "")
    if(EXISTS ${c_DSP_LOADER_MAP})
        file(STRINGS ${c_DSP_LOADER_MAP} c_DSP_ADDR_LINE REGEX "[ ]__dsp_img_load_addr__$")
        string(REGEX MATCH "^([0-9a-fA-F]+)" c_DSP_ADDR_HEX "${c_DSP_ADDR_LINE}")
        file(STRINGS ${c_DSP_LOADER_MAP} c_DSP_SRAM_LO_LINE REGEX "[ ]__dsp_sram_start__$")
        string(REGEX MATCH "^([0-9a-fA-F]+)" c_DSP_SRAM_LO_HEX "${c_DSP_SRAM_LO_LINE}")
        file(STRINGS ${c_DSP_LOADER_MAP} c_DSP_SRAM_HI_LINE REGEX "[ ]__dsp_sram_end__$")
        string(REGEX MATCH "^([0-9a-fA-F]+)" c_DSP_SRAM_HI_HEX "${c_DSP_SRAM_HI_LINE}")
        if(c_DSP_SRAM_LO_HEX AND c_DSP_SRAM_HI_HEX)
            math(EXPR c_DSP_SRAM_LEN "0x${c_DSP_SRAM_HI_HEX} - 0x${c_DSP_SRAM_LO_HEX}")
            if(c_DSP_SRAM_LEN LESS_EQUAL 0)
                set(c_DSP_SRAM_LO_HEX "")
                set(c_DSP_SRAM_HI_HEX "")
            endif()
        endif()
    endif()

    # CONFIG_DSP_SDK_IMAGE_DIR is the DSP SDK image output dir (holds dsp.bin /
    # dsp_all.bin). Absolute -> used as-is; relative -> resolved against the SoC
    # project dir (same rule as build.py / dsp_layout_sync.py).
    set(c_DSP_IMG_DIR "${CONFIG_DSP_SDK_IMAGE_DIR}")
    # Expand a leading '~' to $HOME (CMake does no tilde expansion, unlike the
    # Python os.path.expanduser used by build.py / dsp_layout_sync.py).
    if(c_DSP_IMG_DIR MATCHES "^~")
        string(REGEX REPLACE "^~" "$ENV{HOME}" c_DSP_IMG_DIR "${c_DSP_IMG_DIR}")
    endif()
    if(c_DSP_IMG_DIR AND NOT IS_ABSOLUTE "${c_DSP_IMG_DIR}")
        set(c_DSP_IMG_DIR "${c_SOC_PROJECT_DIR}/${c_DSP_IMG_DIR}")
    endif()

    if (CONFIG_DSP_WITHIN_APP_IMG)
        set(c_DSP_FILE ${c_DSP_IMG_DIR}/dsp.bin)
        if(NOT EXISTS ${c_DSP_FILE})
            message(FATAL_ERROR "dsp file not exist: ${c_DSP_FILE} (check CONFIG_DSP_SDK_IMAGE_DIR)")
        endif()
        if(c_DSP_ADDR_HEX)
            ameba_check_dsp_image(${c_DSP_FILE} ${c_DSP_ADDR_HEX} "${c_DSP_SRAM_LO_HEX}" "${c_DSP_SRAM_HI_HEX}")
        endif()
    else()
        # Standalone DSP image (flashed straight from the SDK image dir, see
        # project_info.json5): validate it against the layout when present; if absent
        # only warn -- packaging must not fail just because the DSP bin isn't built yet.
        set(c_DSP_CHECKED FALSE)
        if(c_DSP_IMG_DIR)
            foreach(c_name dsp_all.bin dsp.bin)
                set(c_DSP_STANDALONE ${c_DSP_IMG_DIR}/${c_name})
                if(c_DSP_ADDR_HEX AND EXISTS ${c_DSP_STANDALONE})
                    ameba_check_dsp_image(${c_DSP_STANDALONE} ${c_DSP_ADDR_HEX} "${c_DSP_SRAM_LO_HEX}" "${c_DSP_SRAM_HI_HEX}")
                    set(c_DSP_CHECKED TRUE)
                    break()
                endif()
            endforeach()
        endif()
        if(NOT c_DSP_CHECKED)
            message(WARNING "DSP enabled (standalone) but no dsp_all.bin/dsp.bin found to validate under "
                            "'${c_DSP_IMG_DIR}'. Set CONFIG_DSP_SDK_IMAGE_DIR and build the DSP image; "
                            "the flashed DSP bin will NOT be layout-checked.")
        endif()
    endif()
endif()

if(CONFIG_TRUSTZONE)
    if(CONFIG_DSP_WITHIN_APP_IMG)
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
            p_IMAGE3 ${c_IMAGE3_ALL_FILES}
            p_DSP ${c_DSP_FILE}
        )
    else()
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
            p_IMAGE3 ${c_IMAGE3_ALL_FILES}
        )
    endif()
else()
    if(CONFIG_DSP_WITHIN_APP_IMG)
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
            p_DSP ${c_DSP_FILE}
        )
    else()
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
        )
    endif()
endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${c_SOC_PROJECT_DIR}/fatfs.bin)
        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${app_full_path} ${app_tmp_full_path}
            COMMAND ${CMAKE_COMMAND} -E rename ${app_ns_full_path} ${app_ns_tmp_full_path}
        )
        ameba_axf2bin_pad(${app_tmp_full_path} 4096)
        ameba_axf2bin_pad(${app_ns_tmp_full_path} 4096)
        ameba_axf2bin_prepend_head(
            ${c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            ${c_SOC_PROJECT_DIR}/fatfs.bin
            VFS1_FLASH_BASE_ADDR
            ${AP_IMAGEDIR}/target_img2.map
        )
        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${app_tmp_full_path} ${c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${app_full_path}
        )
        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${app_ns_tmp_full_path} ${c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${app_ns_full_path}
        )
    else()
        message(FATAL_ERROR "Lack of fatfs.bin when enabling CONFIG_FATFS_WITHIN_APP_IMG")
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    ameba_axf2bin_compress(${app_compress} ${app_full_path})
    ameba_axf2bin_compress(${app_compress_ns} ${app_ns_full_path})
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_compress} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_compress_ns} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
else()
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_full_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_ns_full_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    ameba_modify_file_path(${c_IMAGE1_ALL_FILES} boot_ns_full_path p_SUFFIX _ns)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin ${c_IMAGE1_ALL_FILES})
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin ${boot_ns_full_path})
else()
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
    COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
)
message("========== Image manipulating end ==========")

ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${FINAL_IMAGE_DIR})
ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_full_path} ${FINAL_IMAGE_DIR})
