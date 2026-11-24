#!/bin/bash
# SPDX-License-Identifier: GPL-2.0-only
#
# scan-cve.sh — Periodic CVE scan for rtwpriv SBOM
#
# Scans the CycloneDX SBOM (bom.json) against known vulnerability databases
# using osv-scanner.  Designed to be run from cron.
#
# Usage:
#   ./scan-cve.sh                    # normal run, print results
#   ./scan-cve.sh --email            # send results by email (requires mailx)
#   ./scan-cve.sh --junit            # output JUnit XML for CI integration
#   ./scan-cve.sh --html             # output HTML report
#
# Cron example (every Monday at 08:00):
#   0 8 * * 1 /path/to/rtwpriv/scan-cve.sh --email
#
# Dependencies:
#   - osv-scanner (https://github.com/google/osv-scanner)
#     Install: go install github.com/google/osv-scanner/cmd/osv-scanner@latest
#     Or:      npm install -g @google/osv-scanner
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BOM="$SCRIPT_DIR/bom.json"
SCAN_DIR="$SCRIPT_DIR/.cve-scans"
REPORT="$SCAN_DIR/report.txt"
EMAIL_TO="willistang@realtek.com"
OSV_BIN=""

# ── Find osv-scanner ─────────────────────────────────────────────────────
find_osv() {
    if command -v osv-scanner &>/dev/null; then
        OSV_BIN="osv-scanner"
        return 0
    fi
    if [ -x "$HOME/go/bin/osv-scanner" ]; then
        OSV_BIN="$HOME/go/bin/osv-scanner"
        return 0
    fi
    if [ -x /usr/local/bin/osv-scanner ]; then
        OSV_BIN="/usr/local/bin/osv-scanner"
        return 0
    fi
    return 1
}

# ── Load SBOM version ────────────────────────────────────────────────────
get_version() {
    python3 -c "
import json
with open('$BOM') as f:
    bom = json.load(f)
print(bom['metadata']['component']['version'])
" 2>/dev/null || echo "unknown"
}

# ── Main scan ────────────────────────────────────────────────────────────
do_scan() {
    mkdir -p "$SCAN_DIR"

    local version
    version=$(get_version)

    echo "==========================================="
    echo " rtwpriv CVE Scan Report"
    echo " Version : $version"
    echo " SBOM    : $BOM"
    echo " Date    : $(date '+%Y-%m-%d %H:%M:%S %Z')"
    echo "==========================================="
    echo ""

    if ! find_osv; then
        echo "ERROR: osv-scanner not found."
        echo "Install: go install github.com/google/osv-scanner/cmd/osv-scanner@latest"
        exit 1
    fi

    "$OSV_BIN" -L "$BOM" --format=table 2>&1 | tee "$REPORT"
    local rc=${PIPESTATUS[0]}

    if [ "$rc" -eq 0 ]; then
        echo ""
        echo "✅ No known vulnerabilities found."
    else
        echo ""
        echo "⚠️  One or more vulnerabilities detected (see above)."
    fi

    return "$rc"
}

# ── HTML report ──────────────────────────────────────────────────────────
html_report() {
    local version
    version=$(get_version)

    mkdir -p "$SCAN_DIR"
    do_scan > /dev/null 2>&1 || true

    local html="$SCAN_DIR/report.html"
    local count
    count=$(grep -c 'CVE-' "$REPORT" 2>/dev/null || echo 0)

    cat > "$html" <<EOF
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>rtwpriv CVE Scan Report</title>
<style>
  body { font-family: sans-serif; margin: 2em; }
  h1 { color: #333; }
  .pass { color: green; font-weight: bold; }
  .fail { color: red; font-weight: bold; }
  pre { background: #f5f5f5; padding: 1em; border-radius: 4px; overflow-x: auto; }
</style>
</head>
<body>
<h1>rtwpriv CVE Scan Report</h1>
<table>
  <tr><td><strong>Version</strong></td><td>$version</td></tr>
  <tr><td><strong>Date</strong></td><td>$(date '+%Y-%m-%d %H:%M:%S')</td></tr>
  <tr><td><strong>Vulnerabilities</strong></td><td>
    $( [ "$count" -gt 0 ] && echo '<span class="fail">'$count' found</span>' || echo '<span class="pass">None</span>' )
  </td></tr>
</table>
<hr/>
<h2>Scan Results</h2>
<pre>$(cat "$REPORT")</pre>
</body>
</html>
EOF
    echo "HTML report: $html"
}

# ── CLI handling ─────────────────────────────────────────────────────────
case "${1:-}" in
    --email)
        do_scan > /dev/null 2>&1 || true
        if [ -s "$REPORT" ]; then
            mailx -s "[rtwpriv] CVE Scan Report $(date '+%Y-%m-%d')" "$EMAIL_TO" < "$REPORT"
            echo "Report emailed to $EMAIL_TO"
        fi
        ;;
    --junit)
        do_scan > /dev/null 2>&1 || true
        local version
        version=$(get_version)
        local xml="$SCAN_DIR/report.xml"
        local failures
        failures=$(grep -c 'CVE-' "$REPORT" 2>/dev/null || echo 0)
        cat > "$xml" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<testsuite name="rtwpriv-cve-scan" tests="1" failures="${failures}" time="$(date +%s)">
  <testcase name="osv-scanner SBOM scan" classname="CVE" time="0">
    $( [ "$failures" -gt 0 ] && echo "<failure message=\"$failures vulnerability(ies) found\"><![CDATA[$(cat "$REPORT")]]></failure>" )
  </testcase>
</testsuite>
EOF
        echo "JUnit report: $xml"
        ;;
    --html)
        html_report
        ;;
    *)
        do_scan
        ;;
esac
