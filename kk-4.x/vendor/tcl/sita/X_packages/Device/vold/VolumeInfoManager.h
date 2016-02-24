#ifndef _VOLUMEINFOMANAGER_H
#define _VOLUMEINFOMANAGER_H
#include <utils/List.h>
#include <sysutils/NetlinkEvent.h>
#include "VolumeInfo.h"

typedef android::List<VolumeInfo *> VolumeInfoCollection;

class VolumeInfoManager{
private:
	static VolumeInfoManager *sInstance;
private:
	VolumeInfoCollection *mVolumeInfos;
	
private:
	VolumeInfoManager();
public:
	static VolumeInfoManager *Instance(void);
	int createDeviceNode(const char *path, int major, int minor);
	int handleNetlinkEvent(NetlinkEvent *evt);

	int addVolumeInfo(VolumeInfo *vi);
	int delVolumeInfo(const char *mountpoint);

	const char* getVolFstype(const char *mp);
	int VolumeInfoFstype(SocketClient *cli, const char *mp);
	const char* getVolLabel(const char *mp);
	int VolumeInfoLabel(SocketClient *cli, const char *mp);
	const char* getVolUuid(const char *mp);
	int VolumeInfoUuid(SocketClient *cli, const char *mp); 
	const char* getVolDevpath(const char *mp);
	int VolumeInfoDevpath(SocketClient *cli, const char *mp);
	const char* getMountpointViaDevpath(const char *dp);
	static int isConfigDevpath(const char* dp);
	static const char* getConfigMountpoint(const char* dp);
	~VolumeInfoManager();
};
#endif
