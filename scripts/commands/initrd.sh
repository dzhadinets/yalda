#!/bin/bash

log_info "Building InitRamDisk at $YALDA_INITRD"
pushd "$YALDA_OUTPUT_DIR" >/dev/null
find . -print0 | cpio --null -ov --format=newc 2>/dev/null | gzip > "$YALDA_INITRD"
popd >/dev/null
