#!/system/bin/sh

# Get the build type
buildtype=`getprop ro.build.type`

# Based on the build type decide whether to run a script or not
case "$buildtype" in
    "eng")
        echo "Is eng build, running /data/local/bin/glass.sh"
        /data/local/bin/glass.sh
        ;;
    "userdebug")
        echo "Is userdebug build not running any script"
        ;;
    "user")
        echo "Is user build not running any script"
        ;;
esac

