#!/system/bin/sh

GPS_DIR=/data/gps

LOGS=(BriefLog.txt DetailedLog.txt agps.txt ee_download_debug.txt nav.gps sirf_interface_log.txt)

# If GPS directory doesn't exist, do nothing and exit
if [[ ! -d "$GPS_DIR" ]]; then
   echo "No gps logs\n"
   exit 0;
fi

# Check for individual log files, and delete them
for l in "${LOGS[@]}"
do
   log="$GPS_DIR/$l"
   if [[ -e "$log" ]]; then
      echo "Remove $log."
      rm $log
   fi
done
