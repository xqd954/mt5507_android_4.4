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

import android.net.ethernet.EthernetDevInfo;
import android.os.Parcel;
import android.os.Parcelable;
import android.os.Parcelable.Creator;
import java.io.*;

/**
 * Describes the state of any Ethernet connection that is active or
 * is in the process of being set up.
 */

public class EthernetDevInfo implements Parcelable {
    /**
     * The ethernet interface is configured by dhcp
     */
    public static final String ETHERNET_CONN_MODE_DHCP= "dhcp";
    /**
     * The ethernet interface is configured manually
     */
    public static final String ETHERNET_CONN_MODE_MANUAL = "manual";

    private String dev_name;
    private String ipaddr;
    private String netmask;
    private String route;
    private String dns;
    private String dns2;
    private String mode;
    private int proxy_on;
    private String proxy_host;
    private String proxy_port;
    private String proxy_exclusion;

    public EthernetDevInfo () {
        dev_name = null;
        ipaddr = null;
        dns = null;
        dns2 = null;
        route = null;
        netmask = null;
        mode = ETHERNET_CONN_MODE_DHCP;
        proxy_on = 0;
        proxy_host = null;
        proxy_port = null;
        proxy_exclusion = null;
    }

    /**
     * save interface name into the configuration
     */
    public void setIfName(String ifname) {
        //Slog.i(TAG, "setIfName:"+ifname);
        if (ifname!=null)
            this.dev_name = ifname;
    }

    /**
     * Returns the interface name from the saved configuration
     * @return interface name
     */
    public String getIfName() {
        return this.dev_name;
    }

    public void setIpAddress(String ip) {
        //Slog.i(TAG, "setIpAddress:"+ip);
        if (ip!=null)
            this.ipaddr = ip;
    }

    public String getIpAddress( ) {
        return this.ipaddr;
    }

    public void setNetMask(String ip) {
        //Slog.i(TAG, "setNetMask:"+ip);
        if (ip!=null)
            this.netmask = ip;
    }

    public String getNetMask( ) {
        return this.netmask;
    }

    public void setRouteAddr(String route) {
        //Slog.i(TAG, "setRouteAddr:"+route);
        if (route!=null)
            this.route = route;
    }

    public String getRouteAddr() {
        return this.route;
    }

    public void setDnsAddr(String dns) {
        //Slog.i(TAG, "setDnsAddr:"+dns);
        if (dns!=null)
            this.dns = dns;
    }

    public String getDnsAddr( ) {
        return this.dns;
    }

    public void setDns2Addr(String dns2) {
        //Slog.i(TAG, "setDns2Addr:"+dns);
        if (dns2!=null)
            this.dns2 = dns2;
    }

    public String getDns2Addr( ) {
        return this.dns2;
    }

    /**
     * Set ethernet configuration mode
     * @param mode {@code ETHERNET_CONN_MODE_DHCP} for dhcp {@code ETHERNET_CONN_MODE_MANUAL} for manual configure
     * @return
     */
    public boolean setConnectMode(String mode) {
        //Slog.i(TAG, "setConnectMode:"+mode);
        if (mode != null) {
            if (mode.equals(ETHERNET_CONN_MODE_DHCP) || mode.equals(ETHERNET_CONN_MODE_MANUAL)) {
                this.mode = mode;
                return true;
            }
        }
        return false;
    }

    public String getConnectMode() {
        return this.mode;
    }

    public void setProxyOn(boolean enabled) {
        this.proxy_on = (enabled ? 1 : 0);
    }

    public boolean getProxyOn() {
        return ( (this.proxy_on==1) ? true : false );
    }

    public void setProxyHost(String host) {
        //Slog.i(TAG, "setProxyHost:"+host);
        if ( host != null ) {
            this.proxy_host = host;
        }
    }

    public String getProxyHost( ) {
        return this.proxy_host;
    }

    public void setProxyPort(String port) {
        //Slog.i(TAG, "setProxyPort:"+port);
        if ( port != null ) {
            this.proxy_port = port;
        }
    }

    public String getProxyPort( ) {
        return this.proxy_port;
    }

    public void setProxyExclusionList(String exclusion) {
        if ( exclusion != null ) {
            this.proxy_exclusion = exclusion;
        }
    }

    public String getProxyExclusionList( ) {
        return this.proxy_exclusion;
    }

    public int describeContents() {
        return 0;
    }

    /*
     * Load file content to String
     */
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

    /*
     * Get device MacAddress
     */
    public String getMacAddress(){
        try {
            return loadFileAsString("/sys/class/net/eth0/address")
            .toUpperCase().substring(0, 17);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(this.dev_name);
        dest.writeString(this.ipaddr);
        dest.writeString(this.netmask);
        dest.writeString(this.route);
        dest.writeString(this.dns);
        dest.writeString(this.dns2);
        dest.writeString(this.mode);
        dest.writeInt(this.proxy_on);
        dest.writeString(this.proxy_host);
        dest.writeString(this.proxy_port);
        dest.writeString(this.proxy_exclusion);
    }

    /** Implement the Parcelable interface {@hide} */
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
            info.setProxyOn((in.readInt()==1)?true:false);
            info.setProxyHost(in.readString());
            info.setProxyPort(in.readString());
            info.setProxyExclusionList(in.readString());
            return info;
        }

        public EthernetDevInfo[] newArray(int size) {
            return new EthernetDevInfo[size];
        }
    };
}
