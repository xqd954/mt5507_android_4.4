package com.mediatek.ui.mmp.commonview;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.ui.R;
import com.mediatek.ui.mmp.multimedia.MediaPlayActivity;
import com.mediatek.ui.mmp.multimedia.MusicPlayActivity;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class ShowInfoView extends Dialog implements OnDismissListener {
	private static final String TAG = "ShowInfoView";

	private View mView;

	private int mediatype;

	private LogicManager mLogicManager;

	private static final int MSG_DISMISS_DELAY = 10000;

	private static final int MSG_DISMISS = 1;

	// /video
	private TextView videoTitle;
	private TextView videoDirector;
	private TextView videoCopyRight;
	private TextView videoDate;
	private TextView videoGenre;
	private TextView videoDuration;
	private TextView videoNext;

	// /audio
	private TextView audioTitle;
	private TextView audioArtist;
	private TextView audioAlbum;
	private TextView audioGenre;
	private TextView audioYear;
	private TextView audioDuration;
	private TextView audioNext;

	// photo
	private TextView photoAlbum;
	private TextView photoOrientation;
	private TextView whiteBalance;
	private TextView photoName;
	private TextView photoDate;
	private TextView photoSize;
	private TextView photoNext;
	private TextView model;
	private TextView photoFlash;
	private TextView focalLength;
	private TextView make;
	// text
	private TextView textAlbum;
	private TextView textName;
	private TextView textSize;
	private TextView textNext;

	private int menuWidth = 260;

	private int menuHight = 247;

	private int marginY = -50;

	private int marginX = 0;

	private Context mContext;

	public ShowInfoView(Context context, int theme) {
		super(context, theme);
	}

	public ShowInfoView(Context context, View view, int type,
			LogicManager manager) {
		this(context, R.style.dialog);
		mView = view;
		mediatype = type;
		mLogicManager = manager;

		mContext = context;
	}

	private Handler mHandler = new Handler() {

		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_DISMISS: {
				if (mediatype != MultiMediaConstant.AUDIO) {
					if (isShowing()) {
						dismiss();
					}
				}
				break;
			}
			default:
				break;
			}

		}

	};

	public boolean onKeyDown(int keyCode, KeyEvent event) {

		if (mContext == null) {
			return true;
		}
		mHandler.removeMessages(MSG_DISMISS);
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);

		if (keyCode == KeyMap.KEYCODE_MENU) {
			dismiss();
		}

		if (keyCode == KeyMap.KEYCODE_BACK) {
			mHandler.removeMessages(MSG_DISMISS);
			dismiss();
			return true;
		}

		if (null != mContext && mContext instanceof MediaPlayActivity) {
			((MediaPlayActivity) mContext).onKeyDown(keyCode, event);
			return true;
		}

		/*if (keyCode == KeyMap.KEYCODE_VOLUME_UP
				|| keyCode == KeyMap.KEYCODE_VOLUME_DOWN) {
			return true;
		}
		if (keyCode == KeyMap.KEYCODE_MTKIR_MUTE){
			return true;
		}
		if(keyCode == KeyMap.KEYCODE_MTKIR_PLAYPAUSE){
			return true;
		}*/
		return super.onKeyDown(keyCode, event);

	};

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		if (mContext == null) {
			return true;
		}

		if (mediatype == MultiMediaConstant.AUDIO) {
			((MusicPlayActivity) mContext).onKeyUp(keyCode, event);
		}
		return super.onKeyUp(keyCode, event);
	}

	public void updateView() {
		if (mediatype == MultiMediaConstant.VIDEO) {
			setVideoView();
		} else if (mediatype == MultiMediaConstant.AUDIO) {
			setAudioView();
		} else if (mediatype == MultiMediaConstant.TEXT) {
			setTextView();
		} else if (mediatype == MultiMediaConstant.PHOTO 
			||mediatype == MultiMediaConstant.THRD_PHOTO) {
			setPhotoView();
		}
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(mView);
		switchView();
		setWindowPosition();

		mHandler.sendEmptyMessageDelayed(MSG_DISMISS, MSG_DISMISS_DELAY);
	}

	// set position at screen
	public void setWindowPosition() {
		WindowManager m = getWindow().getWindowManager();
		Display display = m.getDefaultDisplay();
		Window window = getWindow();
		WindowManager.LayoutParams lp = window.getAttributes();

		Drawable d;
		if (mediatype == MultiMediaConstant.VIDEO) {
			// TODO change
			d = mContext.getResources().getDrawable(
					R.drawable.detail_page_little_video_960);
		} else if (mediatype == MultiMediaConstant.PHOTO
				|| mediatype == MultiMediaConstant.THRD_PHOTO) {
			d = mContext.getResources().getDrawable(
					R.drawable.mmp_infodetail_page_960);
		} else {
			d = mContext.getResources().getDrawable(
					R.drawable.detail_page_little_960);
		}

		if (null != d) {
			menuWidth = d.getIntrinsicWidth()*(ScreenConstant.SCREEN_WIDTH/1280);
			menuHight = d.getIntrinsicHeight()*(ScreenConstant.SCREEN_HEIGHT/720);
		}

		marginX = (int) (ScreenConstant.SCREEN_WIDTH * 0.15);

		lp.width = menuWidth;
		lp.height = menuHight;

		lp.x = ScreenConstant.SCREEN_WIDTH / 2 - menuWidth / 2 - marginX;
		lp.y = (int) (ScreenConstant.SCREEN_HEIGHT * 3 / 8) - (int)(ScreenConstant.SCREEN_HEIGHT*0.16) - menuHight / 2;
		window.setAttributes(lp);
	}

	private void switchView() {
		if (mediatype == MultiMediaConstant.VIDEO) {
			findVideoView();
		} else if (mediatype == MultiMediaConstant.AUDIO) {
			findAudioView();
		} else if (mediatype == MultiMediaConstant.TEXT) {
			findTextView();
		} else if (mediatype == MultiMediaConstant.PHOTO
				|| mediatype == MultiMediaConstant.THRD_PHOTO) {
			findPhotoView();
		}
	}

	private void findVideoView() {
		videoTitle = (TextView) mView.findViewById(R.id.mmp_info_title);
		videoDirector = (TextView) mView.findViewById(R.id.mmp_info_director);
		videoCopyRight = (TextView) mView.findViewById(R.id.mmp_info_copyrght);
		videoDate = (TextView) mView.findViewById(R.id.mmp_info_date);
		videoDuration = (TextView) mView.findViewById(R.id.mmp_info_duration);
		videoGenre = (TextView) mView.findViewById(R.id.mmp_info_genre);
		videoNext = (TextView) mView.findViewById(R.id.mmp_info_next);

		setVideoView();

	}

	// TODO
	public void setVideoView() {
		videoTitle.setText(formatText(mLogicManager.getVideoTitle()));
		videoDirector.setText(formatText(mLogicManager.getVideoDirector()));
		videoCopyRight.setText(formatText(mLogicManager.getVideoCopyright()));
		videoDate.setText(formatText(mLogicManager.getVideoYear()));
		videoGenre.setText(formatText(mLogicManager.getVideoGenre()));
		videoNext.setText(formatText(mLogicManager
				.getNextName(Const.FILTER_VIDEO)));
		int dur = mLogicManager.getVideoDuration();
		dur = (dur > 0 ? dur : 0);
		if (dur == 0) {
			videoDuration.setText("N/A");
		} else {
			videoDuration.setText(formatTime(dur));
		}
	
	}

	private String formatTime(int mills) {
		mills /= 1000;
		int minute = mills / 60;
		int hour = minute / 60;
		int second = mills % 60;
		minute %= 60;
		String text;
		try {
			text = String.format("%02d:%02d:%02d", hour, minute, second);
		} catch (Exception e) {
		    text = "";
			MtkLog.i(TAG, e.getMessage());
		}
		return text;
	}

	private void findAudioView() {
		audioAlbum = (TextView) mView.findViewById(R.id.mmp_info_album);
		audioArtist = (TextView) mView.findViewById(R.id.mmp_info_artist);
		audioDuration = (TextView) mView.findViewById(R.id.mmp_info_duration);
		audioGenre = (TextView) mView.findViewById(R.id.mmp_info_genre);
		audioNext = (TextView) mView.findViewById(R.id.mmp_info_next);
		audioTitle = (TextView) mView.findViewById(R.id.mmp_info_title);
		audioYear = (TextView) mView.findViewById(R.id.mmp_info_year);

		setAudioView();
	}
	
	
	public void updateTime(int totalTime)
	{
		audioDuration.setText(formatText(formatTime(totalTime)));
	}

	// TODO
	public void setAudioView() {
		audioTitle.setText(formatText(mLogicManager.getMusicTitle()));
		audioArtist.setText(formatText(mLogicManager.getMusicArtist()));
		audioAlbum.setText(formatText(mLogicManager.getMusicAlbum()));
		audioGenre.setText(formatText(mLogicManager.getMusicGenre()));

		// TODO have chnage

		int dur = (int) mLogicManager.getTotalPlaybackTime();
		dur = (dur > 0 ? dur : 0);
		if (dur == 0) {
			audioDuration.setText("N/A");
		} else {
			audioDuration.setText(formatTime(dur));
		}

		// TODO next music mPlaylist NullPointexception
		audioNext.setText(formatText(mLogicManager
				.getNextName(Const.FILTER_AUDIO)));
		audioYear.setText(formatText(mLogicManager.getMusicYear()));

	}

	private void findPhotoView() {
		photoAlbum = (TextView) mView.findViewById(R.id.mmp_info_album);
		whiteBalance = (TextView) mView.findViewById(R.id.mmp_info_artist);
		make = (TextView) mView
				.findViewById(R.id.mmp_info_colorspace);
		photoDate = (TextView) mView.findViewById(R.id.mmp_info_date);
		model = (TextView) mView.findViewById(R.id.mmp_info_exposure);
		photoFlash = (TextView) mView.findViewById(R.id.mmp_info_fnumber);
		photoName = (TextView) mView.findViewById(R.id.mmp_info_name);
		photoNext = (TextView) mView.findViewById(R.id.mmp_info_next);
		photoOrientation = (TextView) mView
				.findViewById(R.id.mmp_info_orientation);
		focalLength = (TextView) mView.findViewById(R.id.mmp_info_program);
		photoSize = (TextView) mView.findViewById(R.id.mmp_info_size);

		setPhotoView();
	}
	private String getPhotoOrientation(){
		int photoOrientation = mLogicManager.getPhotoOrientation();
		String orientationStr = "";
		int index = 0;

		if(photoOrientation >0 && photoOrientation <= 8){
			index = Const.ORIENTATION_NEXT_ARRAY[photoOrientation]-1;
			orientationStr += index/4 > 0 ? "f ":"";
			orientationStr += (index%4) * 90 + "";

		}else {
			orientationStr = "0";
		}
				
		return orientationStr;

	}
	public void setPhotoView() {
		photoAlbum.setText((mLogicManager.getAlbum()));
		
		//White balance 
		whiteBalance.setText((mLogicManager.getWhiteBalance()));
		make.setText((mLogicManager.getMake()));
		photoDate.setText((mLogicManager.getModifyDate()));
		model.setText((mLogicManager.getPhotoModel()));
		photoFlash.setText((mLogicManager.getFlash()));
		photoName.setText((mLogicManager.getPhotoName()));

		photoOrientation.setText(mContext.getString(R.string.mmp_lable_rorate)
				+ " " + getPhotoOrientation()
				+ mContext.getString(R.string.mmp_lable_degree));
		photoSize.setText((mLogicManager.getResolution()));
		focalLength.setText((mLogicManager.getFocalLength()));
		photoNext.setText((mLogicManager.getNextName(Const.FILTER_IMAGE)));
	}

	private String formatText(String value) {
		if (null == value || value.length() == 0) {
			return "N/A";
		}

		return value;
	}

	private void findTextView() {
		textAlbum = (TextView) mView.findViewById(R.id.mmp_info_album);
		textName = (TextView) mView.findViewById(R.id.mmp_info_name);
		textNext = (TextView) mView.findViewById(R.id.mmp_info_next);
		textSize = (TextView) mView.findViewById(R.id.mmp_info_size);

		setTextView();
	}

	public void setTextView() {
		textAlbum.setText(formatText(mLogicManager.getTextAlbum()));
		
		textNext.setText(formatText(mLogicManager
				.getNextName(Const.FILTER_TEXT)));
		textName.setText(formatText(mLogicManager
				.getCurrentFileName(Const.FILTER_TEXT)));
		textSize.setText(formatText(mLogicManager.getTextSize()));

	}

	public void onDismiss(DialogInterface dialog) {

		mView = null;
		mContext = null;

	}
}
