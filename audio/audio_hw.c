/*
 * Copyright (C) 2016 Justin Driggers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "audio_hw_primary"
/*#define LOG_NDEBUG 0*/

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

#include <cutils/log.h>

#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <tinyalsa/asoundlib.h>

/* Mixer control names */
#define MIXER_DL2_LEFT_EQUALIZER            "DL2 Left Equalizer"
#define MIXER_DL2_RIGHT_EQUALIZER           "DL2 Right Equalizer"
#define MIXER_DL1_MEDIA_PLAYBACK_VOLUME     "DL1 Media Playback Volume"
#define MIXER_DL1_VOICE_PLAYBACK_VOLUME     "DL1 Voice Playback Volume"
#define MIXER_DL1_TONES_PLAYBACK_VOLUME     "DL1 Tones Playback Volume"
#define MIXER_DL2_MEDIA_PLAYBACK_VOLUME     "DL2 Media Playback Volume"
#define MIXER_DL2_VOICE_PLAYBACK_VOLUME     "DL2 Voice Playback Volume"
#define MIXER_DL2_TONES_PLAYBACK_VOLUME     "DL2 Tones Playback Volume"
#define MIXER_SDT_DL_VOLUME                 "SDT DL Volume"
#define MIXER_SDT_UL_VOLUME                 "SDT UL Volume"

#define MIXER_HEADSET_PLAYBACK_VOLUME       "Headset Playback Volume"
#define MIXER_HANDSFREE_PLAYBACK_VOLUME     "Handsfree Playback Volume"
#define MIXER_EARPHONE_PLAYBACK_VOLUME      "Earphone Playback Volume"
#define MIXER_BT_UL_VOLUME                  "BT UL Volume"

#define MIXER_DL1_EQUALIZER                 "DL1 Equalizer"
#define MIXER_DL1_MIXER_MULTIMEDIA          "DL1 Mixer Multimedia"
#define MIXER_DL1_MIXER_VOICE               "DL1 Mixer Voice"
#define MIXER_DL1_MIXER_TONES               "DL1 Mixer Tones"
#define MIXER_DL2_MIXER_MULTIMEDIA          "DL2 Mixer Multimedia"
#define MIXER_DL2_MIXER_VOICE               "DL2 Mixer Voice"
#define MIXER_DL2_MIXER_TONES               "DL2 Mixer Tones"
#define MIXER_SIDETONE_MIXER_PLAYBACK       "Sidetone Mixer Playback"
#define MIXER_SIDETONE_MIXER_CAPTURE        "Sidetone Mixer Capture"
#define MIXER_DL2_MONO_MIXER                "DL2 Mono Mixer"
#define MIXER_DL1_PDM_SWITCH                "DL1 PDM Switch"
#define MIXER_DL1_BT_VX_SWITCH              "DL1 BT_VX Switch"
#define MIXER_VOICE_CAPTURE_MIXER_CAPTURE   "Voice Capture Mixer Capture"

#define MIXER_HS_LEFT_PLAYBACK              "Headset Left Playback"
#define MIXER_HS_RIGHT_PLAYBACK             "Headset Right Playback"
#define MIXER_HF_LEFT_PLAYBACK              "Handsfree Left Playback"
#define MIXER_HF_RIGHT_PLAYBACK             "Handsfree Right Playback"
#define MIXER_EARPHONE_ENABLE_SWITCH        "Earphone Playback Switch"

#define MIXER_ANALOG_LEFT_CAPTURE_ROUTE     "Analog Left Capture Route"
#define MIXER_ANALOG_RIGHT_CAPTURE_ROUTE    "Analog Right Capture Route"
#define MIXER_CAPTURE_PREAMPLIFIER_VOLUME   "Capture Preamplifier Volume"
#define MIXER_CAPTURE_VOLUME                "Capture Volume"
#define MIXER_AMIC_UL_VOLUME                "AMIC UL Volume"
#define MIXER_AUDUL_VOICE_UL_VOLUME         "AUDUL Voice UL Volume"
#define MIXER_DMIC1_UL_VOLUME               "DMIC1 UL Volume"
#define MIXER_MUX_VX0                       "MUX_VX0"
#define MIXER_MUX_VX1                       "MUX_VX1"
#define MIXER_MUX_UL10                      "MUX_UL10"
#define MIXER_MUX_UL11                      "MUX_UL11"

/* Glass-specific sysfs paths */
#define SYS_CONTROL_HEADSET                 "/sys/devices/platform/usb_mux.0/headset"
#define SYS_CONTROL_MUX_MODE                "/sys/devices/platform/usb_mux.0/mux_mode"
#define SYS_CONTROL_ID_MONITOR              "/sys/devices/platform/usb_mux.0/id_monitor"

struct pcm_config pcm_config_mm = {
    .chanels = 2,
    .rate = DEFAULT_OUT_SAMPLING_RATE,
    .period_size = LONG_PERIOD_SIZE,
    .period_count = PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
}

struct pcm_config pcm_config_wb_vx = {
    .chanels = 0,
    .rate = DEFAULT_OUT_SAMPLING_RATE,
    .period_size = LONG_PERIOD_SIZE,
    .period_count = PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
}

struct pcm_config pcm_config_nb_vx = {
    .chanels = 0,
    .rate = DEFAULT_OUT_SAMPLING_RATE,
    .period_size = LONG_PERIOD_SIZE,
    .period_count = PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
}

struct pcm_config pcm_config_mm_vx = {
    .chanels = 0,
    .rate = DEFAULT_OUT_SAMPLING_RATE,
    .period_size = LONG_PERIOD_SIZE,
    .period_count = PLAYBACK_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
}

struct mixer_ctls
{
    struct mixer_ctl *mm_dl1;
    struct mixer_ctl *vx_dl1;
    struct mixer_ctl *dl1_headset;
    struct mixer_ctl *hs_left_playback;
    struct mixer_ctl *hs_right_playback;
    struct mixer_ctl *earpiece_enable;
    struct mixer_ctl *voice_ul_volume;
    struct mixer_ctl *headset_volume;
    struct mixer_ctl *earpiece_volume;
    struct mixer_ctl *dmic1_ul_volume;
};

struct notle_audio_device {
    struct audio_hw_device hw_device;

    pthread_mutex_t lock;
    struct mixer *mixer;
    struct mixer_ctls mixer_ctls;
};

/* The enable flag when 0 makes the assumption that enums are disabled by
 * "Off" and integers/booleans by 0 */
static int set_route_by_array(struct mixer *mixer, struct route_setting *route,
                              int enable)
{
    struct mixer_ctl *ctl;
    unsigned int i, j;

    /* Go through the route array and set each value */
    i = 0;
    while (route[i].ctl_name) {
        ctl = mixer_get_ctl_by_name(mixer, route[i].ctl_name);
        if (!ctl)
            return -EINVAL;

        if (route[i].strval) {
            if (enable)
                mixer_ctl_set_enum_by_string(ctl, route[i].strval);
            else
                mixer_ctl_set_enum_by_string(ctl, "Off");
        } else {
            /* This ensures multiple (i.e. stereo) values are set jointly */
            for (j = 0; j < mixer_ctl_get_num_values(ctl); j++) {
                if (enable)
                    mixer_ctl_set_value(ctl, j, route[i].intval);
                else
                    mixer_ctl_set_value(ctl, j, 0);
            }
        }
        i++;
    }

    return 0;
}

static int adev_init_check(const struct audio_hw_device *dev __unused)
{
    return -ENOSYS;
}

static int adev_set_voice_volume(struct audio_hw_device *dev __unused,
                                 float volume __unused)
{
    return -ENOSYS;
}

static int adev_set_master_volume(struct audio_hw_device *dev __unused,
                                  float volume __unused)
{
    return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev __unused,
                         audio_mode_t mode __unused)
{
    return -ENOSYS;
}

static int adev_set_mic_mute(struct audio_hw_device *dev __unused,
                             bool state __unused)
{
    return -ENOSYS;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev __unused,
                             bool *state __unused)
{
    return -ENOSYS;
}

static int adev_set_parameters(struct audio_hw_device *dev __unused,
                               const char *kv_pairs __unused)
{
    return -ENOSYS;
}

static char * adev_get_parameters(const struct audio_hw_device *dev __unused,
                                  const char *keys __unused)
{
    return -ENOSYS; // TODO char *
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev __unused,
                                         const struct audio_config *config __unused)
{
    return -ENOSYS;
}

static int adev_open_output_stream(struct audio_hw_device *dev __unused,
                                   audio_io_handle_t handle __unused,
                                   audio_devices_t devices __unused,
                                   audio_output_flags_t flags __unused,
                                   struct audio_config *config __unused,
                                   struct audio_stream_out **stream_out __unused,
                                   const char *address __unused)
{
    return -ENOSYS;
}

static void adev_close_output_stream(struct audio_hw_device *dev __unused,
                                     struct audio_stream_out *stream_out __unused)
{
    // do nothing
}

static int adev_open_input_stream(struct audio_hw_device *dev __unused,
                                  audio_io_handle_t handle __unused,
                                  audio_devices_t devices __unused,
                                  struct audio_config *config __unused,
                                  struct audio_stream_in **stream_in __unused,
                                  audio_input_flags_t flags __unused,
                                  const char *address __unused,
                                  audio_source_t source __unused)
{
    return -ENOSYS;
}

static void adev_close_input_stream(struct audio_hw_device *dev __unused,
                                    struct audio_stream_in *stream_in __unused)
{
    // do nothing
}

static int adev_dump(const struct audio_hw_device *dev __unused,
                     int fd __unused)
{
    return -ENOSYS;
}

static int adev_close(hw_device_t *device)
{
    struct notle_audio_device *adev = (struct notle_audio_device *)device;
    mixer_close(adev->mixer);
    free(device);
    return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct notle_audio_device *adev;

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    adev = calloc(1, sizeof(struct notle_audio_device));
    if (!adev)
        return -ENOMEM;

    adev->hw_device.common.tag = HARDWARE_DEVICE_TAG;
    adev->hw_device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->hw_device.common.module = (struct hw_module_t *) module;
    adev->hw_device.common.close = adev_close;

    adev->hw_device.init_check = adev_init_check;
    adev->hw_device.set_voice_volume = adev_set_voice_volume;
    adev->hw_device.set_master_volume = adev_set_master_volume;
    adev->hw_device.set_mode = adev_set_mode;
    adev->hw_device.set_mic_mute = adev_set_mic_mute;
    adev->hw_device.get_mic_mute = adev_get_mic_mute;
    adev->hw_device.set_parameters = adev_set_parameters;
    adev->hw_device.get_parameters = adev_get_parameters;
    adev->hw_device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->hw_device.open_output_stream = adev_open_output_stream;
    adev->hw_device.close_output_stream = adev_close_output_stream;
    adev->hw_device.open_input_stream = adev_open_input_stream;
    adev->hw_device.close_input_stream = adev_close_input_stream;
    adev->hw_device.dump = adev_dump;
    
    adev->mixer = mixer_open(0);
    if (!adev->mixer) {
        free(adev);
        ALOGE("Unable to open the mixer, aborting.");
        return -EINVAL;
    }
    
    adev->mixer_ctls.mm_dl1 = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_MIXER_MULTIMEDIA);
    adev->mixer_ctls.vx_dl1 = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_MIXER_VOICE);
    adev->mixer_ctls.dl1_headset = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DL1_PDM_SWITCH);
    adev->mixer_ctls.hs_left_playback = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_HS_LEFT_PLAYBACK);
    adev->mixer_ctls.hs_right_playback = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_HS_RIGHT_PLAYBACK);
    adev->mixer_ctls.earpiece_enable = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_EARPHONE_ENABLE_SWITCH);
    adev->mixer_ctls.voice_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_AUDUL_VOICE_UL_VOLUME);
    adev->mixer_ctls.headset_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_HEADSET_PLAYBACK_VOLUME);
    adev->mixer_ctls.earpiece_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_EARPHONE_PLAYBACK_VOLUME);
    adev->mixer_ctls.dmic1_ul_volume = mixer_get_ctl_by_name(adev->mixer,
                                           MIXER_DMIC1_UL_VOLUME);

    if (!adev->mixer_ctls.mm_dl1 ||
        !adev->mixer_ctls.vx_dl1 ||
        !adev->mixer_ctls.dl1_headset ||
        !adev->mixer_ctls.hs_left_playback ||
        !adev->mixer_ctls.hs_right_playback ||
        !adev->mixer_ctls.earpiece_enable ||
        !adev->mixer_ctls.voice_ul_volume ||
        !adev->mixer_ctls.headset_volume ||
        !adev->mixer_ctls.earpiece_volume ||
        !adev->mixer_ctls.dmic1_ul_volume) {
        mixer_close(adev->mixer);
        free(adev);
        ALOGE("Unable to locate all mixer controls, aborting.");
        return -EINVAL;
    }

    /* Set the default route before the PCM stream is opened */
    pthread_mutex_lock(&adev->lock);
    // TODO set_route_by_array(adev->mixer, defaults, 1);

    int headset_fd = open(SYS_CONTROL_HEADSET, O_RDONLY);
    if (headset_fd < 0) {
        ALOGE("Cannot open %s to check for headset\\n", SYS_CONTROL_HEADSET);
    } else {
        ALOGE("Successfully opened %s to check for headset\\n", SYS_CONTROL_HEADSET);
    }

    int mux_mode_fd = open(SYS_CONTROL_MUX_MODE, O_WRONLY);
    if (mux_mode_fd < 0) {
        ALOGE("Cannot open %s to switch USB MUX\\n", SYS_CONTROL_MUX_MODE);
    } else {
        ALOGE("Successfully opened %s to switch USB MUX\\n", SYS_CONTROL_MUX_MODE);
    }

    int id_monitor_fd = open(SYS_CONTROL_ID_MONITOR, O_WRONLY);
    if (id_monitor_fd < 0) {
        ALOGE("Cannot open %s to monitor ID line\\n", SYS_CONTROL_ID_MONITOR);
    } else {
        ALOGE("Successfully opened %s to monitor ID line\\n", SYS_CONTROL_ID_MONITOR);
    }

    pthread_mutex_unlock(&adev->lock);

    *device = &adev->hw_device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "Notle audio HW HAL",
        .author = "Justin Driggers",
        .methods = &hal_module_methods,
    },
};
