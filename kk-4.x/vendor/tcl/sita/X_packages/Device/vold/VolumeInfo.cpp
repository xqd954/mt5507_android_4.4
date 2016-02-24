#include <cutils/log.h>
#include <blkid/blkid.h>
#include <unicode/utypes.h>
#include <unicode/ucsdet.h>
#include <unicode/ucnv.h>
#include "VolumeInfo.h"
#include "VolumeInfoManager.h"
#define LOG_TAG "Vold_VolumeInfo"

VolumeInfo::VolumeInfo(const char *nodepath, const char* dp){
	if (nodepath)
		devname = strdup(nodepath);
	else {
		devname = NULL;
		SLOGE("Sarah Warning!VolumeInfo: nodepath=NULL\n");
	}
	if (dp)
		devpath = strdup(dp);
	else {
		devpath = NULL;
		SLOGE("Sarah Warning! VolumeInfo: dp=NULL\n");
	}
	mountpoint = NULL;
	label = NULL;
	fstype = NULL;
	uuid = NULL;
	blkid_cache cache = NULL;
	char *tmp;

	if(blkid_get_cache(&cache, NULL) < 0){
		SLOGE("Couldn't get blkid cache\n");
	}else{
		tmp = blkid_get_tag_value(cache, "TYPE", devname);
		if (tmp) {
			fstype = strdup(tmp);
			tmp = blkid_get_tag_value(cache, "UUID",devname);
			if (tmp)
				uuid = strdup(tmp);
			else {
				uuid = strdup("test");
				SLOGE("SARAH: Warning!! uuid is NULL!!! Just workround to give %s to uuid \n", uuid);
			}
			SLOGD("SARAH: uuid=%s\n", uuid);
			tmp = blkid_get_tag_value(cache, "LABEL",devname);
			if (tmp) {
				const char *encodings;
				char source[255], target[255];
				UErrorCode status = U_ZERO_ERROR; 
				snprintf(source, sizeof(source), "%s", tmp);
				if(!strncmp(fstype, "ntfs", 4)){	
					int count = strlen(source);
					for(int i = 0; i < count; i+=2){
						if(source[i] == 0x18){
							source[i] = 0x00;
						}
					}
					ucnv_convert("utf-8", "ucs-2", target, sizeof(target), source, count, &status);
					label = strndup(target, sizeof(target));
				}else if(!strncmp(fstype, "vfat", 4)){
					ucnv_convert("utf-8", "GB18030", target, sizeof(target), source, strlen(source), &status);
					label = strndup(target, sizeof(target));
				}else{
					label = strndup(source, sizeof(source));
				}
			}else{
			//	label = strdup("newVolume");
				SLOGD("VolumeInfo: LABEL is NULL\n");
			}

			if(!VolumeInfoManager::isConfigDevpath(dp)){
				if(VolumeInfoManager::getConfigMountpoint(dp))
					mountpoint = strdup(VolumeInfoManager::getConfigMountpoint(dp));
				else {
					mountpoint = NULL;
					SLOGE("SARAH Warning!getConfigMountpoint is NULL\n ");
				}
			}else{
				asprintf(&mountpoint,"%s/%s", UMSDIR, uuid);
			}

			SLOGD("fstype:%s, label:%s, uuid:%s, mountpoint:%s, devpath:%s\n", fstype, label, uuid, mountpoint, devpath);
		}

		blkid_put_cache(cache);
	}
}

VolumeInfo::~VolumeInfo(){
	if(uuid){
		free(uuid);
	}
	if(fstype){
		free(fstype);
	}
	if(label){
		free(label);
	}
	if(mountpoint){
		free(mountpoint);
	}
	if(devname){
		free(devname);
	}
	if(devpath){
		free(devpath);
	}
}

const char* VolumeInfo::getVolumeLabel(){
	return label;
}

const char* VolumeInfo::getVolumeFstype(){
	return fstype;
}

const char* VolumeInfo::getVolumeUuid(){
	return uuid;
}
const char* VolumeInfo::getVolumeMountpoint(){
	return mountpoint;
}
const char* VolumeInfo::getVolumeDevpath(){
	return devpath;
}

const char* VolumeInfo::detectCharset(const char *str, int len){
	int conf;
    const char *name;
    UErrorCode status = U_ZERO_ERROR;

    UCharsetDetector *csd = ucsdet_open(&status);
    const UCharsetMatch *match;

    ucsdet_setText(csd, str, len, &status);
    match = ucsdet_detect(csd, &status);
    if(match == NULL){
        SLOGE("Encoding detection failure.\n");
        ucsdet_close(csd);
        return NULL;
    }   

    name = ucsdet_getName(match, &status);
    conf = ucsdet_getConfidence(match, &status);
    
    if(conf != 100){
        SLOGD("did not get 100%% confidence, value:%d\n", conf);
    }   

    ucsdet_close(csd);
	SLOGD("encoding name: %s\n", name);
    if(conf > 0){
        return name; 
    }else{
        SLOGD("conf:%d is so low.\n", conf);
        return NULL;
    }   

}
