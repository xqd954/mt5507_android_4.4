package com.mediatek.ui.mmp.util;

import java.lang.ref.SoftReference;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Enumeration;

import android.graphics.Bitmap;
import android.util.Log;

public final class BitmapCache {
	private static final String TAG = "BitmapCache";
	private static BitmapCache mInstance;
	private ConcurrentHashMap<String, SoftReference<Bitmap>> mCache = new ConcurrentHashMap<String, SoftReference<Bitmap>>();

	private ConcurrentHashMap<String, SoftReference<DecodeInfo>> mInfoCache = 
			new ConcurrentHashMap<String, SoftReference<DecodeInfo>>();
	
	public  class DecodeInfo{
		private Bitmap mBitmap;
		private boolean mIsDecodeFailed;
		
		public DecodeInfo(final Bitmap bitmap, final boolean isDecodeFailed){
			mBitmap = bitmap;
			mIsDecodeFailed = isDecodeFailed;
		}
		
		public Bitmap getBitmap(){
			return mBitmap;
		}
		
		public boolean getDecodeFailed(){
			return mIsDecodeFailed;
		}
		
	}
	
	private BitmapCache() {
	}

	public static BitmapCache createCache(boolean clear) {
		if (mInstance == null) {
			mInstance = new BitmapCache();
		} else {
			if (clear) {
				mInstance.clear();
			}
		}
		return mInstance;
	}

	/**
	 * @deprecated
	 * @param key
	 * @return
	 */
	public Bitmap get(String key) {
		synchronized (mCache) {
			SoftReference<Bitmap> ref = mCache.get(key);
			Bitmap target = null;
			if (ref != null) {
				target = ref.get();
			}
			return target;
		}
	}

	/**
	 * @deprecated
	 * @param key
	 * @param bitmap
	 */
	public void put(String key, Bitmap bitmap) {
		synchronized (mCache) {
			mCache.put(key, new SoftReference<Bitmap>(bitmap));
		}
	}

	public DecodeInfo getDecodeInfo (String key){
		synchronized (mInfoCache) {
			SoftReference<DecodeInfo> ref = mInfoCache.get(key);
			DecodeInfo target = null;
			if (ref != null) {
				target = ref.get();
			}
			return target;
		}
		
		
	}
	
	public void putDecodeInfo(String key, DecodeInfo info){
		synchronized (mInfoCache) {
			mInfoCache.put(key, new SoftReference<DecodeInfo>(info));
		}
}
	public void clear() {
		synchronized (mCache) {
			Log.d(TAG,"thumbnail cache cleanr~~");
			Enumeration<String> keys = mCache.keys();
			int size = mCache.size();
			for (int index = 0; index < size; index++) {
				String key = keys.nextElement();
				SoftReference<Bitmap> ref = mCache.remove(key);
				Bitmap bmp = null;
				if (ref != null) {
					bmp = ref.get();
				}
				
				if (bmp != null ) {
					//bmp.recycle();
					bmp = null;

				}
			}
			mCache.clear();
		}
		mInfoCache.clear();
	}
	public ConcurrentHashMap<String, SoftReference<Bitmap>> getCache()
	{
		return mCache;
	}
	public ConcurrentHashMap<String, SoftReference<DecodeInfo>> getDecodeInfoCache()
	{
		return mInfoCache;
	}
}
