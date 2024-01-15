#!/bin/bash

set -e

APT_INSTALL=()
ADD_INSTALL=()

while IFS= read -r LINE ; do
    IFS=$'\t' read -r TYPE ARCH DEPENDENCY INSTALL <<< "$LINE"
    if [ "$ARCH" == "all" ] || [ "$ARCH$CONFIG_YALDA_KERNEL_FROM_HOST" == "hosty" ] || [[ $YALDA_ARCH =~ ^($ARCH)$ ]]; then
        case "$TYPE" in
            app)
                which "$DEPENDENCY" >/dev/null || APT_INSTALL+=($INSTALL)
            ;;
            lib)
                pkg-config --exists >/dev/null  "$DEPENDENCY" || APT_INSTALL+=($INSTALL)
            ;;
            cmd)
                ( eval $DEPENDENCY >/dev/null ) || ADD_INSTALL+=$(exec echo $INSTALL)
            ;;
            *)
                continue
            ;;
        esac
    fi
done < <(cat $1)

[ -n "$APT_INSTALL" ] && APT_INSTALL="sudo apt install ${APT_INSTALL[@]}"
if [ -n "$APT_INSTALL" -o -n "$ADD_INSTALL" ]; then
    log_error "There are missed dependencies, recommended fix:\n$APT_INSTALL\n$ADD_INSTALL"
    exit 1
fi
