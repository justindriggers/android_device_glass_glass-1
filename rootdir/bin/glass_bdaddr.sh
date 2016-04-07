#!/system/bin/sh

BDFILE=/data/misc/bluedroid/bdaddr
getprop ro.boot.bdaddr > ${BDFILE}
chmod 660 ${BDFILE}
