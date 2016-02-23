/*
 * Copyright (C) 2010 The Android-X86 Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Yi Sun <beyounn@gmail.com>
 */

package android.net.ethernet;

import java.net.InetAddress;
import java.net.UnknownHostException;

import android.R;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.bluetooth.BluetoothHeadset;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
//import android.net.DhcpInfoInternal;//commented by hqs@tcl
import android.net.DhcpResults;
import android.net.LinkCapabilities;
import android.net.LinkQualityInfo;//add by hqs@tcl
import android.net.LinkAddress;


import java.util.Collection;

import android.net.LinkProperties;
import android.net.NetworkInfo;
import android.net.NetworkInfo.DetailedState;
import android.net.NetworkStateTracker;
import android.net.SamplingDataTracker;
import android.net.NetworkUtils;
import android.net.ProxyProperties;
import android.net.RouteInfo;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;//add by hqs@tcl
import android.os.Parcel;
import android.os.SystemProperties;
import android.provider.Settings;
import android.util.*;
import java.util.Iterator;

import java.util.concurrent.atomic.AtomicBoolean;
import java.net.Inet4Address;

/**
 * Track the state of Ethernet connectivity. All event handling is done here,
 * and all changes in connectivity state are initiated here.
 *
 * @hide
 */

public class EthernetStateTracker extends Handler implements NetworkStateTracker {
    private static final String TAG                                 = "EthernetStateTracker";
    private static final String DEFAULT_DNS_PROPERTY1="net.dns1";
	private static final String DEFAULT_DNS_PROPERTY2="net.dns2";
    private static final String DEFAULT_DNS_SERVER="8.8.8.8";
    private static final String DEFAULT_PROXY_HOST_PROPERTY="http.proxyHost";
    private static final String DEFAULT_PROXY_PORT_PROPERTY="http.proxyPort";
    private static final String VERSION="v1.0.1";

    public static final int EVENT_DHCP_START                        = 0;
    public static final int EVENT_INTERFACE_CONFIGURATION_SUCCEEDED = 1;
    public static final int EVENT_INTERFACE_CONFIGURATION_FAILED    = 2;
    public static final int EVENT_HW_CONNECTED                      = 3;
    public static final int EVENT_HW_DISCONNECTED                   = 4;
    public static final int EVENT_HW_PHYCONNECTED                   = 5;
    private static final int NOTIFY_ID                              = 6;
    private static final boolean localLOGV = true;

    private AtomicBoolean mTeardownRequested = new AtomicBoolean(false);
    private AtomicBoolean mPrivateDnsRouteSet = new AtomicBoolean(false);
    private AtomicBoolean mDefaultRouteSet = new AtomicBoolean(false);

    private EthernetManager mEM;
    private boolean mServiceStarted;
    private NetworkInfo mNetworkInfo;

    private boolean mStackConnected;
    
    private boolean mLastConfigbySetStaticIP;
    private boolean mlastConfigStatic;
		
    private boolean mEthCfgfinished = true;//add by HQS--20131125

    private boolean mCableConnected;
    private boolean mInterfaceStopped;
    private DhcpHandler mDhcpTarget;
    private String mInterfaceName ;
    private DhcpResults mDhcpInfo;
    private EthernetMonitor mMonitor;
    private String[] sDnsPropNames;
    private boolean mStartingDhcp = false;
    private Handler mTrackerTarget;
	private boolean mNeedSetState = false;

    private LinkProperties mLinkProperties;
    private BroadcastReceiver mEthernetStateReceiver;

    /* For sending events to connectivity service handler */
    private Handler mCsHandler;
    private Context mContext;
    private static EthernetStateTracker sInstance;
    //add by hqs@tcl
    private SamplingDataTracker mSamplingDataTracker = new SamplingDataTracker();
	private String maddress;
	private String mdns1;
	private String mdns2;
	private String mdns3;
	private String mdns4;
	private String mgatew;
	private int mprefixL;

    private EthernetStateTracker() {
        mNetworkInfo = new NetworkInfo(ConnectivityManager.TYPE_ETHERNET, 0, "ETH", "");
        mLinkProperties = new LinkProperties();
        if (localLOGV) Slog.v(TAG, "start to init device ...");

        if (EthernetNative.initEthernetNative() != 0) {
            Slog.e(TAG,"Can not init ethernet device layers");
            return;
        }

        if (localLOGV) Slog.v(TAG,"successed to init device!");

        mServiceStarted = true;
        HandlerThread dhcpThread = new HandlerThread("DHCP Handler Thread");
        dhcpThread.start();
        mDhcpTarget = new DhcpHandler(dhcpThread.getLooper(), this);
        mMonitor = new EthernetMonitor(this);
        mDhcpInfo = new DhcpResults();
		mNeedSetState = false;
    }

    public static synchronized EthernetStateTracker getInstance() {
        if (sInstance == null) sInstance = new EthernetStateTracker();
        return sInstance;
    }
    /**
     * Stop etherent interface
     * @param suspend {@code false} disable the interface {@code true} only reset the connection without disable the interface
     * @return true
     */
    public boolean stopInterface(boolean suspend) {
        if (mEM != null) {
            EthernetDevInfo info = mEM.getSavedConfig();
            if (info != null && mEM.isConfigured()) {
				Slog.i(TAG,"---stopInterface");
                synchronized (this) {
                    mInterfaceStopped = true;
                    if (localLOGV) Slog.i(TAG, "stop dhcp and interface");
                    mDhcpTarget.removeMessages(EVENT_DHCP_START);
                    String ifname = info.getIfName();
                    if (!NetworkUtils.stopDhcp(ifname)) {
                        if (localLOGV) Slog.w(TAG, "Could not stop DHCP");
		//return false;//add by HQS---20131125
                    }
                    mStartingDhcp = false;//HQS---20131112
                    NetworkUtils.resetConnections(ifname, NetworkUtils.RESET_ALL_ADDRESSES);
                    if (!suspend) {
                        NetworkUtils.disableInterface(ifname);
                    }else{
                    	boolean ret;
		ret = NetworkUtils.clearAddresses(ifname);
		if (localLOGV) Slog.w(TAG, "stopInterface --> clearAddresses ret = "+ret);
	       }
                    mLinkProperties.clear();
                }
            }
        }
        return true;
    }
	//add for ethernet monitor when sys suspend - HQS@TCL--20131105--start
	public boolean disableInterface(boolean suspend) {
   		if (mEM != null) {
            EthernetDevInfo info = mEM.getSavedConfig();
            if (info != null && mEM.isConfigured()) {
                Slog.i(TAG,"showboz-----disableInterface");
               // synchronized (mDhcpTarget) {
                    mInterfaceStopped = true;
                    if (localLOGV) Slog.i(TAG, "stop dhcp and interface");
                    mDhcpTarget.removeMessages(EVENT_DHCP_START);
                    String ifname = info.getIfName();
                    if (!NetworkUtils.stopDhcp(ifname)) {
                        if (localLOGV) Slog.w(TAG, "Could not stop DHCP");
                    }
                    NetworkUtils.resetConnections(ifname, NetworkUtils.RESET_ALL_ADDRESSES);
                    if (!suspend) {
                        NetworkUtils.disableInterface(ifname);
                    //Begin added by TCL Xi'an NT liukun:liukun@tcl.com -2012.4.16
                    }else{
                        boolean ret;
                        ret = NetworkUtils.clearAddresses(ifname);
                        if (localLOGV) Slog.w(TAG, "stopInterface --> clearAddresses ret = "+ret);
                    }
                    //End added by TCL Xi'an NT liukun:liukun@tcl.com -2012.4.16
                    mLinkProperties.clear();
                //}
            }
        }
        return true;
 
	}
	//add for ethernet monitor when sys suspend - HQS@TCL--20131105--end
    private boolean configureInterface(EthernetDevInfo info) throws UnknownHostException {
        mStackConnected = false;
        mInterfaceStopped = false;
        mLastConfigbySetStaticIP = false;

        if (info.getConnectMode().equals(EthernetDevInfo.ETHERNET_CONN_MODE_DHCP)) {
            if (localLOGV) Slog.i(TAG, "trigger dhcp for device " + info.getIfName());
            sDnsPropNames = new String[] {
                "dhcp." + mInterfaceName + ".dns1",
                "dhcp." + mInterfaceName + ".dns2"
            };
         	//add 140623
            if(mlastConfigStatic){
                NetworkUtils.disableInterface(info.getIfName());
                NetworkUtils.enableInterface(info.getIfName());
            }else{//
	//HQS---20131112--bellow
            synchronized (this) {
                if (!mStartingDhcp) {
		            mStartingDhcp = true;
		            mDhcpTarget.sendEmptyMessage(EVENT_DHCP_START);
                    if (localLOGV) Slog.i(TAG, "trigger dhcp for dev " + info.getIfName());
                }
            }
        }//
        } else if(mCableConnected){ //zengyongying: do not set static ip if cable not connected
            int event;
			
        	mNeedSetState = true;				
        	mLastConfigbySetStaticIP = true;
            mStartingDhcp = false;
            DhcpInfo mStaticInfo = new DhcpInfo();
            sDnsPropNames = new String[] {
                "net." + mInterfaceName + ".dns1",
                "net." + mInterfaceName + ".dns2"
            };

            mStaticInfo.ipAddress = lookupHost(info.getIpAddress());
            mStaticInfo.gateway = lookupHost(info.getRouteAddr());
            mStaticInfo.netmask = lookupHost(info.getNetMask());
            mStaticInfo.dns1 = lookupHost(info.getDnsAddr());
            mStaticInfo.dns2 = lookupHost(info.getDns2Addr());

            if (localLOGV) Slog.i(TAG, "set ip manually " + mStaticInfo.toString());
            NetworkUtils.removeDefaultRoute(info.getIfName());
            if (NetworkUtils.configureInterface(info.getIfName(), mStaticInfo)) {
                event = EVENT_INTERFACE_CONFIGURATION_SUCCEEDED;
                mCableConnected = true;
                if (localLOGV) Slog.v(TAG, "Static IP configuration succeeded");
                PostLinkageConfig(makeInfoInternal(mStaticInfo));
            } else {
                event = EVENT_INTERFACE_CONFIGURATION_FAILED;
                if (localLOGV) Slog.w(TAG, "Static IP configuration failed");
            }
            this.sendEmptyMessage(event);

        } else {
             Slog.w(TAG, "do not set static ip if cable not connected");
             NetworkUtils.enableInterface(mInterfaceName);
             mEthCfgfinished = true;
             mNeedSetState = false;
             mStartingDhcp = false;
        }
        
        return true;
    }

    /**
     * reset ethernet interface
     * @return true
     * @throws UnknownHostException
     */
    public boolean resetInterface()  throws UnknownHostException{
        /*
         * This will guide us to enabled the enabled device
         */
	if (localLOGV) Slog.i(TAG, "resetInterface(), mEthCfgfinished=" + mEthCfgfinished);
       
        if(mLastConfigbySetStaticIP){
            mlastConfigStatic = true;
        }else{
            mlastConfigStatic = false;
        }
        if(mEthCfgfinished){//mod for eth auto get ip error when cable reconnected  by HQS
            mEthCfgfinished = false;//add by HQS--20131125
	        if (mEM != null) {
	            EthernetDevInfo info = mEM.getSavedConfig();
	            if (info != null && mEM.isConfigured()) {
	                synchronized (this) {
	                    mInterfaceName = info.getIfName();
	                    if (localLOGV) Slog.i(TAG, "reset device " + mInterfaceName);
	                    NetworkUtils.resetConnections(mInterfaceName, NetworkUtils.RESET_ALL_ADDRESSES);
	                    // Stop DHCP
	                    if (mDhcpTarget != null) {
	                        mDhcpTarget.removeMessages(EVENT_DHCP_START);
	                    }
	                    if (!NetworkUtils.stopDhcp(mInterfaceName)) {
	                        if (localLOGV) Slog.w(TAG, "Could not stop DHCP");
	                    }
	                    mStartingDhcp = false;//HQS---20131112
	                    
	                    mDhcpInfo.clear();//mod by hqs@tcl---20140225
	                    mLinkProperties.clear();
	                    configureInterface(info);
						mNeedSetState = true;
	                }
	            }
	        }
		}

        return true;
    }

    public void StartPolling() {
        mMonitor.startMonitoring();
    }

    @Override
    public boolean isAvailable() {
        // Only say available if we have interfaces and user did not disable us.
        return((mEM.getTotalInterface() != 0) && (mEM.getState() != EthernetManager.ETHERNET_STATE_DISABLED));
    }

    @Override
    public boolean reconnect() {
        try {
            synchronized (this) {
                if (mCableConnected && mStackConnected) {
                		if (localLOGV) Slog.i(TAG, "enter reconnect(), stack=" + mStackConnected + " HW=" + mCableConnected);
                    return true;
                }
            }
            if (mEM.getState() != EthernetManager.ETHERNET_STATE_DISABLED) {
                // maybe this is the first time we run, so set it to enabled
                mEM.setEnabled(true);
                if (!mEM.isConfigured()) {
                    mEM.setDefaultConf();
                }
                return resetInterface();
            }
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        return false;
    }

	@Override
    public void captivePortalCheckComplete() {
        return;
    }
	
    @Override
    public boolean setRadio(boolean turnOn) {
        return false;
    }

    @Override
    public void startMonitoring(Context context, Handler target) {
        if (localLOGV) Slog.v(TAG,"start to monitor the ethernet devices . . .");
        if (mServiceStarted) {
            mEM = (EthernetManager)context.getSystemService(Context.ETHERNET_SERVICE);
            mContext = context;
            mCsHandler = target;	
            int state = mEM.getState();
			
            if (state != mEM.ETHERNET_STATE_DISABLED) {
                if (state == mEM.ETHERNET_STATE_UNKNOWN) {
                    // maybe this is the first time we run, so set it to enabled
                    mEM.setEnabled(mEM.getDeviceNameList() != null);
                } else {
                    try {
		resetInterface();
                    } catch (UnknownHostException e) {
                        Slog.e(TAG, "Wrong ethernet configuration");
                    }
                }
            }
        }
    }

    @Override
    public void setUserDataEnable(boolean enabled) {
        Log.w(TAG, "ignoring setUserDataEnable(" + enabled + ")");
    }

    @Override
    public void setPolicyDataEnable(boolean enabled) {
        Log.w(TAG, "ignoring setPolicyDataEnable(" + enabled + ")");
    }

    @Override
    public boolean teardown() {
        return(mEM != null) ? stopInterface(false) : false;
    }

    private void IntentBroadcast(int event) {
        Intent intent = new Intent(EthernetManager.ETHERNET_STATE_CHANGED_ACTION);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent.putExtra(EthernetManager.EXTRA_ETHERNET_STATE, event);
        mContext.sendBroadcast(intent);
        if (localLOGV) Slog.i(TAG, "IntentBroadcast, event=" + event);
    }

    private void postNotification(int event) {
        Message msg = mCsHandler.obtainMessage(EVENT_STATE_CHANGED, new NetworkInfo(mNetworkInfo));
        msg.sendToTarget();
        // Broadcast Intent
        IntentBroadcast(event);
    }

    private boolean setState(boolean state, int event) {
		Slog.i(TAG, "setState ---> state = "+state
				+" isConnected = "+mNetworkInfo.isConnected());

        // network cable status check
        switch (event) {
            case EVENT_INTERFACE_CONFIGURATION_SUCCEEDED:
            case EVENT_HW_CONNECTED:
                if (!mCableConnected) {
                    if (localLOGV) Slog.i(TAG, "ignore wrong events: " + event);
                    return false;
                }
            default:
        }

        // set network state & broadcast events
        if ((mNetworkInfo.isConnected() != state) || mNeedSetState) {
            if (state) {
                mNetworkInfo.setDetailedState(DetailedState.CONNECTED, null, null);
            } else {
                mNetworkInfo.setDetailedState(DetailedState.DISCONNECTED, null, null);
                if(stopInterface(true)){
	                mEthCfgfinished = true;//add by HQS--20131125
	    }
            }
            mNetworkInfo.setIsAvailable(state);
            postNotification(event);
			mNeedSetState = false;
            return true;
        }else {
            if (localLOGV) Slog.i(TAG, "setState, skip: " + event);
            return false;
        }
    }

		private void sendEthBroadcast(String action, boolean state){
			String bootStr = SystemProperties.get("sys.boot_completed");
			int tryTimes = 0;
			if(localLOGV) Slog.i(TAG, "sendEthBroadcast  --> bootStr: "+bootStr);
			while(!bootStr.equals("1")) {	//	boot complete   //zengyongying: wait 3s to complete, but never lost this event(important)
			    bootStr = SystemProperties.get("sys.boot_completed");
			    
                    try {
                        Thread.sleep(100);
                    } catch (Exception e) {
                    }
                if(tryTimes++ > 30) {
                    break;
                }
            }
            
			{
				Intent intent = new Intent(action);
				intent.putExtra(EthernetManager.EXTRA_ETHERNET_STATE, state);
	
				if (localLOGV) Slog.i(TAG, "sendEthBroadcast  --> action = "+action
										+" state = "+state);
			
				mContext.sendBroadcast(intent);
			}
	}



    public void handleMessage(Message msg) {
        synchronized (this) {
            switch (msg.what) {
            case EVENT_INTERFACE_CONFIGURATION_SUCCEEDED:
                if (localLOGV) Slog.i(TAG, "received configured succeeded, stack=" + mStackConnected + " HW=" + mCableConnected);
                mStackConnected = true;
                if (mCableConnected) {
                    if(!setState(true, msg.what)) {IntentBroadcast(msg.what);}
                }
                EthernetDevInfo DevInfo = mEM.getSavedConfig();
                SystemProperties.set(DEFAULT_DNS_PROPERTY1, DevInfo.getDnsAddr());
				SystemProperties.set(DEFAULT_DNS_PROPERTY2, DevInfo.getDns2Addr());
                mEthCfgfinished = true;//add by HQS--20131125

                sendEthBroadcast(EthernetManager.ETHERNET_INTERFACE_CONF_CHANGED, true);
                break;
            case EVENT_INTERFACE_CONFIGURATION_FAILED:
                mStackConnected = false;
                mEthCfgfinished = true;//add by HQS--20131125
                sendEthBroadcast(EthernetManager.ETHERNET_INTERFACE_CONF_CHANGED, false);
                break;
            case EVENT_HW_CONNECTED:
                if (localLOGV) Slog.i(TAG, "received HW connected, stack=" + mStackConnected + " HW=" + mCableConnected);
				mCableConnected = true;
                break;
            case EVENT_HW_DISCONNECTED:
                if (localLOGV) Slog.i(TAG, "received disconnected events, stack=" + mStackConnected + " HW=" + mCableConnected);
                mStackConnected = false;
                mCableConnected = false;
                
                mLastConfigbySetStaticIP = false;
                setState(false, msg.what);
                mEthCfgfinished = true;//add by zengyy
                break;
            case EVENT_HW_PHYCONNECTED:
                if (localLOGV) Slog.i(TAG, "interface up event, kick off connection request");
                mCableConnected = true;
                mStackConnected = false;
                
                /*if(mLastConfigbySetStaticIP){
                	Slog.i(TAG, "interface up event, the last action is config static ip, skip even");
                	mLastConfigbySetStaticIP = false;
                	break;
                }*/
                
                IntentBroadcast(msg.what);
				 reconnect();
                /*if (!mStartingDhcp) {
                    int state = mEM.getState();
                    if (state != mEM.ETHERNET_STATE_DISABLED) {
                        EthernetDevInfo info = mEM.getSavedConfig();
                        if (info != null && mEM.isConfigured()) {
                            try {
                                configureInterface(info);
                            } catch (UnknownHostException e) {
                                // TODO Auto-generated catch block
                                //e.printStackTrace();
                                Slog.e(TAG, "Cannot configure interface");
                            }
                        }
                    }
                }*/
                break;
            }
        }
    }
	//add for new dhcp modifies by HQS---START
	private void GetDhcpResults(DhcpResults mDhcprst){

			mLinkProperties = mDhcprst.linkProperties;
			mLinkProperties = new LinkProperties(mLinkProperties);

		  	int mlast = 0;
			Collection<LinkAddress> eaddres = mLinkProperties.getLinkAddresses();
			for (LinkAddress addr : eaddres) {
                                        maddress = addr.getAddress().getHostAddress(); // lvh@tcl, remove hostname prefix
			    mprefixL = addr.getNetworkPrefixLength();
			}

			int tlast = 0;
			Collection<InetAddress> taddres = mLinkProperties.getDnses();
			for (InetAddress addr : taddres) {
                                        ++tlast;
                                        String value = addr.getHostAddress();
				if(value.length() > 0){
					if(1 == tlast){
						mdns1 = value;
					}else if(2 == tlast){
						mdns2 = value;
					}else if(3 == tlast){
						mdns3 = value;
					}else if(4 == tlast){
						mdns4 = value;
					}
				}
                                    }
			
			int blast = 0;
			Collection<RouteInfo> brouteinfo = mLinkProperties.getRoutes();
			for (RouteInfo brinfo : brouteinfo) {
                                        ++blast;
                                        mgatew = brinfo.getGateway().getHostAddress();
                                    }
	}
            //add for new dhcp modifies by HQS---END

    private void updateDhcpDevInfo(DhcpResults DHCPInfot) {
        EthernetDevInfo info = mEM.getSavedConfig();
        //add for new dhcp modifies by HQS---START
        GetDhcpResults(DHCPInfot);
        if ( (info!=null) && mEM.isConfigured() ) {
            info.setIpAddress(maddress);
            info.setRouteAddr(mgatew);
            info.setDnsAddr(mdns1);
            info.setDns2Addr(mdns2);
            info.setNetMask(NetworkUtils.intToInetAddress(NetworkUtils.prefixLengthToNetmaskInt(mprefixL)).getHostAddress());

            if (mServiceStarted) {
                final ContentResolver cr = mContext.getContentResolver();
                Settings.Global.putString(cr, Settings.Global.ETHERNET_IP, info.getIpAddress());
                Settings.Global.putString(cr, Settings.Global.ETHERNET_DNS, info.getDnsAddr());
                Settings.Global.putString(cr, Settings.Global.ETHERNET_DNS2, info.getDns2Addr());
                Settings.Global.putString(cr, Settings.Global.ETHERNET_ROUTE, info.getRouteAddr());
                Settings.Global.putString(cr, Settings.Global.ETHERNET_MASK, info.getNetMask());
            }
        }
        //add for new dhcp modifies by HQS---END
        Slog.i(TAG, "---updateDhcpDevInfo, IP " + info.getIpAddress() + ", GW " + info.getRouteAddr() + ", DNS " + info.getDnsAddr()+ ", DNS2 " + info.getDns2Addr() + ", MASK " + info.getNetMask());
    }

    public void PostLinkageConfig(DhcpResults DhcpInfo) {
        mLinkProperties = DhcpInfo.linkProperties;
        EthernetDevInfo DevInfo = mEM.getSavedConfig();

        //mod by hqs@tcl---20140305
       	mDhcpInfo = new DhcpResults(DhcpInfo);
        mLinkProperties.setInterfaceName(mInterfaceName);

        // HTTP Proxy
        if (DevInfo.getProxyOn() && (DevInfo.getProxyHost()!=null) && (DevInfo.getProxyPort()!=null) ) {
            ProxyProperties mProxy =
                new ProxyProperties(DevInfo.getProxyHost(), Integer.parseInt(DevInfo.getProxyPort()), null);
            mLinkProperties.setHttpProxy(mProxy);
            if (localLOGV) Slog.d(TAG, "Proxy host " + DevInfo.getProxyHost() + ", port " + DevInfo.getProxyPort());
            SystemProperties.set(DEFAULT_PROXY_HOST_PROPERTY, DevInfo.getProxyHost());
            SystemProperties.set(DEFAULT_PROXY_PORT_PROPERTY, DevInfo.getProxyPort());
        }
    }

    private String convertToString(int addr) {
        return NetworkUtils.intToInetAddress(addr).getHostAddress();
    }

    public DhcpResults makeInfoInternal(DhcpInfo Info) {
        DhcpResults InfoInt = new DhcpResults();

        InfoInt.addLinkAddress(convertToString(Info.ipAddress), NetworkUtils.netmaskIntToPrefixLength(Info.netmask));
        InfoInt.addGateway(convertToString(Info.gateway));
        InfoInt.addDns(convertToString(Info.dns1));
        InfoInt.addDns(convertToString(Info.dns2));

        return InfoInt;
    }

    private class DhcpHandler extends Handler {
         public DhcpHandler(Looper looper, Handler target) {
             super(looper);
             mTrackerTarget = target;
         }

        public void PreDhcpStart() {
            // to speed up system boot up; run DHCP while boot animation is done
            while (true) {
                String p = SystemProperties.get("init.svc.bootanim");
                if (p.compareTo("stopped") == 0) {
                    break;
                } else {
                    try {
                        Thread.sleep(1);
                    } catch (Exception e) {
                    }
                }
            }
        }

        public void handleMessage(Message msg) {
            int event;

            switch (msg.what) {
            case EVENT_DHCP_START:
                synchronized (mDhcpTarget) {
                	if (localLOGV) Slog.d(TAG, "DhcpHandler: mInterfaceStopped="+mInterfaceStopped+ ";mCableConnected=" + mCableConnected);
                    if (!mInterfaceStopped && mCableConnected) {
                        if (localLOGV) Slog.d(TAG, "DhcpHandler: DHCP request started");
                        PreDhcpStart();
		if (mCableConnected && 
		NetworkUtils.runDhcp(mInterfaceName, mDhcpInfo)) {
                                event = EVENT_INTERFACE_CONFIGURATION_SUCCEEDED;
                                if (localLOGV) Slog.d(TAG, "DhcpHandler: DHCP request succeeded: " + mDhcpInfo.toString());
                                PostLinkageConfig(mDhcpInfo);
                        } else {
                             event = EVENT_INTERFACE_CONFIGURATION_FAILED;
                             Slog.e(TAG, "DhcpHandler: DHCP request failed: " + NetworkUtils.getDhcpError());
                        }
                        updateDhcpDevInfo(mDhcpInfo);
                        mTrackerTarget.sendEmptyMessage(event);
                    } else {
                    	NetworkUtils.enableInterface(mInterfaceName);
                        mInterfaceStopped = false;
                        mEthCfgfinished = true;
                    }
                    mStartingDhcp = false;
                }
                break;
            }
        }

    }

    public void notifyPhyConnected(String ifname) {
        if (localLOGV) Slog.v(TAG, "report interface is up for " + ifname);
        synchronized(this) {
            this.sendEmptyMessage(EVENT_HW_PHYCONNECTED);
        }

    }

    public void notifyStateChange(String ifname,DetailedState state) {
        if (localLOGV) Slog.i(TAG, "report new state " + state.toString() + " on dev " + ifname);
        if (ifname.equals(mInterfaceName)) {
            if (localLOGV) Slog.v(TAG, "update network state tracker");
            synchronized(this) {
                this.sendEmptyMessage(state.equals(DetailedState.CONNECTED)
                                      ? EVENT_HW_CONNECTED : EVENT_HW_DISCONNECTED);
            }
        }
    }


    private static int lookupHost(String hostname) {
        InetAddress inetAddress;
        try {
            inetAddress = InetAddress.getByName(hostname);
        } catch (UnknownHostException e) {
            return -1;
        }
        byte[] addrBytes;
        int addr;
        addrBytes = inetAddress.getAddress();
        addr = ((addrBytes[3] & 0xff) << 24)
                | ((addrBytes[2] & 0xff) << 16)
                | ((addrBytes[1] & 0xff) << 8)
                |  (addrBytes[0] & 0xff);
        return addr;
    }

    public void setDependencyMet(boolean met) {
        // not supported on this network
    }

    public void setTeardownRequested(boolean isRequested) {
        mTeardownRequested.set(isRequested);
    }

    public boolean isTeardownRequested() {
        return mTeardownRequested.get();
    }

    /**
     * Check if private DNS route is set for the network
     */
    public boolean isPrivateDnsRouteSet() {
        return mPrivateDnsRouteSet.get();
    }

    /**
     * Set a flag indicating private DNS route is set
     */
    public void privateDnsRouteSet(boolean enabled) {
        mPrivateDnsRouteSet.set(enabled);
    }

    /**
     * Fetch NetworkInfo for the network
     */
    public NetworkInfo getNetworkInfo() {
        return new NetworkInfo(mNetworkInfo);
    }

    /**
     * Fetch Ethernet Connection State
     */
    public boolean IsNetworkConnected() {
        return ( mNetworkInfo.isConnected() && mCableConnected );
    }

    /**
     * Fetch Ethernet Cable Connection
     */
    public boolean IsCableConnected() {
        return mCableConnected;
    }

    /**
     * Fetch LinkProperties for the network
     */
    public LinkProperties getLinkProperties() {
        return new LinkProperties(mLinkProperties);
    }

    /**
     * A capability is an Integer/String pair, the capabilities
     * are defined in the class LinkSocket#Key.
     *
     * @return a copy of this connections capabilities, may be empty but never null.
     */
    public LinkCapabilities getLinkCapabilities() {
        return new LinkCapabilities();
    }

    /**
     * Check if default route is set
     */
    public boolean isDefaultRouteSet() {
        return mDefaultRouteSet.get();
    }

    /**
     * Set a flag indicating default route is set for the network
     */
    public void defaultRouteSet(boolean enabled) {
        mDefaultRouteSet.set(enabled);
    }

    /**
     * Return the system properties name associated with the tcp buffer sizes
     * for this network.
     */
    public String getTcpBufferSizesPropName() {
        return "net.tcp.buffersize.default";
    }

    //add by hqs@tcl---below
    @Override
    public void startSampling(SamplingDataTracker.SamplingSnapshot s) {
        mSamplingDataTracker.startSampling(s);
    }

    @Override
    public void stopSampling(SamplingDataTracker.SamplingSnapshot s) {
        mSamplingDataTracker.stopSampling(s);
    }

    @Override
    public String getNetworkInterfaceName() {
        if (mLinkProperties != null) {
            return mLinkProperties.getInterfaceName();
        } else {
            return null;
        }
    }
    @Override
    public void supplyMessenger(Messenger messenger) {
        // not supported on this network
    }
    @Override
    public void removeStackedLink(LinkProperties link) {
        mLinkProperties.removeStackedLink(link);
    }
    @Override
    public void addStackedLink(LinkProperties link) {
        mLinkProperties.addStackedLink(link);
    }
    /**
     * Captive check is complete, switch to network
     */
    @Override
    public void captivePortalCheckCompleted(boolean isCaptivePortal) {
        // not implemented
    }
    /**
     * Return link info
     * @return an object of type WifiLinkQualityInfo
     */
    //this func used just for compile temp---hqs@tcl
    @Override
    public LinkQualityInfo getLinkQualityInfo() {
        //if (mNetworkInfo == null) {
            // no data available yet; just return
            return null;
        //}
    }

}
