package com.mediatek.ui.nav.commonview;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.content.Context;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcm.TVTeleTextManager;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOption;
import com.mediatek.tvcommon.TVOutputCommon;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.epginfo.EPGChannel;
import com.mediatek.ui.nav.util.DetailTextReader;
import com.mediatek.ui.nav.util.DetailTextReader.TextReaderPageChangeListener;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.BypassWindowManager;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class BannerView extends LinearLayout {

	private Context mContext;

	// program panel
	private BasicBanner mBasicBanner;
	// channel panel
	private SimpleBanner mSimpleBanner;
	// detail panner
	private DetailBanner mDetailBanner;
	private View bannerLayout;
	private View topBanner;

	private NavIntegration mNavIntegration;
	private NavSundryImplement nsi;
	private TVTeleTextManager mTvTeleTextManager;
	private static BypassWindowManager mBypassWindowManager;
	private EPGChannel epgChannel;
	private TVOutput mTvOutput;
	private static TVContent mTv;
	private TVInputManager mTVInputManager;
	private TVConfigurer tvcfg;
	private StateManage mStateManage;
	private Rect mSimpleBannerViewRect;
	private int mSimpleBannerViewWindowId = -1;
	private Rect mBasicBannerViewRect;
	private int mBasicBannerViewWindowId = -1;
	private Rect mDetailBannerViewRect;
	private int mDetailBannerViewWindowId = -1;

	private final static String TAG = "BannerView";

	public final static int SPECIAL_NO_CHANNEL = 5;
	public final static int SPECIAL_NO_SIGNAL = 1;
	public final static int SPECIAL_INPUT_LOCK = 2;
	public final static int SPECIAL_CHANNEL_LOCK = 3;
	public final static int SPECIAL_RETRIVING = 4;
	public final static int SPECIAL_NO_SUPPORT = 6;

	// hide all panel
	public final static int CHILD_TYPE_INFO_NONE = 0;
	// show program panel
	public final static int CHILD_TYPE_INFO_SIMPLE = 1;
	// show channel and program panel
	public final static int CHILD_TYPE_INFO_BASIC = 2;
	// show channel program and detail panel
	public final static int CHILD_TYPE_INFO_DETAIL = 3;

	String[] mtsModeName;
	String[] languageModeName = getResources().getStringArray(
			R.array.nav_audio_strings);
	String[] languageTvcmName = getResources().getStringArray(
			R.array.nav_audio_tvcm_strings);

	private boolean isRetriving = false;

	private Handler mHandler;
	private int totalState = -1;

	private boolean isOnkeyInfo = false;
	
	public int getBarTotalState() {
		return totalState;
	}


	private ITimeChange videoFormatTimeChange = new ITimeChange() {

		public void refresh(final TextView tv) {
			// TODO Auto-generated method stub
			final String value;
			if (mNavIntegration.isVideoScrambled()) {
				value = mContext
						.getString(R.string.nav_channel_video_scrambled);

			} else {
				value = videoResolution();
			}
			tv.post(new Runnable() {
				public void run() {
					// TODO Auto-generated method stub
					tv.setText(value);
				}
			});
		}
	};

	private ITimeChange videoFormatSourceTimeChange = new ITimeChange() {

		public void refresh(final TextView tv) {
			// TODO Auto-generated method stub
			final String value;
			if (mNavIntegration.isVideoScrambled()) {
				value = mContext
						.getString(R.string.nav_channel_video_scrambled);

			} else {
				value = videoResolution();
			}
			tv.post(new Runnable() {
				public void run() {
					// TODO Auto-generated method stub
					if (value.equals(mContext
							.getString(R.string.nav_channel_video_scrambled))
							|| value.equals(mContext
									.getString(R.string.nav_resolution_null))) {
						tv.setText(null);
					}else{
						tv.setText(value);
					}
				}
			});
		}
	};

	private ITimeChange timeTimeChange = new ITimeChange() {

		public void refresh(final TextView tv) {
			// TODO Auto-generated method stub
			MtkLog.v(TAG, "refresh");
			final String value;
			value = getCurTime();
			tv.post(new Runnable() {
				public void run() {
					// TODO Auto-generated method stub
					tv.setText(value);
				}
			});
		}
	};

	public BannerView(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
		mContext = context;
		mBasicBanner = new BasicBanner(context);
		mSimpleBanner = new SimpleBanner(context);
		mDetailBanner = new DetailBanner(context);
		mBypassWindowManager = BypassWindowManager.getInstance(context);
	}

	public BannerView(Context context, AttributeSet attrs) {
		super(context, attrs);
		mContext = context;
		mBypassWindowManager = BypassWindowManager.getInstance(context);
	}

	@Override
	protected void onFinishInflate() {
		// TODO Auto-generated method stub
		super.onFinishInflate();

		mContext = getContext();
		((Activity) mContext).getLayoutInflater().inflate(
				R.layout.nav_banner_layout, this);
		init();
	}

	public void init() {
		initView();
		initTVComponent();
		initData();
	}

	private void initView() {
		mBasicBanner = new BasicBanner(mContext);
		mSimpleBanner = new SimpleBanner(mContext);
		mDetailBanner = new DetailBanner(mContext);
		bannerLayout = (View) findViewById(R.id.banner_info_layout);
		topBanner = (View) findViewById(R.id.banner_top_layout);
		LinearLayout.LayoutParams mParams = new LinearLayout.LayoutParams(
				(int) (ScreenConstant.SCREEN_WIDTH *0.889),
				(int) (ScreenConstant.SCREEN_HEIGHT * 0.160));

		topBanner.setLayoutParams(mParams);
		mBasicBanner.initView();
		mSimpleBanner.initView();
		mDetailBanner.initView();
		mTv = TVContent.getInstance(mContext);
		mTVInputManager = mTv.getInputManager();
		mTvOutput = mTVInputManager.getDefaultOutput();
		tvcfg = mTv.getConfigurer();
	}

	private void initTVComponent() {
		mNavIntegration = NavIntegration.getInstance(mContext);
		nsi = NavSundryImplement.getInsNavSundryImplement(mContext);
		epgChannel = EPGChannel.getInstance(mContext);
		mTvTeleTextManager = TVTeleTextManager.getInstance(mContext);
	}

	private void initData() {
		mStateManage = new StateManage();
		mStateManage.addState(CHILD_TYPE_INFO_NONE);
		mStateManage.addState(CHILD_TYPE_INFO_SIMPLE);
		mStateManage.addState(CHILD_TYPE_INFO_BASIC);
		mStateManage.addState(CHILD_TYPE_INFO_DETAIL);
	}

	public Handler getHandler() {
		return mHandler;
	}

	public void setHandler(Handler mHandler) {
		this.mHandler = mHandler;
	}

	/**
	 * get the visibility of each panel
	 * 
	 * @param barNum
	 *            panel num
	 * @return
	 */
	public int getPartVisibility(int barNum) {
		switch (barNum) {
		case CHILD_TYPE_INFO_SIMPLE:
			return mSimpleBanner.getVisibility();
		case CHILD_TYPE_INFO_BASIC:
			return mBasicBanner.getVisibility();
		case CHILD_TYPE_INFO_DETAIL:
			return mDetailBanner.getVisibility();
		}
		return View.INVISIBLE;
	}

	public int getTotalVisibility() {
		return bannerLayout.getVisibility();
	}

	/**
	 * hide external panel layout
	 */
	public void hideAllBanner() {
		mBasicBanner.hide();
		mSimpleBanner.hide();
		mDetailBanner.hide();
		bannerLayout.setVisibility(View.INVISIBLE);
		removeAutoShut();
	}

	/**
	 * hide three panels but show external panel layout
	 */
	public void showNoneBar(boolean isVisible) {
		if (isVisible) {
			mBasicBanner.hide();
			mSimpleBanner.hide();
			mDetailBanner.hide();
		} else {
			showSimpleBar();
			setBannerState(CHILD_TYPE_INFO_SIMPLE);
		}
	}

	/**
	 * show channel panel
	 */
	public void showSimpleBar() {
		bannerLayout.setVisibility(View.VISIBLE);
		mBasicBanner.hide();
		mSimpleBanner.show(false);
		mDetailBanner.hide();
	}

	/**
	 * update channel panel when input channel num
	 * 
	 * @param num
	 */
	public void updateInputting(String num) {
		bannerLayout.setVisibility(View.VISIBLE);
		mBasicBanner.hide();
		mDetailBanner.hide();
		mSimpleBanner.updateInputting(num);
		sendAutoShut(-1);
	}

	public void updateSubtitle() {
		TVChannel channel = mNavIntegration.iGetCurrentChannel();
		if (mNavIntegration.isCurrentSourceDTV() && channel != null
				&& !mNavIntegration.isCurrentChannelBlocking()
				&& mNavIntegration.iCurrentChannelHasSignal()) {
			sendAutoShut(-1);
			if (bannerLayout.getVisibility() != View.VISIBLE) {
				bannerLayout.setVisibility(View.VISIBLE);
				if (mBasicBanner.getVisibility() != View.VISIBLE) {
					show(false, CHILD_TYPE_INFO_BASIC, false);
					setBannerState(CHILD_TYPE_INFO_BASIC);
					mStateManage.setOrietation(true);
				}
			} else if (mBasicBanner.getVisibility() != View.VISIBLE) {
				show(false, CHILD_TYPE_INFO_BASIC, false);
				setBannerState(CHILD_TYPE_INFO_BASIC);
				mStateManage.setOrietation(true);
			} else {
				mNavIntegration.iSetNextSubtitleLang();
				mBasicBanner.show();
			}
		}
	}

	/**
	 * show channel and program panel
	 */
	public void showBasicBar() {
		bannerLayout.setVisibility(View.VISIBLE);
		mBasicBanner.show();
		mSimpleBanner.show(true);
		mDetailBanner.hide();
	}

	public void updateFavorite() {
		TVChannel channel = mNavIntegration.iGetCurrentChannel();
		if (mNavIntegration.iGetChannelLength() > 0 && channel != null) {
			mBasicBanner.setFavoriteVisibility(true);
			sendAutoShut(-1);
			if (bannerLayout.getVisibility() != View.VISIBLE) {
				bannerLayout.setVisibility(View.VISIBLE);
				if (mBasicBanner.getVisibility() != View.VISIBLE) {
					show(false, CHILD_TYPE_INFO_BASIC, false);
					setBannerState(CHILD_TYPE_INFO_BASIC);
					mStateManage.setOrietation(true);
				}
			} else if (mBasicBanner.getVisibility() != View.VISIBLE) {
				show(false, CHILD_TYPE_INFO_BASIC, false);
				setBannerState(CHILD_TYPE_INFO_BASIC);
				mStateManage.setOrietation(true);
			} 			
		}				
	}

	/**
	 * show channel program and detail panel
	 */
	public void showDetailBar() {
		bannerLayout.setVisibility(View.VISIBLE);
		showBasicBar();
		mDetailBanner.show();
	}

	/**
	 * show panel at special state
	 * 
	 * @param state
	 */
	public void showSpecialBar(int state) {
		bannerLayout.setVisibility(View.VISIBLE);
		if (mNavIntegration.isCurrentSourceBlocking()) {
			mSimpleBanner.show(false);
			mBasicBanner.hide();
		} else {
			mSimpleBanner.show(true);
			mBasicBanner.showSpecialInfo(state);
		}
		mDetailBanner.hide();
	}

	public void showByState(int state) {
		switch (state) {
		case CHILD_TYPE_INFO_NONE:
			showNoneBar(true);
			break;
		case CHILD_TYPE_INFO_SIMPLE:
			showSimpleBar();
			break;
		case CHILD_TYPE_INFO_BASIC:
			showBasicBar();
			break;
		case CHILD_TYPE_INFO_DETAIL:
			showDetailBar();
			break;
		}
	}

	/**
	 * show panel according to the previous state
	 * 
	 * @param isForward
	 *            whether to move to the next state after showing
	 * @param state
	 *            whether to show the specific state ,if -1 ,will obey the
	 *            StateManager
	 * @param manualClose
	 *            whether close when it's already showed
	 */
	public void show(boolean isForward, int state, boolean manualClose) {
		if (!mNavIntegration.isTKMainActivity()) {
			return;
		}

		MtkLog.v(TAG, isForward + "");
		MtkLog.v(TAG, state + "");
		MtkLog.v(TAG, manualClose + "");

		if (mBasicBanner.isAnimation || mSimpleBanner.isAnimation
				|| mDetailBanner.isAnimation) {
			return;
		}

		int curState;
		boolean flag = false;

		if (bannerLayout.getVisibility() == View.VISIBLE) {
			flag = true;
		}

		if (state >= 0) {
			showByState(state);
			sendAutoShut(-1);
			totalState = -1;
			return;
		}
		
		if (isSpecial(manualClose)) {
			return;
		}		

		totalState = -1;
		if (isForward && flag) {
			curState = mStateManage.getNextState();
			setBannerState(curState);
		} else {
			curState = mStateManage.getState();
		}
		curState = mStateManage.getState();

		switch (curState) {
		case CHILD_TYPE_INFO_NONE:
			showNoneBar(flag);
			break;
		case CHILD_TYPE_INFO_SIMPLE:
			showSimpleBar();
			break;
		case CHILD_TYPE_INFO_BASIC:
			showBasicBar();
			break;
		case CHILD_TYPE_INFO_DETAIL:
			showDetailBar();
			break;
		}
		sendAutoShut(-1);
	}

	private void sendAutoShut(int state) {
		mHandler.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);

		Message msg = new Message();
		msg.what = MessageType.NAV_BANNERVIEW_DIMISS;
		msg.arg1 = state;

		mHandler.sendMessageDelayed(msg, MessageType.delayMillis4);
	}

	private void sendAutoShutNoDelay(int state) {
		mHandler.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);

		Message msg = new Message();
		msg.what = MessageType.NAV_BANNERVIEW_DIMISS;
		msg.arg1 = state;
		mHandler.sendMessage(msg);
	}

	private void removeAutoShut() {
		mHandler.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);
	}

	@SuppressWarnings("unchecked")
	private boolean isSpecial(boolean manualClose) {
		boolean result = false;
		TVChannel channel = mNavIntegration.iGetCurrentChannel();

		boolean flag = false;
		if (bannerLayout.getVisibility() == View.VISIBLE) {
			flag = true;
		}

		if (mNavIntegration.isCurrentSourceTv()) {
			MtkLog.v(TAG, "tv");
			if (mNavIntegration.isCurrentSourceBlocking()) {
				MtkLog.v(TAG, "tv source blocked");
				showSpecialBar(SPECIAL_INPUT_LOCK);
				sendAutoShut(SPECIAL_INPUT_LOCK);
				totalState = SPECIAL_INPUT_LOCK;
				result = true;
			} else {
				if (mNavIntegration.iGetChannelLength() <= 0) {
					MtkLog.v(TAG, "tv no channel");
					showSpecialBar(SPECIAL_NO_CHANNEL);
					sendAutoShut(SPECIAL_NO_CHANNEL);
					totalState = SPECIAL_NO_CHANNEL;
					result = true;
				} else {
					if (mNavIntegration.isCurrentChannelBlocking()) {
						MtkLog.v(TAG, "tv blocked");
						showSpecialBar(SPECIAL_CHANNEL_LOCK);
						sendAutoShut(SPECIAL_CHANNEL_LOCK);
						totalState = SPECIAL_CHANNEL_LOCK;
						result = true;
					} else {
						if (!mNavIntegration.iCurrentChannelHasSignal()) {
							MtkLog.v(TAG, "tv no signal");
							showSpecialBar(SPECIAL_NO_SIGNAL);
							sendAutoShut(SPECIAL_NO_SIGNAL);
							totalState = SPECIAL_NO_SIGNAL;
							result = true;
						} else {
							MtkLog.v(TAG, "tv normal");
							result = false;
							totalState = -1;
						}
					}
				}
			}
		} else{
		 if (mNavIntegration.vgaNotSupport()) {
			//if (mTvOutput.getInstanceVideoResolution().getTimingType() == TVOutput.VideoResolution.TimingType.TIMING_TYPE_NOT_SUPPORT) {
				//MtkLog.d("ping", "here have source but did't support");
				showSimpleBar();
				MtkLog.v(TAG, "none tv no support");
				sendAutoShut(SPECIAL_NO_SUPPORT);
				totalState = SPECIAL_NO_SUPPORT;
				TVOption<Integer> blueScreeOpt = (TVOption<Integer>) tvcfg
						.getOption(ConfigType.CFG_BLUE_SCREEN);
						if(null != blueScreeOpt){
								//blueScreeOpt.set(ConfigType.COMMON_ON);
			        	result = true;
						}
			//}

		} else {
			MtkLog.v(TAG, "none tv");
			if (mNavIntegration.isCurrentSourceBlocking()) {
				MtkLog.v(TAG, "none tv blocked");
				showSpecialBar(SPECIAL_INPUT_LOCK);
				sendAutoShut(SPECIAL_INPUT_LOCK);
				totalState = SPECIAL_CHANNEL_LOCK;
				result = true;
			} else {
				if (!mNavIntegration.iCurrentInputSourceHasSignal()) {
					showSimpleBar();
					MtkLog.v(TAG, "none tv no signal");
					sendAutoShut(SPECIAL_NO_SIGNAL);
					totalState = SPECIAL_NO_SIGNAL;
					result = true;
				} else {
					showSimpleBar();
					sendAutoShut(-1);
					totalState = -1;
					result = true;
				}
			}
			}
		}

		if (flag && manualClose && result) {
			bannerLayout.setVisibility(View.INVISIBLE);
			removeAutoShut();
			sendAutoShutNoDelay(totalState);
		}
		return result;
	}

	/**
	 * update AudioLanguage
	 */
	public void updateAudioLanguage() {
		mBasicBanner.updateAudioLanguage();
	}

	/**
	 * turn to the previous page of detail program content if existing
	 */
	public void detailPageUp() {
		mDetailBanner.pageUp();
		if (mDetailBanner.getVisibility() == View.VISIBLE) {
			sendAutoShut(-1);
		}
	}

	/**
	 * turn to the next page of detail program content if existing
	 */
	public void detailPageDown() {
		mDetailBanner.pageDown();
		if (mDetailBanner.getVisibility() == View.VISIBLE) {
			sendAutoShut(-1);
		}
	}

	/**
	 * update the banner state in sequence
	 */
	public void setBannerState(int state) {
		mStateManage.setState(state);
	}

	interface ITimeChange {
		void refresh(TextView tv);
	}

	class DelayTextTask {

		TextView tv;
		ITimeChange iTimeChange;
		Timer  mTimer;

		DelayTextTask(TextView tv, ITimeChange iTimeChange) {
			this.tv = tv;
			this.iTimeChange = iTimeChange;
			this.mTimer=new Timer();
		}

		public void start() {
			mTimer.schedule(new TimerTask() {

				@Override
				public void run() {
					if (!Thread.currentThread().isInterrupted()) {
					         iTimeChange.refresh(tv);
				        }
				}
			}, 2000);
		}
	}

	class StateManage {

		private List<Integer> stateArray = new ArrayList<Integer>();
		private int curState = -1;

		private boolean orietation = true;

		private void addState(int state) {
			stateArray.add(state);
			curState = 0;
		}

		private void setState(int state) {
			for (int temp : stateArray) {
				if (temp == state) {
					curState = stateArray.indexOf(temp);
				}
			}
		}

		private int getState() {
			return stateArray.get(curState);
		}

		private int getNextState() {
			int temp;
			MtkLog.v(TAG, "curState " + curState);
			MtkLog.v(TAG, "stateArray.size() " + stateArray.size());
			MtkLog.v(TAG, "orietation " + orietation);
			if (orietation) {
				temp = curState + 1;
				MtkLog.v(TAG, "orietation " + orietation);
				if (temp >= stateArray.size()) {
					orietation = false;
					temp = curState - 1;
				} else {
					return stateArray.get(temp);
				}
			} else {
				temp = curState - 1;
				if (temp < 0) {
					orietation = true;
					temp = curState + 1;
				} else {
					return stateArray.get(temp);
				}
			}
			return stateArray.get(temp);
		}

		private void setOrietation(boolean flag) {
			this.orietation = flag;
		}
	}

	class BasicBanner {

		private Context mContext;

		private View mSelfLayout;

		private TextView mCurProgramName;

		private TextView mSpecialInfo;
		private View mSecondLineLayout;
		private TextView mCurProgramDuration;
		private TextView mAudioLanguage;

		private View mThirdLineLayout;
		private View mNextProgramLayout;
		private TextView mNextProgramName;
		private TextView mNextProgramDuration;

		private View mIconsLayout;
		private TextView mLockIcon;
		private TextView mFavoriteIcon;
		private TextView mSubtitleIcon;
		private TextView mTTXICon;

		private TextView mVideoFormat;

		private DelayTextTask vfDelayTextTask;

		String[] mtsAudioMode;
		
		public boolean isAnimation = false;

		public BasicBanner(Context context) {
			// TODO Auto-generated constructor stub
			mContext = context;
			mtsAudioMode = mContext.getResources().getStringArray(
					R.array.nav_mts_strings);
		}

		public BasicBanner(Context context, AttributeSet attrs) {
			mContext = context;
			mtsAudioMode = mContext.getResources().getStringArray(
					R.array.nav_mts_strings);
		}

		private void initView() {
			mSelfLayout = (View) findViewById(R.id.banner_basic_layout);
			mCurProgramName = (TextView) findViewById(R.id.banner_current_program_title);
			mSecondLineLayout = (View) findViewById(R.id.banner_basic_second_line);
			mSpecialInfo = (TextView) findViewById(R.id.banner_special_info);
			mSpecialInfo.setPadding((int)(0.334*ScreenConstant.SCREEN_WIDTH), 0, 0, 0);
			mCurProgramDuration = (TextView) findViewById(R.id.banner_current_program_duration);
			mAudioLanguage = (TextView) findViewById(R.id.banner_audio_language);
			mThirdLineLayout = (View) findViewById(R.id.banner_basic_third_line);
			mNextProgramLayout = (View) findViewById(R.id.banner_next_program_layout);
			mNextProgramName = (TextView) findViewById(R.id.banner_next_program_name);
			mNextProgramDuration = (TextView) findViewById(R.id.banner_next_program_duration);
			mIconsLayout = (View) findViewById(R.id.banner_basic_icons_layout);
			mLockIcon = (TextView) findViewById(R.id.banner_channel_lock_icon);
			mFavoriteIcon = (TextView) findViewById(R.id.banner_channel_favorite_icon);
			mSubtitleIcon = (TextView) findViewById(R.id.banner_channel_subtitle_icon);
			mTTXICon = (TextView) findViewById(R.id.banner_channel_ttx_icon);
			mVideoFormat = (TextView) findViewById(R.id.banner_video_format);
			vfDelayTextTask = new DelayTextTask(mVideoFormat,
					videoFormatTimeChange);
		}

		private int getVisibility() {
			return mSelfLayout.getVisibility();
		}

		private void showSpecialInfo(int state) {
			String text = "";
			if (mNavIntegration.isCurrentSourceBlocking() && mSelfLayout.getVisibility()==VISIBLE) {
				mSelfLayout.setVisibility(View.INVISIBLE);
				MtkLog.v(TAG,"Basic hide");
				if (mBasicBannerViewWindowId!=-1) {
					mBypassWindowManager.setBypassWindow(false,
							mBasicBannerViewWindowId, mBasicBannerViewRect);
				}
				topBanner
						.setBackgroundResource(R.drawable.translucent_background);
			} else {
				if (mSelfLayout.getVisibility()!=VISIBLE) {
					mSelfLayout.setVisibility(View.VISIBLE);
					
					if(isOnkeyInfo &&
							AnimationManager.getInstance().getIsAnimation()){
						AnimationManager.getInstance().startAnimation(AnimationManager.TYPE_BASIC_BANNER_ENTER, mSelfLayout, new AnimatorListenerAdapter() {
							public void onAnimationStart(Animator animation) {
								isAnimation = true;
								topBanner
								.setBackgroundResource(R.drawable.translucent_background);

								mSelfLayout.setBackgroundResource(R.drawable.nav_infobar_basic_bg);
								mSimpleBanner.setSimpleBannerBg(true);
								super.onAnimationStart(animation);
							}
							@Override
							public void onAnimationEnd(Animator animation) {
								isAnimation = false;
								super.onAnimationEnd(animation);
								mSelfLayout.setBackgroundResource(R.drawable.translucent_background);
								topBanner
									.setBackgroundResource(R.drawable.nav_infobar_basic_bg);
								mSimpleBanner.setSimpleBannerBg(false);
							}
						});
					}else{
						mSimpleBanner.setSimpleBannerBg(false);

						topBanner
						.setBackgroundResource(R.drawable.nav_infobar_basic_bg);
					}
					
					mBasicBannerViewWindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBasicBannerViewRect = getRectInScreen(mSelfLayout);
					MtkLog.i(TAG, "mBasicBannerViewRect.width():"
							+ mBasicBannerViewRect.width());
					MtkLog.i(TAG, "show mBasicBannerViewWindowId:"
							+ mBasicBannerViewWindowId);
					mBypassWindowManager.setBypassWindow(true,
							mBasicBannerViewWindowId, mBasicBannerViewRect);
					MtkLog.i(TAG, "mBasicBannerView show:"
							+ mBasicBannerViewWindowId);
				}
				 if (mNavIntegration.isCurrentSourceTv()) {
                     if (null!=TimeShiftManager.getInstance()) {
                         TimeShiftManager.getInstance().stopAllRunning();
                     }
                 }
				setSpecialVisibility(true);
				switch (state) {
				case SPECIAL_NO_CHANNEL:
					mIconsLayout.setVisibility(View.INVISIBLE);
					text = mContext
							.getString(R.string.nav_please_scan_channels);
					break;
				case SPECIAL_NO_SIGNAL:
					mIconsLayout.setVisibility(View.VISIBLE);
					setLockVisibility(true);
					setFavoriteVisibility(true);
					setSubtitleVisibility(false);
					setTTXIconVisibility(false);
					text = mContext.getString(R.string.nav_no_signal);
					break;
					case SPECIAL_NO_SUPPORT :
						mIconsLayout.setVisibility(View.VISIBLE);
						setLockVisibility(true);
						setFavoriteVisibility(true);
						setSubtitleVisibility(false);
						setTTXIconVisibility(false);
						text = mContext.getString(R.string.nav_no_support);
						break;
				case SPECIAL_INPUT_LOCK:
					mIconsLayout.setVisibility(View.INVISIBLE);
					text = mContext.getString(R.string.nav_input_has_locked);
					break;
				case SPECIAL_CHANNEL_LOCK:
					mIconsLayout.setVisibility(View.VISIBLE);
					setLockVisibility(true);
					setFavoriteVisibility(true);
					setSubtitleVisibility(false);
					setTTXIconVisibility(false);
					text = mContext.getString(R.string.nav_channel_has_locked);
					break;
				case SPECIAL_RETRIVING:
					mIconsLayout.setVisibility(View.INVISIBLE);
					text = mContext.getString(R.string.nav_channel_retrieving);
					break;
				}
				MtkLog.v(TAG, mSpecialInfo.getVisibility() + "  " + text);
				mSpecialInfo.setText(text);
			}
		}

		private void show() {
			if (mSelfLayout.getVisibility()!=VISIBLE) {

				mSelfLayout.setVisibility(View.VISIBLE);
				mBasicBannerViewWindowId = mBypassWindowManager
						.getAvailableWindowId();
				mBasicBannerViewRect = getRectInScreen(mSelfLayout);
				MtkLog.i(TAG, " show mBasicBannerViewWindowId:"
						+ mBasicBannerViewWindowId);
				MtkLog.i(TAG, "mBasicBannerViewRect.left:"
						+ mBasicBannerViewRect.left);
				mBypassWindowManager.setBypassWindow(true,
						mBasicBannerViewWindowId, mBasicBannerViewRect);

				if (isOnkeyInfo &&
					AnimationManager.getInstance().getIsAnimation()){
					AnimationManager.getInstance().startAnimation(
							AnimationManager.TYPE_BASIC_BANNER_ENTER,
							mSelfLayout, new AnimatorListenerAdapter() {
								@Override
								public void onAnimationStart(Animator animation) {
									isAnimation = true;
									mSelfLayout.setBackgroundResource(R.drawable.nav_infobar_basic_bg);
									mSimpleBanner.setSimpleBannerBg(true);
									super.onAnimationStart(animation);
								}
								public void onAnimationEnd(Animator animation) {
									isAnimation = false;
									mSelfLayout.setBackgroundResource(R.drawable.translucent_background);
									mSimpleBanner.setSimpleBannerBg(false);
									topBanner.setBackgroundResource(R.drawable.nav_infobar_basic_bg);
								}
							});
				}
				else{
					mSimpleBanner.setSimpleBannerBg(false);
					topBanner.setBackgroundResource(R.drawable.nav_infobar_basic_bg);
				}
			}
			setSpecialVisibility(false);
			mCurProgramName.setText(epgChannel.getCurEventTitle());
			mCurProgramDuration.setText(epgChannel.getCurEventTime());

			updateAudioLanguage();

			mNextProgramName.setText(epgChannel.getNextEventTitle());
			mNextProgramDuration.setText(epgChannel.getNextEventTime());

			setLockVisibility(true);
			setFavoriteVisibility(true);
			setSubtitleVisibility(true);
			setTTXIconVisibility(true);

			if (mNavIntegration.isVideoScrambled()) {
				mVideoFormat.setText(R.string.nav_channel_video_scrambled);
			} else {
				mVideoFormat.setText(videoResolution());
			}
			vfDelayTextTask.start();
		}

		private void updateAudioLanguage() {
			if (mSecondLineLayout.getVisibility() == View.VISIBLE) {
				if (mNavIntegration.isAudioScrambled()) {
					mAudioLanguage
							.setText(R.string.nav_channel_audio_scrambled);
				} else {
					if (mNavIntegration.isCurrentChannelPAL()) {

						int curMts = nsi.getCurrentMtsEffect();
						int totalNum = nsi.getCurrentMtsEffectTotalNum();
						int curNum = nsi.getCurrentMtsEffecIndex() + 1;
						String mtsNum;
						if (totalNum <= 1 || curNum < 1) {
							mtsNum = "";
						} else {
							mtsNum = curNum + "/" + totalNum;
						}	
						mtsModeName = getResources().getStringArray(
								R.array.nav_mts_strings);
						mAudioLanguage.setText(mtsNum + "  "
								+ mtsModeName[curMts]);
					} else if (mNavIntegration.isCurrentChannelDTV()) {
						String curEffect = nsi.getLanguageShowString(nsi
								.getCurrentLanguageEffect());
						String noFunction = mContext
								.getString(R.string.nav_no_function);
						if (curEffect.equals(noFunction) || curEffect == null) {
							curEffect = mContext
									.getString(R.string.nav_unknown_audio);
						}
						int totalEffectNum = nsi.getTotalLanguageEffectNum();
						int curEffectNumIndex = nsi
								.getCurrentLanguageEffectIndex();
						String curEffectNum;
						if (totalEffectNum <= 1) {
							curEffectNum = "";
						} else {
							if (curEffectNumIndex < 1) {
								curEffectNum = "";
							} else {
								curEffectNum = curEffectNumIndex + "/"
										+ totalEffectNum;
							}
						}
						mAudioLanguage.setText(curEffectNum + " " + curEffect);
					} else if (mNavIntegration.isCurrentChannelRadio()) {
						String curEffect = nsi.getLanguageShowString(nsi
								.getCurrentLanguageEffect());
						String noFunction = mContext
								.getString(R.string.nav_no_function);
						if (curEffect.equals(noFunction) || curEffect == null) {
							curEffect = mContext
									.getString(R.string.nav_unknown_stereo);
						}
						mAudioLanguage.setText(curEffect);
					}
				}
			}
		}

		private void hide() {
			if (mSelfLayout.getVisibility()==VISIBLE) {

				if (mBasicBannerViewWindowId!=-1) {
					mBypassWindowManager.setBypassWindow(false,
							mBasicBannerViewWindowId, mBasicBannerViewRect);
				}
				MtkLog.i(TAG," hide mBasicBannerViewWindowId:"+mBasicBannerViewWindowId);
				
				if (isOnkeyInfo && AnimationManager.getInstance().getIsAnimation()) {
					AnimationManager.getInstance().startAnimation(
							AnimationManager.TYPE_BASIC_BANNER_EXIT,
							mSelfLayout, new AnimatorListenerAdapter() {
								@Override
								public void onAnimationStart(Animator animation) {
									isAnimation = true;
									mSelfLayout
											.setBackgroundResource(R.drawable.nav_infobar_basic_bg);
									mSimpleBanner.setSimpleBannerBg(true);
									super.onAnimationStart(animation);
								}

								@Override
								public void onAnimationEnd(Animator animation) {
									isAnimation = false;
									mSelfLayout.setVisibility(View.INVISIBLE);
									super.onAnimationEnd(animation);
								}
							});
				} else {
					mSimpleBanner.setSimpleBannerBg(true);
					mSelfLayout.setVisibility(View.INVISIBLE);
				}
			}
			topBanner.setBackgroundResource(R.drawable.translucent_background);
		}

		private void setSpecialVisibility(boolean flag) {
			if (flag) {
				mCurProgramName.setVisibility(View.INVISIBLE);
				mSecondLineLayout.setVisibility(View.GONE);
				mNextProgramLayout.setVisibility(View.INVISIBLE);
				mSpecialInfo.setVisibility(View.VISIBLE);
				mVideoFormat.setVisibility(View.INVISIBLE);
			} else {
				mCurProgramName.setVisibility(View.VISIBLE);
				mSecondLineLayout.setVisibility(View.VISIBLE);
				mNextProgramLayout.setVisibility(View.VISIBLE);
				mIconsLayout.setVisibility(View.VISIBLE);
				mSpecialInfo.setVisibility(View.GONE);
				mVideoFormat.setVisibility(View.VISIBLE);
			}
		}

		private void setLockVisibility(boolean flag) {
			TVChannel mCurChannel = mNavIntegration.iGetCurrentChannel();
			if (mCurChannel == null) {
				mLockIcon.setVisibility(View.INVISIBLE);
			} else {
				if (flag) {
					if (mNavIntegration.isCurrentChannelBlocking()) {
						mLockIcon.setVisibility(View.VISIBLE);
					} else {
						mLockIcon.setVisibility(View.INVISIBLE);
					}
				} else {
					mLockIcon.setVisibility(View.INVISIBLE);
				}
			}
		}
		
		private void setTTXIconVisibility(boolean flag) {
			if (flag) {
				if (mNavIntegration.isCurrentSourceATV()
						&& mTvTeleTextManager.getTTXIconShow()) {
					MtkLog.i(TAG," mTTXICon.setVisibility(View.VISIBLE)");
					mTTXICon.setVisibility(View.VISIBLE);
				}
			} else {
				MtkLog.i(TAG," mTTXICon.setVisibility(View.INVISIBLE)");
				mTTXICon.setVisibility(View.INVISIBLE);
			}
		}

		private void setFavoriteVisibility(boolean flag) {
			TVChannel mCurChannel = mNavIntegration.iGetCurrentChannel();
			if (mCurChannel == null) {
				mFavoriteIcon.setVisibility(View.INVISIBLE);
			} else {
				if (flag) {
					if (mCurChannel.isFavorite()) {
						mFavoriteIcon.setVisibility(View.VISIBLE);
					} else {
						mFavoriteIcon.setVisibility(View.INVISIBLE);
					}
				} else {
					mFavoriteIcon.setVisibility(View.INVISIBLE);
				}
			}
		}

		private void setSubtitleVisibility(boolean flag) {
			if (flag) {
				if (mNavIntegration.isCurrentChannelDTV()) {
					// wait to be update
					String sub = mNavIntegration
							.iGetCurrentSubtitleLangAddIndex();
					if (null == sub || sub.length() == 0) {
						mSubtitleIcon.setVisibility(View.INVISIBLE);
					} else {
						mSubtitleIcon.setVisibility(View.VISIBLE);
						if (!sub.equals("stop")) {
							mSubtitleIcon.setText(sub);
						}else{
							mSubtitleIcon.setText(null);
						}
					}
				} else {
					mSubtitleIcon.setVisibility(View.INVISIBLE);
				}
			} else {
				mSubtitleIcon.setVisibility(View.INVISIBLE);
			}
		}
	}

	class SimpleBanner {

		private Context mContext;

		private View mSelfLayout;

		/**
		 * for channel number or source name
		 */
		private TextView mFirstLine;

		private View mSecondLine;
		private TextView mChannelName;
		private ImageView mLockIcon;

		private View mThirdLine;
		/**
		 * resolution
		 */
		private TextView mThirdMiddle;
		private View mTypeTimeLayout;
		private TextView mReceiverType;
		private TextView mTime;

		private DelayTextTask vfsDelayTextTask;
		private DelayTextTask timeDelayTextTask;

		public boolean isAnimation = false;

		public SimpleBanner(Context context) {
			// TODO Auto-generated constructor stub
			mContext = context;
		}

		public SimpleBanner(Context context, AttributeSet attrs) {
			mContext = context;
		}

		private void initView() {
			mSelfLayout = (View) findViewById(R.id.banner_simple_layout);
			mFirstLine = (TextView) findViewById(R.id.banner_simple_first_line);
			mSecondLine = (View) findViewById(R.id.banner_simple_second_line_layout);
			mChannelName = (TextView) findViewById(R.id.banner_simple_channel_name);
			mLockIcon = (ImageView) findViewById(R.id.banner_simple_lock_icon);
			mThirdLine = (View) findViewById(R.id.banner_simple_third_line_layout);
			mThirdMiddle = (TextView) findViewById(R.id.banner_simple_third_middle);
			vfsDelayTextTask = new DelayTextTask(mThirdMiddle,
					videoFormatSourceTimeChange);
			mTypeTimeLayout = (View) findViewById(R.id.banner_simple_type_time_layout);
			mReceiverType = (TextView) findViewById(R.id.banner_simple_receiver_type);
			mTime = (TextView) findViewById(R.id.banner_simple_time);
			timeDelayTextTask = new DelayTextTask(mTime, timeTimeChange);
		}

		private int getVisibility() {
			return mSelfLayout.getVisibility();
		}

		private void show(boolean isBasicShow) {
			boolean doShow = (getVisibility() != View.VISIBLE || mTime
					.getVisibility() != View.VISIBLE);
			if (getVisibility()!=View.VISIBLE) {
				mSelfLayout.setVisibility(View.VISIBLE);
				mSimpleBannerViewWindowId = mBypassWindowManager
						.getAvailableWindowId();
				MtkLog.i(TAG, "show mSimpleBannerViewWindowId:"
						+ mSimpleBannerViewWindowId);
				mSimpleBannerViewRect = getRectInScreen(mSelfLayout);
				mBypassWindowManager.setBypassWindow(true,
						mSimpleBannerViewWindowId, mSimpleBannerViewRect);

				if (isOnkeyInfo && AnimationManager.getInstance().getIsAnimation()){
					AnimationManager.getInstance().startAnimation(
							AnimationManager.TYPE_SIMPLE_BANNER_ENTER,
							mSelfLayout, new AnimatorListenerAdapter() {
								@Override
								public void onAnimationEnd(Animator animation) {
									isAnimation = false;
									super.onAnimationEnd(animation);
								}
								@Override
								public void onAnimationStart(Animator animation) {
									isAnimation = true;
									mSelfLayout
											.setBackgroundResource(R.drawable.nav_infobar_smaple_bg);
									super.onAnimationStart(animation);
								}
							});
				}else{
					AnimationManager.getInstance().startAnimation(
							AnimationManager.TYPE_SIMPLE_BANNER_ENTER_0,
							mSelfLayout, new AnimatorListenerAdapter() {
								@Override
								public void onAnimationStart(Animator animation) {
									super.onAnimationStart(animation);
								}
							});
					if (isBasicShow) {
						// show both simple and simple banner
						mSelfLayout
								.setBackgroundResource(R.drawable.translucent_background);
					} else {
						// show only simple banner
						mSelfLayout
								.setBackgroundResource(R.drawable.nav_infobar_smaple_bg);
					}
				}
			}
			
			if (mNavIntegration.isCurrentSourceTv()) {
				if (mNavIntegration.isCurrentSourceBlocking()) {
					showSimpleSource();
				} else {
					if (mNavIntegration.iGetChannelLength() <= 0) {
						showSimpleTime();
					} else {
						showSimpleChannel();
						showTime(doShow);
					}
				}
			} else {
				showSimpleSource();
			}				
		}
		
		private void showTime (boolean doShow) {
			if(doShow){
				mTime.setVisibility(View.VISIBLE);
				mTime.setText(null);
				timeDelayTextTask.start();
			}
		}

		private void updateInputting(String num) {
			mSelfLayout.setVisibility(View.VISIBLE);
			mSimpleBannerViewWindowId = mBypassWindowManager
					.getAvailableWindowId();
			mSimpleBannerViewRect = getRectInScreen(mSelfLayout);
			Log.i(TAG, "mSimpleBannerViewWindowId:"+mSimpleBannerViewWindowId);
			mBypassWindowManager.setBypassWindow(true,
					mSimpleBannerViewWindowId, mSimpleBannerViewRect);
			mSelfLayout.setBackgroundResource(R.drawable.nav_infobar_smaple_bg);
			mFirstLine.setVisibility(View.VISIBLE);
			mFirstLine.setText(num);
			mLockIcon.setVisibility(View.GONE);
			mChannelName.setVisibility(View.INVISIBLE);
			mTypeTimeLayout.setVisibility(View.INVISIBLE);
			mThirdMiddle.setVisibility(View.GONE);

			// this may be changed if the type can be set
			mReceiverType.setVisibility(View.INVISIBLE);
			mTime.setVisibility(View.INVISIBLE);
		}

		public void hide() {
			if (mSelfLayout.getVisibility()==VISIBLE) {
				MtkLog.i(TAG, "hide mSimpleBannerViewWindowId: "
						+ mSimpleBannerViewWindowId);
				if (mSimpleBannerViewWindowId!=-1) {
					mBypassWindowManager.setBypassWindow(false,
							mSimpleBannerViewWindowId, mSimpleBannerViewRect);
				}
				
				if (isOnkeyInfo && AnimationManager.getInstance().getIsAnimation()) {
					AnimationManager.getInstance().startAnimation(
							AnimationManager.TYPE_SIMPLE_BANNER_EXIT,
							mSelfLayout, new AnimatorListenerAdapter() {
								@Override
								public void onAnimationStart(Animator animation) {
									isAnimation = true;
									super.onAnimationStart(animation);
								}
								@Override
								public void onAnimationEnd(Animator animation) {
									isAnimation = false;
									mSelfLayout.setVisibility(View.INVISIBLE);
									super.onAnimationEnd(animation);
								}
							});
				} else {
					mSelfLayout.setVisibility(View.INVISIBLE);
				}
			}
		}

		public void setSimpleBannerBg(boolean isShow) {
			if (isShow) {
				mSelfLayout
						.setBackgroundResource(R.drawable.nav_infobar_smaple_bg);
			} else {
				mSelfLayout
						.setBackgroundResource(R.drawable.translucent_background);
			}
		}
		private void showSimpleSource() {
			mFirstLine.setVisibility(View.VISIBLE);
			String sourceName = mNavIntegration.iGetCurrentSouce();
			int mhlNum = TVOutputCommon.getInstance(mContext).getMHLPortNum(
					TVCommonManager.getInstance(mContext).getCurrentOutput());
			int supportMHL = SystemProperties.getInt("mtk.isSupportMHL", 0);
			if (sourceName.equals("hdmi" + mhlNum) && (supportMHL == 1)) {
				sourceName = sourceName + "/MHL";
			}
			mFirstLine.setText(sourceName);
			mChannelName.setVisibility(View.GONE);
			if (mNavIntegration.isCurrentSourceBlocking()) {
				mLockIcon.setVisibility(View.VISIBLE);
			} else {
				mLockIcon.setVisibility(View.INVISIBLE);
			}
			mTypeTimeLayout.setVisibility(View.GONE);
			mThirdMiddle.setVisibility(View.VISIBLE);
			mThirdMiddle.setText(null);
			vfsDelayTextTask.start();
		}

		private void showSimpleChannel() {
			TVChannel curChannel = mNavIntegration.iGetCurrentChannel();

			if (curChannel == null) {
				mFirstLine.setVisibility(View.INVISIBLE);
				mChannelName.setVisibility(View.INVISIBLE);
				mReceiverType.setVisibility(View.INVISIBLE);
			} else {
				mFirstLine.setVisibility(View.VISIBLE);
				mFirstLine.setText(curChannel.getChannelNum() + "");
				mChannelName.setVisibility(View.VISIBLE);
				mChannelName.setText(curChannel.getChannelName());
				mReceiverType.setVisibility(View.VISIBLE);
				mReceiverType.setText(getTunnerModeInfo());
			}
			mLockIcon.setVisibility(View.GONE);

			mTypeTimeLayout.setVisibility(View.VISIBLE);
			mThirdMiddle.setVisibility(View.GONE);			
		}

		private void showSimpleTime() {
			mFirstLine.setVisibility(View.INVISIBLE);
			mChannelName.setVisibility(View.INVISIBLE);
			mLockIcon.setVisibility(View.GONE);
			mTypeTimeLayout.setVisibility(View.VISIBLE);
			mThirdMiddle.setVisibility(View.GONE);
			mReceiverType.setVisibility(View.INVISIBLE);

			mTime.setVisibility(View.VISIBLE);
			mTime.setText(null);
			timeDelayTextTask.start();
		}

		public String getChannelType() {
			if (mNavIntegration.isCurrentSourceTv()) {
				if (mNavIntegration.isCurrentSourceDTV()) {
					return mContext.getResources().getStringArray(
							R.array.nav_receiver_type)[0];
				} else if (mNavIntegration.iGetChannelLength() > 0
						&& mNavIntegration.isCurrentChannelPAL()) {
					return mContext.getResources().getStringArray(
							R.array.nav_receiver_type)[1];
				} else if (mNavIntegration.isCurrentChannelRadio()) {
					return mContext.getResources().getStringArray(
							R.array.nav_receiver_type)[1];
				} else {
					return null;
				}
			}else{
				return null;
			}			
		}
	}
	public String getTunnerModeInfo() {
		if(mNavIntegration.isCurrentSourceTv()){
			 int tunnerMode =MenuConfigManager.getInstance(mContext).getDefault(MenuConfigManager.TUNER_MODE);
			 if(tunnerMode ==ConfigType.BS_SRC_AIR){
				 return mContext.getResources().getStringArray(
						 R.array.nav_receiver_type)[1];
							
			 }else{
				 return mContext.getResources().getStringArray(
						 R.array.nav_receiver_type)[0];
			 }
		}else{
			return null;
		}
		
	}
	class DetailBanner {

		private Context mContext;

		private View mSelfLayout;

		private TextView mDetailInfo;

		private View mDetailIconLayout;
		private ImageView mDetailUpArrow;
		private ImageView mDetailDownArrow;
		private TextView mPageNum;

		private DetailTextReader detailTextReader;
		
		public boolean isAnimation;

		private TextReaderPageChangeListener detailPageChangeListener = new TextReaderPageChangeListener() {

			public void onPageChanged(int page) {
				// TODO Auto-generated method stub
				if (detailTextReader.getTotalPage() <= 1) {
					mDetailIconLayout.setVisibility(View.INVISIBLE);
				} else {
					mDetailIconLayout.setVisibility(View.VISIBLE);
					if (page <= 1) {
						mDetailUpArrow.setVisibility(View.INVISIBLE);
						mDetailDownArrow.setVisibility(View.VISIBLE);
					} else if (page >= detailTextReader.getTotalPage()) {
						mDetailUpArrow.setVisibility(View.VISIBLE);
						mDetailDownArrow.setVisibility(View.INVISIBLE);
					} else {
						mDetailUpArrow.setVisibility(View.VISIBLE);
						mDetailDownArrow.setVisibility(View.VISIBLE);
					}
					mPageNum.setText(page + "/"
							+ detailTextReader.getTotalPage());
				}
			}
		};

		public DetailBanner(Context context) {
			// TODO Auto-generated constructor stub
			mContext = context;
			detailTextReader = DetailTextReader.getInstance();
			detailTextReader
					.registerPageChangeListener(detailPageChangeListener);
		}

		public DetailBanner(Context context, AttributeSet attrs) {
			mContext = context;
			detailTextReader = DetailTextReader.getInstance();
			detailTextReader
					.registerPageChangeListener(detailPageChangeListener);
		}

		private void initView() {
			mSelfLayout = (View) findViewById(R.id.banner_detail_layout);
			LinearLayout.LayoutParams mParams = new LinearLayout.LayoutParams(
					(int) (ScreenConstant.SCREEN_WIDTH * 0.889),
					(int) (ScreenConstant.SCREEN_HEIGHT  * 0.150));

			mSelfLayout.setLayoutParams(mParams);
			mDetailInfo = (TextView) findViewById(R.id.banner_detail_info);
			mDetailIconLayout = (View) findViewById(R.id.banner_detail_info_right_layout);
			mDetailUpArrow = (ImageView) findViewById(R.id.banner_detail_info_uparrow);
			mDetailDownArrow = (ImageView) findViewById(R.id.banner_detail_info_downarrow);
			mPageNum = (TextView) findViewById(R.id.banner_detail_info_pagenum);
			detailTextReader.setTextView(mDetailInfo);
		}

		private int getVisibility() {
			return mSelfLayout.getVisibility();
		}

		private void show() {
			if(mSelfLayout.getVisibility()!=VISIBLE){
			if(isAnimation)	return;	
			
			mSelfLayout.setVisibility(View.VISIBLE);
			mDetailBannerViewWindowId = mBypassWindowManager
					.getAvailableWindowId();
			MtkLog.i(TAG, "show mDetailBannerViewWindowId:"
					+ mDetailBannerViewWindowId);
			mDetailBannerViewWindowId = mBypassWindowManager
					.getAvailableWindowId();
			
			mDetailBannerViewRect = getRectInScreen(mSelfLayout);
			MtkLog.i(TAG, "mDetailBannerViewRect.left:"
					+ mDetailBannerViewRect.left);
			
			mBypassWindowManager.setBypassWindow(true,
					mDetailBannerViewWindowId, mDetailBannerViewRect);
					String mEventDetail = epgChannel.getEventDetail();
			if (null == mEventDetail 
					|| mEventDetail.length() == 0 ) {
				mDetailInfo.setText(mContext
						.getString(R.string.nav_channel_infoNoProgram));
			} else {
				mDetailInfo.setText(mEventDetail);
			}
			detailTextReader.setTextView(mDetailInfo);
			
				if (isOnkeyInfo
						&& AnimationManager.getInstance().getIsAnimation())
					AnimationManager.getInstance().startAnimation(
							AnimationManager.TYPE_DETAIL_BANNER_ENTER,
							mSelfLayout, new AnimatorListenerAdapter() {
								@Override
								public void onAnimationStart(Animator animation) {
									isAnimation = true;
									super.onAnimationStart(animation);
								}
								@Override
								public void onAnimationEnd(Animator animation) {
									isAnimation = false;
									super.onAnimationEnd(animation);
								}
							});
			}
		}

		private void hide() {
			if (mSelfLayout.getVisibility()==VISIBLE) {
				MtkLog.i(TAG, "hide mDetailBannerViewWindowId: "
						+ mDetailBannerViewWindowId);
				if (mDetailBannerViewWindowId!=-1) {
					mBypassWindowManager.setBypassWindow(false,
							mDetailBannerViewWindowId, mDetailBannerViewRect);
				}
				if (isOnkeyInfo && AnimationManager.getInstance().getIsAnimation()) {
					AnimationManager.getInstance().startAnimation(
							AnimationManager.TYPE_DETAIL_BANNER_EXIT,
							mSelfLayout, new AnimatorListenerAdapter() {
								@Override
								public void onAnimationStart(Animator animation) {
									isAnimation = true;
									super.onAnimationStart(animation);
								}
								@Override
								public void onAnimationEnd(Animator animation) {
									isAnimation = false;
									mSelfLayout.setVisibility(View.INVISIBLE);
									super.onAnimationEnd(animation);
								}
							});
				} else {
					mSelfLayout.setVisibility(View.INVISIBLE);
				}
			}
		}

		private void pageDown() {
			if (mSelfLayout.getVisibility() == View.VISIBLE) {
				detailTextReader.pageDown();
			}
		}

		private void pageUp() {
			if (mSelfLayout.getVisibility() == View.VISIBLE) {
				detailTextReader.pageUp();
			}
		}
	}

	private String videoResolution() {
		int width = 0;
		int height = 0;
		String rString = null;
		String pString = null;
		String dString = null;
		
		Rect mResolutionRect = mNavIntegration.iGetResolution();
    
		if(mResolutionRect!=null){
		width = mResolutionRect.width();
		height = mResolutionRect.height(); 
		 MtkLog.v(TAG, "width" + width);
		 MtkLog.v(TAG, "height" + height);
		}
		if (400 <= height&& height < 528) {
			rString = "480";
		} else if (528 <= height && height < 648) {
			rString = "576";
		} else if (648 <= height && height < 900) {
			rString = "720";
		} else if (900 <= height && height <= 1200) {
			rString = "1080";
		}
		if (height > 0 && width > 0) {
			if (rString == null) {
				rString = height + "";
			}
			if (mNavIntegration.isVideoProgressive()) {
				pString = "p";
			} else {
				pString = "i";
			}
			dString = mNavIntegration.iGetVideoFormat();
			if (mNavIntegration.isCurrentSourceVGA()) {
				int frequency = mNavIntegration.iGetVedioFrequency();
				rString = "" + width + "X" + height + "@" + frequency + "Hz";
				return rString;
			} else if (mNavIntegration.isCurrentSourceHDMI()) {
//				int hdmiMode = MenuConfigManager.getInstance(mContext)
//						.getDefault(MenuConfigManager.HDMI_MODE);
//				if (2 == hdmiMode) {
				if (mNavIntegration.isTimingTypeGraphic()) {	
					int frequency = mNavIntegration.iGetVedioFrequency();
					rString = "" + width + "X" + height + "@" + frequency
							+ "Hz";
					return rString;
				} else {
					return rString + pString + " " + dString;
				}
			} else {
				return rString + pString + " " + dString;
			}
		} else {
			if (mNavIntegration.isCurrentSourceTv()) {
				return mContext.getString(R.string.nav_resolution_null);
			} else {
				return "";
			}
		}
	}

	private String getCurTime() {
		mNavIntegration.syncTimeIfNeed();
		Date mDate = new Date(TVTimerManager.getInstance(mContext)
				.getBroadcastUTC());
		SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm");
		return dateFormat.format(mDate);
	}

	public boolean isNormalState() {
		TVChannel channel = mNavIntegration.iGetCurrentChannel();
		if (channel == null) {
			return false;
		}
		if (!mNavIntegration.iCurrentInputSourceHasSignal()
				|| !mNavIntegration.iCurrentChannelHasSignal()
				|| mNavIntegration.isCurrentSourceBlocking()
				|| mNavIntegration.isCurrentChannelBlocking()
				|| mNavIntegration.iGetChannelLength() <= 0
				|| mNavIntegration.isAudioScrambled()
				|| mNavIntegration.isVideoScrambled()
				|| mNavIntegration.isCurrentChannelRadio()
				|| mNavIntegration.vgaNotSupport()) {
			return false;
		}
		return true;
	}
	private Rect getRectInScreen(View v) {
		Rect r = new Rect();
		v.getGlobalVisibleRect(r);
		MtkLog.e(TAG,"rect :"+ r.top+"  "+" "+r.left+" "+r.bottom+" "+r.right);
		return r;
	}
	
	public void setIsOnKeyInfo(boolean bool){
		this.isOnkeyInfo = bool;
	}
}
