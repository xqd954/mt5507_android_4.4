package com.mediatek.ui.mmp.model;

import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.util.LinkedList;
import java.util.List;

import jcifs.smb.SmbAuthException;
import jcifs.smb.SmbException;
import android.content.Context;
import android.util.Log;

import com.mediatek.mmpcm.audioimpl.AudioInfo;
import com.mediatek.mmpcm.videoimpl.VideoInfo;
import com.mediatek.netcm.samba.SambaManager;
import com.mediatek.ui.util.MtkLog;

public class SmbFilesManager extends FilesManager<FileAdapter> {
	private static final String TAG = "SmbFilesManager";

	private static SmbFilesManager sInstance;
	private SambaManager mOperator;
	private int mLoginCount;
	private AudioInfo mAudioInfo;
	private VideoInfo mFileInfo;
	private static final boolean localLOGV = false;

	private SmbFilesManager(Context context) {
		super(context);

		mOperator = SambaManager.getInstance();
		mOperator.initSamba();

		mAudioInfo =  AudioInfo.getInstance();
		mFileInfo = VideoInfo.getInstance();
	}

	public static SmbFilesManager getInstance(Context context) {
		if (sInstance == null) {
			sInstance = new SmbFilesManager(context);
		}

		return sInstance;
	}

	public boolean login(String path, String userName, String userPwd) {
		boolean success = false;
		try {
			SambaManager.login(path, userName, userPwd);
			success = true;
		} catch (SmbAuthException e) {
			MtkLog.w(TAG, "Login SmbAuthException!!");
			e.printStackTrace();
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (SmbException e) {
			e.printStackTrace();
		}

		return success;
	}
	
	public boolean login(String path, String domain, String userName, String userPwd) {
		boolean success = false;
		try {
			SambaManager.login(path, domain, userName, userPwd);
			success = true;
		} catch (SmbAuthException e) {
			MtkLog.w(TAG, "Login SmbAuthException!!");
			e.printStackTrace();
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (SmbException e) {
			e.printStackTrace();
		}

		return success;
	}

	private boolean isSmbRoute(String path) {
		if(path == null) {
			return false;
		}
		
		if(path.equals("smb://")) {
			return true;
		}
		
		return false;
	}
	
	@Override
	public List<FileAdapter> listAllFiles(String path) {
		MtkLog.d(TAG, "------List All login Files : " + path);

		try {
			mFiles.clear();
			int type = SambaManager.TYPE_ALL;

			switch (mContentType) {
			case CONTENT_PHOTO:
				type = SambaManager.TYPE_IMAGE;
				break;
			case CONTENT_AUDIO:
				type = SambaManager.TYPE_AUDIO;
				break;
			case CONTENT_VIDEO:
				type = SambaManager.TYPE_VIDEO;
				break;
			case CONTENT_TEXT:
				type = SambaManager.TYPE_TEXT;
				break;
			case CONTENT_THRDPHOTO:
				type = SambaManager.TYPE_THRIMAGE;
				break;
			default:
				break;
			}

			LinkedList<String> filePaths = mOperator.getSmbFileList(path, type);
			filePaths = mOperator.sortByName(filePaths);

			mFiles = wrapFiles(filePaths);
			logFiles(TAG);

		} catch (SmbAuthException e) {
			e.printStackTrace();
			if(localLOGV)MtkLog.d(TAG, "Try to connect to login : " + mLoginCount);
			if(isSmbRoute(path)) {
				if (mLoginCount == 0) {
					if(localLOGV)	MtkLog.d(TAG, "Try to connect to login : ?  GUEST  '' " );
					login(path, "?", "GUEST", "");
					mLoginCount++;
					listAllFiles(path);
				}else if (mLoginCount == 1) {
					if(localLOGV)	MtkLog.d(TAG, "Try to connect to login : null null null" );
					login(path, null, null, null);
					mLoginCount++;
					listAllFiles(path);
				}
				else if (mLoginCount == 2) {
					
					if(localLOGV) MtkLog.d(TAG, " --- User name and Password is not correct. " );
					mLoginCount = 0;
					setChanged();
					notifyObservers(REQUEST_LOGIN);
				}
			}
			else {
				setChanged();
				notifyObservers(REQUEST_LOGIN);
			}
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (SmbException e) {	
			e.printStackTrace();
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
		System.setProperty("java.util.Arrays.useLegacyMergeSort", "true");
		sortFile();
		return mFiles;
	}

	@Override
	protected String retriveParentPath(String path) {
		String parent;
		String newPath = path.substring(0, path.length() - 1);

		if (newPath.indexOf("/") + 1 == newPath.lastIndexOf("/")) {
			parent = newPath.substring(0, newPath.lastIndexOf("/") + 1);
		} else {
			parent = super.retriveParentPath(newPath) + "/";
		}

		MtkLog.d(TAG, "RetriveParentPath : " + parent);
		return parent;
	}

	@Override
	protected FileAdapter newWrapFile(Object originalFile) {
		return new SmbFileAdapter((String) originalFile, mAudioInfo, mFileInfo);
	}

	public interface AuthListener {
		void onNeedAuth();
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
		sInstance = null;
	}
	protected void sortFile(){
		MtkLog.i("sort","sortFile");
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
