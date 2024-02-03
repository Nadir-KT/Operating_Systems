#!/bin/bash
if [ $# != 3 ];then
echo "usage: $0 <start_address_cpu> <start_address_serialflash> <board> "
exit 1
fi
VLIB_ROOT=$(pwd | sed '{s/vlib/vlib /;}' | awk '{print $1};')
cd tmp
ls | $VLIB_ROOT/middleware/romfs/tool/generate_header.sh
cd cpu
ls | $VLIB_ROOT/middleware/romfs/tool/generate_cpu_flash.sh $1
cd ..
cd serial
ls | $VLIB_ROOT/middleware/romfs/tool/generate_serial_flash.sh $2
cd ..
ls | $VLIB_ROOT/middleware/romfs/tool/generate_footer.sh $1
mkdir -p ../src/platform/$3
mkdir -p ../binary/platform/$3
mv *.c ../src/platform/$3
mv data_flash*.* ../binary/platform/$3
mv data_flash* ../binary/platform/$3
cd ..
rm -rf ./tmp
