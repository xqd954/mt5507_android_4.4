package com.mediatek.ui.mmp.model;

import java.io.File;
import java.io.IOException;
import java.sql.Time;

import android.graphics.Bitmap;
import java.io.FileInputStream;

import com.mediatek.mmpcm.Info;
import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.audioimpl.AudioInfo;
import com.mediatek.mmpcm.fileimpl.AudioFile;
import com.mediatek.mmpcm.fileimpl.FileConst;
import com.mediatek.mmpcm.fileimpl.MtkFile;
import com.mediatek.mmpcm.fileimpl.PhotoFile;
import com.mediatek.mmpcm.fileimpl.UsbFileOperater;
import com.mediatek.mmpcm.fileimpl.VideoFile;
import com.mediatek.mmpcm.videoimpl.VideoInfo;
import com.mediatek.ui.util.MtkLog;

public class LocalFileAdapter extends FileAdapter {
	private static final String TAG = "LocalFileAdapter";
	private MtkFile mFile;
	private String mName;
	private static UsbFileOperater sOperator = UsbFileOperater.getInstance();

	public LocalFileAdapter(MtkFile file) {
		mFile = file;
	}

	public LocalFileAdapter(String path) {
		mFile = new MtkFile(path);
	}

	public LocalFileAdapter(String path, String name) {
		mFile = new MtkFile(path);
		mName = name;
	}

	public LocalFileAdapter(File file) {
		mFile = new MtkFile(file);
	}

	public void stopDecode() {
		if (null != mFile && mFile instanceof PhotoFile) {
			MtkLog.i(TAG, " Bitmap  stopDecode --------------");
			((PhotoFile) mFile).stopDecode();
		}
	}

	@Override
	public boolean isPhotoFile() {
		return mFile.isPhotoFile();
	}

	@Override
	public boolean isAudioFile() {
		return mFile.isAudioFile();
	}

	@Override
	public boolean isVideoFile() {
		return mFile.isVideoFile();
	}

	@Override
	public boolean isTextFile() {
		return mFile.isTextFile();
	}

	@Override
	public String getSize() {
		return mFile.getSize();
	}

	public String getTextSize() {
		return getTextSize(mFile.getFileSize());
	}

	@Override
	public String getAbsolutePath() {
		return mFile.getAbsolutePath();
	}

	@Override
	public String getPath() {
		return mFile.getPath();
	}

	@Override
	public String getName() {
		if (mName != null && mName.length()>0) {
			return mName;
		}

		return mFile.getName();
	}

	@Override
	public Bitmap getThumbnail(int width, int height,boolean isThumbnail) {
		if ((isPhotoFile() || isThrdPhotoFile()) && !isValidSizePhoto()){
			return null;
		}
		return mFile.getThumbnail(width, height,isThumbnail);
	}

	@Override
	public boolean isDirectory() {
		return mFile.isDirectory();
	}

	@Override
	public boolean isFile() {
		return mFile.isFile();
	}

	@Override
	public long lastModified() {
		return mFile.lastModified();
	}

	public void stopThumbnail(){
		
		mFile.stopThumbnail();
	}

	@Override
	public long length() {
		return mFile.length();
	}

	@Override
	public boolean delete() {
		try {
			sOperator.addFileToDeleteList(mFile);
		} catch (IOException e) {
			e.printStackTrace();
		}
		sOperator.deleteFiles();

		return true;
	}

	@Override
	public String getInfo() {
		return getInfoByCache(false);
	}
	
	private String getInfoByCache(boolean isCache){
		String info ="";
		MetaData data = null;
		if (isPhotoFile() || isThrdPhotoFile()) {
			if(isValidSizePhoto()){
				info = assemblyInfos(getName(), getResolution(), getSize());
			
			}else{
				info = assemblyInfos(getName(), "", getSize());
			
			}
			
			
		}else if (isAudioFile()) {
			AudioFile file = (AudioFile) mFile;
			if(isCache){
				data =  Info.getCacheMetaData();
				if (null == data) {
					return null;
				}
			}else{
				data = file.getMetaDataInfo();
			}


			if (null == data) {
			
				info = assemblyInfos(getName(), "","", getSize());
			}else{
				String title = data.getTitle();
				if (title == null || title.length()<=0) {
					title = getName();
				}
				info = assemblyInfos(title, data.getAlbum(), data.getGenre(), data
						.getYear(), getSize());
			}
			
		} else if (isVideoFile()) {
			VideoFile file = (VideoFile) mFile;
			if(isCache){
				data =  Info.getCacheMetaData();
				if (null == data) {
					return null;
				}
			}else{
				data = file.getMetaDataInfo();
			}
			
			if (null == data) {
				
				info = assemblyInfos(getName(), "","", getSize());
			}else{
			
				String title = data.getTitle();
				if (title == null || title.length()<=0) {
					title = getName();
				}		
				int dur = data.getDuration();
				MtkLog.i(TAG, "$$$$$$$$$$$$time = " + dur);
				info = assemblyInfos(title, data.getYear(), setTime(dur), getSize());
				
			}

		} else if (isTextFile()) {
			info = assemblyInfos(getName(), getLastModified(), getTextSize());
		}

		return info;
	}

	public String getPreviewBuf(){
		MtkLog.d(TAG,"getPreviewBuf getAbsolutePath ="+ getAbsolutePath());
		try{
			FileInputStream fileIS = new FileInputStream(getAbsolutePath());
			return super.getPreviewBuf(fileIS);


		}catch(Exception ex){
			ex.printStackTrace();
		}
		

		return "";


	}
	
	
	@Override
	public String getResolution() {
//		String resolution = null;
//		if (isPhotoFile()) {
//			resolution = mFile.getResolution();
//		}

		return mFile.getResolution();
	}

	@Override
	public String getSuffix() {
		return "";
	}

	@Override
	public String getCacheInfo() {

		return getInfoByCache(true);
	}
}
