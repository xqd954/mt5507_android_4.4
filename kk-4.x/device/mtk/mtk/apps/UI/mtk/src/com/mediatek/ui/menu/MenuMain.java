package com.mediatek.ui.menu;

import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.app.Activity;
import android.app.StatusBarManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.SystemProperties;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;
import android.view.ViewTreeObserver.OnPreDrawListener;
import android.view.animation.AccelerateInterpolator;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListAdapter;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVChannelManager;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVChannelSelector.SelectorListener;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcommon.TVCAMManager;
import com.mediatek.tvcommon.TVCAMManager.CAMMenu;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVConfigurer;
import com.mediatek.tvcommon.TVOptionRange;
import com.mediatek.tvcommon.TVOutputCommon.AdjustListener;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.tvcommon.TVChannelState;
import com.mediatek.tvcommon.TVChannelState.ChannelListener;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem;
import com.mediatek.ui.menu.adapter.SetConfigListViewAdapter.DataItem.DataType;
import com.mediatek.ui.menu.commonview.AnalogProgressView;
import com.mediatek.ui.menu.commonview.CIMainView;
import com.mediatek.ui.menu.commonview.ChangeValueBarView;
import com.mediatek.ui.menu.commonview.ChannelListPoweronView;
import com.mediatek.ui.menu.commonview.ChannelMenuView;
import com.mediatek.ui.menu.commonview.ChannelMenuViewBottom;
import com.mediatek.ui.menu.commonview.CiDialog;
import com.mediatek.ui.menu.commonview.CustListView;
import com.mediatek.ui.menu.commonview.DateTimeInputView;
import com.mediatek.ui.menu.commonview.DateTimeView;
import com.mediatek.ui.menu.commonview.EffectOptionView;
import com.mediatek.ui.menu.commonview.HaveSubPageView;
import com.mediatek.ui.menu.commonview.KeyboardDialog;
import com.mediatek.ui.menu.commonview.ListViewItemView;
import com.mediatek.ui.menu.commonview.MenuMjcDemoDialog;
import com.mediatek.ui.menu.commonview.MenuSetUpListView;
import com.mediatek.ui.menu.commonview.MessageType;
import com.mediatek.ui.menu.commonview.NetUpdateGider;
import com.mediatek.ui.menu.commonview.NumView;
import com.mediatek.ui.menu.commonview.OptionView;
import com.mediatek.ui.menu.commonview.PasswordView;
import com.mediatek.ui.menu.commonview.RespondedKeyEvent;
import com.mediatek.ui.menu.commonview.ScanChannelsOptionView;
import com.mediatek.ui.menu.commonview.SwitchOptionView;
import com.mediatek.ui.menu.commonview.TVScan;
import com.mediatek.ui.menu.commonview.TVSourceView;
import com.mediatek.ui.menu.commonview.TabMenuButtonBar;
import com.mediatek.ui.menu.commonview.TextCommView;
import com.mediatek.ui.menu.commonview.ToastDialog;
import com.mediatek.ui.menu.commonview.TurnkeyCommDialog;
import com.mediatek.ui.menu.commonview.TurnkeyCommFinetune;
import com.mediatek.ui.menu.commonview.UpdateTime;
import com.mediatek.ui.menu.util.EditChannel;
import com.mediatek.ui.menu.util.FactoryPresetChannel;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.MenuDataContainer;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.menu.util.TVCommonInterface;
import com.mediatek.ui.menu.util.TimeConsume;
import com.mediatek.ui.mmp.MeidaMainActivity;
import com.mediatek.ui.nav.EPGActivity;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.NavIntegration.IInputSourceChangeListener;
import com.mediatek.ui.nav.util.CaptureLogoImp;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.nav.util.NewPipLogic;
import com.mediatek.ui.setup.Loading;
import com.mediatek.ui.setup.SetupWizardActivity;
import com.mediatek.ui.util.AnimationManager;
import com.mediatek.ui.util.BypassWindowManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

/**
 * The activity of whole Menu
 * 
 * @author hs_haosun menu main layout
 */

public class MenuMain extends Activity implements View.OnKeyListener,
		View.OnFocusChangeListener {

	private static MenuMain menuMainInstance;
	private static final String TAG = "MenuMain";
	private static final String CHILD_DISABLE = "child_disable";
	private static final String BACKEDITVIEW = "backEditView";
	public static final String OUTPUT_SIGNAL_CHANGE = "OUTPUT_SIGNAL_CHANGE";
	private TabMenuButtonBar mTabMenuButtonBar;// MenuButtonBar the top menu bar
	private ImageView mMenuImageView;// Upper left corner of picture frame

	private MenuSetUpListView mMenuSetUpListView; // Bottom right shows the
	// expansion of entries to
	// set parameters ListView
	private RelativeLayout mFrameLayout;// Lower right window layout
	private int selIdx = 0;
	private boolean isTab = true;
	private boolean isMouse = false;
	private static boolean hasChangeFreq = false;
//	private boolean isDhcpRegistered = false;
	private String pinCode;
	private boolean isScanning = false;
	private boolean isProgress = false;
	private boolean isDeleting = false;
	private List<String> netSetbackItems;

	// Lower left corner of the prompt text function
	private TextView mTextViewEnter;
	private TextView mTextViewSelect;
	private TextView mTextViewExit;
	private LinearLayout mRootRelativeLayout;// The root of the tree view
	private BypassWindowManager mBypassWindowManager;
	private Rect mRootRelativeLayoutRect;
	private int mRootLayoutwindowId = -1;
	private Boolean isRootRelativeLayoutFirst = true;

	private Rect mChangeviewRect;
	private int mChangeviewWindowId = -1;
	private Boolean isChangeviewFirst = true;
	private Boolean isChangeviewVisible = false;
	private Boolean ismWifiSettingLayoutFirst = true;
	private Boolean ismWifiSettingLayoutVisible = false;
	private Rect mTurnkeyCommFinetuneRect;
	private int mTurnkeyCommFinetuneWindowId = -1;
	private LinearLayout mTabMenuButtonBarRootView;// View toolbar layout root
	private ChangeValueBarView mChangeview;
	/** menumain variable data */

	private Context mContext;
	private int mCurrentSelectedPosition = 0; // Location currently selected
	// ListView
	private int mBeforeSelectedPosition = 0;
	private int mCurrentFouceButtonID = 1;
	// Data relevant variables
	private SetConfigListViewAdapter mListViewAdpter;
	private List<SetConfigListViewAdapter.DataItem> mGroup;
	private ParentData mFirstLevelData = new ParentData();// A menu of data
	private ParentData mSecondLevelData = new ParentData();// The second level
	// of menu data
	private ParentData mThirdLevelData = new ParentData();// The third level of
	// menu data
	private ParentData mForthLevelData = new ParentData(); // The fourth level
	// of the menu data
	private ParentData mFifthLevelData = new ParentData();// The fifth level of
	// the menu data
	private ParentData mSixthLevelData = new ParentData();// The sixth level of
	// the menu data
	private MenuDataContainer mDataContainer;
	private MyUpdata mMyUpData;// Responsible for the ListView page, the update
	// data Adpter
	private SetConfigListViewAdapter.DataItem mListViewSelectedItemData;
	private RespondedKeyEvent childView;
	private Map<Integer, String> mTitleMap = new HashMap<Integer, String>();

	private int mExitLevel = 0;// Remember the ListView level
	private int mPageSize = 9;// Number of records per page
	private static final int NORMALPAGE_SIZE = 9;// the page size for n page
	private static final int CHANNELPAGE_SIZE = 7;// the page size for channel
	// page
	private boolean m3d_enable_page = true;
	/** part of scanner tv */
	// Scan station
	private AnalogProgressView angProgressView;// Scan station class
	private Loading loading;
	private TextView mStateTextView;// Scan status text
	private TextView mAnaloguechannel;// Analog channel num
	private TextView mNumberChannel;
	private int mPercentage;// percent Complete
	private TextView mFinishpercentage;
	private Handler progressHandler;
	private AnalogProgressView analogScanBar;// Progress of Manual scan station
	private TextView text1; 
	private TextView text2;
	private NumView mNumView = null;
	private NumView mNumView2=null;
	// Channel Edit
	private int switchChannel_from;// Record the first position channel to
	// exchange
	private int switchChannel_from_page;// Record the page number where the
	// channel to exchange
	private int switchChannel_to;// Record the to position channel to exchange
	private int switchChannel_to_page;// 
	private String switchChannel_from_num;// Record exchange channel num
	private String switchChannel_to_num;// Records are exchanged channel num
	private int insertChannel_from;
	private int insertChannel_from_page;
	private int insertChannel_to;
	private int insertChannel_to_page;
	private String keyColor = "";
	private String[] oldChannelNum;
	private int currentChannelNum = 0;
	private int gotoPage = 0;
	private int gotoPosition = 0;
	private int goToSourcePosition = 0;
	private String backEditView = "";
	private int mChannelPownBrePositon = 1;
	private EditChannel mEditChannel;
	private static int freFrom;
	private static int freTo;
	private static int rfchanel;
    private static int sysrate;
	/** Parental control part */
	private String[] channelPowerOnArray = null;

	/** setup part of the */
//	public static boolean mInterruptExit = false; // Activity exit,non-users
	private TurnkeyCommDialog autoAdjustDialog;
	private Rect autoAdjustDialogRect;
	private int autoAdjustDialogWindowId = -1;
	private boolean mIPInputBoxBlockUPDownKeyEvent = false;
	private String application_subpage = "";
	private String[] ipSettingString = new String[5];
	private boolean goBack = true;

	/** factory parts */
	private TurnkeyCommDialog factroyCofirm;
	private Rect factroyCofirmRect;
	private int factroyCofirmWindowId = -1;
	private LinearLayout linearScan, linearList, versionInfoLinear;
	private int editChannelView = 0;
	private ChannelMenuViewBottom editChannelViewBottom;
	private ChannelMenuView editChannelViewMenu;
	private TimeConsume mTimeConsume;
	private float numHz = 10;
	private int zeroTimes = 0;

	// after clean channel list, Channel Edit(index 4) and Clear Channel
	// List(index 5) state is disable
	private int[] TV_DISABLE_ITEMS = new int[2];

	private int[] IPINPUT_ITEMS = { 1, 2, 3, 4 };

	private TVCommonInterface tvCommon;

	/** common logic part */
	private TVScan mScanDialog;
	private TVChannelSelector mChSel;
	private TVContent mTV;
	private TVChannelManager mTM;
	private MenuConfigManager mcf;

	private Thread myThread;
	Handler handler;

	// common interface
	private CIMainView ciMainView;
	private TVCAMManager mCAMManager;
	// reading CI info or not
	private boolean mReadingCIInfo = false;
	private CIStatusBroadcastReceiver mCiStatusBroadcastReceiver;
	private SignalChangeReceiver mSignalChangeReceiver;
	// come from nav
	private boolean mCIStatusNotice = false;
	// CI show from nav,use in press exit,finish menu or not
	public static boolean mCINav = false;
	// scanning or not
	public static boolean mScanningStatus = false;
	// left layout show"enter""select""exit"
	private RelativeLayout mLeftLayout;
	// edit channel
	private TurnkeyCommDialog editChSameNumDialog;
	private Rect editChSameNumDialogRect;
	private int editChSameNumDialogWindowId = -1;
	// save value
	private SaveValue saveValue;
	private NavIntegration mNavIntegration;

	public boolean goToChannelEditorTag = false;
	public boolean exitEditChannelNeedCI = false;
	private boolean loseFoucs = false;
	
	private boolean isPressChUPAndChDN = false;
	private boolean isEditChannelToCI = false;
	private boolean isChangeChannel = true;
	private boolean mFromMmp;
	private StatusBarManager mStatusBarManager;


	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {

			// show CI name
			case MessageType.CI_NAME_MESSAGE:
				mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
				mHandler.sendEmptyMessageDelayed(MessageType.MENU_AUTO_EXIT_MESSAGE,
						MessageType.delayMillis2);
				if(isEditChannelToCI){
					mTabMenuButtonBar.mButtonShow.setText(R.string.menu_tab_setup);
				}else{
				mTitleMap.put(mExitLevel - 1, mTabMenuButtonBar.mButtonShow
						.getText().toString());
				mTabMenuButtonBar.mButtonShow
						.setText(mListViewSelectedItemData.mName);
				}
				// no card
				if (mCAMManager.isSlotActive() == false) {
					ciMainView.showChildView(MessageType.CI_DATA_TYPE_NO_CARD);
					ciMainView
							.showNoCardInfo(getString(R.string.menu_setup_ci_no_card));
					// has card
				} else if (TurnkeyUiMainActivity.mCIStatus == true
						|| mCAMManager.isSlotActive() == true) {
					mReadingCIInfo = true;
				//	ciMainView.requestFocus();
				//	ciMainView.setCIOnKyListener();
					ciMainView.showChildView(MessageType.CI_DATA_TYPE_NO_CARD);
					ciMainView.showNoCardInfo(mCAMManager.getCamName());
						ciMainView.requestFocus();
					ciMainView.setCIOnKyListener();
				}

				ciMainView.registerNosignalReceiver();
				ciMainView.setVisibility(View.VISIBLE);
				linearList.setVisibility(View.GONE);
				break;

			case MessageType.EDITCHANNEL_NEED_CI:
				mCAMManager = TVContent.getInstance(mContext).getCAMManager();
				if (TurnkeyUiMainActivity.mCIStatus == true
						|| mCAMManager.isSlotActive() == true) {
					exitEditChannelNeedCI = true;
					if(ciMainView == null){
						return;
					}
					ciMainView.registerNosignalReceiver();
					ciMainView.setVisibility(View.VISIBLE);

					mCAMManager.enterCAMMainMenu();
					//ciMainView.showChildView(MessageType.CI_DATA_TYPE_MENU);

					CAMMenu mCIManager = mCAMManager.getCamMenu();
					//ciMainView.showChildView(MessageType.CI_DATA_TYPE_MENU);

					ciMainView.showCiMenuInfo(mCAMManager.getCamName(),
							mCIManager.getTitle(), mCIManager.getSubtitle(),
							mCIManager.getBottom(),
							mContext.getResources().getStringArray(
									R.array.menu_ci_not_support_channel));
		
					MenuMain.mCINav = false;
				}
				break;
			case MessageType.FROM_MENUMAIN_TO_TK:
				Intent intent = new Intent();
				intent.putExtra("FROM_MENUMAIN_TO_TK", true);
				intent.setClass(MenuMain.this,TurnkeyUiMainActivity.class);
				setResult(0,intent);
				mBypassWindowManager.setBypassWindow(false,
						mRootLayoutwindowId, mRootRelativeLayoutRect);
				exit();
				break;
			// pull out card
			case MessageType.CI_PAGE_EXIT_MESSAGE:
				if (ciMainView.getVisibility() == View.VISIBLE) {
					ciMainView.showChildView(MessageType.CI_DATA_TYPE_NO_CARD);
					ciMainView
							.showNoCardInfo(getString(R.string.menu_setup_ci_no_card));
					mReadingCIInfo = false;
				}
				return;

			case MessageType.WAIT_DELAY_MESSAGE:
				mBypassWindowManager.setBypassWindow(false,
						mRootLayoutwindowId, mRootRelativeLayoutRect);
				exit();
				break;
			case MessageType.MENU_AUTO_EXIT_MESSAGE:
				if (mScanDialog != null) {
					mScanDialog.dismiss();
				}
				mBypassWindowManager.setBypassWindow(false,
						mRootLayoutwindowId, mRootRelativeLayoutRect);
				exit();
				break;
			case MessageType.MESSAGE1:
				if (mChangeview != null) {
					mChangeview.setVisibility(View.GONE);
					mBypassWindowManager.setBypassWindow(false,
							mChangeviewWindowId, mChangeviewRect);
					isChangeviewVisible = false;
					mListViewAdpter.notifyDataSetChanged();
				}
				if (mRootRelativeLayout != null) {
					mRootRelativeLayout.setVisibility(View.VISIBLE);
					mRootLayoutwindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBypassWindowManager.setBypassWindow(true,
							mRootLayoutwindowId, mRootRelativeLayoutRect);
				}
				DataItem parentDataItem = mListViewSelectedItemData.mParent;
				if (parentDataItem != null
						&& parentDataItem.getmInitValue() != parentDataItem.userDefined) {
					((EffectOptionView) mMenuSetUpListView
							.getChildAt(parentDataItem.getPosition()))
							.showValue(mDataContainer
									.getValueById(parentDataItem.getmItemID()));
				}
				isProgress = false;
				mMenuSetUpListView.requestFocus();
				mMenuSetUpListView.setSelection(msg.arg1);
				selIdx = mCurrentSelectedPosition;
				mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
				mHandler.sendEmptyMessageDelayed(
						MessageType.MENU_AUTO_EXIT_MESSAGE,
						MessageType.delayMillis2);
				break;
			case MessageType.MENU_TV_SCAN_COMPLETED:
				if (msg.arg1 == AnalogProgressView.ATV_SCAN_COMPLETED) {
					   updateATVState(ConfigType.BS_SRC_CABLE);
				} else if (msg.arg1 == AnalogProgressView.DTV_SCAN_COMPLETED) {
						updateDTVState(msg.arg2);
				}
				break;
			case MessageType.MENU_TV_RF_SCAN_CONNECTTURN:
				MtkLog.d(TAG, "handle MENU_TV_RF_SCAN_CONNECTTURN");
				MtkLog.d(TAG, "iGetCurrentSouce:"+mNavIntegration.iGetCurrentSouce());
				MtkLog.d(TAG, "iCurrentInputSourceHasSignal:"+mNavIntegration.iCurrentInputSourceHasSignal());
				if (hasChangeFreq || mNavIntegration.iGetCurrentSouce() == null
						|| !mNavIntegration.iCurrentInputSourceHasSignal()) {
					MtkLog.d(TAG, "changeFreqById");
					 mEditChannel.changeFreqById();
				}			
			    removeMessages(MessageType.MENU_TV_RF_SCAN_REFRESH);
                sendEmptyMessage(
                        MessageType.MENU_TV_RF_SCAN_REFRESH);
			    break;
			case MessageType.MENU_TV_RF_SCAN_REFRESH:
			    if (linearScan.getVisibility() == View.VISIBLE) {
				int signalLevel =mEditChannel.getSignalLevel();
				int signalQuality =mEditChannel.getSignalQuality();
				MtkLog.d("fff", "MenuMain---------handled--MessageType.MENU_TV_RF_SCAN_REFRESH---signalLevel--->"+signalLevel+"signalQuality:"+signalQuality);
				RespondedKeyEvent signalLevelChildView = (RespondedKeyEvent)mMenuSetUpListView.getChildAt(1);
				RespondedKeyEvent signalQualityChildView =(RespondedKeyEvent)mMenuSetUpListView.getChildAt(2);
				if(signalLevelChildView !=null&& signalQualityChildView !=null){
					MtkLog.d("fff", "MenuMain---------handled--MessageType.MENU_TV_RF_SCAN_REFRESH---signalLevel--->"+signalLevel);
					signalLevelChildView.showValue(signalLevel);
					signalQualityChildView.showValue(signalQuality);
				}
				 removeMessages(MessageType.MENU_TV_RF_SCAN_REFRESH);
                 sendEmptyMessageDelayed(
                         MessageType.MENU_TV_RF_SCAN_REFRESH,
                         MessageType.delayMillis6);
				}else {
				    removeMessages(MessageType.MENU_TV_RF_SCAN_REFRESH);
                }
				break;
			default:
				break;
			}
			super.handleMessage(msg);
		}
	};

	protected void onCreate(Bundle savedInstanceState) {
		menuMainInstance = MenuMain.this;
		mBypassWindowManager = BypassWindowManager.getInstance(mContext);
		mTV = TVContent.getInstance(MenuMain.this);
		mChSel = mTV.getChannelSelector();
		mTM = mTV.getChannelManager();
		//Redundant code  
		//mTM.getChannels().size();
		mEditChannel = EditChannel.getInstance(MenuMain.this);
		saveValue = SaveValue.getInstance(this);
		mNavIntegration = NavIntegration.getInstance(this);
		if (mNavIntegration.isCurrentSourceDTV()) {
			TV_DISABLE_ITEMS[0] = 2;
			TV_DISABLE_ITEMS[1] = 3;
		} else {
			TV_DISABLE_ITEMS[0] = 3;
			TV_DISABLE_ITEMS[1] = 4;
		}

		if (NavSundryImplement.getInsNavSundryImplement(this).isFreeze()) {
			CaptureLogoImp.getInstance(this).freezeScreen(false);
		}

		mCAMManager = TVContent.getInstance(this).getCAMManager();
		tvCommon = TVCommonInterface.getInstance(MenuMain.this);
		mCiStatusBroadcastReceiver = new CIStatusBroadcastReceiver();
		registerReceiver(mCiStatusBroadcastReceiver, new IntentFilter(
				TurnkeyUiMainActivity.CISTATUS_REMOVE));

		mSignalChangeReceiver = new SignalChangeReceiver();
		registerReceiver(mSignalChangeReceiver, new IntentFilter(
				OUTPUT_SIGNAL_CHANGE));

		super.onCreate(savedInstanceState);
		setContentView(R.layout.menu_main);
		// get current source
		String mCurrentSource = mTV.getInputManager()
				.getCurrInputSource("main");
		mDataContainer = MenuDataContainer.getInstance(this);
		mDataContainer.mCurrentSource = mCurrentSource;
		mContext = this;
		mMyUpData = new MyUpdata();
		Bundle mBundle = getIntent().getExtras();
//		if (savedInstanceState != null) {
//			mCurrentFouceButtonID = savedInstanceState
//					.getInt("mCurrentFouceButtonID");
//			mCurrentSelectedPosition = savedInstanceState
//					.getInt("mCurrentSelectedPosition");
//			mExitLevel = savedInstanceState.getInt("mExitLevel");
//			mInterruptExit = savedInstanceState.getBoolean("mInterruptExit");
//		} else {
			if (null != mBundle) {
				int mCIValue = mBundle.getInt("CIValue");
				// camNamed
				// source is TV
				// not scanning
				if (mCIValue == 1) {
					mCurrentFouceButtonID = 4;
					mCurrentSelectedPosition = 8;
					mCIStatusNotice = true;
					mExitLevel = 1;
					mCINav = true;
				}else if(mCIValue == 2){
					isEditChannelToCI = true;
					mCurrentFouceButtonID = 4;
					mCurrentSelectedPosition = 8;
					mCIStatusNotice = true;
					mExitLevel = 1;
					mCINav = true;
					mHandler.sendEmptyMessage(MessageType.EDITCHANNEL_NEED_CI);
				} else {
					mCINav = false;
				}
			}
//		}

		// init UI
		initUI();
		ciMainView.getMenuLeftLayout(mLeftLayout);
		// init data for menumain

		// when the source is not tv, the tv button is gone
		if ((!TVInputManager.INPUT_TYPE_TV.equals(mTV.getInputManager()
				.getTypeFromInputSource(mDataContainer.mCurrentSource)))
				|| mNavIntegration.isCurrentSourceBlocking()) {
			mTabMenuButtonBar.mButtonTV.setVisibility(View.GONE);
			mTabMenuButtonBar.mButtonAudio
					.setNextFocusRightId(R.id.widget_buttonSetUp);
			mTabMenuButtonBar.mButtonSetup
					.setNextFocusLeftId(R.id.widget_buttonAudio);
		}

		if (null != mBundle) {
			int mFromWhere = mBundle.getInt("fromwhere");
			if (mFromWhere == 1) {
				mFromMmp = true;
				mTabMenuButtonBar.mButtonTV.setVisibility(View.INVISIBLE);
				mTabMenuButtonBar.mButtonAudio.setVisibility(View.INVISIBLE);
				mTabMenuButtonBar.mButtonSetup.setVisibility(View.INVISIBLE);
				mTabMenuButtonBar.mButtonParental.setVisibility(View.INVISIBLE);
				TVContent.getInstance(this).setOpacity(200);
			}
		}else {
		    mFromMmp = false;
        }
		 MtkLog.d(TAG, "mFromMmp:"+mFromMmp);
		if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().startActivityEnterAnimation(this, findViewById(R.id.menumain_relativeLayout), null);
		}
		// eNetImp = EthernetImplement.getInstance(mContext);
	}



//	protected void onSaveInstanceState(Bundle outState) {
//		if (true == mInterruptExit) {
//			outState.putBoolean("mInterruptExit", true);
//			outState.putInt("mCurrentSelectedPosition", 0);
//			outState.putInt("mCurrentFouceButtonID", 4);
//			outState.putInt("mExitLevel", 1);
//		}
//		super.onSaveInstanceState(outState);
//	}

	protected void onResume() {
		mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
		mHandler.sendEmptyMessageDelayed(MessageType.MENU_AUTO_EXIT_MESSAGE,
				MessageType.delayMillis2);
//		mStatusBarManager = (StatusBarManager) getSystemService(Context.STATUS_BAR_SERVICE);
//		mStatusBarManager.setSystemUiBarHight(0);
		getRootRelativeLayoutRect();
		super.onResume();
	}

	private void initUI() {
		mTabMenuButtonBar = (TabMenuButtonBar) findViewById(R.id.menumain_tabmenubar);
		mTabMenuButtonBar.setLayoutParams(new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT,ScreenConstant.SCREEN_HEIGHT/10));
		mMenuSetUpListView = (MenuSetUpListView) findViewById(R.id.menumain_listView);
		tempListView=(MenuSetUpListView) findViewById(R.id.menumain_listView_temp);
		mMenuSetUpListView.setOnItemSelectedListener(mListItemSelectListener);
		mMenuSetUpListView.setOnFocusChangeListener(mOnFocusChangeListener);
		mFrameLayout = (RelativeLayout) findViewById(R.id.menumain_framLayout);
		mRootRelativeLayout = (LinearLayout) findViewById(R.id.layout_root);
		RelativeLayout.LayoutParams rParams=new RelativeLayout.LayoutParams(
				(int)(ScreenConstant.SCREEN_WIDTH*0.87), (int)(ScreenConstant.SCREEN_HEIGHT*0.91));
		rParams.addRule(RelativeLayout.CENTER_IN_PARENT);
		mRootRelativeLayout.setLayoutParams(rParams);
		mTabMenuButtonBarRootView = (LinearLayout) mTabMenuButtonBar
				.getChildAt(0);
		analogScanBar = (AnalogProgressView) findViewById(R.id.menumain_factory_progresscomm);
		linearScan = (LinearLayout) findViewById(R.id.menumain_analog_layout);
		linearList = (LinearLayout) findViewById(R.id.menumain_contentlist_layout);
		versionInfoLinear = (LinearLayout) findViewById(R.id.menumain_version_layout);
		editChannelViewBottom = (ChannelMenuViewBottom) findViewById(R.id.menumain_channelmenu_bottom);
		editChannelViewMenu = (ChannelMenuView) findViewById(R.id.menumain_channelmenu);
		mMenuImageView = (ImageView) findViewById(R.id.menumain_left_icon);
		mTextViewEnter = (TextView) findViewById(R.id.menumain_left_enter);
		mTextViewSelect = (TextView) findViewById(R.id.menumain_left_select);
		mTextViewExit = (TextView) findViewById(R.id.menumain_left_exit);
		mLeftLayout = (RelativeLayout) findViewById(R.id.menumain_left_function);

		mChangeview = (ChangeValueBarView) findViewById(R.id.changeValueViewBar);
		int viewHeight = (int) (ScreenConstant.SCREEN_HEIGHT * 0.125);
		int wiewWeight = (int) (ScreenConstant.SCREEN_WIDTH * 0.64);
		RelativeLayout.LayoutParams lp = new RelativeLayout.LayoutParams(
				wiewWeight, viewHeight);
		lp.addRule(RelativeLayout.CENTER_HORIZONTAL);
		lp.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
		lp.bottomMargin = (int) (ScreenConstant.SCREEN_HEIGHT * 0.1);
		mChangeview.setLayoutParams(lp);
	
		channelPowerOnArray = MenuMain.this.getResources().getStringArray(
				R.array.menu_setup_power_on_channel_mode_array);
		mChangeview.setVisibility(View.GONE);
		mMenuSetUpListView.setOnKeyListener(this);
		mcf = MenuConfigManager.getInstance(this);
		mListViewAdpter = new SetConfigListViewAdapter(this);
		mLastAdapter=new SetConfigListViewAdapter(this);
		// register keyboard event listener for the toolbar's butttons
		setTabMenuButtonListener();
		if (!mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
				.hasFocus()) {
			mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
					.requestFocus();
		}
		mTimeConsume = new TimeConsume();
		ciMainView = (CIMainView) findViewById(R.id.menumain_ci_view);
		initScannerUi();
		blockAllTouch();

		addHasSignalListener();
		addSourceChangeListener();
	}

	private void blockAllTouch() {
		mMenuSetUpListView.setOnTouchListener(remainOnTouchListener);
		linearScan.setOnTouchListener(remainOnTouchListener);
	}

	private void showListView() {
		linearScan.setVisibility(View.GONE);
		editChannelViewMenu.setVisibility(View.GONE);
		versionInfoLinear.setVisibility(View.GONE);
		editChannelViewBottom.setVisibility(View.GONE);
		ciMainView.setVisibility(View.GONE);
		ciMainView.unRegisterNosignalReceiver();
		mMenuSetUpListView.setVisibility(View.VISIBLE);
		linearList.setVisibility(View.VISIBLE);
		mMenuSetUpListView.setFocusable(true);
		mMenuSetUpListView.requestFocus();
	}

	private OnFocusChangeListener mOnFocusChangeListener = new OnFocusChangeListener() {
		public void onFocusChange(View v, boolean hasFocus) {
			if (hasFocus) {
				if (null != mMenuSetUpListView
						&& null != mMenuSetUpListView.getSelectedItem()) {
							mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
					setFunctionTipVisibility(((DataItem) mMenuSetUpListView
							.getSelectedItem()).getDataType());
					tvCommon.resetKeyDelayTime(((DataItem) mMenuSetUpListView
							.getSelectedItem()).getmItemID());
				}
			} else {
				mListViewAdpter.setSelectPos(-1);
			}
		}
	};

	private OnItemSelectedListener mListItemSelectListener = new OnItemSelectedListener() {
		public void onItemSelected(AdapterView<?> parent, View view,
				int position, long id) {
			if (mMenuSetUpListView.hasFocus()) {
				if (mMenuSetUpListView.getChildAt(position) != null && !(mMenuSetUpListView.getChildAt(position) instanceof PasswordView) 
						&& !(mMenuSetUpListView.getChildAt(position) instanceof TextCommView)) {
					mListViewAdpter.setSelectPos(position);
				}
				setFunctionTipVisibility(((DataItem) mMenuSetUpListView
						.getSelectedItem()).getDataType());
				tvCommon.resetKeyDelayTime(((DataItem) mMenuSetUpListView
						.getSelectedItem()).getmItemID());
			}
		}

		public void onNothingSelected(AdapterView<?> parent) {
		}
	};

	private void initScannerUi() {
		mStateTextView = (TextView) findViewById(R.id.state);
		mFinishpercentage = (TextView) findViewById(R.id.finishpercentage);
		loading = (Loading) findViewById(R.id.setup_tv_scan_loading);
		mFinishpercentage.setText(mPercentage + "%");
		if (mNavIntegration.isCurrentSourceDTV()) {
			mStateTextView.setText(MenuMain.this
					.getString(R.string.menu_tv_single_rf_scan_init));
		} else {
			mStateTextView.setText(MenuMain.this
					.getString(R.string.menu_tv_analog_manual_scan_init));
		}
		mAnaloguechannel = (TextView) findViewById(R.id.analoguechannel);
		mNumberChannel = (TextView) findViewById(R.id.numberchannel);
	}

	private void setTabMenuButtonListener() {
		mTabMenuButtonBar.mButtonVideo.setOnFocusChangeListener(this);
		mTabMenuButtonBar.mButtonAudio.setOnFocusChangeListener(this);
		mTabMenuButtonBar.mButtonTV.setOnFocusChangeListener(this);
		mTabMenuButtonBar.mButtonSetup.setOnFocusChangeListener(this);
		mTabMenuButtonBar.mButtonParental.setOnFocusChangeListener(this);

		OnKeyListener TabOnKeyListener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					switch (keyCode) {
					case KeyEvent.KEYCODE_ENTER:
					case KeyEvent.KEYCODE_DPAD_CENTER:
						mMenuSetUpListView.requestFocus();
						mMenuSetUpListView.setSelection(0);
						if (mMenuSetUpListView.hasFocus()) {
							mExitLevel++;
							selIdx = 0;
						}
						break;
					case KeyEvent.KEYCODE_DPAD_DOWN:
						mExitLevel++;
						selIdx = 0;
						break;
					default:
						break;
					}
				}
				return false;
			}
		};

		mTabMenuButtonBar.mButtonVideo.setOnKeyListener(TabOnKeyListener);
		mTabMenuButtonBar.mButtonAudio.setOnKeyListener(TabOnKeyListener);
		mTabMenuButtonBar.mButtonTV.setOnKeyListener(TabOnKeyListener);
		mTabMenuButtonBar.mButtonSetup.setOnKeyListener(TabOnKeyListener);
		mTabMenuButtonBar.mButtonParental.setOnKeyListener(TabOnKeyListener);

		OnTouchListener onTouchListener = new OnTouchListener() {

			public boolean onTouch(View v, MotionEvent event) {
				// TODO Auto-generated method stub
				if (v.getId() == R.id.widget_buttonShow) {
					seekMouthFocus();
				}
				isTab = true;
				v.requestFocus();
				showListView();
				return false;
			}
		};

		mTabMenuButtonBar.mButtonVideo.setFocusableInTouchMode(true);
		mTabMenuButtonBar.mButtonAudio.setFocusableInTouchMode(true);
		mTabMenuButtonBar.mButtonTV.setFocusableInTouchMode(true);
		mTabMenuButtonBar.mButtonSetup.setFocusableInTouchMode(true);
		mTabMenuButtonBar.mButtonParental.setFocusableInTouchMode(true);

		mTabMenuButtonBar.mButtonVideo.setOnTouchListener(onTouchListener);
		mTabMenuButtonBar.mButtonAudio.setOnTouchListener(onTouchListener);
		mTabMenuButtonBar.mButtonTV.setOnTouchListener(onTouchListener);
		mTabMenuButtonBar.mButtonSetup.setOnTouchListener(onTouchListener);
		mTabMenuButtonBar.mButtonParental.setOnTouchListener(onTouchListener);
		mTabMenuButtonBar.mButtonShow.setOnTouchListener(onTouchListener);

		// FIXME May not be here
		linearScan.setOnKeyListener(new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyEvent.KEYCODE_BACK) {
						--mExitLevel;
						// TODO cancel scan
						mMenuSetUpListView.requestFocus();
						// Cancel the scan
						int index = 0;
						if (mNavIntegration.isCurrentSourceDTV()) {
							if(MenuConfigManager.getInstance(mContext).getDefault(MenuConfigManager.TUNER_MODE) ==ConfigType.BS_SRC_AIR){
								index = -1;
							}else{
								index = 3;
							}
						} else {
							index = 2;
						}
						String mExitString = "";
						if(index < 0) {
							mExitString = MenuMain.this
									.getString(R.string.menu_tv_status_value);
						} else {
							mExitString =((TextView) mMenuSetUpListView
									.getChildAt(index).findViewById(
											R.id.common_itemname)).getText()
									.toString();
						}
						if (mStateTextView
								.getText()
								.toString()
								.equals(
										getString(R.string.menu_setup_channel_scan_done))) {
							loading.stopDraw();
							if (null != mDataContainer.getmFactoryDataGroup()) {
								mDataContainer.getmFactoryDataGroup().clear();
							}
							mHandler
                            .removeMessages(MessageType.MENU_TV_RF_SCAN_REFRESH);
							mHandler.removeMessages(MessageType.MENU_TV_RF_SCAN_CONNECTTURN);
							/* mScanningStatus = true; */
							return false;
						} else if (mMenuSetUpListView.hasFocus()
								&& (mExitString
										.equals(MenuMain.this
												.getString(R.string.menu_tv_status_value)))) {
							mMenuSetUpListView.setSelection(0);
							TVScanner tv_scan = mcf.getScanner();
							tv_scan.cancelScan();
							loading.stopDraw();
							loading.setVisibility(View.GONE);
							mNumberChannel.setVisibility(View.GONE);
							mStateTextView
									.setText(MenuMain.this
											.getString(R.string.menu_tv_analog_manual_scan_cancel));
							mScanningStatus = true;
							mTextViewEnter.setVisibility(View.INVISIBLE);
							mTextViewSelect.setVisibility(View.INVISIBLE);
							mTextViewExit
									.setText(getString(R.string.menu_back));
						}
						mHandler
								.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
						mHandler.sendEmptyMessageDelayed(
								MessageType.MENU_AUTO_EXIT_MESSAGE,
								MessageType.delayMillis2);
						if (null != mDataContainer.getmFactoryDataGroup()) {
							mDataContainer.getmFactoryDataGroup().clear();
						}
						return true;
					}
				}
				if (keyCode == KeyMap.KEYCODE_MENU) {
					TVScanner tv_scan = mcf.getScanner();
					tv_scan.cancelScan();
					loading.stopDraw();
					mBypassWindowManager.setBypassWindow(false,
							mRootLayoutwindowId, mRootRelativeLayoutRect);
					exit();
					return true;
				}
				if (keyCode == KeyEvent.KEYCODE_DPAD_DOWN
						|| keyCode == KeyEvent.KEYCODE_DPAD_UP) {
					return true;
				}
				return false;
			}
		});
	}

	int isTurn=-1;
	// override activity.onKeyDown to handle KEYCODE_BACK event
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		isTurn=-1;
		mTimeConsume.breforeTime = System.currentTimeMillis();
		setIsMouse(false);
		MtkLog.v(TAG, "keydown");
		if (null!=TimeShiftManager.getInstance()) {
		    
		    if (TimeShiftManager.getInstance().onKeyDown(keyCode)) {
		        if (null==PasswordView.getInstance()) {
		            return true;
	            }
                
            }
        }
		switch (keyCode) {

		//DONOT pass these media keys to TV Window
		case KeyEvent.KEYCODE_MTKIR_STOP:
		case KeyEvent.KEYCODE_MTKIR_PREVIOUS:
		case KeyEvent.KEYCODE_MTKIR_NEXT:
		case KeyEvent.KEYCODE_MTKIR_REWIND:
		case KeyEvent.KEYCODE_MTKIR_FASTFORWARD:
			return true;
		case KeyMap.KEYCODE_MTKIR_GUIDE:
			if (mNavIntegration.isCurrentSourceTv() && mNavIntegration.isCurrentSourceDTV()
				&& mNavIntegration.isDTVHasChannels()) {
				if (mNavIntegration.iGetChannelLength() > 0 && !mNavIntegration.isCurrentSourceBlocking()) {
					startActivityForResult(new Intent(MenuMain.this, EPGActivity.class), 0);
					exit();
				}
			}
			return true;
		case KeyEvent.KEYCODE_VOLUME_UP:
		case KeyEvent.KEYCODE_VOLUME_DOWN:
			if (mNavIntegration.isMute()) {
				TurnkeyUiMainActivity.switchMute();
			} else {
				TVOptionRange<Integer> volumeOption;
				TVConfigurer tvcfg = mTV.getConfigurer();
				volumeOption = (TVOptionRange<Integer>) tvcfg
						.getOption(ConfigType.CFG_VOLUME);
				int current = 0,max = 0,min = 0;
				if (volumeOption != null) {
					current = volumeOption.get();
					max = volumeOption.getMax();
					min = volumeOption.getMin();
					int value = -1;
				if (keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
					value = 1;
				}
				current += value;
				if (current >= min && current <= max) {
					volumeOption.set(current);
				} else if (current > max) {
					volumeOption.set(max);
				} else {
					volumeOption.set(min);
					}
				}
			}
			return true;

		case KeyEvent.KEYCODE_BACK:
			if (exitEditChannelNeedCI) {
				mBypassWindowManager.setBypassWindow(false,
						mRootLayoutwindowId, mRootRelativeLayoutRect);
				MenuMain.this.exit();
				exitEditChannelNeedCI = false;
				return true;
			}
			MtkLog.d(TAG, "------back---mExitLevel-------" + mExitLevel);
			--mExitLevel;
			mTabMenuButtonBar.mButtonShow.setText(mTitleMap.get(mExitLevel));
			application_subpage = "";
			m3d_enable_page = true;

			if (mMenuSetUpListView.hasFocus() == false
					&& mTabMenuButtonBar.hasFocus() == false) {
				mMenuSetUpListView.setFocusable(true);
				mMenuSetUpListView.requestFocus();
			}
			if (versionInfoLinear.getVisibility() == View.VISIBLE) {
				versionInfoLinear.setVisibility(View.GONE);
				linearList.setVisibility(View.VISIBLE);
				mMenuSetUpListView.setFocusable(true);
				mMenuSetUpListView.requestFocus();
				return true;
			}

			if (ciMainView.getVisibility() == View.VISIBLE) {
				if (mCINav == false) {
					ciMainView.setVisibility(View.GONE);
					ciMainView.unRegisterNosignalReceiver();
					linearList.setVisibility(View.VISIBLE);
					mMenuSetUpListView.requestFocus();
					return true;
					// come from nav,exit
				} else {
					mCINav = false;
					mBypassWindowManager.setBypassWindow(false,
							mRootLayoutwindowId, mRootRelativeLayoutRect);
					MenuMain.this.exit();
					return true;
				}
			}


			if (linearScan.getVisibility() == View.VISIBLE) {
				if (linearScan.hasFocus()) {
					// TODO Cancel scan

					mMenuSetUpListView.requestFocus();
					return true;
				}
				linearScan.setVisibility(View.GONE);
				// Single RF Scan/Analog Manual Scan
				mScanningStatus = false;
				mTextViewEnter.setVisibility(View.VISIBLE);
				mTextViewSelect.setVisibility(View.VISIBLE);
			}

			if (mRootRelativeLayout.getVisibility() != View.VISIBLE) {
				mRootRelativeLayout.setVisibility(View.VISIBLE);
				mRootLayoutwindowId = mBypassWindowManager
						.getAvailableWindowId();
				mBypassWindowManager.setBypassWindow(true, mRootLayoutwindowId,
						mRootRelativeLayoutRect);
			}
			if (linearList.getVisibility() != View.VISIBLE) {
				linearList.setVisibility(View.VISIBLE);
			}

			if (editChannelViewBottom.getVisibility() == View.VISIBLE
					|| editChannelViewMenu.getVisibility() == View.VISIBLE) {

				editChannelViewBottom.setVisibility(View.GONE);
				editChannelViewMenu.setVisibility(View.GONE);
				mPageSize = NORMALPAGE_SIZE;
				mMenuSetUpListView.setVisibility(View.VISIBLE);
				mMenuSetUpListView.requestFocus();
			}

			if (mThirdLevelData.mParentGroup != null
					&& mThirdLevelData.mParentGroup.size() > 0) {
				if (mThirdLevelData.mParentGroup.get(0).mItemID
						.equals(MenuConfigManager.AUTO_SYNC)) {
					changePowerOnChannel();
				}
				if (mThirdLevelData.mParentGroup.get(0).getDataType() == DataType.CHANNELLISTVIEW) {
					mEditChannel.restoreOrignalTVSystem();
					mEditChannel.setStoredFlag(true);
					editChannelBack();
				}
			}
			if (mSecondLevelData.mParentGroup != null) {
				if (mSecondLevelData.mParentGroup.get(0).mItemID
						.equals(MenuConfigManager.TV_CHANNEL_SCAN)) {
					// Channel Scan/Update Scan
					// Single RF Scan/Analog Manual Scan
					mScanningStatus = false;
					mTextViewEnter.setVisibility(View.VISIBLE);
					mTextViewEnter.setText(R.string.menu_enter);
					mTextViewSelect.setVisibility(View.VISIBLE);
					List<TVChannel> channelList = mTM.getChannels();
					if (null != channelList && channelList.size() == 0) {
						mSecondLevelData.mParentGroup.get(TV_DISABLE_ITEMS[0])
								.setEnable(false);
						mSecondLevelData.mParentGroup.get(TV_DISABLE_ITEMS[1])
								.setEnable(false);
					} else {
						mSecondLevelData.mParentGroup.get(TV_DISABLE_ITEMS[0])
								.setEnable(true);
						mSecondLevelData.mParentGroup.get(TV_DISABLE_ITEMS[1])
								.setEnable(true);
					}
				}
				if (mSecondLevelData.mParentGroup.get(0).mItemID
						.equals(MenuConfigManager.SETUP_TIME_ZONE)) {
					UpdateTime updateTime = new UpdateTime();
					updateTime.shutDownThread();
					mDataContainer.loadTimeSetUpData();
				}
			}

			if (mExitLevel < 0) {
				mBypassWindowManager.setBypassWindow(false,
						mRootLayoutwindowId, mRootRelativeLayoutRect);
				exit();
			} else if (mExitLevel == 0) {
				if (mMenuSetUpListView.hasFocus()) {
					mMenuSetUpListView.clearFocus();
				}
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.requestFocus();
			} else {
				if (goBack) {
					reloadParentPageData(mExitLevel);
					if (mCurrentFouceButtonID == 5 && mExitLevel == 1) {
						--mExitLevel;
						if (mMenuSetUpListView.hasFocus()) {
							mMenuSetUpListView.clearFocus();
						}
						mTabMenuButtonBarRootView.getChildAt(
								mCurrentFouceButtonID).requestFocus();
					} else {
						selIdx = mMenuSetUpListView.getSelectedItemPosition();
					}
				}
			}
			return true;
		case KeyEvent.KEYCODE_0:
		case KeyEvent.KEYCODE_1:
		case KeyEvent.KEYCODE_2:
		case KeyEvent.KEYCODE_3:
		case KeyEvent.KEYCODE_4:
		case KeyEvent.KEYCODE_5:
		case KeyEvent.KEYCODE_6:
		case KeyEvent.KEYCODE_7:
		case KeyEvent.KEYCODE_8:
		case KeyEvent.KEYCODE_9:
			return handleKeyboardInput(keyCode);
		case KeyEvent.KEYCODE_PERIOD:
			if (mListViewSelectedItemData.getmName().equals(
					getResources().getString(R.string.menu_tv_freq))) {
				TextCommView textComm = (TextCommView) childView;
				textComm.inputFreq(".");
				return true;
			}
			return false;
		case KeyMap.KEYCODE_MENU:
			changePowerOnChannel();
			mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
			exit();
			return true;

			// do channel up,channel down channel previous
		case KeyMap.KEYCODE_MTKIR_CHDN:
			mChSel.channelDown();
			mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
			exit();
			return true;
		case KeyMap.KEYCODE_MTKIR_CHUP:
			mChSel.channelUp();
			mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
			exit();
			return true;
		case KeyMap.KEYCODE_MTKIR_PRECH:
			Bundle mBundle =getIntent().getExtras();
			if(mBundle != null){
				int mFromWhere = mBundle.getInt("fromwhere");
				if(mFromWhere == 1){
					return true;
				}
			}else{
				mChSel.selectPrev();
				mBypassWindowManager.setBypassWindow(false,
						mRootLayoutwindowId, mRootRelativeLayoutRect);
				exit();
			}
			return true;
			// mute
		case KeyMap.KEYCODE_MTKIR_MUTE:
			TurnkeyUiMainActivity.switchMute();
			return true;
			// MMP
		case KeyMap.KEYCODE_MTKIR_ANGLE:
			TurnkeyUiMainActivity.setMuteGone();
			NewPipLogic pl = NewPipLogic.getInstance(this);
			pl.pauseMainOutput();
			Intent intent = new Intent();
			intent.setClass(MenuMain.this, MeidaMainActivity.class);
			startActivity(intent);
			mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
			MenuMain.this.finish();

			return true;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			if (mExitLevel == 3 && application_subpage.equals(CHILD_DISABLE)) {
				return true;
			}
			if (versionInfoLinear.getVisibility() == View.VISIBLE
					|| ciMainView.getVisibility() == View.VISIBLE) {
				return true;
			}
			if (!mMenuSetUpListView.hasFocus()) {
				mMenuSetUpListView.requestFocus();
				mMenuSetUpListView.setSelection(0);
				return true;
			}
		case KeyEvent.KEYCODE_DPAD_UP:
		case KeyEvent.KEYCODE_DPAD_RIGHT:
		case KeyEvent.KEYCODE_DPAD_LEFT:
			if(keyCode == KeyEvent.KEYCODE_DPAD_RIGHT){
				isTurn=0;
			}else if(keyCode == KeyEvent.KEYCODE_DPAD_LEFT){
				isTurn=1;
			}
			if (mExitLevel == 3 && application_subpage.equals(CHILD_DISABLE)) {
				return true;
			}
			if (versionInfoLinear.getVisibility() == View.VISIBLE
					|| ciMainView.getVisibility() == View.VISIBLE) {
				return true;
			}
		default:
			mTimeConsume.currentTime = System.currentTimeMillis();
			return super.onKeyDown(keyCode, event);
		}
	}

	protected void onDestroy() {
		super.onDestroy();
		if (mCiStatusBroadcastReceiver != null) {
			unregisterReceiver(mCiStatusBroadcastReceiver);
		}
		if (ciMainView != null) {
			ciMainView.removeMenuEnqUpdateListener();
			ciMainView = null;
		}
		if (mSignalChangeReceiver != null) {
			unregisterReceiver(mSignalChangeReceiver);
		}
		mCINav = false;
		mScanningStatus = false;
		if(true == mFromMmp ){
			TVContent.getInstance(this).setOpacity(255);
		}
		mCAMManager.closeCurrentMMI();

	
	}


	protected void onPause() {
		MtkLog.d(TAG, "MenuMain-------come in onPause");
		mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
				mRootRelativeLayoutRect);
			TVChannel currentChannel = mChSel.getCurrentChannel();
			if(null != currentChannel) {
				TVCommonManager cmManager = TVCommonManager.getInstance(mContext);
				boolean ret = cmManager.isInputSourcePhysicalBlocked(cmManager
				.getCurrentInputSource());
		    /*TVConfigurer tvc = TVConfigurer.getInstance(mContext);
		    TVOptionRange<Integer> opt = (TVOptionRange<Integer>)tvc.getOption(ConfigType.CFG_EARLY_PLAY);
			
		    if(null != opt){
				if(!mNavIntegration.isCurrentSourceBlocking()&&(mNavIntegration.isCurrentSourceTv()&&mNavIntegration.iGetChannelLength()>0)
					&&!mNavIntegration.isCurrentChannelBlocked()){
					opt.set(ConfigType.EARLY_PLAY_ENABLED);
				}else{
					opt.set(ConfigType.EARLY_PLAY_DISENABLED);
				}
		  	}*/
	  	}
			mHandler
            .removeMessages(MessageType.MENU_TV_RF_SCAN_REFRESH);
			mHandler.removeMessages(MessageType.MENU_TV_RF_SCAN_CONNECTTURN);
		if(null != mScanDialog && mScanDialog.onScanning){
			mScanDialog.cancelScan();
		}
		super.onPause();
	}

	protected void onRestart() {
		super.onRestart();
	}

	protected void onStop() {


		
	  if (mNavIntegration != null && mInternalSourceChangeListener != null) {
			mNavIntegration.iRemoveInputsourceChangeListener(mInternalSourceChangeListener);
			mInternalSourceChangeListener = null;
	  }
		
	  if (mChSel != null && mInternalSelectorListener != null) {
			mChSel.removeSelectorListener(mInternalSelectorListener);
			mInternalSelectorListener=null;
	  }
		
		mEditChannel.flush();
		if(mScanDialog!=null){
			mScanDialog.onScanning = false;
		}
		super.onStop();
	}

	class MyUpdata implements CustListView.UpDateListView {
		public void updata() {
			mGroup = (List<DataItem>) mMenuSetUpListView.getCurrentList();
			mListViewAdpter.setmGroup(mGroup);
			mMenuSetUpListView.setAdapter(mListViewAdpter);
		}
	}

	/**
	 * Enter 0 for 4 times
	 */
	private void loadFactoryData() {
		mTabMenuButtonBar.mButtonShow.setText(R.string.menu_factory_name);
		mDataContainer = MenuDataContainer.getInstance(this);
		mDataContainer.loadFactoryData();
		setAdapter(mDataContainer.getmFactoryDataGroup());
		mMenuSetUpListView.requestFocus();
		mExitLevel++;
		/*
		 * if(mEditChannel.isInputBlock(mDataContainer.getBlockSourceName())&&TVInputManager
		 * .INPUT_TYPE_TV.equals(mTV.getInputManager()
		 * .getTypeFromInputSource(mDataContainer.mCurrentSource))){
		 * mMenuSetUpListView.getChildAt(2).setVisibility(View.GONE); }
		 */
	}

	/**
	 * 
	 */
	private void editChannelBack() {
		if (mExitLevel == 3) {
			editChannelView = 1;

			channelEditView();
			if (mThirdLevelData.mParentGroup.size() > 0) {
				mThirdLevelData.mParentGroup.clear();

				backEditView = BACKEDITVIEW;
			}
			mListViewAdpter.notifyDataSetChanged();
			getTVData();
			mDataContainer.loadChannelData();
			mListViewAdpter.notifyDataSetChanged();
			oldChannelNum = new String[mDataContainer.getChannelDataGroup()
					.size()];
			// The channel number stored in an array list
			for (int i = 0; i < mDataContainer.getChannelDataGroup().size(); i++) {
				String[] optionStr = mDataContainer.getChannelDataGroup()
						.get(i).getmOptionValue();
				oldChannelNum[i] = optionStr[0];
			}
			mMenuSetUpListView.setSelection(gotoPosition);
		} else {
			editChannelViewBottom.setVisibility(View.GONE);
			editChannelViewMenu.setVisibility(View.GONE);
		}
	}

	/**
	 * 
	 */

	/**
	 * reset show,and Hide the main view
	 */
	public void resetConfirm() {
		factroyCofirm = new TurnkeyCommDialog(MenuMain.this, 3);
		factroyCofirm.setMessage(MenuMain.this
				.getString(R.string.menu_tv_reset_all));
		factroyCofirm.setButtonYesName(MenuMain.this
				.getString(R.string.menu_dialog_confirm));
		factroyCofirm.setButtonNoName(MenuMain.this
				.getString(R.string.menu_dialog_cancel));
		factroyCofirm.show();
		factroyCofirm.getButtonNo().requestFocus();
		mRootRelativeLayout.setVisibility(View.GONE);
		mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
				mRootRelativeLayoutRect);
		factroyCofirm.setPositon(-20, 70);
		factroyCofirmRect = new Rect(-20
				+ (ScreenConstant.SCREEN_WIDTH - factroyCofirm.width) / 2,
				70 + (ScreenConstant.SCREEN_HEIGHT - factroyCofirm.height) / 2,
				-20 + (ScreenConstant.SCREEN_WIDTH + factroyCofirm.width) / 2,
				70 + (ScreenConstant.SCREEN_HEIGHT + factroyCofirm.height) / 2);
    factroyCofirmWindowId = mBypassWindowManager.getAvailableWindowId();
		mBypassWindowManager.setBypassWindow(true, factroyCofirmWindowId,
				factroyCofirmRect);
		factroyCofirm.setOnKeyListener(new DialogInterface.OnKeyListener() {
			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				int action = event.getAction();
				if (keyCode == KeyEvent.KEYCODE_BACK
						&& action == KeyEvent.ACTION_DOWN) {
					factroyCofirm.dismiss();
					mBypassWindowManager.setBypassWindow(false,
							factroyCofirmWindowId, factroyCofirmRect);
					mExitLevel--;
					mRootRelativeLayout.setVisibility(View.VISIBLE);
					mRootLayoutwindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBypassWindowManager.setBypassWindow(true,
							mRootLayoutwindowId, mRootRelativeLayoutRect);
					mMenuSetUpListView.requestFocus();
					return true;
				}
				return false;
			}
		});

		OnKeyListener yesListener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyEvent.KEYCODE_ENTER
							|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
						Intent intent = new Intent();
						intent.setClass(MenuMain.this,
								SetupWizardActivity.class);
						startActivity(intent);
						MenuMain.this.exit();
						mBypassWindowManager.setBypassWindow(false,
								mRootLayoutwindowId, mRootRelativeLayoutRect);
						mEditChannel.resetDefAfterClean();
						tvCommon.selectCaptureLogo(0);
						clearSharePreference();
                        TurnkeyUiMainActivity.cancelMute();					
						return true;
					}
				}
				return false;
			}
		};

		OnKeyListener noListener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyEvent.KEYCODE_ENTER
							|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
						factroyCofirm.dismiss();
						mBypassWindowManager.setBypassWindow(false,
								factroyCofirmWindowId, factroyCofirmRect);
						mExitLevel--;
						mRootRelativeLayout.setVisibility(View.VISIBLE);
						mRootLayoutwindowId = mBypassWindowManager
								.getAvailableWindowId();
						mBypassWindowManager.setBypassWindow(true,
								mRootLayoutwindowId, mRootRelativeLayoutRect);
						mMenuSetUpListView.requestFocus();
						return true;
					}
				}
				return false;
			}
		};
		factroyCofirm.getButtonNo().setOnKeyListener(noListener);
		factroyCofirm.getButtonYes().setOnKeyListener(yesListener);
	}
	
	private void clearSharePreference(){
		saveValue.saveValue(MenuConfigManager.FAST_BOOT,0);
		SystemProperties.set("sys.pm.warmboot","0");
	}

	/**
	 * Network the update, and hide the interface display
	 */
	private void networkUpdate() {
		TurnkeyCommDialog nu = new TurnkeyCommDialog(MenuMain.this, 0);
		Rect nuRect;
		int nuWindowId = -1;
		nu.setMessage(getString(R.string.menu_setup_upgrade_info1));
		nu.show();
		mRootRelativeLayout.setVisibility(View.GONE);
		mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
				mRootRelativeLayoutRect);
		MtkLog.i("tag", "networkUpdate show");
		nuRect = new Rect((ScreenConstant.SCREEN_WIDTH - nu.width) / 2,
				(ScreenConstant.SCREEN_HEIGHT - nu.height) / 2,
				(ScreenConstant.SCREEN_WIDTH + nu.width) / 2,
				(ScreenConstant.SCREEN_HEIGHT + nu.height) / 2);
		nuWindowId = mBypassWindowManager.getAvailableWindowId();
		mBypassWindowManager.setBypassWindow(true, nuWindowId, nuRect);
		final Handler handler = new Handler();
		final NetUpdateGider netUpdateGiderDialog = new NetUpdateGider(
				MenuMain.this, nu);
		mBypassWindowManager.setBypassWindow(false, nuWindowId, nuRect);
		handler.postDelayed(netUpdateGiderDialog, 1000);
		netUpdateGiderDialog.getShowDialog().setTitle(
				getString(R.string.menu_setup_upgrade_info2_title));
		netUpdateGiderDialog.getShowDialog().setMessage(
				getString(R.string.menu_setup_upgrade_info2));
		netUpdateGiderDialog.getShowDialog().setOnKeyListener(
				new DialogInterface.OnKeyListener() {

					public boolean onKey(DialogInterface dialog, int keyCode,
							KeyEvent event) {
						int action = event.getAction();
						if (keyCode == KeyEvent.KEYCODE_BACK
								&& action == KeyEvent.ACTION_DOWN) {
							netUpdateGiderDialog.getShowDialog().dismiss();
							exit();
							mBypassWindowManager.setBypassWindow(false,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);
							return true;
						}
						return false;
					}
				});
	}

	


	private void requestWhenExit() {
		mMenuSetUpListView.postDelayed(new Runnable() {
			public void run() {
				// TODO Auto-generated method stub
				View view = mMenuSetUpListView.getChildAt(selIdx);
				if (view != null) {
					view.requestFocusFromTouch();
                    mMenuSetUpListView.requestFocus();
					mMenuSetUpListView.setSelection(selIdx);
				}
			}
		}, 100 * (mMenuSetUpListView.getCount() <= 9 ? mMenuSetUpListView
				.getCount() : 9));
	}

	private void requestWhenExit(final int index) {
		mMenuSetUpListView.postDelayed(new Runnable() {
			public void run() {
				// TODO Auto-generated method stub
				View view = mMenuSetUpListView.getChildAt(index);
				if (view != null) {
					selIdx = index;
					view.requestFocusFromTouch();
					mMenuSetUpListView.requestFocus();
					mMenuSetUpListView.setSelection(index);
				}
			}
		}, 100 * (mMenuSetUpListView.getCount() <= 9 ? mMenuSetUpListView
				.getCount() : 9));
	}


	/**
	 * fineture dialog
	 */
	private void finetuneInfoDialog() {
		final TurnkeyCommFinetune tcf = new TurnkeyCommFinetune(MenuMain.this);
		final NumberFormat numberFormat = NumberFormat.getNumberInstance();
		numberFormat.setMaximumFractionDigits(3);
		numberFormat.setGroupingUsed(false);
		mRootRelativeLayout.setVisibility(View.INVISIBLE);
		mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
				mRootRelativeLayoutRect);
		tcf.show();
		mTurnkeyCommFinetuneWindowId = mBypassWindowManager
				.getAvailableWindowId();
		mTurnkeyCommFinetuneRect = new Rect(
				(ScreenConstant.SCREEN_WIDTH - tcf.menuWidth) / 2,
				(ScreenConstant.SCREEN_HEIGHT - tcf.menuHeight) / 2,
				(ScreenConstant.SCREEN_WIDTH + tcf.menuWidth) / 2,
				(ScreenConstant.SCREEN_HEIGHT + tcf.menuHeight) / 2);
		mBypassWindowManager.setBypassWindow(true,
				mTurnkeyCommFinetuneWindowId, mTurnkeyCommFinetuneRect);
		final TextView fineNum = (TextView) mMenuSetUpListView.getChildAt(0)
				.findViewById(R.id.common_text_content);
		final TextView fineName = (TextView) mMenuSetUpListView.getChildAt(1)
				.findViewById(R.id.common_text_content);
		final TextView fineFre = (TextView) mMenuSetUpListView.getChildAt(2)
				.findViewById(R.id.common_text_content);
		numHz = Float.parseFloat(fineFre.getText().toString());
		tcf.setNumText(fineNum.getText().toString());
		tcf.setNameText(fineName.getText().toString());
		String tem = numberFormat.format(numHz) + "MHZ";
		tcf.setAdjustText(tem);
		mEditChannel.setRestoreHZ(numHz);
		tcf.setOnKeyListener(new DialogInterface.OnKeyListener() {
			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				int action = event.getAction();
				if ((keyCode == KeyEvent.KEYCODE_ENTER || keyCode == KeyEvent.KEYCODE_DPAD_CENTER)
						&& action == KeyEvent.ACTION_DOWN) {
					mEditChannel.setStoredFlag(true);
					mRootRelativeLayout.setVisibility(View.VISIBLE);
					mRootLayoutwindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBypassWindowManager.setBypassWindow(true,
							mRootLayoutwindowId, mRootRelativeLayoutRect);
					mMenuSetUpListView.requestFocus();
					mExitLevel--;
					mMenuSetUpListView.setSelection(mBeforeSelectedPosition);
					tcf.dismiss();
					mBypassWindowManager.setBypassWindow(false,
							mTurnkeyCommFinetuneWindowId,
							mTurnkeyCommFinetuneRect);
					fineFre.setText(Float.toString(numHz));
					mEditChannel.exitFinetune();
					requestWhenExit();
					return true;
				} else if (keyCode == KeyEvent.KEYCODE_BACK
						&& action == KeyEvent.ACTION_DOWN) {
					mEditChannel.setStoredFlag(false);
					mEditChannel.restoreFineTune();
					mRootRelativeLayout.setVisibility(View.VISIBLE);
					mRootLayoutwindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBypassWindowManager.setBypassWindow(true,
							mRootLayoutwindowId, mRootRelativeLayoutRect);
					mMenuSetUpListView.requestFocus();
					mExitLevel--;
					mMenuSetUpListView.setSelection(mBeforeSelectedPosition);
					tcf.dismiss();
					mBypassWindowManager.setBypassWindow(false,
							mTurnkeyCommFinetuneWindowId,
							mTurnkeyCommFinetuneRect);
					mEditChannel.exitFinetune();
					requestWhenExit();
					return true;
				} else if ((keyCode == KeyEvent.KEYCODE_DPAD_LEFT || keyCode == KeyEvent.KEYCODE_DPAD_RIGHT)
						&& action == KeyEvent.ACTION_DOWN) {
					numHz = mEditChannel.fineTune(numHz, keyCode);
					MtkLog.i(TAG,"numHz:"+numHz);
					tcf.setAdjustText(numberFormat.format(numHz) + "MHZ");
				}
				return false;
			}
		});
		tcf.setOnDismissListener(new OnDismissListener(){

			@Override
			public void onDismiss(DialogInterface di) {
				// TODO Auto-generated method stub
				float hz = mEditChannel.getRestoreHZ();
				MtkLog.i(TAG, "numHz:"+numHz+"  hz:"+hz);
				
				if(numHz!=mEditChannel.getRestoreHZ()&& mEditChannel.getStoredFlag()==true){
				
					String[] ch=((DataItem)mListViewAdpter.getItem(2)).getmOptionValue();
					ch[2]	=	numberFormat.format(numHz);
					((DataItem)mListViewAdpter.getItem(2)).setmOptionValue(ch);
					mListViewAdpter.notifyDataSetChanged();
				}
			}
			
		});
	}

	/**
	 * Get toast window object add time: on July 20 ChenLe content: add
	 * initialization hide the interface of the operation the tooltip window
	 * 
	 * @return
	 */
	private void getFactroyCofirm(String msg) {
		factroyCofirm = new TurnkeyCommDialog(MenuMain.this, 3);
		factroyCofirm.setMessage(msg);
		factroyCofirm.setButtonYesName(MenuMain.this
				.getString(R.string.menu_dialog_confirm));
		factroyCofirm.setButtonNoName(MenuMain.this
				.getString(R.string.menu_dialog_cancel));
		factroyCofirm.show();
		mRootRelativeLayout.setVisibility(View.GONE);
		mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
				mRootRelativeLayoutRect);
		factroyCofirm.setPositon(-20, 70);
		factroyCofirm.getButtonNo().requestFocus();
		factroyCofirmRect = new Rect(-20
				+ (ScreenConstant.SCREEN_WIDTH - factroyCofirm.width) / 2,
				70 + (ScreenConstant.SCREEN_HEIGHT - factroyCofirm.height) / 2,
				-20 + (ScreenConstant.SCREEN_WIDTH + factroyCofirm.width) / 2,
				70 + (ScreenConstant.SCREEN_HEIGHT + factroyCofirm.height) / 2);
    factroyCofirmWindowId = mBypassWindowManager.getAvailableWindowId();
		mBypassWindowManager.setBypassWindow(true, factroyCofirmWindowId,
				factroyCofirmRect);
		factroyCofirm.setOnKeyListener(new DialogInterface.OnKeyListener() {
			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				int action = event.getAction();
				if (keyCode == KeyEvent.KEYCODE_BACK
						&& action == KeyEvent.ACTION_DOWN) {
					factroyCofirm.dismiss();
					mBypassWindowManager.setBypassWindow(false,
							factroyCofirmWindowId, factroyCofirmRect);
					mExitLevel--;
					mRootRelativeLayout.setVisibility(View.VISIBLE);
					mRootLayoutwindowId = mBypassWindowManager
							.getAvailableWindowId();
					mBypassWindowManager.setBypassWindow(true,
							mRootLayoutwindowId, mRootRelativeLayoutRect);
					mMenuSetUpListView.requestFocus();
					requestWhenExit();
					return true;
				}
				return false;
			}
		});
	}

	/**
	 * TV shows, and hide the interface modification time: on July 20, the
	 * modifier: ChenLe content: take initial main interface operation of the
	 * hidden code the tooltip window
	 */
	private void cleanChannelConfirm() {
		getFactroyCofirm(MenuMain.this
				.getString(R.string.menu_tv_clear_channel_info));

		OnKeyListener listener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyEvent.KEYCODE_ENTER
							|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
						if (v.getId() == factroyCofirm.getButtonYes().getId()) {
							mEditChannel.cleanChannelList();
							if (null != mDataContainer.getmFactoryDataGroup()) {
								mDataContainer.getmFactoryDataGroup().clear();
							}
							factroyCofirm.dismiss();
							mBypassWindowManager.setBypassWindow(false,
									factroyCofirmWindowId, factroyCofirmRect);
							mExitLevel--;
							mRootRelativeLayout.setVisibility(View.VISIBLE);
							mRootLayoutwindowId = mBypassWindowManager
									.getAvailableWindowId();
							mBypassWindowManager.setBypassWindow(true,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);
							mMenuSetUpListView.requestFocus();
							requestWhenExit(0);
							mMenuSetUpListView.setSelection(0);
							for (int i = 0; i < TV_DISABLE_ITEMS.length; i++) {
								((HaveSubPageView) mMenuSetUpListView
										.getChildAt(TV_DISABLE_ITEMS[i]))
										.getmDataItem().setEnable(false);
								((HaveSubPageView) mMenuSetUpListView
										.getChildAt(TV_DISABLE_ITEMS[i]))
										.getNameView().setTextColor(Color.GRAY);
							}
							mMenuSetUpListView.setAdapter(mListViewAdpter);
						} else if (v.getId() == factroyCofirm.getButtonNo().getId()) {
							factroyCofirm.dismiss();
							mBypassWindowManager.setBypassWindow(false,
									factroyCofirmWindowId, factroyCofirmRect);
							mExitLevel--;
							mRootRelativeLayout.setVisibility(View.VISIBLE);
							mRootLayoutwindowId = mBypassWindowManager
									.getAvailableWindowId();
							mBypassWindowManager.setBypassWindow(true,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);
							mMenuSetUpListView.requestFocus();
							requestWhenExit();
						}
						return true;
					}
				}
				return false;
			}
		};
		factroyCofirm.getButtonNo().setOnKeyListener(listener);
		factroyCofirm.getButtonYes().setOnKeyListener(listener);
	}

	/**
	 * parental clean all ,And hide the interface modification time: on July 20,
	 * the modifier: ChenLe content: take initial main interface operation of
	 * the hidden code the tooltip window
	 */
	private void cleanParentalChannelConfirm() {
		getFactroyCofirm(MenuMain.this
				.getString(R.string.menu_tv_clear_channel_info));

		OnKeyListener listener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyEvent.KEYCODE_ENTER
							|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
						if (v.getId() == factroyCofirm.getButtonYes().getId()) {
							mEditChannel.resetParental(MenuMain.this,
									new Runnable() {
										public void run() {
											mNavIntegration.iSetStorageZero();
											Intent intent = new Intent();
											intent.setClass(MenuMain.this,
													SetupWizardActivity.class);
											TurnkeyUiMainActivity.cancelMute();
											startActivity(intent);
											MenuMain.this.finish();
											factroyCofirm.dismiss();
											mBypassWindowManager
													.setBypassWindow(
															false,
															factroyCofirmWindowId,
															factroyCofirmRect);
											mEditChannel.resetDefAfterClean();
										}
									});
						} else if (v.getId() == factroyCofirm.getButtonNo().getId()) {
							// do nothing
							factroyCofirm.dismiss();
							mBypassWindowManager.setBypassWindow(false,
									factroyCofirmWindowId, factroyCofirmRect);
							mExitLevel--;
							mTabMenuButtonBar.mButtonShow.setText(mTitleMap
									.get(mExitLevel));
							mRootRelativeLayout.setVisibility(View.VISIBLE);
							mRootLayoutwindowId = mBypassWindowManager
									.getAvailableWindowId();
							mBypassWindowManager.setBypassWindow(true,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);
							mMenuSetUpListView.requestFocus();
							requestWhenExit();
						}
						return true;
					}
				}
				return false;
			}
		};
		factroyCofirm.getButtonNo().setOnKeyListener(listener);
		factroyCofirm.getButtonYes().setOnKeyListener(listener);
	}

	/**
	 * Factory clean Restorage ,And hide the interface modification time: on
	 * July 20, the modifier: ChenLe content: take initial main interface
	 * operation of the hidden code the tooltip window
	 */
	private void factoryCleanStorage() {
		getFactroyCofirm(MenuMain.this
				.getString(R.string.menu_factory_setup_clean_dialog));

		OnKeyListener listener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyEvent.KEYCODE_ENTER
							|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
						if (v.getId() == factroyCofirm.getButtonYes().getId()) {
							TVConfigurer tvcfg = mTV.getConfigurer();
							tvcfg.resetFactory();
							mEditChannel.resetParental(MenuMain.this,
									new Runnable() {
										public void run() {
											// TODO Auto-generated method stub
											mNavIntegration.iSetStorageZero();
											Intent intent = new Intent();
											intent.setClass(MenuMain.this,
													SetupWizardActivity.class);
											TurnkeyUiMainActivity.cancelMute();
											startActivity(intent);
											MenuMain.this.finish();
											mBypassWindowManager
													.setBypassWindow(
															false,
															mRootLayoutwindowId,
															mRootRelativeLayoutRect);
										}

									});
							return true;

						} else if (v.getId() == factroyCofirm.getButtonNo().getId()) {
							factroyCofirm.dismiss();
							mBypassWindowManager.setBypassWindow(false,
									factroyCofirmWindowId, factroyCofirmRect);
							mExitLevel--;
							mRootRelativeLayout.setVisibility(View.VISIBLE);
							mRootLayoutwindowId = mBypassWindowManager
									.getAvailableWindowId();
							mBypassWindowManager.setBypassWindow(true,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);
							mMenuSetUpListView.requestFocus();
							requestWhenExit();
							return true;
						}
					}
				}
				return false;
			}
		};
		factroyCofirm.getButtonNo().setOnKeyListener(listener);
		factroyCofirm.getButtonYes().setOnKeyListener(listener);
	}

	private void factoryCi(int state) {
		final CiDialog ciDialog = new CiDialog(this);
		Rect mCiDialogRect;
		int mCiDialogWindowId = -1;
		factroyCofirm = new TurnkeyCommDialog(MenuMain.this, 0);
		switch (state) {
		case 1:
			factroyCofirm.setMessage(MenuMain.this
					.getString(R.string.menu_factory_setup_ci_update_false));
			break;
		case 2:
			factroyCofirm.setMessage(MenuMain.this
					.getString(R.string.menu_factory_setup_ci_erase_false));
			break;
		case 3:
			String title = "Valid:N Customer Code:";
			String text = "Serial Number:";

			ciDialog.setMessage(text);
			ciDialog.setTitle(title);
			break;
		}
		if (state == 3) {
			ciDialog.show();
			mRootRelativeLayout.setVisibility(View.GONE);
			mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
			ciDialog.setPosition(-20, 70);
			ciDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {

				public boolean onKey(DialogInterface dialog, int keyCode,
						KeyEvent event) {
					// TODO Auto-generated method stub
					int action = event.getAction();
					if (keyCode == KeyEvent.KEYCODE_BACK
							&& action == KeyEvent.ACTION_DOWN) {
						ciDialog.dismiss();
						mExitLevel--;
						mRootRelativeLayout.setVisibility(View.VISIBLE);
						mRootLayoutwindowId = mBypassWindowManager
								.getAvailableWindowId();
						mBypassWindowManager.setBypassWindow(true,
								mRootLayoutwindowId, mRootRelativeLayoutRect);
						mMenuSetUpListView.requestFocus();
						requestWhenExit();
					}
					return false;
				}
			});
		} else {
			factroyCofirm.show();
			mRootRelativeLayout.setVisibility(View.GONE);
			mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
			factroyCofirm.setPositon(-20, 70);
			factroyCofirmRect = new Rect(
					-20 + (ScreenConstant.SCREEN_WIDTH - factroyCofirm.width)
							/ 2,
					70 + (ScreenConstant.SCREEN_HEIGHT - factroyCofirm.height) / 2,
					-20 + (ScreenConstant.SCREEN_WIDTH + factroyCofirm.width)
							/ 2,
					70 + (ScreenConstant.SCREEN_HEIGHT + factroyCofirm.height) / 2);
			factroyCofirmWindowId	= mBypassWindowManager.getAvailableWindowId();
			mBypassWindowManager.setBypassWindow(true, factroyCofirmWindowId,
					factroyCofirmRect);
			factroyCofirm.setOnKeyListener(new DialogInterface.OnKeyListener() {

				public boolean onKey(DialogInterface dialog, int keyCode,
						KeyEvent event) {
					// TODO Auto-generated method stub
					int action = event.getAction();
					if (keyCode == KeyEvent.KEYCODE_BACK
							&& action == KeyEvent.ACTION_DOWN) {
						factroyCofirm.dismiss();
						mBypassWindowManager.setBypassWindow(false,
								factroyCofirmWindowId, factroyCofirmRect);
						mExitLevel--;
						mRootRelativeLayout.setVisibility(View.VISIBLE);
						mRootLayoutwindowId = mBypassWindowManager
								.getAvailableWindowId();
						mBypassWindowManager.setBypassWindow(true,
								mRootLayoutwindowId, mRootRelativeLayoutRect);
						mMenuSetUpListView.requestFocus();
						requestWhenExit();
						return true;
					}
					return false;
				}
			});
		}
	}

	/**
	 * Channel, and hide the Lord keep display interface modification time: on
	 * July 20, the modifier: ChenLe content: take initial main interface
	 * operation of the hidden code the tooltip window
	 */
	private void storeChannel() {
		getFactroyCofirm(MenuMain.this.getString(R.string.menu_tv_store_data));

		OnKeyListener listener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyEvent.KEYCODE_ENTER
							|| keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
						if (v.getId() == factroyCofirm.getButtonYes().getId()) {
							boolean mSameChNumFlag = false;
							final TextView textNum = (TextView) mMenuSetUpListView
									.getChildAt(0).findViewById(
											R.id.common_text_content);
							TextView textName = (TextView) mMenuSetUpListView
									.getChildAt(1).findViewById(
											R.id.common_text_content);
							TextView textFreq = (TextView) mMenuSetUpListView
									.getChildAt(2).findViewById(
											R.id.common_text_content);

							String channelNum = textNum.getText().toString();
							String channelName = textName.getText().toString();
							if ("________".equals(channelName)) {
								channelName = "";
							}
							String channelFreq = textFreq.getText().toString();
							channelNum = Integer.parseInt(channelNum) + "";
//							if (mGroup.get(EditChannel.SKIP).mInitValue == 0) {
//
//							}
							int i = 0;
							for (i = 0; i < oldChannelNum.length; i++) {
								if (channelNum.equals(oldChannelNum[i])) {
									break;
								}
							}

							List<TVChannel> tvChList = mEditChannel
									.getChannelList();
							if (i <= oldChannelNum.length) {
								// the same channel num, toast info,cannot save
								if(null != tvChList){
								for (TVChannel ch : tvChList) {
									if (mEditChannel.getCurrentChannelNumber() != Short
											.valueOf(channelNum)
											&& ch.getChannelNum() == Short
													.valueOf(channelNum)) {
										mSameChNumFlag = true;
										break;
										}
									}
								}

								if (mSameChNumFlag) {
									mSameChNumFlag = false;
									factroyCofirm.dismiss();
									mBypassWindowManager.setBypassWindow(false,
											factroyCofirmWindowId,
											factroyCofirmRect);
									toastEditChSameNum();
									Handler mEditHandler = new Handler();
									mEditHandler.postDelayed(new Runnable() {
										public void run() {
											editChSameNumDialog.dismiss();
											mBypassWindowManager
													.setBypassWindow(
															false,
															editChSameNumDialogWindowId,
															editChSameNumDialogRect);
											mEditChannel.exitFinetune();
											--mExitLevel;
											mRootRelativeLayout
													.setVisibility(View.VISIBLE);
											mRootLayoutwindowId = mBypassWindowManager
													.getAvailableWindowId();
											mBypassWindowManager
													.setBypassWindow(
															true,
															mRootLayoutwindowId,
															mRootRelativeLayoutRect);
											mMenuSetUpListView.requestFocus();
											mMenuSetUpListView.setSelection(0);
											textNum
													.setText(""
															+ mEditChannel
																	.getCurrentChannelNumber());
											requestWhenExit();
										}
									}, 3000);
								} else {
									mEditChannel
											.storeChannel(
													channelNum,
													channelName,
													channelFreq,
													mGroup
															.get(EditChannel.COLOR).mInitValue,
													mGroup
															.get(EditChannel.SOUND).mInitValue,
													mGroup.get(EditChannel.AFT).mInitValue,
													mGroup
															.get(EditChannel.SKIP).mInitValue);
									factroyCofirm.dismiss();
									mBypassWindowManager.setBypassWindow(false,
											factroyCofirmWindowId,
											factroyCofirmRect);
									mEditChannel.exitFinetune();
									--mExitLevel;
									mRootRelativeLayout
											.setVisibility(View.VISIBLE);
									mRootLayoutwindowId = mBypassWindowManager
											.getAvailableWindowId();
									mBypassWindowManager.setBypassWindow(true,
											mRootLayoutwindowId,
											mRootRelativeLayoutRect);
									mMenuSetUpListView.requestFocus();
									requestWhenExit();
									KeyEvent chBackEvent = new KeyEvent(
											KeyEvent.ACTION_DOWN,
											KeyEvent.KEYCODE_BACK);
									MenuMain.this.dispatchKeyEvent(chBackEvent);
								}
								return true;
							}
						} else if (v.getId() == factroyCofirm.getButtonNo().getId()) {
							factroyCofirm.dismiss();
							mBypassWindowManager.setBypassWindow(false,
									factroyCofirmWindowId, factroyCofirmRect);
							mExitLevel--;
							mRootRelativeLayout.setVisibility(View.VISIBLE);
							mRootLayoutwindowId = mBypassWindowManager
									.getAvailableWindowId();
							mBypassWindowManager.setBypassWindow(true,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);
							mMenuSetUpListView.requestFocus();
							requestWhenExit();
							return true;
						}
					}
				}
				return false;
			}
		};
		factroyCofirm.getButtonNo().setOnKeyListener(listener);
		factroyCofirm.getButtonYes().setOnKeyListener(listener);
	}

	public interface IChannelNameChange {
		void nameChanged(String channelName);
	}
	private IChannelNameChange mIChannelNameChange;
	public void setIChannelNameChangeListener(IChannelNameChange listener) {
		mIChannelNameChange = listener;
	}
	/**
	 * Channel, and hide the Lord keep display interface
	 */
	private void channelNameEdit() {
		final TextView channelName = (TextView) mMenuSetUpListView
				.getSelectedView().findViewById(R.id.common_text_content);
		final KeyboardDialog tcg = new KeyboardDialog(MenuMain.this);
		tcg.show();
		tcg.setEditMaxLength(11);
		linearList.setVisibility(View.INVISIBLE);
		tcg.setSize(0.5f, 0.33f);
		tcg.setPositon(70, 0);
		if (channelName.getText().toString().equals("________")) {
			tcg.getvInput().setText("_");
		} else {
			tcg.getvInput().setText(channelName.getText().toString() + "_");
		}

		tcg.setOnKeyListener(new DialogInterface.OnKeyListener() {
			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				int action = event.getAction();
				if (keyCode == KeyMap.KEYCODE_BACK
						&& action == KeyEvent.ACTION_DOWN) {
					tcg.dismiss();
					mExitLevel--;
					linearList.setVisibility(View.VISIBLE);
					mMenuSetUpListView.requestFocus();
					requestWhenExit();
					return true;
				}
				if (keyCode == KeyMap.KEYCODE_MTKIR_MUTE
						&& action == KeyEvent.ACTION_DOWN) {
					
						TurnkeyUiMainActivity.switchMute();
						return true;
				}
				return false;
			}
		});

		OnKeyListener listener = new OnKeyListener() {
			public boolean onKey(View v, int keyCode, KeyEvent event) {
				if (event.getAction() == KeyEvent.ACTION_DOWN) {
					if (keyCode == KeyMap.KEYCODE_DPAD_CENTER
							|| keyCode == KeyEvent.KEYCODE_ENTER) {
						if (v.getId() == tcg.getButton_ok().getId()) {
							// tm.clear();
							String name = tcg.getInput();
							if (name.length() == 0) {
								channelName.setText("________");
							} else {
								channelName.setText(name);
								if (mIChannelNameChange != null) {
									mIChannelNameChange.nameChanged(name);
								}
							}
							tcg.dismiss();
						} else if (v.getId() == tcg.getButton_cancel().getId()) {
							tcg.dismiss();
						}

						mExitLevel--;
						linearList.setVisibility(View.VISIBLE);
						mMenuSetUpListView.requestFocus();
						requestWhenExit();
						return true;
					}
				}

				return false;
			}

		};

		tcg.getButton_ok().setOnKeyListener(listener);
		tcg.getButton_cancel().setOnKeyListener(listener);
	}

	/**
	 * Judge whether the current page editing pages
	 */
	private void channelEditView() {
		if (editChannelView == 1) {
			editChannelViewBottom.setVisibility(View.VISIBLE);
			editChannelViewMenu.setVisibility(View.VISIBLE);
			mPageSize = CHANNELPAGE_SIZE;
		}
		if (editChannelView == 2) {
			editChannelViewBottom.setVisibility(View.GONE);
			editChannelViewMenu.setVisibility(View.GONE);
			mPageSize = NORMALPAGE_SIZE;
		}
	}

	/**
	 * From the floor for any information
	 * 
	 */
	private void getTVData() {
		List<TVChannel> ch_list = mEditChannel.getChannelList();
		List<String[]> channelInfo = new ArrayList<String[]>();
		final NumberFormat numberFormat = NumberFormat.getNumberInstance();
		numberFormat.setMaximumFractionDigits(3);
		numberFormat.setGroupingUsed(false);
		
		int chNum = 0;
		if(null != ch_list){
			chNum = ch_list.size();
			MtkLog.d(TAG, "Channel List Length: " + ch_list.size());
		}
		String[] tempStr;
		TVChannel tempChannel;
		for (int i = 0; i < chNum; i++) {
			tempStr = new String[5];
			tempChannel = ch_list.get(i);
			int sysNum = 0;
			if (tempChannel.getTvSystemOption() != null) {
				sysNum = tempChannel.getTvSystemOption().get();
				MtkLog.d(TAG, "----- tv system Num: " + sysNum + "   value:"
						+ tempChannel.getTvSystemOption().getMin() + ","
						+ tempChannel.getTvSystemOption().getMax());
				if (sysNum < tempChannel.getTvSystemOption().getMin()
						|| sysNum > tempChannel.getTvSystemOption().getMax()) {
					sysNum = tempChannel.getTvSystemOption().getMin();
				}
				tempStr[1] = "Analog";
				tempStr[3] = MenuMain.this.getResources().getStringArray(
						R.array.menu_tv_sound_system_array)[sysNum
						- tempChannel.getTvSystemOption().getMin()];
			} else {
				sysNum = 1;
				tempStr[1] = "Digital";
				MtkLog.d(TAG, "----- tv system option is null---- " + i);
			}
			tempStr[0] = "" + tempChannel.getChannelNum();
			tempStr[2] = "" + numberFormat.format(((float)(tempChannel.getFreq()))/1000000);
			tempStr[4] = tempChannel.getChannelName();

			channelInfo.add(tempStr);
		}
		TVChannel currentChannel = mChSel.getCurrentChannel();
		if(null != ch_list){
		currentChannelNum = ch_list.indexOf(currentChannel);
		}
		saveValue.readValue("START_CHANNEL");
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.PARENTAL_CHANNEL_BLOCK)) {
			gotoPage = currentChannelNum / (NORMALPAGE_SIZE) + 1;
			gotoPosition = currentChannelNum % NORMALPAGE_SIZE;
		} else {
			gotoPage = currentChannelNum / (CHANNELPAGE_SIZE) + 1;
			gotoPosition = currentChannelNum % CHANNELPAGE_SIZE;
		}
		mDataContainer.setChannelInfo(channelInfo);
		mDataContainer.setChNum(chNum);
		mDataContainer.setCh_list(ch_list);
	}


	// OnKeyListener.onKey to handle ListView KeyEvent
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		MtkLog.d(TAG, "onKey,keyCode="+keyCode);
		if(isChangeChannel ==false){
			return true;
		}
		if (mMenuSetUpListView == null
				|| mMenuSetUpListView.hasFocus() == false) {
			return false;
		}
		if (mMenuSetUpListView.getAdapter() == null) {
			return false;
		}
		boolean handled = false;
		int action = event.getAction();


		if (action != KeyEvent.ACTION_UP) {
			mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
			mHandler.sendEmptyMessageDelayed(
					MessageType.MENU_AUTO_EXIT_MESSAGE,
					MessageType.delayMillis2);
			mCurrentSelectedPosition = mMenuSetUpListView
					.getSelectedItemPosition();
			if (mCurrentSelectedPosition < 0) {
				mCurrentSelectedPosition = mBeforeSelectedPosition;
			}
			mBeforeSelectedPosition = mCurrentSelectedPosition;
			childView = (RespondedKeyEvent) mMenuSetUpListView
					.getSelectedView();
			mListViewSelectedItemData = (DataItem) mMenuSetUpListView
					.getItemAtPosition(mCurrentSelectedPosition);
			if (childView == null) {
				childView = (RespondedKeyEvent) mMenuSetUpListView
						.getChildAt(mCurrentSelectedPosition);
			}
			if(childView ==null){
				return true;
			}
			switch (keyCode) {
			case KeyMap.KEYCODE_DPAD_LEFT:
				handled = handleHorizontalFocusWithinListItem(KeyMap.KEYCODE_DPAD_LEFT);
				break;
			case KeyMap.KEYCODE_DPAD_RIGHT:
				handled = handleHorizontalFocusWithinListItem(KeyMap.KEYCODE_DPAD_RIGHT);
				if (null != mListViewSelectedItemData && mListViewSelectedItemData.getDataType() == DataType.HAVESUBCHILD) {
					if (mListViewSelectedItemData.getmSubChildGroup() != null
							&& mListViewSelectedItemData.getmSubChildGroup()
									.size() != 0) {
						mTitleMap.put(mExitLevel - 1,
								mTabMenuButtonBar.mButtonShow.getText()
										.toString());
						mTabMenuButtonBar.mButtonShow
								.setText(mListViewSelectedItemData.mName);
					}
				}
				// For Tshift/Pvr
                if (null != mListViewSelectedItemData
                        && MenuConfigManager.DTV_DEVICE_INFO
                                .equals(mListViewSelectedItemData.mItemID)) {
                    exitCalledByTshiftManager();
                    handled = true;
                }
				break;
			case KeyMap.KEYCODE_DPAD_CENTER:
			case KeyEvent.KEYCODE_ENTER:
				handled = handleWithListItemKeyEnterEvent();
				if (null != mListViewSelectedItemData && mListViewSelectedItemData.getDataType() == DataType.HAVESUBCHILD) {
					if (mListViewSelectedItemData.getmSubChildGroup() != null
							&& mListViewSelectedItemData.getmSubChildGroup()
									.size() != 0) {
						mTitleMap.put(mExitLevel - 1,
								mTabMenuButtonBar.mButtonShow.getText()
										.toString());
						mTabMenuButtonBar.mButtonShow
								.setText(mListViewSelectedItemData.mName);
					}

				}
				if (null != mListViewSelectedItemData && MenuConfigManager.TV_CHANNEL_EDIT
						.equals(mListViewSelectedItemData.mItemID)) {
					goToChannelEditorTag = true;
				}
				// For Tshift/Pvr
                if (null != mListViewSelectedItemData
                        && MenuConfigManager.DTV_DEVICE_INFO
                                .equals(mListViewSelectedItemData.mItemID)) {
                    exitCalledByTshiftManager();
                    handled = true;
                }

                // For ScheudleList
                if (null != mListViewSelectedItemData
                        && MenuConfigManager.DTV_SCHEDULE_LIST
                                .equals(mListViewSelectedItemData.mItemID)) {
                    calledByScheduleList();
                    handled = true;
                }

                if (null != mListViewSelectedItemData
                        && MenuConfigManager.DTV_SCHEDULE_LIST
                                .equals(mListViewSelectedItemData.mItemID)) {
                    // tshiftmanager--> stateScheduleList
                    Toast.makeText(
                            mContext,
                            mContext.getString(R.string.menu_setup_schedule_list),
                            Toast.LENGTH_SHORT).show();
                    handled = true;
                    // this.finish();
                }
				break;
			case KeyMap.KEYCODE_MTKIR_CHUP:
				if (mListViewSelectedItemData != null
						&& mListViewSelectedItemData.getDataType() == DataType.CHANNELLISTVIEW) {
					isPressChUPAndChDN =true;
					dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN,
							KeyEvent.KEYCODE_DPAD_DOWN));
				}
				handled = true;
			    break;
			case KeyMap.KEYCODE_MTKIR_CHDN:
				if (mListViewSelectedItemData != null
						&& mListViewSelectedItemData.getDataType() == DataType.CHANNELLISTVIEW) {
					isPressChUPAndChDN =true;
					dispatchKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN,
							KeyEvent.KEYCODE_DPAD_UP));
				}
				handled = true;
				break;
			case KeyMap.KEYCODE_DPAD_DOWN:
			case KeyMap.KEYCODE_DPAD_UP:
				// Digest whether ListView key events
				handled = false;
				setIsMouse(false);
				if (mListViewSelectedItemData != null
						&& mListViewSelectedItemData.getDataType() == DataType.CHANNELLISTVIEW) {
						isChangeChannel =false;
						mEditChannel.channelUpAndDown(keyCode);
						break;
				}
				if (keyCode == KeyMap.KEYCODE_DPAD_DOWN) {
					selIdx = mCurrentSelectedPosition + 1;

					if (selIdx < mGroup.size()) {
						while (!mGroup.get(selIdx).isEnable
								&& selIdx < mGroup.size() - 1) {
							selIdx++;
						}
					}
					if (selIdx == mGroup.size()) {
						selIdx = 0;
					}

				} else {
					selIdx = mCurrentSelectedPosition - 1;
					if (selIdx != -1) {
						while (selIdx > -1 && !mGroup.get(selIdx).isEnable) {
							selIdx--;
						}
					}
					if (selIdx == -1) {
						selIdx = -2;
					}
				}
		

				if (mListViewSelectedItemData != null
						&& mListViewSelectedItemData.getDataType() == DataType.NUMVIEW) {

					if ((MenuConfigManager.TV_CHANNEL_START_FREQUENCY).equals(mListViewSelectedItemData.mItemID)
							|| (MenuConfigManager.TV_CHANNEL_FREQUENCY).equals(mListViewSelectedItemData.mItemID)) {
						text1 = (TextView) mMenuSetUpListView.getChildAt(0)
								.findViewById(R.id.common_tv_itemshow);
						mNumView = (NumView) childView;
						mNumView.onKeyEnter();
					}

					if ((MenuConfigManager.TV_CHANNEL_END_FREQUENCY).equals(mListViewSelectedItemData.mItemID)) {
						mNumView2 = (NumView) childView;
						mNumView2.onKeyEnter();
						text2 = (TextView) mMenuSetUpListView.getChildAt(1)
								.findViewById(R.id.common_tv_itemshow);
						String str1 = text1.getText().toString();
						String str2 = text2.getText().toString();
						if (Integer.parseInt(str1.substring(0, str1
								.indexOf("."))) > Integer.parseInt(str2
								.substring(0, str2.indexOf(".")))) {
							text1.setText(str2);
							text2.setText(str1);
							mGroup.get(0).mInitValue = Integer.parseInt(str2
									.substring(0, str2.indexOf(".")));
							mNumView.changeValueLister(mNumView
									.getValuedListener(), mGroup.get(0));
							mGroup.get(1).mInitValue = Integer.parseInt(str1
									.substring(0, str1.indexOf(".")));
							mNumView2.changeValueLister(mNumView2
									.getValuedListener(), mGroup.get(1));

						}
					}
				}
				if (mExitLevel == 3 && mListViewSelectedItemData != null
						&& mListViewSelectedItemData.getDataType() == DataType.PASSWORDVIEW
						&& childView != null) {
					PasswordView passwordView = (PasswordView) childView;
					if (passwordView.getmId().equals(
							MenuConfigManager.PARENTAL_PASSWORD_NEW)) {
						if (passwordView.getmShowView().getText().toString()
								.length() == 4) {
							mMenuSetUpListView.setSelection(1);
						} else {
							mMenuSetUpListView.setSelection(0);
						}
					} else if (passwordView.getmId().equals(
							MenuConfigManager.PARENTAL_PASSWORD_NEW_RE)) {
						mMenuSetUpListView.setSelection(1);
					}
					handled = true;
				}
				if (mCurrentFouceButtonID == 4
						&& mExitLevel == 4
						&& ((TextView) mMenuSetUpListView.getChildAt(0)
								.findViewById(R.id.common_tv_itemshow))
								.getText().toString().equals(
										channelPowerOnArray[0])) {
					handled = true;
				}
				break;
			case KeyMap.KEYCODE_BACK:
				handled = false;
				break;
			case KeyMap.KEYCODE_MTKIR_RED:
			case KeyEvent.KEYCODE_R:
				handled = false;
				if (mListViewSelectedItemData.getDataType() == DataType.CHANNELLISTVIEW) {
					keyColor = "red";
					switchChannel_from_page = mMenuSetUpListView
							.getCurrentPageNum();
					switchChannel_from = mCurrentSelectedPosition
							+ (switchChannel_from_page - 1) * mPageSize;
					String[] strtemp = mListViewSelectedItemData
							.getmOptionValue();
					switchChannel_from_num = strtemp[0];
					handled = true;
				}
				break;
			case KeyMap.KEYCODE_MTKIR_GREEN:
			case KeyEvent.KEYCODE_G:
				handled = false;
				if (mListViewSelectedItemData.getDataType() == DataType.CHANNELLISTVIEW) {
					keyColor = "green";
					insertChannel_from_page = mMenuSetUpListView
							.getCurrentPageNum();
					insertChannel_from = mCurrentSelectedPosition
							+ (insertChannel_from_page - 1) * mPageSize;
					handled = true;
				}
				break;
			case KeyMap.KEYCODE_MTKIR_BLUE:
			case KeyEvent.KEYCODE_B:
				handled = false;
				if (mListViewSelectedItemData.getDataType() == DataType.CHANNELLISTVIEW) {

//					if (isDeleting) {
//						return true;
//					}
//
//					isDeleting = true;
//
//					mHandler.postDelayed(new Runnable() {
//
//						public void run() {
//							// TODO Auto-generated method stub
//							isDeleting = false;
//						}
//					}, 2000);

					keyColor = "blue";

					int deleteId = mCurrentSelectedPosition
							+ (mMenuSetUpListView.getCurrentPageNum() - 1)
							* mPageSize;
					List<TVChannel> lt = mEditChannel.deleteChannel(deleteId);
					mDataContainer.getChannelDataGroup().remove(deleteId);
					List<String> list = new ArrayList<String>();
					for (String s : oldChannelNum) {
						list.add(s);
					}
					list.remove(deleteId);
					oldChannelNum = new String[list.size()];
					for (int i = 0; i < list.size(); i++) {
						oldChannelNum[i] = list.get(i);
					}
					keyColor = "";
					setAdapter(mDataContainer.getChannelDataGroup(), 1);
					mCurrentSelectedPosition = 0;
					if (null != lt && lt.size() < 1) {
						--mExitLevel;
						if (editChannelViewBottom.getVisibility() == View.VISIBLE
								|| editChannelViewMenu.getVisibility() == View.VISIBLE) {

							editChannelViewBottom.setVisibility(View.GONE);
							editChannelViewMenu.setVisibility(View.GONE);
							mPageSize = NORMALPAGE_SIZE;
							mMenuSetUpListView.setVisibility(View.VISIBLE);
							mMenuSetUpListView.requestFocus();
						}
						if (mSecondLevelData.mParentGroup != null) {
							List<TVChannel> channelList = mTM.getChannels();
							if (null != channelList && channelList.size() == 0) {
								mSecondLevelData.mParentGroup.get(
										TV_DISABLE_ITEMS[0]).setEnable(false);
								mSecondLevelData.mParentGroup.get(
										TV_DISABLE_ITEMS[1]).setEnable(false);
							} else {
								mSecondLevelData.mParentGroup.get(
										TV_DISABLE_ITEMS[0]).setEnable(true);
								mSecondLevelData.mParentGroup.get(
										TV_DISABLE_ITEMS[1]).setEnable(true);
							}
						}
						reloadParentPageData(mExitLevel);
					} else {
						if(null != lt){
						mChSel.select(lt.get(0));
						}
						selIdx = 0;
						mMenuSetUpListView
								.setSelection(mCurrentSelectedPosition);
					}
					handled = true;
				}
				break;
			}
		}
		if (handled) {
			return true;
		} else {
			return false;
		}

	}

	@SuppressWarnings("deprecation")
	private void showNextLevelCofigPage() {
		MtkLog.d(TAG,"showNextLevelConfigPage");
		mBeforeSelectedPosition = mCurrentSelectedPosition;
		++mExitLevel;
		/** Loading and Search channel data sets */
		if (!mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.RESET_DEFAULT)
				&& !mListViewSelectedItemData.getmItemID().equals("upgradeNet")
				&& !mListViewSelectedItemData.getmItemID().equals(
						"ConnectionTest")
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.TV_CHANNEL_CLEAR)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.PARENTAL_CLEAN_ALL)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.FACTORY_SETUP_CLEAN_STORAGE)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.TV_STORE)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.AUTO_ADJUST)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.FACTORY_SETUP_CI_ERASE)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.FACTORY_SETUP_CI_UPDATE)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.FACTORY_SETUP_CI_QUERY)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.TV_CHANNEL_NAME)
				&& !mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.TV_FINETUNE)) {
			selIdx = 0;
		}
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.TV_CHANNEL_EDIT)
				|| mListViewSelectedItemData.getmItemID().equals("PowerOnCh")
				|| mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.PARENTAL_CHANNEL_BLOCK)) {
			if (mListViewSelectedItemData.getmSubChildGroup().size() > 0) {
				mListViewSelectedItemData.getmSubChildGroup().clear();
			}
			mListViewAdpter.notifyDataSetChanged();
			getTVData();
			if (mListViewSelectedItemData.getmItemID().equals("PowerOnCh")) {
				mDataContainer.loadPowerOnChannelData();
			} else if (mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.PARENTAL_CHANNEL_BLOCK)) {
				mDataContainer.loadParentalChannelData();
			} else {
				mDataContainer.loadChannelData();
			}
			mListViewAdpter.notifyDataSetChanged();
			saveParentPageData(mExitLevel);
		}

//		if (mListViewSelectedItemData.getmItemID().equals("Network")) {
//			if (!isDhcpRegistered) {
//				netWork.startRegisterDhcp();
//				isDhcpRegistered = true;
//			}
//		}

		/** load analog diag */
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.FACTORY_TV_TUNER_DIAGNOSTIC)) {
			mDataContainer.getFvTunerDiagnostic().getmSubChildGroup().clear();
			mDataContainer.loadTuneDiagInfo(false);
		}
		/** load 3d data */
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.VIDEO_3D)) {
			mDataContainer.getM3DItem().getmSubChildGroup().clear();
			mDataContainer.load3DData();
		}
		if(mListViewSelectedItemData.getmItemID().equals(
							MenuConfigManager.TV_SINGLE_RF_SCAN)){
			hasChangeFreq = false;
			mDataContainer.loadDTMBSingleRF();
			if (mcf.getDefault(MenuConfigManager.TUNER_MODE) == ConfigType.BS_SRC_AIR) {
			    MtkLog.d(TAG, "load init scan MENU_TV_RF_SCAN_REFRESH MENU_TV_RF_SCAN_CONNECTTURN getsignal");
			    mHandler.removeMessages(MessageType.MENU_TV_RF_SCAN_CONNECTTURN);
                mHandler.sendEmptyMessageDelayed(
                        MessageType.MENU_TV_RF_SCAN_CONNECTTURN,
                        MessageType.delayMillis6);
            }
		}
		/** load time setup data */
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.SETUP_TIME_SETUP)) {
			mDataContainer.loadTimeSetUpData();
		}

		/** load channel scan and update scan data */
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.TV_CHANNEL_SCAN)
				|| mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.TV_UPDATE_SCAN)) {
			mDataContainer.getChannelScan().clear();
			NavIntegration mNavIntegration = NavIntegration
					.getInstance(mContext);
			if(!mNavIntegration.isCurrentSourceDTV()){
			    mDataContainer.loadDataChannelScan();
				TVScan.mDataItem = mListViewSelectedItemData;
			} else {
				showDTVScan();
			}
		}

		/** load time data */
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.SETUP_TIME_SET)) {
			mDataContainer.getTimeSet().getmSubChildGroup().clear();
			mDataContainer.loadTimeData();
		}
		/** load TV source data */
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.PARENTAL_INPUT_BLOCK)) {
			mDataContainer.getInputBlock().getmSubChildGroup().clear();
			String[] tvSourceName = mDataContainer.loadParentalSourceData();
			if(null != tvSourceName){
			for (int i = 0; i < tvSourceName.length; i++) {
				if (tvSourceName[i].equals(mDataContainer.mCurrentSource)) {
					goToSourcePosition = i;
					break;
					}
				}
			}
		}
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.SETUP_APPLICATION)) {
			mDataContainer.getApplication().getmSubChildGroup().clear();
			mDataContainer.loadApplicationData();
		}
		if (mListViewSelectedItemData.getmSubChildGroup() != null
				&& mListViewSelectedItemData.getmSubChildGroup().size() > 0) {
			// From the beat the third level directory, need to start save
			// parent page data
			
			
			if(AnimationManager.getInstance().getIsAnimation()){
	//			kfjkahfdk'
	//			mDataContainer.load3DData();
				mLastAdapter.setmGroup(mListViewAdpter.getmGroup());
	//			mMenuSetUpListView.setRotationY(90f);
	//			mMenuSetUpListView.setAlpha(0f);
				tempListView.setVisibility(View.VISIBLE);
//				mMenuSetUpListView.setVisibility(View.GONE);
				mMenuSetUpListView.setAlpha(0);
				tempListView.setAlpha(1);
				tempListView.setScaleX(1);
				tempListView.setScaleY(1);
	//			tempListView.set
	//			Log.e("plz", "button audio:******************");
				
				tempListView.setAdapter(mLastAdapter);
				
				
	//			float locPositon=mMenuSetUpListView.getTranslationX();
	//			float width=mMenuSetUpListView.getWidth();
	//			tempListView.setTranslationX(locPositon);
				int dely=100;
				ObjectAnimator rAlpAnim=ObjectAnimator.ofFloat(tempListView, "alpha", 1.0f , 0f);
				rAlpAnim.setDuration(dely);
	//			ObjectAnimator rRotationYAnim=ObjectAnimator.ofFloat(tempListView, "rotationY", 0.0f,-90.0f);
	//			rRotationYAnim.setDuration(dely);
				ObjectAnimator rscaleXAnim=ObjectAnimator.ofFloat(tempListView, "scaleX", 1.0f,0f);
				rscaleXAnim.setDuration(dely);
				ObjectAnimator rscaleYAnim=ObjectAnimator.ofFloat(tempListView, "scaleY", 1.0f,0f);
				rscaleYAnim.setDuration(dely);
	//			ObjectAnimator rmoveAnim=ObjectAnimator.ofFloat(tempListView, "translationX", 0,-(width*3/4));
	//			rmoveAnim.setDuration(dely);
				
				ObjectAnimator AAlpAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "alpha", 0.0f,1.0f);
				AAlpAnim.setDuration(dely);
	//			ObjectAnimator aRotationYAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "rotationY", 90.0f,0.0f);
	//			aRotationYAnim.setDuration(dely);
				ObjectAnimator ascaleXAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "scaleX", 0f,1f);
				ascaleXAnim.setDuration(dely);
				ObjectAnimator ascaleYAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "scaleY", 0f,1f);
				ascaleYAnim.setDuration(dely);
	//			ObjectAnimator AmoveAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "translationX", locPositon+width,locPositon);
	//			AmoveAnim.setDuration(dely);
				AnimatorSet total=new AnimatorSet();
				AnimatorSet set1 =new AnimatorSet();
				set1.playTogether(rAlpAnim,rscaleXAnim,rscaleYAnim);
	//			set1.start();
				AnimatorSet setAfter=new AnimatorSet();
				setAfter.playTogether(AAlpAnim,ascaleXAnim,ascaleYAnim);
				
				set1.addListener(new AnimatorListenerAdapter(){
					@Override
					public void onAnimationEnd(Animator anim) {
		//				linearList.removeView(tempListView);
//						mMenuSetUpListView.setVisibility(View.VISIBLE);
						tempListView.setVisibility(View.GONE);
		//				tempListView=null;
					}
					
				});
				total.playSequentially(set1,setAfter);
				total.start();
			}
			saveParentPageData(mExitLevel);
			if (mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.PARENTAL_CHANNEL_BLOCK)) {
				setAdapter(mListViewSelectedItemData.getmSubChildGroup(),
						gotoPage);
			} else if (mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.PARENTAL_INPUT_BLOCK)) {
				gotoPage = goToSourcePosition / 9 + 1;
				setAdapter(mListViewSelectedItemData.getmSubChildGroup(),
						gotoPage);
			} else if (mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FA_A2SYSTEM)
					|| mListViewSelectedItemData.getmItemID().equals(
							MenuConfigManager.FA_PALSYSTEM)) {
				mMenuSetUpListView.initData(mListViewSelectedItemData
						.getmSubChildGroup(), mPageSize, mMyUpData, 1);
				mGroup = (List<DataItem>) mMenuSetUpListView.getCurrentList();
				mListViewAdpter.setmGroup(mGroup);
				mMenuSetUpListView.setAdapter(mListViewAdpter);
				return;
			} else {
				if (mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.FACTORY_VIDEO)) {
					mListViewSelectedItemData = mDataContainer
							.refreshVideoFactorySub(mListViewSelectedItemData);
				}

					if ( mListViewSelectedItemData != null && mListViewSelectedItemData
							.getmItemID().equals(
						MenuConfigManager.VIDEO_ADVANCED_VIDEO)) {
					mDataContainer.loadAdvancedData();
				}
					if (mListViewSelectedItemData != null && mListViewSelectedItemData.getmItemID().equals(
							MenuConfigManager.TV_ANALOG_SCAN)) {
						mDataContainer.loadAnaLogChild();
					}

					if (mListViewSelectedItemData != null &&
						 mListViewSelectedItemData.mSubChildGroup.size() > 0) {
					setAdapter(mListViewSelectedItemData.getmSubChildGroup());
				}
			}
			/**
			 * Judge channel editor section shows a header and trailer,Define
			 * variable editChannelView = true
			 */
				if (mListViewSelectedItemData != null && mListViewSelectedItemData
						.getmItemID().equals(MenuConfigManager.TV_CHANNEL_EDIT)) {
				editChannelViewMenu.setVisibility(View.VISIBLE);
				editChannelViewBottom.setVisibility(View.VISIBLE);
				selIdx = gotoPosition;
				mMenuSetUpListView.setSelection(gotoPosition);
				oldChannelNum = new String[mDataContainer.getChannelDataGroup()
						.size()];
				// all num in channel save in alone array
				for (int i = 0; i < mDataContainer.getChannelDataGroup().size(); i++) {
					String[] optionStr = mDataContainer.getChannelDataGroup()
							.get(i).getmOptionValue();
					oldChannelNum[i] = optionStr[0];
				}
				return;
			}
				if (mListViewSelectedItemData != null
						&& mListViewSelectedItemData.getmItemID().equals(
								MenuConfigManager.PARENTAL_CHANNEL_BLOCK)) {
				mMenuSetUpListView.setSelection(gotoPosition);
				return;
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.PARENTAL_INPUT_BLOCK)) {
				mMenuSetUpListView.setSelection(goToSourcePosition);
				return;
			}
			/** simulation manual search channel */
			if (null != mListViewSelectedItemData) {
			if (mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_ANALOG_SCAN)
					|| mListViewSelectedItemData.getmItemID().equals(
							MenuConfigManager.TV_SINGLE_RF_SCAN)) {
				MtkLog.d(TAG,"load analogProgressView");
				linearScan.setVisibility(View.VISIBLE);
				mTextViewSelect.setVisibility(View.INVISIBLE);
				// Analog Manual Scan
				mScanningStatus = true;
				angProgressView = (AnalogProgressView) linearScan.getChildAt(0);
				int viewHeight = (int) ((int) ScreenConstant.SCREEN_HEIGHT * 0.4);
				int wiewWeight = (int) ((int) ScreenConstant.SCREEN_WIDTH * 0.64);
				linearScan.setLayoutParams(new RelativeLayout.LayoutParams(
						wiewWeight, viewHeight));
				angProgressView.setLayoutParams(new LinearLayout.LayoutParams(
						wiewWeight * 2 / 3, viewHeight * 5 / 8));
				linearScan.setPadding(0, 40, 0, 0);
				analogScanBar.getProbarLine().setLayoutParams(
						new LinearLayout.LayoutParams(wiewWeight,
								LayoutParams.WRAP_CONTENT));
				analogScanBar.getProbarLine().setPadding(20, 20, 0, 30);
				if (mNavIntegration.isCurrentSourceDTV()) {
					mStateTextView.setText(MenuMain.this
							.getString(R.string.menu_tv_single_rf_scan_init));
				} else {
					mStateTextView
							.setText(MenuMain.this
									.getString(R.string.menu_tv_analog_manual_scan_init));
				}

				loading.setVisibility(View.GONE);
				mAnaloguechannel.setVisibility(View.GONE);
				mNumberChannel.setVisibility(View.GONE);
				mFinishpercentage.setText("0%");
				analogScanBar.setProgress(0);
				mTextViewEnter.setVisibility(View.INVISIBLE);
			}
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_CHANNEL)) {
				List<TVChannel> channelList = mTM.getChannels();
				if (null != channelList && channelList.size() == 0) {
					mListViewSelectedItemData.mSubChildGroup.get(
							TV_DISABLE_ITEMS[0]).setEnable(false);
					mListViewSelectedItemData.mSubChildGroup.get(
							TV_DISABLE_ITEMS[1]).setEnable(false);
				} else {
					mListViewSelectedItemData.mSubChildGroup.get(
							TV_DISABLE_ITEMS[0]).setEnable(true);
					mListViewSelectedItemData.mSubChildGroup.get(
							TV_DISABLE_ITEMS[1]).setEnable(true);
				}
				setAdapter(mListViewSelectedItemData.getmSubChildGroup());
			}
//			if (mListViewSelectedItemData.getmItemID().equals(
//					MenuConfigManager.SETUP_APPLICATION)) {

//				if (saveValue.readValue(MenuConfigManager.NETWORK_CONNECTION) == 0) {
//					mMenuSetUpListView.setFocusable(false);
//					application_subpage = CHILD_DISABLE;

//				} else {
//					mMenuSetUpListView.setFocusable(true);
//				}

//			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.PARENTAL_PASSWORD)) {
				List<TVChannel> chaneList = mTM.getChannels();
				if ((null != chaneList&& chaneList.size() == 0)
						|| TVInputManager.INPUT_TYPE_VGA.equals(TVContent
								.getInstance(MenuMain.this).getInputManager()
								.getTypeFromInputSource(
										mDataContainer.mCurrentSource))
						|| TVInputManager.INPUT_TYPE_AV.equals(TVContent
								.getInstance(MenuMain.this).getInputManager()
								.getTypeFromInputSource(
										mDataContainer.mCurrentSource))) {
					mListViewSelectedItemData.mSubChildGroup.get(0).setEnable(
							false);
				} else {
					mListViewSelectedItemData.mSubChildGroup.get(0).setEnable(
							true);
				}
				mMenuSetUpListView.setAdapter(mListViewAdpter);
				PasswordView mPasswordView=(PasswordView)childView;
				mPasswordView.setInputNum(true);
			}
		} else {
			// With no children here children configuration parameters, and pop
			// up a processing interface
			/** setup part :version info */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.VERSION_INFO)) {
				mTitleMap.put(mExitLevel - 1, mTabMenuButtonBar.mButtonShow
						.getText().toString());
				mTabMenuButtonBar.mButtonShow
						.setText(mListViewSelectedItemData.mName);
				versionInfoLinear.setVisibility(View.VISIBLE);
				linearList.setVisibility(View.GONE);
				mMenuSetUpListView.setFocusable(false);
				return;
			}

			// common interface
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.COMMON_INTERFACE)) {
				mHandler.sendEmptyMessage(MessageType.CI_NAME_MESSAGE);
				return;
			}

			// MJC_DEMO
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.DEMO)) {
				final MenuMjcDemoDialog mjcDemoDialog = new MenuMjcDemoDialog(
						MenuMain.this);
				mjcDemoDialog.show();
				mRootRelativeLayout.setVisibility(View.GONE);
				mBypassWindowManager.setBypassWindow(false,
						mRootLayoutwindowId, mRootRelativeLayoutRect);

				mjcDemoDialog
						.setOnKeyListener(new DialogInterface.OnKeyListener() {
							public boolean onKey(DialogInterface dialog,
									int keyCode, KeyEvent event) {
								if (event.getAction() == KeyEvent.ACTION_DOWN) {
									switch (keyCode) {
									case KeyEvent.KEYCODE_MENU:
										mjcDemoDialog.dismiss();
										exit();
										mBypassWindowManager.setBypassWindow(
												false, mRootLayoutwindowId,
												mRootRelativeLayoutRect);
										return true;
										
									case KeyMap.KEYCODE_MTKIR_CHDN:
										mChSel.channelDown();
										exit();
										return true;
										
									case KeyMap.KEYCODE_MTKIR_CHUP:
										mChSel.channelUp();
										mBypassWindowManager.setBypassWindow(
												false, mRootLayoutwindowId,
												mRootRelativeLayoutRect);
										exit();
										return true;
										
									case KeyMap.KEYCODE_MTKIR_PRECH:
										mChSel.selectPrev();
										mBypassWindowManager.setBypassWindow(
												false, mRootLayoutwindowId,
												mRootRelativeLayoutRect);
										exit();
										return true;
										
									// mute
									case KeyMap.KEYCODE_MTKIR_MUTE:
										TurnkeyUiMainActivity.switchMute();
										return true;
										
									// MMP
									case KeyMap.KEYCODE_MTKIR_ANGLE:
										TurnkeyUiMainActivity.setMuteGone();
										NewPipLogic pl = NewPipLogic
												.getInstance(MenuMain.this);
										pl.pauseMainOutput();
										Intent intent = new Intent();
										intent.setClass(MenuMain.this,
												MeidaMainActivity.class);
										startActivity(intent);
										MenuMain.this.exit();
										mBypassWindowManager.setBypassWindow(
												false, mRootLayoutwindowId,
												mRootRelativeLayoutRect);
										return true;
										
									case KeyMap.KEYCODE_MTKIR_GUIDE:
										if (TVInputManager.INPUT_TYPE_TV
												.equals(mTV
														.getInputManager()
														.getTypeFromInputSource(
																mDataContainer.mCurrentSource))) {
											Intent mintent = new Intent();
											mintent.setClass(MenuMain.this,
													EPGActivity.class);
											startActivity(mintent);
											MenuMain.this.exit();
											mBypassWindowManager
													.setBypassWindow(
															false,
															mRootLayoutwindowId,
															mRootRelativeLayoutRect);
										}
										return true;
										
									case KeyMap.KEYCODE_VOLUME_DOWN:
									case KeyMap.KEYCODE_VOLUME_UP:
										handleVolume(keyCode);
										return true;

									default:
										mjcDemoDialog.dismiss();
										mExitLevel--;
										mRootRelativeLayout
												.setVisibility(View.VISIBLE);
										mRootLayoutwindowId = mBypassWindowManager
												.getAvailableWindowId();
										mBypassWindowManager.setBypassWindow(
												true, mRootLayoutwindowId,
												mRootRelativeLayoutRect);
										mMenuSetUpListView.requestFocus();
										return false;
									}

								}
								return false;

							}
						});

				return;
			}


			/*
			 * video - vga - auto adjust I do not realize the specific function
			 */
			if (null != mListViewSelectedItemData) {
			if (mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.AUTO_ADJUST)
					|| mListViewSelectedItemData.getmItemID().equals(
							MenuConfigManager.FV_AUTOCOLOR)
					|| mListViewSelectedItemData.getmItemID().equals(
							MenuConfigManager.FV_AUTOPHASE)) {
				autoAdjustInfo(MenuMain.this
						.getString(R.string.menu_video_auto_adjust_info));
				int item = 0;
				if (mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.AUTO_ADJUST)) {
					item = TVCommonInterface.AUTO_ADJUST;
				} else if (mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.FV_AUTOCOLOR)) {
					item = TVCommonInterface.AUTO_COLOR;
				} else {
					item = TVCommonInterface.AUTO_PHASE;
				}
				tvCommon.autoAdjust(item, new AdjustListener() {

					public void onEvent(int event) {
						// TODO Auto-generated method stub
						MtkLog.d(TAG,
								"------------received auto adjust event: "
										+ event);
						if (event == 0) {
							autoAdjustDialog.dismiss();
							mBypassWindowManager.setBypassWindow(false,
									autoAdjustDialogWindowId,
									autoAdjustDialogRect);
							mExitLevel--;
							if (mListViewSelectedItemData.getmItemID().equals(
									MenuConfigManager.AUTO_ADJUST)) {
								mDataContainer.loadVGA();
								setAdapter(mDataContainer.getVGADataGroup());
							} else {
								DataItem parent = mListViewSelectedItemData
										.getmParent();
								if (parent != null) {
									parent = mDataContainer
											.refreshVideoFactorySub(parent);
									if (parent != null && parent.getmSubChildGroup().size() > 0) {
										setAdapter(parent.getmSubChildGroup());
									}
								}
							}
							// mListViewAdpter.setmGroup(mDataContainer
							// .getVGADataGroup());
							// mMenuSetUpListView.setAdapter(mListViewAdpter);
							mTabMenuButtonBar.mButtonShow.setText(mTitleMap
									.get(mExitLevel));
							mRootRelativeLayout.setVisibility(View.VISIBLE);
							mRootLayoutwindowId = mBypassWindowManager
									.getAvailableWindowId();
							mBypassWindowManager.setBypassWindow(true,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);

							mMenuSetUpListView.requestFocus();
						}
					}
				});
				return;
			}
			}
		
			/**
			 * setup part :display reset default
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.RESET_DEFAULT)) {
				resetConfirm();
				return;
			}
			/**
			 * TV part :Show clear channel list dialog
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_CHANNEL_CLEAR)) {
				cleanChannelConfirm();
				return;
			}
			/**
			 * TV part :Channel editor _ to enter
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_CHANNEL_EDIT)) {
				saveParentPageData(mExitLevel);
				editChannelViewMenu.setVisibility(View.VISIBLE);
				editChannelViewBottom.setVisibility(View.VISIBLE);
				mMenuSetUpListView.setVisibility(View.GONE);
				selIdx = gotoPosition;
				return;
			}
			/**
			 * TV part :Channel editor finetune_dialogInfo fine-tuning
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_FINETUNE)) {
				finetuneInfoDialog();
				return;
			}
			/**
			 * TV part :Channel editor store_dialogInfo
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_STORE)) {
				storeChannel();
				return;
			}
			/**
			 * ---TV part:---channelNameEdit
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_CHANNEL_NAME)) {
				channelNameEdit();
				return;
			}
			/**
			 * netWork update
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals("upgradeNet")) {
				networkUpdate();
				return;
			}
			/**
			 * parental:clean all
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.PARENTAL_CLEAN_ALL)) {
				cleanParentalChannelConfirm();
				return;
			}

			/**
			 * manual search channel
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.TV_CHANNEL_STARTSCAN)) {
				mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
				mTextViewEnter.setVisibility(View.INVISIBLE);
				mTextViewSelect.setVisibility(View.INVISIBLE);
				mTextViewExit.setText(getString(R.string.menu_cancel));
				// Analog Manual Scan
				mScanningStatus = true;
				isScanning = true;
				mMenuSetUpListView.clearFocus();
				linearScan.requestFocus();
				progressHandler = new Handler();
				TVScanner tv_scan = mcf.getScanner();
				angProgressView.loading.drawLoading();
				angProgressView.setHandler(mHandler);
				freFrom = getFreFrom();
				freTo = getFreTo();
				angProgressView.scan(freFrom, freTo);
			}

			/**
			 * Factory part :clean Restorage
			 */
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FACTORY_SETUP_CLEAN_STORAGE)) {
				factoryCleanStorage();
				return;
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FACTORY_SETUP_CI_ERASE)) {
				factoryCi(2);
				return;
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FACTORY_SETUP_CI_UPDATE)) {
				factoryCi(1);
				return;
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FACTORY_SETUP_CI_QUERY)) {
				factoryCi(3);
				return;
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FACTORY_PRESET_CH_DUMP)) {
				--mExitLevel;
				return;
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FACTORY_PRESET_CH_PRINT)) {
				--mExitLevel;
				FactoryPresetChannel fpc = new FactoryPresetChannel(
						MenuMain.this);
				fpc.dumpChannels();
				return;
			}
			if (mListViewSelectedItemData != null
					&&mListViewSelectedItemData.getmItemID().equals(
					MenuConfigManager.FACTORY_PRESET_CH_RESTORE)) {
				--mExitLevel;
				FactoryPresetChannel fpc = new FactoryPresetChannel(
						MenuMain.this);
				fpc.presetChannels();
				return;
			}
			return;
		}
	}

	// At the next higher level for ListView outfit data
	private void setParentAdapter(ParentData parent) {
		if (parent != null) {
			if (backEditView.equals(BACKEDITVIEW)) {
				parent.mPageIndex = gotoPage;
				parent.mSelectedPosition = gotoPosition;
			}
			mMenuSetUpListView.initData(parent.mParentGroup, mPageSize,
					mMyUpData, parent.mPageIndex);
			backEditView = "";
			mGroup = (List<DataItem>) mMenuSetUpListView.getCurrentList();
			mListViewAdpter.setmGroup(mGroup);
			mMenuSetUpListView.setAdapter(mListViewAdpter);
			mMenuSetUpListView.requestFocus();
			mMenuSetUpListView.setSelection(parent.mSelectedPosition);
			selIdx = parent.mSelectedPosition;
			MtkLog.d(TAG, "parent.mSelectedPosition-------------------->"
					+ parent.mSelectedPosition);
		}

	}
	
	private void handleVolume(int keyCode){
		if (mNavIntegration.isMute()) {
			TurnkeyUiMainActivity.switchMute();
		} else {
			TVOptionRange<Integer> volumeOption;
			TVConfigurer tvcfg = mTV.getConfigurer();
			volumeOption = (TVOptionRange<Integer>) tvcfg
					.getOption(ConfigType.CFG_VOLUME);
			int current = 0,max = 0,min = 0;
			if (volumeOption != null) {
				current = volumeOption.get();
				max = volumeOption.getMax();
				min = volumeOption.getMin();
				int value = -1;
			if (keyCode == KeyEvent.KEYCODE_VOLUME_UP) {
				value = 1;
			}
			current += value;
			if (current >= min && current <= max) {
				volumeOption.set(current);
			} else if (current > max) {
				volumeOption.set(max);
			} else {
				volumeOption.set(min);
				}
			}
		}
	}

	private SetConfigListViewAdapter mLastAdapter=null;
	// ListView load Original data
	private void setAdapter(List<?> adpter) {
		long start = System.currentTimeMillis();
		int pageIndex = 0;
		if (mListViewSelectedItemData != null) {
			if (MenuConfigManager.TV_CHANNEL_EDIT
					.equals(mListViewSelectedItemData.getmItemID())) {
				mPageSize = CHANNELPAGE_SIZE;
				pageIndex = gotoPage;
			} else {
				mPageSize = NORMALPAGE_SIZE;
				pageIndex = 0;
			}
		}
		mMenuSetUpListView.initData(adpter, mPageSize, mMyUpData, pageIndex);
		mGroup = (List<DataItem>) mMenuSetUpListView.getCurrentList();
		mListViewAdpter.setmGroup(mGroup);
		mMenuSetUpListView.setAdapter(mListViewAdpter);
	} // ListView load Original data,Designated page

	private void setAdapter(List<?> adpter, int pageNum) {
		long start = System.currentTimeMillis();
		mMenuSetUpListView.initData(adpter, mPageSize, mMyUpData, pageNum);
		mGroup = (List<DataItem>) mMenuSetUpListView.getCurrentList();
		mListViewAdpter.setmGroup(mGroup);
		mMenuSetUpListView.setAdapter(mListViewAdpter);
	}

	// Save directory data at the next higher level
	private void saveParentPageData(int mExitLevel) {
		ParentData parent = null;
		switch (mExitLevel) {
		case 2:
			parent = mFirstLevelData;
			break;
		case 3:
			parent = mSecondLevelData;
			break;
		case 4:
			parent = mThirdLevelData;
			break;
		case 5:
			parent = mForthLevelData;
			break;
		case 6:
			parent = mFifthLevelData;
			break;
		case 7:
			parent = mSixthLevelData;
			break;
		default:
			// TODO to be extended
			break;
		}

		setParentData(parent);

		return;
	}

	// Return to the directory data out level
	private void reloadParentPageData(int mExitLevel) {
		switch (mExitLevel) {
		case 1:
			setParentAdapter(mFirstLevelData);
			break;
		case 2:
			setParentAdapter(mSecondLevelData);
			break;
		case 3:
			setParentAdapter(mThirdLevelData);
			break;
		case 4:
			setParentAdapter(mForthLevelData);
			break;
		case 5:
			setParentAdapter(mFifthLevelData);
			break;
		case 6:
			setParentAdapter(mSixthLevelData);
			break;

		default:
			// TODO If more than sixth level, return to the default menu now,
			// to be extended;
			setAdapter(mDataContainer.getVideoDataGroup());
			mMenuSetUpListView.setSelection(mBeforeSelectedPosition);
			break;
		}

		return;
	}

	// According to choose the type of item ListView, adjust the left lower
	// window tips button displays
	private void setFunctionTipVisibility(DataType dataType) {
		if (null == dataType) {
			return;
		}

		mTextViewExit.setText(R.string.menu_back);
		if (dataType == DataType.OPTIONVIEW
				|| dataType == DataType.EFFECTOPTIONVIEW
				|| dataType == DataType.SWICHOPTIONVIEW) {
			mTextViewEnter.setVisibility(View.INVISIBLE);
			if (dataType == dataType.SWICHOPTIONVIEW
					&& mExitLevel == 4
					&& mMenuSetUpListView.getChildAt(mMenuSetUpListView
							.getSelectedItemPosition()) != null) {
				if (((SwitchOptionView) mMenuSetUpListView
						.getChildAt(mMenuSetUpListView
								.getSelectedItemPosition())).getmId().equals(
						MenuConfigManager.POWER_ON_CH_MODE)) {
					mTextViewEnter.setVisibility(View.VISIBLE);
					mTextViewEnter.setText(R.string.menu_set);
				}
			}
		} else {
			MtkLog.i(TAG, "-----TEXT VIEW ENTER----");
			if (dataType == dataType.SCANCHANNELSOPTIONVIEW
					&& ((ScanChannelsOptionView) mMenuSetUpListView
							.getChildAt(mMenuSetUpListView
									.getSelectedItemPosition())).getmId()
							.equals(MenuConfigManager.SCAN_MODE)) {
				MtkLog.i(TAG, "-----TEXT VIEW ENTER----INVISIBLE");
				mTextViewEnter.setVisibility(View.INVISIBLE);
			} else {
				MtkLog.i(TAG, "-----TEXT VIEW ENTER----VISIBLE");
				mTextViewEnter.setVisibility(View.VISIBLE);
			}
		}

		if (dataType == DataType.PROGRESSBAR
				|| dataType == DataType.POSITIONVIEW) {
			mTextViewEnter.setText(R.string.menu_adjust);
		} else if (dataType == DataType.HAVESUBCHILD){
			
			mTextViewEnter.setText(R.string.menu_enter);
			mTextViewSelect.setVisibility(View.VISIBLE);
				if(mListViewSelectedItemData != null
						&& mListViewSelectedItemData.getmItemID() != null
						&& !(mListViewSelectedItemData.getmItemID().length()== 0)
						&& mListViewSelectedItemData.getmItemID() != null && (mListViewSelectedItemData
						.getmItemID().equals(MenuConfigManager.TV_ANALOG_SCAN))) {
				mTextViewEnter.setText(R.string.menu_enter);
				mTextViewSelect.setVisibility(View.VISIBLE);
				}
				
		} else if (dataType == DataType.CHANNELLISTVIEW
				|| dataType == DataType.CHANNELPOWERONVIEW
				|| dataType == DataType.TVSOURCEVIEW) {
			mTextViewEnter.setText(R.string.menu_set);
		} else if (dataType == DataType.PASSWORDVIEW) {
			mTextViewEnter.setVisibility(View.INVISIBLE);
			mTextViewSelect.setVisibility(View.INVISIBLE);
		} else if (dataType == DataType.NUMVIEW
				|| mListViewSelectedItemData != null
				&& mListViewSelectedItemData.getmItemID() != null
				&& !(mListViewSelectedItemData.getmItemID().length()==0)
				&& mListViewSelectedItemData.getmItemID() != null
				&& mExitLevel == 3
				&& (mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.TV_ANALOG_SCAN)
						|| mListViewSelectedItemData.getmItemID().equals(
								MenuConfigManager.TV_CHANNEL_START_FREQUENCY) || mListViewSelectedItemData
						.getmItemID().equals(
								MenuConfigManager.TV_CHANNEL_END_FREQUENCY)))  {
			mTextViewEnter.setVisibility(View.INVISIBLE);
			mTextViewSelect.setVisibility(View.INVISIBLE);
		}

	}
	MenuSetUpListView tempListView=null;
	AnimatorSet set=null;
	float width;
	
	private void turnLeftAnim(){
		
//		mMenuSetUpListView.setPivotX(0);
//		mMenuSetUpListView.setPivotY(0);
		mMenuSetUpListView.setRotationY(90f);
		mMenuSetUpListView.setAlpha(0f);
		int dely=600;
//		ObjectAnimator rAlpAnim=ObjectAnimator.ofFloat(tempListView, "alpha", 1.0f,0.0f);
//		rAlpAnim.setDuration(dely);
		ObjectAnimator rRotationYAnim=ObjectAnimator.ofFloat(tempListView, "rotationY", 0.0f,-90.0f);
		rRotationYAnim.setDuration(dely);
		
		ObjectAnimator rscaleYAnim=ObjectAnimator.ofFloat(tempListView, "scaleX", 1f,0.0f);
		rscaleYAnim.setDuration(dely);
		ObjectAnimator rmoveAnim=ObjectAnimator.ofFloat(tempListView, "translationX", 0f,-(width*3/4));
		rmoveAnim.setDuration(dely);
		
		mMenuSetUpListView.setAlpha(1);
//		ObjectAnimator aAlpAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "alpha", 0.0f,1.0f);
//		aAlpAnim.setDuration(dely);
		ObjectAnimator aRotationYAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "rotationY", 90.0f,0.0f);
		aRotationYAnim.setDuration(dely);
		ObjectAnimator ascaleYAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "scaleX", 0.1f,1f);
		ascaleYAnim.setDuration(dely);
		ObjectAnimator amoveAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "translationX", (float)width,0);
		amoveAnim.setDuration(dely);
		
		set =new AnimatorSet();
//		set.setStartDelay(0);
		set.setInterpolator(new AccelerateInterpolator());
		set.playTogether(/*rAlpAnim,*/rmoveAnim,rscaleYAnim,rRotationYAnim,/*aAlpAnim,*/amoveAnim,ascaleYAnim,aRotationYAnim);
		set.addListener(new AnimatorListenerAdapter(){
			@Override
			public void onAnimationEnd(Animator anim) {
				tempListView.setVisibility(View.GONE);
			}
			
		});
		
		set.start();
		
	}
	
	
	private void turnRightAnim(){
//		mMenuSetUpListView.setPivotX(0);
//		mMenuSetUpListView.setPivotY(0);
		mMenuSetUpListView.setRotationY(-90f);
		mMenuSetUpListView.setAlpha(0f);
		int dely=600;
//		ObjectAnimator rAlpAnim=ObjectAnimator.ofFloat(tempListView, "alpha", 1.0f,0.0f);
//		rAlpAnim.setDuration(dely);
		ObjectAnimator rRotationYAnim=ObjectAnimator.ofFloat(tempListView, "rotationY", 0.0f,90.0f);
		rRotationYAnim.setDuration(dely);
		ObjectAnimator rscaleYAnim=ObjectAnimator.ofFloat(tempListView, "scaleX", 1f,0.0f);
		rscaleYAnim.setDuration(dely);
		ObjectAnimator rmoveAnim=ObjectAnimator.ofFloat(tempListView, "translationX", 0f,width);
		rmoveAnim.setDuration(dely);
		
		mMenuSetUpListView.setAlpha(1);
//		ObjectAnimator aAlpAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "alpha", 0.0f,1.0f);
//		aAlpAnim.setDuration(dely);
		ObjectAnimator aRotationYAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "rotationY", -90.0f,0.0f);
		aRotationYAnim.setDuration(dely);
		ObjectAnimator ascaleYAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "scaleX", 0.1f,1f);
		ascaleYAnim.setDuration(dely);
		ObjectAnimator amoveAnim=ObjectAnimator.ofFloat(mMenuSetUpListView, "translationX", -(width*3/4),0);
		amoveAnim.setDuration(dely);
		
		set =new AnimatorSet();
		set.setInterpolator(new AccelerateInterpolator());
//		set.setInterpolator(new DecelerateInterpolator(1.0f));
		set.playTogether(/*rAlpAnim,*/rmoveAnim,rscaleYAnim,rRotationYAnim,/*aAlpAnim,*/amoveAnim,ascaleYAnim,aRotationYAnim);
		set.addListener(new AnimatorListenerAdapter(){
			@Override
			public void onAnimationEnd(Animator anim) {
				tempListView.setVisibility(View.GONE);
			}
			
		});
//		set.setStartDelay(0);
		set.start();
		
	}
	public void onFocusChange(View v, boolean hasFocus) {
		mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
		mHandler.sendEmptyMessageDelayed(MessageType.MENU_AUTO_EXIT_MESSAGE,
				MessageType.delayMillis2);
		
		Log.d("plz","turn is :"+isTurn);
		if(hasFocus&&AnimationManager.getInstance().getIsAnimation()&&(isTurn == 0 || isTurn == 1)){
			mLastAdapter.setmGroup(mListViewAdpter.getmGroup());
			
			Log.d("plz", "button audio:******************");
			tempListView.setAdapter(mLastAdapter);
			tempListView.setVisibility(View.VISIBLE);
			tempListView.setAlpha(1);
			tempListView.setScaleX(1);
			tempListView.setScaleY(1);
			if(width==0){
				width=mMenuSetUpListView.getWidth();
			}
			if(isTurn==0){
				turnLeftAnim();
			}else if(isTurn==1){
				turnRightAnim();
			}
		
		}
		
		switch (v.getId()) {
		case R.id.widget_buttonVideo:
			if (hasFocus) {
				if (mCINav == true) {
					return;
				}

				isTab = true;
				if (mCurrentFouceButtonID == 1 && isMouse) {
					// mExitLevel = 0;
					// selIdx = -1;
					seekMouthFocus();
					return;
				}
				mTabMenuButtonBar.mButtonShow.setText(R.string.menu_tab_video);
				mTextViewEnter.setVisibility(View.VISIBLE);
				mTextViewEnter.setText(R.string.menu_enter);
				mMenuImageView
						.setImageResource(R.drawable.menu_main_icon_video);
				mDataContainer = MenuDataContainer.getInstance(this);
				mDataContainer.loadDataVideo();

				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.setBackgroundResource(
								R.drawable.menu_tab_button_bg_normal);
				mTabMenuButtonBar.mButtonVideo
						.setBackgroundResource(R.drawable.menu_tab_button_select_bg);
				setAdapter(mDataContainer.getVideoDataGroup());
				mCurrentFouceButtonID = 1;
				mExitLevel = 0;
				selIdx = -1;
			} else {

			}
			break;
		case R.id.widget_buttonAudio:
			if (hasFocus) {
				isTab = true;
				if (mCurrentFouceButtonID == 2 && isMouse) {
					// mExitLevel = 0;
					// selIdx = -1;
					seekMouthFocus();
					return;
				}
				mTabMenuButtonBar.mButtonShow.setText(R.string.menu_tab_audio);
				mTextViewEnter.setVisibility(View.VISIBLE);
				mTextViewEnter.setText(R.string.menu_enter);
				mMenuImageView
						.setImageResource(R.drawable.menu_main_icon_audio);
				mDataContainer = MenuDataContainer.getInstance(this);
				mDataContainer.loadDataAudio();
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.setBackgroundResource(
								R.drawable.menu_tab_button_bg_normal);
				mTabMenuButtonBar.mButtonAudio
						.setBackgroundResource(R.drawable.menu_tab_button_select_bg);
				setAdapter(mDataContainer.getAudioDataGroup());
				mCurrentFouceButtonID = 2;
				mExitLevel = 0;
				selIdx = -1;
			} else {
			}
			break;
		case R.id.widget_buttonTV:
			if (hasFocus) {
				isTab = true;
				if (mCurrentFouceButtonID == 3 && isMouse) {
					// mExitLevel = 0;
					// selIdx = -1;
					seekMouthFocus();
					return;
				}
				mTabMenuButtonBar.mButtonShow.setText(R.string.menu_tab_tv);
				mMenuImageView.setImageResource(R.drawable.menu_main_icon_tv);
				mTextViewEnter.setVisibility(View.VISIBLE);
				mTextViewEnter.setText(R.string.menu_enter);
				mDataContainer = MenuDataContainer.getInstance(this);
				mDataContainer.loadDataTV();
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.setBackgroundResource(
								R.drawable.menu_tab_button_bg_normal);
				mTabMenuButtonBar.mButtonTV
						.setBackgroundResource(R.drawable.menu_tab_button_select_bg);
				setAdapter(mDataContainer.getTVDataGroup());
				mCurrentFouceButtonID = 3;
				mExitLevel = 0;
				selIdx = -1;
			} else {
			}
			break;
		case R.id.widget_buttonSetUp:
			if (hasFocus) {
				isTab = true;
				if (mCurrentFouceButtonID == 4 && isMouse) {
					// mExitLevel = 0;
					// selIdx = -1;
					seekMouthFocus();
					return;
				}
				mTabMenuButtonBar.mButtonShow.setText(R.string.menu_tab_setup);
				mTextViewEnter.setVisibility(View.VISIBLE);
				mTextViewEnter.setText(R.string.menu_enter);
				mMenuImageView
						.setImageResource(R.drawable.menu_main_icon_setup);
				mDataContainer = MenuDataContainer.getInstance(this);
				mDataContainer.loadDataSetup();
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.setBackgroundResource(
								R.drawable.menu_tab_button_bg_normal);
				mTabMenuButtonBar.mButtonSetup
						.setBackgroundResource(R.drawable.menu_tab_button_select_bg);
				setAdapter(mDataContainer.getSetUpDataGroup());
				mCurrentFouceButtonID = 4;					
					if (!mDataContainer.isHaveScreenMode()) {
						mCurrentSelectedPosition = 7;
					}
					
				if (mCIStatusNotice == true) {
					mMenuSetUpListView.requestFocus();

//					if (mInterruptExit == true) {
//						mInterruptExit = false;
//						mCurrentSelectedPosition = 0;
//					}
					mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
					mExitLevel = 1;
					selIdx = 0;
//					if (mCIStatusNotice == true) {
						mListViewSelectedItemData = (DataItem) mMenuSetUpListView
								.getItemAtPosition(mCurrentSelectedPosition);
						showNextLevelCofigPage();
						if (mMenuSetUpListView.hasFocus()) {
							mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
						} else {
							mListViewAdpter.setSelectPos(-1);
						}
						mCIStatusNotice = false;
//					}
				} else {
					mExitLevel = 0;
					selIdx = -1;
				}
			} else {
			}
			break;
		case R.id.widget_buttonParental:
			if (hasFocus) {
				isTab = true;
				if (mCurrentFouceButtonID == 5 && isMouse) {
					// mExitLevel = 0;
					// selIdx = -1;
					seekMouthFocus();
					return;
				}
				mTabMenuButtonBar.mButtonShow.setText(R.string.menu_tab_parent);
				mTextViewEnter.setVisibility(View.VISIBLE);
				mTextViewEnter.setText(R.string.menu_enter);
				mTextViewSelect.setVisibility(View.VISIBLE);
				mMenuImageView
						.setImageResource(R.drawable.menu_main_icon_parental);
				mDataContainer = MenuDataContainer.getInstance(this);
				mDataContainer.loadParentalData();
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.setBackgroundResource(
								R.drawable.menu_tab_button_bg_normal);
				mTabMenuButtonBar.mButtonParental
						.setBackgroundResource(R.drawable.menu_tab_button_select_bg);
				setAdapter(mDataContainer.getmParentalDataGroup());
				mCurrentFouceButtonID = 5;
				mExitLevel = 0;
				selIdx = -1;
			} else {
			}
			break;
		default:
			break;
		}
	}
	ListAdapter adapter=null;

	private class ParentData {
		private int mSelectedPosition;
		private int mPageIndex;
		private List<DataItem> mParentGroup;
	}

	private void setParentData(ParentData parent) {
		if (parent != null) {
			parent.mParentGroup = mListViewSelectedItemData.getmParentGroup();
			parent.mPageIndex = mMenuSetUpListView.getCurrentPageNum();
			parent.mSelectedPosition = mCurrentSelectedPosition;

		}
	}

	private boolean handleHorizontalFocusWithinListItem(int direction) {
		if (KeyMap.KEYCODE_DPAD_LEFT == direction) {
			MtkLog.d(TAG, "----------option type ---"
					+ mListViewSelectedItemData.getDataType().toString());
			MtkLog.d(TAG, "---------Location in -ListView Item------>"
					+ mCurrentSelectedPosition);
			if (mListViewSelectedItemData.getDataType() == DataType.HAVESUBCHILD) {
				--mExitLevel;
				mTabMenuButtonBar.mButtonShow
						.setText(mTitleMap.get(mExitLevel));
				if (linearScan.getVisibility() == View.VISIBLE) {
					linearScan.setVisibility(View.GONE);
				}
				if (mThirdLevelData.mParentGroup != null
						&& mThirdLevelData.mParentGroup.size() > 0) {
					if (mThirdLevelData.mParentGroup.get(0).getDataType() == DataType.CHANNELLISTVIEW) {
						// Enter editChannelBack () method, processing to return
						// to channellist page of data
						editChannelBack();
					}
				}
				if (mSecondLevelData.mParentGroup != null) {
					if (mSecondLevelData.mParentGroup.get(0).mItemID
							.equals(MenuConfigManager.SETUP_TIME_ZONE)) {
						UpdateTime updateTime = new UpdateTime();
						updateTime.shutDownThread();
						mDataContainer.loadTimeSetUpData();
					}
				}
				if (mExitLevel == 0) {
					mMenuSetUpListView.clearFocus();
					mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
							.requestFocus();
					return true;
				} else {
					reloadParentPageData(mExitLevel);
					if (mCurrentFouceButtonID == 5 && mExitLevel == 1) {
						--mExitLevel;
						if (mMenuSetUpListView.hasFocus()) {
							mMenuSetUpListView.clearFocus();
						}
						mTabMenuButtonBarRootView.getChildAt(
								mCurrentFouceButtonID).requestFocus();
					}
					return true;
				}

			} else if (mListViewSelectedItemData.getDataType() == DataType.EFFECTOPTIONVIEW
					|| mListViewSelectedItemData.mParent != null
					|| mListViewSelectedItemData.getDataType() == DataType.SWICHOPTIONVIEW) {
				// for 3d.change value of one 3d item,all related items change
				// too.
				if (mListViewSelectedItemData.mParent != null
						&& (MenuConfigManager.VIDEO_3D).equals(mListViewSelectedItemData.mParent.mItemID)
						&& mListViewSelectedItemData.getDataType() == DataType.OPTIONVIEW) {
					childView.onKeyLeft();
					mDataContainer.load3DData();
					setAdapter(mListViewSelectedItemData.mParent
							.getmSubChildGroup());
					mMenuSetUpListView.requestFocus();
					mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
					return true;
				} else {
					childView.onKeyLeft();
					DataItem parentDataItem = mListViewSelectedItemData.mParent;
					if (parentDataItem != null
							&& parentDataItem.getmInitValue() != parentDataItem.userDefined) {
						((EffectOptionView) mMenuSetUpListView
								.getChildAt(parentDataItem.getPosition()))
								.showValue(mDataContainer
										.getValueById(parentDataItem
												.getmItemID()));
						return true;
					}
				}
				if (mListViewSelectedItemData.getDataType() == DataType.EFFECTOPTIONVIEW) {
					List<DataItem> mEeffectGroup = mListViewSelectedItemData
							.getmEffectGroup();
					int i = 0;
					int initValues[] = mDataContainer
							.getEffectGroupInitValues(mListViewSelectedItemData);
					for (; i < mEeffectGroup.size(); i++) {
						DataItem effectChildData = mEeffectGroup.get(i);
						RespondedKeyEvent effectChildView = (RespondedKeyEvent) mMenuSetUpListView
								.getChildAt(effectChildData.getPosition());
						effectChildView.showValue(initValues[i]);
					}
					return true;
				}
				if (mListViewSelectedItemData.getDataType() == DataType.SWICHOPTIONVIEW) {
					if (!m3d_enable_page) {
						OptionView o = (OptionView) mMenuSetUpListView
								.getChildAt(3);
						o.getmDataItem().isEnable = false;
					}
					mMenuSetUpListView.setAdapter(mListViewAdpter);
					mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
				}

				return true;
			} else if (mListViewSelectedItemData.getDataType() == DataType.PASSWORDVIEW
					&& mExitLevel == 1) {
				--mExitLevel;
				mMenuSetUpListView.clearFocus();
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.requestFocus();
				return true;
			} else if (mListViewSelectedItemData.getDataType() == DataType.CHANNELPOWERONVIEW) {// channelBlock 
				// to
				// the
				// first
				// position
				ChannelListPoweronView mChannelListPoweronView = (ChannelListPoweronView) mMenuSetUpListView
						.getChildAt(mCurrentSelectedPosition);
				if (mChannelListPoweronView.getmId().equals(
						MenuConfigManager.PARENTAL_CHANNEL_BLOCK_CHANNELLIST)) {
					if (mMenuSetUpListView.getSelectedItemPosition() == 0) {
						setAdapter(mListViewSelectedItemData.mParentGroup, 0);
					} else {
						setAdapter(mListViewSelectedItemData.mParentGroup,
								mMenuSetUpListView.getCurrentPageNum());
					}
					mMenuSetUpListView.setSelection(0);
				} else {
					if (mMenuSetUpListView.getSelectedItemPosition() == 1) {
						setAdapter(mListViewSelectedItemData.mParentGroup, 0);
					} else {
						setAdapter(mListViewSelectedItemData.mParentGroup,
								mMenuSetUpListView.getCurrentPageNum());
					}
					mMenuSetUpListView.setSelection(1);
				}
			} else if (mListViewSelectedItemData.getDataType() == DataType.TVSOURCEVIEW) {// inputBlock
				// to
				// the
				// first
				// position
				TVSourceView tvSourceView = (TVSourceView) mMenuSetUpListView
						.getChildAt(mCurrentSelectedPosition);
				if (tvSourceView.getmId().equals(
						MenuConfigManager.PARENTAL_INPUT_BLOCK_SOURCE)) {
					mMenuSetUpListView.setSelection(0);
				}
			} else if (mListViewSelectedItemData.getmItemID().equals(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL)) {
                MtkLog.d(TAG, "left MENU_TV_RF_SCAN_REFRESH getsignal");
				hasChangeFreq = true;
                childView.onKeyLeft();
                mHandler.removeMessages(MessageType.MENU_TV_RF_SCAN_CONNECTTURN);
                mHandler.sendEmptyMessageDelayed(
                        MessageType.MENU_TV_RF_SCAN_CONNECTTURN,
                        MessageType.delayMillis6);
                return true;
            } else {
				childView.onKeyLeft();
				return true;
			}
		}
		if (KeyMap.KEYCODE_DPAD_RIGHT == direction) {
			if (mListViewSelectedItemData.getDataType() == DataType.HAVESUBCHILD) {
				showNextLevelCofigPage();
				if (mMenuSetUpListView.hasFocus()) {
					mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
				} else {
					mListViewAdpter.setSelectPos(-1);
				}
				return true;
			} else if (mListViewSelectedItemData.getDataType() == DataType.EFFECTOPTIONVIEW
					|| mListViewSelectedItemData.mParent != null
					|| mListViewSelectedItemData.getDataType() == DataType.SWICHOPTIONVIEW) {
				// for 3d.change value of one 3d item,all related items change
				// too.
				if (mListViewSelectedItemData.mParent != null
						&& (MenuConfigManager.VIDEO_3D).equals(mListViewSelectedItemData.mParent.mItemID)
						&& mListViewSelectedItemData.getDataType() == DataType.OPTIONVIEW) {
					childView.onKeyRight();
					mDataContainer.load3DData();
					setAdapter(mListViewSelectedItemData.mParent
							.getmSubChildGroup());
					mMenuSetUpListView.requestFocus();
					mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
					return true;
				} else {
					childView.onKeyRight();
					DataItem parentDataItem = mListViewSelectedItemData.mParent;
					if (parentDataItem != null
							&& parentDataItem.getmInitValue() != parentDataItem.userDefined) {
						((EffectOptionView) mMenuSetUpListView
								.getChildAt(parentDataItem.getPosition()))
								.showValue(mDataContainer
										.getValueById(parentDataItem
												.getmItemID()));
						return true;
					}
				}
				if (mListViewSelectedItemData.getDataType() == DataType.EFFECTOPTIONVIEW) {
					List<DataItem> mEeffectGroup = mListViewSelectedItemData
							.getmEffectGroup();
					int i = 0;
					int initValues[] = mDataContainer
							.getEffectGroupInitValues(mListViewSelectedItemData);
					for (; i < mEeffectGroup.size(); i++) {
						DataItem effectChildData = mEeffectGroup.get(i);
						RespondedKeyEvent effectChildView = (RespondedKeyEvent) mMenuSetUpListView
								.getChildAt(effectChildData.getPosition());
						effectChildView.showValue(initValues[i]);
					}
					return true;
				}
				if (mListViewSelectedItemData.getDataType() == DataType.SWICHOPTIONVIEW) {
					if (!m3d_enable_page) {
						OptionView o = (OptionView) mMenuSetUpListView
								.getChildAt(3);
						o.getmDataItem().isEnable = false;
					}
					mMenuSetUpListView.setAdapter(mListViewAdpter);
					mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
				}
				return true;
			} else if (mListViewSelectedItemData.getDataType() == DataType.CHANNELPOWERONVIEW) {// channelBlock
				// to
				// the
				// last
				// position
				ChannelListPoweronView mChannelListPoweronView = (ChannelListPoweronView) mMenuSetUpListView
						.getChildAt(mCurrentSelectedPosition);
				if (mChannelListPoweronView.getmId().equals(
						MenuConfigManager.PARENTAL_CHANNEL_BLOCK_CHANNELLIST)) {
					if (mMenuSetUpListView.getPageNum() > mMenuSetUpListView
							.getCurrentPageNum()) {
						setAdapter(mListViewSelectedItemData.mParentGroup,
								mMenuSetUpListView.getCurrentPageNum());
						mMenuSetUpListView.setSelection(8);
					} else {
						setAdapter(mListViewSelectedItemData.mParentGroup,
								mMenuSetUpListView.getPageNum());
						int t = mListViewSelectedItemData.mParentGroup.size()
								% mPageSize - 1;
						t = t < 0 ? 0 : t;
						mMenuSetUpListView.setSelection(t);
					}

				} else {
					if (mMenuSetUpListView.getPageNum() > mMenuSetUpListView
							.getCurrentPageNum()) {
						if (mCurrentSelectedPosition == 8) {
							setAdapter(mListViewSelectedItemData.mParentGroup,
									mMenuSetUpListView.getCurrentPageNum() + 1);
							if (mMenuSetUpListView.getPageNum() == mMenuSetUpListView
									.getCurrentPageNum()) {
								int t = mListViewSelectedItemData.mParentGroup
										.size()
										% mPageSize - 1;
								t = t < 1 ? 1 : t;
								mMenuSetUpListView.setSelection(t);
							} else {
								mMenuSetUpListView.setSelection(8);
							}
						} else {
							setAdapter(mListViewSelectedItemData.mParentGroup,
									mMenuSetUpListView.getCurrentPageNum());
							mMenuSetUpListView.setSelection(8);
						}
					} else {
						setAdapter(mListViewSelectedItemData.mParentGroup,
								mMenuSetUpListView.getPageNum());
						int t = mListViewSelectedItemData.mParentGroup.size()
								% mPageSize - 1;
						t = t < 1 ? 1 : t;
						mMenuSetUpListView.setSelection(t);
					}
				}
			} else if (mListViewSelectedItemData.getDataType() == DataType.TVSOURCEVIEW) {// inputBlock
				// to
				// the
				// last
				// position
				TVSourceView tvSourceView = (TVSourceView) mMenuSetUpListView
						.getChildAt(mCurrentSelectedPosition);
				if (tvSourceView.getmId().equals(
						MenuConfigManager.PARENTAL_INPUT_BLOCK_SOURCE)) {
					mMenuSetUpListView
							.setSelection(mListViewSelectedItemData.mParentGroup
									.size() - 1);
				}
			}else if (mListViewSelectedItemData.getmItemID().equals(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL)) {
			    MtkLog.d(TAG, "right MENU_TV_RF_SCAN_REFRESH getsignal");
				hasChangeFreq = true;
			    childView.onKeyRight();
			    mHandler.removeMessages(MessageType.MENU_TV_RF_SCAN_CONNECTTURN);
                mHandler.sendEmptyMessageDelayed(
                        MessageType.MENU_TV_RF_SCAN_CONNECTTURN,
                        MessageType.delayMillis6);
                return true;
            } else {
				childView.onKeyRight();
				return true;
			}
		}
		return false;
	}

	// ListView select ItemView do Enter key event
	private boolean handleWithListItemKeyEnterEvent() {
		MtkLog.d(TAG,"handleWithListItemKeyEnterEvent");
		if (mListViewSelectedItemData.getDataType() == DataType.POSITIONVIEW
				|| mListViewSelectedItemData.getDataType() == DataType.PROGRESSBAR) {

			mRootRelativeLayout.setVisibility(View.INVISIBLE);
			mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
			mChangeview.setVisibility(View.VISIBLE);
			mChangeview.requestFocus();
			mChangeview.setHandler(mHandler);

			mChangeview.setAdapter(mListViewSelectedItemData);
			mChangeview.setAdapter(mGroup);
			mChangeview.setVisibility(View.VISIBLE);
			mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
			mHandler.sendEmptyMessageDelayed(MessageType.WAIT_DELAY_MESSAGE,
					60000);
			isProgress = true;
			mChangeviewWindowId = mBypassWindowManager.getAvailableWindowId();
			mChangeview.getViewTreeObserver().addOnPreDrawListener(
					new OnPreDrawListener() {
						public boolean onPreDraw() {
							if (isChangeviewFirst) {
								mChangeviewRect = new Rect(mChangeview
										.getLeft(), mChangeview.getTop(),
										mChangeview.getRight(), mChangeview
												.getBottom());
								isChangeviewFirst = false;
							}
							if (!isChangeviewVisible) {
								mBypassWindowManager.setBypassWindow(true,
										mChangeviewWindowId, mChangeviewRect);
								isChangeviewVisible = true;
							}
							return true;
						}
					});
			return true;
		}
		// search channel
		if (mListViewSelectedItemData.getDataType() == DataType.SCANCHANNELSOPTIONVIEW
				&& (mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.COLOR_SYSTEM) || mListViewSelectedItemData
						.getmItemID().equals(MenuConfigManager.TV_SYSTEM))) {
			mScanDialog = new TVScan(this, mListViewSelectedItemData
					.getmItemID(), mFrameLayout);
			mScanDialog.count = 1;
			mScanDialog.getMenuLeftLayout(mLeftLayout);
			mScanDialog.setHandler(mHandler);
			linearList.setVisibility(View.INVISIBLE);
			mTextViewEnter.setVisibility(View.INVISIBLE);
			mTextViewSelect.setVisibility(View.INVISIBLE);
			mTextViewExit.setText(getString(R.string.menu_cancel));
			mScanDialog.show();
			mScanDialog.loading.drawLoading();
			// Channel Scan,Update Scan
			mScanningStatus = true;
			mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
			mScanDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
				public boolean onKey(DialogInterface dialog, int keyCode,
						KeyEvent event) {
					int action = event.getAction();

					if (keyCode == KeyMap.KEYCODE_BACK
							&& action == KeyEvent.ACTION_DOWN) {
						mScanDialog.onScanning = false;
						if (mScanDialog.count == 0) {
							linearList.setVisibility(View.VISIBLE);
							mTextViewEnter
									.setText(getString(R.string.menu_enter));
							mTextViewSelect
									.setText(getString(R.string.menu_select));
						//	mTextViewExit
								//	.setText(getString(R.string.menu_exit));
							mScanDialog.loading.stopDraw();
							mScanDialog.loading.setText("");
							mScanDialog.dismiss();
							mHandler.sendEmptyMessageDelayed(
									MessageType.MENU_AUTO_EXIT_MESSAGE,
									MessageType.delayMillis2);
							mScanningStatus = false;
							if (null != mDataContainer.getmFactoryDataGroup()) {
								mDataContainer.getmFactoryDataGroup().clear();
							}
							// mMenuSetUpListView.requestFocus();
							isScanning = false;
							mMenuSetUpListView.requestFocus();
							if (mMenuSetUpListView.hasFocus()) {
								mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
							} else {
								mListViewAdpter.setSelectPos(-1);
							}
//							mMenuSetUpListView.postDelayed(new Runnable() {
//
//								public void run() {
//									// TODO Auto-generated method stub
//									View view = mMenuSetUpListView
//											.getChildAt(selIdx);
//									if (view != null) {
//										view.requestFocusFromTouch();
//										mMenuSetUpListView.setSelection(selIdx);
//									}
//								}
//							}, 200);

							return true;
						} else {
							mTextViewEnter.setVisibility(View.INVISIBLE);
							mTextViewSelect.setVisibility(View.VISIBLE);
							mTextViewSelect
									.setText(getString(R.string.menu_scan));
							mTextViewExit
									.setText(getString(R.string.menu_back));
							if (mScanDialog.showScanStateInfo()) {
								linearList.setVisibility(View.VISIBLE);
								mMenuSetUpListView.requestFocus();
								if (mMenuSetUpListView.hasFocus()) {
									mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
								} else {
									mListViewAdpter.setSelectPos(-1);
								}
							}

							mHandler.sendEmptyMessageDelayed(
									MessageType.MENU_AUTO_EXIT_MESSAGE,
									MessageType.delayMillis2);
							/* mScanningStatus = false; */
							if (null != mDataContainer.getmFactoryDataGroup()) {
								mDataContainer.getmFactoryDataGroup().clear();
							}
							return true;
						}
					}
					if ((keyCode == KeyMap.KEYCODE_DPAD_CENTER)
							&& action == KeyEvent.ACTION_DOWN) {
						if(mScanDialog.onScanning){
							return false;
						}else{
							mScanDialog.onScanning = true;
						}
						mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
						if (mScanDialog.mStateTextView
								.getText()
								.toString()
								.equals(
										getString(R.string.menu_setup_channel_scan_done))) {
							mScanDialog.continueScan();
							return false;
						}
						if (mScanDialog.count == 1) {
							return false;
						}
						++mScanDialog.count;
						mScanDialog.mStateTextView.setText(MenuMain.this
								.getString(R.string.menu_tv_scann_allchannels));
						mScanDialog.mNumberChannel
								.setVisibility(View.INVISIBLE);
						mScanDialog.continueScan();
						mHandler
								.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
						if (null != mDataContainer.getmFactoryDataGroup()) {
							mDataContainer.getmFactoryDataGroup().clear();
						}
						return true;
					}
					if (keyCode == KeyMap.KEYCODE_MENU) {
						mScanDialog.cancelScan();
						mScanDialog.loading.stopDraw();
						mScanDialog.dismiss();
						exit();
						return true;
					}
					if (null != mDataContainer.getmFactoryDataGroup()) {
						mDataContainer.getmFactoryDataGroup().clear();
					}
					return false;
				}
			});
		}

		if (mListViewSelectedItemData.getDataType() == DataType.HAVESUBCHILD) {
			showNextLevelCofigPage();
			if (mMenuSetUpListView.hasFocus()) {
				mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
			} else {
				mListViewAdpter.setSelectPos(-1);
			}
			return true;
		}
		/**
		 * ---channelNameEdit:---press enter to back
		 */
    if(MenuConfigManager.TV_SINGLE_SCAN_RF_CHANNEL.equals(mListViewSelectedItemData.getmItemID())){
        MtkLog.d(TAG, "when enter scan remove MENU_TV_RF_SCAN_REFRESH getsignal");
		mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
		mHandler.removeMessages(MessageType.MENU_TV_RF_SCAN_REFRESH);
		mHandler.removeMessages(MessageType.MENU_TV_RF_SCAN_CONNECTTURN);
		mTextViewEnter.setVisibility(View.INVISIBLE);
		mTextViewSelect.setVisibility(View.INVISIBLE);
		mTextViewExit.setText(getString(R.string.menu_cancel));
		// Analog Manual Scan
		mScanningStatus = true;
		isScanning = true;
		mMenuSetUpListView.clearFocus();
		linearScan.requestFocus();
		progressHandler = new Handler();
		angProgressView.loading.drawLoading();
		angProgressView.setHandler(mHandler);
		angProgressView.singleRFChannelScan();
		return true;
     }
		if (mListViewSelectedItemData.getmItemID().equals(
				MenuConfigManager.TV_CHANNEL_NO)
				|| mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.TV_FREQ)) {

			--mExitLevel;
			mTabMenuButtonBar.mButtonShow.setText(mTitleMap.get(mExitLevel));
			editChannelBack();
//			editChannelView = 1;
//			channelEditView();
			reloadParentPageData(mExitLevel);
			return true;
		}
		if (mListViewSelectedItemData.getDataType() == DataType.TEXTCOMMVIEW) {
			showNextLevelCofigPage();
			if (mMenuSetUpListView.hasFocus()) {
							mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
						} else {
							mListViewAdpter.setSelectPos(-1);
						}
			return true;
		}
		if (mListViewSelectedItemData.getDataType() == DataType.CHANNELLISTVIEW) {
			mTitleMap.put(mExitLevel, mTabMenuButtonBar.mButtonShow.getText()
					.toString());
			editChannelView = 1;// This page variables is 1
			channelEditView();// Judge whether the current page editView:
			// marking value is always 1
			if (keyColor.length()==0) {
				int colorSystem = mListViewSelectedItemData.mSubChildGroup
						.get(EditChannel.COLOR).mInitValue;
				int soundSystem = mListViewSelectedItemData.mSubChildGroup
						.get(EditChannel.SOUND).mInitValue;
				mEditChannel.setOriginalTVSystem(colorSystem, soundSystem);
				TVChannel currentChannel = mChSel.getCurrentChannel();
				if(null != currentChannel){
					mEditChannel.setOriginalFrequency(((float)currentChannel
						.getFreq())/1000000);
				}
				showNextLevelCofigPage();
				if (mMenuSetUpListView.hasFocus()) {
							mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
						} else {
							mListViewAdpter.setSelectPos(-1);
						}
				editChannelView = 2;// This page variables is 1
				channelEditView();
				return true;
			}
			if (keyColor.equals("red")) {
				switchChannel_to_page = mMenuSetUpListView.getCurrentPageNum();
				switchChannel_to = mCurrentSelectedPosition
						+ (switchChannel_to_page - 1) * mPageSize;
				Collections.swap(mDataContainer.getChannelDataGroup(),
						switchChannel_from, switchChannel_to);
				mEditChannel.swapChannel(switchChannel_from, switchChannel_to);
				String[] strtemp = mListViewSelectedItemData.getmOptionValue();
				switchChannel_to_num = strtemp[0];

				String[] modifyStr_to = mDataContainer.getChannelDataGroup()
						.get(switchChannel_to).getmOptionValue();
				String[] modifyStr_from = mDataContainer.getChannelDataGroup()
						.get(switchChannel_from).getmOptionValue();
				modifyStr_to[0] = switchChannel_to_num;
				modifyStr_from[0] = switchChannel_from_num;
				mDataContainer.getChannelDataGroup().get(switchChannel_to)
						.setmOptionValue(modifyStr_to);
				mDataContainer.getChannelDataGroup().get(switchChannel_from)
						.setmOptionValue(modifyStr_from);
				keyColor = "";

				setAdapter(mDataContainer.getChannelDataGroup(),
						mMenuSetUpListView.getCurrentPageNum());
				mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
			}
			if (keyColor.equals("green")) {
				insertChannel_to_page = mMenuSetUpListView.getCurrentPageNum();
				insertChannel_to = mCurrentSelectedPosition
						+ (insertChannel_to_page - 1) * mPageSize;
				mEditChannel
						.insertChannel(insertChannel_from, insertChannel_to);
				if (insertChannel_to > insertChannel_from) {
					mDataContainer.getChannelDataGroup().add(
							insertChannel_to + 1,
							mDataContainer.getChannelDataGroup().get(
									insertChannel_from));
					mDataContainer.getChannelDataGroup().remove(
							insertChannel_from);
				} else {
					mDataContainer.getChannelDataGroup().add(
							insertChannel_to,
							mDataContainer.getChannelDataGroup().get(
									insertChannel_from));
					mDataContainer.getChannelDataGroup().remove(
							insertChannel_from + 1);
				}

				for (int i = 0; i < mDataContainer.getChannelDataGroup().size(); i++) {
					String[] optionStr = mDataContainer.getChannelDataGroup()
							.get(i).getmOptionValue();
					optionStr[0] = oldChannelNum[i];
					mDataContainer.getChannelDataGroup().get(i)
							.setmOptionValue(optionStr);
				}
				keyColor = "";

				setAdapter(mDataContainer.getChannelDataGroup(),
						mMenuSetUpListView.getCurrentPageNum());
				mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
			}
		}
		if (mListViewSelectedItemData.getDataType() == DataType.CHANNELPOWERONVIEW) {
			if (childView != null) {
				ChannelListPoweronView channelListPoweronView = (ChannelListPoweronView) childView;
				gotoPage = mMenuSetUpListView.getCurrentPageNum();
				gotoPosition = mCurrentSelectedPosition + (gotoPage - 1)
						* mPageSize;
				if (mListViewSelectedItemData.getmItemID().equals(
						MenuConfigManager.SETUP_POWER_ONCHANNEL_LIST)) {
					if (mChannelPownBrePositon != gotoPosition) {
						mDataContainer.getChannelPowerOnDataGroup().get(
								mChannelPownBrePositon)
								.setmBrightBackGroundFlag(false);
					}
					if (mDataContainer.getChannelPowerOnDataGroup().get(
							gotoPosition).ismBrightBackGroundFlag() == true) {
						mDataContainer.getChannelPowerOnDataGroup().get(
								gotoPosition).setmBrightBackGroundFlag(false);
					} else {
						mDataContainer.getChannelPowerOnDataGroup().get(
								gotoPosition).setmBrightBackGroundFlag(true);
					}
					setAdapter(mDataContainer.getChannelPowerOnDataGroup(),
							gotoPage);
					mMenuSetUpListView.setSelection(mCurrentSelectedPosition);

					mDataContainer.getChannelPowerOnDataGroup().get(
						saveValue.readValue("Power_On_Channel"))
						.setmBrightBackGroundFlag(false);
					saveValue.saveValue("Power_On_Channel", 0);
					if (mDataContainer.getChannelPowerOnDataGroup().get(
						gotoPosition).ismBrightBackGroundFlag() == true) {
							saveValue.saveValue("Power_On_Channel", gotoPosition);
					}
				}
				channelListPoweronView.onKeyEnter();// enter view
				mChannelPownBrePositon = gotoPosition;
			}
		}
		if (mListViewSelectedItemData.getDataType() == DataType.TVSOURCEVIEW) {
			if (childView != null) {
				TVSourceView selectChildView = (TVSourceView) childView;
				selectChildView.onKeyEnter(mTabMenuButtonBar);
				if (null != mDataContainer.getmFactoryDataGroup()) {
					mDataContainer.getmFactoryDataGroup().clear();
				}
				return true;
			}
		}
		if (mListViewSelectedItemData.getDataType() == DataType.NUMVIEW) {
			if (childView != null) {
				if ((MenuConfigManager.TV_CHANNEL_END_FREQUENCY).equals(mListViewSelectedItemData.mItemID)
						||(MenuConfigManager.TV_CHANNEL_START_FREQUENCY).equals(mListViewSelectedItemData.mItemID)) {
					mMenuSetUpListView
							.setSelection(mCurrentSelectedPosition + 1);
					mCurrentSelectedPosition += 1;
					if ((MenuConfigManager.TV_CHANNEL_START_FREQUENCY).equals(mListViewSelectedItemData.mItemID)) {
						text1 = (TextView) mMenuSetUpListView.getChildAt(0)
								.findViewById(R.id.common_tv_itemshow);
						mNumView = (NumView) childView;
						mNumView.onKeyEnter();
					}
					if ((MenuConfigManager.TV_CHANNEL_END_FREQUENCY).equals(mListViewSelectedItemData.mItemID)) {
						mNumView2 = (NumView) childView;
						mNumView2.onKeyEnter();
						text2 = (TextView) mMenuSetUpListView.getChildAt(1)
								.findViewById(R.id.common_tv_itemshow);
						String str1 = text1.getText().toString();
						String str2 = text2.getText().toString();
						if (Integer.parseInt(str1.substring(0, str1
								.indexOf("."))) > Integer.parseInt(str2
								.substring(0, str2.indexOf(".")))) {
							text1.setText(str2);
							text2.setText(str1);
							mGroup.get(0).mInitValue = Integer.parseInt(str2
									.substring(0, str2.indexOf(".")));
							mNumView.changeValueLister(mNumView
									.getValuedListener(), mGroup.get(0));
							mGroup.get(1).mInitValue = Integer.parseInt(str1
									.substring(0, str1.indexOf(".")));
							mNumView2.changeValueLister(mNumView2
									.getValuedListener(), mGroup.get(1));
						}
					}

				}
			}
		}
		return false;
	}

	// Deal with key figures input
	private boolean handleKeyboardInput(int keyCode) {
		mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
		mHandler.sendEmptyMessageDelayed(MessageType.MENU_AUTO_EXIT_MESSAGE,
				MessageType.delayMillis2);
		char _ch = 0;
		String _mStr = "0";
		switch (keyCode) {
		case KeyMap.KEYCODE_0:
			_ch = '0';
			_mStr = "0";
			break;
		case KeyMap.KEYCODE_1:
			_ch = '1';
			_mStr = "1";
			break;
		case KeyMap.KEYCODE_2:
			_ch = '2';
			_mStr = "2";
			break;
		case KeyMap.KEYCODE_3:
			_ch = '3';
			_mStr = "3";
			break;
		case KeyMap.KEYCODE_4:
			_ch = '4';
			_mStr = "4";
			break;
		case KeyMap.KEYCODE_5:
			_ch = '5';
			_mStr = "5";
			break;
		case KeyMap.KEYCODE_6:
			_ch = '6';
			_mStr = "6";
			break;
		case KeyMap.KEYCODE_7:
			_ch = '7';
			_mStr = "7";
			break;
		case KeyMap.KEYCODE_8:
			_ch = '8';
			_mStr = "8";
			break;
		case KeyMap.KEYCODE_9:
			_ch = '9';
			_mStr = "9";
			break;
		default:
			break;
		}
	
		if (mListViewSelectedItemData != null && mMenuSetUpListView.hasFocus()) {
			if (mListViewSelectedItemData.getDataType() == DataType.DATETIMEVIEW
					&& childView != null) {
				DateTimeInputView dateTimeInputView = (DateTimeInputView) childView;
				DateTimeView _dateTimeView = dateTimeInputView
						.getmDateTimeView();
				if (_dateTimeView != null) {
					_dateTimeView.input(_ch, dateTimeInputView.getmDataItem());
					return true;
				}
			} else if (mListViewSelectedItemData.getDataType() == DataType.TEXTCOMMVIEW
					&& childView != null) {
				if (mListViewSelectedItemData.getmName() != null
						&& getString(R.string.menu_tv_channel_no).equals(
								mListViewSelectedItemData.getmName())) {
					TextCommView textComm = (TextCommView) childView;
					textComm.inputNum(_mStr);
					return true;
				} else if (mListViewSelectedItemData.getmName() != null
						&& getString(R.string.menu_tv_freq).equals(
								mListViewSelectedItemData.getmName().trim())) {
					TextCommView textComm = (TextCommView) childView;
					textComm.inputFreq(_mStr);
					return true;
				}
			} else if (mListViewSelectedItemData.getDataType() == DataType.PASSWORDVIEW
					&& childView != null) {
				PasswordView passwordView = (PasswordView) childView;
				Boolean boolean1 = passwordView.dealPassword(mExitLevel, _mStr,
						mMenuSetUpListView);
				if (!boolean1 && mExitLevel == 1) {
					mHandler.postDelayed(new Runnable() {

						public void run() {
							// TODO Auto-generated method stub
							showNextLevelCofigPage();
							if (mMenuSetUpListView.hasFocus()) {
								mListViewAdpter.setSelectPos(mMenuSetUpListView.getSelectedItemPosition());
							} else {
								mListViewAdpter.setSelectPos(-1);
							}
						}
					}, 500);
					// showNextLevelCofigPage();
					return true;
				} else if (!boolean1 && mExitLevel == 3) {
					--mExitLevel;
					mHandler.postDelayed(new Runnable() {

						public void run() {
							// TODO Auto-generated method stub
							reloadParentPageData(mExitLevel);
						}
					}, 500);
				}
			} else if (mListViewSelectedItemData.getDataType() == DataType.NUMVIEW
					&& childView != null) {
				NumView mNumView = (NumView) childView;
				mNumView.input(_mStr);
				return true;
			}
		}
		/**
		 * This section is the entrance into the menu is.
		 */
		if (mCurrentFouceButtonID == 1 && !mMenuSetUpListView.hasFocus()) {
			zeroTimes++;
			if (zeroTimes == 4) {
				loadFactoryData();
				zeroTimes = 0;
			}
		} else {
			zeroTimes = 0;
		}

		return false;
	}

	public static void setFreFrom(int frequency) {
		freFrom = frequency;
	}

	public static void setFreTo(int frequency) {
		freTo = frequency;
	}

	private int getFreFrom() {
		return freFrom;
	}

	private int getFreTo() {
		return freTo;
	}

    public static int getRfchanel() {
        return rfchanel;
    }

    public static void setRfchanel(int rfchanel) {
        MenuMain.rfchanel = rfchanel;
    }

    public static int getSysrate() {
        return sysrate;
    }

    public static void setSysrate(int sysrate) {
        MenuMain.sysrate = sysrate;
    }
	/**
	 * Automatic adjustment, please wait
	 */
	private void autoAdjustInfo(String mShowMessage) {
		autoAdjustDialog = new TurnkeyCommDialog(MenuMain.this, 5);
		autoAdjustDialog.setMessage(mShowMessage);
		autoAdjustDialog.show();
		mRootRelativeLayout.setVisibility(View.GONE);
		mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
				mRootRelativeLayoutRect);
		autoAdjustDialogWindowId = mBypassWindowManager.getAvailableWindowId();
		autoAdjustDialogRect = new Rect(
				(ScreenConstant.SCREEN_WIDTH - autoAdjustDialog.width) / 2,
				(ScreenConstant.SCREEN_HEIGHT - autoAdjustDialog.height) / 2,
				(ScreenConstant.SCREEN_WIDTH + autoAdjustDialog.width) / 2,
				(ScreenConstant.SCREEN_HEIGHT + autoAdjustDialog.height) / 2);
		mBypassWindowManager.setBypassWindow(true, autoAdjustDialogWindowId,
				autoAdjustDialogRect);
		autoAdjustDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {
			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				int action = event.getAction();
				// Does not respond to any key
//				if (action == KeyEvent.ACTION_DOWN) {
//					// autoAdjustDialog.dismiss();
//					// MenuMain.this.finish();
//				}
				return true;
			}
		});
	}


	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// TODO Auto-generated method stub
		MtkLog.v(TAG, "******************************");
		if (isScanning) {
			return true;
		}

		if (selIdx == -2) {
			seekMouthFocus();
		} else {
			seekMouthFocus("");
		}
		if (isProgress && mChangeview != null) {
			mChangeview.requestFocus();
			mChangeview.requestFocusFromTouch();
		}
		return super.onTouchEvent(event);
	}

	public void seekMouthFocus(String mID) {
		int i = getSelectedIndexByID(mID);
		mMenuSetUpListView.requestFocus();
		View view;
		if (i != -1) {
			if (mExitLevel == 0) {
				mExitLevel = 1;
			}
			view = mMenuSetUpListView.getChildAt(i);
			if (view != null) {
				view.requestFocusFromTouch();
				mMenuSetUpListView.setSelection(i);
			}
			selIdx = i;
		} else {
			if (selIdx >= 0) {
				view = mMenuSetUpListView.getChildAt(selIdx);
				if (view != null) {
					view.requestFocusFromTouch();
					mMenuSetUpListView.setSelection(selIdx);
				}
			} else {
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.requestFocus();
			}
		}
	}

	public void seekMouthFocus(int num, int option) {
		mMenuSetUpListView.requestFocus();
		View view;
		if (num != -1) {
			if (mExitLevel == 0) {
				mExitLevel = 1;
			}
			view = mMenuSetUpListView.getChildAt(num);
			if (view != null) {
				view.requestFocusFromTouch();
				mMenuSetUpListView.setSelection(num);
			}
			switch (option) {
			case MenuConfigManager.FOCUS_OPTION_CHANGE_CHANNEL:
				mEditChannel.selectChannel((short) num);
				break;
			}

			selIdx = num;
		} else {
			if (selIdx >= 0) {
				view = mMenuSetUpListView.getChildAt(selIdx);
				if (view != null) {
					view.requestFocusFromTouch();
					mMenuSetUpListView.setSelection(selIdx);
				}
			} else {
				mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
						.requestFocus();
			}
		}
	}

	public void seekMouthFocus() {
		View view;
		if (isScanning) {
			return;
		}

		if (selIdx == -2) {
			int len = mGroup.size();
			for (int i = len - 1; i >= 0; i--) {
				if (mGroup.get(i).isEnable) {
					selIdx = i;
					break;
				}
			}
		}
		if (selIdx >= 0) {
			view = mMenuSetUpListView.getChildAt(selIdx);
			if (view != null) {
				view.requestFocusFromTouch();
				mMenuSetUpListView.setSelection(selIdx);
			}
		} else {
			mTabMenuButtonBarRootView.getChildAt(mCurrentFouceButtonID)
					.requestFocus();
		}
	}

	public int getSelectedIndexByID(String mID) {
		for (int i = 0; i < mGroup.size(); i++) {
			if (mGroup.get(i).getmItemID().equalsIgnoreCase(mID)) {
				return i;
			}
		}
		return -1;
	}

	public void setIsTab(boolean isTab) {
		this.isTab = isTab;
	}

	public boolean getIsTab() {
		return isTab;
	}

	public void setIsMouse(boolean isMouse) {
		this.isMouse = isMouse;
	}

	public boolean getIsMouse() {
		return isMouse;
	}

	public String getPinCode() {
		return pinCode;
	}

	public void setPinCode(String pin) {
		this.pinCode = pin;
	}


	public void setListViewItemSelector(int resId) {
		mMenuSetUpListView.setSelector(resId);
	}

	public void setIsProgress(boolean flag) {
		isProgress = flag;
	}

	OnTouchListener remainOnTouchListener = new OnTouchListener() {

		public boolean onTouch(View v, MotionEvent event) {
			// TODO Auto-generated method stub
			isMouse = true;
			seekMouthFocus();
			return false;
		}
	};

	public class CIStatusBroadcastReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			if ("com.mediatek.ui.intent.CISTATUS_REMOVE".equals(intent
					.getAction())) {
				if (mReadingCIInfo == true) {
					mHandler.sendEmptyMessage(MessageType.CI_PAGE_EXIT_MESSAGE);
				}
			}
		}
	}

	public class SignalChangeReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			// TODO Auto-generated method stub
			if (OUTPUT_SIGNAL_CHANGE.equals(intent.getAction())) {
				DataItem curItem = null;
				if (mListViewAdpter.getItem(0) != null) {
					curItem = (DataItem) mListViewAdpter.getItem(0);
				}
				if (curItem != null) {
					DataItem parent;
					if (MenuConfigManager.FV_AUTOCOLOR.equals(curItem
							.getmItemID())
							&& (mNavIntegration.isCurrentSourceComponent() || mNavIntegration
									.isCurrentSourceVGA())) {
						parent = curItem.getmParent();
						if (parent != null) {
							parent = mDataContainer
									.refreshVideoFactorySub(parent);
							if (parent != null && parent.getmSubChildGroup().size() > 0) {
								setAdapter(parent.getmSubChildGroup());
							}
						}
					} else if (MenuConfigManager.DNR.equals(curItem
							.getmItemID())) {
						mDataContainer.loadAdvancedData();
						int sel = mMenuSetUpListView.getSelectedItemPosition();
						setAdapter(mDataContainer.getAdvancedData()
								.getmSubChildGroup());
						mMenuSetUpListView.setSelection(sel);
					}
				}
			}
		}
	}

	/*
	 * // for two activities protected void onNewIntent(Intent intent) { // TODO
	 * Auto-generated method stub super.onNewIntent(intent); }
	 */

	// edit channel,has two same channel num,toast info
	private void toastEditChSameNum() {
		editChSameNumDialog = new TurnkeyCommDialog(MenuMain.this, 0);
		editChSameNumDialog.setMessage(MenuMain.this
				.getString(R.string.menu_dialog_numrepeat));
		editChSameNumDialog.show();
		mRootRelativeLayout.setVisibility(View.GONE);
		mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
				mRootRelativeLayoutRect);
		editChSameNumDialog.setPositon(-20, 70);
		editChSameNumDialogWindowId = mBypassWindowManager
				.getAvailableWindowId();
		editChSameNumDialogRect = new Rect(
				-20 + (ScreenConstant.SCREEN_WIDTH - editChSameNumDialog.width)
						/ 2,
				70 + (ScreenConstant.SCREEN_HEIGHT - editChSameNumDialog.height) / 2,
				-20 + (ScreenConstant.SCREEN_WIDTH + editChSameNumDialog.width)
						/ 2,
				70 + (ScreenConstant.SCREEN_HEIGHT + editChSameNumDialog.height) / 2);
		mBypassWindowManager.setBypassWindow(true, editChSameNumDialogWindowId,
				editChSameNumDialogRect);
	}

	private void changePowerOnChannel() {
		List<TVChannel> ch_list = mEditChannel.getChannelList();
		int chNum = 0 ;
		if(null != ch_list){
			chNum = ch_list.size();
		}
		int page = 0;
		int num = 0;
		for (int j = 0; j < chNum; j++) {
			if (j % 8 == 0 && j != 0) {
				page += 1;
			}
			if (saveValue.readValue("Power_On_Channel") - page == j + 1)
				break;
			num++;
		}
		if (num == chNum) {
			saveValue.saveValue(MenuConfigManager.POWER_ON_CH_MODE, 0);
		}
	}

	private SelectorListener mInternalSelectorListener = null;
	private void addHasSignalListener(){
			mInternalSelectorListener = new SelectorListener() {
			private boolean hasSignal = mChSel.hasSignal();
			private boolean hasScramble = mChSel.hasScramble();

			public void onSignalChange(boolean hasSignal) {				
				if (this.hasSignal == hasSignal) {					
					return;
				}				
				this.hasSignal = hasSignal;				
//				refreshSetupView(hasSignal);
				}		

			public void onScramble(boolean hasScramble, String state) {
				if (goToChannelEditorTag && hasScramble
						&& "videoAndAudioScrambled".equals(state)
						&& mCAMManager.isSlotActive() == true) {
					if(isPressChUPAndChDN){
						isPressChUPAndChDN = false;
						mHandler.sendEmptyMessage(MessageType.FROM_MENUMAIN_TO_TK);
					}else{
					editChannelViewMenu.setVisibility(View.GONE);
					editChannelViewBottom.setVisibility(View.GONE);
					mMenuSetUpListView.setVisibility(View.GONE);
					goToChannelEditorTag = false;
					mHandler.sendEmptyMessage(MessageType.EDITCHANNEL_NEED_CI);
					}
				}
				if (this.hasScramble == hasScramble) {
					return;
				}
				this.hasScramble = hasScramble;
				if(TurnkeyUiMainActivity.mCIStatus){
					return;
				}else{
				refreshSetupView((!hasScramble));
				}
			}

			public void onChannelSelect(TVChannel ch) {
				// TODO Auto-generated method stub
				 isChangeChannel =true;
			}

			public void onBlock(TVChannel ch) {
				// TODO Auto-generated method stub
				isChangeChannel =true;
			}
		};
		mChSel.addSelectorListener(mInternalSelectorListener);
	}
	private NavIntegration.IInputSourceChangeListener mInternalSourceChangeListener = null;
	private void addSourceChangeListener() {
		
		mInternalSourceChangeListener = new IInputSourceChangeListener() {
			private boolean hasSignal = mNavIntegration.iCurrentInputSourceHasSignal();
			
			public void isBlocked(String input) {
				// TODO Auto-generated method stub
			}
			
			public void isInputGotSignal(String input) {
				// TODO Auto-generated method stub
			}

			public void isOutputSignalChange(String output, boolean hasSignal) {
				// TODO Auto-generated method stub
				MtkLog.i(TAG, "isOutputSignalChange");
				
				Intent intent = new Intent(OUTPUT_SIGNAL_CHANGE);
				MenuMain.this.sendBroadcast(intent);

					if(1 == mCurrentFouceButtonID ){
						if(mNavIntegration.isCurrentSourceVGA()&&!mNavIntegration.iCurrentChannelHasSignal()){
							
							if(2==mExitLevel){
					DataItem item = ((DataItem)mMenuSetUpListView.getSelectedItem());
		
					if(item!=null){
						String type=item.getmItemID();
				
						if(type.equals(MenuConfigManager.AUTO_ADJUST) ||
								type.equals(MenuConfigManager.HPOSITION)||
								type.equals(MenuConfigManager.VPOSITION)||
								type.equals(MenuConfigManager.PHASE)||
								type.equals(MenuConfigManager.CLOCK)
								){
							finish();
									}
								}
							}else if(mExitLevel==1){
								MtkLog.i(TAG, "isOutputSignalChange mExitLevel==1 mCurrentFouceButtonID:"+mCurrentFouceButtonID);
								mCurrentSelectedPosition = mMenuSetUpListView.getSelectedItemPosition();
					
									MtkLog.i(TAG, "isOutputSignalChange mCurrentFouceButtonID == 2 mCurrentSelectedPosition:"+mCurrentSelectedPosition);
									DataItem item = ((DataItem)mMenuSetUpListView.getSelectedItem());
									String type = item.getmItemID();
									int gotoPage = mMenuSetUpListView.getCurrentPageNum();
					
									mDataContainer = MenuDataContainer.getInstance(mContext);
									mDataContainer.loadDataVideo();
									MtkLog.i(TAG, "isOutputSignalChange gotoPage:"+gotoPage+" pageNum:"+mMenuSetUpListView.getPageNum());
									int allPage = mMenuSetUpListView.getPageNum();
									if(gotoPage > allPage){
										gotoPage = allPage;
						}
									setAdapter(mDataContainer.getVideoDataGroup(),gotoPage);
									int position = itemPosition(type);
									if(position!=-1){
										mCurrentSelectedPosition = position;
									}
									mListViewAdpter.notifyDataSetChanged();
									mMenuSetUpListView.setSelection(mCurrentSelectedPosition);
								}
			
								
					}
				}
				if(!hasSignal){
	                if (null!=TimeShiftManager.getInstance()) {
	                   TimeShiftManager.getInstance().stopAllRunning();
	                }
	                
				}
				if (this.hasSignal == hasSignal) {
					return;
				}
				this.hasSignal = hasSignal;
				refreshSetupView(hasSignal);
		  }

		  public void isSelected(String output, String input) {
				// TODO Auto-generated method stub
			}
		};
		mNavIntegration.iAddInputsourceChangeListener(mInternalSourceChangeListener);
	}
	
	private int itemPosition(String type){
		int item = -1;
		DataItem di;
		
		for(int i =0;i<mGroup.size();i++){
			di = mGroup.get(i);
			if(di.getmItemID().equals(type)){
				item =  i;
				break;
			}
		}
		
		return item;
	}

	private void refreshSetupView(boolean hasSignal) {
		if (mCurrentFouceButtonID == 4) {
			mDataContainer.loadDataSetup();
			if (selIdx > 0 && hasSignal) {
				selIdx = selIdx + 1;
			} else if (selIdx > 1 && !hasSignal) {
				selIdx = selIdx - 1;
			}
			if (selIdx >= mPageSize) {
				setAdapter(mDataContainer.getmSetUpDataGroup(), 2);
			} else {
				setAdapter(mDataContainer.getmSetUpDataGroup());
			}
			mMenuSetUpListView.setSelection(selIdx);
		}
		}

	private void updateDTVState(int flag ) {
		updateATVState(flag);
		mHandler.postDelayed(new Runnable() {

			public void run() {
				// TODO Auto-generated method stub
				MtkLog.d(TAG, "updateDTVState run");
				MtkLog.d(TAG, "iGetCurrentSouce:"+mNavIntegration.iGetCurrentSouce());
				MtkLog.d(TAG, "iCurrentInputSourceHasSignal:"+mNavIntegration.iCurrentInputSourceHasSignal());
				if (hasChangeFreq || mNavIntegration.iGetCurrentSouce() == null
						|| !mNavIntegration.iCurrentInputSourceHasSignal()) {
					MtkLog.d(TAG, "changeFreqById");
					mEditChannel.changeFreqById();
				}
				int level = mEditChannel.getSignalLevel();
				mNumberChannel.setText(MenuMain.this
						.getString(R.string.menu_tv_rf_signal_level)
						+ level);
			}
		}, 1000);
	}

	private void updateATVState(int flag) {
		isScanning = false;
		if(flag ==ConfigType.BS_SRC_CABLE){
			--mExitLevel;
		}
		// Start frequency gains focus
		mMenuSetUpListView.requestFocus();
		mMenuSetUpListView.setSelection(0);
		mStateTextView.setText(MenuMain.this
				.getString(R.string.menu_setup_channel_scan_done));
		mTextViewEnter.setVisibility(View.INVISIBLE);
		mTextViewSelect.setVisibility(View.INVISIBLE);
		mTextViewExit.setText(getString(R.string.menu_back));
		angProgressView.loading.stopDraw();
		loading.setVisibility(View.GONE);
		if (null != mDataContainer.getmFactoryDataGroup()) {
			mDataContainer.getmFactoryDataGroup().clear();
		}
	}

	public void showDTVScan() {
		TVScan.mDataItem = mListViewSelectedItemData;
		mScanDialog = new TVScan(this, mListViewSelectedItemData.getmItemID(),
				mFrameLayout);
		mScanDialog.getMenuLeftLayout(mLeftLayout);
		mScanDialog.setHandler(mHandler);
		linearList.setVisibility(View.INVISIBLE);
		mTextViewEnter.setVisibility(View.INVISIBLE);
		mTextViewSelect.setVisibility(View.INVISIBLE);
		mTextViewExit.setText(getString(R.string.menu_cancel));
		mScanDialog.show();
		mScanDialog.loading.drawLoading();
		mScanningStatus = true;
		mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
		mScanDialog.setOnKeyListener(new DialogInterface.OnKeyListener() {

			public boolean onKey(DialogInterface dialog, int keyCode,
					KeyEvent event) {
				int action = event.getAction();
				if (keyCode == KeyMap.KEYCODE_BACK
						&& action == KeyEvent.ACTION_DOWN) {
					MtkLog.d(TAG, "*****Scan Count =  *******"
							+ mScanDialog.count);
					if (mScanDialog.count == 0) {
						--mExitLevel;
						linearList.setVisibility(View.VISIBLE);
						mTextViewEnter.setText(getString(R.string.menu_enter));
						mTextViewSelect
								.setText(getString(R.string.menu_select));
						mTextViewExit.setText(getString(R.string.menu_exit));
						mScanDialog.loading.stopDraw();
						mScanDialog.loading.setText("");
						mScanDialog.dismiss();
						mMenuSetUpListView.requestFocus();
						mHandler.sendEmptyMessageDelayed(
								MessageType.MENU_AUTO_EXIT_MESSAGE,
								MessageType.delayMillis1);
						mScanningStatus = false;
						if (null != mDataContainer.getmFactoryDataGroup()) {
							mDataContainer.getmFactoryDataGroup().clear();
						}

						isScanning = false;
						mMenuSetUpListView.postDelayed(new Runnable() {

							public void run() {
								// TODO Auto-generated method stub
								if (mListViewSelectedItemData.mItemID
										.equals(MenuConfigManager.TV_CHANNEL_SCAN)) {
									selIdx = 0;
								} else {
									selIdx = 1;
								}
								View view = mMenuSetUpListView
										.getChildAt(selIdx);
								if (view != null) {
									MtkLog.d(TAG, "*****View != null *******");
									mMenuSetUpListView.requestFocusFromTouch();
									mMenuSetUpListView.setSelection(selIdx);
									boolean flag = true;
									List<TVChannel> channelList = mTM.getChannels();
									if (null != channelList && channelList.size() > 0) {
										flag = true;
									} else {
										flag = false;
									}
									((HaveSubPageView) mMenuSetUpListView
											.getChildAt(TV_DISABLE_ITEMS[0]))
											.getmDataItem().setEnable(flag);
									((HaveSubPageView) mMenuSetUpListView
											.getChildAt(TV_DISABLE_ITEMS[1]))
											.getmDataItem().setEnable(flag);
									mListViewAdpter.notifyDataSetChanged();
//									((CustListView) mMenuSetUpListView)
											mMenuSetUpListView.resetPosition();

								}
							}
						}, 200);

						return true;
					} else {

						mTextViewEnter.setVisibility(View.INVISIBLE);
						mTextViewSelect.setVisibility(View.VISIBLE);
						mTextViewSelect.setText(getString(R.string.menu_scan));
						mTextViewExit.setText(getString(R.string.menu_back));
						if (mScanDialog.showScanStateInfo()) {
							linearList.setVisibility(View.VISIBLE);
							mMenuSetUpListView.requestFocus();
							--mExitLevel;
						}

						mHandler.sendEmptyMessageDelayed(
								MessageType.MENU_AUTO_EXIT_MESSAGE,
								MessageType.delayMillis1);
						mScanningStatus = false;
						if (null != mDataContainer.getmFactoryDataGroup()) {
							mDataContainer.getmFactoryDataGroup().clear();
						}
						return true;
					}

				}
				if ((keyCode == KeyMap.KEYCODE_DPAD_CENTER)
						&& action == KeyEvent.ACTION_DOWN) {
					if(mScanDialog.onScanning){
						return false;
					}else{
						mScanDialog.onScanning = true;
					}
					mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
					if (mScanDialog.mStateTextView.getText().toString().equals(
							getString(R.string.menu_setup_channel_scan_done))) {
						mScanDialog.continueScan();

						return true;
					}
					if (mScanDialog.count == 1) {
						return true;
					}
					++mScanDialog.count;
					mScanDialog.mStateTextView.setText(MenuMain.this
							.getString(R.string.menu_tv_scann_allchannels));
					mScanDialog.mNumberChannel.setVisibility(View.INVISIBLE);
					mScanDialog.continueScan();
					mHandler.removeMessages(MessageType.MENU_AUTO_EXIT_MESSAGE);
					if (null != mDataContainer.getmFactoryDataGroup()) {
						mDataContainer.getmFactoryDataGroup().clear();
					}
					return true;
				}
				if (keyCode == KeyMap.KEYCODE_MENU) {
					mScanDialog.cancelScan();
					mScanDialog.loading.stopDraw();
					mScanDialog.dismiss();
					exit();
					return true;
				}
				if (null != mDataContainer.getmFactoryDataGroup()) {
					mDataContainer.getmFactoryDataGroup().clear();
				}
				return false;
			}

		});

	}
		// for language setting
	public void onConfigurationChanged(Configuration newConfig) {
		
//		mStatusBarManager.setSystemUiBarHight(0);
		mCurrentSelectedPosition = 0;
		mCurrentFouceButtonID = 4;
		mExitLevel = 1;
		mDataContainer = MenuDataContainer.getInstance(this);
		mDataContainer.loadDataSetup();
		setAdapter(mDataContainer.getSetUpDataGroup());

		mTextViewSelect.setText(getString(R.string.menu_select));
		mTextViewExit.setText(getString(R.string.menu_exit));
		mTabMenuButtonBar.mButtonShow.setText(mContext
				.getString(R.string.menu_tab_setup));
		mTabMenuButtonBar.mButtonVideo.setText(mContext
				.getString(R.string.menu_tab_video));
		mTabMenuButtonBar.mButtonAudio.setText(mContext
				.getString(R.string.menu_tab_audio));
		mTabMenuButtonBar.mButtonTV.setText(mContext
				.getString(R.string.menu_tab_tv));
		mTabMenuButtonBar.mButtonSetup.setText(mContext
				.getString(R.string.menu_tab_setup));
		mTabMenuButtonBar.mButtonParental.setText(mContext
				.getString(R.string.menu_tab_parent));
		super.onConfigurationChanged(newConfig);
		
//		StatusBarManager mStatusBarManager = (StatusBarManager) getSystemService(Context.STATUS_BAR_SERVICE);
//		mStatusBarManager.setSystemUiBarHight(0);
	}
	

	public static synchronized MenuMain getInstance() {
				if(menuMainInstance==null){
					menuMainInstance = new MenuMain();
		}
		return menuMainInstance;
	}
	
	public void requestFocus(){
		if(mMenuSetUpListView != null && mMenuSetUpListView.getVisibility()==View.VISIBLE){
			mMenuSetUpListView.setFocusable(true);
			mMenuSetUpListView.requestFocus();
		}
	}
	private void getRootRelativeLayoutRect() {
		mRootLayoutwindowId = mBypassWindowManager.getAvailableWindowId();
		if (mRootRelativeLayoutRect != null) {
			mBypassWindowManager.setBypassWindow(true, mRootLayoutwindowId,
					mRootRelativeLayoutRect);
		}
		mRootRelativeLayout.getViewTreeObserver().addOnPreDrawListener(
				new OnPreDrawListener() {
					public boolean onPreDraw() {
						if (isRootRelativeLayoutFirst) {
							mRootRelativeLayoutRect = new Rect();
							mRootRelativeLayout
									.getGlobalVisibleRect(mRootRelativeLayoutRect);
							isRootRelativeLayoutFirst = false;
							mBypassWindowManager.setBypassWindow(true,
									mRootLayoutwindowId,
									mRootRelativeLayoutRect);
						}
						return true;
					}
				});
 }
	
	public void exit() {
		MtkLog.d(TAG, " MenuMain exit!!!");
		if(AnimationManager.getInstance().getIsAnimation()){
			AnimationManager.getInstance().startActivityEndAnimation(this, findViewById(R.id.menumain_relativeLayout), null);
		}else{
			finish();
		}
	}
	   public void exitCalledByTshiftManager() {
	        changePowerOnChannel();
	        mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
	                mRootRelativeLayoutRect);

	        setResult(TurnkeyUiMainActivity.RESULT_CODE_SHOW_DEVICE_INFO_FROM_MENU);
	        exit();
	    }

	    public void calledByScheduleList() {
	        changePowerOnChannel();
	        mBypassWindowManager.setBypassWindow(false, mRootLayoutwindowId,
	                mRootRelativeLayoutRect);

	        setResult(TurnkeyUiMainActivity.RESULT_CODE_SHOW_SCHEDULE_LIST_FROM_MENU);
	        exit();
	    }
	    public void registerNoSignal(){
//	    	if(mChSel!=null){
//	    		mChSel.addSelectorListener((SelectorListener)ciMainView);
//	    	}
	    	TVChannelState.getInstance(MenuMain.this).addChannelListener((ChannelListener)ciMainView);
	    }
	    public void unRegisterNoSignal(){
	    	TVChannelState.getInstance(MenuMain.this).removeChannelListener((ChannelListener)ciMainView);
	    }
	    
	    public static boolean getChangeFreq(){
	    	MtkLog.d(TAG, "getChangeFreq:"+hasChangeFreq);
	    	return hasChangeFreq;
	    }
	    public static void setChangeFreq(boolean hasChange){
	    	hasChangeFreq = hasChange;
	    	MtkLog.d(TAG, "setChangeFreq:"+hasChangeFreq);
	    }
}
