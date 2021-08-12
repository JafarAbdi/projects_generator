#!/bin/bash

set -e

APPDIR="$(dirname "$(readlink -e "$0")")"

clone_repo()
{
  local template_path=$(mktemp -d -p "/tmp" template-XXXXXX)
  git clone --depth=1 $1 $template_path
  echo $template_path
}

export LD_LIBRARY_PATH="${APPDIR}/usr/lib/:${APPDIR}/usr/lib/x86_64-linux-gnu${LD_LIBRARY_PATH+:$LD_LIBRARY_PATH}"
if [ "$#" -eq "0" ];then
  exec "${APPDIR}/usr/bin/projects_generator" --templates_path=${APPDIR}/usr/share/templates
elif [ "$#" -eq "1" ];then
  case $1 in
      --templates_path*) template_path="$1";;
      --git_url*)        template_path="--templates_path=$(clone_repo "$(echo $1 | cut -d'=' -f2)")";;
      *)                 echo "Unknown argument '$1'" && exit;;
  esac
  exec "${APPDIR}/usr/bin/projects_generator" "$template_path"
else
  echo "Command only takes one argument --templates_path=PATH or --git_url=GIT_URL"
fi
