package com.mediatek.ui;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.AlarmManager;
import android.app.StatusBarManager;
import android.app.ActivityManagerNative;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.graphics.Point;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.provider.Settings;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.ViewTreeObserver.OnGlobalLayoutListener;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.Toast;

import com.mediatek.timeshift_pvr.controller.ScheduleItem;
import com.mediatek.timeshift_pvr.controller.StateDiskSetting;
import com.mediatek.timeshift_pvr.controller.StateNormal;
import com.mediatek.timeshift_pvr.controller.StateScheduleList;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.ui.cec.CecManager;
import com.mediatek.tv.model.CompListener;
import com.mediatek.tv.service.ComponentService;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVTeleTextManager;
import com.mediatek.tvcommon.TVCAMManager;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVCommonNative;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.menu.commonview.MTKPowerManager;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.menu.commonview.SleepTimerOff;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.OSDLanguage;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.mmp.MeidaMainActivity;
import com.mediatek.ui.mmp.model.MultiFilesManager;
import com.mediatek.ui.mmp.multimedia.MtkFilesListActivity;
import com.mediatek.ui.mmp.multimedia.VideoPlayActivity;
import com.mediatek.ui.mmp.util.Util;
import com.mediatek.ui.nav.CaptureLogoActivity;
import com.mediatek.ui.nav.EPGActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.NavIntegration.IChannelSelectorListener;
import com.mediatek.ui.nav.commonview.AdjustVolumeView;
import com.mediatek.ui.nav.commonview.BannerView;
import com.mediatek.ui.nav.commonview.InputPwdDialog;
import com.mediatek.ui.nav.commonview.NavSundryShowTextView;
import com.mediatek.ui.nav.commonview.ShowChannelListView;
import com.mediatek.ui.nav.commonview.ShowFavoriteChannelListView;
import com.mediatek.ui.nav.commonview.ShowSourceListView;
import com.mediatek.ui.nav.commonview.SnowTextView;
import com.mediatek.ui.nav.commonview.SnowTextView.ShowType;
import com.mediatek.ui.nav.commonview.ToastInfoView;
import com.mediatek.ui.nav.commonview.ZoomView;
import com.mediatek.ui.nav.util.CICardDelayNotification;
import com.mediatek.ui.nav.util.CheckLockSignalChannelState;
import com.mediatek.ui.nav.util.NavIntegrationZoom;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.nav.util.NewPipLogic;
import com.mediatek.ui.pippop.PippopState;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.setup.SetupWizardActivity;
import com.mediatek.timeshift_pvr.ui.OnPVRDialogListener;
import com.mediatek.timeshift_pvr.ui.PvrDialog;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.BypassWindowManager;
import com.mediatek.ui.util.GetCurrentTask;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.POSTManager;
import com.mediatek.ui.util.ScreenConstant;
import com.mediatek.ui.vssdemo.VssDemoActivity;
import com.mediatek.ui.util.DestroyApp;

import android.media.AudioManager;
import com.mediatek.ui.pippop.PippopMode_Normal;

import java.io.File;
import java.util.Date;
import java.util.List;
import android.graphics.RectF;

public class TurnkeyUiMainActivity extends Activity {
	private static final String TAG = "TurnkeyUiMainActivity";
	private static TurnkeyUiMainActivity mainActivity;
	private static NavIntegration mNavIntegration;
	private static BypassWindowManager mBypassWindowManager;
	public boolean mSetupFlag = false;
	private static ImageView mMuteImageView;
	private WindowManager wm;// manager whole screen, use to do mute
	private WindowManager.LayoutParams wmParams;

	private SaveValue saveV;
	private boolean bFirstInitial = true;
	private ToastInfoView mToastInfo;

	// CI
	private TVCAMManager mCAMManager;
	public static boolean mCIStatus = false;
	private boolean mJump = false;
	private TVContent mTV;
	private TVInputManager inpMgr = null;
	private TVConfigurer cfg = null;
	private TVOptionRange<Integer> optionRange = null;
	private TVOptionRange<Integer> optEarlyPlay = null;
	public static final String CISTATUS_REMOVE = "com.mediatek.ui.intent.CISTATUS_REMOVE";
	public static final String ACTION_NO_SIGNAL = "NO_SIGNAL";
	private CICardDelayNotification mCICardDelayNotification;
	// Channel
	private ShowChannelListView mShowChannelListView;
	// private ShowChannelTypeView mShowChannelTypeView;
	private ShowFavoriteChannelListView mShowFavoriteChannelListView;
	private short mSelectedChannelNum;

	private ShowSourceListView mShowSourceListView;
	private StringBuffer inputChannelNumStrBuffer = new StringBuffer();

	private boolean mNumputChangeChannel = false;
	// volume
	private static LinearLayout mAdjustVolLayout;
	private AdjustVolumeView adjustVolumeView;

	// timer sleep ...
	private static LinearLayout mSundryLayout;
	private NavSundryShowTextView mNavSundryShowTextView;

	private Animation mTipEnterAnimation;
	private int mOldShortTipType;
	// zoom view
	private ZoomView mZoomView;
	// no sign, please scan, input lock
	private SnowTextView mSpecial_SnowTextView;
	private InputPwdDialog mInputPwdView;
	public boolean mFirstShowInputPwdDialog = false;
	private CheckLockSignalChannelState mCheckLockSignalChannelState;
	private boolean mFirstTimeToSystem = true;
	private TVTeleTextManager tvTeleTextManager;
	private boolean mReturnFromThirdApp = false;

	private StatusBarManager mStatusBarManager;
	/* only for get audio forcus , close third app audio play */
	private AudioManager mAudioManager = null;

	private Rect mShowSourceListViewRect;
	private Rect mShowChannelListViewRect;
	private Rect mShowFavoriteChannelListViewRect;
	public static int mShowSourceListWindowId = -1;
	public static int mShowChannelListWindowId = -1;
	public static int mShowFavoriteChannelListWindowId = -1;
	private ProgressBar mpb;
	private Rect mSundryLayoutRect;
	private int mSundryLayoutWindowId = -1;
	private Boolean isSundryLayoutFirst = true;

	private static Rect mAdjustVolumeViewRect;
	private static int mAdjustVolumeViewWindowId;
	private Boolean isAdjustVolumeViewFirst = true;
	// receive mmp broadcast message, to resolve cr that cannot play tv back
	// from mmp.
	public static boolean isResume = false;
	public static boolean canOpenTv = false;
	private static final String PAUSE_ACTION = "com.mediatek.mmp.pause";
	// private VssDemo mGLView;
	private RelativeLayout OriginalView, OriginalLayout;
	private LinearLayout viewLayout;
	private int videoBlueMuteValue = 0;
	private boolean bOnVssDemo = false;

	private HandlerThread mFBMThread = null;
	private Handler mFBMHandler = null;

	private static PippopUiLogic mPippopUiLogic;

	private final static String OUT_PUT_SUB = "sub";
	private final static String OUT_PUT_MAIN = "main";
	private final static String INPUT_DTV = "dtv";
	private final static String INPUT_ATV = "atv";
	private boolean toNotPauseTV = false;
	private boolean keyUp = true;
	private boolean isShutdownFlow = false;
	private final static int PVR_TO_PLAY_VIDEO = 555;
	private boolean pvrResumeTV = false;
	
	BroadcastReceiver mmpBroadcastReceiver = new BroadcastReceiver() {
		public void onReceive(Context context, Intent intent) {
			Log.d(TAG,"mmpBroadcastReceiver enter isResume:" + isResume);
			if (isResume) {
//				NewPipLogic.getInstance(TurnkeyUiMainActivity.this)
//						.resumeMainOutput();
//				mPippopState = PippopState.getPippopStateInstance(TurnkeyUiMainActivity.this.getApplicationContext());
			}
		}
	};
	BroadcastReceiver selectSourceReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG,"selectSourceReceiver");
            if ("1".equals(SystemProperties.get("mtk.auto_test"))) {
                String action = intent.getAction();
                String sourcename =action.substring(action.lastIndexOf(".")+1);
                if(inpMgr!= null && inpMgr.getCurrInputSource()!=null && !sourcename.equalsIgnoreCase(inpMgr.getCurrInputSource())){
                    inpMgr.changeInputSource(mPippopUiLogic.getOutputFocus(),sourcename);
                }
            }
        }
    };
    
    BroadcastReceiver tunnerModeReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG,"tunnerModeReceiver enter");
            if ("1".equals(SystemProperties.get("mtk.auto_test"))) {
                String action = intent.getAction();
                String tunnermode = action.substring(action.lastIndexOf(".")+1);
				try {
					ITVCommon mCommon = TVCommonNative.getDefault(TurnkeyUiMainActivity.this);
					if(null != mCommon) {
						if (tunnermode.equals("air")) {
							mCommon.setTunnerMode(0);
							Log.d(TAG, "setTunnerMode(0)");
						} else if (tunnermode.equals("cable")) {
							mCommon.setTunnerMode(1);
							Log.d(TAG, "setTunnerMode(1)");
						}
					} else {
						Log.d(TAG, "null == mCommon");
					}
				} catch (RemoteException e) {
					e.printStackTrace();
				}
			} else {
				Log.d(TAG, "0 == mtk.auto_test");
			}
		}
	};

	BroadcastReceiver tkBroadcastReceiver = new BroadcastReceiver() {
		public void onReceive(Context context, Intent intent) {
			MtkLog.d(TAG,"come in tkBroadcastReceiver  == " + intent.getAction());
			if (Intent.ACTION_PREPARE_SHUTDOWN.equals(intent.getAction())){
				Log.i(TAG, "prepare shutdown in main activity!");
                isShutdownFlow = true;
				SystemProperties.set("debug.mtk.tkui.suspend_mmp", "0");
				GetCurrentTask curTask = GetCurrentTask.getInstance(TurnkeyUiMainActivity.this);
				if(curTask.isCurActivtyMeidaMainActivity()){
					DestroyApp destroyApp = (DestroyApp) getApplicationContext();
					destroyApp.finishAll();
				}
				if(MultiFilesManager.hasInstance()){
					MultiFilesManager mFilesManager=MultiFilesManager.getInstance(TurnkeyUiMainActivity.this);
					mFilesManager.deleteObservers();
					mFilesManager.destroy();
				}
				SystemProperties.set("debug.mtk.tkui.suspend_mmp", "1");				
				MtkLog.d(TAG, "SystemProperties.set(debug.mtk.tkui.suspend_mmp)=1.");
			}else if (Intent.ACTION_STANDBY.equals(intent.getAction())) {
				Log.i(TAG, "standby TV in main activity!");
				if (mToastInfo != null) {
					mToastInfo.cancelVGAInfo();
				}
				hideSundryLayout();
				if (null != mTV) {
					mTV.setAudioMute(false);
				}
				if (getTshiftManager()!=null) {
	                getTshiftManager().stopAllRunning();
	            }
				MtkFilesListActivity.removeView();
			}else if(Intent.ACTION_SCREEN_ON.equals(intent.getAction())){
				//Log.i(TAG, "screen on TV in main activity! =="+mPippopUiLogic.isPipOrPopState());
				if(null!=CecManager.getInstance()){
					MtkLog.e(TAG," CecManager Screen on......");
					CecManager.getInstance().updateSystemPanelOn();
				}
				if(null != mPippopUiLogic && mPippopUiLogic.isPipOrPopState()){
					if(null != mPippopState){
						//Log.i(TAG, "screen on TV in main activity! mPippopState != null");
						mPippopState.getmController().hiddenAllTVViews();
						mPippopState.getmController().releaseFocus();
						mPippopState.setState(new PippopMode_Normal(mPippopState));
					}
					NewPipLogic.getInstance(TurnkeyUiMainActivity.this)
						.resumeMainOutput();
					canOpenTv = true;
					if(mpb != null) {
						mpb.setVisibility(View.GONE);
					}
				}	
			}else if(Intent.ACTION_CLOSE_SYSTEM_DIALOGS.equals(intent.getAction())){
				MtkLog.d(TAG,"come in ACTION_CLOSE_SYTEM_DIALOGS");
				if(GetCurrentTask.getInstance(TurnkeyUiMainActivity.this).isCurTaskTKUI()){
					MtkLog.d(TAG,"come in ACTION_CLOSE_SYTEM_DIALOGS toNotPauseTV = true");
					toNotPauseTV = true;
				}
			}
		}
	};
	
	private final int START_VIDEO_PLAYER =0xF001;	//For PVR Play
	
	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case MessageType.SHOW_NO_TTX:
				Log.d("handler", "show no ttx");
				Toast.makeText(TurnkeyUiMainActivity.this, R.string.menu_teletext_notsupport_tip, 1000).show();
				break;
			case SnowTextView.DRAW_TEXT:
				if (mSpecial_SnowTextView != null) {
					mSpecial_SnowTextView.invalidate();
				}
				break;
			case MessageType.NAV_ADUST_VOLUME_DIMISS:
				unlockAdjustVolLayoutRect();
				if(!mPippopUiLogic.isPipOrPopState()){
				mCheckLockSignalChannelState
						.checkLockedSignStateOrHasChannel(false);
				}
				break;
			case MessageType.NAV_NUMKEY_CHANGE_CHANNEL:
				inputChannelNumStrBuffer = new StringBuffer();
				mNumputChangeChannel = false;
				
				//Fix CR:DTV00598382
				if (TimeShiftManager.getInstance().pvrIsRecording()&&mNavIntegration.hasSelectChannel(mSelectedChannelNum)) {
					PvrDialog conDialog = new PvrDialog(TimeShiftManager.getInstance()
							.getActivity(), PvrDialog.TYPE_Confirm,
							PvrDialog.KEYCODE_FROM_FAV, PvrDialog.TYPE_Record);
					conDialog.setOnPVRDialogListener(new OnPVRDialogListener(){
						@Override
						public void onPVRDialogListener(int keyCode) {
							// TODO Auto-generated method stub
							
							if (mNavIntegration.iSelectChannel(mSelectedChannelNum)) {
								mNavIntegration.iSetSourcetoTv();
							} else {
								mBannerView.show(false, -1, false);
							}
						}
					});
					conDialog.show();
					return;
				}else
				{
					if (mNavIntegration.iSelectChannel(mSelectedChannelNum)) {
						mNavIntegration.iSetSourcetoTv();
					} else {
						mBannerView.show(false, -1, false);
					}	
				}
				break;
			case MessageType.NAV_BANNERVIEW_DIMISS:
				mBannerView.hideAllBanner();
				if (!mShowSourceListView.isShowing()
						&& !mShowFavoriteChannelListView.isShowing()
						&& !mShowChannelListView.isShowing()) {
					if (msg.arg1 == BannerView.SPECIAL_NO_CHANNEL
							|| msg.arg1 == BannerView.SPECIAL_NO_SIGNAL 
							|| msg.arg1 == BannerView.SPECIAL_NO_SUPPORT) {					
						if(!mPippopUiLogic.isPipOrPopState()){
						setSnowTextTagByState(msg.arg1);
						hideAllOnBannerDismiss();
						mSpecial_SnowTextView.setVisibility(View.VISIBLE);
						}
					} else {
						if (!mBannerView.isNormalState()
								&& !mInputPwdView.isShowing()) {
							if(!mPippopUiLogic.isPipOrPopState()){
							mCheckLockSignalChannelState
									.checkLockedSignStateOrHasChannel(false);
							}
						}
					}
				}
				break;
			case MessageType.NAV_INPUTPWDVIEW_DIMISS:
				mInputPwdView.dismiss();
				if (msg.arg1 == MessageType.NAV_CURRENT_SOURCE_LOCKED) {
					mSpecial_SnowTextView
							.showSpecialView(ShowType.SPECIAL_INPUT_LOCKED);
					return;
				}
				if (msg.arg1 == MessageType.NAV_CURRENT_CHANNEL_LOCKED) {
					mSpecial_SnowTextView
							.showSpecialView(ShowType.SPECIAL_CHANNEL_LOCKED);
					return;
				}
				break;
			case MessageType.NAV_CURRENT_SOURCE_LOCKED:
			case MessageType.NAV_CURRENT_CHANNEL_LOCKED:
				// mSpecial_SnowTextView.setVisibility(View.GONE);
				mBannerView.show(false, -1, false);
				if (mShowSourceListView.isShowing() == false) {
					hideChannelList();
					hideTypeList();
					hideFavoriteList();
					hideZoomView();
					hideAdustVolLayout();
					hideSundryLayout();
					hideInputPwdView();
					mInputPwdView.show();
				} else {
					mFirstShowInputPwdDialog = true;
				}

				break;
			case MessageType.NAV_SHOW_CURRENT_CHANNEL_INFO:
				mBannerView.showSimpleBar();
				mHandler.sendEmptyMessageDelayed(
						MessageType.NAV_SHOW_CHANNEL_INFO,
						MessageType.delayMillis5);
				break;

			case MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS:
				unlockSundryLayoutRect();
				if(!mPippopUiLogic.isPipOrPopState()){
				mCheckLockSignalChannelState
						.checkLockedSignStateOrHasChannel(false);
				}
				break;
			case MessageType.NAV_ZOOMVIEW_DIMISS:
				mZoomView.setVisibility(View.GONE);
				break;
			case MessageType.FORM_TK_TO_MENUMAIN:
				Bundle bundle = new Bundle();
				bundle.putInt("CIValue", 2);
				Intent intent = new Intent();
				intent.putExtras(bundle);
				intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
				intent.setClass(TurnkeyUiMainActivity.this, MenuMain.class);
				mNavIntegration.setIsTKMainActivity(false);
				startActivity(intent);
				break;

			case MessageType.FBM_MODE_SWITCH_OK:
				MtkLog.i(TAG,"i have got FBM_MODE_SWITCH_OK message...");

				NewPipLogic pipLogic = NewPipLogic.getInstance(TurnkeyUiMainActivity.this);
				if(!VideoPlayActivity.video_player_Activity_resumed){
					pipLogic.resumeMainOutput();
				}
				
				if(mpb != null)				
					mpb.setVisibility(View.GONE);
				canOpenTv = true;
				break;
			case MessageType.VIDEO_BLUE_MUTE_REVERT:
				if (videoBlueMuteValue == 1) {
					mTV.setVideoBlueMute(true);
					optionRange.set(videoBlueMuteValue);
				}
				break;
			case MessageType.MESSAGE_SETOPACITY:
				if(null != viewLayout){
						viewLayout.setVisibility(View.GONE);
				}
				if(GetCurrentTask.getInstance(TurnkeyUiMainActivity.this).isCurActivityTkuiMainActivity()){
					TVContent.getInstance(TurnkeyUiMainActivity.this).setOpacity(200);// set OSD opacity

				}
				
				break;
				
			case START_VIDEO_PLAYER:
				String path=msg.getData().getString("PATH");
				MtkLog.d(TAG, "StartVideoPlayer:"+path);
				startVideoPlayer(path);
				
				break;
			default:
				break;
			}
		}

	};
	private BannerView mBannerView;
	private static PippopState mPippopState;
	private TimeShiftManager mTshiftManager;

	public final static int RESULT_CODE_SHOW_DEVICE_INFO_FROM_MENU = 0x123;
	public final static int RESULT_CODE_SHOW_SCHEDULE_LIST_FROM_MENU = 0x1234;
	public final static int RESULT_CODE_ADD_SCHEDULE_ITEM_FROM_EPG = 0x12345;
	public final static int RESULT_CODE_CALL_START_PVR = 0x123456;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		Point outSize = new Point();
		getWindow().getWindowManager().getDefaultDisplay().getRealSize(outSize);
		ScreenConstant.SCREEN_WIDTH = outSize.x;
		ScreenConstant.SCREEN_HEIGHT = outSize.y;
		mNavIntegration = NavIntegration.getInstance(this);
		mBypassWindowManager = BypassWindowManager.getInstance(this);
		AnimationManager.getInstance();
		mTV = TVContent.getInstance(this);
		inpMgr = mTV.getInputManager();
		cfg = mTV.getConfigurer();
		optionRange = (TVOptionRange<Integer>)(cfg.getOption(MenuConfigManager.BLUE_MUTE));
		optEarlyPlay = (TVOptionRange<Integer>)(cfg.getOption(ConfigType.CFG_EARLY_PLAY));
		saveV = SaveValue.getInstance(this);
		tvTeleTextManager=mTV.getTeleTextManager();
		tvTeleTextManager.registerNotifyLisenter(myCompListener);
		
        if (saveV.readValue("power_off_timer") == 1) {
			saveV.saveValue("power_off_timer", 0);
			if (saveV.readValue(MenuConfigManager.POWER_OFF_TIMER) == 1) {
				saveV.saveValue(MenuConfigManager.POWER_OFF_TIMER, 0);
				MTKPowerManager mMTKPowerManager = MTKPowerManager
						.getInstance(TurnkeyUiMainActivity.this);
				mMTKPowerManager
						.cancelPowOffTimer(MenuConfigManager.POWER_OFF_TIMER);
            }
        }
        int reason = MenuConfigManager.getInstance(getApplicationContext()).getDefault(MenuConfigManager.WAKEUPREASON);
        Log.e(TAG,"reason"+reason);
        if (reason == ConfigType.WAKE_UP_REASON_RTC) {
            if (saveV.readValue(MenuConfigManager.POWER_ON_TIMER) == 1) {
//                saveV.saveValue(MenuConfigManager.POWER_ON_TIMER, 0);
                MTKPowerManager mMTKPowerManager = MTKPowerManager
                        .getInstance(TurnkeyUiMainActivity.this);
                mMTKPowerManager
                        .cancelPowOnTimer();
            }
        }
        mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
		
		SystemProperties.set("sys.pm.warmboot",saveV.readValue(MenuConfigManager.FAST_BOOT)+"");
		OSDLanguage osdLanguage= new OSDLanguage(this.getApplicationContext());
		MenuConfigManager.getInstance(getApplicationContext()).autoTestCheck();
		try{
		osdLanguage.setOSDLanguage(saveV.readValue(MenuConfigManager.OSD_LANGUAGE));
	}	catch(Exception e){
		Log.e(TAG,"tk oncreate setosd lan exception"+e);
		}
		initMuteImageView();
		mStatusBarManager = (StatusBarManager) getSystemService(Context.STATUS_BAR_SERVICE);

		// set auto sync time to get correct time when system booted
		mNavIntegration.setAutoSync();
		if (mNavIntegration.isStartSetupwizard()) {
			mSetupFlag = true;
			mJump = true;
			Intent intent = new Intent(this, SetupWizardActivity.class);
			intent.setFlags(Intent.FLAG_ACTIVITY_NO_HISTORY);
			// startActivity(intent);
			toNotPauseTV = true;
			mNavIntegration.setIsTKMainActivity(false);
			startActivityForResult(intent, 200);
		}
		registerSelectSource();
		registerTunnerMode();
		registerMmpReceiver();
		IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_STANDBY);
		filter.addAction(Intent.ACTION_SCREEN_ON);
		filter.addAction(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);
		filter.addAction(Intent.ACTION_PREPARE_SHUTDOWN);
		filter.setPriority(IntentFilter.SYSTEM_LOW_PRIORITY);
		this.registerReceiver(tkBroadcastReceiver, filter);
		
		if("1".equals(SystemProperties.get("ro.mtk.system.switchfbm"))) {
			if (mFBMThread == null) {
				mFBMThread = new HandlerThread("FBM");	
				mFBMThread.start();			
				mFBMHandler = new Handler(mFBMThread.getLooper()) {
					public void handleMessage(Message msg) {
						super.handleMessage(msg);
						switch (msg.what) {
							case 1:
							{
								MtkLog.i(TAG, "Turn Off FBM Mode.");
								mNavIntegration.setFbmMode(1);
								MtkLog.i(TAG, "switch OK.");	
						
								mHandler.removeMessages(MessageType.FBM_MODE_SWITCH_OK);
								mHandler.sendEmptyMessage(MessageType.FBM_MODE_SWITCH_OK);
								
								break;
							}
							case 2:
							{
								MtkLog.i(TAG, "Turn On FBM Mode.");
								mNavIntegration.setFbmMode(2);	
								break;
							}
							default:
								break;
						}
						
					}
				};												
			}						
		}
				
		mPippopState = PippopState.getPippopStateInstance(this);
		mPippopUiLogic = PippopUiLogic.getInstance(this);
		setTshiftManager(TimeShiftManager.getInstance(this));
        CecManager.getInstance(this);
        registShutdownReceive();
	}

	private void registerMmpReceiver() {
		IntentFilter filter = new IntentFilter();
		filter.addAction("com.mediatek.closeVideo");
		filter.addAction("com.mediatek.closeAudio");
		this.registerReceiver(mmpBroadcastReceiver, filter);
	}
    private void registerSelectSource() {
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.mediatek.select.dtv");
        filter.addAction("com.mediatek.select.atv");
        filter.addAction("com.mediatek.select.av0");
        filter.addAction("com.mediatek.select.component0");
        filter.addAction("com.mediatek.select.hdmi0");
        filter.addAction("com.mediatek.select.hdmi1");
        filter.addAction("com.mediatek.select.hdmi2");
        filter.addAction("com.mediatek.select.hdmi3");
        filter.addAction("com.mediatek.select.vga0");
        this.registerReceiver(selectSourceReceiver, filter);
    }
    
    private void registerTunnerMode() {
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.mediatek.tunnermode.air");
        filter.addAction("com.mediatek.tunnermode.cable");
        this.registerReceiver(tunnerModeReceiver, filter);
    }
    
	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		startSchedulePvr(intent);
	}

	/**
	 * @param intent
	 */
	private void startSchedulePvr(Intent intent) {
		int taskID = intent.getIntExtra(Core.ALARM_ACTION_TAG, -1);
		
		Log.d("TimeShift_PVR", "onNewIntent(),TaskID: "+taskID);
		
		if (taskID != -1) {
			ScheduleItem item = com.mediatek.timeshift_pvr.manager.Util
					.getScheduleItem(getApplicationContext(), taskID);
			if(item!=null){
				item.showDebugInfo();
			}
			if(!(getTshiftManager().getState() instanceof StateNormal)){
				getTshiftManager().restoreAllToNormal();
			}
			getTshiftManager().startSchedulePvrTask(item);
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == 200) {
			mSetupFlag = false;
		}
		if(requestCode == PVR_TO_PLAY_VIDEO){
			pvrResumeTV = true;
		}
		if(requestCode == 0){
			if(data != null && data.getBooleanExtra("FROM_MENUMAIN_TO_TK", false)){
				mHandler.sendEmptyMessageDelayed(MessageType.FORM_TK_TO_MENUMAIN, MessageType.delayForTKToMenu);
			}
		}
		if (resultCode == RESULT_CODE_SHOW_DEVICE_INFO_FROM_MENU) {
			getTshiftManager().setState(
					new StateDiskSetting(this, getTshiftManager()));
		} else if (resultCode == RESULT_CODE_SHOW_SCHEDULE_LIST_FROM_MENU) {
			getTshiftManager().setState(
					new StateScheduleList(this, getTshiftManager()));
		} else if (resultCode == RESULT_CODE_ADD_SCHEDULE_ITEM_FROM_EPG) {

			ScheduleItem item = new ScheduleItem();
			item.setChannelName("-1");
			
			long startTime = data.getLongExtra(ScheduleItem.START_TIME, -1L);
			if (startTime != -1L) {
				item.setStartTime(new Date(startTime));
			}

			long endTime = data.getLongExtra(ScheduleItem.END_TIME, -1L);
			if (endTime != -1L) {
				item.setStartTime(new Date(endTime));
			}

			item.setNewItem(true);
			getTshiftManager().setState(
					new StateScheduleList(this, getTshiftManager(), item));
		}
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	protected void onStart() {
		NavIntegration.setColorKey(false);
		TVContent.getInstance(this).setOpacity(200);// set OSD opacity
		super.onStart();
	}

	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		/*
		 * if(null != optEarlyPlay){
		 * if(!mNavIntegration.isCurrentSourceBlocking(
		 * )&&(mNavIntegration.isCurrentSourceTv
		 * ()&&mNavIntegration.iGetChannelLength()>0)
		 * &&!mNavIntegration.isCurrentChannelBlocked()){
		 * optEarlyPlay.set(ConfigType.EARLY_PLAY_ENABLED); }else{
		 * optEarlyPlay.set(ConfigType.EARLY_PLAY_DISENABLED); } }
		 */
		// if(!mPippopUiLogic.isPipOrPopState()){
		// mPippopState.getmController().releaseFocus();
		// }
		MtkLog.d(TAG, "onResume()");
		if((null != viewLayout) && (View.VISIBLE == viewLayout.getVisibility())){
			viewLayout.setVisibility(View.GONE);
		}
		
        mAudioManager.requestAudioFocus(null, AudioManager.STREAM_MUSIC, AudioManager.AUDIOFOCUS_GAIN);
		
		TVContent.getInstance(this).setOpacity(200);// set OSD opacity
		MenuConfigManager.getInstance(this).setBackLightToTV();

		if(isShutdownFlow){
            isShutdownFlow = false;
			if(MultiFilesManager.hasInstance()){
				MultiFilesManager mFilesManager=MultiFilesManager.getInstance(this);
				mFilesManager.deleteObservers();
				mFilesManager.destroy();
				MtkLog.i(TAG,"mmp resource destroyed.. onResume");
				return;
			}
        }

		MtkLog.i(TAG,"~~~~~~~~~~~~~~~~~~~~~~~enter onResume");
		Intent pauseIntent = new Intent(PAUSE_ACTION);
		sendBroadcast(pauseIntent);
		mStatusBarManager.setSystemUiBarHight(0);

		isResume = true;
	//	TVContent.getInstance(this).setOpacity(200);// set OSD opacity
		if (mNavIntegration.isMute()) {
			mMuteImageView.setVisibility(View.VISIBLE);
		}

		// setup
		if (mSetupFlag == false && mFirstTimeToSystem == true) {
			initCheckPowTimer();
			LayoutInflater inflater = (LayoutInflater) this
					.getSystemService(this.LAYOUT_INFLATER_SERVICE);
			OriginalView = (RelativeLayout) inflater.inflate(
					R.layout.nav_main_layout_test, null);
			OriginalLayout = (RelativeLayout) OriginalView
					.findViewById(R.id.linear_glview);
			viewLayout = (LinearLayout) OriginalView
					.findViewById(R.id.nav_viewlayout);		
			setContentView(OriginalLayout);
			init();
			mpb = (ProgressBar)findViewById(R.id.fbm_mode_progressbar);
			
			mToastInfo = ToastInfoView.getInstance(this);
			mToastInfo.setmSnowTextView(mSpecial_SnowTextView);
			setTimeZone();
		}
		if (mSetupFlag == false) {
			mNavIntegration.setIsTKMainActivity(true);
			toNotPauseTV = false;
			if (mFirstTimeToSystem == true) {
				if (mNavIntegration.isCurrentSourceTv()
						&& mNavIntegration.iGetChannelLength() > 0) {
					mHandler.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);
					mBannerView.showSimpleBar();
					mHandler.sendEmptyMessageDelayed(
							MessageType.NAV_BANNERVIEW_DIMISS,
							MessageType.delayMillis4);
					mHandler.postDelayed(new Runnable() {
						public void run() {
							registeListeners();
							if (!mNavIntegration.isCurrentSourceBlocked()) {
								mHandler
										.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);
								if (mNavIntegration.isCurrentSourceTv()) {
									MtkLog.d(TAG, "come in onResume,show basic bar");
									mBannerView.showBasicBar();
									mBannerView
											.setBannerState(BannerView.CHILD_TYPE_INFO_BASIC);
								} else {
									mBannerView.showSimpleBar();
								}
								
								mHandler.sendEmptyMessageDelayed(
										MessageType.NAV_BANNERVIEW_DIMISS,
										MessageType.delayMillis4);
							}
							if(!mPippopUiLogic.isPipOrPopState()){
							mCheckLockSignalChannelState
									.checkLockedSignStateOrHasChannel(true);
							}
						}
					}, 1000);
				} else {
					if(!mNavIntegration.isCurrentSourceTv()){
						mBannerView.showSimpleBar();
					}
					if (!mPippopUiLogic.isPipOrPopState()) {
						mCheckLockSignalChannelState
								.checkLockedSignStateOrHasChannel(true);
					}
					registeListeners();
				}

				mFirstTimeToSystem = false;
			} else {
				registeListeners();
				if (mCheckLockSignalChannelState
						.checkLockedSignStateOrHasChannel(true) == false) {
					mBannerView.show(false, BannerView.CHILD_TYPE_INFO_SIMPLE,
							false);
				}
			}
		}
		if (!mPippopUiLogic.isPipOrPopState()) {
			if (!NewPipLogic.isTvNormal()) {
				if (mpb != null){
					mpb.setVisibility(View.VISIBLE);
				}
				if ("1".equals(SystemProperties.get("ro.mtk.system.switchfbm"))) {
					if (mFBMHandler.hasMessages(1) == false) {
						mFBMHandler.sendEmptyMessage(1);
					} else {
						MtkLog.i(TAG, "mFBMHandler Queue has message 1!");
					}
				} else {
					mHandler.removeMessages(MessageType.FBM_MODE_SWITCH_OK);
					mHandler.sendEmptyMessage(MessageType.FBM_MODE_SWITCH_OK);
			}
								
		} else {
				/*
				 * //if
				 * ("2".equals(SystemProperties.get("sys.pm.warmboot.flag"))) {
				 * NewPipLogic pipLogic =
				 * NewPipLogic.getInstance(TurnkeyUiMainActivity.this);
				 * pipLogic.resumeMainOutput(); //} if(mpb != null) {
				 * mpb.setVisibility(View.GONE); }
				 */

				// ?? by YT,fix canOpenTv always equal false after return from
				// videoPlayActivity.
				if(pvrResumeTV){
					mHandler.removeMessages(MessageType.FBM_MODE_SWITCH_OK);
					mHandler.sendEmptyMessage(MessageType.FBM_MODE_SWITCH_OK);
					pvrResumeTV = false;
				}else{
					//exit after when enter menu/epg,not to resume tv
					if (MenuMain.getChangeFreq()) {// change freq in menu,the current channel will mute,so select channel again.  related CR:DTV00608457
						MtkLog.d(TAG, "hasChangeFreq true");
						MenuMain.setChangeFreq(false);
						TVChannel mCurrentChannel = mNavIntegration.iGetCurrentChannel();
						if (mCurrentChannel != null) {
							ITVCommon mTVCommon = TVCommonNative.getDefault(TurnkeyUiMainActivity.this);
							try {
								mTVCommon.select(mCurrentChannel);
							} catch (RemoteException e) {
								e.printStackTrace();
							}
							return;
						}
					}
					mHandler.postDelayed(new Runnable() {
						public void run() {
							mBannerView.show(false, -1, false);
						}
					}, 1000);	
					boolean channelBlocked = mNavIntegration
							.isCurrentChannelBlocked();
					boolean sourceBlocked = mNavIntegration
							.isCurrentSourceBlocked();
					if (mNavIntegration.isCurrentSourceTv() && !sourceBlocked
							&& channelBlocked) {
						MtkLog.d(TAG, "onResume,show password dialog when channel blocked");
						mHandler.removeMessages(MessageType.NAV_CURRENT_CHANNEL_LOCKED);
						mHandler.sendEmptyMessage(MessageType.NAV_CURRENT_CHANNEL_LOCKED);
					}
					
				}	
			}

		}else{
			if (NewPipLogic.isTvNormal()) {
				//exit after when enter menu/epg,not to resume tv
				mHandler.postDelayed(new Runnable() {
					public void run() {
						mBannerView.show(false, -1, false);
					}
				}, 1000);	
			}
		}
		//when enter TV, stop music playing in 3rd APP
		Intent mIntent = new Intent(Intent.ACTION_MEDIA_BUTTON, null);
		mIntent.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(
				KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_MTKIR_STOP));
		sendOrderedBroadcast(mIntent,null);
		mIntent.putExtra(Intent.EXTRA_KEY_EVENT, new KeyEvent(
				KeyEvent.ACTION_UP, KeyEvent.KEYCODE_MTKIR_STOP));
		sendOrderedBroadcast(mIntent,null);
		mPippopState.onResume();
	
//		mHandler.sendEmptyMessageDelayed(MessageType.MESSAGE_SETOPACITY,300);
		
		getTshiftManager().onResume();
	}
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		MtkLog.i("onConfigurationChanged", "TURNKEY");
		if(mInputPwdView!=null){
			mInputPwdView.updateConfigration();
			MtkLog.i("onConfigurationChanged", "TURNKEY");
		}
		super.onConfigurationChanged(newConfig);
	}

	@Override
	protected void onPause(){
		super.onPause();
		MtkLog.d(TAG, "onPause()");
		
		if(isShutdownFlow){
            isShutdownFlow = false;
        }
		unregisteListeners();
		//TVContent.getInstance(this).setOpacity(255);
		MtkLog.i("fff","~~~~~~~~~~~~~~~~~~~~~~~enter onPause");
		/*if(null != optEarlyPlay){
			if(!mNavIntegration.isCurrentSourceBlocking()&&(mNavIntegration.isCurrentSourceTv()&&mNavIntegration.iGetChannelLength()>0)
				&&!mNavIntegration.isCurrentChannelBlocked()){
				optEarlyPlay.set(ConfigType.EARLY_PLAY_ENABLED);
			}else{
				optEarlyPlay.set(ConfigType.EARLY_PLAY_DISENABLED);
			}
		}*/
        //for [KK]
		GetCurrentTask ctask = GetCurrentTask.getInstance(this);
		NewPipLogic pipLogic = NewPipLogic.getInstance(this);

		//if (!ctask.isCurTaskTKUI()) {	
			//MtkLog.i("fff","~~~~~~~~~~~~~~~~~~~~~~~not tkui");
			if(!toNotPauseTV){
				mAudioManager.abandonAudioFocus(null);
				isResume = false;
				if(!VideoPlayActivity.video_player_Activity_resumed){
					MtkLog.d(TAG, "onPause(),hasStartVideoPlayer?"+VideoPlayActivity.video_player_Activity_resumed);
					pipLogic.pauseMainOutput();
				}
				
				mStatusBarManager.setSystemUiBarHight(getApplicationContext().getResources().getDimensionPixelSize(com.android.internal.R.dimen.navigation_bar_height));
				canOpenTv = false;
				TVContent.getInstance(this).setOpacity(255);
			
                mReturnFromThirdApp = true;
            NavIntegration.setColorKey(false);
                setMuteGone();
                mHandler.removeMessages(MessageType.MESSAGE_SETOPACITY);
                if("1".equals(SystemProperties.get("ro.mtk.system.switchfbm"))) {
                    if (mFBMHandler.hasMessages(2) == false) {
                        mFBMHandler.sendEmptyMessage(2);
                    } else {
                        MtkLog.i(TAG,"mFBMHandler Queue has message 2!");
                    }
                }
			
			}
		//}
		toNotPauseTV = false;
	
		mPippopState.onPause();
		getTshiftManager().onPause();
	}
	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
        // for [KK]
        /*GetCurrentTask ctask = GetCurrentTask.getInstance(this);
        NewPipLogic pipLogic = NewPipLogic.getInstance(this);

        if (!ctask.isCurTaskTKUI()) {   
            MtkLog.i("fff","~~~~~~~~~~~~~~~~~~~~~~~not tkui");
            isResume = false;
            if(!toNotPauseTV){
                pipLogic.pauseMainOutput();
                mStatusBarManager.setSystemUiBarHight(getApplicationContext().getResources().getDimensionPixelSize(com.android.internal.R.dimen.navigation_bar_height));
                canOpenTv = false;
                TVContent.getInstance(this).setOpacity(255);
            
                mReturnFromThirdApp = true;
                NavIntegration.setColorKey(false);
                setMuteGone();
                mHandler.removeMessages(MessageType.MESSAGE_SETOPACITY);
                if("1".equals(SystemProperties.get("ro.mtk.system.switchfbm"))) {
                    if (mFBMHandler.hasMessages(2) == false) {
                        mFBMHandler.sendEmptyMessage(2);
                    } else {
                        MtkLog.i(TAG,"mFBMHandler Queue has message 2!");
                    }
                }
            
            }
        }
        toNotPauseTV = false;*/
        //for [KK] end
		getTshiftManager().onStop();
	}

	public void onDestroy() {
		MtkLog.d(TAG,"come in onDestroy()");
		super.onDestroy();
		if (wm != null) {
			wm.removeView(mMuteImageView);
		}
		tvTeleTextManager.removeNotifyLisenter(myCompListener);
		unregisterReceiver(mmpBroadcastReceiver);
		unregisterReceiver(tkBroadcastReceiver);
		if (mNavSundryShowTextView != null) {
			mNavSundryShowTextView.unregisterReceiver();
		}
	}

	private boolean isActive=false;
	
	CompListener myCompListener=new CompListener() {
		
		public void CompNotifyInfo(String MsgContent) {
			Log.d("TKUIMAIN TTX", "MsgContent:"+MsgContent);
			if(ComponentService.COMP_TTX_SHOW_NO_TELETEXT.equals(MsgContent)){
				Log.d("TKUIMAIN TTX", "show no ttx notify"+isActive);
					isActive=false;
					tvTeleTextManager.inActivateCompoent(ComponentService.COMP_TTX_NAME);
					mHandler.sendEmptyMessage(MessageType.SHOW_NO_TTX);
			}else if(ComponentService.COMP_TTX_ACTIVATE.equals(MsgContent)){
				isActive=true;
			}else if(ComponentService.COMP_TTX_COMP_INACTIVATE.equals(MsgContent)){
				isActive=false;
			}else if(ComponentService.COMP_TTX_AVAIL.equals(MsgContent)){
				Log.d("TKUIMAIN TTX", "active notify COMP_TTX_AVAIL");
				tvTeleTextManager.setTTXIconShow(true);
			}else if(ComponentService.COMP_TTX_NOT_AVAIL.equals(MsgContent)){
				Log.d("TKUIMAIN TTX", "inactive notify COMP_TTX_NOT_AVAIL");
				tvTeleTextManager.setTTXIconShow(false);
			}
		}
	};

	private boolean sendkeyTTXEvent(int keyCode){
		if(keyCode==KeyMap.KEYCODE_MTKIR_MUTE||keyCode==KeyMap.KEYCODE_MTKIR_CHDN||keyCode==KeyMap.KEYCODE_MTKIR_CHUP){
			return false;
		}
		return true;
	}
	
	
	private boolean isATV(){
		if(inpMgr!= null && inpMgr.getCurrInputSource()!=null && "atv".equals(inpMgr.getCurrInputSource())){
			return true;
		}
		return false;
	}
	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		MtkLog.d(TAG, "come in onKeyUp === " + keyCode + "event.getAction ==="
				+ event.getAction());
		if(KeyMap.KEYCODE_MTKIR_PIPPOP == keyCode){
			keyUp = true;
		}
		return super.onKeyUp(keyCode, event);
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		MtkLog.d(TAG, "come in onKeyDown === " + keyCode);
		//MtkLog.d(TAG, "come in onKeyDown ===,event.getRepeatCount()= " + event.getRepeatCount());
		if (mPippopUiLogic.getCurrentMode() == ITVCommon.OUTPUT_MODE_MMP) {
			return true;
		}

        if (CecManager.getInstance() != null) {
            boolean result = CecManager.getInstance().sendKeyEvent(event.getKeyCode(), ComponentService.KEY_EVENT_DOWN);
            if(true == result){
                return true;
            }
        }
		
		if(isActive&&sendkeyTTXEvent(keyCode)&&isATV()){
			
				Log.d("TKUIMAIN TTX", "send key to ttx !"+keyCode);
				tvTeleTextManager.sendkeyEvent(keyCode, ComponentService.KEY_EVENT_DOWN);
			return true;
		}

                if (canOpenTv == false){
			MtkLog.i(TAG,"TV not switch fbm mode ok, waiting.......");
			return false;
		}

		mToastInfo.toastVGAInfo();
		if (saveV.readValue(MenuConfigManager.AUTO_SLEEP) != 0) {
			SleepTimerOff sleepTimerOff = new SleepTimerOff(this);
			sleepTimerOff.shutDownAuto(saveV
					.readValue(MenuConfigManager.AUTO_SLEEP));
		}
		if (keyCode != KeyMap.KEYCODE_MENU
				&& keyCode != KeyMap.KEYCODE_MTKIR_ANGLE
				&& keyCode != KeyMap.KEYCODE_DPAD_LEFT
				&& keyCode != KeyMap.KEYCODE_DPAD_RIGHT
				&& keyCode != KeyMap.KEYCODE_MTKIR_PIPPOS
				&& keyCode != KeyMap.KEYCODE_MTKIR_PIPSIZE
				&& keyCode != KeyEvent.KEYCODE_MTKIR_SWAP
				&& keyCode != KeyMap.KEYCODE_MTKIR_PIPPOP) {
			reshowFoucs();
		}
        // For TimeShift&PVR @2012.12.07
        Log.d("MTKTKUI", "timeshift pvr dispatch keycode");
        if (getTshiftManager().onKeyDown(keyCode)) {
            return true;
        }
		switch (keyCode) {
		case KeyMap.KEYCODE_DPAD_LEFT:
		case KeyMap.KEYCODE_DPAD_RIGHT:
			if(mPippopUiLogic.isPipOrPopState()){
				if(null != mShowChannelListView && mShowChannelListView.isShowing()){
					return true;
				}
				if(null != mShowSourceListView && mShowSourceListView.isShowing()){
					return true;
				}
				if(null != mShowFavoriteChannelListView && mShowFavoriteChannelListView.isShowing()){
					return true;
				}
			}
			break;
		
		
		case KeyMap.KEYCODE_MTKIR_PIPPOS:
		case KeyMap.KEYCODE_MTKIR_PIPSIZE:
		case KeyEvent.KEYCODE_MTKIR_SWAP:
			if(mPippopUiLogic.isNormalState()){
				return true;
			}
		case KeyMap.KEYCODE_MTKIR_PIPPOP:
//			if (KeyMap.KEYCODE_MTKIR_PIPPOP == keyCode) {
//				if (keyUp != true) {
//					MtkLog.d(TAG, "come in keyUp != true");
//					return true;
//				}
//				keyUp = false;
//			}
			if((null != event) && (event.getRepeatCount() != 0)){
				MtkLog.d(TAG, "come in event.getRepeatCount() != 0");
				return true;
			}
			if (null != mShowSourceListView && mShowSourceListView.isShowing()) {
				return true;
			}
			if(null != mShowChannelListView && mShowChannelListView.isShowing()){
				mShowChannelListView.dismiss();
			}
			hideFavoriteList();
			hideSundryLayout();
			if(null != mSpecial_SnowTextView && mSpecial_SnowTextView.getVisibility() == View.VISIBLE){
				mSpecial_SnowTextView.setVisibility(View.GONE);
			}
			break;
		default:
			break;
		}
		if(mPippopState.dispatchAction(keyCode)){
			MtkLog.d(TAG,"come in mPippopState.dispatchAction(keyCode)");
			switch (keyCode) {
			case KeyMap.KEYCODE_DPAD_LEFT:
			case KeyMap.KEYCODE_DPAD_RIGHT:
				mBannerView.hideAllBanner();
				hideSundryLayout();
//				if(null != mShowSourceListView&&mShowSourceListView.isShowing()){
//					mShowSourceListView.exit();
//				}
				mBannerView.show(false, -1, false);
				if(showCurrentInputsourceLocked(keyCode)){
					MtkLog.d(TAG,"come in showCurrentInputsourceLocked = true");
					return true;
				}
				showCurrentChannelLocked();
				return true;
//			case KeyMap.KEYCODE_DPAD_CENTER:
//				if(showCurrentInputsourceLocked()||showCurrentChannelLocked()){
//					return true;
//				}else{
//					channelListViewShow();
//				}
//				
//				return true;
//			case KeyMap.KEYCODE_MTKIR_PIPPOP:
//			case KeyMap.KEYCODE_MTKIR_PIPSIZE:
//			case KeyMap.KEYCODE_MTKIR_PIPPOS:
			case KeyMap.KEYCODE_MTKIR_MTKIR_SWAP:
//				if(mPippopUiLogic.isPipOrPopState()){
//					if(null != mShowChannelListView && mShowChannelListView.isShowing()){
//						mShowChannelListView.dismiss();
//						return true;
//					}
//					if(null != mShowFavoriteChannelListView && mShowFavoriteChannelListView.isShowing()){
//						mShowFavoriteChannelListView.dismiss();
//						return true;
//					}
//				}
				if(showCurrentInputsourceLocked(keyCode)){
					MtkLog.d(TAG,"come in showCurrentInputsourceLocked = true");
				}else{
					showCurrentChannelLocked();
				}
				
				break;
			default:
				break;
			}
			return true;
		}
		boolean handled = false;
		switch (event.getKeyCode()) {
		case KeyMap.KEYCODE_MTKIR_REWIND:
				Log.d("TKUIMAIN TTX", "activeComponent!!!!!!!"+	 inpMgr.getCurrInputSource());
				if(isATV()){
					tvTeleTextManager.activateComponent(ComponentService.COMP_TTX_NAME);
				}
				
			break;
		case KeyMap.KEYCODE_BACK:
			if (mSpecial_SnowTextView.getVisibility() != View.VISIBLE) {
				if (mNumputChangeChannel == true) {
					cancelNumChangeChannel();
				}

				hideAll();

				if (mSpecial_SnowTextView.mForcedtoGONE == true) {
					if(!mPippopUiLogic.isPipOrPopState()){
					mCheckLockSignalChannelState
							.checkLockedSignStateOrHasChannel(false);
					}
				}
			}
			break;
		case KeyMap.KEYCODE_MENU:
			// service for ci
		
			mCICardDelayNotification.clearDelayNotifications();
			mCICardDelayNotification.setPushToQueue(false);
			hideAll();
			NavSundryImplement nsi=NavSundryImplement.getInsNavSundryImplement(
					TurnkeyUiMainActivity.this);
			if (nsi.isFreeze()) {
				nsi.setFreeze(false);				
			}
			
			//Return "Zoom=1" when "Zoom=1/2 or Zoom=2" and press "Menu" key.
			if(!mPippopUiLogic.isPipOrPopState()){
			NavIntegrationZoom.getInstance(TurnkeyUiMainActivity.this)
					.setZoomMode(NavIntegrationZoom.ZOOM_1);
			}
			
			Intent intent = new Intent(TurnkeyUiMainActivity.this,
					MenuMain.class);
			intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			mNavIntegration.setIsTKMainActivity(false);
			startActivityForResult(intent, 0);
			toNotPauseTV = true;
			break;
		case KeyMap.KEYCODE_MTKIR_ANGLE:
			enterMultimediaApp();
			break;
		case KeyMap.KEYCODE_MTKIR_GUIDE:

			if (mPippopUiLogic.isPipOrPopState()) {
				return true;
			}
			// service for ci
			mCICardDelayNotification.clearDelayNotifications();
			mCICardDelayNotification.setPushToQueue(false);
			if (mNavIntegration.isCurrentSourceTv()) {
				if (mNavIntegration.isDTVHasChannels()) {
					if (!mNavIntegration.isCurrentSourceDTV()) {
						mNavIntegration.changeDTVSource();
					}
					if (mNavIntegration.iGetChannelLength() > 0
							&& !mNavIntegration.isCurrentSourceBlocking()) {
						hideAll();

					//Return "Zoom=1" when "Zoom=1/2 or Zoom=2" and press "guide" key.
					NavIntegrationZoom.getInstance(TurnkeyUiMainActivity.this)
								.setZoomMode(NavIntegrationZoom.ZOOM_1);
					mNavIntegration.setIsTKMainActivity(false);	
						startActivityForResult(new Intent(
								TurnkeyUiMainActivity.this, EPGActivity.class),
								0);
						toNotPauseTV = true;
					}
				}
			}
			break;
		/** CECFUN ***/
		case KeyMap.KEYCODE_MTKIR_TITLEPBC:
			hideAll();
			CecManager.getInstance(this).onKeyDown(keyCode);
			break;
		case KeyMap.KEYCODE_0:
		case KeyMap.KEYCODE_1:
		case KeyMap.KEYCODE_2:
		case KeyMap.KEYCODE_3:
		case KeyMap.KEYCODE_4:
		case KeyMap.KEYCODE_5:
		case KeyMap.KEYCODE_6:
		case KeyMap.KEYCODE_7:
		case KeyMap.KEYCODE_8:
		case KeyMap.KEYCODE_9:
			if (mNavIntegration.isCurrentSourceTv()) {
				mSpecial_SnowTextView.setVisibility(View.GONE);
				if (mNavIntegration.iGetChannelLength() <= 0) {
					mBannerView.show(false, -1, false);
					return true;
				} else {
					inputChannelNum(keyCode);
					mBannerView.updateInputting(mSelectedChannelNum + "");
					mHandler.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);
					mHandler.sendEmptyMessageDelayed(
							MessageType.NAV_BANNERVIEW_DIMISS,
							MessageType.delayMillis4);
					mHandler
							.removeMessages(MessageType.NAV_NUMKEY_CHANGE_CHANNEL);
					mHandler.sendEmptyMessageDelayed(
							MessageType.NAV_NUMKEY_CHANGE_CHANNEL,
							MessageType.delayMillis5);
				}
			} else {
				if (mPippopUiLogic.isPipOrPopState()) {
					return true;
				}
				mSpecial_SnowTextView.setVisibility(View.GONE);
				inputChannelNum(keyCode);
				mBannerView.updateInputting(mSelectedChannelNum + "");
				mHandler.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);
				mHandler.sendEmptyMessageDelayed(
						MessageType.NAV_BANNERVIEW_DIMISS,
						MessageType.delayMillis4);
				mHandler.removeMessages(MessageType.NAV_NUMKEY_CHANGE_CHANNEL);
				mHandler.sendEmptyMessageDelayed(
						MessageType.NAV_NUMKEY_CHANGE_CHANNEL,
						MessageType.delayMillis5);
			}
			break;

		case KeyMap.KEYCODE_VOLUME_DOWN:
		case KeyMap.KEYCODE_VOLUME_UP:
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}
			hideChannelList();
			hideTypeList();
			hideFavoriteList();
			hideZoomView();
			hideSundryLayout();
			hideInputPwdView();
			mSpecial_SnowTextView.setVisibility(View.GONE);

			if (mAdjustVolLayout.getVisibility() != View.VISIBLE) {
				mAdjustVolLayout.setVisibility(View.VISIBLE);
				if(AnimationManager.getInstance().getIsAnimation()){
					AnimationManager.getInstance().adjustVolEnterAnimation( mAdjustVolLayout);
				}
				adjustVolumeView.initVolume();
				getVolumeViewRec();
				if (mSundryLayout.getVisibility() == View.VISIBLE) {
					unlockSundryLayoutRect();
				}
			} else {
				adjustVolumeView.volumeEvent(keyCode, event);
			}
			break;

		case KeyMap.KEYCODE_DPAD_DOWN:
		case KeyMap.KEYCODE_DPAD_UP:
			if (mAdjustVolLayout.getVisibility() == View.VISIBLE) {
				adjustVolumeView.volumeEvent(keyCode, event);

			} else if (mZoomView.getVisibility() == View.VISIBLE) {
				mHandler
						.removeMessages(MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS);
				mZoomView.changPicture(keyCode);
				mHandler.sendEmptyMessageDelayed(
						MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS,
						MessageType.delayMillis4);
			} else if (!mShowChannelListView.isShowing()
					&& !mShowFavoriteChannelListView.isShowing()) {
				if (keyCode == KeyMap.KEYCODE_DPAD_UP) {
					mBannerView.detailPageUp();
				} else {
					mBannerView.detailPageDown();
				}
			}

			break;

		case KeyMap.KEYCODE_MTKIR_SOURCE:

			// gain current item selected position
			if (mNumputChangeChannel == true) {
				cancelNumChangeChannel();
				if(!mPippopUiLogic.isPipOrPopState()){
				mCheckLockSignalChannelState
						.checkLockedSignStateOrHasChannel(false);
				}
				return true;
			}
			if (mShowSourceListView.isShowing() == false) {
				hideAllOnSource();
				mShowSourceListView.show();
				mShowSourceListWindowId = mBypassWindowManager
						.getAvailableWindowId();
				mShowSourceListViewRect = new Rect(mShowSourceListView.x
						- mShowSourceListView.menuWidth / 2
						+ ScreenConstant.SCREEN_WIDTH / 2,
						mShowSourceListView.y - mShowSourceListView.menuHeight
								/ 2 + ScreenConstant.SCREEN_HEIGHT / 2,
						mShowSourceListView.x + mShowSourceListView.menuWidth
								/ 2 + ScreenConstant.SCREEN_WIDTH / 2,
						mShowSourceListView.y + mShowSourceListView.menuHeight
								/ 2 + ScreenConstant.SCREEN_HEIGHT / 2);
				MtkLog.i(TAG, "mShowSourceListViewRect  left:"
						+ mShowSourceListViewRect.left + "  top "
						+ mShowSourceListViewRect.top + "  right "
						+ mShowSourceListViewRect.right + " bottom"
						+ mShowSourceListViewRect.bottom);
				mBypassWindowManager.setBypassWindow(true,
						mShowSourceListWindowId, mShowSourceListViewRect);
				return true;
			}

			break;
		case KeyMap.KEYCODE_MTKIR_TIMER:
			mNavIntegration.syncTimeIfNeed();
		case KeyMap.KEYCODE_MTKIR_SLEEP:
		case KeyMap.KEYCODE_MTKIR_PEFFECT:
		case KeyMap.KEYCODE_MTKIR_SEFFECT:
		case KeyMap.KEYCODE_MTKIR_ASPECT:
		case KeyMap.KEYCODE_MTKIR_FREEZE:
		case KeyMap.KEYCODE_MTKIR_MTSAUDIO:
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}

			hideZoomView();
			
			if (mSundryLayout.getVisibility() != View.VISIBLE) {
				MtkLog.i(TAG, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ok");
				hideTypeList();
				hideFavoriteList();
				hideChannelList();
				mSpecial_SnowTextView.setVisibility(View.GONE);
				mSundryLayout.setVisibility(View.VISIBLE);
				mNavSundryShowTextView.setAndShowTextContent(false, keyCode);
				getSundryLayoutRec();
			} else {
				if (mSundryLayout.getVisibility() == View.VISIBLE
						&& mOldShortTipType == keyCode) {
					MtkLog.i(TAG, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~no");
					mNavSundryShowTextView.setThirdApp(mReturnFromThirdApp);
					mNavSundryShowTextView.setAndShowTextContent(true, keyCode);
				} else {
					mNavSundryShowTextView.startAnimation(mTipEnterAnimation);
					mNavSundryShowTextView.setThirdApp(mReturnFromThirdApp);
					mNavSundryShowTextView
							.setAndShowTextContent(false, keyCode);
				}
			}
			mOldShortTipType = keyCode;
			break;

		case KeyMap.KEYCODE_MTKIR_ZOOM:
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}
			mHandler.removeMessages(MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS);
			NavSundryShowTextView.timeFlag = false;
			if (isNormalForZoom()) {
				if (mSpecial_SnowTextView.getVisibility() == View.VISIBLE) {
					mSpecial_SnowTextView.setVisibility(View.GONE);
				}
				mSundryLayout.setVisibility(View.VISIBLE);
				mNavSundryShowTextView.setAndShowTextContent(false, keyCode);
				if (mNavSundryShowTextView.getVisibility() != View.VISIBLE) {
					mNavSundryShowTextView.setVisibility(View.VISIBLE);
				}
				mNavSundryShowTextView
						.setText(getString(R.string.nav_no_function));
				getSundryLayoutRec();
			} else {
				if (mNavSundryShowTextView.isFreeze == true) {
					mZoomView.zoomSetfreeze();
					mNavSundryShowTextView.isFreeze = false;
				}
				if (mSundryLayout.getVisibility() == View.VISIBLE
						&& mOldShortTipType == keyCode) {
					mZoomView.changeZoom(mNavSundryShowTextView);
				} else if (mShowSourceListView.isShowing() == false) {
					mSundryLayout.setVisibility(View.VISIBLE);
					if (mSpecial_SnowTextView.getVisibility() == View.VISIBLE) {
						mSpecial_SnowTextView.setVisibility(View.GONE);
					}
					hideChannelList();
					hideFavoriteList();
					mNavSundryShowTextView
							.setAndShowTextContent(false, keyCode);
					mZoomView.showCurrentZoom(mNavSundryShowTextView);
					mNavSundryShowTextView.startAnimation(mTipEnterAnimation);
					getSundryLayoutRec();
				}
			}
			mOldShortTipType = keyCode;
			mHandler.sendEmptyMessageDelayed(
					MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS,
					MessageType.delayMillis4);
			break;

		case KeyMap.KEYCODE_MTKIR_YELLOW:

			// fix CR DTV00579362
			// if(mPippopUiLogic.isPipOrPopState()){
			// return true;
			// }
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}

			if (mNavIntegration.canDoCapLogo()) {
				hideAll();
				mNavIntegration.setIsTKMainActivity(false);
				startActivityForResult(new Intent(TurnkeyUiMainActivity.this,
						CaptureLogoActivity.class), 0);
				toNotPauseTV = true;
			} else {
				mToastInfo.toastCaptureInfo();
			}
			break;

		case KeyMap.KEYCODE_MTKIR_CHUP:
		case KeyMap.KEYCODE_MTKIR_CHDN:
		case KeyMap.KEYCODE_MTKIR_PRECH:
			if(handleChangeChannel(keyCode)){
				return true;
			}
			break;

		case KeyMap.KEYCODE_MTKIR_INFO:
			TVChannel curChannel = mNavIntegration.iGetCurrentChannel();
			hideChannelList();
			hideTypeList();
			hideFavoriteList();
			hideAdustVolLayout();
			hideSundryLayout();
			hideInputPwdView();
			mSpecial_SnowTextView.setVisibility(View.GONE);
			mBannerView.setIsOnKeyInfo(true);
			mBannerView.show(true, -1, true);
			mBannerView.setIsOnKeyInfo(false);
			mZoomView.zoomGone();
			break;

		case KeyMap.KEYCODE_MTKIR_SUBTITLE:
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}
			if (mNavIntegration.isCurrentSourceTv()) {
				mSpecial_SnowTextView.setVisibility(View.GONE);
				if (mNavIntegration.iGetChannelLength() > 0) {
					hideZoomView();
					hideAdustVolLayout();
					mSpecial_SnowTextView.setVisibility(View.GONE);
					NavSundryImplement mNsi=NavSundryImplement.getInsNavSundryImplement(
							TurnkeyUiMainActivity.this);
					if (mNsi.isFreeze() ) {
						mNsi.setFreeze(false);
						String[] freezeModeName = TurnkeyUiMainActivity.this
								.getResources().getStringArray(
										R.array.nav_freeze_strings);
						mNavSundryShowTextView.setText(freezeModeName[1]);
						mBannerView.show(false, -1, false);
						// hideSundryLayout();
					}
					mBannerView.updateSubtitle();
				} else {
					mSpecial_SnowTextView.setVisibility(View.GONE);
					mBannerView.show(false, -1, false);
				}
			}
			break;
		case KeyMap.KEYCODE_DPAD_CENTER:
			if (mNumputChangeChannel) {
				cancelNumChangeChannel();
				mHandler.sendEmptyMessage(MessageType.NAV_NUMKEY_CHANGE_CHANNEL);
				return true;
			}
			if (mNavIntegration.isCurrentSourceBlocking()
					|| mNavIntegration.isCurrentChannelBlocking()) {
				if (mInputPwdView != null && mInputPwdView.isShowing() == false) {
					hideAll();
					mBannerView.show(false, -1, false);
					hideTimeShiftInfoBar();
					mInputPwdView.show();
				}
				return true;
			}
//			if (mShowSourceListView.isShowing() == false
//					&& mNavIntegration.isCurrentSourceTv()) {
//				if (mShowChannelListView.isShowing() == false
//						&& mNavIntegration.isCurrentSourceTv()) {
//					if (mNavIntegration.iGetChannelLength() > 0) {
//						hideZoomView();
//						hideAdustVolLayout();
//						hideSundryLayout();
//						mSpecial_SnowTextView.setVisibility(View.GONE);
//						mShowChannelListView.show();
//						mShowChannelListViewRect = new Rect(
//								mShowChannelListView.x
//										- mShowChannelListView.menuWidth / 2
//										+ ScreenConstant.SCREEN_WIDTH / 2,
//								mShowChannelListView.y
//										- mShowChannelListView.menuHeight / 2
//										+ ScreenConstant.SCREEN_HEIGHT / 2,
//								mShowChannelListView.x
//										+ mShowChannelListView.menuWidth / 2
//										+ ScreenConstant.SCREEN_WIDTH / 2,
//								mShowChannelListView.y
//										+ mShowChannelListView.menuHeight / 2
//										+ ScreenConstant.SCREEN_HEIGHT / 2);
//						MtkLog.i(TAG, "mShowChannelListViewRect  left:"
//								+ mShowChannelListViewRect.left + "  top "
//								+ mShowChannelListViewRect.top + "  right "
//								+ mShowChannelListViewRect.right + " bottom"
//								+ mShowChannelListViewRect.bottom);
//						mShowChannelListWindowId = mBypassWindowManager
//								.getAvailableWindowId();
//						mBypassWindowManager.setBypassWindow(true,
//								mShowChannelListWindowId,
//								mShowChannelListViewRect);
//					} else {
//						mSpecial_SnowTextView.setVisibility(View.GONE);
//						mBannerView.show(false, -1, false);
//					}
//				}
//			}
			channelListViewShow();
			break;

		case KeyMap.KEYCODE_MTKIR_RED:
			if(startLauncher()){
				return true;
			}
			break;

		case KeyMap.KEYCODE_MTKIR_EJECT:
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}
			if (mNavIntegration.isCurrentSourceTv()) {
				mSpecial_SnowTextView.setVisibility(View.GONE);
				if (mNavIntegration.iGetChannelLength() > 0) {
					if (!mShowFavoriteChannelListView.isShowing()) {
						mNavIntegration.isSetChannelFavorite(mNavIntegration
								.iGetCurrentChannel());
					}
					mBannerView.updateFavorite();
				} else {
					mSpecial_SnowTextView.setVisibility(View.GONE);
					mBannerView.show(false, -1, false);
				}
			}

			break;
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}
			if (mNavIntegration.isCurrentSourceTv()) {
				mSpecial_SnowTextView.setVisibility(View.GONE);
				if (mNavIntegration.iGetChannelLength() > 0) {
					hideZoomView();
					hideAdustVolLayout();
					hideSundryLayout();
					hideTimeShiftInfoBar();
					mSpecial_SnowTextView.setVisibility(View.GONE);
					mShowFavoriteChannelListView.show();
					mShowFavoriteChannelListViewRect = new Rect(
							mShowFavoriteChannelListView.x
									- mShowFavoriteChannelListView.menuWidth
									/ 2 + ScreenConstant.SCREEN_WIDTH / 2,
							mShowFavoriteChannelListView.y
									- mShowFavoriteChannelListView.menuHeight
									/ 2 + ScreenConstant.SCREEN_HEIGHT / 2,
							mShowFavoriteChannelListView.x
									+ mShowFavoriteChannelListView.menuWidth
									/ 2 + ScreenConstant.SCREEN_WIDTH / 2,
							mShowFavoriteChannelListView.y
									+ mShowFavoriteChannelListView.menuHeight
									/ 2 + ScreenConstant.SCREEN_HEIGHT / 2);
					MtkLog.i(TAG, "mShowFavoriteChannelListViewRect  left:"
							+ mShowFavoriteChannelListViewRect.left + "  top "
							+ mShowFavoriteChannelListViewRect.top + "  right "
							+ mShowFavoriteChannelListViewRect.right
							+ " bottom"
							+ mShowFavoriteChannelListViewRect.bottom);
					mShowFavoriteChannelListWindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBypassWindowManager.setBypassWindow(true,
							mShowFavoriteChannelListWindowId,
							mShowFavoriteChannelListViewRect);
				} else {
					mSpecial_SnowTextView.setVisibility(View.GONE);
					mBannerView.show(false, -1, false);
				}
			}

			break;

		case KeyMap.KEYCODE_MTKIR_STOP:
			 List<TVChannel> mChannelList =mNavIntegration.iGetFavoriteList();
			if ((mChannelList != null && mChannelList.size() > 0)
					&& !mNavIntegration.isCurrentSourceBlocked()) {
				// channel list and current channel not null
				hideSundryLayout();
				mNavIntegration.iSelectFavChanUp();
			}
			break;

		case KeyMap.KEYCODE_MTKIR_MUTE:
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}
			TurnkeyUiMainActivity.switchMute();

			break;

		// MTS/AUDIO

		// left/right to change focus
		case KeyMap.KEYCODE_DPAD_LEFT:
		case KeyMap.KEYCODE_DPAD_RIGHT:
			if (mShowSourceListView.isShowing()) {
				return true;
			}
			if (mZoomView.getVisibility() == View.VISIBLE) {
				mHandler
						.removeMessages(MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS);
				mZoomView.changPicture(keyCode);
				mHandler.sendEmptyMessageDelayed(
						MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS,
						MessageType.delayMillis4);
			}
			break;
		case KeyMap.KEYCODE_MTKIR_GREEN:
		 
		  if (SystemProperties.getInt("mtk.vss.demo", 0) == 1) {
				try {
					mNavIntegration.setIsTKMainActivity(false);
					startActivityForResult(new Intent(TurnkeyUiMainActivity.this,
							VssDemoActivity.class), 0);
					toNotPauseTV = true;
				} catch (Exception e) {
					Toast.makeText(this, "no VssDemo.", Toast.LENGTH_SHORT).show();
				}
			}
			break;
		case KeyMap.KEYCODE_MTKIR_MTKIR_SWAP:
			return false;
		case KeyMap.KEYCODE_MTKIR_FASTFORWARD:
		case KeyMap.KEYCODE_MTKIR_PREVIOUS:
		case KeyMap.KEYCODE_MTKIR_NEXT:
            return true;
		default:
			break;
		}

		// Return true to prevent this event from being propagated further
		return true;

	}

	public boolean startLauncher() {
		if (mPippopUiLogic.isPipOrPopState()) {
//				reshowFoucs();
			return true;
		} else {
			if(null != viewLayout){
				viewLayout.setVisibility(View.VISIBLE);
				}
			MtkLog.i(TAG, "viewLayout.setVisibility(View.VISIBLE~~~ok");
		mMuteImageView.setVisibility(View.GONE);
		Intent mIntent = new Intent();
		TVContent.getInstance(this).setOpacity(255);
		mIntent.setClassName("com.android.launcher",
				"com.android.launcher2.Launcher");
		boolean hasLauncher = false;
		try {
			int setValue=Settings.System.getInt(getContentResolver(), Settings.System.AUTO_TIME, 0);
			if(setValue==1){
//					NetworkTime.getInstance(this).getNetTime();
			}
		mStatusBarManager.setSystemUiBarHight(getApplicationContext().getResources().getDimensionPixelSize(com.android.internal.R.dimen.navigation_bar_height));
			mNavIntegration.setIsTKMainActivity(false);
			startActivity(mIntent);
			hasLauncher = true;
		} catch (Exception e) {
			Toast.makeText(this, "no Launcher.", Toast.LENGTH_SHORT).show();
		}
		if (hasLauncher && mToastInfo != null) {
			mHandler.removeMessages(MessageType.NAV_BANNERVIEW_DIMISS);
			mToastInfo.cancelVGAInfo();
		}
		MenuConfigManager.getInstance(this).setBrightness();

		}
		return false;
	}

	public boolean handleChangeChannel(int keyCode) {
		boolean handled = false;
		switch (keyCode) {
		case KeyMap.KEYCODE_MTKIR_PRECH:
			// add by lzj for fix bug DTV00387849
			if (mNavIntegration.isCurrentSourceBlocking()) {
				return true;
			}

			hideViewsWithChangeChannel();
			handled = keyEventNoResponse();
			if (handled == true) {
				return true;
			}
			if (!mNavIntegration.isCurrentSourceTv()) {
				break;
			}
			if (mNavIntegration.iGetChannelLength() > 0) {
				mNavIntegration.iSetChannel(NavIntegration.CHANNEL_PRE);
			} else {
				// mSpecial_SnowTextView.setVisibility(View.GONE);
				// showNullInfoBar(
				// getString(R.string.nav_please_scan_channels), false);
			}

			break;
		case KeyMap.KEYCODE_MTKIR_CHUP:
		case KeyMap.KEYCODE_MTKIR_CHDN:
			if (isChangeChannelResponse()) {
				if (isActive && isATV()) {
					tvTeleTextManager
							.inActivateCompoent(ComponentService.COMP_TTX_NAME);
				}
				hideViewsWithChangeChannel();
				handled = keyEventNoResponse();
				if (handled == true) {
					return true;
				}

				if (mNavIntegration.isCurrentSourceTv() == false) {
					mNavIntegration.iSetSourcetoTv();
					return true;
				} else {
					if (mNavIntegration.iGetChannelLength() > 0) {
						if(mNavIntegration.isCurrentSourceBlocked()){
							return true;
						}
						if (keyCode == KeyMap.KEYCODE_MTKIR_CHDN) {
							mNavIntegration
									.iSetChannel(NavIntegration.CHANNEL_DOWN);
						} else {
							mNavIntegration
									.iSetChannel(NavIntegration.CHANNEL_UP);
						}
					} else {
						mHandler.sendEmptyMessage(MessageType.NAV_SCANN_CHANNEL);
					}
				}
			}
			break;
		}
		return handled;
	}

	/**
	 * 
	 * this method is used to enter another MMPOnly.apk
	 * 
	 */
	public static void enterMultimediaApp() {
		if (TimeShiftManager.getInstance().pvrIsPlaying()) {
			PvrDialog conDialog = new PvrDialog(TimeShiftManager.getInstance()
					.getActivity(), PvrDialog.TYPE_Confirm,
					KeyMap.KEYCODE_MTKIR_ANGLE, PvrDialog.TYPE_Timeshift);
			conDialog.setOnPVRDialogListener(new OnPVRDialogListener() {

				@Override
				public void onPVRDialogListener(int keyCode) {
					// TODO Auto-generated method stub
					mainActivity.enterMMP();
				}
			});
			conDialog.show();
		} else {
			mainActivity.enterMMP();
		}
	}

	public void enterMMP() {
		if (MeidaMainActivity.isValid()) {
			// service for ci
			mCICardDelayNotification.clearDelayNotifications();
			mCICardDelayNotification.setPushToQueue(false);
			hideAll();
			mMuteImageView.setVisibility(View.GONE);

			// enter mmp, set isResume to false.
			isResume = false;
			NewPipLogic pl = NewPipLogic.getInstance(this);
			pl.pauseMainOutput();
			if (mPippopUiLogic.isPipOrPopState()) {
				pl.pauseSubOutput();
				mPippopState.getmController().hiddenAllTVViews();
				mPippopState.getmController().releaseFocus();
				// mPippopState.getmController().getMainOutPutWindow().setFullScreen();
				// mPippopState.getmController().modifyMainOutput();
				// mPippopState.getmController().modifySubOutput();
				mPippopState.setState(new PippopMode_Normal(mPippopState));
				mPippopUiLogic.setOutputPosition("sub", new RectF(0, 0, 0, 0));
				try {
					TVCommonNative.getDefault(this).enterOutputMode(
							ITVCommon.OUTPUT_MODE_MMP);
				} catch (RemoteException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			TVContent.getInstance(this).setOpacity(255);
			toNotPauseTV = true;
			mNavIntegration.setIsTKMainActivity(false);
			startActivityForResult(new Intent(TurnkeyUiMainActivity.this,
					MeidaMainActivity.class), 0);
		}
	}
	
	public void enterMMP(String path) {
		if (MeidaMainActivity.isValid()) {
			// service for ci
			mCICardDelayNotification.clearDelayNotifications();
			mCICardDelayNotification.setPushToQueue(false);
			hideAll();
			mMuteImageView.setVisibility(View.GONE);
			
			// enter mmp, set isResume to false.
			isResume = false;
			NewPipLogic pl = NewPipLogic.getInstance(this);
			pl.pauseMainOutput();
			if (mPippopUiLogic.isPipOrPopState()) {
				pl.pauseSubOutput();
				mPippopState.getmController().hiddenAllTVViews();
				mPippopState.getmController().releaseFocus();
				// mPippopState.getmController().getMainOutPutWindow().setFullScreen();
				// mPippopState.getmController().modifyMainOutput();
				// mPippopState.getmController().modifySubOutput();
				mPippopState.setState(new PippopMode_Normal(mPippopState));
				mPippopUiLogic.setOutputPosition("sub", new RectF(0, 0, 0, 0));
				try {
					TVCommonNative.getDefault(this).enterOutputMode(
							ITVCommon.OUTPUT_MODE_MMP);
				} catch (RemoteException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			Message msg=mHandler.obtainMessage();
			msg.what=START_VIDEO_PLAYER;
			Bundle bundle=new Bundle();
			bundle.putString("PATH", path);
			msg.setData(bundle);
			mHandler.sendMessageDelayed(msg, 500);
		}
	}
	
	
	private void startVideoPlayer(String path){
		TVContent.getInstance(this).setOpacity(255);
		toNotPauseTV = true;
		Intent intent=new Intent(TurnkeyUiMainActivity.this,
				VideoPlayActivity.class);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		intent.setData(Uri.parse(path));
		intent.putExtra("playlocal", true);
		
		VideoPlayActivity.video_player_Activity_resumed=true;
		mNavIntegration.setIsTKMainActivity(false);
		startActivityForResult(intent, PVR_TO_PLAY_VIDEO);
		
	}
	private boolean keyEventNoResponse() {
		if (mShowSourceListView.isShowing()) {
			mHandler.removeMessages(MessageType.NAV_SOURCE_LISTVIEW_DIMISS);
			mHandler.sendEmptyMessageDelayed(
					MessageType.NAV_SOURCE_LISTVIEW_DIMISS,
					MessageType.delayMillis4);
			return true;
		}
		if (mNumputChangeChannel) {
			cancelNumChangeChannel();
			return true;
		}
		return false;
	}

	private void init() {
		mainActivity = TurnkeyUiMainActivity.this;

		mCICardDelayNotification = CICardDelayNotification.getInstance();
		mCAMManager = TVContent.getInstance(this).getCAMManager();
		mCAMManager.registerCamStatusListener(mCamStatusListener);

		mShowChannelListView = new ShowChannelListView(this);
		// mShowChannelTypeView = new ShowChannelTypeView(this);
		mShowFavoriteChannelListView = new ShowFavoriteChannelListView(this);
		mShowSourceListView = new ShowSourceListView(this);

		mInputPwdView = new InputPwdDialog(this);
		mAdjustVolLayout = (LinearLayout) findViewById(R.id.nav_volume_layout);
		adjustVolumeView = (AdjustVolumeView) findViewById(R.id.nav_adjust_volume_view);
		adjustVolumeView.setHandler(mHandler);

		mSundryLayout = (LinearLayout) findViewById(R.id.nav_sundry_layout);
		mNavSundryShowTextView = (NavSundryShowTextView) findViewById(R.id.nav_tv_shortTip_textview);
		mNavSundryShowTextView.setHandler(mHandler);

		mBannerView = (BannerView) findViewById(R.id.nav_banner_info_bar);
		// mBannerView.hideAllBanner();
		mBannerView.setHandler(mHandler);
		mBannerView.setBannerState(BannerView.CHILD_TYPE_INFO_SIMPLE);
		mShowSourceListView.setBannerView(mBannerView);
		mInputPwdView.setBannerView(mBannerView);
		mNavSundryShowTextView.setBannerView(mBannerView);

		mSpecial_SnowTextView = (SnowTextView) findViewById(R.id.nav_stv_special_model);
		mCheckLockSignalChannelState = CheckLockSignalChannelState
				.getInstance(this);
		mCheckLockSignalChannelState.setHandler(mHandler);
		mCheckLockSignalChannelState.setSnowTextView(mSpecial_SnowTextView);
		mCheckLockSignalChannelState.setBannerView(mBannerView);
		mZoomView = (ZoomView) findViewById(R.id.nav_zoomview);
		mZoomView.setmHandler(mHandler);
		mTipEnterAnimation = AnimationUtils.loadAnimation(this,
				R.anim.zoom_enter);
	}

	private void initCheckPowTimer() {
		POSTManager.getInstance(this).checkPowOnTimer();
		POSTManager.getInstance(this).checkPowOffTimer();
		POSTManager.getInstance(this).checkSleepTime();
		POSTManager.getInstance(this).checkAutoSleep();
	}

	private void registeListeners() {
		mNavIntegration.iAddChSelectListener(mChannelSelectListener);
		mNavIntegration.iAddInputsourceChangeListener(mSourceChangeListener);
		// mModel_Parental_InputPwd
		// .addTextChangedListener(passwordInputTextWatcher);
	}

	private void unregisteListeners() {
		mNavIntegration.iRemoveChSelectListener(mChannelSelectListener);
		mNavIntegration.iRemoveInputsourceChangeListener(mSourceChangeListener);
	}

	private void initMuteImageView() {
		mMuteImageView = new ImageView(getApplicationContext());
		mMuteImageView.setImageResource(R.drawable.nav_mute);
		mMuteImageView.setBackgroundResource(R.drawable.translucent_background);
		wm = (WindowManager) getApplicationContext().getSystemService(
				WINDOW_SERVICE);
		wmParams = new LayoutParams();
		wmParams.type = LayoutParams.TYPE_PHONE;
		wmParams.flags |= LayoutParams.FLAG_NOT_FOCUSABLE;

		wmParams.gravity = Gravity.RIGHT | Gravity.BOTTOM;
		wmParams.width = LayoutParams.WRAP_CONTENT;
		wmParams.height = LayoutParams.WRAP_CONTENT;
		wmParams.format = android.graphics.PixelFormat.TRANSPARENT;
		mMuteImageView.setVisibility(View.GONE);
		wm.addView(mMuteImageView, wmParams);
	}

	public static void setMuteGone() {
		mMuteImageView.setVisibility(View.GONE);
	}

	public static void setMuteVisible() {
		mMuteImageView.setVisibility(View.VISIBLE);
	}

	public static void cancelMute() {
		mNavIntegration.iSetMute(false);
		mMuteImageView.setVisibility(View.GONE);

	}

	IChannelSelectorListener mChannelSelectListener = new IChannelSelectorListener() {

		public void signalChange(boolean hasSignal) {
			MtkLog.v(TAG, "signal has changed  0410, hasSignal is  " + hasSignal);
			mSpecial_SnowTextView.setVisibility(View.GONE);
			mBannerView.hideAllBanner();
			if (!mSetupFlag && mNavIntegration.isTKMainActivity()) {
				mBannerView.show(false, -1, false);
			}
			MtkLog.v(TAG, "signal has changed  come in hideInputPwdView == " + isChangeChannelResponse());
			if(isChangeChannelResponse()){	
				hideInputPwdView();
			}
		}

		public void updateChannel(TVChannel ch) {
			MtkLog.v(TAG, "updateChannel 0410");
			mCICardDelayNotification.clearDelayNotifications();
			mCICardDelayNotification.setPushToQueue(false);
			mSpecial_SnowTextView.setVisibility(View.GONE);
			if(mNavIntegration.isCurrentChannelDTV()){
			mNavIntegration.resetSubtitle();
			}
			mBannerView.show(false, BannerView.CHILD_TYPE_INFO_SIMPLE, false);
			MtkLog.v(TAG, "updateChannel come in hideInputPwdView == " + isChangeChannelResponse());
			if(isChangeChannelResponse()){
				hideInputPwdView();
			}
			if (!mPippopUiLogic.isPipOrPopState()
					&& (NavIntegrationZoom.ZOOM_1 != NavIntegrationZoom
							.getInstance(TurnkeyUiMainActivity.this)
							.iGetCurrentZoom())) {
			NavIntegrationZoom.getInstance(TurnkeyUiMainActivity.this)
					.setZoomMode(NavIntegrationZoom.ZOOM_1);
			}
			mHandler.removeMessages(MessageType.NAV_CURRENT_CHANNEL_LOCKED);
		}

		public void channelLocked() {
			// TODO Auto-generated method stub
			MtkLog.v(TAG, "channelLocked");
			mCICardDelayNotification.clearDelayNotifications();
			mCICardDelayNotification.setPushToQueue(true);
			mHandler.removeMessages(MessageType.NAV_CURRENT_CHANNEL_LOCKED);
			mHandler.sendEmptyMessageDelayed(
					MessageType.NAV_CURRENT_CHANNEL_LOCKED,
					MessageType.delayMillis6);
		}

		public void channelScrambled(boolean hasScramble, String state) {
			// TODO Auto-generated method stub
			if (hasScramble) {
			} else {
			}

			mSpecial_SnowTextView.setVisibility(View.GONE);
			mBannerView.show(false, -1, false);
		}

	};
	NavIntegration.IInputSourceChangeListener mSourceChangeListener = new NavIntegration.IInputSourceChangeListener() {

		public void isBlocked(String input) {
			// INPUT SOURCE LOCKE
			MtkLog.i(TAG, "isBlocked !   " + input);
			// service for ci,when source is block
			if (mNavIntegration.isCurrentSourceTv()) {
				mCICardDelayNotification.clearDelayNotifications();
				mCICardDelayNotification.setPushToQueue(true);
			}
			if ((OUT_PUT_SUB.equals(mPippopUiLogic.getOutputFocus()) && input
					.equals(mPippopUiLogic.getSubOutputSourceName()))
					|| (OUT_PUT_MAIN.equals(mPippopUiLogic.getOutputFocus()) && input
							.equals(mPippopUiLogic.getMainOutputSourceName()))) {
				mHandler.removeMessages(MessageType.NAV_CURRENT_SOURCE_LOCKED);
				mHandler.sendEmptyMessage(MessageType.NAV_CURRENT_SOURCE_LOCKED);
			}
			
		}

		public void isInputGotSignal(String input) {
			// INPUT SOURCE HAS SIGAL
			// INPUT IS INOUT SOURCE NAME
			MtkLog.i(TAG, "isInputGotSignal !   " + input);
			mReturnFromThirdApp = true;
			mBannerView.show(false, -1, false);
		}

		public void isOutputSignalChange(String output, boolean hasSignal) {
			// OUTPUT IS MAIN / SUB
			MtkLog.i(TAG, "isOutputSignalChange 0410" + hasSignal);
			if (mNavIntegration.isTKMainActivity()) {
				mBannerView.show(false, -1, false);
			}
			mHandler.removeMessages(MessageType.MESSAGE_SETOPACITY);
			mHandler.sendEmptyMessage(MessageType.MESSAGE_SETOPACITY);
			if (hasSignal) {
                // SUB OUTPUT HAS SIGNAL
            } else {
                // SUB OUTPUT NO SIGNAL
                if (mNavIntegration.isCurrentSourceTv()) {
                    if (null!=TimeShiftManager.getInstance()) {
                        TimeShiftManager.getInstance().stopAllRunning();
                    }
                }
            }
			if ("main".equalsIgnoreCase(output)) {
				mSpecial_SnowTextView.setVisibility(View.GONE);
				if (mNavIntegration.isTKMainActivity()) {
					mBannerView.show(false, -1, false);
				}
				// mToastInfo.toastVGAInfo();
			} else if ("sub".equalsIgnoreCase(output)) {
				if (hasSignal) {
					// SUB OUTPUT HAS SIGNAL
				} else {
					// SUB OUTPUT NO SIGNAL
				}

			}
			MtkLog.v(TAG, "isOutputSignalChange come in hideInputPwdView == "+ output.equals(mPippopUiLogic.getOutputFocus()));
			if(output.equals(mPippopUiLogic.getOutputFocus())&& hasSignal){
				hideInputPwdView();
			}
			
			//resolve CR DTV00371791 by jun gu 
			if (mNavIntegration.isCurrentSourceVGA()) {
				if (hasSignal) {
					mToastInfo.cancelVGAInfo();
				} 
			}
		}

		public void isSelected(String output, String input) {
			MtkLog.i(TAG, "isSelected " + input);
			mHandler.sendEmptyMessageDelayed(MessageType.MESSAGE_SETOPACITY,1500);
			mSpecial_SnowTextView.setVisibility(View.GONE);
			mNavIntegration.resetSubtitle();
			if (mNavIntegration.isTKMainActivity()) {
				if (mNavIntegration.isCurrentSourceTv()
						&& mNavIntegration.iGetChannelLength() <= 0) {
					mBannerView.show(false, -1, false);

				} else {
					mBannerView.showSimpleBar();
				}
			}
			mCICardDelayNotification.clearDelayNotifications();
			mCICardDelayNotification.setPushToQueue(false);
			MtkLog.i("source", "isSelected !   " + output + "  input   "
					+ input);
		}
	};

	private TVCAMManager.CamStatusUpdateListener mCamStatusListener = new TVCAMManager.CamStatusUpdateListener() {

		public void camSystemIDStatusUpdated(byte sysIdStatus) {

		}

		// send remove broadcast
		public void camRemoveUpdated() {
			if(mCICardDelayNotification.isPushToQueue()){
				mCICardDelayNotification.addDelayNotifications(new Runnable() {
					
					@Override
					public void run() {
						mCamStatusListener.camRemoveUpdated();
					}
				});
				return;
			}
			/*if(GetCurrentTask.getInstance(TurnkeyUiMainActivity.this).isCurMPGActivity()){
				MtkLog.i(TAG,"MPG out");
				return;
			}
			
			if(!GetCurrentTask.getInstance(TurnkeyUiMainActivity.this).isCICardVisible()){
				MtkLog.i(TAG,"no visible out");
				return;
			}*/
			mJump = false;
			mCIStatus = false;
			mCAMManager.clearMenuEnqUpdateListener();
			Intent intent = new Intent(CISTATUS_REMOVE);
			sendBroadcast(intent);
		    if(MenuConfigManager.getInstance(getApplicationContext()).getDefault(MenuConfigManager.TUNER_MODE)==ConfigType.BS_SRC_AIR){
			    
			}else{
			    if (null!=TimeShiftManager.getInstance()) {
	                 TimeShiftManager.getInstance().stopAllRunning();
	             }
			}
		}

		// turn to menu-CI-name
		public void camNameUpdated(final String camName) {
			if(mCICardDelayNotification.isPushToQueue()){
				mCICardDelayNotification.addDelayNotifications(new Runnable() {
					
					@Override
					public void run() {
						mCamStatusListener.camNameUpdated(camName);
					}
				});
				return ;
			}
			boolean hasTV = false;
			for (String output : inpMgr.getOutputArray()) {
				String input = inpMgr.getCurrInputSource(output);
				if (input != null
						&& TVInputManager.INPUT_TYPE_TV.equals(
								inpMgr.getTypeFromInputSource(input))) {
					hasTV = true;
				}
			}
			

			if (!hasTV) {
				return;
			}
			if (Util.getMMPFlag()) {
				return;
			}
			
			/*if(GetCurrentTask.getInstance(TurnkeyUiMainActivity.this).isCurMPGActivity()){
				MtkLog.i(TAG,"MPG in");
				return;
			}
			
			if(!GetCurrentTask.getInstance(TurnkeyUiMainActivity.this).isCICardVisible()){
				MtkLog.i(TAG,"not visible in");
				return;
			}*/
			
			if (mJump == false && MenuMain.mScanningStatus == false) {
				mCIStatus = true;
				mJump = true;
				Bundle bundle = new Bundle();
				bundle.putInt("CIValue", 1);
				Intent intent = new Intent();
				intent.putExtras(bundle);
				intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
				intent.setClass(TurnkeyUiMainActivity.this, MenuMain.class);
				toNotPauseTV = true;
				mNavIntegration.setIsTKMainActivity(false);
				startActivity(intent);
			}
		}

		public void camInsertUpdated() {
			// TODO Auto-generated method stub
		}
	};


	private void inputChannelNum(int keycode) {
		mNumputChangeChannel = true;
		mHandler.removeMessages(MessageType.NAV_NUMKEY_CHANGE_CHANNEL);

		if (keycode >= KeyMap.KEYCODE_0 && keycode <= KeyMap.KEYCODE_9) {
			inputChannelNumStrBuffer.append("" + (keycode - KeyMap.KEYCODE_0));
		}

		// show data at panal

		String showString = "";
		if (inputChannelNumStrBuffer.length() <= 4) {
			showString = inputChannelNumStrBuffer.toString();
		} else {
			showString = inputChannelNumStrBuffer.substring(
					inputChannelNumStrBuffer.length() - 4,
					inputChannelNumStrBuffer.length());
		}
		mSelectedChannelNum = Short.valueOf(showString);
		mHandler.sendEmptyMessageDelayed(MessageType.NAV_NUMKEY_CHANGE_CHANNEL,
				MessageType.delayMillis6);

	}

	public void hideAll() {
		hideChannelList();
		hideTypeList();
		hideFavoriteList();
		hideSourceList();
		hideZoomView();
		hideAdustVolLayout();
		hideSundryLayout();
		hideInputPwdView();
		mBannerView.hideAllBanner();
		mSpecial_SnowTextView.setVisibility(View.GONE);
	}

	public void hideAllOnSource() {
		hideChannelList();
		hideTypeList();
		hideFavoriteList();
		hideSourceList();
		hideZoomView();
		hideAdustVolLayout();
		hideSundryLayout();
		hideInputPwdView();
		hideTimeShiftInfoBar();
		mSpecial_SnowTextView.setVisibility(View.GONE);
	}
	
	public void hideAllOnBannerDismiss(){		
		hideZoomView();			
		hideInputPwdView();
		hideAdustVolLayout();
		mSpecial_SnowTextView.setVisibility(View.GONE);
	}

	public void hideInputPwdView() {
		if (mInputPwdView != null && mInputPwdView.isShowing()) {
			mInputPwdView.dismiss();
		}
	}

	private void hideAdustVolLayout() {
		if (mAdjustVolLayout != null
				&& mAdjustVolLayout.getVisibility() != View.GONE) {
			mHandler.removeMessages(MessageType.NAV_ADUST_VOLUME_DIMISS);
			unlockAdjustVolLayoutRect();
		}
	}

	private void hideZoomView() {
		if (mZoomView != null && mZoomView.getVisibility() != View.GONE) {
			mHandler.removeMessages(MessageType.NAV_ZOOMVIEW_DIMISS);
			mZoomView.setVisibility(View.GONE);
		}
	}

	private void hideSundryLayout() {
		if (mSundryLayout != null && mSundryLayout.getVisibility() != View.GONE) {
			mHandler.removeMessages(MessageType.NAV_SHORTTIP_TEXTVIEW_DIMISS);
			unlockSundryLayoutRect();
		}
	}

	private void unlockSundryLayoutRect() {
		if (mSundryLayout.getVisibility() != View.GONE) {
		mSundryLayout.setVisibility(View.GONE);
		mBypassWindowManager.setBypassWindow(false, mSundryLayoutWindowId,
				mSundryLayoutRect);
	}
}
	private void getVolumeViewRec() {
		mAdjustVolumeViewWindowId = mBypassWindowManager.getAvailableWindowId();
		if (!isAdjustVolumeViewFirst) {
			mBypassWindowManager.setBypassWindow(true,
					mAdjustVolumeViewWindowId, mAdjustVolumeViewRect);
		} else {
			ViewTreeObserver vto = mAdjustVolLayout.getViewTreeObserver();
			vto.addOnGlobalLayoutListener(new OnGlobalLayoutListener() {
				@Override
				public void onGlobalLayout() {
					if (isAdjustVolumeViewFirst) {
						isAdjustVolumeViewFirst = false;
						mAdjustVolumeViewRect = new Rect();
						adjustVolumeView
								.getGlobalVisibleRect(mAdjustVolumeViewRect);
						mBypassWindowManager.setBypassWindow(true,
								mAdjustVolumeViewWindowId,
								mAdjustVolumeViewRect);
					}
				}
			});
			
		}
	}
	private static void unlockAdjustVolLayoutRect() {
		if (mAdjustVolLayout.getVisibility() != View.GONE) {
			if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().adjustVolExitAnimation( mAdjustVolLayout, new AnimatorListenerAdapter() {
				@Override
				public void onAnimationEnd(Animator animation) {
					mAdjustVolLayout.setVisibility(View.GONE);
					super.onAnimationEnd(animation);
				}
			});
			}else{
				mAdjustVolLayout.setVisibility(View.GONE);
			}
			MtkLog.i(TAG, "hide mAdjustVolLayout unlock");
			mBypassWindowManager.setBypassWindow(false,
					mAdjustVolumeViewWindowId, mAdjustVolumeViewRect);
		}
	}
	private void getSundryLayoutRec() {
		mSundryLayoutWindowId = mBypassWindowManager.getAvailableWindowId();
		if (mSundryLayoutRect != null) {
			mBypassWindowManager.setBypassWindow(true, mSundryLayoutWindowId,
					mSundryLayoutRect);
		} else {
			ViewTreeObserver vto = mNavSundryShowTextView.getViewTreeObserver();
			vto.addOnGlobalLayoutListener(new OnGlobalLayoutListener() {
				@Override
				public void onGlobalLayout() {
					if (isSundryLayoutFirst) {
						isSundryLayoutFirst = false;
						mSundryLayoutRect = new Rect();
						mNavSundryShowTextView
								.getGlobalVisibleRect(mSundryLayoutRect);
						mBypassWindowManager.setBypassWindow(true,
								mSundryLayoutWindowId, mSundryLayoutRect);
					}
				}
			});
		}
	}

	private void hideChannelList() {
		if (mShowChannelListView != null && mShowChannelListView.isShowing()) {
			mShowChannelListView.exit();
		}
	}

	private void hideTypeList() {
		/*if (mShowChannelTypeView != null && mShowChannelTypeView.isShowing()) {
			mShowChannelTypeView.dismiss();
		}*/
	}

	private void hideFavoriteList() {
		if (mShowFavoriteChannelListView != null
				&& mShowFavoriteChannelListView.isShowing()) {
			mShowFavoriteChannelListView.dismiss();
		}
	}

	private void hideSourceList() {
		if (mShowSourceListView != null && mShowSourceListView.isShowing()) {
			mShowSourceListView.exit();
		}
		unlockAdjustVolLayoutRect();
	}

	private void cancelNumChangeChannel() {
		mNumputChangeChannel = false;
		mBannerView.hideAllBanner();
		mHandler.removeMessages(MessageType.NAV_NUMKEY_CHANGE_CHANNEL);
		inputChannelNumStrBuffer.replace(0, inputChannelNumStrBuffer.length(),
				"");
	}

	public static void switchMute() {
		// volume no set
		unlockAdjustVolLayoutRect();
		
		if (CecManager.getInstance()!=null &&CecManager.getInstance().isAmpVolume()) {
		    CecManager.getInstance().changeVolume(CecManager.Volume_Mute);
        }else {
            if (!mNavIntegration.iSetMute()) {
                mMuteImageView.setVisibility(View.GONE);
            } else {
                mMuteImageView.setVisibility(View.VISIBLE);
            }
        }

	}
	
    public void switchAmpMute() {
        // volume no set
        unlockAdjustVolLayoutRect();

        if (!CecManager.getInstance().getVolumeAmpMute()) {
            mMuteImageView.setVisibility(View.GONE);
        } else {
            mMuteImageView.setVisibility(View.VISIBLE);
        }

    }

	public void setSnowTextTagByState(int state) {
		switch (state) {
		case BannerView.SPECIAL_NO_CHANNEL:
			mSpecial_SnowTextView.showSpecialView(ShowType.SPECIAL_PLEASE_SCAN);
			break;
		case BannerView.SPECIAL_NO_SIGNAL:
			mSpecial_SnowTextView.showSpecialView(ShowType.SPECIAL_NO_SIGNAL);
			break;
		case BannerView.SPECIAL_NO_SUPPORT:
			mSpecial_SnowTextView.showSpecialView(ShowType.SPECIAL_NO_SUPPORT);		
			break;
			default:
				break;
		}
	}

	void setTimeZone() {
		int mApkFirstRun = saveV.readValue("apkfirstrun");
		if (mApkFirstRun == 0) {
			saveV.saveValue("apkfirstrun", 5);
			AlarmManager alarm = (AlarmManager) this
					.getSystemService(Context.ALARM_SERVICE);
			alarm.setTimeZone("Asia/Shanghai");

		}
	}
	
	boolean isNormalForZoom(){
		boolean hasChannels;
		if(mNavIntegration.isCurrentSourceTv()){
			if(mNavIntegration.iGetChannelLength() >0){
				hasChannels = true;
			}else{
				hasChannels = false;
			}
		}else{
			hasChannels = true;
		}
		return mNavIntegration.isCurrentSourceBlocking()
		|| mNavIntegration.isCurrentChannelBlocking()
		|| !mNavIntegration.iCurrentInputSourceHasSignal()
		|| !mNavIntegration.iCurrentChannelHasSignal()
		|| mNavIntegration.isAudioScrambled()
//		|| mNavIntegration.isCurrentSourceHDMI()
		|| mZoomView.isNotShow()
		|| !hasChannels
//		|| !mNavIntegration.aspectRatioCanSet()
		|| mNavIntegration.isCurrentChannelRadio()
		|| mPippopUiLogic.isPipOrPopState();
	}

	/*sync time from menu->auto synchorization and Launcher ->settings ->Date &time*/
	/*private void syncSysTime() {
		int autoSyncValue = SaveValue.getInstance(this).readValue(
				MenuConfigManager.AUTO_SYNC);
		int sysSyncValue = Settings.System.getInt(getContentResolver(),
				Settings.System.AUTO_TIME, 1);
		if (autoSyncValue == 2) {
			Settings.System.putInt(getContentResolver(),
					Settings.System.AUTO_TIME, 1);
		}else{
			Settings.System.putInt(getContentResolver(),
					Settings.System.AUTO_TIME, 0);
		}
		if (sysSyncValue == 1) {
			SaveValue.getInstance(this).saveValue(MenuConfigManager.AUTO_SYNC,
					2);
		}
	}*/
	private boolean isChangeChannelResponse() {
		if (OUT_PUT_SUB.equals(mPippopUiLogic.getOutputFocus())
				&& (INPUT_DTV.equals(mPippopUiLogic.getSubOutputSourceName()) || INPUT_ATV
						.equals(mPippopUiLogic.getSubOutputSourceName()))) {
			return true;
		} else if (OUT_PUT_MAIN.equals(mPippopUiLogic.getOutputFocus())
				&& (INPUT_DTV.equals(mPippopUiLogic.getMainOutputSourceName()) || INPUT_ATV
						.equals(mPippopUiLogic.getMainOutputSourceName()))) {
			return true;
		}
		return false;
	}
	
	private boolean showCurrentInputsourceLocked(int keyValue){
		if(null != mInputPwdView && mInputPwdView.isShowing()){
			MtkLog.d(TAG,"come in showCurrentInputsourceLocked dismiss pwd");
			mInputPwdView.dismiss();
		}
		boolean sourceBlocked = false;
		if(OUT_PUT_SUB.equals(mPippopUiLogic.getOutputFocus())){
			try {
				sourceBlocked = TVCommonNative.getDefault(this).isInputSourceBlocked(mPippopUiLogic.getSubOutputSourceName());
				MtkLog.d(TAG,"come in showCurrentInputsourceLocked OUT_PUT_SUB.equals(mPippopUiLogic.getOutputFocus() = " + sourceBlocked);
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}else{
			try {
				sourceBlocked = TVCommonNative.getDefault(this).isInputSourceBlocked(mPippopUiLogic.getMainOutputSourceName());
				MtkLog.d(TAG,"come in showCurrentInputsourceLocked OUT_PUT_MAIN.equals(mPippopUiLogic.getOutputFocus() = " + sourceBlocked);
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		if(sourceBlocked){
			if(keyValue != KeyEvent.KEYCODE_MTKIR_SWAP){
				mInputPwdView.show();
			}
			MtkLog.d(TAG,"come in showCurrentInputsourceLocked show pwd");
			return true;
		}else{
			return false;
		}
	}
	
	private boolean showCurrentChannelLocked(){
		if(null != mInputPwdView && mInputPwdView.isShowing()){
			mInputPwdView.dismiss();
		}
		boolean channelBlocked = mNavIntegration.isCurrentChannelBlocked();
		if(channelBlocked){
			mInputPwdView.show();
			return true;
		}else{
			return false;
		}
	}
	private void channelListViewShow(){
		if (mShowSourceListView.isShowing() == false
				&& mNavIntegration.isCurrentSourceTv()) {
			if (mShowChannelListView.isShowing() == false
					&& mNavIntegration.isCurrentSourceTv()) {
				if (mNavIntegration.iGetChannelLength() > 0) {
					hideZoomView();
					hideAdustVolLayout();
					hideSundryLayout();
					hideTimeShiftInfoBar();
					mSpecial_SnowTextView.setVisibility(View.GONE);
					mShowChannelListView.show();
					mShowChannelListViewRect = new Rect(
							mShowChannelListView.x
									- mShowChannelListView.menuWidth / 2
									+ ScreenConstant.SCREEN_WIDTH / 2,
							mShowChannelListView.y
									- mShowChannelListView.menuHeight / 2
									+ ScreenConstant.SCREEN_HEIGHT / 2,
							mShowChannelListView.x
									+ mShowChannelListView.menuWidth / 2
									+ ScreenConstant.SCREEN_WIDTH / 2,
							mShowChannelListView.y
									+ mShowChannelListView.menuHeight / 2
									+ ScreenConstant.SCREEN_HEIGHT / 2);
					MtkLog.i(TAG, "mShowChannelListViewRect  left:"
							+ mShowChannelListViewRect.left + "  top "
							+ mShowChannelListViewRect.top + "  right "
							+ mShowChannelListViewRect.right + " bottom"
							+ mShowChannelListViewRect.bottom);
					mShowChannelListWindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBypassWindowManager.setBypassWindow(true,
							mShowChannelListWindowId,
							mShowChannelListViewRect);
				} else {
					mSpecial_SnowTextView.setVisibility(View.GONE);
					mBannerView.show(false, -1, false);
				}
			}
		}
	}
	
	public static void reshowFoucs(){
		if(mPippopUiLogic.isPipOrPopState()){
			mPippopState.getmController().reDrawFocus();
		}
	}
	/**
	 * @return the mTshiftManager
	 */
	public TimeShiftManager getTshiftManager() {
		return mTshiftManager;
	}

	/**
	 * @param mTshiftManager
	 *            the mTshiftManager to set
	 */
	public void setTshiftManager(TimeShiftManager mTshiftManager) {
		this.mTshiftManager = mTshiftManager;
	}
	
	private void registShutdownReceive() {
        // TODO Auto-generated method stub
        IntentFilter shutdownFilter = new IntentFilter();
        shutdownFilter.addAction(Intent.ACTION_SHUTDOWN);
        shutdownFilter.setPriority(IntentFilter.SYSTEM_LOW_PRIORITY);
        registerReceiver(new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
               Log.e("onReceive", "shutdown");
               if (CecManager.getInstance()!=null) {
                CecManager.getInstance().exit();
            }
               
            }
        }, shutdownFilter);
    }
	
	public void resume(){
		if((null != mCheckLockSignalChannelState) && !mPippopUiLogic.isPipOrPopState()){
			mCheckLockSignalChannelState
        		.checkLockedSignStateOrHasChannel(false);
		}	
	}
	public static TurnkeyUiMainActivity getInstance(){
	    if (mainActivity==null) {
	            mainActivity = new TurnkeyUiMainActivity();
        }
	    return mainActivity;
	}
	public AdjustVolumeView getVolumeView(){
	    return adjustVolumeView;
	}
	public BannerView getBannerView(){
		return mBannerView;
	}
	
	private void hideTimeShiftInfoBar() {
		if ((null != getTshiftManager())
				&& (null != getTshiftManager().getUiManager())) {
			getTshiftManager().getUiManager().dissmiss();
		}
	}
	
	private void hideViewsWithChangeChannel(){
		hideZoomView();
		hideSundryLayout();
		hideTimeShiftInfoBar();
	}
}
