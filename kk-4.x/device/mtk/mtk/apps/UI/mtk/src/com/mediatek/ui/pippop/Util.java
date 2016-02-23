/**   
 * @Description: TODO()
 * 
 * @date 2012-10-10 ����2:40:00  
 */
package com.mediatek.ui.pippop;

import android.content.Context;
import android.util.Log;

/**
 * 
 */
public class Util {

	private final static boolean DEBUG = true;
	private final static String TAG = "PIP_POP";

	public static void showDLog(String string) {
		if (DEBUG)
			Log.d(TAG, string);
	}

	public static void showELog(String string) {
		if (DEBUG)
			Log.e(TAG, string);
	}

	public static void showTestButtons(Context context) {
		if (DEBUG) {

		}

	}

}
