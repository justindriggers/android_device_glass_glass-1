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

LOCAL_PATH := device/glass/glass-1

TARGET_BOARD_OMAP_CPU := 4430

# Include common omap4 makefile
$(call inherit-product, hardware/ti/omap4/omap4.mk)

WIFI_BAND := 802_11_BG

# Include bcm4330 makefile
$(call inherit-product, hardware/broadcom/wlan/bcmdhd/firmware/bcm4330/device-bcm.mk)

# Init files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/init.omap4430.rc:root/init.omap4430.rc \
    $(LOCAL_PATH)/rootdir/init.omap4430.usb.rc:root/init.omap4430.usb.rc \
    $(LOCAL_PATH)/rootdir/ueventd.omap4430.rc:root/ueventd.omap4430.rc

# Fstab
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/rootdir/fstab.omap4430:root/fstab.omap4430

# Audio
PRODUCT_COPY_FILES += \
    device/glass/glass-1/audio/audio_policy.conf:system/etc/audio_policy.conf

PRODUCT_COPY_FILES += \
    device/glass/glass-1/media_codecs.xml:system/etc/media_codecs.xml \
    device/glass/glass-1/media_profiles.xml:system/etc/media_profiles.xml

PRODUCT_COPY_FILES += \
    device/glass/glass-1/wifi/bcmdhd.cal:system/etc/wifi/bcmdhd.cal

# Hardware-specific features
PRODUCT_COPY_FILES += \
    device/glass/glass-1/permissions/hardware.xml:system/etc/permissions/hardware.xml

DEVICE_PACKAGE_OVERLAYS := \
    device/glass/glass-1/overlay

# Wi-Fi
PRODUCT_PACKAGES += \
    libwpa_client \
    dhcpd.conf \
    wpa_supplicant \
    wpa_supplicant.conf

# Packages
PRODUCT_PACKAGES += \
    audio.primary.glass_1 \
    camera.omap4 \
    lights.omap4 \
    power.omap4

# Filesystem
PRODUCT_PACKAGES += \
    e2fsck \
    make_ext4fs \
    setup_fs

$(call inherit-product-if-exists, vendor/glass/glass-1/device-vendor.mk)
$(call inherit-product, frameworks/native/build/phone-xhdpi-1024-dalvik-heap.mk)
