#include <errno.h>
#include <sys/mount.h>
#include <cutils/log.h>
#include "ExtFs.h"

#define LOG_TAG "Vold_Extfs"

int Ext2::doMount(const char *fsPath, const char *mountPoint, bool ro, bool remount, bool executable){
	int rc;
	unsigned long flags;
	flags = MS_NOATIME | MS_NODEV | MS_NOSUID | MS_DIRSYNC;

	flags |= (executable ? 0 : MS_NOEXEC); 
    flags |= (ro ? MS_RDONLY : 0); 
    flags |= (remount ? MS_REMOUNT : 0);
	
	rc = mount(fsPath, mountPoint, "ext2", flags, NULL);

	if(rc && errno == EROFS){
		SLOGD("%s appears to be a read only filesystem -retrying mount RO", fsPath);
		flags |= MS_RDONLY;
		rc = mount(fsPath, mountPoint, "ext2", flags, NULL);
	}
	
	return rc;
}

int Ext2::format(const char *fsPath){
	SLOGD("Skipping ext2 filesystems check.\n");
	return 0;
}

int Ext3::doMount(const char *fsPath, const char *mountPoint, bool ro, bool remount, bool executable){
	int rc;
	unsigned long flags;
	flags = MS_NOATIME | MS_NODEV | MS_NOSUID | MS_DIRSYNC;

	flags |= (executable ? 0 : MS_NOEXEC); 
    flags |= (ro ? MS_RDONLY : 0); 
    flags |= (remount ? MS_REMOUNT : 0);
	
	rc = mount(fsPath, mountPoint, "ext3", flags, "barrier=1");

	if(rc && errno == EROFS){
		SLOGD("%s appears to be a read only filesystem -retrying mount RO", fsPath);
		flags |= MS_RDONLY;
		rc = mount(fsPath, mountPoint, "ext3", flags, "barrier=1");
	}
	
	return rc;
}

int Ext3::format(const char *fsPath){
	SLOGD("Skipping ext3 filesystems check.\n");
	return 0;
}
