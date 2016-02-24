#!/bin/sh

ADDON_SVN=http://10.120.99.80/svn/applications/trunk/java/components
addon_path=$PWD
CD=cd

if [ -d $addon_path/Device ]; then
	svn up $addon_path/Device 
else
	svn co $ADDON_SVN/Device
fi

if [ -d $addon_path/HuanAccount ]; then
	svn up $addon_path/HuanAccount
else
	svn co $ADDON_SVN/HuanAccount
fi

if [ -d $addon_path/Media ]; then
      svn up $addon_path/Media
else
      svn co $ADDON_SVN/Media
fi

if [ -d $addon_path/Net ]; then
      svn up $addon_path/Net
else
      svn co $ADDON_SVN/Net
fi

if [ -d $addon_path/RootSeeker ]; then
      svn up $addon_path/RootSeeker
else
      svn co $ADDON_SVN/RootSeeker
fi

if [ -d $addon_path/adSystem ]; then
      svn up $addon_path/adSystem
else
      svn co $ADDON_SVN/adSystem
fi

if [ -d $addon_path/Tvinfo ]; then
      svn up $addon_path/Tvinfo
else
      svn co $ADDON_SVN/Tvinfo
fi

if [ -d $addon_path/UIKit ]; then
      svn up $addon_path/UIKit
else
      svn co $ADDON_SVN/UIKit
fi
