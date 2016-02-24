/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.browser;

import android.app.Application;
import android.util.Log;
import android.webkit.CookieSyncManager;

// -- MTK_WEBKIT_FIX START --
import com.android.browser.mtkblacklist.MtkBlacklist;
// -- MTK_WEBKIT_FIX END --

public class Browser extends Application { 

    private final static String LOGTAG = "browser";
    
    // Set to true to enable verbose logging.
    final static boolean LOGV_ENABLED = false;

    // Set to true to enable extra debug logging.
    final static boolean LOGD_ENABLED = true;

    @Override
    public void onCreate() {
        super.onCreate();

        if (LOGV_ENABLED)
            Log.v(LOGTAG, "Browser.onCreate: this=" + this);

        // create CookieSyncManager with current Context
        CookieSyncManager.createInstance(this);
        BrowserSettings.initialize(getApplicationContext());
        Preloader.initialize(getApplicationContext());

        // -- MTK_WEBKIT_FIX START --
        MtkBlacklist.initialize(getApplicationContext());
        // -- MTK_WEBKIT_FIX END --
    }

}

