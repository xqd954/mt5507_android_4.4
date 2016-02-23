package com.mediatek.netcm.wifi;

public class WifiConst {
	
	public static final boolean DummyMode = false;
	
	//CONNECT STATUS
	public static int CONNCETED = 0;
	public static int DISCONNECT = 1;
	
	//RSSI
	public static int MAX_RSSI = -55;
	public static int MIN_RSSI = -100;
	
	//Encrypt
	public static final int W_ENCRYPT_NONE = 0;
	public static final int W_ENCRYPT_WEP = 1;
	public static final int W_ENCRYPT_TKIP = 2;
	public static final int W_ENCRYPT_AES = 3;
	public static final int W_ENCRYPT_TKIP_AES = 4;

	//Security
	public static final int W_SECURITY_NONE = 0;
	public static final int W_SECURITY_WEP = 1;
	public static final int W_SECURITY_PSK = 2;
	public static final int W_SECURITY_EAP = 3;

	//State
	public static final int W_STATE_DISABLING = 0;
	public static final int W_STATE_DISABLED = 1;
	public static final int W_STATE_ENABLING = 2;
	public static final int W_STATE_ENABLED = 3;
	public static final int W_STATE_UNKNOWN = 4;
	
	//Confirm
	public static final int W_CONFIRM_UNKNOWN   = 0;
	public static final int W_CONFIRM_OPEN      = 1;
	public static final int W_CONFIRM_SHARE     = 2;
	public static final int W_CONFIRM_PSK_AUTO  = 3;
	public static final int W_CONFIRM_WEP       = 4;
	public static final int W_CONFIRM_WPA_PSK   = 5;
	public static final int W_CONFIRM_WPA2_PSK  = 6;
	public static final int W_CONFIRM_EAP_AUTO  = 7;
	public static final int W_CONFIRM_WPA_EAP   = 8;
	public static final int W_CONFIRM_WPA2_EAP  = 9;
	
	//supplicant 
	public static final int W_SUPPLICANT_CONNECTED = 1;
	public static final int W_SUPPLICANT_DISCONNECTED = 2;
	public static final int W_SUPPLICANT_FAILED = 3;

	public final static String SCAN_READY = "com.mediatek.netcm.wifi.SCAN_RESULTS_AVAILABLE_ACTION";
	public final static String STATE_CHANGED = "com.mediatek.netcm.wifi.STATE_CHANGED_ACTION";
	public final static String WIFI_STATE = "wifi_state";
	public final static String SUPPLICANT_STATE = "supplicant_state";
	public final static String SUPPLICANT_STATE_CHANGED = "com.mediatek.netcm.wifi.SUPPLICANT_STATE_CHANGED";

	public final static String NETWORK_CHANGED = "com.mediatek.netcm.wifi.NETWORK_STATE_CHANGED";
	
	// frequency Table
	public static  int[] FREQUENCY[] = {
		//LOWER  CENTER UPPER  (MHZ)
		{0, 	0, 		0}, // CHANNAL NUMBER
		{2401,	2412,	2423}, //1
		{2404,	2417,	2428}, //2
		{2411,	2422,	2433},//3
		{2416,	2427,	2438},//4
		{2421,	2432,	2443},//5
		{2426,	2437,	2448},//6
		{2431,	2442,	2453},//7
		{2436,	2447,	2458},//8
		{2441,	2452,	2463},//9
		{2451,	2457,	2468},//10
		{2451,	2462,	2473},//11
		{2456,	2467,	2478},//12
		{2461,	2472,	2483},//13
		{2473,	2484,	2495},//14		
	};
	
	public static String[] DRIVERNAME[] = {
		{"wlan0", "ar6000"},
	};
	
}
