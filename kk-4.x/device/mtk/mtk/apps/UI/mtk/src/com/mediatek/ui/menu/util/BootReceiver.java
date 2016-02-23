package com.mediatek.ui.menu.util;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.Log;

import com.mediatek.netcm.wifi.WifiDevService;
import com.mediatek.ui.util.MtkLog;

public class BootReceiver extends BroadcastReceiver {

	static String TAG = "WifiAutoConnectListener";

	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub
		String action = intent.getAction();
		MtkLog.v(TAG, "*************the action is *****************" + action);
		if (action != null && action.equals(Intent.ACTION_BOOT_COMPLETED)) {
			//SharedPreferences mSharedPreferences = PreferenceManager
			//		.getDefaultSharedPreferences(context);
			//int isWifi = mSharedPreferences.getInt(
			//		MenuConfigManager.NETWORK_INTERFACE, 0);
			//int isConnect = mSharedPreferences.getInt(
				//	MenuConfigManager.NETWORK_CONNECTION, 0);
			//MtkLog.v(TAG, "****************isConnect*******************"
				//	+ isConnect);
			//MtkLog.v(TAG, "****************isWifi*******************" + isWifi);
			//if (isConnect == 1 && isWifi == 1) {
				Log
						.v(TAG,
								"****************start dongle service*******************");
				Intent intentService = new Intent(context, WifiDevService.class);
				context.startService(intentService);
			//}			
			}			
		}
	}

	// private void startupConnect(Context context) {
	// SaveValue saveV = SaveValue.getInstance(context);
	// if (saveV.readValue(MenuConfigManager.NETWORK_CONNECTION) == 1
	// && saveV.readValue(MenuConfigManager.NETWORK_INTERFACE) == 1) {
	// Log
	// .v(
	// TAG,
	// "*********************startupConnect**************************\n"
	// + "*********************startupConnect**************************\n"
	// + "*********************startupConnect**************************\n"
	// + "*********************startupConnect**************************\n"
	// + "*********************startupConnect**************************\n");
	// Log.v(TAG, "***********************************************"
	// + context.getClass().getName());
	// Log.v(TAG, "***********************************************"
	// + context.toString());
	// Log.v(TAG, "***********************************************"
	// + context.getPackageName());
	// final NetworkManager networkManager = NetworkManager
	// .getInstance(context);
	// new Thread(new Runnable() {
	//
	// public void run() {
	// // TODO Auto-generated method stub
	// networkManager.wifiAutoConnect();
	// }
	// }).start();
	// }
	// }
