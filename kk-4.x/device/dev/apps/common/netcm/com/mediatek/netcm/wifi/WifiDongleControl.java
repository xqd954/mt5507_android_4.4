package com.mediatek.netcm.wifi;

import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;

/**
 * This class use to control the wifi dongle.
 * <ul>
 * <li> Judge if dongle exist.</li>
 * <li> Open and Close wifi.</li>
 * </ul>
 * 
 */
public class WifiDongleControl {
	/**
	 * Ping supplicant success message. 
	 * Application should register this receiver When call 'pingSupplicant()'.
	 * 
	 */
	public static final String PING_SUPPLICANT_SUCCESS = "com.mediatek.netcm.wifi.PING_SUPPLICANT_SUCCESS";
	
	/**
	 * Ping supplicant failed message. 
	 * Application should register this receiver When call 'pingSupplicant()'.
	 * 
	 */
	public static final String PING_SUPPLICANT_FAILED = "com.mediatek.netcm.wifi.PING_SUPPLICANT_FAILED";
	
	private WifiManager mWifiManager;
	private static WifiDongleControl mDongleControl = null;
	private Context mContext = null;
	
	private WifiDongleControl(Context context) {
		this.mContext = context;
		this.mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
	}
	
	private WifiDongleControl(Context context, String text){
		
	}
	
	/**
	 * Create a new WifiDongleControl instance.
	 * Applications will use for control the wifi dongle.
	 * 
	 */
	public static WifiDongleControl getInstance(Context context) {
		if(WifiConst.DummyMode){
			if(mDongleControl == null) {
				mDongleControl = new WifiDongleControl(context, null);
			}
			return mDongleControl;
		}
		
		if(mDongleControl == null) {
			mDongleControl = new WifiDongleControl(context);
		}
		return mDongleControl;
	}
	
	/**
	 * Check if wifi dongle exist or not.
	 * @return Return true wifi dongle is exist, else return false. 
	 */
	public boolean isWifiDongleExist() {
		/*if(WifiConst.DummyMode) {
			return true;
		}

		return mWifiManager.isDongleExist();*/
		return false;
	}
	
	/**
	 * Check if wifi dongle exist or not, and return the current type of dongle.
	 * 
	 * @return {@code -1} Wifi dongle does not exist.
	 *         {@code  0} Atheros 'ath0'
	 *         {@code  1} Realtek 'rea0'
	 *         {@code  2} Ralink  'wlan0'
	 */
	public int wifi0Check() {
		if(WifiConst.DummyMode) {
			return -1;
		}
		
		return mWifiManager.wifi0Check();
	}
	
	
	/**
	 * Gets the Wi-Fi enabled state.
	 * @return WifiConst.W_STATE_UNKNOWN;
	 *         WifiConst.W_STATE_DISABLING;
	 *         WifiConst.W_STATE_DISABLED;
	 *         WifiConst.W_STATE_ENABLING;
	 *         WifiConst.W_STATE_ENABLED;
	 */
	public int getWifiState() {
		if(WifiConst.DummyMode){
			return WifiConst.W_STATE_UNKNOWN;
		}
		
		int wifiState = mWifiManager.getWifiState();
		switch(wifiState) {
		case WifiManager.WIFI_STATE_DISABLING:
				return WifiConst.W_STATE_DISABLING;
				
		case WifiManager.WIFI_STATE_DISABLED:
				return WifiConst.W_STATE_DISABLED;
				
		case WifiManager.WIFI_STATE_ENABLING:
			return WifiConst.W_STATE_ENABLING;
			
		case WifiManager.WIFI_STATE_ENABLED:
			return WifiConst.W_STATE_ENABLED;
			
		default:
			return WifiConst.W_STATE_UNKNOWN;
		}
	}
	
	
	/**
	 * If Wi-Fi disabled, enable it.
	 * @return Return true if the operation succeeds (or if the existing state is
	 *         the same as the requested state). 
	 */
	public boolean OpenWifi() {
		if(WifiConst.DummyMode){
			return false;
		}
		
		if (!mWifiManager.isWifiEnabled()) {
			return mWifiManager.setWifiEnabled(true);
		} else {
			return true;
		}
	}

	/**
	 * If Wi-Fi enabled, disable it.
	 * @return Return true if the operation succeeds (or if the existing state is
	 *         the same as the requested state). 
	 */
	public boolean CloseWifi() {
		if(WifiConst.DummyMode){
			return false;
		}
		
		if (!mWifiManager.isWifiEnabled()) {
			return true;
		} else{
			if(mWifiManager.setWifiEnabled(false)) {
				return true;
			}
		}
		
		return false;
	}
	
	/**
	 * Check that the supplicant daemon is responding to requests.
	 * @return true if we were to able to communciate with the supplicant and
	 * it returned the exppected reponse to the PING message.
	 */
	public boolean checkSupplicant() {
		return mWifiManager.pingSupplicant();
	}
}
