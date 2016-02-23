package com.mediatek.ui.mmp.commonview;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceView;
import android.view.View;
import android.media.MediaPlayer;



import com.mediatek.mmp.MtkMediaPlayer;
import com.mediatek.mmp.MtkMediaPlayer.OnCompletionListener;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.mmpcm.videoimpl.VideoManager;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.util.MtkLog;

public class MtkVideoView extends SurfaceView {
	private static final String TAG = "MtkVideoView";
	private VideoManager mVideoManager;
	private Context mContext;
	// Added by Dan for fix bug DTV00375890
	private boolean mIsStop;

	public MtkVideoView(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mContext = context;
		init();
	}

	public MtkVideoView(Context context, AttributeSet attrs) {
		this(context, attrs, 0);
		init();
	}

	public MtkVideoView(Context context) {
		this(context, null, 0);
		init();
	}

	public void init() {
		MultiFilesManager filesManager = MultiFilesManager
				.getInstance(mContext);
		int source = filesManager.getCurrentSourceType();
		switch (source) {
		case MultiFilesManager.SOURCE_LOCAL:
			source = VideoConst.PLAYER_MODE_MMP;
			break;
		case MultiFilesManager.SOURCE_SMB:
			source = VideoConst.PLAYER_MODE_SAMBA;
			break;
		case MultiFilesManager.SOURCE_DLNA:
			source = VideoConst.PLAYER_MODE_DLNA;
			break;
		default:
			break;
		}

		mVideoManager = VideoManager.getInstance(this, source);
		mVideoManager.setPreviewMode(true);
		if(source == VideoConst.PLAYER_MODE_MMP){
		mVideoManager.setOnCompletionListener(mCompleteListener);

		}else{
			mVideoManager.setOnCompletionListener(mtkCompleteListener);

		}
		
	}
	
	public int getVideoPlayStatus() {
		if (null == mVideoManager) {
			return VideoConst.PLAY_STATUS_INITED;
		}
		return mVideoManager.getPlayStatus();
	}

	private OnCompletionListener mtkCompleteListener = new OnCompletionListener() {

		@Override
		public void onCompletion(MtkMediaPlayer mp) {
			handleCompletion();
		}

	};

	private MediaPlayer.OnCompletionListener mCompleteListener = new MediaPlayer.OnCompletionListener() {

		@Override
		public void onCompletion(MediaPlayer mp) {
			handleCompletion();
		}

	};


	private void handleCompletion(){
			if (mIsStop) {
				return;
			}

			mVideoManager.autoNext();


	}

	public void setPreviewMode(boolean model) {
		mVideoManager.setPreviewMode(model);
	}

	public boolean isVideoPlaybackInit() {
		return mVideoManager == null ? false : true;
	}

	public void play(String path) {
		try {
			// Added by Dan
			mVideoManager.setDataSource(path);
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (IllegalStateException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
		// Added by Dan for fix bug DTV00375890
		mIsStop = false;
	}

	public void stop() {
		if (mVideoManager != null) {
			try {
				// Added by Dan for fix bug DTV00375890
				mIsStop = true;
				mVideoManager.stopVideo();
				
			} catch (IllegalStateException e) {
				MtkLog.w(TAG, e.getMessage());
			}
		}
	}

	
	public void reset() {
		if (mVideoManager != null) {
			try {
				// Added by Dan for fix bug DTV00375890
				
					mVideoManager.reset();
				
			} catch (IllegalStateException e) {
				MtkLog.w(TAG, e.getMessage());
			}
		}
	}

	public void onRelease() {

		if (mVideoManager != null) {
			try {
				stop();
				mVideoManager.onRelease();
				mVideoManager = null;
			} catch (IllegalStateException e) {
				MtkLog.w(TAG, e.getMessage());
			}
		}

		/* video had been close and send broadcast tell it. */
//		LogicManager.getInstance(mContext).videoZoomReset();

//		LogicManager.getInstance(mContext).sendCloseBroadCast();
	}
}
