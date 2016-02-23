package com.mediatek.ui.menu.util;


import com.mediatek.tvcm.TVContent;
import com.mediatek.ui.util.MtkLog;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.util.Log;


public class SaveValue {
	 private static  Object syncRoot = new Object();
    private static final String TAG = "SaveValue";
    private SharedPreferences mSharedPreferences;
    Context mContext;

    public static SaveValue save_data;

    private SaveValue(Context context) {
        mContext = context;
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(mContext);
    }

    public static synchronized SaveValue getInstance(Context context) {
		if (save_data == null) {
			save_data = new SaveValue(context);
		}
	     return save_data;
	
    }

    public void saveValue(String name, int value) {
        mSharedPreferences.edit().putInt(name, value).commit();
        TVContent.getInstance(mContext).flushMedia();
        MtkLog.d(TAG,"save value: " + name + "--- " + value);
    }
    public void saveValue(String name, float value) {
    	mSharedPreferences.edit().putFloat(name, value).commit();
    	TVContent.getInstance(mContext).flushMedia();
    	MtkLog.d(TAG,"save value: " + name + "--- " + value);
    }
    public void saveStrValue(String name, String value) {
        mSharedPreferences.edit().putString(name, value).commit();
        TVContent.getInstance(mContext).flushMedia();
    }
    public void saveBooleanValue(String name, boolean value){
    	mSharedPreferences.edit().putBoolean(name, value).commit();
    	TVContent.getInstance(mContext).flushMedia();
    }

	public int readValue(String id) {
		int value = 0;

		if (id.equals(MenuConfigManager.DPMS)
		|| id.equals(MenuConfigManager.AUTO_SYNC) || id.equals(MenuConfigManager.SETUP_WOW)) {
			value = mSharedPreferences.getInt(id, 1);
        } else if (id.equals(MenuConfigManager.CAPTURE_LOGO_SELECT)) {
            value = mSharedPreferences.getInt(id, 3);  
        }else {
            value = mSharedPreferences.getInt(id, 0);    
        }
        MtkLog.d(TAG,"read value: " + id + "--- " + value);
        return value;
	}
	
	public float readFloatValue(String id) {
		float value = 0f;
		
		if (id.equals(MenuConfigManager.DPMS)
				|| id.equals(MenuConfigManager.AUTO_SYNC)) {
			value = mSharedPreferences.getInt(id, 1);
		} else {
			value = mSharedPreferences.getFloat(id, 0f);    
		}
		//MtkLog.d(TAG,"read value: " + id + "--- " + value);
		return value;
	}
    public String readStrValue(String id){
    	String value = null;
    	if (id.equals(MenuConfigManager.TIMER1)||id.equals(MenuConfigManager.TIMER2)) {
    		value = mSharedPreferences.getString(id, "00:00:00");
		}else if(id.equals("password")){
			value = mSharedPreferences.getString(id, "1234");
		}else {
			value = mSharedPreferences.getString(id, "0");
		}
    	return value;
    }
    public boolean readBooleanValue(String id){
    	Boolean value = false;
    	value = mSharedPreferences.getBoolean(id, false);
    	return value;
    }

}
