package com.mediatek.ui.mmp.model;

import java.util.List;
import java.util.Observable;
import java.util.Stack;

import android.content.Context;
import android.util.Log;

import com.mediatek.ui.mmp.MeidaMainActivity;
import com.mediatek.ui.mmp.util.Lists;
import com.mediatek.ui.util.MtkLog;

public abstract class FilesManager<T extends FileAdapter> extends Observable {
	private static final String TAG = "FilesManager";

	public static final int SORT_BY_NAME = 0;
	public static final int SORT_BY_DATE = 1;
	public static final int SORT_BY_ALBUM = 2;
	public static final int SORT_BY_ARTIST = 3;
	public static final int SORT_BY_GENRE = 4;
	public static final int SORT_BY_TYPE = 5;

	public static final int CONTENT_ALL = -1;
	public static final int CONTENT_PHOTO = 0;
	public static final int CONTENT_AUDIO = 1;
	public static final int CONTENT_VIDEO = 2;
	public static final int CONTENT_TEXT = 3;
	public static final int CONTENT_THRDPHOTO = 4;

	public static final int REQUEST_REFRESH = 1;
	public static final int REQUEST_LOGIN = 2;
	public static final int REQUEST_BACK_TO_ROOT = 3;

	public static final int REQUEST_BACK_DEVICE_LEFT = 4;
	public static final int REQUEST_DEVICE_LEFT = 5;
	public static final int REQUEST_SUB_DIRECTORY = 6;
	public static final int REQUEST_MMP_MENU = 7;

	public static final int INVALID_INDEX = -1;

	protected Context mContext;

	protected List<T> mFiles;
	protected List<T> mDevices;

	protected String mRootPath;
	protected String mCurrentPath;
	protected String mParentPath;

	protected int mContentType;
	protected int mSortType;

	protected int mPositionInParent;
	private Stack<Integer> mOpenedHistory;

	protected FilesManager(Context context) {
		mContext = context;

		mFiles = Lists.newArrayList();
		mDevices = Lists.newArrayList();

		mRootPath = null;
		mCurrentPath = null;
		mParentPath = null;

		mContentType = CONTENT_ALL;
		mSortType = SORT_BY_NAME;

		mPositionInParent = INVALID_INDEX;
		mOpenedHistory = Lists.newStack();
	}

	public List<T> getCurrentFiles() {
		synchronized (mFiles) {
			return mFiles;
		}
	}

	public void setRootPath(String path) {
		mRootPath = path;
		MtkLog.d(TAG, "Root Path : " + mRootPath);
	}

	public String getRootPath() {
		return mRootPath;
	}

	public void setCurrentPath(String path) {
		mCurrentPath = path;

		if (mCurrentPath == null || mCurrentPath.length()<=0
				|| mCurrentPath.equals(mRootPath)) {
			mCurrentPath = mRootPath;
			mParentPath = null;
		} else {
			mParentPath = retriveParentPath(path);
		}

		MtkLog.d(TAG, "Current Path : " + mCurrentPath);
		MtkLog.d(TAG, "Parent Path : " + mParentPath);
	}

	public String getCurrentPath() {
		return mCurrentPath;
	}

	public String getParentPath() {
		return mParentPath;
	}

	public String getBackPath(String curPath){
		String path = curPath;
		
		if (path == null) return null;
		
		int index = path.lastIndexOf("/");
		if (index > 0)
		{
			path = path.substring(0, index);
		}
		return path;
	}
	public String getBackPath()
	{
		String path = getCurrentPath();

		if ("/mnt".equals(path))
		{
			return null;
		}

		if (mRootPath.equals(path)|| "/mnt/usb".equals(path))
		{
			return "/mnt";
		}
		int index = path.lastIndexOf("/");
		if (index > 0)
		{
			path = path.substring(0, index);
		}
		return path;
	}
	public void setContentType(int contenType) {
		MtkLog.d(TAG, "Content Type : " + contenType);
		if (contenType > INVALID_INDEX) {
			mContentType = contenType;
			MeidaMainActivity.mSelection=mContentType;
		}
	}

	public int getContentType() {
		return mContentType;
	}

	public int getContentType(String path) {
		return CONTENT_ALL;
	}

	public void setSortType(int sortType) {
		mSortType = sortType;
		MtkLog.d(TAG, "Sort Type : " + mSortType);
	}

	public int getSortType() {
		return mSortType;
	}

	public void setPositionInParent(int position) {
		if (position > INVALID_INDEX) {
			mPositionInParent = position;
		} else {
			mPositionInParent = 0;
		}
		MtkLog.d(TAG, "Current Selection : " + mPositionInParent);
	}

	public int getPositionInParent() {
		return mPositionInParent;
	}

	public int getFilesCount() {
		MtkLog.d(TAG, "Files Count : " + mFiles.size());
		return mFiles.size();
	}

	public T getFile(int postion) {
		if (postion >= 0 && postion < mFiles.size()){
			return mFiles.get(postion);
		}
		
		return null;
	}

	public void pushOpenedHistory(int position) {
		mOpenedHistory.push(position);
	}

	public int popOpenedHistoryRoot() {
		while (mOpenedHistory.size() > 1) {
			mOpenedHistory.pop();
		}

		return popOpenedHistory();
	}

	public int popOpenedHistory() {
		if (mOpenedHistory.size() == 0) {
			return 0;
		}

		return mOpenedHistory.pop();
	}

	public boolean canPaste(String file) {
		if (mCurrentPath == null){
			return false;
		}
		return !mCurrentPath.equals(retriveParentPath(file));
	}

	public boolean isInSameFolder(String path1, String path2) {
		return retriveParentPath(path1).equals(retriveParentPath(path2));
	}

	protected String retriveParentPath(String path) {
		String parnet = path.substring(0, path.lastIndexOf('/'));
		MtkLog.d(TAG, "RetriveParentPath : " + parnet);

		return parnet;
	}

	protected List<T> wrapFiles(List<?> originalFiles) {
		List<T> wrapedFiles = Lists.newArrayList();
		if (originalFiles != null) {
			for (Object file : originalFiles) {
				T wrapedFile = newWrapFile(file);
				wrapedFiles.add(wrapedFile);
			}
		}

		return wrapedFiles;
	}

	protected T newWrapFile(Object originalFile) {
		return null;
	}

	public abstract List<T> listAllFiles(String path);

	protected void logFiles(String tag) {
		logFiles(tag, mFiles);
	}

	protected void logFiles(String tag, List<T> files) {
		if (files != null) {
			for (FileAdapter file : files) {
				MtkLog.d(tag, "File : " + file.getAbsolutePath());
			}
		}
	}

	public abstract List<T> listRecursiveFiles(int contentType);

	public abstract void destroy();
	
	public abstract void destroyManager();
}
