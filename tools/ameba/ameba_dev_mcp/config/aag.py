"""
AAG (Aggregate log) protocol parser.

A self-contained, dependency-free stream decoder that auto-detects AAG
frames and tags multi-core output with [HP]/[LP]/[AP]. Kept in its own
leaf module (no internal imports) so it can be reused by BOTH:

  - the agent-facing serial read tools (tools/serial.py), where the
    per-alias parser is reset on drain/reset for clean line tagging, and
  - the background serial-log capture (config/serial_log.py), where a
    separate, never-reset instance decodes the full stream into the log.

Both use the SAME class so framing/tagging behaviour is identical.

De-interleaving
---------------
AGG is a LOGUART *hardware* feature: it chops each CPU's log into small
0xFF-framed packets (1-8 payload bytes, flushed on a threshold/timeout)
tagged with a source id. When two cores log at once their packets land
on the wire byte-interleaved. The host MUST reassemble per core or the
text comes out garbled (`[HP] [B[LP] [AOOPPT...`).

So framed payloads are accumulated in a PER-SOURCE line buffer and only
emitted once a line completes (newline); the tag is then placed before
the first visible char of each line. This mirrors the SDK's own Monitor
tool (`serial_handler.py`, `_np/_lp/_ap_last_line_part`).

Raw bytes (anything not inside a valid frame — e.g. pre-sync junk, a
checksum failure, or the ENTIRE stream when AGG is disabled) are passed
through verbatim and immediately, so AGG-off behaviour is unchanged.
"""

from __future__ import annotations

from typing import Dict


class AAGParser:
    """Auto-detecting AAG frame parser; tags + de-interleaves multi-core output."""

    _SRC_NAMES = {0x1: "HP", 0x2: "LP", 0x4: "AP"}
    _VALID_SRC = (0x1, 0x2, 0x4)

    def __init__(self):
        self._buf = bytearray()              # incomplete-frame byte buffer
        self._line: Dict[int, str] = {}      # per-source partial-line text

    def parse(self, data: bytes) -> str:
        self._buf.extend(data)
        events = self._extract_frames()
        parts = []
        for ev in events:
            if ev[0] == "raw":
                _, raw = ev
                if raw:
                    parts.append(self._decode(raw))      # passthrough, immediate
            else:
                _, src, payload = ev
                if payload:
                    parts.append(self._feed_line(src, self._decode(payload)))
        return "".join(parts)

    def peek_partial(self) -> str:
        """Held (incomplete) per-source lines, tagged, WITHOUT clearing.

        For transient use — e.g. pattern matching that must see a line
        before its terminating newline arrives. Do NOT accumulate the
        result across calls: the same bytes are re-emitted (once, tagged)
        by parse() when the line finally completes.
        """
        return self._render_partial(clear=False)

    def take_partial(self) -> str:
        """Held per-source lines, tagged, AND cleared.

        Use to finalize output at the end of a read or before a return so
        nothing is withheld and the next read starts clean.
        """
        return self._render_partial(clear=True)

    def flush(self) -> str:
        """Close-time drain: incomplete-frame residual (as raw) + all held
        partial lines."""
        residual = ""
        if self._buf:
            residual = self._decode(bytes(self._buf))
            self._buf.clear()
        return residual + self.take_partial()

    def reset(self) -> None:
        """Drop the partial-frame buffer AND all per-source partial lines.

        Use after a board reset / re-flash so the next decoded bytes are
        tagged from a clean line state — otherwise [HP]/[LP]/[AP] prefixes
        can land mid-line because the parser still thinks the previous
        boot was mid-line.
        """
        self._buf.clear()
        self._line.clear()

    # -- line reassembly ---------------------------------------------------

    def _feed_line(self, src: int, text: str) -> str:
        """Accumulate `text` for source `src`; return only the lines that are
        now COMPLETE (tagged). The trailing partial line is held until its
        newline arrives, so a concurrent core's packets cannot splice into
        the middle of this core's line."""
        if not text:
            return ""
        buf = self._line.get(src, "") + text
        nl = buf.rfind("\n")
        if nl < 0:
            self._line[src] = buf
            return ""
        end = nl + 1
        # Swallow CR(s) right after the final LF so the reversed "\n\r" line
        # ending stays attached to its line instead of leaking onto the next
        # held partial (which would otherwise read as a spurious blank line).
        while end < len(buf) and buf[end] == "\r":
            end += 1
        self._line[src] = buf[end:]
        return self._tag_lines(buf[:end], src)

    def _render_partial(self, *, clear: bool) -> str:
        if not self._line:
            return ""
        out = []
        for src in sorted(self._line):
            rem = self._line[src]
            if rem:
                out.append(self._tag_lines(rem, src))
        if clear:
            self._line.clear()
        return "".join(out)

    def _tag_lines(self, text: str, src: int) -> str:
        """Prefix the [HP]/[LP]/[AP] tag before the first VISIBLE char of each
        line. Both "\\n" and "\\r" reset to line-start and pass through
        untagged — AGG frames commonly begin with a "\\r", and tagging before
        it would strand a bare "[HP] " on its own line."""
        name = self._SRC_NAMES.get(src)
        if name is None:
            return text
        tag = f"[{name}] "
        out = []
        at_start = True
        for ch in text:
            if ch == "\n" or ch == "\r":
                out.append(ch)
                at_start = True
                continue
            if at_start:
                out.append(tag)
                at_start = False
            out.append(ch)
        return "".join(out)

    @staticmethod
    def _decode(raw: bytes) -> str:
        try:
            return raw.decode("utf-8")
        except UnicodeDecodeError:
            return raw.decode("utf-8", errors="replace")

    def _extract_frames(self):
        events = []
        while self._buf:
            try:
                idx = self._buf.index(0xFF)
            except ValueError:
                events.append(("raw", bytes(self._buf)))
                self._buf.clear()
                break
            if idx > 0:
                events.append(("raw", bytes(self._buf[:idx])))
                del self._buf[:idx]
            if len(self._buf) < 2:
                break
            hdr = self._buf[1]
            src = (hdr >> 5) & 0x07
            length = ((hdr >> 2) & 0x07) + 1
            csum = hdr & 0x03
            if len(self._buf) < 2 + length:
                break
            frame = bytes(self._buf[: 2 + length])
            del self._buf[: 2 + length]
            high6 = hdr >> 2
            parity = bin(high6).count("1") & 1
            if (csum & 0x2) != 0 or (csum & 0x1) != parity:
                events.append(("raw", frame))
                continue
            if src in self._VALID_SRC:
                events.append(("frame", src, frame[2:]))
            else:
                events.append(("raw", frame))
        return events
