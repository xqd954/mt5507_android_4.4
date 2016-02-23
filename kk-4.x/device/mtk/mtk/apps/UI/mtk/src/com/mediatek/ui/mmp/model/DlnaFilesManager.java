package com.mediatek.ui.mmp.model;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Stack;

import android.content.Context;

import com.mediatek.dlna.object.DLNADevice;
import com.mediatek.mmpcm.audioimpl.AudioInfo;
import com.mediatek.mmpcm.videoimpl.VideoInfo;
import com.mediatek.netcm.dlna.DLNAFile;
import com.mediatek.netcm.dlna.DLNAManager;
import com.mediatek.netcm.dlna.FileEvent;
import com.mediatek.netcm.dlna.FileEventListener;
import com.mediatek.ui.mmp.util.Lists;
import com.mediatek.ui.util.MtkLog;

public class DlnaFilesManager extends FilesManager<FileAdapter> {
	private static final String TAG = "DlnaFilesManager";
	private static final boolean LocalLog = false;
	private static DlnaFilesManager sInstance;
	private DLNAManager mOperator;

	private Stack<String> mHistory;
	private String mTempParent;
	private AudioInfo mAudioInfo;
	private VideoInfo mFileInfo;
	
	protected DlnaFilesManager(Context context) {
		super(context);
		mHistory = Lists.newStack();

		mOperator = DLNAManager.getInstance();
		mOperator.setOnFileEventListener(new FileEventListener() {
			public void onFileLeft(FileEvent event) {
				MtkLog.i(TAG, "OnFileLeft");
				String currentDeviceName;
				LinkedList<DLNAFile> files = event.getFileList(getType());
				for (DLNAFile file : files) {
					MtkLog.d(TAG, "OnFileLeft : " + file.getPath());
				}
				int sourceType = MultiFilesManager.getInstance(mContext)
						.getCurrentSourceType();
				if (sourceType == MultiFilesManager.SOURCE_ALL) {
					onFileFound(event);
				} else if (sourceType == MultiFilesManager.SOURCE_DLNA) {
					DLNADevice device = event.getLeftDevice();
					if (null != device) {
						String leftDeviceName = device.getName();
						MtkLog
								.i(TAG, " DLNA device  leave  :"
										+ leftDeviceName);
						synchronized (mFiles) {
							if (mFiles.size() > 0) {
								FileAdapter currentFile = mFiles.get(0);

								MtkLog.i(TAG, "  Current file  path  :"
										+ currentFile.getPath());

								if (currentFile.isDevice()) {
									currentDeviceName = currentFile
											.getDeviceName();
								} else {
									currentDeviceName = mOperator
											.getDevice(currentFile.getPath());
								}
								MtkLog.i(TAG, "  Current DLNA device  name  :"
										+ currentDeviceName);
								if (currentFile.getDeviceName().equals(
										leftDeviceName)) {
									// TODO leave
									MtkLog.i(TAG, "goto  the root dic");
									addFiles(files, REQUEST_DEVICE_LEFT);
								} else {
									addFiles(files, REQUEST_BACK_DEVICE_LEFT);
								}
							} else {
								currentDeviceName = mOperator
										.getDevice(getCurrentPath());
								DLNADevice device1 = event.getLeftDevice();
								if (currentDeviceName == null
										|| (null != device1 && currentDeviceName.equals(device1.getName()))) {
									addFiles(files, REQUEST_DEVICE_LEFT);
								} else {
									addFiles(files, REQUEST_BACK_DEVICE_LEFT);
								}
							}

						}
					}
				}
			}

			public void onFileFound(FileEvent event) {
				MtkLog.i(TAG, "OnFileFound");
				LinkedList<DLNAFile> files = new LinkedList<DLNAFile>();
				files.addAll(event.getFileList(getType()));
				for (DLNAFile file : files) {
					MtkLog.d(TAG, "OnFileFound : " + file.getName());
				}

				List<FileAdapter> wrapedFiles = wrapFiles(files);
				synchronized (mFiles) {
					mFiles.clear();
					mFiles.addAll(wrapedFiles);
					mFiles = Collections.synchronizedList(mFiles);
					sortFile();
					MtkLog.d(TAG, "OnFileFound mFiles size : " +mFiles.size());
					setChanged();
					notifyObservers(REQUEST_REFRESH);
				}
			}

			public void onFileFailed(FileEvent event) {
				MtkLog.i(TAG, "OnFileFailed");
				setChanged();
				notifyObservers(REQUEST_REFRESH);
			}
		});

		mAudioInfo = AudioInfo.getInstance();
		mFileInfo = VideoInfo.getInstance();
	}

	private void addFiles(LinkedList<DLNAFile> files, int flag){
		if (files == null ){
			return;
		}
		List<FileAdapter> wrapedFiles = wrapFiles(files);
		if (flag == REQUEST_BACK_DEVICE_LEFT){
			
			synchronized (mDevices) {
				mDevices.clear();
				mDevices.addAll(wrapedFiles);
			
				setChanged();
				notifyObservers(REQUEST_BACK_DEVICE_LEFT);
			}
			
		} else if (flag == REQUEST_DEVICE_LEFT){
			mHistory.clear();
			MtkLog.d(TAG, "addFiles REQUEST_DEVICE_LEFT mHistory clear ");
			synchronized (mFiles) {
				mFiles.clear();
				mFiles.addAll(wrapedFiles);
			
				setChanged();
				notifyObservers(REQUEST_DEVICE_LEFT);
			}

		}
	}

	private int getType() {
		int type = FileEvent.FILTER_TYPE_ALL;
		switch (mContentType) {
		case CONTENT_PHOTO:
			type = FileEvent.FILTER_TYPE_IMAGE;
			break;
		case CONTENT_AUDIO:
			type = FileEvent.FILTER_TYPE_AUDIO;
			break;
		case CONTENT_VIDEO:
			type = FileEvent.FILTER_TYPE_VIDEO;
			break;
		case CONTENT_TEXT:
			type = FileEvent.FILTER_TYPE_TEXT;
			break;
		default:
			break;
		}
		return type;
	}

	public static DlnaFilesManager getInstance(Context context) {
		if (sInstance == null) {
			sInstance = new DlnaFilesManager(context);
		}

		return sInstance;
	}

	@Override
	public void setCurrentPath(String path) {
		mTempParent = mParentPath;

		super.setCurrentPath(path);
	}

	public List<FileAdapter> getDevices() {
		synchronized (mDevices) {
			return mDevices;
		}
	}

	@Override
	public List<FileAdapter> listAllFiles(String path) {
		MtkLog.d(TAG, "List Path : " + path);
		MtkLog.d(TAG, "Parent Path : " + mTempParent);

		synchronized (mFiles) {
			mFiles.clear();

			try {
				String name = retriveName(path);
				if (path != null && path.equals(mTempParent)) {
					MtkLog.i(TAG, "Back!!");
					mOperator.parseDLNAFile(name, false);
					if(!mHistory.empty()){
						String popVal = mHistory.pop();
						MtkLog.d(TAG, "listAllFiles mHistory pop  : " + popVal);
					}else{
						MtkLog.d(TAG, "listAllFiles mHistory is empty~~ " );

					}
				} else {
					MtkLog.i(TAG, " listAllFiles Into ");
					mOperator.parseDLNAFile(name, true);
					
					String absolute = retriveHistoryStack();

					MtkLog.d(TAG, "listAllFiles   absolute  : " + absolute);
					if(path != null && path.equals(absolute)){

						MtkLog.d(TAG, "listAllFiles need not  push  : " + name);


					}else if (name != null) {
						mHistory.push(name);
						
						MtkLog.d(TAG, "listAllFiles mHistory push  : " + name);
					}

					mRootPath = "";
				}
			} catch (ExceptionInInitializerError e) {
				e.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
			return mFiles;
		}
	}

	@Override
	protected FileAdapter newWrapFile(Object originalFile) {
		DLNAFile file = (DLNAFile) originalFile;
		String name = file.getName();
		String absolutePath = retriveAbsolutePath(name);

		return new DlnaFileAdapter(file, absolutePath, mAudioInfo, mFileInfo);
	}


	private String retriveHistoryStack(){
		String absolute;
		StringBuilder builder = new StringBuilder();

		for (int i = 0; i < mHistory.size(); i++) {
			builder.append("/").append(mHistory.get(i));
		}

		
		absolute = builder.toString();
		MtkLog.d(TAG, "retriveHistoryStack : " + absolute);

		return absolute;


	}

	private String retriveAbsolutePath(String name) {
		String absolute = retriveHistoryStack() + "/" + name;
		MtkLog.d(TAG, "RetriveAbsolutePath : " + absolute);

		return absolute;
	}

	private String retriveName(String path) {
		if (path == null) {
			return null;
		}

		String name = path.substring(path.lastIndexOf("/") + 1, path.length());
		MtkLog.d(TAG, "RetriveName : " + name);

		return name;
	}

	public void clearHistory() {
		mHistory.clear();
		
		MtkLog.d(TAG, "clearHistory ~ " );
	}

	@Override
	public void destroy() {
		destroyManager();
	}

	@Override
	public List<FileAdapter> listRecursiveFiles(int contentType) {
		return null;
	}

	@Override
	public void destroyManager() {
		try {
			MtkLog.i(TAG, "Destory!!");
			mOperator.destroy();
			sInstance = null;
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	
	protected void sortFile(){
		MtkLog.i("sort","dlna sortFile");
		int type = MultiFilesManager.getInstance(mContext).getSortType();
		if(FilesManager.SORT_BY_NAME == type){
			new com.mediatek.ui.mmp.util.SortList().sort(mFiles, "getName",null);
		}else if(FilesManager.SORT_BY_DATE == type){
			int source = MultiFilesManager.getInstance(mContext).getCurrentSourceType();
			if( MultiFilesManager.SOURCE_DLNA != source ){
				new com.mediatek.ui.mmp.util.SortList().sort(mFiles, "getSize",null);
			}
		}else if(FilesManager.SORT_BY_TYPE == type){
			new com.mediatek.ui.mmp.util.SortList().sort(mFiles, "getSuffix","getName");
		}
		
	}

}
