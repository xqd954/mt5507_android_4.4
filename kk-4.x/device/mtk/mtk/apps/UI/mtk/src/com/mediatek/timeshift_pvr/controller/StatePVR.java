/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import java.lang.ref.WeakReference;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.widget.PopupWindow.OnDismissListener;
import android.widget.TextView;

import com.mediatek.mmpcm.videoimpl.VideoManager;
import com.mediatek.timeshift_pvr.manager.PvrConstant;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.timeshift_pvr.ui.OnPVRDialogListener;
import com.mediatek.timeshift_pvr.ui.PVRControlbar;
import com.mediatek.timeshift_pvr.ui.PVRTimerView;
import com.mediatek.timeshift_pvr.ui.PvrDialog;
import com.mediatek.tvcm.TVContent;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.MenuMain;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.util.KeyMap;

/**
 * 
 */
public class StatePVR extends StateBase {

	private static StatePVR mStateSelf;
	private MyHandler mHandler;

	private PVRFiles mPVRFile;

	private PVRControlbar mBigCtrlBar;
	private PVRTimerView mSmallCtrlBar;

	private TextView mBRecordTimer;
	private TextView mBTextViewDate;

	private TextView mBTextViewFileInfo;

	private String recordTimeStr = "00:00:00";
	private String remainTimeStr = "00:00"; // Depend On Disk's free Size

	public int recordTimingLong = 30 * 60; // MAX time: 30 Minutes?
	public int schedulePvrTimeing = -1; // MAX time: 30 Minutes?
	private String recordTimingStr = "00:30:00";
	private final int INTERVAL_SECOND = 30;

	private final int MAX_TIMING = 1 * 60 * 60;
	private final int MIN_TIMING = 30;

	private int lastFreeSize = 0; // unit kb.

	private int recordTimer = 0;

	private StringBuilder mAllTimerToStr = new StringBuilder();

	private final Long mDuration = 10L * 1000;

	private final static int SHOW_BIG_CTRL_BAR = 1;
	private final static int SHOW_SMALL_CTRL_BAR = 2;
	private final static int Clear_All_View = 12;

	private final static int START_RECORD_TIMER = 3;
	private final static int PAUSE_RECORD_TIMER = 4;
	private final static int STOP_RECORD_TIMER = 5;

	private final static int REFRESH_TIMER = 6;

	private final static int RECORD_START = 10;
	private final static int RECORD_STOP = 11;
	private final static int RECORD_PAUSE = 13;

	private final static int DEBUGDEBUG = 111111;

	protected static final int MMP_PLAYER_ERROR = 20;
	private static final String TAG = "StatePVR";
	public String isSameSource = "";

	private VideoManager playback;
	public static boolean isDotByDotMode=false;
	private boolean mShowDiskAttention = true;
	
	static class MyHandler extends Handler {
		WeakReference<Activity> mActivity;

		MyHandler(Activity activity) {
			mActivity = new WeakReference<Activity>(activity); // reserve.
		}

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case SHOW_BIG_CTRL_BAR:
				if (mStateSelf.isRunning()) {
					mStateSelf.showBigCtrlBar();
					mStateSelf.clearWindow(false);
				}
				break;
			case SHOW_SMALL_CTRL_BAR:
				if (mStateSelf.isRunning()) {
					mStateSelf.showSmallCtrlBar();
					mStateSelf.clearWindow(true);
				}
				break;
			case START_RECORD_TIMER:
				break;
			case PAUSE_RECORD_TIMER:
				break;
			case STOP_RECORD_TIMER:
				break;

			case REFRESH_TIMER:
				removeMessages(REFRESH_TIMER);
				boolean willStopPvr = mStateSelf.checkTimer();

				if (willStopPvr) {
					mStateSelf
							.stopPVRRecord(PvrConstant.RECORD_FINISH_AND_SAVE);
					mStateSelf.getManager().restoreToDefault(mStateSelf);
					return;
				}
				mStateSelf.setRecordTimer(mStateSelf.getRecordTimer() + 1);

				mStateSelf.refreshTimer();

				if (mStateSelf.isRunning()) {
					sendEmptyMessageDelayed(REFRESH_TIMER, 1 * 1000);
				}
				break;

			case MMP_PLAYER_ERROR:

				break;
			case RECORD_START:
				mStateSelf.setRunning(true);
				mStateSelf.getController().preparePvr(true, false);
				// sendEmptyMessageDelayed(DEBUGDEBUG, 10*1000);
				Util.showRecordToast(mStateSelf.getActivity());
				break;

			case DEBUGDEBUG:
				// mStateSelf.getController().startPvr();
				break;

			case RECORD_STOP:
				mStateSelf.setRunning(false);
				mStateSelf.getController().stopPvr(true);
				break;
			case RECORD_PAUSE:
				mStateSelf.setRunning(false);
				mStateSelf.getController().pausePvr();
				break;
			case PvrConstant.Dissmiss_PVR_BigCtrlbar:
				mStateSelf.dissmissBigCtrlBar();
				break;
			case Clear_All_View:
				mStateSelf.clearAllWindow();
				//MenuMain.getInstance().refreshRecordSetting();
				break;
			default:
				break;
			}

			super.handleMessage(msg);
		}
	}

	public static StatePVR getInstance(Context mContext,
			TimeShiftManager manager) {
		if (mStateSelf == null) {
			mStateSelf = new StatePVR(mContext, manager);
		}
		return mStateSelf;
	}

	public static StatePVR getInstance() {
		if (mStateSelf != null) {
			return mStateSelf;
		} else {
			return null;
		}
	}

	public StatePVR(Context mContext, TimeShiftManager manager) {
		super(mContext, manager);
		setType(StatusType.PVR);
		mStateSelf = this;
		mShowDiskAttention = true;
		mHandler = new MyHandler(getActivity());
		mHandler.sendEmptyMessage(REFRESH_TIMER);

		this.startPVRrecord();
		setRunning(true);

		onResume();
	}

	/**
	 * @return
	 */
	private PVRFiles debugPVRFile() {
		PVRFiles file = new PVRFiles("I'm Singer", "CCTV-x");

		return file;
	}

	/*
	 * 
	 */
	@Override
	public void initViews() {
		super.initViews();
		mPVRFile = debugPVRFile();
		initBigCtrlBar();
	}

	/**
	 * 
	 */
	private void initBigCtrlBar() {
		mBigCtrlBar = new PVRControlbar(getActivity(),
				R.layout.pvr_timeshfit_pvrworking, 10L * 1000, this);

		mBRecordTimer = (TextView) mBigCtrlBar.getContentView().findViewById(
				R.id.pvr_working_rec_time);
		mBTextViewDate = (TextView) mBigCtrlBar.getContentView().findViewById(
				R.id.pvr_working_currenttime);

		mBTextViewFileInfo = (TextView) mBigCtrlBar.getContentView()
				.findViewById(R.id.info);

		mBigCtrlBar.setOnDismissListener(new OnDismissListener() {
			@Override
			public void onDismiss() {
				mHandler.sendEmptyMessage(SHOW_SMALL_CTRL_BAR);
			}
		});

		showProgramInfo();
	}

	/**
	 * 
	 */
	private void showProgramInfo() {

		StringBuilder info = new StringBuilder();
		info.append("1");
		info.append("      ");
		info.append(getManager().getTvLogicManager().getChannelNumStr());
		info.append("      ");
		info.append(getManager().getTvLogicManager().getChannelName());
		// mPVRFile
		mBTextViewFileInfo.setText(info.toString());

	}

	/*
	 * (non-Javadoc)
	 */
	@Override
	public boolean onKeyDown(int keycode) {
		Util.Hlog("onKeyDown: StatePVR.java");
		switch (keycode) {
		case KeyEvent.KEYCODE_MEDIA_STOP:
			// stopPVRRecord();
			stopPVRRecord(PvrConstant.RECORD_FINISH_AND_SAVE);
			return true;
		case KeyMap.KEYCODE_DPAD_UP:
			if (mBigCtrlBar != null && mBigCtrlBar.isShowing()) {
				addHourTiming();
				showBigCtrlBar();
			}
			break;

		case KeyMap.KEYCODE_DPAD_DOWN:
			if (mBigCtrlBar != null && mBigCtrlBar.isShowing()) {
				decreaseHourTiming();
				showBigCtrlBar();
			}
			break;
		case KeyMap.KEYCODE_DPAD_LEFT:
			if (mBigCtrlBar != null && mBigCtrlBar.isShowing()) {
				decreaseRecordTiming();
				showBigCtrlBar();
			}
			break;

		case KeyMap.KEYCODE_DPAD_RIGHT:
			if (mBigCtrlBar != null && mBigCtrlBar.isShowing()) {
				addRecordTiming();
				showBigCtrlBar();
			}
			break;

		case KeyEvent.KEYCODE_MEDIA_RECORD:
			if (isRecording()) {
				mHandler.sendEmptyMessage(SHOW_BIG_CTRL_BAR);
				getManager().showPvrIsRecording();
			} else {
				startPVRrecord();
			}
			return true;
		case KeyEvent.KEYCODE_MTKIR_REPEAT: // play list
			if (mBigCtrlBar != null && mBigCtrlBar.isShowing()) {
				return true;
			} else {
				getManager().setState(StateFileList.getInstance(getManager()));
				((StateFileList) getManager().getState()).reShowPVRlist();
			}

			return true;
		case KeyEvent.KEYCODE_MTKIR_RED:
			
				PvrDialog launchDialog = new PvrDialog(getActivity(),
						PvrDialog.TYPE_Confirm, KeyMap.KEYCODE_MTKIR_RED,
						PvrDialog.TYPE_Record);
				launchDialog.setOnPVRDialogListener(new OnPVRDialogListener() {

					@Override
					public void onPVRDialogListener(int keyCode) {
						// TODO Auto-generated method stub
						TurnkeyUiMainActivity.getInstance().startLauncher();
						mStateSelf.getManager().restoreAllToNormal();
					}
				});
				launchDialog.show();
			
			return true;
		case KeyEvent.KEYCODE_MEDIA_EJECT:
		case KeyEvent.KEYCODE_MTKIR_ZOOM:
		case KeyEvent.KEYCODE_MEDIA_PLAY_PAUSE:
			return false;
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

			return false;
		case KeyEvent.KEYCODE_MTKIR_FREEZE:
		case KeyEvent.KEYCODE_MTKIR_ASPECT:
		case KeyEvent.KEYCODE_DPAD_CENTER:
			Util.Hlog("Do nothing");
			return false;

		case KeyEvent.KEYCODE_MTKIR_GREEN:
			Util.Hlog("onKeyDown: StatePVR.java-->return green.");
			return true;
		case KeyEvent.KEYCODE_MEDIA_FAST_FORWARD:
			return true;
		case KeyEvent.KEYCODE_MEDIA_REWIND:
			return true;

		case KeyEvent.KEYCODE_CHANNEL_DOWN:
		case KeyEvent.KEYCODE_CHANNEL_UP:
		case KeyMap.KEYCODE_MTKIR_CHUP:
		case KeyMap.KEYCODE_MTKIR_CHDN:
			
			//Fix CR:DTV00598384
			if(!getManager().getTvLogicManager().isDTV()){
				return false;
			}
		case KeyMap.KEYCODE_MTKIR_ANGLE:
		case KeyMap.KEYCODE_MTKIR_PIPPOP:
		case KeyMap.KEYCODE_MTKIR_PRECH:

			PvrDialog conDialog = new PvrDialog(getActivity(),
					PvrDialog.TYPE_Confirm, keycode, PvrDialog.TYPE_Record);
			conDialog.setOnPVRDialogListener(new OnPVRDialogListener() {

				@Override
				public void onPVRDialogListener(int keyCode) {
					// TODO Auto-generated method stub
					TurnkeyUiMainActivity.getInstance().handleChangeChannel(
							keyCode);
				}
			});
			conDialog.show();
			return true;

		case KeyEvent.KEYCODE_GUIDE:
		case KeyMap.KEYCODE_MTKIR_GUIDE:
			if (NavIntegration.getInstance(mContext).isCurrentSourceDTV()
					&& !NavIntegration.getInstance(mContext)
							.isCurrentSourceBlocked()) {
				PvrDialog conDialog2 = new PvrDialog(getActivity(),
						PvrDialog.TYPE_Confirm, keycode, PvrDialog.TYPE_Record);
				conDialog2.show();
			}
			return true;
		case KeyMap.KEYCODE_BACK:
			if (mHandler != null) {
				mHandler.sendEmptyMessage(SHOW_SMALL_CTRL_BAR);
			}
			return false;
		default:
			break;
		}

		return false;
	}

	/**
	 * 
	 */
	private void addRecordTiming() {
		recordTimingLong = INTERVAL_SECOND + recordTimingLong;

		if (recordTimingLong > MAX_TIMING) {
			recordTimingLong = MAX_TIMING;
		}

		setRecordTimingStr(Util.secondToString(recordTimingLong));
		refreshTimer();
	}

	/**
	 * 
	 */
	private void decreaseRecordTiming() {
		recordTimingLong = recordTimingLong - INTERVAL_SECOND;

		if (recordTimingLong < MIN_TIMING || recordTimingLong < recordTimer) {
			recordTimingLong = Math.max(MIN_TIMING, (recordTimer
					/ INTERVAL_SECOND + 1)
					* INTERVAL_SECOND);
		}
		setRecordTimingStr(Util.secondToString(recordTimingLong));
		refreshTimer();
	}

	private void decreaseHourTiming() {
		recordTimingLong = recordTimingLong - INTERVAL_SECOND * 60;

		if (recordTimingLong < MIN_TIMING || recordTimingLong < recordTimer) {
			recordTimingLong = Math.max(MIN_TIMING, (recordTimer
					/ INTERVAL_SECOND + 1)
					* INTERVAL_SECOND);
		}
		setRecordTimingStr(Util.secondToString(recordTimingLong));
		refreshTimer();
	}

	private void addHourTiming() {
		recordTimingLong = INTERVAL_SECOND * 60 + recordTimingLong;

		if (recordTimingLong > MAX_TIMING) {
			recordTimingLong = MAX_TIMING;
		}

		setRecordTimingStr(Util.secondToString(recordTimingLong));
		refreshTimer();
	}

	/*
	 * 
	 */
	@Override
	public void onResume() {
		// TODO Auto-generated method stub
		if (isRunning()) {
			showBigCtrlBar();
		}

		clearWindow(false);

		mHandler.sendEmptyMessage(REFRESH_TIMER);
	}

	/* 
	 */
	@Override
	public void onPause() {
		clearWindow(true);
	}

	/* 
	 */
	@Override
	public void onStop() {
		stopPVRRecord(TimeShiftManager.DEFAULT);
	}

	/* 
	 */
	@Override
	public void onRelease() {
		isSameSource="";
		if (isRunning()) {
			stopPVRRecord(PvrConstant.RECORD_FINISH_AND_SAVE);
			setRunning(false);
		}
		
		mHandler.sendEmptyMessage(Clear_All_View);
		
		if(isDotByDotMode){
			getManager().getTvLogicManager().setDotByDotScreenMode();
		}
		
		getManager().getController().setTsFile("");
	}
	public boolean startPVRrecord() {
		if (mShowDiskAttention) {
			getManager().showDiskAttention();
			mShowDiskAttention = false;
		}
		
		isSameSource = getManager().getTvLogicManager().getInputSource();
		mHandler.sendEmptyMessageDelayed(RECORD_START, 50);
		return false;
	}

	public boolean stopPVRRecord(int messageType) {
		Util.showDLog("state,stopPVRRecord()");

		if (isRunning()) {
			TimeShiftManager.getInstance().showRecordFinish(messageType);
		}
		setRunning(false);
		getController().stopPvr(true);
		isSameSource = "";

		setRunning(false);
		mHandler.removeMessages(Clear_All_View);
		mHandler.sendEmptyMessageDelayed(Clear_All_View, 500);
		getManager().showRecordFinish(messageType);

		TimeShiftManager.getInstance().restoreToDefault(mStateSelf);
		return false;
	}
	public boolean stopPVRFromFileList() {
		Util.showDLog("state,stopPVRFromFileList()");

		// if (!(BGMManager.getInstance().getWakeupReason()==72)) {
		// BGMManager.getInstance().setWakeupReasonNormal();
		// }
//		if (isRunning()) {
//			TimeShiftManager.getInstance().showRecordFinish(
//					PvrConstant.RECORD_FINISH_AND_SAVE);
//		}
		clearPVRParams();
		
		getController().stopPvr(true);
		isSameSource = "";
		setRunning(false);
		mHandler.sendEmptyMessage(Clear_All_View);

		// TimeShiftManager.getInstance().restoreToDefault();
		return false;
	}

	/*
	 * 
	 */
	public void showSmallCtrlBar() {
		// TODO Auto-generated method stub
		//if (mShowDiskAttention) {
		//	getManager().showDiskAttention();
		//	mShowDiskAttention = false;
		//}
		if (mSmallCtrlBar != null) {
			if (!mSmallCtrlBar.isShowing()) {
				mSmallCtrlBar.setInfo("["
						+ Util.secondToString(mStateSelf.getRecordTimer())
						+ "]");
				Util.Hlog("1---------");
				mSmallCtrlBar.show();
			}
		} else {
			mSmallCtrlBar = new PVRTimerView(getActivity());
			mSmallCtrlBar.getContentView()
					.findViewById(R.id.pvr_rec_icon_small)
					.setVisibility(View.VISIBLE);

			mSmallCtrlBar.setOnDismissListener(new OnDismissListener() {
				@Override
				public void onDismiss() {
					if (isRunning()) {
						mHandler.sendEmptyMessage(SHOW_BIG_CTRL_BAR);
					}
				}
			});

			// mSmallCtrlBar.setDuration(Long.MAX_VALUE);
			mSmallCtrlBar.setDuration(MAX_TIMING * 1000L);
			mSmallCtrlBar.setInfo(Util.secondToString(mStateSelf
					.getRecordTimer()));
			mSmallCtrlBar.show();
		}

		// refreshTimer(); //refresh UI ASAP..
	}

	/*
	 * 
	 */
	public void showBigCtrlBar() {

		if (mBigCtrlBar != null) {
			mBigCtrlBar.show();
		} else {
			initBigCtrlBar();
			mBigCtrlBar.show();
		}
	}

	public void dissmissBigCtrlBar() {
		mBigCtrlBar.dismiss();
	}

	/*
	 * bigger=true? hidden bigger ctrl bar:hidden small ctrl bar.
	 */
	public void clearWindow(boolean bigger) {
		// TODO Auto-generated method stub
		if (bigger) {
			if (mBigCtrlBar != null && mBigCtrlBar.isShowing())
				mBigCtrlBar.dismiss();
		} else {
			if (mSmallCtrlBar != null && mSmallCtrlBar.isShowing())
				mSmallCtrlBar.dismiss();
		}
	}

	/*
	 * 
	 */
	public void clearAllWindow() {
		// TODO Auto-generated method stub
		clearWindow(true);
		clearWindow(false);
		getManager().getTvLogicManager().reSetZoomValues(getActivity());
		
		resetAllParams();
	}

	private void resetAllParams() {
		// TODO Auto-generated method stub
		recordTimeStr = "00:00:00";
		remainTimeStr = "00:00"; // Depend On Disk's free Size

		recordTimingLong = 30 * 60; // MAX time: 30 Minutes?
		schedulePvrTimeing=-1;
		
		recordTimingStr = "00:30:00";
		isSameSource = "";
	}
	
	public void setSchedulePVRDuration(int duration){
		Util.showDLog("setSchedulePVRDuration()");
		schedulePvrTimeing=duration;
	}

	/**
	 * 
	 */
	private void refreshTimer() {
		// TODO Auto-generated method stub
		mAllTimerToStr = new StringBuilder();

		mAllTimerToStr.append("[");

		recordTimeStr = Util.secondToString(mStateSelf.getRecordTimer());

		if(schedulePvrTimeing!=-1){
			recordTimingLong=schedulePvrTimeing;
			setRecordTimingStr(Util.secondToString(recordTimingLong));
			schedulePvrTimeing=-1;
		}
		
		if(mStateSelf.getRecordTimer()>=recordTimingLong){
			getManager().restoreToDefault(StatusType.PVR);
			return;
		}
		if (mSmallCtrlBar != null && mSmallCtrlBar.isShowing()) {
			mAllTimerToStr.append(recordTimeStr);
			mAllTimerToStr.append("]");

			mSmallCtrlBar.setInfo(mAllTimerToStr.toString());
		} else if (mBigCtrlBar != null && mBigCtrlBar.isShowing()) {

			remainTimeStr = countRemainSizeTime();

			mAllTimerToStr.append(recordTimeStr);
			mAllTimerToStr.append("/");
			// mAllTimerToStr.append(remainTimeStr);
			// mAllTimerToStr.append("/");
			mAllTimerToStr.append(getRecordTimingStr());
			mAllTimerToStr.append("]");

			mBRecordTimer.setText(mAllTimerToStr);
		}
		Date mDate = new Date(getManager().getBroadcastUTC()); // tv system UTC
		SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");
		mBTextViewDate.setText(dateFormat.format(mDate));
		
		rollBackUIState();
	}
	/**
	 * Double check UI items.
	 */
	private void rollBackUIState() {
		if(!isRunning()){
			return;
		}
		
		try {
			if ((!mBigCtrlBar.isShowing()) && (!mSmallCtrlBar.isShowing())) {
				showBigCtrlBar();
			}
		} catch (Exception e) {
			Util.showELog(e.toString());
		}
		if ((mBigCtrlBar != null && mBigCtrlBar.isShowing())
				&& (mSmallCtrlBar != null && mSmallCtrlBar.isShowing())) {
			mSmallCtrlBar.dismiss();
		}
	}

	public int getRecordTimer() {
		return recordTimer;
	}

	public void setRecordTimer(int recordTimer) {
		this.recordTimer = recordTimer;
	}

	public VideoManager getPlayback() {
		return playback;
	}

	public void setPlayback(VideoManager mPlayback) {
		this.playback = mPlayback;
	}

	/**
	 * @return the recordTimingStr
	 */
	public String getRecordTimingStr() {
		return recordTimingStr;
	}

	/**
	 * @param recordTimingStr
	 *            the recordTimingStr to set
	 */
	public void setRecordTimingStr(String recordTimingStr) {
		this.recordTimingStr = recordTimingStr;
	}

	/**
	 * @return
	 */
	private String countRemainSizeTime() {
		Long freeSize = getManager().getDiskFreesize(getManager().getPvrMP());
		int remainTime = (int) (freeSize / 1000 / getManager().getDiskSpeed() + 0.5f); // default:6.0
																						// MB/s

		return Util.secondToString(remainTime);
	}

	/**
	 * @return
	 */
	private boolean checkTimer() {

		int currentTime = Util.strToSecond(recordTimeStr);
		int TotalTime = Util.strToSecond(recordTimingStr);

		if (currentTime == TotalTime) {
			return true;
		}
		return false;
	}

	public Handler getStatePVRHandler() {
		return mHandler;
	}

	public String getPvrSource() {
		Util.showDLog("getPvrSource(),"+isSameSource);
		return isSameSource;
	}
	public boolean isRecording() {
		boolean running = false;
		if (mBigCtrlBar != null && mBigCtrlBar.isShowing()) {
			running = true;
		}

		if (mSmallCtrlBar != null && mSmallCtrlBar.isShowing()) {
			running = true;
		}

		running = running || isRunning();
		return running;
	}
	
	public void clearPVRParams(){
		this.setRunning(false);
		getController().stopPvr(true);
		if (mBigCtrlBar != null) {
			mBigCtrlBar.setOnDismissListener(null);
		}

		if (mSmallCtrlBar != null) {
			mSmallCtrlBar.setOnDismissListener(null);
		}
	}
}
