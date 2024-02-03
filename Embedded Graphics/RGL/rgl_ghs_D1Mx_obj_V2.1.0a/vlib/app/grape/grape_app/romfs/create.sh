#!/bin/bash
if [ $# != 2 ];then
echo "usage: $0 <start_address> <board>"
exit 1
fi
VLIB_ROOT=$(pwd | sed '{s/vlib/vlib /;}' | awk '{print $1};')
cd tmp
ls | $VLIB_ROOT/middleware/romfs/tool/generate_flash.sh $1
mkdir -p ../src/platform/$2
mkdir -p ../binary/platform/$2
mv *.c ../src/platform/$2
mv data_flash*.* ../binary/platform/$2
mv data_flash* ../binary/platform/$2
cd ..
rm -rf ./tmp
