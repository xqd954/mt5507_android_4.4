package com.mediatek.ui.mmp.util;

import java.io.File;
import java.util.Vector;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Bitmap;
import android.os.IBinder;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;
//import android.widget.ScrollView;
//import android.widget.TextView;
import android.graphics.Rect;
import com.mediatek.ui.util.ScreenConstant;

import com.mediatek.common.PhotoPlayer.MtkPhotoHandler;
import com.mediatek.common.PhotoPlayer.NotSupportException;
import com.mediatek.mmp.MtkMediaPlayer.OnCompletionListener;
import com.mediatek.mmp.MtkMediaPlayer.OnErrorListener;
import com.mediatek.mmp.MtkMediaPlayer.OnInfoListener;
import com.mediatek.mmp.MtkMediaPlayer.OnPreparedListener;
import com.mediatek.mmp.MtkMediaPlayer.OnSeekCompleteListener;
import com.mediatek.mmpcm.CommonSet;
import com.mediatek.mmpcm.audioimpl.AudioConst;
import com.mediatek.mmpcm.audioimpl.CorverPic;
import com.mediatek.mmpcm.audioimpl.Lyric;
import com.mediatek.mmpcm.audioimpl.LyricTimeContentInfo;
import com.mediatek.mmpcm.audioimpl.PlaybackService;
import com.mediatek.mmpcm.audioimpl.PlaybackService.LocalBinder;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.mmcimpl.PlayList;
import com.mediatek.mmpcm.photoimpl.Capture;
import com.mediatek.mmpcm.photoimpl.ConstPhoto;
import com.mediatek.mmpcm.photoimpl.EffectView;
import com.mediatek.mmpcm.photoimpl.Imageshowimpl;
import com.mediatek.mmpcm.photoimpl.Imageshowimpl.OnPhotoCompletedListener;
import com.mediatek.mmpcm.photoimpl.Imageshowimpl.OnPhotoDecodeListener;
import com.mediatek.mmpcm.photoimpl.PhotoUtil;
//import com.mediatek.mmpcm.text.ITextEventListener;
//import com.mediatek.mmpcm.text.ITextReader;
//import com.mediatek.mmpcm.textimpl.TextReader;
import com.mediatek.mmpcm.threedimen.photo.IThrdPhotoEventListener;
import com.mediatek.mmpcm.threedimen.photoimpl.MPlayback;
import com.mediatek.mmpcm.threedimen.photoimpl.PhotoManager;
import com.mediatek.mmpcm.videoimpl.VideoComset;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.mmpcm.videoimpl.VideoManager;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.mmp.util.AsyncLoader.LoadWork;
import com.mediatek.ui.util.MtkLog;

public class LogicManager {

	private static final String TAG = "LogicManager";

	private static LogicManager mLogicManager = null;

	private VideoManager mVideoManager;

	private PlayList mPlayList;


	private PlaybackService mAudioPlayback = null;

	private Intent serviceIntent;

	private ServiceConnection serviceConnection;

	private Lyric mLyric;

	private Object mPreparedListener;

	private Object mCompletionListener;

	private Object mInfoListener;

	private Object mErrorListener; // fix bug by hs_haizhudeng

	private Imageshowimpl mImageManager;

	private PhotoManager mPhotoManager; // add by lei

	private MPlayback mPhotoPlayback; // add by lei

	private CommonSet mmpset;

	private Context mContext;

	private VideoComset mVideoComSet;

	private EffectView mImageEffectView;

	static public final int MMP_EQ_ELEM_NUM = 10;

	static public final int MMP_EQ_MAX = 0x3FFFFF;

	static public final int MMP_EQ_MIN = 0x000FFF;

	private UnmountLisenter unmountLisenter;

	private TVConfigurer mtvcfg;

	private Capture mCapturer;

	private AsyncLoader<Integer> mPlayLoader;

	private int mPlayFlag = 0;
	
	private boolean is3DPhotoMpo=false;
	
	private String thrdMode;
	
	private int thrdModeValue =1;

	private class PlayWork implements LoadWork<Integer> {

		private MPlayback mPlayBack;
		private String mPath;
		private int mSource;

		public PlayWork(MPlayback playBack, String path, int source ) {
			mPlayBack = playBack;
			mPath = path;
			mSource = source;

		}

		public Integer load() {
			MtkLog.d(TAG,"playwork load mPlayBack = "+mPlayBack);
			if (mPlayBack == null) {
				mPlayFlag = -1;
			} else {
				setPicSetting();
				mPlayBack.decode3DPhoto(mPath,mSource);
				mPlayFlag = 1;
			}
			return mPlayFlag;
		}

		public void loaded(Integer result) {
			// TODO Auto-generated method stub

		}

	}

	private LogicManager(Context context) {
		//mPlayLoader = new AsyncLoader<Integer>(1);
		mPlayLoader = AsyncLoader.getInstance(1);
		mPlayList = PlayList.getPlayList();
		mContext = context;
		mtvcfg = TVContent.getInstance(context)
				.getConfigurer();

		mmpset = CommonSet.getInstance(mContext);
		// add by shuming for Fix CR DTV00401969
		mCapturer = new Capture();
		// MtkLog.i("", "--------EffectView------");
		//mEffectView = new EffectView(context);
	}

	public static LogicManager getInstance(Context context) {

		if (null == mLogicManager) {
			mLogicManager = new LogicManager(context);
		}
		return mLogicManager;
	}


	public void initVideo(SurfaceView surface, int videoSource) {

		mPlayList = PlayList.getPlayList();
		// mPlayList.setRepeatMode(Const.FILTER_VIDEO, Const.REPEAT_NONE);
		mVideoManager = VideoManager.getInstance(surface, videoSource);
		//mVideoManager.setVideoRect(new Rect(0,0,ScreenConstant.SCREEN_WIDTH,ScreenConstant.SCREEN_HEIGHT));
		mVideoManager.setPlayerMode(videoSource);
		mVideoManager.setPreviewMode(false);

		if (null == mVideoComSet) {
			mVideoComSet = mVideoManager.getComset();
		} else {
			mVideoComSet.videoZoom(0);
		}
		new Thread(new Runnable() {
			public void run() {
				try {
					mVideoManager.setDataSource(mPlayList
							.getCurrentPath(Const.FILTER_VIDEO));
				} catch (IllegalArgumentException e) {
					MtkLog.e(TAG, e.getMessage());
				}  catch (Exception e) {
					MtkLog.e(TAG, e.getMessage());
				}
			}
		}).start();

	}

	/*--------------------------------------- Video --------------------------------*/


	public void freeVideoResource() {
		mmpset.mmpFreeVideoResource();
	}

	public void restoreVideoResource() {
		mmpset.mmpRestoreVideoResource();
	}





	public void setCapturer(View view) {
		if (null == mCapturer) {
			mCapturer = new Capture();
		}
		mCapturer.captureImage(view);
	}

	public int getNativeBitmap() {
		return mCapturer.getNativeBitmap();
	}

	public int getWidth() {
		return mCapturer.getWidth();
	}

	public int getHeight() {
		return mCapturer.getHeight();
	}

	public int getPitch() {
		return mCapturer.getPitch();
	}

	public int getMode() {
		return mCapturer.getColorMode();
	}

	public void setSubtitleTrack(short index) {
		if (null == mVideoManager) {
			return;
		}
		MtkLog.i(TAG, "------setSubtitleTrack  index:" + index);
		mVideoManager.setSubtitleTrack(index);
	}

	public void setSubOnOff(boolean flag) {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.setSubOnOff(flag);
	}

	public short getSubtitleTrackNumber() {
		if (null == mVideoManager) {
			return 0;
		}
		return mVideoManager.getSubtitleTrackNumber();
	}

	public int getAudioTranckNumber() {
		if (null == mVideoManager) {
			return 0;
		}
		return mVideoManager.getAudioTranckNumber();

	}

	public boolean setAudioTranckNumber(short mtsIdx) {
		if (null == mVideoManager) {
			return false;
		}
		return mVideoManager.selectMts(mtsIdx);
	}
	public boolean isPlaying() {
		if (null == mVideoManager) {
			return false;
		}
		return mVideoManager.isPlaying();

	}

	public int[] getAvailableScreenMode() {
		return mmpset.getAvailableScreenMode();
	}

	public void videoZoom(int zoomType) {
		mVideoComSet.videoZoom(zoomType);
	}

	/**
	 * set picture zoom type 1X, 2X 4X.
	 * 
	 * @param zoomType
	 */
	public void setPicZoom(int zoomType) {
		if (mImageEffectView != null) {
			mImageEffectView.setMultiple(zoomType);
		}
	}

	/**
	 * get current setting zoom value
	 * 
	 * @return
	 */
	public int getPicCurZoom() {
		if (mImageEffectView == null) {
			return 1;
		}
		return mImageEffectView.getMultiple();
	}

	public int getCurZomm() {
		if (null == mVideoComSet) {
			return 1;
		}
		return mVideoComSet.getCurZoomType();
	}

	public int getMaxZoom() {
		return mVideoComSet.getMaxZoom();
	}

	public void videoZoomReset() {
		if (mVideoComSet == null) {
			mVideoComSet = new VideoComset();
		}
		mVideoComSet.videoZoomReset();
	}

	public String getVideoPageSize() {
		return (mPlayList.getCurrentIndex(Const.FILTER_VIDEO) + 1) + "/"
				+ mPlayList.getFileNum(Const.FILTER_VIDEO);
	}

	public void setOnInfoListener(Object infoListener){
		if(null != mVideoManager){
			mVideoManager.setOnInfoListener(infoListener);
		}
	}
	public void playVideo() {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.startVideo();
	}
	public void autoNext(){
		if (null == mVideoManager) {
			return;
		}
		try {
			mVideoManager.autoNext();
		} catch (IllegalStateException e) {
			MtkLog.e(TAG, "IllegalStateException:"+e.getMessage());
			throw new IllegalStateException(e);
		}

	}

	public void pauseVideo() {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.pauseVideo();
	}

	public void stepVideo() {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.step();
	}

	public void stopVideo() {
		if (null == mVideoManager) {
			return;
		}
		try {
			mVideoManager.stopVideo();
		} catch (IllegalStateException e) {
			MtkLog.e(TAG, "stop  " + e.getMessage());
		}
	}

	public void finishVideo() {
		if (null == mVideoManager) {
			MtkLog.e(TAG, "finishVideo  is null");
			return;
		}
		try {
			mVideoManager.stopVideo();
			MtkLog.i(TAG, " stop video");
			
		} catch (IllegalStateException e) {
			MtkLog.e(TAG, "stop  " + e.getMessage());
		}
		try {
			mVideoManager.onRelease();
			mVideoManager = null;
			MtkLog.i(TAG, "OnRelease VideoPlayback ");
			/* Had closed video play and send broadcast tell it */
		} catch (Exception e) {
			mVideoManager = null;
			MtkLog.e(TAG, "onRelease  " + e.toString());
		}

	}

	public void sendCloseBroadCast() {

		Intent intent = new Intent(MultiMediaConstant.STOPMUSIC);
		mContext.sendBroadcast(intent);
		MtkLog.e(TAG, "Video Play Activity sendCloseVideoBroadCast ! ");
		clearAudio();
	}

	/**
	 * Play prev video.
	 * 
	 * @return -1, play failed, 0, successful.
	 */
	public int playPrevVideo() {
		if (null == mVideoManager) {
			return -1;
		}
		try {
			mVideoManager.playPrev();
		} catch (Exception e) {
			e.printStackTrace();
			return -1;
		}
		return 0;
	}

	/**
	 * Replay video.
	 * 
	 * @return -1, replay failed, 0, successful.
	 */
	public int replayVideo() {
		if (null == mVideoManager) {
			return -1;
		}
		try {
			mVideoManager.replay();
		} catch (Exception e) {
			e.printStackTrace();
			return -1;
		}
		return 0;
	}

	/**
	 * Play next video.
	 * 
	 * @return -1, play failed, 0, successful.
	 */
	public int playNextVideo() {
		if (null == mVideoManager) {
			return -1;
		}
		try {
			mVideoManager.playNext();
		} catch (Exception e) {
			e.printStackTrace();
			return -1;
		}

		return 0;
	}

	// add by shuming fix CR00385698
	/**
	 * 
	 * @param featurenotsurport
	 */

	/**
	 * 
	 * @return isVideoFeaturenotsurport
	 */

	// end
	public void onDevUnMount() {
		if(unmountLisenter != null){
			MtkLog.e(TAG,"unmount dismiss music view~~~");
			unmountLisenter.onUnmount();
		}
		finishAudioService();
		finishVideo();
	}

	public void slowForwardVideo()  {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.slowForward();

	}

	public void fastForwardVideo() {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.fastForward();

	}

	public void fastRewindVideo() {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.fastRewind();

	}
	
	public boolean canDoSeek() {
		if (null == mVideoManager) {
			return false;
		}
		return mVideoManager.canDoSeek();
	}

	public int getVideoSpeed() {
		if (null == mVideoManager) {
			return 0;
		}
		return mVideoManager.getPlaySpeed();
	}

	public void seek(int positon){
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.seek(positon);
	}

	public boolean isVideoFast() {
		if (null == mVideoManager) {
			return false;
		}
		return (mVideoManager.getPlayStatus() == VideoConst.PLAY_STATUS_FR)
				|| (mVideoManager.getPlayStatus() == VideoConst.PLAY_STATUS_FF);
	}

	public int getVideoDuration() {
		return mVideoManager.getDuration();
	}

	/**
	 * Get video width;
	 * 
	 * @return
	 */
	public int getVideoWidth() {
		int width = 0;
		if (mVideoManager != null) {
			width = mVideoManager.getVideoWidth();
		}
		return width;
	}

	/**
	 * Get video height.
	 * 
	 * @return
	 */
	public int getVideoHeight() {
		int heghit = 0;
		if (mVideoManager != null) {
			heghit = mVideoManager.getVideoHeight();
			// end
		}

		return heghit;
	}

	public String getVideoTitle() {
		if (null != mVideoManager) {
			return mVideoManager.getVideoTitle();
		}
		return "";
	}

	public String getVideoCopyright() {
		if (null != mVideoManager) {
			return mVideoManager.getVideoCopyright();
		}
		return "";
	}

	public String getVideoYear() {
		if (null != mVideoManager) {
			return mVideoManager.getVideoYear();
		}
		return "";
	}

	public String getVideoGenre() {
		if (null != mVideoManager) {
			return mVideoManager.getVideoGenre();
		}
		return "";
	}

	public String getVideoDirector() {
		if (null != mVideoManager) {
			return mVideoManager.getVideoDirector();
		}
		return "";
	}
	public long getVideoFileSize(){
		if (null == mVideoManager) {
			return 0;
		}
		return mVideoManager.getFileSize();
	}

	public int getVideoBytePosition(){
		if (null == mVideoManager) {
			return 0;
		}
		return mVideoManager.getBytePosition();
	}
	public int getVideoProgress() {
		if (null == mVideoManager) {
			return 0;
		}
		return mVideoManager.getProgress();
	}

	public boolean isInPlaybackState(){
		if (null == mVideoManager) {
			return false;
		}
		return mVideoManager.isInPlaybackState();
	}


	/*------------------- mmpset -----------------------*/
	public int getVolume() {
		if (null == mmpset) {
			return 0;
		}
		return mmpset.getVolume();
	}

	public void setVolume(int volume) {
		if (null == mmpset) {
			return;
		}
		mmpset.setVolume(volume);
	}
	
	public void setVolumeUp(){
		if (mLogicManager.isMute()) {
			setMute();
			return;
		}
		int maxVolume = mLogicManager.getMaxVolume();
		int currentVolume = mLogicManager.getVolume();
		currentVolume = currentVolume + 1;
		if (currentVolume > maxVolume) {
			currentVolume = maxVolume;
		}
		mLogicManager.setVolume(currentVolume);
	}
	
	public void setVolumeDown(){
		if (mLogicManager.isMute()) {
			setMute();
			return;
		}
		int currentVolume = mLogicManager.getVolume();
		currentVolume = currentVolume - 1;
		if (currentVolume < 0) {
			currentVolume = 0;
		}
		mLogicManager.setVolume(currentVolume);
	}

	public void setAudioOnly(boolean switchFlag) {
		if (null == mmpset) {
			return;
		}

		mmpset.setAudOnly(switchFlag);
	}

	public boolean isAudioOnly() {
		if (null == mmpset) {
			return false;
		}
		return mmpset.getAudOnly();
	}

	public int[] getAudSpectrum() {
		int[] valueArray = new int[15];
		int[] array = mtvcfg.getSpectrum();
		if (array != null) {
			for (int i = 0; i < array.length; i++) {
				valueArray[i] = (MMP_EQ_ELEM_NUM * (array[i] - MMP_EQ_MIN) / (MMP_EQ_MAX - MMP_EQ_MIN));
				MtkLog.d(TAG,"getAudSpectrum valueArray[i] ="+valueArray[i] +" array[i] ="+array[i]);
			}
		}
		return valueArray;
	}

	public int getMaxVolume() {
		return mmpset.getMaxVolume();
	}

	/**
	 * set picture mode
	 * 
	 * @param type
	 */
	public void setPictureMode(int type) {
		mmpset.setPictureMode(type);
	}

	/**
	 * set screen mode
	 * 
	 * @param type
	 */
	public void setScreenMode(int type) {
		mmpset.setScreenMode(type);
	}

	public int getCurPictureMode() {
		return mmpset.getCurPictureMode();
	}

	public int getCurScreenMode() {
		if (null == mVideoManager) {
			return 0;
		}
		return mmpset.getCurScreenMode();
	}

	public void setMute() {
		if (null == mmpset) {
			return;
		}
		mmpset.setMute();
	}

	public boolean isMute() {
		if (null == mmpset) {
			return false;
		}
		return mmpset.isMute();
	}

	public int getVideoPlayStatus() {
		if (null == mVideoManager) {
			return VideoConst.PLAY_STATUS_INITED;
		}
		return mVideoManager.getPlayStatus();
	}



	public String getFileDuration() {
		if (null == mVideoManager) {
			return "";
		}
		return mVideoManager.getVideoYear();
	}

	public String getFileName() {
		if (null == mVideoManager) {
			return "";
		}
		String filename = mVideoManager.getCurFileName();
		Log.e(TAG,"getFilename:"+filename);
		try {
			return filename.substring(filename.lastIndexOf("/") + 1);

		} catch (Exception e) {
			MtkLog.d(TAG, "IllegalStateException:"+e.toString());
			return null;
		}
	}



	public void setVideoPreparedListener(Object listener) {
		if (null == mVideoManager) {
			return;
		}
		mVideoManager.setOnPreparedListener(listener);
	}



	public void setCompleteListener(Object listener) {
		mVideoManager.setOnCompletionListener(listener);// setOnPBCompleteListener(listener);
	}

	/*-------------------aduido ------------------*/
	private int mAudioSource;


	public int getAudioSourceType(){
		return mAudioSource;

	}

	// change by browse fix CR DTV00384318
	/**
	 * New Service when service not exist.
	 * */
	private void initService(Context context) {
		serviceIntent = new Intent(context,
				com.mediatek.mmpcm.audioimpl.PlaybackService.class);
		serviceIntent.putExtra(PlaybackService.PLAY_TYPE, mAudioSource);
		serviceConnection = new ServiceConnection() {
			public void onServiceConnected(ComponentName name, IBinder service) {
				LocalBinder binder = (LocalBinder) service;
				mAudioPlayback = binder.getService();
				startPlayAudio(mAudioSource);
			}

			public void onServiceDisconnected(ComponentName name) {
				MtkLog.d(TAG, "onServiceDisconnected");
			}

		};
		startService(context);
		bindService(context);
	}

	public void initAudio(Context context, final int audioSource) {
		mPlayList = PlayList.getPlayList();
		mmpset = CommonSet.getInstance(context);
		mAudioSource = audioSource;
		if (mAudioPlayback == null) {
			initService(context);
		} else {
			if (mAudioPlayback.getPlayStatus() < AudioConst.PLAY_STATUS_STOPPED) {
				stopAudio();
			}
			bindService(context);
		}
	}

	public void clearAudio() {
		mAudioPlayback = null;
		unmountLisenter = null;
	}
	// end
	private void startPlayAudio(int audioSource) {
		mAudioPlayback.registerAudioPreparedListener(mPreparedListener);
		mAudioPlayback.registerAudioCompletionListener(mCompletionListener);
		mAudioPlayback.setPlayMode(audioSource);
		mAudioPlayback.registerAudioErrorListener(mErrorListener);
		mAudioPlayback.registerInfoListener(mInfoListener);
		mAudioPlayback.setDataSource(mPlayList.getCurrentPath(Const.FILTER_AUDIO));






	}

	public void startService(Context context) {
		context.startService(serviceIntent);
	}

	public void stopService(Context context) {
		context.stopService(serviceIntent);
	}

	public void bindService(Context context) {
		try {
			context.bindService(serviceIntent, serviceConnection,
					Context.BIND_AUTO_CREATE);
		} catch (Exception e) {
			MtkLog.e(TAG, "Exception:"+e.toString());
		}

	}

	public void unbindService(Context context) {
		try {
			context.unbindService(serviceConnection);
		} catch (Exception e) {
			MtkLog.e(TAG, "Exception:"+e.toString());
		}

	}

	public PlaybackService getAudioPlaybackService() {
		return mAudioPlayback;
	}

	public void playAudio() {
		if(null==mAudioPlayback)
		{
			return;
		}
		try {
			mAudioPlayback.play();
		} catch (IllegalStateException e) {
			MtkLog.e(TAG, e.getMessage());
		}

	}

	public void pauseAudio() {
		try {
			mAudioPlayback.pause();
		} catch (IllegalStateException e) {
			MtkLog.e(TAG, e.getMessage());
			if (AudioConst.MSG_ERR_CANNOTPAUSE
					.equals(e.getMessage())) {
				throw new IllegalStateException(AudioConst.MSG_ERR_CANNOTPAUSE);
			}else if (AudioConst.MSG_ERR_PAUSEEXCEPTION.equals(e.getMessage())) {
				throw new IllegalStateException(
						AudioConst.MSG_ERR_PAUSEEXCEPTION);
			}else{
				throw new IllegalStateException(e.getMessage());
			}

		}

	}

	public void stopAudio() {
		MtkLog.d(TAG, "stopAudio");
		if (mAudioPlayback != null) {
			try {
				mAudioPlayback.stop();
			} catch (IllegalStateException e) {
				MtkLog.e(TAG, e.getMessage());
			}

		}

	}
	public interface UnmountLisenter{

		public void onUnmount();
	}

	public void registerUnMountLisenter(UnmountLisenter lisenter){
		MtkLog.e(TAG,"register unmount listener");
		unmountLisenter = lisenter;

	}

	public void finishAudioService() {
		if (null != mAudioPlayback) {
			try {
				mAudioPlayback.stop();
			} catch (IllegalStateException e) {
				MtkLog.e(TAG, e.getMessage());
			}
		}

	}

	public void playNextAudio() {
		if (null != mAudioPlayback) {
			try {
				mAudioPlayback.playNext();
			} catch (IllegalStateException e) {
				MtkLog.i(TAG, e.getMessage());
			}

		}
	}

	public void playPrevAudio() {
		if (null != mAudioPlayback) {
			try {
				mAudioPlayback.playPrevious();
			} catch (IllegalStateException e) {
				MtkLog.i(TAG, e.getMessage());
			}

		}
	}

	public void replayAudio() {
		if (null != mAudioPlayback) {
			try {
				mAudioPlayback.setDataSource(mAudioPlayback.getFilePath());
			} catch (IllegalStateException e) {
				MtkLog.i(TAG, e.getMessage());
			}

		}
	}

	public boolean isAudioFast() {
		if(null==mAudioPlayback)
		{
			return false;
		}
		return (mAudioPlayback.getPlayStatus() == AudioConst.PLAY_STATUS_FR)
				|| (mAudioPlayback.getPlayStatus() == AudioConst.PLAY_STATUS_FF)
				|| (mAudioPlayback.getPlayStatus() == AudioConst.PLAY_STATUS_SF);
	}

	public boolean isAudioPlaying() {

		if (null != mAudioPlayback) {
			return mAudioPlayback.isPlaying();
		}
		return false;
	}

	public boolean isAudioPause() {

		if (null == mAudioPlayback) {
			return false;
		}

		return mAudioPlayback.getPlayStatus() == AudioConst.PLAY_STATUS_PAUSED;
	}

	public boolean isAudioStarted() {
		if (null == mAudioPlayback) {
			return false;
		}
		int status = mAudioPlayback.getPlayStatus();
		return ((status >= AudioConst.PLAY_STATUS_STARTED) && (status < AudioConst.PLAY_STATUS_STOPPED));
	}


	public String getAudioFilePath(){
		if (null == mAudioPlayback) {
			return null;
		}

		return mAudioPlayback.getFilePath();


	}

	public boolean isAudioStoped() {
		if (null == mAudioPlayback) {
			return true;
		}
		return mAudioPlayback.getPlayStatus() == AudioConst.PLAY_STATUS_STOPPED;
	}

	public void seekToCertainTime(long time) throws IllegalStateException{
		if (null != mAudioPlayback) {
				mAudioPlayback.seekToCertainTime(time);
		}
	}
	public boolean canSeek(){
		return mAudioPlayback.canSeek();
	}
	public void fastForwardAudio() throws IllegalStateException {
		if (mAudioPlayback == null) {
			return;
		}

		
		mAudioPlayback.fastForward();

		

	}

	public int getAudioSpeed() {
		if (null == mAudioPlayback) {
			return 0;
		}
		return mAudioPlayback.getSpeed();
	}


	
	public long getAudioFileSize(){
		if (null == mAudioPlayback) {
			return 0;
		}
		return mAudioPlayback.getFileSize();
	}

	public void setAuidoSpeed(int speed) {
		if (null != mAudioPlayback) {
			mAudioPlayback.setSpeed(speed);
		}
	}

	public int getAudioStatus() {
		if (null == mAudioPlayback) {
			return -1;
		}
		return mAudioPlayback.getPlayStatus();
	}

	public void fastRewindAudio() throws IllegalStateException {
		if (mAudioPlayback == null) {
			return;
		}
		
		mAudioPlayback.fastRewind();
		

	}

	public int getPlaybackProgress() {
		if (mAudioPlayback != null) {
			return mAudioPlayback.getPlaybackProgress();
		} else {
			return 0;
		}

	}


	public int getAudioBytePosition(){
		if (null == mAudioPlayback) {
			return 0;
		}
		return mAudioPlayback.getCurrentBytePosition();
	}

	public int getTotalPlaybackTime() {
		if (null == mAudioPlayback) {
			return 0;
		}
		int dur = mAudioPlayback.getTotalPlaybackTime();
		MtkLog.d(TAG,"getTotalPlaybackTime dur ="+ dur);
		return dur > 0 ? dur : 0;
	}

	public Bitmap getAlbumArtwork(int srcType, String path, int width,
			int height) {
		return CorverPic.getInstance().getAudioCorverPic(srcType, path, width,
				height);
	}

	public String getMusicAlbum() {
		if(null==mAudioPlayback)
		{
			return "";
		}
		return mAudioPlayback.getAlbum();
	}

	public String getMusicArtist() {
		if(null==mAudioPlayback)
		{
			return "";
		}
		return mAudioPlayback.getArtist();
	}

	public String getMusicGenre() {
		return mAudioPlayback.getGenre();
	}

	public String getMusicTitle() {
		if (null == mAudioPlayback) {
			return "";
		}
		return mAudioPlayback.getTitle();
	}

	public String getMusicYear() {
		return mAudioPlayback.getYear();
	}
	public int getPlayStatus() {
		if (mAudioPlayback != null) {
			return mAudioPlayback.getPlayStatus();
		}
		return 0;
	}

	public Vector<LyricTimeContentInfo> getLrcInfo() {

		// TODO change
		Vector<LyricTimeContentInfo> lrcInfo = new Vector<LyricTimeContentInfo>();
		String mp3Path = mPlayList.getCurrentPath(Const.FILTER_AUDIO);
		try {
			if(null != mp3Path){
				int index = mp3Path.lastIndexOf(".");
				if (index == -1) {
					return lrcInfo;
				}
				String lrcPath = mp3Path.substring(0, index) + ".lrc";
				MtkLog.i(TAG, "  lrcPath =" + lrcPath + "  mp3Path=" + mp3Path);
				File lrcFile = new File(lrcPath);
				if (lrcFile.exists()) {
					mLyric = new Lyric(lrcPath);
					lrcInfo = mLyric.getLyricTimeContentInfo();
				}
			}
		} catch (Exception e) {
			MtkLog.i(TAG, e.getMessage());
			return null;
		}
		return lrcInfo;
	}

	public int getLrcLine(long time) {
		if (mLyric != null) {
			return mLyric.getLine(time);
		} else {
			return 0;
		}
	}

	public String getCurrentPath(int type) {
		return mPlayList.getCurrentPath(type);
	}

	// public String getAudioFilenmae() {
	//
	// return mPlayList.getCurrentFileName(Const.FILTER_AUDIO);
	// }

	public String getAudioPageSize() {
		return (mPlayList.getCurrentIndex(Const.FILTER_AUDIO) + 1) + "/"
				+ mPlayList.getFileNum(Const.FILTER_AUDIO);
	}

	public void setAudioPreparedListener(Object listener) {

		mPreparedListener = listener;

	}

	// fix bug by hs_hzd
	public void setAudioErrorListener(Object listener) {

		mErrorListener = listener;

	}

	public void setVideoErrorListener(Object listener){
		if(mVideoManager!= null){
			mVideoManager.setOnErrorListener(listener);
		}
	}
	public void setSeekCompleteListener(Object listener) {
		if (null != mVideoManager) {
			mVideoManager.setOnSeekCompleteListener(listener);
		}
	}
	public void removeErrorListener() {
		if (mAudioPlayback != null) {
			mAudioPlayback.unregisterAudioErrorListener();
			mErrorListener = null;
		}
	}

	public void setAudioCompletionListener(Object listener) {

		mCompletionListener = listener;

	}

	
	public void setAudioInfoListener(Object listener) {
		mInfoListener = listener;
	}

	public int getRepeatModel(int type) {
		if (null == mPlayList) {
			return 0;
		}
		return mPlayList.getRepeatMode(type);
	}

	public boolean getShuffleMode(int fileType) {
		return mPlayList.getShuffleMode(fileType);
	}

	public void setShuffle(int type, boolean model) {
		mPlayList.setShuffleMode(type, model);
	}

	public void initPhoto(Display display, EffectView view) {
		mPlayList = PlayList.getPlayList();
		mImageEffectView = view;
		if (null == mImageManager) {
			mImageManager = new Imageshowimpl(display);
		}
	}
	
	public void initPhotoFor4K2K(Display display) {
		mPlayList = PlayList.getPlayList();
		if (null == mImageManager) {
			mImageManager = new Imageshowimpl(display);
		}
	}

	public int getPicIndex(){
		String paths[] = {".jpg",".png",".gif",".bmp"};
		String path = mPlayList.getCurrentPath(Const.FILTER_IMAGE);
		MtkLog.i(TAG,"cur file path:"+path);
		try{
			for(int index = 0; index < paths.length ; index++){
				if( paths[index].equalsIgnoreCase(path.substring(path.lastIndexOf('.')))){
					return index;
				}
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		return 5;

	}
	
	public void stopDecode() {
		if (null != mImageManager) {
			new Thread(new Runnable() {
				public void run() {
					mImageManager.stopDecode();
				}
			}).start();
		}
	}
	
	public void initThrdPhoto(Context context) {
		mPlayList = PlayList.getPlayList();
		mPhotoPlayback = PhotoManager.getInstance(context).getPlayback();

	}

	public void stopPlayWork() {

		if (null != mPlayLoader) {
			mPlayLoader.clearQueue();
			mPlayLoader = null;
		}
	}

	public void playThrdPhoto(int type) {
		try {
			String path = null;

			if(type == Const.CURRENTPLAY){
				path = mPlayList.getCurrentPath(Const.FILTER_IMAGE);
			}else{
				path = mPlayList.getNext(Const.FILTER_IMAGE, type);
			}
			
			MtkLog.d(TAG,"playThrdPhoto path ="+path);
			
			if (mPlayLoader != null) {
				mPhotoPlayback.cancel();
				mPhotoPlayback.close();
				mPlayLoader.clearQueue();
				mPlayLoader.addWork(new PlayWork(mPhotoPlayback, path,mPlayList.getSource()));
			}

		} catch (NotSupportException e) {
			e.printStackTrace();
		}
	}

	public void closeThrdPhoto() {
		try {
			if (mPhotoPlayback != null) {
				new Thread(new Runnable() {

					public void run() {

						mPhotoPlayback.close();
					}

				}).start();

			}
		} catch (NotSupportException e) {

		}
	}

	public void setPhotoCompleteListener(
			OnPhotoCompletedListener completeListener) {
		mImageManager.setCompleteListener(completeListener);
	}

	public void setPhotoDecodeListener(OnPhotoDecodeListener decodeListener) {
		mImageManager.setDecodeListener(decodeListener);
	}

	public PhotoUtil transfBitmap(String path,int source) {

		if (null == path || path.length()<=0) {
			return null;
		}
		try {
			mImageManager.setLocOrNet(source);
			return mImageManager.getPhoto(path);

		} catch (OutOfMemoryError error) {
			MtkLog.i(TAG, " transfBitmap  " + error.getMessage());
			return null;
		}
	}

	public void setImageSource(int source) {
		mImageManager.setLocOrNet(source);
	}





	public PhotoUtil loadImageBitmap(int type) {
	
		return mImageManager.loadBitmap(type);
	}

	

	
	public int getImageEffect() {
		if (null == mImageEffectView) {
			return ConstPhoto.DEFAULT;
		}
		return mImageEffectView.getEffectValue();
	}

	public Bitmap setLeftRotate(Bitmap bitmap) {
		return mImageManager.leftRotate(bitmap);
	}

	public Bitmap setRightRotate(Bitmap bitmap) {
		return mImageManager.rightRotate(bitmap);
	}

	public void zoomImage(ImageView view, int inOrOut, Bitmap bitmap, int size) {
		mImageManager.Zoom(view, inOrOut, bitmap, size);
	}

	// add by xiaojie fix cr DTV00389237
	public int getCurrentZoomSize() {
		return mImageManager.getZoomOutSize();
	}

	public int getCurrentImageIndex() {

		return mPlayList.getCurrentIndex(Const.FILTER_IMAGE) + 1;
	}

	public int getImageNumber() {

		return mPlayList.getFileNum(Const.FILTER_IMAGE);
	}

	public String getImagePageSize() {
		return (mPlayList.getCurrentIndex(Const.FILTER_IMAGE) + 1) + "/"
				+ mPlayList.getFileNum(Const.FILTER_IMAGE);
	}

	public String getThrdPhotoPageSize() {
		return (mPlayList.getCurrentIndex(Const.FILTER_IMAGE) + 1) + "/"
				+ mPlayList.getFileNum(Const.FILTER_IMAGE);
	}

	// Public method
	public int getMode(int type) {
		return mPlayList.getRepeatMode(type);
	}

	public void setRepeatMode(int type, int mode) {
		if (null != mPlayList) {
			mPlayList.setRepeatMode(type, mode);
		}
	}

	public int getPhotoOrientation() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getOrientation();
	}

	public String getPhotoName() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getName();
	}

	public String getWhiteBalance() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getWhiteBalance();
	}

	public String getAlbum() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getAlbum();
	}

	public String getMake() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getMake();
	}

	public String getModifyDate() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getModifyDate();
	}

	public int getPhotoDur() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getDuration();
	}

	public String getPhotoModel() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getModel();
	}

	public String getFlash() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getFlash();
	}

	/*
	 * public String getResolution() { if(mImageManager == null){ mImageManager
	 * = new Imageshowimpl(); } return mImageManager.getPwidth() + " x " +
	 * mImageManager.getPheight(); }
	 */
	/* add by lei 2011-12-26, fix 3d photo get resolution issue */
	public String getResolution() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getResolution();
	}

	public String getPhotoSize() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getSize();
	}

	public String getFocalLength() {
		if (mImageManager == null) {
			mImageManager = new Imageshowimpl();
		}
		return mImageManager.getFocalLength();
	}

	
	public String getCurrentFileName(int fileType) {

		return mPlayList.getCurrentFileName(fileType);
	}



	public String getTextPageSize() {
		// Modified by Dan for fix bug DTV00375633
		int currentPos = mPlayList.getCurrentIndex(Const.FILTER_TEXT) + 1;
		int count = mPlayList.getFileNum(Const.FILTER_TEXT);

		String result = "";
		if (currentPos > 0 && count > 0) {
			result = currentPos + "/" + count;
		}

		return result;
	}

	public String getTextAlbum() {
		String album = mPlayList.getCurrentFileName(Const.FILTER_TEXT);
		int start = 0;
		if (album != null) {
			start = album.indexOf(".");
			if (start + 1 < album.length()) {
				album = album.substring(start + 1);
			} else {
				album = "";
			}
		} else {
			album = "";
		}
		return album + " ...";
	}

	public String getTextSize() {
		long length = mPlayList.getCurrentFileSize(Const.FILTER_TEXT);
		return length + " Byte";
	}

	public String getNextName(int type) {
		return mPlayList.getNextFileName(type);
	}

	public String getCurrentPhotoPath() {
		String path = mPlayList.getCurrentFileName(Const.FILTER_IMAGE);
		if (null == path || path.length()<=0) {
			return "";
		} else {
			return path;
		}
	}

	public void setThrdPhotoCompelet(IThrdPhotoEventListener thrdPhotoListener) {
		
		if (mPhotoPlayback == null) {
			return;
		}
		mPhotoPlayback.setEventListener(thrdPhotoListener);
	}

	// Added by Dan for fix bug DTV00389362
	private int mLrcOffsetMode = 0;

	public void setLrcOffsetMode(int lrcOffsetMode) {
		mLrcOffsetMode = lrcOffsetMode;
	}

	public int getLrcOffsetMode() {
		return mLrcOffsetMode;
	}

	
	/**
	 * Delete
	 */
	public void setPicSetting() {
		thrdMode = MenuConfigManager.VIDEO_3D_MODE;
		String fileName = getCurrentPhotoPath();
		if (fileName != null && 
				fileName.toLowerCase().endsWith(".mpo")){
			is3DPhotoMpo = true;
		} else {
			is3DPhotoMpo = false;
		}
		MenuConfigManager mConfigManager = MenuConfigManager.getInstance(mContext);
		if(is3DPhotoMpo){
			mConfigManager.setValue(thrdMode, 
					thrdModeValue);
		} else {
			mConfigManager.setValue(MenuConfigManager.VIDEO_3D_MODE, 
					0);
		}
	}
}
