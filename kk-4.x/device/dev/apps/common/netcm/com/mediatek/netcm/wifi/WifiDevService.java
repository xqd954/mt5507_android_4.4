package com.mediatek.netcm.wifi;

import com.mediatek.dm.DeviceManager;
import com.mediatek.dm.DeviceManagerEvent;
import com.mediatek.dm.DeviceManagerListener;
import com.mediatek.netcm.util.NetLog;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Binder;
import android.os.IBinder;

/**
 * Wifi device serive used when wifi usb device plug in
 * or plug out. 
 * When wifi usb device plug in, this service will start wifi,
 * and when wifi usb device plug out, this service will stop wifi.
 * 
 */
public class WifiDevService extends Service {
	private final LocalBinder mBinder = new LocalBinder();
	private WifiManager mWifiManager;
	private static DeviceManager mDeviceManager;
	private static  Object syncRoot = new Object();
	 
	private static String TAG = "CM_WifiDevService";
	
	private Thread mWifiInitThread = new Thread(new Runnable() {		
		public void run() {
			NetLog.d(TAG, "[WifiDevService][mWifiInitThread]: Open wifi dongle.");
			if (!mWifiManager.isWifiEnabled()){
				NetLog.d(TAG, "[WifiDevService][mWifiInitThread]: Wifi Dongle is not Enable.");
				mWifiManager.setWifiEnabled(true);
			}
		}
	});
	
	private Thread mWifiDeinitThread = new Thread(new Runnable() {		
		public void run() {
			NetLog.d(TAG, "[WifiDevService][mWifiDeinitThread]: Close wifi dongle.");
			if (mWifiManager.isWifiEnabled()){
				NetLog.d(TAG, "[WifiDevService][mWifiDeinitThread]: Wifi Dongle is Enable.");
				mWifiManager.setWifiEnabled(false);
			}
		}
	});
	
	
	private DeviceManagerListener mDevListener = new DeviceManagerListener() { 
		public void onEvent(DeviceManagerEvent event) {
			int type = event.getType();
			switch (type) {
			case DeviceManagerEvent.wificonnected:
				NetLog.d(TAG, "[WifiDevService][onEvent]:  WIFI connected.");
				synchronized (syncRoot) {
					if (!mWifiManager.isWifiEnabled()){
						NetLog.d(TAG, "[WifiDevService][mWifiInitThread]: Wifi Dongle is not Enable.");
						//mWifiManager.setWifiEnabled(true);
					}
				}
				break;

			case DeviceManagerEvent.wifidisconnected:
				NetLog.d(TAG, "[WifiDevService][onEvent]:  WIFI disconnected.");
				synchronized (syncRoot) {
					if (mWifiManager.isWifiEnabled()){
						NetLog.d(TAG, "[WifiDevService][mWifiDeinitThread]: Wifi Dongle is Enable.");
						//mWifiManager.setWifiEnabled(false);
					}
				}
				
				break;
			case DeviceManagerEvent.productconnected:
				NetLog.d(TAG, "[WifiDevService][mWifiDeinitThread]:mark start call wifiIsDongleIn.");
			//	mWifiManager.wifiIsDongleIn();
				NetLog.d(TAG, "[WifiDevService][mWifiDeinitThread]: finish call wifiIsDongleIn.");
                break;
			case DeviceManagerEvent.productdisconnected:
				NetLog.d(TAG, "[WifiDevService][mWifiDeinitThread]: start call wifiIsDongleOut.");
			//	mWifiManager.wifiIsDongleOut();
				NetLog.d(TAG, "[WifiDevService][mWifiDeinitThread]: finish call wifiIsDongleOut.");
                break;

			default:
				NetLog.d(TAG, "[WifiDevService][onEvent]:  WIFI NULL NULL.");
				break;
			}
		}	
	};
	
	
	public class LocalBinder extends Binder {
		public WifiDevService getService() {
			return WifiDevService.this;
		}
	}
	
	public IBinder onBind(Intent intent) {
		return mBinder;
	}
	
	 public void onCreate(){
		 NetLog.d(TAG, "[WifiDevService][onCreate]:  onCreat.");
		 this.mWifiManager = (WifiManager) this.getSystemService(Context.WIFI_SERVICE);
		 this.mDeviceManager = DeviceManager.getInstance();
		// mWifiManager.wifiIsDongleIn();
		 NetLog.d(TAG, "[WifiDevService][onCreate]:  wifiIsDongleIn, it put the pid to a list and load the driver.");
		// mWifiManager.wifi0Check();
		 NetLog.d(TAG, "[WifiDevService][onCreate]:  wifi0Check");
		 //if (!mWifiManager.isWifiEnabled()){
		 NetLog.d(TAG, "[WifiDevService][onCreate]: mWifiManager.checkAndStartWifi()");
		 //mWifiManager.setWifiEnabled(true);
		// mWifiManager.checkAndStartWifi();
		 
	 }

	
	public void onDestroy() {
		super.onDestroy();
		mDeviceManager.removeListener(mDevListener);
	}

	public int onStartCommand(Intent intent, int flags, int startId) {
		 super.onStartCommand(intent, flags, startId);
		 mDeviceManager.addListener(mDevListener);
		 return START_REDELIVER_INTENT;
	}
	 
}
