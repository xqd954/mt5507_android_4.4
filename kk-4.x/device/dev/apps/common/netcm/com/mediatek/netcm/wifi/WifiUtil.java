package com.mediatek.netcm.wifi;

import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;

import com.mediatek.netcm.util.NetLog;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiConfiguration.GroupCipher;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.text.format.Time;

public class WifiUtil {
	private static HttpURLConnection connect = null;
	private static URL url = null;
	private static int code = 0;
	private final static String Testlink = "http://www.google.com";
	private static String TAG = "CM_WifiUtil";
	public static int yearDay = 0;
	public static int hour = 0;
	public static int minute = 0;
	public static int second = 0;
	public static boolean startTimer = false;

	/**
	 * Gets the type of access point's encryption.
	 * 
	 * @param config
	 *            the wifi configuration of the access point waiting to be parsed.
	 *            
	 * @return WifiConst.W_ENCRYPT_WEP;
	 *         WifiConst.W_ENCRYPT_TKIP;
	 *         WifiConst.W_ENCRYPT_AES;
	 *         WifiConst.W_ENCRYPT_NONE;
	 */
	public static int getEncrypt(WifiConfiguration config) {
		if (config.allowedGroupCiphers.get(GroupCipher.WEP40)
				|| config.allowedGroupCiphers.get(GroupCipher.WEP104)) {
			return WifiConst.W_ENCRYPT_WEP;
		}

		if (config.allowedGroupCiphers.get(GroupCipher.TKIP)) {
			return WifiConst.W_ENCRYPT_TKIP;
		}

		if (config.allowedGroupCiphers.get(GroupCipher.CCMP)) {
			return WifiConst.W_ENCRYPT_AES;
		}

		return WifiConst.W_ENCRYPT_NONE;
	}

	/**
	 * Gets the type of access point's encryption.
	 * 
	 * @param result
	 *            the scan result of the access point waiting to be parsed.
	 *            
	 * @return WifiConst.W_ENCRYPT_WEP;
	 *  	   WifiConst.W_ENCRYPT_TKIP_AES;
	 *         WifiConst.W_ENCRYPT_TKIP;
	 *         WifiConst.W_ENCRYPT_AES;
	 *         WifiConst.W_ENCRYPT_NONE;
	 */
	public static int getEncrypt(ScanResult result) {
		if (result.capabilities.contains("WEP")) {
			return WifiConst.W_ENCRYPT_WEP;
		}

		else if (result.capabilities.contains("TKIP+CCMP")) {
			return WifiConst.W_ENCRYPT_TKIP_AES;
		} else if (result.capabilities.contains("TKIP")) {
			return WifiConst.W_ENCRYPT_TKIP;
		} else if (result.capabilities.contains("CCMP")
				|| result.capabilities.contains("AES")) {
			return WifiConst.W_ENCRYPT_AES;
		}

		return WifiConst.W_ENCRYPT_NONE;
	}

	/**
	 * Gets the type of access point's confirm way.
	 * 
	 * @param result
	 *            the scan result of the access point waiting to be parsed.
	 *            
	 * @return WifiConst.W_CONFIRM_OPEN;
	 *  	   WifiConst.W_CONFIRM_WEP;
	 *         WifiConst.W_CONFIRM_PSK_AUTO;
	 *         WifiConst.W_CONFIRM_WPA_PSK;
	 *         WifiConst.W_CONFIRM_WPA2_PSK;
	 *         WifiConst.W_CONFIRM_EAP_AUTO;
	 *         WifiConst.W_CONFIRM_WPA_EAP;
	 *         WifiConst.W_CONFIRM_WPA2_EAP;
	 *         WifiConst.W_CONFIRM_UNKNOWN;
	 */
	public static int getConfirmType(ScanResult result) {
		if (result.capabilities == null || result.capabilities.equals("WPS")) {
			return WifiConst.W_CONFIRM_OPEN;
		}

		else if (result.capabilities.contains("WEP")) {
			return WifiConst.W_CONFIRM_WEP;
		}

		else if (result.capabilities.contains("WPA-PSK")
				&& result.capabilities.contains("WPA2-PSK")) {
			return WifiConst.W_CONFIRM_PSK_AUTO;
		}

		else if (result.capabilities.contains("WPA")
				&& result.capabilities.contains("WPA-PSK")) {
			return WifiConst.W_CONFIRM_WPA_PSK;
		}

		else if (result.capabilities.contains("WPA2")
				&& result.capabilities.contains("WPA2-PSK")) {
			return WifiConst.W_CONFIRM_WPA2_PSK;
		}

		else if (result.capabilities.contains("WPA-EAP")
				&& result.capabilities.contains("WPA2-EAP")) {
			return WifiConst.W_CONFIRM_EAP_AUTO;
		}

		else if (result.capabilities.contains("WPA")
				&& result.capabilities.contains("WPA-EAP")) {
			return WifiConst.W_CONFIRM_WPA_EAP;
		}

		else if (result.capabilities.contains("WPA2")
				&& result.capabilities.contains("WPA2-EAP")) {
			return WifiConst.W_CONFIRM_WPA2_EAP;
		}

		return WifiConst.W_CONFIRM_UNKNOWN;
	}

	public static void clearConfiguration(WifiConfiguration config) {
		config.allowedAuthAlgorithms.clear();
		config.allowedGroupCiphers.clear();
		config.allowedKeyManagement.clear();
		config.allowedPairwiseCiphers.clear();
		config.allowedProtocols.clear();
	}

	
	/**
	 * Check if the preSharedKey is valid or not, this API
	 * used for available AP.
	 * 
	 * @param result, the scanned access point.
	 *        preSharedKey, the pre-shared key, also the password.
	 * 
	 * @return true: is preSharedKey valid   false: is invalid.
	 */
	public static boolean isPskValid(ScanResult result, String preSharedKey) {
		if (result == null || preSharedKey == null) {
			return false;
		}

		int length = preSharedKey.length();
		if (length > 64) {
			return false;
		}

		int encrypt = getEncrypt(result);
		switch (encrypt) {
		case WifiConst.W_ENCRYPT_WEP:
			if (length == 10 || length == 26) {
				if (preSharedKey.matches("[0-9A-Fa-f]*")) {
					return true;
				} else {
					return false;
				}
				
			} else if (length == 5 || length == 13) {
				return true;
			}

			return false;

		case WifiConst.W_ENCRYPT_TKIP:
		case WifiConst.W_ENCRYPT_AES:
		case WifiConst.W_ENCRYPT_TKIP_AES:
			if (length == 64 && preSharedKey.matches("[0-9A-Fa-f]{64}")) {
				return true;
			} else if(length >= 8 && length <= 63) {
				return true;
			}

			return false;
		default:
			break;
		}

		return true;
	}

	/**
	 * Check if the preSharedKey is valid or not, this API
	 * used for manual setting.
	 * 
	 * @param encryptionMethod, the type of access point's encryption. 
	 *        preSharedKey, the pre-shared key, also the password.
	 * 
	 * @return true: is preSharedKey valid   false: is invalid.
	 */
	public static boolean isPskValid(int encryptionMethod, String preSharedKey) {
		if (encryptionMethod < WifiConst.W_ENCRYPT_NONE
				|| encryptionMethod > WifiConst.W_ENCRYPT_TKIP_AES
				|| preSharedKey == null) {
			return false;
		}
		
		int length = preSharedKey.length();
		if (length > 64) {
			return false;
		}
		
		switch (encryptionMethod) {
		case WifiConst.W_ENCRYPT_WEP:
			if (length == 10 || length == 26) {
				if (preSharedKey.matches("[0-9A-Fa-f]*")) {
					return true;
				} else {
					return false;
				}
				
			} else if (length == 5 || length == 13) {
				return true;
			}

			return false;

		case WifiConst.W_ENCRYPT_TKIP:
		case WifiConst.W_ENCRYPT_AES:
		case WifiConst.W_ENCRYPT_TKIP_AES:
			if (length == 64 && preSharedKey.matches("[0-9A-Fa-f]{64}")) {
				return true;
			} else if(length >= 8 && length <= 63) {
				return true;
			} 
			
			return false;
		default:
			break;
		}

		return false;
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

	
	/*hide*/
	public static int getSecurity(WifiConfiguration config) {
		if (config.allowedKeyManagement.get(KeyMgmt.WPA_PSK)) {
			return WifiConst.W_SECURITY_PSK;
		}
		if (config.allowedKeyManagement.get(KeyMgmt.WPA_EAP)
				|| config.allowedKeyManagement.get(KeyMgmt.IEEE8021X)) {
			return WifiConst.W_SECURITY_EAP;
		}

		return (config.wepKeys[0] != null) ? WifiConst.W_SECURITY_EAP
				: WifiConst.W_SECURITY_NONE;
	}

	/*hide*/
	public static int getSecurity(ScanResult result) {
		if (result.capabilities.contains("WEP")) {
			return WifiConst.W_SECURITY_WEP;
		} else if (result.capabilities.contains("PSK")) {
			return WifiConst.W_SECURITY_PSK;
		} else if (result.capabilities.contains("EAP")) {
			return WifiConst.W_SECURITY_EAP;
		}

		return WifiConst.W_SECURITY_NONE;
	}

	/**
	 * Convert the int address to string.
	 * 
	 * @param data
	 *           the int data waiting to be parsed.
	 *            
	 * @return the String which be converted from int data.
	 */
	public static String IntToString(int data) {
		StringBuffer StringBuf = new StringBuffer();
		StringBuf.append(data & 0xff).append('.').append((data >>>= 8) & 0xff)
				.append('.').append((data >>>= 8) & 0xff).append('.').append(
						(data >>>= 8) & 0xff);
		return StringBuf.toString();
	}

	/**
	 * Test network connectivity. This method is try to connect Google.
	 *            
	 * @return true if the operation success, else return false.
	 */
	public static boolean ConnectTest() {
		try {
			url = new URL(Testlink);
			connect = (HttpURLConnection) url.openConnection();
			connect.setRequestMethod("GET");
			connect.setConnectTimeout(6 * 1000);
			code = connect.getResponseCode();
		} catch (Exception e) {
			return false;
		}

		if (code == HttpURLConnection.HTTP_OK) {
			return true;
		} else {
			return false;
		}
	}

	public static boolean pingCompany() {
		try {
			HttpGet request = new HttpGet(Testlink);
			HttpParams httpParams = new BasicHttpParams();
			HttpConnectionParams.setConnectionTimeout(httpParams, 3000);
			HttpClient httpClient = new DefaultHttpClient(httpParams);
			HttpResponse response = httpClient.execute(request);
			int status = response.getStatusLine().getStatusCode();

			if (status == HttpStatus.SC_OK) {
				return true;
			}

			return false;

		} catch (SocketTimeoutException e) {
			return false;
		} catch (ClientProtocolException e) {
			return false;
		} catch (IOException e) {
			return false;
		}

	}

	/**
	 * Test network connectivity. This method is try to ping gateway.
	 *            
	 * @return true if the operation success, else return false.
	 */
	public static boolean pingGateway(Context context) {
		WifiManager wifiManager = (WifiManager) context
				.getSystemService(Context.WIFI_SERVICE);
		DhcpInfo info = wifiManager.getDhcpInfo();
		int routeAddr = info.gateway;
		String gateWay = IntToString(routeAddr);

		try {
			Socket server = new Socket();
			NetLog.d(TAG, "[WifiUtil][pingGateway]: gateWay -> " + gateWay);
			InetSocketAddress address = new InetSocketAddress(gateWay, 80);
			server.connect(address, 5000);
			server.close();
			return true;
		} catch (UnknownHostException e) {
			NetLog.d(TAG, "[WifiUtil][pingGateway]: UnknownHostException. ");
			return false;
		} catch (IOException e) {
			NetLog.d(TAG, "[WifiUtil][pingGateway]: IOException. ");
			return false;
		}
	}

	/**
	 * Test if access point support WPS connect.
	 * 
	 * @param result
	 *           the scan result of the access point waiting to be parsed.
	 *            
	 * @return true if the access point support WPS, else return false.
	 */
	public static boolean isSupportWps(ScanResult result) {

		if (result.capabilities == null) {
			return false;
		}

		if (result.capabilities.contains("WPS")) {
			return true;
		}

		return false;
	}

	public static String convertToQuotedString(String string) {
		return "\"" + string + "\"";
	}

	public static void setTime() {
		startTimer = true;
		Time tm = new Time();
		tm.setToNow();
		hour = tm.hour; // 0-23
		minute = tm.minute;
		second = tm.second;
		yearDay = tm.yearDay;
	}

	public static int calcRssi(int rssi) {

		if (rssi >= WifiConst.MAX_RSSI) {
			return 100;
		} else if (rssi <= WifiConst.MIN_RSSI) {
			return 0;
		} else {
			return (rssi - WifiConst.MIN_RSSI) * 100
					/ (WifiConst.MAX_RSSI - WifiConst.MIN_RSSI);
		}
	}

	/*@{hide}*/
	public static int getChannelNumber(int frequency) {
		for (int i = 0; i < WifiConst.FREQUENCY.length; i++) {
			for (int j = 0; j < WifiConst.FREQUENCY[i].length; j++) {
				if (frequency == WifiConst.FREQUENCY[i][j]) {
					return i;
				}
			}
		}

		return 0;
	}
	
	/*@{hide}*/
	public static int getFrequency(int channelNumber) {
		int length = WifiConst.FREQUENCY.length;
		if(length < channelNumber || channelNumber < 0) {
			return 0;
		}
		
		return WifiConst.FREQUENCY[channelNumber][1];
	}
	
	/*@{hide}*/
	public static String getDriverName(String dongleName) {
		if(dongleName == null){
			return null;
		}
		
		for (int i = 0; i < WifiConst.DRIVERNAME.length; i++) {
			if(WifiConst.DRIVERNAME[i][0].equals(dongleName)) {
				return WifiConst.DRIVERNAME[i][1];
			}
		}
		
		return null;
	}

	/*@{hide}*/
	public static String removeDoubleQuotes(String string) {
		if (string == null) {
			return null;
		}
		int length = string.length();
		if ((length > 1) && (string.charAt(0) == '"')
				&& (string.charAt(length - 1) == '"')) {
			return string.substring(1, length - 1);
		}
		return string;
	}

}
