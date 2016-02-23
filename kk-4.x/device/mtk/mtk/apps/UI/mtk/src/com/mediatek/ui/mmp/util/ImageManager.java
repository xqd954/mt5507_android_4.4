package com.mediatek.ui.mmp.util;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.mediatek.mmpcm.photoimpl.PhotoUtil;
import com.mediatek.ui.mmp.util.AsyncLoader.LoadWork;

public class ImageManager {

	public interface ImageLoad {
		void imageLoad(PhotoUtil bitmap);
	}

	private static final int MSG_LOAD_IMAGE = 1;


	private static ImageManager mInstance;

	ImageLoad mImageLoad;

	private AsyncLoader<PhotoUtil> mLoader;

	private LogicManager mLogicManager;

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_LOAD_IMAGE: {
				if (mImageLoad == null){
					break;
				}
				if (msg.obj == null) {
					mImageLoad.imageLoad(null);
				} else {
					mImageLoad.imageLoad((PhotoUtil) (msg.obj));
				}
				break;

			}
			default:
				break;
			}

		}

	};

	private class LoadBitmap implements LoadWork<PhotoUtil> {

		private int mType;

		public LoadBitmap(int type) {
			mType = type;
		}

		public PhotoUtil load() {
			if (mLogicManager == null){
				return null;
			}

			return mLogicManager.loadImageBitmap(mType);

			
		}

		public void loaded(PhotoUtil result) {
			Message msg = mHandler.obtainMessage(MSG_LOAD_IMAGE);
			msg.obj = result;
			mHandler.sendMessage(msg);
		}

	}

	public static ImageManager getInstance() {

		if (mInstance == null) {
			mInstance = new ImageManager();
		}

		return mInstance;
	}

	public void setImageLoad(ImageLoad img,LogicManager manager) {
		mImageLoad = img;
		mLogicManager=manager;
	}

	private ImageManager() {
		//mLoader = AsyncLoader.getInstance(1);
		
		mLoader = new AsyncLoader<PhotoUtil>("DecodeBitmapThread");
	}

	public void load(int type){
		mLoader.clearQueue();
		mLoader.addWork(new LoadBitmap(type));

	}

	public void finish() {
		mLoader.clearQueue();

	}
}
