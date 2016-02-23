package com.mediatek.ui.util;

import android.util.Log;

/**
 * 
 * @author mtk40530
 *
 */
public final class MtkLog {
	public static boolean logOnFlag = true;
	private static final String LOG = "MtkLog";
	
	private MtkLog(){}
	
	public static void v(String tag, String msg){
		if (logOnFlag){
			Log.v(tag, LOG+msg);
		}
	}
	
	public static void d(String tag, String msg){
		if (logOnFlag){
			Log.d(tag, LOG+msg);
		}
	}
	
	public static void i(String tag, String msg){
		if (logOnFlag){
			Log.i(tag, LOG+msg);
		}
	}
	
	public static void w(String tag, String msg){
		if (logOnFlag){
			Log.w(tag, LOG+msg);
		}
	}
	
	public static void e(String tag, String msg){
		if (logOnFlag){
			Log.e(tag, LOG+msg);
		}
	}
}
