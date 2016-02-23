/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.manager;

import com.mediatek.ui.R;

/**
 *
 */
public class Core {

	public final static int ROOT_VIEW = R.id.linear_glview;

	private final static String pvrFolder = "PVR";
	private final static String tsFolder = "tshift";

	private final static String defaultTshiftFileName = "tshift_ing";

	public final static String PVR_tmp_FILE = "/sdcard/chuanqi.mp4";
	public final static String PVR_DISK_TAG = "/pvrdisk.tag";
	public final static String PVR_DISK = "pvr_disk_tag";
	
	public final static String DISK_SPEED = "disk_speed_tag";
	public final static float DEFAULT_DISK_SPEED = 6.0f;

	public static float AVERAGE_DISK_SPEED = 5.0f;

	public final static String TIMESHIFT_SIZE = "TIMESHIFT_DEFAULT_SIZE";
	public final static int DEFAULT_DISK_FREE_SIZE = 1 * 1024 * 1024;

	public final static String ALARM_ACTION="com.mediatek.ui.schedulepvr";
	public final static String ALARM_ACTION_TAG="schedulepvr_taskid";
	public final static String ALARM_DIALOG_ACTION="com.mediatek.ui.schedulepvr.activity";
	
	public final static String SUFFIX_PVR=".pvr";
//	public final static String SUFFIX_TSHIFT=".pvr";
	public final static String SUFFIX_TSHIFT=".tshift";
	public final static String NO_DEVICES="Nodevice";
	
	public final static int AUTO_DISSMISS_TIMER=15000;
	
	/**
	 * @return the PVR folder
	 */
	public static String getPvrfolder() {
		return pvrFolder;
	}
	
	public static String getTSfolder() {
        return tsFolder;
    }

	/**
	 * @return the default Tshift file name
	 */
	public static String getDefaulttshiftfilename() {
		return defaultTshiftFileName;
	}

}
