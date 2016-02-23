package com.mediatek.ui.mmp.model;

import java.io.InputStream;
import java.sql.Time;

import android.graphics.Bitmap;
import android.util.Log;

import com.mediatek.mmpcm.Info;
import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.audioimpl.AudioInfo;
import com.mediatek.mmpcm.audioimpl.CorverPic;
import com.mediatek.mmpcm.fileimpl.FileConst;
import com.mediatek.mmpcm.videoimpl.Thumbnail;
import com.mediatek.mmpcm.videoimpl.VideoInfo;
import com.mediatek.netcm.dlna.DLNAFile;
import com.mediatek.netcm.dlna.DLNAManager;
import com.mediatek.ui.util.MtkLog;

public class DlnaFileAdapter extends FileAdapter {
	private static final String TAG = "DlnaFileAdapter";
	private String mName;
	private String mAbsolutePath;
	private DLNAFile mFile;
	private static DLNAManager sManager = DLNAManager.getInstance();
	private AudioInfo mAudioInfo;
	private VideoInfo mFileInfo;
	
	public DlnaFileAdapter(DLNAFile file, String absolutePath) {
		mFile = file;
		mName = mFile.getName();
		mAbsolutePath = absolutePath;
	}
	public DlnaFileAdapter(DLNAFile file, String absolutePath,
			AudioInfo audioInfo, VideoInfo fileInfo) {
		this(file, absolutePath);
		mAudioInfo = audioInfo;
		mFileInfo = fileInfo;
	}

	@Override
	public boolean isPhotoFile() {
		return sManager.getType(mName) == DLNAManager.FILE_TYPE_IMAGE;
	}

	@Override
	public boolean isAudioFile() {
		return sManager.getType(mName) == DLNAManager.FILE_TYPE_AUDIO;
	}

	@Override
	public boolean isVideoFile() {
		return sManager.getType(mName) == DLNAManager.FILE_TYPE_VIDEO;
	}

	@Override
	public boolean isTextFile() {
		return sManager.getType(mName) == DLNAManager.FILE_TYPE_TEXT;
	}

	@Override
	public boolean delete() {
		return false;
	}

	@Override
	public String getAbsolutePath() {
		return mAbsolutePath;
	}

	@Override
	public String getName() {
		return mName;
	}

	@Override
	public String getPath() {
		return mAbsolutePath;
	}

	@Override
	public boolean isDirectory() {
		int type = sManager.getType(mName);
		MtkLog.d(TAG, "Is Directory : " + mName + "; Type : " + type);

		if (type == DLNAManager.FILE_TYPE_DIRECTORY
				|| type == DLNAManager.FILE_TYPE_DEVICE) {
			return true;
		}

		return false;
	}
	
	@Override
	public boolean isDevice() {
		int type = sManager.getType(mName);
		return (type == DLNAManager.FILE_TYPE_DEVICE);
	}

	@Override
	public String getLastModified() {
		return "-";
	}

	@Override
	public long lastModified() {
		return 0;
	}
	
	public String getDeviceName()
	{
		
		if (isDevice()){
			return getName();
		}
		String path = getPath();
		if (path != null){
			if(path.length()>1)
			{
				int end = path.indexOf(0x2f, 1);
				if (end <= 0) {
					return "";
				}
				return path.substring(1, end);
			}
		}
		
		return "";
	}

	@Override
	public long length() {
		return mFile.getSize();
	}

	@Override
	public boolean isFile() {
		return !isDirectory();
	}

	@Override
	public String getSize() {
		return getSize(length());
	}
	
	@Override
	public  String getTextSize(){
		return getTextSize(length());
	}
	
	@Override
	protected InputStream getInputStream() {
		return sManager.getFileStream(mName);
	}

	@Override
	public Bitmap getThumbnail(int width, int height,boolean isThumbnail) {

		Bitmap bitmap = null;
		if (isVideoFile()){
			Thumbnail vThumb = Thumbnail.getInstance();
			bitmap = vThumb.getVideoThumbnail(FileConst.SRC_DLNA, 
					this.getName(), width, height);
			
		} else if (isPhotoFile() && isValidSizePhoto()){
			bitmap = decodeBitmap(getInputStream(), width, height);
			
		} else if (isAudioFile()){
			CorverPic aCorver =  CorverPic.getInstance();
			bitmap = aCorver.getAudioCorverPic(FileConst.SRC_DLNA, 
					this.getName(), width, height);          
		}
		
		return bitmap;
	}

	public void stopThumbnail(){
		if (isVideoFile()){
			Thumbnail vThumb = Thumbnail.getInstance();
			vThumb.stopThumbnail();
			
		} else if (isPhotoFile()){
			stopDecode();
			
		} else if (isAudioFile()){
			CorverPic aCorver =  CorverPic.getInstance();
			aCorver.stopThumbnail();          
		}
		
	}
	
	@Override
	public String getInfo() {
		return getInfoByCache(false);
	}
	



	public String getPreviewBuf(){
		
		
		InputStream is = sManager.getFileStream(mName);
		return super.getPreviewBuf(is);

	}
	
	


	

	private String getInfoByCache(boolean isCache){
		
		String info;
		MetaData data = null;
		if(mFile == null){
			MtkLog.d(TAG,"getInfoByCache mFile is null and  isCache "+isCache );
			return getName();

		}
		
		if (isPhotoFile()) {

			if(isValidSizePhoto()){
				info = assemblyInfos(getName() + mFile.getSuffix(),
						getResolution(), getSize());
			
			}else{
				info = assemblyInfos(getName() + mFile.getSuffix(),
						"", getSize());
			

			}
			
		} else if (isAudioFile()) {
			MtkLog.d(TAG,"getInfoByCache isCache "+isCache);
			if(isCache){
				data = Info.getCacheMetaData();
				if (null == data) {
					return null;
				}
			}else{
				data = mAudioInfo
					.getMetaDataInfo(mName, FileConst.SRC_DLNA);
			}
			MtkLog.d(TAG,"getInfoByCache data ="+data+"mFile = "+mFile);
			
			if(data == null){
				info = assemblyInfos(getName() + mFile.getSuffix(), "","","" ,getSize());
			}else{
			
				info = assemblyInfos(getName() + mFile.getSuffix(), data.getAlbum(), data.getGenre(),data.getYear(), getSize());
			}
		} else if (isVideoFile()) {
			if(isCache){
				data = Info.getCacheMetaData();
				if (null == data) {
					return null;
				}
			}else{
				data = mFileInfo
						.getMetaDataInfo(mName, FileConst.SRC_DLNA);
			}
			
			if(data == null ){
				info = assemblyInfos(getName()+ mFile.getSuffix()+ "", "",
						"", getSize());

			}else{
				info = assemblyInfos(getName() + mFile.getSuffix(), data.getYear(),
						setTime(data.getDuration()), getSize());
			}
			
		} else {
			
			info = assemblyInfos(getName() + mFile.getSuffix(), getLastModified(), getTextSize());
			
		}

		return info;
	}

	@Override
	public String getResolution() {
		String resolution = getResolution(getInputStream());

		return resolution;
	}

	@Override
	public String getSuffix() {
		if(null!=mFile)
		{
			return mFile.getSuffix();
		}
		return "";
	}
	@Override
	public String getCacheInfo() {
		return getInfoByCache(true);
	}
}
