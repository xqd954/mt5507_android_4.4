package com.mediatek.ui.mmp;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.app.StatusBarManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.ViewGroup;
import com.mediatek.mmpcm.audioimpl.AudioConst;

import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.videoimpl.Thumbnail;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.tv.service.InputService;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.mmp.carousel.controls.Carousel;
import com.mediatek.ui.mmp.carousel.controls.CarouselAdapter;
import com.mediatek.ui.mmp.carousel.controls.CarouselAdapter.OnItemClickListener;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MtkFilesBaseListActivity;
import com.mediatek.ui.mmp.multimedia.MtkFilesGridActivity;
import com.mediatek.ui.mmp.multimedia.MtkFilesListActivity;
import com.mediatek.ui.mmp.util.AsyncLoader;
import com.mediatek.ui.mmp.util.LogicManager;
import com.mediatek.ui.mmp.util.MultiMediaConstant;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.nav.util.CaptureLogoImp;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.DestroyApp;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

/**
 * MMP main activity
 * 
 * @author hs_weihuiluo
 * 
 */
public class MeidaMainActivity extends Activity {

	/**
	 * Log tag
	 */
	private static final String TAG = "MeidaMainActivity";

	public static TVChannel mTvChannel;

	public static int mSelection;

	private ShutDownBroadcastReceiver mReceiver;

	private StatusBarManager mStatusBarManager;

	private Carousel carousel ;
	private boolean mmp_flag = false;
	/**
	 * {@inheritDoc}
	 */
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		DestroyApp des = (DestroyApp) this.getApplication();
		des.add(this);
		mStatusBarManager = (StatusBarManager) getSystemService(Context.STATUS_BAR_SERVICE);
		setContentView(R.layout.mmp_multimeida);
		setFocus();
		setClickListener();

		// Enter to MMP stop TV
		// TVContent.getInstance(MeidaMainActivity.this).pushStop();
		TVContent.getInstance(MeidaMainActivity.this).getInputManager()
				.stopDesignateOutput(InputService.INPUT_OUTPUT_MAIN, false);
		LogicManager.getInstance(this).freeVideoResource();
		if (NavSundryImplement.getInsNavSundryImplement(this).isFreeze()) {
			CaptureLogoImp.getInstance(this).freezeScreen(false);
		}

		mReceiver = new ShutDownBroadcastReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_SHUTDOWN);
		registerReceiver(mReceiver, filter);

		Util.setMMPFlag(true);
		mmp_flag = true;
		
		MtkLog.d(TAG, "MediaMain onCreate~~~ ");
		if(AnimationManager.getInstance().getIsAnimation()){
			carousel = (Carousel)findViewById(R.id.carousel);
			carousel.setVisibility(View.VISIBLE);
			AnimationManager.getInstance().startActivityEnterAnimation(this,findViewById(R.id.mmp_main_layout),null);
			carousel.setOnItemClickListener(new OnItemClickListener(){

				public void onItemClick(CarouselAdapter<?> parent, View view,
						int position, long id) {				
					mSelection = carousel.getSelectedItemPosition();
					showFilesGrid(mSelection);
				}
	        });
		}else{
			findViewById(R.id.mmp_btn_layout).setVisibility(View.VISIBLE);
		}
	}
	
	private int mCenterX = 160;	
	private int mCenterY = 240;	
	
	private OnKeyListener mOnKeyListener = new OnKeyListener() {

		public boolean onKey(View v, int keyCode, KeyEvent event) {

			if (event.getAction() == KeyEvent.ACTION_DOWN) {
				if ((keyCode == KeyEvent.KEYCODE_DPAD_CENTER)
						|| (keyCode == KeyEvent.KEYCODE_MTKIR_ENTER)) {
					mSelection = (Integer) v.getTag();
					showFilesGrid(mSelection);
				} else if (keyCode == KeyEvent.KEYCODE_MTKIR_BLUE) {
					mSelection = (Integer) v.getTag();
					showFilesList(mSelection);
				}
			}

			return false;
		}
	};

	private void setClickListener() {
		if(!AnimationManager.getInstance().getIsAnimation()){
			
		ViewGroup parent = (ViewGroup) findViewById(R.id.mmp_btn_layout);

		for (int i = 0, length = parent.getChildCount(); i < length; i++) {
			View view = ((ViewGroup) parent.getChildAt(i)).getChildAt(0);
			view.setTag(i);
			view.setOnKeyListener(mOnKeyListener);
		}
		}

	}

	private void setFocus() {
		View photo = findViewById(R.id.mmp_btn_photo);
		photo.setNextFocusRightId(R.id.mmp_btn_audio);
		photo.setNextFocusLeftId(R.id.mmp_btn_text);

		View audio = findViewById(R.id.mmp_btn_audio);
		audio.setNextFocusRightId(R.id.mmp_btn_video);
		audio.setNextFocusLeftId(R.id.mmp_btn_photo);

		View video = findViewById(R.id.mmp_btn_video);
		video.setNextFocusRightId(R.id.mmp_btn_text);
		video.setNextFocusLeftId(R.id.mmp_btn_audio);
	}

	private void setFocus(View view) {
		if(null == view){
			return;
		}
		switch (view.getId()) {
		case R.id.mmp_btn_photo:
			View photo = findViewById(R.id.mmp_btn_photo);
			photo.setNextFocusDownId(R.id.mmp_btn_audio);
		
			photo.setNextFocusUpId(R.id.mmp_btn_text);
			break;
		case R.id.mmp_btn_audio:
			View audio = findViewById(R.id.mmp_btn_audio);
			audio.setNextFocusDownId(R.id.mmp_btn_video);

			audio.setNextFocusUpId(R.id.mmp_btn_photo);
			break;
		case R.id.mmp_btn_video:
			View video = findViewById(R.id.mmp_btn_video);
			video.setNextFocusDownId(R.id.mmp_btn_text);

			video.setNextFocusUpId(R.id.mmp_btn_audio);
			break;
		case R.id.mmp_btn_text:
			View text = findViewById(R.id.mmp_btn_text);
			text.setNextFocusDownId(R.id.mmp_btn_photo);
			
			text.setNextFocusUpId(R.id.mmp_btn_video);
			break;
		default:
			break;
		}
	}
	/**
	 * {@inheritDoc}
	 */
	protected void onResume() {
	
		super.onResume();
		
		MtkLog.d(TAG, "MediaMain onResume ");
		mStatusBarManager.setSystemUiBarHight(0);
		if(!AnimationManager.getInstance().getIsAnimation()){
		ViewGroup parent = (ViewGroup) findViewById(R.id.mmp_btn_layout);
		ViewGroup layout = (ViewGroup) parent.getChildAt(mSelection);
		if (null != layout) {
			View view = layout.getChildAt(0);
			view.setFocusableInTouchMode(true);
			view.requestFocus();
		}
		}else{
			carousel.setSelection(mSelection);
		}
	}
	
	@Override
	protected void onRestart() {
		super.onRestart();
		MtkLog.d(TAG, "MediaMain onRestart ");
		if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().startActivityReEnterAnimation(this, findViewById(R.id.mmp_main_layout), null);
		}
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		MtkLog.d(TAG, "MediaMain onPause ");
	}

	/**
	 * {@inheritDoc}
	 */
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		
		LogicManager mLogicManager = LogicManager.getInstance(this);
		switch (keyCode) {
		case KeyMap.KEYCODE_DPAD_CENTER:
			if(AnimationManager.getInstance().getIsAnimation()){
			mSelection = carousel.getSelectedItemPosition();
			showFilesGrid(mSelection);
			}
			break;
		case KeyMap.KEYCODE_MTKIR_BLUE:
			if(AnimationManager.getInstance().getIsAnimation()){
			mSelection = carousel.getSelectedItemPosition();
			showFilesList(mSelection);
			}
			break;

		case KeyMap.KEYCODE_MTKIR_ANGLE: 
		case KeyMap.KEYCODE_BACK:{
			mLogicManager.finishAudioService();
			mLogicManager.finishVideo();
			DevManager.getInstance().destroy();
			MultiFilesManager.getInstance(this).destroy();
			com.mediatek.ui.mmp.util.BitmapCache.createCache(true);
			com.mediatek.ui.mmp.gamekit.util.BitmapCache.createCache(true);
			exitMediaMain();
			if(AnimationManager.getInstance().getIsAnimation()){
				AnimationManager.getInstance().startActivityEndAnimation(this , findViewById(R.id.mmp_main_layout) , null);
			}else{
			if (isValid()) {
//					Util.exitMmpActivity(this);
					finish();
			}
			}
			// LogicManager.getInstance(this).finishAudioService();
			// LogicManager.getInstance(this).finishVideo();
			// DevManager.getInstance().removeDevListeners();
			// DestroyApp destroyApp = (DestroyApp) getApplicationContext();
			// destroyApp.finishAll();
			break;
		}
		case KeyMap.KEYCODE_DPAD_UP: {
			// View view = findViewById(R.id.mmp_btn_photo);
			// view.setFocusableInTouchMode(true);
			// view.requestFocus();
			setFocus(getCurrentFocus());
			break;
		}
		case KeyMap.KEYCODE_DPAD_DOWN: {
			// View view = findViewById(R.id.mmp_btn_text);
			// view.setFocusableInTouchMode(true);
			// view.requestFocus();
			setFocus(getCurrentFocus());
			break;
		}
		// Mute
		case KeyMap.KEYCODE_MTKIR_MUTE: {
			mLogicManager.setMute();
			return true;
		}
		// Repeat
		case KeyMap.KEYCODE_MTKIR_REPEAT: {
			onRepeat();
			return true;
		}
		// Play next audio
		case KeyMap.KEYCODE_MTKIR_NEXT: {
			mLogicManager.playNextAudio();
			return true;
		}
		// Play previous audio
		case KeyMap.KEYCODE_MTKIR_PREVIOUS: {
			mLogicManager.playPrevAudio();
			return true;
		}

		case KeyMap.KEYCODE_MTKIR_REWIND: {
			
			if (isValid() && mLogicManager.getAudioStatus() < AudioConst.PLAY_STATUS_STOPPED) {
				if (mLogicManager.getAudioStatus() <  AudioConst.PLAY_STATUS_STARTED) {
				
					return true;
				}
				try {
					mLogicManager.fastRewindAudio();
					
				} catch (IllegalStateException e) {
					//featureNotWork(getString(R.string.mmp_featue_notsupport));
				}
			}
			
			return true;
		}
		case KeyMap.KEYCODE_MTKIR_FASTFORWARD: {
			
			if (isValid() && mLogicManager.getAudioStatus() < AudioConst.PLAY_STATUS_STOPPED) {
				if (mLogicManager.getAudioStatus() <  AudioConst.PLAY_STATUS_STARTED) {
					
					return true;
				}
				try {
					mLogicManager.fastForwardAudio();
					
				} catch (IllegalStateException e) {
					

				}
			}
			return true;
		}



		
		// Pause audio
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE: {
			onPauseOrPlay();
			return true;
		}
		// Stop audio
		case KeyMap.KEYCODE_MTKIR_STOP: {
			mLogicManager.finishAudioService();
			return true;
		}
		case KeyMap.KEYCODE_VOLUME_UP:
				
				if (mLogicManager.isMute()) {
					mLogicManager.setMute();
				}else{
					int max=mLogicManager.getMaxVolume();
					int curVol=mLogicManager.getVolume();
					curVol+=1;
					if(curVol<max){
						mLogicManager.setVolume(curVol);
					}else{
						mLogicManager.setVolume(max);
					}
				}
			return true;
		case KeyMap.KEYCODE_VOLUME_DOWN:
			
					if (mLogicManager.isMute()) {
						mLogicManager.setMute();
					}else{
						int curVol=mLogicManager.getVolume();
						curVol-=1;
						if(curVol>0){
							mLogicManager.setVolume(curVol);
						}else{
							mLogicManager.setVolume(0);
						}
					}
			return true;
		case KeyMap.KEYCODE_MTKIR_PIPPOP:
		case KeyMap.KEYCODE_MTKIR_PIPPOS:
		case KeyMap.KEYCODE_MTKIR_PIPSIZE:
		case KeyMap.KEYCODE_MTKIR_CHDN:
		case KeyMap.KEYCODE_MTKIR_CHUP:
			return false;
		case KeyMap.KEYCODE_MTKIR_GUIDE:
			Util.startEPGActivity(this);
			break;
		default:
			break;
		}

		return super.onKeyDown(keyCode, event);
	}

	/**
	 * Pause or play the current audio
	 */
	private void onPauseOrPlay() {
		LogicManager manage = LogicManager.getInstance(this);
		if (manage.getAudioPlaybackService() == null) {
			return;
		}

		if (manage.isAudioPause() || manage.isAudioFast()
				|| manage.isAudioStoped()) {
			manage.playAudio();
		} else if (manage.isAudioPlaying()) {
			manage.pauseAudio();
		}
	}

	/**
	 * Switch repeat mode
	 */
	private void onRepeat() {
		int repeatModel = LogicManager.getInstance(this).getRepeatModel(
				Const.FILTER_AUDIO);
		
		repeatModel = (repeatModel+1) % 3;
		
		LogicManager.getInstance(this).setRepeatMode(Const.FILTER_AUDIO,
				repeatModel);
	}

	/**
	 * Switch to grid mode
	 * 
	 * @param contentType
	 *            photo audio video or text
	 */
	private void showFilesGrid(int contentType) {
		SaveValue pref = SaveValue.getInstance(this);
		boolean smbAvailable = pref.readValue(MenuConfigManager.MY_NET_PLACE) == 0 ? false
				: true;
		MtkLog.d(TAG, "Samba Available : " + smbAvailable);
		boolean dlnaAvailable = pref.readValue(MenuConfigManager.DLNA) == 0 ? false
				: true;
		MtkLog.d(TAG, "Dlna Available : " + dlnaAvailable);
		MultiFilesManager.getInstance(this, smbAvailable, dlnaAvailable)
				.getLocalDevices();
		int deviceNum = MultiFilesManager.getInstance(this).getAllDevicesNum();
		if (deviceNum == MultiFilesManager.NO_DEVICES && !dlnaAvailable
				&& !smbAvailable) {
			return;
		}
        MultiFilesManager.getInstance(this).pushOpenedHistory(contentType);
		Intent intent = new Intent(this, MtkFilesGridActivity.class);
		intent.putExtra(MultiMediaConstant.MEDIAKEY, contentType);
		startActivity(intent);
	}

	/**
	 * Switch to list mode
	 */
	private void showFilesList(int content) {
		Intent intent = new Intent();
		intent.setClass(this, MtkFilesListActivity.class);
		intent.putExtra(MtkFilesListActivity.INTENT_NAME_SELECTION, content);
		startActivity(intent);
	}


	private void exitMediaMain(){

	
		MtkLog.d(TAG, "MediaMain exitMediaMain ");
		((DestroyApp) getApplication()).remove(this);
		DestroyApp destroyApp = (DestroyApp) getApplicationContext();
		 destroyApp.finishAll();
	
		 mSelection = 0;
		AsyncLoader.getInstance(0).clearQueue();
		Thumbnail thumbnail = Thumbnail.getInstance();
		if (thumbnail.isLoadThumanil()) {
			thumbnail.stopThumbnail();
		}
		LogicManager.getInstance(this).restoreVideoResource();
		
		// TVContent.getInstance(this).popStop();
		// DevManager.getInstance().removeDevListener(mDevListener);
		

		MtkFilesBaseListActivity.reSetModel();
		
		Util.setMMPFlag(false);
		mmp_flag = false;

	}

	/**
	 * {@inheritDoc}
	 */
	protected void onDestroy() {
		super.onDestroy();
		MtkLog.d(TAG, "MediaMain onDestroy ");
		if(mmp_flag){

			exitMediaMain();


		}
		LogicManager.getInstance(this).sendCloseBroadCast();
		unregisterReceiver(mReceiver);
		

	}

//	/**
//	 * An adapter that can be used in menu list.
//	 */
//	private class MenuAdapter extends EfficientAdapter<MenuItem> {
//		public MenuAdapter(Context context, List<MenuItem> dataList) {
//			super(context, dataList);
//		}
//
//		/**
//		 * Get ListView item layout resources
//		 * 
//		 * @return ListView item layout resources
//		 */
//		protected int getItemLayout() {
//			return R.layout.mmp_multimedia_griditem;
//		}
//
//		/**
//		 * 
//		 * create a viewHolder
//		 */
//		protected void initView(View v) {
//			ViewHolder holder = new ViewHolder();
//
//			holder.img = (ImageView) v.findViewById(R.id.multimedia_gv_img);
//			holder.tv = (TextView) v.findViewById(R.id.multimedia_gv_tv);
//
//			v.setTag(holder);
//		}
//
//		/**
//		 * set view value
//		 */
//		protected void bindView(View v, MenuItem data, int position) {
//			ViewHolder holder = (ViewHolder) v.getTag();
//
//			holder.tv.setText(data.getName());
//			holder.img.setImageResource(data.getImageResourceId());
//		}
//
//		private class ViewHolder {
//			ImageView img;
//			TextView tv;
//		}
//	}

	/**
	 * 
	 * Menu items bean
	 * 
	 * @author hs_weihuiluo
	 * 
	 */
	private class MenuItem {
		private int imageResourceId;
		private String name;

		public void setImageResourceId(int imageResourceId) {
			this.imageResourceId = imageResourceId;
		}

		public int getImageResourceId() {
			return imageResourceId;
		}

		public void setName(String name) {
			this.name = name;
		}

		public String getName() {
			return name;
		}
	}

	/**
	 * Last key down milliseconds
	 */
	private static long mLastKeyDownTime;

	/**
	 * Is the current key down valid
	 * 
	 * @return true:valid,false:invalid
	 */
	public static boolean isValid() {
		long currentTime = System.currentTimeMillis();
		if ((currentTime - mLastKeyDownTime) >= 1000) {
			mLastKeyDownTime = currentTime;
			return true;
		} else {
			MtkLog.i(TAG, " key down duration :"
					+ (currentTime - mLastKeyDownTime) + "< 1000 mmm");
			mLastKeyDownTime = currentTime;
			return false;
		}
	}

	/**
	 * Is the current key down valid by specified millsenconds
	 * 
	 * @param millSeconds
	 * @return
	 */
	public static boolean isValid(int millSeconds) {
		long currentTime = System.currentTimeMillis();
		if ((currentTime - mLastKeyDownTime) >= millSeconds) {
			mLastKeyDownTime = currentTime;
			return true;
		} else {
			MtkLog.i(TAG, " key down duration :"
					+ (currentTime - mLastKeyDownTime) + "< millSeconds mmm");
			mLastKeyDownTime = currentTime;
			return false;
		}
	}

	private class ShutDownBroadcastReceiver extends BroadcastReceiver {

		public void onReceive(Context context, Intent intent) {
			LogicManager.getInstance(MeidaMainActivity.this)
					.restoreVideoResource();
		}
	}
}
