package com.mediatek.ui.setup;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import android.app.Activity;
import android.app.AlarmManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.RemoteException;
import android.os.SystemClock;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager.LayoutParams;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TableLayout;
import android.widget.TextView;
import android.widget.AdapterView.OnItemClickListener;

import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.tv.common.ConfigType;
import com.mediatek.tvcm.TVChannelSelector;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcommon.ITVCommon;
import com.mediatek.tvcommon.TVCommonNative;
import com.mediatek.tvcommon.TVEventManager;
import com.mediatek.tvcommon.TVScanner;
import com.mediatek.tvcommon.TVCommonManager;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.ui.R;
import com.mediatek.ui.commonview.CustListView;
import com.mediatek.ui.menu.util.GetTimeZone;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.OSDLanguage;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.pippop.PippopMode_Normal;
import com.mediatek.ui.pippop.PippopState;
import com.mediatek.ui.pippop.PippopUiLogic;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;
import android.os.ServiceManager;
import android.app.StatusBarManager;


public class SetupWizardActivity extends Activity {
	private TextView mTvTopLeft, mTvTopRight, mTvFootBack, mTvFootSelect,
			mTvFootNext, mTvFootExit, mTvWelcome, mTvAnalog, mTvDigital,
			mTvPercent, mTvScanStatus, mTvScaning, mTvList, mTvCancelScan,
			mTvScanLoading,mLlWelcome;
	private CustListView mLvMiddle,mLvTuner, mLvScan;
	private LinearLayout mLlChannelScan, mLlList;
	private TableLayout mLlFoot;
	private Loading mLoading;
	private MyAdapter listAdapter;
	private ProgressBar mPbScanBar;
	private int mIntCurrent; // current value of progress bar
	private static final int MAX_PROGRESS = 100;
	private static final int MIN_PROGRESS = 0;
	private static final String TAG = "SetupWizardActivity";
	private static final int WELCOME = 0, LANGUAGE = 1, CHANNEL = 2,
			COMPLETE = 3, CANCELSCAN = 4, SCAN = 5 ,TUNER = 6;
	private int mDialogMark;
	private int mLanguageMark; // selected item of language
	private int mTunerMark; // selected item of tuner
	private int mScanMark; // selected item of scan
	private List<String> mLanguageData = new ArrayList<String>();
	private List<String> mTunerData = new ArrayList<String>();
	private List<String> mChannelData = new ArrayList<String>();
	private Handler mHandler;
	private TVScanner tvScan;
	private MenuConfigManager mConfigManager;
//	private static final String OSD_LANGUAGE = "SETUP_osd_language";
	private OSDLanguage osdLanguage; // set language
	private SaveValue saveLanguage; // save language
	private boolean mFirstMark = true; // set English for the first time
	// activity will restart after changing language
	private boolean mRestart = false;
	private TVChannelSelector mChannelSeclector;
	private NavIntegration navIntegration;

    private int nowProgress = 0;
    private int atvChannels = 0;
    private int dtvChannels = 0;
    private static boolean atvComplete = false;
    private StatusBarManager mStatusBarManager;
    private TVEventManager tvEventMg;
    private TVCommonManager mTvManager;
	private PippopUiLogic mLogic;
	private PippopState mPippopState;
    BroadcastReceiver mbr = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            NavIntegration.getInstance(SetupWizardActivity.this)
                    .iSetStorageOne();
        }
    };

    private TVScanner.ScannerListener mScannerListener = new TVScanner.ScannerListener() {
        public void onCompleted(final int completeValue) {
            mHandler.post(new Runnable() {
                public void run() {
                    switch (completeValue) {
                    // scan complete
                        case TVScanner.ScannerListener.COMPLETE_OK:
                            if (atvComplete == false) {
                                atvComplete = true;
                                if (mTunerMark == 0) {
                                    Log.d("0224", "mTunerMark:" + mTunerMark
                                            + "adjustTunnerMode dtv:");
                                    mTvManager.adjustTunnerMode(TVCommonManager.DTV);
                                }
                                tvScan.dtvScan(mScannerListener);
                            } else {
                                nowProgress = 0;
                                atvComplete = false;
                                if (mTunerMark == 0) {
                                    Log.d("0224", "mTunerMark:" + mTunerMark
                                            + "adjustTunnerMode atv:");
                                    mTvManager.adjustTunnerMode(TVCommonManager.ATV);
                                }
                                showComplete();
                            }
                            break;
                        // cancel scan
                        case TVScanner.ScannerListener.COMPLETE_CANCEL:
                            if (mTunerMark == 0) {
                                Log.d("0224", "COMPLETE_CANCEL:" + mTunerMark
                                        + "adjustTunnerMode atv:");
                                mTvManager.adjustTunnerMode(TVCommonManager.ATV);
                            }
                            mLoading.stopDraw();
                            atvComplete = false;
                            nowProgress = 0;
                            showCancelScan();
                            break;
                        // scan error
                        case TVScanner.ScannerListener.COMPLETE_ERROR:
                            if (mTunerMark == 0) {
                                Log.d("0224", "COMPLETE_ERROR:" + mTunerMark
                                        + "adjustTunnerMode atv:");
                                mTvManager.adjustTunnerMode(TVCommonManager.ATV);
                            }
                            atvComplete = false;
                            nowProgress = 0;
                            break;
                        default:
                            break;
                    }
                }
            });

        }

		public void onFrequence(int freq) {
		}

		public void onProgress(final int progress, final int channels) {

			mHandler.post(new Runnable() {
				public void run() {
					if (nowProgress == 0) {
						atvChannels = channels;
						dtvChannels = 0;
					} else {
						dtvChannels = channels;
					}

					if (progress == 100 && nowProgress == 0) {
						nowProgress = 50;
						setProgressChange(atvChannels, dtvChannels, nowProgress);
					} else {
						setProgressChange(atvChannels, dtvChannels, progress
								/ 2 + nowProgress);
					}
				}
			});
		}
	};

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		mLogic = PippopUiLogic.getInstance(this);
		if (mLogic.isPipOrPopState()) {
			try {
				TVCommonNative.getDefault(this).enterOutputMode(
						ITVCommon.OUTPUT_MODE_NORMAL);
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			mPippopState = PippopState.getPippopStateInstance(this);
			mPippopState.getmController().hiddenAllTVViews();
			mPippopState.getmController().releaseFocus();
			mPippopState.setState(new PippopMode_Normal(mPippopState));
		}
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.setup_main_layout);
		LinearLayout setupLayout = (LinearLayout) findViewById(R.id.setup_layout);
		int width = ScreenConstant.SCREEN_WIDTH*4/5;
		int height = ScreenConstant.SCREEN_HEIGHT*4/5;

        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                width, height);
        lp.gravity = Gravity.CENTER;
        setupLayout.setLayoutParams(lp);
        // TODO integration
        // set time zone TimeZone.getTimeZone("Asia/Shanghai")
        mConfigManager = MenuConfigManager.getInstance(this);
        tvScan = TVContent.getInstance(this).getScanner();
        mChannelSeclector = TVContent.getInstance(this).getChannelSelector();
        navIntegration = NavIntegration.getInstance(this);
        tvEventMg = TVContent.getInstance(this).getEventManager();
        mTvManager = TVCommonManager.getInstance(this);
        // activity will restart after changing language
        // if (savedInstanceState != null) {
        // mRestart = savedInstanceState.getBoolean("mRestart");
        // mLanguageMark = savedInstanceState.getInt("mLanguageMark");
        // mTunerMark = savedInstanceState.getInt("mTunerMark");
        // mScanMark = savedInstanceState.getInt("mScanMark");
        // Log.e("0326", "=====onCreate====mRestart=========" + mRestart);
        // }

        initView();
        mTunerMark = mConfigManager.getDefault(MenuConfigManager.TUNER_MODE) == ConfigType.BS_SRC_AIR ? 0
                : 1;
        registerReceiver();
        NavIntegration.getInstance(this).iResetDefault();
        mFirstMark = true;
        setLanguage(0);

        clearTimeShiftManagerStacks();
	}
	private void clearTimeShiftManagerStacks() {
		try {
			// TODO Auto-generated method stub
			TimeShiftManager.getInstance(this).restoreAllAfterException();
			TimeShiftManager.getInstance(this).restoreAllToNormal();
		} catch (Exception e) {
			Util.showELog("clearTimeShiftManagerStacks(),Exception:"+e.toString());
		}
	}
	protected void onResume() {
//		mStatusBarManager = (StatusBarManager) getSystemService(Context.STATUS_BAR_SERVICE);
//		mStatusBarManager.setSystemUiBarHight(0);
		super.onResume();
	}

		// restart after changing language, mRestart is true, show whether scan
	public void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
//		mStatusBarManager.setSystemUiBarHight(0);
		if (mRestart == true) {
			//showTuner();
			mRestart = false;
			showWelcome();
		} else {
			//showChannel();
			showTuner();
		}
		super.onConfigurationChanged(newConfig);
	}
	
	private void registerReceiver(){
		IntentFilter filter = new IntentFilter();
		filter.addAction(Intent.ACTION_SHUTDOWN);
		filter.setPriority(IntentFilter.SYSTEM_LOW_PRIORITY);
		Log.v("ActivityThread", "registerReceiver");
		this.registerReceiver(mbr, filter);
	}

	private void initView() {
		// top_left TextView
		mTvTopLeft = (TextView) findViewById(R.id.setup_tv_top_left);

		mTvTopLeft.setText(getString(R.string.setup_ini));
		// top_right TextView
		mTvTopRight = (TextView) findViewById(R.id.setup_tv_top_right);
		mLlWelcome = (TextView) findViewById(R.id.setup_tv_welcome); // welcome,complete
		mTvWelcome = (TextView) findViewById(R.id.setup_tv_welcome); // welcome,complete
		mTvWelcome.setText(getString(R.string.setup_welcome_set));
		mLlList = (LinearLayout) findViewById(R.id.setup_ll_middle_list); // list
		mTvList = (TextView) findViewById(R.id.setup_tv_middle); // list view

		// cancel scan,list view
		mTvCancelScan = (TextView) findViewById(R.id.setup_tv_middle_cancel_scan);
		// List in the middle
		mLvMiddle = (CustListView) findViewById(R.id.setup_list);
		mLvTuner = (CustListView) findViewById(R.id.setup_tuner_list);
		// scan List
		mLvScan = (CustListView) findViewById(R.id.setup_scan_list);
		// layout of scan TextView
		mLlChannelScan = (LinearLayout) findViewById(R.id.setup_ll_scan);
		// in middle of scan
		mTvScaning = (TextView) findViewById(R.id.setup_tv_scan_time);
		// scan status:complete
		mTvScanStatus = (TextView) findViewById(R.id.setup_tv_scan_complete);
		// scan status:scaning...
		mTvScanLoading = (TextView) findViewById(R.id.setup_tv_scan_scaning);
		// three points
		mLoading = (Loading) findViewById(R.id.setup_tv_scan_loading);
		// analog channels
		mTvAnalog = (TextView) findViewById(R.id.setup_tv_scan_analog);
		// digital channels
		mTvDigital = (TextView) findViewById(R.id.setup_tv_scan_digital);
		// percent of scan
		mTvPercent = (TextView) findViewById(R.id.setup_tv_scan_percent);
		// scan progressBar
		mPbScanBar = (ProgressBar) findViewById(R.id.setup_tv_scan_pbar);
		mPbScanBar.setMax(MAX_PROGRESS);
		mPbScanBar.setProgress(MIN_PROGRESS);
		mTvAnalog.setText(getString(R.string.setup_channel_scan_analog) + "0");
		mTvDigital
				.setText(getString(R.string.setup_channel_scan_digital) + "0");
		mTvPercent.setText("0%");
		// LinearLayout in foot
		mLlFoot = (TableLayout) findViewById(R.id.setup_ll_foot);
		mTvFootBack = (TextView) findViewById(R.id.setup_bottom_back);
		mTvFootSelect = (TextView) findViewById(R.id.setup_bottom_select);
		mTvFootNext = (TextView) findViewById(R.id.setup_bottom_next);
		mTvFootExit = (TextView) findViewById(R.id.setup_bottom_exit);
	}

	private void initData() {
		mLanguageData.clear();
		mLanguageData.add(getString(R.string.setup_language_english)); // languageData
		mLanguageData.add(getString(R.string.setup_language_chinese));
		mChannelData.clear();
		mChannelData.add(getString(R.string.setup_scan)); // channelData
		mChannelData.add(getString(R.string.setup_skip_scan));
		mTunerData.clear();
		mTunerData.add(getString(R.string.setup_tuner_antenna)); // tunerData
		mTunerData.add(getString(R.string.setup_tuner_cable));
	}

	private void initList(List<String> mDataList) {
		mLvMiddle.initData(mDataList, mDataList.size());
		listAdapter = new MyAdapter(SetupWizardActivity.this, mDataList,
				R.layout.setup_data);
		mLvMiddle.setAdapter(listAdapter);
	}
	private void initTunerList(List<String> mDataList){
		mLvTuner.initData(mDataList, mDataList.size());
		listAdapter = new MyAdapter(SetupWizardActivity.this,mDataList,R.layout.setup_data);
	    mLvTuner.setAdapter(listAdapter);
	}
	// scan
	private void initScanList(List<String> mDataList) {
		mLvScan.initData(mDataList, mDataList.size());
		listAdapter = new MyAdapter(SetupWizardActivity.this, mDataList,
				R.layout.setup_data);
		mLvScan.setAdapter(listAdapter);
	}

	// welcome
	private void showWelcome() {
		hideAll();
		mTvTopRight.setText(getString(R.string.setup_welcome));
		mLlWelcome.setVisibility(View.VISIBLE);
		mTvWelcome.setText(getString(R.string.setup_welcome_set));
		mDialogMark = WELCOME;
	}

	// set language
	private void showLanguage() {
		hideAll();
		mTvTopRight.setText(getString(R.string.setup_language));
		mLlList.setVisibility(View.VISIBLE);
		mLvMiddle.setVisibility(View.VISIBLE);
		mLvScan.setVisibility(View.GONE);
		mTvList.setVisibility(View.VISIBLE);
		mTvList.setText(getString(R.string.setup_select_language));
		initData();
		initList(mLanguageData);
		mLvMiddle.requestFocus();
		mLvMiddle.setFocusable(true);
		mLvMiddle.setItemsCanFocus(true);
		mLvMiddle.setFocusableInTouchMode(true);
		mLvMiddle.requestFocusFromTouch();
		mLvMiddle.setSelection(mLanguageMark);
		initFoot();
		mLvMiddle.setOnItemClickListener(listViewClick);
		mDialogMark = LANGUAGE;
	}
	private void showTuner(){
		hideAll();
		mTvTopRight.setText(getString(R.string.setup_tunner));
		mLlList.setVisibility(View.VISIBLE);
		mLvMiddle.setVisibility(View.GONE);
		mLvTuner.setVisibility(View.VISIBLE);
		mLvScan.setVisibility(View.GONE);
		mTvList.setVisibility(View.VISIBLE);
		mTvList.setText(getString(R.string.setup_select_tuner));
		initData();
		initTunerList(mTunerData);
		mLvTuner.requestFocus();
		mLvTuner.setFocusable(true);
		mLvTuner.setItemsCanFocus(true);
		mLvTuner.setFocusableInTouchMode(true);
		mLvTuner.requestFocusFromTouch();
		mLvTuner.setSelection(mTunerMark);
		initFoot();
		mLvTuner.setOnItemClickListener(listViewClick);
		mDialogMark = TUNER;
	}

	// whether scan or not
	private void showChannel() {
		hideAll();
		mTvTopRight.setText(getString(R.string.setup_channelscan));
		mLlList.setVisibility(View.VISIBLE);
		mTvList.setVisibility(View.VISIBLE);
		// First,please connect antenna.
		mTvList.setText(getString(R.string.setup_set_setps));
		mLvMiddle.setVisibility(View.GONE);
		mLvScan.setVisibility(View.VISIBLE);
		initData();
		initScanList(mChannelData);
		mLvScan.requestFocus();
		mLvScan.setFocusable(true);
		mLvScan.setItemsCanFocus(true);
		mLvScan.setFocusableInTouchMode(true);
		mLvScan.requestFocusFromTouch();
		mLvScan.setSelection(mScanMark);
		mLlChannelScan.setVisibility(View.GONE);
		initFoot();
		mLvScan.setOnItemClickListener(listViewClick);
		mDialogMark = CHANNEL;
	}

	// scan complete
	private void showComplete() {
		hideAll();
		mTvTopRight.setText(getString(R.string.setup_complete));
		mLlWelcome.setVisibility(View.VISIBLE);
		mTvWelcome.setText(getString(R.string.setup_congratulations));
		mLlFoot.setVisibility(View.VISIBLE);
		mTvFootBack.setVisibility(View.VISIBLE);
		mTvFootSelect.setVisibility(View.INVISIBLE);
		mTvFootNext.setVisibility(View.INVISIBLE);
		mTvFootExit.setVisibility(View.INVISIBLE);
		mTvFootBack.setText(getString(R.string.setup_back));
		mTvFootBack.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_back), null, null, null);
		mDialogMark = COMPLETE;
	}

	// cancel scan
	private void showCancelScan() {
		hideAll();
		mTvTopRight.setText(getString(R.string.setup_channelscan));
		mLlList.setVisibility(View.VISIBLE);
		mTvList.setVisibility(View.GONE);
		mTvCancelScan.setVisibility(View.VISIBLE);
		mTvCancelScan.setText(getString(R.string.setup_cancel_scan));
		mLvMiddle.setVisibility(View.GONE);
		mLvScan.setVisibility(View.VISIBLE);
		initData();
		initList(mChannelData);
		mLvScan.requestFocus();
		mLvScan.setFocusable(true);
		mLvScan.setItemsCanFocus(true);
		mLvScan.setFocusableInTouchMode(true);
		mLvScan.requestFocusFromTouch();
		mLvScan.setSelection(mScanMark);
		initFoot();
		mLvScan.setOnItemClickListener(listViewClick);
		mDialogMark = CANCELSCAN;
	}

	// scan
	private void showScan() {
		hideAll();
		mTvTopRight.setText(getString(R.string.setup_channelscan));
		mLlChannelScan.setVisibility(View.VISIBLE);
		mTvScaning.setText(getString(R.string.setup_channel_scan_note));
		mLoading.drawLoading();
		mTvScanLoading.setText(getString(R.string.setup_channel_scan_status));
		mLlFoot.setVisibility(View.VISIBLE);
		mTvFootBack.setVisibility(View.INVISIBLE);
		mTvFootSelect.setVisibility(View.INVISIBLE);
		mTvFootNext.setVisibility(View.INVISIBLE);
		mTvFootExit.setVisibility(View.VISIBLE);
		mTvFootExit.setText(getString(R.string.setup_cancel));
		// drawable set null
		mTvFootExit.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_exit), null, null, null);
		setProgressChange(0, 0, 0);

        mIntCurrent = 0;
        mHandler = new Handler();
        if (mTunerMark == 0) {
            Log.d("0224", "mTunerMark:" + mTunerMark + "adjustTunnerMode:");
            mTvManager.adjustTunnerMode(TVCommonManager.ATV);
        }
        tvScan.atvScan(mScannerListener);
        // ANR Reason: keyDispatchingTimedOut
		mDialogMark = SCAN;
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {
		// enter
		case KeyMap.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:
			switch (mDialogMark) {
			// welcome
			case WELCOME:
				showLanguage();
				break;

			// set language
			case LANGUAGE:
				languageSelect();
				break;

			case TUNER:
				mTunerMark = mLvTuner.getSelectedItemPosition();
				showChannel();
				break;
			// whether scan or not
			case CHANNEL:
				scanSelect();
				break;

			// complete
			case COMPLETE:
			List<TVChannel> mChannelList = navIntegration.iGetChannelList();
				// selected the first channel .
				if (null != mChannelList && navIntegration.iGetChannelLength() > 0) {
					MtkLog.d(TAG, "----- channel list size------"
							+ navIntegration.iGetChannelLength());
					mChannelSeclector.select(mChannelList.get(0));
				}
				// Intent intent=new
				// Intent(TurnkeyUiMainActivity.SIGNAL_BANNER_SHOW);
				// this.sendBroadcast(intent);
				mFirstMark = true; // set English
				// TurnkeyUiMainActivity.mSetupFlag = false;
				SetupWizardActivity.this.finish();
				break;

			// cancel scan
			case CANCELSCAN:
				scanSelect();
				break;

			default:
				break;
			}
			break;

		case KeyMap.KEYCODE_DPAD_RIGHT:
			switch (mDialogMark) {

			case WELCOME:
				showLanguage();
				break;

			case LANGUAGE:
				languageSelect();
				break;

			case TUNER:
				mTunerMark = mLvTuner.getSelectedItemPosition();
				showChannel();
				break;
			case CHANNEL:
			case CANCELSCAN:
				scanSelect();
				break;

			default:
				break;
			}
			break;

		case KeyMap.KEYCODE_DPAD_LEFT:
			MtkLog.d(TAG,"KEYCODE_DPAD_LEFT dialogMark:"+mDialogMark);
			switch (mDialogMark) {
			case LANGUAGE:
				mLanguageMark = mLvMiddle.getSelectedItemPosition();
				showWelcome();
				break;
			case TUNER:
				mTunerMark = mLvTuner.getSelectedItemPosition();
				showLanguage();
				break;

			case CHANNEL:
				mScanMark = mLvScan.getSelectedItemPosition();
				showLanguage();
				showTuner();
				break;

			case COMPLETE:
				showChannel();
				break;

			case CANCELSCAN:
//				showLanguage();
				showTuner();
				break;

			default:
				break;
			}
			break;

		case KeyMap.KEYCODE_DPAD_DOWN:
			switch (mDialogMark) {
			case LANGUAGE:
				mLanguageMark = mLvMiddle.getSelectedItemPosition();
				break;

			case TUNER:
				mTunerMark = mLvTuner.getSelectedItemPosition();
				break;
			case CHANNEL:
			case CANCELSCAN:
				mScanMark = mLvScan.getSelectedItemPosition();
				break;
			default:
				break;
			}
			break;

		case KeyMap.KEYCODE_DPAD_UP:
			switch (mDialogMark) {
			case LANGUAGE:
				mLanguageMark = mLvMiddle.getSelectedItemPosition();
				break;
			case TUNER:
				mTunerMark = mLvTuner.getSelectedItemPosition();
				break;

			case CHANNEL:
			case CANCELSCAN:
				mScanMark = mLvScan.getSelectedItemPosition();
				break;
			default:
				break;
			}
			break;

		case KeyMap.KEYCODE_BACK:
			switch (mDialogMark) {
			case LANGUAGE:
			case TUNER:
			case CHANNEL:
			case CANCELSCAN:
			
			List<TVChannel> mChannelList = navIntegration.iGetChannelList();
				if (null != mChannelList && navIntegration.iGetChannelLength() > 0) {
					mChannelSeclector.select(mChannelList.get(0));
				}
				// Intent intent=new
				// Intent(TurnkeyUiMainActivity.SIGNAL_BANNER_SHOW);
				// this.sendBroadcast(intent);
				// TurnkeyUiMainActivity.mSetupFlag = false;
				// NavIntegration.getInstance(this).iSetStorageOne();
				SetupWizardActivity.this.finish();
				break;

			case SCAN:
				mIntCurrent = 0;
				tvScan.cancelScan();
				return true;
			default:
				break;
			}

			break;
		default:
			break;
		}
		// Return true to prevent this event from being propagated further
		return true;
	}

	/*
	 * choose language
	 */
	private void languageSelect() {
		mLanguageMark = mLvMiddle.getSelectedItemPosition();
		changeLanguage(mLanguageMark);
	}

	/*
	 * select scan or not
	 */
	private void scanSelect() {
		mScanMark = mLvScan.getSelectedItemPosition();
		// 0,scan
		if (0 == mScanMark) {
			showScan();
		} else {
			showComplete();
		}
	}

	private class MyAdapter extends BaseAdapter {
		private List<String> mList;
		private LayoutInflater mInflater;

		private MyAdapter(Context context, List<String> data, int resource) {
			mInflater = LayoutInflater.from(context);
			mList = data;
		}

		public int getCount() {
			return mList.size();
		}

		public Object getItem(int position) {
			return position;
		}

		public long getItemId(int position) {
			return position;
		}

		public View getView(int position, View convertView, ViewGroup parent) {
			if (convertView == null) {
				convertView = mInflater.inflate(R.layout.setup_data, null);
			}
			TextView tv = (TextView) convertView
					.findViewById(R.id.common_tv_setup_dialog_data_index);
			tv.setText(mList.get(position));
			return convertView;
		}
	}

	private void setProgressChange(int analog, int digita, int percent) {
		mPbScanBar.incrementProgressBy(1);
		mPbScanBar.setProgress(percent);
		mTvPercent.setText(String.valueOf(percent) + "%");
		mTvAnalog.setText(getString(R.string.setup_channel_scan_analog)
				+ String.valueOf(analog));
		mTvDigital.setText(getString(R.string.setup_channel_scan_digital)
				+ String.valueOf(digita));
		if (percent >= MAX_PROGRESS) {
			mLoading.setVisibility(View.GONE);
			mLoading.stopDraw();
			mTvScanLoading.setVisibility(View.GONE);
			mTvScanStatus.setVisibility(View.VISIBLE);
			mTvScanStatus
					.setText(getString(R.string.setup_channel_scan_status_complete));
		} else {
			mLoading.setVisibility(View.VISIBLE);
			mTvScanLoading.setVisibility(View.VISIBLE);
			mTvScanLoading
					.setText(getString(R.string.setup_channel_scan_status));
			mTvScanStatus.setVisibility(View.GONE);
		}
	}

    /*
     * list view
     */
    private OnItemClickListener listViewClick = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> l, View v, int position, long id) {
            switch (mDialogMark) {
                case LANGUAGE:
                    if (0 == position) {
                        mLanguageMark = 0;
                    } else if (1 == position) {
                        mLanguageMark = 1;
                    } else {
                        mLanguageMark = 2;
                    }
                    changeLanguage(mLanguageMark);
                    break;
                case TUNER:
                    if (0 == position) {
                        mTunerMark = 0;
                        mConfigManager
                                .setValue(MenuConfigManager.TUNER_MODE, ConfigType.BS_SRC_AIR);
                    } else {
                        mTunerMark = 1;
                        mConfigManager.setValue(MenuConfigManager.TUNER_MODE,
                                ConfigType.BS_SRC_CABLE);
                        // showTuner();
                    }

                    showChannel();
                    break;

			case CHANNEL:
			case CANCELSCAN:
				if (0 == position) {
					mScanMark = 0;
					showScan();
				} else {
					mScanMark = 1;
					showComplete();
				}
				break;
			default:
				break;
			}
		}
	};

	private void initFoot() {
		mLlFoot.setVisibility(View.VISIBLE);
		mTvFootBack.setVisibility(View.VISIBLE);
		mTvFootSelect.setVisibility(View.VISIBLE);
		mTvFootNext.setVisibility(View.VISIBLE);
		mTvFootExit.setVisibility(View.VISIBLE);
		mTvFootBack.setText(getString(R.string.setup_back));
		mTvFootBack.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_back), null, null, null);
		mTvFootSelect.setText(getString(R.string.setup_select));
		mTvFootSelect.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_select_up), null, null, null);
		mTvFootNext.setText(getString(R.string.setup_next));
		mTvFootNext.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_next), null, null, null);
		mTvFootExit.setText(getString(R.string.setup_exit));
		mTvFootExit.setCompoundDrawablesWithIntrinsicBounds(getResources()
				.getDrawable(R.drawable.tk_cm_key_exit), null, null, null);
	}

	/*
	 * view gone
	 */
	private void hideAll() {
		mTvTopLeft.setText(getString(R.string.setup_ini));
		mLlWelcome.setVisibility(View.GONE); // welcome,complete
		mLlList.setVisibility(View.GONE); // list
		mLvTuner.setVisibility(View.GONE);//tuner
		mLlChannelScan.setVisibility(View.GONE); // scan
		mTvCancelScan.setVisibility(View.GONE); // cancel scan,list view
		mLlFoot.setVisibility(View.INVISIBLE);
	}

	// set language
	private void setLanguage(int value) {
		osdLanguage = new OSDLanguage(this.getApplicationContext());
	

		if (value < 0 || value > 1) {
			value = 0;
		}
		
		
		try {
			osdLanguage.setOSDLanguage(value);
			
				if (mFirstMark == true) {
				mFirstMark = false;
				mRestart = true;
			} else {
				mRestart = false;
			}
		} catch (RemoteException e) {
			e.printStackTrace();
		}
		saveLanguage = SaveValue.getInstance(this);
		saveLanguage.saveValue(MenuConfigManager.OSD_LANGUAGE, value);
	}

	// store values after setting language
	// protected void onSaveInstanceState(Bundle outState) {
	// Log.e("0326", "=====onSaveInstanceState====mRestart=========" +
	// mRestart);
	// if (true == mRestart) {
	// outState.putBoolean("mRestart", true);
	// outState.putInt("mLanguageMark", mLanguageMark);
	// outState.putInt("mTunerMark", mTunerMark);
	// outState.putInt("mScanMark", mScanMark);
	// }
	// super.onSaveInstanceState(outState);
	// }

	// zh-CN is Chinese
	// en- is English
	// zh-TW is Chinese-TW
	private static String getLocaleLanguage() {
		Locale mLocale = Locale.getDefault();
		return String.format("%s-%s", mLocale.getLanguage(), mLocale
				.getCountry());
	}

	// change language or not
	private void changeLanguage(int mLanMark) {
		// mCurrentLan is true,means English
		// mCurrentLan is false,means Chinese
		boolean mCurrentLan = true;
		// to tell current language is the same with selected language or not
		String mLanguage = getLocaleLanguage();
		if ("zh-CN".equals(mLanguage)) {
			mCurrentLan = false;
		} else {
			mCurrentLan = true;
		}

		// true,0 means current and selected is English
		// true,0 means current and selected is Chinese
		// current language is the same with selected language,go to scan
		// different,set system language
		if ((mCurrentLan == true && 0 == mLanMark)
				|| (mCurrentLan == false && 1 == mLanMark)) {
			showChannel();
			showTuner();
		} else {
			// set language
			setLanguage(mLanMark);
		}
	}

	protected void onStop() {
		super.onStop();
		
		this.unregisterReceiver(mbr);
	}
	
	protected void onPause() {
		NavIntegration.getInstance(this).iSetStorageOne();
		super.onPause();
	}
//	@Override
//	protected void onDestroy() {
//		// TODO Auto-generated method stub
//		super.onDestroy();
//		// set blue to on
//		mConfigManager.setValue(MenuConfigManager.BLUE_STRETCH, 1);
//	}


}
