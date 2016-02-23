package com.mediatek.netcm.ethernet;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.ethernet.EthernetDevInfo;
import android.net.ethernet.EthernetManager;

public class DnsForPppoe {
	private static DnsForPppoe instance = null;
	private static EthernetManager mEthManager = null;
	private Context mContext = null;
	private static EthernetDevInfo mEthInfo = null;
	private DhcpInfo mDhcpInfo = null;
	private final static int E_INFO_NONE = 0;
	private final static int E_INFO_MANUAL = 1;
	private final static int E_INFO_DHCP = 2;
	private static int mEnableInfo = E_INFO_NONE;
	
	private DnsForPppoe(Context context){
		this.mContext = context;
		mEthManager = (EthernetManager) mContext.getSystemService(Context.ETHERNET_SERVICE);
		int state = mEthManager.getState();
		
		if (mEthManager.isConfigured()
				&& state == EthernetManager.ETHERNET_STATE_ENABLED) {
			mEthInfo = mEthManager.getSavedConfig();
			if (mEthInfo.getConnectMode().equals(
					EthernetDevInfo.ETHERNET_CONN_MODE_DHCP)) {
				mDhcpInfo = mEthManager.getDhcpInfo();			
				mEnableInfo = E_INFO_DHCP;
			}
			else {
				mEnableInfo = E_INFO_MANUAL;
			}
		}		
	}
	
	public static final DnsForPppoe getInstance(Context context) {
		if (instance == null) {
			instance = new DnsForPppoe(context);
		}
		return instance;
	}
	
	public String getDnsAddr() {
		switch(mEnableInfo){
			case E_INFO_MANUAL:
				return mEthInfo.getDnsAddr();
				
			case E_INFO_DHCP:
				return intToString(mDhcpInfo.dns1);
				
			default:
					return null;
		}
	}
	
	private String intToString(int data) {
		StringBuffer StringBuf = new StringBuffer();
		StringBuf.append(data & 0xff).append('.').append((data >>>= 8) & 0xff)
				.append('.').append((data >>>= 8) & 0xff).append('.').append(
						(data >>>= 8) & 0xff);
		return StringBuf.toString();
	}
}
