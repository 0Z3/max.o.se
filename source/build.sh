#!/bin/bash

modules="o.se.oscbn"

module_install_dir="../misc"

usage() {
    echo "Usage: build.sh -dh" 1>&2
    echo
    echo "-d	: Debug (default: Release)" 1>&2
    echo "-h	: Help" 1>&2
    echo
}

config="Release"

while getopts "d" options; do
    case "${options}" in
        d)
            config="Debug"
            ;;
        h)
            usage
            exit 0
            ;;
    esac
done
shift $((OPTIND - 1))

xcodebuild -project o.se.xcodeproj -alltargets -configuration "$config"

if [ ! -d ../misc ]; then
    mkdir "$module_install_dir"
fi

for m in "$modules"; do
    cp "$m/$m.so" "$module_install_dir"
done
