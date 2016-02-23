package com.mediatek.netcm.wifi;

/**
 * This class use to notify the dongle plug-in and out action which called
 * by WifiDevManager.
 * 
 */
public interface WifiDevListener {
	
	/**
	 * Notify the application wifi dongle found.
	 *
	 */
    public void notifyWifiDevFound(WifiDevEvent event);
    
	/**
	 * Notify the application wifi dongle left.
	 *
	 */
    public void notifyWifiDevLeft(WifiDevEvent event);
    
}

