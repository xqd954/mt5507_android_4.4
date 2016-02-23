package com.mediatek.netcm.util;

import android.util.Log;

/**
 * This class use to control log ouput for network CM.
 * 
 */
public class NetLog {
	private static boolean localLOGV = true;
	private static final String LOG = "NetLog";
	
	public static void v(String tag, String msg){
		if (localLOGV){
			Log.v(tag, LOG+msg);
		}
	}
	
	public static void d(String tag, String msg){
		if (localLOGV){
			Log.d(tag, LOG+msg);
		}
	}
	
	public static void i(String tag, String msg){
		if (localLOGV){
			Log.i(tag, LOG+msg);
		}
	}
	
	public static void w(String tag, String msg){
		if (localLOGV){
			Log.w(tag, LOG+msg);
		}
	}
	
	public static void e(String tag, String msg){
		if (localLOGV){
			Log.e(tag, LOG+msg);
		}
	}
}
