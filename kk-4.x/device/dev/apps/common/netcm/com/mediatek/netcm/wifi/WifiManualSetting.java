package com.mediatek.netcm.wifi;

import java.util.List;

import com.mediatek.netcm.util.NetLog;

import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiConfiguration.AuthAlgorithm;
import android.net.wifi.WifiConfiguration.GroupCipher;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.net.wifi.WifiConfiguration.PairwiseCipher;
import android.net.wifi.WifiConfiguration.Protocol;

/**
 * This class use to wifi Operation, only use for manual setting.
 * 
 */
public class WifiManualSetting {
	private Context mContext = null;
	private WifiManager mWifiManager;
	private boolean mIsWepMode = false;
	private static WifiManualSetting mWifiManualSetting = null;

	public String TAG = "CM_WifiManualSetting";
	private static final boolean localLOGV = false;
	private List<WifiConfiguration> mWifiConfigList = null;

	private WifiManualSetting(Context context) {
		this.mContext = context;
		this.mWifiManager = (WifiManager) mContext
				.getSystemService(Context.WIFI_SERVICE);
	}

	private WifiManualSetting(Context context, String text) {

	}

	/**
	 * Create a new WifiManualSetting instance.
	 * Applications will use only for manual setting.
	 * 
	 */
	public static WifiManualSetting getInstance(Context context) {
		if (WifiConst.DummyMode) {
			if (mWifiManualSetting == null) {
				mWifiManualSetting = new WifiManualSetting(context, null);
			}
			return mWifiManualSetting;
		}

		if (mWifiManualSetting == null) {
			mWifiManualSetting = new WifiManualSetting(context);
		}
		return mWifiManualSetting;
	}

	/*@{hide}*/
	private boolean isConfigured(WifiConfiguration config) {
		if (WifiConst.DummyMode) {
			return false;
		}
		mWifiConfigList = mWifiManager.getConfiguredNetworks();
		int total = mWifiConfigList.size();
		for (int i = 0; i < total; i++) {
			if (mWifiConfigList.get(i).SSID.equals(WifiUtil
					.convertToQuotedString(config.SSID)) && 
					mWifiConfigList.get(i).allowedKeyManagement.equals(config.allowedKeyManagement)) {
				return true;
			}
		}

		return false;
	}
	
	/*@{hide}*/
	private int getNetworkId(WifiConfiguration config) {
		mWifiConfigList = mWifiManager.getConfiguredNetworks();
		int total = mWifiConfigList.size();
		for (int i = 0; i < total; i++) {
			if (mWifiConfigList.get(i).SSID.
					equals(WifiUtil.convertToQuotedString(config.SSID))) {
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
		//return mWifiManager.removeAllNetwork();
		return false;
	}
	
	/**
	 * This API packing the 'disConnect()' and 'removeAllNetwork()',
	 * Application used for preparing to connect.
	 * 
	 * @return
	 */
	public void prepareToConnect() {
		mWifiManager.disconnect();
		//mWifiManager.removeAllNetwork();
		
		return;
	}
	
	/**
	 * Enable the access point which has no security. This may result in the asynchronous
	 * delivery of state change events. Applications should register the 
	 * 'SUPPLICANT_STATE_CHANGED_ACTION' and 'NETWORK_STATE_CHANGED_ACTION' receiver to receive 
	 * the state change events.
	 * 
	 * @param One of the access point which want to link.
	 * @return Return true if the operation succeed, false otherwise.
	 */
	public boolean manualEnableNetworkLink(String ssid, int validationWay,
			int EncryptionMethod) {
		if (WifiConst.DummyMode) {
			return false;
		}

		if (ssid == null) {
			return false;
		}
		
		if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][manualEnableNetworkLink]: Enable Access Point with no Password.");
		WifiConfiguration config = new WifiConfiguration();
		WifiUtil.clearConfiguration(config);

		config.SSID = WifiUtil.convertToQuotedString(ssid);
		config.status = WifiConfiguration.Status.ENABLED;
		config.hiddenSSID = true;

		switch (validationWay) {
		case WifiConst.W_CONFIRM_OPEN:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_OPEN");
			config.allowedAuthAlgorithms.clear();
			config.allowedPairwiseCiphers.clear();
			config.allowedGroupCiphers.clear();
			config.allowedKeyManagement.set(KeyMgmt.NONE);
			config.allowedProtocols.clear();
			break;

		case WifiConst.W_CONFIRM_SHARE:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_SHARE");
			config.allowedAuthAlgorithms.clear();
			config.allowedPairwiseCiphers.clear();
			config.allowedGroupCiphers.clear();
			config.allowedKeyManagement.set(KeyMgmt.NONE);
			config.allowedProtocols.clear();
			break;

		default:
			return false;
		}

		int networkId;
//		if (!isConfigured(config)) {
			networkId = mWifiManager.addNetwork(config);
//			mWifiManager.updateNetwork(config);
//		} else {
//			networkId = getNetworkId(config);
//		}

		if (networkId != -1 && mWifiManager.enableNetwork(networkId, false)) {
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][manualEnableNetworkLink]: Enable network link success.");

			mWifiManager.reconnect();
			
			return true;
		}

		return false;
	}

	/**
	 * Enable the access point which has security. This may result in the asynchronous
	 * delivery of state change events. Applications should register the 
	 * 'SUPPLICANT_STATE_CHANGED_ACTION' and 'NETWORK_STATE_CHANGED_ACTION' receiver to receive 
	 * the state change events.
	 * 
	 * @param One of the access point which want to link.
	 * @return Return true if the operation succeed, false otherwise.
	 */
	public boolean manualEnableNetworkLink(String ssid, int confirmType,
			int EncryptionMethod, String password) {
		if (WifiConst.DummyMode) {
			return false;
		}

		if (ssid == null || password == null) {
			return false;
		}

		if (confirmType == WifiConst.W_CONFIRM_UNKNOWN) {
			return false;
		}
		
		if( ((confirmType == WifiConst.W_CONFIRM_OPEN) && 
				(EncryptionMethod != WifiConst.W_ENCRYPT_WEP)) 
				|| ((confirmType == WifiConst.W_CONFIRM_SHARE)
						&& (EncryptionMethod != WifiConst.W_ENCRYPT_WEP))) {
			return false;
		}
		
		if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][manualEnableNetworkLink]: Enable Access Point with Password.");

		int networkId = 0;
		WifiConfiguration config = null;
		int passwordLength = 0;
		passwordLength = password.length();
		
		config = new WifiConfiguration();
		
		WifiUtil.clearConfiguration(config);
		config.SSID = WifiUtil.convertToQuotedString(ssid);
		config.status = WifiConfiguration.Status.ENABLED;
		config.hiddenSSID = true;
		
		switch (confirmType) {
		case WifiConst.W_CONFIRM_OPEN:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_OPEN");
			config.allowedKeyManagement.set(KeyMgmt.NONE);
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedPairwiseCiphers.set(PairwiseCipher.NONE);
			config.allowedProtocols.clear();
			break;
			
		case WifiConst.W_CONFIRM_SHARE:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_SHARE");
			config.allowedKeyManagement.set(KeyMgmt.NONE);
			config.allowedAuthAlgorithms.set(AuthAlgorithm.SHARED);
			config.allowedPairwiseCiphers.set(PairwiseCipher.NONE);
			config.allowedProtocols.clear();
			break;

		case WifiConst.W_CONFIRM_WEP:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_WEP");
			config.allowedKeyManagement.set(KeyMgmt.NONE);
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedAuthAlgorithms.set(AuthAlgorithm.SHARED);
//			config.allowedPairwiseCiphers.set(PairwiseCipher.NONE);
			config.allowedProtocols.clear();
			break;

		case WifiConst.W_CONFIRM_WPA_PSK:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_WPA_PSK");
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedProtocols.set(Protocol.WPA);
			config.allowedKeyManagement.set(KeyMgmt.WPA_PSK);
			break;
			
		case WifiConst.W_CONFIRM_WPA2_PSK:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_WPA2_PSK");
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedProtocols.set(Protocol.RSN);
			config.allowedKeyManagement.set(KeyMgmt.WPA_PSK);
			break;	
			
		case WifiConst.W_CONFIRM_PSK_AUTO:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_PSK_AUTO");
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedProtocols.set(Protocol.RSN);
			config.allowedProtocols.set(Protocol.WPA);
			config.allowedKeyManagement.set(KeyMgmt.WPA_PSK);
			break;
			
		case WifiConst.W_CONFIRM_WPA_EAP:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_WPA_EAP");
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedProtocols.set(Protocol.WPA);
			config.allowedKeyManagement.set(KeyMgmt.WPA_EAP);
			break;
			
		case WifiConst.W_CONFIRM_WPA2_EAP:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_WPA2_EAP");
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedProtocols.set(Protocol.RSN);
			config.allowedKeyManagement.set(KeyMgmt.WPA_EAP);
			break;
			
		case WifiConst.W_CONFIRM_EAP_AUTO:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM_EAP_AUTO");
			config.allowedAuthAlgorithms.set(AuthAlgorithm.OPEN);
			config.allowedProtocols.set(Protocol.RSN);
			config.allowedProtocols.set(Protocol.WPA);
			config.allowedKeyManagement.set(KeyMgmt.WPA_EAP);
			break;
			
		default:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_CONFIRM default.");
			break;
			
		}
		
		switch (EncryptionMethod) {
		case WifiConst.W_ENCRYPT_WEP:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_ENCRYPT_WEP");
//			if(passwordLength == 5 || passwordLength == 10) {
//				config.allowedGroupCiphers.set(GroupCipher.WEP40);
//			} else {
//				config.allowedGroupCiphers.set(GroupCipher.WEP104);
//			}
			config.wepTxKeyIndex = 0;
			mIsWepMode = true; 
			break;
			
		case WifiConst.W_ENCRYPT_TKIP:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_ENCRYPT_TKIP");
			config.allowedGroupCiphers.set(GroupCipher.TKIP);
			config.allowedGroupCiphers.set(GroupCipher.CCMP);
			config.allowedPairwiseCiphers.set(PairwiseCipher.TKIP);
			break;
			
		case WifiConst.W_ENCRYPT_AES:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_ENCRYPT_AES");
			config.allowedGroupCiphers.set(GroupCipher.TKIP);
			config.allowedGroupCiphers.set(GroupCipher.CCMP);
			config.allowedPairwiseCiphers.set(PairwiseCipher.CCMP);
			break;
			
		case WifiConst.W_ENCRYPT_TKIP_AES:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_ENCRYPT_TKIP_AES");
			config.allowedGroupCiphers.set(GroupCipher.TKIP);
			config.allowedGroupCiphers.set(GroupCipher.CCMP);
			config.allowedPairwiseCiphers.set(PairwiseCipher.TKIP);
			config.allowedPairwiseCiphers.set(PairwiseCipher.CCMP);
			break;

		default:
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: W_ENCRYPT default. ");
			break;
		}

		if(mIsWepMode) {
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: WEP encrypt.");
			if(( passwordLength == 5 || passwordLength == 10 || passwordLength == 13|| 
					passwordLength == 26) && password.matches("[0-9A-Fa-f]*")) {
				config.wepKeys[0] = password;
			} else {
				config.wepKeys[0] = '"' + password + '"';
			}
		}
		else {
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][EnableNetworkLink]: other encrypt.");
			if (password.matches("[0-9A-Fa-f]{64}")) {
				config.preSharedKey = password;
			} else {
				config.preSharedKey = "\"" + password + "\"";
			}
		}
		
		networkId = mWifiManager.addNetwork(config);

		if (networkId != -1 && mWifiManager.enableNetwork(networkId, false)) {
			
			if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][manualEnableNetworkLink]: Enable network link SUCCESS.");
			mWifiManager.reconnect();
			
			return true;
		} 
		
		if (localLOGV) NetLog.d(TAG, "[WifiManualSetting][manualEnableNetworkLink]: Enable network link failed.");
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
