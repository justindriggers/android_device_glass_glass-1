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
#include <stdlib.h>

#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <tinyalsa/asoundlib.h>

struct notle_audio_device {
    struct audio_hw_device hw_device;

    struct mixer *mixer;
};

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
