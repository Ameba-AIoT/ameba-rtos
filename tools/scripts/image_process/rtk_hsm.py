"""
Realtek HSM (devops.realtek.com) signing backend for compile-time sboot signing.

The manifest's ``sboot_private_key`` may be a key spec ``rtk_hsm:<name>`` (e.g.
``rtk_hsm:sboot``) instead of a raw private key.  When the build sees this
prefix it delegates ECDSA signing to the Realtek HSM via the ``hsm-cli`` tool
(async ``pure-sign`` create/download).  The private key never leaves the HSM.

Algorithm: ECDSA SECP256R1 + SHA-256 (the only mode the Realtek HSM offers).

Config resolution --- environment first so nothing secret / environment-specific
lands in the tracked manifest:

    RTK_HSM_SERVER        default https://devops.realtek.com
    RTK_HSM_USER          HSM user name       (per-user, applied for)
    RTK_HSM_TOKEN         HSM user token      (per-user, secret)
    RTK_HSM_<NAME>_UUID   key UUID for logical <name> (e.g. RTK_HSM_SBOOT_UUID)
    RTK_HSM_CLI           path to hsm-cli     (default: <repo>/hsm_service/hsm-cli)
    RTK_HSM_CONFIG        optional hsm.config.yaml (only if env vars absent)

The logical name (``sboot``) keeps the tracked manifest generic; the actual UUID
+ credentials come from the environment / an untracked config, never the repo.
"""

import base64
import json
import os
import subprocess
import sys
import time

_DEFAULT_SERVER = "https://devops.realtek.com"
# Known sboot key UUID; overridable via RTK_HSM_SBOOT_UUID env / config.
_DEFAULT_UUIDS = {
    "sboot": "daec0289-2484-47d6-b735-e09ace644715",
}


def _repo_root():
    # tools/scripts/image_process/rtk_hsm.py -> repo root is three levels up
    return os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                          "..", "..", ".."))


def _load_config_file():
    """Read an optional hsm.config.yaml (env RTK_HSM_CONFIG) as a flat dict.

    Only used as fallback when env vars are absent.  Parsed with a tiny
    line-based reader so no PyYAML dependency is required.
    """
    path = os.environ.get("RTK_HSM_CONFIG")
    if not path or not os.path.isfile(path):
        return {}
    cfg = {}
    section = None
    with open(path) as fh:
        for raw in fh:
            line = raw.split("#", 1)[0].rstrip()
            if not line.strip():
                continue
            if not line.startswith(" ") and line.rstrip().endswith(":"):
                section = line.strip()[:-1]
                continue
            if ":" in line:
                k, v = line.split(":", 1)
                k, v = k.strip(), v.strip().strip('"').strip("'")
                if line.startswith(" ") and section:
                    cfg[f"{section}.{k}"] = v
                else:
                    cfg[k] = v
                    section = None
    return cfg


def _parse_spec(spec):
    """`rtk_hsm:<name>` -> logical name (default 'sboot')."""
    name = spec[len("rtk_hsm:"):].strip()
    return name or "sboot"


def _resolve(spec):
    """Resolve (uuid, server, user, token, cli) for an rtk_hsm spec."""
    name = _parse_spec(spec)
    cfg = _load_config_file()

    uuid = (os.environ.get(f"RTK_HSM_{name.upper()}_UUID")
            or cfg.get(f"keys.{name}")
            or _DEFAULT_UUIDS.get(name))
    if not uuid:
        raise ValueError(
            f"rtk_hsm: no UUID for key '{name}'. "
            f"Set RTK_HSM_{name.upper()}_UUID or keys.{name} in RTK_HSM_CONFIG.")

    server = os.environ.get("RTK_HSM_SERVER") or cfg.get("SERVER") or _DEFAULT_SERVER
    user = os.environ.get("RTK_HSM_USER") or cfg.get("USER_NAME")
    token = os.environ.get("RTK_HSM_TOKEN") or cfg.get("USER_TOKEN")
    if not user or not token:
        raise ValueError(
            "rtk_hsm: missing credentials. Set RTK_HSM_USER + RTK_HSM_TOKEN "
            "(or USER_NAME/USER_TOKEN in RTK_HSM_CONFIG).")

    cli = os.environ.get("RTK_HSM_CLI") or os.path.join(_repo_root(), "hsm_service", "hsm-cli")
    if not os.path.isfile(cli):
        raise FileNotFoundError(f"rtk_hsm: hsm-cli not found at '{cli}' (set RTK_HSM_CLI).")

    return uuid, server, user, token, cli


def _cli_base(server, user, token, cli):
    return [cli, "-server", server, "-user_name", user, "-user_token", token]


def rtk_hsm_get_pubkey(spec):
    """Return the raw EC public key X||Y (64 bytes) for the spec's HSM key."""
    uuid, server, user, token, cli = _resolve(spec)
    cmd = _cli_base(server, user, token, cli) + ["get-public-key", "--uuid", uuid]
    try:
        out = subprocess.run(cmd, capture_output=True, text=True, check=True).stdout
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"rtk_hsm get-public-key failed: {e.stderr.strip()}")
    resp = json.loads(out)
    # {"data": {"pubKey": "04<X><Y>", ...}}
    pub_hex = resp.get("data", {}).get("pubKey", "")
    pub = bytes.fromhex(pub_hex)
    if len(pub) == 65 and pub[0] == 0x04:
        return pub[1:]          # strip uncompressed-point 0x04 -> X||Y (64B)
    if len(pub) == 64:
        return pub
    raise RuntimeError(f"rtk_hsm: unexpected pubKey length {len(pub)} ({pub_hex[:16]}...)")


def rtk_hsm_sign(spec, msg_bytes):
    """Sign msg_bytes with the HSM key; return DER-encoded ECDSA signature.

    The HSM performs SHA-256 internally (pure-sign), matching ECDSA(SHA256, msg).
    """
    uuid, server, user, token, cli = _resolve(spec)
    base = _cli_base(server, user, token, cli)

    # 1. create signing task
    create = base + ["pure-sign", "create", "--uuid", uuid, "--data", msg_bytes.hex()]
    try:
        out = subprocess.run(create, capture_output=True, text=True, check=True).stdout
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"rtk_hsm pure-sign create failed: {e.stderr.strip()}")
    resp = json.loads(out)
    task_id, status = resp.get("id", ""), resp.get("status", "")
    if status != "SUCCESS":
        raise RuntimeError(f"rtk_hsm pure-sign create status={status}: {resp}")

    # 2. download the signature (base64 DER)
    time.sleep(1)  # let the HSM finish
    out_file = os.path.join("/tmp", f"rtk_hsm_sig_{task_id}.txt")
    download = base + ["pure-sign", "download", "--id", task_id, "--output", out_file]
    try:
        subprocess.run(download, capture_output=True, text=True, check=True)
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"rtk_hsm pure-sign download failed: {e.stderr.strip()}")
    with open(out_file) as fh:
        b64 = fh.read().strip()
    os.remove(out_file)
    return base64.b64decode(b64)


def is_rtk_hsm_spec(spec):
    return isinstance(spec, str) and spec.startswith("rtk_hsm:")
