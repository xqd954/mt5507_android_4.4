package com.mediatek.ui.mmp.multimedia;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Display;
import android.view.KeyEvent;
import android.view.SurfaceView;
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.util.Log;

import com.mediatek.mmp.MtkMediaPlayer;
import com.mediatek.mmp.MtkMediaPlayer.OnCompletionListener;
import com.mediatek.mmp.MtkMediaPlayer.OnErrorListener;
import com.mediatek.mmp.MtkMediaPlayer.OnInfoListener;
import com.mediatek.mmp.MtkMediaPlayer.OnPreparedListener;
import com.mediatek.mmp.MtkMediaPlayer.OnSeekCompleteListener;

import android.media.MediaPlayer;

import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.mmpcm.videoimpl.VideoManager;
import com.mediatek.tvcm.TVContent;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.mmp.commonview.ControlView.ControlPlayState;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.nav.CaptureLogoActivity;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;
import com.mediatek.mmpcm.device.DevListener;
import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.dm.DeviceManagerEvent;

import android.view.View;
import android.os.SystemProperties;


public class VideoPlayActivity extends MediaPlayActivity {

	private static final String TAG = "VideoPlayActivity";


	private static final int PROGRESS_CHANGED = 1;

	private static final int HIDE_CONTROLER = 2;

	private static final int DELAY_AUTO_NEXT = 3;

	private static final int MSG_DISMISS_NOT_SUPPORT = 5;


	private static final int MSG_GET_CUR_POS = 7;


	private static final int DELAYTIME = 1000;

	private static final int HIDE_DELAYTIME = 10000;
	

	private LinearLayout vLayout;

	private TimeDialog mTimeDialog;
	private VideoDialog mVideoStopDialog;

	private int mVideoSource = 0;

	private boolean videoPlayStatus = false;
	
	private SurfaceView mSurfaceView = null;

	private boolean progressFlag = false;
	
	private Resources mResource;
	private boolean exitState = false;
	
	public static boolean video_player_Activity_resumed=false;

	public Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {

			case PROGRESS_CHANGED: {
				Log.e(TAG,"progressFlag:"+progressFlag+mLogicManager.getVideoPlayStatus());
				if (progressFlag || mLogicManager.getVideoPlayStatus() < VideoConst.PLAY_STATUS_PREPARED) {
					break;
				}
				if (mControlView != null) {
					
					int progress = 0;
					if(isTotal){
						progress = mLogicManager.getVideoProgress();
					}else{
						progress = mLogicManager.getVideoBytePosition();

					}
					if (progress >= 0 && progress <= mControlView.getProgressMax()) {
						mControlView.setCurrentTime(progress);
						mControlView.setProgress(progress);
					}
				}
				if (mControlView != null && mLogicManager.getVideoPlayStatus()!=VideoConst.PLAY_STATUS_PAUSED 
						&& mLogicManager.getVideoPlayStatus()>VideoConst.PLAY_STATUS_PREPAREING 
						&& mLogicManager.getVideoPlayStatus()<VideoConst.PLAY_STATUS_STOPPED) {
					sendEmptyMessageDelayed(PROGRESS_CHANGED, DELAYTIME);
				}
				break;
			}
			case HIDE_CONTROLER: {
				if (menuDialog != null && menuDialog.isShowing()) {
					if(mHandler.hasMessages(HIDE_CONTROLER)){
						mHandler.removeMessages(HIDE_CONTROLER);
					}
					sendEmptyMessageDelayed(HIDE_CONTROLER, MSG_DISMISS_DELAY);
					break;
				}
				hideController();
				break;
			}
			case DELAY_AUTO_NEXT:
				dismissNotSupprot();
				dismissMenuDialog();
				mLogicManager.playNextVideo();
				break;
			case MSG_DISMISS_NOT_SUPPORT: {
					
				break;
			}
			case MSG_GET_CUR_POS: {
				progressFlag = true;
				if (mControlView != null) {
					long pos = mLogicManager.getVideoBytePosition();
					if (mLargeFile) {
						pos = pos >> RATE;
					}
					if (pos > 0)
					mControlView.setProgress((int) pos);
				}
				break;
			}
			default:
				break;
			}

		}

	};

	
	/**
	 * Remove to get progress inforamtion and time information message.
	 */
	protected void removeProgressMessage(){
		mHandler.removeMessages(PROGRESS_CHANGED);
	}
	/**
	 * Add to get progress inforamtion and time information message
	 */
	protected void addProgressMessage(){
		if (!mHandler.hasMessages(PROGRESS_CHANGED)) {
			mHandler.sendEmptyMessage(PROGRESS_CHANGED);
		}
	}
	
	private ControlPlayState mControlImp = new ControlPlayState() {

		public void play() {
			mLogicManager.playVideo();
			addProgressMessage();
		}

		public void pause() {
			try{
			mLogicManager.pauseVideo();
			}catch(Exception e){
				featureNotWork(getString(R.string.mmp_featue_notsupport));
				//mControlView.setPlayIcon();
				throw new IllegalStateException(e);
			}
		}
	};



	static private int MAX_VALUE = 2147483647;
	static private int RATE = 2;
	static private int BASE = 31;
	private boolean mLargeFile = false;

	private boolean isLargeFile(long size) {
		long multiple;
		RATE = 2;
		if (size > MAX_VALUE) {
			multiple = size >> BASE;
			while (true) {
				switch ((int) multiple) {
				case 1:
				case 2:
				case 3:
					return true;
				default:
					multiple = multiple >> 1;
					RATE += 1;
					break;
				}
			}
		}
		return false;
	}

	private boolean isTotal = true;
	private void update() {
		isTotal = true;
		if (mControlView != null) {
			
			int i = mLogicManager.getVideoDuration();
			long size;
			mLargeFile = false;
			if (i <= 0) {
				isTotal = false;
				size = mLogicManager.getVideoFileSize();
				mControlView.setVisibility(false);
				mLargeFile = isLargeFile(size);
				size = (size > MAX_VALUE) ? size >> RATE : size;
				i = (int) size;
			} else {
				mControlView.setVisibility(true);
			}
			i = (i > 0 ? i : 0);
			mControlView.setProgressMax(i);
			mControlView.setEndtime(i);
			mControlView.initVideoTrackNumber();
		
			mControlView.initSubtitle(mLogicManager.getSubtitleTrackNumber());
		

			MtkLog.i(TAG, "width:" + mLogicManager.getVideoWidth() + "heght:"
						+ mLogicManager.getVideoHeight());
			mLogicManager.videoZoom( VideoConst.VOUT_ZOOM_TYPE_1X );
			int mAudioTrackNum = mLogicManager.getAudioTranckNumber();

			if(playExce == PlayException.DEFAULT_STATUS ){
				
				if(mAudioTrackNum == 0){
					playExce = PlayException.VIDEO_ONLY;
					featureNotWork(mResource.getString(R.string.mmp_video_only));
				}
				mControlView.setZoomSize();
			}else if(playExce == PlayException.VIDEO_NOT_SUPPORT){

				if(mAudioTrackNum == 0){
					playExce = PlayException.FILE_NOT_SUPPORT;
					featureNotWork(mResource.getString(R.string.mmp_file_notsupport));
					mControlView.setPauseIconGone();
					autoNext();
				}

			}else if(playExce == PlayException.AUDIO_NOT_SUPPORT){
				if(mAudioTrackNum == 0){
					playExce = PlayException.VIDEO_ONLY;
					featureNotWork(mResource.getString(R.string.mmp_video_only));
				}
				mControlView.setZoomSize();

			}

		}
		if (null != mInfo && mInfo.isShowing()) {
			mInfo.setVideoView();
		}
	}
	
	private void resetVideoInfo(){
		playExce = PlayException.DEFAULT_STATUS;
		SCREENMODE_NOT_SUPPORT = false;
		
		progressFlag = false;
		
		if (mControlView != null) {
			mControlView.setInforbarNull();
			Log.e(TAG,"resetVideoInfo  fileName:"+mLogicManager.getFileName());
			mControlView.setFileName(mLogicManager.getCurrentFileName(Const.FILTER_VIDEO));
			mControlView.setFilePosition(mLogicManager.getVideoPageSize());
			mControlView.reSetVideo();
		}
		removeFeatureMessage();
		dismissNotSupprot();
		dismissTimeDialog();
		dismissMenuDialog();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.mmp_videoplay);
		mResource = getResources();
		getIntentData();
		
		mLogicManager = LogicManager.getInstance(this);
		
		mSurfaceView = (SurfaceView) findViewById(R.id.video_player_suface);

		vLayout = (LinearLayout) findViewById(R.id.mmp_video);
		getPopView(R.layout.mmp_popupvideo, MultiMediaConstant.VIDEO,
				mControlImp);

		MtkLog.d(TAG, "onCreate()");
		if(0!=SystemProperties.getInt(AUTO_TEST_PROPERTY,0)||getIntent().getData()!=null ){
			if(getIntent().getData()!=null){
				video_player_Activity_resumed=true;
			}else
			{
				video_player_Activity_resumed=false;
			}
			
			autoTest(Const.FILTER_VIDEO,MultiFilesManager.CONTENT_VIDEO);
		}else
		{
			video_player_Activity_resumed=false;
		}
		
		
		mLogicManager.initVideo(mSurfaceView, mVideoSource);
		if(mVideoSource == VideoConst.PLAYER_MODE_MMP){
		mLogicManager.setVideoPreparedListener(preparedListener);
		mLogicManager.setCompleteListener(completeListener);
		mLogicManager.setVideoErrorListener(mOnErrorListener);
		mLogicManager.setOnInfoListener(mInfoListener);
		mLogicManager.setSeekCompleteListener(mSeekCompListener);
		
		}else{
			mLogicManager.setVideoPreparedListener(mtkPreparedListener);
			mLogicManager.setCompleteListener(mtkCompleteListener);
			mLogicManager.setVideoErrorListener(mtkOnErrorListener);
			mLogicManager.setOnInfoListener(mtkInfoListener);
			mLogicManager.setSeekCompleteListener(mtkSeekCompListener);

		}
		
		
		
		//mControlView.setFilePosition(mLogicManager.getVideoPageSize());
		
		initVulume(mLogicManager);
	
		mControlView.setRepeatVisibility(Const.FILTER_VIDEO);
		showPopUpWindow(vLayout);
		setRepeatMode();

	}

	protected void onResume() {
		super.onResume();
		MtkLog.d(TAG, "onResume()");
		
		exitState = false;
		reSetController();
		isSetPicture = false;
		if (isBackFromCapture) {
			if (videoPlayStatus) {
				if(null != mControlView){
					mControlView.play();
				}
				videoPlayStatus = false;
			}
			isBackFromCapture = false;
		}
	}

	protected void onStart() {
		super.onStart();
	}

	
	private void autoNext(){
		mHandler.removeMessages(DELAY_AUTO_NEXT);
		mHandler.sendEmptyMessageDelayed(DELAY_AUTO_NEXT, DELAYTIME);
	}





	//MTK MEDIAPLAYER


	private OnPreparedListener mtkPreparedListener = new OnPreparedListener() {
		@Override
		public void onPrepared(MtkMediaPlayer mp) {
			Log.i(TAG, "---------OnPBPreparedListener --------------isControlBarShow---"+isControlBarShow);
			handlePrepare();
		}
	};
	private OnCompletionListener mtkCompleteListener = new OnCompletionListener() {
		
		@Override
		public void onCompletion(MtkMediaPlayer mp) {
			Log.i(TAG, "---------onCompletion -------------");
			handleComplete();
		}
	};

	private OnSeekCompleteListener mtkSeekCompListener = new OnSeekCompleteListener(){

		public void onSeekComplete(MtkMediaPlayer mp) {
			Log.d(TAG, "enter onSeekComplete~~~~ " );
			handleSeekComplete();
		}

	};

	
	private OnInfoListener mtkInfoListener = new OnInfoListener() {
		
		@Override
		public boolean onInfo(MtkMediaPlayer mp, int what, int extra) {
			return handleInfo(what);
		}
	};
	
	private OnErrorListener mtkOnErrorListener = new OnErrorListener() {
		
		@Override
		public boolean onError(MtkMediaPlayer mp, int what, int extra) {
			Log.e("plz","onError~~~"+what+"extra:"+extra);
			return handleError(what);
		}
	};













	// MEDIAPLAYER
	private MediaPlayer.OnPreparedListener preparedListener = new MediaPlayer.OnPreparedListener() {
		@Override
		public void onPrepared(MediaPlayer mp) {
			Log.i(TAG, "---------OnPBPreparedListener --------------isControlBarShow---"+isControlBarShow);
			handlePrepare();
		}
	};
	private MediaPlayer.OnCompletionListener completeListener = new MediaPlayer.OnCompletionListener() {
		
		@Override
		public void onCompletion(MediaPlayer mp) {
			Log.i(TAG, "---------onCompletion -------------");
			handleComplete();
		}
	};

	private MediaPlayer.OnSeekCompleteListener mSeekCompListener = new MediaPlayer.OnSeekCompleteListener(){

		public void onSeekComplete(MediaPlayer mp) {
			Log.d(TAG, "enter onSeekComplete~~~~ " );
			handleSeekComplete();
		}

	};

	
	private MediaPlayer.OnInfoListener mInfoListener = new MediaPlayer.OnInfoListener() {
		
		@Override
		public boolean onInfo(MediaPlayer mp, int what, int extra) {
			return handleInfo(what);
		}
	};
	
	private MediaPlayer.OnErrorListener mOnErrorListener = new MediaPlayer.OnErrorListener() {
		
		@Override
		public boolean onError(MediaPlayer mp, int what, int extra) {
			Log.e("plz","onError~~~"+what+"extra:"+extra);
			return handleError(what);
		}
	};




	private void handlePrepare(){
		if (mControlView != null) {
			mControlView.setVolumeMax(maxVolume);
			mControlView.setCurrentVolume(currentVolume);
		}
		reSetController();
	}


	private void handleComplete(){
		if(!EPG_KEY_PRESS){
			mLogicManager.autoNext();
			removeFeatureMessage();
			dismissNotSupprot();
		}else{
			EPG_KEY_PRESS = false;
		
		}


	}


	

	private void handleSeekComplete(){
		if(isControlBarShow){
			addProgressMessage();
		}

	}
	
	private boolean handleInfo(int what){
			switch (what) {
			case VideoManager.MEDIA_INFO_DATA_PREPAREED_STATE:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_DATA_PREPAREED_STATE");
				resetVideoInfo();
				break;
			case VideoManager.MEDIA_INFO_METADATA_COMPLETE:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_METADATA_COMPLETE");
				mHandler.removeMessages(DELAY_AUTO_NEXT);
				mLogicManager.finishVideo();
				dismissTimeDialog();
				dismissNotSupprot();
				finish();
				break;
			case VideoManager.MEDIA_INFO_START_INVALID_STATE:
			case VideoManager.MEDIA_INFO_PAUSE_INVALID_STATE:
			case VideoManager.MEDIA_INFO_STOP_INVALID_STATE:
			case VideoManager.MEDIA_INFO_SEEK_INVALID_STATE:
			case VideoManager.MEDIA_INFO_NOT_SUPPORT:
				Log.d(TAG,"enter onInfo:mmp_featue_notsupport");
				featureNotWork(getString(R.string.mmp_featue_notsupport));
				break;
			case VideoManager.MEDIA_INFO_ON_REPLAY:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_ON_REPLAY");
				//mControlView.play();
				mLogicManager.replayVideo();
				break;
			case VideoManager.MEDIA_INFO_3D_VIDEO_PLAYED:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_3D_VIDEO_PLAYED");
				break;
			case VideoManager.MEDIA_INFO_AUDIO_ONLY_SERVICE:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_AUDIO_ONLY_SERVICE");
			case VideoManager.MEDIA_INFO_VIDEO_ENCODE_FORMAT_UNSUPPORT:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_VIDEO_ENCODE_FORMAT_UNSUPPORT");
				SCREENMODE_NOT_SUPPORT = true;
				mControlView.setZoomEmpty();
				dismissMenuDialog();
			 
				if(playExce == PlayException.VIDEO_ONLY){
					playExce = PlayException.FILE_NOT_SUPPORT;
					featureNotWork(mResource.getString(R.string.mmp_file_notsupport));
					mControlView.setPauseIconGone();
					autoNext();
				}else if(playExce == PlayException.AUDIO_NOT_SUPPORT){
					playExce = PlayException.AV_NOT_SUPPORT;
					featureNotWork(mResource
							.getString(R.string.mmp_audio_notsupport)
							+ "\n"
							+ mResource.getString(R.string.mmp_video_notsupport));
					mControlView.setPauseIconGone();
					autoNext();
				}else if(playExce == PlayException.DEFAULT_STATUS){
					if (mLogicManager.getVideoPlayStatus() >= VideoConst.PLAY_STATUS_FF && mLogicManager.getVideoPlayStatus() <= VideoConst.PLAY_STATUS_SF) {
						reSetController();
						if (mControlView != null){
							mControlView.setMediaPlayState();
						}

					}
					playExce = PlayException.VIDEO_NOT_SUPPORT;
					featureNotWork(mResource.getString(R.string.mmp_video_notsupport));
					mControlView.setPauseIconGone();
				}
				break;
			case VideoManager.MEDIA_INFO_VIDEO_ONLY_SERVICE:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_VIDEO_ONLY_SERVICE");
				if(playExce == PlayException.VIDEO_NOT_SUPPORT){
					playExce = PlayException.FILE_NOT_SUPPORT;
					featureNotWork(mResource.getString(R.string.mmp_file_notsupport));
					mControlView.setPauseIconGone();
					autoNext();
				}else if(playExce == PlayException.DEFAULT_STATUS){
					playExce = PlayException.VIDEO_ONLY;
					featureNotWork(mResource.getString(R.string.mmp_video_only));
					mControlView.setPauseIconGone();
				}
				break;
			case VideoManager.MEDIA_INFO_AUDIO_ENCODE_FORMAT_UNSUPPORT:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_AUDIO_ENCODE_FORMAT_UNSUPPORT");
				if(playExce == PlayException.VIDEO_NOT_SUPPORT){
					playExce = PlayException.AV_NOT_SUPPORT;
					featureNotWork(mResource
							.getString(R.string.mmp_audio_notsupport)
							+ "\n"
							+ mResource.getString(R.string.mmp_video_notsupport));
					mControlView.setPauseIconGone();
					autoNext();
				}else if(playExce == PlayException.DEFAULT_STATUS){
					playExce = PlayException.AUDIO_NOT_SUPPORT;
					featureNotWork(mResource.getString(R.string.mmp_audio_notsupport));
					mControlView.setPauseIconGone();
				}
				break;
			case VideoManager.MEDIA_INFO_BAD_INTERLEAVING:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_BAD_INTERLEAVING");
				break;
			case VideoManager.MEDIA_INFO_BUFFERING_END:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_BUFFERING_END");
				break;
			case VideoManager.MEDIA_INFO_BUFFERING_START:
				Log.d(TAG,"enter onInfo: MEDIA_INFO_BUFFERING_START");
				break;
			case VideoManager.MEDIA_INFO_METADATA_UPDATE:
				Log.d(TAG,"enter onInfo: MEDIA_INFO_METADATA_UPDATE");
				break;
			case VideoManager.MEDIA_INFO_NOT_SEEKABLE:
				Log.d(TAG,"enter onInfo: MEDIA_INFO_NOT_SEEKABLE");
				featureNotWork(mResource.getString(R.string.mmp_featue_notsupport));
				break;
			case VideoManager.MEDIA_INFO_POSITION_UPDATE:
				if (mHandler.hasMessages(PROGRESS_CHANGED)) {
					mHandler.removeMessages(PROGRESS_CHANGED);
				}
				mHandler.sendEmptyMessage(MSG_GET_CUR_POS);
				break;
//			case VideoManager.MEDIA_INFO_SUBTITLE_UPDATA:
//				Log.d(TAG,"enter onInfo: MEDIA_INFO_SUBTITLE_UPDATA");
//				break;
			case VideoManager.MEDIA_INFO_UNKNOWN:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_UNKNOWN");
				break;
			case VideoManager.MEDIA_INFO_VIDEO_TRACK_LAGGING:
				Log.d(TAG,"enter onInfo:MEDIA_INFO_VIDEO_TRACK_LAGGING");
				break;

			case VideoManager.MTK_MEDIA_INFO_SCRAMBLED_AUDIO_VIDEO_SERVICE:
				Log.d(TAG,"enter onInfo:MTK_MEDIA_INFO_SCRAMBLED_AUDIO_VIDEO_SERVICE");
				featureNotWork(mResource.getString(R.string.mmp_media_info_scrambled_audio_video_service));
				break;
			case VideoManager.MTK_MEDIA_INFO_SCRAMBLED_AUDIO_CLEAR_VIDEO_SERVICE:
				Log.d(TAG,"enter onInfo:MTK_MEDIA_INFO_SCRAMBLED_AUDIO_CLEAR_VIDEO_SERVICE");
				featureNotWork(mResource.getString(R.string.mmp_media_info_scrambled_audio_clear_video_service));
				break;
			case VideoManager.MTK_MEDIA_INFO_SCRAMBLED_AUDIO_NO_VIDEO_SERVICE:
				Log.d(TAG,"enter onInfo:MTK_MEDIA_INFO_SCRAMBLED_AUDIO_NO_VIDEO_SERVICE");
				featureNotWork(mResource.getString(R.string.mmp_media_info_scrambled_audio_no_video_service));
				break;
			case VideoManager.MTK_MEDIA_INFO_SCRAMBLED_VIDEO_CLEAR_AUDIO_SERVICE:
				Log.d(TAG,"enter onInfo:MTK_MEDIA_INFO_SCRAMBLED_VIDEO_CLEAR_AUDIO_SERVICE");
				featureNotWork(mResource.getString(R.string.mmp_media_info_scrambled_video_clear_audio_service));
				break;
			case VideoManager.MTK_MEDIA_INFO_SCRAMBLED_VIDEO_NO_AUDIO_SERVICE:
				Log.d(TAG,"enter onInfo:MTK_MEDIA_INFO_SCRAMBLED_VIDEO_NO_AUDIO_SERVICE");
				featureNotWork(mResource.getString(R.string.mmp_media_info_scrambled_video_no_audio_service));
				break;
            case VideoManager.MEDIA_INFO_VIDEO_RENDERING_START:
				MtkLog.d(TAG,"enter onInfo:MEDIA_INFO_VIDEO_RENDERING_START");
               	update();
                break;
			default:
				Log.d(TAG,"enter onInfo:"+what);
				break;
			}
			return false;
		

	}

	private boolean handleError(int what){
			switch (what) {
			case VideoManager.MEDIA_ERROR_FILE_CORRUPT:
				Log.d(TAG,"enter onError:  MEDIA_ERROR_FILE_CORRUPT");
				playExce = PlayException.FILE_NOT_SUPPORT;
				featureNotWork(mResource.getString(R.string.mmp_file_corrupt));
				autoNext();
				return true;
			case VideoManager.MEDIA_ERROR_FILE_NOT_SUPPORT:
				Log.d(TAG,"enter onError:  MEDIA_ERROR_FILE_NOT_SUPPORT");
			case VideoManager.MEDIA_ERROR_OPEN_FILE_FAILED:
				Log.d(TAG,"enter onError:  MEDIA_ERROR_OPEN_FILE_FAILED");
				playExce = PlayException.FILE_NOT_SUPPORT;
				featureNotWork(mResource.getString(R.string.mmp_file_notsupport));
				autoNext();
				return true;
//			case VideoManager.MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK:
//				Log.d(TAG,"enter onError:  MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK");
//				break;
//			
//			case VideoManager.MEDIA_ERROR_SERVER_DIED:
//				Log.d(TAG,"enter onError:  MEDIA_ERROR_SERVER_DIED");
//				break;
//			case VideoManager.MEDIA_ERROR_UNKNOWN:
//				Log.d(TAG,"enter onError:  MEDIA_ERROR_UNKNOWN");
//				break;
			default:
//				Log.d(TAG,"enter onError:  MEDIA_ERROR_OPEN_FILE_FAILED  what:"+what);
//				FILE_NOT_SUPPORT = true;
//				onNotSuppsort(mResource.getString(R.string.mmp_file_notsupport));
//				mHandler.sendEmptyMessageDelayed(DELAY_AUTO_NEXT, DELAYTIME);
				return true;
			}

		}

	/**
	 * 
	 */
	private void getIntentData() {
		Intent it = getIntent();
		//added for playing from TKUI
		mVideoSource = MultiFilesManager.getInstance(this)
				.getCurrentSourceType();
		MtkLog.i(TAG, "mVideoSource:"+mVideoSource);
		boolean playlocal = it.getBooleanExtra("playlocal", false);
		if(true == playlocal){
			mVideoSource = VideoConst.PLAYER_MODE_MMP;
			onRegisterUsbEvent();
		}else{
	
	
			switch (mVideoSource) {
			case MultiFilesManager.SOURCE_LOCAL:
				mVideoSource = VideoConst.PLAYER_MODE_MMP;
				break;
			case MultiFilesManager.SOURCE_SMB:
				mVideoSource = VideoConst.PLAYER_MODE_SAMBA;
				break;
			case MultiFilesManager.SOURCE_DLNA:
				mVideoSource = VideoConst.PLAYER_MODE_DLNA;
				break;
			default:
				break;
			}
		}
	}


	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		int keycode = event.getKeyCode();
		switch (keycode) {
		case KeyMap.KEYCODE_MTKIR_NEXT:
		case KeyMap.KEYCODE_MTKIR_PREVIOUS:
			return true;
		default:
			break;
		}

		return super.dispatchKeyEvent(event);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		Log.i(TAG,"onKeyDown");
		if (mTimeDialog != null && mTimeDialog.isShowing()) {
			mTimeDialog.onKeyDown(keyCode, event);
			return true;
		}
		
		switch (keyCode) {
		case KeyMap.KEYCODE_DPAD_CENTER:
			if(isValid()){
				if (mLogicManager.getVideoPlayStatus() < VideoConst.PLAY_STATUS_STARTED) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				hideFeatureNotWork();
			}
			break;
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:
			//if (isValid()) {
				reSetController();
				if (!mLogicManager.isInPlaybackState()) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				
				try {
					mLogicManager.stepVideo();
				} catch (Exception e) {
					MtkLog.d(TAG, e.getMessage());
					featureNotWork(getString(R.string.mmp_featue_notsupport));
				}
			//}
			return true;
		case KeyMap.KEYCODE_MTKIR_CHDN:
			if (isValid()) {
				mHandler.removeMessages(DELAY_AUTO_NEXT);
				reSetController();
				mLogicManager.playPrevVideo();
			}
			return true;
		case KeyMap.KEYCODE_MTKIR_CHUP:
			if (isValid()) {
				mHandler.removeMessages(DELAY_AUTO_NEXT);
				reSetController();
				mLogicManager.playNextVideo();
			}
			return true;
			
		case KeyMap.KEYCODE_MENU:
			if(playExce == PlayException.AV_NOT_SUPPORT || playExce == PlayException.FILE_NOT_SUPPORT){
				return true;
			}
			break;
		case KeyMap.KEYCODE_MTKIR_TIMER: {
			MtkLog.i(TAG,"KEYCODE_MTKIR_TIMER");
			Log.i(TAG,"onKeyDown");
			reSetController();
			if (isValid()) {
				
				if (!mLogicManager.isInPlaybackState()) {
					Log.i(TAG,"isInPlaybackState");
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				
				if (mLogicManager.getVideoWidth() <= 0
						|| mLogicManager.getVideoHeight() <= 0 || !mLogicManager.canDoSeek() || mLogicManager.getVideoDuration() <= 0) {
					if(!mLogicManager.canDoSeek()){
						Log.i(TAG,"can't seek");
					}
					if(mLogicManager.getVideoWidth() <= 0){
						Log.i(TAG,"can't seek getVideoWidth <=0");
					} 
					if(mLogicManager.getVideoHeight() <= 0){
						Log.i(TAG,"can't seek getVideoHeight <=0");
					} 
					if(mLogicManager.getVideoDuration() <= 0){
						Log.i(TAG,"can't seek getVideoDuration <=0");
					}
	
					if(!mLogicManager.canDoSeek()){
						featureNotWork(VideoPlayActivity.this
								.getString(R.string.mmp_seek_notsupport));
						return true;
					}
				}
					
				if (null == mTimeDialog) {
					mTimeDialog = new TimeDialog(VideoPlayActivity.this);
				}
				mHandler.removeMessages(HIDE_CONTROLER);
				mTimeDialog.show();
			}
			return true;
		}
		
		case KeyMap.KEYCODE_MTKIR_FASTFORWARD: {
			reSetController();
			if (isValid()) {
				if (!mLogicManager.isInPlaybackState()
					|| playExce ==PlayException.VIDEO_NOT_SUPPORT) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				try {
					mLogicManager.fastForwardVideo();
					setFast(0);
				}catch(IllegalStateException e){
					MtkLog.d(TAG, "Exception" + e.getMessage());
					featureNotWork(getString(R.string.mmp_buffer_not_enough));
				} catch (Exception e) {
					MtkLog.d(TAG, "Exception" + e.getMessage());
					featureNotWork(getString(R.string.mmp_featue_notsupport));
				}
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_REWIND: {
			reSetController();
			if (isValid()) {
				if (!mLogicManager.isInPlaybackState()
					|| playExce ==PlayException.VIDEO_NOT_SUPPORT) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				try {
					mLogicManager.fastRewindVideo();
					setFast(1);
				}catch(IllegalStateException e){
					MtkLog.d(TAG, "Exception" + e.getMessage());
					featureNotWork(getString(R.string.mmp_buffer_not_enough));
				} catch (Exception e) {
					MtkLog.d(TAG, "Exception" + e.getMessage());
					featureNotWork(getString(R.string.mmp_featue_notsupport));
				}
			
			}
			return true;
		}
		
		case KeyMap.KEYCODE_MTKIR_MTSAUDIO: {
			reSetController();
			if (isValid()) {
				if (!mLogicManager.isInPlaybackState()) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				if (null != mControlView) {
					if(mControlView.changeVideoTrackNumber()){
						if(playExce == PlayException.AUDIO_NOT_SUPPORT){
							playExce = PlayException.DEFAULT_STATUS;
						}
						hideFeatureNotWork();
					}
				}
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_REPEAT: {
			reSetController();
			if (isValid()) {
				if (!mLogicManager.isInPlaybackState()) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				onRepeat();
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_EJECT: {
			reSetController();
			if (isValid()) {
				if (!mLogicManager.isInPlaybackState()) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				try {
					mLogicManager.slowForwardVideo();
					setFast(2);
				}catch(IllegalStateException e){
					MtkLog.d(TAG, "Exception" + e.getMessage());
					featureNotWork(getString(R.string.mmp_buffer_not_enough));
				} catch (Exception e) {
					MtkLog.d(TAG, "Exception" + e.getMessage());
					featureNotWork(getString(R.string.mmp_featue_notsupport));
				}
				
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_STOP: {
			reSetController();
			if (isValid()) {
				if (!mLogicManager.isInPlaybackState()) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				hideFeatureNotWork();
				if ( mInfo != null && mInfo.isShowing()) {
					mInfo.dismiss();
				}
				if (playExce == PlayException.VIDEO_NOT_SUPPORT) {
					showFullSotpStatus();
					removeFeatureMessage();
				} else {
					showResumeDialog();
				}
			}
			
			return true;
		}
		case KeyMap.KEYCODE_BACK: {
			exitState = true;
			mHandler.removeMessages(DELAY_AUTO_NEXT);
			mLogicManager.finishVideo();
			dismissTimeDialog();
			dismissNotSupprot();
			finish();
			break;
		}
		case KeyMap.KEYCODE_MTKIR_ANGLE:
			exitState = true;
			mHandler.removeMessages(DELAY_AUTO_NEXT);
			break;
		case KeyMap.KEYCODE_MTKIR_MTKIR_CC: {
			reSetController();
			if (isValid()) {
				
				if (!mLogicManager.isInPlaybackState()
					|| playExce ==PlayException.VIDEO_NOT_SUPPORT) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				
				if (null != mControlView) {
					short index = (short) (mControlView.getSubtitleIndex() + 1);
					short number = mLogicManager.getSubtitleTrackNumber();
					if (number <= 0) {
						return true;
					}
					if (index >= number) {
						index = -1;
					}
					mControlView.setVideoSubtitle(number, index);
				}
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_ZOOM: {
			reSetController();
			if (isValid()) {
				if (!mLogicManager.isInPlaybackState()
						|| mLogicManager.getMaxZoom() == VideoConst.VOUT_ZOOM_TYPE_1X 
						|| SCREENMODE_NOT_SUPPORT
						|| mLogicManager.getVideoWidth() > 1920
			 			|| mLogicManager.getVideoHeight() > 1080
						) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				int scMode = 0;
				try {
					String mode = TVContent.getInstance(this).getStorage().get(
							"SCREENMODE_FILELIST");
					if (null != mode && mode.length()>0) {
						scMode = Integer.parseInt(mode);
					}
				} catch (Exception e) {
	                e.printStackTrace();
				}
				if (scMode == 5 || scMode ==3) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				
			
				int zoomType = mLogicManager.getCurZomm();
				if (zoomType >= VideoConst.VOUT_ZOOM_TYPE_1X
						&& zoomType < mLogicManager.getMaxZoom()) {
					zoomType++;
				} else {
					zoomType = VideoConst.VOUT_ZOOM_TYPE_1X;
				}
				mLogicManager.videoZoom(zoomType);
				if (null != mControlView) {
					mControlView.setZoomSize();
				}
			}
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_RECORD: {
			reSetController();
			
			if (!mLogicManager.isInPlaybackState()) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
			
			
			if(MenuConfigManager.getInstance(this).getDefault(MenuConfigManager.VIDEO_3D_MODE) != 0){
				featureNotWork(getString(R.string.mmp_featue_notsupport));
			} else {
				if ( SCREENMODE_NOT_SUPPORT ) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
					return true;
				}
				int palystatus=mLogicManager.getVideoPlayStatus();
				int speed = mLogicManager.getVideoSpeed();
				if ((mLogicManager.isPlaying() && speed == 1)
						|| palystatus == VideoConst.PLAY_STATUS_PAUSED
						/*|| palystatus == VideoConst.PLAY_STATUS_STEP*/) {
					if (mLogicManager.isPlaying() && speed == 1) {
						mControlView.onCapture();
						videoPlayStatus = true;
					} else {
						videoPlayStatus = false;
					}
					hideFeatureNotWork();
					hideController();
					Intent intent = new Intent(this,
							CaptureLogoActivity.class);
					intent.putExtra(CaptureLogoActivity.FROM_MMP,
							CaptureLogoActivity.MMP_VIDEO);
					startActivity(intent);
					isBackFromCapture = true;
				} else {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
				}
			}
			return true;
		}
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
			
			
	}

	private void dismissTimeDialog() {
		if (null != mTimeDialog && mTimeDialog.isShowing()) {
			mTimeDialog.dismiss();
		}
	}

	private void showFullSotpStatus() {
		mControlView.stop();
		mLogicManager.stopVideo();
		mControlView.setInforbarNull();
		dismissNotSupprot();
	}

	private void showResumeDialog() {

		if (mLogicManager.getVideoPlayStatus()!=VideoConst.PLAY_STATUS_PAUSED) {
		   mControlView.pause();
		}
		if(mLogicManager.getVideoPlayStatus()==VideoConst.PLAY_STATUS_PAUSED){
			mVideoStopDialog = new VideoDialog(this);
			mVideoStopDialog.show();
			WindowManager m = mVideoStopDialog.getWindow().getWindowManager();
			Display display = m.getDefaultDisplay();
				mVideoStopDialog.setDialogParams(ScreenConstant.SCREEN_WIDTH, ScreenConstant.SCREEN_HEIGHT);
			mVideoStopDialog.setOnDismissListener(mDismissListener);
			hideController();
		}

	}

	private OnDismissListener mDismissListener = new OnDismissListener() {

		public void onDismiss(DialogInterface dialog) {
			if (mLogicManager.getVideoPlayStatus() == VideoConst.PLAY_STATUS_STOPPED) {
				mControlView.stop();
				mControlView.setInforbarNull();
				dismissNotSupprot();
			} else {
				if(!mLogicManager.isPlaying()){
					mControlView.setMediaPlayState();
				}
			}
			reSetController();
		}
	};

	private void setFast(int isForward) {

		if (null == mControlView) {
			return;
		}
        hideFeatureNotWork();
		int speed = mLogicManager.getVideoSpeed();
		mControlView.onFast(speed, isForward, Const.FILTER_VIDEO);
	}

	public void seek(int positon) {

		if (positon < 0) {
			positon = 0;
		} else if (positon > mLogicManager.getVideoDuration()) {
			positon = mLogicManager.getVideoDuration();
		}
		if (positon >= 0) {
			mLogicManager.seek(positon);
		}

	}

	private class VideoDialog extends Dialog {
		private Context mContext;

		public VideoDialog(Context context) {
			super(context, R.style.videodialog);

			this.mContext = context;
		}

		@Override
		protected void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			setContentView(R.layout.mmp_video_innerdialog);
			View layout = findViewById(R.id.mmp_video_innerdialog_pause);
			layout.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
										|View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
										|View.SYSTEM_UI_FLAG_LOW_PROFILE
										|View.SYSTEM_UI_FLAG_FULLSCREEN
										|View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
			if (null != mTipsDialog && mTipsDialog.isShowing()) {
				hideFeatureNotWork();
			}
		}

		@Override
		public boolean onKeyDown(int keyCode, KeyEvent event) {
			switch (keyCode) {
			case KeyMap.KEYCODE_MTKIR_STOP:
				mControlView.setVideoSubtitle((short) 1, (short) -1);
				dismissNotSupprot();
				mLogicManager.stopVideo();
				this.dismiss();
				return false;
			case KeyMap.KEYCODE_DPAD_CENTER:
				mControlView.reSetVideo();
				this.dismiss();
				return false;
			case KeyMap.KEYCODE_MTKIR_MUTE:
			case KeyMap.KEYCODE_VOLUME_UP:
			case KeyMap.KEYCODE_VOLUME_DOWN:
			case KeyMap.KEYCODE_MTKIR_ANGLE:
				if (null != mContext && mContext instanceof MediaPlayActivity) {
					((MediaPlayActivity) mContext).onKeyDown(keyCode, event);
				}
				return true;
			case KeyMap.KEYCODE_MTKIR_NEXT:
			case KeyMap.KEYCODE_MTKIR_PREVIOUS:
			case KeyMap.KEYCODE_MTKIR_PLAYPAUSE: {
				return true;
			}
			default:
				return false;
			}
		}

		public void setDialogParams(int width, int height) {
			Window window = getWindow();
			WindowManager.LayoutParams lp = window.getAttributes();
			lp.width = width;
			lp.height = height;
			window.setAttributes(lp);
		}
	}

	private class TimeDialog extends Dialog {

		private TextView mHour;

		private TextView mMinute;

		private TextView mSeconds;

		private int focusIndex = 0;

		//private int actionTag;

		private boolean mFocusChanged;
		public TimeDialog(Context context) {
			super(context, R.style.dialog);
		}

		@Override
		protected void onCreate(Bundle savedInstanceState) {
			super.onCreate(savedInstanceState);
			requestWindowFeature(Window.FEATURE_NO_TITLE);
			setContentView(R.layout.mmp_seek_time);

			WindowManager m = getWindow().getWindowManager();
			Display display = m.getDefaultDisplay();
			Window window = getWindow();
			WindowManager.LayoutParams lp = window.getAttributes();
			lp.x = -(int) (ScreenConstant.SCREEN_WIDTH * 0.2);
			lp.y = 0;
			window.setAttributes(lp);

			mHour = ((TextView) findViewById(R.id.time_hour));
			mMinute = ((TextView) findViewById(R.id.time_minute));
			mSeconds = ((TextView) findViewById(R.id.time_seconds));

		}

		@Override
		protected void onStart() {
			focusIndex = 0;
			setFocus();
			int progress = 0;
			mFocusChanged = true;
			if (null != mLogicManager) {
				progress = mLogicManager.getVideoProgress();
			}
			progress = (progress > 0 ? progress : 0);
			progress /= 1000;
			long minute = progress / 60;
			long hour = minute / 60;
			long second = progress % 60;
			minute %= 60;
			mHour.setText(String.format("%02d", hour));
			mMinute.setText(String.format("%02d", minute));
			mSeconds.setText(String.format("%02d", second));
			super.onStart();
		}

		@Override
		public boolean onKeyDown(int keyCode, KeyEvent event) {
			switch (keyCode) {
			case KeyMap.KEYCODE_MTKIR_ANGLE:{
				dismissTimeDialog();
				Util.exitMmpActivity(VideoPlayActivity.this);
				return true;
			}
			case KeyMap.KEYCODE_BACK:
				dismissTimeDialog();
				reSetController();
				return true;
			case KeyMap.KEYCODE_VOLUME_UP: {
				if (mLogicManager.isMute()) {
					onMute();
					return true;
				}
				currentVolume = currentVolume + 1;
				if (currentVolume > maxVolume) {
					currentVolume = maxVolume;
				}
				mLogicManager.setVolume(currentVolume);
				mControlView.setCurrentVolume(currentVolume);
				return true;
			}
			case KeyMap.KEYCODE_VOLUME_DOWN: {
				if (mLogicManager.isMute()) {
					onMute();
					return true;
				}
				currentVolume = currentVolume - 1;
				if (currentVolume < 0) {
					currentVolume = 0;
				}
				mLogicManager.setVolume(currentVolume);
				mControlView.setCurrentVolume(currentVolume);
				return true;
			}
			case KeyMap.KEYCODE_MTKIR_MUTE: {
				onMute();
				return true;
			}
			case KeyMap.KEYCODE_MTKIR_NEXT:
			case KeyMap.KEYCODE_MTKIR_PREVIOUS:
			case KeyMap.KEYCODE_MTKIR_PLAYPAUSE: {
				return true;
			}
			case KeyMap.KEYCODE_DPAD_CENTER: {
				removeFeatureMessage();
				hideFeatureNotWork();
				int hour = 0;
				int minute = 0;
				int seconds = 0;
				try {
					hour = Integer.valueOf(mHour.getText().toString());
					minute = Integer.valueOf(mMinute.getText().toString());
					seconds = Integer.valueOf(mSeconds.getText().toString());
				} catch (Exception e) {
					MtkLog.i(TAG, e.getMessage());
				}
				int time = (hour * 3600 + minute * 60 + seconds) * 1000;
				int total = mLogicManager.getVideoDuration();
				if (time >= total || time < 0) {
					featureNotWork(getString(R.string.mmp_time_out));
					return true;
				}
				dismiss();
				reSetController();
				try {
					removeProgressMessage();
					seek(time);
				} catch (Exception e) {
					featureNotWork(getString(R.string.mmp_featue_notsupport));
				}
				break;
				}
			case KeyMap.KEYCODE_DPAD_LEFT: {
				if (focusIndex > 0) {
					focusIndex -= 1;
				} else {
					focusIndex = 2;
				}
				setFocus();
				mFocusChanged = true;
				//actionTag = keyCode;
				break;
			}
			case KeyMap.KEYCODE_DPAD_RIGHT: {
				if (focusIndex >= 2) {
					focusIndex = 0;
				} else {
					focusIndex += 1;
				}
				setFocus();
				mFocusChanged = true;
				//actionTag = KeyMap.KEYCODE_DPAD_LEFT;
				break;
			}
			case KeyMap.KEYCODE_0:
			case KeyMap.KEYCODE_1:
			case KeyMap.KEYCODE_2:
			case KeyMap.KEYCODE_3:
			case KeyMap.KEYCODE_4:
			case KeyMap.KEYCODE_5:
			case KeyMap.KEYCODE_6:
			case KeyMap.KEYCODE_7:
			case KeyMap.KEYCODE_8:
			case KeyMap.KEYCODE_9: {
				setTime(keyCode - 7);
				//actionTag = keyCode;
				break;
			}
			case KeyMap.KEYCODE_DPAD_UP: {
				UpDownTime(1);
				mFocusChanged = false;
				break;
			}
			case KeyMap.KEYCODE_DPAD_DOWN: {
				UpDownTime(-1);
				mFocusChanged = false;
				break;
			}
			default:
				break;
			}
			return super.onKeyDown(keyCode, event);
		}

		private void UpDownTime(int offset) {
			switch (focusIndex) {
			case 0: {
				int value = Integer.valueOf(mHour.getText().toString())
						+ offset;
				if (value <= 9 && value >= 0) {
					mHour.setText("0" + value);
				} else if (value > 9 && value < 100) {
					mHour.setText("" + value);
				} else if (value >= 100) {
					mHour.setText(R.string.mmp_time_inti);
				} else {
					mHour.setText("99");
				}
				break;
			}
			case 1: {
				int value = Integer.valueOf(mMinute.getText().toString())
						+ offset;
				if (value <= 9 && value >= 0) {
					mMinute.setText("0" + value);
				} else if (value > 59) {
					mMinute.setText(R.string.mmp_time_inti);
				} else if (value < 0) {
					mMinute.setText("59");
				} else {
					mMinute.setText("" + value);
				}

				break;
			}
			case 2: {

				int value = Integer.valueOf(mSeconds.getText().toString())
						+ offset;
				if (value <= 9 && value >= 0) {
					mSeconds.setText("0" + value);
				} else if (value > 59) {
					mSeconds.setText(R.string.mmp_time_inti);
				} else if (value < 0) {
					mSeconds.setText("59");
				} else {
					mSeconds.setText("" + value);
				}
				break;
			}
			default:
				break;
			}
		}

		private void setTime(int value) {

			switch (focusIndex) {
			case 0: {
				setValue(mHour, value);
				break;
			}
			case 1: {
				setValue(mMinute, value);
				break;
			}
			case 2: {
				setValue(mSeconds, value);
				break;
			}
			default:
				break;
			}

		}

		private void setValue(TextView v, int key) {
			MtkLog.d(TAG,"setValue mFocusChanged ="+mFocusChanged+"focusIndex ="+focusIndex);
			if (mFocusChanged) {
				setFocus();
				v.setText("0" + key);
				mFocusChanged = false;
				return;
			}else{
				int value = Integer.valueOf((v.getText().toString()).substring(1))* 10 + key;
				if(value > 59 && focusIndex != 0){

					value = 59;
					v.setText(value + "");

				}else{
					v.setText((v.getText().toString()).substring(1) + key);


				}


				focusIndex = (++focusIndex)%3 ;
				
				mFocusChanged = true;


			}

			
			/*
			int value = Integer.valueOf(v.getText().toString());
			if (value == 0) {
				v.setText("0" + key);
			} else if (value <= 9) {
				int temp = value * 10 + key;
				if (temp > 59 && focusIndex != 0) {
					v.setText("59");
				} else {
					v.setText(value + "" + key);
				}
			} else if (actionTag == KeyMap.KEYCODE_DPAD_LEFT) {
				v.setText("0" + key);
			} else if (focusIndex == 2) {
				focusIndex = 0;
				setFocus();
				mHour.setText("0" + key);
			} else {
				focusIndex++;
				setFocus();
				if (focusIndex == 1) {
					mMinute.setText("0" + key);
				} else if (focusIndex == 2) {
					mSeconds.setText("0" + key);
				}

			}*/

		}

		private void setFocus() {

			mHour.setTextColor(Color.WHITE);
			mMinute.setTextColor(Color.WHITE);
			mSeconds.setTextColor(Color.WHITE);
			switch (focusIndex) {
			case 0: {
				mHour.setTextColor(Color.RED);
				break;
			}
			case 1: {
				mMinute.setTextColor(Color.RED);
				break;
			}
			case 2: {
				mSeconds.setTextColor(Color.RED);
				break;
			}
			default:
				break;
			}

		}

	}

	protected void hideControllerDelay() {
		mHandler.removeMessages(HIDE_CONTROLER);
		mHandler.sendEmptyMessageDelayed(HIDE_CONTROLER, HIDE_DELAYTIME);
	}

	protected void onPause() {
		MtkLog.d(TAG, "onPause()");
		if (!isBackFromCapture) {
			hideFeatureNotWork();
		}
		dismissMenuDialog();
		video_player_Activity_resumed=false;
		super.onPause();
	}
	protected void onDestroy() {
		super.onDestroy();
		if(mDevManager!=null&&mDevListener!=null){
			mDevManager.removeDevListener(mDevListener);
		}
		MtkLog.e(TAG, "Video Play Activity onDestroy! ");
		mHandler.removeMessages(PROGRESS_CHANGED);
		mHandler.removeMessages(HIDE_CONTROLER);
		mHandler.removeMessages(MSG_GET_CUR_POS);
		if(!exitState){
			mLogicManager.finishVideo();

		}
		video_player_Activity_resumed=false;

	}
	
	private DevManager mDevManager = null;
	private MyDevListener mDevListener = null;
	private class MyDevListener implements DevListener{
		public void onEvent(DeviceManagerEvent event) {
			MtkLog.d(TAG, "Device Event : " + event.getType());
			int type = event.getType();

			switch (type) {
			case DeviceManagerEvent.umounted:
				MtkLog.d(TAG, "Device Event Unmounted!!");
				exitState = true;
				mHandler.removeMessages(DELAY_AUTO_NEXT);
				mLogicManager.finishVideo();
				dismissTimeDialog();
				dismissNotSupprot();
				finish();
				break;

			default:
				break;
			}
		}
	};
	private void onRegisterUsbEvent(){
		try {
			mDevListener = new MyDevListener();
			mDevManager = DevManager.getInstance();
			mDevManager.addDevListener(mDevListener);
		} catch (ExceptionInInitializerError e) {
			mDevManager = null;
			mDevListener = null;
		}
	}
}
