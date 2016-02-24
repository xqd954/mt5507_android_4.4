#ifndef _VOLUMEINFO_
#define _VOLUMEINFO_
#define UMSDIR "/mnt/usb"

class VolumeInfo{
private:
	char *devname;
	char *mountpoint;
	char *label;
	char *fstype;
	char *uuid;
	char *devpath;

public:
	VolumeInfo(const char *nodepath, const char* dp);
	const char* getVolumeMountpoint();
	const char* getVolumeLabel();
	const char* getVolumeFstype();
	const char* getVolumeUuid();
	const char* getVolumeDevpath();
	static const char* detectCharset(const char *str, int len);
	~VolumeInfo();
};

#endif
