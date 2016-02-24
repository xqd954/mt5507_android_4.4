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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/mount.h>

#include <linux/kdev_t.h>

#define LOG_TAG "Vold_Ntfs"

#include <cutils/log.h>
#include <cutils/properties.h>

#include "Ntfs.h"
#define  NTFS_3G_PATH  "/system/bin/ntfs-3g"
#define MT5507_VOLD_NTFS
extern "C" int logwrap(int argc, const char **argv, int background);
extern "C" int mount(const char *, const char *, const char *, unsigned long, const void *);

int Ntfs::check(const char *fsPath) {

    // no NTFS file system check is performed, always return true
    SLOGI("Ntfs filesystem: Skipping fs checks\n");
    return 0;

}

/*Patch Begin by Sarah Zhang(zxia@tcl.com) */
int Ntfs::mountVolumeByNtfs3g(const char* devpath, const char* mntpath)
{
    int ret = 0;
    char cmd[128];

    if(access(FUSE_MODULE_PATH, F_OK) != 0)
    {
        memset(cmd, 0, 128);
        sprintf(cmd, "insmod %s", FUSE_KO_PATH);
        ret = system(cmd);
        if(ret == -1 || ret == 127)
                {
                        SLOGE("%s fail\n", cmd);
                        return -1;
                }
    }

    memset(cmd, 0, 128);
    sprintf(cmd, "%s %s %s", NTFS_3G_PATH, devpath, mntpath);
    ret = system(cmd);
    SLOGI("return value: %d, error[%d]: %s\n", ret, errno, strerror(errno));
    if(ret == -1 || ret == 127)
        {
            SLOGE("%s fail!\n", cmd);
            ret = -1;
        }

    return ret;

}

int Ntfs::mountVolumeByNtfsKo(const char* devpath, const char* mntpath, int mntflags)
{
    int    ret = 0;
    char   cmd[128] = {0};

    if(access(NTFS_MODULE_PATH, F_OK) != 0)
    {
        sprintf(cmd, "insmod %s", NTFS_KO_PATH);
        ret = system(cmd);
        if(ret == -1 || ret == 127)
                {
                        SLOGE("%s fail\n", cmd);
                        return -1;
                }
    }
    ret = mount(devpath, mntpath, "ntfs", mntflags, "nls=utf8,umask=0000");

    return ret;
}
/*Patch End by Sarah Zhang(zxia@tcl.com) */
int Ntfs::doMount(const char *fsPath, const char *mountPoint,
                 bool ro, bool remount, int ownerUid, int ownerGid,
                 int permMask, bool createLost) {

	int rc;
	int retries = 10;
    const char *args[16];
    char mountData[255];
	unsigned long flags = 0;
	flags = MS_DIRSYNC | MS_NODEV | MS_NOEXEC;
	flags = flags | (ro? MS_RDONLY : 0);
	flags = flags | (remount?  MS_REMOUNT : 0);
#ifdef REALTEK_VOLD_NTFS
/*Patch Begin by Sarah Zhang(zxia@tcl.com) */
        SLOGD("Sarah debug:REALTEK_VOLD_NTFS\n");
	do {
		SLOGI("mount ntfs block device by realtek ntfs ufsd");
		sprintf(mountData, "force,nls=utf8,uid=%d,gid=%d,fmask=%o,dmask=%o", ownerUid, ownerGid, permMask, permMask);
		rc = mount(fsPath, mountPoint, "ufsd",flags, mountData);
		SLOGI("Sarah ntfs debug: Ntfs::doMount: retries=%d,rc=%d,flags=%d,mountData=%s", retries, rc, flags, mountData);
		usleep(3000*1000);
		if (!rc)
			break;
	}while(retries--);/* Sometimes device hasn't be ready to mount so need to retry */
    if (rc) {
        SLOGE("%s appears to be a read only filesystem - retrying mount RO", fsPath);
        flags |= MS_RDONLY;
        rc = mount(fsPath, mountPoint, "ntfs", flags, mountData);
    }
#elif defined MT5507_VOLD_NTFS
     SLOGD("Sarah debug: MT5507_VOLD_NTFS fsPath=%s,mountPoint=%s,flags=%d\n",fsPath,mountPoint,flags);
     if (access(NTFS_3G_PATH, F_OK) == 0 && access(FUSE_KO_PATH, F_OK) == 0) {
        SLOGI("Sarah debug: ntfs-3g mount\n");
        rc = Ntfs::mountVolumeByNtfs3g(fsPath, mountPoint);
      } else { 
        SLOGI("Sarah debug:ntfs.ko mount\n");
        rc = mountVolumeByNtfsKo(fsPath,mountPoint,flags);
      }
#else
    SLOGD("Sarah debug: mount ntfs block device by tcl ntfs ntfs-3g");
    sprintf(mountData,"locale=utf8,uid=%d,gid=%d,fmask=%o,dmask=%o,inherit, atime",\
			ownerUid, ownerGid, permMask, permMask);

    args[0] = NTFS_3G_PATH;
    args[1] = fsPath;
    args[2] = mountPoint;
    args[3] = "-o";
    args[4] = mountData;
    args[5] = NULL;
    rc = logwrap(5, args, 1);
    if (rc) {
        SLOGE("%s appears to be a read only filesystem - retrying mount RO", fsPath);
        flags |= MS_RDONLY;
        rc = mount(fsPath, mountPoint, "ntfs", flags, mountData);
    }
#endif
/*Patch End by Sarah Zhang(zxia@tcl.com) */
	SLOGD("mount ntfs filesystems finished! rc:%d\n", rc);
	return rc; 
}

int Ntfs::format(const char *fsPath, unsigned int numSectors) {

    SLOGE("Format ntfs filesystem not supported\n");
	return 0;
}
