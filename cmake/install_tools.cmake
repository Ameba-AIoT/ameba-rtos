include_guard(GLOBAL)

# Install rules for release.
#
# This file is included unconditionally by cmake/ameba.cmake so all rules are
# registered once, regardless of which IC is selected.
#
# Excludes .git metadata, __pycache__, build output trees, and internal-only
# files that are not part of the public release.

# ---------------------------------------------------------------------------
# cmake/: entire build-system directory (all file types).
# The full-tree sweep below only matches specific extensions (*.cmake, Kconfig*
# etc.), so non-cmake auxiliary files that cmake scripts depend on at build time
# — build_info.h.in, git_version.c.in, empty_file.c, README-zh.html, etc. —
# would be missed without this explicit rule.
# Excluded: utility_internal.cmake and CMakeLists-template-internal.cmake
# (FATAL_ERROR guards), __pycache__, and .git metadata.
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/cmake"
    p_EXCLUDE_REGEX "__pycache__"
)

# ---------------------------------------------------------------------------
# SDK root files
# ---------------------------------------------------------------------------
ameba_install_files(
    "${c_BASEDIR}/ameba.py"
    "${c_BASEDIR}/env.bat"
    "${c_BASEDIR}/env.sh"
    "${c_BASEDIR}/LICENSE"
    "${c_BASEDIR}/README.md"
    "${c_BASEDIR}/Realtek_Disclaimer-2019.pdf"
    "${c_BASEDIR}/.gitignore"
)

# ---------------------------------------------------------------------------
# tools/DownloadServer(HTTP): all files
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/DownloadServer(HTTP)")

# ---------------------------------------------------------------------------
# tools/DownloadServer(UART): all files
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/DownloadServer(UART)")

# ---------------------------------------------------------------------------
# tools/R-Mesh_Demo_Tool: all files
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/R-Mesh_Demo_Tool")

# ---------------------------------------------------------------------------
# tools/ameba/DeviceProfileEditor: exclude (Internal) build variant
# ---------------------------------------------------------------------------
ameba_install_files(
    "${c_BASEDIR}/tools/ameba/DeviceProfileEditor/AmebaDeviceProfileEditor.exe"
    "${c_BASEDIR}/tools/ameba/DeviceProfileEditor/ChangeLog.txt"
    "${c_BASEDIR}/tools/ameba/DeviceProfileEditor/Newtonsoft.Json.dll"
    "${c_BASEDIR}/tools/ameba/DeviceProfileEditor/RtkUtils.dll"
)

# ---------------------------------------------------------------------------
# tools/ameba/Flash: exclude __pycache__ and .git metadata
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/ameba/Flash"
    p_EXCLUDE_REGEX "__pycache__"
)

# ---------------------------------------------------------------------------
# tools/ameba/ImageTool: exclude .git metadata
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/ameba/ImageTool")

# ---------------------------------------------------------------------------
# tools/ameba/Monitor: exclude __pycache__ and .git metadata
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/ameba/Monitor"
    p_EXCLUDE_REGEX "__pycache__"
)

# ---------------------------------------------------------------------------
# tools/ameba/R-DIAG_Demo_Tool: all files
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/ameba/R-DIAG_Demo_Tool")

# ---------------------------------------------------------------------------
# tools/ameba/RemoteService: only public library modules
# ---------------------------------------------------------------------------
ameba_install_files(
    "${c_BASEDIR}/tools/ameba/RemoteService/remote_serial.py"
    "${c_BASEDIR}/tools/ameba/RemoteService/version_manager.py"
)

# ---------------------------------------------------------------------------
# tools/ameba/TraceTool: exclude (Internal) build variant
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/ameba/TraceTool"
    p_EXCLUDE_REGEX "Internal"
)

# ---------------------------------------------------------------------------
# tools/ameba/ameba_dev_mcp: exclude tests/ and internal dev notes
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/ameba/ameba_dev_mcp"
    p_EXCLUDE_REGEX "/tests"
    p_EXCLUDE_PATTERN "mcp_improvement.md"
)

# ---------------------------------------------------------------------------
# tools/ameba/pyproject.toml
# ---------------------------------------------------------------------------
ameba_install_files("${c_BASEDIR}/tools/ameba/pyproject.toml")

# ---------------------------------------------------------------------------
# tools/image_scripts: top-level public files only; exclude internal/ subdir
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/image_scripts"
    p_EXCLUDE_REGEX "/internal"
)

# ---------------------------------------------------------------------------
# tools/iperf: all files
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/iperf")

# ---------------------------------------------------------------------------
# tools/littlefs: all files
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/littlefs")

# ---------------------------------------------------------------------------
# tools/requirements.txt
# ---------------------------------------------------------------------------
ameba_install_files("${c_BASEDIR}/tools/requirements.txt")

# ---------------------------------------------------------------------------
# tools/scripts: exclude generated artefacts and internal-only content
#   Excluded: __pycache__, .git, clangd_conf/, img_utility/, release_tool/,
#             git_hooks/, rtk_utils.py, setup.py, socmap_generate.py,
#             update.py, SWDPWD_Verify.JLinkScript, .gitignore
# ---------------------------------------------------------------------------
ameba_install_directory("${c_BASEDIR}/tools/scripts"
    p_EXCLUDE_REGEX "__pycache__" "/clangd_conf" "/img_utility" "/release_tool" "/git_hooks"
    p_EXCLUDE_PATTERN "rtk_utils.py" "setup.py" "socmap_generate.py" "update.py"
                      "SWDPWD_Verify.JLinkScript" ".gitignore"
)
