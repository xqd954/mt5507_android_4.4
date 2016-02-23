/*
 * Copyright (C) 
 */

#ifndef _EXTERNAL_VOLUME_H
#define _EXTERNAL_VOLUME_H

#define EXT_SUPPORT
//#define EXFAT_SUPPORT

#include <utils/List.h>
#include "Volume.h"
#include "VolumeManager.h"
#include <pthread.h>
typedef android::List<char *> IsoMountRecord;

class ExternalVolumeInfo
{
public:
    static const int VolumeIdle = -2;
    static const int VolumeInit = -1;
    static const int VolumeUnsupported = 0;   
    static const int VolumeAdded =  1;
    static const int VolumeRemoved = 2;
    static const int VolumeMounted = 601;
    static const int VolumeUmounted = 602;
        
public:
    char* mMountPoint;
    char* mDeviceName;
    char* mVolumeLabel;
    long mVolumeTotalSize;
    long mVolumeFreeSize;
    int mVolumeStatus;
    int mVolumeMajor;
    int mVolumeMinor;
    FS_TYPE_E mFileSystemType;
	IsoMountRecord* mIsoMountRrd;
	
    bool mMulMounted;

public:
    ExternalVolumeInfo()
    {
        mMountPoint =  NULL;
        mDeviceName = NULL;
        mVolumeLabel = NULL;
        mVolumeTotalSize = 0;
        mVolumeFreeSize = 0;
        mVolumeStatus = ExternalVolumeInfo::VolumeInit;
        mVolumeMajor = -1;
        mVolumeMinor = -1;
	    mIsoMountRrd = new IsoMountRecord();
        mFileSystemType = FS_TYPE_INVAL;      
		
		mMulMounted = false;
    }

    ~ExternalVolumeInfo()
    {
        if(mMountPoint != NULL)
        {
            free(mMountPoint);
            mMountPoint = NULL;
        }
        if(mDeviceName != NULL)
        {
            free(mDeviceName);
            mDeviceName = NULL;
        }
        if(mVolumeLabel != NULL)
        {
            free(mVolumeLabel);
            mVolumeLabel = NULL;
        }
		
	    IsoMountRecord::iterator iit;
	    /*for(iit = mIsoMountRrd->begin(); iit != mIsoMountRrd->end(); iit++)
	    {
	        delete (*iit);
	    }*/
	    if(mIsoMountRrd->size()>0)
    	{
    	 mIsoMountRrd->clear();
    	}
	    delete mIsoMountRrd;
    }
};


class Device
{
public:
    static const int DeviceConnected = 701;
    static const int DeviceDisconnected = 702;
	static const int DeviceIsoMounted = 102;
    static const int DeviceIsoExMounted = 104;
	static const int DeviceIsoUMounted = 103;
    static const int DeviceIsoMountFailed = 650;
	static const int DeviceWifiConnected = 751;
	static const int DeviceWifiDisconnected = 752;
    static const int DeviceProductConnected = 753;
    static const int DeviceProductDisconnected = 754;
public:
    char* mDeviceName;
    int mMajor;
    int mMinor;
    int mStatus;

public:
    Device()
    {
        mDeviceName = NULL;
        mMajor = -1;
        mMinor = -1;
        mStatus = DeviceDisconnected;
    }

    ~Device()
    {
        if(mDeviceName != NULL)
        {
            free(mDeviceName);
            mDeviceName = NULL;
        }
    }
};

class VoldServiceEvent
{
public:
    int mEventType;
    const void* mData;

    VoldServiceEvent(int eventType, const void* data)
    {
        mEventType = eventType;
        mData = data;
    }

    ~VoldServiceEvent()
    {
    }
};


typedef android::List<ExternalVolumeInfo*> ExternalVolumeCollection; 
typedef android::List<Device*> DeviceCollection;
typedef android::List<char *> PathCollection;
typedef android::List<VoldServiceEvent*> EventCollection;
typedef android::List<char *> LoopCollection;
typedef android::List<char *> ExVolIterator;
typedef android::List<char *> PcharList;


class ExternalVolume: public Volume
{   
private:
    static const unsigned int MAX_MNT_PATH_LEN = 32;
    //static const unsigned int MAX_DEV_PATH_LEN = 32;
    static bool bDefaultMounted;
    bool mDebug;

private:
    ExternalVolumeCollection* mExVolumes;
    //DeviceCollection* mDevices;
    PathCollection *mPaths;
    ExternalVolumeInfo* mCurExVolume;
    VolumeManager* mVm;
	LoopCollection* mLoops;	
    bool mStartNotify;
	ExVolIterator*  mExVolIterator;
	int err;
	pthread_t ntid;
    pthread_mutex_t dev_lock;

    int            mDiskMajor;
    int            mDiskMinor;

    int            mFlags;
    int            mIsDecrypted;

protected:    
    const char* mMountpoint;
    const char* mFuseMountpoint;
public:	
    static const unsigned int MAX_DEV_PATH_LEN = 32;
	DeviceCollection* mDevices;	
    
public:
    ExternalVolume(VolumeManager *vm, const fstab_rec* rec, int flags);
    ~ExternalVolume();
    
    const char *getMountpoint() { return mMountpoint; }
    const char *getFuseMountpoint() { return mFuseMountpoint; }

    int addPath(const char* path);
    int handleWifiProduct(NetlinkEvent *evt);
	int handleWifiEvent(NetlinkEvent *evt);
    int handleBlockEvent(NetlinkEvent*evt);
    int umountVolumeCmd(const char *label, bool force, bool revert);
    int handleSdcardMountVol();
#ifndef ANDROID_DEFAULT_CODE
    //2012/8/6,added by Daniel Hsiao,for USB Audio detection support
    int handleSoundEvent(NetlinkEvent*evt);
#endif
    void doCommand(int argc, char** argv);
    dev_t getDiskDevice();
    void handleVolumeShared();
    void handleVolumeUnshared();
	pthread_mutex_t getDevLock(){ return dev_lock;}

    void setFlags(int flags);
    int getVolInfo(struct volume_info *v);
protected:
    int getDeviceNodes(dev_t* devs,int max);

    int getFlags() { return mFlags; }
    int isDecrypted();
    
    virtual void revertDeviceInfo(void);
    int updateDeviceInfo(char *new_path, int new_major, int new_minor);
    
private:    
    void handleDiskAdded(NetlinkEvent* evt);
    void handlePartitionAdded(NetlinkEvent* evt);
    void handleDiskChanged(NetlinkEvent* evt);
    void handlePartitionChanged(NetlinkEvent* evt);
    void handleDiskRemoved(NetlinkEvent* evt);
    void handlePartitionRemoved(NetlinkEvent* evt);
    
    void handleAdded(NetlinkEvent* evt);
    void handleChanged(NetlinkEvent* evt);
    void handleRemoved(NetlinkEvent* evt);

	void doMountISOCommand(char* isoFilePath);
    void doMountISOCommandEx(const char* isoFilePath, const char* isoLabel);
	void doUmountISOCommand(char* isoMountPath);
    void doUmountCommand(char* devname);
    void doMountVolCommand(const char* partname, const char* mntpoint);
    void doUmountVolCommand(const char* partname);
    void doStart();
    void doEnd();
    void inISOPreDoUmount(ExternalVolumeCollection::iterator it);
	void inGCExternalVol();
	
    int mountVolume(const char* devpath, const char* mntpath, FS_TYPE_E* pe_fs_type);
    int umountVolume(const char* mntpath);

    #ifdef EXFAT_SUPPORT
    int mountVolumeByExfat(const char* devpath, const char* mntpath);
    #endif

    int getVolumeLabel(char* devname, int volumeLabelLength, char* volumeLabel);
    int createDeviceNode(const char* path, int major, int minor);
    void printMPInfo();
    void printDevInfo();

	//
	bool doRemove(ExternalVolumeInfo* exV);
	bool doJustRemove(ExternalVolumeInfo* exV,  bool is_netlinkEvent);
	bool doRmvAndMulMnt(ExternalVolumeInfo* exV,  bool is_netlinkEvent);
};
    
#endif

