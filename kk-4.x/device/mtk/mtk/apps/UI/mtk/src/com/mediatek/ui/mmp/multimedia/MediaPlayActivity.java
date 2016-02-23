package com.mediatek.ui.mmp.multimedia;

import java.util.ArrayList;

import java.io.File;
import java.util.ArrayList;
import java.util.List;


import com.mediatek.mmpcm.fileimpl.MtkFile;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.mmcimpl.PlayList;

import com.mediatek.ui.mmp.model.FileAdapter;
import com.mediatek.ui.mmp.model.LocalFileAdapter;

import android.util.Log;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.MessageQueue.IdleHandler;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;
import com.mediatek.ui.mmp.model.MultiFilesManager;

import com.mediatek.mmpcm.CommonSet;
import com.mediatek.mmpcm.CommonStorage;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.tvcm.TVContent;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.mmp.commonview.ControlView;
import com.mediatek.ui.mmp.commonview.MenuListView;
import com.mediatek.ui.mmp.commonview.ScoreView;
import com.mediatek.ui.mmp.commonview.ShowInfoView;
import com.mediatek.ui.mmp.commonview.TipsDialog;
import com.mediatek.ui.mmp.commonview.ControlView.ControlPlayState;
import com.mediatek.ui.mmp.gamekit.content.MutilMediaConst;
import com.mediatek.ui.mmp.gamekit.filebrowse.MtkFileFlimBrowseActivity;
import com.mediatek.ui.mmp.util.GetDataImp;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MenuFatherObject;
import com.mediatek.ui.mmp.util.ModelConstant;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;
import android.os.SystemProperties;
import com.mediatek.ui.mmp.util.TextUtils;
import android.graphics.Color;
import android.graphics.Typeface;

/**
 * Multi-media play activty
 * 
 * @author hs_weihuiluo
 */
public class MediaPlayActivity extends Activity {

	/**
	 * Log tag
	 */
	protected static final String TAG = "MediaPlayActivity";

	/**
	 * Media content type :photo audio video text
	 */
	private static int sMediaType = 0;

	/**
	 * Repeat key duration
	 */
	public static final int KEY_DURATION = 400;

	/**
	 * Message to update not support tips dialog
	 */
	private static final int MSG_UPDATE_NOT_SUPPORT = 0;

	/**
	 * Message to dismiss feature not support dialog
	 */
	private static final int MSG_DISMISS_FEARTURE_NOT_SUPPORT = 1;

	/**
	 * Message to show feature not support dialog
	 */
	private static final int MSG_SHOW_FEATURE_NOT_SUPPORT = 2;
    
	private static final int MSG_HIDE_INFORBAR_POINT=4;
	/**
	 * Update not support tips dialog delay milliseconds
	 */
	private static final int MSG_DELAY = 1000;

	/**
	 * Dismiss feature not support tips dialog delay milliseconds
	 */
	protected static final int MSG_DISMISS_DELAY = 2000;

	public static final int DELAYED_LONG = 8000;
	
	public static final int DELAYED_MIDDLE = 5000;
	
	public static final int DELAYED_SHORT = 3000;

	/**
	 * Last key down milliseconds
	 */
	protected long mLastKeyDownTime;

	/**
	* for user press epg key , avoid auto next play.
	*/
	protected boolean EPG_KEY_PRESS = false;

	/**
	 * {@link Resources}
	 */
	private Resources mResources;

	/**
	 * The screen width
	 */
	protected int mDisPlayWidth;

	/**
	 * The screen height
	 */
	protected int mDisPlayHeight;

	/**
	 * {@link ControlView}
	 */
	protected ControlView mControlView;

	/**
	 * Show menu dialog
	 */
	protected MenuListView menuDialog;
	
	protected MenuListView menuDialogFontList;

	/**
	 * Show Lyric view
	 */
	protected ScoreView mScoreView;

	/**
	 * Show info view
	 */
	protected ShowInfoView mInfo;

	/**
	 * {@link LogicManager}
	 */
	protected LogicManager mLogicManager;

	/**
	 * Max volume value
	 */
	protected int maxVolume = 0;

	/**
	 * The current volume value
	 */
	protected int currentVolume = 0;

	/**
	 * Lyric lines per screen
	 */
	protected static int mPerLine = 8;

	/**
	 * Tips dialog
	 */
	protected TipsDialog mTipsDialog;

	/**
	 * Control bar contentView
	 */
	protected View contentView;

	/**
	 * Control bar is showing flag
	 */
	protected boolean isControlBarShow = false;

	/**
	 * Resume from capureLog flag
	 */
	protected boolean isBackFromCapture = false;

	protected boolean isHideSperum = false;

	protected static final String  AUTO_TEST_PROPERTY = "mtk.auto_test";

	/**
	 * Not support flag
	 */
	
	protected boolean SCREENMODE_NOT_SUPPORT = false;
	protected boolean isSetPicture = false;
	
	//add by keke for DTV00383992
	protected boolean isNotSupport = false;

	/**
	 * Last not support content(used to switch from feature not support)
	 */
	public enum PlayException {
		DEFAULT_STATUS,VIDEO_NOT_SUPPORT,VIDEO_ONLY, AUDIO_NOT_SUPPORT, FILE_NOT_SUPPORT, AV_NOT_SUPPORT
	}
	protected PlayException playExce = PlayException.DEFAULT_STATUS;

	/**
	 * The current not support content
	 */
	protected String mTitle;
	public static String  mPhotoFramePath;
	public static int  mPhotoFrameSource;

	// Added by Dan for fix bug DTV00373545
	private boolean mIsMute;

	//add for fix bug DTVDTV00392376
	private boolean mIsActiveLiving = true;

	/**
	 * {@link ListView.OnItemClickListener}
	 */
	private ListView.OnItemClickListener mListener = new ListView.OnItemClickListener() {
		public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
				long arg3) {
			TextView tvTextView = (TextView) arg1
					.findViewById(R.id.mmp_menulist_tv);
			String content = tvTextView.getText().toString();
			controlState(content);
		}
	};
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
	
		// TODO Auto-generated method stub
		super.onConfigurationChanged(newConfig);
	}
	/**
	 * An handler used to send message
	 */
	private Handler mHandler = new Handler() {

		/**
		 * {@inheritDoc}
		 */
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			
			//add for fix bug DTVDTV00392376
			if (!mIsActiveLiving) {
				return;
			}
			switch (msg.what) {
			case MSG_DISMISS_FEARTURE_NOT_SUPPORT: {
				if ( playExce == PlayException.AUDIO_NOT_SUPPORT) {
					onNotSuppsort(mResources
							.getString(R.string.mmp_audio_notsupport));
				}else if(playExce == PlayException.VIDEO_NOT_SUPPORT){
					onNotSuppsort(mResources
							.getString(R.string.mmp_video_notsupport));
				}else if(playExce == PlayException.VIDEO_ONLY){
					onNotSuppsort(mResources
							.getString(R.string.mmp_video_only));
				}else{
					dismissNotSupprot();
				}
				
				break;
			}
			case MSG_SHOW_FEATURE_NOT_SUPPORT: {
				onNotSuppsort(mTitle);
				break;
			}
			
			case MSG_HIDE_INFORBAR_POINT:
				if(mControlView != null){
					if(mControlView.getWidth()==1&&mControlView.getHeight()==1){
						   mControlView.update(-1,-1,-1,-1);
						  // mControlView.hiddlen(View.INVISIBLE);
						}
				}
				break;
			default:
				break;
			}
		}

	};

	/**
	 * {@inheritDoc}
	 */
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		DestroyApp des = (DestroyApp) this.getApplication();
		des.add(this);
		mResources = MediaPlayActivity.this.getResources();
		mLogicManager = LogicManager.getInstance(this);
		// Added by Dan for fix bug DTV00373545
		mIsMute = mLogicManager.isMute();
		
		//add for fix bug DTVDTV00392376
		mIsActiveLiving = true;
		//For 3D gamekit
		get3DExtraData();
	}

	protected boolean autoTest(int constFilter,int mulFilter ){

		Log.d(TAG,"autoTest SystemProperties.getInt(AUTO_TEST_PROPERTY,0):"+SystemProperties.getInt(AUTO_TEST_PROPERTY,0));
		if(0!=SystemProperties.getInt(AUTO_TEST_PROPERTY,0)||getIntent().getData()!=null){
			String dataStr = getIntent().getDataString();
			Log.d(TAG,"autoTest dataStr:"+dataStr);
			if(dataStr != null && dataStr.length()>0){
				File f = new File(dataStr);;
				
				if(!f.exists()){ 
					Log.d(TAG,"!f.exists()");
					return false;
				}
			
				PlayList.getPlayList().cleanList(constFilter);
				LocalFileAdapter  file = new LocalFileAdapter(new MtkFile(f));
				List<FileAdapter> files = new ArrayList<FileAdapter>();
				files.add(file);
				PlayList playlist = MultiFilesManager.getInstance(this).getPlayList(files, 0, mulFilter, MultiFilesManager.SOURCE_LOCAL);
				Log.d(TAG,"playlist path:"+ playlist.getCurrentPath(constFilter));
				return true;
			}
			
		}
		
		
		return false;


	}

	//For 3D gamekit start
	protected int mFrom3DBrowse =MutilMediaConst.CONTENT_UNKNOW;
	//private int m3DFromMode = -1;
	//private String m3DFromPath = null;
	protected Bundle mExtras;
	protected void get3DExtraData(){
		mExtras = getIntent().getExtras();
		if (mExtras != null)
			mFrom3DBrowse= mExtras.getInt(MtkFileFlimBrowseActivity.TO_3DBROWSE, 
					MutilMediaConst.CONTENT_UNKNOW);
	}
	
	protected void isBack3DBrowseMode(){
		if (mFrom3DBrowse != MutilMediaConst.CONTENT_UNKNOW){
			Intent intent = new Intent(this, MtkFileFlimBrowseActivity.class);
			mExtras.putInt(MtkFileFlimBrowseActivity.FROM_PLAYTYPE, mFrom3DBrowse);
			mExtras.putSerializable(MtkFileFlimBrowseActivity.CURRENT_BROWSE_STATUS, 
					MtkFileFlimBrowseActivity.BrowseStatus.FROM_PLAY_STATUS);		
			intent.putExtras(mExtras);
			startActivity(intent);
		}
	}
	/**
	 * show not support tips dialog
	 * 
	 * @param title
	 *            the tips dialog content
	 */
	private void onNotSuppsort(String title) {

		MtkLog.i(TAG, "onNotSuppsort  :" + title);
		if (null == mTipsDialog) {
			mTipsDialog = new TipsDialog(this);
			mTipsDialog.setText(title);
			mTipsDialog.show();
			mTipsDialog.setBackground(R.drawable.toolbar_playerbar_test_bg);
			Drawable drawable = this.getResources().getDrawable(
					R.drawable.toolbar_playerbar_test_bg);

			int weight = (int) (drawable.getIntrinsicWidth() * 0.6);
			int height = drawable.getIntrinsicHeight();
			mTipsDialog.setDialogParams(weight, height);

          
			 WindowManager m = mTipsDialog.getWindow().getWindowManager();
			 Display display = m.getDefaultDisplay();

			 int x = -((int) (ScreenConstant.SCREEN_WIDTH) - weight / 2)
					+ (int) (ScreenConstant.SCREEN_WIDTH / 10);
			 int y = (int) (ScreenConstant.SCREEN_HEIGHT* 3 / 8 -ScreenConstant.SCREEN_HEIGHT*0.16 - height/ 2);
			 mTipsDialog.setWindowPosition(x, y);
			
		} else {
			 try {
				  mTipsDialog.setText(title);
			      mTipsDialog.show();
			} catch (Exception e) {
				// TODO: handle exception
				e.printStackTrace();
			}
		
		}
//		mLastText = title;
	}
	
	
	public void finish() {
		dismissNotSupprot();
		//removeControlView();
		
		//For 3D gamekit start
		isBack3DBrowseMode();
		//For 3D gamekit end.
		super.finish();
	}

	/**
	 * Show feature not support dialog
	 * 
	 * @param title
	 *            the dialog content
	 */
	protected void featureNotWork(String title) {
		new Exception().printStackTrace();

		mTitle = title;
		mHandler.sendEmptyMessage(MSG_SHOW_FEATURE_NOT_SUPPORT);
		if (mHandler.hasMessages(MSG_DISMISS_FEARTURE_NOT_SUPPORT)) {
					mHandler.removeMessages(MSG_DISMISS_FEARTURE_NOT_SUPPORT);
		}
		mHandler.sendEmptyMessageDelayed(MSG_DISMISS_FEARTURE_NOT_SUPPORT,MSG_DISMISS_DELAY);
	}

	/**
	 * Remove feature not support messages
	 */
	protected void removeFeatureMessage() {
		mHandler.removeMessages(MSG_DISMISS_FEARTURE_NOT_SUPPORT);
		mHandler.removeMessages(MSG_SHOW_FEATURE_NOT_SUPPORT);

	}

	/**
	 * {@inheritDoc}
	 */
	protected void onResume() {
		super.onResume();
//		if (isBackFromCapture) {
//			/* fix bug DTV00364858 by lei 1013 */
//			 if (null != mControlView) {
//			 mControlView.play();
//			 }
//			showController();
//		}
//		isBackFromCapture = false;
	}

	/**
	 * Dismiss not support tips dialog
	 */
	protected void dismissNotSupprot() {
             try {
				if (null != mTipsDialog && mTipsDialog.isShowing()) {
				mTipsDialog.dismiss();
			    }
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	protected void dismissMenuDialog() {
        try {
        	if (null != menuDialog && menuDialog.isShowing()) {
				menuDialog.dismiss();
		    }
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Remove lyrics
	 * 
	 * @param ishide
	 *            is hidden lyrics view
	 */
	public void removeScore(boolean ishide) {
	}

	/**
	 * Setup lyrics lines per screen
	 * 
	 * @param perline
	 *            line number
	 */
	public void setLrcLine(int perline) {
	}

	/**
	 * Is show spectrum
	 * 
	 * @return true:show spectrum, false: hidden spectrum
	 */
	public boolean isShowSpectrum() {
		return false;
	}
	
	/**
	 * Is the current audio has lyrics
	 * 
	 * @return true:has lyrics, false: no lyrics
	 */
	public boolean hasLrc() {
		return false;
	}

	/**
	 * Hidden lyrics view
	 */
	public void hideLrc() {
	}

	/**
	 * Initialize volume
	 * 
	 * @param manager
	 */
	protected void initVulume(LogicManager manager) {
		mLogicManager = manager;
		maxVolume = mLogicManager.getMaxVolume();
		currentVolume = mLogicManager.getVolume();
		mControlView.setMute(mLogicManager.isMute());
	}

	/**
	 * Initialize control bar
	 * 
	 * @param resource
	 *            Control bar Layout resource id
	 * @param mediatype
	 *            Media type
	 * @param controlImp
	 *            ControlPlayState:control play or pause
	 */
	protected void getPopView(int resource, int mediatype,
			ControlPlayState controlImp) {
		sMediaType = mediatype;
		MtkLog.i(TAG, " getPopView sMediaType:"+sMediaType);
		contentView = LayoutInflater.from(MediaPlayActivity.this).inflate(
				resource, null);
		mDisPlayWidth = ScreenConstant.SCREEN_WIDTH;
		mDisPlayHeight = ScreenConstant.SCREEN_HEIGHT;
		mControlView = new ControlView(this, sMediaType, controlImp,
				contentView, mDisPlayWidth * 4 / 5, mDisPlayHeight * 4 / 5);
	}

	/**
	 * Show control bar
	 * 
	 * @param topview
	 *            Control bar parent view
	 */
	protected void showPopUpWindow(final View topview) {
		Looper.myQueue().addIdleHandler(new IdleHandler() {

			public boolean queueIdle() {
				MtkLog
						.i(TAG,
								"---------- showPopUpWindow   IdleHandler-------------");
				mControlView.showAtLocation(topview,
						Gravity.LEFT | Gravity.TOP, mDisPlayWidth / 10,
						mDisPlayHeight / 10);
				isControlBarShow = true;
				return false;
			}
		});
	}

	/**
	 * {@inheritDoc}
	 */
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		case KeyMap.KEYCODE_DPAD_CENTER:
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE: {
			reSetController();
			if (mControlView != null){
				mControlView.setMediaPlayState();
			}
			return true;
			//break;
		}
		case KeyMap.KEYCODE_MTKIR_INFO: {
			onInfoClick();
			return true;
		}
		case KeyMap.KEYCODE_VOLUME_UP:
			MtkLog.d(TAG,"KEYCODE_VOLUME_UPs MediaType = "+sMediaType);
			if (sMediaType == MultiMediaConstant.VIDEO
					|| sMediaType == MultiMediaConstant.AUDIO) {
				reSetController();
			}
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
		case KeyMap.KEYCODE_VOLUME_DOWN:
			if (sMediaType == MultiMediaConstant.VIDEO
					|| sMediaType == MultiMediaConstant.AUDIO) {
				reSetController();
			}
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
		case KeyMap.KEYCODE_MENU:
			reSetController();
			if (null != menuDialog && menuDialog.isShowing()) {
				menuDialog.dismiss();

			} else {
				showDialog();
			}
			break;
		case KeyMap.KEYCODE_MTKIR_ANGLE: {
//			LogicManager.getInstance(this).restoreVideoResource();
//			LogicManager.getInstance(this).finishAudioService();
//			LogicManager.getInstance(this).finishVideo();
//			DestroyApp destroyApp = (DestroyApp) getApplicationContext();
//			destroyApp.finishAll();
			Util.exitMmpActivity(this);
			break;
		}
		case KeyMap.KEYCODE_MTKIR_REPEAT: {
			reSetController();
			onRepeat();
			updateInfoView();
			break;
		}
		case KeyMap.KEYCODE_MTKIR_MUTE: {
			if (sMediaType == MultiMediaConstant.VIDEO
					|| sMediaType == MultiMediaConstant.AUDIO) {
				reSetController();
			}
			onMute();
			return true;
		}
		case KeyMap.KEYCODE_BACK: {
			dismissMenuDialog();
			dismissNotSupprot();
			removeControlView();
			break;
		}
		case KeyMap.KEYCODE_MTKIR_GUIDE:{
			EPG_KEY_PRESS = true;
			Util.startEPGActivity(this);
			break;
		}
		default:
			break;
		}
		return super.onKeyDown(keyCode, event);
	}

	/**
	 * Is the current key down valid
	 * 
	 * @return true:valid,false:invalid
	 */
	protected boolean isValid() {
		long currentTime = System.currentTimeMillis();
		if ((currentTime - mLastKeyDownTime) >= KEY_DURATION) {
			mLastKeyDownTime = currentTime;
			return true;
		} else {
			MtkLog.i(TAG, " key down duration :"
					+ (currentTime - mLastKeyDownTime) + "< 400 mmm");
			mLastKeyDownTime = currentTime;
			return false;
		}
	}

	/**
	 * Show or hidden info menu
	 */
	private void onInfoClick() {

			if (!isControlBarShow) {
			reSetController();
				return;
		}
		if (null != mInfo && mInfo.isShowing()) {
				if (sMediaType != MultiMediaConstant.AUDIO) {
					hideController();
				}
				mInfo.dismiss();
				return;
		}
		hideControllerDelay();
		showinfoview(sMediaType);

	}
	protected void updateInfoView() {
		if(null != mInfo && mInfo.isShowing()){
		  mInfo.updateView();	
		}
	}

	/**
	 * Set mute or resume from mute
	 */
	public void onMute() {
		
		mLogicManager.setMute();
		// Added by Dan for fix bug DTV00373545
		//mIsMute = ! mIsMute;
		mIsMute =mLogicManager.isMute();
		// Modified by Dan for fix bug DTV00373545
		mControlView.setMute(mIsMute);
		removeScore(mIsMute);
		
	}

	/**
	 * Switch repeat mode
	 */
	protected void onRepeat() {
		MtkLog.i(TAG, "onRepeat~~");

		if (null == mControlView) {
			MtkLog.i(TAG, "onRepeat mControlView = null");
			return;
		}
		int type;
		MtkLog.i(TAG, "onRepeat mControlView != null sMediaType:"+sMediaType);
		switch (sMediaType) {
		case MultiMediaConstant.AUDIO: {
			type = Const.FILTER_AUDIO;
			break;
		}
		case MultiMediaConstant.VIDEO: {
			type = Const.FILTER_VIDEO;
			break;
		}
		case MultiMediaConstant.PHOTO: {
			type = Const.FILTER_IMAGE;
			break;
		}
		case MultiMediaConstant.TEXT: {
			type = Const.FILTER_TEXT;
			break;
		}
		case MultiMediaConstant.THRD_PHOTO: {
			type = Const.FILTER_IMAGE;
			break;
		}

		default:
			type = 0;
			break;
		}
		int model = mLogicManager.getRepeatModel(type);

		MtkLog.i(TAG, "onRepeat mediatype = "+ type +"repeatmode = "+ model);

		switch (model) {
			case Const.REPEAT_NONE: {
				mControlView.setRepeatSingle();
				mLogicManager.setRepeatMode(type, Const.REPEAT_ONE);
				break;
			}
			case Const.REPEAT_ONE: {
				mControlView.setRepeatAll();
				mLogicManager.setRepeatMode(type, Const.REPEAT_ALL);
				break;
			}
			case Const.REPEAT_ALL: {
				mControlView.setRepeatNone();
				mLogicManager.setRepeatMode(type, Const.REPEAT_NONE);
				break;
			}
			default:
				break;
		}
		
		writeRepeatMode(type);
	}

	private void writeRepeatMode(int type) {
		// TODO Auto-generated method stub
		int model = mLogicManager.getRepeatModel(type);
		MtkLog.i(TAG,"writeRepeatMode model:"+model);

		MtkLog.i(TAG, "onRepeat type = "+ sMediaType +"model = "+ model);	
		Util.setMediaRepeatMode(getApplicationContext(), sMediaType, model);
	}

	/**
	 * Show menu dialog
	 */
	private void showDialog() {

		if (sMediaType == MultiMediaConstant.AUDIO) {
			if(isNotSupport || getPlayerStop() ) {
				ArrayList<MenuFatherObject> menunotList = GetDataImp.getInstance()
				.getComMenu(MediaPlayActivity.this,
						R.array.mmp_menu_musicplaynotsupportlist,
						R.array.mmp_menu_musicplaynotsupportlist_enable,
						R.array.mmp_menu_musicplaynotsupportlist_hasnext);
					menuDialog = new MenuListView(MediaPlayActivity.this, menunotList,
						mListener, null);

				if (null != mLogicManager) {
					boolean isShuffle = mLogicManager
							.getShuffleMode(Const.FILTER_AUDIO);
					if (isShuffle) {
						menuDialog.setItem(1, mResources
								.getString(R.string.mmp_menu_shuffleoff));
					}
				}
				menuDialog.mControlView(MediaPlayActivity.this);	
			}else {
			ArrayList<MenuFatherObject> menuList = GetDataImp.getInstance()
					.getComMenu(MediaPlayActivity.this,
							R.array.mmp_menu_musicplaylist,
							R.array.mmp_menu_musicplaylist_enable,
							R.array.mmp_menu_musicplaylist_hasnext);
			if (!hasLrc()) {
				if (menuList.size() > 5) {
					menuList.get(5).enable = false;
				}
			}

			if (isShowSpectrum()) {
				menuList.get(4).content = mResources
						.getString(R.string.mmp_menu_hidescore);
			} else {
				menuList.get(4).content = mResources
						.getString(R.string.mmp_menu_showscore);
			}
			menuDialog = new MenuListView(MediaPlayActivity.this, menuList,
					mListener, null);

			if (null != mLogicManager) {
				boolean isShuffle = mLogicManager
						.getShuffleMode(Const.FILTER_AUDIO);
				if (isShuffle) {
					menuDialog.setItem(2, mResources
							.getString(R.string.mmp_menu_shuffleoff));

				}
			}
			menuDialog.mControlView(MediaPlayActivity.this);
			}
		} else if (sMediaType == MultiMediaConstant.VIDEO) {
			
			
			menuDialog = new MenuListView(MediaPlayActivity.this, GetDataImp
					.getInstance().getComMenu(MediaPlayActivity.this,
							R.array.mmp_menu_videoplaylist,
							R.array.mmp_menu_videoplaylist_enable,
							R.array.mmp_menu_videoplaylist_hasnext), mListener,mCallBack);
						
			if (SCREENMODE_NOT_SUPPORT
					|| mLogicManager.getVideoPlayStatus() == VideoConst.PLAY_STATUS_STOPPED) {
				menuDialog.setItemEnabled(4, false);
			} else {
				menuDialog.setItemEnabled(4, true);
			}

			if (SCREENMODE_NOT_SUPPORT ) {
				menuDialog.setItemEnabled(5, false);
			} else {
				menuDialog.setItemEnabled(5, true);
			}

			// fix cr DTV00406664 by xiaoyao
		//	menuDialog.setItemEnabled(4, !isNotSupport);
			
		} else if (sMediaType == MultiMediaConstant.TEXT) {
			// Added by Dan for fix bug DTV00375629
			menuDialog = new MenuListView(MediaPlayActivity.this, GetDataImp
					.getInstance().getComMenu(MediaPlayActivity.this,
							R.array.mmp_menu_textplaylist,
							R.array.mmp_menu_textplaylist_enable,
							R.array.mmp_menu_textplaylist_hasnext), mListener,
					null);
			menuDialog.setItemEnabled(0, !isNotSupport);
			menuDialog.setItemEnabled(3, !isNotSupport);
			
			if (null != mLogicManager) {
				boolean isShuffle = mLogicManager
						.getShuffleMode(Const.FILTER_TEXT);
				if (isShuffle) {
					menuDialog.setItem(2, mResources
							.getString(R.string.mmp_menu_shuffleoff));

				}
			}
		} else {
			menuDialog = new MenuListView(MediaPlayActivity.this, GetDataImp
					.getInstance().getComMenu(MediaPlayActivity.this,
							R.array.mmp_menu_textplaylist,
							R.array.mmp_menu_textplaylist_enable,
							R.array.mmp_menu_textplaylist_hasnext), mListener,
					null);
		}
		if (null != mControlView && (!isNotSupport||sMediaType == MultiMediaConstant.TEXT) && !getPlayerStop()) {
				if (mControlView.isPlaying()) {
				menuDialog.setItem(0, mResources
						.getString(R.string.mmp_menu_pause));
			} else {
				menuDialog.setItem(0, mResources
						.getString(R.string.mmp_menu_play));
			}
		}
		menuDialog.setMediaType(sMediaType);
		menuDialog.show();
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onStop() {

		if (null != mInfo && mInfo.isShowing()) {
			mInfo.dismiss();
			mInfo = null;
		}
		super.onStop();
	}

	/**
	 * Dismiss control bar
	 */
	protected void removeControlView() {
		if (mControlView != null && mControlView.isShowing()) {
			try {
				mControlView.dismiss();
			} catch (Exception e) {
				// TODO: handle exception
			}
			mControlView = null;
			contentView = null;
		}
	}

	/**
	 * {@inheritDoc}
	 */
	protected void onDestroy() {
		removeControlView();
		((DestroyApp) getApplication()).remove(this);
		super.onDestroy();
		//add for fix bug DTVDTV00392376
		mIsActiveLiving = false;
	}

	/**
	 * Click menu item callback
	 * 
	 * @param content
	 *            menu item value
	 */
	private void controlState(String content) {
		if (sMediaType == MultiMediaConstant.AUDIO) {
			if (content.equals(mResources.getString(R.string.mmp_menu_none))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.AUDIO,Util.NONE);
				mControlView.setRepeatNone();
				mLogicManager.setRepeatMode(Const.FILTER_AUDIO,
						Const.REPEAT_NONE);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_repeatone))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.AUDIO,Util.REPEATE_ONE);
				mControlView.setRepeatSingle();
				mLogicManager.setRepeatMode(Const.FILTER_AUDIO,
						Const.REPEAT_ONE);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_repeatall))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.AUDIO,Util.REPEATE_ALL);
				mControlView.setRepeatAll();
				mLogicManager.setRepeatMode(Const.FILTER_AUDIO,
						Const.REPEAT_ALL);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_pause))) {
				mControlView.setMediaPlayState();
				menuDialog.initItem(0, mResources
						.getString(R.string.mmp_menu_play));
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_play))) {
				mControlView.setMediaPlayState();
				menuDialog.initItem(0, mResources
						.getString(R.string.mmp_menu_pause));
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_shuffleon))) {
				if (isNotSupport || getPlayerStop()) {
					mControlView.setShuffleVisble(View.VISIBLE);
					menuDialog.initItem(1, mResources
							.getString(R.string.mmp_menu_shuffleoff));
					mLogicManager.setShuffle(Const.FILTER_AUDIO, Const.SHUFFLE_ON);
				}else{
					mControlView.setShuffleVisble(View.VISIBLE);
				menuDialog.initItem(2, mResources
						.getString(R.string.mmp_menu_shuffleoff));
				mLogicManager.setShuffle(Const.FILTER_AUDIO, Const.SHUFFLE_ON);
				}
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_shuffleoff))) {
				if (isNotSupport || getPlayerStop()) {
					mControlView.setShuffleVisble(View.INVISIBLE);
					menuDialog.initItem(1, mResources
							.getString(R.string.mmp_menu_shuffleon));
					mLogicManager.setShuffle(Const.FILTER_AUDIO, Const.SHUFFLE_OFF);
				}else{
					mControlView.setShuffleVisble(View.INVISIBLE);
				menuDialog.initItem(2, mResources
						.getString(R.string.mmp_menu_shuffleon));
				mLogicManager.setShuffle(Const.FILTER_AUDIO, Const.SHUFFLE_OFF);
				}
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_showinfo))) {
				showinfoview(sMediaType);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_lyricoptions))) {
				menuDialog.dismiss();
				MenuListView menuDialog = new MenuListView(
						MediaPlayActivity.this,
						GetDataImp.getInstance().getComMenu(
								MediaPlayActivity.this,
								R.array.mmp_menu_lyricplaylist,
								R.array.mmp_menu_lyricplaylist_enable,
								R.array.mmp_menu_lyricplaylist_hasnext),
						mListener, null);
				menuDialog.show();
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_showscore))) {
				isHideSperum = false;
				menuDialog.initItem(4, mResources
						.getString(R.string.mmp_menu_hidescore));
				removeScore(false);

			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_hidescore))) {
				menuDialog.initItem(4, mResources
						.getString(R.string.mmp_menu_showscore));
				isHideSperum = true;
				removeScore(true);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_singleline))) {
				mPerLine = 1;
				setLrcLine(mPerLine);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_multiline))) {
				mPerLine = 8;
				setLrcLine(mPerLine);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_off))) {
				// Modified by Dan for fix bug DTV00389330&DTV00389362
				if (menuDialog.isInLrcOffsetMenu()) {
					mLogicManager.setLrcOffsetMode(0);
				} else {
					mPerLine = 0;
					hideLrc();
				}
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_onlyaudio))) {
				mLogicManager.setAudioOnly(true);
				MtkLog.i(TAG, "  audio  only  :  "
						+ mLogicManager.isAudioOnly());
				//add by keke 1229 for DTV00386510
				dismissMenuDialog();
				//keke end

			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_auto))) {
				// Modified by Dan for fix bug DTV00389362
				if (menuDialog.isInLrcOffsetMenu()) {
					mLogicManager.setLrcOffsetMode(1);
				}
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_userdefine))) {
				// Added by Dan for fix bug DTV00389362
				if (menuDialog.isInLrcOffsetMenu()) {
					mLogicManager.setLrcOffsetMode(2);
				}
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_big5))) {

			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_utf8))) {

			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_utf16))) {

			}
			return;
		}
		if (sMediaType == MultiMediaConstant.TEXT) {
			if (content.equals(mResources.getString(R.string.mmp_menu_none))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.TEXT,Util.NONE);
				mControlView.setRepeatNone();
				mLogicManager.setRepeatMode(Const.FILTER_TEXT,
						Const.REPEAT_NONE);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_repeatone))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.TEXT,Util.REPEATE_ONE);
				mControlView.setRepeatSingle();
				mLogicManager
						.setRepeatMode(Const.FILTER_TEXT, Const.REPEAT_ONE);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_repeatall))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.TEXT,Util.REPEATE_ALL);
				mControlView.setRepeatAll();
				mLogicManager
						.setRepeatMode(Const.FILTER_TEXT, Const.REPEAT_ALL);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_pause))) {
				mControlView.setMediaPlayState();
				menuDialog.initItem(0, mResources
						.getString(R.string.mmp_menu_play));
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_play))) {
				mControlView.setMediaPlayState();
				menuDialog.initItem(0, mResources
						.getString(R.string.mmp_menu_pause));
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_shuffleon))) {
				menuDialog.initItem(2, mResources
						.getString(R.string.mmp_menu_shuffleoff));
				mControlView.setShuffleVisble(View.VISIBLE);
				// Modified by Dan for fix bug DTV00375629
				mLogicManager.setShuffle(Const.FILTER_TEXT, Const.SHUFFLE_ON);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_shuffleoff))) {
				menuDialog.initItem(2, mResources
						.getString(R.string.mmp_menu_shuffleon));
				mControlView.setShuffleVisble(View.INVISIBLE);
				// Modified by Dan for fix bug DTV00375629
				mLogicManager.setShuffle(Const.FILTER_TEXT, Const.SHUFFLE_OFF);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_showinfo))) {
				showinfoview(sMediaType);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_font))) {
				menuDialog.dismiss();
				menuDialogFontList = new MenuListView(
						MediaPlayActivity.this, GetDataImp.getInstance()
								.getComMenu(MediaPlayActivity.this,
										R.array.mmp_menu_fontlist,
										R.array.mmp_menu_fontlist_enable,
										R.array.mmp_menu_fontlist_hasnext),
						mListener, null);
				menuDialogFontList.show();
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_small))) {
				setFontSize(TextUtils.SMALLSIZE);
				//reflashPageNumber();
				
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_medium))) {
				setFontSize(TextUtils.MEDSIZE);
				//reflashPageNumber();
				
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_large))) {
				setFontSize(TextUtils.LARSIZE);
				//reflashPageNumber();
				
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_regular))) {
				setFontStyle(Typeface.NORMAL);
				
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_italic))) {
				setFontStyle(Typeface.ITALIC);
				
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_bold))) {
				setFontStyle(Typeface.BOLD);
				
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_outline))) {
				setFontStyle(Typeface.BOLD_ITALIC);
				
			} else {
				setFontColor(changeContent(content));
			}
			return;
		}
		if (sMediaType == MultiMediaConstant.VIDEO) {
			if (content.equals(mResources.getString(R.string.mmp_menu_none))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.VIDEO,Util.NONE);
				mControlView.setRepeatNone();
				mLogicManager.setRepeatMode(Const.FILTER_VIDEO,
						Const.REPEAT_NONE);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_repeatone))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.VIDEO,Util.REPEATE_ONE);
				mControlView.setRepeatSingle();
				mLogicManager.setRepeatMode(Const.FILTER_VIDEO,
						Const.REPEAT_ONE);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_repeatall))) {
				Util.setMediaRepeatMode(getApplicationContext(), MultiMediaConstant.VIDEO,Util.REPEATE_ALL);
				mControlView.setRepeatAll();
				mLogicManager.setRepeatMode(Const.FILTER_VIDEO,
						Const.REPEAT_ALL);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_pause))) {
				
					mControlView.setMediaPlayState();
				if(!mLogicManager.isPlaying()){
					menuDialog.initItem(0,
							mResources.getString(R.string.mmp_menu_play));
				}
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_play))) {
				
					mControlView.setMediaPlayState();
				if(mLogicManager.isPlaying()){
					menuDialog.initItem(0,
							mResources.getString(R.string.mmp_menu_pause));
				}
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_showinfo))) {
				showinfoview(sMediaType);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_user))) {
				mLogicManager.setPictureMode(ModelConstant.PICTURE_MODEL_USER);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_cinema))) {
				mLogicManager
						.setPictureMode(ModelConstant.PICTURE_MODEL_CINEMA);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_sport))) {
				mLogicManager.setPictureMode(ModelConstant.PICTURE_MODEL_SPORT);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_vivid))) {
				mLogicManager.setPictureMode(ModelConstant.PICTURE_MODEL_VIVID);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_hibright))) {
				mLogicManager
						.setPictureMode(ModelConstant.PICTURE_MODEL_HIBRIGHT);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_auto))) {
				setScreenMode(CommonSet.VID_SCREEN_MODE_AUTO);
				//mLogicManager.setScreenMode(CommonSet.VID_SCREEN_MODE_AUTO);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_normal))) {
				setScreenMode(CommonSet.VID_SCREEN_MODE_NORMAL);				
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_letterbox))) {
				setScreenMode(CommonSet.VID_SCREEN_MODE_LETTER_BOX);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_scan))) {
				setScreenMode(CommonSet.VID_SCREEN_MODE_PAN_SCAN);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_zoom))) {
				setScreenMode(CommonSet.VID_SCREEN_MODE_NON_LINEAR_ZOOM);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_dotbydot))) {
				setScreenMode(CommonSet.VID_SCREEN_MODE_DOT_BY_DOT);
			} else if (content.equals(mResources
					.getString(R.string.mmp_menu_pic_setting))) {
				dismissMenuDialog();
				if (mControlView != null && isControlBarShow) {
					hideController();
				}
				isSetPicture = true;
				Intent intent = new Intent(this, MenuMain.class);
				intent.putExtra("fromwhere", 1);
				startActivity(intent);
			}
			return;
		}
	}
    
	//add by keke for fix DTV00380564
	protected void setScreenMode(int screenmode){
		mLogicManager.setScreenMode(screenmode);
		TVContent.getInstance(this).getStorage().set("SCREENMODE_FILELIST",
				screenmode+"");
	}
	/**
	 * Refresh control bar page number
	 */
	protected void reflashPageNumber() {

	}

	/**
	 * Change the content to match common logic
	 * 
	 * @param content
	 * @return
	 */
	private int changeContent(String content) {
		int color = Color.WHITE;
		if (mResources.getString(R.string.mmp_menu_red).equals(content)) {
			color = Color.RED;
			
		} else if (mResources.getString(R.string.mmp_menu_green)
				.equals(content)) {
			color = Color.GREEN;
			
		} else if (mResources.getString(R.string.mmp_menu_black)
				.equals(content)) {
			color = Color.BLACK;
			
		} else if (mResources.getString(R.string.mmp_menu_white)
				.equals(content)) {
			color = Color.WHITE;
			
		} else if (mResources.getString(R.string.mmp_menu_blue).equals(content)) {
			color = Color.BLUE;
			
		}

		return color;

	}

	/**
	 * Show info view
	 * 
	 * @param type
	 */
	protected void showinfoview(int type) {
		int resid;
		switch (type) {
		case MultiMediaConstant.AUDIO: {
			resid = R.layout.mmp_musicinfo;
			break;
		}
		case MultiMediaConstant.PHOTO:
		case MultiMediaConstant.THRD_PHOTO: {
			resid = R.layout.mmp_photoinfo;
			break;
		}
		case MultiMediaConstant.VIDEO: {
			resid = R.layout.mmp_videoinfo;
			break;
		}
		case MultiMediaConstant.TEXT: {
			resid = R.layout.mmp_textinfo;
			break;
		}
		default:
			return;
		}
		View contentView = LayoutInflater.from(MediaPlayActivity.this).inflate(
				resid, null);
		mInfo = new ShowInfoView(this, contentView, type, mLogicManager);
		dismissMenuDialog();
		mInfo.show();
	}

	private MenuListView.MenuDismissCallBack mCallBack = new MenuListView.MenuDismissCallBack() {

		public void onDismiss() {
			hideController();
		}

		public void sendMessage() {
		}

		public void noDismissPannel() {

		};
	};

	/**
	 * Send a delay message to hidden control bar
	 */
	protected void hideControllerDelay() {

	}

	/**
	 * Recount hidden control bar delay time
	 */
	protected void reSetController() {
		showController();
		hideControllerDelay();
	}

	/**
	 * Hidden Control bar
	 */
	protected void hideController() {
		
		if (null != menuDialog && menuDialog.isShowing()) {
			return;
		}
		if (mControlView != null && isControlBarShow) {
//add by shuming for fix CR: DTV00407914
			mControlView.hiddlen(View.INVISIBLE);
//			mControlView.update(mDisPlayWidth / 10, mDisPlayHeight*20, -1, -1);
			mHandler.sendEmptyMessageDelayed(MSG_HIDE_INFORBAR_POINT,70);	
		}
		
		isControlBarShow = false;
		
		removeProgressMessage();
		
	}
	/**
	 * Remove to get progress inforamtion and time information message.
	 */
	protected void removeProgressMessage(){
		
	}
	/**
	 * Add to get progress inforamtion and time information message
	 */
	protected void addProgressMessage(){
		
	}
	/**
	 * Show control bar
	 */
	protected void showController() {
		if (mControlView != null && !isControlBarShow) {
//			mControlView.update(mDisPlayWidth / 10, mDisPlayHeight / 10,
//					mDisPlayWidth * 4 / 5, mDisPlayHeight * 4 / 5);
//add by shuming for fix CR: DTV00407914			
			mControlView.hiddlen(View.VISIBLE);
			isControlBarShow = true;
		}
			addProgressMessage();
		
		
	}

	/**
	 * Get lines number per screen
	 * 
	 * @return int lines number
	 */
	
	//change by xudong.chen 20111204 fix DTV00379662
	public static int getPerLine() {
		return mPerLine;
	}
	//end

	/**
	 * Get media type
	 * 
	 * @return int type:photo audio video text
	 */
	public static int getMediaType() {
		return sMediaType;
	}

	/**
	 * blue screen status. true to blue screen.
	 */
	/**
	 * Set bule dialog status.
	 * @param status
	 */
	/**
	 * Get current blue dialog status.
	 * @return
	 */
	// add by keke for fix DTV00383992
	protected void hideFeatureNotWork() {
			mHandler.sendEmptyMessage(MSG_DISMISS_FEARTURE_NOT_SUPPORT);

	}
	protected boolean getPlayerStop() {
		return false;
	}


	protected void setFontSize(float size){
		
	}

	 protected void setFontStyle(int style) {
		
	}

	protected void setFontColor(int color) {
		
	}



	public void setRepeatMode(){
		MtkLog.i(TAG, "setRepeatMode");
		
		if(	mControlView==null||mLogicManager==null){
			MtkLog.i(TAG, "mControlView==null||mLogicManager==null");
			return;
		}
		int type_util 	= Util.AUDIO;
		int type_const	= Const.FILTER_AUDIO;
		MtkLog.i(TAG, "setRepeatMode sMediaType:"+sMediaType);
		switch(sMediaType){
			case MultiMediaConstant.AUDIO:
				type_util = Util.AUDIO;
				type_const = Const.FILTER_AUDIO;
				break;
			case MultiMediaConstant.VIDEO:
				type_util = Util.VIDEO;
				type_const = Const.FILTER_VIDEO;
				break;
			case MultiMediaConstant.PHOTO:
				type_util = Util.PHOTO;
				type_const = Const.FILTER_IMAGE;
				break;
			case MultiMediaConstant.TEXT:
				type_util = Util.TXT;
				type_const = Const.FILTER_TEXT;
				break;
		}
		
//		if (sMediaType == MultiMediaConstant.AUDIO) {
			int value = Util.getMediaRepeatMode(getApplicationContext(), type_util);
			switch(value){
			case Util.NONE:
				MtkLog.i(TAG, "setRepeatMode NONE");
				mControlView.setRepeatNone();
				mLogicManager.setRepeatMode(type_const,
						Const.REPEAT_NONE);
				break;
			case  Util.REPEATE_ONE:
				MtkLog.i(TAG, "setRepeatMode REPEATE_ONE");
				mControlView.setRepeatSingle();
				mLogicManager.setRepeatMode(type_const,
						Const.REPEAT_ONE);
				break;
			case Util.REPEATE_ALL:
				MtkLog.i(TAG, "setRepeatMode REPEATE_ALL");
				mControlView.setRepeatAll();
				mLogicManager.setRepeatMode(type_const,
						Const.REPEAT_ALL);
				break;
			}
//		}
	}

	
}
