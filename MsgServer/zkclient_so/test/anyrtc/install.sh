#!/bin/sh
CLEAN_FLAG=0
if [ "$1" = "-u" ]; then
CLEAN_FLAG=1
fi

TARGET_PREFIX=/usr/local/dync/msgserver/anyrtc
TARGET_LOG_PREFIX=/opt/dync/msgserver/anyrtc
TARGET_CONF_PATH=$TARGET_PREFIX/conf
TARGET_BIN_PATH=$TARGET_PREFIX/bin
TARGET_LOG_PATH=$TARGET_LOG_PREFIX/log
TARGET_LIB_PATH=$TARGET_PREFIX/lib
SYS_BIN_PATH=/usr/bin
SYS_INIT_PATH=/etc/init.d

if [ "$CLEAN_FLAG" = "0" ]; then
printf "Install program....\r\n"
mkdir -p $TARGET_CONF_PATH
mkdir -p $TARGET_BIN_PATH
mkdir -p $TARGET_LOG_PATH
mkdir -p $TARGET_LIB_PATH
mkdir -p $SYS_BIN_PATH
# exe sh
cp -f anyrtc.sh $SYS_BIN_PATH/anyrtc.sh
chmod 755 $SYS_BIN_PATH/anyrtc.sh

cp -f anyrtc.conf $TARGET_CONF_PATH/
cp -f bin/test_zksdk $TARGET_BIN_PATH/
cp -rf lib/* $TARGET_LIB_PATH/
printf "Install program ok!\r\n"
else
printf "UnInstall program....\r\n"
anyrtc.sh stop
rm -f $SYS_BIN_PATH/anyrtc.sh
rm -f $TARGET_CONF_PATH/anyrtc.conf
rm -f $TARGET_BIN_PATH/test_zksdk
rm -rf $TARGET_PREFIX
rm -rf $TARGET_LOG_PREFIX
rm -rf $TARGET_LIB_PREFIX
printf "UnInstall program ok!\r\n"
fi
