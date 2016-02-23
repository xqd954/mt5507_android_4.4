package com.mediatek.netcm;


import com.mediatek.netcm.util.NetLog;
import com.mediatek.netcm.wifi.WifiAutoConnect;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ethernet.EthernetManager;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.Message;

/**
 * This Class use to auto connect network when system start up.
 * <ul>
 * <li> Default is use ethernet.</li>
 * <li> Auto connect to wifi.</li>
 * </ul>
 */
public class NetworkManager {
	
	private final int STOP_ETHERNET_SUCCESS = 1;
	private final int WIFI0_CHECK_SUCCESS = 2;
	private final int WIFI0_CHECK_FAILED = 3;
	private final int SUPPLICANT_START_SUCCESS = 4;
	private final int SUPPLICANT_START_FAILED = 5;
	private  WifiManager mWifiManager;
	private  EthernetManager mEthManager;
	private  WifiAutoConnect mWifiAutoConnect;
	private Context mContext = null;
	private boolean mIsEthStop = true;
	private static final boolean localLOGV = false;
	private static NetworkManager mNetworkManager = null;
	
	private String TAG = "CM_networkManager";
	
	private final BroadcastReceiver mEthStopReceiver = new BroadcastReceiver() {
		
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if(action.equals(EthernetManager.ETHERNET_STOP_INTERFACE_ACTION)) {
				if(mIsEthStop){
					NetLog.d(TAG, "[NetworkManager][onReceive]: Close Ethernet Success.");
					mHandler.sendEmptyMessage(STOP_ETHERNET_SUCCESS);
					mIsEthStop = false;
				}
			}
		}
	};
	
	private final BroadcastReceiver mWifiOpenReceiver = new BroadcastReceiver() {
		
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if(action.equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
				final int wifiState = intent.getIntExtra(
						WifiManager.EXTRA_WIFI_STATE,
						WifiManager.WIFI_STATE_UNKNOWN);
				switch(wifiState) {
				case WifiManager.WIFI_STATE_ENABLED:
					if (localLOGV)NetLog.d(TAG, "[NetworkManager][mWifiOpenReceiver]: Open WIFI Success.");
					new checkSupplicantThread().start();
					break;
					
					default:
						break;
				}
			}
		}
	};
	
	Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case STOP_ETHERNET_SUCCESS: 
				if (localLOGV)NetLog.d(TAG, "[NetworkManager][handleMessage]: Handle Stop Interface Message.");
				openWifi();
				break;
			case WIFI0_CHECK_SUCCESS:
				if (localLOGV)NetLog.d(TAG, "[NetworkManager][handleMessage]: WIFI0_CHECK_SUCCESS.");
				doWifiConnect();
				break;
				
			case WIFI0_CHECK_FAILED:
				if (localLOGV)NetLog.d(TAG, "[NetworkManager][handleMessage]: WIFI0_CHECK_FAILED.");
				break;
				
			case SUPPLICANT_START_SUCCESS:
				if (localLOGV)NetLog.d(TAG, "[NetworkManager][handleMessage]: SUPPLICANT_START_SUCCESS");
				mWifiAutoConnect.autoConnect();
				break;
				
			case SUPPLICANT_START_FAILED:
				if (localLOGV)NetLog.d(TAG, "[NetworkManager][handleMessage]: SUPPLICANT_START_FAILED");
				openWifi();
				break;
				
			default:
				break;
			}
		}
	};
	
	
	private NetworkManager(Context context) {
		this.mContext = context;
		this.mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
		this.mEthManager = (EthernetManager) mContext.getSystemService(Context.ETHERNET_SERVICE);
		this.mWifiAutoConnect = WifiAutoConnect.getInstance(mContext);
	}
	
	/**
	 * Create a new NetworkManager instance.
	 * Applications will use for network manage when system start up.
	 * 
	 */
	public static NetworkManager getInstance(Context context) {
		if (mNetworkManager == null) {
			mNetworkManager = new NetworkManager(context);
		}
		return mNetworkManager;
	}
	
	private void openWifi() {
		if (localLOGV)NetLog.d(TAG, "[NetworkManager][openWifi]: Open wifi operation.");
		IntentFilter filter = new IntentFilter();
		filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
		mContext.registerReceiver(mWifiOpenReceiver, filter);
		mWifiManager.setWifiEnabled(true);
		return;
	}
	
	
	private void closeEthernet() {
		if (localLOGV)NetLog.d(TAG, "[NetworkManager][closeEthernet]: In Close Ethernet Function.");
		int state = mEthManager.getState();
		if(state == EthernetManager.ETHERNET_STATE_UNKNOWN || 
				state == EthernetManager.ETHERNET_STATE_DISABLED) {
			if (localLOGV)NetLog.d(TAG, "[NetworkManager][closeEthernet]: Ethernet has been Closed.");
		}
		
		if (localLOGV)NetLog.d(TAG, "[NetworkManager][closeEthernet]: Ethernet has not been Closed.");
		IntentFilter filter = new IntentFilter();
		filter.addAction(EthernetManager.ETHERNET_STOP_INTERFACE_ACTION);
		mContext.registerReceiver(mEthStopReceiver, filter);
		mEthManager.setEnabled(false);
	}
	
	public void closeEther(){
		int state = mEthManager.getState();
		if(state == EthernetManager.ETHERNET_STATE_UNKNOWN || 
				state == EthernetManager.ETHERNET_STATE_DISABLED) {
			if (localLOGV)NetLog.d(TAG, "[NetworkManager][closeEthernet]: Ethernet has been Closed.");
		}
		mEthManager.setEnabled(false);
	}
	
	
	private void doWifiConnect() {
		int WifiState = mWifiManager.getWifiState();
		int EthState = mEthManager.getState();
		
		if(EthState == EthernetManager.ETHERNET_STATE_ENABLED) {
			if (localLOGV)NetLog.d(TAG, "[NetworkManager][wifiAutoConnect]: Eth0 is open.");
			closeEthernet();
		} 
		
		else if( (EthState == EthernetManager.ETHERNET_STATE_DISABLED || 
				EthState == EthernetManager.ETHERNET_STATE_UNKNOWN ) && 
				(WifiState != WifiManager.WIFI_AP_STATE_ENABLED) ) {
			if (localLOGV)NetLog.d(TAG, "[NetworkManager][wifiAutoConnect]: Wifi Dongle does not open.");
			openWifi();
		} 
		
		else if(WifiState == WifiManager.WIFI_AP_STATE_ENABLED) {
			if (localLOGV)NetLog.d(TAG, "[NetworkManager][wifiAutoConnect]: Wifi Dongle is opened.");
			new checkSupplicantThread().start();
		}
	}
	
	/**
	 * Auto connect to the Internet use wifi.
	 * Application use this API connect to Internet when system start up.
	 * 
	 * @return
	 */
	public void wifiAutoConnect() {
		wifi0CheckThread checkThread = new wifi0CheckThread();
		checkThread.start();
		return;
	}
	
	
	class wifi0CheckThread extends Thread {
		public wifi0CheckThread() {
			super("wifi0CheckThread");
		}
		
		public void run() {
			if(isDongleExist()) {
				mHandler.sendEmptyMessage(WIFI0_CHECK_SUCCESS);
				return;
			} else {
				mHandler.sendEmptyMessage(WIFI0_CHECK_FAILED);
				return;
			}
		}
		
		
		private boolean isDongleExist() {
			/*int tryCount = 0;
			while(true) {
				if(mWifiManager.isDongleExist()){
					return true;
				}
				
				if(tryCount ++ < 6) {
					nap(5);
				} else {
					break;
				}
			}*/
			
			return false;
		}
	}
	
	class checkSupplicantThread extends Thread {
		public checkSupplicantThread() {
			super("checkSupplicantThread");
		}
		
		public void run() {
			if(pingSupplicant()) {
				mHandler.sendEmptyMessage(SUPPLICANT_START_SUCCESS);
				return;
			} else {
				mHandler.sendEmptyMessage(SUPPLICANT_START_FAILED);
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
					nap(5);
				} else {
					break;
				}
			}
			
			return false;
		}
	}
	
	private void nap(int secs) {
		try{
			Thread.sleep(secs * 1000);
		}catch (InterruptedException ignore) {
		}
	}
	
	/**
	 * Auto connect to the Internet use ethernet.
	 * Application use this API connect to Internet when system start up.
	 * 
	 * @return
	 */
	public void ethernetAutoConnect() {
		
	}
	
	public void pppoeAutoConnect() {
		
	}
	

}
