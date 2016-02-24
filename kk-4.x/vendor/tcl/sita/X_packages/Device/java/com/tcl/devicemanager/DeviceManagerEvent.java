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


import android.app.ActivityManagerNative;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.UEventObserver;
import android.util.Slog;
import android.media.AudioManager;
import java.io.File; 
import java.io.FileReader;
import java.io.FileNotFoundException;


public class DeviceManagerEvent  {
    public class Intent{
        public static final String ACTION_MICROPHONE_PLUG_IN = "android.intent.action.MICROPHONE_PLUG_IN";
        public static final String ACTION_MICROPHONE_PLUG_OUT = "android.intent.action.MICROPHONE_PLUG_OUT";
        public static final String ACTION_CAMERA_PLUG_IN = "android.intent.action.CAMERA_PLUG_IN";
        public static final String ACTION_CAMERA_PLUG_OUT = "android.intent.action.CAMERA_PLUG_OUT";
		public static final String ACTION_DEVICE_PLUG_OUT = "com.tcl.intent.action.DEVICE_PLUG_OUT";
		public static final String ACTION_DEVICE_PLUG_IN = "com.tcl.intent.action.DEVICE_PLUG_IN";
        
    }
	// Device Type
	public static final String DEVICE_TYPE_CAMERA = "camera";
	public static final String DEVICE_TYPE_MIC = "mic";
	//设备名称(设备节点)，设备类型，该类设备当前还剩余数目
	public static final String DEVICE_EVENT_KEY_NAME = "name"; // Device name
	public static final String DEVICE_EVENT_KEY_TYPE = "type"; // device type, eg: camera , mic
	public static final String DEVICE_EVENT_KEY_SUBSYSTEM = "subsystem"; // device sub system , eg: video4linux , audio
	public static final String DEVICE_EVENT_KEY_COUNT = "count"; //


}
