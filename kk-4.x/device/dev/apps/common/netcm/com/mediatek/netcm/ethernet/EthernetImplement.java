package com.mediatek.netcm.ethernet;

import com.mediatek.netcm.util.NetLog;

import android.content.Context;
import android.content.Intent;
import android.net.ethernet.*;

/**
 * This class use to ethernet operation.
 * <ul>
 * <li> Set network information.</li>
 * <li> Get network information.</li>
 * </ul>
 */
public class EthernetImplement implements Ethernet {

	private Context mContext = null;
	private static EthernetImplement instance = null;
	private static final String TAG = "CM_EthernetImplement";
	private static EthernetManager mEthManager ;
	private final String mEthernetMessage = EthernetManager.ETHERNET_STATE_CHANGED_ACTION;
	public static boolean dummyMode = false;

	public final static String E_CONN_MODE_MANUAL = "manual";
	public final static String E_CONN_MODE_DHCP = "dhcp";
	
	/**
	 * Stop interface message. 
	 * Application should register this receiver When call 'closeEthernet()'.
	 * 
	 */
	public final static String ETHERNET_STOP_INTERFACE_NOTIFY = EthernetManager.ETHERNET_STOP_INTERFACE_ACTION;
	
	/**
	 * Start interface message. 
	 * Application should register this receiver When call 'openEthernet()'.
	 * 
	 */
	public final static String ETHERNET_START_INTERFACE_NOTIFY = EthernetManager.ETHERNET_START_INTERFACE_ACTION;
	
	
	/**
	 * Ethernet operation message. 
	 * Application should register this receiver When call 'setConnectDhcp()'.
	 * 
	 */
	public final static String ETHERNET_NOTIFY = "com.mediatek.netcm.ethernet.ETHERNET_STATE_CHANGED";
	
	/**
	 * The name of the extra data, with package 'ETHERNET_NOTIFY' prefix.
	 * 
	 */
	public final static String DHCP_STATE = "DHCP_state";

	/**
	 * The default value.
	 */
	public final static int E_DHCP_ORIGINAL = 0;
	
	/**
	 * The value stands for DHCP success.
	 */
	public final static int E_DHCP_SUCCESS = 1;
	
	/**
	 * The value stands for DHCP failed.
	 */
	public final static int E_DHCP_FAILED = 2;
	
	/**
	 * The value stands for connect the network cable.
	 */
	public final static int E_HW_CONNECTED = 3;
	
	/**
	 * The value stands for disconnect the network cable.
	 */
	public final static int E_HW_DISCONNECTED = 4;
	
	public final static int E_STATUS_UNKNOWN = 0;
	public final static int E_STATUS_DISABLED = 1;
	public final static int E_STATUS_ENABLED = 2;

	private static EthernetDevReceiver mDevReceiver;
	
	private EthernetImplement(Context context) {
		this.mContext = context;
		mDevReceiver = EthernetDevReceiver.getInstance();
		mEthManager = (EthernetManager) mContext.getSystemService(Context.ETHERNET_SERVICE);
	}

	private EthernetImplement(Context context, String txt) {

	}

	/**
	 * Create a new EthernetImplement instance. Applications will use for
	 * Ethernet operation.
	 * 
	 */
	public static EthernetImplement getInstance(Context context) {
		if (dummyMode) {
			instance = new EthernetImplement(context, null);
			return instance;
		}

		if (instance == null) {
			instance = new EthernetImplement(context);
		}
		
		return instance;
	}
	
	/**
	 * Get the network cable link status.
	 * 
	 * @return true if linked, false otherwise.
	 */
	public boolean getLinkStatus() {
		if (dummyMode) {
			return true;
		}
		
		if(mEthManager == null) {
			return false;
		}
		
		return mEthManager.getLinkStatus("eth0");
	}

	/**
	 * If ethernet is disable, enable it.
	 * 
	 * @return
	 */
	public boolean openEthernet() {
		if (dummyMode) {
			return true;
		}

		if(mDevReceiver == null) {
			return false;
		}
		
		return mDevReceiver.openEthernet();
	}

	/**
	 * Get current status of ethernet.
	 * 
	 * @return EthernetImplement.E_STATUS_UNKNOWN
	 *         EthernetImplement.E_STATUS_DISABLED
	 *         EthernetImplement.E_STATUS_ENABLED
	 */
	public int getState() {
		if (dummyMode) {
			return E_STATUS_UNKNOWN;
		}

		if(mDevReceiver == null) {
			return E_STATUS_UNKNOWN;
		}
		
		return mDevReceiver.getState();
	}

	/**
	 * Disable the ethernet. Application must register the
	 * 'ETHERNET_STOP_INTERFACE_NOTIFY' receiver to receive the stop interface
	 * message.
	 * 
	 */
	public void closeEthernet() {
		if (dummyMode) {
			return;
		}

		if(mDevReceiver == null) {
			return;
		}
		
		mDevReceiver.closeEthernet();
	}

	/**
	 * Get the connect mode of ethernt.
	 * 
	 * @return EthernetImplement.E_CONN_MODE_MANUAL
	 *         EthernetImplement.E_CONN_MODE_DHCP
	 */
	public String getConnectMode() {
		if (dummyMode) {
			return E_CONN_MODE_DHCP;
		}
		
		if(mDevReceiver == null) {
			return null;
		}
		
		return mDevReceiver.getConnectMode();
	}

	/**
	 * Get the MAC address.
	 * 
	 * @return Ethernet's MAC address.
	 */
	public String getMacAddr() {
		if (dummyMode) {
			return "00:1F:16:16:DB:17";
		}
		
		if(mEthManager == null) {
			return null;
		}
		
		return mEthManager.getMacAddr();
	}
	
	
	/**
	 * Get the IP address.
	 * 
	 * @return
	 */
	public String getIpAddress() {
		if (dummyMode) {
			return "172.26.149.236";
		}
		
		if(mDevReceiver == null) {
			return null;
		}
		
		return mDevReceiver.getIpAddress();
	}

	/**
	 * Get the net mask.
	 * 
	 * @return The current network setting's mask.
	 */
	public String getNetMask() {
		if (dummyMode) {
			return "255.255.255.0";
		}

		if(mDevReceiver == null) {
			return null;
		}
		
		return mDevReceiver.getNetMask();
	}

	/**
	 * Get the gateway address.
	 * 
	 * @return The current network setting's gate way address.
	 */
	public String getRouteAddr() {
		if (dummyMode) {
			return "172.26.149.255";
		}

		if(mDevReceiver == null) {
			return null;
		}
		
		return mDevReceiver.getRouteAddr();
	}

	private String getDevName() {
		String[] devName = mEthManager.getDeviceNameList();
		
		return devName[0];
	}
	
	/**
	 * Get the DNS address.
	 * 
	 * @return The current network setting's DNS address.
	 */
	public String getDnsAddr() {
		if (dummyMode) {
			return "231.032.065.075";
		}

		if(mDevReceiver == null) {
			return null;
		}
		
		return mDevReceiver.getDnsAddr();
	}

	/**
	 * Get alternate DNS address.
	 * 
	 * @return The current network setting's DNS address.
	 */
	public String getDns2Addr() {
		if (dummyMode) {
			return "231.032.065.075";
		}

		if(mDevReceiver == null) {
			return null;
		}
		
		return mDevReceiver.getDns2Addr();
	}

	/**
	 * Remove the sticky broadcast 'ETHERNET_STATE_CHANGED_ACTION'. Application
	 * must call this API to remove the sticky broadcast, otherwise application
	 * may not receive the accurate information.
	 * 
	 * @deprecated
	 */
	public void onRemoveStickyBroadcast() {
		if (dummyMode) {
			return;
		}
		final Intent intent = new Intent(mEthernetMessage);
		mContext.removeStickyBroadcast(intent);
	}

	/**
	 * Register the receiver to receive the broadcast from framework layer.
	 * Application's must call this API to register the receiver.
	 * 
	 * @deprecated
	 */
	public void onStartRegister() {
//		mReceiver = new DhcpResultReceiver();
//		IntentFilter filter = new IntentFilter();
//		filter.addAction(mEthernetMessage);
//		mContext.registerReceiver(mReceiver, filter);
	}

	/**
	 * Set the ethernet connect mode to DHCP. Application's must register the
	 * receiver 'ETHERNET_NOTIFY' to receive the message at the same.
	 * 
	 */
	public void setConnectDhcp() {
		if (dummyMode) {
			return;
		}

		if(mDevReceiver == null) {
			return;
		}
		
		String devName = getDevName();
		
		mDevReceiver.setConnectDhcp(devName);
	}

	/**
	 * Set the ethernet by manual.
	 * 
	 * @param IP
	 *            , NetMask, RouteAddr, DnsAddr
	 * @return true if set the ethernet by manual success, else return false
	 */
	public boolean manualSetting(String ip, String NetMask, String RouteAddr,
			String DnsAddr) {

			NetLog.d(TAG, "manual setting IP: " + ip + "NetMask: " + NetMask
					+ "RouteAddr: " + RouteAddr + "DnsAddr:" + DnsAddr);
		
		if(ip == null || NetMask == null || RouteAddr == null || DnsAddr == null) {
			return false;
		}
			
		if(mDevReceiver == null) {
			return false;
		}
		
		String devName = getDevName();
		
		return mDevReceiver.manualSetting(ip, NetMask, RouteAddr, DnsAddr, devName);
	}

	/**
	 * Set the ethernet by manual, this API need alternate DNS address.
	 * 
	 * @param IP
	 *            , NetMask, RouteAddr, DnsAddr, Dns2Addr.
	 * @return true if set the ethernet by manual success, else return false
	 */
	public boolean manualSetting(String ip, String NetMask, String RouteAddr,
			String Dns1Addr, String Dns2Addr) {
			NetLog.d(TAG, "manual setting IP: " + ip + "NetMask: " + NetMask
					+ "RouteAddr: " + RouteAddr + "Dns1Addr:" + Dns1Addr
					+ " Dns2Addr: " + Dns2Addr);
		
		if(ip == null || NetMask == null || RouteAddr == null 
				|| Dns1Addr == null || Dns2Addr == null) {
			return false;
		}
			
		if(mDevReceiver == null) {
			return false;
		}
		
		String devName = getDevName();
		
		return mDevReceiver.manualSetting(ip, NetMask, RouteAddr, Dns1Addr, Dns2Addr, devName);
	}

}
