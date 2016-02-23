/**
 * Copyright (c) 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.net.wifi.p2p;

import android.os.Messenger;
import android.net.wifi.p2p.WifiP2pGroup;

/**
 * Interface that WifiP2pService implements
 *
 * {@hide}
 */
interface IWifiP2pManager
{
    Messenger getMessenger();
    void setMiracastMode(int mode);
    void setMiracastEnable(boolean enable);
    void ConfigGroupInfo(String mssid,String passphase);
    String getMtkPeerIpAddress(String mac,in WifiP2pGroup mWifiP2pGroup);
    boolean mtkP2pGroupAdd();
    void setMtkP2pListen();
    void setMtkWfdSupport(boolean iswfd);
    boolean getMtkWfdSupport();
    void p2pGroupAdd(int netid);
    void reStartGO(boolean restart);
}

