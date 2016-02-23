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



package com.android.server;

import android.net.pppoe.IPppoeManager;
import android.net.pppoe.PppoeManager;
//import com.tcl.net.pppoe.IPppoeManager;
//import com.tcl.net.pppoe.PppoeManager;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import java.io.InputStream;
import java.io.DataInputStream;
import java.io.IOException;
import android.os.SystemProperties;

public class PppoeService extends IPppoeManager.Stub {
    private static final String TAG = "PppoeService";

    private static final int LINK_UP = 30;
    private static final int LINK_DOWN = 31;
    private static final int LINK_AUTH_FAIL = 32;
    private static final int LINK_TIME_OUT = 33;
    private static final int LINK_PPP_FAIL = 34;
    private static final int LINK_DISCONNECTING = 35;
    private static final int LINK_CONNECTING = 36;

    private Context mContext = null;
    private String mPppoeState = null;
    private Thread mSocketThread = null;

    /**
     *function:PppoeService constructor
     */
    public PppoeService(Context context) {
        mContext = context;
        mPppoeState = "disconnect";
        startMonitorThread();
    }

    /**
     *function:set pppoe status
     */
    public void setPppoeStatus(String status, boolean sendBroadcast) {
        mPppoeState = status;
        if (sendBroadcast) {
            sendPppBroadcast(mPppoeState);
            // Kill all pppoe processes
            /*if (status != null && status.equals(PppoeManager.PPPOE_STATE_DISCONNECT)) {
                SystemProperties.set("ctl.start", "pppoe-stop");
            }*/
        }
    }

    /**
     *function:get pppoe status
     */
    public String getPppoeStatus() {
        return mPppoeState;
    }

    /**
     *function:send ppp status
     */
    private void sendPppBroadcast(String pppState) {
        Intent intent = new Intent();
        intent.setAction("android.net.pppoe.PPPOE_STATE_ACTION");
        intent.putExtra("PppoeStatus", pppState);
        mContext.sendBroadcast(intent);
    }

    private void startMonitorThread() {
        mSocketThread = new Thread("pppoe_monitor_thread") {
            public void run() {
                try {
                    LocalServerSocket server2 = new LocalServerSocket("pppoe.localsocket");
                    LocalSocket receiver2 = null;
                    InputStream in = null;
                    DataInputStream din = null;
                    while (true) {
                        if (receiver2 != null) {
                            receiver2.close();
                            receiver2 = null;
                        }
                        if (in != null) {
                            in.close();
                            in = null;
                        }
                        if (din != null) {
                            din.close();
                            din = null;
                        }

                        receiver2 = server2.accept();
                        if (receiver2 == null) {
                            Log.e(TAG, "Can not accept socket");
                            continue;
                        }
                        in = receiver2.getInputStream();
                        din = new DataInputStream(in);
                        if (receiver2 != null) {
                            String event = din.readLine();
                            handleEvent(event);
                            Log.i(TAG, "socket event = " + event);
                        }
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };
        mSocketThread.start();
    }

    /**
     *function:handleEvent
     */
    private void handleEvent(String event) {
        if (null == event) {
            return;
        }

        String [] eventInfo = event.split(":");
        if (eventInfo.length != 2) {
            
            Log.e(TAG, "pppoe event error");
            return;
        }

        int eventCommand = Integer.parseInt(eventInfo[1]);
        int message = PppoeManager.MSG_PPPOE_DISCONNECT;
        switch (eventCommand) {
            case LINK_UP:
                message = PppoeManager.MSG_PPPOE_CONNECT;
                break;
            case LINK_DOWN:
                message = PppoeManager.MSG_PPPOE_DISCONNECT;
                break;
            case LINK_AUTH_FAIL:
                message = PppoeManager.MSG_PPPOE_AUTH_FAILED;
                break;
        }

        String pppStatus = getStateFromMsg(message);
        if (false == (pppStatus.equals(PppoeManager.PPPOE_STATE_AUTHFAILED))) {
            setPppoeStatus(pppStatus, true);
        } else {
            setPppoeStatus(pppStatus, false);
        }
    }



    /**
     *fuction:getStateFromMsg
     */
    private String getStateFromMsg(int msg) {
        String pppState = PppoeManager.PPPOE_STATE_DISCONNECT;
        switch (msg) {
            case PppoeManager.MSG_PPPOE_CONNECT:
                pppState = PppoeManager.PPPOE_STATE_CONNECT;
                break;
            case PppoeManager.MSG_PPPOE_AUTH_FAILED:
                pppState = PppoeManager.PPPOE_STATE_AUTHFAILED;
                break;
            case PppoeManager.MSG_PPPOE_DISCONNECTING:
                pppState = PppoeManager.PPPOE_STATE_DISCONNECTING;
                break;
            case PppoeManager.MSG_PPPOE_CONNECTING:
                pppState = PppoeManager.PPPOE_STATE_CONNECTING;
                break;
            case PppoeManager.MSG_PPPOE_TIME_OUT:
                pppState = PppoeManager.PPPOE_STATE_LINKTIMEOUT;
                break;
            case PppoeManager.MSG_PPPOE_FAILED:
                pppState = PppoeManager.PPPOE_STATE_FAILED;
                break;
        }
        return pppState;
    }


}
