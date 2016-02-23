/*
 * Copyright (C) 2010 The Android-x86 Open Source Project
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

package com.android.server;

import java.net.UnknownHostException;
import android.net.ethernet.EthernetNative;
import android.net.ethernet.IEthernetManager;
import android.net.ethernet.EthernetManager;
import android.net.ethernet.EthernetStateTracker;
import android.net.ethernet.EthernetDevInfo;
import android.provider.Settings;
import android.provider.Settings.Global;
import android.util.Slog;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler; //HQS@TCL--20131105



/**
 * EthernetService handles remote Ethernet operation requests by implementing
 * the IEthernetManager interface. It also creates a EtherentMonitor to listen
 * for Etherent-related events.
 *
 * @hide
 */
public class EthernetService<syncronized> extends IEthernetManager.Stub {
    private static final String TAG = "EthernetService";
    private static final int ETHERNET_HAS_CONFIG = 1;
    private static final boolean localLOGV = true;

    private int mEthState= EthernetManager.ETHERNET_STATE_UNKNOWN;
    private Context mContext;
    private EthernetStateTracker mTracker;
    private String[] DevName;
    private int isEnabled ;
	private Handler mDelayedHandler; //HQS@TCL--20131105



 //{{lvh@tcl======================gaodw.  addd.==================================================
                 /**
          * Ethernet related configurations
          */
         public static final String ETHERNET_ON          = "ethernet_on";
         public static final String ETHERNET_MODE        = "ethernet_mode";
         public static final String ETHERNET_IP          = "ethernet_ip";
         public static final String ETHERNET_MASK        = "ethernet_netmask";
         public static final String ETHERNET_DNS         = "ethernet_dns";
         public static final String ETHERNET_DNS2        = "ethernet_dns2";
         public static final String ETHERNET_ROUTE       = "ethernet_iproute";
         public static final String ETHERNET_CONF        = "ethernet_conf";
         public static final String ETHERNET_IFNAME      = "ethernet_ifname";
         public static final String ETHERNET_PROXY_ON    = "ethernet_proxy_on";
         public static final String ETHERNET_PROXY_HOST  = "ethernet_proxy_host";
         public static final String ETHERNET_PROXY_PORT  = "ethernet_proxy_port";
         public static final String ETHERNET_PROXY_EXCLUSION = "ethernet_proxy_exclusion";
 //}}=============================================================================================





    public EthernetService(Context context, EthernetStateTracker Tracker) {
        mTracker = Tracker;
        mContext = context;

        isEnabled =  EthernetManager.ETHERNET_STATE_ENABLED;//getPersistedState();
        if (localLOGV == true) Slog.i(TAG, "Ethernet dev enabled " + isEnabled);
        getDeviceNameList();
        setState(isEnabled);
        registerForBroadcasts();//HQS@TCL--20131105
        mTracker.StartPolling();
		mDelayedHandler = new Handler(); 
    }

    /**
     * check if the ethernet service has been configured.
     * @return {@code true} if configured {@code false} otherwise
     */
    public boolean isConfigured() {
        final ContentResolver cr = mContext.getContentResolver();
        return (Settings.Global.getInt(cr, Settings.Global.ETHERNET_CONF, 0) == ETHERNET_HAS_CONFIG);

    }

    /**
     * Return the saved ethernet configuration
     * @return ethernet interface configuration on success, {@code null} on failure
     */
    public synchronized EthernetDevInfo getSavedConfig() {
        if (!isConfigured())
            return null;
/*
        final ContentResolver cr = mContext.getContentResolver();
        EthernetDevInfo info = new EthernetDevInfo();
        info.setConnectMode(Settings.Global.getString(cr, Settings.Global.ETHERNET_MODE));
        info.setIfName(Settings.Global.getString(cr, Settings.Global.ETHERNET_IFNAME));
        info.setIpAddress(Settings.Global.getString(cr, Settings.Global.ETHERNET_IP));
        info.setDnsAddr(Settings.Global.getString(cr, Settings.Global.ETHERNET_DNS));
        info.setDns2Addr(Settings.Global.getString(cr, Settings.Global.ETHERNET_DNS2));
        info.setNetMask(Settings.Global.getString(cr, Settings.Global.ETHERNET_MASK));
        info.setRouteAddr(Settings.Global.getString(cr, Settings.Global.ETHERNET_ROUTE));
        info.setProxyOn((Settings.Global.getInt(cr, Settings.Global.ETHERNET_PROXY_ON, 0)==1)?true:false);
        info.setProxyHost(Settings.Global.getString(cr, Settings.Global.ETHERNET_PROXY_HOST));
        info.setProxyPort(Settings.Global.getString(cr, Settings.Global.ETHERNET_PROXY_PORT));
	info.setProxyExclusionList(Settings.Global.getString(cr, Settings.Global.ETHERNET_PROXY_EXCLUSION));
*/



         final ContentResolver cr = mContext.getContentResolver();
         EthernetDevInfo info = new EthernetDevInfo();
         info.setConnectMode(Settings.Global.getString(cr, ETHERNET_MODE));
         info.setIfName(Settings.Global.getString(cr, ETHERNET_IFNAME));
         info.setIpAddress(Settings.Global.getString(cr, ETHERNET_IP));
         info.setDnsAddr(Settings.Global.getString(cr, ETHERNET_DNS));
         info.setDns2Addr(Settings.Global.getString(cr,ETHERNET_DNS2));
         info.setNetMask(Settings.Global.getString(cr, ETHERNET_MASK));
         info.setRouteAddr(Settings.Global.getString(cr, ETHERNET_ROUTE));
         info.setProxyOn((Settings.Global.getInt(cr, ETHERNET_PROXY_ON, 0)==1)?true:false);
         info.setProxyHost(Settings.Global.getString(cr, ETHERNET_PROXY_HOST));
         info.setProxyPort(Settings.Global.getString(cr, ETHERNET_PROXY_PORT));
         info.setProxyExclusionList(Settings.Global.getString(cr, ETHERNET_PROXY_EXCLUSION));






        return info;


    }

    /*
*
     * Set the ethernet interface configuration mode
     * @param mode {@code ETHERNET_CONN_MODE_DHCP} for dhcp {@code ETHERNET_CONN_MODE_MANUAL} for manual configure
     */
    public synchronized void setMode(String mode) {
        final ContentResolver cr = mContext.getContentResolver();
        if (DevName != null) {
			// liukun add for amlogic error devname
			int i;
			for(i=0; i<DevName.length; i++)
			{
				if(DevName[i].equals("eth0"))
					break;
					
			}
			if(i == DevName.length)
			{
				if (localLOGV) Slog.v(TAG, "setMode error!!!  no devname is eth0!!!" );
				return;
			}
			Settings.Global.putString(cr, Settings.Global.ETHERNET_IFNAME, "eth0");
			//	liukun end
            Settings.Global.putInt(cr, Settings.Global.ETHERNET_CONF, 1);
            Settings.Global.putString(cr, Settings.Global.ETHERNET_MODE, mode);
        }
    }

    /**
     * update a ethernet interface information
     * @param info  the interface infomation
     */
    public synchronized void updateDevInfo(EthernetDevInfo info) {
        final ContentResolver cr = mContext.getContentResolver();

/*        Settings.Global.putInt(cr, Settings.Global.ETHERNET_CONF, 1);
        Settings.Global.putString(cr, Settings.Global.ETHERNET_IFNAME, info.getIfName());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_IP, info.getIpAddress());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_MODE, info.getConnectMode());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_DNS, info.getDnsAddr());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_DNS2, info.getDns2Addr());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_ROUTE, info.getRouteAddr());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_MASK, info.getNetMask());
        Settings.Global.putInt(cr, Settings.Global.ETHERNET_PROXY_ON, ((info.getProxyOn()==true)?1:0));
        Settings.Global.putString(cr, Settings.Global.ETHERNET_PROXY_HOST, info.getProxyHost());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_PROXY_PORT, info.getProxyPort());
        Settings.Global.putString(cr, Settings.Global.ETHERNET_PROXY_EXCLUSION, info.getProxyExclusionList());
  */      

        Settings.Global.putInt(cr, ETHERNET_CONF, 1);
        Settings.Global.putString(cr, ETHERNET_IFNAME, info.getIfName());
        Settings.Global.putString(cr, ETHERNET_IP, info.getIpAddress());
        Settings.Global.putString(cr, ETHERNET_MODE, info.getConnectMode());
        Settings.Global.putString(cr, ETHERNET_DNS, info.getDnsAddr());
        Settings.Global.putString(cr, ETHERNET_DNS2, info.getDns2Addr());
        Settings.Global.putString(cr, ETHERNET_ROUTE, info.getRouteAddr());
        Settings.Global.putString(cr, ETHERNET_MASK, info.getNetMask());
        Settings.Global.putInt(cr, ETHERNET_PROXY_ON, ((info.getProxyOn()==true)?1:0));
        Settings.Global.putString(cr, ETHERNET_PROXY_HOST, info.getProxyHost());
        Settings.Global.putString(cr, ETHERNET_PROXY_PORT, info.getProxyPort());
        Settings.Global.putString(cr, ETHERNET_PROXY_EXCLUSION, info.getProxyExclusionList());


	if (mEthState == EthernetManager.ETHERNET_STATE_ENABLED) {
            try {
                mTracker.resetInterface();
            } catch (UnknownHostException e) {
                Slog.e(TAG, "Wrong ethernet configuration");
            }
        }
    }
	
	//add for ethernet monitor when sys suspend - HQS@TCL--20131105--start
    private void registerForBroadcasts() {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Intent.ACTION_SCREEN_ON);
        intentFilter.addAction(Intent.ACTION_SCREEN_OFF);
        mContext.registerReceiver(mReceiver, intentFilter);
    }
	//add for ethernet monitor when sys suspend - HQS@TCL--20131105--end
	
    /**
     * get the number of ethernet interfaces in the system
     * @return the number of ethernet interfaces
     */
    public int getTotalInterface() {
        return EthernetNative.getInterfaceCnt();
    }


    private int scanDevice() {
        int i, j;
        if ((i = EthernetNative.getInterfaceCnt()) == 0)
            return 0;

        DevName = new String[i];

        for (j = 0; j < i; j++) {
            DevName[j] = EthernetNative.getInterfaceName(j);
            if (DevName[j] == null)
                break;
            if (localLOGV) Slog.v(TAG, "device " + j + " name " + DevName[j]);
        }

        return i;
    }

    /**
     * get all the ethernet device names
     * @return interface name list on success, {@code null} on failure
     */
    public String[] getDeviceNameList() {
        return (scanDevice() > 0) ? DevName : null;
    }

    private int getPersistedState() {
        final ContentResolver cr = mContext.getContentResolver();
        try {
            return Settings.Global.getInt(cr, Settings.Global.ETHERNET_ON);
        } catch (Settings.SettingNotFoundException e) {
            return EthernetManager.ETHERNET_STATE_UNKNOWN;
        }
    }

    private synchronized void persistEnabled(boolean enabled) {
        final ContentResolver cr = mContext.getContentResolver();
        Settings.Global.putInt(cr, Settings.Global.ETHERNET_ON, enabled ? EthernetManager.ETHERNET_STATE_ENABLED : EthernetManager.ETHERNET_STATE_DISABLED);
    }
   //add for ethernet monitor when sys suspend - HQS@TCL--20131105--start
   private final Runnable mResetInterface = new Runnable() {
        public void run() {
            try {
                mTracker.resetInterface();
                Slog.i(TAG, "$$ mResetInterface call resetInterface()");
            } catch (UnknownHostException e) {
                Slog.e(TAG, "Wrong ethernet configuration");
            }
        }
    };
	
	
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (action.equals(Intent.ACTION_SCREEN_ON)) {
                Slog.d(TAG, "ACTION_SCREEN_ON");
                if(getPersistedState() == EthernetManager.ETHERNET_STATE_ENABLED){
                    mDelayedHandler.postDelayed(mResetInterface, 5*1000); // wait 5s for device ready
                }
            } else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                Slog.d(TAG, "ACTION_SCREEN_OFF");
                mDelayedHandler.removeCallbacks(mResetInterface);
                mTracker.disableInterface(false);
            }
        }
    };
	//add for ethernet monitor when sys suspend - HQS@TCL--20131105--end
    /**
     * Enable or Disable a ethernet service
     * @param enable {@code true} to enable, {@code false} to disable
     */
    public synchronized void setState(int state) {

        if (mEthState != state) {
            mEthState = state;
            if (state == EthernetManager.ETHERNET_STATE_DISABLED) {
                persistEnabled(false);
                mTracker.stopInterface(false);
            } else {
                persistEnabled(true);
                if (!isConfigured()) {
                    // If user did not configure any interfaces yet, pick the first one
                    // and enable it.
                    setMode(EthernetDevInfo.ETHERNET_CONN_MODE_DHCP);
                }
                /*try {
                    mTracker.resetInterface();//commented by HQS----20131125
                } catch (UnknownHostException e) {
                    Slog.e(TAG, "Wrong ethernet configuration");
                }*/
            }
        }
    }

    /**
     * Get ethernet service state
     * @return the state of the ethernet service
     */
    public int getState( ) {
        return mEthState;
    }

    public boolean IsNetworkConnected( ) {
        return mTracker.IsNetworkConnected();
    }

    public boolean IsCableConnected( ) {
        return mTracker.IsCableConnected();
    }

}
