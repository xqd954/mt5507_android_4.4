/**   
 * @Description: TODO()
 */
package com.mediatek.timeshift_pvr.manager;

import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnInfoListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.storage.MountServiceListener;
import android.util.Log;

import com.mediatek.dm.MountPoint;

import com.mediatek.mmpcm.videoimpl.VideoComset;
import com.mediatek.mmpcm.videoimpl.VideoConst;
import com.mediatek.mmpcm.videoimpl.VideoManager;
import com.mediatek.record.IRecordRemoteService;
import com.mediatek.record.RecordConstant;
import com.mediatek.record.RecordCreateFileParameter;
import com.mediatek.record.RecordEvent;
import com.mediatek.record.RecordListener;
import com.mediatek.record.RecordService;
import com.mediatek.record.RecordStartParameter;
import com.mediatek.record.RecordStatus;
import com.mediatek.timeshift_pvr.controller.IStateInterface;
import com.mediatek.timeshift_pvr.controller.StateFileList;
import com.mediatek.timeshift_pvr.controller.StatePVR;
import com.mediatek.timeshift_pvr.controller.StateTimeShift;
import com.mediatek.timeshift_pvr.controller.StatusType;
import com.mediatek.ui.nav.util.NewPipLogic;

import java.io.File;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 * manager pvr's record and playback.
 */
public class Controller implements OnPreparedListener, OnCompletionListener {

	private TimeShiftManager mManager;

	private IRecordRemoteService mService;
	private RecordListener mListener;

	private VideoManager playback;

	public static final int RECORD_PVR = RecordConstant.RecordType.RECORD_PVR;
	public static final int RECORD_TSHIFT = RecordConstant.RecordType.RECORD_TSHIFT;

	public static final int INSTANCE_ID = 2;

	public static final boolean STATE_SUCCESS = true;
	public static final boolean STATE_FAIL = false;

	private final Long TSHIFT_SIZE = 300L * 1024 * 1024;
	private final Long PVR_SIZE = 0L;

	private int taskID = -1;
	private String tsFile = "";
	private boolean isPvrTask = false;
	private boolean startPVR = false;

	public static boolean disableScanning = false;

	public static final int COMMON_RECORD_CONTENT = RecordConstant.RecordStream.RECORD_ST_MASK_AUDIO
			| RecordConstant.RecordStream.RECORD_ST_MASK_VIDEO
			| RecordConstant.RecordStream.RECORD_ST_MASK_SUBTITLE;

	private static final String TAG = "Controller";
	public enum Status {
		UNKONWN, PREPARE, MAKE_FILE, RECORDING, STOPPED
	}

	boolean goToStop = false;

	private Status status;

	private String playingFilePath = "";
	public Controller(TimeShiftManager manager) {
		this.mManager = manager;
		startService();
	}

	/**
	 * @param isPvr
	 * @param onlyMakeFile
	 *            For diskSetting's create tshift file.
	 */
	public void preparePvr(boolean isPvr, boolean onlyMakeFile) {
		setPvrTask(isPvr);
		setStatus(Status.PREPARE);

		String folder = prepareFolder(isPvr);

		if (folder.equalsIgnoreCase("")) {
			return;
		}

		String file1 = prepareFile(isPvr);
		// String fileTS = prepareTSFile(isPvr);
		String fileMeta = prepareMetaFile(isPvr, file1);
		String fileBin = prepareBinFile(file1);

		if (!isPvr) {
			// clearTshiftFiles(folder); //won't clear
		}

		if (isPvr) {
			File fileInstancesMeta = new File(folder + "/" + fileMeta);
			if (fileInstancesMeta.exists()) {
				fileInstancesMeta.delete();
			}

			File fileInstancesTS = new File(folder + "/" + fileBin);
			if (fileInstancesTS.exists()) {
				fileInstancesTS.delete();
			}
		}

		RecordCreateFileParameter cfPara;
		if (isPvr) {
			cfPara = new RecordCreateFileParameter(RECORD_PVR,
					RecordConstant.RecordMetaPolicy.RECORD_META_FILE_VISIBLE,
					folder, fileMeta, fileBin, PVR_SIZE);
		} else {
			int size = mManager.getTshiftFileSize();

			cfPara = new RecordCreateFileParameter(RECORD_TSHIFT,
					RecordConstant.RecordMetaPolicy.RECORD_META_FILE_INVISIBLE,
					folder, fileMeta, fileBin, size * 1024L);
		}
		setListener(onlyMakeFile);
		Log.e("prv0523", "preparePvr");
		if (isPvr || onlyMakeFile) {
			createFile(cfPara);
			setTsFile(folder + "/" + fileMeta);
		} else {
			if (hasTshiftFile()) {
				setTsFile(folder + "/" + fileMeta);
				startPvr();
			} else {
				mManager.showFeatureNotAvaiable();
				mManager.restoreToDefault(StatusType.PVR,StatusType.TIMESHIFT);
			}
		}
	}

	/**
	 * 
	 */
	private void clearTshiftFiles(String folder) {
		File file = new File(folder);

		File[] files = file.listFiles();
		for (int i = 0; i < files.length; i++) {
			if (files[i].isFile()) {
				files[i].delete();
			}
		}
	}

	public boolean hasTshiftFile() {
		boolean isPvr = false;

		String folder = prepareFolder(isPvr);

		if (folder.equalsIgnoreCase("")) {
			return false;
		}

		String file1 = prepareFile(isPvr);
		String fileMeta = prepareMetaFile(isPvr, file1);

		File fileInstancesMeta = new File(folder + "/" + fileMeta);

		if (fileInstancesMeta.exists()) {
			return true;
		}
		return false;
	}

	public boolean getRecordStatus(int recordHandle, RecordStatus recordStatus) {
		int status = -1;

		try {
			checkService();

			status = mService.getRecordStatus(getTaskID(), recordStatus);
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		if (status == 0) {
			return STATE_SUCCESS;
		} else {
			return STATE_FAIL;
		}
	}

	public void stopPvr(boolean isPvr) {
		startPVR = false;
		if (isPvr) {
			stopRecord();
		} else {
			stopRecord();
			// deleteTshiftFile(getTsFile()); //won't clear this file.
			// stopPlayPvr();
		}
		// clearObject();
		// mManager.restoreToDefault();
	}

	private void createFile(RecordCreateFileParameter createFileData) {
		setStatus(Status.MAKE_FILE);
		try {
			checkService();

			mService.createRecordFile(createFileData);
		} catch (RemoteException e) {
			Util.showELog("createRecordFile <fail>");
			setTaskID(-1);
			mManager.onErrorHappened(ErrorCode.CALL_CM_ERROR);
			e.printStackTrace();
		}
	}

	public void setTshiftSize(Long size, MountPoint mp, RecordListener listener) {

		createFile(null);
	}

	/**
	 * 
	 */
	private void startPvr() {
		RecordStartParameter arameter;
		// if(startPVR){
		// return ;
		// }
		Log.e("prv0523", "startPvr:" + startPVR);

		if (isPvrTask()) {
			arameter = new RecordStartParameter(getTsFile(), RECORD_PVR,
					INSTANCE_ID, COMMON_RECORD_CONTENT);
		} else {
			arameter = new RecordStartParameter(getTsFile(), RECORD_TSHIFT,
					INSTANCE_ID, COMMON_RECORD_CONTENT);
		}
		startPVR = true;
		startRecord(arameter);
	}

	private void startRecord(RecordStartParameter startRecordData) {
		Util.showDLog("startRecord()");
		Log.e("prv0523", "startRecord");
		final RecordStartParameter data = startRecordData;
		try {
			checkService();

			int code = mService.startRecord(data);
			if(code==-1){
				setTaskID(ErrorCode.CALL_CM_ERROR);
				mManager.onErrorHappened(ErrorCode.CALL_CM_ERROR);
				return;
			}
			setTaskID(code);

			setStatus(Status.RECORDING);
			
			StatePVR.isDotByDotMode=mManager.getTvLogicManager().isDotByDotScreenMode();
			if(StatePVR.isDotByDotMode){
				mManager.getTvLogicManager().setScreenModeToNormal();
			}
		} catch (RemoteException e) {
			setTaskID(ErrorCode.CALL_CM_ERROR);
			mManager.onErrorHappened(ErrorCode.CALL_CM_ERROR);

			e.printStackTrace();
		}
	}

	private void clearObject() {
		try {
			mService.unregisterListener(mListener);
		} catch (RemoteException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		// mService = null;
		setPvrTask(false);
		setStatus(Status.UNKONWN);
		setTsFile("");
	}

	private int stopRecord() {
		Util.showDLog("stopRecord()");
		Log.e("prv0523", "stopRecord");
		try {
			mService.stopRecord(getTaskID());
		} catch (RemoteException e) {
			mManager.onErrorHappened(ErrorCode.CALL_CM_ERROR);
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

		try {
			if (mListener != null)
				mService.unregisterListener(mListener);
		} catch (RemoteException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

		mListener = null;

		setTaskID(-1);

		return 0;
	}

	/**
	 * @param result
	 * @param cfPara
	 * @return
	 */
	private boolean createRecordFile(int result,
			RecordCreateFileParameter cfPara) {
		Util.showDLog("createRecordFile()");

		try {
			checkService();

			result = mService.createRecordFile(cfPara);
		} catch (RemoteException e) {
			e.printStackTrace();
		}

		if (result == 0) {
			return STATE_SUCCESS;
		} else {
			return STATE_FAIL;
		}
	}

	private int deleteTshiftFile(final String path) {
		Util.showDLog("deleteRecordFile()");

		File file = new File(path);
		if (file.exists()) {
			clearTshiftFiles(file.getParentFile().getAbsolutePath());
		}

		try {
			mService.deleteRecordFile(path);
		} catch (RemoteException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return 0;
	}

	/**
	 * 
	 */
	private void startService() {
		IBinder b = (IBinder) android.os.ServiceManager
				.getService(RecordService.SERVICE_NAME);
		Log.e("pvr0606", "startService_b:" + b);
		mService = IRecordRemoteService.Stub.asInterface(b);
		Log.e("pvr0606", "startService:" + mService);
	}

	/**
	 * 
	 */
	private void checkService() {
		if (mService == null) {
			startService();
		}
	}

	private void setListener(boolean onlyMKFile) {
		// checkService();
		try {

			if (mListener != null)
				mService.unregisterListener(mListener);
		} catch (RemoteException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (onlyMKFile) {
			mListener = new MakeFileListenerImpl();

		} else {
			mListener = new RecordListenerImpl();
		}

		try {
			mService.registerListener(mListener);
			Util.showDLog("serviceCLS hashCode:"
					+ mService.getClass().hashCode());
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	private String prepareFolder(boolean isPvr) {

		StringBuilder path = new StringBuilder();
		mManager.checkPvrMP();
		path.append(mManager.getPvrMP().mMountPoint);

		if (isPvr) {
			path.append("/");
			path.append(Core.getPvrfolder());
		} else {

			path.append("/");
			path.append(Core.getTSfolder());
		}

		if (!Util.makeDIR(path.toString())) {
			return "";
		}

		if (isPvr) {
			path.append("/");
			path.append(mManager.getTvLogicManager().getChannelNumStr());
			if (!Util.makeDIR(path.toString())) {
				return "";
			}
		}
		return path.toString();
	}

	private String prepareFile(boolean isPvr) {
		StringBuilder path = new StringBuilder();

		if (isPvr) {
			path.append(mManager.getTvLogicManager().getChannelNumStr() + "_");
			path.append(Util.dateToStringYMD2(new Date()) + "_");
			path.append(Util.timeMillisToChar());
		} else {
			path.append(Core.getDefaulttshiftfilename());
		}

		return path.toString();
	}

	private String prepareBinFile(boolean isPvr) {
		StringBuilder path = new StringBuilder();

		path.append(prepareFile(isPvr) + "_");

		path.append("%1d");
		path.append(".bin");

		return path.toString();
	}

	/**
	 * @param name
	 * @return the file name must end with ".pvr"
	 */
	private String prepareMetaFile(boolean isPvr, String name) {
		StringBuilder path = new StringBuilder();
		path.append(name + "_");
		if (isPvr) {
			path.append(Core.SUFFIX_PVR);
		} else {
			path.append(Core.SUFFIX_TSHIFT);
		}
		Util.showDLog("prepareMetaFile()," + path.toString());
		return path.toString();
	}

	/**
	 * @param name
	 * @return
	 */
	private String prepareBinFile(String name) {
		StringBuilder path = new StringBuilder();

		path.append(name + "_");

		path.append("%1d");
		path.append(".bin");

		Util.showDLog("prepareBinFile()," + path.toString());
		return path.toString();
	}

	/**
	 * @return the status
	 */
	public Status getStatus() {
		return status;
	}

	/**
	 * @param status
	 *            the status to set
	 */
	public void setStatus(Status status) {
		this.status = status;
		Util.showDLog("change status: " + status.name());
	}

	/**
	 * @return the tsFile
	 */
	public String getTsFile() {
		Util.showDLog("getTsFile(): " + tsFile);
		return tsFile;
	}

	/**
	 * @param tsFile
	 *            the tsFile to set
	 */
	public void setTsFile(String tsFile) {
		Util.showDLog("tsFile:" + tsFile);
		this.tsFile = tsFile;
	}

	/**
	 * @return the isPvrTask
	 */
	public boolean isPvrTask() {
		return isPvrTask;
	}

	/**
	 * @param isPvrTask
	 *            the isPvrTask to set
	 */
	public void setPvrTask(boolean isPvrTask) {
		this.isPvrTask = isPvrTask;
	}

	/**
	 * @return the taskID
	 */
	public int getTaskID() {
		return taskID;
	}

	/**
	 * @param taskID
	 *            the taskID to set
	 */
	public void setTaskID(int taskID) {
		this.taskID = taskID;
	}

	public void sampleSample() {
		RecordStartParameter startRecordData = new RecordStartParameter("test",
				RecordConstant.RecordType.RECORD_TSHIFT, 2,
				RecordConstant.RecordStream.RECORD_ST_MASK_AUDIO
						| RecordConstant.RecordStream.RECORD_ST_MASK_VIDEO
						| RecordConstant.RecordStream.RECORD_ST_MASK_SUBTITLE);
		try {
			int recordHandle = mService.startRecord(startRecordData);

			mService.stopRecord(recordHandle);

			RecordStatus recordStatus = new RecordStatus();

			mService.getRecordStatus(recordHandle, recordStatus);

			RecordCreateFileParameter cfPara = new RecordCreateFileParameter(
					RecordConstant.RecordType.RECORD_TSHIFT,
					RecordConstant.RecordMetaPolicy.RECORD_META_FILE_INVISIBLE,
					"test", "meta file", "%3x", 1024 * 1024 * 1024);

			mService.createRecordFile(cfPara);
			mService.deleteRecordFile("test");

			mService.unregisterListener(mListener);
		} catch (RemoteException e) {
			e.printStackTrace();
		}
	}

	class RecordListenerImpl extends RecordListener {

		public void onEvent(RecordEvent event) {
			switch (event.getType()) {
			case RecordEvent.createFileOK:
				Util.showDLog("Record,onEvent(): " + "RecordEvent.createFileOK");
				Log.e("prv0523", "startRecord onEvent");
				startPvr();
				break;
			case RecordEvent.createFileProgress:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.createFileProgress");

				break;
			case RecordEvent.createFileStatus:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.createFileStatus");
				break;

			// Fail:
			case RecordEvent.analysisFileFail:
			case RecordEvent.recordStatusInternalError:
			case RecordEvent.createFileFail:
				Util.showDLog("Record,onEvent(): " + "...Fail...");
				mManager.showFeatureNotAvaiable();
				break;

			case RecordEvent.createFileAbortOK:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.createFileAbortOK");
				break;
			case RecordEvent.createFileAbortFail:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.createFileAbortFail");
				break;
			case RecordEvent.analysisFileOK:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.analysisFileOK");
				break;

			case RecordEvent.recordStatusUnknown:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.recordStatusUnknown");
				break;
			case RecordEvent.recordStatusStarted:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.recordStatusStarted");

				disableScanning = true;
				break;
			case RecordEvent.recordStatusPaused:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.recordStatusPaused");
				break;
			case RecordEvent.recordStatusStopped:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.recordStatusStopped");
				Log.e("prv0523", "RecordEvent recordStatusStopped");
				mManager.restoreToDefault(StatusType.PVR,StatusType.TIMESHIFT);
				break;

			case RecordEvent.recordStatusDiskFull:

				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.recordStatusDiskFull");

				Controller.disableScanning = false;
				mManager.showDiskNotEnoughSpace();
				break;
			case RecordEvent.recordRequestSendMessage:
				Util.showDLog("Record,onEvent(): "
						+ "RecordEvent.recordRequestSendMessage");
				break;
			case Integer.MAX_VALUE: // For debug.

				IStateInterface ms = mManager.getState();
				int result = (int) Math.random() * 100;

				if (ms instanceof StateTimeShift) {
					// ((StateTimeShift)ms).setPlayerOffset(result);
					((StateTimeShift) ms).setPlayerOffset(50);
				}

				break;
			default:
				Util.showDLog("Record,onEvent(): " + "default");
				break;
			}
		}

	}

	class MakeFileListenerImpl extends RecordListener {

		public void onEvent(RecordEvent event) {
			// Util.showDLog("MakeFileListenerImpl,onEvent():" + "RecordEvent: "
			// + event.getType());//event 1

			switch (event.getType()) {
			case RecordEvent.diskFull:
				mManager.showDiskNotEnoughSpace();
				mManager.tshiftCreateFinished();
				break;
			case RecordEvent.createProgress:
				mManager.tshiftCreateProgress(event.getData());
				break;
			case RecordEvent.createComplete:
				mManager.showCreateFileSuccess(true);
				break;

			case RecordEvent.createFileFail:
				mManager.showCreateFileSuccess(false);
				break;
			default:
				break;
			}
		}
	}

	/*
	 * 
	 */
	public boolean initPvrPlayer() {

		// if (getPlayback() == null) {
		setPlayback(VideoManager.getInstance(VideoConst.PLAYER_MODE_MMP));
		// } else {
		// getPlayback().setOnPreparedListener(null);
		// getPlayback().setOnCompletionListener(null);
		// getPlayback().stopVideo();
		// getPlayback().onRelease();
		// }

		getPlayback().setPlayerMode(0);
		getPlayback().setPreviewMode(false);

		VideoComset mVideoComSet = new VideoComset();
		mVideoComSet.videoZoom(0);
		try {
			// getPlayback().setDataSource(Core.PVR_tmp_FILE);
			// LogicManager mLogicManager = LogicManager.getInstance(mManager
			// .getActivity());
			// mLogicManager.setPreparedListener(this);
			// mLogicManager.setCompleteListener(this);
			// mLogicManager.setOnPBMsgListener(this);
			// mLogicManager.setOnPBPlayDoneListener(this);

			// -----
			// getPlayback().setOnPBBufferUpdateListener(this);
			getPlayback().setOnCompletionListener(this);
			// getPlayback().setOnPBDurarionUpdateListener(this);
			// getPlayback().setOnPBEofListener(this);
			getPlayback().setOnPreparedListener(this);

		} catch (IllegalStateException e) {
			getPlayback().onRelease();
		}

		return false;
	}

	/*
	 * 
	 */
	public boolean startPlayPvr() {
		// initPvrPlayer();
		Util.showDLog("startPlayPvr()");

		if (isPlaying()) {
			Util.showDLog("isPlaying");
			return false;
		}
		// if (getPlayback() == null) {
		// Util.showDLog("getPlayback() == null");
		initPvrPlayer();
		// }
		Log.e("timeshift0527", "getTsFile:" + getTsFile());
		getPlayback().setDataSource(getTsFile());
		getPlayback().startVideo();
		return true;
	}

	public void startVideo() {
		getPlayback().startVideo();
	}

	/*
	 * 
	 */
	public boolean startPlayPvr(String path) {
		Util.showDLog("startPlayPvr():" + path);
		if (getPlayback() == null) {
			initPvrPlayer();
		} else if (isPlaying()) {
			return false;
		}

		getPlayback().setDataSource(path);
		getPlayback().startVideo();
		return true;
	}

	/**
	 * @return
	 */
	public boolean isPlaying() {
		if (getPlayback() == null) {
			return false;
		}

		int playStatus = getPlayback().getPlayStatus();
		switch (playStatus) {
		case VideoConst.PLAY_STATUS_INITED:
		case VideoConst.PLAY_STATUS_PREPAREING:
		case VideoConst.PLAY_STATUS_PREPARED:
		case VideoConst.PLAY_STATUS_STARTED:
		case VideoConst.PLAY_STATUS_SEEKING:
		case VideoConst.PLAY_STATUS_STEP:
		case VideoConst.PLAY_STATUS_PAUSED:
		case VideoConst.PLAY_STATUS_FF:
		case VideoConst.PLAY_STATUS_FR:
		case VideoConst.PLAY_STATUS_SF:

			return true;

		case VideoConst.PLAY_STATUS_STOPPED:
		default:
			return false;
		}
	}

	public boolean isPause(){
		if (getPlayback() == null) {
			return false;
		}

		int playStatus = getPlayback().getPlayStatus();
		switch (playStatus) {
		case VideoConst.PLAY_STATUS_PAUSED:
			return true;
		default:
			return false;
		}
	}
	
	public boolean isFForFR(){
		if (getPlayback() == null) {
			return false;
		}
		
		int playStatus = getPlayback().getPlayStatus();
		switch (playStatus) {
		case VideoConst.PLAY_STATUS_FF:
		case VideoConst.PLAY_STATUS_FR:
		case VideoConst.PLAY_STATUS_SF:
			return true;
		default:
			return false;
		}
	}
	
	
	
	
	
	/*
	 * 
	 */
	public boolean stopPlayPvr() {
		if (getPlayback() != null) {
			try {
				getPlayback().stopVideo();
			} catch (Exception e) {

			} finally {
				getPlayback().onRelease();
			}
		}
		//resumeTV();
		return true;
	}

	public int getPlayStatus() {
		return getPlayback().getPlayStatus();
	}

	public int getSpeedStep() {
		return getPlayback().getPlaySpeed();
	}

	/*
		 * 
		 */
	public boolean backToNormalSpeed() {
		getPlayback().startVideo();
		return true;
	}

	/*
	 * 
	 */
	public boolean pausePlayPvr() {
		getPlayback().pauseVideo();
		return true;
	}

	/*
	 * 
	 */
	public boolean forwardPlay(int speed) {
		if (isPlaying()) {
			getPlayback().fastForward();
			if (speed == 2) {
				StateFileList.getInstance().showFastPlayInfo(true);
			} else if (speed == 1) {
				StateTimeShift.getInstance().showFastPlayInfo(true);
			}

		}
		return true;
	}

	/*
	 * 
	 */
	public boolean rewindPlay(int speed) {

		if (isPlaying()) {
			getPlayback().fastRewind();
			if (speed == 2) {
				StateFileList.getInstance().showFastPlayInfo(false);
			} else if (speed == 1) {
				StateTimeShift.getInstance().showFastPlayInfo(false);
			}

		}
		return true;
	}

	public int getVideoProgress() {
		if (playback == null) {
			return 0;
		}
		return playback.getProgress();
	}

	public void setOnInfoListener(OnInfoListener infoListener) {
		Log.e("stateplay", "stateplay:onPrepared1:" + playback);
		if (null != playback) {
			playback.setOnInfoListener(infoListener);
		}
	}

	public void setVideoPreparedListener(OnPreparedListener listener) {
		Log.e("stateplay", "stateplay:onPrepared2:" + playback);
		if (null == playback) {
			return;
		}
		playback.setOnPreparedListener(listener);
	}

	public void setCompleteListener(OnCompletionListener listener) {
		if (null == playback) {
			return;
		}
		playback.setOnCompletionListener(listener);// setOnPBCompleteListener(listener);
	}

	public int getVideoDuration() {
		return playback.getDuration();
	}

	public long getVideoFileSize() {
		if (null == playback) {
			return 0;
		}
		return playback.getFileSize();
	}

	public int getVideoBytePosition() {
		if (null == playback) {
			return 0;
		}
		return playback.getBytePosition();
	}

	/*
	 * 
	 */
	public boolean deletePVRFile(File file) {
		// TODO Auto-generated method stub
		
		return file.delete();
	}

	/**
	 * @return the playback
	 */
	public VideoManager getPlayback() {
		return playback;
	}

	/**
	 * @param playback
	 *            the playback to set
	 */
	public void setPlayback(VideoManager playback) {
		this.playback = playback;
	}

	// /*
	// *
	// */
	// @Override
	// public void onPlayDone(IPlayback pb) {
	// Util.showDLog("onPlayDone()");
	// // mManager.restoreToDefault();
	// }

	/*
	 * 
	 * //
	 */
	// @Override
	// public void onMsg(int msg) {
	// Util.showDLog("onMsg()" + msg);
	// switch (msg) {
	// case VideoConst.MSG_SET_DATA_SOURCE_FAIL:
	// case VideoConst.MSG_SEEK_NOT_SUPPORT:
	// case VideoConst.MSG_PLAY_START_FAIL:
	// case VideoConst.MSG_STEP_NOT_SUPPORT:
	// case VideoConst.MSG_NOT_SUPPORT:
	// case VideoConst.MSG_AUDIO_NOT_SUPPORT:
	// case VideoConst.MSG_VIDEO_NOT_SUPPORT:
	// case VideoConst.MSG_FILE_NOT_SUPPORT:
	// case VideoConst.MSG_INVALID_STATE:
	// case VideoConst.MSG_INPUT_STREAM_FAIL:
	// case VideoConst.MSG_AV_NOT_SUPPORT:
	// // case VideoConst.MSG_FILE_CORRUPT:
	// mManager.restoreToDefault();
	// resumeTV();
	// break;
	// case VideoConst.MSG_PLAY_START:
	// Util.showDLog("...VideoConst.MSG_PLAY_START...");
	// break;
	// }
	// }

	// /*
	// *
	// */
	// @Override
	// public void onComplete(IPlayback pb) {
	// Util.showDLog("onComplete()");
	// try {
	// // pb.stop();
	// pb.reset();
	// pb.onRelease();
	// } catch (Exception e) {
	// Util.showELog("onComplete()" + e.toString());
	// }
	// mManager.restoreToDefault();
	// resumeTV();
	// }

	/*
	 * 
	 */
	// @Overridesz

	@Override
	public void onCompletion(MediaPlayer pb) {
		Util.showDLog("onComplete()");
		try {
			// pb.stop();
			pb.reset();
			pb.release();
		} catch (Exception e) {
			Util.showELog("onComplete()" + e.toString());
		}
		mManager.restoreToDefault(StatusType.FILELIST);
		//resumeTV();
	}

	@Override
	public void onPrepared(MediaPlayer arg0) {
		// TODO Auto-generated method stub
		Util.showDLog("onPrepared()");
	}

	public void resumeTV() {
		NewPipLogic pipLogic = NewPipLogic.getInstance(mManager.getActivity());
		pipLogic.resumeMainOutput();
	}
	public String getRecordingFileName() {
		//return MtkTvRecord.getInstance().getRecordingFileName();
		return "";
	}
	public String getPlayingFilePath() {
		return playingFilePath;
	}

	public void setPlayingFilePath(String playingFilePath) {
		this.playingFilePath = playingFilePath;
	}

	public void setStepValue(int speedCounter) {
		// TODO Auto-generated method stub
		
	}

	public void pausePvr() {
		// TODO Auto-generated method stub
		
	}

	public boolean isEqualRecordingFile(String absolutePath) {
		// TODO Auto-generated method stub
		return false;
	}
}
