package com.mediatek.netcm.wifi;


public class WifiDevEvent {
	private String mDevName;
	
	/**
	 * Create a new WifiDevEvent instance.
	 * 
	 */
	public WifiDevEvent(String deviceName) {
		this.mDevName = deviceName;
	}

	/**
	 * Get the wifi device name when event happen. Such as 'wlan0', etc
	 * @return the wifi device name. 
	 */
	public String getDevName() {
		return mDevName;
	}
	
}
