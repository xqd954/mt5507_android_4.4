package com.mediatek.mmpcm.threedimen.photoimpl;

import android.content.Context;

public class PhotoManager {
	
	private static PhotoManager mPhotoManager;
	private MPlayback mPlayback;
	
	private PhotoManager(){
		
	}
	
	public static PhotoManager getInstance(Context context){
        if(mPhotoManager == null){
            synchronized(PhotoManager.class){
                if(mPhotoManager == null){
                	mPhotoManager = new PhotoManager(context);
                }               
            }
        }
        return mPhotoManager;
	}
	
    public static PhotoManager getInstance(){
        return mPhotoManager;
    }
    
	private PhotoManager(Context context){
		if (mPlayback == null){
			mPlayback = new MPlayback();
		}
	}
	
	public void onRelease() {
		mPlayback.onRelease();
		mPlayback = null;
		mPhotoManager = null;
	}

	public MPlayback getPlayback() {
		return mPlayback;
	}
}
