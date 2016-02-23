package com.mediatek.mmpcm.audioimpl;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.util.Log;

import com.mediatek.mmp.*;
import com.mediatek.mmp.util.*;
import com.mediatek.mmp.MtkMediaPlayer.*;
import com.mediatek.mmpcm.UIMediaPlayer;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;

import com.mediatek.mmpcm.Info;
import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.fileimpl.FileConst;

public class CorverPic extends Info{
	private static final String TAG = "CorverPic";
    private UIMediaPlayer mtkPlayer;
    private static CorverPic aCorver = null;
    private boolean thumbLoadStart = false;
	private boolean cancelLoad = false;
    

	private CorverPic(int srcType) {
		mSrcType = srcType;
		//mtkPlayer = AudioInfo.getInstance().getPlayer();
		mtkPlayer = new UIMediaPlayer(srcType) ;
	}


	private CorverPic() {
		mSrcType = FileConst.SRC_USB;
		//mtkPlayer = AudioInfo.getInstance().getPlayer();
		mtkPlayer = new UIMediaPlayer(mSrcType) ;
	}


	public static synchronized CorverPic getInstance(int srcType) {
		if (aCorver == null) {
			aCorver = new CorverPic(srcType);
		}
		return aCorver;
	}

	public static synchronized CorverPic getInstance() {
		if (aCorver == null) {
					aCorver = new CorverPic();
		}
		return aCorver;
	}
	/**
	 * Get audio corver picture by specified source type,path,
	 * @param srcType
	 * @param filepath
	 * @param width
	 * @param height
	 * @return
	 * @throws IllegalArgumentException
	 */
	public Bitmap getAudioCorverPic(int srcType, String filepath, int width,
			int height) throws IllegalArgumentException {
	

		MmpTool.LOG_DBG("path = " + filepath);
        Log.e(TAG, "getAudioCorverPic()...");
		if (filepath == null) {
			throw new IllegalArgumentException("empty path!");
		}

		if(srcType != mSrcType){
			mSrcType = srcType;
			mtkPlayer.release();
			mtkPlayer = new UIMediaPlayer(srcType);
		}

		if (srcType == FileConst.SRC_USB) {
	        mCacheMetaData = getMediaInfo(filepath);
	        return  getAudioBmp(filepath);
		}
		

		synchronized(this) {
			cancelLoad = false;
			thumbLoadStart = true;
		}
		if (mtkPlayer != null) {

			mFilePath = filepath;

			try {
				mtkPlayer.reset();
		        Log.e(TAG, "filepath="+filepath);

		        mtkPlayer.setDataSource(mFilePath);
		        
				//mtkPlayer.setPlayerRole(PlayerRole.ROLE_GET_METADATA);
				mtkPlayer.prepare();
				
				if(!mtkPlayer.ifMP3Media()){
					
					mtkPlayer.closeStream();
					loadDone();
					return null;
				}
				
			} catch (Exception e) {
				Log.e(TAG, "exception");
				MmpTool.LOG_INFO( " getAudioCorverPic() :" + e.toString());
				
				mtkPlayer.closeStream();
				loadDone();
				return null;
			}
			

			byte[] thBuffer = null;
			if (!cancelLoad) {
				
				thBuffer =mtkPlayer.getEmbeddedPicture();
					
			}


			if (thBuffer == null) {
				
				mtkPlayer.closeStream();
				loadDone();
				return null;
			}else{
				try{	 
					Bitmap bitMap = BitmapFactory.decodeByteArray(thBuffer, 0,thBuffer.length);
					mtkPlayer.closeStream();
					int bitWidth = bitMap.getWidth();
					int bitHeight = bitMap.getHeight();
					float scaleWidth = width / (float) bitWidth;
					float scaleHeight = height / (float) bitHeight;
					Matrix matrix = new Matrix();
					matrix.postScale(scaleWidth, scaleHeight);
					bitMap = Bitmap.createBitmap(bitMap, 0, 0, bitWidth, bitHeight,matrix, true);
					loadDone();

					return bitMap;
				}catch(Exception ex){
	
					ex.printStackTrace();
	
				}

			
				
			}
		
		}
		
		loadDone();
		return null;
	}




	private void loadDone(){
		synchronized(this) {
			thumbLoadStart = false;
			try{
				notifyAll();

			}catch(Exception ex){

				ex.printStackTrace();
			}
		}
		Log.d(TAG,"CorverPic loadDone ");

	}


	

	/**
     * Stop get meta data thumbnail
     */
	public void stopThumbnail() {
		Log.d(TAG,"CorverPic call ");
		synchronized(this) {
			 while (thumbLoadStart) {
                try {
					
					mtkPlayer.reset();
				
					cancelLoad = true;
					
                    wait();
                } catch (Exception e) {
                	e.printStackTrace();
                }
            }
			
		}
		Log.d(TAG,"CorverPic done");
	}
}
