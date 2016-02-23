package com.mediatek.netcm.wifi;

import com.mediatek.dm.DeviceManager;
import com.mediatek.dm.DeviceManagerEvent;
import com.mediatek.dm.DeviceManagerListener;
import com.mediatek.netcm.util.NetLog;

/**
 * This Class use for listening to wifi device plug in and plug out.
 * This class is a service, application should start this service when system
 * boot up.
 * Usage:
 * 1. Application should register a service and receiver in 'AndroidManifest.xml'.
 *    example:
 *        <service android:name="com.mediatek.netcm.wifi.WifiDevService" />
 *        <receiver android:name="com.mediatek.ui.menu.util.BootReceiver">
			<intent-filter>
				<action android:name="android.intent.action.BOOT_COMPLETED" />
			</intent-filter>
		</receiver>
 * 2. When BootReceiver receive the 'BOOT_COMPLETED'  broadcast, application should start
 *    this service.
 *    example:
 *    public void onReceive(Context context, Intent intent) {
 *    	     if (action.equals(Intent.ACTION_BOOT_COMPLETED)){
 *  			Intent intentService = new Intent(context, WifiDevService.class);
				context.startService(intentService);
 *         }
 *    }      
 */
public class WifiDevManager implements DeviceManagerListener {
	private static WifiDevManager mWifiDevManager;
	private static DeviceManager mDeviceManager;
	private WifiDevListener mWifiDevListener;
	private String mDevName;
	private static String TAG = "CM_WifiDevManager";

	private WifiDevManager() {
		mDeviceManager = DeviceManager.getInstance();
	}

	/**
	 * Create a new WifiDevManager instance.
	 * Applications will use for listening to wifi device plug in and plug out.
	 * 
	 */
	public static WifiDevManager getInstance() {
		if (mWifiDevManager == null) {
			synchronized (WifiDevManager.class) {
				if (mWifiDevManager == null) {
					mWifiDevManager = new WifiDevManager();
				}
			}
		}

		return mWifiDevManager;
	}

	private void onWifiDevFound(DeviceManagerEvent event) {
		String deviceName = event.getWifiInterface();
		mDevName = event.getWifiInterface();
		mWifiDevListener.notifyWifiDevFound(new WifiDevEvent(deviceName));
	}

	private void onWifiDevLeft(DeviceManagerEvent event) {
		String deviceName = event.getWifiInterface();
		if (deviceName == null) {
			NetLog.d(TAG, "[WifiDevManager][onWifiDevLeft]: Plug OUT  name = NULL.");
		} else {
				NetLog.d(TAG, "[WifiDevManager][onWifiDevLeft]: Plug OUT -> "
						+ deviceName);
		}

		mDevName = null;
		mWifiDevListener.notifyWifiDevLeft(new WifiDevEvent(deviceName));
	}

	public void onEvent(DeviceManagerEvent event) {

		int type = event.getType();
		switch (type) {
		case DeviceManagerEvent.wificonnected:
			NetLog.d(TAG, "[WifiDevManager][onEvent]:  WIFI connected.");
			onWifiDevFound(event);
			break;

		case DeviceManagerEvent.wifidisconnected:
			NetLog.d(TAG, "[WifiDevManager][onEvent]:  WIFI disconnected.");
			onWifiDevLeft(event);
			break;

		default:
			NetLog.d(TAG, "[WifiDevManager][onEvent]:  WIFI NULL NULL.");
			break;
		}
	}

	/**
	 * Get the Current wifi device name. Such as 'wlan0', etc
	 * @return the current wifi device name. 
	 */
	public String getCurrentDevName() {
		return mDevName;
	}

	/**
	 * Set on wifi device event lister.
	 * 
	 */
	public void setOnDevEventListener(WifiDevListener listener) {
		mDeviceManager.addListener(mWifiDevManager);
		this.mWifiDevListener = listener;
	}
	
	/**
	 * Set off wifi device event lister.
	 * 
	 */
	public void setOffDevEventListener() {
		mDeviceManager.removeListener(mWifiDevManager);
		this.mWifiDevListener = null;
	}

}
