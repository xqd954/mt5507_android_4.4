package com.mediatek.mmpcm;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.UnknownHostException;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import jcifs.smb.SmbException;
import android.util.Log;
import com.mediatek.mmpcm.UIMediaPlayer;
import android.media.*;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class Info {
	private static final String TAG ="Info";
	protected String mFilePath;
	protected int mSrcType;
//	private InputStream mInputStream;
	protected static MetaData mCacheMetaData = null;
	
	public static MetaData getCacheMetaData(){
		return mCacheMetaData;
	}





	public MetaData getMediaInfo(String path){
		  /* String whereCause = MediaStore.Video.Media.DATA+"='"+path+"'";
		   String[] projection = { 
				   MediaStore.Video.Media.TITLE,
				   MediaStore.Video.Media.DATE_ADDED,
				   MediaStore.Video.Media.DATA,
				   MediaStore.Video.Media.ALBUM,
				   MediaStore.Video.Media.ARTIST,
				   MediaStore.Video.Media._ID, };  
		   Log.e("getVideoInfo", "video path:"+whereCause);
		   Cursor cursor = context.getContentResolver().query(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, 
				   projection,
				   whereCause, null, null);
		  
		   if (cursor == null || cursor.getCount() == 0) {	
			   mMetaInfo.setMetaData(null, null, null, null, null, null, null,
					   mtkPlayer.getDuration(), 0);
			   return null;  
		   }  */

		   MetaData mMetaInfo = new MetaData();
		   MediaMetadataRetriever retriever = new MediaMetadataRetriever();
           try{
              // retriever.setDataSource(path);
               String KEY_RETRIEVER_PLAYER = "X-tv-Retriever-Player";
               String KEY_THUMBNAIL_PATH = "X-tv-Thumbnail-Path";
               
               String VALUE_RETRIEVER_PLAYER = "CMPB_PLAYER";
               String VALUE_THUMBNAIL_PATH = "THRD_USB";
               Map<String, String> Headers_t=new HashMap<String, String>();
               
               Headers_t = new HashMap<String,String>();
               
  
               //Use CMPB for MetaDataInfo, old style
               Headers_t.put(KEY_RETRIEVER_PLAYER,VALUE_RETRIEVER_PLAYER);
               
               Log.e("Thumbnail", "setDataSource 1");
               //Option 1
               retriever.setDataSource(path,Headers_t); //OK for old style, change framwork to use cmpb
               
               //Option 2
               /*
               retriever.setPlayerType(6);
               retriever.setDataSource(path);
               */
           }catch(Exception e){
           		
			   retriever.release();

			   Log.d(TAG,"setdataSource fail ~");

		   
               return null;
           }
		  
		   
	
		   String mtitle = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_TITLE);
		   	//cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.TITLE));
		   
		   String mdirector =retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DIRECTOR);
		   String mcopyright = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_COPYRIGHT);

		   String myear = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_YEAR);
		   	//cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.DATE_ADDED));
		   String mgenre = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_GENRE);

		   String martist =  retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_ARTIST);
		   	//cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.ARTIST));

		   String malbum = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_ALBUM);
		   //cursor.getString(cursor.getColumnIndex(MediaStore.Video.Media.ALBUM));

		   String mbitrate = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_BITRATE);
		   Log.e("mbitrate", "mbitrate:"+"null:"+mbitrate+"_re:"+mgenre+"_mdirector:"+mdirector+"_mcopyright:"+mcopyright);

		   String mdur = retriever.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION);

		   int dur = 0;
		  try{
			  dur = Integer.valueOf(mdur);

		  }catch(Exception ex){
			   Log.d(TAG,"duration to int error~~");
		  }




		   int mbitratet= 200;
		   try{
			   mbitratet = Integer.valueOf(mbitrate);

		   }catch(Exception ex){
				Log.d(TAG,"mbitrate to int error~~");
		   }

		   retriever.release();

		   mMetaInfo.setMetaData(mtitle, mdirector, mcopyright, myear,
				   mgenre, martist, malbum, dur,
				   mbitratet);
		   Log.e(TAG, "video myear:"+myear+"_mtitle:"+mtitle+"_martist:"+martist
				   +"_malbum:"+malbum+"_mgenre:"+mgenre);
		    
		   //int videoID = cursor.getInt(cursor.getColumnIndex(MediaStore.Video.Media._ID));
		   
		   return mMetaInfo;
		  }  






	public Bitmap getAudioBmp(String path){
		 MediaMetadataRetriever retriever = new MediaMetadataRetriever();
		  try{
               retriever.setDataSource(path);
               /* Wait confirm
               String KEY_RETRIEVER_PLAYER = "X-tv-Retriever-Player";
               String KEY_THUMBNAIL_PATH = "X-tv-Thumbnail-Path";
               
               String VALUE_RETRIEVER_PLAYER = "CMPB_PLAYER";
               String VALUE_THUMBNAIL_PATH = "THRD_USB";
               
               Map<String, String> Headers_t=new HashMap<String, String>();
               
               Headers_t = new HashMap<String,String>();
               
               //Use CMPB for MetaDataInfo, old style
               Headers_t.put(KEY_RETRIEVER_PLAYER,VALUE_RETRIEVER_PLAYER);
               
               //New API setPlayerType for AOSP
               retriever.setPlayerType(com.mediatek.MtkMediaMetadataRetriever.PlayerType.CMPB_PLAYER);
               
               Log.e("Thumbnail", "setDataSource 1");
               retriever.setDataSource(path,Headers_t); //OK for old style, change framwork to use cmpb
               */

           }catch(Exception e){
           		retriever.release();
               return null;
           }
		 byte[] bit = retriever.getEmbeddedPicture();
		 Bitmap bmp = null;
		 if(bit != null){
			int len = bit.length;
			bmp = BitmapFactory.decodeByteArray(bit,0,len);
		 }else{
		 	retriever.release();
			Log.d(TAG,"get bit = null");
			return null;
		 }
		 retriever.release();
		 Log.d(TAG,"getAudiobmp:"+bmp);
		 return bmp;

	}

	/**
	 * get file size
	 * 
	 */
	 /*
	public long getFileSize(String mcurPath) {
		long fileSize = 0;
		MmpTool.LOG_INFO("getFileSize = $$$$$$$$$$$$$$" + mcurPath);
		if (mcurPath == null) {
			return fileSize;
		}
		switch (mSrcType) {
		case FileConst.SRC_DLNA: {
			DLNADataSource dlnaSource = DLNAManager.getInstance()
					.getDLNADataSource(mcurPath);
			if (dlnaSource != null) {
				fileSize = dlnaSource.getContent().getSize();
				MmpTool.LOG_INFO("getFileSize dlna $$$$$$$$$$$$$$" + fileSize);
			}
		}
			break;
		case FileConst.SRC_SMB: {
			SambaManager sambaManager = SambaManager.getInstance();
			try {
				fileSize = sambaManager.size(mcurPath);
				MmpTool.LOG_INFO("getFileSize samba $$$$$$$$$$$$$$" + fileSize);
			} catch (MalformedURLException e) {
				e.printStackTrace();
			} catch (SmbException e) {
				e.printStackTrace();
			} catch (UnknownHostException e) {
				e.printStackTrace();
			}
		}
			break;
		case FileConst.SRC_USB: {
			MtkFile mFile = null;
		    mFile = new MtkFile(mcurPath);
            if (mFile != null) {
            	fileSize = mFile.getFileSize();
			}
			MmpTool.LOG_INFO("getFileSize local $$$$$$$$$$$$$$" + fileSize);
		}
			break;
		default:
			break;
		}
		return fileSize;
	}

	@Override
	public DataSourceType getSourceType() {
		DataSourceType eSourceType = null;
		switch (mSrcType) {
		case FileConst.SRC_DLNA:
			eSourceType = DataSourceType.SOURCE_TYPE_DLNA;
			break;
		case FileConst.SRC_SMB:
			eSourceType = DataSourceType.SOURCE_TYPE_SAMBA;
			break;
		case FileConst.SRC_USB:
			eSourceType = DataSourceType.SOURCE_TYPE_LOCAL;
			break;
		default:
			break;
		}
		Log.e("plz", "Thumbnail getSourceType:" + eSourceType);
		return eSourceType;
	}

	@Override
	public boolean isSeekable() {
		switch (mSrcType) {
		case FileConst.SRC_DLNA:
			DLNADataSource dlnaSource = DLNAManager.getInstance()
					.getDLNADataSource(mFilePath);
			return dlnaSource.getContent().canSeek();
		case FileConst.SRC_SMB:
			break;
		case FileConst.SRC_USB:
			break;
		default:
			break;
		}
		return true;
	}

	@Override
	public long getSourceSize() {
		return getFileSize(mFilePath);
	}

	@Override
	public InputStream newInputStream() {
		closeStream();
		if (mSrcType == FileConst.SRC_USB) {
			try {
				mInputStream = new FileInputStream(mFilePath);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				return null;
			}
		} else if (mSrcType == FileConst.SRC_SMB) {
			try {
				mInputStream = SambaManager.getInstance()
						.getSambaDataSource(mFilePath).newInputStream();
			} catch (SmbException e) {
				e.printStackTrace();
				return null;
			} catch (MalformedURLException e) {
				e.printStackTrace();
				return null;
			} catch (UnknownHostException e) {
				e.printStackTrace();
				return null;
			}
		} else if (mSrcType == FileConst.SRC_DLNA) { // DLNA
			DLNADataSource dlnaDataSource = DLNAManager.getInstance()
					.getDLNADataSource(mFilePath);
			if (null != dlnaDataSource) {
				mInputStream = dlnaDataSource.newContentInputStream();
			}

		} else {// HTTP
			return null;
		}

		return mInputStream;
	}

	public void closeStream() {
		if (null != mInputStream) {
			MmpTool.LOG_INFO("  input stream is not null  path=" + mFilePath);
			try {
				mInputStream.close();
			} catch (IOException e) {
				MmpTool.LOG_INFO(" close input sream " + e.toString());
			} finally {
				mInputStream = null;
			}
		} else {
			MmpTool.LOG_INFO("  input stream is  null   path=" + mFilePath);
		}
	}
	*/
}
