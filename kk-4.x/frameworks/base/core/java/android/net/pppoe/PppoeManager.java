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



package android.net.pppoe;

import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;
import android.os.ServiceManager;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import java.io.File;
import java.io.Reader;
import java.io.FileWriter;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.io.FileNotFoundException;
import java.io.IOException;

public class PppoeManager {
    private static final String TAG = "PppoeManager";
    private static Thread mSocketThread = null;

    private boolean mIsDialing = false;

    private String user = null;
    private String passwd = null;
    private String netif = null;
    private static final String NET_INTERFACE = "net_if";
    private static final String USER_NAME = "user_name";
    private static final String PASS_WORD = "pass_word";
    private PPPOE_STA gpppoe_sta = PPPOE_STA.DISCONNECTED;

    public static final String PPPOE_STATE_CONNECT = "connect";
    public static final String PPPOE_STATE_DISCONNECT = "disconnect";
    public static final String PPPOE_STATE_CONNECTING = "connecting";
    public static final String PPPOE_STATE_DISCONNECTING = "disconnecting";
    public static final String PPPOE_STATE_AUTHFAILED = "authfailed";
    public static final String PPPOE_STATE_LINKTIMEOUT = "linktimeout";
    public static final String PPPOE_STATE_FAILED = "failed";

    public static final int MSG_PPPOE_CONNECT = 0;
    public static final int MSG_PPPOE_DISCONNECT = 1;
    public static final int MSG_PPPOE_CONNECTING = 2;
    public static final int MSG_PPPOE_DISCONNECTING = 3;
    public static final int MSG_PPPOE_AUTH_FAILED = 4;
    public static final int MSG_PPPOE_TIME_OUT = 5;
    public static final int MSG_PPPOE_FAILED = 6;

    public static final String PPPOE_STATE_ACTION = "android.net.pppoe.PPPOE_STATE_ACTION";
    public static final String PPPOE_STATE_STATUE = "PppoeStatus";

    static final Object mInstanceSync = new Object();
    static PppoeManager mInstance = null;
    IPppoeManager mService = null;


     public PppoeManager(IPppoeManager pppoeService) {

		if (pppoeService == null) {

			Log.e(TAG, "pppoe service is null");

		}

	    mService = pppoeService;

	    init();

	    Log.i(TAG,"create PppoeManager");

	}    

     /**
     *function:connectPppoe
     */
    public void connectPppoe(final String account, final String password) {
        connectPppoe(account, password, "eth0");
    }

    /**
     *function:connectPppoe
     */
    public void connectPppoe(final String account, final String password, final String net) {
        Log.i(TAG, "connectPppoe");
        Thread connectThread = new Thread("pppoe_dialup_thread") {
            public void run() {
                PppoeSetUser(account);
                PppoeSetPW(password);
                PppoeSetInterface(net);
                PppoeDialup();
            }
        };
        connectThread.start();
    }

    /**
     *function:disconnectPppoe
     */
    public void disconnectPppoe() {
        Log.i(TAG, "disconnectPppoe");
        PppoeHangUp();
    }

    /**
     *function:get interface of ppp
     */
    public String getInterfaceName() {
        return readPppInfoFromFile("ifname");
    }

    /**
     *function:get ip of ppp
     */
    public String getIpaddr() {
        return readPppInfoFromFile("ip");
    }

    /**
     *function:get route of ppp
     */
    public String getRoute() {
        return readPppInfoFromFile("route");
    }

    /**
     *function:get net mask of ppp
     */
    public String getMask() {
        return readPppInfoFromFile("mask");
    }

    /**
     *function:get primary dns
     */
    public String getDns1() {
        return readPppInfoFromFile("dns1");
    }

    /**
     *function:get secondary dns
     */
    public String getDns2() {
        return readPppInfoFromFile("dns2");
    }

    /**
     *function:set pppoe status
     */
    private void setPppoeStatus(String status, boolean sendBroadcast) {
        try {
            mService.setPppoeStatus(status, sendBroadcast);
        } catch (RemoteException e) {
        }
    }

    /**
     *function:get pppoe status
     */
    public String getPppoeStatus() {
        try {
            return mService.getPppoeStatus();
        } catch (RemoteException e) {
            return null;
        }
    }

    /**
     *function:set net interface
     */
    public boolean PppoeSetInterface(String pu8NetIf) {
        if (pu8NetIf == null || pu8NetIf.equals("")) {
            return false;
        }
        netif = pu8NetIf;
        writeFile();
        setup();
        return true;
    }

    /**
     *function:set username
     */
    public boolean PppoeSetUser(String Suser) {
        if (Suser == null || Suser.equals("")) {
            return false;
        }
        user = Suser;
        writeFile();
        setup();
        return true;
    }

    /**
     *function:set password
     */
    public boolean PppoeSetPW(String Spasswd) {
        if (Spasswd == null || Spasswd.equals("")) {
            return false;
        }
        passwd = Spasswd;
        writeFile();
        setup();
        return true;
    }


    /**
     *function:write ppp setting to file
     */
    private synchronized void writeFile() {
        FileWriter fw = null;
        try {
            fw = new FileWriter("/data/misc/ppp/pppoe.data");
            fw.write(USER_NAME + "=" + user + "\n");
            fw.write(PASS_WORD + "=" + passwd + "\n");
            fw.write(NET_INTERFACE + "=" + netif + "\n");
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } finally {
            if (fw != null) {
                try {
                    fw.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        } //end try
    }

    /**
     *function:setup
     */
    private void setup() {
        SystemProperties.set("ctl.start", "pppoe-setup");
        while (true) {
            if (!isSetupRunning()) {
                break;
            }
        }
    }

    /**
     *function:isSetupRunning
     */
    private boolean isSetupRunning() {
        String ret = SystemProperties.get("init.svc.pppoe-setup", "");
        if (ret != null && ret.equals("stopped")) {
            return false;
        }

        return true;
    }

    /**
     *function:pppoe dialup
     */
    public boolean PppoeDialup() {
        if (mIsDialing) {
            Log.w(TAG, "you can NOT dial up again when dialing");
            return false;
        }

        mIsDialing = true;

        PppoeSetStatus(PPPOE_STA.CONNECTING);
        setPppoeStatus(PPPOE_STATE_CONNECTING, true);
        connect();

        new Thread() {
            public void run() {
                while (true) {
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        Log.e(TAG,
                              "InterruptedException: " + e.getMessage());
                    }

                    if (isPppoeRunning() == false) {
                        break;
                    }
                } /* end while */

                String currPppState = getPppoeStatus();
                if (currPppState.equals(PPPOE_STATE_AUTHFAILED)) {
                    setPppoeStatus(PPPOE_STATE_AUTHFAILED, true);
                } else if (false == ((PPPOE_STATE_CONNECT).equals(currPppState))) {
                    setPppoeStatus(PPPOE_STATE_DISCONNECT, true);
                }

                updatePppoeStatus();
                mIsDialing = false;

            } /* end run */
        }.start();
        Log.i(TAG,"PppoeDialup");
        return true;
    }

    public void PppoeHangUp() {
        stop();
        PppoeSetStatus(PPPOE_STA.DISCONNECTED);
        setPppoeStatus(PPPOE_STATE_DISCONNECT, true);
        Log.i(TAG,"PppoeHangUp");
    }

    public void PppoeMonitor() {
        // do nothing
    }


    public PPPOE_STA PppoeGetStatus() {
        if (gpppoe_sta != PPPOE_STA.CONNECTING) {
            updatePppoeStatus();
        }
        return gpppoe_sta;
    }

    public String PppoeGetUser() {
        return readSettingInfoFromFile(USER_NAME);
    }

    public String PppoeGetPW() {
        return readSettingInfoFromFile(PASS_WORD);
    }

    public String PppoeGetInterface() {
        return readSettingInfoFromFile(NET_INTERFACE);
    }

    private void connect() {
        SystemProperties.set("ctl.start", "pppoe-start");
    }

    private void stop() {
        SystemProperties.set("ctl.start", "pppoe-stop");
    }

    private void PppoeSetStatus(PPPOE_STA status) {
        gpppoe_sta = status;
    }

    private void updatePppoeStatus() {
        if (getPppoeStatus().equals(PPPOE_STATE_CONNECT)) {
            PppoeSetStatus(PPPOE_STA.CONNECTED);
        } else {
            PppoeSetStatus(PPPOE_STA.DISCONNECTED);
        }
    }

    private void init() {
        PppoeGetInfo();
        updatePppoeStatus();
    }

    private void PppoeGetInfo() {
        user = PppoeGetUser();
        passwd = PppoeGetPW();
        netif = PppoeGetInterface();
    }

    public boolean isPppoeRunning() {
        String ret = SystemProperties.get("init.svc.pppoe-start", "");
        if (ret != null && ret.equals("stopped")) {
            return false;
        }

        return true;
    }

    /**
     *function:read pppoe setting info from file
     */
    private String readSettingInfoFromFile(String what) {
        String result = null;
        if (null == what) {
            return result;
        }

        String pppInfoFilePath = "/data/misc/ppp/pppoe.data";
        File pppInfoFile = new File(pppInfoFilePath);
        if (pppInfoFile.exists() == false) {
            return null;
        }

        BufferedReader br = null;
        String s = null;
        try {
            br = new BufferedReader(new FileReader(pppInfoFile));
            while ((s = br.readLine()) != null) {
                String[] pppInfo = s.split("=");
                if (pppInfo != null && pppInfo.length == 2) {
                    if (what.equals(pppInfo[0])) {
                        result = pppInfo[1];
                        break;
                    }
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (br != null) {
                try {
                    br.close();
                    br = null;
                } catch (IOException e) {
                    Log.e(TAG, "failure to close BufferedReader");
                }
            }
        }


        return result;
    }


    /**
 *function:read ppp info from file
 */
    private String readPppInfoFromFile(String what) {
        String result = null;
        if (null == what) {
            return result;
        }

        int readLineNum = -1;
        if ("ifname".equals(what)) {
            readLineNum = 1;
        } else if ("ip".equals(what)) {
            readLineNum = 2;
        } else if ("route".equals(what)) {
            readLineNum = 3;
        } else if ("mask".equals(what)) {
            readLineNum = 4;
        } else if ("dns1".equals(what)) {
            readLineNum = 5;
        } else if ("dns2".equals(what)) {
            readLineNum = 6;
        }

        if (readLineNum != -1) {
            String pppInfoFilePath = "/data/misc/ppp/ipaddr";
            File pppInfoFile = new File(pppInfoFilePath);
            if (pppInfoFile == null || false == pppInfoFile.exists()) {
                return result;
            }
            BufferedReader br = null;
            String s = null;
            try {
                br = new BufferedReader(new FileReader(pppInfoFile));
                int currReadLineNum = 0;
                while ((s = br.readLine()) != null) {
                    currReadLineNum++;
                    if (currReadLineNum == readLineNum) {
                        result = s;
                        break;
                    }
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                if (br != null) {
                    try {
                        br.close();
                        br = null;
                    } catch (IOException e) {
                        Log.e(TAG, "failure to close BufferedReader");
                    }
                }
            }
        }

        return result;
    }

}
