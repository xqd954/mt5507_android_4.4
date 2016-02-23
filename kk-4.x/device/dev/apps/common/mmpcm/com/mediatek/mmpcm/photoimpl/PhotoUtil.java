package com.mediatek.mmpcm.photoimpl;

import android.graphics.Bitmap;
import android.util.Log;

public class PhotoUtil {
	
	private Bitmap[] mBitmaps;
	private int[] mDelayTime;
	
	
	
	public PhotoUtil(Bitmap[] mBitmaps, int[] mDelayTime) {
		super();
		this.mBitmaps = mBitmaps;
		this.mDelayTime = mDelayTime;
	}
	
	
	public PhotoUtil() {
		super();
		// TODO Auto-generated constructor stub
	}
    

	public PhotoUtil(Bitmap[] mBitmaps) {
		super();
		this.mBitmaps = mBitmaps;
	}


	public Bitmap[] getmBitmaps() {
		return mBitmaps;
	}
	public void setmBitmaps(Bitmap[] mBitmaps) {
		this.mBitmaps = mBitmaps;
	}
	public int[] getmDelayTime() {
	
		return mDelayTime;
	}
	public void setmDelayTime(int[] mDelayTime) {
		
		this.mDelayTime = mDelayTime;
		
	}
	
	

}
