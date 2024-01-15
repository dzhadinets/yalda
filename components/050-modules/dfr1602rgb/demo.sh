#!/bin/sh

echo -e "Welcome \n    Linux !!!" > /dev/dfr1602rgb

oldR=0
oldG=0
oldB=0

set_color()
{
    local R=$1
    local G=$2
    local B=$3
    echo -e "$R \t $G \t $B"
    printf "#%2hhx%2hhx%2hhx\n" $R $G $B > /sys/kernel/dfr1602rgb/color
    usleep 200
}

fade_to() {
    local steps=20
    local R=$1
    local G=$2
    local B=$3
    local incrR=$(( ( $R - $oldR ) / $steps ))
    local incrG=$(( ( $G - $oldG ) / $steps ))
    local incrB=$(( ( $B - $oldB ) / $steps ))
    local step=0
    while [ $step -lt "$steps" ]; do
        oldR=$(( $oldR + $incrR ))
        oldG=$(( $oldG + $incrG ))
        oldB=$(( $oldB + $incrB ))
        step=$(( $step + 1 ))
        set_color $oldR $oldG  $oldB
    done
}

while true
do
    R=$(($RANDOM % 255))
    G=$(($RANDOM % 255))
    B=$(($RANDOM % 255))

    fade_to $R $G $B
    oldR=$R
    oldG=$G
    oldB=$B
    set_color $oldR $oldG  $oldB
done
