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

#ifndef _NTFS_H
#define _NTFS_H

#include <unistd.h>
/* Patch Begin by Sarah Zhang(zxia@tcl.com) */
#define FUSE_MODULE_PATH    "/sys/module/fuse"
#define FUSE_KO_PATH    "/system/lib/modules/fuse.ko"
#define NTFS_KO_PATH    "/system/lib/modules/ntfs.ko"
#define NTFS_MODULE_PATH    "/sys/module/ntfs"
/* Patch End by Sarah Zhang(zxia@tcl.com) */
class Ntfs {
public:
/* Patch Begin by Sarah Zhang(zxia@tcl.com) */
    static int mountVolumeByNtfs3g(const char* devpath, const char* mntpath);
    static int mountVolumeByNtfsKo(const char* devpath, const char* mntpath, int mntflags);
/* Patch End by Sarah Zhang(zxia@tcl.com) */
    static int check(const char *fsPath);
    static int doMount(const char *fsPath, const char *mountPoint, bool ro,
                       bool remount, int ownerUid, int ownerGid, int permMask,
                       bool createLost);
    static int format(const char *fsPath, unsigned int numSectors);
};

#endif
