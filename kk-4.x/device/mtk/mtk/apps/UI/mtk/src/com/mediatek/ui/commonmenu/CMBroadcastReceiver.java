package com.mediatek.ui.commonmenu;

import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

import java.util.List;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.util.Log;

public class CMBroadcastReceiver extends BroadcastReceiver {

	private static final String TAG = "CMBroadcastReceiver";
	private static int lastKeycode = -1;

	public CMBroadcastReceiver() {
		super();
	}

	public CMBroadcastReceiver(Context context) {
		super();
	}

	@Override
	public void onReceive(Context context, Intent intent) {
		int keyCode = -1;
		if("true".equals(SystemProperties.get("enable_common_ui"))){
			keyCode = intent.getIntExtra(Intent.EXTRA_KEY_EVENT, -1);
		}/*else{
			keyCode = -1;
		}*/
	
		if ("tv.policy.SYSTEM_KEY".equals(intent.getAction())) {
			
			if (keyCode == KeyMap.KEYCODE_MTKIR_CC){
				Log.d("fff","keyCode == KeyMap.KEYCODE_MTKIR_CC");
						MtkLog.d(TAG, "onReceive---KeyEvent.KEYCODE_MTKIR_CC");
						ActivityManager am = (ActivityManager) context
								.getSystemService(Context.ACTIVITY_SERVICE);
						List<RunningTaskInfo> rtl = am.getRunningTasks(8);

						if (rtl != null && rtl.size() > 0) {
							ComponentName currentActivity = rtl.get(0).topActivity;
							if (!("com.mediatek.ui.CommonMenuActivity"
									.equals(currentActivity.getClassName()))) {
								MtkLog.d(TAG, "---start CommonMenuActivity---");
								Intent mIntent = new Intent(
										context,
										com.mediatek.ui.CommonMenuActivity.class);
								mIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
								context.startActivity(mIntent);
							}
						}
					}
					
		}
	}
}
