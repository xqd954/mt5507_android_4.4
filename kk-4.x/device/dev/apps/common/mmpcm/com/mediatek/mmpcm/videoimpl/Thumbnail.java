package com.mediatek.mmpcm.videoimpl;

import java.nio.ByteBuffer;

import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Matrix;

import com.mediatek.mmpcm.UIMediaPlayer;

import com.mediatek.mmp.*;
import com.mediatek.mmp.util.*;
import android.media.*;
import android.provider.MediaStore;
import android.provider.MediaStore.Video.Thumbnails;
import android.util.Log;

import com.mediatek.mmpcm.Info;
import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.MmpTool;
import com.mediatek.mmpcm.fileimpl.FileConst;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;

public class Thumbnail extends Info {
	private static final String TAG = "Thumbnail";
    private UIMediaPlayer mtkPlayer;
    private boolean thumbLoadStart = false;
    private static Thumbnail vThumb = null;
	private boolean cancelLoad = false;

    private Thumbnail(int srcType) {
		mSrcType = srcType;
        mtkPlayer = new UIMediaPlayer(srcType);
    }

    private Thumbnail() {
	 	mSrcType = FileConst.SRC_USB;
        mtkPlayer = new UIMediaPlayer(mSrcType);
    }

    public static synchronized Thumbnail getInstance(int srcType) {
        if (vThumb == null) {
            vThumb = new Thumbnail(srcType);
        }
        return vThumb;
    }

    public static synchronized Thumbnail getInstance() {
        if (vThumb == null) {
            vThumb = new Thumbnail();
        }
        return vThumb;
    }

    /**
     * get video thumbnail bitmap
     */
    public Bitmap getVideoThumbnail(int srcType, String filepath, int width,
            int height) throws IllegalArgumentException {
		

        Log.d(TAG,"filepath = " + filepath);

        if (filepath == null) {
            throw new IllegalArgumentException("empty filepath!");
        }

		if(srcType != mSrcType){
			mSrcType = srcType;
			mtkPlayer.release();
			mtkPlayer = new UIMediaPlayer(srcType);
		}

		if (srcType == FileConst.SRC_USB) {
	        mCacheMetaData = getMediaInfo(filepath);
	        return  getVideoThumbnail(filepath, 120, 120, Thumbnails.MINI_KIND);
		}


		 synchronized(this) {
			 cancelLoad = false;
			 thumbLoadStart = true;
		}

        if (mtkPlayer != null ) {

            mFilePath = filepath;
            
            try {

              
                mtkPlayer.reset();
            
                mtkPlayer.setDataSource(mFilePath);
          
              
               	mtkPlayer.prepare();
				Log.d(TAG,"prepare done " );

            } catch (Exception e) {
                MmpTool.LOG_INFO(" getVideoThumbnail() 109 line :"
                        + e.toString());
                mtkPlayer.closeStream();
				loadDone();
                return null;
            }
		 
		 

	        ThumbnailInfo thInfo = new ThumbnailInfo(ThumbnailInfo.RGB_D565,
	                VideoConst.THUMBNAIL_WIDTH, VideoConst.THUMBNAIL_HEIGTH);
	        byte[] thBuffer = null;
		 	if (!cancelLoad) {
				
				Log.d(TAG,"start getThumbnailInfo " );
	            thBuffer = mtkPlayer.getThumbnailInfo(thInfo);
				
			}
			
	   
	        
	        if (thBuffer == null) {
	            mtkPlayer.closeStream();
				loadDone();
	            return null;
	        } else {
	            Bitmap bitmap = Bitmap.createBitmap(VideoConst.THUMBNAIL_WIDTH,
	                    VideoConst.THUMBNAIL_HEIGTH, Bitmap.Config.RGB_565);

	            ByteBuffer buffer = ByteBuffer.wrap(thBuffer);
	            bitmap.copyPixelsFromBuffer(buffer);
	            mtkPlayer.closeStream();

	            int bitWidth = bitmap.getWidth();
	            int bitHeight = bitmap.getHeight() - 2;
	            float scaleWidth = width / (float) bitWidth;
	            float scaleHeight = height / (float) bitHeight;
	            Matrix matrix = new Matrix();
	            matrix.postScale(scaleWidth, scaleHeight);
	            bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitWidth, bitHeight,
	                    matrix, true);
				loadDone();
	            return bitmap;
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
		Log.d(TAG,"Thumbnail loadDone ");

    }

    private Bitmap getVideoThumbnail(String videoPath, int width , int height, int kind){  
        Bitmap bitmap = null;  
        bitmap = ThumbnailUtils.createVideoThumbnail(videoPath, kind);  
        bitmap = ThumbnailUtils.extractThumbnail(bitmap, width, height, ThumbnailUtils.OPTIONS_RECYCLE_INPUT);  
        return bitmap;  
       }  
    
    

    /**
     * stop thumbnail
     */
    public void stopThumbnail() {
		Log.d(TAG,"stopThumbnail call ");

		synchronized(this) {
			 while (thumbLoadStart) {
                try {
					Log.d(TAG,"stopThumbnail  cancelLoad =  "+ cancelLoad );
					mtkPlayer.reset();
					cancelLoad = true;
                    wait();
                } catch (Exception e) {
                	e.printStackTrace();
                }
            }
			
		}
		Log.d(TAG,"stopThumbnail done");
    }

    public boolean isLoadThumanil() {
        synchronized (this) {
            return thumbLoadStart;
        }
    }

}
