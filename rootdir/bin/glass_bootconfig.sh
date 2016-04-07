#!/system/bin/sh

#
# Reads the serial number and region code from the boot config and stashes them
# in a system properties. Also reads IMU calibration data from the
# manufacturing floor.
#

DEV_PATH=/dev/block/platform/omap/omap_hsmmc.1/by-name/bootconfig
SERIAL_NUMBER_PROPERTY=ro.serialno.glass
REGION_CODE_PROPERTY=ro.region
TAG=GlassBootconfig
ADBKEYS=/data/misc/adb/adb_keys
FACTORYSUFFIX=factory@google.com
IMU_CALDATA_DEST=/data/inv_cal_data.bin
BUILDTYPE=`getprop ro.build.type`

#
# Writes a log message.
#
function message() {
    log -p i -t $TAG $1
}

#
# Displays an error message and exits.
#
function fail() {
    message "$1"
    exit 1
}

#
# Checks that the characters in a substring of the given substring are all
# within a given range.
#
function check_characters() {
    local string=$1
    local start=$2
    local end=$3
    local min=$4
    local max=$5

    local i=$start
    while [ $i -le $end ]
    do
        char=${string:$i:1}
        if [ "$char" \< "$min" -o "$char" \> "$max" ]
        then
            fail "Invalid character $char at index $i in $string"
        fi
        true $(( i++ ))
    done
}

function read_partition() {
    local partition=$1;
    # Ignore stderr since it complains about version mismatches there.
    echo `bootconfig $DEV_PATH read $partition 2>/dev/null`;
}

function clear_config() {
    local rev=`getprop persist.sys.clearautoboot`
    if [ "$BUILDTYPE" == "user" -o "$BUILDTYPE" == "userdebug" ] && [ "${rev}" != "1" ]
    then
        message "Clearing AUTOBOOT flag"
        clearautoboot $DEV_PATH
        setprop persist.sys.clearautoboot 1
    fi
}

function update_vendor_key() {
  local flags=`read_partition flags`
  local unlock_bit=$(($flags & 4))
  sed -i /\b$FACTORYSUFFIX$/d $ADBKEYS
  if [ $unlock_bit -eq 4 ]
  then
    message "Device unlocked appending vendor key"

    local VENDOR_KEY=`read_partition vendorkey`
    if [ ! -z "$VENDOR_KEY" ]
    then
      echo "$VENDOR_KEY $FACTORYSUFFIX" >> $ADBKEYS
      chown system $ADBKEYS
      chgrp shell $ADBKEYS
      chmod 640 $ADBKEYS
    else
      message "No vendorkey present"
    fi
  fi
}

function main() {
    message "Setting the serial number..."

    # Read the serial number from the bootconfig.
    local number=`read_partition device_serial`

    # Run some sanity checks on the serial number.
    # It is supposed to have 1 capital letter followed by 13 characters.
    if [ ${#number} -ne 14 ]
    then
        fail "Serial number has invalid length: $number"
    fi
    check_characters $number 0 0 "A" "Z"

    # Add the serial number system property.
    setprop $SERIAL_NUMBER_PROPERTY $number
    message "$SERIAL_NUMBER_PROPERTY is now $number."

    # Add the region code system property.
    message "Setting the region system property..."
    local region_code=`read_partition region_code`
    if [ -z "$region_code" ]
    then
        message "Region code not found in bootconfig."
    else
        setprop $REGION_CODE_PROPERTY $region_code
        message "$REGION_CODE_PROPERTY is now $region_code."
    fi

    # Load IMU calibration data into destination.
    read_partition imu_calibration | hex2bin > $IMU_CALDATA_DEST
    chown system:system $IMU_CALDATA_DEST
    message "IMU calibration data ($length bytes) written to $IMU_CALDATA_DEST."

    # Write out vendor keys if appropriate
    update_vendor_key

    # Check whether autoboot flag needs to be cleared
    clear_config
}

main
