package com.mediatek.ui.mmp.util;

import android.os.Environment;

public class MultiMediaConstant {

	public static final String MEDIAKEY = "Media";
	public static final String MEDIAPATHKEY = "PATH";
	public static final String ROOTPATH = Environment.getExternalStorageDirectory().getAbsolutePath();
	public static final String STOPMUSIC = "com.mediatek.closeAudio";
	public static final String STOPVIDOE = "com.mediatek.closeVideo";
	
	public static final int PHOTO = 0;
	public static final int AUDIO = 1;
	public static final int VIDEO = 2;
	public static final int TEXT = 3;
	public static final int THRD_PHOTO = 4;
 
	public static final int SMALL = 1;
	public static final int MEDIUM = 2;
	public static final int LARGE = 3;
	public static final int ZOOM = 100;
	public static final int LARGE_THUMBNAIL_SIZE = 300;
	public static final String THUMBNAIL_SIZE="M_THUMBNAIL_SIZE";
	public static final int HIGHT = 40;

}
