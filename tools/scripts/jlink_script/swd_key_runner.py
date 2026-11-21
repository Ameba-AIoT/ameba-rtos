#!/usr/bin/env python3
"""One J-Link SWD key comparison (modes: see jlinkscript_generate.gen_swd_key_script), run as a single JLinkGDBServer connect.

The server uses spare ports next to the two of ameba.py -gdb, and its log is parsed. J-Link retries a failed connect once and every
failed comparison counts for the 4 tries of the hardware, only the first one is reported.
WSL runs the Windows JLinkGDBServerCL.exe (the J-Link USB is on the Windows side). Override with SWD_JLINK_EXE, SWD_JLINK_WORKDIR.
"""
import os
import platform
import re
import shutil
import subprocess
import sys
import tempfile
import time
from dataclasses import dataclass, field

HERE = os.path.dirname(os.path.abspath(__file__))
WSL_JLINK = "/mnt/c/Program Files (x86)/SEGGER/JLink/JLinkGDBServerCL.exe"
WSL_WORKDIR = "/mnt/c/temp"
CPUID_BLOCKED = 0xFFFFFFFF
RETRY_GRACE = 4.0                # seconds to keep reading after the first result
PORTS = (2340, 2341, 2342)      # gdb, swo, telnet


@dataclass
class KeyResult:
    mode: int
    ctrl: list = field(default_factory=list)       # SWD_PRTET_CTRL of every comparison in the log
    cpuid: int = None
    as_expected: bool = False
    raw: str = ""

    @property
    def passed(self):
        return bool(self.ctrl) and bool(self.ctrl[0] & (1 << 3))

    @property
    def locked(self):
        return any(c & (1 << 4) for c in self.ctrl)

    @property
    def cpuid_open(self):
        return self.cpuid is not None and self.cpuid != CPUID_BLOCKED


def is_wsl():
    return "microsoft" in platform.release().lower()


def _jlink_exe():
    exe = os.environ.get("SWD_JLINK_EXE")
    if exe:
        return exe
    if is_wsl():
        return WSL_JLINK
    return shutil.which("JLinkGDBServerCL") or shutil.which("JLinkGDBServerCLExe") or "JLinkGDBServerCL"


def _to_win(path):
    return subprocess.check_output(["wslpath", "-w", path], text=True).strip()


def _gen_script(chip, words, mode, out_path):
    sys.path.insert(0, HERE)
    import json5
    import jlinkscript_generate as g
    cfg = json5.load(open(os.path.join(HERE, "ameba_jlink_config.json5")))[chip]
    g.gen_swd_key_script(cfg, out_path, words, mode)


def run(words, mode=0, chip="RLE1509", timeout=90):
    words = [int(w, 16) if isinstance(w, str) else int(w) for w in words]
    if len(words) != 4:
        raise ValueError("a key is 4 words")
    exe = _jlink_exe()
    wsl = is_wsl()
    workdir = os.environ.get("SWD_JLINK_WORKDIR") or (WSL_WORKDIR if wsl else tempfile.gettempdir())
    os.makedirs(workdir, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="swdn_", dir=workdir) as tmp:
        script = os.path.join(tmp, "key.JLinkScript")
        log = os.path.join(tmp, "key.log")
        _gen_script(chip, words, mode, script)
        arg_script, arg_log = (_to_win(script), _to_win(log)) if wsl else (script, log)
        cmd = [exe, "-select", "USB", "-device", "Cortex-M33", "-if", "SWD", "-speed", "4000", "-scriptfile", arg_script,
               "-port", str(PORTS[0]), "-swoport", str(PORTS[1]), "-telnetport", str(PORTS[2]), "-singlerun", "-nogui", "-log", arg_log]
        text = _run_until_result(cmd, log, timeout)
    return parse(text, mode)


def _run_until_result(cmd, log, timeout):
    """JLinkGDBServer keeps waiting for a gdb client after the comparison, so stop it once the script reported its result. A failed
    connect is retried by the server a moment later: wait a little after the first result so those attempts end up in the log."""
    proc = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    deadline = time.time() + timeout
    text = ""
    seen_at = None
    try:
        while time.time() < deadline:
            if os.path.exists(log):
                text = open(log, errors="replace").read()
            if "[KEY] RESULT:" in text and seen_at is None:
                seen_at = time.time()
            if seen_at is not None and time.time() - seen_at > RETRY_GRACE:
                break
            if proc.poll() is not None:
                break
            time.sleep(0.3)
    finally:
        if proc.poll() is None:
            proc.terminate()
            try:
                proc.wait(5)
            except subprocess.TimeoutExpired:
                proc.kill()
        if os.path.exists(log):
            text = open(log, errors="replace").read()
    return text


def parse(text, mode):
    res = KeyResult(mode=mode, raw=text)
    res.ctrl = [int(m, 16) for m in re.findall(r"SWD_PRTET_CTRL:\s*0x([0-9a-fA-F]+)", text)]
    m = re.search(r"CPUID is:\s*0x([0-9a-fA-F]+)", text)
    res.cpuid = int(m.group(1), 16) if m else None
    res.as_expected = "[KEY] RESULT: AS EXPECTED" in text
    return res


if __name__ == "__main__":
    import argparse
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("words", nargs=4, help="REF0..REF3, 8 hex digits each")
    ap.add_argument("--mode", type=int, choices=(0, 1, 2), default=0)
    a = ap.parse_args()
    r = run(a.words, a.mode)
    print("mode %d: ctrl=%s cpuid=%s passed=%s locked=%s as_expected=%s" % (
        r.mode, ["0x%x" % c for c in r.ctrl], None if r.cpuid is None else "0x%08x" % r.cpuid, r.passed, r.locked, r.as_expected))
    sys.exit(0 if r.as_expected else 1)
