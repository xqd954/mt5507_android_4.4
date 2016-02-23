package com.mediatek.mmpcm.videoimpl;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.UnknownHostException;


import jcifs.smb.SmbException;
import android.media.TimedText;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.graphics.Rect;

import com.mediatek.mmp.*;
import com.mediatek.mmp.util.*;
import com.mediatek.mmp.MtkMediaPlayer.*;
import com.mediatek.mmp.MtkMediaPlayer;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnSeekCompleteListener;


import com.mediatek.mmpcm.UIMediaPlayer;

import com.mediatek.mmpcm.fileimpl.VideoFile;
import com.mediatek.netcm.dlna.DLNADataSource;
import com.mediatek.netcm.dlna.DLNAManager;
import com.mediatek.netcm.samba.SambaManager;

public class VideoPlayer extends UIMediaPlayer{
	public static final int MEDIA_INFO_METADATA_COMPLETE = 0;
	public static final int MEDIA_INFO_START_INVALID_STATE = -1;
	public static final int MEDIA_INFO_PAUSE_INVALID_STATE = -2;
	public static final int MEDIA_INFO_STOP_INVALID_STATE = -3;
	public static final int MEDIA_INFO_SEEK_INVALID_STATE = -4;
	public static final int MEDIA_INFO_NOT_SUPPORT = -5;
	public static final int MEDIA_INFO_DATA_PREPAREED_STATE = -100;
	public static final int MEDIA_INFO_VIDEO_SEEK_COMPLETEED = 1003;
	private static final String TAG = "VideoPlayer";
	private SurfaceHolder mSurfaceHolder = null;
	private String mCurrentPath = null;
	private int mPlayStatus = VideoConst.PLAY_STATUS_INITED;
	private boolean isEnd = false;
	private static int mPlayerMode = VideoConst.PLAYER_MODE_MMP;
	private VideoFile mVideoFile;
	//private InputStream mInputStream;
	private Object mPreparedListener;
	private Object mErrorListener;
	private Object mBufferingUpdateListener;
	private Object mCompletionListener;
	private Object mInfoListener;
	private Object mSeekCompleteListener;
	private Object mTimedTextListener;
	private Object mVideoSizeChangedListener;
	private Rect videoRect;
	private int mTmpPlayStatus = VideoConst.PLAY_STATUS_INITED;

	@SuppressWarnings("deprecation")
	public VideoPlayer(SurfaceView surfaceview, int playerMode) {
		super(playerMode);
		Log.d(TAG, "enter VideoPlayer init, playermode:" + playerMode);
		if (mSurfaceHolder == null) {
			Log.d(TAG, "enter VideoPlayer init,mSurfaceHolder ==null");
		}
		mSurfaceHolder = surfaceview.getHolder();
		mSurfaceHolder.addCallback(mSHCallback);
		mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		videoRect = null;
	}
	
	   public VideoPlayer(int playerMode) {
	        super(playerMode);
	        mPlayerMode = playerMode;
			mTmpPlayStatus = VideoConst.PLAY_STATUS_INITED;
	    }
	
	/*public void setRect(Rect rect){
		videoRect = rect;
	}*/

	public void setVideoDataSource(String path) {
		Log.d(TAG, "enter setDataSource, path:" + path);
		mCurrentPath = path;
		notifyInfo(MEDIA_INFO_DATA_PREPAREED_STATE);
		openVideo();
		Log.d(TAG, "leave setDataSource, " + "mPlayStatus:" + mPlayStatus);
	}

	public void startVideo(){
		Log.d(TAG, "enter startVideo, mCurrentPath:" + mCurrentPath
				+ "mPlayStatus:" + mPlayStatus);
		if (mPlayStatus >= VideoConst.PLAY_STATUS_PREPARED && mPlayStatus < VideoConst.PLAY_STATUS_STOPPED) {
			try {
				start();
				mPlayStatus = VideoConst.PLAY_STATUS_STARTED;
			} catch (IllegalStateException e) {
				e.printStackTrace();
				throw new IllegalStateException(e);
			}
		}
		Log.d(TAG, "leave startVideo, " + "mPlayStatus:" + mPlayStatus
				+ "getCurrentPosition:" + getCurrentPosition() + "isPlaying:"
				+ isPlaying());
	}

	public void pauseVideo(){
		Log.d(TAG, "enter pauseVideo, mCurrentPath:" + mCurrentPath
				+ "mPlayStatus:" + mPlayStatus);
		if (isInPlaybackState()) {
			if (isPlaying()) {
				try {
					pause();
					mPlayStatus = VideoConst.PLAY_STATUS_PAUSED;
				} catch (IllegalStateException e) {
					e.printStackTrace();
					throw new IllegalStateException(e);
				}
			}
		}
		Log.d(TAG, "leave pauseVideo, " + "mPlayStatus:" + mPlayStatus);
	}

	public void stopVideo(){
		Log.d(TAG, "enter stopVideo, mCurrentPath:" + mCurrentPath
				+ "mPlayStatus:" + mPlayStatus);
		if(isInPlaybackState()){
			try {
				stop();
				mPlayStatus = VideoConst.PLAY_STATUS_STOPPED;
			} catch (IllegalStateException e) {
				e.printStackTrace();
				throw new IllegalStateException(e);
			}
		}
		Log.d(TAG, "leave stopVideo, " + "mPlayStatus:" + mPlayStatus);
	}

	/*@Override
	public void step(int amount) {
		super.step(amount);
		//mPlayStatus = VideoConst.PLAY_STATUS_STEP;
	}*/

	public int setPlayModeEx(PlayerSpeed speed, int playStatus) {
		int result = setPlayModeEx(speed);
		if(result < 0 && result != UIMediaPlayer.IMTK_PB_ERROR_CODE_NEW_TRICK){
			if(result == UIMediaPlayer.IMTK_PB_BUFFER_NOT_ENOUGH){
				throw new IllegalStateException("BUFFER NOT ENOUGH");
			}else{

				throw new RuntimeException("MEDIA_INFO_NOT_SUPPORT");

			}
			
		}
		mPlayStatus = playStatus;
		return result;
	}

	public void completVideo() {
		Log.d(TAG, "enter completVideo, " + "mPlayStatus:" + mPlayStatus);
		isEnd = true;
		notifyInfo(MEDIA_INFO_METADATA_COMPLETE);
//		releaseVideo();
		Log.d(TAG, "leave completVideo, " + "mPlayStatus:" + mPlayStatus);
	}

	public void notifyInfo(int what) {
		if (null != mInfoListener) {
			if(mInfoListener instanceof OnInfoListener){
				((OnInfoListener)mInfoListener).onInfo(this.getPlayer(), what, what);
			}else{
				((MtkMediaPlayer.OnInfoListener)mInfoListener).onInfo(this.getMtkPlayer(), what, what);
			}
		}
	}

	public void releaseVideo() {
		Log.d(TAG, "enter releaseVideo, " + "mPlayStatus:" + mPlayStatus);
		release();
		closeStream();
		mPreparedListener = null;
		mErrorListener = null;
		mBufferingUpdateListener = null;
		mCompletionListener = null;
		mInfoListener = null;
		mSeekCompleteListener = null;
		mTimedTextListener = null;
		mVideoSizeChangedListener = null;
		mCurrentPath = null;
		//setDisplay = false;
		mPlayStatus = VideoConst.PLAY_STATUS_INITED;
		Log.d(TAG, "leave releaseVideo, " + "mPlayStatus:" + mPlayStatus);
	}

	public void resetVideo() {
		reset();
		mPlayStatus = VideoConst.PLAY_STATUS_INITED;
		Log.d(TAG, "leave resetVideo, " + "mPlayStatus:" + mPlayStatus);
	}

	public String getCurrentPath() {
		return mCurrentPath;
	}

	public int getPlayStatus() {
		return mPlayStatus;
	}

	public void setPlaySourceMode(int mode) {
		mPlayerMode = mode;
	}

	public int getPlaySourceMode() {
		return mPlayerMode;
	}

	public boolean isInPlaybackState() {
		return (mPlayStatus >= VideoConst.PLAY_STATUS_STARTED && mPlayStatus < VideoConst.PLAY_STATUS_STOPPED);
	}


	public void seekTo(int arg0) throws IllegalStateException {
		
		Log.d(TAG,"seekTo arg0 = "+ arg0 +"mPlayStatus = "+mPlayStatus +"curtime ="+System.currentTimeMillis());
		if(mPlayStatus >= VideoConst.PLAY_STATUS_STARTED && 
			mPlayStatus < VideoConst.PLAY_STATUS_SEEKING){

			try{
				super.seekTo(arg0);
				mTmpPlayStatus = mPlayStatus;
				mPlayStatus = VideoConst.PLAY_STATUS_SEEKING;
			
			}catch(IllegalStateException ex){
				Log.d(TAG,"seekTo ex = "+ex);
				throw ex;

			}

		}
		
		
	}

	
	public int getProgress() {

		int position = 0;
		if (isInPlaybackState()) {
			position = super.getCurrentPosition();	
		}
		Log.d(TAG,"getProgress ~position = "+position);
		return position;
	}

	public int getBytePosition() {
	
		int position = 0;
		if (isInPlaybackState()) {
			position = super.getCurrentBytePosition();	
			
		} 
		Log.e(TAG,"getBytePosition pos = " + position);
		return position;

	}



	
	public void step(int amount)throws IllegalStateException{
		
		Log.d(TAG,"step amount = "+ amount + "mPlayStatus = "+mPlayStatus + "curtime = "+System.currentTimeMillis());
		if(mPlayStatus == VideoConst.PLAY_STATUS_PAUSED ){

			try{
				super.step(amount);
				mTmpPlayStatus = mPlayStatus;
				mPlayStatus = VideoConst.PLAY_STATUS_STEP;
	
			}catch(IllegalStateException ex){
				
				Log.d(TAG,"step ex = "+ex);

				throw ex;
				
			}
				
		}
		
		  
	}

	public boolean canDoSeek(Object speed) {
		boolean res = false;
		if(isInPlaybackState()){
			
			res = super.canDoSeek(speed);
			
		}
		return res;
	}

	public boolean canDoTrick(Object speed) {
		boolean res = false;
		if(isInPlaybackState()){
			
			res = super.canDoTrick(speed);
			
		}
		return res;
	}

	public int getDuration() {
        Log.d(TAG,"getDuration ~");
        int dur = 0;
       	if(mPlayStatus >= VideoConst.PLAY_STATUS_PREPARED){
			dur = super.getDuration();
		}
        Log.d(TAG,"getDuration ~dur = " + dur);
        return dur;
	}

	private void openVideo() {
		Log.d(TAG, "enter openVideo, " + "mPlayStatus:" + mPlayStatus);
		if (mCurrentPath == null ) {
			Log.e("TAG","openVideo mCurrentPath:"+mCurrentPath+ "mSurfaceHolder:  "+ (mSurfaceHolder == null));
			return;
		}
		resetVideo();
		if (mPlayerMode == VideoConst.PLAYER_MODE_MMP) {

		        setOnPreparedListener(mOnPreparedListener);
			setOnCompletionListener(mCompletionListener);
			setOnBufferingUpdateListener(mBufferingUpdateListener);
			setOnErrorListener(mOnErrorListener);
			setOnInfoListener(mOnInfoListener);
			setOnVideoSizeChangedListener(mVideoSizeChangedListener);
			setOnSeekCompleteListener(mOnSeekCompletionListener);
			setOnTimedTextListener(mTimedTextListener);
		}else{
			setOnPreparedListener(mtkOnPreparedListener);
			setOnCompletionListener(mCompletionListener);
			setOnBufferingUpdateListener(mBufferingUpdateListener);
			setOnErrorListener(mtkOnErrorListener);
			setOnInfoListener(mtkOnInfoListener);
			setOnVideoSizeChangedListener(mVideoSizeChangedListener);
			setOnSeekCompleteListener(mtkSeekCompletionListener);
			setOnTimedTextListener(mTimedTextListener);


		}
		try {
            //cmpb
            setPlayerType(6);
            Log.d(TAG, "video setPlayerType mCurrentPath = "+ mCurrentPath + "mPlayerMode = "+ mPlayerMode);
			setDataSource(mCurrentPath);
			if(mPlayerMode == VideoConst.PLAYER_MODE_MMP || mPlayerMode == VideoConst.PLAYER_MODE_DLNA){
				setSubtitleDataSource(mCurrentPath);
			}
			setPlayerRole(PlayerRole.ROLE_VIDEO_PLAYBACK);
			
	        if (mCurrentPath.toLowerCase().endsWith(".pvr")) {
	            setSvctxPath("ANDR_PVR");
	        } 
	        else if (mCurrentPath.toLowerCase().endsWith(".tshift")) {
	            setSvctxPath("ANDR_TIMESHIFT");
	        } 
			setScreenOnWhilePlaying(true);
			/*if( mSurfaceHolder!=null){

				Log.d(TAG, "enter openVideo~ mSurfaceHolder!=null mSurfaceHolder.getSurfaceFrame()= "+mSurfaceHolder.getSurfaceFrame() +"videoRect = "+videoRect );
				if(videoRect != null){
					setVideoRect(videoRect);
				}else{
					//setDisplay(mSurfaceHolder); 
					setVideoRect(mSurfaceHolder.getSurfaceFrame());

				}
				//setDisplay = true;
			}*/
			prepareAsync();
			mPlayStatus = VideoConst.PLAY_STATUS_PREPAREING;
		}catch(Exception e) {
			mPlayStatus = VideoConst.PLAY_STATUS_ERROR;

			e.printStackTrace();
			Log.d(TAG, "enter openVideo, "+"file not support error to ap " +"mErrorListener "+mErrorListener);
            if (null != mErrorListener) {

				if(mErrorListener instanceof MtkMediaPlayer.OnErrorListener){
					((MtkMediaPlayer.OnErrorListener)mErrorListener).onError(getMtkPlayer(), VideoManager.MEDIA_ERROR_OPEN_FILE_FAILED, 0);
				}else{
					((OnErrorListener)mErrorListener).onError(getPlayer(), VideoManager.MEDIA_ERROR_OPEN_FILE_FAILED, 0);

				}
			}


		}

		

		
		Log.d(TAG, "leave openVideo, " + "mPlayStatus:" + mPlayStatus
				+ "getCurrentPosition:" + getCurrentPosition());
	}

	public void setPreparedListener(Object l) {
		mPreparedListener = l;
	}

	public void setErrorListener(Object l) {
		mErrorListener = l;
	}

	public void setBufferingUpdateListener(Object l) {
		mBufferingUpdateListener = l;
	}

	public void setCompletionListener(Object l) {
		mCompletionListener = l;
	}

	public void setVideoSizeChangedListener(Object l) {
		mVideoSizeChangedListener = l;
	}

	public void setInfoListener(Object l) {
		mInfoListener = l;
	}

	public void setSeekCompleteListener(Object l) {
		mSeekCompleteListener = l;
	}

	public void setTimedTextListener(Object l) {
		mTimedTextListener = l;
	}
	//private boolean setDisplay = false;
	private OnPreparedListener mOnPreparedListener = new OnPreparedListener() {

		public void onPrepared(MediaPlayer mp) {
			handlePrepare();

				
		}
	};

	private OnErrorListener mOnErrorListener = new OnErrorListener() {

		public boolean onError(MediaPlayer mp, int what, int extra) {
			return handleErr(what,extra);
		}
	};
	private OnInfoListener mOnInfoListener = new OnInfoListener() {

		public boolean onInfo(MediaPlayer mp, int what, int extra) {
			Log.d(TAG, "enter onInfo, " + "mPlayStatus:" + mPlayStatus
					+ "what:" + what);
			return handleInfo(what,extra);
		}
	};

	
	private OnSeekCompleteListener mOnSeekCompletionListener = new OnSeekCompleteListener() {
		public void onSeekComplete(MediaPlayer arg0) {
			handleSeekComplete();
		}
	};



	private MtkMediaPlayer.OnPreparedListener mtkOnPreparedListener = new MtkMediaPlayer.OnPreparedListener() {
	
		public void onPrepared(MtkMediaPlayer mp) {
			handlePrepare();

				
		}
	};

	private MtkMediaPlayer.OnErrorListener mtkOnErrorListener = new MtkMediaPlayer.OnErrorListener() {

		public boolean onError(MtkMediaPlayer mp, int what, int extra) {
			return handleErr(what,extra);
		}
	};
	private MtkMediaPlayer.OnInfoListener mtkOnInfoListener = new MtkMediaPlayer.OnInfoListener() {

		public boolean onInfo(MtkMediaPlayer mp, int what, int extra) {
			Log.d(TAG, "enter onInfo, " + "mPlayStatus:" + mPlayStatus
					+ "what:" + what);
			return handleInfo(what,extra);
		}
	};

	
	private MtkMediaPlayer.OnSeekCompleteListener mtkSeekCompletionListener = new MtkMediaPlayer.OnSeekCompleteListener() {
		 
		public void onSeekComplete(MtkMediaPlayer arg0) {
			handleSeekComplete();
		}
	};


	private void handlePrepare(){
			Log.d(TAG, "enter onPrepared~~~ videoRect ="+videoRect );
			if( /*!setDisplay&& */ mSurfaceHolder!=null){

				Log.d(TAG, "enter onPrepared~~~ mSurfaceHolder!=null mSurfaceHolder.getSurfaceFrame()= "+mSurfaceHolder.getSurfaceFrame() );
				/*if(videoRect != null){
					setVideoRect(videoRect);
					}else{*/
						setDisplay(mSurfaceHolder); 

					//}
					//setDisplay = true;
				}
			Log.d(TAG, "enter onPrepared~~~ " );
			try{
					Log.d(TAG, "enter onPrepared, " + "mPlayStatus:" + mPlayStatus
					+ "getCurrentPosition:" + getCurrentPosition()
					+ "isPlaying:" + isPlaying());
					mPlayStatus = VideoConst.PLAY_STATUS_PREPARED;
					startVideo();
					if (null != mPreparedListener) {
					if(mPreparedListener instanceof MtkMediaPlayer.OnPreparedListener){
						((MtkMediaPlayer.OnPreparedListener)mPreparedListener).onPrepared(getMtkPlayer());

					}else{
						((OnPreparedListener)mPreparedListener).onPrepared(getPlayer());

					}
					
					}
				}catch(Exception e){
					e.printStackTrace();
				}
		


	}


	
	private void handleSeekComplete(){
		Log.d(TAG,"handleSeekComplete mTmpPlayStatus = "+mTmpPlayStatus+"mPlayStatusb ="+mPlayStatus +"curtime = "+System.currentTimeMillis());

		if(mTmpPlayStatus != VideoConst.PLAY_STATUS_INITED ){
			mPlayStatus = mTmpPlayStatus;
			mTmpPlayStatus = VideoConst.PLAY_STATUS_INITED;
		}else{
			mPlayStatus = VideoConst.PLAY_STATUS_STARTED;
			mTmpPlayStatus = VideoConst.PLAY_STATUS_INITED;


		}
		
		if (null != mSeekCompleteListener) {
	
			if(mSeekCompleteListener instanceof MtkMediaPlayer.OnSeekCompleteListener){
				((MtkMediaPlayer.OnSeekCompleteListener)mSeekCompleteListener).onSeekComplete(getMtkPlayer());
			}else{
				((OnSeekCompleteListener)mSeekCompleteListener).onSeekComplete(getPlayer());

			}
		}

		
		

	}


	private boolean handleErr(int what, int extra){
		mPlayStatus = VideoConst.PLAY_STATUS_ERROR;
		Log.d(TAG, "enter onError, " + "mPlayStatus:" + mPlayStatus);
		if (null != mErrorListener) {

			if(mErrorListener instanceof MtkMediaPlayer.OnErrorListener){
				return ((MtkMediaPlayer.OnErrorListener)mErrorListener).onError(getMtkPlayer(), what, extra);
			}else{
				return ((OnErrorListener)mErrorListener).onError(getPlayer(), what, extra);

			}
		}
		return false;
	}


	private boolean handleInfo(int what, int extra){
		switch (what) {
		case MEDIA_INFO_VIDEO_SEEK_COMPLETEED:
			Log.d(TAG,"enter onInfo:MEDIA_INFO_VIDEO_SEEK_COMPLETEED mTmpPlayStatus = "+mTmpPlayStatus +"mPlayStatus = "+mPlayStatus +"curtime = "+System.currentTimeMillis());
			if(mTmpPlayStatus != VideoConst.PLAY_STATUS_INITED ){
				mPlayStatus = mTmpPlayStatus;
				mTmpPlayStatus = VideoConst.PLAY_STATUS_INITED;
			}else{

				mPlayStatus = VideoConst.PLAY_STATUS_STARTED;
				mTmpPlayStatus = VideoConst.PLAY_STATUS_INITED;


			}
			break;
		default:
			break;
		}
		if (null != mInfoListener) {
			if(mInfoListener instanceof MtkMediaPlayer.OnInfoListener){
				return ((MtkMediaPlayer.OnInfoListener)mInfoListener).onInfo(getMtkPlayer(), what, extra);
			}else{
				return ((OnInfoListener)mInfoListener).onInfo(getPlayer(), what, extra);

			}
		}
		return false;


	}

	SurfaceHolder.Callback mSHCallback = new SurfaceHolder.Callback() {

		public void surfaceCreated(SurfaceHolder holder) {
			Log.d(TAG, "enter surfaceCreated, " + "mPlayStatus:" + mPlayStatus +"videoRect="+videoRect);
			try{
			/*if(videoRect != null){
				setVideoRect(videoRect);
			}else{*/
				setDisplay(mSurfaceHolder); 

			//}
			}catch(Exception ex){

			}
			//setDisplay = true;
			Log.d(TAG,"SurfaceView ~~:"+mSurfaceHolder.getSurfaceFrame());
		}

		public void surfaceChanged(SurfaceHolder holder, int format, int width,
				int height) {
			Log.d(TAG, "enter surfaceChanged, " + "width:" + width + "height:"
					+ height);
		}

		public void surfaceDestroyed(SurfaceHolder holder) {
			Log.d(TAG, "enter surfaceDestroyed, ");
			mSurfaceHolder.removeCallback(mSHCallback);
			mSurfaceHolder = null;
		}
	};

	

}
