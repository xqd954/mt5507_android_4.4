/*
 * Copyright (C) 2008 The Android Open Source Project
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

package com.tcl.devicemanager;

import android.content.Context;
import android.content.Intent;
import android.os.UEventObserver;
import android.util.Slog;

/**
 * <p>MouseObserver monitors for a USB Webcam
 */
public class MouseObserver extends UEventObserver {
    private static final String TAG ="UEventObserver";
    private static final boolean LOG = true;
    public static final String ACTION_MOUSE_PLUG_IN = "android.tcl.uevent.mouse.plug.in";
    public static final String ACTION_MOUSE_PLUG_OUT = "android.tcl.uevent.mouse.plug.out";
    private static final String MOUSE_UEVENT_MATCH = "/mouse";
		private boolean mSystemReady;
    private final Context mContext;

    public MouseObserver(Context context) {
        mContext = context;
        startObserving(MOUSE_UEVENT_MATCH);

    }

@Override    
    public void onUEvent(UEventObserver.UEvent event) {
        if (LOG) Slog.i(TAG, "################### mouse UEVENT: " + event.toString());

        synchronized (this) {
            try {
                if (mSystemReady) {
             Slog.i(TAG, "################### mouse UEVENT: " + event.toString());
             Slog.i(TAG, "################### mouse ACTION: " + event.get("ACTION"));
         
             if(event.get("ACTION").equals("add")){
                 Intent intent = new Intent(ACTION_MOUSE_PLUG_IN);
		 String devName = event.get("DEVNAME");
                 intent.putExtra("devName", devName);
                 mContext.sendBroadcast(intent);
                 Slog.i(TAG, "################### ACTION_MOUSE_PLUG_IN" );
             }
             else{ 
                Intent intent = new Intent(ACTION_MOUSE_PLUG_OUT);
		String devName = event.get("DEVNAME");
                intent.putExtra("devName", devName);
                mContext.sendBroadcast(intent);
                Slog.i(TAG, "################### ACTION_MOUSE_PLUG_OUT" );
             }
          
        } 
        }catch (NumberFormatException e) {
            Slog.e(TAG, "Could not parse switch state from event " + event);
        }
    }
  }
    
    public void systemReady() {
        synchronized (this) {
            mSystemReady = true;
        }
      }

}
