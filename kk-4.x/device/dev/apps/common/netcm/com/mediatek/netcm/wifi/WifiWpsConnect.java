package com.mediatek.netcm.wifi;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import java.util.ArrayList;
import java.util.List;

import com.mediatek.netcm.util.NetLog;

/**
 * This class use to wifi WPS Operation, only use for WPS connection.
 * 
 */
public class WifiWpsConnect {
	private WifiManager mWifiManager;
	private Context mContext = null;
	private static WifiWpsConnect mWpsConnect = null;
	private WifiWpsScanReceiver mScanReceiver = null;
	private List<ScanResult> mWifiscanResultList;
	List<WifiAccessPoint> mAccessPoints = null;
	
	private static final String TAG = "CM_WifiWpsConnect";
	private static final boolean localLOGV = false;

	
	private WifiWpsConnect(Context context) {
		this.mContext = context;
		this.mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
	}
	
	/**
	 * Create a new WifiWpsConnect instance.
	 * Applications will use only for WPS connect.
	 * 
	 */
	public static WifiWpsConnect getInstance(Context context) {
		if(mWpsConnect == null) {
			mWpsConnect = new WifiWpsConnect(context);
		}
		
		return mWpsConnect;
	}
	
	private boolean getWifiStatus() {
		return mWifiManager.isWifiEnabled();
	}
	
	private void postNotifcation() {
		Intent intent = new Intent(WifiConst.SCAN_READY);
		mContext.sendBroadcast(intent);
	}

	/**
	 * Register scan broadcast receiver for scan operation.
	 * 
	 */
	public void registerWpsScanReceiver() {
		if(WifiConst.DummyMode) {
			return;
		}
		NetLog.d(TAG, "[WifiWpsConnect][registerWpsScanReceiver]: Register WPS Scan Receiver.");
		mScanReceiver = new WifiWpsScanReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
		mContext.registerReceiver(mScanReceiver, filter);
	}
	
	private void updateAccessPoints(List<ScanResult> scanResultList) {
		List<WifiAccessPoint> accessPoints = new ArrayList<WifiAccessPoint>();
		
		WifiInfo wifiInfo = mWifiManager.getConnectionInfo();
		int ip = wifiInfo.getIpAddress();
		String ssid = wifiInfo.getSSID();
        
		List<ScanResult> results = scanResultList;
		if (results != null){
			for(ScanResult result: results){
				if (result.SSID == null || result.SSID.length() == 0 ||
						result.capabilities.contains("[IBSS]")){
					continue;
				}
				boolean found = false;
				for (WifiAccessPoint accessPoint: accessPoints){
					if (accessPoint.update(result)){
						found = true;
					}
				}
				if (!found){
					WifiAccessPoint accessPoint = new WifiAccessPoint(result);
					ssid = WifiUtil.removeDoubleQuotes(ssid);
					
					if (ssid != null && result.SSID.compareTo(ssid) == 0 && ip > 0){
						accessPoint.setConnectStatus(WifiConst.CONNCETED);
					}
					accessPoints.add(accessPoint);
				}
			}
		}
		mAccessPoints = accessPoints;
	}
	
	private class WifiWpsScanReceiver extends BroadcastReceiver {
		public void onReceive(Context context, Intent intent) {
			final String action = intent.getAction();
			if (action.equals(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
				mWifiscanResultList = mWifiManager.getScanResults();
				
				updateAccessPoints(mWifiscanResultList);
				postNotifcation();
			}
		}
	}

	/**
	 * Unregister scan broadcast receiver.
	 * 
	 */
	public void unregisterWpsScanReceiver() {
		if(WifiConst.DummyMode) {
			return;
		}
		
		NetLog.d(TAG, "[WifiWpsConnect][unregisterWpsScanReceiver]: Unregister WPS Scan Receiver.");
		if (mScanReceiver != null) {
			mContext.unregisterReceiver(mScanReceiver);
			mScanReceiver = null;
		}

		return;
	}
	
	/**
	 * Request a scan for access points. Return immediately. The availability
	 * of the results is made known later by means of an asynchronous event sent
	 * on completion of the scan. Applications should register the 'SCAN_READY' 
	 * receiver to receive the scan ready message.
	 * 
	 * @return Return true if the operation succeeded, else return false.
	 */
	public boolean StartScan() {
		if (mScanReceiver == null || !getWifiStatus()) {
			return false;
		}

		return mWifiManager.startScan();
	}
	
	/**
	 * Return the results of the latest access point scan. These latest access point 
	 * should support WPS connect.
	 * 
	 * @return the list of access points found in the most recent scan and should support
	 * WPS connect.
	 */
	public List<WifiAccessPoint> getWpsScanResultList() {
		if(WifiConst.DummyMode) {
			return null;
		}
		
		if(mAccessPoints == null) {
			return null;
		}
		
		int i = 0;
		int total = mAccessPoints.size();
		if (localLOGV) NetLog.d(TAG, "[WifiWpsConnect][getWpsScanResultList]: Scan Result List size: " + total);
		for(i = 0; i < total; i++) {
			if(!WifiUtil.isSupportWps(mAccessPoints.get(i).getAccessResult())) {
				if (localLOGV) 	NetLog.d(TAG, "[WifiWpsConnect][getWpsScanResultList]: " +  mWifiscanResultList.get(i).SSID +
						" does not support WPS. " + "REMOVE!!!");
			
				mAccessPoints.remove(i);
				total--;
			}
		}
		
		return mAccessPoints;
	}
	
	/**
	 * Print the scan result list.
	 * Applications used for debug.
	 * 
	 */
	public void printScanList(){
		List<WifiAccessPoint>list;
	
		list = mAccessPoints;
		for (int i = 0; i < list.size(); i++){
			NetLog.d(TAG, "number[" + i + "] "+ list.get(i).getSsid() + " " + list.get(i).getAccessResult().toString());
		}
		
		return ;
	}
	
	/**
	 * Disassociate from the currently active access point. This may result
	 * in the asynchronous delivery of state change events.
	 * 
	 * @return Return true if the operation succeeded.
	 */
	public boolean disConnect() {
		return mWifiManager.disconnect();
	}
	
	/**
	 * Remove all networks from the list of configured networks.
	 * 
	 * @return Return true if the operation succeeded.
	 */
	public boolean removeAllNetwork() {
		return false;
	}
	
	/**
	 * This API packing the 'disConnect()' and 'removeAllNetwork()',
	 * Application used for preparing to connect.
	 * 
	 * @return
	 */
	public void prepareToConnect() {
		
		return;
	}
	
	/**
	 * Disconnect current WPS connection.
	 * 
	 * @return {@code true} if the operation succeeds, or else failed.
	 */
	public boolean wpsCancel() {
//		return mWifiManager.wpsCancel();
		return true;
	}
	
	/**
	 * Use WPS PBC mode link to access point. This may result in the asynchronous 
	 * delivery of state change events. Applications should register the 
	 * 'SUPPLICANT_STATE_CHANGED_ACTION', 'NETWORK_STATE_CHANGED_ACTION' 
	 * and 'WPS_CONNECTION_CHANGED_ACTION' receiver to receive the state change events.
	 * 
	 * @return Return true if the operation succeeds, or else failed.
	 * 
	 */
	public boolean EnableWpsPbcLink() {
		if(WifiConst.DummyMode) {
			return true;
		}
		
//		return mWifiManager.associateWpsPbc();
		return true;
	}
	
	/**
	 * Get PIN code for WPS PIN mode.
	 * 
	 * @return The PIN code, in the form of a eight-byte: XXXXXXXX.
	 */
	public String getPinCode() {
		if(WifiConst.DummyMode) {
			return null;
		}
		
//		return (mWifiManager.getPinCode() == null) ? null : mWifiManager.getPinCode();
		return null;
	}
	
	/**
	 * Use WPS PIN mode link to access point. This may result in the asynchronous 
	 * delivery of state change events. Applications should register the 
	 * 'SUPPLICANT_STATE_CHANGED_ACTION', 'NETWORK_STATE_CHANGED_ACTION' 
	 * and 'WPS_CONNECTION_CHANGED_ACTION' receiver to receive the state change events.
	 * 
	 * @return Return true if the operation succeeds, or else failed.
	 */
	public boolean EnableWpsPinLink(String Bssid, String pinCode) {
		if(WifiConst.DummyMode) {
			return true;
		}
		
		if(pinCode == null || Bssid == null) {
			if (localLOGV) NetLog.d(TAG, "[WifiWpsConnect][EnableWpsPinLink]: Argument error.");
			return false;
		}
		
//		return mWifiManager.associateWpsPin(Bssid, pinCode);
		return true;
	}
	
	/**
	 * Tell the supplicant to save the current linked access point.
	 * 
	 */
	public void saveConfiguaration() {
		mWifiManager.saveConfiguration();
	}
}
