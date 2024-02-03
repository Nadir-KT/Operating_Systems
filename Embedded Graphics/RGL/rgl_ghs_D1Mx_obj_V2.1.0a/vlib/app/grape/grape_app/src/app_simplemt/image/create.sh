#!/bin/bash
VLIB_ROOT=$(pwd | sed '{s/vlib/vlib /;}' | awk '{print $1};')

$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i icon_demosimplemt.tga -s IconDemoSimpleMt -o ./src/icon_demosimplemt.c

