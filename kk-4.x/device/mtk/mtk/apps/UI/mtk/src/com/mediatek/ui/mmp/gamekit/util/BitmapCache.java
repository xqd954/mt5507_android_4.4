package com.mediatek.ui.mmp.gamekit.util;

import java.lang.ref.Reference;
import java.lang.ref.SoftReference;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.concurrent.ConcurrentHashMap;

import com.mediatek.ui.util.MtkLog;

import android.graphics.Bitmap;
import android.util.Log;

public final class BitmapCache {
	public static int CACHE_MAX_SIZE = 2;
	private static final String TAG = "BitmapCache";
	private static BitmapCache mInstance;
	/*
	 * private ConcurrentHashMap<String, SoftReference<Bitmap>> mCache = new
	 * ConcurrentHashMap<String, SoftReference<Bitmap>>();
	 */

	private ConcurrentHashMap<String, Bitmap> mCache = new ConcurrentHashMap<String, Bitmap>();
	//private ArrayList<String> mLinkList = new ArrayList<String>();

	private BitmapCache() {
	}

	public static BitmapCache createCache(boolean clear) {
		if (mInstance == null) {
			mInstance = new BitmapCache();
		} else {
			if (clear) {
				mInstance.mCache.clear();
			}
		}
		return mInstance;
	}

	public Bitmap get(String key) {
		synchronized (mCache) {
			Bitmap target = null;

			//target = mCache.get(key);
			target = mCache.remove(key);
			if (target != null) {
			MtkLog.w(TAG, "Hit Cache!!");
			MtkLog.w(TAG, "get key = " + key + "bm=" + target);
			}
			return target;
		}
	}

	public void put(String key, Bitmap bitmap) {

		synchronized (mCache) {
			/*String removeKey = null;

			Enumeration<String> keys = mCache.keys();
			int size = mCache.size();
			if (size >= CACHE_MAX_SIZE) {
				for (int idx = 0; idx < size - 1; idx++) {
					removeKey = keys.nextElement();
					Bitmap bmp = mCache.remove(removeKey);
					if (bmp != null && !bmp.isRecycled()) {
						bmp.recycle();
						bmp = null;
					}
				}
			}*/

			mCache.put(key, bitmap);
		}
	}

	public void clear() {
		synchronized (mCache) {
			Enumeration<String> keys = mCache.keys();
			int size = mCache.size();
			for (int index = 0; index < size; index++) {
				String key = keys.nextElement();
				Bitmap bmp = mCache.remove(key);
				if (bmp != null && !bmp.isRecycled()) {
					bmp.recycle();
					bmp = null;

				}
			}

			mCache.clear();
		}

	}

	/*
	 * public ConcurrentHashMap<String, SoftReference<Bitmap>> getCache() {
	 * return mCache; }
	 */
}
