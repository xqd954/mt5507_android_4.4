/*
 * Copyright (C) 2009 The Android Open Source Project
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

#define LOG_TAG "AudioPolicyManagerDefault"
//#define LOG_NDEBUG 0

#include "AudioPolicyManagerDefault.h"

using namespace android;

namespace android_audio_legacy {

#if 1 //def WITH_A2DP
status_t AudioPolicyManagerDefault::setDeviceConnectionState(audio_devices_t device,
										  AudioSystem::device_connection_state state,
										  const char *device_address)
{
	status_t ret = AudioPolicyManagerBase::setDeviceConnectionState(device,
													 state,
													 device_address);
	
    ALOGD("new setDeviceConnectionState(device=%d,state=%d)",device,state);
    if ((mHasUsb && audio_is_usb_device((audio_devices_t)device)) ||
        (mHasA2dp && audio_is_a2dp_device((audio_devices_t)device)))
    {
		sp<IUploadService> pUS;
		sp<IServiceManager> sm = defaultServiceManager();
		sp<IBinder> binder;
		do {
			binder = sm->getService(String16("MTK.audio.uploadservice"));
			if (binder != 0)
				break;
			ALOGW("MTK.audio.uploadservice not published, waiting...");
			usleep(500000); // 0.5 s
		} while(true);

		pUS = interface_cast<IUploadService>(binder);
		if (pUS != 0) {
			if (state == AudioSystem::DEVICE_STATE_AVAILABLE) {
				pUS->startUpload();
				ALOGD("Uploadservice start running.");
			} else {
				pUS->stopUpload();
				ALOGD("Uploadservice stop running.");
			}
		} else {
			ALOGE("Get MTK.audio.uploadservice fail.");
		}
	}

	return ret;
}
#endif

extern "C" AudioPolicyInterface* createAudioPolicyManager(AudioPolicyClientInterface *clientInterface)
{
    return new AudioPolicyManagerDefault(clientInterface);
}

extern "C" void destroyAudioPolicyManager(AudioPolicyInterface *interface)
{
    delete interface;
}

}; // namespace android
