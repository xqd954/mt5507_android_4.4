#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <cutils/log.h>
#include "Volume.h"
#include "UmsVolume.h"
#include "VolumeInfoManager.h"
#include "ResponseCode.h"

#define LOG_TAG  "Vold_UmsVolume"

UmsVolume::UmsVolume(VolumeManager *vm,const char *label, const char *mountpoint, const char *dp) : Volume(vm, label, mountpoint){
	major = -1;
	minor = -1;
	if (dp)
		devpath = strdup(dp);
	else {
		devpath = NULL;
		SLOGE("Sarah Warning! UmsVolume:strdup(dp), dp=NULL\n");
	}

	setState(Volume::State_NoMedia);
}

UmsVolume::~UmsVolume(){
	if (devpath)
		free(devpath);
	else
		SLOGE("Sarah Warning:UmsVolume free(devpath) parameter NULL\n");
}

int UmsVolume::handleBlockEvent(NetlinkEvent *evt){
//	SLOGD("handleBlockEvent NetlinkEvent devpath:%s, UmsVolume devpath:%s\n", evt->findParam("DEVPATH"), devpath);
	VolumeInfoManager *vim = VolumeInfoManager::Instance();
	const char *mountpoint = vim->getMountpointViaDevpath(evt->findParam("DEVPATH"));
	if(!mountpoint){	
		SLOGD("devpath may be incorrect,devpath:%s", devpath);
		errno = ENODEV;
		return -1;
	}

	if(!strcmp(mountpoint, getMountpoint())){
		int action = evt->getAction();
		major = atoi(evt->findParam("MAJOR"));
		minor = atoi(evt->findParam("MINOR"));
		if(action == NetlinkEvent::NlActionAdd){
			handleVolumeAdded();
		}else if(action == NetlinkEvent::NlActionRemove){
			handleVolumeRemoved();
		}else if(action == NetlinkEvent::NlActionChange){
			handleVolumeChanged();
		}else{
			SLOGW("Ignoring non add/remove/change event\n");
		}
		return 0;
	}else{
		SLOGD("devpath may be incorrect,devpath:%s", devpath);
		errno = ENODEV;
		return -1;
	}
}

void UmsVolume::handleVolumeAdded(){
	char msg[255];
	
	setState(Volume::State_Idle);
	snprintf(msg,sizeof(msg), "Volume %s %s disk inserted (%d:%d)", getLabel(), getMountpoint(), major, minor);
	mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskInserted, msg, false);
}

void UmsVolume::handleVolumeRemoved(){
	char msg[255];	
	char devnode[255];
	int state;
	state = getState();

	if(state != Volume::State_Mounted){
		SLOGD("Volume %s %s disk %d:%d removed\n", getLabel(), getMountpoint(), major, minor);
		snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)", getLabel(), getMountpoint(), major, minor);
		mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved, msg, false);
	}else{
		SLOGD("Volume %s %s disk %d:%d bad removed\n", getLabel(), getMountpoint(), major, minor);
		snprintf(msg, sizeof(msg), "Volume %s %s bad removal (%d:%d)", getLabel(), getMountpoint(), major, minor);
		mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeBadRemoval, msg, false);
		if(Volume::unmountVol(true, false)){
			SLOGE("Failed to unmount volume on bad removal (%s)", strerror(errno));
		}else{
			SLOGD("Crisis averted");
		}
	}
		VolumeInfoManager *vim = VolumeInfoManager::Instance();
		vim->delVolumeInfo(getMountpoint());
		snprintf(devnode, sizeof(devnode), "/dev/block/vold/%d:%d", major, minor);	
		remove(devnode);
		remove(getMountpoint());
		setState(Volume::State_NoMedia);
}

void UmsVolume::handleVolumeChanged(){
	SLOGD("Volume %s %s volume %d:%d changed\n", getLabel(), getMountpoint(), major, minor);	
}

int UmsVolume::getDeviceNodes(dev_t *devs, int max){
	devs[0] = MKDEV(major, minor);
	return 1;	
}

void UmsVolume::handleVolumeShared(){
	setState(Volume::State_Shared);
}

void UmsVolume::handleVolumeUnshared(){
	setState(Volume::State_Idle);
}

