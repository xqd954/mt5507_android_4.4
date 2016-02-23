package com.mediatek.mmpcm;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

public class CommonStorage {

	private SharedPreferences table = null;
	private static CommonStorage mmpComs = null;

	private CommonStorage(Context context) {
		table = context.getSharedPreferences(fileName, Context.MODE_PRIVATE);
	}

	public static CommonStorage getInstance(Context context) {
		if (mmpComs == null) {
			synchronized (CommonSet.class) {
				if (mmpComs == null) {
					mmpComs = new CommonStorage(context);
				}
			}
		}
		return mmpComs;

	}

	/**
	 * @return the instance of share preferences.
	 */
	public SharedPreferences getSharedPreferences() {
		return table;
	}

	private Editor editor = null;

	/**
	 * @return the editor of share preference.
	 */
	public Editor getEditor() {
		if (editor == null) {
			editor = table.edit();
		}
		return editor;
	}

	public void set(String k, String v) {
		getEditor().putString(k, v);
		getEditor().commit();
	}


	public void set(String k, int v) {
		getEditor().putInt(k, v);
		getEditor().commit();
	}

	public void set(String k, float v) {
		getEditor().putFloat(k, v);
		getEditor().commit();
	}

	/**
	 * @return the value of Key k
	 * @param k
	 *            the key to identify value
	 */
	public String get(String k) {
		return table.getString(k, null);
	}

	/**
	 * @return the value of key k
	 * @param k
	 *            the key to identify value
	 * @param defVal
	 *            if the Key k do not exit in share preference, return the
	 *            default value.
	 */
	public String get(String k, String defVal) {
		// String v = table.get(k);
		return table.getString(k, defVal);
	}


	/**
	 * @return the value of key k
	 * @param k
	 *            the key to identify value
	 * @param defVal
	 *            if the Key k do not exit in share preference, return the
	 *            default value.
	 */
	public int get(String k, int defVal) {
		// String v = table.get(k);
		return table.getInt(k, defVal);
	}

	/**
	 * @return the value of key k
	 * @param k
	 *            the key to identify value
	 * @param defVal
	 *            if the Key k do not exit in share preference, return the
	 *            default value.
	 */
	public float get(String k, float defVal) {
		// String v = table.get(k);
		return table.getFloat(k, defVal);
	}

	String fileName = "tvcm_storage_table";

	public void clean() {
		getEditor().clear();
		getEditor().commit();
	}
}
