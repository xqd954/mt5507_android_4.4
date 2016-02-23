package com.mediatek.ui.mmp.commonview;

import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.mmpcm.audioimpl.AudioConst;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.photoimpl.ConstPhoto;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.ui.R;
import com.mediatek.ui.mmp.multimedia.Photo3DPlayActivity;
import com.mediatek.ui.mmp.multimedia.Photo4K2KPlayActivity;
import com.mediatek.ui.mmp.multimedia.PhotoPlayActivity;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.util.MtkLog;

/**
 * {@link PopupWindow}
 * 
 * @author hs_weihuiluo
 * 
 */
public class ControlView extends PopupWindow {

	private static final String TAG = "ControlView";

	private Context mContext;

	private View vControlView;

	private ImageView vPStatePlay;

	private ImageView vPStatePause;

	private TextView vVideoRepeata;

	private TextView vPMediaType;

	private TextView vPZoomSize;

	// private LinearLayout vPRepeat;

	private TextView vPRepeatTv;

	private ImageView vPShuffle;

	private TextView vPTimeLong;

	private TextView vPOrder;

	private TextView vPView;

	private TextView vPFileName;

	private TextView vMStartTime;

	private TextView vMEndTime;

	private ProgressBar vMProgressBar;

	private TextView vMVolumeTv;

	private ProgressBar vMVolumeBar;

	private TextView mVideoTrackNumber;

	private ImageView mRepeatLogo;

	private TextView mVideoSubtitle;

	private ImageView mVideoSubtitleIcon;

	private LinearLayout mVolumeProgressBg;

	private ControlPlayState mControlImp;

	private Drawable repeatAll;

	private Drawable repeatOne;

	private LogicManager mLogicManager;

	private short subtitleIndex = -1;

	// Added by Dan for fix bug DTV00376577
	private boolean mIsPlaying = true;
	
	private int mediaType;
	
	private int type;
	
	public interface ControlPlayState {
		void play();

		void pause();
	}

	public ControlView(View contentView) {
		super(contentView);
	}

	public ControlView(Context context, int mediatype,
			ControlPlayState statecontrol, View contentView, int width,
			int height) {
		super(contentView, width, height);
		mediaType= mediatype;
		vControlView = contentView;
		mControlImp = statecontrol;
		mContext = context;
		mLogicManager = LogicManager.getInstance(context);

		findCommonView();
		initDrawable();
		switchView(mediatype);

		setOnDismissListener(mDismissListener);
	}
	//add by shuming for fix CR: DTV00407914
	
	public void hiddlen(int visibility) {
		View view = vControlView.findViewById(R.id.mmp_control_bottom_bar);
		if (null != view) {
			view.setVisibility(visibility);
		}
	}

	private OnDismissListener mDismissListener = new OnDismissListener() {

		public void onDismiss() {
//			vControlView = null;
//			mContext = null;
		}
	};

	public void initDrawable() {
		repeatAll = mContext.getResources().getDrawable(
				R.drawable.mmp_toolbar_typeicon_repeat);
		repeatAll.setBounds(0, 0, repeatAll.getMinimumWidth(), repeatAll
				.getMinimumHeight());

		repeatOne = mContext.getResources().getDrawable(
				R.drawable.mmp_toolbar_icon_repeat_one);
		repeatOne.setBounds(0, 0, repeatOne.getMinimumWidth(), repeatOne
				.getMinimumHeight());
	}

	private void switchView(int mediatype) {
		if (mediatype == MultiMediaConstant.PHOTO) {
			findPhotoView();
			vPMediaType.setText(mContext.getString(R.string.mmp_photo));
		}else if(mediatype == MultiMediaConstant.THRD_PHOTO){
			findThrdPhotoView();
			vPMediaType.setText(mContext.getString(R.string.mmp_photo));
		} else if (mediatype == MultiMediaConstant.AUDIO) {
			findMusicView();
			vPMediaType.setText(mContext.getString(R.string.mmp_audio));
		} else if (mediatype == MultiMediaConstant.VIDEO) {
			findVideoView();
			vPMediaType.setText(mContext.getString(R.string.mmp_video));
		} else if (mediatype == MultiMediaConstant.TEXT) {
			findTextView();
			vPMediaType.setText(mContext.getString(R.string.mmp_text));
		}
		//Add by keke 1215 for fix cr DTV00383194
		setInforbarTransparent();
	}

	public void initVideoTrackNumber() {
		if (null == mVideoTrackNumber) {
			return;
		}
		int number = mLogicManager.getAudioTranckNumber();
		if (number == 0) {
			// Modifyed by Dan 20111118 for fix bug DTV00373279
			// mVideoTrackNumber.setText(0 + "/" + number);
			mVideoTrackNumber.setText("");
		} else if (number > 0) {
			mVideoTrackNumber.setText(1 + "/" + number);
		}
	}

	public boolean changeVideoTrackNumber() {

		if (null == mVideoTrackNumber) {
			return false;
		}

		int tranckNumber = mLogicManager.getAudioTranckNumber();
		//modif by lei
		if (tranckNumber < 2) {
			return false;
		}
		
		try {
			String value = mVideoTrackNumber.getText().toString();
			String[] datas = value.split("/");
			short currentTrack = Short.parseShort(datas[0]);
			short number = Short.parseShort(datas[1]);

			if (currentTrack >= number) {
				currentTrack = 1;
			} else {
				currentTrack++;
			}
			if(mLogicManager.setAudioTranckNumber((short) (currentTrack - 1))){
			mVideoTrackNumber.setText(currentTrack + "/" + number);
				return true;
			}else{
				MtkLog.d(TAG, "--- changeVideoTrackNumber --setAudioTranckNumber return false, set fail!!!!" );
			}
		} catch (Exception e) {
			MtkLog.d(TAG, "--- changeVideoTrackNumber --:" + e.getMessage());
		}
		return false;
	}

	public void setRepeat(int type) {
		switch (mLogicManager.getRepeatModel(type)) {
		case Const.REPEAT_ALL:
			setRepeatAll();
			break;
		case Const.REPEAT_NONE:
			setRepeatNone();
			break;
		case Const.REPEAT_ONE:
			setRepeatSingle();
			break;
		default:
			break;
		}

	}

	private void findCommonView() {
		vPStatePlay = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_playstateplay);
		vPStatePause = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_playstatepause);
		vPMediaType = (TextView) vControlView
				.findViewById(R.id.mmp_pop_mediatype);
		vPOrder = (TextView) vControlView.findViewById(R.id.mmp_pop_order_tv);
		vPFileName = (TextView) vControlView
				.findViewById(R.id.mmp_pop_filename_tv);
	}

	private void findThrdPhotoView() {
		vPZoomSize = (TextView) vControlView
				.findViewById(R.id.mmp_pop_zoomsize);
		// vPRepeat = (LinearLayout) vControlView
		// .findViewById(R.id.mmp_pop_repeat);
		vPRepeatTv = (TextView) vControlView
				.findViewById(R.id.mmp_pop_repeat_tv);
		vPShuffle = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_shuffle_img);
		vPTimeLong = (TextView) vControlView.findViewById(R.id.mmp_pop_time_tv);
		vPView = (TextView) vControlView.findViewById(R.id.mmp_pop_view_tv);

		setRepeat(Const.FILTER_IMAGE);
		setShuffle(Const.FILTER_IMAGE);
		setThrdPhotoDuration();
	}
	
	private void findPhotoView() {
		vPZoomSize = (TextView) vControlView
				.findViewById(R.id.mmp_pop_zoomsize);
		// vPRepeat = (LinearLayout) vControlView
		// .findViewById(R.id.mmp_pop_repeat);
		vPRepeatTv = (TextView) vControlView
				.findViewById(R.id.mmp_pop_repeat_tv);
		vPShuffle = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_shuffle_img);
		vPTimeLong = (TextView) vControlView.findViewById(R.id.mmp_pop_time_tv);
		vPView = (TextView) vControlView.findViewById(R.id.mmp_pop_view_tv);

		setRepeat(Const.FILTER_IMAGE);
		setShuffle(Const.FILTER_IMAGE);
		setPhotoDuration();

	}

	private void setThrdPhotoDuration(){
		type = Photo3DPlayActivity.mDelayedTime;
		if (type == Photo3DPlayActivity.DELAYED_SHORT) {
			vPTimeLong.setText(R.string.mmp_menu_short);
		} else if (type == Photo3DPlayActivity.DELAYED_MIDDLE) {
			vPTimeLong.setText(R.string.mmp_menu_medium);
		} else if (type == Photo3DPlayActivity.DELAYED_LONG) {
			vPTimeLong.setText(R.string.mmp_menu_long);
		}
	}

	private void setPhotoDuration() {
		if(Util.PHOTO_4K2K_ON){
			
			type = Photo4K2KPlayActivity.mDelayedTime;
			if (type == Photo4K2KPlayActivity.DELAYED_SHORT) {
				vPTimeLong.setText(R.string.mmp_menu_short);
			} else if (type == Photo4K2KPlayActivity.DELAYED_MIDDLE) {
				vPTimeLong.setText(R.string.mmp_menu_medium);
			} else if (type == Photo4K2KPlayActivity.DELAYED_LONG) {
				vPTimeLong.setText(R.string.mmp_menu_long);
			}
		}else{
			type = PhotoPlayActivity.mDelayedTime;
			if (type == PhotoPlayActivity.DELAYED_SHORT) {
				vPTimeLong.setText(R.string.mmp_menu_short);
			} else if (type == PhotoPlayActivity.DELAYED_MIDDLE) {
				vPTimeLong.setText(R.string.mmp_menu_medium);
			} else if (type == PhotoPlayActivity.DELAYED_LONG) {
				vPTimeLong.setText(R.string.mmp_menu_long);
			}
		}
	}

	private void setShuffle(int type) {
		boolean isShuffle = mLogicManager.getShuffleMode(type);
		if (isShuffle) {
			setShuffleVisble(View.VISIBLE);
		} else {
			setShuffleVisble(View.INVISIBLE);
		}
	}

	private void findMusicView() {
		vVideoRepeata = (TextView) vControlView
				.findViewById(R.id.mmp_video_repeata);
		vPRepeatTv = (TextView) vControlView
				.findViewById(R.id.mmp_pop_repeat_tv);
		// vPRepeat = (LinearLayout) vControlView
		// .findViewById(R.id.mmp_pop_repeat);
		vPShuffle = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_shuffle_img);
		vMStartTime = (TextView) vControlView
				.findViewById(R.id.mmp_pop_music_starttime);
		vMEndTime = (TextView) vControlView
				.findViewById(R.id.mmp_pop_music_endtime);
		vMProgressBar = (ProgressBar) vControlView
				.findViewById(R.id.mmp_pop_music_progress);
		vMVolumeTv = (TextView) vControlView
				.findViewById(R.id.mmp_pop_music_volume);
		vMVolumeBar = (ProgressBar) vControlView
				.findViewById(R.id.mmp_pop_musicvolume_progress);
		mVolumeProgressBg = (LinearLayout) vControlView
				.findViewById(R.id.mmp_volume_progress_bg);

		setRepeat(Const.FILTER_AUDIO);
		setShuffle(Const.FILTER_AUDIO);
		// mute
	}
	/**
	 * if true show video display current playback time and duration in control panel
	 * fix DTV00367339
	 *@param flag
	 */
	public void setVisibility(boolean flag)
	{
		if(flag)
		{
			vMStartTime.setVisibility(View.VISIBLE);
			vMEndTime.setVisibility(View.VISIBLE);
		}else
		{
			//add by yongzhengwei for fix bug DTV00379498 
			 //Modified to setRepeatVisibility()
//			String repeatAll = mContext.getResources().getString(R.string.mmp_menu_repeatall);
//			if(vPRepeatTv.getText().equals(repeatAll)){
//			vPRepeatTv.setVisibility(View.VISIBLE);
//			}else{
//				vPRepeatTv.setVisibility(View.INVISIBLE);
//			}
			vMStartTime.setVisibility(View.INVISIBLE);
			vMEndTime.setVisibility(View.INVISIBLE);
		}
	}
	
	/**
	 * fix reStart play 2D,3D photo , music and video repeatOne reserves problem   
	 */
	public void setRepeatVisibility(int type){
			
			String repeatAll = mContext.getResources().getString(R.string.mmp_menu_repeatall);
			if(vPRepeatTv.getText().equals(repeatAll)){
			vPRepeatTv.setVisibility(View.VISIBLE);
			}else{
				vPRepeatTv.setVisibility(View.INVISIBLE);
				mLogicManager.setRepeatMode(type, Const.REPEAT_NONE);
			}		
	}
	
	/**
	 * if true, display start time and end time, else dont.
	 * @return
	 */
	public boolean isTimeViewVisiable(){
		if (vMStartTime.getVisibility() == View.VISIBLE){
			return true;
		}
		return false;
	}
	
	private void findVideoView() {

		vPRepeatTv = (TextView) vControlView
				.findViewById(R.id.mmp_pop_repeat_tv);

		mRepeatLogo = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_repeat_logo);

		vVideoRepeata = (TextView) vControlView
				.findViewById(R.id.mmp_video_repeata);

		vPZoomSize = (TextView) vControlView
				.findViewById(R.id.mmp_pop_zoomsize);

		vMStartTime = (TextView) vControlView
				.findViewById(R.id.mmp_pop_music_starttime);

		vMEndTime = (TextView) vControlView
				.findViewById(R.id.mmp_pop_music_endtime);

		vMProgressBar = (ProgressBar) vControlView
				.findViewById(R.id.mmp_pop_music_progress);

		vMVolumeTv = (TextView) vControlView
				.findViewById(R.id.mmp_pop_music_volume);

		vMVolumeBar = (ProgressBar) vControlView
				.findViewById(R.id.mmp_pop_musicvolume_progress);

		mVideoTrackNumber = (TextView) vControlView
				.findViewById(R.id.mmp_pop_video_order);

		mVolumeProgressBg = (LinearLayout) vControlView
				.findViewById(R.id.mmp_volume_progress_bg);

		mVideoSubtitle = (TextView) vControlView
				.findViewById(R.id.mmp_pop_subtitle_number);
		mVideoSubtitleIcon = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_subtitle_icon);

		setRepeat(Const.FILTER_VIDEO);

		// Deleted by Dan for fix bug DTV00380300
		// vPZoomSize.setText("1X");

	}

	// Added by Dan for fix bug DTV00380300
	public void setZoomEmpty(){
		vPZoomSize.setText("");
	}
	
	public void setPhotoZoomSize(){
		int size = mLogicManager.getCurrentZoomSize();
		if (size == 0) {
			size = 1;
		} else {
			size = 2 << (size - 1);
		}
		vPZoomSize.setText(size + "X");
	}
	
	public void setZoomSize() {

		if(mLogicManager.getVideoWidth() <= 0 
			||mLogicManager.getVideoHeight() <= 0){

			vPZoomSize.setText("");

		}else{

			int size = mLogicManager.getCurZomm();
			if (size == 0) {
				size = 1;
			} else {
				size = 2 << (size - 1);
			}
			vPZoomSize.setText(size + "X");
		}
	}

	private void findTextView() {

		vPRepeatTv = (TextView) vControlView
				.findViewById(R.id.mmp_pop_repeat_tv);

		vPShuffle = (ImageView) vControlView
				.findViewById(R.id.mmp_pop_shuffle_img);

		vPTimeLong = (TextView) vControlView.findViewById(R.id.mmp_pop_time_tv);

		setRepeat(Const.FILTER_TEXT);
		setShuffle(Const.FILTER_TEXT);
	}

	public void setMediaPlayState() {
		// Modified by Dan for fix bug DTV00376577
		if (isPlaying()) {
			pause();
		} else {
			play();
		}

		if (null != mRepeatLogo && !mRepeatLogo.isShown()) {
			mRepeatLogo.setVisibility(View.VISIBLE);
		}

		if (null != vVideoRepeata && vVideoRepeata.isShown()) {
			vVideoRepeata.setVisibility(View.INVISIBLE);
		}
	}
	
	public void setPlayIcon()
	{
		vPStatePause.setVisibility(View.GONE);
		vPStatePlay.setVisibility(View.VISIBLE);
		mIsPlaying = true;
	}

	public void play() {
		//Moved to  bottom by keke 1.18
		try {
			mControlImp.play();
			vPStatePause.setVisibility(View.GONE);
			vPStatePlay.setVisibility(View.VISIBLE);
			// Added by Dan for fix bug DTV00376577
			mIsPlaying = true;
		} catch (Exception e) {
			MtkLog.i(TAG, e.getMessage());
		}
	}

	public void pause() {

		// vPStatePause.setVisibility(View.VISIBLE);
		// vPStatePlay.setVisibility(View.GONE);

		try {
			// add by shuming fix CR 00386020
			mControlImp.pause();	
			vPStatePause.setVisibility(View.VISIBLE);
			vPStatePlay.setVisibility(View.GONE);
			// end
			// Added by Dan for fix bug DTV00376577
			mIsPlaying = false;
		} catch (Exception e) {
			MtkLog.i(TAG, e.getMessage());
		}
	}
	public void onCapture()
	{
		try {
			mControlImp.pause();
			mIsPlaying = false;
		} catch (Exception e) {
			MtkLog.i(TAG, e.getMessage());
		}
	}
		//add by shuming for fix CR: DTV00407914
	public void setPauseIcon(int visibility)
	{
		vPStatePause.setVisibility(visibility);
		vPStatePlay.setVisibility(View.GONE);
	}
		//add by shuming for fix CR: DTV00407914
	public void setPlayIcon(int visibility)
	{
		vPStatePlay.setVisibility(visibility);
		vPStatePause.setVisibility(View.GONE);
	}
	
	
	
	public void setPauseIconGone(){
		vPStatePause.setVisibility(View.GONE);
		vPStatePlay.setVisibility(View.GONE);
	}

	public void reSetVideo() {
		vPStatePause.setVisibility(View.INVISIBLE);
		vPStatePause.setBackgroundResource(R.drawable.toolbar_top_pause);
		vPStatePlay.setVisibility(View.VISIBLE);
		mIsPlaying = true;
		vVideoRepeata.setVisibility(View.INVISIBLE);
	}

	public short getSubtitleIndex() {
		return subtitleIndex;
	}

	public void setVideoSubtitle(short number, short index) {
		if (number <= 0) {
			mVideoSubtitle.setVisibility(View.INVISIBLE);
			mVideoSubtitleIcon.setVisibility(View.INVISIBLE);
		} else {
			mVideoSubtitle.setVisibility(View.VISIBLE);
			mVideoSubtitleIcon.setVisibility(View.VISIBLE);
			if (index < 0) {
				mLogicManager.setSubOnOff(false);
				mVideoSubtitle.setText(R.string.mmp_lable_subtitle_off);
			} else {
				mLogicManager.setSubtitleTrack(index);
				mVideoSubtitle.setText((index + 1) + "/" + number);
			}
			subtitleIndex = index;
		}
	}

	/**
	 * Initialize subtitle,set subtitle off
	 * 
	 * @param number
	 */
	public void initSubtitle(short number) {
		if (number <= 0) {
			mVideoSubtitle.setVisibility(View.INVISIBLE);
			mVideoSubtitleIcon.setVisibility(View.INVISIBLE);
		} else {
			mVideoSubtitle.setVisibility(View.VISIBLE);
			mVideoSubtitleIcon.setVisibility(View.VISIBLE);
			//modified by keke for DTV00384824
			mLogicManager.setSubtitleTrack((short)255);
			mVideoSubtitle.setText(R.string.mmp_lable_subtitle_off);
			subtitleIndex = -1;
		}
	}

	public void reSetAudio() {
		vPStatePause.setVisibility(View.INVISIBLE);
		vPStatePause.setBackgroundResource(R.drawable.mmp_top_pause);
		vPStatePlay.setVisibility(View.VISIBLE);
		mIsPlaying = true;
		if (null != vVideoRepeata) {
			vVideoRepeata.setVisibility(View.INVISIBLE);
		}
	}

	public void stop() {
		if (null != vVideoRepeata && vVideoRepeata.isShown()) {
			vVideoRepeata.setVisibility(View.INVISIBLE);
		}
		vPStatePause.setVisibility(View.VISIBLE);
		vPStatePause.setBackgroundResource(R.drawable.toolbar_top_stop);
		vPStatePlay.setVisibility(View.GONE);
		// Added by Dan for fix bug DTV00376577
		mIsPlaying = false;
	}

	public void onFast(int speed, int status, int type) {
		vPStatePlay.setVisibility(View.INVISIBLE);
		mIsPlaying = false;
		vPStatePause.setVisibility(View.INVISIBLE);
		vVideoRepeata.setVisibility(View.VISIBLE);

		if (speed == 1) {
			vPStatePlay.setVisibility(View.VISIBLE);
			mIsPlaying = true;
			vVideoRepeata.setVisibility(View.INVISIBLE);
			return;
		}

		vVideoRepeata.setText(speed + "x");
		Drawable img_left = null;
		if (type == Const.FILTER_VIDEO) {
			if (null != mRepeatLogo && mRepeatLogo.isShown()) {
				mRepeatLogo.setVisibility(View.INVISIBLE);
			}
			vVideoRepeata.setTextColor(Color.WHITE);
		}

		if (status == 0) {
			if (type == Const.FILTER_VIDEO) {
				img_left = mContext.getResources().getDrawable(
						R.drawable.toolbar_typeicon_ff_video);
			} else {
				img_left = mContext.getResources().getDrawable(
						R.drawable.toolbar_typeicon_ff);
			}
		} else if (status == 1) {
			if (type == Const.FILTER_VIDEO) {
				img_left = mContext.getResources().getDrawable(
						R.drawable.toolbar_typeicon_rew_video);
			} else {
				img_left = mContext.getResources().getDrawable(
						R.drawable.toolbar_typeicon_rew);

			}
		} else if (status == 2) {
			img_left = mContext.getResources().getDrawable(
					R.drawable.mmp_toolbar_typeicon_video_paly);

			vVideoRepeata.setText("1/" + speed + "x");

		}
		if(img_left != null){
			img_left.setBounds(0, 0, img_left.getMinimumWidth(), img_left
				.getMinimumHeight());
			}
		
		vVideoRepeata.setCompoundDrawables(img_left, null, null, null);
	}

	public void setSlow() {

	}

	public boolean isPlaying() {
		switch ( mediaType ) {
		case MultiMediaConstant.VIDEO:
			return mLogicManager.getVideoPlayStatus() == VideoConst.PLAY_STATUS_STARTED;
		case MultiMediaConstant.AUDIO:
			return mLogicManager.getAudioStatus() == AudioConst.PLAY_STATUS_STARTED;
		default:
			break;
		}
		return mIsPlaying;
		// return vPStatePlay.isShown();
	}

	public void setFileName(String name) {
		vPFileName.setText(name);
	}

	public void setFilePosition(String pagesize) {
		vPOrder.setText(pagesize);
	}

	public void setVideoRepeat(int type) {

		// switch (type) {
		// case Const.REPEAT_NONE: {
		// mRepeatLogo.setBackgroundResource(0);
		// break;
		// }
		// case Const.REPEAT_ONE: {
		// mRepeatLogo.setBackgroundResource(R.drawable.toolbar_toprepeat_a);
		// break;
		// }
		// case Const.REPEAT_ALL: {
		// mRepeatLogo.setBackgroundResource(R.drawable.toolbar_toprepeat_ab);
		// }
		// default:
		// break;
		// }

	}

	public void setRepeatAll() {
		vPRepeatTv.setVisibility(View.VISIBLE);
		vPRepeatTv.setCompoundDrawables(repeatAll, null, null, null);
		vPRepeatTv
				.setText(mContext.getString(R.string.mmp_pop_repeat_tvrepeat));
	}

	public void setRepeatSingle() {
		vPRepeatTv.setVisibility(View.VISIBLE);
		vPRepeatTv.setCompoundDrawables(repeatOne, null, null, null);
		vPRepeatTv
				.setText(mContext.getString(R.string.mmp_pop_repeat_tvsingle));

	}

	public void setRepeatNone() {
		vPRepeatTv.setVisibility(View.INVISIBLE);
		 //add by yongzhengwei for fix bug DTV00379498 
		vPRepeatTv.setCompoundDrawables(null, null, null, null);
		vPRepeatTv
				.setText(mContext.getString(R.string.mmp_pop_repeat_tvnone));
	}

	public void setShuffleVisble(int visibility) {
		if (vPShuffle != null) {
			vPShuffle.setVisibility(visibility);
		}

	}

	public void setPhotoZoom(String scale) {
		vPZoomSize.setText(scale);
	}

	public void setPhotoTimeType(String type) {
		vPTimeLong.setText(type);
	}

	public void setPhotoAnimationEffect(String animation) {
		vPView.setText(animation);
	}

	public void setPhotoAnimationEffect(int type) {
		String value = "";
		switch (type) {
		case ConstPhoto.DEFAULT:
			value = mContext.getString(R.string.mmp_menu_none);
			break;
		case ConstPhoto.dissolve:
			value = mContext.getString(R.string.mmp_menu_dissolve);
			break;

		case ConstPhoto.wipe_right:
			value = mContext.getString(R.string.mmp_menu_wiperight);
			break;
		case ConstPhoto.wipe_left:
			value = mContext.getString(R.string.mmp_menu_wipeleft);
			break;
		case ConstPhoto.wipe_top:
			value = mContext.getString(R.string.mmp_menu_wipeup);
			break;
		case ConstPhoto.wipe_bottom:
			value = mContext.getString(R.string.mmp_menu_wipedown);
			break;
		case ConstPhoto.box_in:
			value = mContext.getString(R.string.mmp_menu_boxin);
			break;
		case ConstPhoto.box_out:
			value = mContext.getString(R.string.mmp_menu_boxout);
			break;
		case ConstPhoto.RADNOM:
			value = mContext.getString(R.string.mmp_menu_random);
			break;
		default:
			break;
		}
		vPView.setText(value);
	}

	public void setCurrentTime(long mills) {
		mills /= 1000;
		long minute = mills / 60;
		long hour = minute / 60;
		long second = mills % 60;
		minute %= 60;
		vMStartTime.setText(String.format("%02d:%02d:%02d", hour, minute,
				second));
	}

	public void setEndtime(int mills) {
		mills /= 1000;
		int minute = mills / 60;
		int hour = minute / 60;
		int second = mills % 60;
		minute %= 60;
		//String text = "";
		try {
			vMEndTime.setText(String.format("%02d:%02d:%02d", hour, minute, second));
			//text = String.format("%02d:%02d:%02d", hour, minute, second);
		} catch (Exception e) {
			vMEndTime.setText("");
		}
		
	}

	public void setProgressMax(int max) {
		vMProgressBar.setMax(max);
	}

	public int getProgressMax(){
		return vMProgressBar.getMax();
	}
	public void setProgress(int progress) {
		vMProgressBar.setProgress(progress);
	}

	public void setVolumeMax(int max) {
		vMVolumeBar.setMax(max);
	}

	public void setCurrentVolume(int volume) {
		if (null == vMVolumeBar) {
			return;
		}

		vMVolumeBar.setProgress(volume);

	}

	public void setMute(boolean isMute) {
		if (null == vMVolumeBar) {
			return;
		}

		if (null != mVolumeProgressBg) {
			if (isMute) {
				mVolumeProgressBg.setVisibility(View.INVISIBLE);
			} else if (mVolumeProgressBg.getVisibility() != View.VISIBLE) {
				mVolumeProgressBg.setVisibility(View.VISIBLE);
			}

		}

		if (null != vMVolumeTv) {
			
			if (null != vMVolumeTv.getTag()
					&& (Boolean) vMVolumeTv.getTag()) {
				Drawable img_left = mContext.getResources().getDrawable(
						R.drawable.mmp_toolbar_icon_volume);
				img_left.setBounds(0, 0, img_left.getMinimumWidth(), img_left
						.getMinimumHeight());
				vMVolumeTv.setCompoundDrawables(img_left, null, null, null);
				vMVolumeTv.setTag(Boolean.FALSE);
			}else if (isMute) {
				Drawable img_left = mContext.getResources().getDrawable(
						R.drawable.mmp_toolbar_icon_mute);
				img_left.setBounds(0, 0, img_left.getMinimumWidth(), img_left
						.getMinimumHeight());
				vMVolumeTv.setCompoundDrawables(img_left, null, null, null);
				vMVolumeTv.setTag(Boolean.TRUE);
			} 
		}

	}
	
	public void reSetPause(){
    	vPStatePause.setBackgroundResource(R.drawable.toolbar_top_pause);
    }
	//Added by keke 1202 for fix bug DTV00379478
	public void setInforbarNull(){
		vMStartTime.setVisibility(View.INVISIBLE);
		vMEndTime.setText("");
		vMProgressBar.setProgress(0);
		vPZoomSize.setText("");
		mVideoTrackNumber.setText("");
		mVideoSubtitle.setVisibility(View.INVISIBLE);
		mVideoSubtitleIcon.setVisibility(View.INVISIBLE);
		vPOrder.setText("");
		vPFileName.setText("");
		if (null != vVideoRepeata) {
			vVideoRepeata.setVisibility(View.INVISIBLE);
		}
	}
	// Add by keke 1215 for fix cr DTV00383194
	private void setInforbarTransparent() {
		try {	
			LinearLayout m = (LinearLayout) vControlView.findViewById(R.id.mmp_popwindow);
			m.getBackground().setAlpha(220);
			LinearLayout m2 = (LinearLayout) vControlView.findViewById(R.id.mmp_popwindow_Operator_Message);
			m2.getBackground().setAlpha(220);
		} catch (Exception e) {
		   e.printStackTrace();
		}
	}
	public void hideProgress() {
		vMProgressBar.setVisibility(View.INVISIBLE); 
		vMStartTime.setVisibility(View.INVISIBLE);
		vMEndTime.setVisibility(View.INVISIBLE);
	}

	public void showProgress() {	
		vMStartTime.setVisibility(View.VISIBLE);
		vMEndTime.setVisibility(View.VISIBLE);
		vMProgressBar.setVisibility(View.VISIBLE);
	}
}
