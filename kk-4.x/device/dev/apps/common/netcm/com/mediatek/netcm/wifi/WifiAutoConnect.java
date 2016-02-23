package com.mediatek.netcm.wifi;


import java.util.List;

import com.mediatek.netcm.util.NetLog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import android.net.NetworkInfo;
import android.net.NetworkInfo.DetailedState;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;

/**
 * This class use to auto connect to wifi when system boot up.
 * It is need a access point information stored in 'wpa_supplicant.conf'.
 * 
 */
public class WifiAutoConnect {
	private WifiManager mWifiManager;
	private Context mContext = null;

	private List<WifiConfiguration> mConfigList;
	private NetworkConnectReceiver mConnectReceiver;
	private WifiConfiguration mCurrnetConfig = null; 
	private static WifiAutoConnect mWifiAutoConnect;
	
	public String TAG = "CM_WifiAutoConnect";
	private static final boolean localLOGV = false;
	
	private WifiAutoConnect(Context context) {
		this.mContext = context;
		this.mWifiManager = (WifiManager) mContext
				.getSystemService(Context.WIFI_SERVICE);
	}
	
	/**
	 * Create a new WifiAutoConnect instance.
	 * Applications should not create this instance directly,
	 * use com.mediatek.netcm.NetworkManager relevant API instead.
	 * 
	 */
	public static WifiAutoConnect getInstance(Context context) {
		if (mWifiAutoConnect == null) {
			mWifiAutoConnect = new WifiAutoConnect(context);
		}
		
		return mWifiAutoConnect;
	}
	
	private class NetworkConnectReceiver extends BroadcastReceiver {
		
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if (action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
				DetailedState state = ((NetworkInfo) intent
						.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO))
						.getDetailedState();
				if (state == DetailedState.CONNECTED) {

				}
			}
		}
	}
	
	// Keep Temporarily
	private void registerNetChangedRecveiver() {
		final Intent intent1 = new Intent(
				WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
		final Intent intent3=new Intent(
				WifiManager.NETWORK_STATE_CHANGED_ACTION);
		mContext.removeStickyBroadcast(intent1);
		mContext.removeStickyBroadcast(intent3);

		final Intent intent2 = new Intent(
				WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION);
		mContext.removeStickyBroadcast(intent2);

		mConnectReceiver = new NetworkConnectReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
		filter.addAction(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION);
		filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
		mContext.registerReceiver(mConnectReceiver, filter);

	}
	
	// Keep Temporarily
	private void unregisterNetworkChangedReceiver() {
		if (mConnectReceiver != null) {
			mContext.unregisterReceiver(mConnectReceiver);
			mConnectReceiver = null;
		}
		return;
	}
	
	/**
	 * Print the saved access point list.
	 * Applications used for debug.
	 * 
	 * @return
	 */
	private void printSavedList(){
		if (WifiConst.DummyMode){
			return;
		}
		List<WifiConfiguration>list;
		NetLog.d(TAG, "-------------------------------------------------------------------------");
		
		if(mConfigList == null) {
			return;
		}
		
		list = mConfigList;
		for (int i = 0; i < list.size(); i++){
			
			NetLog.d(TAG, "number[" + i + "] "+ list.get(i).SSID + " " + list.get(i).toString());
		}
		
		NetLog.d(TAG, "-------------------------------------------------------------------------");
		return ;
	}
	
	private WifiConfiguration getConfiguration() {
		if(mConfigList == null) {
			return null;
		}
		
		int total = mConfigList.size();
		if(total == 0) {
			NetLog.d(TAG, "Config list is empty. ");
			return null;
		}
		
//		printSavedList();
		
		for(int i=0; i < total; i++) {
			
			if(mConfigList.get(i).status == WifiConfiguration.Status.ENABLED){
				if (localLOGV) NetLog.d(TAG, "[WifiAutoConnect][getConfiguration]: Some SSID Enabled.--");
				
				if(mCurrnetConfig == null) {
					mCurrnetConfig = mConfigList.get(i);
					break;
				}
				
				if(mCurrnetConfig != mConfigList.get(i)) {
					mCurrnetConfig = mConfigList.get(i);
					break;
				}
			}
		}
		
		return mCurrnetConfig;
	}
	
	/**
	 * Auto connect to the Internet use wifi.
	 * Applications should not call this API directly, use com.mediatek.netcm.NetworkManager's 
	 * wifiAutoConnect() instead.
	 * 
	 * @return
	 */
	public void autoConnect() {
		
		mWifiManager.disconnect();
		mConfigList = mWifiManager.getConfiguredNetworks();
		
		WifiConfiguration config = getConfiguration();
		if( (config == null) || (config.networkId == -1)) {
			if (localLOGV) NetLog.d(TAG, "[WifiAutoConnect][autoConnect]: No Config in wpa_supplicant.conf .");
			return;
		}
		
		if (localLOGV) NetLog.v(TAG, "[WifiAutoConnect][autoConnect]: SSID -> " + config.SSID + " network ID -> " + config.networkId);
		
		if(!mWifiManager.enableNetwork(config.networkId, false)) {
			if (localLOGV) NetLog.d(TAG, "[WifiAutoConnect][autoConnect]: Auto Enable Network Failed.");
			return;
		}
		
		 if(mWifiManager.reconnect()) {
			 if (localLOGV) NetLog.d(TAG, "[WifiAutoConnect][autoConnect]: Reconnect Success.");
		 } else {
			 if (localLOGV) NetLog.d(TAG, "[WifiAutoConnect][autoConnect]: Reconnect Failed.");
		 }
	}
	
}
