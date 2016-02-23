/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.manager;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.util.Log;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.widget.Toast;

import com.mediatek.dm.DeviceManagerEvent;
import com.mediatek.dm.MountPoint;
import com.mediatek.mmpcm.device.DevListener;
import com.mediatek.mmpcm.device.DevManager;
import com.mediatek.timeshift_pvr.controller.IStateInterface;
import com.mediatek.timeshift_pvr.controller.ScheduleItem;
import com.mediatek.timeshift_pvr.controller.StateBase;
import com.mediatek.timeshift_pvr.controller.StateDiskSetting;
import com.mediatek.timeshift_pvr.controller.StateFileList;
import com.mediatek.timeshift_pvr.controller.StateInitDisk;
import com.mediatek.timeshift_pvr.controller.StateNormal;
import com.mediatek.timeshift_pvr.controller.StatePVR;
import com.mediatek.timeshift_pvr.controller.StateScheduleList;
import com.mediatek.timeshift_pvr.controller.StateTimeShift;
import com.mediatek.timeshift_pvr.controller.StatusType;
import com.mediatek.timeshift_pvr.controller.TVLogicManager;
import com.mediatek.timeshift_pvr.controller.UImanager;
import com.mediatek.timeshift_pvr.ui.PvrDialog;
import com.mediatek.tvcm.TVInputManager;
import com.mediatek.tvcm.TVOutput;
import com.mediatek.tvcm.TVTimerManager;
import com.mediatek.tvcommon.TVChannel;
import com.mediatek.ui.R;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.menu.util.MenuConfigManager;
import com.mediatek.ui.menu.util.SaveValue;
import com.mediatek.ui.nav.NavIntegration;
import com.mediatek.ui.nav.NavIntegration.IChannelSelectorListener;
import com.mediatek.ui.util.GetCurrentTask;
import com.mediatek.ui.util.KeyMap;
import com.mediatek.ui.util.MtkLog;
import com.mediatek.ui.util.ScreenConstant;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Stack;

/**
 * manager all window/dialog called by TimeShift&PVR,
 * <p>
 * manager all operation of TimeShift&PVR
 */
public class TimeShiftManager implements DevListener,
		IManagerInterface<StateBase> {

	private Activity mTKUIActivity;
	private Context mContext;

	private SaveValue saveValue;

	private IStateInterface mCurrentState;
	private Stack<StateBase> mStates = new Stack<StateBase>();

	private UImanager uiManager;
	private TVLogicManager mTVLogicManager;
	private Controller mController;

	private int tvWidth = 1920;
	private int tvHeight = 1080;

	private MountPoint pvrDiskMountPoint = null;
	public static boolean isFormating = false;

	private static final int DISK_NOT_READY = 0;
	private static final int CREATE_FAIL = 1;
	private static final int CREATE_SUCCESS = 2;
	private static final int FEATURE_NOT_AVAIABLE = 311;
	private static final int DISK_ATTENTION = 4;
	private static final int RECORD_FINISHED = 5;
	private static final int DISK_IS_FULL = 6;
	private static final int FEATURE_NOT_SUPPORT = 1001;
	public static final int Channel_NOT_Support = 8;
	public static final int CHANGE_CHANNEL = 2002;
	public static final int ALLOW_SYSTEM_SUSPEND = 2003;
	public static final int UNMOUNT_EVENT = 2004;
	public static final String UNMOUNT_EVENT_MSG_KEY = "UNMOUNT_EVENT_MSG_KEY";

	private static final int SCHEDULE_PVR_TASK = 7;
	private static final int SCHEDULE_PVR_TASK_STOP_TIME = 0x2008;

	public static final int DEFAULT = 0x2009;

	private ScheduleItem mTempItem;

	public static TimeShiftManager staticManager = null;
	public static final String SCHEDULE_PVR_SRCTYPE = "SCHEDULE_PVR_SRCTYPE";
	public static final String SCHEDULE_PVR_CHANNELLIST = "SCHEDULE_PVR_CHANNELLIST";
	public static final String SCHEDULE_PVR_REMINDER_TYPE = "SCHEDULE_PVR_REMINDER_TYPE";
	public static final String SCHEDULE_PVR_REPEAT_TYPE = "SCHEDULE_PVR_REPEAT_TYPE";
	public static final String AUTO_SYNC = "SETUP_auto_syn";
	public static final String TIME_DATE = "SETUP_date";
	public static final String TIME_TIME = "SETUP_time";
	public static final int INVALID_VALUE = 10004;
	public static final int STEP_VALUE = 1;
	public static final String TIMER_powerOnTime = "powerOnTime";

	public static final String DTV_TSHIFT_OPTION = "DTV_TSHIFT_OPTION";
	public static final String DTV_DEVICE_INFO = "DTV_DEVICE_INFO";
	/* The status for the timeshifting status. */
	/* pv_ntfy_data = (NAV_REC_TSHIFT_STRG_STATUS_T*) */
	public static final int NAV_REC_TSHIFT_NTFY_STATUS_CHANGE = 0;
	public static final int NAV_REC_TSHIFT_NTFY_STRG_STATUS_CHANGE = 1;
	public static final int NAV_REC_TSHIFT_NTFY_ANALYSIS_BEGIN = 2;
	public static final int NAV_REC_TSHIFT_NTFY_ANALYSIS_PROGRESS = 3;
	public static final int NAV_REC_TSHIFT_NTFY_ANALYSIS_OK = 4;
	public static final int NAV_REC_TSHIFT_NTFY_ANALYSIS_FAIL = 5;
	public static final int NAV_REC_TSHIFT_NTFY_CREATE_FILE_BEGIN = 6;
	public static final int NAV_REC_TSHIFT_NTFY_CREATE_FILE_PROGRESS = 7;
	public static final int NAV_REC_TSHIFT_NTFY_CREATE_FILE_OK = 8;
	public static final int NAV_REC_TSHIFT_NTFY_CREATE_FILE_FAIL = 9;
	public static final int NAV_REC_TSHIFT_NTFY_SPEED_TEST_BEGIN = 10;
	public static final int NAV_REC_TSHIFT_NTFY_SPEED_TEST_PROGRESS = 11;
	public static final int NAV_REC_TSHIFT_NTFY_SPEED_TEST_LOW = 12;
	public static final int NAV_REC_TSHIFT_NTFY_SPEED_TEST_OK = 13;
	public static final int NAV_REC_TSHIFT_NTFY_SPEED_TEST_FAIL = 14;
	public static final int NAV_REC_TSHIFT_NTFY_REG_FINISHED = 15;
	public static final int NAV_REC_TIMESHIFT_NTFY_SPEED_CHANGE = 16;

	/* Last entry */
	public static final int NAV_REC_TSHIFT_NTFY_LAST_ENTRY = 16;

	public static final int PVR_TIMESHIFT_REQUESTCODE = 1001;
	public static final int PVR_TIMESHIFT_DEVICE_INFO = 1002;
	public static final int PVR_TIMESHIFT_SCHEDULE_INFO = 1003;

	public static final int SPECIAL_CHANNEL_LOCK = 0x2002;
	public static final int SPECIAL_PROGRAM_LOCK = 0x2003;
	public static final int SPECIAL_INPUT_LOCK = 0x2004;

	public static class MyHandler extends Handler {
		WeakReference<Activity> mActivity;

		MyHandler(Activity activity) {
			mActivity = new WeakReference<Activity>(activity); // reserve.
		}

		public void handleMessage(Message msg) {
			switch (msg.what) {
			case DISK_NOT_READY:

				break;

			case CREATE_FAIL:
				staticManager.showCreateFileSuccessUI(false);
				break;

			case CREATE_SUCCESS:
				staticManager.showCreateFileSuccessUI(true);
				break;
			case FEATURE_NOT_SUPPORT:
				staticManager.uiManager.showInfoBar(staticManager.getActivity()
						.getString(R.string.feature_not_support));
				break;
			case FEATURE_NOT_AVAIABLE:
				String errorMessage = "Feature Not Support !";

				staticManager.uiManager.showInfoBar(errorMessage);// ;(staticManager.getActivity().getString(R.string.feature_not_support));
				break;
			case DISK_ATTENTION:
				staticManager.uiManager.showInfoBar(staticManager.getActivity()
						.getString(R.string.attention_unplug_device));
				break;

			case RECORD_FINISHED:
				break;

			case DISK_IS_FULL:
				staticManager.uiManager.showInfoBar(staticManager.getActivity()
						.getString(R.string.not_enough_space));
				break;

			case SCHEDULE_PVR_TASK:
				staticManager.prepareSchedulePvrTask(msg.arg1);
				break;
			case SCHEDULE_PVR_TASK_STOP_TIME:
				staticManager.setPVRStopTime(msg.getData().getLong("Duration"));
				break;

			case PvrConstant.RECORD_FINISH_AND_SAVE:
				staticManager.uiManager.showInfoBar(staticManager.getActivity()
						.getString(R.string.pvr_save_record));
				if (!GetCurrentTask.getInstance(staticManager.getActivity())
						.isCurActivityTkuiMainActivity()) {
					Toast.makeText(
							staticManager.getActivity(),
							staticManager.getActivity().getString(
									R.string.pvr_save_record),
							Toast.LENGTH_SHORT).show();
				}
				break;

			case PvrConstant.Disk_Disconnect:
				staticManager.uiManager.showInfoBar(staticManager.getActivity()
						.getString(R.string.disk_disconnect));
				break;
			case PvrConstant.Dissmiss_Info_Bar:
				if (staticManager.uiManager != null) {
					staticManager.uiManager.dissmiss();
				}

				break;
			case UNMOUNT_EVENT:
				staticManager.unmountEvent(msg.getData().getString(
						UNMOUNT_EVENT_MSG_KEY));

			case SPECIAL_CHANNEL_LOCK:
				staticManager.channelBlocked();
				break;
			case SPECIAL_PROGRAM_LOCK:
				staticManager.channelBlocked();
				break;
			case SPECIAL_INPUT_LOCK:
				staticManager.inpuSrcBlocked();
				break;
			case DEFAULT:
			default:
				break;
			}
			super.handleMessage(msg);
		}
	}

	private MyHandler topHandler;

	public static TimeShiftManager getInstance(Activity activity) {
		if (staticManager == null) {
			staticManager = new TimeShiftManager(activity);
		}
		// staticManager.setActivity(activity);
		return staticManager;
	}

	public static TimeShiftManager getInstance() {
		if (staticManager != null) {
			return staticManager;
		} else {
			Util.Hlog("TimeShiftManager: getInstance, return null;");
			return null;
		}

	}

	public TimeShiftManager(Activity activity) {
		super();
		isFormating = false;
		this.mTKUIActivity = activity;
		this.staticManager = this;
		setTopHandler(new MyHandler(mTKUIActivity));

		this.mContext = mTKUIActivity;
		this.saveValue = SaveValue.getInstance(activity);

		WindowManager windowManager = (WindowManager) mContext
				.getSystemService(Context.WINDOW_SERVICE);

		tvWidth = ScreenConstant.SCREEN_WIDTH;// windowManager.getDefaultDisplay().getRawWidth();
		tvHeight = ScreenConstant.SCREEN_HEIGHT;// windowManager.getDefaultDisplay().getRawHeight();

		uiManager = new UImanager(activity);
		setmTVLogicManager(TVLogicManager.getInstance(mContext, this));
		setmController(new Controller(this));

		setState(setDefaultState());

		// add device listener
		DevManager.getInstance().addDevListener(this);

		// check default pvr disk:
		if (!checkPvrMP()) {
			setPvrMP(null);
		}

		registeListeners();
	}

	private StateBase setDefaultState() {

		return new StateNormal(mTKUIActivity, this);
		// return new StateDiskSetting(mContext, this);
		// return new StateInitDisk(mContext, this);
		// return new StateTimeShift(mContext, this);
		// return new StatePVR(mContext, this);
	}

	private IStateInterface debuggingCase() {

		// return new StateNormal(mContext, this);
		// return new StateDiskSetting(mContext, this);
		// return new StateInitDisk(mContext, this);
		return new StateTimeShift(mContext, this);
		// return new StatePVR(mContext, this);
		// return new StateScheduleList(mContext, this);
		// return new StateFileList(mContext, this);
	}

	/**
	 * @return
	 */
	private boolean isPIPPOPMode() {
		// TODO Auto-generated method stub
		return false;
	}

	public UImanager getUiManager() {
		return uiManager;
	}

	public void setUiManager(UImanager uiManager) {
		this.uiManager = uiManager;
	}

	/*
	 * 
	 */
	public boolean diskIsReady() {
		if (hasRemovableDisk()) {
			// ArrayList<MountPoint> diskList = DevManager.getInstance()
			// .getMountList();
			//
			// File tagFile;
			// for (int i = 0; i < diskList.size(); i++) {
			//
			// tagFile = new
			// File(diskList.get(i).mMountPoint+Core.Temp_PVR_DISK_TAG);
			//
			// if (tagFile.exists()) {
			// setPvrDisk(diskList.get(i));
			// return true;
			// }
			// }
			return true;
		}
		return false;
	}

	/*
	 * 
	 */
	public boolean timeShiftIsEnable() {
		// Disable tshift feature on CN.
		if (!enableTshiftModule()) {
			return false;
		}
		return getSaveValue().readValue(MenuConfigManager.DTV_TSHIFT_OPTION) == 1 ? true
				: false;
	}

	public static boolean enableTshiftModule() {
		int enable = SystemProperties.getInt("mtk.tkui.tshift", 0);
		if (enable == 1) {
			return true;
		}
		return false;
	}

	/*
	 * 
	 */
	public boolean isSupportTimeShift() {
		return true;
	}

	public boolean onKeyDown(int keyCode) {
		Util.showDLog("TShiftManager get keycode: " + keyCode);
		if (keyCode == 0 && keyCode == KeyMap.KEYCODE_MTKIR_FREEZE) {
			return false;
		}

		return statesOnKeyDown(keyCode);
	}

	private boolean statesOnKeyDown(int keyCode) {
		dumpStatesStack();

		Iterator<StateBase> statesInterator = mStates.iterator();
		boolean handled = false;
		StateBase state;
		for (int i = mStates.size() - 1; i >= 0; i--) {
			state = mStates.get(i);
			handled = state.onKeyDown(keyCode);

			if (handled) {
				Util.showDLog("stateOnKeyDown()," + state.getType().name()
						+ ",Handled?," + handled);
				return true;
			}
		}

		return false;
	}

	public PvrDialog showPVRDialog(int type) {
		PvrDialog pvrDialog = new PvrDialog(mTKUIActivity, PvrDialog.TYPE_BGM);
		MtkLog.e("onKeyDown", "onKeyDown:timeshiftmanager:in");
		pvrDialog.show();
		return pvrDialog;
	}

	/*
	 * 
	 */
	public void onErrorHappened(int errorCode) {
		Util.showELog("onErrorHappened:" + errorCode);
		switch (errorCode) {
		case ErrorCode.CALL_CM_ERROR:
			if (StatePVR.getInstance() != null) {
				StatePVR.getInstance().clearPVRParams();
			}
			restoreAllToNormal();
			break;
		case ErrorCode.NOFUNCTION:

			break;
		case ErrorCode.DISK_NOT_HAVE_ENOUGH_SPACE:

			break;
		default:
			break;
		}

	}

	public IStateInterface getState() {
		return mCurrentState;
	}

	public StateBase getState(StatusType type) {
		Iterator<StateBase> statesIter = mStates.iterator();
		do {
			StateBase state = statesIter.next();
			if (state.getType() == type) {
				return state;
			}
		} while (statesIter.hasNext());

		return null;
	}

	public boolean setState(StateBase state) {
		boolean pushSuccess = false;
		if (state != null) {
			pushSuccess = putState(state);

			if (pushSuccess) {
				this.mCurrentState = state;
			}
		}
		return pushSuccess;
	}

	private boolean putState(StateBase state) {
		// TODO Auto-generated method stub
		boolean pushSuccess = false;

		int index = mStates.indexOf(state);
		if (index == -1) {
			pushSuccess = true;
		} else {
			pushSuccess = false;
			mStates.remove(index);
		}
		mStates.push(state);
		return pushSuccess;
	}

	/*
	 * 
	 */
	public void onResume() {
		// TODO Auto-generated method stub
		Iterator<StateBase> states = mStates.iterator();

		do {
			states.next().onResume();
		} while (states.hasNext());
	}

	/*
	 * 
	 */
	public void onPause() {
		// TODO Auto-generated method stub
		Iterator<StateBase> states = mStates.iterator();

		do {
			states.next().onPause();
		} while (states.hasNext());
	}

	/*
	 * if TV stopped,must clear all items relate to states expect Normal.
	 */
	public void onStop() {
		Util.showDLog("TimeShiftManager,onStop()");

		getState().onStop();
		// setState(setDefaultState());

		restoreAllToNormal();

		unregisteListeners();
	}

	/*
	 * 
	 */
	public void registListener(Context context) {
		// registerUSBListener();
		// registerOtherListener();

	}

	public int getTVHeight() {
		return tvHeight;
	}

	public int getTVWidth() {
		return tvWidth;
	}

	public void showDiskNotReady() {
		getActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				uiManager.showInfoBar(mContext
						.getString(R.string.disk_not_ready));
			}
		});
	}

	public void showPvrIsRecording() {
		getActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				uiManager.showInfoBar(mContext
						.getString(R.string.state_pvr_recording));
			}
		});
	}

	public void showCreateFileSuccess(boolean success) {

		if (success) {
			getTopHandler().sendEmptyMessage(CREATE_SUCCESS);
		} else {
			getTopHandler().sendEmptyMessage(CREATE_FAIL);
		}

		tshiftCreateFinished();
	}

	private void showCreateFileSuccessUI(boolean success) {

		String str = "";
		if (success) {
			str = mContext.getString(R.string.create_tshift_file_success);
		} else {
			str = mContext.getString(R.string.create_tshift_file_fail);
		}
		Toast.makeText(getActivity(), str, Toast.LENGTH_LONG).show();
		uiManager.showInfoBar(str);
	}

	public void showFeatureNotAvaiable() {

		topHandler.sendEmptyMessage(FEATURE_NOT_AVAIABLE);
		// restoreToDefault();
	}

	public void showFeatureNotSupport() {
		topHandler.sendEmptyMessage(FEATURE_NOT_SUPPORT);
	}

	public void showDiskAttention() {
		topHandler.sendEmptyMessage(DISK_ATTENTION);
	}

	public void showRecordFinish(int messageType) {

		topHandler.removeMessages(messageType);
		topHandler.sendEmptyMessageDelayed(messageType, 500);
	}

	public void showDiskNotEnoughSpace() {
		topHandler.sendEmptyMessage(DISK_IS_FULL);
		restoreToDefault(StatusType.PVR, StatusType.TIMESHIFT);

	}

	/**
	 * Only for StateInitDisk.
	 */
	public void tshiftCreateFinished() {
		if (getState() instanceof StateInitDisk) {
			((StateInitDisk) getState()).enableNext();
		}
	}

	public void tshiftCreateProgress(int progress) {
		if (getState() instanceof StateInitDisk) {
			((StateInitDisk) getState()).setProgressMessage(progress);

		}
	}

	public void tshifSpeedProgress(int progress) {
		if (getState() instanceof StateInitDisk) {
			((StateInitDisk) getState()).setProgressMessage(progress);

		}
	}

	/*
	 * 
	 */
	public boolean isDTVSource() {
		return getTvLogicManager().isDTV();
	}

	/*
	 * 
	 */
	public boolean hasDTVSignal() {
		return getTvLogicManager().hasDTVSignal();
	}

	public Activity getActivity() {
		return mTKUIActivity;
	}

	public Activity setActivity(Activity activity) {
		return mTKUIActivity = activity;
	}

	public SaveValue getSaveValue() {
		if (saveValue == null) {
			saveValue = SaveValue.getInstance(getActivity());
		}
		return saveValue;
	}

	public void setSaveValue(SaveValue saveValue) {
		this.saveValue = saveValue;
	}

	public ArrayList<MountPoint> getDeviceList() {

		return DevManager.getInstance().getMountList();
	}

	/**
	 * @param selectedDisk
	 * @return unit:KB
	 */
	public Long getDiskFreesize(int selectedDisk) {
		return DevManager.getInstance().getMountList().get(selectedDisk).mFreeSize;
	}

	/**
	 * @param selectedDisk
	 * @return unit:KB
	 */
	public Long getDiskFreesize(MountPoint mp) {
		if (mp == null) {
			return 0L;
		}
		return mp.mFreeSize;
	}

	public String[] getSizeList(boolean auto, Long size) {
		return Util.covertFreeSizeToArray(auto, size);
	}

	/*
	 * 
	 */
	public boolean isPipPopMode() {
		return getTvLogicManager().isPipPopMode();
	}

	/*
	 * 
	 */
	public boolean isScanning() {
		return getTvLogicManager().isScanning();
	}

	/**
	 * @return Pvr/Tshift Controller.
	 */
	public Controller getController() {
		return mController;
	}

	/**
	 * @param mController
	 *            the mController to set
	 */
	public void setmController(Controller mController) {
		this.mController = mController;
	}

	/**
	 * @return the mTVLogicManager
	 */
	public TVLogicManager getTvLogicManager() {
		return mTVLogicManager;
	}

	/**
	 * @param mTVLogicManager
	 *            the mTVLogicManager to set
	 */
	public void setmTVLogicManager(TVLogicManager mTVLogicManager) {
		this.mTVLogicManager = mTVLogicManager;
	}

	/*
	 * 
	 */
	public boolean hasRemovableDisk() {
		if (getDeviceList().size() > 0) {
			return true;
		} else {
			return false;
		}
	}

	/**
	 * @return the pvrDisk
	 */
	public MountPoint getRunningPvrMP() {
		return pvrDiskMountPoint;
	}

	/**
	 * @return the pvrDisk
	 */
	public MountPoint getPvrMP() {
		if (pvrDiskMountPoint == null) {
			setDefaultPvrMP();
		}
		return pvrDiskMountPoint;
	}

	/**
	 * @param pvrDisk
	 *            the pvrDisk to set
	 */
	public void setRunningPvrMP(MountPoint pvrDisk) {

		this.pvrDiskMountPoint = pvrDisk;
		if (pvrDisk == null) {
			getSaveValue().saveStrValue(Core.PVR_DISK, "");
		} else {
			Util.showDLog("setRunningPvrMP(),mp," + pvrDisk.mMountPoint);
		}
	}

	/**
	 * @param pvrDisk
	 *            the pvrDisk to set
	 */
	public void setPvrMP(MountPoint pvrDisk) {
		if (pvrDisk != null) {
			Util.showDLog("setPvrMP(),mp," + pvrDisk.mMountPoint);
			this.pvrDiskMountPoint = pvrDisk;
			getSaveValue().saveStrValue(Core.PVR_DISK, pvrDisk.mMountPoint);
		}
	}

	/**
	 */
	public boolean setDefaultPvrMP() {
		Util.showDLog("setDefaultPvrMP()");

		Util.printStackTrace();
		if (getDeviceList().size() < 1) {
			return false;
		} else {
			setPvrMP(getDeviceList().get(0));
		}
		return true;
	}

	public boolean checkPvrMP() {
		if (getDeviceList().size() >= 1) {
			String mp = getSaveValue().readStrValue(Core.PVR_DISK);
			Util.showDLog("checkPvrMP(),readStrValue MP," + mp);
			ArrayList<MountPoint> list = getDeviceList();

			for (int i = 0; i < list.size(); i++) {
				if (mp.equalsIgnoreCase(list.get(i).mMountPoint)) {
					pvrDiskMountPoint = list.get(i);

					Util.tempSetPVR(pvrDiskMountPoint.mMountPoint);
					Util.showDLog("checkPvrMP()-1,MP," + pvrDiskMountPoint);
					return true;
				}
			}

			pvrDiskMountPoint = getDeviceList().get(0);
			setPvrMP(pvrDiskMountPoint);
			Util.tempSetPVR(pvrDiskMountPoint.mMountPoint);
		}
		Util.showDLog("checkPvrMP()-2,MP," + pvrDiskMountPoint);
		return false;
	}

	/*
	 * 
	 */
	public boolean dtvNotReady() {

		boolean notReady = getTvLogicManager().dtvNotReadyForRecord()
				|| (!isSupportTimeShift());
		// boolean isReady = !isDTVSource() || (!isSupportTimeShift())
		// || (!getmTVLogicManager().dtvHasSignal())
		// || (!getmTVLogicManager().hasDTVSignal())
		// || (getmTVLogicManager().dtvIsScrambled());

		return notReady;
	}

	public void restoreToDefault(StateBase state) {
		restoreToDefault(state.getType());
	}

	public void restoreToDefault(StatusType... type) {
		Util.showDLog("restoreToDefault(...)");
		Util.printStackTrace();

		List<StatusType> typeList = Arrays.asList(type);

		StateBase state;
		for (int i = mStates.size() - 1; i >= 0; i--) {
			state = mStates.get(i);

			if (typeList.contains(state.getType())) {
				Util.showDLog("restoreToNormal(...),Name:"
						+ state.getType().name());
				mStates.remove(state);
				state.onRelease();
			}
		}
	}

	public void restoreAllToNormal() {
		do {
			StateBase state = mStates.lastElement();
			if (state.getType() != StatusType.NORMAL) {
				Util.showDLog("Name:" + state.getType().name());

				state.onRelease();
				mStates.remove(state);
			}
		} while (mStates.size() > 1);
	}

	public void restoreAllAfterException() {
		if (getController() != null) {
			getController().stopPvr(false);
			getController().stopPlayPvr();
			getController().stopPvr(true);
		}
	}

	/**
	 * @return tshift file's size,unit:KB
	 */
	public int getTshiftFileSize() {
		int size = getSaveValue().readValue(Core.TIMESHIFT_SIZE);
		return size;
	}

	/**
	 * @param size
	 *            unit:kb
	 */
	public void setTshiftFileSize(int size) {
		getSaveValue().saveValue(Core.TIMESHIFT_SIZE, size);
	}

	/*
	 * 
	 */
	public boolean hasFreeSize(int keycode) {
		if (hasRemovableDisk()) {
			if (getPvrMP() == null) {
				setDefaultPvrMP();
			}
		}

		if (keycode == KeyEvent.KEYCODE_MTKIR_FREEZE) {
			return true; // Don't check freeSize,
		}

		if (getDiskFreesize(getPvrMP()) >= Core.DEFAULT_DISK_FREE_SIZE) {
			return true;
		} else {
			return false;
		}
	}

	/*
	 * 
	 */
	@Override
	public void onEvent(DeviceManagerEvent event) {
		Util.showDLog("DeviceManagerEvent: " + event.getType());
		switch (event.getType()) {
		case DeviceManagerEvent.connected:

			break;
		case DeviceManagerEvent.disconnected:

			break;
		case DeviceManagerEvent.isomountfailed:

			break;
		case DeviceManagerEvent.mounted:
			if (null != StateDiskSetting.getInstance()) {
				if (StateDiskSetting.getInstance().getDialog().isShowing()) {
					StateDiskSetting.getInstance().getHandler()
							.sendEmptyMessage(1226);
				}
			}
			break;
		case DeviceManagerEvent.umounted:
			if (isFormating) {
				return;
			}

			Util.showDLog("DeviceManagerEvent.umounted");

			Message msg = staticManager.getTopHandler().obtainMessage();
			msg.what = UNMOUNT_EVENT;

			Bundle bundle = new Bundle();
			bundle.putString(UNMOUNT_EVENT_MSG_KEY, event.getMountPointPath());
			msg.setData(bundle);

			staticManager.getTopHandler().sendMessage(msg);
			break;
		case DeviceManagerEvent.unsupported:

			break;
		}

	}

	public void stopAllRunningWhenNoSignal() {
		// setPvrMP(null);
				MtkLog.e("stopAllRunning", "mCurrentState:"
						+ mCurrentState.getClass().getSimpleName());
				if (null != mCurrentState
						&& "StateNormal".equals(mCurrentState.getClass()
								.getSimpleName())) {

				} else {

					try {
						if (StateTimeShift.getInstance() != null
								&& StateTimeShift.getInstance().isRunning()) {
							StateTimeShift.getInstance().stopTimeShift();
							// getController().stopPlayPvr();
							// showRecordFinish(PvrConstant.Disk_Disconnect);
							restoreToDefault(StateTimeShift.getInstance());
						}
						
						/*if (StatePVR.getInstance() != null
								&& StatePVR.getInstance().isRunning()) {
							StatePVR.getInstance().stopPVRRecord(
									PvrConstant.RECORD_FINISH_AND_SAVE);
							restoreToDefault(StatePVR.getInstance());
						}*/
						
						if (StateFileList.getInstance() != null
								&& StateFileList.getInstance().isRunning()) {
							StateFileList.getInstance().stopPvrPlayer();
							restoreToDefault(StateFileList.getInstance());
						}
						if (StateFileList.getInstance() != null
								&& StateFileList.getInstance().isShowing()) {
							restoreToDefault(StateFileList.getInstance());
						}
					} catch (Exception e) {
						// TODO Auto-generated catch block
						Util.showELog(e.toString());
						e.printStackTrace();
					}
					if (null != uiManager) {
						uiManager.dissmiss();
					}
				}
		
	}
	
	public void stopAllRunning() {
		// setPvrMP(null);
		MtkLog.e("stopAllRunning", "mCurrentState:"
				+ mCurrentState.getClass().getSimpleName());
		if (null != mCurrentState
				&& "StateNormal".equals(mCurrentState.getClass()
						.getSimpleName())) {

		} else {

			try {
				if (StateTimeShift.getInstance() != null
						&& StateTimeShift.getInstance().isRunning()) {
					StateTimeShift.getInstance().stopTimeShift();
					// getController().stopPlayPvr();
					// showRecordFinish(PvrConstant.Disk_Disconnect);
					restoreToDefault(StateTimeShift.getInstance());
				}
				if (StatePVR.getInstance() != null
						&& StatePVR.getInstance().isRunning()) {
					StatePVR.getInstance().stopPVRRecord(
							PvrConstant.RECORD_FINISH_AND_SAVE);
					restoreToDefault(StatePVR.getInstance());
				}
				if (StateFileList.getInstance() != null
						&& StateFileList.getInstance().isRunning()) {
					StateFileList.getInstance().stopPvrPlayer();
					restoreToDefault(StateFileList.getInstance());
				}
				if (StateFileList.getInstance() != null
						&& StateFileList.getInstance().isShowing()) {
					restoreToDefault(StateFileList.getInstance());
				}
			} catch (Exception e) {
				// TODO Auto-generated catch block
				Util.showELog(e.toString());
				e.printStackTrace();
			}
			if (null != uiManager) {
				uiManager.dissmiss();
			}
		}
	}

	/*
	 * 
	 */
	public boolean isPvrDisk(MountPoint mountPoint) {
		// 1.check saveValue's mountpoint
		// 2.hasTshiftFile(mp)
		return true;
	}

	/*
	 * check the default tshift file in default pvrDisk.
	 */
	public boolean hasTshiftFile() {
		return getController().hasTshiftFile();
	}

	/**
	 * @return the topHandler
	 */
	public MyHandler getTopHandler() {
		return topHandler;
	}

	/**
	 * @param topHandler
	 *            the topHandler to set
	 */
	public void setTopHandler(MyHandler topHandler) {
		this.topHandler = topHandler;
	}

	/*
	 * 
	 */
	public void setDiskSpeed(float speed) {
		getSaveValue().saveValue(Core.DISK_SPEED, speed);

	}

	/*
	 * 
	 */
	public float getDiskSpeed() {
		float size = getSaveValue().readFloatValue(Core.DISK_SPEED);
		if (size == 0f) {
			size = Core.DEFAULT_DISK_SPEED;
		}
		return size;
	}

	public void startSchedulePvrTask(ScheduleItem item) {

		Message msg = getTopHandler().obtainMessage();
		msg.arg1 = item.getTaskID();
		msg.what = SCHEDULE_PVR_TASK;

		getTopHandler().sendMessageDelayed(msg, 50);
	}

	public void prepareSchedulePvrTask(int taskID) {

		/*
		 * 1.switch source 2.set tv channel 3.check state 4.perform virtul key
		 * to ap
		 */
		Util.showDLog("prepareSchedulePvrTask()");
		Util.showDLog("hashCode-2:" + TimeShiftManager.getInstance().hashCode());
		ScheduleItem item = Util.getScheduleItem(mContext, taskID);
		boolean isOK = getTvLogicManager().prepareScheduleTask(item);

		if (isOK) {
			Long endTime = item.getEndTime().getTime();
			Long startTime = item.getStartTime().getTime();

			Message msg = getTopHandler().obtainMessage();
			Bundle bundle = new Bundle();
			bundle.putLong("Duration", endTime - startTime);
			msg.setData(bundle);
			msg.what = SCHEDULE_PVR_TASK_STOP_TIME;

			getTopHandler().sendMessageDelayed(msg, 2 * 1000);
			onKeyDown(KeyEvent.KEYCODE_MTKIR_RECORD);
		} else {
			showFeatureNotAvaiable();
		}
	}

	public void setPVRStopTime(Long duration) {
		Util.showDLog("setPVRStopTime()");

		if (pvrIsRecording()) {
			Long seconds = (duration / 1000);
			int durationInt = new Long(seconds).intValue();
			if (StatePVR.getInstance() != null) {
				StatePVR.getInstance().setSchedulePVRDuration(durationInt);
			}
			Util.showDLog("setPVRStopTime()," + duration);
		} else {
			Util.showDLog("setPVRStopTime()," + "pvrIsRecording()? No");
		}
	}

	/**
	 * @return the mTempItem
	 */
	public void setValueToPrefer(String itemID, int value) {
		getSaveValue().saveValue(itemID, value);
		if (null != TimeShiftManager.getInstance() & value == 0) {
			TimeShiftManager.getInstance().stopAllRunning();
		}
	}

	public ScheduleItem getTempItem() {
		return mTempItem;
	}

	/**
	 * @param intent
	 */
	public void startScheduleAlarmPvr(Intent intent, Context context) {
		int taskID = intent.getIntExtra(Core.ALARM_ACTION_TAG, -1);

		Log.d("TimeShift_PVR", "onNewIntent(),TaskID: " + taskID);

		if (taskID != -1) {
			ScheduleItem item = Util.getScheduleItem(context, taskID);
			if (item != null) {
				item.showDebugInfo();
			}
			TimeShiftManager.getInstance().restoreAllToNormal();

			TimeShiftManager.getInstance().startSchedulePvrTask(item);
			Util.showDLog("hashCode-1:"
					+ TimeShiftManager.getInstance().hashCode());
		}
	}

	public void startSchedulePvr(Intent data) {
		ScheduleItem item = new ScheduleItem();
		item.setChannelName("-1");

		long startTime = data.getLongExtra(ScheduleItem.START_TIME, -1L);
		if (startTime != -1L) {
			item.setStartTime(new Date(startTime));
		}

		long endTime = data.getLongExtra(ScheduleItem.END_TIME, -1L);
		if (endTime != -1L) {
			item.setEndTime(new Date(endTime));
		}

		item.setNewItem(true);
		if (data.getBooleanExtra("schedule_item_null", false)) {
			item = null;
		}

		setState(new StateScheduleList(mContext,
				TimeShiftManager.getInstance(), item));
	}

	private boolean enableDeviceInfo() {
		if (tshiftIsRunning()) {
			// if(CommonIntegration.getInstance().isCurrentSourceATV()&&pvrIsRecording()){
			//
			// }

		}
		return true;
	}

	public void setmTempItem(ScheduleItem mTempItem) {
		this.mTempItem = mTempItem;
	}

	public boolean tshiftIsRunning() {
		if (staticManager == null) {
			return false;
		}

		if (StatePVR.getInstance() != null) {
			if (StatePVR.getInstance().isRunning()) {
				return true;
			}
		}

		if (StateFileList.getInstance() != null) {
			if (StateFileList.getInstance().isRunning()
					|| StateFileList.getInstance().isPlaying()) {
				return true;
			}
		}

		if (StateTimeShift.getInstance() != null) {
			if (StateTimeShift.getInstance().isRunning()) {
				return true;
			}
		}
		return false;
	}

	public boolean pvrIsPlaying() {
		if (staticManager == null) {
			return false;
		}

		if (StateFileList.getInstance() != null) {
			if (StateFileList.getInstance().isPlaying()) {
				return true;
			}
		}

		return false;
	}

	/*
	 * Get result by isRunning state,It's simple,but the result may be wrong.
	 */
	public boolean pvrIsRunning() {
		if (staticManager == null) {
			return false;
		}

		if (StatePVR.getInstance() != null) {
			if (StatePVR.getInstance().isRunning()) {
				return true;
			}
		}

		return false;
	}

	/**
	 * Get result by UI visible or not.
	 * 
	 * @return
	 */
	public boolean pvrIsRecording() {
		if (staticManager == null) {
			return false;
		}

		if (StatePVR.getInstance() != null) {
			if (StatePVR.getInstance().isRecording()) {
				return true;
			}
		}

		return false;
	}

	/**
	 * Fix CR: DTV00584243,Restore system to default.
	 */
	public static void forceSetToDefault() {

		try {
			if (staticManager == null) {
				return;
			}
			staticManager.restoreAllToNormal();
		} catch (Exception e) {
			// TODO: handle exception
		} catch (Error e) {
			// TODO: handle exception
		}

		removeSchedulePVRDB();
	}

	private static void removeSchedulePVRDB() {
		// TODO Auto-generated method stub
		staticManager.getActivity().runOnUiThread(new Runnable() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
				StateScheduleList.deleteAllItems(staticManager.getActivity());
			}
		});
	}

	public static boolean scheduleListIsRunning() {
		try {
			if (staticManager == null) {
				return false;
			}
			if ((staticManager.getState() instanceof StateScheduleList)) {
				return true;
			}
		} catch (Exception e) {
			// TODO: handle exception
		} catch (Error e) {
			// TODO: handle exception
		}

		return false;
	}

	private boolean isShowDiskNotReady() {
		// TODO Auto-generated method stub

		if (mStates.size() == 1) {
			return false;
		}

		StateBase state;
		for (int i = mStates.size() - 1; i > 0; i--) {
			state = mStates.get(i);

			if (state.getType() == StatusType.DISKSETTING) {
				return false;
			}
		}

		return true;
	}

	private void unmountEvent(String mountPointPath) {

		if (isShowDiskNotReady()) {
			showDiskNotReady(); // fix CR: DTV00584030,DTV00584244
		}

		if (null != StateDiskSetting.getInstance()) {
			if (StateDiskSetting.getInstance().getDialog().isShowing()) {
				StateDiskSetting.getInstance().getHandler()
						.sendEmptyMessage(1226);
			}
		}

		// Fix CR: DTV00584310
		if (getState() instanceof StateInitDisk) {
			((StateInitDisk) getState()).getmHandler().sendEmptyMessageDelayed(
					StateInitDisk.DISK_REMOUNT, 10);
		}

		Util.showDLog("onEvent,unmount,Path:" + mountPointPath);
		restoreToDefault(StatusType.FILELIST, StatusType.DISKSETTING);

		if (getRunningPvrMP() == null
				|| mountPointPath
						.equalsIgnoreCase(getRunningPvrMP().mMountPoint)) {
			if (StatePVR.getInstance() != null) {
				StatePVR.getInstance().clearPVRParams();
				
				if(StatePVR.getInstance().isRecording()){
					showDiskNotReady();
				}
			}
			restoreToDefault(StatusType.PVR);
			setRunningPvrMP(null);
		}

		if (getTshiftMP() == null
				|| mountPointPath.equalsIgnoreCase(getTshiftMP().mMountPoint)) {
			restoreToDefault(StatusType.TIMESHIFT);
			setTshiftMP(null);
			
			if(StateTimeShift.getInstance()!=null&&StateTimeShift.getInstance().isRunning()){
				showDiskNotReady();
			}
		}
	}

	private void setTshiftMP(MountPoint mountPoint) {
		// TODO Auto-generated method stub

	}

	private MountPoint getTshiftMP() {
		// TODO Auto-generated method stub
		return null;
	}

	private void dumpStatesStack() {
		Util.showDLog("------dumpStatesStack------");

		StateBase state;
		for (int i = mStates.size() - 1; i >= 0; i--) {
			state = mStates.get(i);
			Util.showDLog("dumpStatesStack:" + state.getType().name());
		}
	}

	@Override
	public void init() {
		// TODO Auto-generated method stub

	}

	@Override
	public void clearErrorState() {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean requestPermission(int keyCode) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean diskIsOperating() {
		// TODO Auto-generated method stub
		return false;
	}

	public long getBroadcastUTC() {
		// TODO Auto-generated method stub
		return TVTimerManager.getInstance(mContext).getBroadcastUTC();
	}

	private void channelBlocked() {
		Util.showDLog("TM-channelBlocked()");
		inpuSrcBlocked();
	}

	private void inpuSrcBlocked() {
		Util.showDLog("TM-inpuSrcBlocked()");
		/*
		 * if (getTvLogicManager().dtvNotReadyForRecord() ||
		 * getTvLogicManager().isCurrentChannelBlocked() ||
		 * getTvLogicManager().isDTVSourceBlocked()) {
		 * restoreToDefault(StatusType.PVR, StatusType.TIMESHIFT); }
		 */

		// Fix CR:DTV00598387
		if (getTvLogicManager().isCurrentChannelBlocked()
				|| getTvLogicManager().isDTVSourceBlocked()) {
			restoreToDefault(StatusType.PVR, StatusType.TIMESHIFT);
		}
	}

	IChannelSelectorListener mChannelChangeListener = new IChannelSelectorListener() {

		public void signalChange(boolean hasSignal) {
		}

		public void updateChannel(TVChannel ch) {
		}

		public void channelLocked() {
			Util.showDLog("channelLocked()");
			getTopHandler().sendEmptyMessage(
					TimeShiftManager.SPECIAL_CHANNEL_LOCK);
		}

		public void channelScrambled(boolean hasScramble, String state) {
			Util.showDLog("channelScrambled()");
			getTopHandler().sendEmptyMessage(
					TimeShiftManager.SPECIAL_CHANNEL_LOCK);
		}
	};

	NavIntegration.IInputSourceChangeListener mSrcChangeListener = new NavIntegration.IInputSourceChangeListener() {

		public void isBlocked(String input) {
			Util.showDLog("channelLocked()");
			getTopHandler().sendEmptyMessage(
					TimeShiftManager.SPECIAL_INPUT_LOCK);
		}

		public void isInputGotSignal(String input) {
		}

		public void isOutputSignalChange(String output, boolean hasSignal) {
		}

		public void isSelected(String output, String input) {
		}
	};

	private void registeListeners() {
		NavIntegration.getInstance(mTKUIActivity).iAddChSelectListener(
				mChannelChangeListener);
		NavIntegration.getInstance(mTKUIActivity)
				.iAddInputsourceChangeListener(mSrcChangeListener);
		// mModel_Parental_InputPwd
		// .addTextChangedListener(passwordInputTextWatcher);
	}

	private void unregisteListeners() {
		NavIntegration.getInstance(mTKUIActivity).iRemoveChSelectListener(
				mChannelChangeListener);
		NavIntegration.getInstance(mTKUIActivity)
				.iRemoveInputsourceChangeListener(mSrcChangeListener);
	}
}
