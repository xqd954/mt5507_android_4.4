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
#include <sys/stat.h>
#include <sys/types.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <dirent.h>
#include <fs_mgr.h>

#define LOG_TAG "Vold"

#include "cutils/klog.h"
#include "cutils/log.h"
#include "cutils/properties.h"

#include "VolumeManager.h"
#include "CommandListener.h"
#include "NetlinkManager.h"
#include "DirectVolume.h"
#include "ExternalVolume.h"
#include "DMCommandListener.h"
#include "cryptfs.h"

static int process_config(VolumeManager *vm);
static int emmc_config(VolumeManager *vm);
static void coldboot(const char *path);

#define FSTAB_PREFIX "/fstab."
struct fstab *fstab;

int main() {

    VolumeManager *vm;
    CommandListener *cl;
    DMCommandListener *dmCl;
    NetlinkManager *nm;

#if 0
    char s_prop[PROPERTY_VALUE_MAX] = {0};
#endif
    SLOGI("Vold 2.1 (the revenge) firing up");

    mkdir("/dev/block/vold", 0755);
    mkdir("/mnt/usb", 0755);
    /* For when cryptfs checks and mounts an encrypted filesystem */
    klog_set_level(6);

    /* Create our singleton managers */
    if (!(vm = VolumeManager::Instance())) {
        SLOGE("Unable to create VolumeManager");
        exit(1);
    };

    if (!(nm = NetlinkManager::Instance())) {
        SLOGE("Unable to create NetlinkManager");
        exit(1);
    };


    cl = new CommandListener();
    dmCl = new DMCommandListener();
    vm->setBroadcaster((SocketListener *) cl);
    vm->setDMBroadcaster((SocketListener *) dmCl);
    nm->setBroadcaster((SocketListener *) cl);

    if (vm->start()) {
        SLOGE("Unable to start VolumeManager (%s)", strerror(errno));
        exit(1);
    }

    if (process_config(vm)) {
        SLOGE("Error reading configuration (%s)... continuing anyways", strerror(errno));
    }

    if (emmc_config(vm)) {
        SLOGE("Error reading configuration (%s)... continuing anyways", strerror(errno));
    }
    if (nm->start()) {
        SLOGE("Unable to start NetlinkManager (%s)", strerror(errno));
        exit(1);
    }

    //coldboot("/sys/block");

#if 1
    coldboot("/sys/devices");
#else
    property_get("ro.mtk.system.tkui", s_prop, "1");
    if(strcmp(s_prop, "1") == 0) {
       SLOGI("Android do coldboot!!\n");
       coldboot("/sys/devices");
    }else {
       SLOGI("No need do coldboot in Android world!!\n");
    }
#endif
//    coldboot("/sys/class/switch");

    /*
     * Now that we're up, we can respond to commands
     */
    if (cl->startListener()) {
        SLOGE("Unable to start CommandListener (%s)", strerror(errno));
        exit(1);
    }

    if (dmCl->startListener()){
        SLOGE("Unable to start DMCommandListener (%s)", strerror(errno));
        exit(1);
    }

    // Eventually we'll become the monitoring thread
    while(1) {
        sleep(1000);
    }

    SLOGI("Vold exiting");
    exit(0);
}

static void do_coldboot(DIR *d, int lvl)
{
    struct dirent *de;
    int dfd, fd;

    dfd = dirfd(d);

    fd = openat(dfd, "uevent", O_WRONLY);
    if(fd >= 0) {
        write(fd, "add\n", 4);
        close(fd);
    }

    while((de = readdir(d))) {
        DIR *d2;

        if (de->d_name[0] == '.')
            continue;

        if (de->d_type != DT_DIR && lvl > 0)
            continue;

        fd = openat(dfd, de->d_name, O_RDONLY | O_DIRECTORY);
        if(fd < 0)
            continue;

        d2 = fdopendir(fd);
        if(d2 == 0)
            close(fd);
        else {
            do_coldboot(d2, lvl + 1);
            closedir(d2);
        }
    }
}

static void coldboot(const char *path)
{
    DIR *d = opendir(path);
    if(d) {
        do_coldboot(d, 0);
        closedir(d);
    }
}

static int process_config(VolumeManager *vm)
{
    char fstab_filename[PROPERTY_VALUE_MAX + sizeof(FSTAB_PREFIX)];
    char propbuf[PROPERTY_VALUE_MAX];
    int i;
    int ret = -1;
    int flags;

    property_get("ro.hardware", propbuf, "");
    snprintf(fstab_filename, sizeof(fstab_filename), FSTAB_PREFIX"%s", propbuf);

    fstab = fs_mgr_read_fstab(fstab_filename);
    if (!fstab) {
        SLOGE("failed to open %s\n", fstab_filename);
        return -1;
    }

    /* Loop through entries looking for ones that vold manages */
    for (i = 0; i < fstab->num_entries; i++) {
        if (fs_mgr_is_voldmanaged(&fstab->recs[i])) {
            ExternalVolume *dv = NULL;
            flags = 0;

            /* Set any flags that might be set for this volume */
            if (fs_mgr_is_nonremovable(&fstab->recs[i])) {
                flags |= VOL_NONREMOVABLE;
            }
            if (fs_mgr_is_encryptable(&fstab->recs[i])) {
                flags |= VOL_ENCRYPTABLE;
            }
            /* Only set this flag if there is not an emulated sd card */
            if (fs_mgr_is_noemulatedsd(&fstab->recs[i]) &&
                !strcmp(fstab->recs[i].fs_type, "auto")) {
                flags |= VOL_PROVIDES_ASEC;
            }
            dv = new ExternalVolume(vm, &(fstab->recs[i]), flags);

            if (dv->addPath(fstab->recs[i].blk_device)) {
                SLOGE("Failed to add devpath %s to volume %s",
                      fstab->recs[i].blk_device, fstab->recs[i].label);
                goto out_fail;
            }

            vm->addVolume(dv);
        }
    }

    ret = 0;

out_fail:
    return ret;
}

static int emmc_config(VolumeManager *vm)
{
    FILE *fp;
    int n = 0;
    char line[255];

    if (!(fp = fopen("/exmount.cfg", "r"))) {
        SLOGD("EMMC config file open fail.\n");       
        return -1;
    }

    while(fgets(line, sizeof(line), fp)) {
        n++;
        SLOGD("EMMC get line:  %s", line); 
        if (line[0] == '#' || line[0] == '\0' || line[0] == 13 || line[0] == 10)
            continue;
        if(line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';    
        vm->mExclusiveMountStrs->push_back(strdup(line));        
    }
#if 0
     ExclusiveMountName::iterator it;
     for(it = vm->mExclusiveMountStrs->begin(); it != vm->mExclusiveMountStrs->end(); it++)
    {
         SLOGD("EMMC exclude: %s\n", *it);        
    }
#endif
    fclose(fp);
    return 0;
}
