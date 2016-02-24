/*
 * Copyright (C) 2010 The Android Open Source Project
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
package com.android.browser.mtkblacklist;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.Resources.NotFoundException;
import android.util.Log;

import com.android.browser.R;

/**
 * Loads and holds data for a given web search engine.
 */
public class MtkBlacklist {

    private static String LOGTAG = "MtkBlacklist";

    private static String[] mBlacklistForDisablingPlugin;
    private static String[] mBlacklistForUsingDesktopUA;
    private static String[] mBlacklistForUsingAndroidUA;
    private static String[] mBlacklistForUsingIPadUA;

    public static void initialize(Context context) throws IllegalArgumentException {
        Resources res = context.getResources();

        mBlacklistForDisablingPlugin = res.getStringArray(R.array.mtk_blacklist_for_disabling_plugin);
        mBlacklistForUsingDesktopUA = res.getStringArray(R.array.mtk_blacklist_for_using_desktop_ua);
        mBlacklistForUsingAndroidUA = res.getStringArray(R.array.mtk_blacklist_for_using_android_ua);
        mBlacklistForUsingIPadUA = res.getStringArray(R.array.mtk_blacklist_for_using_iPad_ua);
    }

    public static boolean disablePlugin(String url) {
        for (int i = 0; i < mBlacklistForDisablingPlugin.length; i++) {
            // both convert to lower case first then do comparision
            if (url != null &&
                    url.toLowerCase().contains(mBlacklistForDisablingPlugin[i].toLowerCase())) {
                return true;
            }
        }

        return false;
    }

    public static boolean useDesktopUA(String url) {
        for (int i = 0; i < mBlacklistForUsingDesktopUA.length; i++) {
            // both convert to lower case first then do comparision
            if (url != null &&
                    url.toLowerCase().contains(mBlacklistForUsingDesktopUA[i].toLowerCase())) {
                return true;
            }
        }

        return false;
    }

    public static boolean useAndroidUA(String url) {
        for (int i = 0; i < mBlacklistForUsingAndroidUA.length; i++) {
            // both convert to lower case first then do comparision
            if (url != null &&
                    url.toLowerCase().contains(mBlacklistForUsingAndroidUA[i].toLowerCase())) {
                return true;
            }
        }

        return false;
    }

    public static boolean useIPadUA(String url) {
        for (int i = 0; i < mBlacklistForUsingIPadUA.length; i++) {
            // both convert to lower case first then do comparision
            if (url != null &&
                    url.toLowerCase().contains(mBlacklistForUsingIPadUA[i].toLowerCase())) {
                return true;
            }
        }

        return false;
    }
}
