#!/bin/bash
VLIB_ROOT=$(pwd | sed '{s/vlib/vlib /;}' | awk '{print $1};')

$VLIB_ROOT/utils/tgadataconvert/target/x86_gnu/tgadataconvert.out -i icon_clock.tga -s IconAppClock -o ./src/icon_clock.c -b ./data/icon_clock.bin

$VLIB_ROOT/utils/tgadataconvert/target/x86_gnu/tgadataconvert.out -i img_clocktrain.tga -s ImgClockTrain -o ./src/img_clocktrain.c -b ./data/img_clocktrain.bin

