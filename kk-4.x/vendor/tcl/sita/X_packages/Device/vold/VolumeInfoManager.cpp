#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/kdev_t.h>
#include <cutils/log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sysutils/NetlinkEvent.h>
#include "VolumeInfoManager.h"
#include "ResponseCode.h"
#include "VolumeManager.h"
#include "VolumeInfo.h"
#include "UmsVolume.h"

#define LOG_TAG "Vold_VolumeInfoManager"

VolumeInfoManager *VolumeInfoManager::sInstance = NULL;

VolumeInfoManager *VolumeInfoManager::Instance(){
	if(!sInstance){
		sInstance = new VolumeInfoManager();
	}
	return sInstance;
}

VolumeInfoManager::VolumeInfoManager(){
	mVolumeInfos = new VolumeInfoCollection();
	if(access(UMSDIR, F_OK) < 0){
		mkdir(UMSDIR, 0755);	
	}
}
int VolumeInfoManager::createDeviceNode(const char *path, int major, int minor){
	mode_t mode = 0660 | S_IFBLK;
	dev_t dev = (major << 8) | minor;
	if(mknod(path, mode, dev) < 0){
		if(errno != EEXIST){
			return -1;
		}
	}
	return 0;
} 

int VolumeInfoManager::handleNetlinkEvent(NetlinkEvent *evt){
	const char *devpath = evt->findParam("DEVPATH");
	int major = atoi(evt->findParam("MAJOR"));
	int minor = atoi(evt->findParam("MINOR"));
	int action = evt->getAction();

	/*only sdcard @vold.fstab and usb mass storage for handle */

	/* Patch Begin by Sarah Zhang(zxia@tcl.com) */
	/* ms628 uses TF card and major=179 and an emmc disk also has 179 major.
	   So cannot distinguish them by major. DEVNAME=mmcblk1 shows TF card.*/
//	if((!VolumeInfoManager::isConfigDevpath(devpath)) || (major == 8)){
	if((!strcmp(evt->findParam("DEVNAME"), "mmcblk1"))  || (major == 8)){
	/* Patch End by Sarah Zhang(zxia@tcl.com) */

		/*handle which has filesystem.*/
		if(!strcmp(evt->findParam("DEVTYPE"), "partition")\
            || (!strcmp(evt->findParam("DEVTYPE"), "disk") \
            && atoi(evt->findParam("NPARTS")) == 0)){
			if(action == NetlinkEvent::NlActionAdd){
				char nodepath[255];
				snprintf(nodepath, sizeof(nodepath), "/dev/block/vold/%d:%d", major,minor);
				if(createDeviceNode(nodepath, major, minor) < 0){
					SLOGE("Error making device node '%s' (%s)", nodepath, strerror(errno));
					errno = ENODEV;
					return -1;
				}
	
				VolumeInfo *vi = new VolumeInfo(nodepath, devpath);
				if(!vi->getVolumeFstype() && !vi->getVolumeUuid()){
					SLOGD("block device (%s) no filesystem\n", nodepath);
					delete vi;
					remove(nodepath);
					return -1;
				}

				addVolumeInfo(vi);
				if(!VolumeInfoManager::isConfigDevpath(devpath)){
					remove(nodepath);
					return 0;
				}

				char mountpoint[255];
				snprintf(mountpoint, sizeof(mountpoint), "%s/%s", UMSDIR, vi->getVolumeUuid());
				mkdir(mountpoint, 0755);
				VolumeManager *vm = VolumeManager::Instance();	
				if(!vm->lookupVolume(vi->getVolumeUuid())){
					UmsVolume *uv = new UmsVolume(vm, vi->getVolumeUuid(), mountpoint, devpath); 	
					vm->addVolume(uv);
				}
			}
		}
		
	return 0;
	}

	return -1;
}

int VolumeInfoManager::addVolumeInfo(VolumeInfo *vi){
	mVolumeInfos->push_back(vi);
	return 0;
}

int VolumeInfoManager::delVolumeInfo(const char *mountpoint){
	VolumeInfoCollection::iterator it;
	for(it = mVolumeInfos->begin(); it != mVolumeInfos->end(); it++){
		if(!strcmp((*it)->getVolumeMountpoint(),mountpoint)){
			if(*it)
				free(*it);
			mVolumeInfos->erase(it);
			break;	
		}
	}		

	return 0;
}

const char* VolumeInfoManager::getVolFstype(const char *mp){
	VolumeInfoCollection::iterator it;

	for(it = mVolumeInfos->begin(); it != mVolumeInfos->end(); it++){
		if(!strcmp(mp, (*it)->getVolumeMountpoint())){
			return (*it)->getVolumeFstype();
		}	
	}

	return NULL;
}

int VolumeInfoManager::VolumeInfoFstype(SocketClient *cli, const char *mp){
	if(getVolFstype(mp)){
		char *buffer;
		asprintf(&buffer, "%s", getVolFstype(mp));
		cli->sendMsg(ResponseCode::VolumeInfoFstypeResult, buffer, false);
		if(buffer){
			free(buffer);
		}
	}

	cli->sendMsg(ResponseCode::CommandOkay, "Volume info fstype.",false);
	return 0;
}

const char* VolumeInfoManager::getVolLabel(const char *mp){
	VolumeInfoCollection::iterator it;

	for(it = mVolumeInfos->begin(); it != mVolumeInfos->end(); it++){
		if(!strcmp(mp, (*it)->getVolumeMountpoint())){
			return (*it)->getVolumeLabel();
		}	
	}
	return NULL;
}

int VolumeInfoManager::VolumeInfoLabel(SocketClient *cli, const char *mp){
	if(getVolLabel(mp)){
		char *buffer;
		asprintf(&buffer, "%s", getVolLabel(mp));
		cli->sendMsg(ResponseCode::VolumeInfoLabelResult, buffer, false);

		if(buffer){
			free(buffer);
		}
	}
	cli->sendMsg(ResponseCode::CommandOkay, "Volume info label.",false);
	return 0;	
}

const char* VolumeInfoManager::getVolUuid(const char *mp){
	VolumeInfoCollection::iterator it;

	for(it = mVolumeInfos->begin(); it != mVolumeInfos->end(); it++){
		if(!strcmp(mp, (*it)->getVolumeMountpoint())){
			return (*it)->getVolumeUuid();
		}	
	}
	return NULL;
}

int VolumeInfoManager::VolumeInfoUuid(SocketClient *cli, const char *mp){
	if(getVolUuid(mp)){
		char *buffer;
		asprintf(&buffer, "%s", getVolUuid(mp));
		cli->sendMsg(ResponseCode::VolumeInfoUuidResult, buffer, false);
		if(buffer){
			free(buffer);
		}
	}
	cli->sendMsg(ResponseCode::CommandOkay, "Volume info uuid.",false);
	return 0;
}

/* Begin to modify by Sarah Zhang(zxia@tcl.com) */
const char* VolumeInfoManager::getVolDevpath(const char *mp){
	VolumeInfoCollection::iterator it;

	for(it = mVolumeInfos->begin(); it != mVolumeInfos->end(); it++){
		if(!strcmp(mp, (*it)->getVolumeMountpoint())){
			return (*it)->getVolumeDevpath();
		}
	}

	return NULL;
}

int VolumeInfoManager::VolumeInfoDevpath(SocketClient *cli, const char *mp){
	if(getVolDevpath(mp)){
		char *buffer;
		asprintf(&buffer, "%s", getVolDevpath(mp));
		cli->sendMsg(ResponseCode::VolumeInfoDevpathResult, buffer, false);
		if(buffer){
			free(buffer);
		}
	}

	cli->sendMsg(ResponseCode::CommandOkay, "Volume info devpath.",false);
	return 0;
}
/* End to modify by Sarah Zhang(zxia@tcl.com) */

const char* VolumeInfoManager::getMountpointViaDevpath(const char *dp){
	VolumeInfoCollection::iterator it;
	
	for(it = mVolumeInfos->begin(); it != mVolumeInfos->end(); it++){
		if(!strcmp(dp, (*it)->getVolumeDevpath())){
			return (*it)->getVolumeMountpoint();
		}
	}
	SLOGE("Could't find volume mountpoint via devpath\n");
	return NULL;
}

int VolumeInfoManager::isConfigDevpath(const char* dp){
	FILE *fp;
	int flag = -1;
	char line[255];
	if(!(fp = fopen("/etc/vold.fstab", "r"))){
		SLOGE("open vold.fstab failed.\n");
		return -1;
	}

	while(fgets(line, sizeof(line), fp)){
		int count = 5;
		char *buf, *tmp;
		if(line[0] == '#' || line[0] == '\0'){
			continue;
		}
		line[strlen(line)-1] = '\0';
		buf = line;
		while((tmp =strtok(buf, " \t")) != NULL){
			buf = NULL;
			count--;
			if((count <= 0) && !strncmp(tmp,dp, strlen(tmp))){
				flag = 0;
				break;	
			}
		}

		if(flag == 0){
			break;
		}
	}
	fclose(fp);
	return flag;
}

const char* VolumeInfoManager::getConfigMountpoint(const char *dp){
	FILE *fp;
	int flag = -1;
	char line[255];
	char *mp = NULL;
	
	if(!(fp = fopen("/etc/vold.fstab", "r"))){
		SLOGE("open vold.fstab failed.\n");
		return NULL;
	}

	while(fgets(line, sizeof(line), fp)){
		if(line[0] == '#' || line[0] == '\0'){
			continue;
		}
		line[strlen(line)-1] = '\0';
		int count = 5;
		char *buf, *tmp;
		buf = line;
		while((tmp = strtok(buf, " \t")) != NULL){	
			buf = NULL;
			count--;
			if(count == 2){
				mp = tmp;	
			}
			if(count <=0){
				flag = 0;
				break;
			}
		}

	  if(flag == 0){
		break;
	  }
	}
	
	fclose(fp);
	return mp;
}
