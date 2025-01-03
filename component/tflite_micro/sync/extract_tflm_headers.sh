#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
HEADER_DIR="${SCRIPT_DIR}/include"

[[ -d "${HEADER_DIR}" ]] && rm -rf "${HEADER_DIR}"
mkdir ${HEADER_DIR} && \
    find  tensorflow signal third_party \( -name *.h -or -name *.hpp \) | \
    xargs -i  cp --parent {} ${HEADER_DIR}
