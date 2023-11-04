#!/bin/bash
set -ue

SRC=$(dirname "${0}")
TYPE="${1:-debug}"
DST="${2:-build}"
[[ "$#" -ge 2 ]] && shift 2 || shift ${#}

echo "Building ${TYPE} in ${DST}"
mkdir -p "${DST}"
cmake -S "${SRC}" -B "${DST}" -D "CMAKE_BUILD_TYPE=${TYPE}"
cmake --build "${DST}" --config "${TYPE}" "${@}"