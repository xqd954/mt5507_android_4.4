package com.mediatek.ui.nav.util;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ethernet.EthernetManager;
import android.os.Handler;

import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.NetworkTime;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.util.MtkLog;

public class DhcpReceiver extends BroadcastReceiver {
	private static final String TAG = "DhcpReceiver";
	private Handler mHandler = new Handler();

	public void onReceive(final Context context, Intent intent) {
		String action = intent.getAction();

		if (null!= action && action.equals(EthernetManager.ETHERNET_STATE_CHANGED_ACTION)) {
			final int value = intent.getIntExtra(
					EthernetManager.EXTRA_ETHERNET_STATE,
					EthernetManager.ETHERNET_STATE_UNKNOWN);
			switch (value) {
			//case EthernetManager.EVENT_INTERFACE_CONFIGURATION_SUCCEEDED:
			//case EthernetManager.EVENT_HW_CONNECTED:
			//case EthernetManager.EVENT_HW_PHYCONNECTED:
			default:
				mHandler.postDelayed(new Runnable() {

					public void run() {
						// TODO Auto-generated method stub
						if (SaveValue.getInstance(context).readValue(
								MenuConfigManager.AUTO_SYNC) == 2) {
							MtkLog.d(TAG, "--1 --auto sync from network--------");
//							NetworkTime.getInstance(context).getNetTime();
						}
					}

				}, 10000);
				break;
			}
		}
	}
	

}
