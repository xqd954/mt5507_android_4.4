package com.mediatek.mmpcm.audioimpl;

import java.io.FileNotFoundException;
import java.io.InputStream;

import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.provider.MediaStore;
import android.util.Log;

import com.mediatek.mmp.*;
import com.mediatek.mmp.util.*;
import com.mediatek.mmp.MtkMediaPlayer.*;
import com.mediatek.mmpcm.UIMediaPlayer;
import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.fileimpl.FileConst;
import com.mediatek.mmpcm.Info;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;

public class AudioInfo extends Info {
	private static final Uri mArtworkUri = Uri
			.parse("content://media/external/audio/albumart");

	private static final String TAG = "AudioInfo";

	private ContentResolver mResolver;
	private Bitmap mBitmap;
	private UIMediaPlayer mtkPlayer;
	// private String dataSource;
	// private int mSrcType;
	private static AudioInfo aInfo = null;
	private boolean metaLoadStart = false;

	private String[] mAlbumId = new String[] { MediaStore.Audio.Media.ALBUM_ID };
	private String[] mAudioId = new String[] { MediaStore.Audio.Media._ID };

//	private InputStream mInputStream;

	private AudioInfo(Context context,int srcType) {
		mSrcType = srcType;
		mResolver = context.getContentResolver();
		mtkPlayer = new UIMediaPlayer(srcType);
		mBitmap = null;
	}

	private AudioInfo(int srcType) {
		// mtkPlayer = new MtkMediaPlayer();
		mSrcType = srcType;
		mtkPlayer = new UIMediaPlayer(srcType);
		mBitmap = null;
	}

	public UIMediaPlayer getPlayer() {
		return mtkPlayer;
	}

	public static synchronized AudioInfo getInstance(int srcType) {
		if (aInfo == null) {
			aInfo = new AudioInfo(srcType);
		}
		return aInfo;
	}


	private AudioInfo(Context context) {
			mSrcType = FileConst.SRC_USB;
			mResolver = context.getContentResolver();
			mtkPlayer = new UIMediaPlayer(mSrcType);
			mBitmap = null;
	}
	
	private AudioInfo() {
		// mtkPlayer = new MtkMediaPlayer();
		mSrcType = FileConst.SRC_USB;
		mtkPlayer = new UIMediaPlayer(mSrcType);
		mBitmap = null;
	}



	public static synchronized AudioInfo getInstance() {
		if (aInfo == null) {
			aInfo = new AudioInfo();
		}
		return aInfo;
	}

	/**
	 * According to the parameters for the meta data info
	 * 
	 * @param path
	 * @param srcType
	 *            , specified source typle
	 * @return return meta data info
	 */
	public MetaData getMetaDataInfo(String path, int srcType)
			throws IllegalArgumentException {
		MmpTool.LOG_DBG("path = " + path);
		if(srcType != mSrcType){
			mtkPlayer.release();
			mSrcType = srcType; 
			mtkPlayer = new UIMediaPlayer(srcType);
		
		}

		if (srcType == FileConst.SRC_USB) {
					
			return	getMediaInfo(path);
		}
		
		MetaData mMetaInfo = new MetaData();
		Log.e("JF", "getMetaDataInfo()...");

		if (path == null) {
			throw new IllegalArgumentException("empty path!");
		}

		if (mtkPlayer != null) {

			mFilePath = path;

			try {
				mtkPlayer.reset();
				//if(mSrcType == FileConst.SRC_USB){
        		mtkPlayer.setDataSource(mFilePath);
		        /*}else{
		         	mtkPlayer.setDataSource(this);
		        	}*/
				mtkPlayer.setPlayerRole(MtkMediaPlayer.PlayerRole.ROLE_GET_METADATA);
				mtkPlayer.prepare();
			} catch (Exception e) {
				MmpTool.LOG_INFO(" getMetaDataInfo()  :" + e.toString());
				mMetaInfo.setMetaData(null, null, null, null, null, null, null,
						0, 0);
				try {
					// mtkPlayer.getMetaDataStop();
					mtkPlayer.stop();
					metaLoadStart = false;
				} catch (IllegalStateException e1) {
					e1.printStackTrace();
				}
				mtkPlayer.closeStream();
				return mMetaInfo;
			}

			metaLoadStart = true;
			//AudioTrackInfo info = mtkPlayer.getAudioTrackInfo(true);
			MetaDataInfo info = mtkPlayer.getMetaDataInfo();
			if (null == info) {
				Log.e("JF", "info is null");
				mMetaInfo.setMetaData(null, null, null, null, null, null, null,
						0, 0);
			} else {
				mMetaInfo.setMetaData(info.getTitle(), info.getDirector(),
						info.getCopyright(), info.getYear(), info.getGenre(),
						info.getArtist(), info.getAlbum(),
						info.getTotalPlayTime(), info.getBiteRate());
			}

			try {
				mtkPlayer.stop();
				metaLoadStart = false;
			} catch (IllegalStateException e) {
				e.printStackTrace();
			}

			mtkPlayer.closeStream();
		}

		return mMetaInfo;
	}

	/*
	 * public int getSrcType() {
	 * 
	 * return mSrcType; }
	 * 
	 * 
	 * private void closeStream() { if (null != mInputStream) {
	 * MmpTool.LOG_INFO("  input stream is not null"); try {
	 * mInputStream.close(); } catch (IOException e) { MmpTool.LOG_INFO(
	 * " close input sream " + e.toString()); } finally { mInputStream = null; }
	 * } }
	 */

	/**
	 * Stop get meta data
	 */
	public void stopMetaData() {
		if (mtkPlayer != null && metaLoadStart == true) {
			try {
				// mtkPlayer.getMetaDataStop();
				mtkPlayer.stop();
				metaLoadStart = false;
			} catch (IllegalStateException e) {
				e.printStackTrace();
				metaLoadStart = false;
				return;
			}
		}
	}

	/**
	 * @deprecated
	 */
	/*
	 * public void setAlbumArtwork(Bitmap artwork) { mBitmap = artwork; }
	 */

	/**
	 * @deprecated
	 */
	/*
	 * public void setAlbumArtwork(String path) { mBitmap =
	 * BitmapFactory.decodeFile(path); }
	 */

	/**
	 * According to the parameters for the get albumArt work
	 * 
	 * @return albumArt work
	 * @deprecated
	 */
	/*
	 * public Bitmap getAlbumArtwork() {
	 * 
	 * File file = PlayList.getPlayList().getCurrentFile(Const.FILTER_AUDIO); if
	 * (file == null) { return null; }
	 * 
	 * return getAlbumArtwork(file.getPath()); }
	 */

	/**
	 * According to the parameters for the get albumArt work
	 * 
	 * @param path
	 * @return return albumArt work
	 * @deprecated
	 */
	/*
	 * public Bitmap getAlbumArtwork(String path) {
	 * 
	 * Cursor mCursor = getCursor(path, mAlbumId); if (mCursor == null ||
	 * mCursor.getCount() == 0) { return null; }
	 * 
	 * mCursor.moveToFirst(); long albumId = mCursor.getLong(0);
	 * 
	 * return getArtwork(albumId); }
	 */
	private Cursor getCursor(String path, String[] projection) {
		if (path == null) {
			return null;
		}

		Uri contentUri = MediaStore.Audio.Media.getContentUriForPath(path);
		String selection = MediaStore.Audio.Media.DATA + "=?";
		String[] selectionArgs = new String[] { path };

		return mResolver.query(contentUri, projection, selection,
				selectionArgs, null);
	}

	/**
	 * @deprecated
	 */
	private Bitmap getArtwork(long albumId) {
		Uri uri = ContentUris.withAppendedId(mArtworkUri, albumId);
		if (uri != null) {
			InputStream in = null;

			try {
				in = mResolver.openInputStream(uri);
			} catch (FileNotFoundException e) {

				// TODO The album artwork file doesn't exist
				// should be replaced by default bitmap

				return mBitmap;
			}

			return BitmapFactory.decodeStream(in, null, null);
		}

		return mBitmap;

	}

	// public boolean isSeekable() {
	// // TODO Auto-generated method stub
	// return false;
	// }

	/**
	 * @deprecated
	 * @SuppressWarnings("unused")
	 */
	/*
	 * private Bitmap getArtworkFromFile(long albumId) { return null; }
	 */

	/**
	 * @deprecated
	 * @SuppressWarnings("unused")
	 */
	/*
	 * private Bitmap getDefaultArtwork() { return null; }
	 */

}
