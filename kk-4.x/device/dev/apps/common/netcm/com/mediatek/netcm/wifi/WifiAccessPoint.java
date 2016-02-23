package com.mediatek.netcm.wifi;

import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;

/**
 * Inner class to get the access point.
 * 
 */
public class WifiAccessPoint {
	
	final String ssid;
	final int security;
	final int networkId;
	
	private int mRssi;
	public ScanResult mResult;
	private WifiConfiguration mConfig;
	private int connectStatus = WifiConst.DISCONNECT;
	
	protected WifiAccessPoint(ScanResult result) {
		ssid = result.SSID;
		security = WifiUtil.getEncrypt(result);
		networkId = -1;
		mRssi = result.level;
		mResult = result;
	}
	
	protected WifiAccessPoint(WifiConfiguration config) {
    
        ssid = (config.SSID == null ? "" : WifiUtil.removeDoubleQuotes(config.SSID));
        security = WifiUtil.getSecurity(config);
        networkId = config.networkId;
        mConfig = config;
        mRssi = Integer.MAX_VALUE;
    }
	
	public ScanResult getAccessResult() {
		return mResult;
	}
	
	public WifiConfiguration getConfig() {
		return mConfig;
	}
	public void setConnectStatus(int status){
		connectStatus = status;
	}
	
	public int getConnectStatus(){
		return connectStatus;
	}
	
	public String getSsid(){
		return ssid;
	}
	
	public int getSignalStrength(){
	
		return mRssi;
//		return WifiUtil.calcRssi(mRssi);
	}
	
	public int getProtocol(){
		
		return WifiUtil.getConfirmType(mResult);
	}
	
	public int getRssi(){
		return mRssi;
	}
	
	public int getEncrypt(){
		return security;
	}
	
	public String getWifiProtocal(){
		int channel = WifiUtil.getChannelNumber(mResult.frequency);
		//802.11n
		if (channel == 3 || channel == 11){		
			return "N";
		}
		
		//802.11g/n
		else if (channel == 1 || channel == 5 || channel == 9 || channel == 13){
			return "N'G";
		}
		
		//802.11b
//		else if (channel == 1 || channel == 6 || channel == 11 || channel == 14){
//			
//		}
		
		return "N'G";
	}
    
	boolean update(ScanResult result){
		if (ssid.equals(result.SSID) && security == WifiUtil.getEncrypt(result)){
			if (WifiManager.compareSignalLevel(result.level, mRssi) > 0){
				mRssi = result.level;
			}
			return true;
		}
		return false;
	}
}
