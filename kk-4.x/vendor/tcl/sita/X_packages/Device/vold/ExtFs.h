#ifndef _EXTFS_H
#define _EXTFS_H

#include <unistd.h>

class Ext2{
public:
	static int doMount(const char *fsPath, const char *mountPoint, bool ro, bool remount, bool executable);
	static int format(const char *fsPath);
};

class Ext3{
public:
	static int doMount(const char *fsPath, const char *mountPoint, bool ro, bool remount, bool executable);
	static int format(const char *fsPath);
};


#endif
