#!/bin/bash

set -e

APPDIR="$(dirname "$(readlink -e "$0")")"

export LD_LIBRARY_PATH="${APPDIR}/usr/lib/:${APPDIR}/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH+:$LD_LIBRARY_PATH}"
if [ "$#" -eq "0" ];then
  exec "${APPDIR}/usr/bin/projects_generator" --templates_path=${APPDIR}/usr/share/templates
else
  exec "${APPDIR}/usr/bin/projects_generator" "$@"
fi
