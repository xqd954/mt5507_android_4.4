package com.mediatek.netcm.ethernet;

import com.mediatek.netcm.util.NetLog;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.DhcpInfo;
import android.net.ethernet.EthernetDevInfo;
import android.net.ethernet.EthernetManager;

/**
 * This class used for receive broadcast which send by framework layer,
 * also used for update the information of network status. 
 * 
 * Application should not use this Class directly, use 'EthernetImplement' instead.
 *
 * Application should register a receiver in 'AndroidManifest.xml',
 * example:
 *    <receiver android:name="com.mediatek.netcm.ethernet.EthernetDevReceiver">
 *    <intent-filter>
 *      		<action android:name="android.intent.action.BOOT_COMPLETED" />
 *       	<action android:name="android.net.ethernet.ETHERNET_STATE_CHANGED" />
 *     </intent-filter>
 *   </receiver>
 *    
 */
public class EthernetDevReceiver extends BroadcastReceiver {
	private static  Context mContext;
	private static EthernetManager mEthManager ;
	private static EthernetDevInfo mEthInfo = new EthernetDevInfo();
	private static DhcpInfo mDhcpInfo  = new DhcpInfo();
	private static boolean mEnablePending = true;
	
	private final static int E_INFO_NONE = 0;
	private final static int E_INFO_MANUAL = 1;
	private final static int E_INFO_DHCP = 2;
	private static int mEthInfoType = E_INFO_NONE;
	
	private static boolean initOnce = false;
	private String TAG = "CM_EthernetDevReceiver";
	private static EthernetDevReceiver instance = null;
	
	public static EthernetDevReceiver getInstance() {
		if (instance == null) {
			instance = new EthernetDevReceiver();
		}
		
		return instance;
	}
	
	private void updateInfoType() {
		if(mEthManager == null) {
			return;
		}
		
		if (mEthManager.isConfigured()) {
			EthernetDevInfo info = mEthManager.getSavedConfig();
			if(info != null) {
				mEthInfo = info;
			}
			
			if (mEthInfo != null && mEthInfo.getConnectMode().equals(
					EthernetDevInfo.ETHERNET_CONN_MODE_DHCP)) {
				DhcpInfo dhcpInfo = mEthManager.getDhcpInfo();
				if(dhcpInfo != null) {
					mDhcpInfo =	dhcpInfo;
				}
				mEthInfoType = E_INFO_DHCP;
			} else {
				mEthInfoType = E_INFO_MANUAL;
			}
		} else {
			mEthInfoType = E_INFO_NONE;
		}
	}
	
	private void postNotification(int event) {
		if(mContext == null) {
			return;
		}
		
		Intent intent = new Intent(EthernetImplement.ETHERNET_NOTIFY);
		intent.putExtra(EthernetImplement.DHCP_STATE, event);
		mContext.sendBroadcast(intent);
	}
	
	
	@Override
	public void onReceive(Context context, Intent intent) {
		String action = intent.getAction();
		NetLog.d(TAG, "Receive broadcast : " + action);
		
		if(!initOnce) {
			mContext = context;
			mEthManager = (EthernetManager) mContext.getSystemService(Context.ETHERNET_SERVICE);
			mEthInfo = mEthManager.getSavedConfig();
			String mode = mEthInfo.getConnectMode();
			if (mode != null && mode.equals(EthernetDevInfo.ETHERNET_CONN_MODE_MANUAL)) {
				NetLog.d(TAG, "Saved mode is manual.");
				mEthInfoType = E_INFO_MANUAL;
			} 
			else if (mode != null && mode.equals(EthernetDevInfo.ETHERNET_CONN_MODE_DHCP)){
				NetLog.d(TAG, "Saved mode is dhcp.");
				DhcpInfo dhcpInfo = mEthManager.getDhcpInfo();
				if(dhcpInfo != null) {
					mDhcpInfo =	dhcpInfo;
				}
				
				mEthInfoType = E_INFO_DHCP;
			} 
			else {
				mEthInfoType = E_INFO_NONE;
			}
			
			initOnce = true;
		}
		
		if ((EthernetManager.ETHERNET_STATE_CHANGED_ACTION).equals(action)) {
			final int value = intent.getIntExtra(
					EthernetManager.EXTRA_ETHERNET_STATE,
					EthernetManager.ETHERNET_STATE_UNKNOWN);
			switch (value) {
			case EthernetManager.EVENT_INTERFACE_CONFIGURATION_SUCCEEDED:
				NetLog.d(TAG, "Receive broadcast value is : EVENT_INTERFACE_CONFIGURATION_SUCCEEDED");
				updateInfoType();
				postNotification(EthernetImplement.E_DHCP_SUCCESS);
				break;
				
			case EthernetManager.EVENT_HW_CONNECTED:
				NetLog.d(TAG, "Receive broadcast value is : EVENT_HW_CONNECTED");
				updateInfoType();
				postNotification(EthernetImplement.E_HW_CONNECTED);
				break;

			case EthernetManager.EVENT_INTERFACE_CONFIGURATION_FAILED:
				NetLog.d(TAG, "Receive broadcast value is : EVENT_INTERFACE_CONFIGURATION_FAILED");
				mEthInfoType = E_INFO_NONE;
				postNotification(EthernetImplement.E_DHCP_FAILED);
				break;
				
			case EthernetManager.EVENT_HW_DISCONNECTED:
				NetLog.d(TAG, "Receive broadcast value is : EVENT_HW_DISCONNECTED");
				String mode = mEthInfo.getConnectMode();
				if (mode != null && mode.equals(EthernetDevInfo.ETHERNET_CONN_MODE_MANUAL)) {
					mEthInfoType = E_INFO_MANUAL;
				} else {
					mEthInfoType = E_INFO_NONE;
				}
				
				postNotification(EthernetImplement.E_HW_DISCONNECTED);	
				break;

			default:
				break;
			}
		}

		return;	
	}

	protected boolean openEthernet() {
		if(mEthManager == null) {
			return false;
		}
		
		//int EthState = 0;
		int EthState = mEthManager.getState();
		switch (EthState) {
			case EthernetManager.ETHERNET_STATE_DISABLED:
			case EthernetManager.ETHERNET_STATE_UNKNOWN: 
				mEthManager.setEnabled(true);
				mEnablePending = false;
				break;
		
			default:
				break;
		}
		
		return true;
	}
	
	protected int getState() {
		if(mEthManager == null) {
			return EthernetImplement.E_STATUS_UNKNOWN;
		}
		
		int state = mEthManager.getState();
		switch (state) {
		case EthernetManager.ETHERNET_STATE_UNKNOWN:
			return EthernetImplement.E_STATUS_UNKNOWN;
		case EthernetManager.ETHERNET_STATE_DISABLED:
			return EthernetImplement.E_STATUS_DISABLED;
		case EthernetManager.ETHERNET_STATE_ENABLED:
			return EthernetImplement.E_STATUS_ENABLED;
		}

		return EthernetImplement.E_STATUS_UNKNOWN;
	}

	protected void closeEthernet() {
		if (mEthManager != null && mEnablePending == false)//before is ||
			mEthManager.setEnabled(false);
		mEnablePending = true;
	}
	

	protected void setConnectDhcp(String devName) {
		if(mEthManager == null) {
			return;
		}
		
		mEthInfo.setConnectMode(EthernetDevInfo.ETHERNET_CONN_MODE_DHCP);
		mEthInfo.setIpAddress(null);
		mEthInfo.setRouteAddr(null);
		mEthInfo.setDnsAddr(null);
		mEthInfo.setDns2Addr(null);
		mEthInfo.setNetMask(null);
		
		//Currently, the 'eth0' is default.
		mEthInfo.setIfName("eth0");
		
		mEthManager.updateDevInfo(mEthInfo);
		mEthInfoType = E_INFO_NONE;
		
		if (mEnablePending) {
			mEthManager.setEnabled(true);
			mEnablePending = false;
		}

		return;
	}
	
	protected boolean manualSetting(String ip, String NetMask, String RouteAddr,
			String DnsAddr, String devName) {
		if(mEthManager == null) {
			return false;
		}
		
		mEthInfo.setConnectMode(EthernetDevInfo.ETHERNET_CONN_MODE_MANUAL);
		mEthInfo.setIpAddress(ip);

		mEthInfo.setRouteAddr(RouteAddr);
		mEthInfo.setDnsAddr(DnsAddr);
		mEthInfo.setNetMask(NetMask);
		
		//Currently, the 'eth0' is default.
		mEthInfo.setIfName("eth0");
		mEthManager.updateDevInfo(mEthInfo);

		if (mEnablePending) {
			mEthManager.setEnabled(true);
			mEnablePending = false;
		}

		mEthInfoType = E_INFO_MANUAL;

		return true;
	}
	
	protected boolean manualSetting(String ip, String NetMask, String RouteAddr,
			String Dns1Addr, String Dns2Addr, String devName) {
		if(mEthManager == null) {
			return false;
		}
		
		mEthInfo.setConnectMode(EthernetDevInfo.ETHERNET_CONN_MODE_MANUAL);
		mEthInfo.setIpAddress(ip);
		mEthInfo.setRouteAddr(RouteAddr);
		mEthInfo.setDnsAddr(Dns1Addr);
		mEthInfo.setDns2Addr(Dns2Addr);
		mEthInfo.setNetMask(NetMask);
		
		//Currently, the 'eth0' is default.
		mEthInfo.setIfName("eth0");
		mEthManager.updateDevInfo(mEthInfo);

		if (mEnablePending) {
			mEthManager.setEnabled(true);
			mEnablePending = false;
		}

		mEthInfoType = E_INFO_MANUAL;

		return true;
	}
	
	protected String getConnectMode() {
		String mode = mEthInfo.getConnectMode();
		if (mode == null) {
			return null;
		} else if (mode.equals(EthernetDevInfo.ETHERNET_CONN_MODE_DHCP)) {
			return EthernetImplement.E_CONN_MODE_DHCP;
		} else
			return EthernetImplement.E_CONN_MODE_MANUAL;
	}
	
	protected String getMacAddr() {
		if(mEthManager == null) {
			return null;
		}
		
		return mEthManager.getMacAddr();
	}
	
	
	protected String getIpAddress() {
		switch (mEthInfoType) {
		case E_INFO_MANUAL:		
			return mEthInfo.getIpAddress();

		case E_INFO_DHCP:
			return intToString(mDhcpInfo.ipAddress);

		case E_INFO_NONE:
			return null;
		}

		return null;
	}

	protected String getNetMask() {
		switch (mEthInfoType) {
		case E_INFO_MANUAL:
			return mEthInfo.getNetMask();

		case E_INFO_DHCP:
			return intToString(mDhcpInfo.netmask);

		case E_INFO_NONE:
			return null;

		}

		return null;
	}

	protected String getRouteAddr() {
		switch (mEthInfoType) {
		case E_INFO_MANUAL:
			return mEthInfo.getRouteAddr();

		case E_INFO_DHCP:
			return intToString(mDhcpInfo.gateway);

		case E_INFO_NONE:
			return null;

		}

		return null;
	}

	protected String getDnsAddr() {
		switch (mEthInfoType) {
		case E_INFO_MANUAL:
			return mEthInfo.getDnsAddr();

		case E_INFO_DHCP:
			return intToString(mDhcpInfo.dns1);

		case E_INFO_NONE:
			return null;

		}

		return null;
	}

	protected String getDns2Addr() {
		switch (mEthInfoType) {
		case E_INFO_MANUAL:
			return mEthInfo.getDns2Addr();

		case E_INFO_DHCP:
			return intToString(mDhcpInfo.dns2);

		case E_INFO_NONE:
			return null;
		}

		return null;
	}
	
	protected boolean getLinkStatus(String nic) {
		if(mEthManager == null) {
			return false;
		}
		
		return mEthManager.getLinkStatus(nic);
	}

	private String intToString(int data) {
		StringBuffer StringBuf = new StringBuffer();
		StringBuf.append(data & 0xff).append('.').append((data >>>= 8) & 0xff)
				.append('.').append((data >>>= 8) & 0xff).append('.').append(
						(data >>>= 8) & 0xff);
		return StringBuf.toString();
	}
	
}
