package com.mediatek.ui.mmp.gamekit.media;

import android.graphics.Bitmap;

import com.mediatek.gamekit.GKTexture;
import com.mediatek.gamekit.GKView;
import com.mediatek.ui.mmp.model.FileAdapter;

public class MtkGLES2TextureInfo  {

	//was mmp used. not common.
	private FileAdapter mFAdapter;
	private String mPath;
	private GKTexture mTextureInfo;
	private Bitmap bm;

	public MtkGLES2TextureInfo(GKTexture info){
		mTextureInfo = info;
	}
	
	public void setFileAdapter(FileAdapter fa){
		mFAdapter = fa;
	}
	
	public FileAdapter getFileAdapter(){
		return mFAdapter;
	}
	
	public void setPath(String path){
		mPath = path;
	}
		
	public String getPath(){
		return mPath;
	}
	
	public int getWidth(){
		if (mTextureInfo == null) return 0;
		return mTextureInfo.getWidth();
	}
	
	public int getColorMode(){
		if (mTextureInfo == null) return -1;
		return mTextureInfo.getColorMode();
	}
	
	public int getHeight(){
		if (mTextureInfo == null) return 0;
		return mTextureInfo.getHeight();
	}
	
	public Bitmap getBitmap(){
		return bm;
	}
	
	public void uploadImage(GKView sender, Bitmap bitmap){
		if (mTextureInfo == null) return ;
		//bm = bitmap;
		sender.uploadImage(sender, mTextureInfo, bitmap);
		if (bm != null && !bm.isRecycled()) {
			bm.recycle();
			bm = null;
		}
	}
}
