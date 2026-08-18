"""
Override: patch AmebaDut to skip shell-prompt wait.
Also override image_dir fixture to resolve relative paths against
the test script directory, matching --auto-build's build output.
Everything else comes from ameba_pytest.plugin.
"""
import functools
import glob
import subprocess
import os
import sys

SDK_ROOT = os.path.normpath(
    os.path.join(os.path.dirname(__file__), "..", "..", "..")
)
sys.path.insert(0, os.path.join(SDK_ROOT, "tools"))

pytest_plugins = ["ameba_pytest.plugin"]

import pytest  # noqa: E402


@pytest.fixture
def image_dir(request, target, count):
    """Override plugin's image_dir: resolve relative paths against
    the test script directory (not CWD).

    The parametrize values (e.g. ``./bin/raw_gpio_rw``) are designed
    to be relative to the test script, matching ``_auto_build_for_item``
    which uses ``item.fspath`` as the base.  The plugin's
    ``_resolve_image_dir`` resolves with ``os.path.abspath()`` against
    CWD, so when CWD != script dir (e.g. pytest launched from a parent
    directory) the flash step looks in the wrong place.
    """
    raw = (getattr(request, 'param', None)
           or request.config.getoption("--image-dir"))
    test_dir = os.path.dirname(str(request.node.fspath))

    def _resolve(r):
        if r is not None:
            r = os.path.expanduser(os.path.expandvars(r))
            if not os.path.isabs(r):
                r = os.path.join(test_dir, r)
        return r

    from ameba_pytest.plugin import _resolve_image_dir

    if count == 1:
        return _resolve_image_dir(_resolve(raw), target)
    raws = [r.strip() for r in raw.split("|")] if raw else [None] * count
    return tuple(
        _resolve_image_dir(_resolve(raws[i] if i < len(raws) else raws[-1]), target[i])
        for i in range(count)
    )


@pytest.fixture(autouse=True)
def _patch():
    """Patch AmebaDut._wait_for_prompt to a no-op.

    Example firmware has no shell, so the default 10-second wait for
    ``#`` would always time out and crash fixture setup.  We make
    it a no-op at the class level — every other bit of the plugin
    keeps working.
    """
    from ameba_pytest.dut import AmebaDut

    orig = AmebaDut._wait_for_prompt
    AmebaDut._wait_for_prompt = lambda self, timeout=10: None
    yield
    AmebaDut._wait_for_prompt = orig


# ---------------------------------------------------------------------------
# Auto-build support (--auto-build)
# ---------------------------------------------------------------------------

def pytest_addoption(parser):
    """Register --auto-build CLI option."""
    parser.addoption(
        "--auto-build", action="store_true", default=False,
        help="auto-compile example firmware via 'ameba.py build -a' before flashing",
    )


def pytest_configure(config):
    """Register app_name and app_defines markers."""
    config.addinivalue_line(
        "markers",
        "app_name(name): example app name to build, multi-DUT uses | separator. "
        "Required when --auto-build is enabled.",
    )
    config.addinivalue_line(
        "markers",
        "app_defines(defines): extra CMake -D definitions, multi-DUT uses | separator, "
        "per-DUT multiple values use , separator.",
    )
    config.addinivalue_line(
        "markers",
        "pristine_build: force pristine build (``-p``) even if the app has no ``prj.conf``. "
        "Use when an app needs a clean build for other reasons.",
    )


@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_setup(item):
    """If --auto-build, compile firmware before fixture setup."""
    if item.config.getoption("--auto-build"):
        if item.get_closest_marker("skip"):
            print("  (skipped, auto-build skipped)")
        else:
            _auto_build_for_item(item)
    yield


def _auto_build_for_item(item):
    """Parse markers and params, build firmware for each DUT."""
    if not item.get_closest_marker("app_name"):
        pytest.exit(
            "--auto-build requires @pytest.mark.app_name(...) on the test",
            returncode=1,
        )

    count = item.callspec.params.get("count", 1)
    targets = (item.config.getoption("--target") or "").split("|")
    app_names = _parse_marker_per_dut(item, "app_name", count)
    image_dirs = _parse_image_dirs(item, count)
    defines_list = _parse_marker_per_dut(item, "app_defines", count)
    test_dir = os.path.dirname(str(item.fspath))

    for i in range(count):
        soc = targets[i] if i < len(targets) else targets[-1]
        app = app_names[i]
        img_dir = os.path.abspath(os.path.join(test_dir, image_dirs[i]))
        extra = defines_list[i] if defines_list else None
        pristine = item.get_closest_marker("pristine_build") is not None

        try:
            print(f"Auto-build for dut[{i}]:")
            _run_build(soc, app, img_dir, extra, pristine)
        except (subprocess.CalledProcessError, FileNotFoundError, subprocess.TimeoutExpired) as e:
            pytest.exit(
                f"Auto-build failed for {app} on {soc}: {e}", returncode=1
            )


def _parse_marker_per_dut(item, marker_name, count):
    """Read marker, expand to per-DUT list.

    Single value + multi-DUT → broadcast to all DUTs.
    Multi values + short → pad with last value.
    No marker → return None.
    """
    marker = item.get_closest_marker(marker_name)
    if not marker:
        return None
    parts = [p.strip() for p in str(marker.args[0]).split("|")]
    if len(parts) == 1:
        return parts * count
    return [parts[i] if i < len(parts) else parts[-1] for i in range(count)]


def _parse_image_dirs(item, count):
    """get image_dir from parametrize, or exit pytest if not found."""
    callspec = getattr(item, "callspec", None)
    if callspec and "image_dir" in callspec.params:
        raw = callspec.params["image_dir"]
        parts = [p.strip() for p in str(raw).split("|")]
        return [parts[i] if i < len(parts) else parts[-1] for i in range(count)]
    pytest.exit(
        "--auto-build requires @pytest.mark.parametrize('image_dir', ..., indirect=True)",
        returncode=1,
    )


@functools.lru_cache(maxsize=None)
def _app_has_prj_conf(app_name):
    """Check if the example app directory contains a ``prj.conf`` file.

    Only apps that ship a ``prj.conf`` need a pristine (``-p``) build to
    pick up its Kconfig overrides.  The build system resolves ``app_name``
    by scanning under ``example/``, so we mirror that search.
    """
    pattern = os.path.join(SDK_ROOT, "example", "**", app_name, "prj.conf")
    return bool(glob.glob(pattern, recursive=True))


def _clean_build_state(soc):
    """Remove ``build/`` and ``menuconfig/`` after a ``-p`` build.

    ``prj.conf`` sets ``CONFIG_*`` variables that persist in the CMake
    cache and compiled ``.o`` files.  If a subsequent app (without
    ``prj.conf``) builds in the same SOC directory, it inherits those
    stale values.
    """
    import shutil

    soc_workdir = os.path.join(SDK_ROOT, f"build_{soc.upper()}")
    build_dir = os.path.join(soc_workdir, "build")
    menuconfig_dir = os.path.join(soc_workdir, "menuconfig")

    if os.path.isdir(build_dir):
        shutil.rmtree(build_dir)
    if os.path.isdir(menuconfig_dir):
        shutil.rmtree(menuconfig_dir)


def _run_build(soc, app_name, image_dir, extra_defines=None, pristine=False):
    """Run ameba.py build from SDK root.

    Parameters
    ----------
    soc : str
        Target SoC name (e.g. "RTL8720F").
    app_name : str
        Example app directory name.
    image_dir : str
        Absolute path to the output firmware directory.
    extra_defines : str, optional
        Comma-separated extra CMake ``-D`` definitions.
    pristine : bool
        Force pristine build (``-p``).  When False (default), pristine
        is still auto-enabled if ``prj.conf`` is found in the app dir.
    """
    has_prj = _app_has_prj_conf(app_name)
    pristine = pristine or has_prj

    defines = [f"FINAL_IMAGE_DIR={image_dir}"]
    if extra_defines:
        defines.extend(d.strip() for d in extra_defines.split(","))

    cmd = [
        "python3", "ameba.py", "build", soc.lower(),
        "-a", app_name,
        "-q",
        *(["-p"] if pristine else []),
        "-D",
        *defines,
    ]
    print(' '.join(cmd))
    subprocess.run(
        cmd,
        cwd=SDK_ROOT,
        check=True,
        capture_output=True,
        text=True,
        timeout=900,  # 15 minutes
    )

    # Prevent stale prj.conf configuration from leaking into the next build.
    if has_prj:
        _clean_build_state(soc)
