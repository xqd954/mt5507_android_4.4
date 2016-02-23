package com.mediatek.netcm.wifi;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.File;

import com.mediatek.netcm.util.NetLog;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.text.format.Time;


/**
 * This class use to get wifi Status, provide get related information capabilites.
 * <ul>
 * <li> IP, Net Mask, DNS, MAC.</li>
 * <li> Channel, Frequency, Confirm Type.</li>
 * </ul>
 */
public class WifiStatus {
	private WifiManager mWifiManager;
	private static WifiStatus mWifiStatus = null;
	private Context mContext = null;
	private WifiInfo mWifiInfo = null;
	
	private static final String STR_INTERFACE_ATH0 = "ath0";
	private static final String STR_INTERFACE_REA0 = "rea0";
	private static final String STR_INTERFACE_WLAN0 = "wlan0";
	
	
	private String TAG = "CM_WifiStatus";
	public static String TYPE = "Base Architecture";
	public static int frequency;
	// /sys/module/ar6000/version 
	private static final String mNdisPathPrefix = "/sys/module";
	private static final String mNdisFile = "version";
	
	private WifiStatus(Context context) {
		mContext = context;
		mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
	}
	
	private WifiStatus(Context context, String text){
		
	}
	
	/**
	 * Create a new WifiStatus instance.
	 * Applications will use for getting current link status.
	 * 
	 */
	public static WifiStatus getInstance(Context context) {
		if(WifiConst.DummyMode){
			if(mWifiStatus == null) {
				mWifiStatus = new WifiStatus(context, null);
			}
			
			return mWifiStatus;
		}
		
		if(mWifiStatus == null) {
			mWifiStatus = new WifiStatus(context);
		}
		
		return mWifiStatus;
	}
	
	/**
	 * Check if the wifi dongle is connect to access point or not.
	 * 
	 * @return true if wifi connect to access point, otherwise false.
	 */
	public boolean isWifiConnected() {
		mWifiInfo = mWifiManager.getConnectionInfo();
		SupplicantState state = mWifiInfo.getSupplicantState();
		
		if(state == SupplicantState.COMPLETED || state == SupplicantState.ASSOCIATED || 
				state == SupplicantState.ASSOCIATING) {
			return true;
		}
		
		return false;
	}
	
	/**
	 * Get current connect's channel number.
	 * 
	 * @return the channel number.
	 */
	public int getChannal(){
		return 0;
	}
	
	/**
	 * Get the wifi status.
	 * {@hide} Do not use this API, use getConnectStatus() instead.
	 * @return true if wifi dongle connect to access point, else return false.
	 */
	public boolean getWifiStatus() {
		//return mWifiManager.isWifiEnabled();
		if(WifiConst.DummyMode){
			return false;
		}
		
		return getIp() == 0 ? false : true;
	}
	
	/**
	 * Get current link's speed.
	 * 
	 * @return the link's speed.
	 */
	public int getLinkSpeed() {
		if(WifiConst.DummyMode){
			return 0;
		}
		
		mWifiInfo = mWifiManager.getConnectionInfo();
	
		return (mWifiInfo == null) ? 0 : mWifiInfo.getLinkSpeed();
	}
	
	/**
	 * Get current link's base architecture.
	 * 
	 * @return the link's base architecture.
	 */
	public String getType() {
		return TYPE;
	}
	
	/**
	 * Gets the type of current link's encryption.
	 * 
	 * @return the type of current link's encryption.
	 */
	public int getEncryptType(){
		return 0;
	}
	
	/**
	 * Gets the current link's ssid.
	 * 
	 * @return the current link's ssid.
	 */
	public String getSsid(){
		if(WifiConst.DummyMode){
			return null;
		}
		
		return null;
	}
	
	
	/**
	 * Gets the version of NDIS.
	 * 
	 * @return the version of NDIS.
	 */
	public String getNdisVersion() {
		
		File file = null;
		BufferedReader bufferedReader = null;
		String dongleName = null;
		
		int wifi0 = mWifiManager.wifi0Check();
		switch (wifi0) {
		case 0:
			dongleName = STR_INTERFACE_ATH0;
			break;
		case 1:
			dongleName = STR_INTERFACE_REA0;
			break;
		case 2:
			dongleName = STR_INTERFACE_WLAN0;
			break;

		default:
			return null;
		}
		
		String dirverName = WifiUtil.getDriverName(dongleName);
		if(dirverName == null) {
			return null;
		}
		
	    String NdisFilePath =  mNdisPathPrefix.concat("/").concat(dirverName).concat("/").concat(mNdisFile);
	    NetLog.d(TAG, "[WifiStatus][getNdisVersion]: NdisFilePath ->  " + NdisFilePath);
	    try {
	    	file = new File(NdisFilePath);
			if(!file.exists()) {
				NetLog.d(TAG, "[WifiStatus][getNdisVersion]: file does not exist! ");
				return null;
			}
			
			bufferedReader = new BufferedReader(new FileReader(file));
			
			String line = bufferedReader.readLine();
			if(line == null) {
				return null;
			}
			
			NetLog.d(TAG, "[WifiStatus][getNdisVersion]: line -- >  " + line);
			return line;
			
	    } catch(IOException e) {
	    	return null;
	    }
	}
	
	/**
	 * Gets the type of current link's confirm method.
	 * 
	 * @return the type of current link's confirm method.
	 */
	public int getConfirmType(){
		return 0;
	}
	
	/**
	 * Gets the type of current link's channel.
	 * 
	 * @return the type of current link's channel.
	 */
	public String getChannelSetting() {
		if(WifiConst.DummyMode){
			return "ETSI";
		}
		
//		int number = mWifiManager.getNumAllowedChannels();
//
//		/* North America */
//		if (number == 11){
//			return "FCC";
//		}
//		
//		/* Europe */ 
//		else if (number == 13){
//			return "ETSI";
//		}
		
		return "ETSI";
	}
	
	/**
	 * Get the type of current link's frequency.
	 * 
	 * @return the type of current link's frequency.
	 */
	public int getFrequency() {
		return 0;
	}
	
	/**
	 * Get the time of the current link.
	 * 
	 * @return the time of the current link.
	 */
	public String getWorkTime() {
		if(WifiConst.DummyMode){
			return "00:00:00";
		}
		
		if (!WifiUtil.startTimer){
			return "00:00:00";
		}
		
		NetLog.d(TAG, "[WifiStatus][getWorkTime]: Get work time." );
		
		Time tm = new Time();
		tm.setToNow();
		int hour = (tm.yearDay - WifiUtil.yearDay)* 24;
		hour = hour + tm.hour - WifiUtil.hour;
		hour -= 1;
		int minute = tm.minute - WifiUtil.minute + 60;
		minute -= 1;
		int second = tm.second - WifiUtil.second + 60;

		if (second >= 60){
			second -= 60;
			minute += 1;
		}
		if (minute >= 60){
			minute -= 60;
			hour += 1;
		}
		
		StringBuffer hourBuffer = null;
		StringBuffer minuteBuffer = null;
		StringBuffer secondBuffer = null;
		
			
		if (hour < 10) {
			hourBuffer = new StringBuffer().append("0").append(hour);
		}
		if (minute < 10) {
			minuteBuffer = new StringBuffer().append("0").append(minute);
		}
		if (second < 10) {
			secondBuffer = new StringBuffer().append("0").append(second);
		}

		return new StringBuffer().append(hourBuffer == null ? hour: hourBuffer).
			append(":").append(minuteBuffer == null ? minute : minuteBuffer).append(":").
			append(secondBuffer == null ? second : secondBuffer).toString();
	}
	
	/**
	 * Get the level of current link's signal.
	 * 
	 * @return the level of current link's signal.
	 */
	public int getSignalLevel(){
		return 0;
	}
	
	/**
	 * Get the quality of current link's signal.
	 * 
	 * @return the quality of current link's signal.
	 */
	public int getLinkQuality(){
		if(WifiConst.DummyMode){
			return 0;
		}
		
		return 0;
	}
	
	
	/**
	 * Get the current link's MAC address.
	 * 
	 * @return the current link's MAC address.
	 */
	public String getMacAddr(){
		if(WifiConst.DummyMode){
			return null;
		}
		
		String macAddr = null;
		
		mWifiInfo = mWifiManager.getConnectionInfo();
		if(mWifiInfo == null) {
			return null;
		}
		
		macAddr = mWifiInfo.getMacAddress();
		return macAddr;
	}
	
	public String getBssid() {
		return null;
	}
	
	/**
	 * Get the current link's IP address.
	 * 
	 * @return the current link's IP address.
	 */
	public String getIpAddr(){
		if(WifiConst.DummyMode){
			return null;
		}
		int ipAddr = 0;
		
		mWifiInfo = mWifiManager.getConnectionInfo();
		if(mWifiInfo == null) {
			return null;
		}
		
		ipAddr = mWifiInfo.getIpAddress();
		return WifiUtil.IntToString(ipAddr);
	}
	
	private int getIp(){
		if(WifiConst.DummyMode){
			return 0;
		}
		int ipAddr = 0;
		
		mWifiInfo = mWifiManager.getConnectionInfo();
		if(mWifiInfo == null) {
			return 0;
		}
		
		ipAddr = mWifiInfo.getIpAddress();
		return ipAddr;
	}
	
	
	/**
	 * Get the wifi status.
	 * @return true if wifi dongle connect to access point, else return false.
	 */
	public boolean getConnectStatus(){
		if(WifiConst.DummyMode){
			return false;
		}
		
		mWifiInfo = mWifiManager.getConnectionInfo();
		if(mWifiInfo == null) {
			return false;
		}
		return mWifiInfo.getIpAddress() == 0 ? false : true;
	}
	
	/**
	 * Get the current link's net mask address.
	 * 
	 * @return the current link's net mask address.
	 */
	public String getNetMask(){
		if(WifiConst.DummyMode){
			return null;
		}
		
		DhcpInfo info = mWifiManager.getDhcpInfo();
		if(info == null) {
			return null;
		}
		
		return WifiUtil.IntToString(info.netmask);
	}
	
	/**
	 * Get the current link's DNS address.
	 * 
	 * @return the current link's DNS address.
	 */
	public String getDnsAddr(){
		if(WifiConst.DummyMode){
			return null;
		}

		DhcpInfo info = mWifiManager.getDhcpInfo();
		if(info == null) {
			return null;
		}
		
		return WifiUtil.IntToString(info.dns1);
	}
	
	/**
	 * Get the current link's alternate DNS address.
	 * 
	 * @return the current link's alternate DNS address.
	 */
	public String getDns2Addr(){
		if(WifiConst.DummyMode){
			return null;
		}
		
		DhcpInfo info = mWifiManager.getDhcpInfo();
		if(info == null) {
			return null;
		}

		return WifiUtil.IntToString(info.dns2);
	}
	/**
	 * Get the current link's get way address.
	 * 
	 * @return the current link's get way address.
	 */
	public  String getRouteAddr(){
		if(WifiConst.DummyMode){
			return null;
		}
		
		DhcpInfo info = mWifiManager.getDhcpInfo();
		if(info == null) {
			return null;
		}
		
		return WifiUtil.IntToString(info.gateway);
		
	}
}
