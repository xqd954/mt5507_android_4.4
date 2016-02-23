package com.mediatek.ui.nav;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import android.app.Activity;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.Intent;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.method.ScrollingMovementMethod;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.KeyEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimer;
import com.mediatek.tvcm.TVTimerManager.TVTimerListener;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.tvcommon.TVEventManager;
import com.mediatek.tvcommon.TVEventManager.SCHEDULEEventListener;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.nav.adapter.EPGListViewAdapter;
import com.mediatek.ui.nav.commonview.EPGLinearLayout;
import com.mediatek.ui.nav.commonview.EPGListView;
import com.mediatek.ui.nav.commonview.EPGPwdDialog;
import com.mediatek.ui.nav.epginfo.EPGChannelInfo;
import com.mediatek.ui.nav.epginfo.EPGProgramInfo;
import com.mediatek.ui.nav.epginfo.EPGTimeConvert;
import com.mediatek.ui.nav.epginfo.EpgType;
import com.mediatek.ui.nav.util.CaptureLogoImp;
import com.mediatek.ui.nav.util.DataReader;
import com.mediatek.ui.nav.util.EPGConfig;
import com.mediatek.ui.nav.util.NavSundryImplement;
import com.mediatek.ui.nav.util.NewPipLogic;
import com.mediatek.ui.util.BypassWindowManager;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

public class EPGActivity extends Activity implements OnDismissListener {

	private static final String TAG = "EPGActivity";
	private static BypassWindowManager mBypassWindowManager;

	private EPGListView mListView;
	private DataReader mReader;
	private EPGTimeConvert tmCvt;

	private List<EPGChannelInfo> mGroup;
	private EPGListViewAdapter mListViewAdpter;
	private MyUpdata mMyUpData;
	private int mPageSize = 6;
	private EPGChannelInfo mListViewSelectedChild;
	private int dayNum = 0;

	private LinearLayout mRootLayout, mListViewLayout; // top layout of epg
	private LinearLayout mLinearLayout;

	private TextView mDataRetrievingShow; // data retrieving show

	private TextView mCurrentDateTv; // current date

	private TextView mSelectedDateTv; // selected date

	private TextView mBeginTimeTv; // begin time

	private TextView mEndTimeTv; // end time

	private TextView mProgramNameTv; // program name

	private TextView mProgramType; // program type

	private TextView mProgramTimeTv; // program time

	private TextView mProgramDetailTv; // program detail info

	private TextView mPageInfoTv; // page info

	private TextView mPrevDayTv; // previous day

	private TextView mNextDayTv; // next day

	private TextView mViewDetailTv; // view detail

	private TextView mTypeFilter; // type filter

	private ImageView mLockImageView;
	private ImageView mSttlImageView;

	private TVTimerListener listener;

	private NavIntegration navIntegration;

	private EPGPwdDialog mEPGPwdDialog;

	private int mTotalPage; // total page

	private int mCurrentPage = 1; // current page
	private EPGProgramInfo mCurrentSelectedProgramInfo;
	//private EPGPasswordEditText mEPGPasswordEditText;
	private String[] preValues = new String[3];

	private static long lastTime = 0L;
	private static long curTime = 0L;
	private Rect mEPGActivityRect;
	private int mEPGActivityWindowId = -1;
	private TVEventManager mTVEventManager;
	private SCHEDULEEventListener mSCHEDULEEventListener;

	private Handler mHandler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case EPGConfig.EPG_SYNCHRONIZATION_MESSAGE:
				int dayNum = msg.arg1;
				int mStartTime = msg.arg2;
				changeTimeViewsShow(dayNum, mStartTime);
				break;
			case EPGConfig.EPG_PROGRAMINFO_SHOW:
				showSelectedProgramInfo();
				break;
			case EPGConfig.EPG_DATA_RETRIEVING:
				mDataRetrievingShow
						.setText(getString(R.string.epg_retrieving_show));
				break;
			case EPGConfig.EPG_DATA_RETRIEVAL_FININSH:
				mDataRetrievingShow.setText("");
				break;
			case EPGConfig.EPG_EVENT_REASON_SCHEDULE_UPDATE_REPEAT:
				if(null != mListView){
				mListView.rawChangedOfChannel();
				}
				break;
			default:
				break;
			}

		}

	};

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		Window mWindow = getWindow();
		if(null != mWindow){
		mWindow.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}		
		setContentView(R.layout.epg_main);
		mLinearLayout = (LinearLayout) findViewById(R.id.epg_root_layout);
		mReader = DataReader.getInstance(this);
		tmCvt = EPGTimeConvert.getInstance(this);
		navIntegration = NavIntegration.getInstance(this);
		mBypassWindowManager = BypassWindowManager.getInstance(this);
		mEPGPwdDialog = new EPGPwdDialog(EPGActivity.this);
		mEPGPwdDialog.setOnDismissListener(this);
		initUI();
		setCurrentDate();
		EPGConfig.init = true;
		EPGConfig.avoidFoucsChange = false;

		if (NavSundryImplement.getInsNavSundryImplement(this).isFreeze()) {
			CaptureLogoImp.getInstance(this).freezeScreen(false);
		}
		if (navIntegration.isMute()) {
			TurnkeyUiMainActivity.setMuteVisible();
		}

	//	wmImpl.setKeyDelay(250);
		mTVEventManager = TVEventManager.getInstance(EPGActivity.this);
		mSCHEDULEEventListener = new SCHEDULEEventListener() {
			public void onChange() {
//				mListView.rawChangedOfChannel();
				mHandler.removeMessages(EPGConfig.EPG_EVENT_REASON_SCHEDULE_UPDATE_REPEAT);
				mHandler.sendEmptyMessageDelayed(EPGConfig.EPG_EVENT_REASON_SCHEDULE_UPDATE_REPEAT, 150);
			}
		};
		mTVEventManager.registerSCHEDULEEventListener(mSCHEDULEEventListener);

	}

	private void initUI() {
		//mEPGPasswordEditText = (EPGPasswordEditText) findViewById(R.id.epg_password);
		mDataRetrievingShow = (TextView) findViewById(R.id.epg_retrieving_data);
		mCurrentDateTv = (TextView) findViewById(R.id.epg_top_date_info_tv);
		mSelectedDateTv = (TextView) findViewById(R.id.epg_title_date_selected_tv);
		mBeginTimeTv = (TextView) findViewById(R.id.epg_title_time_begin_tv);
		mEndTimeTv = (TextView) findViewById(R.id.epg_title_time_end_tv);
		mProgramNameTv = (TextView) findViewById(R.id.epg_program_info_name);
		mProgramTimeTv = (TextView) findViewById(R.id.epg_program_info_time);
		mProgramDetailTv = (TextView) findViewById(R.id.epg_program_info_detail);
		mProgramType = (TextView) findViewById(R.id.epg_program_info_type);
		mPageInfoTv = (TextView) findViewById(R.id.epg_info_page_tv);
		mPrevDayTv = (TextView) findViewById(R.id.epg_bottom_prev_day_tv);
		mNextDayTv = (TextView) findViewById(R.id.epg_bottom_next_day_tv);
		mViewDetailTv = (TextView) findViewById(R.id.epg_bottom_view_detail);
		mTypeFilter = (TextView) findViewById(R.id.epg_bottom_view_filter);

		mLockImageView = (ImageView) findViewById(R.id.epg_info_lock_icon);
		mSttlImageView = (ImageView) findViewById(R.id.epg_info_sttl_icon);

		mRootLayout = (LinearLayout) findViewById(R.id.epg_root_layout);
		mListViewLayout = (LinearLayout) findViewById(R.id.epg_listview_layout);
		mListView = (EPGListView) findViewById(R.id.epg_program_forecast_listview);
		mListView.setHandler(mHandler);

		mListViewAdpter = new EPGListViewAdapter(this, mReader
				.getFirstDayStartTime());
		mProgramDetailTv.setMovementMethod(ScrollingMovementMethod
				.getInstance());
		mListViewAdpter.setHandler(mHandler);
		mMyUpData = new MyUpdata();
		Display display = getWindowManager().getDefaultDisplay();
		DisplayMetrics mDisplayMetrics = new DisplayMetrics();
		display.getMetrics(mDisplayMetrics);
		int width = (int) ((display.getWidth() - 46 * mDisplayMetrics.density) * 0.75f) - 2;
		mListViewAdpter.setWidth(width);
		MtkLog.d(TAG, "List View Program Info Total Width----------------->"
				+ width);

		mRootLayout.requestFocus();
		mListViewLayout.requestFocus();
		if (!mListView.hasFocus()) {
			MtkLog.d(TAG, "[ListView has not focus]");
			mListView.requestFocus();
			mListView.setSelection(0);
		} else {
			MtkLog.d(TAG, "[ListView has  focus]");
		}
		if (mListView.hasFocus()) {
			MtkLog.d(TAG, "[The second time, ListView has focus]");
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		if (!NewPipLogic.isTvNormal()) {
			TVContent.getInstance(this).setOpacity(200);
			NewPipLogic pipLogic = NewPipLogic.getInstance(this);
			pipLogic.resumeMainOutput();
		}
		if (mListViewAdpter != null && mListViewAdpter.getGroup() == null) {
			mHandler.post(new Runnable() {

				public void run() {
					ArrayList<EPGChannelInfo> mChannelList = (ArrayList<EPGChannelInfo>) mReader
							.getChannelList();
					if (mChannelList != null && mChannelList.size() > 0) {
						int index = mReader.getCurrentPlayChannelPosition();
						int pageNum = index / mPageSize + 1;
						setAdapter(mChannelList, 0, pageNum);
						changeTimeViewsShow(dayNum, mListViewAdpter
								.getStartTime());
						int position = index % mPageSize;
						mListView.setSelection(position);
						EPGConfig.SELECTED_CHANNEL_POSITION = position;
						mHandler.sendEmptyMessageDelayed(
								EPGConfig.EPG_PROGRAMINFO_SHOW, 300);
					}
				}
			});
		}
		mEPGActivityWindowId = mBypassWindowManager.getAvailableWindowId();
		mEPGActivityRect = new Rect(0,0,ScreenConstant.SCREEN_WIDTH,ScreenConstant.SCREEN_HEIGHT);
	    mBypassWindowManager.setBypassWindow(true,mEPGActivityWindowId,mEPGActivityRect); 
	}
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		MtkLog.i(TAG,"EPG on Pause");
		mBypassWindowManager.setBypassWindow(false,mEPGActivityWindowId,mEPGActivityRect); 
	}

	@Override
	protected void onStop() {
		if (mGroup != null) {
			mGroup = null;
		}
		if (mListViewAdpter != null) {
			mListViewAdpter = null;
		}
		if (mListView != null) {
			mListView = null;
		}
		mReader.removeTimerListener(listener);
	//	wmImpl.setKeyDelay(50);
		mTVEventManager.unregisterSCHEDULEEventListener(mSCHEDULEEventListener);
		super.onStop();
	}

	class MyUpdata implements EPGListView.UpDateListView {
		@SuppressWarnings("unchecked")
		public void updata() {
			mHandler.removeMessages(EPGConfig.EPG_DATA_RETRIEVING);
			mHandler.sendEmptyMessage(EPGConfig.EPG_DATA_RETRIEVING);
			mGroup = (List<EPGChannelInfo>) mListView.getCurrentList();
			mListViewAdpter.setGroup(mGroup, mListViewAdpter.getDayNum());
			mListView.setAdapter(mListViewAdpter);
		}
	}

	@SuppressWarnings("unchecked")
	private void setAdapter(List<?> adpter, int dayNum, int pageNum) {
		mListView.initData(adpter, mPageSize, mMyUpData, pageNum);
		mGroup = (List<EPGChannelInfo>) mListView.getCurrentList();
		mListViewAdpter.setGroup(mGroup, dayNum);
		mListView.setAdapter(mListViewAdpter);

	}

	/**
	 * set the current date in the top
	 */
	private void setCurrentDate() {
		listener = new TVTimerListener() {
			public void onTimeTick(TVTimer[] timers) {
				// TODO Auto-generated method stub
				lastTime = curTime;
//				curTime = DataReader.getCurrentDate().getTime();
				curTime = DataReader.getCurrentDateAsMills();
				if (curTime - lastTime <= -120 * 1000
						|| curTime - lastTime >= 120 * 1000) {
					MtkLog.d(TAG, "=====curTime - last time: "
							+ (curTime - lastTime));
					dayNum = mListViewAdpter.getDayNum();

//					EPGConfig.FROM_WHERE = EPGConfig.FROM_ANOTHER_STREAM;
					EPGConfig.FROM_WHERE = EPGConfig.AVOID_PROGRAM_FOCUS_CHANGE;
					mListViewAdpter.setDayNum(dayNum);
					mListViewAdpter.setStartTime(DataReader.getCurrentDate()
							.getHours());
					changeTimeViewsShow(dayNum, DataReader.getCurrentDate()
							.getHours());
					mHandler.removeCallbacks(mObtainChannelInfoRunnalbe);
					mHandler.postDelayed(mObtainChannelInfoRunnalbe, 500);
				}
				String mDate = tmCvt.getDetailDate(DataReader.getCurrentDate());
				mCurrentDateTv.setText(mDate);
			}
		};
		mReader.registerTimerListener(listener);

	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		switch (keyCode) {

		case KeyEvent.KEYCODE_MTKIR_GUIDE:
			this.finish();
			break;
		case KeyMap.KEYCODE_MENU:
			Intent intent = new Intent(EPGActivity.this, MenuMain.class);
			startActivity(intent);
			finish();
			break;
		case KeyEvent.KEYCODE_MTKIR_SOURCE:
			return true;
			// yellow key
		case KeyEvent.KEYCODE_Y:
		case KeyEvent.KEYCODE_MTKIR_YELLOW:
			// page info
			if (mTotalPage > 1) {
				mCurrentPage++;
				if (mCurrentPage > mTotalPage) {
					mCurrentPage = 1;
				}
				mProgramDetailTv.scrollTo(0, (mCurrentPage - 1) * 3
						* mProgramDetailTv.getLineHeight());
				mPageInfoTv.setText((mCurrentPage) + "/" + mTotalPage);
			}
			break;
		case KeyEvent.KEYCODE_MTKIR_BLUE:
		case KeyEvent.KEYCODE_B:
			changeBottomViewText(true, keyCode);
			EpgType mEpgType = new EpgType(this);
			mEpgType.show();

			break;
		case KeyEvent.KEYCODE_MTKIR_GREEN:
		case KeyEvent.KEYCODE_G:
			dayNum = mListViewAdpter.getDayNum();
			if (dayNum == EPGConfig.mMaxDayNum) {
				return false;
			}
			EPGConfig.FROM_WHERE = EPGConfig.FROM_KEYCODE_MTKIR_RED;
			dayNum = dayNum + 1;
			mListViewAdpter.setDayNum(dayNum);
			mListViewAdpter.setStartTime(0);
			changeTimeViewsShow(dayNum, 0);
			mHandler.removeMessages(EPGConfig.EPG_DATA_RETRIEVING);
			mHandler.sendEmptyMessage(EPGConfig.EPG_DATA_RETRIEVING);
			mHandler.removeCallbacks(mObtainChannelInfoRunnalbe);
			mHandler.postDelayed(mObtainChannelInfoRunnalbe, 500);
			break;
		case KeyEvent.KEYCODE_MTKIR_RED:
		case KeyEvent.KEYCODE_R:
			dayNum = mListViewAdpter.getDayNum();
			if (dayNum == 0) {
				return false;
			}
			EPGConfig.FROM_WHERE = EPGConfig.FROM_KEYCODE_MTKIR_GREEN;
			dayNum = dayNum - 1;
			mListViewAdpter.setDayNum(dayNum);
			if (dayNum == 0) {
				mListViewAdpter.setStartTime(mReader.getFirstDayStartTime());
			} else {
				mListViewAdpter.setStartTime(0);
			}
			changeTimeViewsShow(dayNum, mListViewAdpter.getStartTime());
			mHandler.removeMessages(EPGConfig.EPG_DATA_RETRIEVING);
			mHandler.sendEmptyMessage(EPGConfig.EPG_DATA_RETRIEVING);
			mHandler.removeCallbacks(mObtainChannelInfoRunnalbe);
			mHandler.postDelayed(mObtainChannelInfoRunnalbe, 500);
			break;
		case KeyMap.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_ENTER:
		case KeyEvent.KEYCODE_E:
			MtkLog.d(TAG, "KEYCODE_DPAD_CENTER");
			mListViewSelectedChild = (EPGChannelInfo) mListView
					.getItemAtPosition(EPGConfig.SELECTED_CHANNEL_POSITION);
			if (mListViewSelectedChild != null) {
				// If current channel is locked
				TVChannel tvChannel = mListViewSelectedChild.getTVChannel();
				if (tvChannel != null && tvChannel.isBlocked()
						/*&& !tvChannel.isUsrUnblocked()*/) {
					mEPGPwdDialog.show();
					changeBottomViewText(true, keyCode);
					setProgramInfoViewsInVisiable();
				}
			}

			break;
		case KeyEvent.KEYCODE_MTKIR_MUTE:
			TurnkeyUiMainActivity.switchMute();
			return true;

		default:
			break;
		}

		if (keyCode == KeyMap.KEYCODE_VOLUME_UP
				|| keyCode == KeyMap.KEYCODE_VOLUME_DOWN) {
			return true;
		}

		return super.onKeyDown(keyCode, event);
	}

	private void changeTimeViewsShow(int dayNum, int mStartTime) {
//		Date now = DataReader.getCurrentDate();
		Long time = 24 * 60 * 60 * 1000L * dayNum;
//		Date day = new Date(now.getTime() + time);
		Date day = new Date(DataReader.getCurrentDateAsMills() + time);
		String dateStr = tmCvt.getSimpleDate(day);
		mSelectedDateTv.setText(dateStr);

		if (dayNum == 0) {
			mPrevDayTv.setText("");
		} else {
			mPrevDayTv.setText(getString(R.string.epg_bottom_prev_day));
		}

		if (dayNum == EPGConfig.mMaxDayNum) {
			mNextDayTv.setText("");
		} else {
			mNextDayTv.setText(getString(R.string.epg_bottom_next_day));
		}
		int mBeginTime = mStartTime % 24;
		mBeginTimeTv.setText(mBeginTime + ":00");
		int mEndTime = (mBeginTime + EPGConfig.mTimeSpan - 1) % 24;
		mEndTimeTv.setText(mEndTime + ":00");
	}

	ObtainChannelInfoRunnalbe mObtainChannelInfoRunnalbe = new ObtainChannelInfoRunnalbe();

	class ObtainChannelInfoRunnalbe implements Runnable {
		public void run() {
			if (mListViewAdpter != null) {
				List<EPGChannelInfo> mGroup = mListViewAdpter.getGroup();
				mListViewAdpter.setGroup(mGroup);
				mListView.setAdapter(mListViewAdpter);
				mHandler.sendEmptyMessage(EPGConfig.EPG_DATA_RETRIEVAL_FININSH);
				// mListViewAdpter.notifyDataSetChanged();
				mListView.setSelection(EPGConfig.SELECTED_CHANNEL_POSITION);
				// mHandler.sendEmptyMessage(EPGConfig.EPG_PROGRAMINFO_SHOW);
				showSelectedProgramInfo();

			}
		}

	}

	protected void showSelectedProgramInfo() {
		// TODO Auto-generated method stub
		setProgramInfoViewsInVisiable();
		if (mListViewAdpter != null) {
			mListViewSelectedChild = (EPGChannelInfo) mListView
					.getItemAtPosition(EPGConfig.SELECTED_CHANNEL_POSITION);
			// if current select channel is digital channel
			if (mListViewSelectedChild != null) {
				// If current channel is locked
				TVChannel tvChannel = mListViewSelectedChild.getTVChannel();
				if (tvChannel != null && tvChannel.isBlocked() && mEPGPwdDialog != null && mEPGPwdDialog.isShowing()
						/*&& !tvChannel.isUsrUnblocked()*/) {
					return;
				}
				// get current program info
				EPGLinearLayout childView = mListView
						.getSelectedDynamicLinearLayout(EPGConfig.SELECTED_CHANNEL_POSITION);
				List<EPGProgramInfo> mProgramList = mListViewSelectedChild
						.getmTVProgramInfoList();
				if (mProgramList != null && childView != null
						&& mProgramList.size() > 0
						&& childView.getmCurrentSelectPosition() >= 0) {
					mCurrentSelectedProgramInfo = mProgramList.get(childView
							.getmCurrentSelectPosition());
					if (mCurrentSelectedProgramInfo != null) {
						mProgramNameTv.setVisibility(View.VISIBLE);
						mProgramNameTv.setText(mCurrentSelectedProgramInfo
								.getTitle());
						mProgramTimeTv.setVisibility(View.VISIBLE);
						mProgramTimeTv
								.setText(tmCvt
										.formatProgramTimeInfo(mCurrentSelectedProgramInfo));
						setLockImageViewState(mListViewSelectedChild
								.getTVChannel());
						setSubTitleImageViewState(mCurrentSelectedProgramInfo);
						setProgramDetailTvState(mListViewSelectedChild
								.getTVChannel(), mCurrentSelectedProgramInfo);
						mProgramType.setText(childView.getProgramType());
					}

				} else if (childView == null) {
					mHandler.removeMessages(EPGConfig.EPG_PROGRAMINFO_SHOW);
					mHandler.sendEmptyMessage(EPGConfig.EPG_PROGRAMINFO_SHOW);
				}
			}

		}
	}

	protected void setProgramInfoViewsInVisiable() {
		mProgramNameTv.setVisibility(View.INVISIBLE);
		mProgramTimeTv.setVisibility(View.INVISIBLE);
		mLockImageView.setVisibility(View.INVISIBLE);
		mProgramDetailTv.setVisibility(View.INVISIBLE);
		mSttlImageView.setVisibility(View.INVISIBLE);
		mPageInfoTv.setText("");
		mViewDetailTv.setText("");
		mProgramType.setText("");
	}

	protected void setLockImageViewState(TVChannel mChannel) {
		if (mChannel != null) {
			if (mChannel.isBlocked()/* && !mChannel.isUsrUnblocked()*/) {
				mLockImageView.setVisibility(View.VISIBLE);
			} else {
				mLockImageView.setVisibility(View.INVISIBLE);
			}
		}
	}

	protected void setSubTitleImageViewState(EPGProgramInfo childProgramInfo) {
		if (childProgramInfo != null) {
			String sub = navIntegration.iGetCurrentSubtitleLang();
			if (null != sub  && sub.length() != 0 ) {
				mSttlImageView.setVisibility(View.VISIBLE);
			} else {
				mSttlImageView.setVisibility(View.INVISIBLE);
			}
		} else {
			mSttlImageView.setVisibility(View.INVISIBLE);
		}
	}

	protected void setProgramDetailTvState(TVChannel mChannel,
			EPGProgramInfo childProgramInfo) {

		if (mChannel.isBlocked()/* && !mChannel.isUsrUnblocked()*/) {
			// if current channel is locked, hide program detail info
			mProgramDetailTv.setVisibility(View.INVISIBLE);
			mViewDetailTv.setText("");
			mPageInfoTv.setText("");
		} else if (childProgramInfo != null
				&& childProgramInfo.getDescribe() != null) {
			mProgramDetailTv.setVisibility(View.VISIBLE);
			String mDetailContent = childProgramInfo.getDescribe();
			mProgramDetailTv.setText(mDetailContent);
			initProgramDetailContent();
		} else {
			mProgramDetailTv.setText("");
			mViewDetailTv.setText("");
			mPageInfoTv.setText("");
			mTotalPage = 0;
		}
	}

	protected void initProgramDetailContent() {
		int line = mProgramDetailTv.getLineCount();
		MtkLog.d(TAG, "--- initProgramDetailContent()---- Lines: " + line);
		if (line > 0) {
			mTotalPage = (line % 3 == 0) ? (line / 3) : (line / 3 + 1);
			mCurrentPage = 1;
			mProgramDetailTv.scrollTo(0, (mCurrentPage - 1) * 3
					* mProgramDetailTv.getLineHeight());
			if (mTotalPage > 1) {
				mPageInfoTv.setText((mCurrentPage) + "/" + mTotalPage);
				mViewDetailTv.setText(getResources().getString(
						R.string.epg_bottom_view_detail));
			}
		} else {
			mHandler.postDelayed(new Runnable() {
				public void run() {
					initProgramDetailContent();

				}
			}, 800);
		}
	}

	/*public boolean passwrodViewInterceptKeyEvent() {
		if (mEPGPasswordEditText != null
				&& mEPGPasswordEditText.getVisibility() == View.VISIBLE) {
			return true;
		}
		return false;
	}*/

	public void changeBottomViewText(boolean isEnter, int keyCode) {
		if (isEnter) {
			savePreValues();
			mPrevDayTv.setText("");
			mNextDayTv.setText("");
			mViewDetailTv.setText("");
			switch (keyCode) {
			case KeyMap.KEYCODE_MTKIR_BLUE:
			case KeyEvent.KEYCODE_B:
				mTypeFilter.setText(getResources().getString(R.string.setup_exit));
				break;
			case KeyMap.KEYCODE_DPAD_CENTER:
			case KeyEvent.KEYCODE_ENTER:
			case KeyEvent.KEYCODE_E:
				mTypeFilter.setText("");
				break;
			default:
				break;
			}
		} else {
			mPrevDayTv.setText(preValues[0]);
			mNextDayTv.setText(preValues[1]);
			mViewDetailTv.setText(preValues[2]);
			mTypeFilter.setText(getResources().getString(R.string.epg_bottom_type_filter));
		}
	}

	private void savePreValues() {
		preValues[0] = mPrevDayTv.getText().toString();
		preValues[1] = mNextDayTv.getText().toString();
		preValues[2] = mViewDetailTv.getText().toString();

	}

	public void onDismiss(DialogInterface dialog) {
		MtkLog.d(TAG, "PWD onDismiss!!");
		showSelectedProgramInfo();
		changeBottomViewText(false, 0);
	}
}
