package com.mediatek.tvcommon;

import java.io.IOException;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

class TVStorage {

	private static final String TVCM_TABLE = "tvcm_storage_table";
	final private SharedPreferences pref;
	final private Editor editor;

	private TVStorage(Context context) {
		pref = context.getSharedPreferences(TVCM_TABLE, Context.MODE_PRIVATE);
		editor = pref.edit();
	}

	private static TVStorage storage;

	static TVStorage getInstance(Context context) {
		if (storage == null) {
			storage = new TVStorage(context);
		}
		return storage;
	}

	public String getString(String key, String def) {
		return pref.getString(key, def);
	}

	public void setString(String key, String val) {
		editor.putString(key, val);
		editor.commit();
		sync();
	}

	public boolean getBoolean(String key, boolean def) {
		return pref.getBoolean(key, def);
	}

	public void setBoolean(String key, boolean val) {
		editor.putBoolean(key, val);
		editor.commit();
		sync();
	}

	public int getInt(String key, int def) {
		return pref.getInt(key, def);
	}

	public void setInt(String key, int val) {
		editor.putInt(key, val);
		editor.commit();
		sync();
	}
	
	public void sync() {
		try {
			Runtime.getRuntime().exec("sync");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
