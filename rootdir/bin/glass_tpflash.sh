#!/system/bin/sh
#
# Script to flash the Synaptics touchpad firmware.
# The script does a check to see if the touchpad reports
# information differing then what is expected in this
# image.  If so it will initiate a touchpad flashing
# process

# All possible error codes from the firmware flashing utility.
readonly RC_SUCCESS=0
readonly RC_ERROR_TIMEOUT=1
readonly RC_ERROR_FLASH_FAILED=2
readonly RC_ERROR_BOOT_ID=3
readonly RC_ERROR_FUNCTION_NOT_SUPPORTED=4
readonly RC_ERROR_FLASH_IMAGED_CORRUPTED=5
readonly RC_ERROR_MODULE_ID_NOT_COMPATIBLE=6
readonly RC_ERROR_FIRMWARE_REV_THE_SAME=7
readonly RC_ERROR_INVALID_PARAMETER=8
readonly RC_ERROR_FILE_NOT_FOUND=9
readonly RC_INPROGRESS=10

# New touchpad firmware image location.
readonly tp_firmware_dir=/system/etc/firmware
readonly tp_firmware_name=PR1469020-tm2240-001.img

# Existing touchpad firmware information location.
readonly base=/sys/bus/rmi/devices/touchpad.fn01
readonly datecode=${base}/datecode
readonly productid=${base}/productid
readonly productinfo=${base}/productinfo

# Bail if there is no firmware available to flash.
tp_firmware=${tp_firmware_dir}/${tp_firmware_name}
if [ ! -e "$tp_firmware" ]; then
  echo "Unable to locate proper touchpad firmware:${tp_firmware}"
  exit 1
fi

# Parse existing firmware information from touchpad.
function display_info {
  echo "time:$(date) datecode:$(cat ${datecode}) product_id:$(cat ${productid}) product_info:$(cat ${productinfo})"
}

# Display the relevant data portion before potential flash.
echo "Before $(display_info)"

# Execute the firmware flash command.
# The flash binary will determine if the flash requires upgrade or not.
# This should take 7-8 seconds to complete if a firmware flash is
# determined to proceed based upon firmware revision levels.
synaptics_fw_updater -b ${tp_firmware}
RC=$?

# Decode the return code from the firmware flash binary.
echo -n "RC:${RC} "
case "$RC" in
  "$RC_SUCCESS")
    echo "Successfully flashed touchpad firmware"
# Restore previous delta position behavior
    echo "2 2" > /d/rmi/touchpad/F11/delta_threshold.0
    ;;
  "$RC_ERROR_TIMEOUT")
    echo "ERROR Timed out flashing touchpad firmware"
    ;;
  "$RC_ERROR_FLASH_FAILED")
    echo "ERROR Failed flashing touchpad firmware"
    ;;
  "$RC_ERROR_BOOT_ID")
    echo "ERROR Failed boot identifier"
    ;;
  "$RC_ERROR_FUNCTION_NOT_SUPPORTED")
    echo "ERROR Flashing capability not supported"
    ;;
  "$RC_ERROR_FLASH_IMAGED_CORRUPTED")
    echo "ERROR Flash image corrupted"
    ;;
  "$RC_ERROR_MODULE_ID_NOT_COMPATIBLE")
    echo "ERROR Module identifier not compatible"
    ;;
  "$RC_ERROR_FIRMWARE_REV_THE_SAME")
    echo "INFO Touchpad firmware identical to image...skipping"
    ;;
  "$RC_ERROR_INVALID_PARAMETER")
    echo "ERROR Invalid flash parameter"
    ;;
  "$RC_ERROR_FILE_NOT_FOUND")
    echo "ERROR Unable to find firmware image file to flash"
    ;;
  "$RC_INPROGRESS")
    echo "ERROR Exited with flashing process still in progress"
    ;;
  *)
    echo "ERROR Unknown exit code"
    ;;
esac

# Display the relevant data portions after potential flash.
echo "After  $(display_info)"
