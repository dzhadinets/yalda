#!/bin/bash
if [ -n $1 ];then
    echo $@  | xargs clang-format -i -style=file:$YALDA_KERNEL_SOURCE_DIR/.clang-format
else
    find . -iname *.h -o -iname *.c | grep -vF '.yalda' | xargs clang-format -i -style=file:$YALDA_KERNEL_SOURCE_DIR/.clang-format
fi
