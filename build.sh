APP=rtos_demo
APP_DIR=m0sense_apps
if [ "$1" != "" ]; then

    if [ ! -d "$1" -o ! -f "$1/CMakeLists.txt" ]; then
        echo no this app project \"$1\"!
        exit
    fi

    APP=${1##*/}
    APP_DIR=${1%%/*}

fi

cd bl_mcu_sdk
make APP=$APP APP_DIR=../$APP_DIR BOARD=bl702_iot  SUPPORT_FLOAT=y
cd ..

TARGET=bl_mcu_sdk/out/$APP_DIR/$APP/${APP}_bl702.bin
ls -alh $TARGET

# extra
if [ ! "$APP" == "m0sense_boot" ]; then
    UF2_CVT="misc/utils/uf2_convert"
    $UF2_CVT $TARGET uf2_demos/${APP}.uf2
fi