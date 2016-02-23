package com.mediatek.ui.nav.commonview;

import java.text.SimpleDateFormat;
import java.util.Date;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.View;
import android.widget.TextView;

import com.mediatek.tv.common.ConfigType;
import com.mediatek.tv.service.BroadcastService;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVOutputCommon.AudioUpdated;
import com.mediatek.ui.R;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.util.NavIntegrationZoom;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;

/**
 * 
 * @author MTK40530
 * 
 */

public class NavSundryShowTextView extends TextView {
	private static final String TAG = "NavSundryShowTextView";
	
	public static boolean isFreeze;

	private Context mContext;
	private NavSundryImplement nsi;
	private NavIntegration mni;
//	private String[] pictureModeName;
//	private String[] audioModeName;
//	private String[] screenModeName;
//	private String[] mtsModeName;
//	private String[] freezeModeName;
	private Handler mHandler;
	private BannerView mBannerView;
	private boolean changeMtsAudio = false;

	public static boolean timeFlag = false;

	private AudioUpdated auListener = null;

	private boolean isFromThirdApp = false;
	private PippopUiLogic mPippopUiLogic;

	public static final int MESSAGE_AUDIO_UPDATE = 1000;
	private static BroadcastReceiver mBroadcastReceiver;
	private Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MESSAGE_AUDIO_UPDATE:
				if (nsi.isCurrentChannelAnalog()) {
					int ind = nsi.getCurrentMtsEffect();
					String[] mtsModeName = mContext.getResources().getStringArray(
							R.array.nav_mts_strings);
					setText(mtsModeName[ind]);
				} else {
					String string = nsi.getCurrentLanguageEffect();
					setText(nsi.getLanguageShowString(string));
				}
				mBannerView.updateAudioLanguage();
				break;

			default:
				break;
			}

		}
	};

	public void setThirdApp(boolean isThirdApp) {
		isFromThirdApp = isThirdApp;
	}

	public NavSundryShowTextView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init(context);
	}

	public NavSundryShowTextView(Context context) {
		super(context);
		init(context);
	}

	public NavSundryShowTextView(Context context, AttributeSet attrs,
			int defStyle) {
		super(context, attrs, defStyle);
		init(context);
	}

	public Handler getHandler() {
		return mHandler;
	}

	public void setHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}
	
	public void setBannerView(BannerView mBannerView) {
		this.mBannerView = mBannerView;
	}

	private void init(Context context) {
		mContext = context;
		nsi = NavSundryImplement.getInsNavSundryImplement(mContext);
		mni = NavIntegration.getInstance(mContext);
		mPippopUiLogic = PippopUiLogic.getInstance(mContext);
		
//		pictureModeName = mContext.getResources().getStringArray(
//				R.array.picture_effect_array);
//		audioModeName = mContext.getResources().getStringArray(
//				R.array.menu_audio_equalizer_array);
//		screenModeName = mContext.getResources().getStringArray(
//				R.array.screen_mode_array);
//		mtsModeName = mContext.getResources().getStringArray(
//				R.array.nav_mts_strings);
//
//		freezeModeName = mContext.getResources().getStringArray(
//				R.array.nav_freeze_strings);
		IntentFilter filter = new IntentFilter();
		filter.addAction(BroadcastService.ACTION_SVCTX_NFY);
		mBroadcastReceiver = new BroadcastReceiver() {

			public void onReceive(Context context, Intent intent) {
				// TODO Auto-generated method stub
				String nfyCode = intent
						.getStringExtra(BroadcastService.SVCTX_NFY_CODE);

				if (nfyCode
						.equals(BroadcastService.SVCTX_NFY_STREAM_STARTED)) {
					 changeMtsAudio = true;
				}
			}
		};
		mContext.registerReceiver(mBroadcastReceiver, filter);

		auListener = new AudioUpdated() {
			public void onUpdate() {
				// TODO Auto-generated method stub
				handler.removeMessages(MESSAGE_AUDIO_UPDATE);
				handler.sendEmptyMessage(MESSAGE_AUDIO_UPDATE);
			}
		};
	}

	/**
	 * 
	 * @param flag
	 *            if true, get and set next information, or just only get
	 *            current information.
	 * @param keyCode
	 */
	public void setAndShowTextContent(boolean flag, int keyCode) {
		int indext = 0;
		String ts = null;

		timeFlag = false;

		mHandler.removeMessages(MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS);
		mHandler.sendEmptyMessageDelayed(
				MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS,
				MessageType.delayMillis4);

		switch (keyCode) {
		case KeyMap.KEYCODE_MTKIR_SLEEP:
			if (flag) {
				indext = nsi.toSleepInMinutes();
			} else {
				indext = nsi.getSleepTimeLeftInMunites();
			}

			if (indext == 0) {
				setText(mContext.getString(R.string.nav_sleep_label)
						+ mContext.getString(R.string.nav_sleep_off));
			} else {
				setText(mContext.getString(R.string.nav_sleep_label) + indext
						+ mContext.getString(R.string.nav_sleep_minutes));
			}

			setVisibility(View.VISIBLE);
			break;

		case KeyMap.KEYCODE_MTKIR_PEFFECT:
			if (flag) {
				nsi.setNextConfigEffect(ConfigType.CFG_PICTURE_MODE);
			}
			indext = nsi.getCurrentConfigEffect(ConfigType.CFG_PICTURE_MODE);
			MtkLog.i(TAG, flag + "~~~~~~~~~~~~~~~~current value:" + indext);
			String[] pictureModeName = mContext.getResources().getStringArray(
					R.array.picture_effect_array);
			setText(pictureModeName[indext]);

			setVisibility(View.VISIBLE);
			break;

		case KeyMap.KEYCODE_MTKIR_SEFFECT:
			if (flag) {
				nsi.setNextConfigEffect(ConfigType.CFG_EQUALIZE);
			}
			indext = nsi.getCurrentConfigEffect(ConfigType.CFG_EQUALIZE);
			MtkLog.i(TAG, flag + "~~~~~~~~~~~~~~~~current value:" + indext);
			String[] audioModeName = mContext.getResources().getStringArray(
					R.array.menu_audio_equalizer_array);
			setText(audioModeName[indext]);

			setVisibility(View.VISIBLE);
			break;

		case KeyMap.KEYCODE_MTKIR_ASPECT:
			boolean tmpflag = false;
			TVChannel mCurrentChannel = mni.iGetCurrentChannel();
			if (mni.isCurrentSourceBlocked()) {
				tmpflag = true;
			} else if (mCurrentChannel != null && mCurrentChannel.isBlocked()) {
				tmpflag = true;
			} else if (!mni.iCurrentInputSourceHasSignal()) {
				tmpflag = true;
			} else if (mni.isAudioScrambled()) {
				tmpflag = true;
			} else if ((mni.isCurrentSourceTv() && (!mni
					.iCurrentChannelHasSignal()))) {
				tmpflag = true;
			} else if (mni.isCurrentSourceTv() && !mni.iHasChannel()) {
				tmpflag = true;
			} else if (mni.isCurrentChannelRadio()){
				tmpflag = true;
//			} else if (isFromThirdApp) {
//				tmpflag = true;
			}
			
			if (tmpflag){	
				setText(R.string.nav_no_function);
				setVisibility(View.VISIBLE);
				break;
			}

			nsi.setFreeze(false);
			
			if (flag) {
				nsi.setNextAspectEffect();
			}
			if(!mPippopUiLogic.isPipOrPopState()){
			NavIntegrationZoom.getInstance(mContext).setZoomMode(
					NavIntegrationZoom.ZOOM_1);
			}
			
			indext = nsi.getCurrentConfigEffect(ConfigType.CFG_SCREEN_MODE);
			MtkLog.i(TAG, flag + "~~~~~~~~~~~~~~~~current value:" + indext);
			String[] screenModeName = mContext.getResources().getStringArray(
					R.array.screen_mode_array);
			setText(screenModeName[indext]);

			setVisibility(View.VISIBLE);
			break;

		case KeyMap.KEYCODE_MTKIR_MTSAUDIO:
			if (isAbnormal()) {
				setText(R.string.nav_no_function);
			} else if (nsi.isCurrentChannelAnalog()) {
				if (flag) {
					nsi.registerAudioUpdateListener(auListener);
					nsi.setNextMtsEffect();
					handler.removeMessages(MESSAGE_AUDIO_UPDATE);
					handler.sendEmptyMessageDelayed(MESSAGE_AUDIO_UPDATE, 500);
				} else {
					indext = nsi.getCurrentMtsEffect();
					MtkLog.i(TAG, flag + "~~~~~~~~~~current value:" + indext);
					String[] mtsModeName = mContext.getResources().getStringArray(
							R.array.nav_mts_strings);
					setText(mtsModeName[indext]);
				}
			} else {
				if (flag) {
					nsi.registerAudioUpdateListener(auListener);
					if(changeMtsAudio){
						nsi.setLanguageNextEffect();
						handler.sendEmptyMessageDelayed(MESSAGE_AUDIO_UPDATE, 500);
						changeMtsAudio = false;
					}	
				} else {
					ts = nsi.getCurrentLanguageEffect();
					MtkLog.i(TAG, flag + "~~~~~~~~~current string:" + ts);
					setText(nsi.getLanguageShowString(ts));
					mBannerView.updateAudioLanguage();
				}
			}

			setVisibility(View.VISIBLE);
			break;

		case KeyMap.KEYCODE_MTKIR_TIMER:
			if (flag) {
				MtkLog.i(TAG, "~~~~~~~~~~~~~~~set view gone");
				mHandler
						.sendEmptyMessage(MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS);
			} else {
				MtkLog.i(TAG, "~~~~~~~~~~~~~~~show time");
				timeFlag = true;
				setText(new SimpleDateFormat("HH:mm:ss").format(TVTimerManager
						.getInstance(mContext).getUTC()));
				MtkLog.d(TAG, "show time:"+getText());
				mHandler.removeCallbacks(updateTimer);
				mHandler.postDelayed(updateTimer, MessageType.delayMillis6);

				setVisibility(View.VISIBLE);
			}

			break;

		case KeyMap.KEYCODE_MTKIR_FREEZE:
			boolean fflag = false;
			
			TVChannel mCurChannel = mni.iGetCurrentChannel();
			
			if (mni.isCurrentSourceBlocked()) {
				fflag = true;
			} else if (mCurChannel != null && mCurChannel.isBlocked()) {
				fflag = true;
			} else if (!mni.iCurrentInputSourceHasSignal()) {
				fflag = true;
			} else if (mni.isAudioScrambled()) {
				fflag = true;
			} else if ((mni.isCurrentSourceTv() && (!mni
					.iCurrentChannelHasSignal()))) {
				fflag = true;
			} else if (mni.isCurrentSourceTv() && !mni.iHasChannel()) {
				fflag = true;
			} else if (mni.isCurrentChannelRadio()){
				fflag = true;
			} else if (!nsi.isCapLogo()) {
				fflag = true;
			}else if(!nsi.isEnableFreeze()){
				fflag = true;
			}else if(mPippopUiLogic.isPipOrPopState() && ("sub").equals(mPippopUiLogic.getOutputFocus())){
				fflag = true;
			}
			
			if (fflag){	
				setText(R.string.nav_no_function);
				setVisibility(View.VISIBLE);
				break;
			}
			
			isFreeze = nsi.isFreeze();
			isFreeze = !isFreeze;
			nsi.setFreeze(isFreeze);
			String[] freezeModeName = mContext.getResources().getStringArray(
					R.array.nav_freeze_strings);
			if (isFreeze) {
				setText(freezeModeName[0]);
			}else {
				setText(freezeModeName[1]);
			}
			
			setVisibility(View.VISIBLE);
			break;
		default:
			break;
		}
	}

	// to show current time
	private Runnable updateTimer = new Runnable() {
		public void run() {
			if (timeFlag) {
				setText(new SimpleDateFormat("HH:mm:ss").format(TVTimerManager
						.getInstance(mContext).getUTC()));
				mHandler.postDelayed(this, MessageType.delayMillis6);
			}
		}
	};

	boolean isAbnormal() {
		return mni.isCurrentSourceBlocking()
				|| mni.isCurrentChannelBlocking()
				|| !mni.iCurrentInputSourceHasSignal()
				|| !mni.iCurrentChannelHasSignal()
				|| mni.isAudioScrambled()				
				|| !mni.isCurrentSourceTv()
				|| (mni.iGetChannelLength() == 0);
	}
	
	public static boolean isFreeze() {
		return isFreeze;
	}

	public static void setFreeze(boolean isFreeze) {
		NavSundryShowTextView.isFreeze = isFreeze;
	}
	public void unregisterReceiver() {
		if (mContext != null) {
			mContext.unregisterReceiver(mBroadcastReceiver);
		}
	}
}
