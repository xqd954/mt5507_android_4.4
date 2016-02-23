/*
 * Copyright (C) 2007 The Android Open Source Project
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

#include <errno.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#include <fs_mgr.h>
#include "mtdutils/mtdutils.h"
#include "mtdutils/mounts.h"
#include "roots.h"
#include "common.h"
#include "make_ext4fs.h"

static struct fstab *fstab = NULL;
// Degin added by TCL pengpeng, mail: pengpeng01@tcl.com
#include <sys/wait.h>

#include <dirent.h>
#include "blkid/blkid.h"

// End of TCL
extern struct selabel_handle *sehandle;

void load_volume_table()
{
    int i;
    int ret;

    fstab = fs_mgr_read_fstab("/etc/recovery.fstab");
    if (!fstab) {
        LOGE("failed to read /etc/recovery.fstab\n");
        return;
    }

    ret = fs_mgr_add_entry(fstab, "/tmp", "ramdisk", "ramdisk", 0);
    if (ret < 0 ) {
        LOGE("failed to add /tmp entry to fstab\n");
        fs_mgr_free_fstab(fstab);
        fstab = NULL;
        return;
    }

    printf("recovery filesystem table\n");
    printf("=========================\n");
    for (i = 0; i < fstab->num_entries; ++i) {
        Volume* v = &fstab->recs[i];
        printf("  %d %s %s %s %lld\n", i, v->mount_point, v->fs_type,
               v->blk_device, v->length);
    }
    printf("\n");
}

Volume* volume_for_path(const char* path) {
    return fs_mgr_get_entry_for_mount_point(fstab, path);
}

int ensure_usb_mounted(Volume* v, const char* path)
{
	FILE *	fstatus;
	char	read_buf[1024];
	char	s_dev_name[128];
	char	s_dev_fullpath[128];
	int 	mount_ok = -1;
	int 	access_ok = -1;
	int 	wait_count = 0; 	
	if (strcmp(v->mount_point, "/storage/sdcard")  == 0){
        while (wait_count < 30){
            memset(s_dev_name, 0, 128);
            memset(s_dev_fullpath, 0, 128);
            fstatus = fopen("/proc/diskstats", "r");
            if (fstatus == NULL){
               return -1;
            }
            while(!feof(fstatus))
            {
            	mount_ok = -1;
                if(!fgets(read_buf, 1023 , fstatus)){
                    break;
                }
                sscanf(read_buf, "%*d %*d %32s %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u", s_dev_name);
                printf("[Root] From /proc/diskstats %s\n", s_dev_name);
                /* for look sdX*/
				if(strncmp("sd", s_dev_name, 2) == 0 && strcmp(v->mount_point, "/storage/sdcard") == 0){
                    sprintf(s_dev_fullpath, "/dev/block/%s", s_dev_name);
					
                    mount_ok = mount(s_dev_fullpath, v->mount_point, v->fs_type, 
									 MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
                    if (mount_ok != 0){                       
                        mount_ok = mount(s_dev_fullpath, v->mount_point, "ntfs",
									 MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
                    }
                }
                else if (strncmp("mmcblk", s_dev_name, 6) == 0){
                    sprintf(s_dev_fullpath, "/dev/block/%s", s_dev_name);
					
                    mount_ok = mount(s_dev_fullpath, v->mount_point, v->fs_type,
									 MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
                    if (mount_ok != 0){
                        mount_ok = mount(s_dev_fullpath, v->mount_point, "ntfs",
									 MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
                    }
                }
				if (mount_ok == 0) {
					if (access(path, R_OK) == 0){
						access_ok = 0;
						FILE* f_tmp = fopen(path, "rb");
    					if (f_tmp == NULL){
        					LOGE("failed to open %s (%s)\n", path, strerror(errno));
							umount2(v->mount_point, MNT_FORCE);
							fclose(f_tmp);
    					}else{
    					    fclose(f_tmp);
							break;
    					}
					}
					else{
						access_ok = -1;
						LOGE("ensure_path_mounted fail to access [%s]\n", path);
						umount2(v->mount_point, MNT_FORCE);
					}
				} 
            }
            fclose(fstatus);
            if (access_ok == 0){
                return 0;
            }
			LOGE("access [%s] fail, polling for mount other usb\n", path);
			usleep(800000);
            wait_count ++;
        }
        return -1;
    }
	return -1;
}
// Begin added by TCL pengpeng, mail: pengpeng01@tcl.com
#define USB_PATH "/mnt/usb"
#define USB_DEV_DIR "/dev/block"
#define USB_NODE_PREFIX "sd"

static int match_usb_dev(struct dirent *dirt, const char *path) {
    struct stat statbuf;

    if (dirt == NULL) {
        LOGE("dirent is NULL\n");
        return -1;
    }

    if (strncmp(dirt->d_name, USB_NODE_PREFIX, strlen(USB_NODE_PREFIX))) {
        LOGD("No usb device\n");
        return -1;
    }

    char device[1024];
    memset(device, 0, sizeof(device));
    snprintf(device, sizeof(device), "%s/%s", USB_DEV_DIR, dirt->d_name);

    if (stat(device, &statbuf)) {
        LOGE("File %s stat failed!!!\n", dirt->d_name);
        return -1;
    }

    if (!S_ISBLK(statbuf.st_mode)) {
        LOGD("NOT ISBLK\n");
        return -1;
    }

    blkid_cache cache = NULL;
    if (blkid_get_cache(&cache, NULL) < 0) {
        LOGE("blkid_get_cache failed!!!\n");
        return -1;
    }
    
  
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%s/%s/", USB_PATH, 
             blkid_get_tag_value(cache, "UUID", device));

    blkid_put_cache(cache);

    if (!strncmp(path, buf, strlen(buf))) {
        LOGD("MATCH UUID\n");
        return 0;
    }

    return -1;
}

static int match_spec_file(const char *dirname, const char *path, 
                           int(*match)(struct dirent *, const char *), 
                           char *matchname, int namelen) {
    struct dirent *dirt = NULL;
    DIR *dir = NULL;
    int isMatched = -1;

    dir = opendir(dirname);
    if (dir == NULL) {
        LOGE("Open dir(%s) failed!!!", dirname);
        return -1;
    }

    while ((dirt = readdir(dir)) != NULL) {
       if (!strcmp(dirt->d_name, ".") || !strcmp(dirt->d_name, "..")) {
           continue;
       }

       isMatched = match(dirt, path);
       if (isMatched == 0) {
           snprintf(matchname, namelen, "%s/%s", dirname, dirt->d_name);
           break;
       }
    }

    return isMatched;
}

static int add_usb_to_volume_table(const char * devname) {
    blkid_cache cache = NULL;

    if (blkid_get_cache(&cache, NULL) < 0) {
        LOGE("blkid_get_cache failed!!!\n");
        return -1;
    }
    
    char *fstype = NULL;
    fstype = blkid_get_tag_value(cache, "TYPE", devname);
    if (fstype == NULL) {
        LOGE("Get block device fstype error\n");
        return -1;
    }

    char *uuid = NULL;
    uuid = blkid_get_tag_value(cache, "UUID", devname);
    if (uuid == NULL) {
        LOGE("Get block device uuid error\n ");
        return -1;
    }

    blkid_put_cache(cache);

    char mountpoint[256];
    memset(mountpoint, 0, sizeof(mountpoint));
    snprintf(mountpoint, sizeof(mountpoint), "%s/%s", USB_PATH, uuid);

    LOGI("device: %s, mountpoint: %s, fstype: %s\n", devname, mountpoint, fstype);
    if (volume_for_path(mountpoint) == NULL) {
/*        while (num_volumes >= alloc_volumes) {
            alloc_volumes *= 2;
            device_volumes = (Volume*)realloc(device_volumes, alloc_volumes*sizeof(Volume));
        }

        device_volumes[num_volumes].mount_point = strdup(mountpoint);
        device_volumes[num_volumes].fs_type = strdup(fstype);
        device_volumes[num_volumes].blk_device = strdup(devname);
        device_volumes[num_volumes].length = 0;
        ++num_volumes;*/
    int ret = fs_mgr_add_entry(fstab, mountpoint, fstype, devname, 0);
    if (0 != ret)
        LOGE("fs_mgr_add_entry :%s fail \n",mountpoint);
    LOGI("Mount %s on %s\n", devname, mountpoint);
    }
    
    free(fstype);
    free(uuid);
    return 0;
}

static int load_usb_to_volume_table(const char *path) {
    int i;
    char devname[256];
    memset(devname, 0, 256);

    // start with USB
    if (strncmp(path, USB_PATH, strlen(USB_PATH))) {
        LOGD("No-startsWith %s\n", USB_PATH);
        return 0;
    }
    int	wait_count = 0; 
    while(1){
    	if (match_spec_file(USB_DEV_DIR, path, match_usb_dev, 
                        devname, sizeof(devname))) {
		LOGE("MATCH FAILED!!!\n");
		sleep(5);
		wait_count ++;
	}
	else	break;	
	if(wait_count>=5)
		return -1;
    }

    if (add_usb_to_volume_table(devname)) {
        LOGE("Add usb device fstab error\n");
        return -1;
    }
 
    printf("current updated recovery filesystem table\n");
    printf("=========================================\n");
    for (i = 0; i < fstab->num_entries; ++i) {
	Volume* v = &fstab->recs[i];
        printf("  %d %s %s %s %lld\n", i, v->mount_point, v->fs_type,
               v->blk_device, v->length);
    }
    printf("\n");

    return 0;
}
// End of TCL
int ensure_path_mounted(const char* path) {
    // Begin added by TCL pengpeng, mail: pengpeng01@tcl.com
    if (load_usb_to_volume_table(path)) {
        LOGE("Load usb fail!!!\n");
        return -1;
    }
    // End of TCL

    Volume* v = volume_for_path(path);
    if (v == NULL) {
        LOGE("unknown volume for path [%s]\n", path);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // the ramdisk is always mounted.
        return 0;
    }

    int result;
    result = scan_mounted_volumes();
    if (result < 0) {
        LOGE("failed to scan mounted volumes\n");
        return -1;
    }

    const MountedVolume* mv =
        find_mounted_volume_by_mount_point(v->mount_point);
    if (mv) {
        // volume is already mounted
        return 0;
    }

    mkdir(v->mount_point, 0755);  // in case it doesn't already exist
    
	if (strcmp(v->mount_point, "/storage/sdcard")  == 0)
	{
		return ensure_usb_mounted(v, path);
	}
	
    if (strcmp(v->fs_type, "yaffs2") == 0) {
        // mount an MTD partition as a YAFFS2 filesystem.
        mtd_scan_partitions();
        const MtdPartition* partition;
        partition = mtd_find_partition_by_name(v->blk_device);
        if (partition == NULL) {
            LOGE("failed to find \"%s\" partition to mount at \"%s\"\n",
                 v->blk_device, v->mount_point);
            return -1;
        }
        return mtd_mount_partition(partition, v->mount_point, v->fs_type, 0);
    } else if (strcmp(v->fs_type, "ext4") == 0 ||
               strcmp(v->fs_type, "vfat") == 0) {
        result = mount(v->blk_device, v->mount_point, v->fs_type,
                       MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
        if (result == 0) return 0;

        LOGE("failed to mount %s (%s)\n", v->mount_point, strerror(errno));
        return -1;
    // Degin added by TCL pengpeng, mail: pengpeng01@tcl.com
    } else if (strcmp(v->fs_type, "ntfs") == 0) {
	LOGI("insmod /ntfs.ko\n");
	recovery_system("busybox insmod /ntfs.ko"); //add by pengpeng
	sleep(1);
        LOGI("ensure_path_mounted start to mount device=%s, mount_point=%s\n", v->blk_device, v->mount_point);
        result = mount(v->blk_device, v->mount_point, v->fs_type,
                       MS_NOATIME | MS_NODEV | MS_NODIRATIME, "");
        if (result == 0) {
            LOGI("ensure_path_mounted mount success\n");
            return 0;
        }

        LOGE("failed to mount %s (%s)\n", v->mount_point, strerror(errno));
        return -1;
    // End of TCL
    }

    LOGE("unknown fs_type \"%s\" for %s\n", v->fs_type, v->mount_point);
    return -1;
}

int ensure_path_unmounted(const char* path) {
    Volume* v = volume_for_path(path);
    if (v == NULL) {
        LOGE("unknown volume for path [%s]\n", path);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // the ramdisk is always mounted; you can't unmount it.
        return -1;
    }

    int result;
    result = scan_mounted_volumes();
    if (result < 0) {
        LOGE("failed to scan mounted volumes\n");
        return -1;
    }

    const MountedVolume* mv =
        find_mounted_volume_by_mount_point(v->mount_point);
    if (mv == NULL) {
        // volume is already unmounted
        return 0;
    }

    return unmount_mounted_volume(mv);
}

int format_volume(const char* volume) {
    Volume* v = volume_for_path(volume);
    if (v == NULL) {
        LOGE("unknown volume \"%s\"\n", volume);
        return -1;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // you can't format the ramdisk.
        LOGE("can't format_volume \"%s\"", volume);
        return -1;
    }
    if (strcmp(v->mount_point, volume) != 0) {
        LOGE("can't give path \"%s\" to format_volume\n", volume);
        return -1;
    }

    if (ensure_path_unmounted(volume) != 0) {
        LOGE("format_volume failed to unmount \"%s\"\n", v->mount_point);
        return -1;
    }

    if (strcmp(v->fs_type, "yaffs2") == 0 || strcmp(v->fs_type, "mtd") == 0) {
        mtd_scan_partitions();
        const MtdPartition* partition = mtd_find_partition_by_name(v->blk_device);
        if (partition == NULL) {
            LOGE("format_volume: no MTD partition \"%s\"\n", v->blk_device);
            return -1;
        }

        MtdWriteContext *write = mtd_write_partition(partition);
        if (write == NULL) {
            LOGW("format_volume: can't open MTD \"%s\"\n", v->blk_device);
            return -1;
        } else if (mtd_erase_blocks(write, -1) == (off_t) -1) {
            LOGW("format_volume: can't erase MTD \"%s\"\n", v->blk_device);
            mtd_write_close(write);
            return -1;
        } else if (mtd_write_close(write)) {
            LOGW("format_volume: can't close MTD \"%s\"\n", v->blk_device);
            return -1;
        }
        return 0;
    }

    if (strcmp(v->fs_type, "ext4") == 0) {
        int result = make_ext4fs(v->blk_device, v->length, volume, sehandle);
        if (result != 0) {
            LOGE("format_volume: make_extf4fs failed on %s\n", v->blk_device);
            return -1;
        }
        return 0;
    }

    LOGE("format_volume: fs_type \"%s\" unsupported\n", v->fs_type);
    return -1;
}

int setup_install_mounts() {
    if (fstab == NULL) {
        LOGE("can't set up install mounts: no fstab loaded\n");
        return -1;
    }
    for (int i = 0; i < fstab->num_entries; ++i) {
        Volume* v = fstab->recs + i;

        if (strcmp(v->mount_point, "/tmp") == 0 ||
            strcmp(v->mount_point, "/cache") == 0) {
            if (ensure_path_mounted(v->mount_point) != 0) return -1;

        } else {
            if (ensure_path_unmounted(v->mount_point) != 0) return -1;
        }
    }
    return 0;
}

// Degin added by TCL pengpeng, mail: pengpeng01@tcl.com
int recovery_system(const char * cmdstring)
{
    pid_t pid;
    int status;

    LOGI("cmdstring: \"%s\"\n" ,cmdstring);
    if (cmdstring == NULL) {
        return 1;
    }

    if ((pid = fork())<0) {
        status = -1;
    } else if (pid == 0) {
        execl("/sbin/busybox", "sh", "-c", cmdstring, (char *)0);
        exit(127);
    } else {
        while (waitpid(pid, &status, 0) < 0){
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
    }

    return status;
}

int storage_removed(const char* path) {
    sleep(1);
    if (strncmp(path, "/mnt/", strlen("/mnt/")) != 0) {
        return -1;
    }

    Volume* v = volume_for_path(path);
    if (access(v->blk_device, F_OK) == 0)  {
        return 1;
    }

    return 0;
}
//end of tcl
