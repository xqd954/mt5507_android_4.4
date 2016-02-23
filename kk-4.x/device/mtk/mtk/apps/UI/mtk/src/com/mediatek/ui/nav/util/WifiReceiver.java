package com.mediatek.ui.nav.util;

import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.NetworkTime;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.util.MtkLog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.NetworkInfo;
import android.net.NetworkInfo.DetailedState;
import android.net.wifi.WifiManager;
import android.os.Handler;

public class WifiReceiver extends BroadcastReceiver {
	private static final String TAG = "WifiReceiver";
	private Handler mHandler = new Handler();

	public void onReceive(final Context context, Intent intent) {
		String action = intent.getAction();
		if (null!= action && action.equals(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION)) {

		} else if (null!= action && action
				.equals(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION)) {
		} else if (null!= action && action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
			DetailedState state = ((NetworkInfo) intent
					.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO))
					.getDetailedState();
			if (state == DetailedState.CONNECTED) {

				mHandler.postDelayed(new Runnable() {

					public void run() {
						// TODO Auto-generated method stub
						if (SaveValue.getInstance(context).readValue(
								MenuConfigManager.AUTO_SYNC) == 2) {
							MtkLog.d(TAG, "---auto sync from network--------");
//							NetworkTime.getInstance(context).getNetTime();
						}
					}

				}, 10000);
			} else {
			}
		}
	}

}
