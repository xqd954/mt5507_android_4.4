#ifndef _UMSVOLUME_H
#define _UMSVOLUME_H
#include <sysutils/NetlinkEvent.h>
#include <linux/kdev_t.h>
#include "VolumeManager.h"
#include "Volume.h"

class UmsVolume : public Volume{
private:
	int major;
	int minor;
	char *devpath;
private:
	void handleVolumeAdded();
	void handleVolumeRemoved();
	void handleVolumeChanged();
public:
	UmsVolume(VolumeManager *vm, const char *label, const char *mountpoint, const char *dp);
	virtual ~UmsVolume();

	int handleBlockEvent(NetlinkEvent *evt);
	void handleVolumeShared();
	void handleVolumeUnshared();
	int getVolInfo(struct volume_info *v) { return 0; };

protected:
	int getDeviceNodes(dev_t *devs, int max);
	int updateDeviceInfo(char *new_path, int new_major, int new_minor) { return 0;};
	void revertDeviceInfo(void) { };
	int isDecrypted(void) {return 0;};
	int getFlags(void) { return 0;};
};
#endif
