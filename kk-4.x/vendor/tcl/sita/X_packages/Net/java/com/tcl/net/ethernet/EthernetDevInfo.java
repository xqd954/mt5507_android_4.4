/*
 * Copyright (C) 2006 The Android Open Source Project
 * 
 * ©2010-2013 TCL CORPORATION All Rights Reserved.
 */

package com.tcl.net.ethernet;

import java.io.*;
import android.util.Slog;


/**
 * Describes the state of any Ethernet connection that is active or
 * is in the process of being set up.
 */

public class EthernetDevInfo extends android.net.ethernet.EthernetDevInfo{
	private static final String TAG = "tcl_EthernetDevInfo";
    /** The ethernet interface is configured by dhcp **/
    public static final String ETHERNET_CONN_MODE_DHCP= "dhcp";

    /**  The ethernet interface is configured manually **/
    public static final String ETHERNET_CONN_MODE_MANUAL = "manual";

    public EthernetDevInfo () {
        super();
    }


	/**
	 * @hide
	 */
	public EthernetDevInfo(android.net.ethernet.EthernetDevInfo info){
		super();
		setIfName( info.getIfName() );
        setIpAddress( info.getIpAddress() );
        setDnsAddr( info.getDnsAddr() );
        setDns2Addr( info.getDns2Addr() );
        setRouteAddr( info.getRouteAddr() );
        setNetMask( info.getNetMask() );
        setConnectMode( info.getConnectMode() );
        setProxyOn( info.getProxyOn() );
        setProxyHost( info.getProxyHost() );
        setProxyPort( info.getProxyPort() );
	}

    /**
     * Save interface name into the configuration
     * @author TCL TVOS Team
     * @param ifname the interface name that you want to set
     */
	@Override
    public void setIfName(String ifname) {
        super.setIfName(ifname);
    }

    /**
     * Get the interface name from the saved configuration
     * @author TCL TVOS Team
     * @param null
     * @return interface name
     */
	@Override
    public String getIfName() {
        return super.getIfName();
    }

    /**
     * Set the IP Address of ethernet interface
     * @author TCL TVOS Team
     * @param ip the ip that you want to set
     */
	@Override
    public void setIpAddress(String ip) {
        super.setIpAddress(ip);
    }
    
     /**
     * Get the IP Address of ethernet interface
     * @author TCL TVOS Team
     * @param null
     * @return  the ip address of ethernet interface
     */
	@Override
    public String getIpAddress( ) {
        return super.getIpAddress();
    }

     /**
     * Set the IP Address of mask
     * @author TCL TVOS Team
     * @param  ip the ip of mask
     */
	@Override
    public void setNetMask(String ip) {
        super.setNetMask(ip);
    }

     /**
     * Get the mask of ethernet interface
     * @author TCL TVOS Team
     * @param null
     * @return the netmask of ethernet interface
     */
	@Override
    public String getNetMask( ) {
        return super.getNetMask();
    }

     /**
     * Set the route of ethernet
     * @author TCL TVOS Team
     * @param route the route address
     */
	@Override
    public void setRouteAddr(String route) {
        super.setRouteAddr(route);
    }

     /**
     * Get the route address of ethernet
     * @author TCL TVOS Team
     * @param null
     * @return the route address of ethernet
     */
	@Override
    public String getRouteAddr() {
        return super.getRouteAddr();
    }

     /**
     * Set the primary DNS address
     * @author TCL TVOS Team
     * @param dns the primary dns address that you want to set
     */
	@Override
    public void setDnsAddr(String dns) {
        super.setDnsAddr(dns);
    }

     /**
     * Get the primary DNS address
     * @author TCL TVOS Team
     * @param null
     * @return the primary DNS address of ethernet
     */
	@Override
    public String getDnsAddr( ) {
        return super.getDnsAddr();
    }

     /**
     * Set the secondary DNS address
     * @author TCL TVOS Team
     * @param the secondary dns address that you want to set
     */
	@Override
    public void setDns2Addr(String dns2) {
        super.setDns2Addr(dns2);
    }

     /**
     * Get the secondary DNS address
     * @author TCL TVOS Team
     * @param null
     * @return the secondary DNS address of ethernet
     */
	@Override
    public String getDns2Addr( ) {
        return super.getDns2Addr();
    }

    /**
     * Set ethernet configuration mode
     * @author TCL TVOS Team
     * @param mode {@code ETHERNET_CONN_MODE_DHCP} for dhcp {@code ETHERNET_CONN_MODE_MANUAL} for manual configure
     * @return  {@code true} if set successed {@code false} otherwise
     */
	@Override
    public boolean setConnectMode(String mode) {
        return super.setConnectMode(mode);
    }

	@Override
    public String getConnectMode() {
        return super.getConnectMode();
    }

    /**
     * Set ethernet proxy state
     * @author TCL TVOS Team
     * @param enabled enable or disable ethernet proxy
     */
	@Override
    public void setProxyOn(boolean enabled) {
		super.setProxyOn(enabled);
    }

    /**
     * Get ethernet proxy state
     * @author TCL TVOS Team
     * @return {@code true} if enabled {@code false} otherwise
     */
	@Override
    public boolean getProxyOn() {
        return super.getProxyOn();
    }

    /**
     * Set ethernet proxy host
     * @author TCL TVOS Team
     * @param host the host name
     */
	@Override
    public void setProxyHost(String host) {
        super.setProxyHost(host);
    }

	@Override
    public String getProxyHost( ) {
        return super.getProxyHost();
    }

    /**
     * Set ethernet proxy port
     * @author TCL TVOS Team
     * @param port the port that proxy used
    */
	@Override
    public void setProxyPort(String port) {
        super.setProxyPort(port);
    }

	@Override
    public String getProxyPort( ) {
        return super.getProxyPort();
    }


    /*
     * Get device MacAddress
     * @author TCL TVOS Team
     * @return the mac address of ethernet interface
     */
	 @Override
    public String getMacAddress(){
		return super.getMacAddress();
		/*
        try {
            return loadFileAsString("/sys/class/net/eth0/address")
            .toUpperCase().substring(0, 17);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
		*/
    }
	////////////////// Following not use by apk	
    /**
     * 
     * @author TCL TVOS Team
     * @param null
     * @return 
     */
	 /*
    public int describeContents() {
        return 0;
    }
	*/
    /*
     * Load file content to String
     */
	 /*
    private String loadFileAsString(String filePath) throws java.io.IOException{
        StringBuffer fileData = new StringBuffer(1000);
        BufferedReader reader = new BufferedReader(new FileReader(filePath));
        char[] buf = new char[1024];
        int numRead=0;
        while ((numRead=reader.read(buf)) != -1) {
            String readData = String.valueOf(buf, 0, numRead);
            fileData.append(readData);
        }
        reader.close();
        return fileData.toString();
    }
	*/

    /*
     * write data to parcel
     * @author TCL TVOS Team
     * @param dest : the parcel destination side;flags : the flag of parcel attribute
     */
	 /*
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(this.dev_name);
        dest.writeString(this.ipaddr);
        dest.writeString(this.netmask);
        dest.writeString(this.route);
        dest.writeString(this.dns);
        dest.writeString(this.dns2);
        dest.writeString(this.mode);
    }
	*/
    /** Implement the Parcelable interface
     * @author TCL TVOS Team
     * @param EthernetDevInfo  ethernet configuration
     */
	 /*
    public static final Creator<EthernetDevInfo> CREATOR = new Creator<EthernetDevInfo>() {
        public EthernetDevInfo createFromParcel(Parcel in) {
            EthernetDevInfo info = new EthernetDevInfo();
            info.setIfName(in.readString());
            info.setIpAddress(in.readString());
            info.setNetMask(in.readString());
            info.setRouteAddr(in.readString());
            info.setDnsAddr(in.readString());
            info.setDns2Addr(in.readString());
            info.setConnectMode(in.readString());
            return info;
        }
	*/
    /*
     * creat the ethernetdevinfo array
     * @author TCL TVOS Team
     * @param size the array size that you want to set
     */
	 /*
        public EthernetDevInfo[] newArray(int size) {
            return new EthernetDevInfo[size];
        }
		
    };
	*/
	///// lvh@tcl add for ms628, 
    public void setProxyExclusionList(String exclusion) {
         super.setProxyExclusionList(exclusion);
    }

    public String getProxyExclusionList( ) {
        return super.getProxyExclusionList();
    }

}
