#!/bin/bash

mapfile -t shared_libraries < <(ldd $1 | grep vcpkg | awk '{ print $3 }')
mapfile -t -O "${#shared_libraries[@]}" shared_libraries < <(ldd $1 | grep appimage | awk '{ print $3 }')
mapfile -t shared_libraries_path < <(printf -- '%s\n' "${shared_libraries[@]}" | xargs dirname | uniq)
mapfile -t shared_libraries_path < <(printf -- '%s:' "${shared_libraries_path[@]}")
echo "${shared_libraries_path[*]}"
