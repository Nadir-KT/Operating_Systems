#!/bin/bash
VLIB_ROOT=$(pwd | sed '{s/vlib/vlib /;}' | awk '{print $1};')

$VLIB_ROOT/utils/tgadataconvert/target/x86_gnu/tgadataconvert.out -i icon_test.tga -s IconDemotest -o ./src/icon_demotest.c -b ./data/icon_demotest.bin

$VLIB_ROOT/utils/tgadataconvert/target/x86_gnu/tgadataconvert.out -i img_nvaders_alien.tga -s ImgTestAlien -o ./src/img_alien.c -b ./data/img_alien.bin
