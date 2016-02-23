/*
 * Copyright (C) 2011 The Android Open Source Project
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
 */

package android.net.wifi.p2p;

import android.net.wifi.WpsInfo;
import android.os.Parcelable;
import android.os.Parcel;
import android.util.Log;
/**
 * A class representing a Wi-Fi P2p configuration for setting up a connection
 *
 * {@see WifiP2pManager}
 */
public class WifiP2pConfig implements Parcelable {

    /**
     * The device MAC address uniquely identifies a Wi-Fi p2p device
     */
    public String deviceAddress = "";

    /**
     * Wi-Fi Protected Setup information
     */
    public WpsInfo wps;

    /** @hide */
    public static final int MAX_GROUP_OWNER_INTENT   =   15;
    /** @hide */
    public static final int MIN_GROUP_OWNER_INTENT   =   0;
	public String rtspPort;

	public String wfdDeviceType;

    /**
     * This is an integer value between 0 and 15 where 0 indicates the least
     * inclination to be a group owner and 15 indicates the highest inclination
     * to be a group owner.
     *
     * A value of -1 indicates the system can choose an appropriate value.
     */
    public int groupOwnerIntent = -1;
    //Current peristent function has some problems,disable it
    //public int netId = WifiP2pGroup.PERSISTENT_NET_ID; //mark by bin
    /** @hide */
    public int netId = WifiP2pGroup.PERSISTENT_NET_ID;

    public WifiP2pConfig() {
        //set defaults
        wps = new WpsInfo();
        wps.setup = WpsInfo.PBC;
    }

    void invalidate() {
        deviceAddress = "";
    }

    /** P2P-GO-NEG-REQUEST 42:fc:89:a8:96:09 dev_passwd_id=4 {@hide}*/
    public WifiP2pConfig(String supplicantEvent) throws IllegalArgumentException {
        String[] tokens = supplicantEvent.split(" ");
        Log.d("WifiP2pConfig","tokens.length:"+tokens.length);
        if (tokens.length < 2 || (!tokens[0].equals("P2P-GO-NEG-REQUEST") && !tokens[0].equals("P2P-WFD-DEV-INFO"))) {
            throw new IllegalArgumentException("Malformed supplicant event");
        }

        deviceAddress = tokens[1];
        wps = new WpsInfo();

		
        for (String token : tokens) {
            String[] nameValue = token.split("=");
            if (nameValue.length != 2) continue;
            int devPasswdId;
         
			if (nameValue[0].equals("dev_passwd_id")) {
            try {
                devPasswdId = Integer.parseInt(nameValue[1]);
            } catch (NumberFormatException e) {
                devPasswdId = 0;
            }
            //Based on definitions in wps/wps_defs.h
            switch (devPasswdId) {
                //DEV_PW_USER_SPECIFIED = 0x0001,
                case 0x01:
                    wps.setup = WpsInfo.DISPLAY;
                    break;
                //DEV_PW_PUSHBUTTON = 0x0004,
                case 0x04:
                    wps.setup = WpsInfo.PBC;
                    break;
                //DEV_PW_REGISTRAR_SPECIFIED = 0x0005
                case 0x05:
                    wps.setup = WpsInfo.KEYPAD;
                    break;
                default:
                    wps.setup = WpsInfo.PBC;
                    break;
            }
                continue;
        }
			if (nameValue[0].equals("device_type")) {
                wfdDeviceType = nameValue[1];
                continue;
            }
			if (nameValue[0].equals("wfd_dev_info")) {
                 rtspPort=String.valueOf(parseHex(nameValue[1].substring(12,16)));
                 Log.d("WifiP2pConfig","rtspPort:"+rtspPort);
                continue;
            }
			
        }
		 //As defined in wps/wps_defs.h
            
    }

    public String toString() {
        StringBuffer sbuf = new StringBuffer();
        sbuf.append("\n address: ").append(deviceAddress);
        sbuf.append("\n wps: ").append(wps);
		sbuf.append("\n rtspPort: ").append(rtspPort);
		sbuf.append("\n wfdDeviceType: ").append(wfdDeviceType);
        sbuf.append("\n groupOwnerIntent: ").append(groupOwnerIntent);
        sbuf.append("\n persist: ").append(netId);
        return sbuf.toString();
    }

    /** Implement the Parcelable interface */
    public int describeContents() {
        return 0;
    }
    
    
    //supported formats: 0x1abc, 0X1abc, 1abc
    private int parseHex(String hexString) {
        int num = 0;
        if (hexString.startsWith("0x") || hexString.startsWith("0X")) {
            hexString = hexString.substring(2);
        }

        try {
            num = Integer.parseInt(hexString, 16);
        } catch(NumberFormatException e) {
            Log.e("WifiP2pConfig", "Failed to parse hex string " + hexString);
        }
        return num;
    }

    /** copy constructor */
    public WifiP2pConfig(WifiP2pConfig source) {
        if (source != null) {
            deviceAddress = source.deviceAddress;
            wps = new WpsInfo(source.wps);
			rtspPort = source.rtspPort;
			wfdDeviceType = source.wfdDeviceType;
            groupOwnerIntent = source.groupOwnerIntent;
            netId = source.netId;
        }
    }

    /** Implement the Parcelable interface */
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(deviceAddress);
        dest.writeParcelable(wps, flags);
		dest.writeString(rtspPort);
		dest.writeString(wfdDeviceType);
        dest.writeInt(groupOwnerIntent);
        dest.writeInt(netId);
    }

    /** Implement the Parcelable interface */
    public static final Creator<WifiP2pConfig> CREATOR =
        new Creator<WifiP2pConfig>() {
            public WifiP2pConfig createFromParcel(Parcel in) {
                WifiP2pConfig config = new WifiP2pConfig();
                config.deviceAddress = in.readString();
                config.wps = (WpsInfo) in.readParcelable(null);
				config.rtspPort = in.readString();
				config.wfdDeviceType = in.readString();
                config.groupOwnerIntent = in.readInt();
                config.netId = in.readInt();
                return config;
            }

            public WifiP2pConfig[] newArray(int size) {
                return new WifiP2pConfig[size];
            }
        };
}
