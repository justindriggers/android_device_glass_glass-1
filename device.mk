#
# Copyright (C) 2016 Justin Driggers
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

PRODUCT_COPY_FILES += \
    device/glass/glass-1/rootdir/init.omap4430.rc:root/init.omap4430.rc \
    device/glass/glass-1/rootdir/init.omap4430.usb.rc:root/init.omap4430.usb.rc \
    device/glass/glass-1/rootdir/init.omap4430.bt.rc:root/init.omap4430.bt.rc \
    device/glass/glass-1/rootdir/init.omap4430.buildspecific.rc:root/init.omap4430.buildspecific.rc \
    device/glass/glass-1/rootdir/fstab.omap4430:root/fstab.omap4430 \
    device/glass/glass-1/rootdir/ueventd.omap4430.rc:root/ueventd.omap4430.rc

PRODUCT_COPY_FILES += \
    device/glass/glass-1/rootdir/bin/glass_bdaddr.sh:system/bin/glass_bdaddr.sh \
    device/glass/glass-1/rootdir/bin/glass_bootconfig.sh:system/bin/glass_bootconfig.sh \
    device/glass/glass-1/rootdir/bin/glass_bqflash.sh:system/bin/glass_bqflash.sh \
    device/glass/glass-1/rootdir/bin/glass_gpsclear.sh:system/bin/glass_gpsclear.sh \
    device/glass/glass-1/rootdir/bin/glass_init.sh:system/bin/glass_init.sh \
    device/glass/glass-1/rootdir/bin/glass_tpflash.sh:system/bin/glass_tpflash.sh

PRODUCT_COPY_FILES += \
    device/glass/glass-1/firmware/bq27520-0.bqfs:system/etc/firmware/bq27520-0.bqfs \
    device/glass/glass-1/firmware/bq27520-0.dffs:system/etc/firmware/bq27520-0.dffs \
    device/glass/glass-1/firmware/bq27520-1.bqfs:system/etc/firmware/bq27520-1.bqfs \
    device/glass/glass-1/firmware/bq27520-1.dffs:system/etc/firmware/bq27520-1.dffs \
    device/glass/glass-1/firmware/bq27520-2.bqfs:system/etc/firmware/bq27520-2.bqfs \
    device/glass/glass-1/firmware/bq27520-2.dffs:system/etc/firmware/bq27520-2.dffs \
    device/glass/glass-1/firmware/dss_fpga.img:system/etc/firmware/dss_fpga.img \
    device/glass/glass-1/firmware/PR1456632-tm2240-001.img:system/etc/firmware/PR1456632-tm2240-001.img \
    device/glass/glass-1/firmware/PR1469020-tm2240-001.img:system/etc/firmware/PR1469020-tm2240-001.img \

PRODUCT_COPY_FILES += \
    device/glass/glass-1/audio_policy.conf:system/etc/audio_policy.conf

PRODUCT_COPY_FILES += \
    device/glass/glass-1/media_codecs.xml:system/etc/media_codecs.xml \
    device/glass/glass-1/media_profiles.xml:system/etc/media_profiles.xml

PRODUCT_COPY_FILES += \
    device/glass/glass-1/wifi/bcmdhd.cal:system/etc/wifi/bcmdhd.cal \
    device/glass/glass-1/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf

PRODUCT_COPY_FILES += \
    device/glass/glass-1/permissions/hardware.xml:system/etc/permissions/hardware.xml

PRODUCT_TAGS += dalvik.gc.type-precise

DEVICE_PACKAGE_OVERLAYS := \
    device/glass/glass-1/overlay

PRODUCT_PACKAGES += \
    com.android.future.usb.accessory

PRODUCT_PACKAGES += \
    libgenlock

PRODUCT_PACKAGES += \
    lights.omap4

PRODUCT_PACKAGES += \
    gps.omap4

PRODUCT_PACKAGES += \
    e2fsck \
    make_ext4fs \
    setup_fs

PRODUCT_PACKAGES += \
    bdAddrLoader

PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=false

PRODUCT_PROPERTY_OVERRIDES += \
    glass.gestureservice.start=1

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=ptp,adb

PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0 \
    wifi.supplicant_scan_interval=60

PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.enable_timeout_ms=10000

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.forced_orientation=0

PRODUCT_PROPERTY_OVERRIDES += \
    ro.hwui.disable_scissor_opt=true \
    ro.hwui.drop_shadow_cache_size=1 \
    ro.hwui.gradient_cache_size=0.5 \
    ro.hwui.layer_cache_size=4 \
    ro.hwui.patch_cache_size=64 \
    ro.hwui.path_cache_size=1 \
    ro.hwui.r_buffer_cache_size=1 \
    ro.hwui.shape_cache_size=0.5 \
    ro.hwui.text_large_cache_height=512 \
    ro.hwui.text_large_cache_width=2048 \
    ro.hwui.text_small_cache_height=256 \
    ro.hwui.text_small_cache_width=1024 \
    ro.hwui.texture_cache_flushrate=0.4 \
    ro.hwui.texture_cache_size=12

PRODUCT_PROPERTY_OVERRIDES += \
    ro.hwui.disable_scissor_opt=true \
    ro.hwui.drop_shadow_cache_size=1 \
    ro.hwui.gradient_cache_size=0.5 \
    ro.hwui.layer_cache_size=4 \
    ro.hwui.patch_cache_size=64 \
    ro.hwui.path_cache_size=1 \
    ro.hwui.r_buffer_cache_size=1 \
    ro.hwui.shape_cache_size=0.5 \
    ro.hwui.text_large_cache_height=512 \
    ro.hwui.text_large_cache_width=2048 \
    ro.hwui.text_small_cache_height=256 \
    ro.hwui.text_small_cache_width=1024 \
    ro.hwui.texture_cache_flushrate=0.4 \
    ro.hwui.texture_cache_size=12

PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=131072

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=240

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapgrowthlimit=72m \
    dalvik.vm.heapmaxfree=2m \
    dalvik.vm.heapminfree=512k \
    dalvik.vm.heapsize=192m \
    dalvik.vm.heapstartsize=5m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.jit.codecachesize=0

$(call inherit-product-if-exists, hardware/ti/omap4-aah/omap4.mk)
