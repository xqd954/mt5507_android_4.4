/*
 * Copyright 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <regex.h>
#include <time.h>

#include <linux/kdev_t.h>

#include <cutils/log.h>
#include <sysutils/NetlinkEvent.h>
#include <cutils/properties.h>

#include "Process.h"
#include "ExternalVolume.h"
#include "ResponseCode.h"

#include "Fat.h"

//#define DEFAULT_MOUNT_POINT "/mnt/sdcard"
//#define DEFAULT_MOUNT_POINT "/storage/sdcard"
#define  DEFAULT_MOUNT_POINT "/mnt/media_rw/sdcard"

#define DIR_SECURE_ASEC "/mnt/secure/asec"

#define EXFAT_PATH     "/sbin/mount.exfat"

#define LOG_TAG "ExternalVolume"

static void* vold_open_device_thrd(void *pvdata);

bool ExternalVolume::bDefaultMounted = false;

ExternalVolume::ExternalVolume(VolumeManager *vm, const fstab_rec* rec, int flags) :
        Volume(vm, rec, flags) 
{
    mDebug = false;
    mVm = vm;

    mExVolumes = new ExternalVolumeCollection();
    mDevices = new DeviceCollection();
    mPaths = new PathCollection();
    mExVolIterator = new ExVolIterator();
    mLoops = new LoopCollection();
    //mLoops->push_back(strdup("loop0")); //reserved for apk intalled in sdcard
    mLoops->push_back(strdup("loop1"));
    mLoops->push_back(strdup("loop2"));
    mLoops->push_back(strdup("loop3"));
    mLoops->push_back(strdup("loop4"));
    mLoops->push_back(strdup("loop5"));
    mLoops->push_back(strdup("loop6"));
    mLoops->push_back(strdup("loop7")); 

    mStartNotify = false;

    mDiskMajor = -1;
    mDiskMinor = -1;

    err = pthread_mutex_init(&dev_lock, NULL);     
    if(err != 0)
    {
        SLOGE("Thread mutex lock initialization failed:(%d)\n.", err);
    }

    err = pthread_create(&ntid, NULL, &vold_open_device_thrd, this);
    if(err != 0)
    {
        SLOGE("vold_open_device_thrd thread creates fail:(%d)\n.", err);
    }

    char mount[PATH_MAX];

    snprintf(mount, PATH_MAX, "%s/%s", Volume::MEDIA_DIR, rec->label);
    mMountpoint = strdup(mount);
    snprintf(mount, PATH_MAX, "%s/%s", Volume::FUSE_DIR, rec->label);
    mFuseMountpoint = strdup(mount);
    
    setState(Volume::State_NoMedia);
}

ExternalVolume::~ExternalVolume() {
    ExternalVolumeCollection::iterator it;

    for (it = mExVolumes->begin(); it != mExVolumes->end(); ++it)
    {
        delete (*it);
    }
    delete mExVolumes;

    LoopCollection::iterator lpit;
     for(lpit = mLoops->begin(); lpit != mLoops->end(); lpit++)
    {
        delete (*lpit);        
    }
    delete mLoops;
    
    DeviceCollection::iterator dvcit;
     for(dvcit = mDevices->begin(); dvcit != mDevices->end(); dvcit++)
    {
       delete (*dvcit);   
    }
    delete mDevices;

    PathCollection::iterator pthit;
    for(pthit = mPaths->begin(); pthit != mPaths->end(); pthit++)
    {
        delete (*pthit);
    }
    delete mPaths;

    pthread_mutex_destroy(&dev_lock);
}

int ExternalVolume::addPath(const char *path) 
{
    mPaths->push_back(strdup(path));
    return 0;
}

int ExternalVolume::handleWifiProduct(NetlinkEvent *evt)
{
    //SLOGD("handleWifiProduct.\n");
    const char *productName = evt->findParam("PRODUCT");
    const char *interfacePara = evt->findParam("INTERFACE");
    if(productName == NULL || interfacePara == NULL)
    {
        SLOGD("handleWifiProduct, return -1.\n");
        return -1;
    }

    int action = evt->getAction();
    
    if (action == NetlinkEvent::NlActionAdd) 
    {
          SLOGD("handleProductEvent, NetlinkEvent::NlActionAdd\n");
          if(mStartNotify)
          {
              //SLOGD("handleWifiProduct, mStartNotify=true.\n");
              //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceProductConnected, (const void*)(strdup(productName))));
              char msg[255];
              memset(msg, 0, 255);
      
              snprintf(msg, sizeof(msg), "Product \"%s \"add", productName);
              mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceProductConnected,
                                                       msg, false);
          }
          Device* dev = new Device();
          char product[32];
          memset(product, 0, 32);
          snprintf(product, sizeof(product), "PROD_%s", productName);
          dev->mDeviceName = strdup(product);
          dev->mMajor = 0;
          dev->mMinor = 0;
          dev->mStatus = Device::DeviceProductConnected;
          pthread_mutex_lock(&dev_lock);
          mDevices->push_back(dev);
          pthread_mutex_unlock(&dev_lock);
    } 
    else if(action == NetlinkEvent::NlActionRemove)
    {
          SLOGD("handleWifiProduct, NetlinkEvent::NlActionRemove\n");
          if(mStartNotify)
          { 
              //SLOGD("handleWifiProduct, mStartNotify=true.\n");
              //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceProductDisconnected, (const void*)(strdup(productName))));
              char msg[255];
              memset(msg, 0, 255);
      
              snprintf(msg, sizeof(msg), "Product \"%s \"remove", productName);
              mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceProductDisconnected,
                                                       msg, false);
          }
          char product[32];
          memset(product, 0, 32);
          snprintf(product, sizeof(product), "PROD_%s", productName);
          DeviceCollection::iterator it;
          pthread_mutex_lock(&dev_lock);
          for(it = mDevices->begin(); it != mDevices->end(); it++)
          {
              if(strcmp(product, (*it)->mDeviceName) == 0)
              {          
                  Device* dev = *it;
                  mDevices->erase(it);
                  delete dev;
                  break;
              }
          }
          pthread_mutex_unlock(&dev_lock);
    }
    return 0;
}

int ExternalVolume::handleWifiEvent(NetlinkEvent *evt)
{
    //SLOGD("handleWifiEvent.\n");
    const char *wifiInterf = evt->findParam("INTERFACE");
    if(wifiInterf == NULL)
    {
        SLOGD("handleWifiEvent, return -1.\n");
        return -1;
    }

    int action = evt->getAction();
    
    if (action == NetlinkEvent::NlActionAdd) 
    {
          SLOGD("handleWifiEvent, NetlinkEvent::NlActionAdd\n");
          if(mStartNotify)
          {
              //SLOGD("handleWifiEvent, mStartNotify=true.\n");
              //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceWifiConnected, (const void*)(strdup(wifiInterf))));
              char msg[255];
              memset(msg, 0, 255);
      
              snprintf(msg, sizeof(msg), "Wifi \"%s \"add", wifiInterf);
              mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceWifiConnected,
                                                       msg, false);
          }
          Device* dev = new Device();
          char wifiname[32];
          memset(wifiname, 0, 32);
          snprintf(wifiname, sizeof(wifiname), "WIFI_%s", wifiInterf);
          dev->mDeviceName = strdup(wifiname);
          dev->mMajor = 0;
          dev->mMinor = 0;
          dev->mStatus = Device::DeviceConnected;
          pthread_mutex_lock(&dev_lock);
          mDevices->push_back(dev);
          pthread_mutex_unlock(&dev_lock);
    } 
    else if(action == NetlinkEvent::NlActionRemove)
    {
          SLOGD("handleWifiEvent, NetlinkEvent::NlActionRemove\n");
          if(mStartNotify)
          { 
              //SLOGD("handleWifiEvent, mStartNotify=true.\n");
              //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceWifiDisconnected, (const void*)(strdup(wifiInterf))));
              char msg[255];
              memset(msg, 0, 255);
      
              snprintf(msg, sizeof(msg), "WIFI \"%s \"remove", wifiInterf);
              mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceWifiDisconnected,
                                                       msg, false);
          }
          char wifiname[32];
          memset(wifiname, 0, 32);
          snprintf(wifiname, sizeof(wifiname), "WIFI_%s", wifiInterf);
          DeviceCollection::iterator it;
          pthread_mutex_lock(&dev_lock);
          for(it = mDevices->begin(); it != mDevices->end(); it++)
          {
              if(strcmp(wifiname, (*it)->mDeviceName) == 0)
              {          
                  Device* dev = *it;
                  mDevices->erase(it);
                  delete dev;
                  break;
              }
          }
          pthread_mutex_unlock(&dev_lock);
    }
    return 0;
}

int ExternalVolume::handleSdcardMountVol()
{
    ExternalVolumeCollection::iterator it;

	for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
	{
	    if(0 == strcmp(getFuseMountpoint(), (*it)->mMountPoint) || (*it)->mMulMounted == true)
	  	{
            return -1; //sdcard exist
	  	}
	}
    //sdcard not exist

    char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
    if(getDevNode() == NULL) return 0;
    sprintf(devpath, "%s%s", "/dev/block/vold/", getDevNode());

    FS_TYPE_E e_fs_type = FS_TYPE_INVAL;
    int ret = 0;

    ret = mountVolume(devpath, "/mnt/secure/staging", &e_fs_type);   
    if(ret != 0)
    {
        SLOGE("mount volume %s to %s fail\n", devpath, "/mnt/secure/staging");
        return 0;
    }

    ExternalVolumeInfo* exVolumeInfo = new ExternalVolumeInfo();
    if(exVolumeInfo == NULL)
    {
        SLOGE("New external volume information structure fail\n");
        umountVolume("/mnt/secure/staging");
        return 0;
    }

    exVolumeInfo->mMountPoint = strdup(getFuseMountpoint());
    exVolumeInfo->mDeviceName = strdup(getDevNode());
    exVolumeInfo->mVolumeStatus = ExternalVolumeInfo::VolumeMounted;
    exVolumeInfo->mVolumeMajor = mDiskMajor;
    exVolumeInfo->mVolumeMinor = mDiskMinor;
    exVolumeInfo->mFileSystemType = e_fs_type;

    struct statfs fsbuf;
    ret = statfs("/mnt/secure/staging", &fsbuf);

    if(ret != 0)
    {
        exVolumeInfo->mVolumeTotalSize = 0;
        exVolumeInfo->mVolumeFreeSize = 0;
    }
    else
    {
        long long tmp = 0;
        tmp = fsbuf.f_blocks * fsbuf.f_bsize;
        exVolumeInfo->mVolumeTotalSize = tmp / 1024;
        tmp = fsbuf.f_bavail * fsbuf.f_bsize;
        exVolumeInfo->mVolumeFreeSize = tmp / 1024;
    }
        
    char buf[32];
    memset(buf, 0, 32);
    ret = getVolumeLabel(exVolumeInfo->mDeviceName, 32, buf);
    if(ret != 0)
    {
        exVolumeInfo->mVolumeLabel = strdup("RemovableDevice");
    }
    else
    {
        if(strlen(buf) == 0)
        {
            exVolumeInfo->mVolumeLabel = strdup("RemovableDevice");
        }
        else
        {
            exVolumeInfo->mVolumeLabel = strdup(buf);
        }
    }
    mExVolumes->push_back(exVolumeInfo);

    setState(Volume::State_Idle);
    setDevNode(exVolumeInfo->mDeviceName);

    char msg[255];
    memset(msg, 0, 255);

    snprintf(msg, sizeof(msg), "Volume %s %s disk inserted (%d:%d)",
                 getLabel(), getFuseMountpoint(), mDiskMajor, mDiskMinor);
    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskInserted,
                                             msg, false);
    ExternalVolume::bDefaultMounted = true;
    
    if(mStartNotify)
    {
        char msg[255];
        memset(msg, 0, 255);

        snprintf(msg, sizeof(msg), "Volume \"%ld \"%ld \"%d \"%d \"%d \"%s \"%s \"%s \"%d \"add", exVolumeInfo->mVolumeTotalSize, 
                                  exVolumeInfo->mVolumeFreeSize, exVolumeInfo->mVolumeMajor, exVolumeInfo->mVolumeMinor,
                                  exVolumeInfo->mVolumeStatus, exVolumeInfo->mMountPoint, exVolumeInfo->mDeviceName,
                                  exVolumeInfo->mVolumeLabel, exVolumeInfo->mFileSystemType);
        mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeMounted,
                                               msg, false);  
    }

    return 0;
}

int ExternalVolume::handleBlockEvent(NetlinkEvent *evt) 
{
    const char *dp = evt->findParam("PHYSDEVPATH");
    if(dp == NULL)
    {
        dp = evt->findParam("DEVPATH");
        if(dp == NULL)
        {
            return -1;
        }
    }
	
	regex_t re;
    PathCollection::iterator  it;
    
    for (it = mPaths->begin(); it != mPaths->end(); ++it) 
    {
		if( regcomp(&re, *it, REG_EXTENDED|REG_NOSUB) != 0 ) 
        {
            regfree(&re);
			continue;
		}
		
		if(regexec(&re, dp, (size_t) 0, NULL, 0) == 0) 
        {     
            int action = evt->getAction();
            const char *devtype = evt->findParam("DEVTYPE");
            
            if (action == NetlinkEvent::NlActionAdd) 
            {
                if (!strcmp(devtype, "disk")) 
                {
                    handleDiskAdded(evt);
                } 
                else 
                {
                    handlePartitionAdded(evt);
                }
            } 
            else if (action == NetlinkEvent::NlActionRemove) 
            {
                if (!strcmp(devtype, "disk")) 
                {
                    handleDiskRemoved(evt);
                } 
                else 
                {
                    handlePartitionRemoved(evt);
                }
            } 
            else if (action == NetlinkEvent::NlActionChange) 
            {
                if (!strcmp(devtype, "disk")) 
                {
                    if(mDebug)SLOGD("handleDiskChanged.\n");
                    handleDiskChanged(evt);
                } 
                else 
                {
                    if(mDebug)SLOGD("handlePatitionChanged.\n");
                    handlePartitionChanged(evt);
                }
            } 
            else 
            {
                SLOGW("Ignoring non add/remove/change event");
            }
		}
        regfree(&re);
    }

    return 0;
}

#ifndef ANDROID_DEFAULT_CODE
//2012/8/6,added by Daniel Hsiao,for USB Audio detection support
int ExternalVolume::handleSoundEvent(NetlinkEvent *evt) 
{
    int action = evt->getAction();
    const char *devname = evt->findParam("DEVNAME");
    int card = devname[8]-'0';
    int device = devname[10]-'0';
    char msg[255];
    
    memset(msg, 0, 255);
    //snprintf(msg, sizeof(msg),"card=%d;device=%d", card, device);
    snprintf(msg, sizeof(msg),"%d %d", card, device);

    if (action == NetlinkEvent::NlActionAdd) 
    {
        SLOGI("Sound Add msg=%s", msg);
        mVm->getBroadcaster()->sendBroadcast(ResponseCode::SoundCardInserted, msg, false);
    }
    else if (action == NetlinkEvent::NlActionRemove)
    {
        SLOGI("Sound Remove msg=%s", msg);
        mVm->getBroadcaster()->sendBroadcast(ResponseCode::SoundCardRemoved, msg, false);
    }
    return 0;
}
#endif

bool ExternalVolume::doRemove(ExternalVolumeInfo* exV)
{
    const char* mntpath = NULL;
	int ret = 0;
	int err = 0;
	bool isDelMul = false;
	
    mntpath = exV->mMountPoint;
    
    if(0 == strcmp(getFuseMountpoint(), mntpath))
    {
         ExternalVolume::bDefaultMounted = false;
         setState(Volume::State_Unmounting);

         usleep(1000 * 1000); // Give the framework some time to react

         char service[64];
         snprintf(service, 64, "fuse_%s", getLabel());
         property_set("ctl.stop", service);
         /* Give it a chance to stop.  I wish we had a synchronous way to determine this... */
         sleep(1);
         
         ret = umountVolume(DIR_SECURE_ASEC);
         if(ret == 0)
         {
            int major = exV->mVolumeMajor;
            int minor = exV->mVolumeMinor;

        	char msg[255];
            memset(msg, 0, 255);

            snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)",
                         getLabel(), getFuseMountpoint(), major, minor);
            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                                     msg, false);
         }
    }
    else
    {
            int major = exV->mVolumeMajor;
            int minor = exV->mVolumeMinor;
            char msg[255] = {0};                
            char service[64] = {0};
            
            snprintf(service, 64, "fuse_%s", exV->mDeviceName);
            property_set("ctl.stop", service);


            snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)",
                         "usbdisk", exV->mMountPoint, major, minor);
            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                                     msg, false);
    }

	ret = umountVolume(exV->mMountPoint);
	if(0 == strcmp(getFuseMountpoint(), mntpath))
	{
		setState(Volume::State_Idle);
	}
	if(0 != ret)
	{
		SLOGI("do Just Remove first umount fail, ret =: %d\n", ret);
		exV->mVolumeStatus = ExternalVolumeInfo::VolumeRemoved;
		return false;
	}                   
    if(false)
    {
        char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
        sprintf(devpath, "/dev/block/vold/%s", exV->mDeviceName);
        unlink(devpath);     
    }
    mExVolIterator->push_back(exV->mMountPoint);  
    inGCExternalVol();
    
    return true;
}

int ExternalVolume::umountVolumeCmd(const char *label, bool force, bool revert)
{
    bool umounted = true;
    const char* mntpath = NULL;
    ExternalVolumeCollection::iterator it;
    
    if(label == NULL)
    {
        SLOGI("umountVolumeCmd: Label is NULL\n");
    }
    
	for(it = mExVolumes->begin(); it != mExVolumes->end();it++)
	{
	  if(0 == strcmp(label, (*it)->mMountPoint))
	  	{
    	  	ExternalVolumeInfo* info = *it;           
            mntpath = info->mMountPoint;            
            if(mStartNotify)
            {
    			char msg[255];
                memset(msg, 0, 255);
    
                snprintf(msg, sizeof(msg), "Volume \"%s \"remove", mntpath);
                mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeUmounted,
                                                       msg, false);  
            }            
            //umount related iso mountpoint first.
            inISOPreDoUmount(it);  
            //end. 
            
			SLOGI("do Remove, volums size: %d\n", mExVolumes->size());
			umounted = doRemove(info);
            
		    break;
	  	}
	}

    if(umounted)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


void ExternalVolume::handleDiskAdded(NetlinkEvent *evt)
{
    Device* dev = new Device();
    dev->mDeviceName = strdup(evt->findParam("DEVNAME"));
    dev->mMajor = atoi(evt->findParam("MAJOR"));
    dev->mMinor = atoi(evt->findParam("MINOR"));
    dev->mStatus = Device::DeviceConnected;
    pthread_mutex_lock(&dev_lock);
    mDevices->push_back(dev);

    mDiskMajor = atoi(evt->findParam("MAJOR"));
    mDiskMinor = atoi(evt->findParam("MINOR"));

    pthread_mutex_unlock(&dev_lock);
    if(mStartNotify)
    {
        //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceConnected, dev));
        char msg[255];
        memset(msg, 0, 255);

        snprintf(msg, sizeof(msg), "Device \"%d \"%d \"%d \"%s \"add", dev->mMajor, dev->mMinor,
                                  dev->mStatus, dev->mDeviceName);
        mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceConnected,
                                               msg, false);  
    }
    if(mDebug) {
        printDevInfo();
    }
    
    handleAdded(evt);

    #if 0
    char msg[128];
    memset(msg, 0, 128);
    sprintf(msg, "External Volume add disk %s null %s %s", evt->findParam("DEVNAME"), evt->findParam("MAJOR"), evt->findParam("MINOR"));
    mVm->getBroadcaster()->sendBroadcast(msg);
    SLOGI("send message: %s\n", msg);
    #endif
    
    return;
}

void ExternalVolume::handlePartitionAdded(NetlinkEvent *evt)
{
    handleAdded(evt);
    return;
}


void ExternalVolume::handleDiskChanged(NetlinkEvent *evt)
{
    SLOGI("Enter %s\n", __FUNCTION__);

    const char* devName = evt->findParam("DEVNAME");
    const char *devPath = evt->findParam("PHYSDEVPATH");
    char path[256] = {0};
    int  fd;
    bool b_found = true;
    ExternalVolumeCollection::iterator it;

    if(devName == NULL)
    {
        SLOGI("%s@%d  DEVNAME is NULL, return!!\n", __FUNCTION__, __LINE__);
        return;
    }
    if(devPath == NULL)
    {
        devPath = evt->findParam("DEVPATH");
        if(devPath == NULL)
        {
            SLOGI("%s@%d  DEVPATH is NULL, return!!\n", __FUNCTION__, __LINE__);
            return;
        }
    }
   SLOGI("DevName@%s, DevPath@%s\n", devName, devPath);

   sprintf(path, "/dev/block/%s1", devName);                    
   
   fd = open(path, O_RDONLY);

   if (fd >= 0)
   {
      close(fd); 
      for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
	  {
    	  if(0 == strncmp(devName, (*it)->mDeviceName, 3))
          {
               SLOGI("Find partition already exist, will write remove\n");
               b_found = true;
               memset(path, 0, 256);
               sprintf(path, "/sys/%s/%s1/uevent", devPath, devName);  
               SLOGI("Uevent Path@ %s\n", path);
               fd = open(path, O_WRONLY);
               if(fd >= 0)
               {
                   SLOGI("write remove \n");
                   write(fd, "remove\n", 7);
                   close(fd);
               }
               else
               {
                   SLOGI("open uevent fail\n");
               }

               break;
           }
      }
      
      if(!b_found)
      {
          SLOGI("Find partitioin not exist, will write add\n");
          memset(path, 0, 256);
          sprintf(path, "/sys/%s/%s1/uevent", devPath, devName);  
          SLOGI("Uevent Path@ %s\n", path);
          fd = open(path, O_WRONLY);
          if(fd >= 0)
          {
              SLOGI("write add \n");
              write(fd, "add\n", 4);
              close(fd);
          }
          else
          {
              SLOGI("open uevent fail\n");
          }
      } 
   }
}

void ExternalVolume::handlePartitionChanged(NetlinkEvent * evt)
{
}


void ExternalVolume::handleDiskRemoved(NetlinkEvent *evt)
{
    char* devname = strdup(evt->findParam("DEVNAME"));
    DeviceCollection::iterator it;
    pthread_mutex_lock(&dev_lock);
    for(it = mDevices->begin(); it != mDevices->end(); it++)
    {
        if(strcmp(devname, (*it)->mDeviceName) == 0)
        {          
            Device* dev = *it;
            mDevices->erase(it);
            delete dev;

            if(mStartNotify)
            {
                //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceDisconnected, (const void*)(strdup(devname))));
                char msg[255];
                memset(msg, 0, 255);
   
                snprintf(msg, sizeof(msg), "Device \"%s \"remove", devname);
                mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceDisconnected,
                                                       msg, false);  
                free(devname);
            }
            
            break;
        }
    }
    pthread_mutex_unlock(&dev_lock);
    if(mDebug)printDevInfo();
    
    handleRemoved(evt);
    #if 0
    char msg[128];
    memset(msg, 0, 128);
    sprintf(msg, "External Volume remove disk %s null %s %s", evt->findParam("DEVNAME"), evt->findParam("MAJOR"), evt->findParam("MINOR"));
    mVm->getBroadcaster()->sendBroadcast(msg);
    SLOGI("send message: %s\n", msg);
    #endif
    return;
}

void ExternalVolume::handlePartitionRemoved(NetlinkEvent *evt)
{
    handleRemoved(evt);
    return;    
}


void ExternalVolume::handleAdded(NetlinkEvent *evt)
{
    if(evt == NULL)
    {
        SLOGE("event is not exist\n");
        return;
    }
    
    int major = atoi(evt->findParam("MAJOR"));
    int minor = atoi(evt->findParam("MINOR"));
	const char* devname = evt->findParam("DEVNAME");

    char mntpath[ExternalVolume::MAX_MNT_PATH_LEN] = {0};
    char fusepath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
    char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
    
    /*Add filter for emmc*/
    ExclusiveMountName::iterator it;
    if(mDebug)
    {
        SLOGD("Ignore partition size:= %d.\n", mVm->mExclusiveMountStrs->size());
    }
    for(it = mVm->mExclusiveMountStrs->begin(); it != mVm->mExclusiveMountStrs->end(); it++)
    {
        if(!strcmp(devname, *it))
        {            
            SLOGI("Ignore EMMC partition: %s\n", *it); 
            return;
        }
    }   

    if(!ExternalVolume::bDefaultMounted)
    {
        strcpy(mntpath, getMountpoint());
        strcpy(fusepath, getFuseMountpoint());
        
    }
    else
    {
        sprintf(mntpath, "%s%s%s", Volume::MEDIA_DIR, "/", evt->findParam("DEVNAME"));
        sprintf(fusepath, "%s%s%s", Volume::FUSE_DIR, "/", evt->findParam("DEVNAME"));
    }
    
    sprintf(devpath, "%s%s", "/dev/block/vold/", evt->findParam("DEVNAME"));
	//sprintf(devpath, "/dev/block/vold/%d:%d", major, minor);

    int ret = 0;
    SLOGI("create device node %s\n", devpath);
    ret = createDeviceNode(devpath, major, minor);
    if(ret != 0)
    {
        SLOGE("create device node %s fail\n", devpath);
        return;
    }

    ret = mkdir(mntpath, 0755);
    if(ret != 0 && errno != EEXIST)
    {
        SLOGE("create mount point %s fail, error info[%d]: %s\n", mntpath, errno, strerror(errno));
        return;
    }

    ret = mkdir(fusepath, 0755);
    if(ret != 0 && errno != EEXIST)
    {
        SLOGE("create fuse point %s fail, error info[%d]: %s\n", fusepath, errno, strerror(errno));
        return;
    }

    bool mounted = true;
    FS_TYPE_E e_fs_type;
    SLOGI("Enter Mount\n");
    if(strcmp(mntpath, getMountpoint()) == 0)
    {
        ret = mountVolume(devpath, "/mnt/secure/staging", &e_fs_type);
    }
    else
    {
        ret = mountVolume(devpath, mntpath, &e_fs_type);
    }
    
    if(ret != 0)
    {
        rmdir(mntpath);
        rmdir(fusepath);
        unlink(devpath);
        SLOGE("mount volume %s to %s fail\n", devpath, mntpath);
        mounted = false;
        return;
    }

    if(mounted)
    {

        ExternalVolumeInfo* exVolumeInfo = new ExternalVolumeInfo();
        if(exVolumeInfo == NULL)
        {
            SLOGE("new external volume information structure fail\n");
            rmdir(mntpath);
            rmdir(fusepath);
            unlink(devpath);
            if(strcmp(mntpath, getMountpoint()) == 0)
            {
                umountVolume("/mnt/secure/staging");
            }
            else
            {
                umountVolume(mntpath);
            }
            return;
        }

        exVolumeInfo->mMountPoint = strdup(fusepath);
        exVolumeInfo->mDeviceName = strdup(devname);
        exVolumeInfo->mVolumeStatus = ExternalVolumeInfo::VolumeMounted;
        exVolumeInfo->mVolumeMajor = major;
        exVolumeInfo->mVolumeMinor = minor;
        exVolumeInfo->mFileSystemType = e_fs_type;

        struct statfs fsbuf;
        if(strcmp(mntpath, getMountpoint()) == 0)
        {
            ret = statfs("/mnt/secure/staging", &fsbuf);
        }
        else
        {
            ret = statfs(mntpath, &fsbuf);
        }
        if(ret != 0)
        {
            exVolumeInfo->mVolumeTotalSize = 0;
            exVolumeInfo->mVolumeFreeSize = 0;
        }
        else
        {
            long long tmp = 0;
            tmp = fsbuf.f_blocks * fsbuf.f_bsize;
            exVolumeInfo->mVolumeTotalSize = tmp / 1024;
            tmp = fsbuf.f_bavail * fsbuf.f_bsize;
            exVolumeInfo->mVolumeFreeSize = tmp / 1024;
        }
        
        char buf[32];
        memset(buf, 0, 32);
        ret = getVolumeLabel(exVolumeInfo->mDeviceName, 32, buf);
        if(ret != 0)
        {
            exVolumeInfo->mVolumeLabel = strdup("RemovableDevice");
        }
        else
        {
            if(strlen(buf) == 0)
            {
                exVolumeInfo->mVolumeLabel = strdup("RemovableDevice");
            }
            else
            {
                exVolumeInfo->mVolumeLabel = strdup(buf);
            }
        }
        mExVolumes->push_back(exVolumeInfo);

        if(strcmp(mntpath, getMountpoint()) == 0)
        {
            //SLOGI("set state mounted\n");
            setState(Volume::State_Idle);
            
            setDevNode(exVolumeInfo->mDeviceName);
            mDiskMajor = major;
            mDiskMinor = minor;

            char msg[255];
            memset(msg, 0, 255);

            snprintf(msg, sizeof(msg), "Volume %s %s disk inserted (%d:%d)",
                         getLabel(), getFuseMountpoint(), major, minor);
            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskInserted,
                                                     msg, false);

            ExternalVolume::bDefaultMounted = true;
        }
        else 
        {
            char msg[255] = {0};
                
            char service[64] = {0};
            snprintf(service, 64, "fuse_%s", devname);
            property_set("ctl.start", service);
            
            snprintf(msg, sizeof(msg),
            "Volume %s %s state changed from %d (%s) to %d (%s)", "usbdisk",
            exVolumeInfo->mMountPoint, Volume::State_Checking, stateToStr(Volume::State_Checking), Volume::State_Mounted,
            stateToStr(Volume::State_Mounted));

            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeStateChange,
                                                 msg, false);
        }

        if(mStartNotify)
        {
            char msg[255];
            memset(msg, 0, 255);

            snprintf(msg, sizeof(msg), "Volume \"%ld \"%ld \"%d \"%d \"%d \"%s \"%s \"%s \"%d \"add", exVolumeInfo->mVolumeTotalSize, 
                                      exVolumeInfo->mVolumeFreeSize, exVolumeInfo->mVolumeMajor, exVolumeInfo->mVolumeMinor,
                                      exVolumeInfo->mVolumeStatus, exVolumeInfo->mMountPoint, exVolumeInfo->mDeviceName,
                                      exVolumeInfo->mVolumeLabel, exVolumeInfo->mFileSystemType);
            mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeMounted,
                                                   msg, false);  
        }
    }
    if(mDebug) {
        printMPInfo();
    }
    return;
}


void ExternalVolume::handleRemoved(NetlinkEvent *evt)
{
    if(evt == NULL)
    {
        SLOGE("event is not exist\n");
        return;
    }

    const char* devname = evt->findParam("DEVNAME");
    char* fusepath = NULL;

    bool umounted = false;	
    ExternalVolumeCollection::iterator it;

	for(it = mExVolumes->begin(); it != mExVolumes->end();it++)
	{
	  if(0 == strcmp(devname,(*it)->mDeviceName))
	  	{
    	  	ExternalVolumeInfo* info = *it;           
            fusepath = strdup(info->mMountPoint);            
            if(mStartNotify)
            {
     			char msg[255] = {0};
                
                snprintf(msg, sizeof(msg), "Volume \"%s \"remove", fusepath);
                mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeUmounted,
                                                       msg, false);  
            }            
            //umount related iso mountpoint first.
            inISOPreDoUmount(it);  
            //end. 
    		if(0 == strcmp(getFuseMountpoint(), fusepath))
			{
			    if(mExVolumes->size()>1)
				{
				    SLOGI("doRmvAndMulmnt, volums size: %d\n", mExVolumes->size());
    				umounted = doRmvAndMulMnt(info, true);
			 	}
			    else
				{
    				SLOGI("do just Remove of sdcard,volums size: %d\n", mExVolumes->size());
    				umounted = doJustRemove(info, true);
    				//setState(Volume::State_Idle);
                    ExternalVolume::bDefaultMounted = false;

                    int major = atoi(evt->findParam("MAJOR"));
                    int minor = atoi(evt->findParam("MINOR"));

                	char msg[255];
                    memset(msg, 0, 255);

                    snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)",
                                 getLabel(), getFuseMountpoint(), major, minor);
                    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                                             msg, false);
				}
			}
		    else
			{
    			SLOGI("do just Remove of usb, volums size: %d\n", mExVolumes->size());
    			umounted = doJustRemove(info, true);
			}
            
		    break;
	  	}
	}   
        
    char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
    sprintf(devpath, "/dev/block/vold/%s", devname);
    if(0 == access(devpath, F_OK))
    {        
       unlink(devpath);
    }
}

bool ExternalVolume::doJustRemove(ExternalVolumeInfo* exV,  bool is_netlinkEvent)
{
    char mntpath[ExternalVolume::MAX_MNT_PATH_LEN] = {0};
    char* fusepath = NULL;
	int ret = 0, ret2 = 0;
	int err = 0;
	bool isDelMul = false;
	
    fusepath = strdup(exV->mMountPoint);

    if(strcmp(fusepath, getFuseMountpoint()) == 0)
    {
        strcpy(mntpath, getMountpoint());        
    }
    else
    {
        sprintf(mntpath, "%s%s%s", Volume::MEDIA_DIR, "/", exV->mDeviceName);
    }
    if(0 == strcmp(getFuseMountpoint(), fusepath))
    {
         setState(Volume::State_Unmounting);

         usleep(1000 * 1000); // Give the framework some time to react

         char service[64];
         snprintf(service, 64, "fuse_%s", getLabel());
         property_set("ctl.stop", service);
         /* Give it a chance to stop.  I wish we had a synchronous way to determine this... */
         sleep(1);
         
         ret = umountVolume(DIR_SECURE_ASEC);
         if(ret == -999)
         {
            SLOGW("Failed to unmount %s(%s) for remove too quickly! Will retry to umount %s\n", DIR_SECURE_ASEC, strerror(errno), "/mnt/secure/staging");
            ret2 = umountVolume("/mnt/secure/staging");
         }

         if(ret == 0 || (ret == -999 && ret2 == 0))
         {
            int major = exV->mVolumeMajor;
            int minor = exV->mVolumeMinor;

        	char msg[255];
            memset(msg, 0, 255);

            snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)",
                         getLabel(), getFuseMountpoint(), major, minor);
            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                                     msg, false);
         }
    }
    else
    {
            int major = exV->mVolumeMajor;
            int minor = exV->mVolumeMinor;
            char msg[255] = {0};                
            char service[64] = {0};
            
            snprintf(msg, sizeof(msg), "Volume %s %s state changed from %d (%s) to %d (%s)",
                    "usbdisk", fusepath, Volume::State_Mounted, stateToStr(Volume::State_Mounted),
                    Volume::State_Unmounting, stateToStr(Volume::State_Unmounting));
            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeStateChange,
                                         msg, false);
            
            snprintf(service, 64, "fuse_%s", exV->mDeviceName);
            property_set("ctl.stop", service);

            memset(msg, 0, sizeof(msg));
            snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)",
                         "usbdisk", fusepath, major, minor);
            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                                     msg, false);
    }

    umountVolume(fusepath);
	ret = umountVolume(mntpath);
	if(0 != ret && -999 != ret)
	{
		SLOGI("do Just Remove first umount fail, ret =: %d\n", ret);
		exV->mVolumeStatus = ExternalVolumeInfo::VolumeRemoved;
		return false;
	}
	
	if(0 == strcmp(getMountpoint(), mntpath))
	{
		setState(Volume::State_Idle);
	}
                   
	if(0 != strcmp(getMountpoint(), mntpath))
	{
	  if(exV->mMulMounted )
	  {
          setState(Volume::State_Unmounting);

          usleep(1000 * 1000); // Give the framework some time to react
          char service[64];
          snprintf(service, 64, "fuse_%s", getLabel());
          property_set("ctl.stop", service);
          /* Give it a chance to stop.  I wish we had a synchronous way to determine this... */
          sleep(1);
          
	      ret = umountVolume(DIR_SECURE_ASEC);
          if(ret == -999)
          {
              SLOGW("Failed to unmount %s(%s) for remove too quickly!! Will retry to umount %s\n", DIR_SECURE_ASEC, strerror(errno), "/mnt/secure/staging");
              ret2 = umountVolume("/mnt/secure/staging");
          }
          else if(ret == 0)
          {
              umountVolume(getFuseMountpoint());
              ret = umountVolume(getMountpoint());
			  setState(Volume::State_Idle);
          }
		  if(0 != ret && ret == -999 && ret2 !=0)
		  {
		      SLOGI("do just Remove second umount, umountVolume(DEFAULT_MOUNT_POINT) fail, ret1=: %d\n", ret);
		  }
          else
          {
              int major = exV->mVolumeMajor;
              int minor = exV->mVolumeMinor;
  
          	  char msg[255] = {0};  
              snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)",
                           getLabel(), getFuseMountpoint(), major, minor);
              mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                                       msg, false);
          }
		  exV->mMulMounted = false;
		  isDelMul = true;
	  }	
	   
	  if(rmdir(mntpath) && rmdir(fusepath))
	  {
	 	  err = errno; 
		  if(err == EBUSY)
		  {
		   	SLOGI(" rmdir fail, EBUSY\n");
			return false;			
		  }
	  }
	}
    
     char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
	 sprintf(devpath, "/dev/block/vold/%s", exV->mDeviceName);
     if(is_netlinkEvent)
     {
         unlink(devpath);
     }
	 mExVolIterator->push_back(exV->mMountPoint);  
	 inGCExternalVol();

	 if(mExVolumes->size()>0)
	 {
	 	if(isDelMul)
	 	 {
	 	    const char* mntpath2 = NULL;
			char devpath2[ExternalVolume::MAX_DEV_PATH_LEN] = {0};	
			mntpath2 = strdup("/mnt/secure/staging");		    
		    sprintf(devpath2, "%s%s", "/dev/block/vold/", (*(mExVolumes->begin()))->mDeviceName);
			FS_TYPE_E e_fs_type;
            ret = mountVolume(devpath2, mntpath2, &e_fs_type);
			//SLOGI("Mul mount Voume, ret= %d\n", ret);
			if(0 != ret)
			{
				return false;
			}
            else
            {
                setState(Volume::State_Idle);
                (*(mExVolumes->begin()))->mMulMounted = true;
                setDevNode((*(mExVolumes->begin()))->mDeviceName);
                int major = (*(mExVolumes->begin()))->mVolumeMajor;
                int minor = (*(mExVolumes->begin()))->mVolumeMinor;
                
                char msg[255];
                memset(msg, 0, 255);
    
                snprintf(msg, sizeof(msg), "Volume %s %s disk inserted (%d:%d)",
                             getLabel(), getFuseMountpoint(), major, minor);
                mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskInserted,
                                                         msg, false);
                
            }
	 	 }			 		
	 }
	 else
	 {
	 	  ExternalVolume::bDefaultMounted = false;
	 }

	 return true;
}

bool ExternalVolume::doRmvAndMulMnt(ExternalVolumeInfo* exV,  bool is_netlinkEvent)
{
	int  ret=0;

    setState(Volume::State_Unmounting);

    usleep(1000 * 1000); // Give the framework some time to react

    char service[64];
    snprintf(service, 64, "fuse_%s", getLabel());
    property_set("ctl.stop", service);
    /* Give it a chance to stop.  I wish we had a synchronous way to determine this... */
    sleep(1);
    
	ret = umountVolume(DIR_SECURE_ASEC);
    if(ret == 0)
    {
        umountVolume(getFuseMountpoint());
        ret = umountVolume(getMountpoint());
    }
    else if(ret == -999)
    {
        SLOGW("Failed to unmount %s(%s) for remove too quickly!!! Will retry to umount %s\n", DIR_SECURE_ASEC, strerror(errno), "/mnt/secure/staging");
        ret = umountVolume("/mnt/secure/staging");
    }
    
    setState(Volume::State_Idle);
	//SLOGI("umountVoume ret:: %d\n", ret);
	if(0!=ret)
	{
		return false;
	}
    int major = exV->mVolumeMajor;
    int minor = exV->mVolumeMinor;

	char msg[255];
    memset(msg, 0, 255);

    snprintf(msg, sizeof(msg), "Volume %s %s disk removed (%d:%d)",
                 getLabel(), getFuseMountpoint(), major, minor);
    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                             msg, false);

	char devpath1[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
	sprintf(devpath1, "/dev/block/vold/%s", exV->mDeviceName);
    if(is_netlinkEvent)
    {
        unlink(devpath1);
    }
    mExVolIterator->push_back(exV->mMountPoint);  
	inGCExternalVol();

	//SLOGI("removed volums later,the size: %d\n", mExVolumes->size());
	
    const char* mntpath = NULL;
	char devpath2[ExternalVolume::MAX_DEV_PATH_LEN] = {0};	

	mntpath = strdup("/mnt/secure/staging");
    
    sprintf(devpath2, "%s%s", "/dev/block/vold/", (*(mExVolumes->begin()))->mDeviceName);

    FS_TYPE_E e_fs_type = FS_TYPE_INVAL;
	ret = mountVolume(devpath2, mntpath, &e_fs_type);
	if(0 != ret)
	{
        SLOGI("Mul mount Voume failed, ret= %d\n", ret);
		return false;
	}

    (*(mExVolumes->begin()))->mMulMounted = true;
    setDevNode((*(mExVolumes->begin()))->mDeviceName);
    major = (*(mExVolumes->begin()))->mVolumeMajor;
    minor = (*(mExVolumes->begin()))->mVolumeMinor;

    memset(msg, 0, 255);

    snprintf(msg, sizeof(msg), "Volume %s %s disk inserted (%d:%d)",
                         getLabel(), getFuseMountpoint(), major, minor);
    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskInserted,
                                                     msg, false);	
	SLOGI("Mulmounted devce name: %s\n", (*(mExVolumes->begin()))->mDeviceName);
	
    return true;
}	


void ExternalVolume::doCommand(int argc, char** argv)
{
    if (mDebug) {
        for (int k = 0; k < argc; k++) {
        SLOGD("arg[%d] = '%s'.\n", k, argv[k]);
        }
    }

    if(argc < 2)
    {
        SLOGE("argument is invalid\n");
        return;
    }

    if(strcmp(argv[1], "start") == 0)
    {
        doStart();
    }
    else if(strcmp(argv[1], "umount") == 0)
    {
        doUmountCommand(argv[2]);
    }
    else if(strcmp(argv[1], "umountVol") == 0)
    {
        doUmountVolCommand(argv[2]);
    }
    else if(strcmp(argv[1], "mountVol") == 0)
    {
        doMountVolCommand(argv[2], argv[3]);
    }
    else if(strcmp(argv[1], "mountISO") == 0)
    {
        doMountISOCommand(argv[2]);
    }
    else if(strcmp(argv[1], "mountISOex") == 0)
    {
        doMountISOCommandEx(argv[2], argv[3]);
    }
    else if(strcmp(argv[1], "umountISO") == 0)
    {
        doUmountISOCommand(argv[2]);
    }
    else if(strcmp(argv[1], "end") == 0)
    {
        doEnd();
    }
}

void ExternalVolume::doStart()
{
    SLOGI("doStart:\n");
    mStartNotify = true;
    
    ExternalVolumeCollection::iterator volIt;
    for(volIt = mExVolumes->begin(); volIt != mExVolumes->end(); volIt++)
    {
        //VoldService::getInstance()->addEvent(new VoldServiceEvent(ExternalVolumeInfo::VolumeMounted, (*volIt)));
        char msg[255];
        memset(msg, 0, 255);

        snprintf(msg, sizeof(msg), "Volume \"%ld \"%ld \"%d \"%d \"%d \"%s \"%s \"%s \"%d \"add", (*volIt)->mVolumeTotalSize, 
                                  (*volIt)->mVolumeFreeSize, (*volIt)->mVolumeMajor, (*volIt)->mVolumeMinor,
                                  (*volIt)->mVolumeStatus, (*volIt)->mMountPoint, (*volIt)->mDeviceName,
                                  (*volIt)->mVolumeLabel, (*volIt)->mFileSystemType);
        mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeMounted,
                                               msg, false);  
    }

    DeviceCollection::iterator devIt;
    pthread_mutex_lock(&dev_lock);
    for(devIt = mDevices->begin(); devIt != mDevices->end(); devIt++)
    {
        //either wifi or device
        //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceConnected, (*devIt)));
        char msg[255];
        memset(msg, 0, 255);

        if(strncmp((*devIt)->mDeviceName, "WIFI", (strlen((*devIt)->mDeviceName) > strlen("WIFI") ? 
            strlen("WIFI"): strlen((*devIt)->mDeviceName))) != 0 
        && strncmp((*devIt)->mDeviceName, "PROD", (strlen((*devIt)->mDeviceName) > strlen("PROD") ? 
                strlen("PROD"): strlen((*devIt)->mDeviceName))) != 0)
        {
            snprintf(msg, sizeof(msg), "Device \"%d \"%d \"%d \"%s \"add", (*devIt)->mMajor, (*devIt)->mMinor,
                                      (*devIt)->mStatus, (*devIt)->mDeviceName);
            mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceConnected,
                                                   msg, false);
        }
        else
        {
            if(strncmp((*devIt)->mDeviceName, "PROD", (strlen((*devIt)->mDeviceName) > strlen("PROD") ? 
                strlen("PROD"): strlen((*devIt)->mDeviceName))) == 0)
            {
                char product[32];
                memset(product, 0, 32);
                strncpy(product, &(((*devIt)->mDeviceName)[5]), strlen((*devIt)->mDeviceName) - 5);
                snprintf(msg, sizeof(msg), "Prod \"%s \"add", product);
                mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceProductConnected,
                                                           msg, false);
            }
            else
            {
                char wifiInterf[32];
                memset(wifiInterf, 0, 32);
                strncpy(wifiInterf, &(((*devIt)->mDeviceName)[5]), strlen((*devIt)->mDeviceName) - 5);
                snprintf(msg, sizeof(msg), "Wifi \"%s \"add", wifiInterf);
                mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceWifiConnected,
                                                           msg, false);
            }
        }
    }
    pthread_mutex_unlock(&dev_lock);
    SLOGI("doStart end.\n");
}

void ExternalVolume::doEnd()
{
    mStartNotify = false;
}

static int is_mounted (const char* mntPath)
{
    FILE *f = NULL;
    char buf[512] = {0};
    int rc = 0;

    sprintf (buf, "df | grep '%s'", mntPath);

    f = popen (buf, "r");
    if (f == NULL) return 0;

    rc = fread (buf, 1, 16, f);
    pclose (f);

    if (rc > 3) return 1;
    return 0;
}

void ExternalVolume::doMountISOCommand(char* isoFilePath)
{
  
    int ret = 0;
    char cmd[256];

    if (isoFilePath == NULL)
    {
        SLOGI("The iso file path is NULL. \n");
        if(mStartNotify)
        {
            char msg[255];
            memset(msg, 0, 255);
    
            snprintf(msg, sizeof(msg), "Volume \"%s \"add", "NULL");
            mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                   msg, false);
        }
        return ;
    }
    memset(cmd, 0, 256);
    LoopCollection::iterator it;    
    if(mLoops->size() >= 1)
    {
        it = mLoops->begin();
        sprintf(cmd, "losetup /dev/block/%s \'%s\'", *it, isoFilePath);  
    }
    else
    {
        SLOGI("The loop devices are all used\n");
        if(mStartNotify)
        {
            char msg[255];
            memset(msg, 0, 255);
    
            snprintf(msg, sizeof(msg), "Volume \"%s \"add", "NULL");
            mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                   msg, false);
        }
        return;
    }
       
    ret = system(cmd);
    if(ret != 0)
	{
		SLOGE("doMountISOCommand: %s fail, ret = %d\n", cmd, ret);
        if(mStartNotify)
        {
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup("")));
            char msg[255];
            memset(msg, 0, 255);
    
            snprintf(msg, sizeof(msg), "Volume \"%s \"add", "NULL");
            mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                   msg, false);
        }
        return;
	}

   //form a ExternalInfo element    

    char mntpath[ExternalVolume::MAX_MNT_PATH_LEN] = {0};
    char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
 
    sprintf(mntpath, "/mnt/%s", *it);
    sprintf(devpath, "/dev/block/%s", *it);    

    ret = mkdir(mntpath, 0755);
    if(ret != 0 && errno != EEXIST)
    {
        if(mStartNotify)
        {
            char msg[255];
            memset(msg, 0, 255);
    
            snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
            mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                   msg, false);
        }
        SLOGE("Create ISO Mount Point %s fail, error info[%d]: %s\n", mntpath, errno, strerror(errno));
        memset(cmd, 0, 256);
        sprintf(cmd, "losetup -d %s", devpath);
        system(cmd);
        return;
    }

    bool mounted = true;
    char mountcmd[128];
    memset(mountcmd, 0, 128);
    sprintf(mountcmd, "mount -n %s %s", devpath, mntpath);    
    system(mountcmd);
    ret = is_mounted(mntpath);
    if(ret == 0)
    {
        if(mStartNotify)
        {
            char msg[255];
            memset(msg, 0, 255);
    
            snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
            mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                   msg, false);
        }
        rmdir(mntpath);
        memset(cmd, 0, 256);
        sprintf(cmd, "losetup -d %s", devpath);
        system(cmd);
        SLOGE("Mount ISO volume %s to %s fail, ret = %d\n", devpath, mntpath, ret);
        mounted = false;
        return;
    }

    if(mounted)
    {
        ExternalVolumeInfo* exVolumeInfo = new ExternalVolumeInfo();
        if(exVolumeInfo == NULL)
        {
            if(mStartNotify)
            {
                char msg[255];
                memset(msg, 0, 255);
        
                snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
                mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                       msg, false);
            }
            char loopcmd[128];
            SLOGE("new ExternalvolumeInfo structure fail\n");            
            umountVolume(mntpath); 
            rmdir(mntpath);            
            memset(loopcmd, 0, 128);
            sprintf(loopcmd, "losetup -d %s", devpath);
            system(loopcmd);
            
            return;
         }
        /*To make sure where the iso file from, and to record it , in order to doUmountISOCommand in time when there is some device removed. */
        ExternalVolumeCollection::iterator t;
        for(t = mExVolumes->begin(); t != mExVolumes->end(); t++)
        {
            if(strncmp(isoFilePath, (*t)->mMountPoint, strlen((*t)->mMountPoint)) == 0)
            {
              ((*t)->mIsoMountRrd)->push_back(strdup(mntpath));
              break;
            }
        }
        exVolumeInfo->mMountPoint = strdup(mntpath);
        exVolumeInfo->mDeviceName = strdup(*it);
        exVolumeInfo->mVolumeStatus = ExternalVolumeInfo::VolumeMounted;
        exVolumeInfo->mVolumeMajor = 0;
        exVolumeInfo->mVolumeMinor = 0;
        exVolumeInfo->mVolumeTotalSize = 0;
        exVolumeInfo->mVolumeFreeSize = 0;
        exVolumeInfo->mVolumeLabel = strdup("ISOVirtualDevice");
        exVolumeInfo->mFileSystemType = FS_TYPE_ISO9660;
        
        mExVolumes->push_back(exVolumeInfo);

        if(mStartNotify)
        {
            SLOGI("add mounted event\n");
            char msg[255];
            memset(msg, 0, 255);

            snprintf(msg, sizeof(msg), "Volume \"%ld \"%ld \"%d \"%d \"%d \"%s \"%s \"%s \"%d \"add", exVolumeInfo->mVolumeTotalSize, 
                                      exVolumeInfo->mVolumeFreeSize, exVolumeInfo->mVolumeMajor, exVolumeInfo->mVolumeMinor,
                                      exVolumeInfo->mVolumeStatus, exVolumeInfo->mMountPoint, exVolumeInfo->mDeviceName,
                                      exVolumeInfo->mVolumeLabel, exVolumeInfo->mFileSystemType);
            mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeMounted,
                                                   msg, false);            
        }
        char * LoopInf = *it; 
        mLoops->erase(it);
        delete LoopInf;
        
        SLOGI("(4)(End)doMmountISOCommand: mount %s successful.\n", isoFilePath);
    }
}

void ExternalVolume::doMountISOCommandEx(const char * isoFilePath, const char * isoLabel)
{
    char mntpath[ExternalVolume::MAX_MNT_PATH_LEN];
    int  i = 0, j;
    const char *p = isoFilePath;
    const char *q = isoLabel;
    memset(mntpath, 0, ExternalVolume::MAX_MNT_PATH_LEN);

    if(isoFilePath == NULL || isoLabel == NULL)
    {
        SLOGE("Both the isoFilePath and the isoLabel cannot be null!\n");
        if(mStartNotify)
        {
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup("")));
            char msg[255];
            memset(msg, 0, 255);
    
            snprintf(msg, sizeof(msg), "Volume \"%s \"add", "NULL");
            mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                   msg, false);
        }
        return;
    }
    SLOGI("[VoldEx]isoFilePath: %s, isoLabel: %s.\n", isoFilePath, isoLabel);
    //copy the whole path firstly.
    while(*p !='\0')
    {
        mntpath[i++] = *p++;
    }
    mntpath[i] = '\0';
    //locate the curren folder secondly.
    char *r = mntpath + i;
    while(*r != '/') r--;
    r++;
    //copy the isoLabel to make the whole mntpath.
    while(*q != '\0')
    {
        *r++ = *q++;
    }
    *r = '\0';

    SLOGI("[VoldEx]mntpath: %s.\n", mntpath);
    
    ExternalVolumeCollection::iterator ex;
    for(ex = mExVolumes->begin();ex != mExVolumes->end(); ex++)
    {
         if(strncmp(mntpath, (*ex)->mMountPoint, strlen(mntpath)) == 0)
            {
                SLOGI("This iso file has already been mounted at current folder,(*ex)->mMountPoint: %s!\n", (*ex)->mMountPoint);
                 if(mStartNotify)
                {
                     //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup(mntpath)));
                     char msg[255];
                     memset(msg, 0, 255);
             
                     snprintf(msg, sizeof(msg), "Volume \"%s \"add", "NULL");
                     mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                            msg, false);
                }
                return;
            }
    }
    
    //do mount iso
    int ret = 0;
    char cmd[256];

    char* ps_tmp = NULL;
    int   iso_path_len = strlen(isoFilePath)+1;

    /* The iso file path may include spaces, we have to convert ' ' to "\ " */
    ps_tmp = (char *)malloc(iso_path_len*2);
    if (ps_tmp == NULL)
    {
        SLOGI("Path_tmp malloc fail.\n");
        if(mStartNotify)
        {
             //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup(mntpath)));
             char msg[255];
             memset(msg, 0, 255);
     
             snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
             mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                    msg, false);
        }
        return;
    }
    for (i = 0, j = 0; i < iso_path_len; i++)
    {
        if (isoFilePath[i] == ' ')
        {
            ps_tmp[j++] = '\\';
        }
        ps_tmp[j++] = isoFilePath[i];
    }
    
    memset(cmd, 0, 256);
    LoopCollection::iterator it;    
    if(mLoops->size() >= 1)
    {
        it = mLoops->begin();
        sprintf(cmd, "losetup /dev/block/%s  %s", *it, ps_tmp);  
    }
    else
    {
         if(mStartNotify)
        {
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup(mntpath)));
             char msg[255];
             memset(msg, 0, 255);
     
             snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
             mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                    msg, false);
        }
        free(ps_tmp);
        SLOGI("The loop devices are all used\n");
        return;
    }
       
    ret = system(cmd);
    if(ret != 0)
	{
         if(mStartNotify)
        {
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup(mntpath)));
             char msg[255];
             memset(msg, 0, 255);
     
             snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
             mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                    msg, false);
        }
		SLOGE("doMountISOCommandex: %s fail, ret = %d\n", cmd, ret);
        free(ps_tmp);
        return;
	}

    free(ps_tmp);

   //form a ExternalInfo element    
    char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
 
    sprintf(devpath, "/dev/block/%s", *it);    

    ret = mkdir(mntpath, 0755);
    if(ret != 0 && errno != EEXIST)
    {
        if(mStartNotify)
        {
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup(mntpath)));
             char msg[255];
             memset(msg, 0, 255);
     
             snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
             mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                    msg, false);
        }
        SLOGE("Create ISO Mount Point %s fail, error info[%d]: %s\n", mntpath, errno, strerror(errno));
        memset(cmd, 0, 256);
        sprintf(cmd, "losetup -d %s", devpath);
        system(cmd);
        return;
    }

    bool mounted = true;
    char mountcmd[128];
    memset(mountcmd, 0, 128);
    sprintf(mountcmd, "mount -n %s %s", devpath, mntpath);    
    system(mountcmd);
    ret = is_mounted(mntpath);
    if(ret == 0)
    {
        if(mStartNotify)
        {
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup(mntpath)));
             char msg[255];
             memset(msg, 0, 255);
     
             snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
             mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                    msg, false);
        }
        rmdir(mntpath);
        memset(cmd, 0, 256);
        sprintf(cmd, "losetup -d %s", devpath);
        system(cmd);
        SLOGE("Mount ISO volume %s to %s fail, ret = %d\n", devpath, mntpath, ret);
        mounted = false;
        return;
    }

    if(mounted)
    {
        ExternalVolumeInfo* exVolumeInfo = new ExternalVolumeInfo();
        if(exVolumeInfo == NULL)
        {
            if(mStartNotify)
            {
                //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceIsoMountFailed, strdup(mntpath)));
                 char msg[255];
                 memset(msg, 0, 255);
         
                 snprintf(msg, sizeof(msg), "Volume \"%s \"add", mntpath);
                 mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceIsoMountFailed,
                                                        msg, false);
            }
            char loopcmd[128];
            SLOGE("new ExternalvolumeInfo structure fail\n");            
            umountVolume(mntpath); 
            rmdir(mntpath);            
            memset(loopcmd, 0, 128);
            sprintf(loopcmd, "losetup -d %s", devpath);
            system(loopcmd);
            
            return;
         }
        /*To make sure where the iso file from, and to record it , in order to doUmountISOCommand in time when there is some device removed. */
        ExternalVolumeCollection::iterator t;
        for(t = mExVolumes->begin(); t != mExVolumes->end(); t++)
        {
            if(strncmp(isoFilePath, (*t)->mMountPoint, strlen((*t)->mMountPoint)) == 0)
            {
              ((*t)->mIsoMountRrd)->push_back(strdup(mntpath));
              break;
            }
        }
        exVolumeInfo->mMountPoint = strdup(mntpath);
        exVolumeInfo->mDeviceName = strdup(*it);
        exVolumeInfo->mVolumeStatus = ExternalVolumeInfo::VolumeMounted;
        exVolumeInfo->mVolumeMajor = 0;
        exVolumeInfo->mVolumeMinor = 0;
        exVolumeInfo->mVolumeTotalSize = 0;
        exVolumeInfo->mVolumeFreeSize = 0;
        exVolumeInfo->mVolumeLabel = strdup("ISOVirtualDevice");
        exVolumeInfo->mFileSystemType = FS_TYPE_ISO9660;
        
        mExVolumes->push_back(exVolumeInfo);

        if(mStartNotify)
        {
            SLOGI("add mounted event\n");
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(ExternalVolumeInfo::VolumeMounted, exVolumeInfo));
             char msg[255];
             memset(msg, 0, 255);
             snprintf(msg, sizeof(msg), "Volume \"%ld \"%ld \"%d \"%d \"%d \"%s \"%s \"%s \"%d \"add", exVolumeInfo->mVolumeTotalSize, 
                                       exVolumeInfo->mVolumeFreeSize, exVolumeInfo->mVolumeMajor, exVolumeInfo->mVolumeMinor,
                                       exVolumeInfo->mVolumeStatus, exVolumeInfo->mMountPoint, exVolumeInfo->mDeviceName,
                                       exVolumeInfo->mVolumeLabel, exVolumeInfo->mFileSystemType);
             mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeMounted,
                                                    msg, false);
        }
        char * LoopInf = *it; 
        mLoops->erase(it);
        delete LoopInf;
        
        SLOGI("doMmountISOCommandex: mount %s successful.\n", isoFilePath);
    }
}

void ExternalVolume::doUmountISOCommand(char* isoMountPath)
{  
    int ret = 0;
    char cmd[128];
    ExternalVolumeCollection::iterator it;
    IsoMountRecord::iterator iit;

    //remove the ISO Record about.
    for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
    {
        if(((*it)->mIsoMountRrd)->size() > 0)
        {
            for(iit = ((*it)->mIsoMountRrd)->begin();iit != ((*it)->mIsoMountRrd)->end(); iit++)
            {
                if(strcmp(*iit, isoMountPath) == 0)
                {
                    char * pth = *iit;
                    ((*it)->mIsoMountRrd)->erase(iit);
                    delete pth;                    
                    break;
                }
            }   
        }
    } 

    memset(cmd, 0, 128);
    //delete the iso ExternalVolumeInfo object
    for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
    {
        if(strcmp((*it)->mMountPoint, isoMountPath) == 0)
        {
            if(mStartNotify)
            {
                SLOGE("notify iso umounted\n");
                //VoldService::getInstance()->addEvent(new VoldServiceEvent(ExternalVolumeInfo::VolumeUmounted, strdup(isoMountPath)));
                char msg[255];
                memset(msg, 0, 255);
             
                snprintf(msg, sizeof(msg), "Volume \"%s \"remove", isoMountPath);
                mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeUmounted,
                                                       msg, false);
            }
            ret = umountVolume(isoMountPath);
             if(ret == 0)
            {
                rmdir(isoMountPath); 
            }
            else
            {
                SLOGE("umount ISO mountPoint %s fail\n", isoMountPath);
                break;
            }
            sprintf(cmd, "losetup -d /dev/block/%s", (*it)->mDeviceName);
            ret = system(cmd);
            if(ret == 0)
            {
               SLOGE("doUmountISOCommand(mDeviceName)is %s .\n", (*it)->mDeviceName);
               mLoops->push_front(strdup((*it)->mDeviceName));

               ExternalVolumeInfo * inf = *it; 
               mExVolumes->erase(it);              
               delete inf;
               
               SLOGE("doUmountISOCommand(End) %s Success!\n", cmd);               
               break;
            }
            else
            {
                SLOGE("doUmountISOCommand(End) %s Fail, ret = %d\n", cmd , ret);
                break;
            }
        }
    }
}



void  ExternalVolume::inISOPreDoUmount(ExternalVolumeCollection::iterator it)
{            
    //the following is for iso unmounted     
    if((*it)->mIsoMountRrd->size()>0)
    {
        SLOGI("\n ISO in PreDoUmount, size: %d.\n", (*it)->mIsoMountRrd->size());
        IsoMountRecord::iterator iit;
      for(iit = ((*it)->mIsoMountRrd)->begin();iit != ((*it)->mIsoMountRrd)->end(); iit++)
         {
           int ret = 0;
           char cmd[128];
           ExternalVolumeCollection::iterator t;
           memset(cmd, 0, 128);
           for(t = mExVolumes->begin(); t != mExVolumes->end(); t++)
           {
              if(strcmp((*t)->mMountPoint, *iit) == 0 )
              {
                if(mStartNotify)
                {
                      //VoldService::getInstance()->addEvent(new VoldServiceEvent(ExternalVolumeInfo::VolumeUmounted, strdup((*t)->mMountPoint)));
                      char msg[255];
                      memset(msg, 0, 255);
                   
                      snprintf(msg, sizeof(msg), "Volume \"%s \"remove", (*t)->mMountPoint);
                      mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeUmounted,
                                                             msg, false);
                }

                ret = umountVolume(*iit);
                if(ret == 0)
                {
                      rmdir(*iit); 
                }
                else
                {
                      SLOGE("umount ISO mountPoint %s fail\n", *iit);
                      (*t)->mVolumeStatus = ExternalVolumeInfo::VolumeRemoved;
                      break;
                }
                sprintf(cmd, "losetup -d /dev/block/%s", (*t)->mDeviceName);               
                ret = system(cmd);
                if(ret == 0)
                {            
                      SLOGE("handleRemoved: umount iso command:(mDeviceName)is %s .\n", (*t)->mDeviceName);
                      mLoops->push_front(strdup((*t)->mDeviceName));                 
                      SLOGE("handleRemoved umount iso(End) %s Success!\n", cmd);
                      mExVolIterator->push_back(strdup((*t)->mMountPoint));                
                      break; 
                }
                else
                {
                      SLOGE("handleRemoved umount iso(End) %s Fail!\n", cmd);                   
                      break;
                }               
              }
           }                 
         }      
       }
     else
       {
           SLOGI("\n in ISO PreDoUmount do nothing, size: %d.\n", (*it)->mIsoMountRrd->size()); 
       }
      //the  iso preunmounted end.
      // SLOGE("Preumount end: mExVolIterator size: %d .\n", mExVolIterator->size());
      // SLOGE("Preumount end: mExVolumes size: %d .\n", mExVolumes->size());
    inGCExternalVol();
    return;
}

void ExternalVolume::inGCExternalVol()
{
    ExVolIterator::iterator dit;
    ExternalVolumeCollection::iterator it;
    if(mExVolIterator->size() > 0)
    {
        for(dit = mExVolIterator->begin(); dit != mExVolIterator->end(); dit++)
        {
            for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
            {
                if(strcmp((*it)->mMountPoint, *dit) == 0)
                {
                  ExternalVolumeInfo *info = *it;
                  SLOGI("[inGCExternalVol]: mExVolumes del the mMountPoint: %s .\n", info->mMountPoint);  
                  mExVolumes->erase(it);
                  delete info;
                  break;
                }
                
            } 
        }              
        mExVolIterator->clear();
        //SLOGE("[inGCExternalVol]-end: mExVolIterator size: %d .\n", mExVolIterator->size());
        //SLOGE("[inGCExternalVol]-end: mExVolumes size: %d .\n", mExVolumes->size());
    } 
}

void ExternalVolume::doUmountCommand(char* devname)
{
    PcharList * tmpPcharList;
    PcharList::iterator tmp_it;
    ExternalVolumeCollection::iterator it;    
    const char* mntpath = NULL;

    tmpPcharList = new PcharList();

    /*To get the related patitions to be umount*/
    for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
    {
        if(strncmp(devname, (*it)->mDeviceName, strlen(devname)) == 0)
	  	{
            tmpPcharList->push_back(strdup((*it)->mMountPoint));
        }
    }
    /*To  umount the mounted partitions related to the device*/
    for(tmp_it = tmpPcharList->begin(); tmp_it != tmpPcharList->end(); tmp_it++)
    {   
        for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
        {
            if(strncmp(*tmp_it, (*it)->mMountPoint, strlen(*tmp_it)) == 0)
    	  	{          
        	  	mntpath = strdup((*it)->mMountPoint);            
                 //umount related iso mountpoint first.
                inISOPreDoUmount(it);  
                //end.  //inISOPreDoUmount(it);            
                
                 if(mStartNotify)
                {
                    //VoldService::getInstance()->addEvent(new VoldServiceEvent(ExternalVolumeInfo::VolumeUmounted,  strdup(mntpath)));
        			//SLOGI("sent message true\n");
        			char msg[255];
                    memset(msg, 0, 255);
                 
                    snprintf(msg, sizeof(msg), "Volume \"%s \"remove", mntpath);
                    mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeUmounted,
                                                           msg, false);        			
                }           
                           
        		if(0 == strcmp(DEFAULT_MOUNT_POINT,mntpath))
    			{
    			    if(mExVolumes->size()>1)
    				{
    				    SLOGI("doRmvAndMulmnt, volums size: %d\n", mExVolumes->size());
        				doRmvAndMulMnt(*it, false);
    			 	}
    			    else
    				{
        				SLOGI("do just Remove of sdcard,volums size: %d\n", mExVolumes->size());
        				doJustRemove(*it, false);
                        ExternalVolume::bDefaultMounted = false;
    				}
    			}
    		    else
    			{
        			SLOGI("do just Remove of usb, volums size: %d\n", mExVolumes->size());
        			doJustRemove(*it, false);
    			}	
                
                break;
    	  	}
    	}
    }

    DeviceCollection::iterator devIt;
    pthread_mutex_lock(&dev_lock);
    for(devIt = mDevices->begin(); devIt != mDevices->end(); devIt++)
    {
        if(strcmp(devname, (*devIt)->mDeviceName) == 0)
        {          
            Device* dev = *devIt;
            mDevices->erase(devIt);
            delete dev;
            
            if(mStartNotify)
            {
                //VoldService::getInstance()->addEvent(new VoldServiceEvent(Device::DeviceDisconnected, (const void*)(strdup(devname))));
                char msg[255];
                memset(msg, 0, 255);
             
                snprintf(msg, sizeof(msg), "Device \"%s \"remove", devname);
                mVm->getDMBroadcaster()->sendBroadcast(Device::DeviceDisconnected,
                                                       msg, false);
            }
            break;
        }
    }
    pthread_mutex_unlock(&dev_lock);
    //printDevInfo();
    
    for(tmp_it = tmpPcharList->begin(); tmp_it != tmpPcharList->end(); tmp_it++)
    {
       delete (*tmp_it);
    }
    delete tmpPcharList;

    return;
}

void ExternalVolume::doMountVolCommand(const char* partname, const char* mntpoint)
{
    char block_dev[ExternalVolume::MAX_MNT_PATH_LEN]= "/dev/block/";
    struct stat statInfo;
    
    if(partname == NULL)
    {
        SLOGE("Partition Name can not be NULL\n");
        return;
    }
    strcat(block_dev, partname);
    if(access(block_dev, F_OK) < 0)
    {
        SLOGE("No such device: %s\n", block_dev);
        return;
    }
    if(stat(block_dev, &statInfo) < 0)
    {
        SLOGE("Stat device<%s> error(%d): %s\n", block_dev, errno, strerror(errno));
        return;
    }
    
    int majorNo = major(statInfo.st_dev);
    int minorNo = minor(statInfo.st_dev);
    const char* devname = partname;

    char mntpath[ExternalVolume::MAX_MNT_PATH_LEN] = {0};
    char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};

    if(strcmp(mntpoint, "NULL") == 0)
    {
        if(!ExternalVolume::bDefaultMounted)
        {
            strcpy(mntpath, DEFAULT_MOUNT_POINT);
        }
        else
        {
            sprintf(mntpath, "%s%s%s", "/mnt/usb", "/", partname);
        }
    }
    else
    {
        strcpy(mntpath, mntpoint);
    }
    
    sprintf(devpath, "%s%s", "/dev/block/vold/", partname);

    int ret = 0;
    ret = createDeviceNode(devpath, majorNo, minorNo);
    if(ret != 0)
    {
        SLOGE("create device node %s fail\n", devpath);
        return;
    }

    ret = mkdir(mntpath, 0755);
    if(ret != 0 && errno != EEXIST)
    {
        SLOGE("create mount point %s fail, error info[%d]: %s\n", mntpath, errno, strerror(errno));
        return;
    }

    bool mounted = true;
    FS_TYPE_E e_fs_type;
    if(strcmp(mntpath, DEFAULT_MOUNT_POINT) == 0)
    {
        ret = mountVolume(devpath, "/mnt/secure/staging", &e_fs_type);
    }
    else
    {
        ret = mountVolume(devpath, mntpath, &e_fs_type);
    }
    
    if(ret != 0)
    {
        rmdir(mntpath);
        unlink(devpath);
        SLOGE("mount volume %s to %s fail\n", devpath, mntpath);
        mounted = false;
        return;
    }

    if(mounted)
    {

        ExternalVolumeInfo* exVolumeInfo = new ExternalVolumeInfo();
        if(exVolumeInfo == NULL)
        {
            SLOGE("new external volume information structure fail\n");
            rmdir(mntpath);
            unlink(devpath);
            if(strcmp(mntpath, DEFAULT_MOUNT_POINT) == 0)
            {
                umountVolume("/mnt/secure/staging");
            }
            else
            {
                umountVolume(mntpath);
            }
            return;
        }

        exVolumeInfo->mMountPoint = strdup(mntpath);
        exVolumeInfo->mDeviceName = strdup(partname);
        exVolumeInfo->mVolumeStatus = ExternalVolumeInfo::VolumeMounted;
        exVolumeInfo->mVolumeMajor = majorNo;
        exVolumeInfo->mVolumeMinor = minorNo;
        exVolumeInfo->mFileSystemType = e_fs_type;

        struct statfs fsbuf;
        if(strcmp(mntpath, DEFAULT_MOUNT_POINT) == 0)
        {
            ret = statfs("/mnt/secure/staging", &fsbuf);
        }
        else
        {
            ret = statfs(exVolumeInfo->mMountPoint, &fsbuf);
        }
        if(ret != 0)
        {
            exVolumeInfo->mVolumeTotalSize = 0;
            exVolumeInfo->mVolumeFreeSize = 0;
        }
        else
        {
            long long tmp = 0;
            tmp = fsbuf.f_blocks * fsbuf.f_bsize;
            exVolumeInfo->mVolumeTotalSize = tmp / 1024;
            tmp = fsbuf.f_bavail * fsbuf.f_bsize;
            exVolumeInfo->mVolumeFreeSize = tmp / 1024;
        }
        
        char buf[32];
        memset(buf, 0, 32);
        ret = getVolumeLabel(exVolumeInfo->mDeviceName, 32, buf);
        if(ret != 0)
        {
            exVolumeInfo->mVolumeLabel = strdup("RemovableDevice");
        }
        else
        {
            if(strlen(buf) == 0)
            {
                exVolumeInfo->mVolumeLabel = strdup("RemovableDevice");
            }
            else
            {
                exVolumeInfo->mVolumeLabel = strdup(buf);
            }
        }
        mExVolumes->push_back(exVolumeInfo);

        if(strcmp(mntpath, DEFAULT_MOUNT_POINT) == 0)
        {
            //SLOGI("set state mounted\n");
            setState(Volume::State_Idle);
            
            setDevNode(exVolumeInfo->mDeviceName);
            mDiskMajor = majorNo;
            mDiskMinor = minorNo;

            char msg[255];
            memset(msg, 0, 255);

            snprintf(msg, sizeof(msg), "Volume %s %s disk inserted (%d:%d)",
                         getLabel(), getMountpoint(), majorNo, minorNo);
            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskInserted,
                                                     msg, false);

            ExternalVolume::bDefaultMounted = true;
        }
        else 
        {
            char msg[255];
            memset(msg, 0, 255);
            
            snprintf(msg, sizeof(msg),
            "Volume %s %s state changed from %d (%s) to %d (%s)", "usbdisk",
            mntpath, Volume::State_Checking, stateToStr(Volume::State_Checking), Volume::State_Mounted,
            stateToStr(Volume::State_Mounted));

            mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeStateChange,
                                                 msg, false);
        }

        if(mStartNotify)
        {
            //SLOGI("add mounted event\n");
            //VoldService::getInstance()->addEvent(new VoldServiceEvent(ExternalVolumeInfo::VolumeMounted, exVolumeInfo));
            char msg[255];
            memset(msg, 0, 255);

            snprintf(msg, sizeof(msg), "Volume \"%ld \"%ld \"%d \"%d \"%d \"%s \"%s \"%s \"%d \"add", exVolumeInfo->mVolumeTotalSize, 
                                      exVolumeInfo->mVolumeFreeSize, exVolumeInfo->mVolumeMajor, exVolumeInfo->mVolumeMinor,
                                      exVolumeInfo->mVolumeStatus, exVolumeInfo->mMountPoint, exVolumeInfo->mDeviceName,
                                      exVolumeInfo->mVolumeLabel, exVolumeInfo->mFileSystemType);
            mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeMounted,
                                                   msg, false);  
        }
    }
    if(mDebug) {
        printMPInfo();
    }
    return;
}


void ExternalVolume::doUmountVolCommand(const char* partname)
{
    ExternalVolumeCollection::iterator it;    
    const char* mntpath = NULL;

    /*To umount the partition*/ 
    for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
    {
        if(strcmp(partname, (*it)->mDeviceName) == 0)
	  	{          
    	  	mntpath = (*it)->mMountPoint;            
             //umount related iso mountpoint first.
            inISOPreDoUmount(it);  
            //end.  //inISOPreDoUmount(it);            
            
            if(mStartNotify)
            {
    			char msg[255] ={0};             
                snprintf(msg, sizeof(msg), "Volume \"%s \"remove", mntpath);
                mVm->getDMBroadcaster()->sendBroadcast(ExternalVolumeInfo::VolumeUmounted,
                                                       msg, false);        			
            }           
                       
    		if(0 == strcmp(DEFAULT_MOUNT_POINT,mntpath))
			{
			    if(mExVolumes->size()>1)
				{
				    SLOGI("doRmvAndMulmnt, volums size: %d\n", mExVolumes->size());
    				doRmvAndMulMnt(*it, false);
			 	}
			    else
				{
    				SLOGI("do just Remove of sdcard,volums size: %d\n", mExVolumes->size());
    				doJustRemove(*it, false);
                    ExternalVolume::bDefaultMounted = false;
				}
			}
		    else
			{
    			SLOGI("do just Remove of usb, volums size: %d\n", mExVolumes->size());
    			doJustRemove(*it, false);
			}	
            
            break;
	  	}
	}
    return;
}

int ExternalVolume::mountVolume(const char * devpath,const char * mntpath,FS_TYPE_E * pe_fs_type)
{ 
    if(devpath == NULL || mntpath == NULL)
    {
        SLOGE("parameter is invalid\n");
        return -1;
    }
    
    int ret = 0;
    
    bool mounted = false;
    int mountflags = 0;
    int vfatErrno = 0;
    int ntfsErrno = 0;
    #ifdef EXT_SUPPORT
    int ext2Errno = 0;
    int ext3Errno = 0;
    int ext4Errno = 0;
    #endif
    #ifdef EXFAT_SUPPORT
    int exfatErrno = 0;
    #endif
    int mountTimes = 0;
	char mountData[255] = {0};
	int permMask = 0007;

    if(mntpath != NULL && strstr(mntpath, "staging") == NULL)
    {
        //do not allow the disk has the execute permission, except the sdcard.
        mountflags |= MS_NOEXEC;
    }

    do
    {
        //if need check disk
        SLOGI("Try to mount %s to %s(FS: vfat, Flags:0X%X)...\n", devpath, mntpath, mountflags); 
		sprintf(mountData, "utf8,uid=1023,gid=1023,dmask=%o,fmask=%o,shortname=mixed",permMask,permMask);       
        ret = mount(devpath, mntpath, "vfat", mountflags, mountData);
        if(ret == 0)
        {
            mounted = true;
            *pe_fs_type = FS_TYPE_FAT;
            break;
        } 
        vfatErrno = errno;
        SLOGI("vfatErrno: %d(%s)\n", vfatErrno, strerror(vfatErrno));
        if (ret != 0 && vfatErrno != EBUSY && vfatErrno != EACCES && vfatErrno != EROFS)
        {
            SLOGI("Try to mount %s to %s(FS: ntfs, Flags:0X%X)...\n", devpath, mntpath, mountflags);
       #ifdef NTFS_WRITE_SUPPORT
			#ifdef TNTFS_SUPPORT
            if(access(TUXERA_NTFS_KO_PATH, F_OK) == 0)
            {
                SLOGI("Tuxera NTFS mount\n");
                sprintf(mountData, "utf8,uid=1000,gid=1015,dmask=%o,fmask=%o",permMask,permMask);  //for not support shortname
                ret = mountVolumeByTuxeraNtfsKO(devpath, mntpath, mountflags, mountData);    
            } 
            else
            #endif
            if(access(NTFS_3G_PATH, F_OK) == 0 && access(FUSE_KO_PATH, F_OK) == 0)
            {
                SLOGI("ntfs-3g mount\n");
                ret = mountVolumeByNtfs3g(devpath, mntpath);           
            }
            else
            {
       #endif
                SLOGI("ntfs.ko mount\n");
                ret = mountVolumeByNtfsKo(devpath,mntpath,mountflags);  
       #ifdef NTFS_WRITE_SUPPORT
            }
       #endif
            
            if(ret == 0)
            {
                mounted = true;
                *pe_fs_type = FS_TYPE_NTFS;
                break;
            }
            ntfsErrno = errno;
            SLOGE("ntfsErrno: %d(%s)\n", ntfsErrno, strerror(ntfsErrno));
        }
#ifdef EXT_SUPPORT
        if (ret != 0 && ntfsErrno!= EBUSY && ntfsErrno != EACCES && ntfsErrno != EROFS)
        {
            SLOGI("Try to mount %s to %s(FS: ext2, Flags:0X%X)...\n", devpath, mntpath, mountflags);          
            ret = mount(devpath, mntpath, "ext2", mountflags, "errors=continue");             
            if(ret == 0)
            {
                mounted = true;
                *pe_fs_type = FS_TYPE_EXT2;
                break;
            }
            ext2Errno = errno;
            SLOGE("ext2Errno: %d(%s)\n", ext2Errno, strerror(ext2Errno));
        }

        if (ret != 0 && ext2Errno!= EBUSY && ext2Errno != EACCES && ext2Errno != EROFS)
        {
            SLOGI("Try to mount %s to %s(FS: ext3, Flags:0X%X)...\n", devpath, mntpath, mountflags);          
            ret = mount(devpath, mntpath, "ext3", mountflags, "errors=continue");             
            if(ret == 0)
            {
                mounted = true;
                *pe_fs_type = FS_TYPE_EXT3;
                break;
            }
            ext3Errno = errno;
            SLOGE("ext3Errno: %d(%s)\n", ext3Errno, strerror(ext3Errno));
        }

        if (ret != 0 && ext3Errno!= EBUSY && ext3Errno != EACCES && ext3Errno != EROFS)
        {
            SLOGI("Try to mount %s to %s(FS: ext4, Flags:0X%X)...\n", devpath, mntpath, mountflags);          
            ret = mount(devpath, mntpath, "ext4", mountflags, "errors=continue");             
            if(ret == 0)
            {
                mounted = true;
                break;
                *pe_fs_type = FS_TYPE_EXT4;
            }
            ext4Errno = errno;
            SLOGE("ext4Errno: %d(%s)\n", ext4Errno, strerror(ext4Errno));
        }
  #endif
  #ifdef EXFAT_SUPPORT
        if (ret != 0 && ext4Errno != EBUSY && ext4Errno != EACCES && ext4Errno != EROFS)
        {
            SLOGI("Try to mount %s to %s(FS: exfat, Flags:0X%X)...\n", devpath, mntpath, mountflags);
            if(access(EXFAT_PATH, F_OK) == 0 && access(FUSE_KO_PATH, F_OK) == 0)
            {
                SLOGI("exfat mount\n");
                ret = mountVolumeByExfat(devpath, mntpath);           
            }            
            if(ret == 0)
            {
                mounted = true;
                *pe_fs_type = FS_TYPE_EXFAT;
                break;
            }
            exfatErrno = errno;
            SLOGE("exfatErrno: %d(%s)\n", exfatErrno, strerror(exfatErrno));
        }
#endif
        mountTimes++;
        
        if (vfatErrno == EBUSY || ntfsErrno == EBUSY 
            #ifdef EXT_SUPPORT
            || ext2Errno == EBUSY || ext3Errno == EBUSY || ext4Errno == EBUSY
            #endif
            #ifdef EXFAT_SUPPORT
            || exfatErrno ==EBUSY
            #endif
            )
        {
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = 100000000;
                
            SLOGI("Mount %s to %s failed as %s, sleep a while\n", devpath, mntpath, strerror(errno));                
            nanosleep(&req, NULL);                
            SLOGI("Wake up, try to remount %s to %s again\n", devpath, mntpath, strerror(errno));

            mountflags |= MS_REMOUNT;
        }
        else if ( vfatErrno == EACCES || ntfsErrno == EACCES 
               || vfatErrno == EROFS || ntfsErrno == EROFS  
               || vfatErrno == EINVAL || ntfsErrno == EINVAL
              #ifdef EXT_SUPPORT
               || ext2Errno == EACCES || ext3Errno == EACCES || ext4Errno == EACCES
               || ext2Errno == EROFS || ext3Errno == EROFS || ext4Errno == EROFS
               || ext2Errno == EINVAL|| ext3Errno == EINVAL || ext4Errno == EINVAL
              #endif
              #ifdef EXFAT_SUPPORT
               || exfatErrno == EACCES || exfatErrno == EROFS || exfatErrno == EINVAL
              #endif
                )
        {
            mountflags |= MS_RDONLY;
        }
    }while(mountTimes < 2);

    if(mounted)
    {
        SLOGI("mount %s to %s successful\n", devpath, mntpath);
        return 0;
    }
    else
    {
        SLOGE("mount %s to %s unsuccessful\n", devpath, mntpath);
        return -1;
    }
}

int ExternalVolume::umountVolume(const char* mntpath)
{
    if(mntpath == NULL)
    {
        SLOGE("mount point is NULL\n");
        return -1;
    }
    #if 0
    int umountTimes = 0;
    bool umounted = false;
    do
    {
        umountTimes++;
        SLOGI("try to umount %s\n", mntpath);
        int ret = umount(mntpath);
        
        if(ret == 0)
        {
            umounted = true;
            break;
        }
        SLOGI("umount %s %s , errno: %s.\n", mntpath, (umounted ? "successful" : "unsuccessful"), strerror(errno));      
        #if 1
        if (errno == EBUSY)
        {
            struct timespec req;
            req.tv_sec = 0;
            req.tv_nsec = 1000000000;

            nanosleep(&req, NULL);
            
            /* Another process still holds some file/dir, wait */
            int action = 0;

            if(umountTimes % 2 == 0) 
            {
                action = 2; // SIGKILL
            } 
            else if(umountTimes % 2 == 1) 
            {
                action = 1; // SIGHUP
            }

            Process::killProcessesWithOpenFiles(mntpath, action);

            continue;
        }
        else
        {
            SLOGE("umount log[%d]: %s\n", errno, strerror(errno));
            break;
        }
        #endif
    }while(umountTimes <= 5);

    SLOGI("umount %s %s.\n", mntpath, (umounted ? "successful" : "unsuccessful"));
    
    if(umounted)
    {
        return 0;
    }
    else
    {
        return -1; 
    }
    #endif 
    int retries = 15;
    bool force = true;
    
    SLOGD("Unmounting {%s}, force = %d", mntpath, force);    

    while (retries--) {
        if (!umount2(mntpath, MNT_FORCE | MNT_DETACH) || errno == EINVAL || errno == ENOENT) {
            SLOGI("%s sucessfully unmounted", mntpath);
            return 0;
        }

        if(errno == EINVAL || errno == ENOENT) {
            SLOGW("Failed to unmount %s (%s), return -999\n", mntpath, strerror(errno));
            return -999;
        }
        int action = 0;

        if (force) {
            if (retries < 10) {
                action = 2; // SIGKILL
            } else if (retries == 10 ) {
                action = 1; // SIGHUP
            }
        }

        SLOGW("Failed to unmount %s (%s, retries %d, action %d)",
                mntpath, strerror(errno), retries, action);

        Process::killProcessesWithOpenFiles(mntpath, action);
        usleep(1000*1000);
    }
    errno = EBUSY;
    SLOGE("Giving up on unmount %s (%s)", mntpath, strerror(errno));
    return -1;
}

int ExternalVolume::createDeviceNode(const char *path, int major, int minor) 
{
    if(path == NULL || major < 0 || minor < 0)
    {
        SLOGE("parameter is invalid\n");
        return -1;
    }
    
    mode_t mode = 0660 | S_IFBLK;
    dev_t dev = (major << 8) | minor;
    if (mknod(path, mode, dev) != 0 && errno != EEXIST) 
    {
        SLOGE("create %s(major: %d, minor: %d), error info[%d]: %s\n", path, major, minor, errno, strerror(errno));
        return -1;
    }
    return 0;
}

void ExternalVolume:: printMPInfo()
{
    ExternalVolumeCollection::iterator it;
    for(it = mExVolumes->begin(); it != mExVolumes->end(); it++)
    {
        SLOGI("mount point: %s\n", (*it)->mMountPoint);
        SLOGI("device name: %s\n", (*it)->mDeviceName);
        SLOGI("volume label: %s\n", (*it)->mVolumeLabel?(*it)->mVolumeLabel:"NULL");
        SLOGI("total size: %d\n", (*it)->mVolumeTotalSize);
        SLOGI("free size: %d\n", (*it)->mVolumeFreeSize);
        SLOGI("volume status: %d\n", (*it)->mVolumeStatus);
        SLOGI("major: %d\n", (*it)->mVolumeMajor);
        SLOGI("minor: %d\n", (*it)->mVolumeMinor);
        SLOGI("\n\n");
    }
}

void ExternalVolume:: printDevInfo()
{
    DeviceCollection::iterator it;
    pthread_mutex_lock(&dev_lock);
    for(it = mDevices->begin(); it != mDevices->end(); it++)
    {
        SLOGI("device name: %s\n", (*it)->mDeviceName);
        SLOGI("volume status: %d\n", (*it)->mStatus);
        SLOGI("major: %d\n", (*it)->mMajor);
        SLOGI("minor: %d\n", (*it)->mMinor);
        SLOGI("\n\n");
    }
     pthread_mutex_unlock(&dev_lock);
}


int ExternalVolume::getVolumeLabel(char* partialname, int volumeLabelLength, char* volumeLabel)
{
    FILE* fp_blkid;
    char ps_line[128];  /* getline() will allocate memory for it */
    size_t i4_line_len = 0;
    char devname[128];
    unsigned int ui4_dev_len;
    char* ps_label = NULL;

    const char *blkid_out_file = "/tmp/blkid.tab";

    int i4_ret;
    int i = 0;
    
    /* Call blkid. The out put will look like:
     * /dev/sda1: LABEL="KINGSTON" UUID="ODBD-6753"
     * /dev/sdb: LABEL="12345" UUID="F044-EB18"
     * /dev/sde1: LABEL="abcde"
     */
    memset(devname, 0, 128);
    sprintf(devname, "/dev/block/%s", partialname);  
    ui4_dev_len = strlen(devname);
    memset(volumeLabel, 0, volumeLabelLength);

    system("blkid > /tmp/blkid.tab");   
    
    fp_blkid = fopen(blkid_out_file, "r");       
    if (fp_blkid == NULL)
    {
        SLOGE("open file fail\n");
        return -1;
    }

    /* For each line of blkid output */
    while ((fgets(ps_line, 128, fp_blkid)) != NULL)
    {
        if (ps_line == NULL)
        {
            break;
        }

        /* Test whether line start with right dev node such as "/dev/sda1:" */
        if (ps_line[ui4_dev_len] == ':' 
            && strncmp(ps_line, devname, ui4_dev_len) == 0)
        {
            if (strncmp(ps_line + ui4_dev_len + strlen(": "), "LABEL", strlen("LABEL")) != 0)
            {
                /* There are no LABEL= in the line. */
                volumeLabel[0] = '\0';
                SLOGI("[fm] Volume Label is empty.");
                break;
            }
            else
            {
                ps_label = ps_line + ui4_dev_len + strlen(": LABEL=\"");
                    
                for (i = 0; i < volumeLabelLength -1; i++)
                {
                    if (*ps_label == '\0')
                    {
                        /* There is no UUID, remove the last  "  */
                        i--;
                        /* Check if there is a new line delimiter */
                        ps_label--;
                        if (*ps_label == '\n')
                        {
                            i--;
                        }
                        break;
                    }
                    
                    if (strncmp(ps_label, "\" UUID=\"", strlen("\" UUID=\"")) == 0)
                    {
                        break;
                    }
                    volumeLabel[i] = *ps_label++;
                }

                volumeLabel[i] = '\0';

                /* Check if the buffer is bigger enough. */
                if (i == volumeLabelLength - 1)
                {
                    /* Find the actual buffer size needed. */
                    while ((*ps_label != '\0')
                            && (strncmp(ps_label, "\" UUID=\"", strlen("\" UUID=\"")) != 0))
                    {
                        i++;
                        ps_label++;
                    }

                    volumeLabelLength = i;

                    fclose(fp_blkid);
                    remove(blkid_out_file);
                }
                break;
            }
        }
    }

    fclose(fp_blkid);
    remove(blkid_out_file);
    return 0;
}

#ifdef EXFAT_SUPPORT
int ExternalVolume::mountVolumeByExfat(const char* devpath, const char* mntpath)
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
    sprintf(cmd, "%s %s %s", EXFAT_PATH, devpath, mntpath);
    ret = system(cmd);
    SLOGI("return value: %d, error[%d]: %s\n", ret, errno, strerror(errno));
    if(ret == -1 || ret == 127)
	{
		SLOGE("%s fail!\n", cmd);
        ret = -1;
	}

    return ret;
}
#endif

dev_t ExternalVolume::getDiskDevice()
{
    return MKDEV(mDiskMajor, mDiskMinor);
}

void ExternalVolume::handleVolumeShared()
{
}
void ExternalVolume::handleVolumeUnshared()
{
}

int ExternalVolume::getDeviceNodes(dev_t * devs,int max)
{
    devs[0] = MKDEV(mDiskMajor, mDiskMinor);
    return 1;
}

//The following function members of ExternalVolume are added for building pass.
int ExternalVolume::getVolInfo(volume_info*)
{
    return 0;
}
int ExternalVolume::updateDeviceInfo(char*, int, int)
{
    return 0;
}
void ExternalVolume::revertDeviceInfo()
{
    return;
}
int ExternalVolume::isDecrypted()
{
    return mIsDecrypted; 
}

//end
static void* vold_open_device_thrd(void *pvdata)
{
    int ret;
    int fd;
    bool b_break;
    ExternalVolume *ev =(ExternalVolume *)pvdata;    
    pthread_mutex_t dev_lock = ev->getDevLock() ;
	 
    SLOGD("[DM/ExternalVol] %s: entered\n", "vold_open_device_thrd");
    while(1)
    {
        pthread_mutex_lock(&dev_lock);
        b_break = false;

        if((ev->mDevices)->size() > 0)
        {
            DeviceCollection::iterator it;
            char devpath[ExternalVolume::MAX_DEV_PATH_LEN] = {0};
            
            for(it = ev->mDevices->begin();it != ev->mDevices->end(); it++)
            {
               if(strncmp((*it)->mDeviceName, "sd", strlen("sd")) != 0)
               {
                    //SLOGI("Ignore %s\n", (*it)->mDeviceName);
                    continue;
               }

               memset(devpath, 0 ,ExternalVolume::MAX_DEV_PATH_LEN); 
   
               if((*it) == NULL || (*it)->mDeviceName == NULL)
               {
                   b_break = true;
                   //SLOGD("There is null point, break open!\n");
                   break;
               }
          
          
               sprintf(devpath, "/dev/block/%s", (*it)->mDeviceName);

               /* remove the behaviour to open/close usb device periodically, in order to avoid that VOLD cannot be freezed during suspend.
               //SLOGD("===total devices: %d===open===%s=======\n",(ev->mDevices)->size(), devpath);
               fd = open(devpath, O_RDONLY | O_NONBLOCK);
   
               if (fd >= 0)
               {
                   close(fd);
                   //SLOGD("[DM/ExternalVol]open %s sucess:%d\n", devpath, errno);
               }
               else
               {
                    //SLOGD("[DM/ExternalVol]open %s fail:%d\n", devpath, errno);
               }
               */
               
               if(b_break)
               {
                    break;
               }
           }
        }
        pthread_mutex_unlock(&dev_lock);
        #if 0
        x_thread_delay(1000);
        #else
        {
            struct timespec req;
            
            req.tv_sec = 1;
            req.tv_nsec = 0;
            
            nanosleep(&req, NULL);
        }
        #endif
    }

    pthread_exit(0); 
    return NULL;
}
