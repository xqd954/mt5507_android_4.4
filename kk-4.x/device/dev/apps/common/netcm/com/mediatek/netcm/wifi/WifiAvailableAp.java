package com.mediatek.netcm.wifi;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import com.mediatek.netcm.util.NetLog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiConfiguration.AuthAlgorithm;
import android.net.wifi.WifiConfiguration.GroupCipher;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.net.wifi.WifiConfiguration.PairwiseCipher;
import android.net.wifi.WifiConfiguration.Protocol;
import android.os.Handler;
import android.os.Message;

/**
 * This class use to wifi Operation, only use in avail AP.
 * <ul>
 * <li> Scan Access Point.</li>
 * <li> Connect to Access Point.</li>
 * </ul>
 */
public class WifiAvailableAp {
	private WifiManager mWifiManager;
	private static WifiAvailableAp mWifiScanAp = null;
	private Context mContext = null;
	private List<ScanResult> mWifiscanResultList;
	private WifiScanReceiver mScanReceiver = null;
	private List<WifiConfiguration> mWifiConfigList = null;
	List<WifiAccessPoint> mAccessPoints = null;
	
	private final int PING_SUPPLICANT_SUCCESS = 1;
	private final int PING_SUPPLICANT_FAILED = 2;
	
	private static final String TAG = "CM_WifiAvailableAp";
	private static final boolean localLOGV = true;
	
	private WifiAvailableAp(Context context) {
		this.mContext = context;
		this.mWifiManager = (WifiManager) mContext
				.getSystemService(Context.WIFI_SERVICE);
	}

	private WifiAvailableAp(Context context, String text) {
		
	}
	
	/**
	 * Create a new WifiAvailableAp instance.
	 * Applications will use only for available AP.
	 * 
	 */
	public static WifiAvailableAp getInstance(Context context) {
		if (WifiConst.DummyMode){
			if (mWifiScanAp == null) {
				mWifiScanAp = new WifiAvailableAp(context, null);
			}
			
			return mWifiScanAp;
		}
		
		if (mWifiScanAp == null) {
			mWifiScanAp = new WifiAvailableAp(context);
		}
		return mWifiScanAp;
	}

	/**
	 * Return whether Wi-Fi is enabled or disabled.
	 * 
	 */
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
	public void registerScanReceiver() {
		if (WifiConst.DummyMode){
			return;
		}
		
		mScanReceiver = new WifiScanReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
		mContext.registerReceiver(mScanReceiver, filter);
	}
	
	public void registerScanReceiver(Context context) {
		if (WifiConst.DummyMode){
			return;
		}
		
		mScanReceiver = new WifiScanReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
		context.registerReceiver(mScanReceiver, filter);
	}
	
	private class WifiScanReceiver extends BroadcastReceiver {
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
	public void unregisterScanReceiver() {
		if (WifiConst.DummyMode){
			return;
		}
		
		if (mScanReceiver != null) {
			mContext.unregisterReceiver(mScanReceiver);
			mScanReceiver = null;
		}

		return;
	}
	
	
	public void unregisterScanReceiver(Context context) {
		if (WifiConst.DummyMode){
			return;
		}
		
		if (mScanReceiver != null) {
			context.unregisterReceiver(mScanReceiver);
			mScanReceiver = null;
		}

		return;
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

	
	Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case PING_SUPPLICANT_SUCCESS:
				mWifiManager.startScan();
				break;
				
			case PING_SUPPLICANT_FAILED:
				//open supplicant failed
				break;
				
			default:
				break;
			}
		}
	};
	
	class pingSupplicantThread extends Thread {
		public pingSupplicantThread() {
			super("pingSupplicantThread");
		}
		
		public void run() {
			if(pingSupplicant()) {
				mHandler.sendEmptyMessage(PING_SUPPLICANT_SUCCESS);
				return;
			} else {
				mHandler.sendEmptyMessage(PING_SUPPLICANT_FAILED);
				return;
			}
		}
		
		private boolean pingSupplicant() {
			int connectTries = 0;
			while(true) {
				if(mWifiManager.pingSupplicant()){
					return true;
				}
				
				if(connectTries ++ < 4) {
					nap(1);
				} else {
					break;
				}
			}
			
			return false;
		}
		
		private void nap(int secs) {
			try{
				Thread.sleep(secs * 1000);
			}catch (InterruptedException ignore) {
			}
		}
	}
	

	
	/**
	 * Request a scan for access points. Return immediately. The availability
	 * of the results is made known later by means of an asynchronous event sent
	 * on completion of the scan. Applications should register the 'SCAN_READY' 
	 * receiver to receive the scan ready message.
	 * If application first time to use this function, it need to make sure the supplicant
	 * is ready. To check the supplicant's status, application must call 'pingSupplicant' of
	 * WifiDongleControl class.
	 * 
	 * @return Return true if the operation succeeded, else return false.
	 */
	public boolean StartScan() {
		if (WifiConst.DummyMode){
			return false;
		}
				
		if (mScanReceiver == null || !getWifiStatus()) {
			if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][StartScan]: Wifi dongle status wrong.");
			return false;
		}
		
		pingSupplicantThread thread = new pingSupplicantThread();
		thread.start();
		
		return true;
	}

	/**
	 * Print the scan result list.
	 * Applications used for debug.
	 * 
	 */
	public void printScanList(){
		if (WifiConst.DummyMode){
			return;
		}
		List<ScanResult>list;
		
		list = mWifiscanResultList;
		
		if(list == null) {
			return;
		}
		
		NetLog.d(TAG, "In common logic,  getScanResults total ---- > " + list.size());
		for (int i = 0; i < list.size(); i++){
			
//			Log.d(TAG, "number[" + i + "] "+ list.get(i).SSID + " " + list.get(i).toString());
			NetLog.d(TAG, "number[" + i + "] "+ list.get(i).SSID);
		}
		
		return ;
	}
	
	/**
	 * Return the results of the latest access point scan.
	 * 
	 * @return the list of access points found in the most recent scan.
	 */
	public List<ScanResult> getScanResultList() {
		if (WifiConst.DummyMode){
			return null;
		}
		return this.mWifiscanResultList;
	}
	
	
	/**
	 * Print the scan result list which filter the same ssid by common logic.
	 * Applications used for debug.
	 * 
	 */
	public void printAccessPointsList(){
		if (WifiConst.DummyMode){
			return;
		}
		List<WifiAccessPoint>list;
		list = mAccessPoints;
		// null!!!
		if (list != null && list.size() >= 0) {
			NetLog.d(TAG,
					"Filtered by common logic,  getScanAccessPoints total ---- > "
							+ list.size());
			for (int i = 0; i < list.size(); i++) {

				NetLog.d(TAG, "number[" + i + "] " + list.get(i).ssid);
			}
		}
		return ;
	}
	/**
	 * Return the results of the latest access point scan without repeated.
	 * @return the list of access points found in the most recent scan.
	 */
	public List<WifiAccessPoint> getScanAccessPoints(){
		if (WifiConst.DummyMode){
			return null;
		}
		
		if (localLOGV) {
			printScanList();
			printAccessPointsList();
		}
		return mAccessPoints;
	}

    /**
     * Sort the access point by signal strength.
     * 
     * @param the list of access point found by scan.
     * @return the list of access point which sorted by signal strength.
     */
    public List<WifiAccessPoint> sortBySignal(List<WifiAccessPoint>  list) {
    	if(list == null) {
    		return null;
    	}
    	
        Collections.sort(list, new Comparator<WifiAccessPoint>() {
            public int compare(WifiAccessPoint object1, WifiAccessPoint object2) {
            	Integer left  = new Integer(object1.getRssi());
            	Integer right = new Integer(object2.getRssi());   	
            	
                return -left.compareTo(right);
            }
        });
        
        return list;
    }
	
	
	private boolean isConfigured(ScanResult result) {	
		if (WifiConst.DummyMode){
			return false;
		}
		
		mWifiConfigList = mWifiManager.getConfiguredNetworks();
		
		int total = mWifiConfigList.size();
		for (int i = 0; i < total; i++) {
			
			if (mWifiConfigList.get(i).SSID.
					equals(WifiUtil.convertToQuotedString(result.SSID))) {
				return true;
			}
		}	
		return false;
	}

	private int getNetworkId(ScanResult result) {
		mWifiConfigList = mWifiManager.getConfiguredNetworks();
		int total = mWifiConfigList.size();
		for (int i = 0; i < total; i++) {
			if (mWifiConfigList.get(i).SSID.
					equals(WifiUtil.convertToQuotedString(result.SSID))) {
				return mWifiConfigList.get(i).networkId;
			}
		}

		return -1;
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
		/*mWifiManager.disconnect();
		mWifiManager.removeAllNetwork();
		
		return;*/
	}

	/**
	 * Enable the scan access point which has no security. This may result in the asynchronous
	 * delivery of state change events. Applications should register the 
	 * 'SUPPLICANT_STATE_CHANGED_ACTION' and 'NETWORK_STATE_CHANGED_ACTION' receiver to receive 
	 * the state change events.
	 * 
	 * @param One of the access point which want to link.
	 * @return Return true if the operation succeed, false otherwise.
	 */
	public boolean EnableNetworkLink(ScanResult result) {
		if (WifiConst.DummyMode){
			return false;
		}
		
		int networkId = 0;
		if (result == null) {
			return false;
		}
		
		if (isConfigured(result)) {
			if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: " + result.SSID + " has been configured.");
			networkId = getNetworkId(result);
		} else {
			if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: " + result.SSID + " has not been configured.");
			WifiConfiguration config = new WifiConfiguration();
			
			config.SSID = WifiUtil.convertToQuotedString(result.SSID);
			config.allowedKeyManagement.set(KeyMgmt.NONE);
			config.status = WifiConfiguration.Status.ENABLED;
			networkId = mWifiManager.addNetwork(config);
			mWifiManager.updateNetwork(config);

		}

		if (networkId != -1 && mWifiManager.enableNetwork(networkId, true)) {			
			mWifiManager.reconnect();

			if (localLOGV)
				NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: enable network link success.");
			return true;
		}

		return false;
	}


	/**
	 * Enable the scan access point which has security. This may result in the asynchronous
	 * delivery of state change events. Applications should register the 
	 * 'SUPPLICANT_STATE_CHANGED_ACTION' and 'NETWORK_STATE_CHANGED_ACTION' receiver to receive 
	 * the state change events.
	 * 
	 * @param One of the access point which want to link.
	 * @return Return true if the operation succeed, false otherwise.
	 */
	public boolean EnableNetworkLink(ScanResult result, String sectretKey) {
		if (WifiConst.DummyMode){
			return false;
		}
		
		int networkId = 0;
		int keyLength = 0;
		
		keyLength = sectretKey.length();
		WifiConfiguration config = null;
		if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: Enable Access Point with Password.");
		if (result == null || sectretKey == null) {
			return false;
		}
		
		
		config = new WifiConfiguration();
		
		WifiUtil.clearConfiguration(config);
		config.status = WifiConfiguration.Status.ENABLED;
			
			config.SSID = WifiUtil.convertToQuotedString(result.SSID);
			
			int encrypt = WifiUtil.getEncrypt(result);
			
			if(encrypt == WifiConst.W_ENCRYPT_WEP) {
				
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: WEP encrypt.");
				if(( keyLength == 5 || keyLength == 10 || keyLength == 13|| 
						keyLength == 26) && sectretKey.matches("[0-9A-Fa-f]*")) {
					config.wepKeys[0] = sectretKey;
				} else {
					config.wepKeys[0] = '"' + sectretKey + '"';
				}
			}
			else {
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: other encrypt.");
				if (sectretKey.matches("[0-9A-Fa-f]{64}")) {
					config.preSharedKey = sectretKey;
				} else {
					config.preSharedKey = "\"" + sectretKey + "\"";
				}
			}
			
			int ConfirmType = WifiUtil.getConfirmType(result);
			switch (ConfirmType) {
			case WifiConst.W_CONFIRM_OPEN: 
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_OPEN");
				config.allowedKeyManagement.set(KeyMgmt.NONE);
				config.allowedAuthAlgorithms.clear();
				config.allowedPairwiseCiphers.set(PairwiseCipher.NONE);
				config.allowedGroupCiphers.clear();
				config.allowedProtocols.clear();
				break;
				
			case WifiConst.W_CONFIRM_WEP:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_WEP");
				config.allowedKeyManagement.set(KeyMgmt.NONE);
				config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
				config.allowedAuthAlgorithms.set(AuthAlgorithm.SHARED);
				config.allowedPairwiseCiphers.set(PairwiseCipher.NONE);
				
				if(keyLength == 5 || keyLength == 10) {
					config.allowedGroupCiphers.set(GroupCipher.WEP40);
				} else {
					config.allowedGroupCiphers.set(GroupCipher.WEP104);
				}
				
				config.allowedProtocols.clear();
				break;
				
			case WifiConst.W_CONFIRM_WPA_PSK:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_WPA_PSK");
				config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
				config.allowedProtocols.set(Protocol.WPA);
				config.allowedKeyManagement.set(KeyMgmt.WPA_PSK);
				break;
				
			case WifiConst.W_CONFIRM_WPA2_PSK:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_WPA2_PSK");
				config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
				config.allowedProtocols.set(Protocol.RSN);
				config.allowedKeyManagement.set(KeyMgmt.WPA_PSK);
				break;
				
			case WifiConst.W_CONFIRM_PSK_AUTO:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_PSK_AUTO");
				config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
				config.allowedProtocols.set(Protocol.RSN);
				config.allowedProtocols.set(Protocol.WPA);
				config.allowedKeyManagement.set(KeyMgmt.WPA_PSK);
				break;
				
			case WifiConst.W_CONFIRM_WPA_EAP:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_WPA_EAP");
				config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
				config.allowedProtocols.set(Protocol.WPA);
				config.allowedKeyManagement.set(KeyMgmt.WPA_EAP);
				break;
				
			case WifiConst.W_CONFIRM_WPA2_EAP:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_WPA2_EAP");
				config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
				config.allowedProtocols.set(Protocol.RSN);
				config.allowedKeyManagement.set(KeyMgmt.WPA_EAP);
				break;
				
			case WifiConst.W_CONFIRM_EAP_AUTO:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_CONFIRM_EAP_AUTO");
				config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
				config.allowedProtocols.set(Protocol.RSN);
				config.allowedProtocols.set(Protocol.WPA);
				config.allowedKeyManagement.set(KeyMgmt.WPA_EAP);
				break;
				
			default:
				break;
			}
			
			
			switch (encrypt) {
			case WifiConst.W_ENCRYPT_WEP:
				break;
				
			case WifiConst.W_ENCRYPT_TKIP:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_ENCRYPT_TKIP");
				config.allowedGroupCiphers.set(GroupCipher.TKIP);
				config.allowedPairwiseCiphers.set(PairwiseCipher.TKIP);
				break;
				
			case WifiConst.W_ENCRYPT_AES:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_ENCRYPT_AES");
				config.allowedGroupCiphers.set(GroupCipher.CCMP);
				config.allowedPairwiseCiphers.set(PairwiseCipher.CCMP);
				break;
				
			case WifiConst.W_ENCRYPT_TKIP_AES:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_ENCRYPT_TKIP_AES");
				config.allowedGroupCiphers.set(GroupCipher.TKIP);
				config.allowedGroupCiphers.set(GroupCipher.CCMP);
				config.allowedPairwiseCiphers.set(PairwiseCipher.TKIP);
				config.allowedPairwiseCiphers.set(PairwiseCipher.CCMP);
				break;

			default:
				if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: W_ENCRYPT default.");
				break;
			}
			
			networkId = mWifiManager.addNetwork(config);
			if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: 398  networkId -> " + networkId);
			mWifiManager.updateNetwork(config);


		if (networkId != -1 && mWifiManager.enableNetwork(networkId, true)) {
			if (localLOGV) NetLog.d(TAG, "[WifiAvailableAp][EnableNetworkLink]: Enable network link SUCCESS.");

			mWifiManager.reconnect();
			
			return true;
		}

		return false;
	}
	
	/**
	 * Tell the supplicant to save the current linked access point.
	 * 
	 */
	public void saveConfiguaration() {
		mWifiManager.saveConfiguration();
	}
	
}
