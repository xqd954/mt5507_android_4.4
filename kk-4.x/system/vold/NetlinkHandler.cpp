/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define LOG_TAG "Vold"

#include <cutils/log.h>
#include <cutils/properties.h>

#include <sysutils/NetlinkEvent.h>
#include "NetlinkHandler.h"
#include "VolumeManager.h"

NetlinkHandler::NetlinkHandler(int listenerSocket) :
                NetlinkListener(listenerSocket) {
}

NetlinkHandler::~NetlinkHandler() {
}

int NetlinkHandler::start() {
    return this->startListener();
}

int NetlinkHandler::stop() {
    return this->stopListener();
}

void NetlinkHandler::onEvent(NetlinkEvent *evt) {
    VolumeManager *vm = VolumeManager::Instance();
    const char *subsys = evt->getSubsystem();
    const char *wifiInterface = evt->findParam("INTERFACE");
    char s_prop[PROPERTY_VALUE_MAX] = {0};

    property_get("sys.vold.autoflag", s_prop, "1");
    if(strcmp(s_prop, "1") == 0) {

        SLOGI("NetlinkEvent: subsys= %s, wifiInterface= %s.\n", subsys, wifiInterface);
        evt->dump();

        vm->handleWifiProduct(evt);    
        
        if(wifiInterface != NULL 
            && (strncmp(wifiInterface, "wlan", strlen("wlan")) == 0 ||
                  strncmp(wifiInterface, "rea", strlen("rea")) == 0 ||
                  strncmp(wifiInterface, "wifi", strlen("wifi")) == 0 ||
                  strncmp(wifiInterface, "ath", strlen("ath")) == 0))
        {
            vm->handleWifiEvent(evt);
            return;
        }
        
        if (!subsys) {
            SLOGW("No subsystem found in netlink event");
            return;
        }

        if (!strcmp(subsys, "block")) {
            vm->handleBlockEvent(evt);
#ifndef ANDROID_DEFAULT_CODE
        //2012/8/6,added by Daniel Hsiao,for USB Audio detection support
        } else if (!strcmp(subsys, "sound")) {
            const char *devname = evt->findParam("DEVNAME");
            const char* devpath = evt->findParam("DEVPATH");
            if (devname != NULL)
            {
                if (strstr(devpath, "Usb"))
                {
                    SLOGI("Check usb sound device '%s'", devname);
                    if (!strncmp(devname, "snd/pcm", 7))
                    {
                        if((devname[7] == 'C') &&
                            (devname[9] == 'D') &&
                            (devname[11] == 'p'))
                        {
                            SLOGI("found device to check handleSoundEvent");
                            vm->handleSoundEvent(evt);
                        }
                    }
                }
            }
#endif
        } else if (!strcmp(subsys, "switch")) {
           // vm->handleSwitchEvent(evt);
        } else if (!strcmp(subsys, "battery")) {
        } else if (!strcmp(subsys, "power_supply")) {
        }
    }
}
