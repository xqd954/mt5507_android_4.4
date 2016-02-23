package com.mediatek.mmpcm.videoimpl;

import com.mediatek.mmpcm.MmpTool;
import android.util.Log;
import android.view.SurfaceView;

import com.mediatek.mmp.*;
import com.mediatek.mmp.util.*;
import com.mediatek.mmp.MtkMediaPlayer.*;

import android.media.MediaPlayer.DataSourceType;
import android.media.MediaPlayer.DivxDrmInfoType;
import android.media.MediaPlayer.PlayerSpeed;
import android.media.MediaPlayer.ABRpeatType;
import android.media.MediaPlayer;

import com.mediatek.mmpcm.UIMediaPlayer;

import com.mediatek.mmpcm.MetaData;
import com.mediatek.mmpcm.fileimpl.VideoFile;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.mmcimpl.PlayList;
import com.mediatek.netcm.dlna.DLNADataSource;
import com.mediatek.netcm.dlna.DLNAManager;
import android.graphics.Rect;

import android.media.SubtitleTrackInfo;
import android.media.AudioTrackInfo;
import android.media.SubtitleAttr;

public class VideoManager {
	private static final String TAG = "VideoManager";
	/**
	 * Unspecified media player info.
	 */
	public static final int MEDIA_INFO_UNKNOWN = MediaPlayer.MEDIA_INFO_UNKNOWN;

	/**
	 * The video is too complex for the decoder: it can't decode frames fast
	 * enough. Possibly only the audio plays fine at this stage.
	 */
	public static final int MEDIA_INFO_VIDEO_TRACK_LAGGING = MediaPlayer.MEDIA_INFO_VIDEO_TRACK_LAGGING;

	/**
	 * CHMtkMediaPlayer is temporarily pausing playback internally in order to
	 * buffer more data.
	 */
	public static final int MEDIA_INFO_BUFFERING_START = MediaPlayer.MEDIA_INFO_BUFFERING_START;

	/**
	 * CHMtkMediaPlayer is resuming playback after filling buffers.
	 */
	public static final int MEDIA_INFO_BUFFERING_END = MediaPlayer.MEDIA_INFO_BUFFERING_END;

	/**
	 * Bad interleaving means that a media has been improperly interleaved or
	 * not interleaved at all, e.g has all the video samples first then all the
	 * audio ones. Video is playing but a lot of disk seeks may be happening.
	 */
	public static final int MEDIA_INFO_BAD_INTERLEAVING = MediaPlayer.MEDIA_INFO_BAD_INTERLEAVING;

	/** The media cannot be seeked (e.g live stream) */
	public static final int MEDIA_INFO_NOT_SEEKABLE = MediaPlayer.MEDIA_INFO_NOT_SEEKABLE;

	/** A new set of metadata is available. */
	public static final int MEDIA_INFO_METADATA_UPDATE = MediaPlayer.MEDIA_INFO_METADATA_UPDATE;

	public static final int MEDIA_INFO_METADATA_COMPLETE = VideoPlayer.MEDIA_INFO_METADATA_COMPLETE;

	public static final int MEDIA_INFO_VIDEO_ENCODE_FORMAT_UNSUPPORT = MediaPlayer.MEDIA_INFO_VIDEO_ENCODE_FORMAT_UNSUPPORT;
	public static final int MEDIA_INFO_AUDIO_ENCODE_FORMAT_UNSUPPORT = MediaPlayer.MEDIA_INFO_AUDIO_ENCODE_FORMAT_UNSUPPORT;
	public static final int MEDIA_ERROR_FILE_NOT_SUPPORT = MediaPlayer.MEDIA_ERROR_FILE_NOT_SUPPORT;
	public static final int MEDIA_ERROR_FILE_CORRUPT = MediaPlayer.MEDIA_ERROR_FILE_CORRUPT;
	public static final int MEDIA_ERROR_OPEN_FILE_FAILED = MediaPlayer.MEDIA_ERROR_OPEN_FILE_FAILED;
	public static final int MEDIA_INFO_3D_VIDEO_PLAYED = MediaPlayer.MEDIA_INFO_3D_VIDEO_PLAYED;

	public static final int MEDIA_INFO_POSITION_UPDATE = MediaPlayer.MEDIA_INFO_POSITION_UPDATE;

	public static final int MEDIA_INFO_START_INVALID_STATE = VideoPlayer.MEDIA_INFO_START_INVALID_STATE;
	public static final int MEDIA_INFO_PAUSE_INVALID_STATE = VideoPlayer.MEDIA_INFO_PAUSE_INVALID_STATE;
	public static final int MEDIA_INFO_STOP_INVALID_STATE = VideoPlayer.MEDIA_INFO_STOP_INVALID_STATE;
	public static final int MEDIA_INFO_SEEK_INVALID_STATE = VideoPlayer.MEDIA_INFO_SEEK_INVALID_STATE;
	public static final int MEDIA_INFO_NOT_SUPPORT = VideoPlayer.MEDIA_INFO_NOT_SUPPORT;
	public static final int MEDIA_INFO_DATA_PREPAREED_STATE = VideoPlayer.MEDIA_INFO_DATA_PREPAREED_STATE;
	public static final int MEDIA_INFO_AUDIO_ONLY_SERVICE = MediaPlayer.MEDIA_INFO_AUDIO_ONLY_SERVICE;
	public static final int MEDIA_INFO_VIDEO_ONLY_SERVICE = -5009;
	public static final int MTK_MEDIA_INFO_SCRAMBLED_AUDIO_VIDEO_SERVICE		= -5010;
	public static final int MTK_MEDIA_INFO_SCRAMBLED_AUDIO_CLEAR_VIDEO_SERVICE	= -5011;
	public static final int MTK_MEDIA_INFO_SCRAMBLED_AUDIO_NO_VIDEO_SERVICE 	= -5012;
	public static final int MTK_MEDIA_INFO_SCRAMBLED_VIDEO_CLEAR_AUDIO_SERVICE	= -5013;
	public static final int MTK_MEDIA_INFO_SCRAMBLED_VIDEO_NO_AUDIO_SERVICE 	= -5014;
        //public static final int MTK_MEDIA_INFO_PLAY_DONE     = 1006;
        public static final int MEDIA_INFO_VIDEO_RENDERING_START     = 3;

	
	public static final int  MEDIA_INFO_ON_REPLAY = MediaPlayer.MEDIA_INFO_ON_REPLAY;
	
	private static VideoManager mVideoManager = null;
	private static VideoPlayer mPlayer = null;
	private static PlayList mPlayList = null;
	private static VideoComset mComset = null;
	private VideoFile mVideoFile;
	private boolean mPreviewMode = false;
	private int mSpeedStep;
	private PlayerSpeed mPlayerSpeed = PlayerSpeed.SPEED_1X;
	private int subTitleNum = -1;

	private VideoManager() {

	}
    
	/**
	 * 
	 * @param context
	 * @param surfaceview
	 * @param playerMode
	 *            VideoConst.PLAYER_MODE_MMP or VideoConst.PLAYER_MODE_NET
	 * @return
	 */
	public static synchronized VideoManager getInstance(
			SurfaceView surfaceview, int playerMode) {

		if (mVideoManager == null) {
			mVideoManager = new VideoManager(surfaceview, playerMode);
		}
		return mVideoManager;
	}

	/*public void setVideoRect(Rect rect){
		if(mPlayer != null){
			mPlayer.setRect(rect);
		}
	}*/
	
	public static synchronized VideoManager getInstance(int playerMode) {

		if (mVideoManager == null) {
			mVideoManager = new VideoManager(playerMode);
		}
		return mVideoManager;
	}

	public static VideoManager getInstance() {
		return getInstance(VideoConst.PLAYER_MODE_MMP);
	}

	private VideoManager(SurfaceView surfaceview, int playerMode) {
		Log.d(TAG, "VideoManager:");
		if (surfaceview != null) {
			mPlayer = new VideoPlayer(surfaceview, playerMode);
			Log.d(TAG, "new mPlayer:");
		}
		if (playerMode != VideoConst.PLAYER_MODE_HTTP) {
			mPlayList = PlayList.getPlayList();
		}
		if (mComset == null) {
			mComset = new VideoComset();
		}
	}
	
	   public VideoManager(int playerMode) {
	        Log.d(TAG, "VideoManager:");
	            mPlayer = new VideoPlayer(playerMode);
//	            mPlayer.setPlayMode(playerMode);
	            Log.d(TAG, "new mPlayer:");
	        if (playerMode != VideoConst.PLAYER_MODE_HTTP) {
	            mPlayList = PlayList.getPlayList();
	        }
	        if (mComset == null) {
	            mComset = new VideoComset();
	        }
	    }

	public void onRelease() {
		Log.d(TAG, "onRelease:");
		if (null != mPlayer) {
			mPlayer.releaseVideo();
		}
		mPlayer = null;
		mComset = null;
		mVideoManager = null;
	}

	public VideoComset getComset() {
		return mComset;
	}

	// //add by hs_binyan
	/**
	 * Register a callback to be invoked when the media source is ready for
	 * playback.
	 * 
	 * @param listener
	 *            the callback that will be run
	 */
	public void setOnPreparedListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setPreparedListener(listener);
		}
	}

	/**
	 * Register a callback to be invoked when the end of a media source has been
	 * reached during playback.
	 * 
	 * @param listener
	 *            the callback that will be run
	 */
	public void setOnCompletionListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setCompletionListener(listener);
		}
	}

	/**
	 * Register a callback to be invoked when the status of a network stream's
	 * buffer has changed.
	 * 
	 * @param listener
	 *            the callback that will be run.
	 */
	public void setOnBufferingUpdateListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setBufferingUpdateListener(listener);
		}
	}

	/**
	 * Register a callback to be invoked when a seek operation has been
	 * completed.
	 * 
	 * @param listener
	 *            the callback that will be run
	 */
	public void setOnSeekCompleteListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setSeekCompleteListener(listener);
		}
	}

	/**
	 * Register a callback to be invoked when the video size is known or
	 * updated.
	 * 
	 * @param listener
	 *            the callback that will be run
	 */
	public void setOnVideoSizeChangedListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setVideoSizeChangedListener(listener);
		}
	}

	/**
	 * Register a callback to be invoked when a timed text is available for
	 * display.
	 * 
	 * @param listener
	 *            the callback that will be run {@hide}
	 */
	public void setOnTimedTextListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setTimedTextListener(listener);
		}
	}

	/**
	 * Register a callback to be invoked when an info/warning is available.
	 * 
	 * @param listener
	 *            the callback that will be run
	 */
	public void setOnInfoListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setInfoListener(listener);
		}
	}

	/**
	 * Register a callback to be invoked when an error has happened during an
	 * asynchronous operation.
	 * 
	 * @param listener
	 *            the callback that will be run
	 */
	public void setOnErrorListener(Object listener) {
		if (null != mPlayer) {
			mPlayer.setErrorListener(listener);
		}
	}

	public void setDataSource(String path) {
		if (null != mPlayer) {
			Log.d(TAG, "setDataSource: null != mPlayer path" + path);
			mPlayer.setVideoDataSource(path);
			subTitleNum = -1;
			mSpeedStep = 1;
			mMetaData = null;

		}
	}

	/**
	 * play or pause video
	 */
	public void startVideo() {
	   
		if (null != mPlayer) {
		   	try{
	            int platstatus = mPlayer.getPlayStatus();
	            Log.d(TAG, "startVideo start:" + platstatus);
	            if ( platstatus == VideoConst.PLAY_STATUS_PAUSED){
					mPlayer.startVideo();
				}else if(platstatus == VideoConst.PLAY_STATUS_FF
	                || platstatus == VideoConst.PLAY_STATUS_FR
	                || platstatus == VideoConst.PLAY_STATUS_SF) {
	                  setNormalSpeed();
	                  mPlayer.startVideo();
	            } else if (platstatus == VideoConst.PLAY_STATUS_STOPPED) {
	                setDataSource(mPlayer.getCurrentPath());
	            }else if (platstatus == VideoConst.PLAY_STATUS_STARTED
	                    || platstatus == VideoConst.PLAY_STATUS_PREPARED) {
	                MmpTool.LOG_DBG("Has played or prepared!");
	                
	            }
				mSpeedStep = 1;
	            mPlayerSpeed = PlayerSpeed.SPEED_1X;
		        
		   	}catch(Exception ex){
				Log.d(TAG,"startVideo exce Exception ex = "+ ex);
				throw new IllegalStateException(ex);


			}
		}
	}


	private void setNormalSpeed() {
		Log.d(TAG, "setNormalSpeed");
		try{
			mPlayer.setPlayModeEx(PlayerSpeed.SPEED_1X, VideoConst.PLAY_STATUS_STARTED);
			mSpeedStep = 1;
			mPlayerSpeed = PlayerSpeed.SPEED_1X;
		}catch(RuntimeException ex){
			Log.d(TAG, "setNormalSpeed exce setPlayModeEx ex " + ex);	
			throw ex;
		}
	}




	public void pauseVideo() {
		if (null != mPlayer) {
			
		
			if (mPlayer.getPlaySourceMode() == VideoConst.PLAYER_MODE_DLNA) {
				DLNADataSource dlnaDataSource = DLNAManager.getInstance()
						.getDLNADataSource(mPlayer.getCurrentPath());
				if (dlnaDataSource != null) {
					if (!dlnaDataSource.getContent().canPause()) {
						throw new IllegalStateException();
					}
				}
			}

			try{
				mPlayer.pauseVideo();
				mSpeedStep = 1;
				mPlayerSpeed = PlayerSpeed.SPEED_1X;
				
                MmpTool.LOG_INFO("VideoManager.java pauseVideo");
			}catch(Exception ex){
				Log.d(TAG,"VideoManager.java pauseVideo exce Exception ex ="+ex);
				throw new IllegalStateException(ex);
			}

                
		}
	}


	public void reset(){
		if (null != mPlayer) {
			
			mPlayer.resetVideo();
			
		}



	}


	public boolean  isInPlaybackState() {

		return mPlayer.isInPlaybackState();
	}

	public void stopVideo(){
		if (null != mPlayer) {
			
			mPlayer.stopVideo();
			
		}
	}

	/**
	 * @return true if currently playing, false otherwise
	 */
	public boolean isPlaying() {
		if (null != mPlayer) {
			Log.d(TAG, "isPlaying");
			return mPlayer.isPlaying();
		}
		return false;
	}

	public void seek(int msec) {
		if (null != mPlayer) {
			
			if (msec < getDuration()) {					
					mPlayer.seekTo(msec);
			}
		
		}
	}

	public boolean canDoSeek() {
		if (null != mPlayer) {
			
			return mPlayer.canDoSeek(mPlayerSpeed);
			
		}
		return false;
	}

	public boolean canDoTrick() {
		if (null != mPlayer) {
			
			return mPlayer.canDoTrick(mPlayerSpeed);
			
		}
		return false;
	}

	// manualNext
	public void playNext() {
		play(true);
	}

	// manualPrev
	public void playPrev() {
		play(false);
	}

	public void autoNext() {
		String path = null;
		if (mPreviewMode) {
			path = mPlayer.getCurrentPath();
		} else {
			
			path = mPlayList.getNext(Const.FILTER_VIDEO, Const.AUTOPLAY);
		}
		Log.d(TAG,"autoNext  path = "+ path);

		if(path == null){
			mPlayer.completVideo();
			return ;
		}
		setDataSource(path);
	}

	public void replay() {
		if (null != mPlayer) {
			int platstatus = mPlayer.getPlayStatus();
			if (platstatus >= VideoConst.PLAY_STATUS_STARTED
					&& platstatus < VideoConst.PLAY_STATUS_STOPPED) {
				mPlayer.stopVideo();
			}
			String path = mPlayer.getCurrentPath();
			setDataSource(path);
		}
	}

	public long getFileSize() {
		if (mPlayer != null) {
			return mPlayer.getSourceSize();
		}
		return 0;
	}

	public int getBytePosition() {

		if (mPlayer != null) {
			
			return  mPlayer.getCurrentBytePosition();
				
		}
		return 0;

	}

	/**
	 * get progress
	 */
	public int getProgress() {

		if (mPlayer != null) {
			
			return mPlayer.getCurrentPosition();
				
			
		}
		return 0;
	}

	public VideoCodecInfo getVideoInfo() {
		if (mPlayer == null) {
			return null;
		}
		return mPlayer.getVideoInfo();
	}

	public void fastForward(){
		if (null != mPlayer) {
			if (!canFastOrSlow()) {
				throw new IllegalStateException();
			}
			if (mPlayer.getPlaySourceMode() == VideoConst.PLAYER_MODE_DLNA) {
				DLNADataSource dlnaDataSource = DLNAManager.getInstance()
						.getDLNADataSource(mPlayer.getCurrentPath());
				if (dlnaDataSource != null) {
					if (!dlnaDataSource.getContent().canSeek()) {
						throw new IllegalStateException();
					}
				}
			}

			int tmpSpeedStep = mSpeedStep;
			PlayerSpeed tmpPlayerSpeed = mPlayerSpeed;

			
			Log.d(TAG, "fastForward mPlayer.getPlayStatus():" + mPlayer.getPlayStatus());
			try{
				switch (mPlayer.getPlayStatus()) {
				case VideoConst.PLAY_STATUS_FF:
					mSpeedStep <<= 1;
					if (mSpeedStep > 32) {
						startVideo();
					} else {
						switch (mSpeedStep) {
						case 2:
							mPlayerSpeed = PlayerSpeed.SPEED_FF_2X;
							break;
						case 4:
							mPlayerSpeed = PlayerSpeed.SPEED_FF_4X;
							break;
						case 8:
							mPlayerSpeed = PlayerSpeed.SPEED_FF_8X;
							break;
						case 16:
							mPlayerSpeed = PlayerSpeed.SPEED_FF_16X;
							break;
						case 32:
							mPlayerSpeed = PlayerSpeed.SPEED_FF_32X;
							break;
						default:
							mPlayerSpeed = PlayerSpeed.SPEED_FF_2X;
							break;
						}
						int result = mPlayer.setPlayModeEx(mPlayerSpeed,
								VideoConst.PLAY_STATUS_FF);

						if(result == UIMediaPlayer.IMTK_PB_ERROR_CODE_NEW_TRICK){
							startVideo();
						}

						Log.d(TAG, "fastForward result = "+result);
					}
					break;

				case VideoConst.PLAY_STATUS_PAUSED:
				case VideoConst.PLAY_STATUS_STARTED:
				case VideoConst.PLAY_STATUS_FR:
				case VideoConst.PLAY_STATUS_SF:
					mSpeedStep = 2;
					mPlayerSpeed = PlayerSpeed.SPEED_FF_2X;
					mPlayer.setPlayModeEx(mPlayerSpeed, VideoConst.PLAY_STATUS_FF);
					break;
				default:
					break;
				}
			}catch(RuntimeException ex){

				mSpeedStep = tmpSpeedStep;
		        mPlayerSpeed = tmpPlayerSpeed;
				Log.d(TAG, "fastForward exce setPlayModeEx ex " + ex);

				throw ex;


			}
		}
	}

	public void fastRewind() {

		if (null != mPlayer) {
			if (!canFastOrSlow()) {
				throw new IllegalStateException();
			}
			if (mPlayer.getPlaySourceMode() == VideoConst.PLAYER_MODE_DLNA) {
				DLNADataSource dlnaDataSource = DLNAManager.getInstance()
						.getDLNADataSource(mPlayer.getCurrentPath());
				if (dlnaDataSource != null) {
					if (!dlnaDataSource.getContent().canSeek()) {
						throw new IllegalStateException();
					}
				}
			}
			int tmpSpeedStep = mSpeedStep;
			PlayerSpeed tmpPlayerSpeed = mPlayerSpeed;
			Log.d(TAG, "fastRewind mPlayer.getPlayStatus():" + mPlayer.getPlayStatus());
			try{
				switch (mPlayer.getPlayStatus()) {
				case VideoConst.PLAY_STATUS_FR:
					mSpeedStep <<= 1;
					Log.d(TAG, "mSpeedStep:" + mSpeedStep);
					if (mSpeedStep > 32) {
						startVideo();
					} else {

						switch (mSpeedStep) {
						case 2:
							mPlayerSpeed = PlayerSpeed.SPEED_FR_2X;
							break;
						case 4:
							mPlayerSpeed = PlayerSpeed.SPEED_FR_4X;
							break;
						case 8:
							mPlayerSpeed = PlayerSpeed.SPEED_FR_8X;
							break;
						case 16:
							mPlayerSpeed = PlayerSpeed.SPEED_FR_16X;
							break;
						case 32:
							mPlayerSpeed = PlayerSpeed.SPEED_FR_32X;
							break;
						/*case 64:
							mPlayerSpeed = PlayerSpeed.SPEED_FR_64X;
							break;*/
						default:
							mPlayerSpeed = PlayerSpeed.SPEED_FR_2X;
							break;
						}
						mPlayer.setPlayModeEx(mPlayerSpeed,
								VideoConst.PLAY_STATUS_FR);

					}
					break;
				/*case VideoConst.PLAY_STATUS_STEP:
					mSpeedStep = 1;
					mPlayerSpeed = PlayerSpeed.SPEED_1X;
					mPlayer.startVideo();
					break;*/
				case VideoConst.PLAY_STATUS_PAUSED:
				case VideoConst.PLAY_STATUS_STARTED:
				case VideoConst.PLAY_STATUS_FF:
				case VideoConst.PLAY_STATUS_SF:
					mSpeedStep = 2;
					mPlayerSpeed = PlayerSpeed.SPEED_FR_2X;
					mPlayer.setPlayModeEx(mPlayerSpeed, VideoConst.PLAY_STATUS_FR);
					break;
				default:
					break;
				}
			}catch(RuntimeException ex){
				mSpeedStep = tmpSpeedStep;
		        mPlayerSpeed = tmpPlayerSpeed;
				Log.d(TAG, "fastRewind exce setPlayModeEx ex " + ex);

				throw ex;


			}
		}

	}

	public void slowForward() {

		if (null != mPlayer) {
			if (!canFastOrSlow()) {
				throw new IllegalStateException();
			}


			int tmpSpeedStep = mSpeedStep;
			PlayerSpeed tmpPlayerSpeed = mPlayerSpeed;
			Log.d(TAG, "slowForward mPlayer.getPlayStatus() " + mPlayer.getPlayStatus());
			try{
				switch (mPlayer.getPlayStatus()) {
				case VideoConst.PLAY_STATUS_SF:
					mSpeedStep <<= 1;
					if (mSpeedStep > 32) {
						startVideo();
					} else {
						switch (mSpeedStep) {
						case 2:
							mPlayerSpeed = PlayerSpeed.SPEED_SF_1_2X;
							break;
						case 4:
							mPlayerSpeed = PlayerSpeed.SPEED_SF_1_4X;
							break;
						case 8:
							mPlayerSpeed = PlayerSpeed.SPEED_SF_1_8X;
							break;
						case 16:
							mPlayerSpeed = PlayerSpeed.SPEED_SF_1_16X;
							break;
						case 32:
							mPlayerSpeed = PlayerSpeed.SPEED_SF_1_32X;
							break;
						default:
							mPlayerSpeed = PlayerSpeed.SPEED_SF_1_2X;
							break;
						}
						mPlayer.setPlayModeEx(mPlayerSpeed,
								VideoConst.PLAY_STATUS_SF);
					}
					break;
			
				case VideoConst.PLAY_STATUS_PAUSED:
				case VideoConst.PLAY_STATUS_STARTED:
				case VideoConst.PLAY_STATUS_FF:
				case VideoConst.PLAY_STATUS_FR:
					mSpeedStep = 2;
					mPlayerSpeed = PlayerSpeed.SPEED_SF_1_2X;
					mPlayer.setPlayModeEx(mPlayerSpeed, VideoConst.PLAY_STATUS_SF);
					break;
				default:
					break;
				}
			}catch(RuntimeException ex){
				mSpeedStep = tmpSpeedStep;
		        mPlayerSpeed = tmpPlayerSpeed;
				Log.d(TAG, "slowForward exce setPlayModeEx ex " + ex);

				throw ex;

			}
		}

	}


	public void step() {
		if (null != mPlayer) {
			
			mPlayer.step(1);
			
		}
	}

	public boolean selectMts(short mtsIdx) {
		if (null != mPlayer) {
			return mPlayer.setAudioTrack(mtsIdx);
		}
		return false;
	}

	public int getDuration() {
		if (null != mPlayer) {
			return mPlayer.getDuration();
		}
		return 0;
	}

	public int getCurrentPosition() {
		return getProgress();
	}

	public boolean setTS(short index) {
		if (null != mPlayer) {
			mPlayer.setTS(index);
		}
		return false;
	}

	public int getPlayStatus() {
		if (null != mPlayer) {
			return mPlayer.getPlayStatus();
		}
		return VideoConst.PLAY_STATUS_INITED;
	}

	public int getVideoWidth() {
		if (null != mPlayer) {
			return mPlayer.getVideoWidth();
		}
		return 0;
	}

	public int getVideoHeight() {
		if (null != mPlayer) {
			return mPlayer.getVideoHeight();
		}
		return 0;
	}

	public int getVideoframeRate() {
		if (null != mPlayer) {
			if (null != mPlayer.getVideoInfo()) {
				return mPlayer.getVideoInfo().getVideoframeRate();
			}
		}
		return 0;
	}

	public int getNumTracks() {
		if (null != mPlayer) {
			if (null != mPlayer.getVideoInfo()) {
				return mPlayer.getVideoInfo().getNumTracks();
			}
		}
		return 0;
	}

	public int getCodecType() {
		if (null != mPlayer) {
			if (null != mPlayer.getVideoInfo()) {
				return mPlayer.getVideoInfo().getCodecType();
			}
		}
		return 0;
	}

	public String getVideoTitle() {
		if(getMetaDataInfo() != null){
			return getMetaDataInfo().getTitle();
		}
		return "";
	}


	public String getVideoDirector() {
		
		if(getMetaDataInfo() != null){
			return getMetaDataInfo().getDirector();
		}
		return "";
	}

	public String getVideoCopyright() {
		
		if(getMetaDataInfo() != null){
			return getMetaDataInfo().getCopyright();
		}
		return "";
	}

	public String getVideoGenre() {
		
		if(getMetaDataInfo() != null){
			return getMetaDataInfo().getGenre();
		}
		return "";
	}

	public String getVideoYear() {
		
		if(getMetaDataInfo() != null){
			return getMetaDataInfo().getYear();
		}
		return "";
	}

	public String getCurFileName() {
		if (null != mPlayer) {
			return mPlayer.getCurrentPath();
		}
		return "";
	}

	public void setPlayerMode(int mode) {
		if (null != mPlayer) {
			mPlayer.setPlaySourceMode(mode);
		}
	}

	public int getPlaySpeed() {
		return mSpeedStep;
	}

	public void setPreviewMode(boolean preview) {
		mPreviewMode = preview;
	}

	public void setVideoZoom(int zoomType) {
		if (null != mComset) {
			mComset.videoZoom(zoomType);
		}
	}

	/**
	 * 
	 * Set the url of subtitle, when subtitle's type is vob_sub
	 * 
	 */
	public void setSubtitleDataSourceEx(String UriSub, String UriIdx) {
		if (null != mPlayer) {
			mPlayer.offSubtitleTrack();
			mPlayer.setSubtitleDataSourceEx(UriSub, UriIdx);
			mPlayer.onSubtitleTrack();
		}
	}


	public void onSubtitleTrack() {
		if (null != mPlayer) {
			mPlayer.onSubtitleTrack();
		}
	}


	public void offSubtitleTrack() {
		if (null != mPlayer) {
			mPlayer.offSubtitleTrack();
		}
	}

	public void setSubtitleTrack(int index) {
		if (null != mPlayer) {
			if (mPlayer.getPlayStatus() >= VideoConst.PLAY_STATUS_STARTED
					&& mPlayer.getPlayStatus() < VideoConst.PLAY_STATUS_STOPPED) {
				mPlayer.setSubtitleTrack(index);
			}
		}
	}

	public SubtitleTrackInfo getSubtitleTrackInfo(final int trackIndex) {
		if (null != mPlayer) {
			return mPlayer.getSubtitleTrackInfo(trackIndex);
		}
		return null;
	}
    
	public SubtitleTrackInfo[] getAllSubtitleTrackInfo() {
		if (null != mPlayer) {
			return mPlayer.getAllSubtitleTrackInfo();
		}
		return null;

	}

	public void setSubOnOff(boolean on) {
		if (on) {
			onSubtitleTrack();
		} else {
			offSubtitleTrack();
		}
	}
	private MetaData mMetaData = null;

	public MetaData getMetaDataInfo() {
		
		if(mMetaData == null){
			mMetaData = new MetaData();
			if(mPlayer != null){
				MetaDataInfo dataInfo = mPlayer.getMetaDataInfo();
				if(mPlayer.getMetaDataInfo()!= null){
					mMetaData.setMetaData(dataInfo.getTitle(), dataInfo.getDirector(), dataInfo.getCopyright(), dataInfo.getYear(), 
							dataInfo.getGenre(), dataInfo.getArtist(), dataInfo.getAlbum(), mPlayer.getDuration(), dataInfo.getBiteRate());
				}else{
					mMetaData.setMetaData(null, null, null, null, null, null, null,
							mPlayer.getDuration(), 0);
				}
			}
		}
		
		return mMetaData;
		
	}

	public boolean setAudioTrack(int track) {
		if (null != mPlayer) {
			return mPlayer.setAudioTrack(track);
		}
		return false;
	}

	public short getSubtitleTrackNumber() {
		if (null != mPlayer) {
			if (subTitleNum == -1) {
				try {
					subTitleNum = (short) mPlayer.getAllSubtitleTrackInfo().length;
				} catch (Exception e) {
					return 0;
				}
			}

			return (short) subTitleNum;

		}
		return 0;
	}

	public int getAudioTranckNumber() {
		if (null != mPlayer && mPlayer.getAudioTrackInfo(true) != null) {
			return mPlayer.getAudioTrackInfo(true).getAudioTrackNum();
		}
		return 0;
	}

	/**
	 * @param isNext
	 *            true is next,false is prev
	 */
	private void play(boolean isNext) {
		if (null != mPlayer) {
			int playStatyus = mPlayer.getPlayStatus();
			if (playStatyus >= VideoConst.PLAY_STATUS_STARTED
					&& playStatyus < VideoConst.PLAY_STATUS_STOPPED) {
				mPlayer.stopVideo();
			}
			
			/*if (isNext) {

				if(mPlayList.isEnd(Const.FILTER_VIDEO)){
					Log.d(TAG,"play next  playlist end to completeVideo.");
					mPlayer.completVideo();
					return ;

				}

				
			} else {
				
				if(mPlayList.isBegin(Const.FILTER_VIDEO)){
					Log.d(TAG,"play pre playlist begin to completeVideo.");
					mPlayer.completVideo();
					return ;

				}
				
			}*/
			
			
			String path = isNext ? mPlayList.getNext(Const.FILTER_VIDEO,
					Const.MANUALNEXT) : mPlayList.getNext(Const.FILTER_VIDEO,
					Const.MANUALPRE);
			Log.d(TAG, "play isNext" + isNext +" path = "+ path);

			if(path == null){
				mPlayer.completVideo();
				return ;
				
			}
			
			setDataSource(path);
		}
	}

	private boolean canFastOrSlow() {
		boolean bFast = true;
		switch (mPlayer.getPlaySourceMode()) {
		case VideoConst.PLAYER_MODE_DLNA:
		case VideoConst.PLAYER_MODE_SAMBA:
		case VideoConst.PLAYER_MODE_MMP: {
			if (mPlayer.getCurrentPath() != null) {
				mVideoFile = new VideoFile(mPlayer.getCurrentPath());
				bFast = !(mVideoFile.isIsoVideoFile());
			}
		}
			break;
		default:
			break;
		}
		return bFast;
	}
}
