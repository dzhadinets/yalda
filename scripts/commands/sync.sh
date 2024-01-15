#!/bin/bash

function component_git_sync
{
    local component=$1
    local path=$(realpath $2)
    local giturl=$3
    local refspec=$4
    log_info "Fetch component $component to the dir $path from $giturl for refspec $refspec"
    pushd $path >/dev/null
    if [ -d "$path/.git" ]; then
        log_info "Already fetched. Sync skipped"
    else
        #git clone alternative. Because the dir can exist
        git init
        git remote add origin "$giturl"
        git pull origin -j$YALDA_NPROC --depth 1 "$refspec"
    fi
    popd >/dev/null
}

do_log mkdir -p "$YALDA_ROOT_DIR/.yalda/cache"

function component_fetch_unpack
{
    local component=$1
    local path="$(realpath $2)"
    local url="$3"
    local filename="$(basename $url)"
    local compressor

    log_info "Fetch component $component from $url"
    do_log mkdir -p "$path"
    wget -nv --show-progress -c "$url" -O "$YALDA_ROOT_DIR/.yalda/cache/$filename"

    local mime="$(file -b --mime-type $YALDA_ROOT_DIR/.yalda/$filename)"
    if [ "$mime" == "application/x-gzip" ]; then
        compressor="z"
    elif [ "$mime" == "application/x-bzip2" ]; then
        compressor="j"
    elif [ "$mime" == "application/x-xz" ]; then
        compressor="J"
    fi

    log_info "Unpack to $path"
    tar --strip-components=1 -x$compressor -f "$YALDA_ROOT_DIR/.yalda/cache/$filename" -C "$path"
}

function sync_component
{
    local component=$1
    if [ ! -x "${YALDA_COMPONENTS[$component]}" ]; then
        log_error "Unknown component $component"
    fi
    unset -f do_sync
    source ${YALDA_COMPONENTS[$component]}
    do_sync
}

function sync_submodule
{
    local component=$1

    log_info "Syncing submodules ..."
    pushd $component >/dev/null
    git submodule init
    git submodule update --recursive
    popd >/dev/null
}


if [ -n "$1" ]; then
    sync_component $1
else
    for component in ${YALDA_COMPONENTS_ORDER[@]}; do
        log_info "Syncing component $component"
        sync_component $component
    done
fi
