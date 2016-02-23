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

#ifndef _VOLUME_H
#define _VOLUME_H

#include <utils/List.h>
#include <fs_mgr.h>

class NetlinkEvent;
class VolumeManager;

#define FUSE_KO_PATH    "/system/lib/modules/fuse.ko"
#define NTFS_KO_PATH    "/system/lib/modules/ntfs.ko"
#define TUXERA_NTFS_KO_PATH   "/basic/lib/ko_modules/tntfs.ko"
#define TUXERA_NTFS_MODULE_PATH   "/sys/module/tntfs"
#define FUSE_MODULE_PATH    "/sys/module/fuse"
#define NTFS_MODULE_PATH    "/sys/module/ntfs"
#define NTFS_3G_PATH    "/system/bin/ntfs-3g"

typedef enum
{
    FS_TYPE_INVAL  = 0,
    FS_TYPE_FAT = 1,
    FS_TYPE_NTFS = 2,
    FS_TYPE_EXT2 = 3,
    FS_TYPE_EXT3 = 4,
    FS_TYPE_EXT4 = 5,
    FS_TYPE_ISO9660 = 6,
    FS_TYPE_EXFAT = 7
}FS_TYPE_E;

class Volume {
private:
    int mState;
    char *mDevNode;
    int mFlags;

public:
    static const int State_Init       = -1;
    static const int State_NoMedia    = 0;
    static const int State_Idle       = 1;
    static const int State_Pending    = 2;
    static const int State_Checking   = 3;
    static const int State_Mounted    = 4;
    static const int State_Unmounting = 5;
    static const int State_Formatting = 6;
    static const int State_Shared     = 7;
    static const int State_SharedMnt  = 8;

    static const char *MEDIA_DIR;
    static const char *FUSE_DIR;
    static const char *SEC_ASECDIR_EXT;
    static const char *SEC_ASECDIR_INT;
    static const char *ASECDIR;
    static const char *LOOPDIR;
    static const char *BLKID_PATH;

protected:
    char* mLabel;
    char* mUuid;
    char* mUserLabel;
    VolumeManager *mVm;
    bool mDebug;
    int mPartIdx;
    int mOrigPartIdx;
    bool mRetryMount;

    /*
     * The major/minor tuple of the currently mounted filesystem.
     */
    dev_t mCurrentlyMountedKdev;

public:
    Volume(VolumeManager *vm, const fstab_rec* rec, int flags);
    virtual ~Volume();

    int mountVol();
    int unmountVol(bool force, bool revert);
    int formatVol(bool wipe);

    const char* getLabel() { return mLabel; }
    const char* getUuid() { return mUuid; }
    const char* getUserLabel() { return mUserLabel; }
    const char *getDevNode() { return mDevNode; }
    int getState() { return mState; }
    int getFlags() { return mFlags; };
    const char* stateToStr(int state);

    /* Mountpoint of the raw volume */
    virtual const char *getMountpoint() = 0;
    virtual const char *getFuseMountpoint() = 0;

    virtual int handleBlockEvent(NetlinkEvent *evt);
    virtual int handleWifiEvent(NetlinkEvent *evt);
    virtual int handleWifiProduct(NetlinkEvent *evt);
    virtual int umountVolumeCmd(const char *label, bool force, bool revert);
    virtual int handleSdcardMountVol();
#ifndef ANDROID_DEFAULT_CODE
    //2012/8/6,added by Daniel Hsiao,for USB Audio detection support
    virtual int handleSoundEvent(NetlinkEvent *evt);
#endif
    virtual dev_t getDiskDevice();
    virtual dev_t getShareDevice();
    virtual void handleVolumeShared();
    virtual void handleVolumeUnshared();

    void setDebug(bool enable);
    virtual int getVolInfo(struct volume_info *v) = 0;

protected:
    void setUuid(const char* uuid);
    void setUserLabel(const char* userLabel);
    void setState(int state);
    void setDevNode(const char * devNode);

    virtual int getDeviceNodes(dev_t *devs, int max) = 0;
    virtual int updateDeviceInfo(char *new_path, int new_major, int new_minor) = 0;
    virtual void revertDeviceInfo(void) = 0;
    virtual int isDecrypted(void) = 0;

    int createDeviceNode(const char *path, int major, int minor);
    int mountVolumeByNtfs3g(const char* devpath, const char* mntpath);
    int mountVolumeByNtfsKo(const char* devpath, const char* mntpath, int mntflags);
    int mountVolumeByTuxeraNtfsKO(const char* devpath, const char* mntpath, int mntflags, const char* mntdata);


private:
    int initializeMbr(const char *deviceNode);
    bool isMountpointMounted(const char *path);
    int mountAsecExternal();
    int doUnmount(const char *path, bool force);
    int doMoveMount(const char *src, const char *dst, bool force);
    int extractMetadata(const char* devicePath);
};

typedef android::List<Volume *> VolumeCollection;

#endif
