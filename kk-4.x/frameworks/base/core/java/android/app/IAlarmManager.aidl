/* //device/java/android/android/app/IAlarmManager.aidl
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
*/
package android.app;

import android.app.PendingIntent;
import android.os.WorkSource;

/**
 * System private API for talking with the alarm manager service.
 *
 * {@hide}
 */
interface IAlarmManager {
	/** windowLength == 0 means exact; windowLength < 0 means the let the OS decide */
    void set(int type, long triggerAtTime, long windowLength,
            long interval, in PendingIntent operation, in WorkSource workSource);
    void setTime(long millis);
    void setTimeZone(String zone);
    void remove(in PendingIntent operation);
    
    //{{gaodw@tcl.com.  patch from ms628.
	//add by cuix@tcl.com 2014.1.8
	void setPoweroffTimer(long rel);
	void cancelPoweroffTimer();
	long getReminderRelativeTime();
	long getPowerOffTimer();
	//end by cuix@tcl.com 2014.1.8


        /* Patch Begin by Sarah Zhang(zxia@tcl.com) */
        /* ScreenSaver API */
        void setScreenoffTimer(long rel);
        void startScreenoffTimer();
        void stopScreenoffTimer();
        long getReminderRelativeScreenTime();
        long getScreenOffTimer();
        void EventResetScreenoffTimer();
        void ReportIfStartScreenSaver(boolean flag);
        /* Patch End by Sarah Zhang(zxia@tcl.com) */

    //}}
}


