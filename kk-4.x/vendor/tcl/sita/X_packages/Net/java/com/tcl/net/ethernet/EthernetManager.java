/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package com.tcl.net.ethernet;

import java.util.List;
import android.annotation.SdkConstant;
import android.annotation.SdkConstant.SdkConstantType;
import android.os.RemoteException;
import android.util.Slog;
import android.net.ethernet.IEthernetManager;
import android.net.ethernet.EthernetDevInfo;
import android.os.IBinder;
import android.content.Context;
import android.os.ServiceManager;

/**
 * This class provides the primary API for managing all aspects of Ethernet
 * connectivity. Get an instance of this class by calling
 * {@link android.content.Context#getSystemService(String) Context.getSystemService(Context.ETHERNET_SERVICE)}.
 *
 * This is the API to use when performing Ethernet specific operations. To
 * perform operations that pertain to network connectivity at an abstract
 * level, use {@link android.net.ConnectivityManager}.
 */
public class EthernetManager {
    public static final String TAG = "EthernetManager";
    public static final int ETHERNET_DEVICE_SCAN_RESULT_READY = 0;
    public static final String ETHERNET_STATE_CHANGED_ACTION =
                   "android.net.ethernet.ETHERNET_STATE_CHANGED";
    public static final String NETWORK_STATE_CHANGED_ACTION =
                   "android.net.ethernet.STATE_CHANGE";
            

	  public static final String ETHERNET_INTERFACE_CONF_CHANGED = 
			        "android.net.ethernet.ETHERNET_INTERFACE_CONF_CHANGED";


    public static final String EXTRA_NETWORK_INFO = "networkInfo";
    public static final String EXTRA_ETHERNET_STATE = "ETHERNET_state";
    public static final String EXTRA_PREVIOUS_ETHERNET_STATE = "previous_ETHERNET_state";

    /**
     * The lookup key for a {@link android.net.LinkProperties} object associated with the
     * Ethernet network. Retrieve with
     * {@link android.content.Intent#getParcelableExtra(String)}.
     * @hide
     */
    public static final String EXTRA_LINK_PROPERTIES = "linkProperties";

    /**
     * The lookup key for a {@link android.net.LinkCapabilities} object associated with the
     * Ethernet network. Retrieve with
     * {@link android.content.Intent#getParcelableExtra(String)}.
     * @hide
     */
    public static final String EXTRA_LINK_CAPABILITIES = "linkCapabilities";

    public static final int ETHERNET_STATE_UNKNOWN = 0;
    public static final int ETHERNET_STATE_DISABLED = 1;
    public static final int ETHERNET_STATE_ENABLED = 2;

	static final Object mInstanceSync = new Object();
    static EthernetManager mInstance = null;
    IEthernetManager mService;
    //Handler mHandler;

    private EthernetManager(IEthernetManager service) {
        mService = service;
    }

    /**     
     * Get the EthernetManager instance.
     * @author TCL TVOS Team
     * @param null
     * @return EthernetManager instance.
     */
	static public EthernetManager getInstance() {
        /* Double-checked locking */
        if (mInstance == null) {
            synchronized (mInstanceSync) {
                if (mInstance == null) {
                    IBinder b = ServiceManager.getService(Context.ETHERNET_SERVICE);
                    mInstance = new EthernetManager(IEthernetManager.Stub.asInterface(b));
                }
            }
        }

        return mInstance;
    }

     /**
     * check if the ethernet service has been configured.
     * @author TCL TVOS Team
     * @param null
     * @return {@code true} if configured {@code false} otherwise
     */
    public boolean isConfigured() {
        try {
            return mService.isConfigured();
        } catch (RemoteException e) {
            Slog.i(TAG, "Can not check eth config state");
        }
        return false;
    }

    /**
     * Get the saved ethernet configuration
     * @author TCL TVOS Team
     * @param null
     * @return ethernet interface configuration on success, {@code null} on failure
     */
    public com.tcl.net.ethernet.EthernetDevInfo getSavedConfig() {
        try {
			android.net.ethernet.EthernetDevInfo ainfo = mService.getSavedConfig();
			com.tcl.net.ethernet.EthernetDevInfo myinfo = new com.tcl.net.ethernet.EthernetDevInfo(ainfo);
			return myinfo;
        } catch (RemoteException e) {
            Slog.i(TAG, "Can not get eth config");
        }
        return null;
    }

    /**
     * Update ethernet interface information
     * @author TCL TVOS Team
     * @param info  the Ethernet interface infomation
     */
    public void updateDevInfo(com.tcl.net.ethernet.EthernetDevInfo info) {
        try {
            mService.updateDevInfo(info);
        } catch (RemoteException e) {
            Slog.i(TAG, "Can not update ethernet device info");
        }
    }

    /**
     * Get all the ethernet interfaces
     * @author TCL TVOS Team
     * @param null
     * @return interface name list on success, {@code null} on failure
     */
    public String[] getDeviceNameList() {
        try {
            return mService.getDeviceNameList();
        } catch (RemoteException e) {
            return null;
        }
    }
    /**
     * Enable or Disable  ethernet
     * @author TCL TVOS Team
     * @param enable {@code true} to enable, {@code false} to disable
     * @hide
     */
    public void setEnabled(boolean enable) {
        try {
            mService.setState(enable ? ETHERNET_STATE_ENABLED:ETHERNET_STATE_DISABLED);
        } catch (RemoteException e) {
            Slog.i(TAG,"Can not set new state");
        }
    }

    /**
     * Get ethernet state
     * @author TCL TVOS Team
     * @param null
     * @return the state of the ethernet:
     * 0:ETHERNET_STATE_UNKNOWN/1:ETHERNET_STATE_DISABLED/2:ETHERNET_STATE_ENABLED
     */
    public int getState( ) {
        try {
            return mService.getState();
        } catch (RemoteException e) {
            return 0;
        }
    }

    /**
     * Get the number of ethernet interfaces in the system
     * @author TCL TVOS Team
     * @param null
     * @return the number of ethernet interfaces
     */
    public int getTotalInterface() {
        try {
            return mService.getTotalInterface();
        } catch (RemoteException e) {
            return 0;
        }
    }

    /**
     *Set as the default mode of ethernet( dhcp)
     * @author TCL TVOS Team
     * @param null
     * @hide
     */
    public void setDefaultConf() {
        try {
            mService.setMode(EthernetDevInfo.ETHERNET_CONN_MODE_DHCP);
        } catch (RemoteException e) {
        }
    }

    /**
     *Get the connectivity state  of ethernet
     * @author TCL TVOS Team
     * @param null
     * @return  {@code true} if Connected {@code false} otherwise
     */
    public boolean IsNetworkConnected() {
        try {
            return mService.IsNetworkConnected();
        } catch (RemoteException e) {
            Slog.i(TAG, "Can not check eth connected state");
        }
        return false;
    }

    /**
     *Get  cable state  of ethernet
     * @author TCL TVOS Team
     * @param null
     * @return  {@code true} if cable Connected {@code false} otherwise
     */
    public boolean IsCableConnected() {
        try {
            return mService.IsCableConnected();
        } catch (RemoteException e) {
            Slog.i(TAG, "Can not check eth cable state");
        }
        return false;
    }
}
