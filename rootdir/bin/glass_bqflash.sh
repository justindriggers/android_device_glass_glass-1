#!/system/bin/sh

# Get the battery profile id
batprofile=`bootconfig /dev/block/platform/omap/omap_hsmmc.1/by-name/bootconfig read battery_profile`
if [ -z $batprofile ]
then
	batprofile=0
fi

# Build the filenames for this battery type
fwfile=/system/etc/firmware/bq27520-$batprofile.bqfs
datafile=/system/etc/firmware/bq27520-$batprofile.dffs

if [ -e $fwfile ]
then
	echo "Flashing gas gauge firmware $fwfile"

	# Flash the gas gauge bq27520 firmware first.
	/system/bin/bqflash $fwfile

	# Flash the gas gauge firmware parameters unless we failed to update the firmware on previous step.
	RETVAL=$?
	[ $RETVAL -eq 0 ] && /system/bin/bqflash $datafile
elif [ -e $datafile ]
then
	echo "Flashing gas gauge data file $datafile"

	# Just try to flash the firmware parameters
	/system/bin/bqflash $datafile
else
	echo "An appropriate fw/data file was not found for this battery; profile id is $batprofile"
fi

