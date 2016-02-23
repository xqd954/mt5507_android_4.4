package com.mediatek.timeshift_pvr.manager;


import com.mediatek.ui.util.MtkLog;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;


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
        MtkLog.d(TAG,"save value: " + name + "--- " + value);
    }
    public void saveValue(String name, float value) {
    	mSharedPreferences.edit().putFloat(name, value).commit();
    	MtkLog.d(TAG,"save value: " + name + "--- " + value);
    }
    public void saveStrValue(String name, String value) {
        mSharedPreferences.edit().putString(name, value).commit();
    }
    public void saveBooleanValue(String name, boolean value){
    	mSharedPreferences.edit().putBoolean(name, value).commit();
    }

	public int readValue(String id) {
		int value = 0;

            value = mSharedPreferences.getInt(id, 0);    
        MtkLog.d(TAG,"read value: " + id + "--- " + value);
        return value;
    }
	public float readFloatValue(String id) {
		float value = 0f;
		
			value = mSharedPreferences.getFloat(id, 0f);    
		//MtkLog.d(TAG,"read value: " + id + "--- " + value);
		return value;
	}
    public String readStrValue(String id){
    	String value = null;
    	if(id.equals("password")){
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
