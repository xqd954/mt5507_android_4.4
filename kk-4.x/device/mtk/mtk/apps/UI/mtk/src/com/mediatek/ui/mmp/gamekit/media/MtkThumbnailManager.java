package com.mediatek.ui.mmp.gamekit.media;

import java.io.File;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import com.mediatek.mmpcm.MmpTool;
import com.mediatek.ui.mmp.gamekit.filebrowse.MtkFileFlimBrowseActivity.Listener;
import com.mediatek.ui.mmp.gamekit.filebrowse.MtkFileFlimBrowseActivity.LoadBitmapListener;
import com.mediatek.ui.mmp.gamekit.util.AsyncLoader;
import com.mediatek.ui.mmp.gamekit.util.BitmapCache;
import com.mediatek.ui.mmp.gamekit.util.AsyncLoader.LoadWork;
import com.mediatek.ui.mmp.model.FileAdapter;
import com.mediatek.ui.mmp.model.LocalFileAdapter;
import com.mediatek.ui.util.MtkLog;

/**
 * 
 * Thumbnail generation routines for media provider.
 */
public class MtkThumbnailManager {
	private static final String TAG = "ThumbnailUtils";
	/**
	 * Constant used to indicate the dimension of mini thumbnail.
	 * 
	 * @hide Only used by media framework and media provider internally.
	 */
	public static final int TARGET_SIZE_MINI_THUMBNAIL = 350;

	// -------------------------------------------------------useful-------------------------------------------------------------
	private static MtkThumbnailManager mInstance;
	private BitmapCache mCache = BitmapCache.createCache(false);
	protected AsyncLoader<String, Bitmap> mLoaderTexture = AsyncLoader
			.getInstance();
	
	public MtkThumbnailManager() {

	}

	public static MtkThumbnailManager getInstance() {
		if (mInstance == null) {
			mInstance = new MtkThumbnailManager();
		}

		return mInstance;
	}



	private static BitmapFactory.Options opt = new BitmapFactory.Options();

	private boolean _invalid(String path) {
		if (null == path) {
			return false;
		}

		int width;
		int height;
		
		opt.mCancel = false;

		opt.inJustDecodeBounds = true;
		opt.inSampleSize = 1;
		BitmapFactory.decodeFile(path, opt);
		width = opt.outWidth;
		height = opt.outHeight;

		
		// for Static gif files
		if (path.toLowerCase().endsWith("bmp")) {
			if ((width * height) > (4896 * 4096)) {
				return false;
			}
		}
		
		return true;
	}
	

	public Bitmap decodeThumbnail(FileAdapter fAdapater) {
		Bitmap dest = null;
		if (fAdapater != null) {
			if (!_invalid(fAdapater.getPath())){
				return null;
			}
			dest = fAdapater.getThumbnail(TARGET_SIZE_MINI_THUMBNAIL,
					TARGET_SIZE_MINI_THUMBNAIL,true);
			if (dest != null) {
				mCache.put(fAdapater.getPath(), dest);
			}
		}
		return dest;
	}

	public Bitmap getThumbnail(FileAdapter fAdapater) {
		Bitmap dest = null;
		if (fAdapater != null) {
			dest = getThumbnail(fAdapater.getPath());
		}

		return dest;
	}

	public Bitmap getThumbnail(String path) {
		Bitmap dest = null;
		if (path != null) {
			dest = mCache.get(path);
		}

		return dest;
	}

	public Bitmap getThumbnail(FileAdapter fAdapter,
			LoadBitmapListener loadBitmapL) {
		Bitmap dest = null;
		dest = getThumbnail(fAdapter);
		if (dest == null) {
			if (fAdapter != null){
				MtkLog.i(TAG, "add work = " + fAdapter.getPath());
			}

			LoadBitmap work = new LoadBitmap(fAdapter, loadBitmapL);
			mLoaderTexture.addWork(work);
		}

		return dest;
	}

	public void destroyThumbnail() {
		mCache.clear();
		mLoaderTexture.clearQueue();
	}

	private class LoadBitmap implements LoadWork<Bitmap> {
		private String mPath;
		private Bitmap mResult;
		private Listener mListener;
		private FileAdapter mfAdapter;

		public LoadBitmap(FileAdapter f, Listener l) {
			mfAdapter = f;
			mListener = l;
			mPath = mfAdapter.getPath();
		}

		/**
		 * @deprecated
		 * @param path
		 * @param l
		 */
		public LoadBitmap(String path, Listener l) {
			mListener = l;
			mPath = path;
		}

		@Override
		public Bitmap load() {
			Bitmap bitmap = null;

			try {
				/*
				 * bitmap = MtkThumbnailUtils.createThumbnailFromFile(FileUtil
				 * .getTypeByName(mPath), mPath,
				 * MtkThumbnailUtils.TARGET_SIZE_MINI_THUMBNAIL);
				 */
				/*
				 * bitmap = mfAdapter.getThumbnail(MtkThumbnailUtils.
				 * TARGET_SIZE_MINI_THUMBNAIL,
				 * MtkThumbnailUtils.TARGET_SIZE_MINI_THUMBNAIL);
				 */
				bitmap = decodeThumbnail(mfAdapter);

				MtkLog.d(TAG, "load()" + bitmap);
			} catch (OutOfMemoryError e) {

			}

			mResult = bitmap;

			return mResult;
		}

		@Override
		public void loaded(Bitmap result) {
			MtkLog.d(TAG, "loaded path =" + mPath + "bm = " + result);

			if (mListener != null && result != null)
				mListener.onEvent();
		}
	}

}
