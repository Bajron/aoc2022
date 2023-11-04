#!/bin/bash
HERE=$(dirname $(readlink -f "${BASH_SOURCE[0]}"))
docker build -t gcc "${HERE}/docker"
exec docker run -it --rm -u $(id -u):$(id -g) -v "${HERE}:${HERE}" -w "${HERE}" gcc