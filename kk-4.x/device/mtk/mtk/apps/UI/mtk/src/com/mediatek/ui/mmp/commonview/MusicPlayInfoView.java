package com.mediatek.ui.mmp.commonview;
import com.mediatek.mmp.MtkMediaPlayer;

import android.content.Context;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.mmpcm.audioimpl.AudioConst;
import com.mediatek.mmpcm.audioimpl.PlaybackService;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.ui.R;
import com.mediatek.ui.mmp.util.GetDataImp;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.util.MtkLog;

public class MusicPlayInfoView extends LinearLayout {

	private static final String TAG = "MusicPlayInfoView";

	private static final int PROGRESS_CHANGED = 0;

	private static final int PROGRESS_SCOREVIEW = 1;

	private static final int AUDIO_CHANGED = 2;

	private static final int SPEED_UPDATE = 3;
	
	private static final int NOSUPPORT_PLAYNEXT = 4;

	private static final int DELAY_TIME = 1000;

	private static final int DELAY_SCO = 400;
	
	private static final int PLAY_COMPLETE = 5;
	
	private static final int PLAY_END = 6;
	
	private static final int AUDIO_RESET =7;
	
	private boolean isNotSupport = false;

	private TextView vMusicName;

	private ProgressBar vProgressBar;

	private TextView vStartTime;

	private TextView vEndtime;

	private ImageView vPlay;

	private ImageView vRepeat;

	private ImageView vShuffle;

	private ImageView vUnkowm;

	private ImageView vVolume;

	private ProgressBar vVolumeBar;

	private ScoreView mScoreView;

	private View mContentView;

	private LinearLayout mVolumeProgressBg;

	private LogicManager mLogicManager;

	private int maxVolume = 0;

	private int currentVolume = 0;

	private int mType = 1;

	private View vMusicView;

	private boolean isMute = false;

	private PopupWindow mPopView ;

	private boolean isDur;

	public MusicPlayInfoView(Context context, View contentView, int type,
			PopupWindow mPopupWindow) {
		super(context);
		mContentView = contentView;
		MtkLog.d(TAG, "MusicPlayInfoView");
		mType = type;
		if (mType == 0) {
			findScoreView();
		}
		mPopView = mPopupWindow;
		findView();
	}



	private PlaybackService mAudioPlayback = null;
	

	public void init(Context context) {
        registerListener(context);
        initView();
		
	}
	
	private void registerListener(Context context){
		mLogicManager = LogicManager.getInstance(context);

		if(mLogicManager.getAudioSourceType() == AudioConst.PLAYER_MODE_LOCAL){
			mLogicManager.setAudioPreparedListener(mPreparedListener);
			mLogicManager.setAudioErrorListener(mErrorListener);
			
			mLogicManager.setAudioCompletionListener(mCompletionListener);
			
			mLogicManager.setAudioInfoListener(mInfoListener);
		
		}else{
			mLogicManager.setAudioPreparedListener(mtkPreparedListener);
			mLogicManager.setAudioErrorListener(mtkErrorListener);
			
			mLogicManager.setAudioCompletionListener(mtkCompletionListener);
			
			mLogicManager.setAudioInfoListener(mtkInfoListener);


		}
		
		mAudioPlayback = mLogicManager.getAudioPlaybackService();

		if(null != mAudioPlayback){
			if(mLogicManager.getAudioSourceType() == AudioConst.PLAYER_MODE_LOCAL){
				mAudioPlayback.registerAudioPreparedListener(
								mPreparedListener);
				mAudioPlayback.registerInfoListener(mInfoListener);
				mAudioPlayback.registerAudioErrorListener(mErrorListener);
		    	mAudioPlayback.registerAudioCompletionListener(mCompletionListener);
			}else{
				mAudioPlayback.registerAudioPreparedListener(
								mtkPreparedListener);
				mAudioPlayback.registerInfoListener(mtkInfoListener);
				mAudioPlayback.registerAudioErrorListener(mtkErrorListener);
				mAudioPlayback.registerAudioCompletionListener(mtkCompletionListener);

			}
			
		}
	}
	
	private void initView(){
		maxVolume = mLogicManager.getMaxVolume();
		currentVolume = mLogicManager.getVolume();
		vVolumeBar.setMax(maxVolume);
		int playStatus = mAudioPlayback.getPlayStatus();
		if (playStatus == AudioConst.PLAY_STATUS_PAUSED) {
			vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_pause);
		}else if(playStatus == AudioConst.PLAY_STATUS_FF){
			vPlay.setImageResource(R.drawable.common_key_ff);
		}else if(playStatus == AudioConst.PLAY_STATUS_FR){
			vPlay.setImageResource(R.drawable.common_key_fr);
		}else if(playStatus == AudioConst.PLAY_STATUS_STOPPED){
			vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_stop);
		}else{
			vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_play);
		}
		onMute();

		int repeatModel = mLogicManager.getRepeatModel(Const.FILTER_AUDIO);
		setRepeate(repeatModel);

		boolean isShuffle = mLogicManager.getShuffleMode(Const.FILTER_AUDIO);
		if (isShuffle) {
			vShuffle.setVisibility(View.VISIBLE);
		} else {
			vShuffle.setVisibility(View.INVISIBLE);
		}

		sendMessage();
	}

	private void setRepeate(int repeatModel) {
		switch (repeatModel) {
		case Const.REPEAT_NONE: {
			vRepeat.setVisibility(View.INVISIBLE);
			break;
		}
		case Const.REPEAT_ONE: {
			vRepeat.setVisibility(View.VISIBLE);
			vRepeat
					.setImageResource(R.drawable.mmp_thumbnail_player_icon_repeatone);
			break;
		}
		case Const.REPEAT_ALL: {
			vRepeat.setVisibility(View.VISIBLE);
			vRepeat
					.setImageResource(R.drawable.mmp_thumbnail_player_icon_repeall);
			break;
		}
		case Const.REPEAT_MARKED: {
			vShuffle.setVisibility(View.INVISIBLE);
			break;
		}
		default:
			break;
		}
	}

	public void onRepeatClick() {
		int repeatModel = mLogicManager.getRepeatModel(Const.FILTER_AUDIO);
		repeatModel = (repeatModel+1) % 3;

		setRepeate(repeatModel);
		mLogicManager.setRepeatMode(Const.FILTER_AUDIO, repeatModel);

	}

	public void onStop() {
		removeMessage();
		mLogicManager.stopAudio();
		vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_stop);
		mLogicManager.finishAudioService();

	}

	public void onComplete(){
		removeMessage();
		if(null != mPopView){
			mPopView.dismiss();
		}
		
	}
	public void onPauseOrPlay() {

		if (mLogicManager.isAudioPause() || mLogicManager.isAudioFast()) {
			mHandler.sendEmptyMessage(PROGRESS_CHANGED);
			mHandler.sendEmptyMessage(PROGRESS_SCOREVIEW);
			vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_play);
			mLogicManager.playAudio();
		} else if (mLogicManager.isAudioPlaying()) {
			removeMessage();
			mLogicManager.pauseAudio();
			vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_pause);
		}

	}

	public void onFast() {
		int speed = mLogicManager.getAudioSpeed();
		if (speed == 0) {
			return;
		} else if (speed == 1) {
			vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_play);
		} else {
			int status = mLogicManager.getAudioStatus();
			if (status == AudioConst.PLAY_STATUS_FR) {
				vPlay.setImageResource(R.drawable.common_key_fr);
				sendMessageMini();
			} else if (status == AudioConst.PLAY_STATUS_FF) {
				vPlay.setImageResource(R.drawable.common_key_ff);				
				sendMessageMini();
			}
		}
	}

	public void setMute() {
		mLogicManager.setMute();
		onMute();
	}

	public void onMute() {

		if (mLogicManager.isMute()) {
			vVolume.setImageResource(R.drawable.mmp_toolbar_icon_mute);
			if (null != mVolumeProgressBg) {
				mVolumeProgressBg.setVisibility(View.INVISIBLE);
			}
			hidScore(true);
		} else {
			hidScore(false);
			vVolume.setImageResource(R.drawable.mmp_toolbar_icon_volume);
			if (null != mVolumeProgressBg) {
				mVolumeProgressBg.setVisibility(View.VISIBLE);
			}

		}

	}

	private void hidScore(boolean ishide) {
		if (null == mScoreView) {
			return;
		}
		if (ishide) {
			mScoreView.setVisibility(View.INVISIBLE);
			mHandler.removeMessages(PROGRESS_SCOREVIEW);
		} else {
			mScoreView.setVisibility(View.VISIBLE);
			mHandler.sendEmptyMessageDelayed(PROGRESS_SCOREVIEW, DELAY_SCO);
		}
	}

	public void setVolumeUp() {
		if (mLogicManager.isMute()) {
			setMute();
			return;
		}
		currentVolume = currentVolume + 1;
		if (currentVolume > maxVolume) {
			currentVolume = maxVolume;
		}
		mLogicManager.setVolume(currentVolume);
		vVolumeBar.setProgress(currentVolume);

	}

	public void setVolumeDown() {
		if (mLogicManager.isMute()) {
			setMute();
			return;
		}
		currentVolume = currentVolume - 1;
		if (currentVolume < 0) {
			currentVolume = 0;
		}
		mLogicManager.setVolume(currentVolume);
		vVolumeBar.setProgress(currentVolume);

	}

	private void reSet() {
		vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_play);
	}



	//MediaPlayer

	private OnPreparedListener mPreparedListener = new OnPreparedListener() {

		public void onPrepared(MediaPlayer mp) {
			handlePrepare();

		}
	};




	private OnErrorListener mErrorListener = new OnErrorListener() {

		public boolean onError(MediaPlayer arg0, int arg1, int arg2) {
			MtkLog.i(TAG, "OnErrorListener  targ1:" + arg1 + "  arg2" + arg2
					+ " " + System.currentTimeMillis());
			return handleError();
		}

	};

	private OnInfoListener mInfoListener = new OnInfoListener() {

		public boolean onInfo(MediaPlayer arg0, int arg1, int arg2) {
			return handleInfo(arg1);
		}
		
	};
	private OnCompletionListener mCompletionListener = new OnCompletionListener() {
		
		@Override
		public void onCompletion(MediaPlayer arg0) {
			mHandler.sendEmptyMessage(PLAY_END);

		}
	};







	//MTK MediaPlayer
	

		private MtkMediaPlayer.OnPreparedListener mtkPreparedListener = new MtkMediaPlayer.OnPreparedListener() {
		
			public void onPrepared(MtkMediaPlayer mp) {
				handlePrepare();
		
			}
		};
		
		private MtkMediaPlayer.OnErrorListener mtkErrorListener = new MtkMediaPlayer.OnErrorListener() {
		
		public boolean onError(MtkMediaPlayer arg0, int arg1, int arg2) {
			MtkLog.i(TAG, "OnErrorListener  targ1:" + arg1 + "  arg2" + arg2
					+ " " + System.currentTimeMillis());
				return handleError();
			}
		
		};
		
		private MtkMediaPlayer.OnInfoListener mtkInfoListener = new MtkMediaPlayer.OnInfoListener() {
		
			public boolean onInfo(MtkMediaPlayer arg0, int arg1, int arg2) {
				
				return handleInfo(arg1);
			}
			
		};




		private MtkMediaPlayer.OnCompletionListener mtkCompletionListener = new MtkMediaPlayer.OnCompletionListener() {
		
			@Override
			public void onCompletion(MtkMediaPlayer arg0) {
		
				mHandler.sendEmptyMessage(PLAY_END);

			}
		};



		private void handlePrepare(){
			reSet();
			if (mLogicManager.isAudioPlaying()) {
				removeMessage();
				mHandler.sendEmptyMessage(AUDIO_CHANGED);
				mHandler.sendEmptyMessage(PROGRESS_CHANGED);
				mHandler.sendEmptyMessage(PROGRESS_SCOREVIEW);
			}



		}

		private boolean handleError(){

			if(isNotSupport){
				return true;
			}
			isNotSupport = true;

			mLogicManager.stopAudio();
			MtkLog.i(TAG, "OnErrorListener  run was execued" );
			mHandler.sendEmptyMessage(NOSUPPORT_PLAYNEXT);
			
			return true;



		}

		
		private boolean handleInfo(int arg1){
			MtkLog.d(TAG,"enter onInfo:"+arg1);
			switch (arg1) {
			case AudioConst.MEDIA_INFO_METADATA_COMPLETE:
				isNotSupport = true;
				mHandler.sendEmptyMessage(PLAY_COMPLETE);
				return false;
			case MediaPlayer.MEDIA_INFO_ON_REPLAY:
				 mHandler.sendEmptyMessage(SPEED_UPDATE);
				break;
			case MediaPlayer.MEDIA_INFO_AUDIO_ENCODE_FORMAT_UNSUPPORT:
				if(isNotSupport){
					return false;
				}
				isNotSupport = true;
				mHandler.sendEmptyMessage(NOSUPPORT_PLAYNEXT);
				break;
			case MediaPlayer.MEDIA_INFO_NOT_SEEKABLE:
				MtkLog.d(TAG,"enter onInfo: MEDIA_INFO_NOT_SEEKABLE");
				break;
			case AudioConst.MEDIA_INFO_DATA_PREPAREED_STATE:
				isNotSupport = false;
				mHandler.sendEmptyMessage(AUDIO_RESET);
				break;

			case AudioConst.MEDIA_INFO_PLAY_RENDERING_START:
				mHandler.sendEmptyMessage(AUDIO_CHANGED);
				break;
			default:
				MtkLog.d(TAG,"enter onInfo:"+arg1);
				break;
			}
			return false;



		}




	
	public String settime(int mills) {
		mills /= 1000;
		int minute = mills / 60;
		int hour = minute / 60;
		int second = mills % 60;
		minute %= 60;
		return String.format("%02d:%02d:%02d", hour, minute, second);
	}

	private void findView() {
		vMusicName = (TextView) mContentView.findViewById(R.id.mmp_musicname);
		vProgressBar = (ProgressBar) mContentView
				.findViewById(R.id.mmp_musicback_progress);
		vStartTime = (TextView) mContentView
				.findViewById(R.id.mmp_musicback_starttime);
		vEndtime = (TextView) mContentView
				.findViewById(R.id.mmp_musicback_endtime);
		vPlay = (ImageView) mContentView.findViewById(R.id.mmp_musicback_play);
		vRepeat = (ImageView) mContentView
				.findViewById(R.id.mmp_musicback_repeat);
		vShuffle = (ImageView) mContentView
				.findViewById(R.id.mmp_musicback_shuffle);
		vUnkowm = (ImageView) mContentView
				.findViewById(R.id.mmp_musicback_unkown);
		vVolume = (ImageView) mContentView
				.findViewById(R.id.mmp_musicback_volume_img);
		vVolumeBar = (ProgressBar) mContentView
				.findViewById(R.id.mmp_musicback_volume);

		mVolumeProgressBg = (LinearLayout) mContentView
				.findViewById(R.id.mmp_volume_progress_bg);
	}

	private void findScoreView() {
		mScoreView = (ScoreView) mContentView.findViewById(R.id.mmp_musicscore);
	}

	public Handler mHandler = new Handler() {

		 
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
			case PROGRESS_CHANGED: {
				int progress = 0;
				if(isDur){
					progress = mLogicManager.getPlaybackProgress();
				}else{
					progress = mLogicManager.getAudioBytePosition();
				}
				MtkLog.d(TAG,"PROGRESS_CHANGED progress = "+ progress +" isDur = "+ isDur);
				if (progress >= 0) {
					if (mLogicManager.getAudioPlaybackService() != null) {
						vStartTime.setText(settime(progress));
						vProgressBar.setProgress(progress);						
					}
				}
				sendEmptyMessageDelayed(PROGRESS_CHANGED, DELAY_TIME);
				break;
			}
			case PROGRESS_SCOREVIEW:
				if (null != mScoreView) {
					if (hasMessages(PROGRESS_SCOREVIEW)) {
						removeMessages(PROGRESS_SCOREVIEW);
					}
					mScoreView.update(mLogicManager.getAudSpectrum());
					mScoreView.invalidate();
					sendEmptyMessageDelayed(PROGRESS_SCOREVIEW, DELAY_SCO);
				}
				break;
			case SPEED_UPDATE:{	
				if (mLogicManager.getAudioPlaybackService() != null){
					mLogicManager.setAuidoSpeed(1);	
					vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_play);
					mLogicManager.playAudio();
				}
				break;		
			}			
			case NOSUPPORT_PLAYNEXT: {
				MtkLog.i(TAG, "OnErrorListener  play nexted was execued" );
				mLogicManager.playNextAudio();
				break;
			}

			case AUDIO_CHANGED: {
				initControl();
				break;
			}
			case AUDIO_RESET:{
				vMusicName.setText(mLogicManager.getCurrentFileName(Const.FILTER_AUDIO));
				vPlay.setImageResource(R.drawable.mmp_thumbnail_player_icon_play);
				break;
			}
			case PLAY_COMPLETE:
				onComplete();
				break;
			case PLAY_END:
				if(null != vStartTime){
					if (!isNotSupport) {
					vStartTime.setText(vEndtime.getText());
					if(null !=vEndtime){
						vEndtime.setText(null);
					}
					vStartTime.setText(null);
					}
				}
				if(null != vProgressBar){
					vProgressBar.setProgress(vProgressBar.getMax());
					vProgressBar.setProgress(0);
				}
				break;
			default:
				break;
			}

		}

	};
	
	public void sendMessageMini()
	{
		mHandler.sendEmptyMessage(PROGRESS_CHANGED);
		if (mType == 0) {
			mHandler.sendEmptyMessage(PROGRESS_SCOREVIEW);
		}
	}
	public void sendMessage() {
		mHandler.sendEmptyMessage(AUDIO_CHANGED);
		mHandler.sendEmptyMessage(PROGRESS_CHANGED);
		if (mType == 0) {
			mHandler.sendEmptyMessage(PROGRESS_SCOREVIEW);
		}
	}

	public void removeMessage() {
		mHandler.removeMessages(PROGRESS_SCOREVIEW);
		mHandler.removeMessages(PROGRESS_CHANGED);
		mHandler.removeMessages(SPEED_UPDATE);
		mHandler.removeMessages(AUDIO_CHANGED);
		mHandler.removeMessages(AUDIO_RESET);
		mHandler.removeMessages(NOSUPPORT_PLAYNEXT);
		mHandler.removeMessages(PLAY_COMPLETE);
	}

	public void initControl() {
		if(null==mLogicManager){
			mLogicManager = LogicManager.getInstance(mContext);
		}
		
		int dur = mLogicManager.getTotalPlaybackTime();
		MtkLog.d(TAG,"initControl dur = "+ dur);
		if(dur <= 0){
			isDur = false;
			dur = (int)mLogicManager.getAudioFileSize();
			vEndtime.setVisibility(View.INVISIBLE);
			vStartTime.setVisibility(View.INVISIBLE);
		}else{
			isDur = true;
			vStartTime.setVisibility(View.VISIBLE);
			vEndtime.setVisibility(View.VISIBLE);
			vEndtime.setText(settime(dur));
		}
		MtkLog.d(TAG,"initControl vProgressBar max = "+ dur);
		vProgressBar.setMax(dur);
		vProgressBar.setProgress(0);
		vVolumeBar.setMax(maxVolume);
		vVolumeBar.setProgress(currentVolume);
		vMusicName.setText(mLogicManager.getCurrentFileName(Const.FILTER_AUDIO));
	}

}
