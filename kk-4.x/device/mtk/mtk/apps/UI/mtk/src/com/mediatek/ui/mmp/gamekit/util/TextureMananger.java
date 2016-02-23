package com.mediatek.ui.mmp.gamekit.util;

import java.util.concurrent.ConcurrentHashMap;

import android.content.Context;
import com.mediatek.ui.R;
import com.mediatek.ui.mmp.gamekit.media.MtkGLES2TextureInfo;
import com.mediatek.ui.util.MtkLog;

public class TextureMananger {

	private Integer[] mTexturePool = { R.string.txture_0, R.string.txture_1,
			R.string.txture_2, R.string.txture_3, R.string.txture_4,
			R.string.txture_5, R.string.txture_6, R.string.txture_7,
			R.string.txture_8 };
	
	private static ConcurrentHashMap<String, MtkGLES2TextureInfo>mTextureInfoList = 
		new ConcurrentHashMap<String, MtkGLES2TextureInfo>();
	
	private static TextureMananger mInstance;
	private Context mContext;
	
	private TextureMananger(Context cxt){
		mContext = cxt;
	}
	
	public static TextureMananger getTextureManager(Context cxt){
		if (mInstance == null){
			mInstance = new TextureMananger(cxt);
			mTextureInfoList.clear();
		}
		
		return mInstance;
	}
	
	public void reset(){
		mTexturePool[0] = R.string.txture_0;
		mTexturePool[1] = R.string.txture_1;
		mTexturePool[2] = R.string.txture_2;
		mTexturePool[3] = R.string.txture_3;
		mTexturePool[4] = R.string.txture_4;
		mTexturePool[5] = R.string.txture_5;
		mTexturePool[6] = R.string.txture_6;
		mTexturePool[7] = R.string.txture_7;
		mTexturePool[8] = R.string.txture_8;
	}
	
	public void moveLeft(){
		Integer[] list = mTexturePool;
		if (list == null ||list.length <= 0) return;
		int size = list.length;
		int i=0;
		int temp = list[0];
		
		for (i=0; i<size -1; i++){
			list[i] = list[i+1]; 
		}
		
		list[size -1] = temp;
		MtkLog.d("Lei", "Left");
		printList();
	}
	
	public void moveRight(){
		Integer[] list = mTexturePool;
		if (list == null ||list.length <= 0) return;
		int size = list.length;		
		int temp = list[size-1];
		
		for (int i=size-1; i>0; i--){
			list[i] = list[i-1]; 
		}
		
		list[0] = temp;
		
		MtkLog.d("Lei", "Right");
		printList();
	}
	
	public int getFoucsIndex(){
		Integer[] list = mTexturePool;
		if (list == null ||list.length <= 0) return 0;
		
		return list.length/2;
	}
	
	public String getFoucsTextureName(){
		int index = getFoucsIndex();
		/*MtkLog.d("Lei", "foucs idx = " + index + "ID" 
				+ mContext.getString(mTexturePool[index]));*/
		return mContext.getString(mTexturePool[index]);
	}
	
	public MtkGLES2TextureInfo getFoucsTextureInfo(){
		return getTextureInfo(getFoucsTextureName());
	}
	
	public int getPreId(){
		MtkLog.d("MtkMainActivity.java", "mcurIdx getPreId" 
		+ mContext.getString(mTexturePool[0]));
		return mTexturePool[0];
	}
	
	public int getNextId(){
		MtkLog.d("MtkMainActivity.java", "mcurIdx getNextId" 
				+ mContext.getString(mTexturePool[mTexturePool.length-1]));
		return mTexturePool[mTexturePool.length-1];
	}
	
	
	public int getId(int index){
		Integer[] list = mTexturePool;
		if (list == null || index < 0 || index >= list.length) return 0;
		
		return mTexturePool[index];
	}
	
	public int getSize(){
		return mTexturePool.length;
	}
	
	public MtkGLES2TextureInfo getTextureInfo(String key){
		return mTextureInfoList.get(key);
	}
	
	public void putTextureInfo(String key, MtkGLES2TextureInfo info){
		mTextureInfoList.put(key, info);
	}
	
	public ConcurrentHashMap<String, MtkGLES2TextureInfo> getTextureInfoList(){
		return mTextureInfoList;
	}
	
	public void clean(){
		mTextureInfoList.clear();
	}
	public void printList(){
		for (int i = 0; i < mTexturePool.length; i++){
			MtkLog.d("Lei", "name = " + mContext.getString(mTexturePool[i]));
		}
	}
}
