/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.controller;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.PopupWindow.OnDismissListener;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.mediatek.dm.MountPoint;
import com.mediatek.mmpcm.mmcimpl.Const;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.mmpcm.videoimpl.VideoManager;
import com.mediatek.timeshift_pvr.manager.Core;
import com.mediatek.timeshift_pvr.manager.PvrConstant;
import com.mediatek.timeshift_pvr.manager.TimeShiftManager;
import com.mediatek.timeshift_pvr.manager.Util;
import com.mediatek.timeshift_pvr.ui.OnPVRDialogListener;
import com.mediatek.timeshift_pvr.ui.PVRPlayView;
import com.mediatek.timeshift_pvr.ui.PvrFilelist;
import com.mediatek.timeshift_pvr.ui.PvrDialog;
import com.mediatek.timeshift_pvr.ui.TshiftTimerView;
import com.mediatek.ui.TurnkeyUiMainActivity;
import com.mediatek.ui.util.KeyMap;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;

/**
 *
 */
public class StateFileList extends StateBase implements IStateInterface,
		OnItemClickListener {

	private MyHandler mHandler;
	private static StateFileList mStateSelf;

	private static PvrFilelist mFileListWindow;
	private List<PVRFiles> mPVRFileList;

	private final static int DELETE_FILE = 0;
	private final static int STOP_PLAYING = 111;

	private final static int INIT_PLAYER = 1;
	private final static int START_PLAYER = 2;
	private final static int STOP_PLAYER = 3;
	private final static int PROGRESS_CHANGED = 11;
	private static final int MSG_GET_CUR_POS = 17;
	private final static int Show_Progress = 18;

	public final static int AUTO_DISMISS_FILE_LIST = 21;

	public final static int DELAY_RELEASE_STATE = 0xa001;
	public final static int UPDATE_SPEED = 0xa002;
	public final static int RESTORE_TO_NORMAL = 0xa003;

	private static final String TAG = "StateFileList";
	private static PVRPlayView mControlView;
	private static com.mediatek.timeshift_pvr.ui.TshiftTimerView mSmallTimerBar;

	private int speedStep = 1;
	private long mDuration = 0;
	private boolean isStoped = true;
	private int speedCounter = 1;
	private boolean doFast = false;
	private boolean doRewind = false;
	private boolean pausing = false;

	private String lastClickedFile = "";
	private boolean continuePlay = true;

	private boolean needResetZoom = false;

	private int mTotalAudioChannel = 1;
	private int mIndexOfAudioChannel = 1;

	static class MyHandler extends Handler {
		// WeakReference<Activity> mActivity;

		// MyHandler(Activity activity) {
		// mActivity = new WeakReference<Activity>(activity); // reserve.
		// }

		public void handleMessage(Message msg) {

			switch (msg.what) {
			case DELETE_FILE:
				removeMessages(DELETE_FILE);
				mStateSelf.delete();
				break;
			case INIT_PLAYER:
				Log.e("pvr0522", "handler------initplayer----------");
				removeMessages(START_PLAYER);
				mStateSelf.getManager().getController().initPvrPlayer();

				Message newMsg = Message.obtain();
				newMsg.copyFrom(msg);
				newMsg.what = START_PLAYER;
				newMsg.setData(msg.getData());

				mStateSelf.initControlView();
				sendMessage(newMsg);
				// sendMessageDelayed(newMsg, 2 * 1000);

				mStateSelf.mFileListWindow.setOnDismissListener(null);
				mStateSelf.mFileListWindow.dimissInfobar();
				// mStateSelf.playView = new
				// PVRPlayView(mStateSelf.getActivity(), mStateSelf);

				break;
			case START_PLAYER:
				// String
				// path=((File)parent.getItemAtPosition(position)).getAbsolutePath()
				String path = msg.getData().getString("FILE");
				Log.e("pvr0522", "start_player_path:" + path);
				mStateSelf.setPlayPath(msg);
				boolean result = mStateSelf.getManager().getController()
						.startPlayPvr(path);

				if (result) {
					// mStateSelf.initControlView();
					mStateSelf.update();
					// mStateSelf.addProgressMessage();
					mStateSelf.setControlBarListener();
				} else {
					mStateSelf.getManager().restoreToDefault(mStateSelf);
				}
				break;
			case Show_Progress:
				// mStateSelf.initControlView();
				// mStateSelf.update();
				mStateSelf.addProgressMessage();
				break;
			case STOP_PLAYER:

				break;
			case AUTO_DISMISS_FILE_LIST:
				mStateSelf.mFileListWindow.dimissInfobar();
				if (!mStateSelf.isPlaying()) {
					mStateSelf.getManager().restoreToDefault(
							StatusType.FILELIST);
				}
				break;
			case 22:
				if (mStateSelf.mControlView != null) {
					mStateSelf.mControlView.setOnDismissListener(null);
					mStateSelf.mControlView.dismiss();
				}
				break;
			case PROGRESS_CHANGED: {
				if (mStateSelf.mControlView != null) {

					int progress = mStateSelf.getManager().getController()
							.getVideoProgress();
					int step = mStateSelf.getManager().getController()
							.getSpeedStep();
					Log.e("5407", "progress:" + progress + "_max:"
							+ mStateSelf.mControlView.getProgressMax());
					if (progress >= 0
							&& progress <= mStateSelf.mControlView
									.getProgressMax()) {
						mStateSelf.mControlView.setCurrentTime(progress);
						mStateSelf.mControlView.setProgress(progress);
						if (progress == 0 && step != 1) {
							mStateSelf.mControlView.dismiss();
							mStateSelf.stopPvrPlayer();
							mStateSelf.getManager()
									.restoreToDefault(mStateSelf);
						}
					}
					if (progress > mStateSelf.mControlView.getProgressMax()) {
						progress = mStateSelf.mControlView.getProgressMax();
						mStateSelf.mControlView
								.setCurrentTime((progress / 1000 - 1) * 1000);
						mStateSelf.mControlView.setProgress(progress);
						// mHandler.removeMessages(DELAY_AUTO_NEXT);
						mStateSelf.mControlView.dismiss();
						mStateSelf.stopPvrPlayer();
						mStateSelf.getManager().restoreToDefault();
						// dismissTimeDialog();
						// dismissNotSupprot();
						// finish();
					}
					if (progress < 0) {
						mStateSelf.mControlView.setCurrentTime(0);
						mStateSelf.mControlView.setProgress(0);
						mStateSelf.mControlView.dismiss();
						mStateSelf.stopPvrPlayer();
						mStateSelf.getManager().restoreToDefault();
					}
				}
				if (mStateSelf.mControlView != null
						&& mStateSelf.getManager().getController()
								.getPlayStatus() > VideoConst.PLAY_STATUS_PREPAREING
						&& mStateSelf.getManager().getController()
								.getPlayStatus() < VideoConst.PLAY_STATUS_STOPPED) {
					sendEmptyMessageDelayed(PROGRESS_CHANGED, 1000);
				}
				break;
			}
			case MSG_GET_CUR_POS: {
				// progressFlag = true;
				if (mStateSelf.mControlView != null) {
					long pos = mStateSelf.getManager().getController()
							.getVideoBytePosition();
					if (mStateSelf.mLargeFile) {
						pos = pos >> RATE;
					}
					if (pos > 0)
						mStateSelf.mControlView.setProgress((int) pos);
				}
				break;
			}
			case DELAY_RELEASE_STATE:
				if (mStateSelf.continuePlay) {
					mStateSelf.continuePlay = false;
					return;
				} else {
					mStateSelf.getManager().restoreToDefault(mStateSelf);
				}

				break;
			case PvrConstant.Dissmiss_PVR_BigCtrlbar:
				if (mControlView != null && mControlView.isShowing()) {
					mControlView.dissmiss();
				}

				break;
			case STOP_PLAYING:
				int progress = mControlView.getProgressMax();
				mControlView.setCurrentTime((progress / 1000 - 1) * 1000);
				mControlView.setProgress(progress);
				mControlView.setFFNumView(View.GONE);
				mControlView.dismiss();
				mControlView = null;
				mStateSelf.stopPvrPlayer();
				mStateSelf.getManager().restoreToDefault(mStateSelf);
				break;

			case UPDATE_SPEED:
				mStateSelf.refreshSpeed();
				break;
			case RESTORE_TO_NORMAL:
				mStateSelf.getManager().restoreAllToNormal();
				break;
			default:
				break;
			}
			super.handleMessage(msg);
		}
	}

	public StateFileList(Context mContext, TimeShiftManager manager) {
		super(mContext, manager);
		setType(StatusType.FILELIST);
		showPVRlist();

		this.mContext = mContext;
		mStateSelf = this;
		mHandler = new MyHandler();

	}

	public void refreshSpeed() {
		// TODO Auto-generated method stub

	}

	@Override
	public void initViews() {
		super.initViews();
	}

	public void initControlView() {
		getManager().getController().setOnInfoListener(onInfoListener);
		getManager().getController().setVideoPreparedListener(preparedListener);
		getManager().getController().setCompleteListener(completeListener);
		mControlView = new PVRPlayView(mStateSelf.getActivity(), mStateSelf);
		mControlView.setCurrentTimeZero();
		mControlView.setProgress(0);

		mSmallTimerBar = TshiftTimerView.getInstance(getActivity());
		mSmallTimerBar.setInfo("");
		mSmallTimerBar.forceHiddenIcons();
		if (mSmallTimerBar != null && isPlaying()) {
			mSmallTimerBar.show(mSmallTimerBar);
		}

		setControlBarListener();
	}

	private void setControlBarListener() {
		mControlView.setOnDismissListener(new OnDismissListener() {
			@Override
			public void onDismiss() {
				// TODO Auto-generated method stub
				Util.showDLog("setControlBarListener(),mControlView-onDismiss(),");
				if (mSmallTimerBar != null && isPlaying()) {
					if (!mSmallTimerBar.isShowing()) {
						mSmallTimerBar.show(mSmallTimerBar);
					}
				}
			}
		});
		mSmallTimerBar.setOnDismissListener(new OnDismissListener() {
			@Override
			public void onDismiss() {
				// TODO Auto-generated method stub
				Util.showDLog("setControlBarListener(),mSmallTimerBar-onDismiss(),");
				if (mControlView != null && isPlaying()) {
					if (!mControlView.isShowing()) {
						mControlView.show();
					}
				}
			}
		});
	}

	private void showPVRlist() {
		mPVRFileList = new ArrayList<PVRFiles>();

		if (getManager().hasRemovableDisk()) {
			if (getManager().getPvrMP() == null) {
				// getManager().setDefaultPvrMP();
			}
			ArrayList<MountPoint> diskList = getManager().getDeviceList();
			String mp = "";
			String folder = "";

			for (int i = 0; i < diskList.size(); i++) {
				mp = diskList.get(i).mMountPoint;
				folder = mp + "/" + Core.getPvrfolder();
				mPVRFileList.addAll(Util.getPvrFiles(folder));
			}
		}

		if (mFileListWindow != null) {
			mFileListWindow.show();
		} else {
			initPVRlist(mPVRFileList);
			mFileListWindow.show();
		}
	}

	private void updateRecordOrPlayFileParams() {
		// TODO Auto-generated method stub
		String filePlaying = "";
		if (getManager().pvrIsPlaying()) {
			filePlaying = getManager().getController().getPlayingFilePath();
		}

		String fileRecording = "";
		if (getManager().pvrIsRecording()) {
			fileRecording = getManager().getController().getRecordingFileName();
			fileRecording = fileRecording.replaceAll("//", "/");
			// fileRecording=(String)
			// fileRecording.subSequence(fileRecording.lastIndexOf("/")+1,
			// fileRecording.length());
		}

		Util.showDLog("filePlaying:" + filePlaying);
		Util.showDLog("fileRecording:" + fileRecording);

		boolean recordingOrPlaying = false;
		String fineNameStr = "";
		for (int i = 0; i < mPVRFileList.size(); i++) {
			fineNameStr = String.format("[%2d] %s", i + 1, mPVRFileList.get(i)
					.getName());
			mPVRFileList.get(i).setIndexAndName(fineNameStr);

			if (recordingOrPlaying) {
				continue;
			}

			String filePath = mPVRFileList.get(i).getAbsolutePath();
			if ((!filePlaying.equalsIgnoreCase(""))
					|| (!fileRecording.equalsIgnoreCase(""))) {
				if (filePath.equalsIgnoreCase(filePlaying)) {
					mPVRFileList.get(i).isPlaying = true;
					recordingOrPlaying = true;
					Util.showDLog("isPlaying true," + filePath);
				}

				if (filePath.equalsIgnoreCase(fileRecording)) {
					mPVRFileList.get(i).isRecording = true;
					recordingOrPlaying = true;
					Util.showDLog("isRecording true," + filePath);
				}
			}
		}
	}

	public void initPVRlist(List<PVRFiles> list) {
		mFileListWindow = new PvrFilelist(getActivity(), this, list, mHandler);
		mFileListWindow.setListener(this);

		mFileListWindow.setOnDismissListener(new OnDismissListener() {

			@Override
			public void onDismiss() {
				// TODO Auto-generated method stub
				if (!isRunning() && (!continuePlay)) {
					getHandler().sendEmptyMessageDelayed(DELAY_RELEASE_STATE,
							300);
				}
			}
		});
	}

	public void reShowPVRlist() {
		if (mFileListWindow != null && mFileListWindow.isShowing()) {
			mFileListWindow.reSetTimeTask();
		} else {
			mFileListWindow = null;
			showPVRlist(); // will re-init again.
		}
	}

	public void deletePvrFile() {
		mHandler.sendEmptyMessage(DELETE_FILE);
	}

	public void delete() {
		mFileListWindow.deleteFile();
	}

	public PVRFiles getSelectedFile() {
		return mFileListWindow.getSelectedFile();
	}

	/*
	 * (non-Javadoc)
	 */
	@Override
	public boolean onKeyDown(int keycode) {
		Util.showDLog(getClass().getSimpleName() + "StateFileList.onKeyDown()"
				+ keycode);

		switch (keycode) {
		case KeyEvent.KEYCODE_DPAD_CENTER:
		case KeyEvent.KEYCODE_DPAD_UP:
		case KeyEvent.KEYCODE_DPAD_DOWN:
		case KeyEvent.KEYCODE_MTKIR_GREEN:
		case KeyEvent.KEYCODE_MTKIR_YELLOW:
		case KeyEvent.KEYCODE_MTKIR_RED:
		case KeyMap.KEYCODE_MTKIR_PLAYPAUSE:
			// case KeyEvent.KEYCODE_MTKIR_INFO:
			// case KeyEvent.KEYCODE_MTKIR_BLUE:
			if (getManager().getController().isPlaying()) {
				if ((!mFileListWindow.isShowing())
						&& (keycode == KeyEvent.KEYCODE_DPAD_CENTER || keycode == KeyMap.KEYCODE_MTKIR_PLAYPAUSE)) {
					if (getManager().getController().isPause()
							|| getManager().getController().isFForFR()) {
						getManager().getController().backToNormalSpeed();
					} else {
						getManager().getController().pausePlayPvr();
					}
					mControlView.getFastView().setVisibility(View.GONE);
				}

				return true;
			}
			reShowPVRlist();

			dispatchTheOnkeyDown(keycode);

			mFileListWindow.onKeyDown(keycode);
			return true;
		case KeyEvent.KEYCODE_MEDIA_STOP:
			// DTV00587256:Should not response stop key in record list menu.
			if (null != mFileListWindow && mFileListWindow.isShowing()) {
				return true;
			}
			stopPvrPlayer();
			getManager().restoreToDefault(mStateSelf);
			// setManager(null);

			return true;
		case KeyMap.KEYCODE_BACK:
			// Fix CR:DTV00583855,DTV00583474
			if (mFileListWindow.isShowing()) {
				mFileListWindow.dismiss();
			}

			if (isPlaying()) {
				// show small control bar.
				mControlView.dissmiss();
				return true;
			} else {
				getManager().restoreToDefault(mStateSelf);
				return true;
			}
		case KeyEvent.KEYCODE_MTKIR_REPEAT:
			if (isPlaying() && !mFileListWindow.isShowing()) {
				this.reShowPVRlist();
				return true;
			}

			if (!mFileListWindow.isShowing()) {
				this.reShowPVRlist();
				return true;
			}

			// Fix CR:DTV00583855,DTV00583474
			if (mFileListWindow.isShowing()) {
				if (!isPlaying()) {
					getManager().restoreToDefault(mStateSelf);
					return true;
				} else {
					if (mFileListWindow != null) {
						mFileListWindow.setOnDismissListener(null);
						mFileListWindow.dimissInfobar();
						// getManager().restoreToDefault(mStateSelf);
					}
					return true;
				}
			}

			return false;
		case KeyEvent.KEYCODE_CHANNEL_DOWN:
		case KeyEvent.KEYCODE_CHANNEL_UP:
		case KeyEvent.KEYCODE_MTKIR_CHUP:
		case KeyEvent.KEYCODE_MTKIR_CHDN:
		case KeyEvent.KEYCODE_MTKIR_SOURCE:
		case KeyEvent.KEYCODE_MTKIR_PIPPOP:
		case KeyEvent.KEYCODE_MTKIR_RECORD:
		case KeyEvent.KEYCODE_MEDIA_EJECT:
		case KeyEvent.KEYCODE_MTKIR_ASPECT:
		case KeyEvent.KEYCODE_MTKIR_ZOOM:
			return true;

		case KeyEvent.KEYCODE_MEDIA_FAST_FORWARD:
			forwardPlay();
			return true;

		case KeyEvent.KEYCODE_MEDIA_REWIND:
			rewindPlay();
			return true;

		case KeyMap.KEYCODE_MTKIR_ANGLE:
			if (this.isPlaying()) {
				PvrDialog conDialog = new PvrDialog(getActivity(),
						PvrDialog.TYPE_Confirm, KeyMap.KEYCODE_MTKIR_ANGLE,
						PvrDialog.TYPE_STOP_PVR_PLAYING_ENTER_MMP);
				conDialog.setOnPVRDialogListener(new OnPVRDialogListener() {

					@Override
					public void onPVRDialogListener(int keyCode) {
						// TODO Auto-generated method stub
						StateFileList.getInstance().enterMMP();
					}
				});
				conDialog.show();
				return true;
			}

			enterMMP();
			return true;
		case KeyMap.KEYCODE_MTKIR_GUIDE:

			return true;
		default:
			break;
		}
		return false;
	}

	private void enterMMP() {
		TimeShiftManager.getInstance().stopAllRunning();
		TimeShiftManager.getInstance().getTopHandler()
				.removeMessages(PvrConstant.RECORD_FINISH_AND_SAVE);
		try {
			Intent intentAn = new Intent();
			Class clazz = Class
					.forName("com.mediatek.ui.mmp.MeidaMainActivity");
			intentAn.setClass(getActivity(), clazz);
			intentAn.putExtra("TKUI", true);

			getActivity().startActivity(intentAn);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	public void replay() {
		doFast = false;
		doRewind = false;
		isStoped = false;
		pausing = false;
		speedCounter = 1;
		if (mControlView != null) {
			mControlView.setFFNumView(View.GONE);
			// mControlView.setPlay();
		}

		getManager().getController().setStepValue(speedCounter);
		isStoped = false;
		mHandler.removeMessages(START_PLAYER);

		Message newMsg = Message.obtain();
		newMsg.what = START_PLAYER;

		Bundle bundle = new Bundle();
		bundle.putString("FILE", path);
		newMsg.setData(bundle);

		mHandler.sendMessageDelayed(newMsg, 500);
		if (mControlView != null && mControlView.getFastView() != null) {
			mControlView.getFastView().setVisibility(View.INVISIBLE);
		}

	}

	String path = "";

	public void setPlayPath(Message newMsg) {
		path = newMsg.getData().getString("FILE");
	}

	/**
	 * 
	 */
	private void forwardPlay() {
		getManager().getController().forwardPlay(2);
	}

	/**
	 * 
	 */
	public void stopPvrPlayer() {
		getManager().getController().stopPlayPvr();
	}

	/**
	 * 
	 */
	private void rewindPlay() {
		getManager().getController().rewindPlay(2);
	}

	/**
	 * @param keycode
	 * @return
	 */
	private boolean dispatchTheOnkeyDown(int keycode) {

		switch (keycode) {
		case KeyEvent.KEYCODE_DPAD_CENTER:
			break;
		case KeyEvent.KEYCODE_DPAD_UP:
			break;
		case KeyEvent.KEYCODE_DPAD_DOWN:
			break;
		case KeyEvent.KEYCODE_MTKIR_GREEN:
			break;
		case KeyEvent.KEYCODE_MTKIR_YELLOW:
			break;
		case KeyEvent.KEYCODE_BACK:
			break;
		default:
			break;
		}
		return false;
	}

	/*
	 * 
	 */
	@Override
	public void onResume() {
		// TODO Auto-generated method stub
		if (mStateSelf.mSmallTimerBar != null
				&& mStateSelf.mSmallTimerBar.isShowing()) {
			mStateSelf.mSmallTimerBar.dismiss();
		}
		if (this.isRunning() && mStateSelf.mControlView != null) {
			mStateSelf.mControlView.show();
		}

	}

	/* 
	 */
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
		// TODO Auto-generated method stub
		if (mFileListWindow != null && mFileListWindow.isShowing()) {
			mFileListWindow.setOnDismissListener(null);
			mFileListWindow.dimissInfobar();
		}

		if (mControlView != null) {
			mControlView.dismiss();
		}
		/*if (isRunning()) {
			mSmallTimerBar.show();
		} else {
			getManager().restoreToDefault(StatusType.FILELIST);
		}*/
		
		getManager().restoreToDefault(StatusType.FILELIST);
	}

	/* 
	 */
	@Override
	public void onStop() {
		stopPvrPlayer();
		if (mControlView != null) {
			mControlView.setFFNumView(View.GONE);
		}
		try {
			mStateSelf.mControlView.setOnDismissListener(null);
			mStateSelf.mSmallTimerBar.setOnDismissListener(null);
			mStateSelf.mControlView.dismiss();
			mStateSelf.mSmallTimerBar.dismiss();
			mHandler.removeMessages(PROGRESS_CHANGED);

		} catch (Exception e) {
			// TODO: handle exception
		}
	}

	/* 
	 */
	@Override
	public void onRelease() {
		if (mFileListWindow != null && mFileListWindow.isShowing()) {
			mFileListWindow.setOnDismissListener(null);
			mFileListWindow.dimissInfobar();
		}
		try {
			mFileListWindow.dismiss();
			clearPlayingUI();
			mHandler.removeMessages(PROGRESS_CHANGED);
			mHandler.sendEmptyMessage(22);

			if (getController().getPlayback() != null) {
				getController().getPlayback().onRelease();
			}

			resetAllParams();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			Util.showELog(TAG,
					"StateFileList,OnRelease(),Exception: " + e.toString());
		}
	}

	private void resetAllParams() {
	}

	private void dismissAllPlayingUI() {
		Util.showDLog("dismissAllPlayingUI()");
		try {
			if (mStateSelf.mControlView != null) {
				mStateSelf.mControlView.setOnDismissListener(null);
				mStateSelf.mControlView.dismiss();
			}
			if (mStateSelf.mSmallTimerBar != null) {
				mStateSelf.mSmallTimerBar.setOnDismissListener(null);
				mStateSelf.mSmallTimerBar.dismiss();
			}
		} catch (Exception e) {
			Util.showDLog("dismissAllPlayingUI()," + e.toString());
		}
	}

	/*
	 * 
	 */
	@Override 
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		Util.showDLog("onItemClick");
		// is DTV?
		// if (getManager().isDTVSource()) {
		if (StatePVR.getInstance() != null
				&& StatePVR.getInstance().isRunning()) {
			TimeShiftManager.getInstance().showFeatureNotAvaiable();
			return;
		}

		if (StateTimeShift.getInstance() != null
				&& StateTimeShift.getInstance().isRunning()) {
			TimeShiftManager.getInstance().showFeatureNotAvaiable();
			return;
		}

		if (getManager().getTvLogicManager().isPipOrPopState()) {
			TimeShiftManager.getInstance().showFeatureNotAvaiable();
			return;
		}
		
		playPvrFileByMMPModule(parent,position);
		//Play by Pvr Playback Module @Deprecated
		/*if (isRunning() && (!continuePlay)) {
			stopPvrPlayer();
			continuePlay = true;
		} else {
			continuePlay = false;
		}

		mHandler.sendEmptyMessage(22);
		Message msg = mHandler.obtainMessage();
		Bundle bundle = new Bundle();
		bundle.putString("FILE",
				((File) parent.getItemAtPosition(position)).getAbsolutePath());
		msg.setData(bundle);
		msg.what = INIT_PLAYER;
		mHandler.sendMessage(msg);
		lastClickedFile = ((File) parent.getItemAtPosition(position))
				.getAbsolutePath();*/
	}

	private void playPvrFileByMMPModule(AdapterView<?> parent,int position) {
		// TODO Auto-generated method stub
		File file=((File) parent.getItemAtPosition(position));
		TurnkeyUiMainActivity.getInstance().enterMMP(file.getAbsolutePath());
		
		mHandler.sendEmptyMessageDelayed(RESTORE_TO_NORMAL, 100);
	}

	public static StateFileList getInstance() {
		if (mStateSelf != null) {
			return mStateSelf;
		} else {
			return null;
		}
	}

	public static StateFileList getInstance(TimeShiftManager manager) {
		if (mStateSelf != null) {
			return mStateSelf;
		} else {
			mStateSelf = new StateFileList(manager.getActivity(), manager);
			return mStateSelf;
		}
	}

	protected void addProgressMessage() {
		if (null != mStateSelf && mStateSelf.mControlView != null) {
			mStateSelf.mControlView.show();
		}
		if (!mHandler.hasMessages(PROGRESS_CHANGED)) {
			mHandler.sendEmptyMessage(PROGRESS_CHANGED);
		}
	}

	private OnInfoListener onInfoListener = new OnInfoListener() {

		@Override
		public boolean onInfo(MediaPlayer mp, int what, int extra) {
			Log.e("stateplay", "stateplay:onInfo----------------------");
			switch (what) {
			case VideoManager.MEDIA_INFO_POSITION_UPDATE:
				Log.e("stateplay", "stateplay:onInfo");
				if (mHandler.hasMessages(PROGRESS_CHANGED)) {
					mHandler.removeMessages(PROGRESS_CHANGED);
				}
				mHandler.sendEmptyMessage(MSG_GET_CUR_POS);
				break;

			default:
				break;
			}
			return false;
		}
	};

	private OnPreparedListener preparedListener = new OnPreparedListener() {
		@Override
		public void onPrepared(MediaPlayer mp) {
			Log.e("stateplay", "stateplay:onPrepared----------------------");
			update();
			// reSetController();
			addProgressMessage();
		}
	};
	private OnCompletionListener completeListener = new OnCompletionListener() {

		@Override
		public void onCompletion(MediaPlayer mp) {
			Log.e("stateplay", "stateplay:onCompletion----------------------");
			mStateSelf.mControlView.dismiss();
			stopPvrPlayer();
			getManager().restoreToDefault(mStateSelf);
		}
	};

	public void clearPlayingUI() {
		Util.showDLog(TAG, "clearPlayingUI()");
		dismissAllPlayingUI();
		stopPvrPlayer();

		if (needResetZoom) {
			getManager().getTvLogicManager().reSetZoomValues(getActivity());
			needResetZoom = false;
		}
	}

	private void update() {
		if (mControlView != null) {
			int i = getManager().getController().getVideoDuration();
			long size;
			mLargeFile = false;
			if (i <= 0) {
				size = getManager().getController().getVideoFileSize();
				mControlView.setVisibility(false);
				mLargeFile = isLargeFile(size);
				size = (size > MAX_VALUE) ? size >> RATE : size;
				i = (int) size;
			} else {
				mControlView.setVisibility(true);
			}
			i = (i > 0 ? i : 0);
			mControlView.setProgressMax(i);
			mControlView.setEndtime(i);
		}
	}

	static private int MAX_VALUE = 2147483647;
	static private int RATE = 2;
	static private int BASE = 31;
	private boolean mLargeFile = false;

	private boolean isLargeFile(long size) {
		long multiple;
		RATE = 2;
		if (size > MAX_VALUE) {
			multiple = size >> BASE;
			while (true) {
				switch ((int) multiple) {
				case 1:
				case 2:
				case 3:
					return true;
				default:
					multiple = multiple >> 1;
					RATE += 1;
					break;
				}
			}
		}
		return false;
	}

	public void showFastPlayInfo(boolean FastForward) {
		int status = getManager().getController().getPlayStatus();
		int speedStep = getManager().getController().getSpeedStep();
		Util.Hlog("status:" + status + "_speedStep_:" + speedStep);

		if (FastForward) {
			onFast(speedStep, 0, Const.FILTER_VIDEO);

		} else {
			onFast(speedStep, 1, Const.FILTER_VIDEO);
		}

	}

	public void onFast(int speed, int status, int type) {
		mControlView.getFastView().setVisibility(View.VISIBLE);

		if (speed == 1) {
			speedStep = 1;
			mControlView.getFastView().setVisibility(View.GONE);
			return;
		}

		mControlView.getFastView().setText(speed + "x");
		Drawable img_left = null;
		if (type == Const.FILTER_VIDEO) {
			mControlView.getFastView().setTextColor(Color.WHITE);
		}

		if (status == 0) {
			if (type == Const.FILTER_VIDEO) {
				img_left = mContext.getResources().getDrawable(
						com.mediatek.ui.R.drawable.toolbar_typeicon_ff_video);
			} else {
				img_left = mContext.getResources().getDrawable(
						com.mediatek.ui.R.drawable.toolbar_typeicon_ff);
			}
		} else if (status == 1) {
			if (type == Const.FILTER_VIDEO) {
				img_left = mContext.getResources().getDrawable(
						com.mediatek.ui.R.drawable.toolbar_typeicon_rew_video);
			} else {
				img_left = mContext.getResources().getDrawable(
						com.mediatek.ui.R.drawable.toolbar_typeicon_rew);

			}
		} else if (status == 2) {
			img_left = mContext.getResources().getDrawable(
					com.mediatek.ui.R.drawable.mmp_toolbar_typeicon_video_paly);

			mControlView.getFastView().setText("1/" + speed + "x");

		}
		if (img_left != null) {
			img_left.setBounds(0, 0, img_left.getMinimumWidth(),
					img_left.getMinimumHeight());
		}

		mControlView.getFastView().setCompoundDrawables(img_left, null, null,
				null);
	}

	public boolean isPlaying() {
		Util.showDLog("-------------isPlaying()?--------------------");
		Util.printStackTrace();

		boolean running = false;
		if (mControlView != null && mControlView.isShowing()) {
			running = true;
		}

		if (mSmallTimerBar != null && mSmallTimerBar.isShowing()) {
			running = true;
		}

		running = !isStoped;
		Util.showDLog(TAG, "isPlaying()?=" + running);
		return running;
	}

	public boolean isShowing() {
		if (mFileListWindow != null) {
			return mFileListWindow.isShowing();
		}
		return false;
	}

	public Handler getHandler() {
		return mHandler;
	}

	public boolean equalPlayingFile(String filePath) {
		Util.showDLog("lastClickedFile:" + lastClickedFile);
		Util.showDLog("filePath:" + filePath);

		if (lastClickedFile.equalsIgnoreCase(filePath)) {
			if (isPlaying()) {
				return true;
			}
		}

		return false;
	}
}
