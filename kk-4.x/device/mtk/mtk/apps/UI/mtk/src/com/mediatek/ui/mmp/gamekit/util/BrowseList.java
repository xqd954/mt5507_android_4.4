package com.mediatek.ui.mmp.gamekit.util;

import java.util.ArrayList;
import java.util.List;

import com.mediatek.ui.mmp.model.FileAdapter;


import android.util.Log;

public class BrowseList {
	private ArrayList<FileAdapter> mForeFileList;
	//private List<FileAdapter> fileList;
	private int mCurHeadIdx;
	private int mNum = 0;
	private final int BORAD_NUM = 9;
	private static BrowseList mInstance;
	private static String TAG = "BrowseList";
	private BrowseList(){
		mForeFileList = new ArrayList<FileAdapter>();
		mCurHeadIdx = -1;
		mInitIndx = -1;
		mNum = 0;
		mGetMode = UNKNOW;
	}
	
	public static BrowseList getList(){
		if (mInstance == null){
			mInstance = new BrowseList();
		}		
		return mInstance;
	}
	
	
	public boolean addFiles(List<FileAdapter> files) {
		
		cleanList();
		
		if (files == null || files.size() == 0) {
			return false;
		}
		
		mForeFileList.addAll(0, files);
		mNum = mForeFileList.size();
		mCurHeadIdx = -1;
		mInitIndx = -1;
		mGetMode = UNKNOW;
		
		return true;
	}
	
	
	public void cleanList(){
		mForeFileList.clear();
		mCurHeadIdx = -1;
		mInitIndx = -1;
		mNum = 0;
		mGetMode = UNKNOW;
	}
	
	public int getCurrentIndex(){
		return mCurHeadIdx;
	}
	
	public int getSpecailIndx(String path){
		if (path == null){
			return -1;
		}
		
		for (int i = 0; i < mForeFileList.size(); i++){
			if (mForeFileList.get(i).getPath().equals(path)){
				return i;
			}
		}
		
		return -1;
	}
	
	public boolean setCurrentIndex(int index){
		mNum = mForeFileList.size();
		if (index > mNum || index < 0){
			return false;
		}
		
		mCurHeadIdx = index;
		
		return true;
	}
	
	public void setCurrentGetMode(int mode){
		if (mode >= UNKNOW && mode <= PRE){
			mGetMode = mode;
		}
	}
	
	public static final int UNKNOW = -1;
	public static final int NEXT = 0;
	public static final int PRE = 1;
	private int mGetMode = UNKNOW;
	
	public FileAdapter getSpecial(int index){
		if (index < 0 || index >= mForeFileList.size()){
			return null;
		}
		return mForeFileList.get(index);
	}
	
	public FileAdapter getNext(){
		mNum = mForeFileList.size();
		//if (mNum <= BORAD_NUM) return null;
		
		Log.d(TAG, " getNext num = " + mNum + "mcurIdx =" + mCurHeadIdx);
		
		if (mGetMode == NEXT || mGetMode == UNKNOW){
			
			mCurHeadIdx += 1;
			
			if (mCurHeadIdx >= mNum || mCurHeadIdx < 0){
				//mCurHeadIdx = 0;	//circulation			
				return null;
			}
			
		}else if (mGetMode == PRE){

			mCurHeadIdx += BORAD_NUM;
			
			//circulation
			//mCurHeadIdx %= mNum;
			if (mCurHeadIdx < 0 || mCurHeadIdx >= mNum) return null;
		}
		
		mGetMode = NEXT;
		
		Log.d(TAG, "getNext mcurIdx =" + mCurHeadIdx);
		return mForeFileList.get(mCurHeadIdx);
	}
	
	private int mInitIndx = -1;
/*	public FileAdapter getPhysicalNext(){
		mNum = mForeFileList.size();
		if (mNum <= 0) return null;
		mInitIndx += 1;
		if (mInitIndx >= mNum){
			//mCurHeadIdx = 0;
			return null;
		}
		
		return mForeFileList.get(mInitIndx);
	}*/
	
	public FileAdapter getPre(){
		mNum = mForeFileList.size();
		Log.d("MtkMainActivity.java", " getPre num = " + mNum + "mcurIdx =" + mCurHeadIdx);
		//if (mNum <= BORAD_NUM) return null;
		
		if (mGetMode == PRE || mGetMode == UNKNOW){
			mCurHeadIdx -= 1;
			
			if (mCurHeadIdx < 0 || mCurHeadIdx >= mNum){
				//mCurHeadIdx = mNum-1;//circulation			
				return null;
			}
			
		}else if (mGetMode == NEXT){
			
			mCurHeadIdx -= BORAD_NUM;
			//circulation
			/*mCurHeadIdx += mNum;
			mCurHeadIdx %= mNum;*/
			if (mCurHeadIdx < 0 || mCurHeadIdx >= mNum) return null;
		}
		
		mGetMode = PRE;
		Log.d("MtkMainActivity.java", " getPre mcurIdx =" + mCurHeadIdx);
		return mForeFileList.get(mCurHeadIdx);
	}
	
	public int getNum(){
		mNum = mForeFileList.size();
		Log.d(TAG, "getNum....filelist size = " + mNum);
		return mNum;
	}
	
}
