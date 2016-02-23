package com.mediatek.ui.menu.util;

import java.util.TimeZone;

import android.content.Context;

import com.mediatek.ui.R;
import com.mediatek.ui.util.MtkLog;

public class GetTimeZone {
	private TimeZone  tz;
	private String[] zoneIdNames;
	private static GetTimeZone getTimeZone;
	private Context mContext;
	
	public GetTimeZone(Context mContext){
		zoneIdNames = mContext.getResources().getStringArray(
				R.array.menu_setup_timezone_name_array);
		tz = java.util.Calendar.getInstance().getTimeZone();
	}
	
	public static GetTimeZone getInstance(Context mContext){
			getTimeZone = new GetTimeZone(mContext);
		return getTimeZone;
	}
	
	public int getZoneId(){
		int zoneNameId = 0;
		String timeZone = tz.getID();
		if (SaveValue.getInstance(mContext).readBooleanValue("Zone_time") == true) {
			zoneNameId = 25;
		} else {

			for (int i = 0; i < zoneIdNames.length; i++) {

				if (timeZone.equals(zoneIdNames[i])
						&& !zoneIdNames[i].equals("As Broadcast")) {

					zoneNameId = i;
					break;
				}
			}
		}

		return zoneNameId;
	}

}
